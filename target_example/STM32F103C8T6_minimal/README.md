# STM32F103C8T6 RTT 测试工程

精简的 STM32F103C8T6 工程，用于测试 J-Link RTT Viewer MCP Server。

## 功能

- RTT 日志输出 (通道 0)
- 模拟传感器数据输出 (通道 1)
- 命令输入处理 (通道 2)

## 编译

使用 Keil MDK 编译：

```bash
cd MDK-ARM
cmd //c 'C:\DevTools\Keil_v5\UV4\UV4.exe -b STM32F103C8T6.uvprojx -j0 -o build.log'
```

## 烧录

```powershell
$proj = (Resolve-Path "MDK-ARM\STM32F103C8T6.uvprojx").Path
$log  = Join-Path (Get-Location) "MDK-ARM\flash.log"
$p = Start-Process -FilePath "C:\DevTools\Keil_v5\UV4\UV4.exe" `
        -ArgumentList @("-f", $proj, "-j0", "-o", $log) `
        -Wait -PassThru -NoNewWindow
"ExitCode=$($p.ExitCode)"
```

## RTT 命令

在通道 2 输入以下命令：

- `ping` - 响应 pong
- `version` - 返回版本号
- `reset` - 系统复位