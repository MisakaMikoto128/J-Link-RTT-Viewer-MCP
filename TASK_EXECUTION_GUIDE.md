# 任务执行指南

## 概述

本文档说明如何使用 goal 指令让 AI 完成 J-Link RTT Viewer MCP Server 的开发任务。

## 前置条件

1. **硬件**
   - J-Link 调试器已连接电脑
   - STM32F103 已通过 J-Link 连接
   - 板子已上电

2. **软件**
   - Python 3.10+
   - Git
   - Claude Desktop（可选，用于测试 MCP 集成）

3. **项目状态**
   - 基础框架已创建
   - 核心代码已编写
   - 文档已完成

## Goal 指令模板

### 任务 1：安装依赖和验证环境

```
goal: 在 C:\Users\liuyu\Desktop\WorkPlace\J-Link-RTT-Viewer-MCP-mimo 项目中执行以下任务：

1. 创建虚拟环境并安装依赖
2. 运行测试验证环境
3. 确认 J-Link 可以连接到 STM32F103

执行步骤：
1. cd C:\Users\liuyu\Desktop\WorkPlace\J-Link-RTT-Viewer-MCP-mimo
2. python -m venv .venv
3. .venv\Scripts\activate
4. pip install -r requirements.txt
5. pip install -e .
6. python test_server.py

预期结果：
- 所有测试通过
- 虚拟环境创建成功
- 依赖安装成功
```

### 任务 2：测试 J-Link 连接

```
goal: 在 C:\Users\liuyu\Desktop\WorkPlace\J-Link-RTT-Viewer-MCP-mimo 项目中测试 J-Link 连接：

1. 编写一个测试脚本，连接到 STM32F103C8T6
2. 获取设备信息
3. 读取 RTT 数据
4. 断开连接

执行步骤：
1. cd C:\Users\liuyu\Desktop\WorkPlace\J-Link-RTT-Viewer-MCP-mimo
2. .venv\Scripts\activate
3. 创建并运行测试脚本

测试脚本内容：
```python
import asyncio
from src.core.jlink_manager import JLinkManager

async def test():
    manager = JLinkManager()
    
    # 连接
    result = await manager.connect(target="STM32F103C8T6")
    print(f"连接: {result}")
    
    # 获取设备信息
    print(f"设备信息: {manager._device_info_to_dict()}")
    
    # 读取 RTT
    result = await manager.read_rtt(timeout=2.0)
    print(f"RTT: {result}")
    
    # 断开
    result = await manager.disconnect()
    print(f"断开: {result}")

asyncio.run(test())
```

预期结果：
- 连接成功
- 设备信息正确
- RTT 数据读取成功
```

### 任务 3：运行完整测试套件

```
goal: 在 C:\Users\liuyu\Desktop\WorkPlace\J-Link-RTT-Viewer-MCP-mimo 项目中运行完整测试套件：

1. 运行所有单元测试
2. 生成测试覆盖率报告
3. 修复任何失败的测试

执行步骤：
1. cd C:\Users\liuyu\Desktop\WorkPlace\J-Link-RTT-Viewer-MCP-mimo
2. .venv\Scripts\activate
3. pytest tests/ -v
4. pytest tests/ --cov=src --cov-report=html

预期结果：
- 所有测试通过
- 覆盖率报告生成
```

### 任务 4：启动 MCP 服务器并测试

```
goal: 在 C:\Users\liuyu\Desktop\WorkPlace\J-Link-RTT-Viewer-MCP-mimo 项目中启动 MCP 服务器：

1. 启动 MCP 服务器
2. 使用 MCP Inspector 测试
3. 验证所有工具可用

执行步骤：
1. cd C:\Users\liuyu\Desktop\WorkPlace\J-Link-RTT-Viewer-MCP-mimo
2. .venv\Scripts\activate
3. python -m src.server（在一个终端）
4. npx -y @modelcontextprotocol/inspector（在另一个终端）

预期结果：
- 服务器启动成功
- Inspector 可以连接
- 所有工具显示在 Inspector 中
```

### 任务 5：实现 RTT Handler

```
goal: 在 C:\Users\liuyu\Desktop\WorkPlace\J-Link-RTT-Viewer-MCP-mimo 项目中实现 RTT Handler：

1. 创建 src/core/rtt_handler.py
2. 实现 RTT 数据解析和格式化
3. 添加 ANSI 颜色代码解析
4. 编写测试

文件内容参考：
- 基于现有 jlink_worker.py 中的 RTT 处理逻辑
- 支持多通道读取
- 支持异步操作
- 完善的错误处理

预期结果：
- RTT Handler 实现完成
- 测试通过
- 可以正确解析 RTT 数据
```

### 任务 6：实现 Tools 模块

```
goal: 在 C:\Users\liuyu\Desktop\WorkPlace\J-Link-RTT-Viewer-MCP-mimo 项目中实现 Tools 模块：

1. 实现 src/tools/connection.py
2. 实现 src/tools/rtt.py
3. 实现 src/tools/reset.py
4. 实现 src/tools/flash.py
5. 实现 src/tools/memory.py
6. 为每个工具编写测试

每个工具应该：
- 有清晰的文档字符串
- 有参数验证
- 有错误处理
- 返回 JSON 格式的结果

预期结果：
- 所有工具实现完成
- 测试通过
- MCP 服务器可以调用所有工具
```

### 任务 7：实现 Resources 模块

```
goal: 在 C:\Users\liuyu\Desktop\WorkPlace\J-Link-RTT-Viewer-MCP-mimo 项目中实现 Resources 模块：

1. 实现 src/resources/device.py
2. 提供设备状态资源
3. 提供 RTT 日志资源
4. 提供固件元数据资源

预期结果：
- 资源实现完成
- 可以通过 MCP 协议访问资源
```

### 任务 8：实现 Prompts 模块

```
goal: 在 C:\Users\liuyu\Desktop\WorkPlace\J-Link-RTT-Viewer-MCP-mimo 项目中实现 Prompts 模块：

1. 实现 src/prompts/templates.py
2. 创建调试指南提示
3. 创建错误诊断提示
4. 创建烧录向导提示

预期结果：
- 提示模板实现完成
- 可以通过 MCP 协议访问提示
```

### 任务 9：完善错误处理

```
goal: 在 C:\Users\liuyu\Desktop\WorkPlace\J-Link-RTT-Viewer-MCP-mimo 项目中完善错误处理：

1. 检查所有工具的错误处理路径
2. 添加详细的错误信息
3. 实现重试机制
4. 添加超时处理
5. 编写错误处理测试

边界情况：
- J-Link 未连接
- MCU 无响应
- 连接超时
- 烧录失败
- 内存读写错误

预期结果：
- 所有边界情况都有处理
- 错误信息清晰
- 测试覆盖所有错误路径
```

### 任务 10：集成测试

```
goal: 在 C:\Users\liuyu\Desktop\WorkPlace\J-Link-RTT-Viewer-MCP-mimo 项目中进行集成测试：

1. 测试完整的连接-读取-断开流程
2. 测试重置和重连流程
3. 测试固件烧录流程（如果可能）
4. 测试内存读写流程
5. 验证 MCP 协议交互

预期结果：
- 所有集成测试通过
- 完整工作流程正常
```

## 执行顺序

建议按以下顺序执行任务：

1. **任务 1**：安装依赖和验证环境
2. **任务 2**：测试 J-Link 连接
3. **任务 3**：运行完整测试套件
4. **任务 5**：实现 RTT Handler
5. **任务 6**：实现 Tools 模块
6. **任务 7**：实现 Resources 模块
7. **任务 8**：实现 Prompts 模块
8. **任务 9**：完善错误处理
9. **任务 10**：集成测试
10. **任务 4**：启动 MCP 服务器并测试

## 常见问题

### Q: 虚拟环境创建失败？

A: 检查 Python 版本，确保是 3.10+。

### Q: 依赖安装失败？

A: 检查网络连接，尝试使用镜像源：
```bash
pip install -r requirements.txt -i https://pypi.tuna.tsinghua.edu.cn/simple
```

### Q: J-Link 连接失败？

A: 检查：
1. J-Link 驱动是否安装
2. USB 连接是否正常
3. MCU 是否上电
4. 尝试降低连接速度

### Q: 测试失败？

A: 查看错误信息，检查：
1. 代码是否正确
2. 依赖是否安装
3. 环境变量是否设置

### Q: MCP 服务器启动失败？

A: 检查：
1. 端口是否被占用
2. 依赖是否安装
3. 日志文件查看错误

## 监控和调试

### 查看日志

```bash
# 实时查看日志
tail -f jlink_mcp.log

# 查看错误
grep ERROR jlink_mcp.log
```

### 使用 MCP Inspector

```bash
npx -y @modelcontextprotocol/inspector
```

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

## 完成标准

当以下条件都满足时，任务完成：

1. ✅ 所有测试通过
2. ✅ J-Link 可以连接到 STM32
3. ✅ RTT 数据可以读取
4. ✅ MCP 服务器可以启动
5. ✅ 所有工具可用
6. ✅ 错误处理完善
7. ✅ 文档完整