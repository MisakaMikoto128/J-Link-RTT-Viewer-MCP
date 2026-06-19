@echo off
REM J-Link RTT Viewer MCP Server 启动脚本

echo ========================================
echo J-Link RTT Viewer MCP Server
echo ========================================
echo.

REM 检查虚拟环境
if not exist ".venv\Scripts\activate.bat" (
    echo [ERROR] 虚拟环境未找到
    echo 请先运行: python -m venv .venv
    pause
    exit /b 1
)

REM 激活虚拟环境
call .venv\Scripts\activate.bat

REM 检查依赖
echo [INFO] 检查依赖...
pip show mcp >nul 2>&1
if %errorlevel% neq 0 (
    echo [INFO] 安装依赖...
    pip install -r requirements.txt
)

REM 启动服务器
echo [INFO] 启动 MCP 服务器...
echo [INFO] 按 Ctrl+C 停止服务器
echo.
python -m src.server

pause