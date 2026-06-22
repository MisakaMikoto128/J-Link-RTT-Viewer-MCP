# J-Link RTT Viewer MCP Server

MCP server for controlling STM32 boards via J-Link RTT. AI assistants use this to read logs, send commands, and flash firmware through natural language.

## Quick Commands

```bash
# Install
pip install -r requirements.txt

# Run tests
pytest tests/ -v

# Type check
mypy src/ --ignore-missing-imports

# Lint
ruff check src/

# Start server
python -m src.server

# Test with inspector
npx @modelcontextprotocol/inspector -- python -m src.server
```

## MCP Tools Available

| Tool | Key Parameters | What It Does |
|------|---------------|--------------|
| `connect` | `target="STM32F103C8"`, `interface="SWD"` | Connect to MCU |
| `disconnect` | - | Disconnect |
| `read_rtt` | `channels=[0]` | Read RTT logs from MCU |
| `write_rtt` | `data="freq 200"` | Send command to MCU |
| `reset` | `mode="normal"` | Reset MCU |
| `flash_firmware` | `firmware_path="path.hex"` | Flash firmware |
| `read_memory` | `address=0x08000000, size=256` | Read MCU memory |
| `write_memory` | `address=0x20000000, data="AABB"` | Write MCU memory |
| `get_device_info` | - | Get device details |

## MCU Commands (via write_rtt)

```
freq <ms>     - Set blink interval (50-5000ms)
mode <on|off|blink> - Set LED mode
status        - Get current settings
ping          - Test connection
reset         - Software reset
```

## Project Structure

```
src/
  server.py              # MCP server entry
  core/jlink_manager.py  # J-Link hardware interface
tests/                   # 125 tests
target_example/          # STM32 test firmware
```

## Key Technical Details

- Uses `pylink-square==1.6.0` for J-Link communication
- RTT reads use pylink's built-in RTT API
- Thread-safe via `_run_serialized` for J-Link operations
- Device names: `STM32F103C8` (not `STM32F103C8T6`)
