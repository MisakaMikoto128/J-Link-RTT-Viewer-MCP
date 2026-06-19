# API 文档

## 工具 (Tools)

### connect

连接到 J-Link 调试器和目标 MCU。

**参数:**

| 参数 | 类型 | 必需 | 默认值 | 说明 |
|------|------|------|--------|------|
| target | string | 是 | - | 目标设备型号 (如 STM32F103C8T6) |
| interface | string | 否 | "SWD" | 调试接口 (SWD 或 JTAG) |
| speed | integer | 否 | 4000 | 连接速度 (kHz) |
| channel | integer | 否 | 0 | RTT 通道号 |
| retry_attempts | integer | 否 | 3 | 重试次数 |

**返回:**

```json
{
  "success": true,
  "message": "Connected to STM32F103C8T6",
  "device_info": {
    "jlink_firmware": "V1.0",
    "jlink_hardware": "J-Link V10",
    "jlink_serial": "123456789",
    "core_name": "STM32F103C8T6",
    "core_id": "0x1ba01477",
    "core_cpu": "Cortex-M3",
    "target_device": "STM32F103C8T6",
    "interface": "SWD",
    "speed_khz": 4000
  }
}
```

**错误情况:**

- J-Link 未连接
- MCU 无响应
- 连接超时
- 参数错误

---

### disconnect

断开 J-Link 连接，清理所有资源。

**参数:** 无

**返回:**

```json
{
  "success": true,
  "message": "Disconnected"
}
```

---

### get_device_info

获取当前连接设备的信息。

**参数:** 无

**返回:**

```json
{
  "success": true,
  "device_info": {
    "jlink_firmware": "V1.0",
    "jlink_hardware": "J-Link V10",
    "jlink_serial": "123456789",
    "core_name": "STM32F103C8T6",
    "core_id": "0x1ba01477",
    "core_cpu": "Cortex-M3",
    "target_device": "STM32F103C8T6",
    "interface": "SWD",
    "speed_khz": 4000
  }
}
```

---

### read_rtt

读取 RTT 日志数据。

**参数:**

| 参数 | 类型 | 必需 | 默认值 | 说明 |
|------|------|------|--------|------|
| channels | array[integer] | 否 | null | 要读取的通道列表，null 表示所有通道 |
| timeout | float | 否 | 1.0 | 读取超时时间（秒） |
| max_size | integer | 否 | 4096 | 最大读取字节数 |

**返回:**

```json
{
  "success": true,
  "message": "Read 5 RTT messages",
  "data": [
    {
      "timestamp": 1705312245.123,
      "channel": 0,
      "content": "[INFO] System initialized",
      "raw": "5b494e464f5d2053797374656d20696e697469616c697a6564",
      "metadata": {
        "has_ansi": false,
        "level": "INFO",
        "source": "unknown"
      }
    }
  ]
}
```

**说明:**

- 默认读取所有通道 (0-15)
- 返回结构化的 JSON 数据
- 包含时间戳、通道、内容和元数据
- 支持 ANSI 颜色代码解析

---

### write_rtt

向 RTT 通道写入数据。

**参数:**

| 参数 | 类型 | 必需 | 默认值 | 说明 |
|------|------|------|--------|------|
| data | string | 是 | - | 要写入的数据 |
| channel | integer | 否 | null | 通道号，null 使用默认通道 |
| is_hex | boolean | 否 | false | 是否为十六进制数据 |

**返回:**

```json
{
  "success": true,
  "message": "Written 17/17 bytes to channel 0",
  "bytes_written": 17
}
```

---

### set_rtt_channel

设置默认 RTT 通道。

**参数:**

| 参数 | 类型 | 必需 | 默认值 | 说明 |
|------|------|------|--------|------|
| channel | integer | 是 | - | 通道号 (0-15) |

**返回:**

```json
{
  "success": true,
  "message": "Default channel set to 5"
}
```

---

### reset

重置目标 MCU。

**参数:**

| 参数 | 类型 | 必需 | 默认值 | 说明 |
|------|------|------|--------|------|
| mode | string | 否 | "normal" | 重置模式 (normal/auto_reconnect/halt) |
| wait_time | float | 否 | 0.3 | 重置后等待时间（秒） |

**重置模式说明:**

- `normal`: 标准重置，保留连接会话
- `auto_reconnect`: 重置后自动断开重连
- `halt`: 重置后暂停 CPU（停在复位状态）

**返回:**

```json
{
  "success": true,
  "message": "Reset completed"
}
```

---

### flash_firmware

烧录固件到 MCU。

**参数:**

| 参数 | 类型 | 必需 | 默认值 | 说明 |
|------|------|------|--------|------|
| firmware_path | string | 是 | - | 固件文件路径 (.hex, .bin, .axf) |
| verify | boolean | 否 | true | 是否在烧录后验证 |

**返回:**

```json
{
  "success": true,
  "message": "Firmware flashed successfully: path/to/firmware.bin",
  "file_size": 12345
}
```

**支持的格式:**

- `.hex`: Intel HEX 格式
- `.bin`: 二进制格式
- `.axf` / `.elf`: ARM ELF 格式

---

### read_memory

读取 MCU 内存。

**参数:**

| 参数 | 类型 | 必需 | 默认值 | 说明 |
|------|------|------|--------|------|
| address | integer | 是 | - | 起始地址 (如 0x08000000) |
| size | integer | 是 | - | 读取字节数 (1-1048576) |

**返回:**

```json
{
  "success": true,
  "message": "Read 256 bytes from 0x08000000",
  "address": 134217728,
  "size": 256,
  "data": "ff0aff..."
}
```

---

### write_memory

写入 MCU 内存。

**参数:**

| 参数 | 类型 | 必需 | 默认值 | 说明 |
|------|------|------|--------|------|
| address | integer | 是 | - | 起始地址 (如 0x08000000) |
| data | string | 是 | - | 要写入的数据 |
| is_hex | boolean | 否 | true | 是否为十六进制数据 |

**返回:**

```json
{
  "success": true,
  "message": "Written 128 bytes to 0x08000000",
  "bytes_written": 128
}
```

---

### start_log_recording

开始日志记录。

**参数:**

| 参数 | 类型 | 必需 | 默认值 | 说明 |
|------|------|------|--------|------|
| log_dir | string | 否 | "logs" | 日志目录 |

**返回:**

```json
{
  "success": true,
  "message": "Recording to logs/rtt_20240115_103045.log"
}
```

---

### stop_log_recording

停止日志记录。

**参数:** 无

**返回:**

```json
{
  "success": true,
  "message": "Log recording stopped"
}
```

## 资源 (Resources)

### device://status

获取设备状态信息。

**返回:**

```json
{
  "state": "connected",
  "device_info": {
    "target_device": "STM32F103C8T6",
    "interface": "SWD",
    "speed_khz": 4000
  }
}
```

---

### rtt://logs

获取 RTT 日志历史（最近 100 条）。

**返回:**

```json
[
  {
    "timestamp": 1705312245.123,
    "channel": 0,
    "content": "[INFO] System initialized",
    "metadata": {
      "has_ansi": false,
      "level": "INFO"
    }
  }
]
```

---

### firmware://metadata

获取固件元数据。

**返回:**

```json
{
  "target_device": "STM32F103C8T6",
  "core_name": "STM32F103C8T6",
  "core_id": "0x1ba01477"
}
```

## 提示 (Prompts)

### debugging_guide

提供 STM32 调试的最佳实践和常见问题解决方案。

**用途:** 帮助用户排查连接、RTT、烧录等问题。

---

### error_diagnosis

根据错误信息提供诊断建议。

**参数:**

| 参数 | 类型 | 必需 | 说明 |
|------|------|------|------|
| error_message | string | 是 | 错误信息 |

**用途:** 分析错误原因，提供解决方案。

---

### flash_wizard

引导用户完成固件烧录流程。

**用途:** 提供烧录步骤、注意事项和常见问题解答。