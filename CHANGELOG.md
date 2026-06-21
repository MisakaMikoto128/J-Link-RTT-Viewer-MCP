# Changelog

[English](#english) | [中文](#中文)

---

## English

### [0.1.0] - 2024-01-20

#### Added
- MCP server with 12 tools for J-Link RTT Viewer
- RTT read/write with multi-channel support
- Device connection management with retry logic
- Firmware flashing for .hex and .bin files
- Memory read/write operations
- Log recording to file
- 125 comprehensive tests
- Full type hints and documentation

#### Features
- **connect**: Connect to J-Link and target MCU (SWD/JTAG)
- **disconnect**: Disconnect and cleanup resources
- **read_rtt**: Read RTT log data from up to 16 channels
- **write_rtt**: Write data to RTT down-buffer channels
- **set_rtt_channel**: Set default RTT channel (0-15)
- **reset**: Reset MCU (normal/auto_reconnect/halt modes)
- **get_device_info**: Get connected device information
- **flash_firmware**: Flash .hex/.bin firmware files
- **read_memory**: Read MCU memory (up to 1MB)
- **write_memory**: Write MCU memory
- **start_log_recording**: Record RTT output to file
- **stop_log_recording**: Stop recording

#### Technical
- Thread-safe J-Link operations via serialization lock
- Per-channel UTF-8 decoders to prevent cross-channel data corruption
- Intel HEX format parser with checksum validation
- Dynamic RTT control block size calculation
- Overlapping memory scan for RTT CB detection (256KB range)

---

## 中文

### [0.1.0] - 2024-01-20

#### 新增
- MCP 服务器，包含 12 个 J-Link RTT Viewer 工具
- 支持多通道的 RTT 读写功能
- 带重试逻辑的设备连接管理
- 支持 .hex 和 .bin 固件烧录
- 内存读写操作
- 日志录制到文件
- 125 个全面的测试用例
- 完整的类型提示和文档

#### 功能
- **connect**: 连接 J-Link 和目标 MCU (SWD/JTAG)
- **disconnect**: 断开连接并清理资源
- **read_rtt**: 从最多 16 个通道读取 RTT 日志数据
- **write_rtt**: 向 RTT down-buffer 通道写入数据
- **set_rtt_channel**: 设置默认 RTT 通道 (0-15)
- **reset**: 重置 MCU (normal/auto_reconnect/halt 模式)
- **get_device_info**: 获取已连接设备信息
- **flash_firmware**: 烧录 .hex/.bin 固件文件
- **read_memory**: 读取 MCU 内存 (最大 1MB)
- **write_memory**: 写入 MCU 内存
- **start_log_recording**: 将 RTT 输出录制到文件
- **stop_log_recording**: 停止录制

#### 技术
- 通过序列化锁实现线程安全的 J-Link 操作
- 每通道独立 UTF-8 解码器防止跨通道数据损坏
- Intel HEX 格式解析器，支持校验和验证
- 动态 RTT 控制块大小计算
- 重叠内存扫描检测 RTT CB (256KB 范围)
