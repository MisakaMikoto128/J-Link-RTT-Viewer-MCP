/**
 * @file main.c
 * @brief STM32F103C8T6 最小 RTT 测试工程
 * @note 参考 SuperWirelessModule 项目的 RTT 配置方式
 */

#include "stm32f1xx_hal.h"
#include "SEGGER_RTT.h"
#include <stdio.h>
#include <string.h>

/* LED 定义 (PC13 = 板载蓝色 LED, 低电平点亮) */
#define LED_PIN        GPIO_PIN_13
#define LED_GPIO_PORT  GPIOC
#define LED_RCC_ENABLE __HAL_RCC_GPIOC_CLK_ENABLE

static void SystemClock_Config(void);
static void Error_Handler(void);
static void LED_Init(void);

static inline void delay_ms(uint32_t ms)
{
    uint32_t start = HAL_GetTick();
    while ((HAL_GetTick() - start) < ms) {}
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    /* 初始化 LED */
    LED_Init();

    /* 初始化 SEGGER_RTT - 参考原始项目的配置方式 */
    SEGGER_RTT_Init();
    SEGGER_RTT_ConfigUpBuffer(0, "RTTUP", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);
    SEGGER_RTT_ConfigDownBuffer(0, "RTTDOWN", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);

    /* 输出启动信息 */
    SEGGER_RTT_WriteString(0, "\r\n=== STM32F103C8T6 RTT Test ===\r\n");

    uint32_t loop = 0;
    uint32_t last_led_tick = HAL_GetTick();
    uint32_t last_log_tick = HAL_GetTick();

    while (1)
    {
        loop++;

        /* LED 翻转 - 每 500ms */
        if ((HAL_GetTick() - last_led_tick) >= 500)
        {
            last_led_tick += 500;
            HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
        }

        /* 每 1000ms 输出一次心跳 */
        if ((HAL_GetTick() - last_log_tick) >= 1000)
        {
            last_log_tick += 1000;
            SEGGER_RTT_printf(0, "Heartbeat: %lu\r\n", loop);
        }

        /* 检查是否有命令输入 */
        if (SEGGER_RTT_HasKey())
        {
            char cmd_buf[32];
            unsigned cmd_len = SEGGER_RTT_Read(0, cmd_buf, sizeof(cmd_buf) - 1);
            if (cmd_len > 0)
            {
                cmd_buf[cmd_len] = '\0';
                SEGGER_RTT_printf(0, "CMD: %s\r\n", cmd_buf);

                if (strcmp(cmd_buf, "ping") == 0)
                {
                    SEGGER_RTT_printf(0, "pong\r\n");
                }
                else if (strcmp(cmd_buf, "reset") == 0)
                {
                    SEGGER_RTT_printf(0, "Resetting...\r\n");
                    HAL_NVIC_SystemReset();
                }
            }
        }

        /* 短暂让出 CPU，避免空转 */
        __WFI();
    }
}

static void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    LED_RCC_ENABLE();
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_SET);
    GPIO_InitStruct.Pin = LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
}

static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

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

static void Error_Handler(void)
{
    __disable_irq();
    while (1) {}
}
