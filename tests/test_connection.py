"""连接功能测试"""

import pytest
import asyncio
from unittest.mock import Mock, AsyncMock, patch
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
        assert self.manager.last_connect_params is None
    
    @pytest.mark.asyncio
    async def test_connect_without_jlink(self):
        """测试无 J-Link 时连接"""
        with patch('pylink.JLink') as mock_jlink:
            mock_instance = Mock()
            mock_instance.opened.return_value = False
            mock_instance.connected.return_value = True
            mock_jlink.return_value = mock_instance
            
            result = await self.manager.connect(
                target="STM32F103C8T6",
                interface="SWD",
                speed=4000,
            )
            
            assert result["success"] is True
            assert self.manager.state == ConnectionState.CONNECTED
    
    @pytest.mark.asyncio
    async def test_disconnect_when_idle(self):
        """测试空闲状态断开"""
        result = await self.manager.disconnect()
        assert result["success"] is True
        assert self.manager.state == ConnectionState.IDLE
    
    @pytest.mark.asyncio
    async def test_read_rtt_when_not_connected(self):
        """测试未连接时读取 RTT"""
        result = await self.manager.read_rtt()
        assert result["success"] is False
        assert "Not connected" in result["message"]
    
    @pytest.mark.asyncio
    async def test_write_rtt_when_not_connected(self):
        """测试未连接时写入 RTT"""
        result = await self.manager.write_rtt("test")
        assert result["success"] is False
        assert "Not connected" in result["message"]
    
    @pytest.mark.asyncio
    async def test_reset_when_not_connected(self):
        """测试未连接时重置"""
        result = await self.manager.reset()
        assert result["success"] is False
        assert "Not connected" in result["message"]
    
    @pytest.mark.asyncio
    async def test_flash_when_not_connected(self):
        """测试未连接时烧录"""
        result = await self.manager.flash_firmware("test.bin")
        assert result["success"] is False
        assert "Not connected" in result["message"]
    
    @pytest.mark.asyncio
    async def test_read_memory_when_not_connected(self):
        """测试未连接时读取内存"""
        result = await self.manager.read_memory(0x08000000, 256)
        assert result["success"] is False
        assert "Not connected" in result["message"]
    
    @pytest.mark.asyncio
    async def test_write_memory_when_not_connected(self):
        """测试未连接时写入内存"""
        result = await self.manager.write_memory(0x08000000, "AABB")
        assert result["success"] is False
        assert "Not connected" in result["message"]
    
    def test_set_rtt_channel_valid(self):
        """测试设置有效 RTT 通道"""
        loop = asyncio.new_event_loop()
        result = loop.run_until_complete(self.manager.set_rtt_channel(5))
        assert result["success"] is True
        assert self.manager._channel == 5
        loop.close()
    
    def test_set_rtt_channel_invalid(self):
        """测试设置无效 RTT 通道"""
        loop = asyncio.new_event_loop()
        result = loop.run_until_complete(self.manager.set_rtt_channel(16))
        assert result["success"] is False
        loop.close()
    
    def test_device_info_to_dict(self):
        """测试设备信息转换为字典"""
        result = self.manager._device_info_to_dict()
        assert isinstance(result, dict)
        assert "target_device" in result
        assert "interface" in result
        assert "speed_khz" in result