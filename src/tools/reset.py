"""重置工具 - MCU 重置"""

from __future__ import annotations

from typing import Any

from ..core.jlink_manager import JLinkManager


async def reset_device(
    manager: JLinkManager,
    mode: str = "normal",
    wait_time: float = 0.3,
) -> dict[str, Any]:
    """重置目标 MCU"""
    return await manager.reset(mode=mode, wait_time=wait_time)
