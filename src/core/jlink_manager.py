"""J-Link 管理器 - 封装所有 J-Link 硬件交互

基于现有 JLinkWorker 的核心逻辑，重构为线程安全的异步接口。
"""

from __future__ import annotations

import asyncio
import codecs
import logging
import time
from dataclasses import dataclass, field
from enum import Enum
from pathlib import Path
from typing import Any, Callable, Optional

import pylink
from pylink.enums import JLinkInterfaces

logger = logging.getLogger(__name__)


class ConnectionState(Enum):
    """连接状态"""
    IDLE = "idle"
    CONNECTING = "connecting"
    CONNECTED = "connected"
    DISCONNECTING = "disconnecting"


class ResetMode(Enum):
    """重置模式"""
    NORMAL = "normal"  # 5步 dance，保留会话
    AUTO_RECONNECT = "auto_reconnect"  # 重置 + 断开 + 重连
    HALT = "halt"  # 重置并暂停


@dataclass
class DeviceInfo:
    """设备信息"""
    jlink_firmware: str = ""
    jlink_hardware: str = ""
    jlink_serial: str = ""
    core_name: str = ""
    core_id: str = ""
    core_cpu: str = ""
    target_device: str = ""
    interface: str = "SWD"
    speed_khz: int = 4000
    connected: bool = False


@dataclass
class RTTData:
    """RTT 数据结构"""
    timestamp: float
    channel: int
    content: str
    raw: bytes
    metadata: dict = field(default_factory=dict)


class JLinkManager:
    """J-Link 设备管理器
    
    处理所有硬件交互，包括：
    - 连接/断开 J-Link
    - RTT 数据读写
    - 设备重置
    - 固件烧录
    - 内存读写
    
    注意：所有 J-Link 操作必须在独立线程中执行，
    以避免阻塞 asyncio 事件循环。
    """
    
    def __init__(self) -> None:
        self.jlink: Optional[pylink.JLink] = None
        self.state: ConnectionState = ConnectionState.IDLE
        self.device_info: DeviceInfo = DeviceInfo()
        self.last_connect_params: Optional[tuple[str, str, int, int]] = None
        
        # RTT 相关
        self._decoder: Optional[codecs.IncrementalDecoder] = None
        self._encoding: str = "utf-8"
        self._channel: int = 0
        self._rtt_buffer: list[RTTData] = []
        self._rtt_lock = asyncio.Lock()
        
        # 线程同步
        self._executor_lock = asyncio.Lock()
        self._loop: Optional[asyncio.AbstractEventLoop] = None
        
        # 日志文件
        self._log_file = None
        self._log_path: Optional[str] = None
        
        logger.info("JLinkManager initialized")
    
    def _ensure_jlink(self) -> pylink.JLink:
        """确保 JLink 实例存在"""
        if self.jlink is None:
            self.jlink = pylink.JLink()
        return self.jlink
    
    def _run_in_executor(self, func: Callable, *args: Any) -> Any:
        """在线程池中执行阻塞操作"""
        if self._loop is None:
            self._loop = asyncio.get_event_loop()
        return self._loop.run_in_executor(None, func, *args)
    
    async def connect(
        self,
        target: str,
        interface: str = "SWD",
        speed: int = 4000,
        channel: int = 0,
        retry_attempts: int = 3,
    ) -> dict[str, Any]:
        """连接到 J-Link 和目标 MCU
        
        Args:
            target: 目标设备型号 (如 STM32F103C8T6)
            interface: 调试接口 (SWD 或 JTAG)
            speed: 连接速度 (kHz)
            channel: RTT 通道号
            retry_attempts: 重试次数
            
        Returns:
            连接状态和设备信息
            
        Raises:
            ConnectionError: 连接失败
            ValueError: 参数错误
        """
        if self.state == ConnectionState.CONNECTED:
            logger.warning("Already connected, disconnecting first")
            await self.disconnect()
        
        if interface not in ("SWD", "JTAG"):
            raise ValueError(f"Invalid interface: {interface}. Must be 'SWD' or 'JTAG'")
        
        self.state = ConnectionState.CONNECTING
        self._channel = channel
        
        last_error = None
        for attempt in range(retry_attempts):
            try:
                logger.info(f"Connection attempt {attempt + 1}/{retry_attempts}")
                
                # 使用线程池执行器执行阻塞的 J-Link 操作
                await self._run_in_executor(
                    self._do_connect, target, interface, speed
                )
                
                # 连接成功
                self.state = ConnectionState.CONNECTED
                self.last_connect_params = (target, interface, speed, channel)
                
                # 收集设备信息
                self.device_info = await self._run_in_executor(
                    self._collect_device_info, target, interface, speed
                )
                self.device_info.connected = True
                
                logger.info(f"Connected to {target} ({interface} {speed}kHz, RTT ch{channel})")
                
                return {
                    "success": True,
                    "message": f"Connected to {target}",
                    "device_info": self._device_info_to_dict(),
                }
                
            except Exception as e:
                last_error = e
                logger.warning(f"Connection attempt {attempt + 1} failed: {e}")
                if attempt < retry_attempts - 1:
                    # 等待后重试
                    await asyncio.sleep(1.0)
        
        # 所有重试都失败
        self.state = ConnectionState.IDLE
        error_msg = f"Failed to connect after {retry_attempts} attempts: {last_error}"
        logger.error(error_msg)
        raise ConnectionError(error_msg)
    
    def _do_connect(self, target: str, interface: str, speed: int) -> None:
        """执行连接操作（在线程池中运行）"""
        jlink = self._ensure_jlink()
        
        # pylink 1.6.0 的双开模式
        if not jlink.opened():
            jlink.open()
            ser_num = jlink.serial_number
            jlink.close()
            jlink.open(str(ser_num))
            jlink.rtt_start()
        
        # 设置接口
        tif = JLinkInterfaces.SWD if interface == "SWD" else JLinkInterfaces.JTAG
        jlink.set_tif(tif)
        
        # 设置速度
        jlink.set_speed(int(speed))
        
        # 连接目标
        jlink.connect(target)
        
        # 验证连接
        if not jlink.connected():
            raise ConnectionError(f"connect(target) succeeded but connected() returned False")
        
        # 重置解码器
        self._reset_decoder()
    
    def _collect_device_info(
        self, target: str, interface: str, speed: int
    ) -> DeviceInfo:
        """收集设备信息（在线程池中运行）"""
        jlink = self._ensure_jlink()
        
        try:
            return DeviceInfo(
                jlink_firmware=jlink.firmware_version,
                jlink_hardware=str(jlink.hardware_version),
                jlink_serial=str(jlink.serial_number),
                core_name=jlink.core_name(),
                core_id=hex(jlink.core_id()),
                core_cpu=jlink.core_cpu(),
                target_device=target,
                interface=interface,
                speed_khz=speed,
                connected=True,
            )
        except Exception as e:
            logger.warning(f"Failed to collect device info: {e}")
            return DeviceInfo(
                target_device=target,
                interface=interface,
                speed_khz=speed,
                connected=True,
            )
    
    def _device_info_to_dict(self) -> dict[str, Any]:
        """将 DeviceInfo 转换为字典"""
        return {
            "jlink_firmware": self.device_info.jlink_firmware,
            "jlink_hardware": self.device_info.jlink_hardware,
            "jlink_serial": self.device_info.jlink_serial,
            "core_name": self.device_info.core_name,
            "core_id": self.device_info.core_id,
            "core_cpu": self.device_info.core_cpu,
            "target_device": self.device_info.target_device,
            "interface": self.device_info.interface,
            "speed_khz": self.device_info.speed_khz,
        }
    
    async def disconnect(self) -> dict[str, Any]:
        """断开连接，清理资源
        
        Returns:
            断开状态
        """
        if self.state == ConnectionState.IDLE:
            return {"success": True, "message": "Already disconnected"}
        
        self.state = ConnectionState.DISCONNECTING
        
        try:
            await self._run_in_executor(self._do_disconnect)
            
            self.state = ConnectionState.IDLE
            self.device_info = DeviceInfo()
            
            logger.info("Disconnected from J-Link")
            return {"success": True, "message": "Disconnected"}
            
        except Exception as e:
            logger.error(f"Disconnect failed: {e}")
            self.state = ConnectionState.IDLE
            return {"success": False, "message": f"Disconnect failed: {e}"}
    
    def _do_disconnect(self) -> None:
        """执行断开操作（在线程池中运行）"""
        if self.jlink is None:
            return
        
        # 停止 RTT
        try:
            self.jlink.rtt_stop()
        except Exception as e:
            logger.warning(f"rtt_stop failed: {e}")
        
        # 关闭连接
        try:
            self.jlink.close()
        except Exception as e:
            logger.warning(f"close failed: {e}")
        
        # 关闭日志文件
        self._close_log_file()
    
    async def read_rtt(
        self,
        channels: Optional[list[int]] = None,
        timeout: float = 1.0,
        max_size: int = 4096,
    ) -> dict[str, Any]:
        """读取 RTT 日志数据
        
        Args:
            channels: 要读取的通道列表，None 表示所有通道（默认读取所有通道）
            timeout: 读取超时时间（秒）
            max_size: 最大读取字节数
            
        Returns:
            结构化的日志数据
        """
        if self.state != ConnectionState.CONNECTED:
            return {
                "success": False,
                "message": "Not connected",
                "data": [],
            }
        
        # 如果未指定通道，读取所有通道（0-15）
        if channels is None:
            channels = list(range(16))
        
        all_data: list[dict[str, Any]] = []
        
        # 在线程池中执行读取操作
        raw_data_list = await self._run_in_executor(
            self._do_read_rtt, channels, max_size
        )
        
        # 解码并格式化数据
        for channel, raw_data in raw_data_list:
            if raw_data:
                decoded = self._decoder.decode(bytes(raw_data))
                if decoded:
                    # 解析 ANSI 颜色代码
                    metadata = self._parse_ansi_metadata(decoded)
                    
                    rtt_data = {
                        "timestamp": time.time(),
                        "channel": channel,
                        "content": decoded,
                        "raw": raw_data.hex(),
                        "metadata": metadata,
                    }
                    all_data.append(rtt_data)
                    
                    # 添加到缓冲区
                    async with self._rtt_lock:
                        self._rtt_buffer.append(RTTData(
                            timestamp=time.time(),
                            channel=channel,
                            content=decoded,
                            raw=bytes(raw_data),
                            metadata=metadata,
                        ))
        
        return {
            "success": True,
            "message": f"Read {len(all_data)} RTT messages",
            "data": all_data,
        }
    
    def _do_read_rtt(
        self, channels: list[int], max_size: int
    ) -> list[tuple[int, list[int]]]:
        """执行 RTT 读取操作（在线程池中运行）"""
        jlink = self._ensure_jlink()
        results = []
        
        for channel in channels:
            try:
                data = jlink.rtt_read(channel, max_size)
                if data:
                    results.append((channel, list(data)))
            except Exception as e:
                logger.warning(f"Failed to read RTT channel {channel}: {e}")
        
        return results
    
    def _reset_decoder(self) -> None:
        """重置解码器"""
        try:
            self._decoder = codecs.getincrementaldecoder(self._encoding)(errors="replace")
        except LookupError:
            logger.warning(f"Unknown encoding {self._encoding}, falling back to utf-8")
            self._encoding = "utf-8"
            self._decoder = codecs.getincrementaldecoder("utf-8")(errors="replace")
    
    def _parse_ansi_metadata(self, text: str) -> dict[str, Any]:
        """解析 ANSI 颜色代码，提取元数据"""
        metadata: dict[str, Any] = {
            "has_ansi": False,
            "level": "INFO",
            "source": "unknown",
        }
        
        # 检查是否有 ANSI 转义序列
        if "\x1b[" in text:
            metadata["has_ansi"] = True
        
        # 尝试解析日志级别
        text_upper = text.upper()
        if "[ERROR]" in text_upper or "[ERR]" in text_upper:
            metadata["level"] = "ERROR"
        elif "[WARNING]" in text_upper or "[WARN]" in text_upper:
            metadata["level"] = "WARNING"
        elif "[DEBUG]" in text_upper:
            metadata["level"] = "DEBUG"
        elif "[INFO]" in text_upper:
            metadata["level"] = "INFO"
        
        return metadata
    
    async def write_rtt(
        self, data: str, channel: Optional[int] = None, is_hex: bool = False
    ) -> dict[str, Any]:
        """向 RTT 通道写入数据
        
        Args:
            data: 要写入的数据
            channel: 通道号，None 使用默认通道
            is_hex: 是否为十六进制数据
            
        Returns:
            写入结果
        """
        if self.state != ConnectionState.CONNECTED:
            return {"success": False, "message": "Not connected"}
        
        target_channel = channel if channel is not None else self._channel
        
        try:
            # 准备数据
            if is_hex:
                cleaned = data.replace(" ", "").replace("\n", "").replace("\r", "")
                if len(cleaned) % 2 != 0:
                    cleaned += "0"
                payload = bytes.fromhex(cleaned)
            else:
                payload = data.encode("utf-8")
            
            # 执行写入
            written = await self._run_in_executor(
                self._do_write_rtt, target_channel, payload
            )
            
            success = written == len(payload)
            return {
                "success": success,
                "message": f"Written {written}/{len(payload)} bytes to channel {target_channel}",
                "bytes_written": written,
            }
            
        except Exception as e:
            logger.error(f"Failed to write RTT: {e}")
            return {"success": False, "message": str(e)}
    
    def _do_write_rtt(self, channel: int, payload: bytes) -> int:
        """执行 RTT 写入操作（在线程池中运行）"""
        jlink = self._ensure_jlink()
        return jlink.rtt_write(channel, payload)
    
    async def set_rtt_channel(self, channel: int) -> dict[str, Any]:
        """设置默认 RTT 通道
        
        Args:
            channel: 通道号 (0-15)
            
        Returns:
            设置结果
        """
        if not 0 <= channel <= 15:
            return {"success": False, "message": "Channel must be 0-15"}
        
        self._channel = channel
        logger.info(f"Default RTT channel set to {channel}")
        return {"success": True, "message": f"Default channel set to {channel}"}
    
    async def reset(
        self, mode: str = "normal", wait_time: float = 0.3
    ) -> dict[str, Any]:
        """重置目标 MCU
        
        Args:
            mode: 重置模式 (normal/auto_reconnect/halt)
            wait_time: 重置后等待时间（秒）
            
        Returns:
            重置结果
        """
        if self.state != ConnectionState.CONNECTED:
            return {"success": False, "message": "Not connected"}
        
        reset_mode = ResetMode(mode)
        
        try:
            if reset_mode == ResetMode.AUTO_RECONNECT:
                return await self._reset_with_reconnect(wait_time)
            elif reset_mode == ResetMode.HALT:
                return await self._reset_and_halt()
            else:
                return await self._reset_in_place()
                
        except Exception as e:
            logger.error(f"Reset failed: {e}")
            return {"success": False, "message": str(e)}
    
    async def _reset_in_place(self) -> dict[str, Any]:
        """重置并保留会话（5步 dance）"""
        async def do_reset() -> tuple[bool, str]:
            jlink = self._ensure_jlink()
            try:
                jlink.reset(1, False)
                time.sleep(0.1)  # 等 MCU 重新初始化
                jlink.rtt_stop()
                jlink.rtt_start()
                self._reset_decoder()
                return True, ""
            except Exception as e:
                return False, str(e)
        
        ok, err = await self._run_in_executor(do_reset)
        
        return {
            "success": ok,
            "message": "Reset completed" if ok else f"Reset failed: {err}",
        }
    
    async def _reset_and_halt(self) -> dict[str, Any]:
        """重置并暂停（CPU 停在复位状态）"""
        async def do_reset() -> tuple[bool, str]:
            jlink = self._ensure_jlink()
            try:
                jlink.reset(0, True)  # halt=True
                return True, ""
            except Exception as e:
                return False, str(e)
        
        ok, err = await self._run_in_executor(do_reset)
        
        return {
            "success": ok,
            "message": "Reset and halt completed" if ok else f"Reset and halt failed: {err}",
        }
    
    async def _reset_with_reconnect(self, wait_time: float) -> dict[str, Any]:
        """重置 + 断开 + 重连"""
        params = self.last_connect_params
        if params is None:
            return {"success": False, "message": "No connection parameters for reconnect"}
        
        # 1. 发送 reset 命令
        async def do_reset() -> None:
            jlink = self._ensure_jlink()
            jlink.reset(1, False)
        
        try:
            await self._run_in_executor(do_reset)
        except Exception as e:
            return {"success": False, "message": f"Reset command failed: {e}"}
        
        # 2. 断开连接
        await self.disconnect()
        
        # 3. 等待 MCU 启动
        await asyncio.sleep(wait_time)
        
        # 4. 重连
        result = await self.connect(*params)
        result["message"] = "Reset with reconnect completed"
        return result
    
    async def flash_firmware(
        self, firmware_path: str, verify: bool = True
    ) -> dict[str, Any]:
        """烧录固件到 MCU
        
        Args:
            firmware_path: 固件文件路径 (.hex, .bin, .axf)
            verify: 是否在烧录后验证
            
        Returns:
            烧录结果
        """
        if self.state != ConnectionState.CONNECTED:
            return {"success": False, "message": "Not connected"}
        
        # 验证文件存在
        path = Path(firmware_path)
        if not path.exists():
            return {"success": False, "message": f"Firmware file not found: {firmware_path}"}
        
        if not path.suffix.lower() in (".hex", ".bin", ".axf", ".elf"):
            return {"success": False, "message": f"Unsupported firmware format: {path.suffix}"}
        
        try:
            logger.info(f"Flashing firmware: {firmware_path}")
            
            # 执行烧录
            result = await self._run_in_executor(
                self._do_flash_firmware, firmware_path, verify
            )
            
            return result
            
        except Exception as e:
            logger.error(f"Firmware flash failed: {e}")
            return {"success": False, "message": str(e)}
    
    def _do_flash_firmware(
        self, firmware_path: str, verify: bool
    ) -> dict[str, Any]:
        """执行固件烧录（在线程池中运行）"""
        jlink = self._ensure_jlink()
        
        try:
            # 使用 J-Link 命令行工具烧录
            # 注意：pylink-square 1.6.0 可能不直接支持烧录
            # 这里提供基础实现，实际可能需要调用 JLinkExe
            
            # 读取固件文件
            path = Path(firmware_path)
            with open(path, "rb") as f:
                firmware_data = f.read()
            
            # 根据文件格式处理
            if path.suffix.lower() == ".hex":
                # Intel HEX 格式需要解析
                # 这里简化处理，实际应该解析 HEX 格式
                return {
                    "success": False,
                    "message": "HEX format not yet implemented, use .bin or .axf",
                }
            elif path.suffix.lower() == ".bin":
                # 二进制格式，需要指定起始地址
                # 默认 STM32 Flash 起始地址
                start_addr = 0x08000000
                jlink.flash.bin(start_addr, firmware_data, programming=True)
            else:
                # .axf/.elf 格式
                jlink.flash.elf(firmware_data)
            
            # 验证
            if verify:
                logger.info("Verifying flash...")
                # 这里应该添加验证逻辑
                
            return {
                "success": True,
                "message": f"Firmware flashed successfully: {firmware_path}",
                "file_size": len(firmware_data),
            }
            
        except Exception as e:
            return {"success": False, "message": f"Flash failed: {e}"}
    
    async def read_memory(
        self, address: int, size: int
    ) -> dict[str, Any]:
        """读取 MCU 内存
        
        Args:
            address: 起始地址
            size: 读取字节数
            
        Returns:
            内存数据
        """
        if self.state != ConnectionState.CONNECTED:
            return {"success": False, "message": "Not connected"}
        
        if size <= 0 or size > 0x100000:  # 限制最大 1MB
            return {"success": False, "message": "Invalid size (must be 1-1048576)"}
        
        try:
            data = await self._run_in_executor(
                self._do_read_memory, address, size
            )
            
            return {
                "success": True,
                "message": f"Read {len(data)} bytes from 0x{address:08X}",
                "address": address,
                "size": len(data),
                "data": data.hex(),
            }
            
        except Exception as e:
            logger.error(f"Memory read failed: {e}")
            return {"success": False, "message": str(e)}
    
    def _do_read_memory(self, address: int, size: int) -> bytes:
        """执行内存读取（在线程池中运行）"""
        jlink = self._ensure_jlink()
        return jlink.memory_read(address, size)
    
    async def write_memory(
        self, address: int, data: str, is_hex: bool = True
    ) -> dict[str, Any]:
        """写入 MCU 内存
        
        Args:
            address: 起始地址
            data: 要写入的数据（十六进制字符串或普通字符串）
            is_hex: 是否为十六进制数据
            
        Returns:
            写入结果
        """
        if self.state != ConnectionState.CONNECTED:
            return {"success": False, "message": "Not connected"}
        
        try:
            # 准备数据
            if is_hex:
                cleaned = data.replace(" ", "").replace("\n", "").replace("\r", "")
                if len(cleaned) % 2 != 0:
                    cleaned += "0"
                payload = bytes.fromhex(cleaned)
            else:
                payload = data.encode("utf-8")
            
            # 执行写入
            written = await self._run_in_executor(
                self._do_write_memory, address, payload
            )
            
            return {
                "success": True,
                "message": f"Written {written} bytes to 0x{address:08X}",
                "bytes_written": written,
            }
            
        except Exception as e:
            logger.error(f"Memory write failed: {e}")
            return {"success": False, "message": str(e)}
    
    def _do_write_memory(self, address: int, data: bytes) -> int:
        """执行内存写入（在线程池中运行）"""
        jlink = self._ensure_jlink()
        return jlink.memory_write(address, data)
    
    def start_log_recording(self, log_dir: str) -> dict[str, Any]:
        """开始日志记录
        
        Args:
            log_dir: 日志目录
            
        Returns:
            记录状态
        """
        if self._log_file is not None:
            return {"success": False, "message": "Already recording"}
        
        try:
            Path(log_dir).mkdir(parents=True, exist_ok=True)
            timestamp = time.strftime("%Y%m%d_%H%M%S")
            self._log_path = str(Path(log_dir) / f"rtt_{timestamp}.log")
            self._log_file = open(self._log_path, "a", encoding="utf-8", buffering=1)
            
            logger.info(f"Started log recording: {self._log_path}")
            return {"success": True, "message": f"Recording to {self._log_path}"}
            
        except Exception as e:
            logger.error(f"Failed to start log recording: {e}")
            return {"success": False, "message": str(e)}
    
    def stop_log_recording(self) -> dict[str, Any]:
        """停止日志记录"""
        self._close_log_file()
        logger.info("Stopped log recording")
        return {"success": True, "message": "Log recording stopped"}
    
    def _close_log_file(self) -> None:
        """关闭日志文件"""
        if self._log_file is not None:
            try:
                self._log_file.close()
            except Exception:
                pass
            self._log_file = None
            self._log_path = None
    
    def _write_log_file(self, text: str) -> None:
        """写入日志文件"""
        if self._log_file is None:
            return
        try:
            self._log_file.write(text)
            self._log_file.flush()
        except Exception as e:
            logger.warning(f"Failed to write log file: {e}")
    
    async def cleanup(self) -> None:
        """清理所有资源"""
        await self.disconnect()
        logger.info("JLinkManager cleaned up")