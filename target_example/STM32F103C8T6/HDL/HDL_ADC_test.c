/**
* @file HDL_ADC_test.c
* @author Liu Yuanlin (liuyuanlins@outlook.com)
* @brief
* @version 0.1
* @date 2023-03-29
* @last modified 2023-03-29
*
* @copyright Copyright (c) 2023 Liu Yuanlin Personal.
*
*/
#include "HDL_ADC_test.h"
#include "HDL_CPU_Time.h"
#include "mtime.h"
#include "log.h"
/*
CPU时间、RTC时间和Flsah已经在前面开发板初始化是初始化了。
*/

void HDL_ADC_test()
{
    ulog_init_user();
    HDL_ADC_Init(ADC_1);
    HDL_ADC_Init(ADC_2);
    while (1)
    {
        float volt1 = HDL_ADC_Read(ADC_1);
        float volt2 = HDL_ADC_Read(ADC_2);
        ULOG_DEBUG("ADC_1: %f V, ADC_2: %f V", volt1, volt2);
        HDL_CPU_Time_DelayMs(1000);
    }
}
