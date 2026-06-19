"""MCP 服务器测试脚本

用于验证 MCP 服务器的基本功能。
"""

import asyncio
import json
import sys
from src.core.jlink_manager import JLinkManager


async def test_connection():
    """测试连接功能"""
    print("=== 测试连接功能 ===")
    manager = JLinkManager()
    
    # 测试初始状态
    assert manager.state.value == "idle", "初始状态应为 idle"
    print("✓ 初始状态正确")
    
    # 测试未连接时的操作
    result = await manager.read_rtt()
    assert result["success"] is False, "未连接时读取应失败"
    print("✓ 未连接时读取 RTT 正确返回失败")
    
    result = await manager.write_rtt("test")
    assert result["success"] is False, "未连接时写入应失败"
    print("✓ 未连接时写入 RTT 正确返回失败")
    
    result = await manager.reset()
    assert result["success"] is False, "未连接时重置应失败"
    print("✓ 未连接时重置正确返回失败")
    
    result = await manager.read_memory(0x08000000, 256)
    assert result["success"] is False, "未连接时读取内存应失败"
    print("✓ 未连接时读取内存正确返回失败")
    
    print("✓ 所有基本测试通过")
    print()


async def test_rtt_channel():
    """测试 RTT 通道设置"""
    print("=== 测试 RTT 通道设置 ===")
    manager = JLinkManager()
    
    # 测试有效通道
    result = await manager.set_rtt_channel(5)
    assert result["success"] is True, "设置有效通道应成功"
    assert manager._channel == 5, "通道号应更新"
    print("✓ 设置有效通道成功")
    
    # 测试无效通道
    result = await manager.set_rtt_channel(16)
    assert result["success"] is False, "设置无效通道应失败"
    print("✓ 设置无效通道正确返回失败")
    
    result = await manager.set_rtt_channel(-1)
    assert result["success"] is False, "设置负数通道应失败"
    print("✓ 设置负数通道正确返回失败")
    
    print("✓ 所有通道测试通过")
    print()


async def test_device_info():
    """测试设备信息转换"""
    print("=== 测试设备信息转换 ===")
    manager = JLinkManager()
    
    info_dict = manager._device_info_to_dict()
    assert isinstance(info_dict, dict), "应返回字典"
    assert "target_device" in info_dict, "应包含 target_device"
    assert "interface" in info_dict, "应包含 interface"
    assert "speed_khz" in info_dict, "应包含 speed_khz"
    print("✓ 设备信息转换正确")
    
    print("✓ 所有设备信息测试通过")
    print()


async def main():
    """主测试函数"""
    print("J-Link RTT Viewer MCP Server 测试")
    print("=" * 40)
    print()
    
    try:
        await test_connection()
        await test_rtt_channel()
        await test_device_info()
        
        print("=" * 40)
        print("✓ 所有测试通过！")
        print()
        
        # 显示使用说明
        print("下一步:")
        print("1. 连接 J-Link 和 MCU")
        print("2. 运行: python -m src.server")
        print("3. 配置 Claude Desktop")
        print("4. 在 Claude Desktop 中测试连接")
        
    except AssertionError as e:
        print(f"✗ 测试失败: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"✗ 测试出错: {e}")
        sys.exit(1)


if __name__ == "__main__":
    asyncio.run(main())