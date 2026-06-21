@echo off
REM J-Link RTT Viewer MCP Server Installation Script

echo ========================================
echo J-Link RTT Viewer MCP Server Installation
echo ========================================
echo.

REM Check Python
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] Python not installed or not in PATH
    echo Please install Python 3.10+ first
    pause
    exit /b 1
)

REM Create virtual environment
echo [INFO] Creating virtual environment...
if not exist ".venv" (
    python -m venv .venv
) else (
    echo [INFO] Virtual environment already exists
)

REM Activate virtual environment
echo [INFO] Activating virtual environment...
call .venv\Scripts\activate.bat

REM Install package with dev dependencies
echo [INFO] Installing package...
pip install -e ".[dev]"

echo.
echo ========================================
echo Installation complete!
echo ========================================
echo.
echo Usage:
echo 1. Run start.bat to start MCP server
echo 2. Or run: python -m src.server
echo.
echo Configure Claude Desktop:
echo Edit %%APPDATA%%\Claude\claude_desktop_config.json
echo Add the following:
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
