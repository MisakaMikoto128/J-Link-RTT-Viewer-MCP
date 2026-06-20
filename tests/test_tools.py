"""工具模块测试"""

import pytest
from unittest.mock import Mock, AsyncMock, patch
from src.core.jlink_manager import JLinkManager, ConnectionState
from src.tools.connection import connect, disconnect, get_device_info
from src.tools.rtt import read_rtt, write_rtt, set_rtt_channel
from src.tools.reset import reset_device
from src.tools.flash import flash_firmware
from src.tools.memory import read_memory, write_memory


class TestConnectionTools:
    """连接工具测试"""
    
    def setup_method(self):
        self.manager = JLinkManager()
    
    @pytest.mark.asyncio
    async def test_connect_tool(self):
        with patch('pylink.JLink') as mock_jlink:
            mock_instance = Mock()
            mock_instance.opened.return_value = False
            mock_instance.connected.return_value = True
            mock_instance.memory_read.return_value = b'\x00' * 256
            mock_jlink.return_value = mock_instance

            with patch.object(self.manager, '_find_rtt_cb', return_value=0):
                result = await connect(self.manager, target="STM32F103C8T6")
                assert result["success"] is True
    
    @pytest.mark.asyncio
    async def test_disconnect_tool(self):
        result = await disconnect(self.manager)
        assert result["success"] is True
    
    @pytest.mark.asyncio
    async def test_get_device_info_tool(self):
        result = await get_device_info(self.manager)
        assert result["success"] is False
        assert result["message"] == "Not connected"


class TestRTTTools:
    """RTT 工具测试"""
    
    def setup_method(self):
        self.manager = JLinkManager()
    
    @pytest.mark.asyncio
    async def test_read_rtt_tool(self):
        result = await read_rtt(self.manager)
        assert result["success"] is False
    
    @pytest.mark.asyncio
    async def test_write_rtt_tool(self):
        result = await write_rtt(self.manager, "test")
        assert result["success"] is False
    
    @pytest.mark.asyncio
    async def test_set_rtt_channel_tool(self):
        result = await set_rtt_channel(self.manager, 5)
        assert result["success"] is True
        assert self.manager._channel == 5


class TestResetTools:
    """重置工具测试"""
    
    def setup_method(self):
        self.manager = JLinkManager()
    
    @pytest.mark.asyncio
    async def test_reset_tool(self):
        result = await reset_device(self.manager)
        assert result["success"] is False


class TestFlashTools:
    """烧录工具测试"""
    
    def setup_method(self):
        self.manager = JLinkManager()
    
    @pytest.mark.asyncio
    async def test_flash_tool(self):
        result = await flash_firmware(self.manager, "test.bin")
        assert result["success"] is False


class TestMemoryTools:
    """内存工具测试"""
    
    def setup_method(self):
        self.manager = JLinkManager()
    
    @pytest.mark.asyncio
    async def test_read_memory_tool(self):
        result = await read_memory(self.manager, 0x08000000, 256)
        assert result["success"] is False
    
    @pytest.mark.asyncio
    async def test_write_memory_tool(self):
        result = await write_memory(self.manager, 0x08000000, "AABB")
        assert result["success"] is False
