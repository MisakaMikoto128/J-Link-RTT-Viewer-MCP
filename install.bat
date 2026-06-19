@echo off
REM J-Link RTT Viewer MCP Server 安装脚本

echo ========================================
echo J-Link RTT Viewer MCP Server 安装
echo ========================================
echo.

REM 检查 Python
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] Python 未安装或未添加到 PATH
    echo 请先安装 Python 3.10+
    pause
    exit /b 1
)

REM 创建虚拟环境
echo [INFO] 创建虚拟环境...
if not exist ".venv" (
    python -m venv .venv
) else (
    echo [INFO] 虚拟环境已存在
)

REM 激活虚拟环境
echo [INFO] 激活虚拟环境...
call .venv\Scripts\activate.bat

REM 安装依赖
echo [INFO] 安装依赖...
pip install -r requirements.txt

REM 安装包
echo [INFO] 安装包...
pip install -e .

echo.
echo ========================================
echo 安装完成！
echo ========================================
echo.
echo 使用方法:
echo 1. 运行 start.bat 启动 MCP 服务器
echo 2. 或运行: python -m src.server
echo.
echo 配置 Claude Desktop:
echo 编辑 %%APPDATA%%\Claude\claude_desktop_config.json
echo 添加以下内容:
echo {
echo   "mcpServers": {
echo     "jlink-rtt": {
echo       "command": "python",
echo       "args": ["-m", "src.server"],
echo       "cwd": "%CD%"
echo     }
echo   }
echo }
echo.
pause