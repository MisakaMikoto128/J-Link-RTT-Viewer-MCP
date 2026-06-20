/**
 * @file HDL_Freq_Measure.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2025-12-26
 * @last modified 2025-12-26
 *
 * @copyright Copyright (c) 2025 Liu Yuanlin Personal.
 *
 */
#include "HDL_Freq_Measure.h"
#include "description.h"
#include "main.h"

void HDL_Freq_Measure_Init(FREQ_MEASURE_ID_t freqMeasureID)
{
    UNUSED(freqMeasureID);

    /* USER CODE BEGIN TIM2_Init 0 */

    /* USER CODE END TIM2_Init 0 */

    LL_TIM_InitTypeDef TIM_InitStruct = {0};

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    /**TIM2 GPIO Configuration
    PA0-WKUP   ------> TIM2_ETR
    */
    GPIO_InitStruct.Pin  = LL_GPIO_PIN_0;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* TIM2 interrupt Init */
    // NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    // NVIC_EnableIRQ(TIM2_IRQn);

    /* USER CODE BEGIN TIM2_Init 1 */

    /* USER CODE END TIM2_Init 1 */
    TIM_InitStruct.Prescaler     = 0;
    TIM_InitStruct.CounterMode   = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload    = 65535;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(TIM2, &TIM_InitStruct);
    LL_TIM_DisableARRPreload(TIM2);
    LL_TIM_ConfigETR(TIM2, LL_TIM_ETR_POLARITY_NONINVERTED, LL_TIM_ETR_PRESCALER_DIV1, LL_TIM_ETR_FILTER_FDIV1);
    LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_EXT_MODE2);
    LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(TIM2);
    /* USER CODE BEGIN TIM2_Init 2 */
    LL_TIM_EnableCounter(TIM2); // 计数使能
    /* USER CODE END TIM2_Init 2 */
}

PluseCnt_t HDL_Freq_Measure_GetPlusCount(FREQ_MEASURE_ID_t freqMeasureID)
{
    UNUSED(freqMeasureID);
    return LL_TIM_GetCounter(TIM2);
}

void HDL_Freq_Measure_ResetPlusCount(FREQ_MEASURE_ID_t freqMeasureID)
{
    UNUSED(freqMeasureID);
    LL_TIM_SetCounter(TIM2, 0);
}