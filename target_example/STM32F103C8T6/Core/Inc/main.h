/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

#include "stm32f1xx_ll_adc.h"
#include "stm32f1xx_ll_crc.h"
#include "stm32f1xx_ll_i2c.h"
#include "stm32f1xx_ll_iwdg.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_rtc.h"
#include "stm32f1xx_ll_spi.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED0_Pin LL_GPIO_PIN_13
#define LED0_GPIO_Port GPIOC
#define IO4_Pin LL_GPIO_PIN_7
#define IO4_GPIO_Port GPIOA
#define IO3_Pin LL_GPIO_PIN_0
#define IO3_GPIO_Port GPIOB
#define IO2_Pin LL_GPIO_PIN_1
#define IO2_GPIO_Port GPIOB
#define IO1_Pin LL_GPIO_PIN_2
#define IO1_GPIO_Port GPIOB
#define UART3_RE__Pin LL_GPIO_PIN_12
#define UART3_RE__GPIO_Port GPIOB
#define UART3_DE_Pin LL_GPIO_PIN_13
#define UART3_DE_GPIO_Port GPIOB
#define IO_485_R120_EN_Pin LL_GPIO_PIN_14
#define IO_485_R120_EN_GPIO_Port GPIOB
#define IO_485_T120_EN_Pin LL_GPIO_PIN_15
#define IO_485_T120_EN_GPIO_Port GPIOB
#define IO_CAN_120_EN_Pin LL_GPIO_PIN_10
#define IO_CAN_120_EN_GPIO_Port GPIOA
#define IO_485_HALF_EN_Pin LL_GPIO_PIN_15
#define IO_485_HALF_EN_GPIO_Port GPIOA
#define IO_CAN_EN_Pin LL_GPIO_PIN_3
#define IO_CAN_EN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
