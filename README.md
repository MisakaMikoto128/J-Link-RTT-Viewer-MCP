# J-Link RTT Viewer MCP Server

[English](#english) | [中文](#中文)

---

## English

An MCP server that wraps J-Link RTT Viewer, enabling AI assistants to control STM32 development boards via natural language.

### Quick Start (3 steps)

```bash
# 1. Clone and install
git clone https://github.com/MisakaMikoto128/J-Link-RTT-Viewer-MCP.git
cd J-Link-RTT-Viewer-MCP
python -m venv .venv && .venv\Scripts\activate  # Windows
pip install -r requirements.txt

# 2. Configure Claude Desktop
# Add to %APPDATA%\Claude\claude_desktop_config.json:
# { "mcpServers": { "jlink-rtt": { "command": "python", "args": ["-m", "src.server"], "cwd": "C:\\path\\to\\J-Link-RTT-Viewer-MCP" } } }

# 3. Restart Claude Desktop and use natural language!
```

### Tool Reference

| Tool | Parameters | Description |
|------|-----------|-------------|
| `connect` | `target`, `interface="SWD"`, `speed=4000` | Connect to MCU. Target example: `STM32F103C8` |
| `disconnect` | (none) | Disconnect from J-Link |
| `read_rtt` | `channels=[0]`, `max_size=4096` | Read RTT log data from MCU |
| `write_rtt` | `data`, `channel=0` | Send command to MCU via RTT |
| `reset` | `mode="normal"` | Reset MCU. Modes: normal, auto_reconnect, halt |
| `flash_firmware` | `firmware_path` | Flash .hex or .bin file to MCU |
| `get_device_info` | (none) | Get connected device details |
| `read_memory` | `address`, `size` | Read MCU memory (hex address like 0x08000000) |
| `write_memory` | `address`, `data` | Write data to MCU memory |
| `start_log_recording` | `log_dir="logs"` | Record RTT output to file |
| `stop_log_recording` | (none) | Stop recording |
| `set_rtt_channel` | `channel` | Set default RTT channel (0-15) |

### Common Commands (for write_rtt)

```
freq 200      - Set LED blink interval to 200ms
freq 1000     - Set LED blink interval to 1s
mode on       - Turn LED on
mode off      - Turn LED off
mode blink    - Set LED to blink mode
status        - Get current settings
ping          - Test connection (returns "pong")
reset         - Reset MCU software
```

### Example: Control LED

```
User: Connect to my STM32F103C8 board
AI: [connect] Connected to STM32F103C8

User: What's in the RTT logs?
AI: [read_rtt] STM32F103C8T6 LED Control ===\r\nCommands: freq, mode, status...

User: Make the LED blink faster
AI: [write_rtt, data="freq 200"] Done! LED now blinks every 200ms

User: Turn the LED on
AI: [write_rtt, data="mode on"] LED is now on

User: Flash new firmware
AI: [flash_firmware, firmware_path="path/to/firmware.hex"] Flash complete!
```

### Supported Hardware

- **Debugger**: SEGGER J-Link (V7-V10)
- **MCU**: STM32F1xx, STM32F4xx, nRF52xxx, etc.
- **Interface**: SWD, JTAG

### Test with MCP Inspector

```bash
npx @modelcontextprotocol/inspector -- python -m src.server
```

---

## 中文

封装 J-Link RTT Viewer 的 MCP 服务器，使 AI 助手能通过自然语言控制 STM32 开发板。

### 快速开始（3 步）

```bash
# 1. 克隆并安装
git clone https://github.com/MisakaMikoto128/J-Link-RTT-Viewer-MCP.git
cd J-Link-RTT-Viewer-MCP
python -m venv .venv && .venv\Scripts\activate  # Windows
pip install -r requirements.txt

# 2. 配置 Claude Desktop
# 添加到 %APPDATA%\Claude\claude_desktop_config.json:
# { "mcpServers": { "jlink-rtt": { "command": "python", "args": ["-m", "src.server"], "cwd": "C:\\path\\to\\J-Link-RTT-Viewer-MCP" } } }

# 3. 重启 Claude Desktop，用自然语言对话！
```

### 工具参考

| 工具 | 参数 | 说明 |
|------|------|------|
| `connect` | `target`, `interface="SWD"`, `speed=4000` | 连接 MCU。示例: `STM32F103C8` |
| `disconnect` | (无) | 断开 J-Link |
| `read_rtt` | `channels=[0]`, `max_size=4096` | 读取 MCU 的 RTT 日志 |
| `write_rtt` | `data`, `channel=0` | 通过 RTT 发送命令到 MCU |
| `reset` | `mode="normal"` | 重置 MCU。模式: normal, auto_reconnect, halt |
| `flash_firmware` | `firmware_path` | 烧录 .hex 或 .bin 文件 |
| `get_device_info` | (无) | 获取设备详细信息 |
| `read_memory` | `address`, `size` | 读取 MCU 内存 |
| `write_memory` | `address`, `data` | 写入 MCU 内存 |
| `start_log_recording` | `log_dir="logs"` | 录制 RTT 输出到文件 |
| `stop_log_recording` | (无) | 停止录制 |
| `set_rtt_channel` | `channel` | 设置默认 RTT 通道 (0-15) |

### 常用命令（用于 write_rtt）

```
freq 200      - 设置 LED 闪烁间隔为 200ms
freq 1000     - 设置 LED 闪烁间隔为 1s
mode on       - 打开 LED
mode off      - 关闭 LED
mode blink    - 设置 LED 闪烁模式
status        - 获取当前设置
ping          - 测试连接（返回 "pong"）
reset         - 软件重置 MCU
```

### 示例：控制 LED

```
用户: 连接到我的 STM32F103C8 开发板
AI: [connect] 已连接到 STM32F103C8

用户: RTT 日志里有什么？
AI: [read_rtt] STM32F103C8T6 LED Control ===\r\nCommands: freq, mode, status...

用户: 让 LED 闪快一点
AI: [write_rtt, data="freq 200"] 完成！LED 现在每 200ms 闪烁一次

用户: 打开 LED
AI: [write_rtt, data="mode on"] LED 已打开

用户: 烧录新固件
AI: [flash_firmware, firmware_path="path/to/firmware.hex"] 烧录完成！
```

### 支持的硬件

- **调试器**: SEGGER J-Link (V7-V10)
- **MCU**: STM32F1xx, STM32F4xx, nRF52xxx 等
- **接口**: SWD, JTAG

### 使用 MCP Inspector 测试

```bash
npx @modelcontextprotocol/inspector -- python -m src.server
```
