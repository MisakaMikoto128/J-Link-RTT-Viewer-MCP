# J-Link RTT Viewer MCP Server 开发计划

## 项目概述

将现有的 J-Link RTT Viewer PyQt 项目的核心功能重构为 MCP (Model Context Protocol) 服务器，使 AI 助手能够通过标准化接口与 STM32 开发板进行交互。

## 开发目标

1. **创建一个 MCP 服务器**，暴露 J-Link RTT Viewer 的核心功能
2. **实现 AI 友好的接口**，支持自动化调试、日志读取、设备控制
3. **完善错误处理**，确保 AI 能处理所有边界情况
4. **提供清晰的文档**，包括 API 文档和使用示例

## 技术栈

- **语言**: Python 3.10+
- **MCP SDK**: `mcp[cli]` (Python SDK Tier 1)
- **J-Link 库**: `pylink-square==1.6.0` (必须)
- **异步框架**: `asyncio` + `anyio`
- **测试**: `pytest` + `pytest-asyncio`

## 开发阶段

### 阶段 1：项目初始化和基础框架 (Day 1)

#### 任务 1.1：项目设置
- 创建项目结构
- 配置 `pyproject.toml` 和依赖
- 设置虚拟环境
- 创建基础 MCP 服务器框架

#### 任务 1.2：J-Link 管理器
- 实现 `JLinkManager` 类
- 封装 pylink 调用，处理线程安全
- 实现连接/断开逻辑
- 添加状态跟踪和错误处理

#### 任务 1.3：基础工具实现
- `connect` 工具：连接 J-Link 和 MCU
- `disconnect` 工具：断开连接
- `get_device_info` 工具：获取设备信息

**交付物**: 可运行的 MCP 服务器，支持基本连接/断开操作

### 阶段 2：核心 RTT 功能 (Day 2-3)

#### 任务 2.1：RTT 处理器
- 实现 `RTTHandler` 类
- 封装 RTT 读写操作
- 处理多通道数据
- 实现异步读取循环

#### 任务 2.2：RTT 工具实现
- `read_rtt` 工具：读取 RTT 日志（所有通道）
- `write_rtt` 工具：向 RTT 通道写入数据
- `set_rtt_channel` 工具：设置默认通道

#### 任务 2.3：日志格式化
- 实现结构化日志输出（JSON 格式）
- 添加时间戳、通道号、数据类型信息
- 支持 ANSI 颜色代码解析
- 实现日志过滤和搜索

**交付物**: 完整的 RTT 读写功能，支持 AI 友好的日志格式

### 阶段 3：设备控制和重置 (Day 4)

#### 任务 3.1：重置功能
- 实现多种重置模式：
  - `normal`: 5 步 dance（保留会话）
  - `auto_reconnect`: 重置 + 断开 + 重连
  - `halt`: 重置并暂停（CPU 停在复位状态）

#### 任务 3.2：内存操作
- `read_memory` 工具：读取 MCU 内存
- `write_memory` 工具：写入 MCU 内存
- 地址验证和边界检查

#### 任务 3.3：电源控制
- `power_on` / `power_off` 工具
- 电源状态查询

**交付物**: 完整的设备控制功能，支持安全的重置和内存操作

### 阶段 4：固件烧录 (Day 5)

#### 任务 4.1：烧录功能
- `flash_firmware` 工具：烧录固件到 MCU
- 支持 `.hex`、`.bin`、`.axf` 文件
- 实现烧录进度反馈
- 添加烧录前确认机制

#### 任务 4.2：固件验证
- 烧录后验证
- CRC 校验
- 读取设备固件版本

#### 任务 4.3：错误恢复
- 烧录失败处理
- 常见错误诊断
- 恢复建议

**交付物**: 完整的固件烧录功能，包含错误处理和恢复机制

### 阶段 5：资源和提示 (Day 6)

#### 任务 5.1：资源实现
- `device://status` 资源：设备状态信息
- `rtt://logs` 资源：RTT 日志历史
- `firmware://metadata` 资源：固件文件元数据

#### 任务 5.2：提示模板
- 自动化调试流程提示
- 错误诊断助手提示
- 固件烧录向导提示

#### 任务 5.3：配置管理
- 实现配置文件支持
- 默认配置和用户配置
- 配置验证

**交付物**: 完整的资源和提示系统

### 阶段 6：测试和文档 (Day 7)

#### 任务 6.1：单元测试
- 测试所有工具函数
- 测试错误处理路径
- 测试边界情况

#### 任务 6.2：集成测试
- 端到端测试流程
- 模拟 J-Link 设备测试
- 真实设备测试

#### 任务 6.3：文档完善
- API 文档
- 使用示例
- 故障排除指南
- 贡献指南

**交付物**: 完整的测试套件和文档

## 边界情况处理清单

### 连接场景
1. ✅ J-Link 未连接 → 返回错误信息，建议检查硬件
2. ✅ MCU 未响应 → 尝试降低连接速度，提供调试建议
3. ✅ 连接超时 → 自动重试机制，最多3次
4. ✅ 接口选择错误 → 自动检测或提供选项
5. ✅ 已连接状态重复连接 → 先断开再重连
6. ✅ 连接参数错误 → 参数验证和默认值

### 重置场景
1. ✅ 重置后无日志 → 自动重连并重新开始 RTT 会话
2. ✅ 重置失败 → 提供诊断信息，建议硬件检查
3. ✅ 需要暂停 → 支持 halt 模式，停在复位状态
4. ✅ 重置期间数据丢失 → 保存最后的日志数据
5. ✅ 多次重置 → 限制重置频率，避免硬件损坏

### 烧录场景
1. ✅ 固件文件不存在 → 检查路径，提供文件选择
2. ✅ 固件文件损坏 → 验证文件格式和完整性
3. ✅ 烧录失败 → 详细错误信息，常见原因列表
4. ✅ 烧录中断 → 恢复机制，建议拔插 J-Link
5. ✅ 烧录权限不足 → 检查读写保护状态
6. ✅ 烧录后 MCU 无响应 → 建议硬件检查

### 内存操作
1. ✅ 地址越界 → 地址范围验证
2. ✅ 读写保护 → 检测 RDP 状态，提供解保护建议
3. ✅ 数据校验 → 可选的 CRC/校验和验证
4. ✅ 内存对齐 → 自动处理对齐问题
5. ✅ 大数据读取 → 分块读取，避免超时

### 通用错误处理
1. ✅ 操作超时 → 超时设置和重试机制
2. ✅ 资源不足 → 内存和句柄管理
3. ✅ 并发操作 → 操作锁和队列
4. ✅ 状态不一致 → 状态恢复机制
5. ✅ 异常退出 → 资源清理和状态重置

## 实现细节

### J-Link 管理器设计

```python
class JLinkManager:
    """J-Link 设备管理器，处理所有硬件交互"""
    
    def __init__(self):
        self.jlink = None
        self.state = "IDLE"  # IDLE, CONNECTING, CONNECTED, DISCONNECTING
        self.device_info = {}
        self.last_connect_params = None
        
    async def connect(self, target: str, interface: str = "SWD", 
                     speed: int = 4000, channel: int = 0) -> dict:
        """连接到 J-Link 和目标 MCU"""
        # 实现连接逻辑，包含错误处理和重试
        
    async def disconnect(self) -> dict:
        """断开连接，清理资源"""
        # 实现断开逻辑，确保资源清理
        
    async def read_rtt(self, channel: int = -1, size: int = 4096) -> dict:
        """读取 RTT 数据，-1 表示所有通道"""
        # 实现异步读取，支持多通道
        
    async def reset(self, mode: str = "normal") -> dict:
        """重置目标 MCU"""
        # 支持多种重置模式
        
    async def flash(self, firmware_path: str, verify: bool = True) -> dict:
        """烧录固件"""
        # 实现烧录逻辑，包含验证
```

### MCP 工具定义

```python
from mcp.server.fastmcp import FastMCP

mcp = FastMCP("J-Link RTT Viewer")

@mcp.tool()
async def connect(target: str, interface: str = "SWD", 
                 speed: int = 4000, channel: int = 0) -> str:
    """连接到 J-Link 调试器和目标 MCU
    
    Args:
        target: 目标设备型号 (如 STM32F103C8T6)
        interface: 调试接口 (SWD 或 JTAG)
        speed: 连接速度 (kHz)
        channel: RTT 通道号
    
    Returns:
        连接状态和设备信息
    """
    # 实现连接逻辑
    
@mcp.tool()
async def read_rtt(channels: list[int] = None, 
                  timeout: float = 1.0) -> str:
    """读取 RTT 日志数据
    
    Args:
        channels: 要读取的通道列表，None 表示所有通道
        timeout: 读取超时时间（秒）
    
    Returns:
        结构化的日志数据 (JSON 格式)
    """
    # 实现读取逻辑
```

### 日志格式设计

```json
{
  "timestamp": "2024-01-15T10:30:45.123Z",
  "channel": 0,
  "type": "text",
  "content": "[INFO] System initialized",
  "raw": "[INFO] System initialized\r\n",
  "metadata": {
    "level": "INFO",
    "source": "main",
    "has_ansi": false
  }
}
```

## 测试策略

### 单元测试
- 测试每个工具函数的独立功能
- 使用 mock 模拟 J-Link 设备
- 测试错误处理路径

### 集成测试
- 测试完整的连接-读取-断开流程
- 测试重置和重连流程
- 测试烧录流程

### 边界测试
- 测试所有边界情况
- 测试并发操作
- 测试异常退出

## 部署和配置

### 安装方式
```bash
# 开发模式
pip install -e .

# 生产模式
pip install .
```

### 配置文件
```json
{
  "jlink": {
    "default_interface": "SWD",
    "default_speed": 4000,
    "connection_timeout": 5.0,
    "retry_attempts": 3
  },
  "rtt": {
    "default_channel": 0,
    "read_interval": 0.1,
    "buffer_size": 4096
  },
  "flash": {
    "verify_after_flash": true,
    "auto_reset": true
  }
}
```

## 风险评估

### 技术风险
1. **pylink 版本兼容性** - 必须使用 1.6.0
2. **线程安全** - J-Link 操作必须在独立线程
3. **资源泄漏** - 必须确保所有资源正确清理

### 硬件风险
1. **J-Link 连接问题** - 需要完善的错误处理
2. **MCU 无响应** - 需要超时和重试机制
3. **烧录失败** - 需要验证和恢复机制

### 缓解措施
1. 严格的版本锁定
2. 完善的错误处理
3. 充分的测试覆盖
4. 详细的文档和示例

## 成功标准

1. ✅ 所有核心工具正常工作
2. ✅ 错误处理完善，AI 能处理所有边界情况
3. ✅ 测试覆盖率达到 80%+
4. ✅ 文档完整，包含使用示例
5. ✅ 能够通过 MCP Inspector 测试
6. ✅ 能够与 Claude Desktop 集成

## 时间线

- **Day 1**: 项目初始化和基础框架
- **Day 2-3**: 核心 RTT 功能
- **Day 4**: 设备控制和重置
- **Day 5**: 固件烧录
- **Day 6**: 资源和提示
- **Day 7**: 测试和文档

总计：7 天开发时间