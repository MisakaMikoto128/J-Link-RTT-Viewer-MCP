# 测试指南

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

## 运行测试

### 安装测试依赖

```bash
pip install -r requirements.txt
```

### 运行所有测试

```bash
pytest tests/ -v
```

### 运行特定测试

```bash
# 运行连接测试
pytest tests/test_connection.py -v

# 运行 RTT 测试
pytest tests/test_rtt.py -v

# 运行烧录测试
pytest tests/test_flash.py -v
```

### 运行带覆盖率的测试

```bash
pytest tests/ --cov=src --cov-report=html
```

## 编写测试

### 测试文件结构

```python
import pytest
from unittest.mock import Mock, patch, AsyncMock
from src.core.jlink_manager import JLinkManager, ConnectionState


class TestJLinkManager:
    """JLinkManager 测试类"""
    
    def setup_method(self):
        """每个测试方法前重置管理器"""
        self.manager = JLinkManager()
    
    @pytest.mark.asyncio
    async def test_initial_state(self):
        """测试初始状态"""
        assert self.manager.state == ConnectionState.IDLE
        assert self.manager.device_info.connected is False
```

### 模拟 J-Link 设备

```python
@pytest.mark.asyncio
async def test_connect(self):
    with patch('pylink.JLink') as mock_jlink:
        mock_instance = Mock()
        mock_instance.opened.return_value = False
        mock_instance.connected.return_value = True
        mock_jlink.return_value = mock_instance
        
        manager = JLinkManager()
        result = await manager.connect(target="STM32F103C8T6")
        
        assert result["success"] is True
        assert manager.state == ConnectionState.CONNECTED
```

### 测试错误处理

```python
@pytest.mark.asyncio
async def test_connect_failure(self):
    with patch('pylink.JLink') as mock_jlink:
        mock_instance = Mock()
        mock_instance.open.side_effect = Exception("J-Link not found")
        mock_jlink.return_value = mock_instance
        
        manager = JLinkManager()
        result = await manager.connect(target="STM32F103C8T6")
        
        assert result["success"] is False
        assert "J-Link not found" in result["message"]
```

### 测试异步操作

```python
@pytest.mark.asyncio
async def test_read_rtt(self):
    with patch('pylink.JLink') as mock_jlink:
        mock_instance = Mock()
        mock_instance.rtt_read.return_value = [72, 101, 108, 108, 111]  # "Hello"
        mock_jlink.return_value = mock_instance
        
        manager = JLinkManager()
        manager.state = ConnectionState.CONNECTED
        manager.jlink = mock_instance
        
        result = await manager.read_rtt()
        
        assert result["success"] is True
        assert len(result["data"]) > 0
```

## 测试覆盖

### 必须覆盖的场景

1. **连接场景**
   - 正常连接
   - 连接失败
   - 重试机制
   - 已连接状态重复连接

2. **RTT 场景**
   - 正常读取
   - 多通道读取
   - 无数据读取
   - 写入数据

3. **重置场景**
   - 标准重置
   - 自动重连重置
   - 重置并暂停
   - 重置失败

4. **烧录场景**
   - 正常烧录
   - 文件不存在
   - 烧录失败
   - 验证失败

5. **内存场景**
   - 正常读写
   - 地址越界
   - 大数据读写

### 边界情况

1. **并发操作**
   - 同时读取和写入
   - 多次快速连接/断开

2. **异常情况**
   - 网络中断
   - 设备断开
   - 超时

3. **资源清理**
   - 正常退出
   - 异常退出
   - 资源泄漏

## 使用 MCP Inspector 测试

### 安装 Inspector

```bash
npx -y @modelcontextprotocol/inspector
```

### 启动 Inspector

```bash
# 启动 MCP 服务器
python -m src.server

# 在另一个终端启动 Inspector
npx -y @modelcontextprotocol/inspector
```

### 测试工具

在 Inspector 中可以：
- 测试工具调用
- 查看返回结果
- 测试错误情况

## 手动测试

### 测试脚本

```python
import asyncio
from src.core.jlink_manager import JLinkManager

async def test():
    manager = JLinkManager()
    
    # 测试连接
    result = await manager.connect(target="STM32F103C8T6")
    print(f"连接: {result}")
    
    # 测试读取
    result = await manager.read_rtt()
    print(f"读取: {result}")
    
    # 测试断开
    result = await manager.disconnect()
    print(f"断开: {result}")

asyncio.run(test())
```

## 持续集成

### GitHub Actions

```yaml
name: Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        python-version: [3.10, 3.11, 3.12]
    
    steps:
    - uses: actions/checkout@v3
    - name: Set up Python
      uses: actions/setup-python@v4
      with:
        python-version: ${{ matrix.python-version }}
    
    - name: Install dependencies
      run: |
        python -m pip install --upgrade pip
        pip install -r requirements.txt
    
    - name: Run tests
      run: |
        pytest tests/ -v
```

## 测试最佳实践

1. **独立性**: 每个测试应该独立运行
2. **可重复**: 测试应该可重复运行
3. **快速**: 测试应该快速执行
4. **清晰**: 测试名称应该清晰描述测试内容
5. **覆盖**: 测试应该覆盖所有重要路径

## 常见问题

### Q: 测试失败怎么办？

A: 检查以下几点：
1. 依赖是否正确安装
2. mock 是否正确设置
3. 异步操作是否正确处理

### Q: 如何模拟硬件设备？

A: 使用 `unittest.mock` 模拟 `pylink.JLink` 类。

### Q: 如何测试异步代码？

A: 使用 `pytest.mark.asyncio` 装饰器。

### Q: 如何查看测试覆盖率？

A: 使用 `pytest --cov=src --cov-report=html`。