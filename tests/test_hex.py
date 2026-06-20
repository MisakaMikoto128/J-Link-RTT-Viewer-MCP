"""HEX 格式解析测试"""

import pytest
from src.core.jlink_manager import JLinkManager


def _make_hex_line(byte_count: int, address: int, record_type: int, data: bytes) -> str:
    """创建正确的 Intel HEX 行"""
    parts = f"{byte_count:02X}{address:04X}{record_type:02X}"
    data_hex = data.hex().upper()
    all_bytes = bytes.fromhex(parts + data_hex)
    checksum = (~sum(all_bytes) + 1) & 0xFF
    return f":{parts}{data_hex}{checksum:02X}"


class TestIntelHexParsing:
    """Intel HEX 格式解析测试"""
    
    def test_parse_simple_hex(self):
        line1 = _make_hex_line(16, 0x0000, 0x00, bytes(range(16)))
        hex_content = (line1 + "\n:00000001FF\n").encode()
        result = JLinkManager._parse_intel_hex(hex_content)
        assert len(result) == 1
        addr, data = result[0]
        assert addr == 0x0000
        assert len(data) == 16
    
    def test_parse_hex_with_extended_address(self):
        line1 = _make_hex_line(2, 0x0000, 0x04, b"\x08\x00")
        line2 = _make_hex_line(16, 0x0000, 0x00, bytes(range(16)))
        hex_content = (line1 + "\n" + line2 + "\n:00000001FF\n").encode()
        result = JLinkManager._parse_intel_hex(hex_content)
        assert len(result) == 1
        addr, data = result[0]
        assert addr == 0x08000000
    
    def test_parse_empty_hex(self):
        result = JLinkManager._parse_intel_hex(b"")
        assert result == []
    
    def test_parse_hex_records(self):
        line1 = _make_hex_line(3, 0x0000, 0x00, b"\x01\x02\x03")
        line2 = _make_hex_line(3, 0x0010, 0x00, b"\x04\x05\x06")
        hex_content = (line1 + "\n" + line2 + "\n:00000001FF\n").encode()
        result = JLinkManager._parse_intel_hex(hex_content)
        assert len(result) == 2
        assert result[0][0] == 0x0000
        assert result[1][0] == 0x0010
