"""J-Link RTT Viewer MCP Server

将 J-Link RTT Viewer 的核心功能封装为 MCP 服务器，
使 AI 助手能够通过标准化接口与 STM32 开发板进行交互。
"""

from __future__ import annotations

import json
import logging
import sys

from mcp.server.fastmcp import FastMCP

from .core.jlink_manager import JLinkManager

# 配置日志
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s - %(name)s - %(levelname)s - %(message)s",
    handlers=[
        logging.FileHandler("jlink_mcp.log", encoding="utf-8"),
        logging.StreamHandler(sys.stderr),  # MCP 服务器不能写入 stdout
    ],
)
logger = logging.getLogger(__name__)

# 创建 MCP 服务器
mcp = FastMCP(
    "J-Link RTT Viewer",
    json_response=True,
)

# 全局 J-Link 管理器
_manager: JLinkManager | None = None


def get_manager() -> JLinkManager:
    """获取 J-Link 管理器实例"""
    global _manager
    if _manager is None:
        _manager = JLinkManager()
    return _manager


# ============================================================================
# 工具实现
# ============================================================================


@mcp.tool()
async def connect(
    target: str,
    interface: str = "SWD",
    speed: int = 4000,
    channel: int = 0,
    retry_attempts: int = 3,
) -> str:
    """连接到 J-Link 调试器和目标 MCU

    Args:
        target: 目标设备型号 (如 STM32F103C8T6, STM32F407VG, nRF52840)
        interface: 调试接口 (SWD 或 JTAG)，默认 SWD
        speed: 连接速度 (kHz)，默认 4000
        channel: RTT 通道号，默认 0
        retry_attempts: 重试次数，默认 3

    Returns:
        连接状态和设备信息的 JSON 字符串
    """
    manager = get_manager()
    try:
        result = await manager.connect(
            target=target,
            interface=interface,
            speed=speed,
            channel=channel,
            retry_attempts=retry_attempts,
        )
        return json.dumps(result, ensure_ascii=False, indent=2)
    except ConnectionError as e:
        return json.dumps(
            {
                "success": False,
                "message": str(e),
                "hint": "Check if J-Link is connected and MCU is powered",
            },
            ensure_ascii=False,
        )
    except Exception as e:
        logger.exception("Unexpected error during connect")
        return json.dumps(
            {"success": False, "message": f"Unexpected error: {e}"},
            ensure_ascii=False,
        )


@mcp.tool()
async def disconnect() -> str:
    """断开 J-Link 连接

    安全地断开与 J-Link 调试器的连接，清理所有资源。

    Returns:
        断开状态的 JSON 字符串
    """
    manager = get_manager()
    try:
        result = await manager.disconnect()
        return json.dumps(result, ensure_ascii=False, indent=2)
    except Exception as e:
        logger.exception("Unexpected error during disconnect")
        return json.dumps(
            {"success": False, "message": f"Unexpected error: {e}"},
            ensure_ascii=False,
        )


@mcp.tool()
async def get_device_info() -> str:
    """获取当前连接设备的信息

    返回 J-Link 调试器和目标 MCU 的详细信息。

    Returns:
        设备信息的 JSON 字符串
    """
    manager = get_manager()
    if manager.state.value != "connected":
        return json.dumps(
            {"success": False, "message": "Not connected"},
            ensure_ascii=False,
        )

    return json.dumps(
        {
            "success": True,
            "device_info": manager._device_info_to_dict(),
        },
        ensure_ascii=False,
        indent=2,
    )


@mcp.tool()
async def read_rtt(
    channels: list[int] | None = None,
    timeout: float = 1.0,
    max_size: int = 4096,
) -> str:
    """读取 RTT 日志数据

    从目标 MCU 读取 RTT (Real-Time Transfer) 日志数据。
    默认读取所有通道 (0-15)，返回结构化的日志数据。

    Args:
        channels: 要读取的通道列表，None 表示所有通道 (默认)
        timeout: 读取超时时间（秒），默认 1.0
        max_size: 最大读取字节数，默认 4096

    Returns:
        RTT 数据的 JSON 字符串，包含时间戳、通道、内容和元数据
    """
    manager = get_manager()
    try:
        result = await manager.read_rtt(
            channels=channels,
            timeout=timeout,
            max_size=max_size,
        )
        return json.dumps(result, ensure_ascii=False, indent=2)
    except Exception as e:
        logger.exception("Unexpected error during RTT read")
        return json.dumps(
            {"success": False, "message": f"Unexpected error: {e}"},
            ensure_ascii=False,
        )


@mcp.tool()
async def write_rtt(
    data: str,
    channel: int | None = None,
    is_hex: bool = False,
) -> str:
    """向 RTT 通道写入数据

    向目标 MCU 的 RTT 通道发送数据。

    Args:
        data: 要写入的数据
        channel: 通道号，None 使用默认通道
        is_hex: 是否为十六进制数据，默认 False

    Returns:
        写入结果的 JSON 字符串
    """
    manager = get_manager()
    try:
        result = await manager.write_rtt(
            data=data,
            channel=channel,
            is_hex=is_hex,
        )
        return json.dumps(result, ensure_ascii=False, indent=2)
    except Exception as e:
        logger.exception("Unexpected error during RTT write")
        return json.dumps(
            {"success": False, "message": f"Unexpected error: {e}"},
            ensure_ascii=False,
        )


@mcp.tool()
async def set_rtt_channel(channel: int) -> str:
    """设置默认 RTT 通道

    设置后续 RTT 操作使用的默认通道。

    Args:
        channel: 通道号 (0-15)

    Returns:
        设置结果的 JSON 字符串
    """
    manager = get_manager()
    try:
        result = await manager.set_rtt_channel(channel)
        return json.dumps(result, ensure_ascii=False, indent=2)
    except Exception as e:
        logger.exception("Unexpected error setting RTT channel")
        return json.dumps(
            {"success": False, "message": f"Unexpected error: {e}"},
            ensure_ascii=False,
        )


@mcp.tool()
async def reset(
    mode: str = "normal",
    wait_time: float = 0.3,
) -> str:
    """重置目标 MCU

    重置连接的目标 MCU。支持多种重置模式。

    Args:
        mode: 重置模式
            - "normal": 标准重置，保留连接会话 (默认)
            - "auto_reconnect": 重置后自动断开重连
            - "halt": 重置后暂停 CPU（停在复位状态）
        wait_time: 重置后等待时间（秒），默认 0.3

    Returns:
        重置结果的 JSON 字符串
    """
    manager = get_manager()
    try:
        result = await manager.reset(mode=mode, wait_time=wait_time)
        return json.dumps(result, ensure_ascii=False, indent=2)
    except Exception as e:
        logger.exception("Unexpected error during reset")
        return json.dumps(
            {"success": False, "message": f"Unexpected error: {e}"},
            ensure_ascii=False,
        )


@mcp.tool()
async def flash_firmware(
    firmware_path: str,
    verify: bool = True,
) -> str:
    """烧录固件到 MCU

    将固件文件烧录到连接的目标 MCU。

    Args:
        firmware_path: 固件文件路径 (.hex, .bin, .axf)
        verify: 是否在烧录后验证，默认 True

    Returns:
        烧录结果的 JSON 字符串
    """
    manager = get_manager()
    try:
        result = await manager.flash_firmware(
            firmware_path=firmware_path,
            verify=verify,
        )
        return json.dumps(result, ensure_ascii=False, indent=2)
    except Exception as e:
        logger.exception("Unexpected error during firmware flash")
        return json.dumps(
            {"success": False, "message": f"Unexpected error: {e}"},
            ensure_ascii=False,
        )


@mcp.tool()
async def read_memory(address: int, size: int) -> str:
    """读取 MCU 内存

    从目标 MCU 的指定地址读取内存数据。

    Args:
        address: 起始地址 (如 0x08000000)
        size: 读取字节数 (1-1048576)

    Returns:
        内存数据的 JSON 字符串（十六进制格式）
    """
    manager = get_manager()
    try:
        result = await manager.read_memory(address=address, size=size)
        return json.dumps(result, ensure_ascii=False, indent=2)
    except Exception as e:
        logger.exception("Unexpected error during memory read")
        return json.dumps(
            {"success": False, "message": f"Unexpected error: {e}"},
            ensure_ascii=False,
        )


@mcp.tool()
async def write_memory(
    address: int,
    data: str,
    is_hex: bool = True,
) -> str:
    """写入 MCU 内存

    向目标 MCU 的指定地址写入数据。

    Args:
        address: 起始地址 (如 0x08000000)
        data: 要写入的数据（十六进制字符串或普通字符串）
        is_hex: 是否为十六进制数据，默认 True

    Returns:
        写入结果的 JSON 字符串
    """
    manager = get_manager()
    try:
        result = await manager.write_memory(
            address=address,
            data=data,
            is_hex=is_hex,
        )
        return json.dumps(result, ensure_ascii=False, indent=2)
    except Exception as e:
        logger.exception("Unexpected error during memory write")
        return json.dumps(
            {"success": False, "message": f"Unexpected error: {e}"},
            ensure_ascii=False,
        )


@mcp.tool()
async def start_log_recording(log_dir: str = "logs") -> str:
    """开始日志记录

    开始将 RTT 数据记录到文件。

    Args:
        log_dir: 日志目录，默认 "logs"

    Returns:
        记录状态的 JSON 字符串
    """
    manager = get_manager()
    try:
        result = manager.start_log_recording(log_dir)
        return json.dumps(result, ensure_ascii=False, indent=2)
    except Exception as e:
        logger.exception("Unexpected error starting log recording")
        return json.dumps(
            {"success": False, "message": f"Unexpected error: {e}"},
            ensure_ascii=False,
        )


@mcp.tool()
async def stop_log_recording() -> str:
    """停止日志记录

    停止将 RTT 数据记录到文件。

    Returns:
        停止状态的 JSON 字符串
    """
    manager = get_manager()
    try:
        result = manager.stop_log_recording()
        return json.dumps(result, ensure_ascii=False, indent=2)
    except Exception as e:
        logger.exception("Unexpected error stopping log recording")
        return json.dumps(
            {"success": False, "message": f"Unexpected error: {e}"},
            ensure_ascii=False,
        )


# ============================================================================
# 资源实现
# ============================================================================


@mcp.resource("device://status")
async def get_device_status() -> str:
    """获取设备状态信息

    返回当前 J-Link 连接状态和设备信息。

    Returns:
        设备状态的 JSON 字符串
    """
    manager = get_manager()
    is_connected = manager.state.value == "connected"
    return json.dumps(
        {
            "state": manager.state.value,
            "device_info": manager._device_info_to_dict() if is_connected else None,
        },
        ensure_ascii=False,
        indent=2,
    )


@mcp.resource("rtt://logs")
async def get_rtt_logs() -> str:
    """获取 RTT 日志历史

    返回最近的 RTT 日志数据。

    Returns:
        RTT 日志的 JSON 字符串
    """
    manager = get_manager()
    logs = [
        {
            "timestamp": data.timestamp,
            "channel": data.channel,
            "content": data.content,
            "metadata": data.metadata,
        }
        for data in manager._rtt_buffer[-100:]  # 最近 100 条
    ]
    return json.dumps(logs, ensure_ascii=False, indent=2)


@mcp.resource("firmware://metadata")
async def get_firmware_metadata() -> str:
    """获取固件元数据

    返回当前连接设备的固件相关信息。

    Returns:
        固件元数据的 JSON 字符串
    """
    manager = get_manager()
    if manager.state.value != "connected":
        return json.dumps({"error": "Not connected"}, ensure_ascii=False)

    return json.dumps(
        {
            "target_device": manager.device_info.target_device,
            "core_name": manager.device_info.core_name,
            "core_id": manager.device_info.core_id,
        },
        ensure_ascii=False,
        indent=2,
    )


# ============================================================================
# 提示实现
# ============================================================================


@mcp.prompt()
async def debugging_guide() -> str:
    """调试指南提示

    提供 STM32 调试的最佳实践和常见问题解决方案。
    """
    return """# STM32 调试指南

## 连接问题排查

1. **J-Link 未识别**
   - 检查 USB 连接
   - 安装最新 J-Link 驱动
   - 尝试不同 USB 端口

2. **MCU 无响应**
   - 检查电源供应
   - 尝试降低连接速度 (4000 → 1000 kHz)
   - 检查 SWD/JTAG 接线

3. **连接后无 RTT 数据**
   - 确认 MCU 代码已初始化 SEGGER_RTT
   - 检查 RTT 通道号是否匹配
   - 尝试重置设备

## RTT 日志分析

- **结构化输出**: 日志包含时间戳、通道、内容和元数据
- **ANSI 颜色**: 支持解析终端颜色代码
- **日志级别**: 自动识别 INFO/WARNING/ERROR 级别

## 常用操作

1. 连接设备: `connect(target="STM32F103C8T6", interface="SWD")`
2. 读取日志: `read_rtt()` (读取所有通道)
3. 重置设备: `reset(mode="auto_reconnect")`
4. 烧录固件: `flash_firmware(firmware_path="path/to/firmware.bin")`
"""


@mcp.prompt()
async def error_diagnosis(error_message: str) -> str:
    """错误诊断提示

    根据错误信息提供诊断建议。

    Args:
        error_message: 错误信息
    """
    return f"""# 错误诊断

## 错误信息
{error_message}

## 诊断步骤

1. **检查硬件连接**
   - J-Link 是否正确连接
   - MCU 是否上电
   - SWD/JTAG 接线是否正确

2. **检查软件配置**
   - 目标设备型号是否正确
   - 接口类型是否匹配
   - 连接速度是否合适

3. **常见解决方案**
   - 连接超时: 降低速度或增加重试次数
   - 设备未找到: 检查设备管理器
   - 权限不足: 以管理员身份运行

## 需要更多信息

如果问题持续，请提供：
- 完整的错误信息
- 使用的设备型号
- J-Link 固件版本
"""


@mcp.prompt()
async def flash_wizard() -> str:
    """烧录向导提示

    引导用户完成固件烧录流程。
    """
    return """# 固件烧录向导

## 烧录前检查

1. ✅ J-Link 已连接
2. ✅ MCU 已上电
3. ✅ 固件文件已准备好

## 烧录步骤

### 步骤 1: 连接设备
```python
connect(target="您的设备型号", interface="SWD")
```

### 步骤 2: 验证连接
```python
get_device_info()
```

### 步骤 3: 烧录固件
```python
flash_firmware(firmware_path="path/to/your/firmware.bin")
```

### 步骤 4: 验证烧录
```python
read_memory(address=0x08000000, size=256)  # 读取 Flash 起始地址
```

### 步骤 5: 重置设备
```python
reset(mode="normal")
```

## 注意事项

- 烧录会覆盖原有固件
- 建议先备份重要数据
- 烧录过程中不要断开连接
- 如烧录失败，尝试拔插 J-Link

## 常见问题

**Q: 烧录失败怎么办？**
A: 检查 J-Link 连接，尝试降低速度，或以管理员身份运行。

**Q: 烧录后 MCU 无响应？**
A: 检查固件是否正确，尝试重新烧录或检查硬件。
"""


# ============================================================================
# 服务器入口
# ============================================================================


def main() -> None:
    """服务器主入口"""
    logger.info("Starting J-Link RTT Viewer MCP Server")
    mcp.run(transport="stdio")


if __name__ == "__main__":
    main()
