/**
 * @file main.c
 * @brief STM32F103C8T6 最小 RTT 测试工程
 * @note 用于测试 J-Link RTT Viewer MCP Server
 */

#include "stm32f1xx_hal.h"
#include "SEGGER_RTT.h"
#include <stdio.h>
#include <string.h>

/* RTT 通道定义 */
#define RTT_CHANNEL_LOG    0    /* 日志通道 */
#define RTT_CHANNEL_DATA   1    /* 数据通道 */
#define RTT_CHANNEL_CMD    2    /* 命令通道 */

/* 全局变量 */
static volatile uint32_t g_tick_counter = 0;
static volatile uint32_t g_loop_counter = 0;

/* 函数声明 */
static void SystemClock_Config(void);
static void Error_Handler(void);

/**
 * @brief 系统滴答定时器中断处理
 */
void SysTick_Handler(void)
{
    HAL_IncTick();
    g_tick_counter++;
}

/**
 * @brief 主函数
 */
int main(void)
{
    /* HAL 初始化 */
    HAL_Init();
    
    /* 配置系统时钟 */
    SystemClock_Config();
    
    /* 初始化 SEGGER_RTT */
    SEGGER_RTT_Init();
    
    /* 输出启动信息 */
    SEGGER_RTT_SetStringOutputColor(RTT_CHANNEL_LOG, RTT_LOG_COLOR_GREEN);
    SEGGER_RTT_printf(RTT_CHANNEL_LOG, "=== STM32F103C8T6 RTT Test ===\r\n");
    SEGGER_RTT_printf(RTT_CHANNEL_LOG, "Build: %s %s\r\n", __DATE__, __TIME__);
    SEGGER_RTT_printf(RTT_CHANNEL_LOG, "MCU: STM32F103C8T6 (Cortex-M3)\r\n");
    SEGGER_RTT_printf(RTT_CHANNEL_LOG, "RTT Channels: 0-15\r\n");
    SEGGER_RTT_SetStringOutputColor(RTT_CHANNEL_LOG, RTT_LOG_COLOR_DEFAULT);
    
    /* 主循环 */
    while (1)
    {
        g_loop_counter++;
        
        /* 每 1000ms 输出一次状态信息 */
        if (g_tick_counter >= 1000)
        {
            g_tick_counter = 0;
            
            /* 输出心跳信息 */
            SEGGER_RTT_printf(RTT_CHANNEL_LOG, "[INFO] Heartbeat: loop=%lu\r\n", g_loop_counter);
            
            /* 输出模拟传感器数据 */
            uint32_t temp = 250 + (g_loop_counter % 100);  /* 25.0 - 35.0 度 */
            uint32_t humi = 400 + (g_loop_counter % 200);  /* 40.0 - 60.0 % */
            
            SEGGER_RTT_printf(RTT_CHANNEL_DATA, "TEMP=%lu.%lu,HUMI=%lu.%lu\r\n", 
                             temp / 10, temp % 10, humi / 10, humi % 10);
            
            /* 检查是否有命令输入 */
            if (SEGGER_RTT_HasKey(RTT_CHANNEL_CMD))
            {
                char cmd_buf[32];
                int cmd_len = SEGGER_RTT_Read(RTT_CHANNEL_CMD, cmd_buf, sizeof(cmd_buf) - 1);
                if (cmd_len > 0)
                {
                    cmd_buf[cmd_len] = '\0';
                    SEGGER_RTT_printf(RTT_CHANNEL_LOG, "[CMD] Received: %s\r\n", cmd_buf);
                    
                    /* 简单命令处理 */
                    if (strcmp(cmd_buf, "ping") == 0)
                    {
                        SEGGER_RTT_printf(RTT_CHANNEL_LOG, "[CMD] pong\r\n");
                    }
                    else if (strcmp(cmd_buf, "version") == 0)
                    {
                        SEGGER_RTT_printf(RTT_CHANNEL_LOG, "[CMD] Version: 1.0.0\r\n");
                    }
                    else if (strcmp(cmd_buf, "reset") == 0)
                    {
                        SEGGER_RTT_printf(RTT_CHANNEL_LOG, "[CMD] Resetting...\r\n");
                        HAL_NVIC_SystemReset();
                    }
                    else
                    {
                        SEGGER_RTT_printf(RTT_CHANNEL_LOG, "[CMD] Unknown command: %s\r\n", cmd_buf);
                    }
                }
            }
        }
        
        /* 短暂延时 */
        HAL_Delay(1);
    }
}

/**
 * @brief 系统时钟配置
 * @note 使用 HSI 8MHz 内部时钟
 */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    
    /* 配置 HSI */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    
    /* 配置系统时钟 */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief 错误处理函数
 */
static void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
        /* 错误状态 */
    }
}