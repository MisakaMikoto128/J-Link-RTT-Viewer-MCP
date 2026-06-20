/**
 * @file HDL_PWM_test.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-02-10
 * @last modified 2023-02-10
 *
 * @copyright Copyright (c) 2023 Liu Yuanlin Personal.
 *
 */
#include "HDL_PWM_test.h"
#include "HDL_PWM.h"
#include "BFL_LED.h"
#include "HDL_CPU_Time.h"
#include "mtime.h"
void HDL_PWM_Test(void)
{
    HDL_CPU_Time_Init();
    BFL_LED_init();
    HDL_PWM_Init(PWM_1);
    HDL_PWM_SetFreq(PWM_1,100);
    HDL_PWM_SetDuty(PWM_1, 0.5f);
    while (1)
    {
        if (period_query(0, 500))
        {
            BFL_LED_toggle(LED1);
        }
    }
}