"""安装脚本"""

from setuptools import setup, find_packages

setup(
    name="jlink-rtt-mcp",
    version="0.1.0",
    description="MCP Server for J-Link RTT Viewer - AI-powered STM32 debugging",
    long_description=open("README.md", encoding="utf-8").read(),
    long_description_content_type="text/markdown",
    author="J-Link RTT Viewer MCP Contributors",
    python_requires=">=3.10",
    packages=find_packages(),
    install_requires=[
        "mcp[cli]>=1.2.0",
        "pylink-square==1.6.0",
        "anyio>=4.0.0",
    ],
    extras_require={
        "dev": [
            "pytest>=7.0.0",
            "pytest-asyncio>=0.23.0",
            "pytest-mock>=3.10.0",
            "mypy>=1.0.0",
            "ruff>=0.1.0",
        ],
    },
    entry_points={
        "console_scripts": [
            "jlink-rtt-mcp=src.server:main",
        ],
    },
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Topic :: Software Development :: Debuggers",
        "Topic :: Software Development :: Embedded Systems",
    ],
)