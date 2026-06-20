/**
 * @file Board_Fan.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2025-12-26
 * @last modified 2025-12-26
 *
 * @copyright Copyright (c) 2025 Liu Yuanlin Personal.
 *
 */
#include "Board_Fan.h"
#include "HDL_Freq_Measure.h"
#include "asyn_sys.h"
#include "mtime.h"
#include "HDL_CPU_Time.h"
#include "pid_fp32.h"
#include "HDL_PWM.h"

typedef struct {
    uint32_t rpm_set;
    uint32_t rpm_current; // 当前转速
    PeriodREC_t _poll_rec;
    PluseCnt_t _last_pluse_cnt;
    uint32_t last_check_moment;
    bool check_inited;
    Inc_PID_FP32_t pid;
} Board_Fan_t;

static Board_Fan_t fan = {0};

static void Board_Fan_Poll();

void Board_Fan_Init()
{
    HDL_Freq_Measure_Init(FREQ_MEASURE_1);
    Board_Set_Fan_Speed(0);
    HDL_PWM_Init(PWM_2);
    HDL_PWM_SetDuty(PWM_2, 0.5f); //Fan
    asyn_sys_register(Board_Fan_Poll);
}

/**
 * @brief
 *
 * @param rpm 转速，单位RPM
 */
void Board_Set_Fan_Speed(uint32_t rpm)
{
    fan.rpm_set    = rpm;
    fan.pid.target = fan.rpm_set;
}

static void Board_Fan_Poll()
{
#define POLL_PERIOD_MS (800)
    static int cnt = 0;
    if (period_query_user(&fan._poll_rec, MS_TO_TICK(POLL_PERIOD_MS))) {
        if(cnt ++ % 2)
        {
         HDL_PWM_SetDuty(PWM_1, 1.0f);
        }else
         HDL_PWM_SetDuty(PWM_1, 0.0f);
        
        if (!fan.check_inited) {
            // 初始化check
            fan.check_inited      = true;
            fan.last_check_moment = getCPUTick();
            fan._last_pluse_cnt   = HDL_Freq_Measure_GetPlusCount(FREQ_MEASURE_1);
            Inc_PID_FP32_Init(&fan.pid);
            pInc_PID_FP32_t pPID = &fan.pid;
            pPID->kp             = 0.002f;
            pPID->ki             = 0.01f;
            pPID->kd             = 0.0f;
            pPID->outputMax      = 1.0f;
            pPID->outputMin      = 0.0f;
        } else {
            PluseCnt_t pluse_cnt_diff = HDL_Freq_Measure_GetPlusCount(FREQ_MEASURE_1) - fan._last_pluse_cnt;
            fan._last_pluse_cnt       = HDL_Freq_Measure_GetPlusCount(FREQ_MEASURE_1);
            uint32_t time_diff_ms     = getCPUTick() - fan.last_check_moment;
            fan.last_check_moment     = getCPUTick();
            fan.rpm_current           = pluse_cnt_diff * 60 * 1000 * 30 / time_diff_ms;
            fan.pid.sampling          = fan.rpm_current;
            Inc_PID_FP32_Update_AddDelta(&fan.pid);
            // 设置占空比
        }
    }
}