"""使用示例

展示如何使用 J-Link RTT Viewer MCP Server 的各种功能。
"""

import asyncio
import json
from src.core.jlink_manager import JLinkManager


async def example_connect():
    """示例：连接设备"""
    print("=== 示例：连接设备 ===")
    
    manager = JLinkManager()
    
    # 连接到 STM32F103C8T6
    result = await manager.connect(
        target="STM32F103C8T6",
        interface="SWD",
        speed=4000,
    )
    
    print(f"连接结果: {json.dumps(result, indent=2, ensure_ascii=False)}")
    
    # 获取设备信息
    if result["success"]:
        print(f"\n设备信息:")
        print(f"  核心: {manager.device_info.core_name}")
        print(f"  ID: {manager.device_info.core_id}")
        print(f"  接口: {manager.device_info.interface}")
    
    # 断开连接
    await manager.disconnect()
    print("\n已断开连接")


async def example_rtt():
    """示例：RTT 读写"""
    print("\n=== 示例：RTT 读写 ===")
    
    manager = JLinkManager()
    
    # 连接设备
    await manager.connect(target="STM32F103C8T6")
    
    # 读取 RTT 数据
    print("读取 RTT 数据...")
    result = await manager.read_rtt(timeout=2.0)
    print(f"读取结果: {json.dumps(result, indent=2, ensure_ascii=False)}")
    
    # 写入 RTT 数据
    print("\n写入 RTT 数据...")
    result = await manager.write_rtt("Hello from MCP!")
    print(f"写入结果: {json.dumps(result, indent=2, ensure_ascii=False)}")
    
    # 设置 RTT 通道
    print("\n设置 RTT 通道为 1...")
    result = await manager.set_rtt_channel(1)
    print(f"设置结果: {json.dumps(result, indent=2, ensure_ascii=False)}")
    
    await manager.disconnect()


async def example_reset():
    """示例：重置设备"""
    print("\n=== 示例：重置设备 ===")
    
    manager = JLinkManager()
    await manager.connect(target="STM32F103C8T6")
    
    # 标准重置
    print("标准重置...")
    result = await manager.reset(mode="normal")
    print(f"重置结果: {json.dumps(result, indent=2, ensure_ascii=False)}")
    
    # 自动重连重置
    print("\n自动重连重置...")
    result = await manager.reset(mode="auto_reconnect")
    print(f"重置结果: {json.dumps(result, indent=2, ensure_ascii=False)}")
    
    # 重置并暂停
    print("\n重置并暂停...")
    result = await manager.reset(mode="halt")
    print(f"重置结果: {json.dumps(result, indent=2, ensure_ascii=False)}")
    
    await manager.disconnect()


async def example_memory():
    """示例：内存读写"""
    print("\n=== 示例：内存读写 ===")
    
    manager = JLinkManager()
    await manager.connect(target="STM32F103C8T6")
    
    # 读取内存
    print("读取 Flash 起始地址 (0x08000000)...")
    result = await manager.read_memory(address=0x08000000, size=64)
    print(f"读取结果: {json.dumps(result, indent=2, ensure_ascii=False)}")
    
    # 写入内存
    print("\n写入内存...")
    result = await manager.write_memory(
        address=0x20000000,  # RAM 起始地址
        data="AABBCCDD",
        is_hex=True,
    )
    print(f"写入结果: {json.dumps(result, indent=2, ensure_ascii=False)}")
    
    await manager.disconnect()


async def example_logging():
    """示例：日志记录"""
    print("\n=== 示例：日志记录 ===")
    
    manager = JLinkManager()
    await manager.connect(target="STM32F103C8T6")
    
    # 开始日志记录
    print("开始日志记录...")
    result = manager.start_log_recording(log_dir="logs")
    print(f"开始结果: {json.dumps(result, indent=2, ensure_ascii=False)}")
    
    # 读取一些 RTT 数据（会自动记录到文件）
    print("\n读取 RTT 数据（会记录到日志）...")
    await manager.read_rtt(timeout=2.0)
    
    # 停止日志记录
    print("\n停止日志记录...")
    result = manager.stop_log_recording()
    print(f"停止结果: {json.dumps(result, indent=2, ensure_ascii=False)}")
    
    await manager.disconnect()


async def example_flash():
    """示例：固件烧录"""
    print("\n=== 示例：固件烧录 ===")
    
    manager = JLinkManager()
    await manager.connect(target="STM32F103C8T6")
    
    # 烧录固件
    print("烧录固件...")
    result = await manager.flash_firmware(
        firmware_path="path/to/your/firmware.bin",
        verify=True,
    )
    print(f"烧录结果: {json.dumps(result, indent=2, ensure_ascii=False)}")
    
    await manager.disconnect()


async def main():
    """主函数"""
    print("J-Link RTT Viewer MCP Server 使用示例")
    print("=" * 50)
    
    # 运行示例（需要实际连接 J-Link 和 MCU）
    # 取消注释以运行对应的示例
    
    # await example_connect()
    # await example_rtt()
    # await example_reset()
    # await example_memory()
    # await example_logging()
    # await example_flash()
    
    print("\n注意：要运行这些示例，需要：")
    print("1. 连接 J-Link 调试器")
    print("2. 连接 STM32 开发板")
    print("3. 取消注释对应的示例函数调用")


if __name__ == "__main__":
    asyncio.run(main())