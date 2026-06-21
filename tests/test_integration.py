"""集成流程测试 - 模拟完整的工程师工作流"""

import pytest
import asyncio
from unittest.mock import Mock, patch
from src.core.jlink_manager import JLinkManager, ConnectionState


class TestIntegrationConnectReadWriteDisconnect:
    """完整连接-读取-写入-断开流程"""

    def setup_method(self):
        self.manager = JLinkManager()

    @pytest.mark.asyncio
    async def test_full_flow(self):
        with patch('pylink.JLink') as mock_jlink:
            mock_instance = Mock()
            mock_instance.opened.return_value = False
            mock_instance.connected.return_value = True
            mock_instance.memory_read.return_value = b'\x00' * 256
            mock_jlink.return_value = mock_instance

            with patch.object(self.manager, '_find_rtt_cb', return_value=0):
                # 1. 连接
                result = await self.manager.connect(
                    target="STM32F103C8", interface="SWD", speed=4000
                )
                assert result["success"] is True
                assert self.manager.state == ConnectionState.CONNECTED

                # 2. 读取 RTT
                with patch.object(
                    self.manager, '_run_serialized',
                    return_value=[(0, list(b"[INFO] Hello"))]
                ):
                    rtt = await self.manager.read_rtt(channels=[0])
                    assert rtt["success"] is True
                    assert len(rtt["data"]) > 0

                # 3. 写入 RTT
                with patch.object(
                    self.manager, '_run_serialized', return_value=4
                ):
                    wr = await self.manager.write_rtt("ping")
                    assert wr["success"] is True

                # 4. 设置通道
                ch_result = await self.manager.set_rtt_channel(5)
                assert ch_result["success"] is True
                assert self.manager._channel == 5

                # 5. 读取内存
                with patch.object(
                    self.manager, '_run_serialized',
                    return_value=b'\xAA\xBB'
                ):
                    mem = await self.manager.read_memory(0x08000000, 2)
                    assert mem["success"] is True

                # 6. 断开
                disc = await self.manager.disconnect()
                assert disc["success"] is True
                assert self.manager.state == ConnectionState.IDLE


class TestIntegrationConnectResetRead:
    """连接-重置-读取流程"""

    def setup_method(self):
        self.manager = JLinkManager()

    @pytest.mark.asyncio
    async def test_connect_reset_read(self):
        with patch('pylink.JLink') as mock_jlink:
            mock_instance = Mock()
            mock_instance.opened.return_value = False
            mock_instance.connected.return_value = True
            mock_instance.memory_read.return_value = b'\x00' * 256
            mock_jlink.return_value = mock_instance

            with patch.object(self.manager, '_find_rtt_cb', return_value=0):
                # 1. 连接
                await self.manager.connect(target="STM32F103C8")

                # 2. 重置
                with patch.object(
                    self.manager, '_run_serialized',
                    return_value=(True, "")
                ):
                    reset_result = await self.manager.reset(mode="normal")
                    assert reset_result["success"] is True

                # 3. 读取
                with patch.object(
                    self.manager, '_run_serialized',
                    return_value=[(0, list(b"post-reset data"))]
                ):
                    rtt = await self.manager.read_rtt()
                    assert rtt["success"] is True


class TestIntegrationConnectFlashReset:
    """连接-烧录-重置流程"""

    def setup_method(self):
        self.manager = JLinkManager()

    @pytest.mark.asyncio
    async def test_connect_flash_reset(self):
        with patch('pylink.JLink') as mock_jlink:
            mock_instance = Mock()
            mock_instance.opened.return_value = False
            mock_instance.connected.return_value = True
            mock_instance.memory_read.return_value = b'\x00' * 256
            mock_jlink.return_value = mock_instance

            with patch.object(self.manager, '_find_rtt_cb', return_value=0):
                # 1. 连接
                await self.manager.connect(target="STM32F103C8")

                # 2. 烧录（模拟）
                import tempfile, os
                with tempfile.NamedTemporaryFile(
                    suffix=".bin", delete=False
                ) as f:
                    f.write(b'\x00' * 100)
                    tmp_path = f.name
                try:
                    with patch.object(
                        self.manager, '_run_serialized',
                        return_value={
                            "success": True,
                            "message": "Flashed",
                            "file_size": 100,
                        }
                    ):
                        flash = await self.manager.flash_firmware(tmp_path)
                        assert flash["success"] is True
                finally:
                    os.unlink(tmp_path)

                # 3. 重置
                with patch.object(
                    self.manager, '_run_serialized',
                    return_value=(True, "")
                ):
                    reset = await self.manager.reset(mode="normal")
                    assert reset["success"] is True


class TestIntegrationLogRecording:
    """日志录制完整流程"""

    def setup_method(self):
        self.manager = JLinkManager()

    @pytest.mark.asyncio
    async def test_log_recording_flow(self):
        import tempfile
        with tempfile.TemporaryDirectory() as tmpdir:
            # 1. 开始录制
            start = self.manager.start_log_recording(tmpdir)
            assert start["success"] is True

            # 2. 模拟写入日志
            self.manager._write_log_file("test log line\n")
            self.manager._write_log_file("another line\n")

            # 3. 停止录制
            stop = self.manager.stop_log_recording()
            assert stop["success"] is True
            assert self.manager._log_file is None


class TestIntegrationErrorRecovery:
    """错误恢复流程"""

    def setup_method(self):
        self.manager = JLinkManager()

    @pytest.mark.asyncio
    async def test_connect_fail_then_succeed(self):
        with patch('pylink.JLink') as mock_jlink:
            mock_instance = Mock()
            mock_instance.opened.return_value = False
            mock_instance.connected.side_effect = [False, True]
            mock_instance.memory_read.return_value = b'\x00' * 256
            mock_jlink.return_value = mock_instance

            with patch.object(self.manager, '_find_rtt_cb', return_value=0):
                result = await self.manager.connect(
                    target="STM32F103C8", retry_attempts=2
                )
                assert result["success"] is True

    @pytest.mark.asyncio
    async def test_operation_after_disconnect(self):
        with patch('pylink.JLink') as mock_jlink:
            mock_instance = Mock()
            mock_instance.opened.return_value = False
            mock_instance.connected.return_value = True
            mock_instance.memory_read.return_value = b'\x00' * 256
            mock_jlink.return_value = mock_instance

            with patch.object(self.manager, '_find_rtt_cb', return_value=0):
                await self.manager.connect(target="STM32F103C8")
                await self.manager.disconnect()

                # 断开后操作应返回 Not connected
                result = await self.manager.read_rtt()
                assert result["success"] is False

                result = await self.manager.write_rtt("test")
                assert result["success"] is False

                result = await self.manager.reset()
                assert result["success"] is False
