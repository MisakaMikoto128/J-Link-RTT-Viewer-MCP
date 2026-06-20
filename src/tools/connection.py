"""连接工具 - J-Link 连接/断开/设备信息"""

from __future__ import annotations

from typing import Any

from ..core.jlink_manager import JLinkManager


async def connect(
    manager: JLinkManager,
    target: str,
    interface: str = "SWD",
    speed: int = 4000,
    channel: int = 0,
    retry_attempts: int = 3,
) -> dict[str, Any]:
    """连接到 J-Link 和目标 MCU"""
    return await manager.connect(
        target=target,
        interface=interface,
        speed=speed,
        channel=channel,
        retry_attempts=retry_attempts,
    )


async def disconnect(manager: JLinkManager) -> dict[str, Any]:
    """断开 J-Link 连接"""
    return await manager.disconnect()


async def get_device_info(manager: JLinkManager) -> dict[str, Any]:
    """获取当前连接设备的信息"""
    if manager.state.value != "connected":
        return {"success": False, "message": "Not connected"}
    return {
        "success": True,
        "device_info": manager._device_info_to_dict(),
    }
