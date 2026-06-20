# STM32F103C8T6 RTT 测试工程

精简的 STM32F103C8T6 工程，用于测试 J-Link RTT Viewer MCP Server。

## 工程概览

| 项 | 值 |
|---|---|
| MCU | STM32F103C8T6 (Cortex-M3, 64KB Flash, 20KB RAM) |
| 工具链 | Keil MDK (ARMCC/ARMCLANG) |
| Keil 工程 | `MDK-ARM/STM32F103C8T6.uvprojx` |
| Target | `STM32F103C8T6` (单 target) |
| Output 目录 | `MDK-ARM/STM32F103C8T6/` |
| Output 名 | `STM32F103C8T6` |

## 功能说明

本工程实现了一个简单的 RTT 测试程序，包含：

1. **RTT 日志输出** (通道 0): 系统状态、心跳信息
2. **RTT 数据输出** (通道 1): 模拟传感器数据
3. **RTT 命令输入** (通道 2): 接收并处理简单命令

### 支持的命令

- `ping` - 响应 pong
- `version` - 返回版本号
- `reset` - 系统复位

## AI 自助 build

**改完代码后, AI 自己跑 UV4 验证**, 不要把构建当甩手活留给用户。

```bash
# AI 跑在 bash (Git Bash) 里, 用 cmd //c 调 UV4
cd MDK-ARM && cmd //c 'C:\DevTools\Keil_v5\UV4\UV4.exe -b STM32F103C8T6.uvprojx -j0 -o build.log > build_stdout.txt 2>&1 & echo UV4EXIT=%errorlevel%'
cat build.log | tail -30   # 看编译结果; 末行 "... - N Error(s), M Warning(s)"
```

UV4 退出码语义 (Keil 官方):

| ExitCode | 含义 | AI 行为 |
|---|---|---|
| 0 | 0 Error / 0 Warning | ✅ 编译通过, 可以提交 |
| 1 | 0 Error, 但有 Warning | ⚠️ 看 build.log 评估是否新增的 |
| 2 | 有 Error, 链接未生成 axf | ❌ 不要 commit, 自己排查根因 |
| ≥3 | UV4 自身报错 (参数 / 工程文件损坏) | 退回检查命令行参数 |

注意事项:
- 工程文件路径必须**绝对路径**
- `-j0` 关掉 IDE 弹窗交互, 强制纯命令行
- `-o build.log` 把构建输出写到指定文件; **同一文件被覆盖**, 不会追加
- 跑 build 前先确认没有 UV4 进程残留 (`Get-Process UV4`), 有就杀掉

## AI 自助 flash

build 通过后, AI **可以**用 `UV4.exe -f` 直接走 J-Link 烧到板上, 等价于 Keil F8.
但烧录是**硬件动作**, AI **必须先问用户**才能烧.

```powershell
$proj = (Resolve-Path "MDK-ARM\STM32F103C8T6.uvprojx").Path
$log  = Join-Path (Get-Location) "MDK-ARM\flash.log"
$p = Start-Process -FilePath "C:\DevTools\Keil_v5\UV4\UV4.exe" `
        -ArgumentList @("-f", $proj, "-j0", "-o", $log) `
        -Wait -PassThru -NoNewWindow
"ExitCode=$($p.ExitCode)"
Get-Content $log -Tail 30
```

ExitCode 语义同 build:

| ExitCode | 含义 | AI 行为 |
|---|---|---|
| 0 | 烧录成功 | ✅ 报告 "已烧录, 可上电测试" |
| 1 | 有 Warning (烧成功了但 Keil 有 warning) | ⚠️ 看 flash.log 决定是否需要关心 |
| 2 | 烧录失败 | ❌ 报告失败 + flash.log 末尾, 让用户检查硬件 |
| ≥3 | UV4 自身报错 | 退回检查命令行参数 / Keil 工程 Debug 配置 |

**常见失败原因**:

- J-Link 调试器 USB 没插 / 没识别 (设备管理器看)
- 板子没上电 / 电源接触不良
- Keil 工程 Options for Target → Debug → 没选 J-Link / 选错 device
- SWD/JTAG 引脚接触不良 / 排线松了
- 目标 MCU 进入读保护 (RDP, level 1+), 需要先解保护
- 之前的烧录中断, flash 算法未释放, 拔插 J-Link 一次

**AI 烧录工作流** (用户允许后):

1. 先跑一次 build 确认 ExitCode=0 (如果之前已经 build, 可跳过)
2. **再次问用户**: "ExitCode=0, 烧录到板子吗? (J-Link 已接, 板上电了吗?)"
3. 用户确认后跑 `-f` 命令
4. ExitCode=0 → 报告"烧录成功"
5. ExitCode!=0 → 报告失败 + flash.log 末尾节选 + 上面失败原因清单的前几条, **不要自动重试**, 让用户排查硬件
6. 烧成功后**不要**自动假设功能正常, 用户接 RTT Viewer 看实际行为

## RTT 通道说明

| 通道 | 用途 | 数据格式 |
|------|------|----------|
| 0 | 日志输出 | `[LEVEL] message\r\n` |
| 1 | 传感器数据 | `TEMP=xx.x,HUMI=yy.y\r\n` |
| 2 | 命令输入 | 文本命令 |

### 测试 RTT

烧录后, 使用 SEGGER RTT Viewer 或 J-Link RTT Viewer 连接:

1. 选择 J-Link 设备
2. 选择目标 MCU: STM32F103C8
3. 连接
4. 查看通道 0 的日志输出
5. 在通道 2 输入命令 (如 `ping`)

## 目录结构

```
STM32F103C8T6_minimal/
├── main.c                 # 主程序
├── CLAUDE.md              # 本文件
└── MDK-ARM/               # Keil 工程文件
    ├── STM32F103C8T6.uvprojx
    └── STM32F103C8T6/     # 输出目录
```

## 出错的事 AI 自己排, 别甩给用户

build 失败时, AI 应:
1. 读 `build.log` 末尾 30 行, 定位 `error:` 关键字所在文件/行号
2. 把错误归类: 缺 include / 类型不匹配 / 链接缺符号 / ...
3. 直接定位到自己刚写的代码, 修一遍再 UV4 一次
4. 重复直到 ExitCode=0 才向用户报告"完成"
5. 反复 (≥3 次) 还是修不掉, 才把 build.log 节选给用户求救

**Keil + ARMCC/ARMCLANG 已知坑**:

- 宏定义里的 `/* ... */` 注释不能含 `*/` 子序列
- `.uvprojx` 加新 `.c` 也要同步加到对应 target group
- `#include` 路径在 `.uvprojx` 的 Include Path 里