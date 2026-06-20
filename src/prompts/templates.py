"""提示模板 - MCP 提示模板实现"""


def get_debugging_prompt() -> str:
    """调试指南提示"""
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


def get_error_diagnosis_prompt(error_message: str) -> str:
    """错误诊断提示"""
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


def get_flash_wizard_prompt() -> str:
    """烧录向导提示"""
    return """# 固件烧录向导

## 烧录前检查

1. J-Link 已连接
2. MCU 已上电
3. 固件文件已准备好

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
