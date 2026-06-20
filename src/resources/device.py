"""设备资源 - MCP 资源实现"""

from __future__ import annotations

from typing import Any

from ..core.jlink_manager import JLinkManager


async def get_device_status(manager: JLinkManager) -> dict[str, Any]:
    """获取设备状态信息"""
    return {
        "state": manager.state.value,
        "device_info": (
            manager._device_info_to_dict()
            if manager.state.value == "connected"
            else None
        ),
    }


async def get_rtt_logs(manager: JLinkManager) -> list[dict[str, Any]]:
    """获取 RTT 日志历史"""
    return [
        {
            "timestamp": data.timestamp,
            "channel": data.channel,
            "content": data.content,
            "metadata": data.metadata,
        }
        for data in manager._rtt_buffer[-100:]
    ]


async def get_firmware_metadata(manager: JLinkManager) -> dict[str, Any]:
    """获取固件元数据"""
    if manager.state.value != "connected":
        return {"error": "Not connected"}
    return {
        "target_device": manager.device_info.target_device,
        "core_name": manager.device_info.core_name,
        "core_id": manager.device_info.core_id,
    }
