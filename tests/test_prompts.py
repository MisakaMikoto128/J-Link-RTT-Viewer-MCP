"""提示模板测试"""

from src.prompts.templates import (
    get_debugging_prompt,
    get_error_diagnosis_prompt,
    get_flash_wizard_prompt,
)


class TestPromptTemplates:
    """提示模板测试"""
    
    def test_debugging_prompt(self):
        result = get_debugging_prompt()
        assert isinstance(result, str)
        assert "调试指南" in result
        assert "连接问题" in result
    
    def test_error_diagnosis_prompt(self):
        result = get_error_diagnosis_prompt("Connection timeout")
        assert isinstance(result, str)
        assert "Connection timeout" in result
        assert "诊断步骤" in result
    
    def test_flash_wizard_prompt(self):
        result = get_flash_wizard_prompt()
        assert isinstance(result, str)
        assert "烧录向导" in result
        assert "步骤" in result
