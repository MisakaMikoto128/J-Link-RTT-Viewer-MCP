/**
 * @file HDL_RTC.h
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-10-05
 *
 * @copyright Copyright (c) 2022 Liu Yuanlin Personal.
 *
 */

#ifndef HDL_RTC_H
#define HDL_RTC_H

#include "main.h"
#include "mytime.h"

#define RTC_SUBSEC_MAX 1024U
void HDL_RTC_Init();
uint64_t HDL_RTC_GetTimeTick(uint16_t *pSub);
void HDL_RTC_GetStructTime(mytime_struct *myTime);
void HDL_RTC_SetTimeTick(uint64_t localTime);
void HDL_RTC_SetStructTime(mytime_struct *myTime);
#define HDL_RTC_Subsec2mSec(subsec) ((uint64_t)((subsec)*(1000.0f/RTC_SUBSEC_MAX) + 0.5f))
#define HDL_RTC_mSec2Subsec(ms) ((uint64_t)((ms)*RTC_SUBSEC_MAX*0.001f + 0.5f))
#endif // HDL_RTC_H
