/**
 * @file mtime.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-01-30
 * @last modified 2023-01-30
 *
 * @copyright Copyright (c) 2023 Liu Yuanlin Personal.
 *
 */
#include "mtime.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
mtime_t mgmtime(struct mtm_t *mtm)
{
    uint64_t sec_timestamp = getUnixSecTimestamp();
    mytime_struct datetime = {0};
    utc_sec_2_mytime(sec_timestamp, &datetime);
    if (mtm != NULL)
    {
        mtm->nYear = datetime.nYear;
        mtm->nMonth = datetime.nMonth;
        mtm->nDay = datetime.nDay;
        mtm->nHour = datetime.nHour;
        mtm->nMin = datetime.nMin;
        mtm->nSec = datetime.nSec;
        mtm->nWeek = datetime.nWeek;
    }
    return sec_timestamp;
}

mtime_t mlocaltime(struct mtm_t *mtm)
{
    uint64_t sec_timestamp = getUnixSecTimestamp();
    sec_timestamp = sec_timestamp + get_timezone() * 3600;
    mytime_struct datetime = {0};
    utc_sec_2_mytime(sec_timestamp, &datetime);
    if (mtm != NULL)
    {
        mtm->nYear = datetime.nYear;
        mtm->nMonth = datetime.nMonth;
        mtm->nDay = datetime.nDay;
        mtm->nHour = datetime.nHour;
        mtm->nMin = datetime.nMin;
        mtm->nSec = datetime.nSec;
        mtm->nWeek = datetime.nWeek;
    }
    return sec_timestamp;
}

static PeriodREC_t period_last_exe_tick_table[MAX_PERIOD_ID + 1] = {0};

/**
 * @brief 查询是否到了需要的周期。这个函数中高速查询，如果判断周期到了，就会
 * 返回true，否则返回false,并且当周期到了之后会更新last_exe_tick，保证每周期只会判
 * 断结果为真一次。用于在主循环中方便的构建周期性执行的代码段。
 *
 * 内置一个Period_t的最后一次执行时间的时间戳表，period_id标识。
 * @param period_id 周期id，全局唯一。
 * @param period 周期,单位tick。
 * @return true 周期到了
 * @return false 周期未到。
 */
bool period_query(uint8_t period_id, PeriodREC_t period)
{
    bool ret = false;

    // 这里一定是>=，如果是 > ，那么在1 cpu tick间隔的时候时间上是2cpu tick执行一次。
    // 这里不允许period为0，不然就会失去调度作用。
    if (((PeriodREC_t)getCPUTick() - period_last_exe_tick_table[period_id]) >= period)
    {
        period_last_exe_tick_table[period_id] = getCPUTick();
        ret = true;
    }
    return ret;
}

/**
 * @brief 同period_query_user，只是时间记录再一个uint32_t*指向的变量中。
 *
 * @param period_recorder 记录运行时间的变量的指针。
 * @param period 周期,单位tick。
 * @return true 周期到了
 * @return false 周期未到。
 */
bool period_query_user(PeriodREC_t *period_recorder, PeriodREC_t period)
{
    bool ret = false;
    // 这里一定是>=，如果是 > ，那么在1 cpu tick间隔的时候时间上是2cpu tick执行一次。
    // 这里不允许period为0，不然就会失去调度作用。
    if ((getCPUTick() - *period_recorder) >= period)
    {
        *period_recorder = getCPUTick();
        ret = true;
    }
    return ret;
}

/**
 * @brief 从第一次被调用这个方法处理delay_rec对象开始，是否经过了delay时间。
 *
 * @param delay_recorder
 * @param delay
 * @return true 延时条件满足
 * @return false
 */
bool delay_one_times(DelayREC_t *delay_rec, uint32_t delay)
{
    if (delay_rec->isStarted == false)
    {
        delay_rec->start = getCPUTick();
        delay_rec->isStarted = true;
        delay_rec->isFinished = false;
    }
    else if (delay_rec->isFinished == false)
    {
        if ((getCPUTick() - delay_rec->start) >= delay)
        {
            delay_rec->isFinished = true;
        }
    }

    return delay_rec->isFinished;
}

/**
 * @brief 将时间字符串转换为秒数
 *
 * @param timeStr
 * @return uint32_t 如果格式错误返回-1, 否则返回秒数
 */
int timeStr2Sec(char timeStr[TIME_STR_LEN + 1])
{
    char _timeStr[TIME_STR_LEN + 1] = {0};
    memcpy(_timeStr, timeStr, TIME_STR_LEN);
    _timeStr[TIME_STR_LEN] = '\0';
    // check timeStr format is 00:00:00
    if (_timeStr[2] != ':' || _timeStr[5] != ':')
    {
        return -1;
    }

    if (!isdigit(_timeStr[0]) || !isdigit(_timeStr[1]) || !isdigit(_timeStr[3]) || !isdigit(_timeStr[4]) || !isdigit(_timeStr[6]) || !isdigit(_timeStr[7]))
    {
        return -1;
    }

    int hour = 0;
    int min = 0;
    int sec = 0;
    sscanf(_timeStr, "%d:%d:%d", &hour, &min, &sec);
    return hour * 3600 + min * 60 + sec;
}

/**
 * @brief 将秒数转换为时间字符串
 *
 * @param sec
 * @param timeStr
 */
void sec2TimeStr(uint32_t sec, char timeStr[TIME_STR_LEN + 1])
{
    char _timeStr[TIME_STR_LEN + 1] = {0};
    memcpy(_timeStr, timeStr, TIME_STR_LEN);
    int hour = sec / 3600;
    int min = (sec % 3600) / 60;
    int second = sec % 60;
    sprintf(_timeStr, "%02d:%02d:%02d", hour, min, second);
}