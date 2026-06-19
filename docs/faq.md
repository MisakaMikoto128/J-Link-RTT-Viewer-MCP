# 常见问题解答

## 一般问题

### Q: 什么是 MCP？

A: MCP (Model Context Protocol) 是一个开放标准，用于连接 AI 应用和外部系统。它允许 AI 助手（如 Claude）访问工具、资源和数据。

### Q: 这个项目是做什么的？

A: 这个项目将 J-Link RTT Viewer 的功能封装为 MCP 服务器，使 AI 助手能够通过标准化接口与 STM32 开发板进行交互，包括连接、读取日志、重置设备、烧录固件等。

### Q: 需要什么硬件？

A: 需要：
- J-Link 调试器
- STM32 开发板
- USB 线缆

### Q: 支持哪些 MCU？

A: 支持所有 J-Link 支持的 MCU，包括：
- STM32 系列
- nRF52 系列
- 其他 ARM Cortex-M 系列

## 安装问题

### Q: 如何安装？

A: 
```bash
git clone <repository-url>
cd J-Link-RTT-Viewer-MCP-mimo
pip install -r requirements.txt
```

### Q: Python 版本要求？

A: 需要 Python 3.10 或更高版本。

### Q: 安装失败怎么办？

A: 检查以下几点：
1. Python 版本是否正确
2. pip 是否是最新的：`pip install --upgrade pip`
3. 是否有管理员权限

### Q: 如何更新？

A: 
```bash
git pull origin main
pip install -r requirements.txt
```

## 配置问题

### Q: 如何配置 Claude Desktop？

A: 编辑 `%APPDATA%\Claude\claude_desktop_config.json`，添加服务器配置。详见 [快速入门](QUICKSTART.md)。

### Q: Claude Desktop 看不到 MCP 工具？

A: 检查以下几点：
1. 配置文件路径是否正确
2. Python 路径是否正确
3. 完全重启 Claude Desktop（不是仅关闭窗口）

### Q: 配置文件在哪里？

A: 
- Windows: `%APPDATA%\Claude\claude_desktop_config.json`
- macOS: `~/Library/Application Support/Claude/claude_desktop_config.json`
- Linux: `~/.config/Claude/claude_desktop_config.json`

## 连接问题

### Q: 连接失败怎么办？

A: 检查以下几点：
1. J-Link 是否正确连接
2. MCU 是否上电
3. SWD/JTAG 接线是否正确
4. 尝试降低连接速度

### Q: 连接后断开？

A: 可能原因：
1. 电源不稳定
2. 看门狗触发
3. 接线松动

### Q: 如何检查 J-Link 是否识别？

A: 
- Windows: 设备管理器中查看
- macOS/Linux: `lsusb` 命令

### Q: 连接速度设置多少合适？

A: 
- 默认: 4000 kHz
- 如果连接失败，尝试: 1000 kHz 或 500 kHz

## RTT 问题

### Q: RTT 没有数据？

A: 检查以下几点：
1. MCU 代码是否初始化了 SEGGER_RTT
2. RTT 通道号是否匹配
3. 尝试重置设备

### Q: RTT 数据乱码？

A: 可能原因：
1. 编码不匹配
2. 数据格式错误

### Q: 如何查看所有通道的数据？

A: 默认会读取所有通道 (0-15)。

### Q: RTT 通道号怎么确定？

A: 检查 MCU 代码中 `SEGGER_RTT_ConfigUpBuffer` 或 `SEGGER_RTT_ConfigDownBuffer` 的通道号。

## 烧录问题

### Q: 烧录失败？

A: 检查以下几点：
1. 固件文件是否存在
2. 文件格式是否支持
3. J-Link 连接是否稳定
4. 尝试以管理员身份运行

### Q: 支持哪些固件格式？

A: 支持：
- `.hex` (Intel HEX)
- `.bin` (二进制)
- `.axf` / `.elf` (ARM ELF)

### Q: 烧录后 MCU 无响应？

A: 可能原因：
1. 固件编译错误
2. 链接地址错误
3. 时钟配置错误

### Q: 如何验证烧录是否成功？

A: 可以读取 Flash 内容验证：
```
read_memory(address=0x08000000, size=256)
```

## 内存操作问题

### Q: 地址越界？

A: 检查 MCU 的内存映射：
- STM32F103C8: Flash 0x08000000-0x080FFFFF, RAM 0x20000000-0x20004FFF

### Q: 读写保护？

A: 需要解除读写保护 (RDP)。

### Q: 内存写入失败？

A: 可能原因：
1. 地址不在可写范围
2. 写保护
3. 电源问题

## 日志问题

### Q: 如何查看日志？

A: 
```bash
# MCP 服务器日志
cat jlink_mcp.log

# Claude Desktop 日志
# Windows: %APPDATA%\Claude\logs\mcp.log
# macOS: ~/Library/Logs/Claude/mcp.log
```

### Q: 日志记录在哪里？

A: 默认在 `logs/` 目录。

### Q: 如何停止日志记录？

A: 
```
stop_log_recording()
```

## 性能问题

### Q: 响应很慢？

A: 可能原因：
1. 连接速度太低
2. 读取数据量太大
3. 系统资源不足

### Q: 数据丢失？

A: 可能原因：
1. 读取间隔太长
2. 缓冲区溢出

### Q: 如何提高性能？

A: 
1. 增加连接速度
2. 减少读取间隔
3. 增大缓冲区

## 调试问题

### Q: 如何调试 MCP 服务器？

A: 
1. 查看日志文件
2. 使用 MCP Inspector
3. 手动测试函数

### Q: 如何查看 Claude Desktop 的日志？

A: 
- Windows: `%APPDATA%\Claude\logs\`
- macOS: `~/Library/Logs/Claude/`

### Q: 如何测试 MCP 服务器？

A: 
```bash
# 使用 MCP Inspector
npx -y @modelcontextprotocol/inspector

# 或手动测试
python test_server.py
```

## 其他问题

### Q: 如何贡献代码？

A: 详见 [贡献指南](contributing.md)。

### Q: 如何报告问题？

A: 在 GitHub 上创建 Issue。

### Q: 许可证是什么？

A: MIT License。

### Q: 如何获取帮助？

A: 
1. 查看文档
2. 搜索 GitHub Issues
3. 创建新的 Issue