/**
 * @file rtc_time.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-01-30
 * @last modified 2023-01-30
 *
 * @copyright Copyright (c) 2023 Liu Yuanlin Personal.
 *
 */
#include "rtc_time.h"
#if defined(_WIN32) || defined(_WIN64)
// define something for Windows (64-bit only)
#include <windows.h>
#include <time.h>
/**
 * @brief get current second unix timestamp
 *
 * @return uint64_t
 */
uint64_t getUnixSecTimestamp()
{
    time_t t;
    time(&t);
    return t;
}

uint64_t getUnixMilliSecTimestamp()
{
    FILETIME file_time;
    GetSystemTimeAsFileTime(&file_time);
    uint64_t time = ((uint64_t)file_time.dwLowDateTime) + ((uint64_t)file_time.dwHighDateTime << 32);

    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970
    uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

    return (uint64_t)((time - EPOCH) / 10000LL);
}

int8_t get_timezone(void)
{
    time_t time_utc = 0;
    struct tm *p_tm_time;
    int8_t time_zone = 0;
    p_tm_time = localtime(&time_utc); // 转成当地时间
    time_zone = (p_tm_time->tm_hour > 12) ? (p_tm_time->tm_hour -= 24) : p_tm_time->tm_hour;
    return time_zone;
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
#include <sys/time.h>
#include <unistd.h>
uint64_t getUnixSecTimestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}

uint64_t getUnixMilliSecTimestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

#elif __unix__ // all unices not caught above
// Unix
#elif defined(_POSIX_VERSION)
// POSIX
#elif defined(_HDL_LIB)
#include "HDL_RTC.h"
// HDL
inline uint64_t getUnixSecTimestamp()
{
    return HDL_RTC_GetTimeTick(NULL);
}

inline uint64_t getUnixMilliSecTimestamp()
{
    return HDL_RTC_GetTimeTick(NULL)*1000;
}

int8_t get_timezone(void)
{
    return 8;
}

void setUnixMilliSecTimestamp(uint64_t unix_ts_ms)
{
    HDL_RTC_SetTimeTick(unix_ts_ms/1000);
}
#else
#error "Unknown"
#endif
