# ADIT_Code_HAL_V1.3_ModbusSlave 项目说明

STM32L431RCTx + Keil MDK + STM32CubeMX HAL, Modbus RTU Slave (9600 8N1).

## 工程概览

| 项 | 值 |
|---|---|
| MCU | STM32L431RCTx (Cortex-M4F) |
| 工具链 | Keil MDK (ARMCC/ARMCLANG) |
| Keil 工程 | `MDK-ARM/ADIT_Code_HAL.uvprojx` |
| Target | `ADIT_Code_HAL` (单 target) |
| Output 目录 | `build/` (`.axf` / `.hex` / `.map`) |
| Output 名 | `ADIT_Code_HAL_V13_PCBV16` |
| 业务代码 | `User/` (BFL_*, HDL_*, HAL_*, modbus_slave, GENFUN_*) |
| HAL 库 | `Drivers/STM32L4xx_HAL_Driver/`, `Drivers/CMSIS/` |
| CubeMX 配置 | `ADIT_Code_HAL.ioc` (改外设/管脚回 CubeMX 重生成 `Core/`) |

### 目录约定

- `Core/` — CubeMX 自动生成 (main.c, gpio.c, spi.c, usart.c, ...). 改这里要从 `.ioc` 走, 直接手改下次重生成会被覆盖.
- `User/` — 业务代码, 手工维护. 加新 `.c` 必须同步加到 `.uvprojx` 的对应 group, 否则 link 时缺符号.
- `Drivers/` — ST HAL + CMSIS, 通常不动.
- `MDK-ARM/` — Keil 工程文件 + IDE 个人态.
- `build/` — Keil 输出 (已 ignore).
- `release/` — 对外发布归档 (`.7z` 已 ignore, 不进 git).

## AI 自助 build (重要)

**改完代码后, AI 自己跑 UV4 验证**, 不要把构建当甩手活留给用户。

本机 (Keil v5.42 / UV4 5.42.0.0) 实测: 下面这份 PowerShell `Start-Process` 命令**会返回 ExitCode=41 且不生成 build.log** (UV4 自身报错档, 根本没进编译, 跟代码无关), 不可用. 改用 **cmd 重定向**方式, 工作目录在 `MDK-ARM/` 下, 工程名用相对路径:

```bash
# AI 跑在 bash (Git Bash) 里, 用 cmd //c 调 UV4
cd MDK-ARM && cmd //c 'C:\DevTools\Keil_v5\UV4\UV4.exe -b ADIT_Code_HAL.uvprojx -j0 -o build.log > build_stdout.txt 2>&1 & echo UV4EXIT=%errorlevel%'
cat build.log | tail -30   # 看编译结果; 末行 "... - N Error(s), M Warning(s)"
```

判断 UV4 是否真的编译了: 看 `build/ADIT_Code_HAL_V13_PCBV16.build_log.htm` 的 mtime 有没有更新 (没更新=没编译, 多半是参数错或工程被 Keil GUI 锁着). `-j0` 必须保留, 去掉会弹 GUI 对话框把批处理卡死. 跑 build 前先确认没有 UV4 进程残留 (`Get-Process UV4`), 有就杀掉, 否则工程被锁同样不编译.

UV4 退出码语义 (Keil 官方):

| ExitCode | 含义 | AI 行为 |
|---|---|---|
| 0 | 0 Error / 0 Warning | ✅ 编译通过, 可以提交 |
| 1 | 0 Error, 但有 Warning | ⚠️ 看 build.log 评估是否新增的 |
| 2 | 有 Error, 链接未生成 axf | ❌ 不要 commit, 自己排查根因 |
| ≥3 | UV4 自身报错 (参数 / 工程文件损坏) | 退回检查命令行参数 |

注意事项:
- 工程文件路径必须**绝对路径** (Start-Process 默认工作目录与 PowerShell 不同, 相对路径会失败)
- `-j0` 关掉 IDE 弹窗交互, 强制纯命令行
- `-o build.log` 把构建输出写到指定文件; **同一文件被覆盖**, 不会追加
- `build.log` 已加进 `.gitignore` (`MDK-ARM/**/*.log`)
- 当前 `ADIT_Code_HAL.uvprojx` 只有一个 target (`ADIT_Code_HAL`), 省略 `-T` 即默认; 若以后加 target 用 `-T <target_name>` 指定

## AI 自助 flash (可选, 需用户许可)

build 通过后, AI **可以**用 `UV4.exe -f` 直接走 J-Link 烧到板上, 等价于 Keil F8.
但烧录是**硬件动作**, 类似 `git push --force` 不可逆 (旧 firmware 被覆盖,
板上可能进入意外状态), AI **必须先问用户**才能烧.

```powershell
$proj = (Resolve-Path "MDK-ARM\ADIT_Code_HAL.uvprojx").Path
$log  = Join-Path (Get-Location) "MDK-ARM\flash.log"
$p = Start-Process -FilePath "C:\DevTools\Keil_v5\UV4\UV4.exe" `
        -ArgumentList @("-f", $proj, "-j0", "-o", $log) `
        -Wait -PassThru -NoNewWindow
"ExitCode=$($p.ExitCode)"
Get-Content $log -Tail 30
```

`-f` 假设 `.axf` 已经 build 好; 不会自己先 build. 想要"build → flash" 链式,
先跑 build 命令 (`-b`), ExitCode=0 后再跑 flash (`-f`).

ExitCode 语义同 build:

| ExitCode | 含义 | AI 行为 |
|---|---|---|
| 0 | 烧录成功 | ✅ 报告 "已烧录, 可上电测试" |
| 1 | 有 Warning (烧成功了但 Keil 有 warning) | ⚠️ 看 flash.log 决定是否需要关心 |
| 2 | 烧录失败 | ❌ 报告失败 + flash.log 末尾, 让用户检查硬件 |
| ≥3 | UV4 自身报错 | 退回检查命令行参数 / Keil 工程 Debug 配置 |

**常见失败原因** (AI 报告失败时优先提示用户检查这些):

- J-Link 调试器 USB 没插 / 没识别 (设备管理器看)
- 板子没上电 / 电源接触不良
- Keil 工程 Options for Target → Debug → 没选 J-Link / 选错 device
- ST-Link 等其它调试器接了, 但工程配置的是 J-Link
- SWD/JTAG 引脚接触不良 / 排线松了
- 目标 MCU 进入读保护 (RDP, level 1+), 需要先解保护
- 目标进入意外 hardfault / WDG 复位循环, 调试器接不上, 试 nRST 拉低重连
- 之前的烧录中断, flash 算法未释放, 拔插 J-Link 一次
- 板子电流不足 (USB 供电时 J-Link 抢电导致 brownout)

**AI 烧录工作流** (用户允许后):

1. 先跑一次 build 确认 ExitCode=0 (如果之前已经 build, 可跳过)
2. **再次问用户**: "ExitCode=0, 烧录到板子吗? (J-Link 已接, 板上电了吗?)"
3. 用户确认后跑 `-f` 命令
4. ExitCode=0 → 报告"烧录成功"
5. ExitCode!=0 → 报告失败 + flash.log 末尾节选 + 上面失败原因清单的前几条, **不要自动重试**, 让用户排查硬件
6. 烧成功后**不要**自动假设功能正常, 用户接 RTT Viewer / 串口看实际行为

## 出错的事 AI 自己排, 别甩给用户

build 失败时, AI 应:
1. 读 `build.log` 末尾 30 行, 定位 `error:` 关键字所在文件/行号
2. 把错误归类: 缺 include / 类型不匹配 / 链接缺符号 / 宏值非法 token / ...
3. 直接定位到自己刚写的代码, 修一遍再 UV4 一次
4. 重复直到 ExitCode=0 才向用户报告"完成"
5. 反复 (≥3 次) 还是修不掉, 才把 build.log 节选给用户求救

绝不能写完代码不验证就交付。

**Keil + ARMCC/ARMCLANG 已知坑** (写新代码前扫一眼):

- 宏定义里的 `/* ... */` 注释不能含 `*/` 子序列 (如 `(+/-/*/=)` 会提前闭合), 改用空格分隔或 `//`
- `.uvprojx` 加新 `.c` 也要同步加到对应 target group, 否则 link 时缺符号
- `#include` 路径在 `.uvprojx` 的 Include Path 里, 加新目录 / 新文件夹时要同步, 否则 `fatal error: 'xxx.h' file not found`
- 改 CubeMX 生成的 `Core/*.c` 时, 修改写在 `/* USER CODE BEGIN */ ... /* USER CODE END */` 之间, 否则下次 CubeMX 重生成会被吃掉
- **手改 CubeMX 生成区域 (`Core/Src/*.c` 的初始化结构体赋值) 不会被 USER CODE 块保护**, 下次 CubeMX 重生成会还原. 本次 `spi.c` 的 `DMA_NORMAL`、`tim.c` 的 `Period=999` 就是手改, 已同步写进 `.ioc`, 但任何在 CubeMX 里动 SPI/TIM 的操作重生成后要复查这两行
- `Core/Src/usart.c` `rtc.c` (CubeMX 生成) 与 `User/HDL_UART.c` `HDL_RTC.c` 重复定义 `huart1`/`huart2`/`HAL_RTC_MspInit`, link 报 `L6200E Symbol ... multiply defined`. 已在 `.uvprojx` 里把 CubeMX 那两个 `<IncludeInBuild>` 设 `0` 排除 (Keil 里文件灰显). HDL 版本自配 GPIO/时钟/NVIC, 不依赖 CubeMX 版本 — **别把它们重新加回 build**

## 后续接手 AI 看这里
