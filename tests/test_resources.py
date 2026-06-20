"""资源模块测试"""

import pytest
from src.core.jlink_manager import JLinkManager, ConnectionState
from src.resources.device import get_device_status, get_rtt_logs, get_firmware_metadata


class TestDeviceResources:
    """设备资源测试"""
    
    def setup_method(self):
        self.manager = JLinkManager()
    
    @pytest.mark.asyncio
    async def test_get_device_status_idle(self):
        result = await get_device_status(self.manager)
        assert result["state"] == "idle"
        assert result["device_info"] is None
    
    @pytest.mark.asyncio
    async def test_get_rtt_logs_empty(self):
        result = await get_rtt_logs(self.manager)
        assert isinstance(result, list)
        assert len(result) == 0
    
    @pytest.mark.asyncio
    async def test_get_firmware_metadata_not_connected(self):
        result = await get_firmware_metadata(self.manager)
        assert "error" in result
