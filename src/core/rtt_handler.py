"""RTT 处理器 - 封装 RTT 读写操作

提供 RTT 数据的结构化解析、多通道管理和缓冲功能。
"""

from __future__ import annotations

import codecs
import logging
import re
import time
from dataclasses import dataclass, field
from typing import Any

logger = logging.getLogger(__name__)


@dataclass
class RTTMessage:
    """单条 RTT 消息"""
    timestamp: float
    channel: int
    content: str
    raw: bytes
    metadata: dict[str, Any] = field(default_factory=dict)


class RTTHandler:
    """RTT 数据处理器

    功能：
    - RTT 数据解码和结构化
    - 多通道数据管理
    - ANSI 颜色代码解析
    - 日志级别检测
    - 数据缓冲和历史记录
    """

    def __init__(self, encoding: str = "utf-8", max_buffer_size: int = 10000) -> None:
        self._encoding = encoding
        self._decoder: codecs.IncrementalDecoder | None = None
        self._max_buffer_size = max_buffer_size
        self._buffers: dict[int, list[RTTMessage]] = {}
        self._ansi_pattern = re.compile(r'\x1b\[[0-9;]*[a-zA-Z]')

        self._reset_decoder()
        logger.info(f"RTTHandler initialized (encoding={encoding})")

    def _reset_decoder(self) -> None:
        """重置文本解码器"""
        try:
            self._decoder = codecs.getincrementaldecoder(self._encoding)(errors="replace")
        except LookupError:
            logger.warning(f"Unknown encoding {self._encoding}, falling back to utf-8")
            self._encoding = "utf-8"
            self._decoder = codecs.getincrementaldecoder("utf-8")(errors="replace")

    def decode_raw(self, raw_data: bytes) -> str:
        """解码原始字节为文本"""
        if self._decoder is None:
            self._reset_decoder()
        return self._decoder.decode(raw_data)  # type: ignore

    def reset_session(self) -> None:
        """重置解码会话（用于重置后重新开始）"""
        self._reset_decoder()
        logger.info("RTT decoder session reset")

    def parse_ansi(self, text: str) -> dict[str, Any]:
        """解析 ANSI 颜色代码，提取元数据

        Returns:
            包含 ANSI 信息的元数据字典
        """
        metadata: dict[str, Any] = {
            "has_ansi": False,
            "ansi_codes": [],
            "clean_text": text,
        }

        codes = self._ansi_pattern.findall(text)
        if codes:
            metadata["has_ansi"] = True
            metadata["ansi_codes"] = codes
            metadata["clean_text"] = self._ansi_pattern.sub("", text)

        return metadata

    def detect_log_level(self, text: str) -> str:
        """检测日志级别

        Returns:
            日志级别字符串
        """
        upper = text.upper()
        if "[ERROR]" in upper or "[ERR]" in upper or "[FATAL]" in upper:
            return "ERROR"
        elif "[WARNING]" in upper or "[WARN]" in upper:
            return "WARNING"
        elif "[DEBUG]" in upper or "[DBG]" in upper:
            return "DEBUG"
        elif "[INFO]" in upper or "[INF]" in upper:
            return "INFO"
        elif "[TRACE]" in upper or "[TRC]" in upper:
            return "TRACE"
        return "INFO"

    def process_data(
        self, channel: int, raw_data: bytes
    ) -> RTTMessage | None:
        """处理原始 RTT 数据

        Args:
            channel: RTT 通道号
            raw_data: 原始字节数据

        Returns:
            处理后的 RTT 消息，空数据返回 None
        """
        if not raw_data:
            return None

        content = self.decode_raw(raw_data)
        if not content:
            return None

        ansi_info = self.parse_ansi(content)
        clean_text = ansi_info["clean_text"]
        level = self.detect_log_level(clean_text)

        metadata: dict[str, Any] = {
            "level": level,
            "has_ansi": ansi_info["has_ansi"],
            "source": f"ch{channel}",
        }

        msg = RTTMessage(
            timestamp=time.time(),
            channel=channel,
            content=clean_text,
            raw=raw_data,
            metadata=metadata,
        )

        self._add_to_buffer(channel, msg)
        return msg

    def _add_to_buffer(self, channel: int, msg: RTTMessage) -> None:
        """添加消息到通道缓冲区"""
        if channel not in self._buffers:
            self._buffers[channel] = []

        buffer = self._buffers[channel]
        buffer.append(msg)

        # 限制缓冲区大小
        if len(buffer) > self._max_buffer_size:
            self._buffers[channel] = buffer[-self._max_buffer_size:]

    def get_buffer(self, channel: int | None = None, limit: int = 100) -> list[RTTMessage]:
        """获取缓冲区消息

        Args:
            channel: 通道号，None 返回所有通道
            limit: 最大返回数量

        Returns:
            消息列表
        """
        if channel is not None:
            msgs = self._buffers.get(channel, [])
            return msgs[-limit:]

        all_msgs = []
        for ch_msgs in self._buffers.values():
            all_msgs.extend(ch_msgs)
        all_msgs.sort(key=lambda m: m.timestamp)
        return all_msgs[-limit:]

    def clear_buffer(self, channel: int | None = None) -> None:
        """清空缓冲区

        Args:
            channel: 通道号，None 清空所有通道
        """
        if channel is not None:
            self._buffers.pop(channel, None)
        else:
            self._buffers.clear()

    def format_message(self, msg: RTTMessage, include_raw: bool = False) -> dict[str, Any]:
        """格式化消息为字典

        Args:
            msg: RTT 消息
            include_raw: 是否包含原始数据

        Returns:
            格式化的字典
        """
        result: dict[str, Any] = {
            "timestamp": msg.timestamp,
            "timestamp_iso": time.strftime("%Y-%m-%dT%H:%M:%S", time.localtime(msg.timestamp)),
            "channel": msg.channel,
            "content": msg.content,
            "metadata": msg.metadata,
        }
        if include_raw:
            result["raw"] = msg.raw.hex()
        return result

    def search(
        self,
        pattern: str,
        channel: int | None = None,
        limit: int = 100,
        case_sensitive: bool = False,
    ) -> list[RTTMessage]:
        """在缓冲区中搜索消息

        Args:
            pattern: 搜索模式（支持正则表达式）
            channel: 通道号，None 搜索所有通道
            limit: 最大返回数量
            case_sensitive: 是否区分大小写

        Returns:
            匹配的消息列表
        """
        flags = 0 if case_sensitive else re.IGNORECASE
        try:
            regex = re.compile(pattern, flags)
        except re.error:
            return []

        messages = self.get_buffer(channel=channel, limit=10000)
        results = [msg for msg in messages if regex.search(msg.content)]
        return results[-limit:]
