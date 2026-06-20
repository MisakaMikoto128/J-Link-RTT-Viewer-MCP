/**
 * @file BFL_LED.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-12-13
 *
 * @copyright Copyright (c) 2022 Liu Yuanlin Personal.
 *
 */
#include "BFL_LED.h"
#include "main.h"

#define LED1_PIN LL_GPIO_PIN_13
#define LED1_Port GPIOC
#define LED1_Port_CLK_EN() LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC)

#define LED2_PIN LL_GPIO_PIN_0
#define LED2_Port GPIOA
#define LED2_Port_CLK_EN() LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA)

void BFL_LED_init()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    LED1_Port_CLK_EN();
    BFL_LED_off(LED1);
    GPIO_InitStruct.Pin = LED1_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(LED1_Port, &GPIO_InitStruct);

    LED2_Port_CLK_EN();
    BFL_LED_off(LED2);
    GPIO_InitStruct.Pin = LED2_PIN;
    LL_GPIO_Init(LED2_Port, &GPIO_InitStruct);
}

void BFL_LED_on(int led_id)
{
    switch (led_id)
    {
    case LED1:
        LL_GPIO_ResetOutputPin(LED1_Port, LED1_PIN);
        break;
    case LED2:
        LL_GPIO_ResetOutputPin(LED2_Port, LED2_PIN);
        break;
    default:
        break;
    }
}

void BFL_LED_off(int led_id)
{
    switch (led_id)
    {
    case LED1:
        LL_GPIO_SetOutputPin(LED1_Port, LED1_PIN);
        break;
    case LED2:
        LL_GPIO_SetOutputPin(LED2_Port, LED2_PIN);
        break;
    default:
        break;
    }
}

void BFL_LED_toggle(int led_id)
{
    switch (led_id)
    {
    case LED1:
        LL_GPIO_TogglePin(LED1_Port, LED1_PIN);
        break;
    case LED2:
        LL_GPIO_TogglePin(LED2_Port, LED2_PIN);
        break;
    default:
        break;
    }
}