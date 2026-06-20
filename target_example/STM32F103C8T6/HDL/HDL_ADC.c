/**
 * @file HDL_ADC.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-01-30
 * @last modified 2023-01-30
 *
 * @copyright Copyright (c) 2023 Liu Yuanlin Personal.
 *
 */
#include "HDL_ADC.h"

/**
 * @brief ADC Init
 *
 * @param adcID ADC通道编号。
 */
void HDL_ADC_Init(ADC_ID_t adcID)
{
    /* USER CODE BEGIN ADC1_Init 0 */

    /* USER CODE END ADC1_Init 0 */

    LL_ADC_InitTypeDef ADC_InitStruct             = {0};
    LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {0};
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct     = {0};

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Peripheral clock enable */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    /**ADC1 GPIO Configuration
    PA0-WKUP   ------> ADC1_IN0
    PA1   ------> ADC1_IN1
    PA4   ------> ADC1_IN4
    */
    GPIO_InitStruct.Pin  = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_4;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USER CODE BEGIN ADC1_Init 1 */

    /* USER CODE END ADC1_Init 1 */
    /** Common config
     */
    ADC_InitStruct.DataAlignment      = LL_ADC_DATA_ALIGN_RIGHT;
    ADC_InitStruct.SequencersScanMode = LL_ADC_SEQ_SCAN_DISABLE;
    LL_ADC_Init(ADC1, &ADC_InitStruct);
    ADC_CommonInitStruct.Multimode = LL_ADC_MULTI_INDEPENDENT;
    LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &ADC_CommonInitStruct);
    ADC_REG_InitStruct.TriggerSource    = LL_ADC_REG_TRIG_SOFTWARE;
    ADC_REG_InitStruct.SequencerLength  = LL_ADC_REG_SEQ_SCAN_DISABLE;
    ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    ADC_REG_InitStruct.ContinuousMode   = LL_ADC_REG_CONV_SINGLE;
    ADC_REG_InitStruct.DMATransfer      = LL_ADC_REG_DMA_TRANSFER_NONE;
    LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);
    /** Configure Regular Channel
     */
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_0);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_0, LL_ADC_SAMPLINGTIME_1CYCLE_5);
    /* USER CODE BEGIN ADC1_Init 2 */
    LL_ADC_Enable(ADC1);
    /* USER CODE END ADC1_Init 2 */

    // /* USER CODE BEGIN ADC2_Init 0 */

    // /* USER CODE END ADC2_Init 0 */

    // /* Peripheral clock enable */
    // LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC2);

    // LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    // /**ADC2 GPIO Configuration
    // PA0-WKUP   ------> ADC2_IN0
    // PA1   ------> ADC2_IN1
    // */
    // GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1;
    // GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    // LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // /* USER CODE BEGIN ADC2_Init 1 */

    // /* USER CODE END ADC2_Init 1 */
    // /** Common config
    //  */
    // ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    // ADC_InitStruct.SequencersScanMode = LL_ADC_SEQ_SCAN_DISABLE;
    // LL_ADC_Init(ADC2, &ADC_InitStruct);
    // ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
    // ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
    // ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    // ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
    // ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
    // LL_ADC_REG_Init(ADC2, &ADC_REG_InitStruct);
    // /** Configure Regular Channel
    //  */
    // LL_ADC_REG_SetSequencerRanks(ADC2, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_1);
    // LL_ADC_SetChannelSamplingTime(ADC2, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_1CYCLE_5);
    // /* USER CODE BEGIN ADC2_Init 2 */
    // LL_ADC_Enable(ADC2);
    // /* USER CODE END ADC2_Init 2 */

    // /* USER CODE BEGIN ADC1_Init 0 */

    // /* USER CODE END ADC1_Init 0 */

    // LL_ADC_InitTypeDef ADC_InitStruct = {0};
    // LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {0};
    // LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};

    // LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    // /* Peripheral clock enable */
    // LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);

    // LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    // /**ADC1 GPIO Configuration
    // PA0-WKUP   ------> ADC1_IN0
    // PA1   ------> ADC1_IN1
    // PA4   ------> ADC1_IN4
    // */
    // GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_4;
    // GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    // LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // /* USER CODE BEGIN ADC1_Init 1 */

    // /* USER CODE END ADC1_Init 1 */

    // /** Common config
    //  */
    // ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    // ADC_InitStruct.SequencersScanMode = LL_ADC_SEQ_SCAN_ENABLE;
    // LL_ADC_Init(ADC1, &ADC_InitStruct);
    // ADC_CommonInitStruct.Multimode = LL_ADC_MULTI_INDEPENDENT;
    // LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &ADC_CommonInitStruct);
    // ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
    // ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS;
    // ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    // ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
    // ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
    // LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);

    // /** Configure Regular Channel
    //  */
    // LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_0);
    // LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_0, LL_ADC_SAMPLINGTIME_7CYCLES_5);

    // /** Configure Regular Channel
    //  */
    // LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_1);
    // LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_7CYCLES_5);

    // /** Configure Regular Channel
    //  */
    // LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_3, LL_ADC_CHANNEL_4);
    // LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_4, LL_ADC_SAMPLINGTIME_7CYCLES_5);
    // /* USER CODE BEGIN ADC1_Init 2 */
    // LL_ADC_Enable(ADC1);
    // /* USER CODE END ADC1_Init 2 */
}

/**
 * @brief ADC DeInit
 *
 * @param adcID ADC通道编号。
 */
void HDL_ADC_DeInit(ADC_ID_t adcID)
{
    ;
}

/**
 * @brief 读取ADC的实际电压值。
 *
 * @param adcID ADC通道编号。
 * @return float 单位V，范围0-3.3V。
 */
float HDL_ADC_Read(ADC_ID_t adcID)
{
    uint16_t adcValue = 0;
    float voltage     = 0.0f;
    LL_ADC_Enable(ADC1);
    switch (adcID) {
        case ADC_1:
            // 配置采集通道
            LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_0);
            LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_0, LL_ADC_SAMPLINGTIME_1CYCLE_5);
            break;
        case ADC_2:
            LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_1);
            LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_1CYCLE_5);
            break;
        case ADC_3:
            LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_4);
            LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_4, LL_ADC_SAMPLINGTIME_1CYCLE_5);
            break;
        default:
            break;
    }
    
    LL_ADC_REG_StartConversionSWStart(ADC1);
    while ((LL_ADC_IsActiveFlag_EOS(ADC1) == 0)) {
        ;
    }
    adcValue = LL_ADC_REG_ReadConversionData12(ADC1);
    LL_ADC_ClearFlag_EOS(ADC1);
    LL_ADC_Disable(ADC1);
    voltage = (float)adcValue * 3.3f / 4096.0f;
    return voltage;
}