/**
 * @file HDL_RTC.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-10-07
 *
 * @copyright Copyright (c) 2022 Liu Yuanlin Personal.
 *
 */

/**
 * @brief 没有时区概念在这个RTC库中，这里RTC的时间时本地时间还是UTC时间有用户自己决定。
 *
 */
// 存在一定的问题，ST的这个RTC没有一个单独的秒寄存器，就是一个DateTime日历，
// 对于闰年的处理取决于RTC_BASE_YEAR,但是RTC_BASE_YEAR手册中却从来没有提及。

#include "HDL_RTC.h"
#include "mytime.h"

#define RTC_TR_RESERVED_MASK (RTC_TR_PM | RTC_TR_HT | RTC_TR_HU |   \
                              RTC_TR_MNT | RTC_TR_MNU | RTC_TR_ST | \
                              RTC_TR_SU)
#define RTC_DR_RESERVED_MASK (RTC_DR_YT | RTC_DR_YU | RTC_DR_WDU | \
                              RTC_DR_MT | RTC_DR_MU | RTC_DR_DT |  \
                              RTC_DR_DU)

/* rtc base year , it is crucial to judge leap years */
#define RTC_BASE_YEAR 2000U
#define RTC_BASE_MONTH 1U
#define RTC_BASE_DAY 1U
#define RTC_BASE_HOUR 0U
#define RTC_BASE_MINUTE 0U
#define RTC_BASE_SECOND 0U

#define RTC_ERROR_NONE 0
__IO uint64_t rtc_base_year_timestamp = 0;

#define HDL_RTC_CLOCK_SOURCE_LSI 1

#if HDL_RTC_CLOCK_SOURCE_LSI == 1
#define HDL_RTC_CLOCK LSI_VALUE
#else
#define HDL_RTC_CLOCK LSE_VALUE
#endif

/**
 * @brief RTC外设初始化。启动RTC，但是不会设置RTC时间。默认LSE时钟源。
 * 设置/获取时间默认使用BIN格式的时间。
 *
 */
void HDL_RTC_Init()
{
    LL_RTC_InitTypeDef RTC_InitStruct = {0};
    mytime_struct setTime = {0};
    setTime.nYear = RTC_BASE_YEAR;
    setTime.nMonth = RTC_BASE_MONTH;
    setTime.nDay = RTC_BASE_DAY;
    setTime.nHour = RTC_BASE_HOUR;
    setTime.nMin = RTC_BASE_MINUTE;
    setTime.nSec = RTC_BASE_SECOND;
    setTime.wSub = 0;

    rtc_base_year_timestamp = mytime_2_utc_sec(&setTime);
    // 使能后备寄存器访问
    /* Update LSE configuration in Backup Domain control register */
    /* Requires to enable write access to Backup Domain if necessary */
    if (LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_PWR) != 1U)
    {
        /* Enables the PWR Clock and Enables access to the backup domain */
        // 打开RTC后备寄存器电源时钟
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    }

    if (LL_PWR_IsEnabledBkUpAccess() != 1U)
    {
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_BKP);
        /* Enable write access to Backup domain */
        // 打开后备寄存器访问
        LL_PWR_EnableBkUpAccess();
        while (LL_PWR_IsEnabledBkUpAccess() == 0U)
        {
        }
    }

    LL_RCC_ForceBackupDomainReset();
    LL_RCC_ReleaseBackupDomainReset();
#if HDL_RTC_CLOCK_SOURCE_LSI == 0
    // 使能LSE
    LL_RCC_LSE_Enable();
    /* Wait till LSE is ready */
    while (LL_RCC_LSE_IsReady() != 1)
    {
    }

    // 设置RTC时钟源为LSE
    if (LL_RCC_GetRTCClockSource() != LL_RCC_RTC_CLKSOURCE_LSE)
    {
        LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);

        /* Restore clock configuration if changed */
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_PWR);
    }
#else
    // 使能LSI
    LL_RCC_LSI_Enable();
    /* Wait till LSI is ready */
    while (LL_RCC_LSI_IsReady() != 1)
    {
    }

    // 设置RTC时钟源为LSI
    if (LL_RCC_GetRTCClockSource() != LL_RCC_RTC_CLKSOURCE_LSI)
    {
        LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);

        /* Restore clock configuration if changed */
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_PWR);
    }
#endif

    // 清除RSF位，防止例如从低功耗模式（停机模式或待机模式）唤醒之后带来的不同步。当然是对于等待RSF位的程序才会。
    LL_RTC_ClearFlag_RS(RTC);

    // BYPSHAD = 1可以直接读取，但是如果在对寄存器的两次读访问之间出现 RTCCLK 沿，则不同寄存器的结果彼此可能不一致
    //		LL_RTC_DisableWriteProtection(RTC);
    //		LL_RTC_EnableShadowRegBypass(RTC);
    //		LL_RTC_EnableWriteProtection(RTC);

    /*##-1- Enable RTC peripheral Clocks #######################################*/
    /* Enable RTC Clock */
    LL_RCC_EnableRTC();

// 初始化RTC
#if HDL_RTC_CLOCK_SOURCE_LSI == 1
    RTC_InitStruct.AsynchPrescaler = HDL_RTC_CLOCK - 1; // sub-second = 1 sec / 40000
#else
    RTC_InitStruct.AsynchPrescaler = (32 - 1) * (RTC_SUBSEC_MAX - 1); // sub-second = 1 sec / RTC_SUBSEC_MAX
#endif
    LL_RTC_Init(RTC, &RTC_InitStruct);

    /*##-2- Disable RTC registers write protection ##############################*/
    LL_RTC_DisableWriteProtection(RTC);

    /*##-3- Enter in initialization mode #######################################*/

    /* Initialization Error */
    while (LL_RTC_EnterInitMode(RTC) != RTC_ERROR_NONE)
    {
    }

#define BAK_VALUE 0x32F1
    if (LL_RTC_BKP_GetRegister(BKP, LL_RTC_BKP_DR1) != BAK_VALUE)
    {
        setTime.nYear = 2022;
        setTime.nMonth = 12;
        setTime.nDay = 14;
        setTime.nMin = 33;
        setTime.nHour = 15;
        setTime.nSec = 0;
        setTime.wSub = 0;
        HDL_RTC_SetStructTime(&setTime);
        LL_RTC_BKP_SetRegister(BKP, LL_RTC_BKP_DR1, BAK_VALUE);
    }

    /*##-6- Exit of initialization mode #######################################*/
    LL_RTC_ExitInitMode(RTC);

    /*##-7- Enable RTC registers write protection #############################*/
    LL_RTC_EnableWriteProtection(RTC);
}

/**
 * @brief 获取本地1970-1-1以来总秒数
 *
 * @param pSub 用于获取亚秒数。
 * @return uint32_t
 */
uint64_t HDL_RTC_GetTimeTick(uint16_t *pSub)
{
    uint64_t timestamp = 0;
    timestamp = LL_RTC_TIME_Get(RTC) + rtc_base_year_timestamp;
    if (pSub != NULL)
    {
        *pSub = 0;
    }
    return timestamp;
}

/**
 * @brief 获取mytime_struct类型的时间。
 *
 * @param myTime
 */
void HDL_RTC_GetStructTime(mytime_struct *myTime)
{
    uint32_t timestamp = 0;
    timestamp = HDL_RTC_GetTimeTick(NULL);
    utc_sec_2_mytime(timestamp, myTime);
}

/**
 * @brief 使用时间戳设置日历时间。
 *
 * @param unix_ts_sec 1970-1-1以来总秒数。必须大于rtc_base_year_timestamp。参考RTC_BASE_YEAR。
 *
 */
void HDL_RTC_SetTimeTick(uint64_t unix_ts_sec)
{
    if (unix_ts_sec >= rtc_base_year_timestamp)
    {
        LL_RTC_TIME_SetCounter(RTC, unix_ts_sec - rtc_base_year_timestamp);
    }
}

/**
 * @brief 使用mytime_struct对象来设置时间。被设置的时间必须大于等于RTC_BASE_YEAR
 * 否则没有效果。设置时间的时间精度为秒。
 *
 * @param myTime
 */
void HDL_RTC_SetStructTime(mytime_struct *myTime)
{
    uint64_t timestamp = 0;
    timestamp = mytime_2_utc_sec(myTime);
    HDL_RTC_SetTimeTick(timestamp);
}