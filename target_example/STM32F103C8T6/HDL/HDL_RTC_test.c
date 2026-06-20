/**
 * @file HDL_RTC_test.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-10-07
 *
 * @copyright Copyright (c) 2022 Liu Yuanlin Personal.
 *
 */

#include "HDL_RTC_test.h"
#include "log.h"
#include "mytime.h"
void HDL_RTC_test()
{
	ulog_init_user();

	HDL_RTC_Init();
	mytime_struct setTime;
	mytime_struct datetime;
	mytime_struct utc_datetime;
	uint16_t subsec = 0;
	uint32_t timestamp = 0;
	uint8_t week = 0;

	setTime.nYear = 2022;
	setTime.nMonth = 11;
	setTime.nDay = 03;
	setTime.nHour = 18;
	setTime.nMin = 9;
	setTime.nSec = 0;
	setTime.wSub = 0;
	HDL_RTC_SetStructTime(&setTime);
	while (1)
	{
		HAL_Delay(1000 - 1);

		// 测试RTC_GetStructTime
		HDL_RTC_GetStructTime(&datetime);
		ULOG_INFO("\r\n");
		ULOG_INFO("The time get by HDL_RTC_GetStructTime:\r\n");
		ULOG_INFO("%04d-%02d-%02d %02d:%02d:%02d %02d\r\n", datetime.nYear, datetime.nMonth, datetime.nDay, datetime.nHour, datetime.nMin, datetime.nSec, datetime.wSub);
		week = applib_dt_dayindex(datetime.nYear, datetime.nMonth, datetime.nDay);
		ULOG_INFO("week : %u\r\n", (uint32_t)week);

		// 测试RTC_GetTimeTick
		timestamp = HDL_RTC_GetTimeTick(&subsec);
		ULOG_INFO("The timestamp get by HDL_RTC_GetTimeTick:\r\n");
		ULOG_INFO("timestamp = %u, subsec = %02d\r\n", timestamp, subsec);

		// 时区调整测试：假设RTC时间为本地时间
		utc_datetime = datetime;
		mytime_sub_hours(&utc_datetime, 8);
		ULOG_INFO("\r\n");
		ULOG_INFO("The time get by HDL_RTC_GetStructTime:\r\n");
		ULOG_INFO("%04d-%02d-%02d %02d:%02d:%02d %02d\r\n", utc_datetime.nYear, utc_datetime.nMonth,
				  utc_datetime.nDay, utc_datetime.nHour, utc_datetime.nMin, utc_datetime.nSec, utc_datetime.wSub);
		week = applib_dt_dayindex(utc_datetime.nYear, utc_datetime.nMonth, utc_datetime.nDay);
		ULOG_INFO("week : %u\r\n", (uint32_t)week);
	}
}

void HDL_RTC_leap_year_test()
{
	ulog_init_user();

	HDL_RTC_Init();
	mytime_struct setTime;
	mytime_struct datetime;
	uint16_t subsec = 0;
	uint32_t timestamp = 0;
	uint8_t week = 0;

	// CASE 1:0 [INFO]: 2000-03-01 00:00:00 01
	setTime.nYear = 2000;
	setTime.nMonth = 2;
	setTime.nDay = 29;
	setTime.nHour = 23;
	setTime.nMin = 59;
	setTime.nSec = 58;
	setTime.wSub = 0;

	// CASE 2: 2044-2-29
	// setTime.nYear = 2044;
	// setTime.nMonth = 2;
	// setTime.nDay = 28;
	// setTime.nHour = 23;
	// setTime.nMin = 59;
	// setTime.nSec = 57;
	// setTime.wSub = 0;
	HDL_RTC_SetStructTime(&setTime);

	int n = 4;
	while (n--)
	{
		HAL_Delay(1000 - 1);

		HDL_RTC_GetStructTime(&datetime);
		ULOG_INFO("\r\n");
		ULOG_INFO("%04d-%02d-%02d %02d:%02d:%02d %02d\r\n", datetime.nYear, datetime.nMonth, datetime.nDay, datetime.nHour, datetime.nMin, datetime.nSec, datetime.wSub);
		week = applib_dt_dayindex(datetime.nYear, datetime.nMonth, datetime.nDay);
		ULOG_INFO("week : %u\r\n", (uint32_t)week);
	}

	while (1)
	{
	}
}