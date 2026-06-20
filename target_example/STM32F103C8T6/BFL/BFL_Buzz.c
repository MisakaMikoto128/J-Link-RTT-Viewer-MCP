/**
 * @file BFL_Buzz.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-03-19
 * @last modified 2023-03-19
 *
 * @copyright Copyright (c) 2023 Liu Yuanlin Personal.
 *
 */
#include "BFL_Buzz.h"

HDL_GPIO_PinState_t buzzTrigLevel = HDL_GPIO_LOW;

void BFL_Buzz_off()
{
    if (buzzTrigLevel == HDL_GPIO_LOW)
    {
        LL_GPIO_SetOutputPin(BUZZ_Port, BUZZ_PIN);
    }
    else if (buzzTrigLevel == HDL_GPIO_HIGH)
    {
        LL_GPIO_ResetOutputPin(BUZZ_Port, BUZZ_PIN);
    }
}

void BFL_Buzz_toggle()
{
    LL_GPIO_TogglePin(BUZZ_Port, BUZZ_PIN);
}

void BFL_Buzz_on()
{
    if (buzzTrigLevel == HDL_GPIO_LOW)
    {
        LL_GPIO_ResetOutputPin(BUZZ_Port, BUZZ_PIN);
    }
    else if (buzzTrigLevel == HDL_GPIO_HIGH)
    {
        LL_GPIO_SetOutputPin(BUZZ_Port, BUZZ_PIN);
    }
}

void BFL_Buzz_init()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    BUZZ_Port_CLK_EN();
    BFL_Buzz_off();
    GPIO_InitStruct.Pin = BUZZ_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(BUZZ_Port, &GPIO_InitStruct);
    BFL_Buzz_off();
}

bool BFL_Buzz_isOff()
{
    if (buzzTrigLevel == HDL_GPIO_LOW)
    {
        return LL_GPIO_IsOutputPinSet(BUZZ_Port, BUZZ_PIN);
    }
    else if (buzzTrigLevel == HDL_GPIO_HIGH)
    {
        return !LL_GPIO_IsOutputPinSet(BUZZ_Port, BUZZ_PIN);
    }
		return false;
}

void BFL_Buzz_setTrigLevel(HDL_GPIO_PinState_t level)
{
    buzzTrigLevel = level;
}
