# 快速入门

## 5 分钟开始

### 1. 安装

```bash
# 克隆项目
git clone <repository-url>
cd J-Link-RTT-Viewer-MCP-mimo

# Windows 用户运行安装脚本
install.bat

# 或手动安装
python -m venv .venv
.venv\Scripts\activate
pip install -r requirements.txt
```

### 2. 配置 Claude Desktop

编辑 `%APPDATA%\Claude\claude_desktop_config.json`：

```json
{
  "mcpServers": {
    "jlink-rtt": {
      "command": "C:\\path\\to\\J-Link-RTT-Viewer-MCP-mimo\\.venv\\Scripts\\python.exe",
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

## 常用命令

### 连接设备
```
连接到 STM32F103C8T6，使用 SWD 接口
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

### 断开连接
```
断开 J-Link 连接
```

## 故障排除

### 看不到 MCP 工具？

1. 检查配置文件路径
2. 完全重启 Claude Desktop
3. 查看日志: `jlink_mcp.log`

### 连接失败？

1. 检查 J-Link 连接
2. 确认 MCU 上电
3. 降低连接速度: `connect(target="STM32F103C8T6", speed=1000)`

### RTT 没有数据？

1. 确认 MCU 代码初始化了 SEGGER_RTT
2. 检查 RTT 通道号
3. 尝试重置: `reset(mode="auto_reconnect")`

## 下一步

- 查看 [完整文档](docs/)
- 了解 [API 参考](docs/api.md)
- 查看 [示例代码](examples/)