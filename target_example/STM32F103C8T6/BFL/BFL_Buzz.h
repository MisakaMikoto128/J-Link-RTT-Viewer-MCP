/**
* @file BFL_Buzz.h
* @author Liu Yuanlin (liuyuanlins@outlook.com)
* @brief
* @version 0.1
* @date 2023-03-19
* @last modified 2023-03-19
*
* @copyright Copyright (c) 2023 Liu Yuanlin Personal.
*
*/
#ifndef BFL_BUZZ_H
#define BFL_BUZZ_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "HDL_GPIO.h"
#include "main.h"
#define BUZZ_PIN LL_GPIO_PIN_12
#define BUZZ_Port GPIOA
#define BUZZ_Port_CLK_EN() LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA)

void BFL_Buzz_off();

void BFL_Buzz_toggle();

void BFL_Buzz_on();

void BFL_Buzz_init();

bool BFL_Buzz_isOff();

void BFL_Buzz_setTrigLevel(HDL_GPIO_PinState_t level);
#ifdef __cplusplus
}
#endif
#endif //!BFL_BUZZ_H
