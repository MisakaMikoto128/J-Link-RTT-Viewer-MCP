"""烧录工具 - 固件烧录"""

from __future__ import annotations

from typing import Any

from ..core.jlink_manager import JLinkManager


async def flash_firmware(
    manager: JLinkManager,
    firmware_path: str,
    verify: bool = True,
) -> dict[str, Any]:
    """烧录固件到 MCU"""
    return await manager.flash_firmware(
        firmware_path=firmware_path,
        verify=verify,
    )
