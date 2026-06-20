/**
 * @file HDL_CPU_Time.h
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2022-10-08
 * 
 * @copyright Copyright (c) 2022 Liu Yuanlin Personal.
 * 
 */

#ifndef HDL_CPU_TIME_H
#define HDL_CPU_TIME_H

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

typedef void(*CPU_Time_Callback_t)(void);

void HDL_CPU_Time_Init();
uint32_t HDL_CPU_Time_GetTick();
void HDL_CPU_Time_ResetTick();
uint32_t HDL_CPU_Time_GetUsTick();
void HDL_CPU_Time_ResetUsTick();
void HDL_CPU_Time_SetCPUTickCallback(CPU_Time_Callback_t _pCallBack);

void HDL_CPU_Time_DelayUs(uint16_t DelayUs);
void HDL_CPU_Time_DelayMs(uint32_t DelayMs);
void HDL_CPU_Time_StartHardTimer(uint8_t _CC, uint16_t _uiTimeOut, void *_pCallBack);
void HDL_CPU_Time_StopHardTimer(uint8_t _CC);

#define HDL_CPU_TIME_OEN_TICK_TIME 1000ULL // 1000 us
/**
 * @brief 将时间转换为tick,time单位为ms,
 * 
 */
#define HDL_TIME_TO_TICK(_time) (((_time*1000ULL) / HDL_CPU_TIME_OEN_TICK_TIME))

/**
 * @brief 将tick转换为时间,返回值单位为ms
 * 
 */
#define HDL_TICK_TO_TIME(_tick) (((_tick*HDL_CPU_TIME_OEN_TICK_TIME) / 1000ULL))

#define MS_TO_TICK(_time) (((_time*1000ULL) / HDL_CPU_TIME_OEN_TICK_TIME))
#ifdef __cplusplus
}
#endif
#endif //!HDL_CPU_TIME_H
