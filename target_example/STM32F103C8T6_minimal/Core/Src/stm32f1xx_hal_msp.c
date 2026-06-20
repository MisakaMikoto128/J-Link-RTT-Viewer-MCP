/**
  ******************************************************************************
  * @file    stm32f1xx_hal_msp.c
  * @brief   HAL MSP (Memory Support Package) for STM32F103C8T6 minimal project
  ******************************************************************************
  */

#include "stm32f1xx_hal.h"

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize the Global MSP.
  * @retval None
  */
void HAL_MspInit(void)
{
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}

/**
  * @brief  DeInitialize the Global MSP.
  * @retval None
  */
void HAL_MspDeInit(void)
{
}

/**
  * @brief  Initialize the PPP MSP.
  * @retval None
  */
void HAL_MspInit(void)
{
}

/**
  * @brief  DeInitialize the PPP MSP.
  * @retval None
  */
void HAL_MspDeInit(void)
{
}