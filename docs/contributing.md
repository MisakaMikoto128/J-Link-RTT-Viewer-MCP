# 贡献者指南

感谢你对 J-Link RTT Viewer MCP Server 项目的关注！我们欢迎任何形式的贡献。

## 如何贡献

### 报告问题

1. 检查是否已有相关 Issue
2. 创建新的 Issue，包含：
   - 清晰的问题描述
   - 复现步骤
   - 期望行为 vs 实际行为
   - 环境信息（Python 版本、操作系统等）
   - 相关日志

### 提交代码

1. Fork 项目
2. 创建功能分支: `git checkout -b feature/amazing-feature`
3. 提交更改: `git commit -m 'Add amazing feature'`
4. 推送到分支: `git push origin feature/amazing-feature`
5. 创建 Pull Request

## 开发环境设置

### 前置条件

- Python 3.10+
- Git
- J-Link 调试器（用于测试）

### 安装步骤

```bash
# 克隆仓库
git clone https://github.com/your-username/J-Link-RTT-Viewer-MCP-mimo.git
cd J-Link-RTT-Viewer-MCP-mimo

# 创建虚拟环境
python -m venv .venv
.venv\Scripts\activate  # Windows
# source .venv/bin/activate  # macOS/Linux

# 安装依赖
pip install -r requirements.txt
pip install -e .
```

## 代码规范

### 代码风格

- 使用 `ruff` 进行代码格式化和检查
- 遵循 PEP 8 规范
- 使用类型提示

```bash
# 格式化代码
ruff format src/

# 检查代码质量
ruff check src/

# 自动修复
ruff check --fix src/
```

### 类型提示

所有函数和方法必须有完整的类型提示：

```python
async def connect(
    self,
    target: str,
    interface: str = "SWD",
    speed: int = 4000,
) -> dict[str, Any]:
    """连接到 J-Link 和目标 MCU"""
    pass
```

### 文档字符串

所有公共函数必须有 docstring：

```python
async def read_rtt(
    self,
    channels: Optional[list[int]] = None,
    timeout: float = 1.0,
) -> dict[str, Any]:
    """读取 RTT 日志数据

    Args:
        channels: 要读取的通道列表，None 表示所有通道
        timeout: 读取超时时间（秒）

    Returns:
        结构化的日志数据

    Raises:
        ConnectionError: 未连接时抛出
    """
    pass
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

- 测试文件放在 `tests/` 目录
- 文件名以 `test_` 开头
- 使用 `pytest.mark.asyncio` 测试异步函数
- 使用 `unittest.mock` 模拟外部依赖

```python
import pytest
from unittest.mock import Mock, patch
from src.core.jlink_manager import JLinkManager

class TestJLinkManager:
    @pytest.mark.asyncio
    async def test_connect(self):
        with patch('pylink.JLink') as mock_jlink:
            mock_instance = Mock()
            mock_instance.connected.return_value = True
            mock_jlink.return_value = mock_instance
            
            manager = JLinkManager()
            result = await manager.connect(target="STM32F103C8T6")
            
            assert result["success"] is True
```

## 提交规范

### Commit Message 格式

```
<type>(<scope>): <subject>

<body>

<footer>
```

### Type 类型

- `feat`: 新功能
- `fix`: 修复 bug
- `docs`: 文档更新
- `style`: 代码格式（不影响功能）
- `refactor`: 重构
- `test`: 测试相关
- `chore`: 构建/工具相关

### 示例

```
feat(rtt): add multi-channel support

Add support for reading from multiple RTT channels simultaneously.

Closes #123
```

## Pull Request 流程

1. **创建 PR**
   - 标题清晰描述更改
   - 说明更改内容和原因
   - 关联相关 Issue

2. **代码审查**
   - 等待维护者审查
   - 根据反馈修改

3. **合并**
   - 确保所有测试通过
   - 确保代码质量检查通过
   - 维护者合并 PR

## 功能请求

1. 检查是否已有相关 Issue
2. 创建新的 Issue，标签为 `enhancement`
3. 描述：
   - 功能用途
   - 使用场景
   - 期望行为
   - 可能的实现方案

## 文档贡献

- 修复错别字
- 改进说明
- 添加示例
- 翻译文档

## 行为准则

- 尊重所有参与者
- 接受建设性批评
- 专注于对社区最有利的事情
- 对其他社区成员表示同理心

## 许可证

贡献即表示你同意将你的代码在 MIT 许可证下发布。

## 问题？

如有任何问题，请创建 Issue 或联系维护者。