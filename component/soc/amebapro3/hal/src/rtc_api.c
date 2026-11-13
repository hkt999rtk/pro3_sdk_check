/** mbed Microcontroller Library
 ******************************************************************************
 * @file	rtc_api.c
 * @author
 * @version V1.0.0
 * @date	2016-08-01
 * @brief	This file provides mbed API for RTC.
 ******************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
 ******************************************************************************
 */

#include "rtc_api.h"
#include <time.h>

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_RTC
 *  @brief      MBED_RTC driver modules.
 *  @{
 */

/* group: MBED_RTC Exported Types */
/* Since all the members in the group are static which will not be extracted to the doxygen doc,
  original comment has been deleted to avoid there is nothing displayed under the group. */

static int rtc_en = 0;
static alarm_irq_handler rtc_alarm_handler;

//static const u8 dim[12] = {
//	31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
//};
/* end of the group */

/* group MBED_RTC_Exported_Functions MBED_RTC Exported Functions */
/* Since static function in the group should not be extracted to the doxygen doc */

/**
  * @brief  Judge whether a year is a leap year or not.
  * @param  year: Actual year - 1900.
  * @return Result.
  * @retval 1: This year is a leap year.
  * @retval 0: This year is not a leap year.
  */
static inline bool is_leap_year(unsigned int year)
{
	u32 full_year = year + 1900;
	return (!(full_year % 4) && (full_year % 100)) || !(full_year % 400);
}

#if 0//fix warning
/**
  * @brief  Calculate total days in a specified month of a specified year.
  * @param  year: Actual year - 1900.
  * @param  month: Specified the number of months from  a specified year.
  * @note 0 represents January.
  * @return Number of days in the month of the year.
  */
static u8 days_in_month(u8 month, u8 year)
{
	u8 ret = dim[month % 12];
	if (ret == 0) {
		ret = is_leap_year(year + month / 12) ? 29 : 28;
	}
	return ret;
}

/**
  * @brief  Calculate month and day of the month according to year and day of the year.
  * @param  year: Actual year - 1900.
  * @param  yday: Day of the year.
  * @param  mon: Pointer to the variable that stores month, which can be 0~11.
  * @note 0 represents January.
  * @param  mday: Pointer to the variable that stores day of month, which can be 1~31.
  * @retval none
  */
static void rtc_calculate_mday(int year, int yday, int *mon, int *mday)
{
	int t_mon = -1, t_yday = yday + 1;

	while (t_yday > 0) {
		t_mon ++;
		t_yday -= days_in_month(t_mon, year);
	}

	*mon = t_mon;
	*mday = t_yday + days_in_month(t_mon, year);
}

/**
  * @brief  Calculate the day of a week according to date.
  * @param  year: Actual year - 1900.
  * @param  mon: Month of the year, which can be 0~11.
  * @note 0 represents January.
  * @param  mday: Day of the month.
  * @param  wday: Pointer to the variable that stores day of a week, which can be 0~6.
  * @note 0 represents Sunday.
  * @retval none
  */
static void rtc_calculate_wday(int year, int mon, int mday, int *wday)
{
	int t_year = year + 1900, t_mon = mon + 1;

	if (t_mon == 1 || t_mon == 2) {
		t_year --;
		t_mon += 12;
	}

	int c = t_year / 100;
	int y = t_year % 100;
	int week = (c / 4) - 2 * c + (y + y / 4) + (26 * (t_mon + 1) / 10) + mday - 1;

	while (week < 0) {
		week += 7;
	}
	week %= 7;

	*wday = week;
}
#endif
/* end of the group */


/** @defgroup MBED_RTC_Exported_Functions MBED_RTC Exported Functions
  * @{
  */

/**
  * @brief  Initialize the RTC device, including clock, function and RTC registers.
  * @param  none
  * @retval  none
  */
void rtc_init(void)
{
	RCC_PeriphClockCmd(APBPeriph_RTC, APBPeriph_RTC_CLOCK, ENABLE);

	// todo compensation
	// todo not sure should can enable or not
	//RTC_Enable();

	rtc_en = 1;
}

/**
  * @brief  Deinitialize the RTC device.
  * @param  none
  * @retval  none
  */
void rtc_free(void)
{
	rtc_en = 0;
	rtc_alarm_handler = NULL;
}

/**
  * @brief  Judge whether RTC is enabled or not.
  * @param  none
  * @return RTC status. It can be one of the following values:
  * @retval 1: RTC has been enabled.
  * @retval 0: RTC has not been enabled.
  */
int rtc_isenabled(void)
{
	return rtc_en;
}

/**
  * @brief  Set the specified timestamp in seconds to RTC.
  * @param  t: Seconds from 1970.1.1 00:00:00 to specified data and time which is to be set.
  * @retval none
  */
void rtc_write(time_t t)
{
	struct tm *readable_time;
	RTC_TimeTypeDef RTC_TimeStruct;

	readable_time = gmtime(&t);

	RTC_TimeStruct.RTC_Seconds = readable_time->tm_sec;
	RTC_TimeStruct.RTC_Minutes = readable_time->tm_min;
	RTC_TimeStruct.RTC_Hours = readable_time->tm_hour;
	RTC_TimeStruct.RTC_DayOfMonth = readable_time->tm_mday;
	RTC_TimeStruct.RTC_DayOfWeek = readable_time->tm_wday;
	RTC_TimeStruct.RTC_Days = readable_time->tm_yday;
	RTC_TimeStruct.RTC_Month = readable_time->tm_mon;
	RTC_TimeStruct.RTC_Year = readable_time->tm_year + RTC_BASE_YEAR;
	//RTC_TimeStruct.RTC_IsLeapYear = is_leap_year(RTC_TimeStruct.RTC_Year);

	RTC_Enable(DISABLE);
	RTC_SetTime(0, &RTC_TimeStruct);
	RTC_Enable(ENABLE);
}

/**
  * @brief  Get current timestamp in seconds from RTC.
  * @param  none
  * @return Current timestamp in seconds which is calculated from 1970.1.1 00:00:00.
  */
time_t rtc_read(void)
{
	struct tm t;
	u32 seconds;
	RTC_TimeTypeDef RTC_TimeStruct;

	RTC_GetTime(0, &RTC_TimeStruct);

	t.tm_sec = RTC_TimeStruct.RTC_Seconds;
	t.tm_min = RTC_TimeStruct.RTC_Minutes;
	t.tm_hour = RTC_TimeStruct.RTC_Hours;
	t.tm_mday = RTC_TimeStruct.RTC_DayOfMonth;
	t.tm_wday = RTC_TimeStruct.RTC_DayOfWeek;
	t.tm_yday = RTC_TimeStruct.RTC_Days;
	t.tm_mon = RTC_TimeStruct.RTC_Month;
	t.tm_year = RTC_TimeStruct.RTC_Year - RTC_BASE_YEAR;

	seconds = mktime(&t);
	//DiagPrintf("[epoch_time %u ]\r\n", seconds);

	// DiagPrintf("%u %u %u, %u.%u.%u\r\n", RTC_TimeStruct.RTC_Year + RTC_BASE_YEAR, RTC_TimeStruct.RTC_Month + 1, RTC_TimeStruct.RTC_DayOfMonth,
	// 		   RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes, RTC_TimeStruct.RTC_Seconds);
	// DiagPrintf("yday %u\r\n", RTC_TimeStruct.RTC_Days);
	// DiagPrintf("is leap year = %u\r\n", RTC_TimeStruct.RTC_IsLeapYear);

	return seconds;
}

/**
  * @brief  Disable RTC Alarm.
  * @param  none
  * @retval   none
  */
void rtc_disable_alarm(void)
{
	RTC_IRQDeInit();

	rtc_alarm_handler = NULL;
}

/**
  * @brief  Register RTC alarm interrupt handler.
  * @param  data: RTC IRQ callback data.
  * @retval 0
  */
u32 rtc_alarm_intr_handler(void *data)
{
	/* To avoid gcc warnings */
	(void) data;

	alarm_irq_handler hdl;

	/*clear alarm flag*/
	RTC_INTClear();

	/* execute user handler*/
	if (rtc_alarm_handler != NULL) {
		hdl = (alarm_irq_handler)rtc_alarm_handler;
		hdl();
	}

	/*disable alarm*/
	rtc_disable_alarm();

	return 0;
}

/**
  * @brief  Set the specified RTC Alarm and interrupt.
  * @param  alarm: Alarm object defined in application software.
  * @param  alarmHandler:  Alarm interrupt callback function.
  * @return Status. It can be one of the following values:
  * @retval 1: Success.
  * @retval Others: Error.
  */
u32 rtc_set_alarm(alarm_t *alrm, alarm_irq_handler alarmHandler)
{
	RTC_AlarmTypeDef RTC_AlarmStruct_temp;

	rtc_alarm_handler = alarmHandler;

	/* set alarm */
	//RTC_TimeStructInit(&RTC_AlarmStruct_temp);
	RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_Days = alrm->yday;
	RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_Hours = alrm->hour;
	RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_Minutes = alrm->min;
	RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_Seconds = alrm->sec;

	// RTC_AlarmStruct.RTC_AlarmTime.RTC_DayOfMonth = t->tm_mday;
	// RTC_AlarmStruct.RTC_AlarmTime.RTC_DayOfWeek = t->tm_wday;
	// RTC_AlarmStruct.RTC_AlarmTime.RTC_Month = t->tm_mon;
	// RTC_AlarmStruct.RTC_AlarmTime.RTC_Year = t->tm_year

	RTC_SetAlarm(0, &RTC_AlarmStruct_temp);

	RTC_IRQInit(rtc_alarm_intr_handler, NULL);

	return TRUE;
}

/** @} */
/** @} */
/** @} */
