"""MCP Server 工具层测试"""

import pytest
import json
import asyncio
from unittest.mock import Mock, patch
from src.core.jlink_manager import JLinkManager, ConnectionState


class TestServerConnectTool:
    """server.py connect 工具测试"""

    @pytest.mark.asyncio
    async def test_connect_tool_success(self):
        with patch('pylink.JLink') as mock_jlink:
            mock_instance = Mock()
            mock_instance.opened.return_value = False
            mock_instance.connected.return_value = True
            mock_instance.memory_read.return_value = b'\x00' * 256
            mock_jlink.return_value = mock_instance

            from src.server import get_manager, connect
            manager = get_manager()
            with patch.object(manager, '_find_rtt_cb', return_value=0):
                result = await connect("STM32F103C8", "SWD", 4000)
                data = json.loads(result)
                assert data["success"] is True

    @pytest.mark.asyncio
    async def test_connect_tool_error(self):
        from src.server import get_manager, connect
        manager = get_manager()
        with patch.object(
            manager, 'connect', side_effect=ConnectionError("No J-Link")
        ):
            result = await connect("STM32F103C8")
            data = json.loads(result)
            assert data["success"] is False


class TestServerDisconnectTool:
    """server.py disconnect 工具测试"""

    @pytest.mark.asyncio
    async def test_disconnect_tool_idle(self):
        from src.server import get_manager, disconnect
        manager = get_manager()
        result = await disconnect()
        data = json.loads(result)
        assert data["success"] is True


class TestServerDeviceInfoTool:
    """server.py get_device_info 工具测试"""

    @pytest.mark.asyncio
    async def test_get_device_info_not_connected(self):
        from src.server import get_manager, get_device_info
        manager = get_manager()
        result = await get_device_info()
        data = json.loads(result)
        assert data["success"] is False


class TestServerReadRttTool:
    """server.py read_rtt 工具测试"""

    @pytest.mark.asyncio
    async def test_read_rtt_not_connected(self):
        from src.server import get_manager, read_rtt
        manager = get_manager()
        result = await read_rtt()
        data = json.loads(result)
        assert data["success"] is False


class TestServerWriteRttTool:
    """server.py write_rtt 工具测试"""

    @pytest.mark.asyncio
    async def test_write_rtt_not_connected(self):
        from src.server import get_manager, write_rtt
        manager = get_manager()
        result = await write_rtt("test")
        data = json.loads(result)
        assert data["success"] is False


class TestServerSetRttChannelTool:
    """server.py set_rtt_channel 工具测试"""

    @pytest.mark.asyncio
    async def test_set_rtt_channel_success(self):
        from src.server import get_manager, set_rtt_channel
        manager = get_manager()
        result = await set_rtt_channel(5)
        data = json.loads(result)
        assert data["success"] is True

    @pytest.mark.asyncio
    async def test_set_rtt_channel_invalid(self):
        from src.server import get_manager, set_rtt_channel
        manager = get_manager()
        result = await set_rtt_channel(16)
        data = json.loads(result)
        assert data["success"] is False


class TestServerResetTool:
    """server.py reset 工具测试"""

    @pytest.mark.asyncio
    async def test_reset_not_connected(self):
        from src.server import get_manager, reset
        manager = get_manager()
        result = await reset()
        data = json.loads(result)
        assert data["success"] is False


class TestServerFlashFirmwareTool:
    """server.py flash_firmware 工具测试"""

    @pytest.mark.asyncio
    async def test_flash_not_connected(self):
        from src.server import get_manager, flash_firmware
        manager = get_manager()
        result = await flash_firmware("test.bin")
        data = json.loads(result)
        assert data["success"] is False


class TestServerReadMemoryTool:
    """server.py read_memory 工具测试"""

    @pytest.mark.asyncio
    async def test_read_memory_not_connected(self):
        from src.server import get_manager, read_memory
        manager = get_manager()
        result = await read_memory(0x08000000, 256)
        data = json.loads(result)
        assert data["success"] is False


class TestServerWriteMemoryTool:
    """server.py write_memory 工具测试"""

    @pytest.mark.asyncio
    async def test_write_memory_not_connected(self):
        from src.server import get_manager, write_memory
        manager = get_manager()
        result = await write_memory(0x08000000, "AABB")
        data = json.loads(result)
        assert data["success"] is False


class TestServerResources:
    """MCP 资源测试"""

    @pytest.mark.asyncio
    async def test_device_status_idle(self):
        from src.server import get_device_status
        result = await get_device_status()
        data = json.loads(result)
        assert data["state"] == "idle"

    @pytest.mark.asyncio
    async def test_rtt_logs_empty(self):
        from src.server import get_rtt_logs
        result = await get_rtt_logs()
        data = json.loads(result)
        assert isinstance(data, list)

    @pytest.mark.asyncio
    async def test_firmware_metadata_not_connected(self):
        from src.server import get_firmware_metadata
        result = await get_firmware_metadata()
        data = json.loads(result)
        assert "error" in data


class TestServerPrompts:
    """MCP 提示模板测试"""

    @pytest.mark.asyncio
    async def test_debugging_guide(self):
        from src.server import debugging_guide
        result = await debugging_guide()
        assert "调试指南" in result

    @pytest.mark.asyncio
    async def test_error_diagnosis(self):
        from src.server import error_diagnosis
        result = await error_diagnosis("test error")
        assert "test error" in result

    @pytest.mark.asyncio
    async def test_flash_wizard(self):
        from src.server import flash_wizard
        result = await flash_wizard()
        assert "烧录向导" in result
