# Contributing to J-Link RTT Viewer MCP Server

[English](#english) | [中文](#中文)

---

## English

Thank you for your interest in contributing! This document provides guidelines for contributing to this project.

### How to Contribute

1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/amazing-feature`)
3. **Commit** your changes (`git commit -m 'Add amazing feature'`)
4. **Push** to the branch (`git push origin feature/amazing-feature`)
5. **Open** a Pull Request

### Development Setup

```bash
# Clone your fork
git clone https://github.com/your-username/J-Link-RTT-Viewer-MCP-mimo.git
cd J-Link-RTT-Viewer-MCP-mimo

# Create virtual environment
python -m venv .venv
.venv\Scripts\activate  # Windows
# source .venv/bin/activate  # Linux/Mac

# Install dependencies
pip install -r requirements.txt

# Run tests
pytest tests/ -v
```

### Code Style

- Follow PEP 8 style guide
- Use type hints for all functions
- Add docstrings to public functions
- Keep functions focused and concise

### Testing

- Write tests for new features
- Ensure all tests pass before submitting
- Aim for good test coverage

```bash
# Run tests
pytest tests/ -v

# Run with coverage
pytest tests/ --cov=src --cov-report=html
```

### Pull Request Guidelines

- Provide a clear description of changes
- Reference any related issues
- Include tests for new functionality
- Update documentation if needed

### Reporting Issues

- Use the GitHub issue tracker
- Include reproduction steps
- Provide system information (OS, Python version, J-Link model)

### License

By contributing, you agree that your contributions will be licensed under the MIT License.

---

## 中文

感谢您对本项目的贡献兴趣！本文档提供了贡献指南。

### 如何贡献

1. **Fork** 本仓库
2. **创建** 功能分支 (`git checkout -b feature/amazing-feature`)
3. **提交** 您的更改 (`git commit -m 'Add amazing feature'`)
4. **推送** 到分支 (`git push origin feature/amazing-feature`)
5. **创建** Pull Request

### 开发环境设置

```bash
# 克隆您的 fork
git clone https://github.com/your-username/J-Link-RTT-Viewer-MCP-mimo.git
cd J-Link-RTT-Viewer-MCP-mimo

# 创建虚拟环境
python -m venv .venv
.venv\Scripts\activate  # Windows
# source .venv/bin/activate  # Linux/Mac

# 安装依赖
pip install -r requirements.txt

# 运行测试
pytest tests/ -v
```

### 代码风格

- 遵循 PEP 8 风格指南
- 为所有函数使用类型提示
- 为公共函数添加文档字符串
- 保持函数专注和简洁

### 测试

- 为新功能编写测试
- 提交前确保所有测试通过
- 追求良好的测试覆盖率

```bash
# 运行测试
pytest tests/ -v

# 运行带覆盖率的测试
pytest tests/ --cov=src --cov-report=html
```

### Pull Request 指南

- 提供清晰的更改描述
- 引用相关的问题
- 为新功能包含测试
- 必要时更新文档

### 报告问题

- 使用 GitHub issue tracker
- 包含重现步骤
- 提供系统信息（操作系统、Python 版本、J-Link 型号）

### 许可证

通过贡献，您同意您的贡献将在 MIT 许可证下许可。
