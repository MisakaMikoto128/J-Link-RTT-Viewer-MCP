"""基本使用示例

展示如何使用 J-Link RTT Viewer MCP Server 的基本功能。
"""

import asyncio
import json
from src.core.jlink_manager import JLinkManager


async def main():
    """主函数"""
    # 创建管理器实例
    manager = JLinkManager()
    
    try:
        # 1. 连接设备
        print("=== 连接设备 ===")
        result = await manager.connect(
            target="STM32F103C8T6",
            interface="SWD",
            speed=4000,
        )
        print(json.dumps(result, indent=2, ensure_ascii=False))
        
        if not result["success"]:
            print("连接失败，退出")
            return
        
        # 2. 获取设备信息
        print("\n=== 设备信息 ===")
        print(json.dumps(manager._device_info_to_dict(), indent=2, ensure_ascii=False))
        
        # 3. 读取 RTT 数据
        print("\n=== 读取 RTT 数据 ===")
        result = await manager.read_rtt(timeout=2.0)
        print(json.dumps(result, indent=2, ensure_ascii=False))
        
        # 4. 写入 RTT 数据
        print("\n=== 写入 RTT 数据 ===")
        result = await manager.write_rtt("Hello from MCP!")
        print(json.dumps(result, indent=2, ensure_ascii=False))
        
        # 5. 设置 RTT 通道
        print("\n=== 设置 RTT 通道 ===")
        result = await manager.set_rtt_channel(1)
        print(json.dumps(result, indent=2, ensure_ascii=False))
        
        # 6. 重置设备
        print("\n=== 重置设备 ===")
        result = await manager.reset(mode="normal")
        print(json.dumps(result, indent=2, ensure_ascii=False))
        
        # 7. 读取内存
        print("\n=== 读取内存 ===")
        result = await manager.read_memory(address=0x08000000, size=64)
        print(json.dumps(result, indent=2, ensure_ascii=False))
        
        # 8. 断开连接
        print("\n=== 断开连接 ===")
        result = await manager.disconnect()
        print(json.dumps(result, indent=2, ensure_ascii=False))
        
    except Exception as e:
        print(f"错误: {e}")
    finally:
        # 确保清理
        await manager.cleanup()


if __name__ == "__main__":
    asyncio.run(main())