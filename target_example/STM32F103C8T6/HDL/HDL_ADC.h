/**
* @file HDL_ADC.h
* @author Liu Yuanlin (liuyuanlins@outlook.com)
* @brief
* @version 0.1
* @date 2023-01-30
* @last modified 2023-01-30
*
* @copyright Copyright (c) 2023 Liu Yuanlin Personal.
*
*/
#ifndef HDL_ADC_H
#define HDL_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/* USER CODE BEGIN Prototypes */
typedef enum
{
    ADC_1 = 0,
    ADC_2,
    ADC_3,
    ADC_NUM,
}ADC_ID_t;

void HDL_ADC_Init(ADC_ID_t adcID);
void HDL_ADC_DeInit(ADC_ID_t adcID);
float HDL_ADC_Read(ADC_ID_t adcID);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif //!HDL_ADC_H
