# 部署指南

## 本地部署

### 开发模式

```bash
# 克隆项目
git clone <repository-url>
cd J-Link-RTT-Viewer-MCP-mimo

# 创建虚拟环境
python -m venv .venv
.venv\Scripts\activate

# 安装依赖
pip install -r requirements.txt
pip install -e .

# 运行服务器
python -m src.server
```

### 生产模式

```bash
# 安装依赖
pip install -r requirements.txt

# 运行服务器
python -m src.server
```

## 配置 Claude Desktop

### Windows

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

### macOS

编辑 `~/Library/Application Support/Claude/claude_desktop_config.json`：

```json
{
  "mcpServers": {
    "jlink-rtt": {
      "command": "/path/to/J-Link-RTT-Viewer-MCP-mimo/.venv/bin/python",
      "args": ["-m", "src.server"],
      "cwd": "/path/to/J-Link-RTT-Viewer-MCP-mimo"
    }
  }
}
```

### Linux

编辑 `~/.config/Claude/claude_desktop_config.json`：

```json
{
  "mcpServers": {
    "jlink-rtt": {
      "command": "/path/to/J-Link-RTT-Viewer-MCP-mimo/.venv/bin/python",
      "args": ["-m", "src.server"],
      "cwd": "/path/to/J-Link-RTT-Viewer-MCP-mimo"
    }
  }
}
```

## 使用 systemd 部署 (Linux)

### 创建服务文件

```bash
sudo nano /etc/systemd/system/jlink-mcp.service
```

### 添加以下内容

```ini
[Unit]
Description=J-Link RTT Viewer MCP Server
After=network.target

[Service]
Type=simple
User=your-username
WorkingDirectory=/path/to/J-Link-RTT-Viewer-MCP-mimo
ExecStart=/path/to/J-Link-RTT-Viewer-MCP-mimo/.venv/bin/python -m src.server
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
```

### 启动服务

```bash
sudo systemctl daemon-reload
sudo systemctl enable jlink-mcp
sudo systemctl start jlink-mcp

# 查看状态
sudo systemctl status jlink-mcp

# 查看日志
sudo journalctl -u jlink-mcp -f
```

## 使用 Docker 部署

### 创建 Dockerfile

```dockerfile
FROM python:3.10-slim

WORKDIR /app

# 安装依赖
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# 复制代码
COPY . .

# 暴露端口（如果使用 HTTP 传输）
# EXPOSE 8000

# 运行服务器
CMD ["python", "-m", "src.server"]
```

### 构建和运行

```bash
# 构建镜像
docker build -t jlink-mcp .

# 运行容器
docker run -d \
  --name jlink-mcp \
  --device=/dev/bus/usb \  # 需要访问 USB 设备
  jlink-mcp
```

## 环境变量

可以通过环境变量配置服务器：

```bash
# 日志级别
export JLINK_MCP_LOG_LEVEL=INFO

# 日志文件路径
export JLINK_MCP_LOG_FILE=jlink_mcp.log

# 默认连接参数
export JLINK_MCP_DEFAULT_TARGET=STM32F103C8T6
export JLINK_MCP_DEFAULT_INTERFACE=SWD
export JLINK_MCP_DEFAULT_SPEED=4000
```

## 常见部署问题

### 权限问题

**问题**: 无法访问 USB 设备

**解决方案**:
```bash
# Linux: 添加用户到 plugdev 组
sudo usermod -aG plugdev $USER

# 或使用 udev 规则
sudo nano /etc/udev/rules.d/99-jlink.rules
```

添加以下内容：
```
SUBSYSTEM=="usb", ATTR{idVendor}=="1366", MODE="0666"
```

然后重新加载规则：
```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```

### 路径问题

**问题**: Python 路径不正确

**解决方案**:
- 使用绝对路径
- 确认虚拟环境已激活
- 检查 Python 版本

### 依赖问题

**问题**: 缺少依赖

**解决方案**:
```bash
pip install -r requirements.txt
pip install -e .
```

### 端口冲突

**问题**: 端口已被占用

**解决方案**:
- 检查是否有其他 MCP 服务器运行
- 修改配置使用不同端口

## 监控和日志

### 查看日志

```bash
# 实时查看日志
tail -f jlink_mcp.log

# 搜索错误
grep ERROR jlink_mcp.log
```

### 性能监控

```bash
# 查看进程
ps aux | grep jlink

# 查看资源使用
top -p $(pgrep -f "jlink-mcp")
```

## 更新和维护

### 更新代码

```bash
git pull origin main
pip install -r requirements.txt
```

### 重启服务

```bash
# systemd
sudo systemctl restart jlink-mcp

# Docker
docker restart jlink-mcp
```

## 备份

### 备份配置

```bash
cp %APPDATA%\Claude\claude_desktop_config.json backup/
```

### 备份日志

```bash
cp jlink_mcp.log backup/
```

## 卸载

### 停止服务

```bash
# systemd
sudo systemctl stop jlink-mcp
sudo systemctl disable jlink-mcp
sudo rm /etc/systemd/system/jlink-mcp.service

# Docker
docker stop jlink-mcp
docker rm jlink-mcp
```

### 删除文件

```bash
rm -rf /path/to/J-Link-RTT-Viewer-MCP-mimo
```