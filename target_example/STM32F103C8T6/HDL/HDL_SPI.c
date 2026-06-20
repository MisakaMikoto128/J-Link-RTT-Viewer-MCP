/**
 * @file HDL_SPI.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-01-19
 *
 * @copyright Copyright (c) 2022 Liu Yuanlin Personal.
 *
 */
#include "HDL_SPI.h"
#include "HDL_CPU_Time.h"
/**
 * @brief SPI初始化。默认为全双工主机,MSB First。通信频率会尽量接近SPI外设允许的最大频率。
 *
 * @param spiID SPI设备ID。
 * @param dataSize 数SPI数据读写据位宽8或者16bit。
 * @param CPOL Specifies the serial clock steady state. 0: Low level, 1: High level.
 * @param CPHA Specifies the clock active edge for the bit capture. 1: The first clock transition is the first data capture edge, 2: The second clock transition is the first data capture edge.
 */
void HDL_SPI_Init(SPI_ID_t spiID, uint8_t dataSize, uint32_t CPOL, uint32_t CPHA)
{
    /* USER CODE BEGIN SPI1_Init 0 */

    /* USER CODE END SPI1_Init 0 */
    LL_RCC_ClocksTypeDef RCC_Clocks;

    LL_SPI_InitTypeDef SPI_InitStruct = {0};

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    LL_RCC_GetSystemClocksFreq(&RCC_Clocks);
    /* Peripheral clock enable */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    /**SPI1 GPIO Configuration
    PA5   ------> SPI1_SCK
    PA6   ------> SPI1_MISO
    PA7   ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin        = LL_GPIO_PIN_5 | LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin  = LL_GPIO_PIN_6;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* SPI1 interrupt Init */
    NVIC_SetPriority(SPI1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(SPI1_IRQn);

    /* USER CODE BEGIN SPI1_Init 1 */

    /* USER CODE END SPI1_Init 1 */
    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
    SPI_InitStruct.Mode              = LL_SPI_MODE_MASTER;
    SPI_InitStruct.DataWidth         = dataSize == 8 ? LL_SPI_DATAWIDTH_8BIT : LL_SPI_DATAWIDTH_16BIT;
    SPI_InitStruct.ClockPolarity     = CPOL == 0 ? LL_SPI_POLARITY_LOW : LL_SPI_POLARITY_HIGH;
    SPI_InitStruct.ClockPhase        = CPHA == 0 ? LL_SPI_PHASE_1EDGE : LL_SPI_PHASE_2EDGE;
    SPI_InitStruct.NSS               = LL_SPI_NSS_SOFT;
#define LL_SPI_BAUDRATEPRESCALER_DIV_MIN ((uint8_t)2)
#define LL_SPI_BAUDRATEPRESCALER_DIV_MAX ((uint8_t)256)
    uint32_t SPI1PeripheralCLK = RCC_Clocks.PCLK2_Frequency;
    uint8_t i                  = 0;
    for (i = LL_SPI_BAUDRATEPRESCALER_DIV_MIN; i <= LL_SPI_BAUDRATEPRESCALER_DIV_MAX; i *= 2) {
        if (SPI1PeripheralCLK / i <= HDL_SPI_MAX_BIT_RATE) {
            break;
        }
    }

    const uint8_t SPI_BaudRatePrescaler[] = {LL_SPI_BAUDRATEPRESCALER_DIV2, LL_SPI_BAUDRATEPRESCALER_DIV4, LL_SPI_BAUDRATEPRESCALER_DIV8, LL_SPI_BAUDRATEPRESCALER_DIV16, LL_SPI_BAUDRATEPRESCALER_DIV32, LL_SPI_BAUDRATEPRESCALER_DIV64, LL_SPI_BAUDRATEPRESCALER_DIV128, LL_SPI_BAUDRATEPRESCALER_DIV256};

    SPI_InitStruct.BaudRate       = LL_SPI_BAUDRATEPRESCALER_DIV8;//SPI_BaudRatePrescaler[i / 2 * 2 - 1];
    SPI_InitStruct.BitOrder       = LL_SPI_MSB_FIRST;
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    SPI_InitStruct.CRCPoly        = 10;
    LL_SPI_Init(SPI1, &SPI_InitStruct);
    /* USER CODE BEGIN SPI1_Init 2 */

    /* USER CODE END SPI1_Init 2 */
}

/**
 * @brief SPI读写。阻塞式读写方法。不能再中断中调用。默认超时时间为10ms。
 *
 * @param spiID SPI设备ID。
 * @param pTxData 发送数据缓冲区。
 * @param pRxData 接收数据缓冲区。
 * @param size 数据长度。
 * @return true 读写成功。
 * @return false 读写失败。
 */
bool HDL_SPI_WriteRead(SPI_ID_t spiID, uint8_t *pTxData, uint8_t *pRxData, uint16_t size)
{
    if (pTxData == NULL || pRxData == NULL || size == 0) {
        return false;
    }
    uint32_t startMoment = 0;
    uint32_t timeout     = HDL_TIME_TO_TICK(1000);
    LL_SPI_Enable(SPI1);
    while (size--) {
        startMoment = HDL_CPU_Time_GetTick();
        while (!LL_SPI_IsActiveFlag_TXE(SPI1)) {
            if (HDL_CPU_Time_GetTick() - startMoment > timeout) {
                LL_SPI_Disable(SPI1);
                return false;
            }
        }
        LL_SPI_TransmitData8(SPI1, *pTxData++);
        startMoment = HDL_CPU_Time_GetTick();
        while (!LL_SPI_IsActiveFlag_RXNE(SPI1)) {
            if (HDL_CPU_Time_GetTick() - startMoment > timeout) {
                LL_SPI_Disable(SPI1);
                return false;
            }
        }
        *pRxData++ = LL_SPI_ReceiveData8(SPI1);
    }
    LL_SPI_Disable(SPI1);
    return true;
}

bool HDL_SPI_Write(SPI_ID_t spiID, uint8_t *pTxData, uint16_t size)
{
    if (pTxData == NULL || size == 0) {
        return false;
    }
    uint32_t startMoment = 0;
    uint32_t timeout     = HDL_TIME_TO_TICK(1000);
    LL_SPI_Enable(SPI1);
    while (size--) {
        startMoment = HDL_CPU_Time_GetTick();
        while (!LL_SPI_IsActiveFlag_TXE(SPI1)) {
            if (HDL_CPU_Time_GetTick() - startMoment > timeout) {
                LL_SPI_Disable(SPI1);
                return false;
            }
        }
        LL_SPI_TransmitData8(SPI1, *pTxData++);
    }
    LL_SPI_Disable(SPI1);
    return true;
}

bool HDL_SPI_Read(SPI_ID_t spiID, uint8_t *pRxData, uint16_t size)
{
    if (pRxData == NULL || size == 0) {
        return false;
    }
    uint32_t startMoment = 0;
    uint32_t timeout     = HDL_TIME_TO_TICK(1000);
    LL_SPI_Enable(SPI1);
    while (size--) {
        startMoment = HDL_CPU_Time_GetTick();
        while (!LL_SPI_IsActiveFlag_TXE(SPI1)) {
            if (HDL_CPU_Time_GetTick() - startMoment > timeout) {
                LL_SPI_Disable(SPI1);
                return false;
            }
        }
        LL_SPI_TransmitData8(SPI1, 0xFF);
        startMoment = HDL_CPU_Time_GetTick();
        while (!LL_SPI_IsActiveFlag_RXNE(SPI1)) {
            if (HDL_CPU_Time_GetTick() - startMoment > timeout) {
                LL_SPI_Disable(SPI1);
                return false;
            }
        }
        *pRxData++ = LL_SPI_ReceiveData8(SPI1);
    }
    LL_SPI_Disable(SPI1);
    return true;
}

/**
 * @brief SPI去初始化。
 *
 * @param spiID SPI设备ID。
 */
void HDL_SPI_DeInit(SPI_ID_t spiID)
{
    LL_SPI_DeInit(SPI1);
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SPI1);
}