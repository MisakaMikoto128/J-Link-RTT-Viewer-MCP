# J-Link RTT Viewer MCP Server

将 J-Link RTT Viewer 的核心功能封装为 MCP (Model Context Protocol) 服务器，使 AI 助手能够通过标准化接口与 STM32 开发板进行交互。

## 功能概述

### 核心工具 (Tools)
1. **connect** - 连接 J-Link 调试器并开始 RTT 会话
2. **disconnect** - 断开 J-Link 连接
3. **read_rtt** - 读取 RTT 日志数据（所有通道）
4. **write_rtt** - 向 RTT 通道写入数据
5. **reset** - 重置目标 MCU（支持多种模式）
6. **get_device_info** - 获取设备信息
7. **flash_firmware** - 烧录固件到 MCU
8. **read_memory** - 读取 MCU 内存
9. **write_memory** - 写入 MCU 内存

### 资源 (Resources)
- 设备状态信息
- RTT 日志历史
- 固件文件元数据

### 提示 (Prompts)
- 自动化调试流程
- 错误诊断助手
- 固件烧录向导

## 设计原则

### 1. AI 友好的日志格式
- 结构化输出（JSON 格式）
- 包含时间戳、通道号、数据类型
- 支持过滤和搜索
- 默认读取所有通道，不区分通道

### 2. 完善的错误处理
- 连接失败处理（J-Link 未连接、MCU 未响应）
- 重置操作的安全检查
- 烧录前的确认机制
- 超时和重试逻辑

### 3. 状态管理
- 清晰的连接状态跟踪
- 操作结果的明确反馈
- 支持操作中断和恢复

## 开发计划

### 阶段 1：基础框架搭建 (1-2天)
- [ ] 项目初始化和依赖配置
- [ ] MCP 服务器基础结构
- [ ] 基础连接/断开工具实现
- [ ] 错误处理框架

### 阶段 2：核心功能实现 (2-3天)
- [ ] RTT 读取/写入工具
- [ ] 设备信息获取
- [ ] 内存读写功能
- [ ] 状态管理和资源暴露

### 阶段 3：高级功能 (2-3天)
- [ ] 固件烧录功能
- [ ] 多种重置模式
- [ ] 日志记录和回放
- [ ] 提示模板实现

### 阶段 4：测试和优化 (1-2天)
- [ ] 单元测试和集成测试
- [ ] 边界情况处理
- [ ] 性能优化
- [ ] 文档完善

## 安装和使用

### 安装依赖
```bash
# 使用 uv（推荐）
uv init jlink-rtt-mcp
cd jlink-rtt-mcp
uv venv
.venv\Scripts\activate  # Windows
uv add "mcp[cli]" pylink-square==1.6.0

# 或使用 pip
pip install "mcp[cli]" pylink-square==1.6.0
```

### 配置 MCP 客户端
在 Claude Desktop 的配置文件中添加：
```json
{
  "mcpServers": {
    "jlink-rtt": {
      "command": "uv",
      "args": [
        "--directory",
        "C:\\ABSOLUTE\\PATH\\TO\\J-Link-RTT-Viewer-MCP-mimo",
        "run",
        "server.py"
      ]
    }
  }
}
```

### 使用示例
```
# 连接设备
使用 jlink-rtt 工具连接到 STM32F103C8T6，接口选择 SWD，速度 4000kHz

# 读取日志
读取所有通道的 RTT 日志，格式化为结构化数据

# 重置设备
重置 MCU 并自动重连，确保 RTT 会话继续

# 烧录固件
将 build/output.axf 烧录到连接的设备
```

## 边界情况处理

### 连接场景
1. **J-Link 未连接** → 返回明确错误信息，建议检查硬件
2. **MCU 未响应** → 尝试降低连接速度，提供调试建议
3. **连接超时** → 自动重试机制，最多3次
4. **接口选择错误** → 自动检测或提供选项

### 重置场景
1. **重置后无日志** → 自动重连并重新开始 RTT 会话
2. **重置失败** → 提供诊断信息，建议硬件检查
3. **需要暂停** → 支持 halt 模式，停在复位状态

### 烧录场景
1. **固件文件不存在** → 检查路径，提供文件选择
2. **烧录失败** → 详细错误信息，常见原因列表
3. **烧录中断** → 恢复机制，建议拔插 J-Link

### 内存操作
1. **地址越界** → 地址范围验证
2. **读写保护** → 检测 RDP 状态，提供解保护建议
3. **数据校验** → 可选的 CRC/校验和验证

## 项目结构

```
J-Link-RTT-Viewer-MCP-mimo/
├── src/
│   ├── __init__.py
│   ├── server.py          # MCP 服务器主入口
│   ├── tools/
│   │   ├── __init__.py
│   │   ├── connection.py  # 连接/断开工具
│   │   ├── rtt.py         # RTT 读写工具
│   │   ├── reset.py       # 重置工具
│   │   ├── flash.py       # 烧录工具
│   │   └── memory.py      # 内存操作工具
│   ├── resources/
│   │   ├── __init__.py
│   │   └── device.py      # 设备状态资源
│   ├── prompts/
│   │   ├── __init__.py
│   │   └── templates.py   # 提示模板
│   └── core/
│       ├── __init__.py
│       ├── jlink_manager.py  # J-Link 管理器
│       └── rtt_handler.py    # RTT 处理器
├── tests/
│   ├── __init__.py
│   ├── test_connection.py
│   ├── test_rtt.py
│   └── test_flash.py
├── examples/
│   └── usage_examples.py
├── docs/
│   ├── api.md
│   └── troubleshooting.md
├── requirements.txt
├── pyproject.toml
└── README.md
```

## 开发注意事项

### pylink 版本要求
必须使用 `pylink-square==1.6.0`，2.x 版本存在 API 不兼容问题。

### 线程安全
- J-Link 操作必须在独立线程中执行
- 使用 Qt 信号槽机制进行跨线程通信
- 避免在 native thread 中直接 emit Qt signal

### 资源清理
- 连接断开时必须清理所有资源
- 使用 try/except 包裹所有清理操作
- 确保退出时停止所有线程

## 许可证

MIT License