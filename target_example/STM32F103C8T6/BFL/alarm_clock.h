/**
 * @file alarm_clock.h
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-12-19
 *
 * @copyright Copyright (c) 2022 Liu Yuanlin Personal.
 *
 */
#ifndef ALARM_CLOCK_H
#define ALARM_CLOCK_H

#include <stdint.h>
#include "mytime.h"
#include "sc_list.h"
/**
 * 这个库默认使用本地时间。
 *
 */

#define DAY_HOUR 24
#define HOUR_MINUTE 60
#define MINUTE_SECOND 60


#define CLOCK_MONDAY    0x01U
#define CLOCK_TUESDAY   0x02U
#define CLOCK_WEDNESDAY 0x04U
#define CLOCK_THURSDAY  0x08U
#define CLOCK_FRIDAY    0x10U
#define CLOCK_SATURDAY  0x20U
#define CLOCK_SUNDAY    0x40U

#define CLOCK_EVERY_WEEK (\
CLOCK_MONDAY    | \
CLOCK_TUESDAY   | \
CLOCK_WEDNESDAY | \
CLOCK_THURSDAY  | \
CLOCK_FRIDAY    | \
CLOCK_SATURDAY  | \
CLOCK_SUNDAY      \
)

#define CLOCK_IN_WEEK(clock,_weeks) (((clock)->week & (_weeks)) != 0)

typedef struct tagClock_t
{
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    // 按位计算，例如bit0为1表示周一使能。只在用作闹钟时间设置时设置多位。
    uint8_t week; 
} Clock_t;

uint32_t Clock_to_sec(Clock_t *clock);
void Clock_from_sec(Clock_t *clock, uint32_t sec);
#define Clock_cmp(clockA, clockB) Clock_minus((clockA), (clockB))
int32_t Clock_minus(Clock_t *clockA, Clock_t *clockB);
void Clock_now(Clock_t *clock);

typedef void (*AlarmClockCallback_t)(void);

typedef struct tagAlarmClock_t
{
    uint8_t executed;         // 0今天没有执行过，1今天执行过了
    Clock_t clock;            // 执行的日期时间，精确到秒
    AlarmClockCallback_t fun; // 任务
    struct sc_list next;      // for self
} AlarmClock_t;

void AlarmClock_init();
void AlarmClock_obj_init(AlarmClock_t *palarm_clock, AlarmClockCallback_t fun, uint8_t hour, uint8_t minute, uint8_t second, uint8_t week);
void AlarmClock_start(AlarmClock_t *palarm_clock);
void AlarmClock_stop(AlarmClock_t *palarm_clock);
void AlarmClock_handler();

#endif //! ALARM_CLOCK_H