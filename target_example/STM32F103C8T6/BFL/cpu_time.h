/**
* @file cpu_time.h
* @author Liu Yuanlin (liuyuanlins@outlook.com)
* @brief
* @version 0.1
* @date 2023-02-02
* @last modified 2023-02-02
*
* @copyright Copyright (c) 2023 Liu Yuanlin Personal.
*
*/
#ifndef CPU_TIME_H
#define CPU_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief 获取CPU的毫秒滴答计数
 * 
 * @return uint32_t 
 */
uint32_t getCPUMsTick();

/**
 * @brief 获取CPU的滴答计数
 * 
 * @return uint32_t 
 */
uint32_t getCPUTick();

/**
 * @brief 获取CPU的滴答计数的时间，单位ms
 * 
 * @return float 
 */
float getCPUOneTickTime();

/**
 * @brief 获取指定时间对应的CPU tick数量。
 *
 * @param ms 时间，单位ms.
 * @return uint32_t tick数。
 */
uint32_t MsToCPUticks(uint32_t ms);

uint32_t MsToTicks(uint32_t ms);
#ifdef __cplusplus
}
#endif
#endif //!CPU_TIME_H
