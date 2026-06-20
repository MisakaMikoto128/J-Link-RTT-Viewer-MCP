/**
 * @file rtc_time.h
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-01-30
 * @last modified 2023-01-30
 *
 * @copyright Copyright (c) 2023 Liu Yuanlin Personal.
 *
 */
#ifndef RTC_TIME_H
#define RTC_TIME_H

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <stdbool.h>

    /**
     * @brief Get the Unix Sec Timestamp object
     *
     * @return uint64_t
     */
    uint64_t getUnixSecTimestamp();
    /**
     * @brief Get the Unix Milli Sec Timestamp object
     *
     * @return uint64_t 在嵌入式环境中一般没有实现。
     */
    uint64_t getUnixMilliSecTimestamp();

    /**
     * @brief 获取当前系统的时区
     *
     * @return int8_t
     */
    int8_t get_timezone(void);

    /**
     * @brief Set the Unix Milli Sec Timestamp object
     * 
     * @param unix_ts_ms 
     */
    void setUnixMilliSecTimestamp(uint64_t unix_ts_ms);
#ifdef __cplusplus
}
#endif
#endif //! RTC_TIME_H
