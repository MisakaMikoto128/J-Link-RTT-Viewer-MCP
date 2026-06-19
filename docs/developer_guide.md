# 开发者指南

## 项目结构

```
J-Link-RTT-Viewer-MCP-mimo/
├── src/                    # 源代码
│   ├── __init__.py
│   ├── server.py          # MCP 服务器主入口
│   ├── core/              # 核心模块
│   │   ├── __init__.py
│   │   ├── jlink_manager.py  # J-Link 管理器
│   │   └── rtt_handler.py    # RTT 处理器
│   ├── tools/             # MCP 工具
│   │   ├── __init__.py
│   │   ├── connection.py  # 连接工具
│   │   ├── rtt.py         # RTT 工具
│   │   ├── reset.py       # 重置工具
│   │   ├── flash.py       # 烧录工具
│   │   └── memory.py      # 内存工具
│   ├── resources/         # MCP 资源
│   │   ├── __init__.py
│   │   └── device.py      # 设备资源
│   └── prompts/           # MCP 提示
│       ├── __init__.py
│       └── templates.py   # 提示模板
├── tests/                 # 测试代码
├── docs/                  # 文档
├── examples/              # 示例代码
├── requirements.txt       # 依赖列表
├── pyproject.toml         # 项目配置
└── README.md              # 项目说明
```

## 核心组件

### JLinkManager

`JLinkManager` 是核心组件，封装了所有 J-Link 硬件交互。

**主要功能：**
- 连接/断开 J-Link
- RTT 数据读写
- 设备重置
- 固件烧录
- 内存读写

**设计原则：**
1. 所有阻塞操作在线程池中执行
2. 使用 asyncio 进行异步编程
3. 完善的错误处理
4. 线程安全设计

### MCP 服务器

MCP 服务器使用 `FastMCP` 框架，提供：
- 工具 (Tools): 可调用的函数
- 资源 (Resources): 可读取的数据
- 提示 (Prompts): 预定义的提示模板

## 开发流程

### 1. 环境设置

```bash
# 克隆项目
git clone <repository-url>
cd J-Link-RTT-Viewer-MCP-mimo

# 创建虚拟环境
python -m venv .venv
.venv\Scripts\activate

# 安装依赖
pip install -r requirements.txt
pip install -e .
```

### 2. 代码规范

**代码风格：**
- 使用 `ruff` 进行格式化和检查
- 遵循 PEP 8 规范
- 使用类型提示

**异步编程：**
```python
# 正确：使用 async/await
async def read_rtt(self) -> dict:
    result = await self._run_in_executor(self._do_read)
    return result

# 错误：直接调用阻塞操作
def read_rtt(self) -> dict:
    return self.jlink.rtt_read(channel, size)  # 阻塞！
```

**错误处理：**
```python
# 标准错误处理模式
async def some_operation(self) -> dict:
    if self.state != ConnectionState.CONNECTED:
        return {"success": False, "message": "Not connected"}
    
    try:
        result = await self._run_in_executor(self._do_operation)
        return {"success": True, "data": result}
    except SpecificError as e:
        return {"success": False, "message": f"Error: {e}"}
    except Exception as e:
        logger.exception("Unexpected error")
        return {"success": False, "message": f"Unexpected error: {e}"}
```

### 3. 添加新工具

**步骤 1: 定义工具函数**

```python
@mcp.tool()
async def my_new_tool(param1: str, param2: int = 10) -> str:
    """工具描述
    
    Args:
        param1: 参数1描述
        param2: 参数2描述，默认值10
    
    Returns:
        结果的 JSON 字符串
    """
    manager = get_manager()
    try:
        result = await manager.my_operation(param1, param2)
        return json.dumps(result, ensure_ascii=False, indent=2)
    except Exception as e:
        return json.dumps({"success": False, "message": str(e)})
```

**步骤 2: 在 JLinkManager 中实现**

```python
async def my_operation(self, param1: str, param2: int) -> dict:
    """执行操作"""
    if self.state != ConnectionState.CONNECTED:
        return {"success": False, "message": "Not connected"}
    
    try:
        # 实现逻辑
        result = await self._run_in_executor(self._do_operation, param1, param2)
        return {"success": True, "data": result}
    except Exception as e:
        return {"success": False, "message": str(e)}
```

**步骤 3: 编写测试**

```python
@pytest.mark.asyncio
async def test_my_new_tool():
    manager = JLinkManager()
    # 模拟连接状态
    manager.state = ConnectionState.CONNECTED
    
    result = await manager.my_operation("test", 10)
    assert result["success"] is True
```

### 4. 添加新资源

```python
@mcp.resource("my://resource")
async def get_my_resource() -> str:
    """获取资源数据"""
    manager = get_manager()
    return json.dumps({
        "data": "resource content"
    }, ensure_ascii=False, indent=2)
```

### 5. 添加新提示

```python
@mcp.prompt()
async def my_prompt(param: str) -> str:
    """提示描述
    
    Args:
        param: 参数描述
    """
    return f"""提示模板
参数: {param}
"""
```

## 测试

### 运行测试

```bash
# 运行所有测试
pytest tests/ -v

# 运行特定测试
pytest tests/test_connection.py -v

# 运行带覆盖率的测试
pytest tests/ --cov=src --cov-report=html
```

### 编写测试

```python
import pytest
from unittest.mock import Mock, patch
from src.core.jlink_manager import JLinkManager, ConnectionState

class TestMyFeature:
    @pytest.mark.asyncio
    async def test_feature(self):
        with patch('pylink.JLink') as mock_jlink:
            mock_instance = Mock()
            mock_jlink.return_value = mock_instance
            
            manager = JLinkManager()
            result = await manager.my_operation()
            
            assert result["success"] is True
```

## 调试

### 查看日志

```bash
# 查看 MCP 服务器日志
tail -f jlink_mcp.log

# 查看 Claude Desktop 日志 (Windows)
type "%APPDATA%\Claude\logs\mcp.log"
```

### 使用 MCP Inspector

```bash
npx -y @modelcontextprotocol/inspector
```

在 Inspector 中可以：
- 测试工具调用
- 查看资源数据
- 测试提示模板

### 手动测试

```python
import asyncio
from src.core.jlink_manager import JLinkManager

async def test():
    manager = JLinkManager()
    result = await manager.connect(target="STM32F103C8T6")
    print(result)
    await manager.disconnect()

asyncio.run(test())
```

## 常见问题

### Q: 如何添加新的 MCU 支持？

A: MCU 支持主要由 pylink-square 库提供。如果需要特殊处理，在 `JLinkManager` 中添加对应的逻辑。

### Q: 如何处理新的固件格式？

A: 在 `flash_firmware` 方法中添加对应的解析逻辑。

### Q: 如何扩展 RTT 功能？

A: 在 `RTTHandler` 中添加新的处理逻辑，然后在工具中暴露。

## 贡献

1. 创建功能分支
2. 编写测试
3. 确保所有测试通过
4. 更新文档
5. 提交 Pull Request

详见 [CONTRIBUTING.md](../CONTRIBUTING.md)。