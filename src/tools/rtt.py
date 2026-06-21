"""RTT 工具 - RTT 数据读写和通道管理"""

from __future__ import annotations

from typing import Any

from ..core.jlink_manager import JLinkManager


async def read_rtt(
    manager: JLinkManager,
    channels: list[int] | None = None,
    max_size: int = 4096,
) -> dict[str, Any]:
    """读取 RTT 日志数据"""
    return await manager.read_rtt(
        channels=channels,
        max_size=max_size,
    )


async def write_rtt(
    manager: JLinkManager,
    data: str,
    channel: int | None = None,
    is_hex: bool = False,
) -> dict[str, Any]:
    """向 RTT 通道写入数据"""
    return await manager.write_rtt(
        data=data,
        channel=channel,
        is_hex=is_hex,
    )


async def set_rtt_channel(
    manager: JLinkManager,
    channel: int,
) -> dict[str, Any]:
    """设置默认 RTT 通道"""
    return await manager.set_rtt_channel(channel)
