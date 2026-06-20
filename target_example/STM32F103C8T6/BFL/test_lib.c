/**
 * @file test_lib.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-11-04
 *
 * @copyright Copyright (c) 2022 Liu Yuanlin Personal.
 *
 */
#include "test_lib.h"

/**
 * @brief 初始化测试环境。
 *
 * @param loop_frq_test
 * @param measure_time 单位ms
 */
void test_LoopFrequencyTest_init(LoopFrequencyTest_t *loop_frq_test, uint32_t measure_time)
{
    loop_frq_test->measure_time = measure_time;
    loop_frq_test->state = LOOP_FREQUENCY_TEST_STATE_START;
    loop_frq_test->exe_cnt = 0;
    loop_frq_test->max_period = 0;
    loop_frq_test->min_period = 0xFFFFFFFF;
    loop_frq_test->freq = 0;
    loop_frq_test->measure_start_tick = 0;
    loop_frq_test->__last_query_tick = 0;
    loop_frq_test->__current_peroid = 0;
}

/**
 * @brief 完成时间测量。
 *
 * @param loop_frq_test
 */
void test_LoopFrequencyTest_handler(LoopFrequencyTest_t *loop_frq_test)
{
    switch (loop_frq_test->state)
    {
    case LOOP_FREQUENCY_TEST_STATE_START:
        loop_frq_test->measure_start_tick = HDL_CPU_Time_GetTick();
        loop_frq_test->__last_query_tick = HDL_CPU_Time_GetTick();
        loop_frq_test->state = LOOP_FREQUENCY_TEST_STATE_MEASURING;
        break;
    case LOOP_FREQUENCY_TEST_STATE_MEASURING:

        loop_frq_test->exe_cnt++;
        if ((HDL_CPU_Time_GetTick() - loop_frq_test->measure_start_tick) >= loop_frq_test->measure_time)
        {
            loop_frq_test->freq = loop_frq_test->exe_cnt * 1.0f / loop_frq_test->measure_time;
            loop_frq_test->exe_cnt = 0;
            loop_frq_test->state = LOOP_FREQUENCY_TEST_STATE_END;
        }

        loop_frq_test->__current_peroid = HDL_CPU_Time_GetTick() - loop_frq_test->__last_query_tick;
        loop_frq_test->max_period = loop_frq_test->__current_peroid > loop_frq_test->max_period ? loop_frq_test->__current_peroid : loop_frq_test->max_period;
        loop_frq_test->min_period = loop_frq_test->__current_peroid < loop_frq_test->min_period ? loop_frq_test->__current_peroid : loop_frq_test->min_period;
        loop_frq_test->__last_query_tick = HDL_CPU_Time_GetTick();
        break;
    case LOOP_FREQUENCY_TEST_STATE_END:
        break;
    default:
        break;
    }
}

/**
 * @brief 显示当前循环测量测量到的循环频率。
 *
 * @param loop_frq_test
 */
void test_LoopFrequencyTest_show(LoopFrequencyTest_t *loop_frq_test, const char *prifix)
{
    ULOG_INFO("[Loop Freq: %s]:%.4f KHz Max:%u ms,Min %u ms\r\n", prifix, loop_frq_test->freq, loop_frq_test->max_period, loop_frq_test->min_period);
}

void test_LoopFrequencyTest_reset(LoopFrequencyTest_t *loop_frq_test)
{
    ((loop_frq_test)->state = LOOP_FREQUENCY_TEST_STATE_START);
    loop_frq_test->__current_peroid = 0;
    loop_frq_test->max_period = 0;
    loop_frq_test->min_period = 0xFFFFFFFF;
}