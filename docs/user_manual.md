# 用户手册

## 简介

J-Link RTT Viewer MCP Server 是一个将 J-Link RTT Viewer 功能封装为 MCP 服务器的工具，使 AI 助手能够通过标准化接口与 STM32 开发板进行交互。

## 快速开始

### 1. 安装

```bash
# 克隆项目
git clone <repository-url>
cd J-Link-RTT-Viewer-MCP-mimo

# 安装依赖
pip install -r requirements.txt
```

### 2. 配置 Claude Desktop

编辑 `%APPDATA%\Claude\claude_desktop_config.json`：

```json
{
  "mcpServers": {
    "jlink-rtt": {
      "command": "python",
      "args": ["-m", "src.server"],
      "cwd": "C:\\path\\to\\J-Link-RTT-Viewer-MCP-mimo"
    }
  }
}
```

### 3. 重启 Claude Desktop

完全退出并重新启动 Claude Desktop。

### 4. 测试连接

在 Claude Desktop 中输入：

```
连接到 STM32F103C8T6
```

## 功能概述

### 连接管理

- **连接设备**: 连接到 J-Link 调试器和目标 MCU
- **断开连接**: 安全断开连接并清理资源
- **获取设备信息**: 查看连接设备的详细信息

### RTT 功能

- **读取日志**: 从所有通道读取 RTT 日志数据
- **写入数据**: 向 RTT 通道发送数据
- **设置通道**: 设置默认 RTT 通道

### 设备控制

- **重置设备**: 支持多种重置模式
- **烧录固件**: 烧录固件到 MCU
- **读写内存**: 读取和写入 MCU 内存

### 日志记录

- **开始记录**: 将 RTT 数据记录到文件
- **停止记录**: 停止日志记录

## 使用示例

### 连接设备

```
连接到 STM32F103C8T6，使用 SWD 接口，速度 4000kHz
```

### 读取日志

```
读取 RTT 日志
```

### 重置设备

```
重置 MCU 并自动重连
```

### 烧录固件

```
将 build/output.bin 烧录到设备
```

### 读取内存

```
读取地址 0x08000000 的 256 字节数据
```

### 写入内存

```
向地址 0x20000000 写入数据 AABBCCDD
```

## 命令参考

### connect

**参数:**
- `target` (必需): 目标设备型号
- `interface` (可选): 调试接口，默认 "SWD"
- `speed` (可选): 连接速度，默认 4000
- `channel` (可选): RTT 通道号，默认 0
- `retry_attempts` (可选): 重试次数，默认 3

**示例:**
```
connect(target="STM32F103C8T6", interface="SWD", speed=4000)
```

### disconnect

**参数:** 无

**示例:**
```
disconnect()
```

### get_device_info

**参数:** 无

**示例:**
```
get_device_info()
```

### read_rtt

**参数:**
- `channels` (可选): 通道列表，默认所有通道
- `timeout` (可选): 超时时间，默认 1.0 秒
- `max_size` (可选): 最大读取字节数，默认 4096

**示例:**
```
read_rtt(channels=[0, 1], timeout=2.0)
```

### write_rtt

**参数:**
- `data` (必需): 要写入的数据
- `channel` (可选): 通道号，默认使用默认通道
- `is_hex` (可选): 是否为十六进制数据，默认 false

**示例:**
```
write_rtt(data="Hello", channel=0)
```

### set_rtt_channel

**参数:**
- `channel` (必需): 通道号 (0-15)

**示例:**
```
set_rtt_channel(channel=5)
```

### reset

**参数:**
- `mode` (可选): 重置模式，默认 "normal"
- `wait_time` (可选): 等待时间，默认 0.3 秒

**重置模式:**
- `normal`: 标准重置
- `auto_reconnect`: 自动重连
- `halt`: 重置并暂停

**示例:**
```
reset(mode="auto_reconnect")
```

### flash_firmware

**参数:**
- `firmware_path` (必需): 固件文件路径
- `verify` (可选): 是否验证，默认 true

**示例:**
```
flash_firmware(firmware_path="build/output.bin", verify=True)
```

### read_memory

**参数:**
- `address` (必需): 起始地址
- `size` (必需): 读取字节数

**示例:**
```
read_memory(address=0x08000000, size=256)
```

### write_memory

**参数:**
- `address` (必需): 起始地址
- `data` (必需): 要写入的数据
- `is_hex` (可选): 是否为十六进制数据，默认 true

**示例:**
```
write_memory(address=0x20000000, data="AABBCCDD")
```

### start_log_recording

**参数:**
- `log_dir` (可选): 日志目录，默认 "logs"

**示例:**
```
start_log_recording(log_dir="logs")
```

### stop_log_recording

**参数:** 无

**示例:**
```
stop_log_recording()
```

## 故障排除

### 连接失败

**可能原因:**
- J-Link 未连接
- MCU 未上电
- 接线错误
- 速度太快

**解决方案:**
1. 检查 J-Link 连接
2. 确认 MCU 上电
3. 检查 SWD/JTAG 接线
4. 降低连接速度

### RTT 没有数据

**可能原因:**
- MCU 未初始化 SEGGER_RTT
- RTT 通道不匹配
- 缓冲区太小

**解决方案:**
1. 检查 MCU 代码
2. 确认 RTT 通道号
3. 尝试重置设备

### 烧录失败

**可能原因:**
- 固件文件不存在
- 文件格式不支持
- 连接不稳定
- 读写保护

**解决方案:**
1. 检查文件路径
2. 确认文件格式
3. 检查 J-Link 连接
4. 解除读写保护

## 技术支持

- 查看 [故障排除指南](troubleshooting.md)
- 查看 [API 文档](api.md)
- 提交 GitHub Issue