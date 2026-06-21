# J-Link RTT Viewer MCP Server

[English](#english) | [中文](#中文)

---

## English

An MCP (Model Context Protocol) server that wraps J-Link RTT Viewer functionality, enabling AI assistants to interact with STM32 development boards through a standardized interface.

### What is MCP?

MCP is like a USB-C port for AI applications. It provides a standardized way for AI assistants (Claude, ChatGPT, etc.) to connect to external systems like J-Link debuggers.

### Quick Start

#### 1. Install

```bash
git clone https://github.com/MisakaMikoto128/J-Link-RTT-Viewer-MCP.git
cd J-Link-RTT-Viewer-MCP

# Option A: Simple install (recommended for users)
pip install -r requirements.txt

# Option B: Development install (for contributors)
pip install -e ".[dev]"
```

#### 2. Configure in Claude Desktop

Add to `%APPDATA%\Claude\claude_desktop_config.json`:

```json
{
  "mcpServers": {
    "jlink-rtt": {
      "command": "python",
      "args": ["-m", "src.server"],
      "cwd": "C:\\path\\to\\J-Link-RTT-Viewer-MCP"
    }
  }
}
```

#### 3. Restart Claude Desktop

#### 4. Use natural language!

That's it! No code required. Just talk to Claude:

```
You: Connect to my STM32F103C8 board
Claude: [calls connect tool] Connected successfully!

You: Read the RTT logs
Claude: [calls read_rtt tool] Here are the logs...

You: Make the LED blink faster
Claude: [calls write_rtt tool with "freq 200"] Done!

You: Flash the new firmware
Claude: [calls flash_firmware tool] Firmware flashed successfully!
```

### Available Tools

| Tool | What it does |
|------|-------------|
| `connect` | Connect to J-Link and target MCU |
| `disconnect` | Disconnect from J-Link |
| `read_rtt` | Read RTT log data |
| `write_rtt` | Write data to RTT channel |
| `set_rtt_channel` | Set default RTT channel (0-15) |
| `reset` | Reset MCU (normal/auto_reconnect/halt) |
| `get_device_info` | Get connected device info |
| `flash_firmware` | Flash .hex or .bin firmware |
| `read_memory` | Read MCU memory |
| `write_memory` | Write MCU memory |
| `start_log_recording` | Record RTT to file |
| `stop_log_recording` | Stop recording |

### Test with MCP Inspector

```bash
npx @modelcontextprotocol/inspector -- python -m src.server
```

### Supported Hardware

- **Debugger**: SEGGER J-Link (V7, V8, V9, V10)
- **MCU**: STM32F1xx, STM32F4xx, nRF52xxx
- **Interface**: SWD, JTAG

### Development

```bash
# Run tests
pytest tests/ -v

# Lint
ruff check src/

# Type check
mypy src/ --ignore-missing-imports
```

### License

MIT License

---

## 中文

一个封装 J-Link RTT Viewer 功能的 MCP 服务器，使 AI 助手能够通过标准化接口与 STM32 开发板进行交互。

### 什么是 MCP？

MCP 就像 AI 应用的 USB-C 接口。它为 AI 助手（Claude、ChatGPT 等）提供了一种标准化的方式来连接外部系统，如 J-Link 调试器。

### 快速开始

#### 1. 安装

```bash
git clone https://github.com/MisakaMikoto128/J-Link-RTT-Viewer-MCP.git
cd J-Link-RTT-Viewer-MCP

# 方式 A：简单安装（推荐用户使用）
pip install -r requirements.txt

# 方式 B：开发安装（贡献者使用）
pip install -e ".[dev]"
```

#### 2. 在 Claude Desktop 中配置

添加到 `%APPDATA%\Claude\claude_desktop_config.json`:

```json
{
  "mcpServers": {
    "jlink-rtt": {
      "command": "python",
      "args": ["-m", "src.server"],
      "cwd": "C:\\path\\to\\J-Link-RTT-Viewer-MCP"
    }
  }
}
```

#### 3. 重启 Claude Desktop

#### 4. 用自然语言对话！

就这样！不需要写代码。直接和 Claude 对话：

```
你: 连接到我的 STM32F103C8 开发板
Claude: [调用 connect 工具] 连接成功！

你: 读取 RTT 日志
Claude: [调用 read_rtt 工具] 这是日志数据...

你: 让 LED 闪烁更快
Claude: [调用 write_rtt 工具，参数 "freq 200"] 完成！

你: 烧录新固件
Claude: [调用 flash_firmware 工具] 固件烧录成功！
```

### 可用工具

| 工具 | 功能 |
|------|------|
| `connect` | 连接 J-Link 和目标 MCU |
| `disconnect` | 断开 J-Link 连接 |
| `read_rtt` | 读取 RTT 日志数据 |
| `write_rtt` | 向 RTT 通道写入数据 |
| `set_rtt_channel` | 设置默认 RTT 通道 (0-15) |
| `reset` | 重置 MCU (normal/auto_reconnect/halt) |
| `get_device_info` | 获取已连接设备信息 |
| `flash_firmware` | 烧录 .hex 或 .bin 固件 |
| `read_memory` | 读取 MCU 内存 |
| `write_memory` | 写入 MCU 内存 |
| `start_log_recording` | 录制 RTT 到文件 |
| `stop_log_recording` | 停止录制 |

### 使用 MCP Inspector 测试

```bash
npx @modelcontextprotocol/inspector -- python -m src.server
```

### 支持的硬件

- **调试器**: SEGGER J-Link (V7, V8, V9, V10)
- **MCU**: STM32F1xx, STM32F4xx, nRF52xxx
- **接口**: SWD, JTAG

### 开发

```bash
# 运行测试
pytest tests/ -v

# 代码检查
ruff check src/

# 类型检查
mypy src/ --ignore-missing-imports
```

### 许可证

MIT 许可证
