# 快速入门指南

## 前置条件

1. **Python 3.10+**
2. **J-Link 调试器** (已连接到电脑)
3. **STM32 开发板** (已通过 J-Link 连接)
4. **Claude Desktop** (或其他 MCP 客户端)

## 安装步骤

### 1. 克隆项目

```bash
git clone <repository-url>
cd J-Link-RTT-Viewer-MCP-mimo
```

### 2. 创建虚拟环境

```bash
# Windows
python -m venv .venv
.venv\Scripts\activate

# macOS/Linux
python -m venv .venv
source .venv/bin/activate
```

### 3. 安装依赖

```bash
pip install -r requirements.txt
```

### 4. 验证安装

```bash
# 运行测试
pytest tests/ -v

# 检查代码质量
ruff check src/
```

## 配置 Claude Desktop

### Windows

编辑 `%APPDATA%\Claude\claude_desktop_config.json`：

```json
{
  "mcpServers": {
    "jlink-rtt": {
      "command": "C:\\path\\to\\J-Link-RTT-Viewer-MCP-mimo\\.venv\\Scripts\\python.exe",
      "args": [
        "-m",
        "src.server"
      ],
      "cwd": "C:\\path\\to\\J-Link-RTT-Viewer-MCP-mimo"
    }
  }
}
```

### macOS

编辑 `~/Library/Application Support/Claude/claude_desktop_config.json`：

```json
{
  "mcpServers": {
    "jlink-rtt": {
      "command": "/path/to/J-Link-RTT-Viewer-MCP-mimo/.venv/bin/python",
      "args": [
        "-m",
        "src.server"
      ],
      "cwd": "/path/to/J-Link-RTT-Viewer-MCP-mimo"
    }
  }
}
```

### 重启 Claude Desktop

完全退出并重新启动 Claude Desktop。

## 基本使用

### 连接设备

在 Claude Desktop 中输入：

```
连接到 STM32F103C8T6，使用 SWD 接口，速度 4000kHz
```

### 读取日志

```
读取 RTT 日志
```

### 重置设备

```
重置 MCU
```

### 烧录固件

```
将 build/output.bin 烧录到设备
```

## 验证连接

### 使用 MCP Inspector

```bash
npx -y @modelcontextprotocol/inspector
```

在 Inspector 中连接到 `http://localhost:8000/mcp`。

### 手动测试

```python
import asyncio
from src.core.jlink_manager import JLinkManager

async def test():
    manager = JLinkManager()
    result = await manager.connect(target="STM32F103C8T6")
    print(result)
    await manager.disconnect()

asyncio.run(test())
```

## 常见问题

### Q: 连接失败？

**A:** 检查以下几点：
1. J-Link 是否正确连接到电脑
2. MCU 是否上电
3. SWD/JTAG 接线是否正确
4. 设备管理器中是否识别到 J-Link

### Q: RTT 没有数据？

**A:** 检查以下几点：
1. MCU 代码是否初始化了 SEGGER_RTT
2. RTT 通道号是否匹配（默认是通道 0）
3. 尝试重置设备

### Q: Claude Desktop 看不到 MCP 工具？

**A:** 检查以下几点：
1. 配置文件路径是否正确
2. Python 路径是否正确
3. 完全重启 Claude Desktop（不是仅关闭窗口）

### Q: 烧录失败？

**A:** 检查以下几点：
1. 固件文件是否存在
2. 文件格式是否支持（.hex, .bin, .axf）
3. J-Link 连接是否稳定
4. 尝试以管理员身份运行

## 下一步

- 查看 [API 文档](api.md) 了解所有可用工具
- 查看 [故障排除指南](troubleshooting.md) 解决问题
- 查看 [示例代码](../examples/) 学习更多用法