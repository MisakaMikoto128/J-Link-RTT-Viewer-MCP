"""连接功能测试"""

import pytest
from unittest.mock import Mock, patch
from src.core.jlink_manager import JLinkManager, ConnectionState


class TestJLinkManager:
    """JLinkManager 测试类"""
    
    def setup_method(self):
        self.manager = JLinkManager()
    
    @pytest.mark.asyncio
    async def test_initial_state(self):
        assert self.manager.state == ConnectionState.IDLE
        assert self.manager.device_info.connected is False
        assert self.manager.last_connect_params is None
    
    @pytest.mark.asyncio
    async def test_connect_without_jlink(self):
        with patch('pylink.JLink') as mock_jlink:
            mock_instance = Mock()
            mock_instance.opened.return_value = False
            mock_instance.connected.return_value = True
            mock_instance.memory_read.return_value = b'\x00' * 256
            mock_jlink.return_value = mock_instance

            with patch.object(self.manager, '_find_rtt_cb', return_value=0):
                result = await self.manager.connect(
                    target="STM32F103C8T6",
                    interface="SWD",
                    speed=4000,
                )
            
            assert result["success"] is True
            assert self.manager.state == ConnectionState.CONNECTED
    
    @pytest.mark.asyncio
    async def test_disconnect_when_idle(self):
        result = await self.manager.disconnect()
        assert result["success"] is True
        assert self.manager.state == ConnectionState.IDLE
    
    @pytest.mark.asyncio
    async def test_read_rtt_when_not_connected(self):
        result = await self.manager.read_rtt()
        assert result["success"] is False
        assert "Not connected" in result["message"]
    
    @pytest.mark.asyncio
    async def test_write_rtt_when_not_connected(self):
        result = await self.manager.write_rtt("test")
        assert result["success"] is False
        assert "Not connected" in result["message"]
    
    @pytest.mark.asyncio
    async def test_reset_when_not_connected(self):
        result = await self.manager.reset()
        assert result["success"] is False
        assert "Not connected" in result["message"]
    
    @pytest.mark.asyncio
    async def test_flash_when_not_connected(self):
        result = await self.manager.flash_firmware("test.bin")
        assert result["success"] is False
        assert "Not connected" in result["message"]
    
    @pytest.mark.asyncio
    async def test_read_memory_when_not_connected(self):
        result = await self.manager.read_memory(0x08000000, 256)
        assert result["success"] is False
        assert "Not connected" in result["message"]
    
    @pytest.mark.asyncio
    async def test_write_memory_when_not_connected(self):
        result = await self.manager.write_memory(0x08000000, "AABB")
        assert result["success"] is False
        assert "Not connected" in result["message"]
    
    @pytest.mark.asyncio
    async def test_set_rtt_channel_valid(self):
        result = await self.manager.set_rtt_channel(5)
        assert result["success"] is True
        assert self.manager._channel == 5
    
    @pytest.mark.asyncio
    async def test_set_rtt_channel_invalid(self):
        result = await self.manager.set_rtt_channel(16)
        assert result["success"] is False
    
    @pytest.mark.asyncio
    async def test_set_rtt_channel_boundary(self):
        result = await self.manager.set_rtt_channel(0)
        assert result["success"] is True
        result = await self.manager.set_rtt_channel(15)
        assert result["success"] is True
    
    def test_device_info_to_dict(self):
        result = self.manager._device_info_to_dict()
        assert isinstance(result, dict)
        assert "target_device" in result
        assert "interface" in result
        assert "speed_khz" in result
    
    @pytest.mark.asyncio
    async def test_reconnect_when_connected(self):
        with patch('pylink.JLink') as mock_jlink:
            mock_instance = Mock()
            mock_instance.opened.return_value = False
            mock_instance.connected.return_value = True
            mock_instance.memory_read.return_value = b'\x00' * 256
            mock_jlink.return_value = mock_instance

            with patch.object(self.manager, '_find_rtt_cb', return_value=0):
                await self.manager.connect(target="STM32F103C8T6")
                assert self.manager.state == ConnectionState.CONNECTED

                result = await self.manager.connect(target="STM32F407VG")
                assert result["success"] is True
    
    @pytest.mark.asyncio
    async def test_invalid_interface(self):
        with pytest.raises(ValueError, match="Invalid interface"):
            await self.manager.connect(target="STM32F103C8T6", interface="SPI")
    
    @pytest.mark.asyncio
    async def test_connection_retry(self):
        with patch('pylink.JLink') as mock_jlink:
            mock_instance = Mock()
            mock_instance.opened.return_value = False
            mock_instance.connected.side_effect = [False, True]
            mock_instance.memory_read.return_value = b'\x00' * 256
            mock_jlink.return_value = mock_instance

            with patch.object(self.manager, '_find_rtt_cb', return_value=0):
                result = await self.manager.connect(
                    target="STM32F103C8T6",
                    retry_attempts=2,
                )
                assert result["success"] is True
    
    @pytest.mark.asyncio
    async def test_connection_all_retries_fail(self):
        with patch('pylink.JLink') as mock_jlink:
            mock_instance = Mock()
            mock_instance.opened.return_value = False
            mock_instance.connected.return_value = False
            mock_jlink.return_value = mock_instance
            
            with pytest.raises(ConnectionError):
                await self.manager.connect(
                    target="STM32F103C8T6",
                    retry_attempts=2,
                )
            assert self.manager.state == ConnectionState.IDLE
    
    @pytest.mark.asyncio
    async def test_cleanup(self):
        await self.manager.cleanup()
        assert self.manager.state == ConnectionState.IDLE
