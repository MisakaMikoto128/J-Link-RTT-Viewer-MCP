"""RTT 处理器测试"""

import time
import pytest
from src.core.rtt_handler import RTTHandler, RTTMessage


class TestRTTHandler:
    """RTTHandler 测试类"""
    
    def setup_method(self):
        self.handler = RTTHandler()
    
    def test_initial_state(self):
        assert self.handler._decoder is not None
        assert self.handler._buffers == {}
    
    def test_decode_raw(self):
        raw = b"Hello, World!"
        decoded = self.handler.decode_raw(raw)
        assert decoded == "Hello, World!"
    
    def test_decode_raw_utf8(self):
        raw = "你好世界".encode("utf-8")
        decoded = self.handler.decode_raw(raw)
        assert decoded == "你好世界"
    
    def test_process_data(self):
        raw = b"[INFO] System initialized"
        msg = self.handler.process_data(0, raw)
        assert msg is not None
        assert msg.channel == 0
        assert msg.content == "[INFO] System initialized"
        assert msg.metadata["level"] == "INFO"
    
    def test_process_data_empty(self):
        assert self.handler.process_data(0, b"") is None
    
    def test_detect_log_level(self):
        assert self.handler.detect_log_level("[ERROR] something") == "ERROR"
        assert self.handler.detect_log_level("[ERR] something") == "ERROR"
        assert self.handler.detect_log_level("[FATAL] something") == "ERROR"
        assert self.handler.detect_log_level("[WARNING] something") == "WARNING"
        assert self.handler.detect_log_level("[WARN] something") == "WARNING"
        assert self.handler.detect_log_level("[DEBUG] something") == "DEBUG"
        assert self.handler.detect_log_level("[DBG] something") == "DEBUG"
        assert self.handler.detect_log_level("[INFO] something") == "INFO"
        assert self.handler.detect_log_level("[TRACE] something") == "TRACE"
        assert self.handler.detect_log_level("no level") == "INFO"
    
    def test_parse_ansi(self):
        result = self.handler.parse_ansi("hello")
        assert result["has_ansi"] is False
        assert result["clean_text"] == "hello"
        
        result = self.handler.parse_ansi("\x1b[31merror\x1b[0m")
        assert result["has_ansi"] is True
        assert result["clean_text"] == "error"
    
    def test_buffer_management(self):
        for i in range(5):
            self.handler.process_data(0, f"msg{i}".encode())
        
        buffer = self.handler.get_buffer(channel=0)
        assert len(buffer) == 5
        assert buffer[0].content == "msg0"
        assert buffer[4].content == "msg4"
    
    def test_buffer_max_size(self):
        handler = RTTHandler(max_buffer_size=3)
        for i in range(5):
            handler.process_data(0, f"msg{i}".encode())
        
        buffer = handler.get_buffer(channel=0)
        assert len(buffer) == 3
        assert buffer[0].content == "msg2"
    
    def test_buffer_limit(self):
        for i in range(10):
            self.handler.process_data(0, f"msg{i}".encode())
        
        buffer = self.handler.get_buffer(channel=0, limit=3)
        assert len(buffer) == 3
    
    def test_clear_buffer(self):
        self.handler.process_data(0, b"msg1")
        self.handler.process_data(1, b"msg2")
        
        self.handler.clear_buffer(channel=0)
        assert self.handler.get_buffer(channel=0) == []
        assert len(self.handler.get_buffer(channel=1)) == 1
        
        self.handler.clear_buffer()
        assert self.handler.get_buffer() == []
    
    def test_reset_session(self):
        self.handler.decode_raw(b"test")
        self.handler.reset_session()
        assert self.handler._decoder is not None
    
    def test_format_message(self):
        msg = RTTMessage(
            timestamp=1000000.0,
            channel=0,
            content="hello",
            raw=b"hello",
            metadata={"level": "INFO"},
        )
        result = self.handler.format_message(msg)
        assert result["channel"] == 0
        assert result["content"] == "hello"
        assert "raw" not in result
        
        result = self.handler.format_message(msg, include_raw=True)
        assert "raw" in result
    
    def test_search(self):
        self.handler.process_data(0, b"[INFO] started")
        self.handler.process_data(0, b"[ERROR] failed")
        self.handler.process_data(0, b"[INFO] stopped")
        
        results = self.handler.search("ERROR")
        assert len(results) == 1
        assert results[0].content == "[ERROR] failed"
        
        results = self.handler.search("INFO")
        assert len(results) == 2
        
        results = self.handler.search("started", case_sensitive=True)
        assert len(results) == 1
    
    def test_search_invalid_regex(self):
        self.handler.process_data(0, b"test")
        results = self.handler.search("[invalid")
        assert results == []
    
    def test_multi_channel(self):
        self.handler.process_data(0, b"ch0 msg")
        self.handler.process_data(1, b"ch1 msg")
        self.handler.process_data(2, b"ch2 msg")
        
        all_msgs = self.handler.get_buffer()
        assert len(all_msgs) == 3
        
        ch1_msgs = self.handler.get_buffer(channel=1)
        assert len(ch1_msgs) == 1
        assert ch1_msgs[0].channel == 1
