#include "ameba_soc.h"
#include "ameba_rtc.h"

static const char *TAG = "RTC";

// IRQ_FUN RTC_AlarmIRQHandler;
// void *RTC_AlarmIRQData;

// void RTC_IRQHandler(void)
// {
//     // RTC_INTClear();

//     if (RTC_AlarmIRQHandler != NULL) {
//         RTC_AlarmIRQHandler(RTC_AlarmIRQData);
//     }

// }

void RTC_IRQInit(IRQ_FUN irq_func, void *irq_data)
{
	InterruptRegister(irq_func, RTC_IRQ, (u32)irq_data, INT_PRI_MIDDLE);
	InterruptEn(RTC_IRQ, INT_PRI_MIDDLE);
}


void RTC_IRQDeInit(void)
{
	RTC_INTClear();

	InterruptDis(RTC_IRQ);
	InterruptUnRegister(RTC_IRQ);
}

u32 RTC_WaitReady(void)
{
	AON_TypeDef *AON = (AON_TypeDef *)AON_BASE;

	u32 i;

	for (i = 0; i < 1000000; i++) {
		if ((AON->AON_LSFIF_CMD & AON_BIT_AON_LSFIF_POLL) == 0) {
			return TRUE;
		} else {
			DelayMs(1);
		}
	}

	RTK_LOGW(TAG, "RTC indirect RW not done\r\n");

	return FALSE;
}

void RTC_Write(u16 addr, u32 data)
{
	AON_TypeDef *AON = (AON_TypeDef *)AON_BASE;

	AON->AON_LSFIF_CMD = AON_LSF_SEL(0);

	AON->AON_LSFIF_RWD = data;
	AON->AON_LSFIF_CMD |= AON_MASK_AON_LSFIF_WE;
	AON->AON_LSFIF_CMD |= AON_BIT_AON_LSFIF_WR;
	AON->AON_LSFIF_CMD |= AON_AON_LSFIF_AD(addr);
	AON->AON_LSFIF_CMD |= AON_BIT_AON_LSFIF_POLL;

	RTC_WaitReady();
}

u32 RTC_Read(u16 addr)
{
	AON_TypeDef *AON = (AON_TypeDef *)AON_BASE;
	u32 data;

	AON->AON_LSFIF_CMD = AON_LSF_SEL(0);

	AON->AON_LSFIF_CMD &= ~(AON_MASK_AON_LSFIF_WE);
	AON->AON_LSFIF_CMD &= ~(AON_BIT_AON_LSFIF_WR);
	AON->AON_LSFIF_CMD |= AON_AON_LSFIF_AD(addr);
	AON->AON_LSFIF_CMD |= AON_BIT_AON_LSFIF_POLL;

	RTC_WaitReady();

	data = AON->AON_LSFIF_RWD;

	return data;
}

/**
  * @brief  Set the RTC current time.
  * @param  RTC_Format: not used in pro3
  * @param  RTC_TimeStruct: pointer to a RTC_TimeTypeDef structure that contains
  *                        the time configuration information for the RTC.
  * @retval None
  * @note	disable rtc -> set time -> enable rtc
  */
void RTC_SetTime(u32 RTC_Format, RTC_TimeTypeDef *RTC_TimeStruct)
{
	(void)RTC_Format;
	assert_param(IS_RTC_HOUR24(RTC_TimeStruct->RTC_Hours));
	assert_param(IS_RTC_MINUTES(RTC_TimeStruct->RTC_Minutes));
	assert_param(IS_RTC_SECONDS(RTC_TimeStruct->RTC_Seconds));

	assert_param(IS_RTC_YEAR(RTC_TimeStruct->RTC_Year));
	assert_param(IS_RTC_MONTH(RTC_TimeStruct->RTC_Month + 1));
	assert_param(IS_RTC_DOY(RTC_TimeStruct->RTC_Days + 1));
	assert_param(IS_RTC_DOM(RTC_TimeStruct->RTC_DayOfMonth));
	assert_param(IS_RTC_DOW(RTC_TimeStruct->RTC_DayOfWeek));

	assert_param(IS_RTC_H12_AMPM(RTC_TimeStruct->RTC_H12_PMAM));

	u32 data = 0;
	u8 is_leap_year = 0;

	data = RTC_Read(Rtc_Fen);
	data &= ~(RTC_BIT_CKDIV_EN | RTC_BIT_RTC_EN);
	RTC_Write(Rtc_Fen, data);

	DelayUs(10);

	data = 0;
	data |= RTC_RTC_DOW(RTC_TimeStruct->RTC_DayOfWeek);
	data |= RTC_RTC_HRS(RTC_TimeStruct->RTC_Hours);
	data |= RTC_RTC_MIN(RTC_TimeStruct->RTC_Minutes);
	data |= RTC_RTC_SEC(RTC_TimeStruct->RTC_Seconds);
	RTC_Write(Rtc_Tim0, data);

	DelayUs(10);

	data = 0;
	data |= RTC_RTC_YEAR(RTC_TimeStruct->RTC_Year);
	data |= RTC_RTC_MON(RTC_TimeStruct->RTC_Month + 1);
	data |= RTC_RTC_DOM(RTC_TimeStruct->RTC_DayOfMonth);
	RTC_Write(Rtc_Tim1, data);

	DelayUs(10);

	data = 0;
	IS_LEAP_YEAR_CHECK((RTC_TimeStruct->RTC_Year), is_leap_year);
	RTC_TimeStruct->RTC_IsLeapYear = is_leap_year;

	if (is_leap_year) {
		data &= ~(RTC_BIT_RTC_M4ISCYR);
	} else {
		data |= RTC_BIT_RTC_M4ISCYR;
	}

	data |= RTC_RTC_DOY(RTC_TimeStruct->RTC_Days + 1);
	RTC_Write(Rtc_Tim2, data);

	DelayUs(10);

	data = RTC_Read(Rtc_Fen);
	data |= (RTC_BIT_CKDIV_EN | RTC_BIT_RTC_EN);       // clock gate
	RTC_Write(Rtc_Fen, data);
}

/**
  * @brief  Get the RTC current Time.
  * @param  RTC_Format: not used in pro3
  * @param  RTC_TimeStruct: pointer to a RTC_TimeTypeDef structure that will
  *                        contain the returned current time configuration.
  * @retval None
  */
void RTC_GetTime(u32 RTC_Format, RTC_TimeTypeDef *RTC_TimeStruct)
{
	u32 data;
	u32 year;

	(void)RTC_Format;
	data = RTC_Read(Rtc_Tim0);
	RTC_TimeStruct->RTC_DayOfWeek = RTC_GET_RTC_DOW(data);
	RTC_TimeStruct->RTC_Hours = RTC_GET_RTC_HRS(data);
	RTC_TimeStruct->RTC_Minutes = RTC_GET_RTC_MIN(data);
	RTC_TimeStruct->RTC_Seconds = RTC_GET_RTC_SEC(data);

	data = RTC_Read(Rtc_Tim1);
	RTC_TimeStruct->RTC_Year = RTC_GET_RTC_YEAR(data);
	RTC_TimeStruct->RTC_Month = RTC_GET_RTC_MON(data) - 1;
	RTC_TimeStruct->RTC_DayOfMonth = RTC_GET_RTC_DOM(data);

	year = RTC_GET_RTC_YEAR(data);
	IS_LEAP_YEAR_CHECK(year, RTC_TimeStruct->RTC_IsLeapYear);

	data = RTC_Read(Rtc_Tim2);
	RTC_TimeStruct->RTC_Days = RTC_GET_RTC_DOY(data) - 1;

}

/**
  * @brief  Set the specified RTC Alarm.
  * @note   The Alarm register can only be written when the corresponding Alarm
  *         is disabled (Use the RTC_AlarmCmd(DISABLE)). Below info is required:
  * 			RTC_AlarmTime.RTC_Hours
  * 			RTC_AlarmTime.RTC_Minutes
  * 			RTC_AlarmTime.RTC_Seconds
  * 			RTC_AlarmTime.RTC_Days
  * @param  RTC_Format: not used in pro3
  * @param  RTC_AlarmStruct: pointer to a RTC_AlarmTypeDef structure that
  *                          contains the alarm configuration parameters.
  * @retval None
  */
void RTC_SetAlarm(u32 RTC_Format, RTC_AlarmTypeDef *RTC_AlarmStruct)
{
	(void)RTC_Format;
	assert_param(IS_RTC_HOUR24(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours));
	assert_param(IS_RTC_MINUTES(RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes));
	assert_param(IS_RTC_SECONDS(RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds));
	assert_param(IS_RTC_DOY(RTC_AlarmStruct->RTC_AlarmTime.RTC_Days + 1));

	u32 data;

	data = 0;
	data |= RTC_RTC_ALRM_DOW(RTC_AlarmStruct->RTC_AlarmTime.RTC_DayOfWeek);
	data |= RTC_RTC_ALRM_HRS(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours);
	data |= RTC_RTC_ALRM_MIN(RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes);
	data |= RTC_RTC_ALRM_SEC(RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds);
	RTC_Write(Rtc_Ctrl0, data);

	data = 0;
	data |= RTC_RTC_ALRM_YEAR(RTC_AlarmStruct->RTC_AlarmTime.RTC_Year);
	data |= RTC_RTC_ALRM_MON(RTC_AlarmStruct->RTC_AlarmTime.RTC_Month + 1);
	data |= RTC_RTC_ALRM_DOM(RTC_AlarmStruct->RTC_AlarmTime.RTC_DayOfMonth);
	RTC_Write(Rtc_Ctrl1, data);

	data = 0;
	data |= RTC_RTC_ALRM_DOY(RTC_AlarmStruct->RTC_AlarmTime.RTC_Days + 1);
	RTC_Write(Rtc_Ctrl2, data);

}

/**
  * @brief  Get the RTC Alarm value and masks.
  * @param  RTC_Format: not used in pro3
  * @param  RTC_AlarmStruct: pointer to a RTC_AlarmTypeDef structure that will
  *                          contains the output alarm configuration values.
  * @retval None
  */
void RTC_GetAlarm(u32 RTC_Format, RTC_AlarmTypeDef *RTC_AlarmStruct)
{
	u32 data;

	(void)RTC_Format;
	data = RTC_Read(Rtc_Ctrl0);
	RTC_AlarmStruct->RTC_AlarmTime.RTC_DayOfWeek = RTC_GET_RTC_ALRM_DOW(data);
	RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours = RTC_GET_RTC_ALRM_HRS(data);
	RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes = RTC_GET_RTC_ALRM_MIN(data);
	RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds = RTC_GET_RTC_ALRM_SEC(data);

	data = RTC_Read(Rtc_Ctrl1);
	RTC_AlarmStruct->RTC_AlarmTime.RTC_Year = RTC_GET_RTC_ALRM_YEAR(data);
	RTC_AlarmStruct->RTC_AlarmTime.RTC_Month = RTC_GET_RTC_ALRM_MON(data) - 1;
	RTC_AlarmStruct->RTC_AlarmTime.RTC_DayOfMonth = RTC_GET_RTC_ALRM_DOM(data);

	data = RTC_Read(Rtc_Ctrl2);
	RTC_AlarmStruct->RTC_AlarmTime.RTC_Days = RTC_GET_RTC_ALRM_DOY(data) - 1;

	data = RTC_Read(Rtc_Imr);
	RTC_AlarmStruct->RTC_AlarmMask = data;

}

/**
  * @brief  Set RTC Compensation value
  * @param  dir: adjust direction
  *         This parameter can be one of the following values:
  *             @arg RTC_CMP_ADJUST_DIR_PLUS
  *             @arg RTC_CMP_ADJUST_DIR_MINUS
  * @param  time_in_sec: +/- 1 second for every time_in_sec set
  * @param  NewState: function enable / disable
  *         This parameter can be one of the following:
  *             @arg ENABLE
  *             @arg DISABLE
  * @retval None
  */
void RTC_SetCompensation(u32 dir, u32 time_in_sec, u32 NewState)
{
	assert_param(IS_RTC_CMP_ADJUST_DIR(dir));

	u32 data = 0;

	if (NewState == ENABLE) {

		data |= RTC_BIT_FUNCTION_EN;

		if (dir == RTC_CMP_ADJUST_DIR_PLUS) {
			data &= ~(RTC_BIT_ADJUST_DIRECTION);
		} else {
			data |= RTC_BIT_ADJUST_DIRECTION;
		}

		data |= RTC_RTC_ADJUST_PERIOD_SEL(time_in_sec);

	} else {
		data &= ~(RTC_BIT_FUNCTION_EN);
	}

	RTC_Write(Rtc_Cmp, data);

}

void RTC_SetCntAlarm(u8 cnt_alarm)
{
	u32 data;

	data = 0;
	RTC_Write(Rtc_Ctrl2, data);

	data = 0;
	data |= cnt_alarm;
	RTC_Write(Rtc_Imr, data);

}

void RTC_Enable(u8 NewState)
{
	u32 data;

	if (NewState == ENABLE) {

		data = (RTC_BIT_RTC_EN | RTC_BIT_CKDIV_EN);
		RTC_Write(Rtc_Fen, data);

	} else {

		data = 0;
		RTC_Write(Rtc_Fen, data);

	}

}

u32 RTC_GetINTStatus(void)
{
	u32 data;

	data = RTC_Read(Rtc_Isr);

	return data;
}

void RTC_INTClear(void)
{
	u32 data;

	// write 1 clear, clears all alarm
	data = RTC_Read(Rtc_Isr);
	RTC_Write(Rtc_Isr, data);
}

void RTC_TimeStructInit(RTC_TimeTypeDef *RTC_TimeStruct)
{
	RTC_TimeStruct->RTC_H12_PMAM = RTC_H12_AM;
	RTC_TimeStruct->RTC_Year = 0;
	RTC_TimeStruct->RTC_Month = 0;
	RTC_TimeStruct->RTC_DayOfMonth = 1;
	RTC_TimeStruct->RTC_Hours = 0;
	RTC_TimeStruct->RTC_Minutes = 0;
	RTC_TimeStruct->RTC_Seconds = 0;
	RTC_TimeStruct->RTC_DayOfWeek = 0;
	RTC_TimeStruct->RTC_Days = 0;
}

void RTC_Init(RTC_InitTypeDef *RTC_InitStruct)
{
	(void)RTC_InitStruct;
	// RTC_IRQInit();
	// RTC_Enable();
}

void RTC_AlarmStructInit(RTC_AlarmTypeDef *RTC_AlarmStruct)
{
	(void) RTC_AlarmStruct;
}

void RTC_StructInit(RTC_InitTypeDef *RTC_InitStruct)
{
	(void) RTC_InitStruct;
}

void RTC_AlarmCmd(u32 NewState)
{
	u32 data = 0;

	if (NewState == ENABLE) {

		data = RTC_Read(Rtc_Ctrl2);
		data |= RTC_BIT_RTC_ALRM_SEC_EN;
		data |= RTC_BIT_RTC_ALRM_MIN_EN;
		data |= RTC_BIT_RTC_ALRM_HRS_EN;
		data |= RTC_BIT_RTC_ALRM_DOY_EN;
		RTC_Write(Rtc_Ctrl2, data);

		data = 0;
		data |= RTC_BIT_ALRM_IMR;
		RTC_Write(Rtc_Imr, data);

	} else {

		data = RTC_Read(Rtc_Ctrl2);
		data &= ~(RTC_BIT_RTC_ALRM_SEC_EN);
		data &= ~(RTC_BIT_RTC_ALRM_MIN_EN);
		data &= ~(RTC_BIT_RTC_ALRM_HRS_EN);
		data &= ~(RTC_BIT_RTC_ALRM_DOY_EN);
		RTC_Write(Rtc_Ctrl2, data);

		data = RTC_Read(Rtc_Imr);
		data &= ~(RTC_BIT_ALRM_IMR);
		RTC_Write(Rtc_Imr, data);

	}

}

void RTC_AlarmClear(void)
{
	u32 data;

	// write 1 clear, clears alarm only
	data = RTC_Read(Rtc_Isr);

	data &= (RTC_BIT_IT_ALARM_ALL_STS);

	RTC_Write(Rtc_Isr, data);
}

void RTC_CntAlarmClear(void)
{
	u32 data;

	// write 1 clear, clears cnt alarm only
	data = RTC_Read(Rtc_Isr);

	data &= (RTC_BIT_IT_CNT_ALARM_ALL_STS);

	RTC_Write(Rtc_Isr, data);
}
