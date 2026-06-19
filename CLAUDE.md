# J-Link RTT Viewer MCP Server 开发指南

## 项目概述

本项目将 J-Link RTT Viewer 的核心功能封装为 MCP (Model Context Protocol) 服务器，使 AI 助手能够通过标准化接口与 STM32 开发板进行交互。

## AI 自助 build (重要)

**改完代码后, AI 自己跑验证**, 不要把构建当甩手活留给用户。

### 安装依赖

```powershell
# 创建虚拟环境
python -m venv .venv
.venv\Scripts\activate

# 安装依赖
pip install -r requirements.txt
```

### 运行测试

```powershell
# 运行所有测试
pytest tests/ -v

# 运行特定测试
pytest tests/test_connection.py -v

# 运行带覆盖率的测试
pytest tests/ --cov=src --cov-report=html
```

### 类型检查

```powershell
mypy src/ --ignore-missing-imports
```

### 代码格式化

```powershell
# 格式化代码
ruff format src/

# 检查代码质量
ruff check src/
```

## MCP 服务器配置

### Claude Desktop 配置

在 Claude Desktop 的配置文件中添加：

**Windows**: `%APPDATA%\Claude\claude_desktop_config.json`

```json
{
  "mcpServers": {
    "jlink-rtt": {
      "command": "python",
      "args": [
        "-m",
        "src.server"
      ],
      "cwd": "C:\\path\\to\\J-Link-RTT-Viewer-MCP-mimo"
    }
  }
}
```

### 测试 MCP 服务器

```powershell
# 使用 MCP Inspector 测试
npx -y @modelcontextprotocol/inspector

# 或手动测试
echo '{"jsonrpc": "2.0", "method": "initialize", "params": {"protocolVersion": "2024-11-05", "capabilities": {}, "clientInfo": {"name": "test", "version": "1.0"}}, "id": 1}' | python -m src.server
```

## 开发规范

### 代码风格

1. **使用类型提示**: 所有函数和方法必须有完整的类型提示
2. **文档字符串**: 所有公共函数必须有 docstring
3. **异步优先**: 所有 I/O 操作使用 async/await
4. **错误处理**: 使用 try/except 包裹所有可能失败的操作

### 线程安全

**重要**: J-Link 操作必须在独立线程中执行，避免阻塞 asyncio 事件循环。

```python
# 正确做法
async def connect(self, target: str) -> dict:
    result = await self._run_in_executor(self._do_connect, target)
    return result

def _do_connect(self, target: str) -> None:
    # 这里是阻塞操作，在线程池中执行
    self.jlink.connect(target)
```

### 错误处理模式

```python
# 标准错误处理模式
async def some_operation(self) -> dict:
    if self.state != ConnectionState.CONNECTED:
        return {"success": False, "message": "Not connected"}
    
    try:
        result = await self._run_in_executor(self._do_operation)
        return {"success": True, "message": "Success", "data": result}
    except SpecificError as e:
        return {"success": False, "message": f"Specific error: {e}"}
    except Exception as e:
        logger.exception("Unexpected error")
        return {"success": False, "message": f"Unexpected error: {e}"}
```

## 边界情况处理

### 连接场景

1. **J-Link 未连接**
   - 返回明确错误信息
   - 建议检查硬件连接

2. **MCU 无响应**
   - 尝试降低连接速度
   - 提供调试建议

3. **连接超时**
   - 自动重试机制
   - 最多重试 3 次

4. **接口选择错误**
   - 自动检测或提供选项

### 重置场景

1. **重置后无日志**
   - 自动重连并重新开始 RTT 会话

2. **重置失败**
   - 提供诊断信息
   - 建议硬件检查

3. **需要暂停**
   - 支持 halt 模式
   - 停在复位状态

### 烧录场景

1. **固件文件不存在**
   - 检查路径
   - 提供文件选择

2. **烧录失败**
   - 详细错误信息
   - 常见原因列表

3. **烧录中断**
   - 恢复机制
   - 建议拔插 J-Link

## pylink 重要注意事项

### 版本要求

**必须使用 `pylink-square==1.6.0`**

2.x 版本存在 API 不兼容问题，会导致 RTT 功能失效。

### 连接顺序

```python
# 正确的连接顺序（pylink 1.6.0）
jlink.open()
ser = jlink.serial_number
jlink.close()
jlink.open(str(ser))
jlink.rtt_start()  # 必须在 connect 之前

jlink.set_tif(JLinkInterfaces.SWD)
jlink.set_speed(4000)
jlink.connect(target)
```

### 清理操作

```python
# 安全的清理操作
try:
    jlink.rtt_stop()
except Exception as e:
    logger.warning(f"rtt_stop failed: {e}")

try:
    jlink.close()
except Exception as e:
    logger.warning(f"close failed: {e}")
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

## 常见问题

### Q: 连接失败怎么办？

A: 检查以下几点：
1. J-Link 是否正确连接
2. MCU 是否上电
3. SWD/JTAG 接线是否正确
4. 尝试降低连接速度

### Q: RTT 没有数据？

A: 检查以下几点：
1. MCU 代码是否初始化了 SEGGER_RTT
2. RTT 通道号是否匹配
3. 尝试重置设备

### Q: 烧录失败？

A: 检查以下几点：
1. 固件文件格式是否正确
2. J-Link 连接是否稳定
3. 尝试以管理员身份运行
4. 检查 MCU 读写保护状态

## 贡献指南

1. 创建功能分支
2. 编写测试
3. 确保所有测试通过
4. 更新文档
5. 提交 Pull Request

## 许可证

MIT License