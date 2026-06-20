/**
 * @file HDL_PWM.h
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-02-10
 * @last modified 2023-02-10
 *
 * @copyright Copyright (c) 2023 Liu Yuanlin Personal.
 *
 */
#ifndef HDL_PWM_H
#define HDL_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "main.h"

typedef enum {
    PWM_1 = 0,
    PWM_2,
    PWM_NUM,
} PWM_ID_t;

typedef enum {
    PWM_CH1 = 0,
    PWM_CH2,
    PWM_CH3,
    PWM_CH4,
    PWM_CH_NUM,
} PWM_Channel_t;

void HDL_PWM_Init(PWM_ID_t pwmID);

void HDL_PWM_DeInit(PWM_ID_t pwmID);

void HDL_PWM_SetDuty(PWM_ID_t pwmID, float duty);

void HDL_PWM_SetFreq(PWM_ID_t pwmID, float freq);

#ifdef __cplusplus
}
#endif
#endif //! HDL_PWM_H
