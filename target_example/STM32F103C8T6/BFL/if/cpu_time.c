/**
 * @file cpu_time.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-12-14
 *
 * @copyright Copyright (c) 2022 Liu Yuanlin Personal.
 *
 */
#include "cpu_time.h"
#include "rtc_time.h"
#if defined(_WIN32) || defined(_WIN64)
// define something for Windows (64-bit only)


inline uint32_t getCPUMsTick()
{
    //这里直接截断高位，作为32bit的cpu tick使用
    return (uint32_t)getUnixMilliSecTimestamp();
}

inline uint32_t getCPUTick()
{
    //这里直接截断高位，作为32bit的cpu tick使用
    return (uint32_t)getUnixMilliSecTimestamp();
}

inline float getCPUOneTickTime()
{
    return 1.0f;
}

#elif __APPLE__
#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#elif TARGET_OS_IPHONE
// iOS device
#elif TARGET_OS_MAC
// Other kinds of Mac OS
#endif
#elif __ANDROID__
// android
#elif __linux__
// linux

inline uint32_t getCPUMsTick()
{
    //这里直接截断高位，作为32bit的cpu tick使用
    return (uint32_t)getUnixMilliSecTimestamp();
}

inline uint32_t getCPUTick()
{
    //这里直接截断高位，作为32bit的cpu tick使用
    return (uint32_t)getUnixMilliSecTimestamp();
}

inline float getCPUOneTickTime()
{
    return 1.0f;
}
#elif __unix__ // all unices not caught above
// Unix
#elif defined(_POSIX_VERSION)
// POSIX
#elif defined(_HDL_LIB)
#include "HDL_RTC.h"
#include "HDL_CPU_Time.h"
// HDL
inline uint32_t getCPUMsTick()
{
#if HDL_CPU_TIME_OEN_TICK_TIME == 1000 // 1ms
    return HDL_CPU_Time_GetTick();
#else
    return (uint32_t)(HDL_TICK_TO_TIME(HDL_CPU_Time_GetTick()));
#endif
}

inline uint32_t getCPUTick()
{
    return HDL_CPU_Time_GetTick();
}

inline float getCPUOneTickTime()
{
    return HDL_CPU_TIME_OEN_TICK_TIME / 1000.0f;
}

inline uint32_t MsToTicks(uint32_t ms)
{
    return (uint32_t)(HDL_TIME_TO_TICK(ms));
}
#else
#error "Unknown"
#endif
