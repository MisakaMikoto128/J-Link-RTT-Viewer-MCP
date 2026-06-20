"""内存工具 - MCU 内存读写"""

from __future__ import annotations

from typing import Any

from ..core.jlink_manager import JLinkManager


async def read_memory(
    manager: JLinkManager,
    address: int,
    size: int,
) -> dict[str, Any]:
    """读取 MCU 内存"""
    return await manager.read_memory(address=address, size=size)


async def write_memory(
    manager: JLinkManager,
    address: int,
    data: str,
    is_hex: bool = True,
) -> dict[str, Any]:
    """写入 MCU 内存"""
    return await manager.write_memory(address=address, data=data, is_hex=is_hex)
