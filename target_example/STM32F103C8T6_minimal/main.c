/**
 * @file main.c
 * @brief STM32F103C8T6 RTT LED Control Test
 * @note LED on PC13 (active low), controlled via RTT commands
 */

#include "stm32f1xx_hal.h"
#include "SEGGER_RTT.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LED_PIN        GPIO_PIN_13
#define LED_GPIO_PORT  GPIOC
#define LED_RCC_ENABLE __HAL_RCC_GPIOC_CLK_ENABLE

typedef enum { LED_MODE_BLINK, LED_MODE_ON, LED_MODE_OFF } LED_Mode;

static volatile LED_Mode  g_led_mode   = LED_MODE_BLINK;
static volatile uint32_t  g_led_interval_ms = 500;

static void SystemClock_Config(void);
static void Error_Handler(void);
static void LED_Init(void);
static void ProcessCommand(const char *cmd);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    LED_Init();

    SEGGER_RTT_Init();
    SEGGER_RTT_ConfigUpBuffer(0, "RTTUP", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);
    SEGGER_RTT_ConfigDownBuffer(0, "RTTDOWN", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);

    SEGGER_RTT_WriteString(0, "\r\n=== STM32F103C8T6 LED Control ===\r\n");
    SEGGER_RTT_WriteString(0, "Commands: freq <ms>, mode <on|off|blink>, status, ping, reset\r\n");

    uint32_t last_led_tick  = HAL_GetTick();
    uint32_t last_log_tick  = HAL_GetTick();
    uint32_t loop = 0;

    while (1)
    {
        loop++;

        /* LED control */
        if (g_led_mode == LED_MODE_BLINK)
        {
            if ((HAL_GetTick() - last_led_tick) >= g_led_interval_ms)
            {
                last_led_tick += g_led_interval_ms;
                HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
            }
        }
        else if (g_led_mode == LED_MODE_ON)
        {
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_RESET); /* active low = ON */
        }
        else
        {
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_SET);   /* OFF */
        }

        /* Heartbeat every 2s */
        if ((HAL_GetTick() - last_log_tick) >= 2000)
        {
            last_log_tick += 2000;
            SEGGER_RTT_printf(0, "Heartbeat: %lu\r\n", loop);
        }

        /* RTT command input */
        if (SEGGER_RTT_HasKey())
        {
            char cmd_buf[64];
            unsigned cmd_len = SEGGER_RTT_Read(0, cmd_buf, sizeof(cmd_buf) - 1);
            if (cmd_len > 0)
            {
                cmd_buf[cmd_len] = '\0';
                /* strip trailing \r\n */
                while (cmd_len > 0 && (cmd_buf[cmd_len-1] == '\r' || cmd_buf[cmd_len-1] == '\n'))
                    cmd_buf[--cmd_len] = '\0';
                ProcessCommand(cmd_buf);
            }
        }

        __WFI();
    }
}

static void ProcessCommand(const char *cmd)
{
    SEGGER_RTT_printf(0, "CMD: %s\r\n", cmd);

    if (strcmp(cmd, "ping") == 0)
    {
        SEGGER_RTT_printf(0, "pong\r\n");
    }
    else if (strcmp(cmd, "status") == 0)
    {
        const char *mode_str = (g_led_mode == LED_MODE_BLINK) ? "blink" :
                               (g_led_mode == LED_MODE_ON)    ? "on"    : "off";
        SEGGER_RTT_printf(0, "LED mode=%s interval=%lums\r\n", mode_str, g_led_interval_ms);
    }
    else if (strncmp(cmd, "freq ", 5) == 0)
    {
        uint32_t ms = (uint32_t)strtoul(cmd + 5, NULL, 10);
        if (ms < 50) ms = 50;
        if (ms > 5000) ms = 5000;
        g_led_interval_ms = ms;
        g_led_mode = LED_MODE_BLINK;
        SEGGER_RTT_printf(0, "OK freq=%lums\r\n", g_led_interval_ms);
    }
    else if (strncmp(cmd, "mode ", 5) == 0)
    {
        const char *arg = cmd + 5;
        if (strcmp(arg, "on") == 0)
        {
            g_led_mode = LED_MODE_ON;
            SEGGER_RTT_printf(0, "OK mode=on\r\n");
        }
        else if (strcmp(arg, "off") == 0)
        {
            g_led_mode = LED_MODE_OFF;
            SEGGER_RTT_printf(0, "OK mode=off\r\n");
        }
        else if (strcmp(arg, "blink") == 0)
        {
            g_led_mode = LED_MODE_BLINK;
            SEGGER_RTT_printf(0, "OK mode=blink\r\n");
        }
        else
        {
            SEGGER_RTT_printf(0, "ERR unknown mode: %s (use on|off|blink)\r\n", arg);
        }
    }
    else if (strcmp(cmd, "reset") == 0)
    {
        SEGGER_RTT_printf(0, "Resetting...\r\n");
        HAL_NVIC_SystemReset();
    }
    else
    {
        SEGGER_RTT_printf(0, "ERR unknown cmd: %s\r\n", cmd);
    }
}

static void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    LED_RCC_ENABLE();
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_SET);
    GPIO_InitStruct.Pin   = LED_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
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
        Error_Handler();

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
        Error_Handler();
}

static void Error_Handler(void)
{
    __disable_irq();
    while (1) {}
}
