/**
 * @file HDL_PWM.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-02-10
 * @last modified 2023-02-10
 *
 * @copyright Copyright (c) 2023 Liu Yuanlin Personal.
 *
 */
#include "HDL_PWM.h"
#include <math.h>

#define HDL_TIM_MAX_DUTY  (2000 * 4)
#define HDL_TIM_PRESCALER (72 / 2)
#define HDL_TIM_CLK_FREQ  72000000UL

static void HDL_PWM_1_Init()
{
    /* USER CODE BEGIN TIM3_Init 0 */

    /* USER CODE END TIM3_Init 0 */

    LL_TIM_InitTypeDef TIM_InitStruct       = {0};
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

    /* USER CODE BEGIN TIM3_Init 1 */

    /* USER CODE END TIM3_Init 1 */
    TIM_InitStruct.Prescaler     = (HDL_TIM_PRESCALER >> 2) - 1;
    TIM_InitStruct.CounterMode   = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload    = HDL_TIM_MAX_DUTY - 1;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(TIM3, &TIM_InitStruct);
    LL_TIM_DisableARRPreload(TIM3);
    TIM_OC_InitStruct.OCMode       = LL_TIM_OCMODE_PWM1;
    TIM_OC_InitStruct.OCState      = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState     = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.CompareValue = 0;
    TIM_OC_InitStruct.OCPolarity   = LL_TIM_OCPOLARITY_LOW;
    LL_TIM_OC_Init(TIM3, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(TIM3, LL_TIM_CHANNEL_CH1);
    LL_TIM_SetTriggerOutput(TIM3, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(TIM3);
    LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH1);
    /* USER CODE BEGIN TIM3_Init 2 */

    /* USER CODE END TIM3_Init 2 */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    /**TIM3 GPIO Configuration
    PB4     ------> TIM3_CH1
    */
    GPIO_InitStruct.Pin        = LL_GPIO_PIN_4;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    LL_GPIO_AF_RemapPartial_TIM3();

    // Enable TIM3 Output Compare to generate PWM on CH1 using LL library
    LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1);
    LL_TIM_EnableCounter(TIM3);
    LL_TIM_EnableAllOutputs(TIM3);
}

void HDL_PWM_2_Init()
{
    /* USER CODE BEGIN TIM4_Init 0 */

    /* USER CODE END TIM4_Init 0 */

    LL_TIM_InitTypeDef TIM_InitStruct       = {0};
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);

    /* USER CODE BEGIN TIM4_Init 1 */

    /* USER CODE END TIM4_Init 1 */
    TIM_InitStruct.Prescaler     = HDL_TIM_PRESCALER - 1;
    TIM_InitStruct.CounterMode   = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload    = HDL_TIM_MAX_DUTY - 1;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(TIM4, &TIM_InitStruct);
    LL_TIM_DisableARRPreload(TIM4);
    LL_TIM_OC_EnablePreload(TIM4, LL_TIM_CHANNEL_CH3);
    TIM_OC_InitStruct.OCMode       = LL_TIM_OCMODE_PWM1;
    TIM_OC_InitStruct.OCState      = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState     = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.CompareValue = 0;
    TIM_OC_InitStruct.OCPolarity   = LL_TIM_OCPOLARITY_HIGH;
    LL_TIM_OC_Init(TIM4, LL_TIM_CHANNEL_CH3, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(TIM4, LL_TIM_CHANNEL_CH3);
    LL_TIM_SetTriggerOutput(TIM4, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(TIM4);
    /* USER CODE BEGIN TIM4_Init 2 */

    /* USER CODE END TIM4_Init 2 */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    /**TIM4 GPIO Configuration
    PB8     ------> TIM4_CH3
    */
    GPIO_InitStruct.Pin        = LL_GPIO_PIN_8;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Enable TIM4 Output Compare to generate PWM on CH1 using LL library
    LL_TIM_CC_EnableChannel(TIM4, LL_TIM_CHANNEL_CH3);
    LL_TIM_EnableCounter(TIM4);
    LL_TIM_EnableAllOutputs(TIM4);
}

void HDL_PWM_Init(PWM_ID_t pwmID)
{
    switch (pwmID) {
        case PWM_1:
            HDL_PWM_1_Init();
            break;
        case PWM_2:
            HDL_PWM_2_Init();
            break;
        default:
            break;
    }
}

void HDL_PWM_DeInit(PWM_ID_t pwmID)
{
    switch (pwmID) {
        case PWM_1:
            LL_TIM_CC_DisableChannel(TIM3, LL_TIM_CHANNEL_CH1);
            LL_TIM_CC_DisableChannel(TIM3, LL_TIM_CHANNEL_CH2);
            LL_TIM_CC_DisableChannel(TIM3, LL_TIM_CHANNEL_CH3);
            LL_TIM_CC_DisableChannel(TIM3, LL_TIM_CHANNEL_CH4);
            LL_TIM_DisableCounter(TIM3);
            LL_TIM_DisableAllOutputs(TIM3);
            break;
        case PWM_2:
            LL_TIM_CC_DisableChannel(TIM4, LL_TIM_CHANNEL_CH1);
            LL_TIM_CC_DisableChannel(TIM4, LL_TIM_CHANNEL_CH2);
            LL_TIM_CC_DisableChannel(TIM4, LL_TIM_CHANNEL_CH3);
            LL_TIM_CC_DisableChannel(TIM4, LL_TIM_CHANNEL_CH4);
            LL_TIM_DisableCounter(TIM4);
            LL_TIM_DisableAllOutputs(TIM4);
            break;
        default:
            break;
    }
}

/**
 * @brief Set the duty of the PWM
 *
 * @param pwmID
 * @param duty 0-1
 */
void HDL_PWM_SetDuty(PWM_ID_t pwmID, float duty)
{
    duty              = fminf(fmaxf(duty, 0.0f), 1.0f);
    uint16_t duty_set = (uint16_t)(HDL_TIM_MAX_DUTY * duty);
    switch (pwmID) {
        case PWM_1:
            LL_TIM_OC_SetCompareCH1(TIM3, duty_set);
            break;
        case PWM_2:
            LL_TIM_OC_SetCompareCH3(TIM4, duty_set);
            break;
        default:
            break;
    }
}

void HDL_PWM_SetFreq(PWM_ID_t pwmID, float freq)
{
    // TODO: Add freq limit and atuo adjust prescaler to get the accrate freq and duty subdivision.
    uint16_t prescaler = (uint16_t)(HDL_TIM_CLK_FREQ / (HDL_TIM_PRESCALER * freq));
    switch (pwmID) {
        case PWM_1:
            LL_TIM_SetAutoReload(TIM3, prescaler);
            break;
        case PWM_2:
            LL_TIM_SetAutoReload(TIM4, prescaler);
            break;
        default:
            break;
    }
}