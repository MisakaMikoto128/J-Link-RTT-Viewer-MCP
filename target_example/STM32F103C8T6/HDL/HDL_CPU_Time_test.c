/**
 * @file HDL_CPU_Time_test.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-10-09
 *
 * @copyright Copyright (c) 2022 Liu Yuanlin Personal.
 *
 */

#include "HDL_CPU_Time_test.h"
#include "log.h"
#include "main.h"
/**
 * @brief CPU时钟测试。对比和HALtick的误差。
 *
 */
void HDL_CPU_Time_test()
{
    HDL_CPU_Time_Init();
    ulog_init_user();

    uint32_t cpu_tick = 0;
    uint32_t cpu_us_tick = 0;
    uint32_t hal_tick = 0;
    while (1)
    {
        hal_tick = HAL_GetTick();
        cpu_tick = HDL_CPU_Time_GetTick();
        cpu_us_tick = HDL_CPU_Time_GetUsTick();
        ULOG_INFO("hal_tick = %u,cpu_tick = %u,hal_tick - cpu_tick %d\r\n", hal_tick, cpu_tick, hal_tick - cpu_tick);
        ULOG_INFO("cpu_us_tick = %u\r\n", cpu_us_tick);
    }
}

void call1()
{
    static int i = 0;
    ULOG_INFO("call 1 = %u\r\n", i++);
    uint32_t cpu_tick = HDL_CPU_Time_GetTick();
    ULOG_INFO("cpu_us_tick = %u\r\n", cpu_tick);
    if (i < 10)
    {
        HDL_CPU_Time_StartHardTimer(1, 1000, call1);
    }
}

void call2()
{
    static int i = 0;
    ULOG_INFO("call 2 = %u\r\n", i++);
}

/**
 * @brief CPU时钟测试。对比和HALtick的误差。测试TIMx比较器中断。
 *
 */
void HDL_CPU_Time_hard_timer_test()
{
    HDL_CPU_Time_Init();
    ulog_init_user();
    HDL_CPU_Time_StartHardTimer(1, 1000, call1);
    HDL_CPU_Time_StartHardTimer(4, 1000, call2);
    while (1)
    {
    }
}