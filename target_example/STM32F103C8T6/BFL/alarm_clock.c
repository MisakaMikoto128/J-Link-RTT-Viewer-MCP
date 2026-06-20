#include "alarm_clock.h"
#include "scheduler.h"
#include "mtime.h"
#define ALARM_CLOCK_POLL_PERIOD 1000 // ms

static struct sc_list _gAlarmClockList = {NULL, NULL};
static struct sc_list *gAlarmClockList = &_gAlarmClockList;

static inline void AlarmClockCallback_execute(AlarmClockCallback_t fun)
{
    if (fun != NULL)
    {
        fun();
    }
}

void AlarmClock_init()
{
    sc_list_init(gAlarmClockList);
}

void AlarmClock_obj_init(AlarmClock_t *palarm_clock, AlarmClockCallback_t fun, uint8_t hour, uint8_t minute, uint8_t second, uint8_t week)
{
    palarm_clock->executed = 0;
    palarm_clock->fun = fun;
    palarm_clock->clock.hour = hour;
    palarm_clock->clock.minute = minute;
    palarm_clock->clock.second = second;
    palarm_clock->clock.week = week;
}

void AlarmClock_start(AlarmClock_t *palarm_clock)
{
    if (palarm_clock != NULL)
    {
        //is palarm_clock->next already exist?
        struct sc_list *it = NULL;
        AlarmClock_t *_palarm_clock = NULL;
        struct sc_list *item, *tmp;

        sc_list_foreach_safe(gAlarmClockList, tmp, it)
        {
            _palarm_clock = sc_list_entry(it, AlarmClock_t, next);
            if (_palarm_clock == palarm_clock)
            {
                return;
            }
        }
        palarm_clock->executed = 0;
        //if not exist, add it
        sc_list_init(&palarm_clock->next);
        sc_list_add_tail(gAlarmClockList, &palarm_clock->next);
    }
}

void AlarmClock_stop(AlarmClock_t *palarm_clock)
{
    if (palarm_clock != NULL)
    {
        struct sc_list *it = NULL;
        AlarmClock_t *_palarm_clock = NULL;
        struct sc_list *item, *tmp;

        sc_list_foreach_safe(gAlarmClockList, tmp, it)
        {
            _palarm_clock = sc_list_entry(it, AlarmClock_t, next);
            if (_palarm_clock == palarm_clock)
            {
                sc_list_del(gAlarmClockList, &_palarm_clock->next);
                break;
            }
        }
    }
}

void AlarmClock_handler()
{
    struct sc_list *it = NULL;
    AlarmClock_t *palarm_clock = NULL;
    static Clock_t current_clock;
    static uint32_t poll_period_rec = 0;
    if (period_query_user(&poll_period_rec, scheduler_get_ms_ticks(ALARM_CLOCK_POLL_PERIOD)))
    {
        sc_list_foreach(gAlarmClockList, it)
        {
            palarm_clock = sc_list_entry(it, AlarmClock_t, next);
            Clock_now(&current_clock);
            if (CLOCK_IN_WEEK(&current_clock, palarm_clock->clock.week))
            {
                if (Clock_cmp(&current_clock, &palarm_clock->clock) >= 0)
                {
                    if (palarm_clock->executed == 0)
                    {
                        palarm_clock->executed = 1;
                        AlarmClockCallback_execute(palarm_clock->fun);
                    }
                }
                else
                {
                    palarm_clock->executed = 0;
                }
            }
        }
    }
}

/**
 * @brief 将当前时间转化为秒数。
 *
 * @param clock
 * @return uint32_t 秒数。
 */
uint32_t Clock_to_sec(Clock_t *clock)
{
    return clock->hour * HOUR_MINUTE * MINUTE_SECOND + clock->minute * MINUTE_SECOND + clock->second;
}


/**
 * @brief 从秒数转化为时钟结构体。
 * 
 * @param clock 
 * @param sec 
 */
void Clock_from_sec(Clock_t *clock, uint32_t sec)
{
    clock->hour = sec / HOUR_MINUTE / MINUTE_SECOND;
    clock->minute = (sec % (HOUR_MINUTE * MINUTE_SECOND)) / MINUTE_SECOND;
    clock->second = sec % MINUTE_SECOND;
}

/**
 * @brief 时钟比较,比较对应的一天中的秒数。返回值时两个时钟秒数之差。
 *
 * @param clockA
 * @param clockB
 * @return int32_t 0:clockA == clockB, >0:lockA > clockB, <0 :lockA < clockB
 */
int32_t Clock_minus(Clock_t *clockA, Clock_t *clockB)
{
    return Clock_to_sec(clockA) - Clock_to_sec(clockB);
}

/**
 * @brief 获取本地的时间。
 *
 * @param clockA
 * @param clockB
 */
void Clock_now(Clock_t *clock)
{
    struct mtm_t datetime = {0};
    mlocaltime(&datetime);
    clock->hour = datetime.nHour;
    clock->minute = datetime.nMin;
    clock->second = datetime.nSec;
    clock->week = 1 << (datetime.nWeek - 1);
}