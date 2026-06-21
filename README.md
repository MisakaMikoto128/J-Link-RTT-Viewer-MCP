# J-Link RTT Viewer MCP Server

[English](#english) | [中文](#中文)

---

## English

An MCP (Model Context Protocol) server that wraps J-Link RTT Viewer functionality, enabling AI assistants to interact with STM32 development boards through a standardized interface.

### Features

- **RTT Communication**: Read/write Real-Time Transfer data from embedded devices
- **Device Control**: Connect, disconnect, reset, and flash firmware
- **Memory Access**: Read/write MCU memory for debugging
- **AI-Friendly**: Structured JSON output with timestamps, channels, and metadata
- **Thread-Safe**: Serialized J-Link operations prevent concurrent access issues

### Supported Hardware

- **Debugger**: SEGGER J-Link (V7, V8, V9, V10)
- **MCU**: STM32F1xx, STM32F4xx, nRF52xxx, and other J-Link compatible devices
- **Interface**: SWD, JTAG

### Quick Start

#### Prerequisites

- Python 3.10+
- J-Link drivers installed ([Download](https://www.segger.com/products/debug-probes/j-link/drivers/))
- J-Link debugger connected to your development board

#### Installation

```bash
# Clone the repository
git clone https://github.com/your-username/J-Link-RTT-Viewer-MCP-mimo.git
cd J-Link-RTT-Viewer-MCP-mimo

# Create virtual environment
python -m venv .venv
.venv\Scripts\activate  # Windows
# source .venv/bin/activate  # Linux/Mac

# Install dependencies
pip install -r requirements.txt
```

#### Configure MCP Client

Add to your Claude Desktop configuration (`%APPDATA%\Claude\claude_desktop_config.json`):

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

#### Usage Example

```
# Connect to device
connect(target="STM32F103C8", interface="SWD", speed=4000)

# Read RTT logs
read_rtt()

# Control LED via RTT
write_rtt("freq 200")   # Set blink frequency to 200ms
write_rtt("mode on")     # Turn LED on
write_rtt("status")      # Check current settings

# Flash firmware
flash_firmware(firmware_path="path/to/firmware.hex")

# Disconnect
disconnect()
```

### Available Tools

| Tool | Description |
|------|-------------|
| `connect` | Connect to J-Link and target MCU |
| `disconnect` | Disconnect from J-Link |
| `read_rtt` | Read RTT log data from channels |
| `write_rtt` | Write data to RTT channel |
| `set_rtt_channel` | Set default RTT channel (0-15) |
| `reset` | Reset MCU (normal/auto_reconnect/halt) |
| `get_device_info` | Get connected device information |
| `flash_firmware` | Flash .hex or .bin firmware |
| `read_memory` | Read MCU memory |
| `write_memory` | Write MCU memory |
| `start_log_recording` | Start recording RTT to file |
| `stop_log_recording` | Stop recording |

### Project Structure

```
J-Link-RTT-Viewer-MCP-mimo/
├── src/
│   ├── server.py              # MCP server entry point
│   ├── core/
│   │   ├── jlink_manager.py   # J-Link hardware interface
│   │   └── rtt_handler.py     # RTT data processing
│   ├── tools/                 # MCP tool implementations
│   ├── resources/             # MCP resource implementations
│   └── prompts/               # MCP prompt templates
├── tests/                     # Test suite (125 tests)
├── target_example/            # STM32 test firmware
├── requirements.txt
└── pyproject.toml
```

### Testing

```bash
# Run all tests
pytest tests/ -v

# Run with coverage
pytest tests/ --cov=src --cov-report=html
```

### Development

```bash
# Install dev dependencies
pip install -r requirements.txt

# Run linting
ruff check src/

# Run type checking
mypy src/ --ignore-missing-imports

# Format code
ruff format src/
```

### License

MIT License - see [LICENSE](LICENSE) for details.

---

## 中文

一个封装 J-Link RTT Viewer 功能的 MCP (Model Context Protocol) 服务器，使 AI 助手能够通过标准化接口与 STM32 开发板进行交互。

### 功能特性

- **RTT 通信**: 从嵌入式设备读写实时传输数据
- **设备控制**: 连接、断开、重置和烧录固件
- **内存访问**: 读写 MCU 内存用于调试
- **AI 友好**: 结构化 JSON 输出，包含时间戳、通道和元数据
- **线程安全**: 序列化 J-Link 操作防止并发访问问题

### 支持的硬件

- **调试器**: SEGGER J-Link (V7, V8, V9, V10)
- **MCU**: STM32F1xx, STM32F4xx, nRF52xxx 及其他 J-Link 兼容设备
- **接口**: SWD, JTAG

### 快速开始

#### 前置条件

- Python 3.10+
- 已安装 J-Link 驱动 ([下载](https://www.segger.com/products/debug-probes/j-link/drivers/))
- J-Link 调试器已连接到开发板

#### 安装

```bash
# 克隆仓库
git clone https://github.com/your-username/J-Link-RTT-Viewer-MCP-mimo.git
cd J-Link-RTT-Viewer-MCP-mimo

# 创建虚拟环境
python -m venv .venv
.venv\Scripts\activate  # Windows
# source .venv/bin/activate  # Linux/Mac

# 安装依赖
pip install -r requirements.txt
```

#### 配置 MCP 客户端

在 Claude Desktop 配置文件中添加 (`%APPDATA%\Claude\claude_desktop_config.json`):

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

#### 使用示例

```
# 连接设备
connect(target="STM32F103C8", interface="SWD", speed=4000)

# 读取 RTT 日志
read_rtt()

# 通过 RTT 控制 LED
write_rtt("freq 200")   # 设置闪烁频率为 200ms
write_rtt("mode on")     # 打开 LED
write_rtt("status")      # 查看当前设置

# 烧录固件
flash_firmware(firmware_path="path/to/firmware.hex")

# 断开连接
disconnect()
```

### 可用工具

| 工具 | 描述 |
|------|------|
| `connect` | 连接 J-Link 和目标 MCU |
| `disconnect` | 断开 J-Link 连接 |
| `read_rtt` | 从通道读取 RTT 日志数据 |
| `write_rtt` | 向 RTT 通道写入数据 |
| `set_rtt_channel` | 设置默认 RTT 通道 (0-15) |
| `reset` | 重置 MCU (normal/auto_reconnect/halt) |
| `get_device_info` | 获取已连接设备信息 |
| `flash_firmware` | 烧录 .hex 或 .bin 固件 |
| `read_memory` | 读取 MCU 内存 |
| `write_memory` | 写入 MCU 内存 |
| `start_log_recording` | 开始录制 RTT 到文件 |
| `stop_log_recording` | 停止录制 |

### 项目结构

```
J-Link-RTT-Viewer-MCP-mimo/
├── src/
│   ├── server.py              # MCP 服务器入口
│   ├── core/
│   │   ├── jlink_manager.py   # J-Link 硬件接口
│   │   └── rtt_handler.py     # RTT 数据处理
│   ├── tools/                 # MCP 工具实现
│   ├── resources/             # MCP 资源实现
│   └── prompts/               # MCP 提示模板
├── tests/                     # 测试套件 (125 个测试)
├── target_example/            # STM32 测试固件
├── requirements.txt
└── pyproject.toml
```

### 测试

```bash
# 运行所有测试
pytest tests/ -v

# 运行带覆盖率的测试
pytest tests/ --cov=src --cov-report=html
```

### 开发

```bash
# 安装开发依赖
pip install -r requirements.txt

# 运行 lint 检查
ruff check src/

# 运行类型检查
mypy src/ --ignore-missing-imports

# 格式化代码
ruff format src/
```

### 许可证

MIT 许可证 - 详见 [LICENSE](LICENSE)。
