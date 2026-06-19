"""资源模块 - MCP 资源实现"""

from .device import get_device_status, get_rtt_logs, get_firmware_metadata

__all__ = ["get_device_status", "get_rtt_logs", "get_firmware_metadata"]