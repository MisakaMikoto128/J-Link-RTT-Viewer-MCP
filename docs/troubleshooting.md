# 故障排除指南

## 连接问题

### J-Link 未识别

**症状:**
- 连接时返回 "J-Link not found" 错误
- 设备管理器中看不到 J-Link

**解决方案:**

1. **检查 USB 连接**
   - 尝试不同的 USB 端口
   - 使用原装 USB 线缆
   - 避免使用 USB 集线器

2. **安装/更新驱动**
   - 下载最新 J-Link 驱动: https://www.segger.com/downloads/jlink/
   - 安装后重启电脑

3. **检查设备管理器**
   - Windows: 设备管理器 → 通用串行总线设备
   - 应该看到 "J-Link" 设备

### MCU 无响应

**症状:**
- 连接超时
- 返回 "connect(target) failed" 错误

**解决方案:**

1. **检查电源**
   - 确认 MCU 已上电
   - 检查电源指示灯

2. **降低连接速度**
   ```python
   connect(target="STM32F103C8T6", speed=1000)  # 从 4000 降到 1000
   ```

3. **检查接线**
   - SWD: SWDIO, SWCLK, GND
   - JTAG: TDI, TDO, TCK, TMS, nTRST, GND

4. **检查 MCU 状态**
   - MCU 可能进入低功耗模式
   - 尝试复位 MCU

### 连接后断开

**症状:**
- 连接成功后立即断开
- 返回 "connection lost" 错误

**解决方案:**

1. **检查电源稳定性**
   - USB 供电可能不足
   - 使用外部电源

2. **检查看门狗**
   - MCU 可能触发了看门狗复位
   - 检查固件代码

## RTT 问题

### RTT 没有数据

**症状:**
- 连接成功但 `read_rtt()` 返回空数据

**解决方案:**

1. **检查 RTT 初始化**
   - MCU 代码必须调用 `SEGGER_RTT_Init()`
   - 检查是否包含 `SEGGER_RTT.h`

2. **检查通道号**
   - 默认通道是 0
   - 确认 MCU 代码使用的通道
   ```python
   set_rtt_channel(0)  # 设置为通道 0
   ```

3. **尝试重置**
   ```python
   reset(mode="auto_reconnect")
   ```

4. **检查缓冲区大小**
   - MCU 端的 RTT 缓冲区可能太小
   - 增大 `SEGGER_RTT_CONF_NUM_BUFFERS`

### RTT 数据乱码

**症状:**
- 读取到的数据是乱码或不可读

**解决方案:**

1. **检查编码**
   - 默认使用 UTF-8
   - 如果 MCU 使用 GBK，需要修改编码

2. **检查波特率**
   - RTT 不需要设置波特率
   - 如果使用 UART 转 RTT，检查转换器设置

3. **检查数据格式**
   - MCU 代码是否正确格式化输出
   - 检查 `SEGGER_RTT_printf` 的格式字符串

### RTT 数据不完整

**症状:**
- 读取的数据缺少部分字符

**解决方案:**

1. **增加读取大小**
   ```python
   read_rtt(max_size=8192)  # 增加到 8KB
   ```

2. **增加轮询频率**
   - 当前轮询间隔是 100ms
   - 高速数据可能丢失

3. **检查缓冲区溢出**
   - MCU 端的 RTT 缓冲区可能溢出
   - 增大缓冲区或减少输出频率

## 烧录问题

### 烧录失败

**症状:**
- 返回 "flash failed" 错误

**解决方案:**

1. **检查固件文件**
   - 文件是否存在
   - 文件格式是否正确

2. **检查读写保护**
   ```python
   # 尝试读取芯片 ID
   read_memory(0x1FFFF7E0, 4)  # STM32F1 的芯片 ID 地址
   ```

3. **以管理员身份运行**
   - 某些操作需要管理员权限

4. **拔插 J-Link**
   - 烧录中断后可能需要重新初始化

### 烧录后 MCU 无响应

**症状:**
- 烧录成功但 MCU 不运行

**解决方案:**

1. **检查固件**
   - 固件是否正确编译
   - 链接地址是否正确

2. **检查时钟配置**
   - MCU 可能时钟配置错误
   - 检查 HSE/HSI 配置

3. **检查中断向量表**
   - 中断向量表地址是否正确
   - STM32F1: 0x08000000

4. **尝试复位**
   ```python
   reset(mode="normal")
   ```

### 烧录超时

**症状:**
- 烧录过程卡住或超时

**解决方案:**

1. **降低烧录速度**
   - 在 J-Link 配置中降低速度

2. **检查连接**
   - SWD/JTAG 连接是否稳定
   - 线缆是否过长

3. **检查目标电压**
   - MCU 电压是否正常
   - J-Link 是否支持目标电压

## 内存操作问题

### 地址越界

**症状:**
- 返回 "address out of range" 错误

**解决方案:**

1. **检查地址范围**
   - Flash: 0x08000000 - 0x080FFFFF (STM32F103C8)
   - RAM: 0x20000000 - 0x20004FFF (STM32F103C8)

2. **检查 MCU 型号**
   - 不同型号的地址范围不同
   - 查阅 MCU 数据手册

### 读写保护

**症状:**
- 返回 "read protection" 错误

**解决方案:**

1. **检查 RDP 状态**
   ```python
   read_memory(0x1FFFF800, 1)  # 读取 RDP 字节
   ```

2. **解除保护**
   - 需要使用 J-Link Commander
   - 命令: `si SWD speed 4000 device STM32F103C8 lock 0`

## MCP 相关问题

### Claude Desktop 看不到工具

**症状:**
- Claude Desktop 没有显示 MCP 工具

**解决方案:**

1. **检查配置文件**
   - 路径是否正确
   - JSON 格式是否正确

2. **检查 Python 路径**
   - 使用完整路径
   - 确认虚拟环境激活

3. **重启 Claude Desktop**
   - 完全退出（不是仅关闭窗口）
   - 重新启动

### 工具调用失败

**症状:**
- Claude 尝试调用工具但失败

**解决方案:**

1. **查看日志**
   - 检查 `jlink_mcp.log` 文件
   - 查看 Claude Desktop 日志

2. **测试服务器**
   ```bash
   python -m src.server
   ```

3. **检查依赖**
   - 确认所有依赖已安装
   - 版本是否正确

### 响应超时

**症状:**
- 工具调用超时

**解决方案:**

1. **检查网络**
   - MCP 使用 stdio 通信
   - 确认进程正常运行

2. **增加超时时间**
   - 某些操作（如烧录）需要较长时间

3. **检查进程**
   - 确认 MCP 服务器进程未崩溃

## 获取帮助

### 查看日志

```bash
# 查看 MCP 服务器日志
cat jlink_mcp.log

# 查看 Claude Desktop 日志 (Windows)
type "%APPDATA%\Claude\logs\mcp.log"

# 查看 Claude Desktop 日志 (macOS)
tail -f ~/Library/Logs/Claude/mcp*.log
```

### 联系支持

- 提交 GitHub Issue
- 提供完整的错误信息
- 描述复现步骤
- 附上日志文件