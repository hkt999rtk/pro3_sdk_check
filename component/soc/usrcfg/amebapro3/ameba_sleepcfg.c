/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "ameba_soc.h"

#define RUN_WAKEUP_EVENT_SAMPLE 1

__weak uint32_t  pmc_event_init(uint32_t arg, void *Data)
{
	UNUSED(Data);
	UNUSED(arg);

	return 0;
}

__weak uint32_t  pmc_event_start(uint32_t arg, void *Data)
{
	UNUSED(Data);
	UNUSED(arg);

	return 0;
}

__weak uint32_t  pmc_event_handler(uint32_t arg, void *Data)
{
	UNUSED(Data);
	UNUSED(arg);


	return 0;
}

// AON GPIOA0
void pmc_aongpio0_isr(void *id, u32 event)
{
	(void)id;
	(void)event;

#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	DiagPrintf("pmc_aongpio0_isr\r\n");

#endif
#endif

}

__weak uint32_t  pmc_aongpio0_init(uint32_t arg, void *Data)
{
	UNUSED(Data);
	UNUSED(arg);
#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	/* GPIO init */
	GPIO_InitTypeDef GPIO_InitStruct_A0 = {0};
	GPIO_InitStruct_A0.GPIO_Pin = _PA_0;
	GPIO_InitStruct_A0.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct_A0.GPIO_Mode = GPIO_Mode_INT;
	GPIO_InitStruct_A0.GPIO_ITTrigger  = GPIO_INT_Trigger_EDGE;
	GPIO_InitStruct_A0.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_HIGH;
	GPIO_InitStruct_A0.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_DISABLE;
	GPIO_Init(&GPIO_InitStruct_A0);
	GPIO_UserRegIrq(_PA_0, pmc_aongpio0_isr, &GPIO_InitStruct_A0);
	GPIO_INTMode(_PA_0, ENABLE, GPIO_INT_Trigger_EDGE, GPIO_INT_POLARITY_ACTIVE_HIGH, GPIO_INT_DEBOUNCE_DISABLE);
	GPIO_INTConfig(_PA_0, DISABLE);
#endif
#endif
	return 0;
}

__weak uint32_t  pmc_aongpio0_start(uint32_t arg, void *Data)
{
	UNUSED(Data);
	UNUSED(arg);

#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	GPIO_INTConfig(_PA_0, ENABLE);
#endif
#endif

	return 0;
}

__weak uint32_t  pmc_aongpio0_wake_handler(uint32_t arg, void *Data)
{
	UNUSED(Data);
	(void)arg;

#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	if (arg == 0) {
		//disable interrupt
		GPIO_INTConfig(_PA_0, DISABLE);
	} else {
		AON_BASE->AON_SLP_WAKE_EVENT_STATUS0 |= (AON_BIT_AON_WEVT_AON_GPIO0_STS);
		AON_NS_BASE->SLP_WAKE_EVENT_STATUS0 |= (AON_NS_BIT_AON_WEVT_AON_GPIO0_STS);
		GPIO_INTConfig(_PA_0, DISABLE);
		//do something for wakeup handle here
		pmu_acquire_wakelock(PMU_OS);
		pmu_set_power_event(EVENT_ENTER_ACTIVE);
	}

#endif
#endif

	return 0;
}

// AON GPIOA1
void pmc_aongpio1_isr(void *id, u32 event)
{
	(void)id;
	(void)event;

#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	DiagPrintf("pmc_aongpio1_isr\r\n");

#endif
#endif

}

__weak uint32_t  pmc_aongpio1_init(uint32_t arg, void *Data)
{
	UNUSED(Data);
	UNUSED(arg);
#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	/* GPIO init */
	GPIO_InitTypeDef GPIO_InitStruct_A1 = {0};
	GPIO_InitStruct_A1.GPIO_Pin = _PA_1;
	GPIO_InitStruct_A1.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct_A1.GPIO_Mode = GPIO_Mode_INT;
	GPIO_InitStruct_A1.GPIO_ITTrigger  = GPIO_INT_Trigger_EDGE;
	GPIO_InitStruct_A1.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
	GPIO_InitStruct_A1.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_DISABLE;
	GPIO_Init(&GPIO_InitStruct_A1);
	GPIO_UserRegIrq(_PA_1, pmc_aongpio1_isr, &GPIO_InitStruct_A1);
	GPIO_INTMode(_PA_1, ENABLE, GPIO_INT_Trigger_EDGE, GPIO_INT_POLARITY_ACTIVE_LOW, GPIO_INT_DEBOUNCE_DISABLE);
	GPIO_INTConfig(_PA_1, DISABLE);
#endif
#endif
	return 0;
}

__weak uint32_t  pmc_aongpio1_start(uint32_t arg, void *Data)
{
	UNUSED(Data);
	UNUSED(arg);

#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	GPIO_INTConfig(_PA_1, ENABLE);
#endif
#endif

	return 0;
}

__weak uint32_t  pmc_aongpio1_wake_handler(uint32_t arg, void *Data)
{
	UNUSED(Data);
	(void)arg;

#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	if (arg == 0) {
		//disable interrupt
		GPIO_INTConfig(_PA_1, DISABLE);
	} else {
		AON_BASE->AON_SLP_WAKE_EVENT_STATUS0 |= (AON_BIT_AON_WEVT_AON_GPIO1_STS);
		AON_NS_BASE->SLP_WAKE_EVENT_STATUS0 |= (AON_NS_BIT_AON_WEVT_AON_GPIO1_STS);
		GPIO_INTConfig(_PA_1, DISABLE);
		//do something for wakeup handle here
		pmu_acquire_wakelock(PMU_OS);
		pmu_set_power_event(EVENT_ENTER_ACTIVE);
	}
#endif
#endif

	return 0;
}

// AON GPIOA2
void pmc_aongpio2_isr(void *id, u32 event)
{
	(void)id;
	(void)event;
#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	DiagPrintf("pmc_aongpio2_isr\r\n");

#endif
#endif

}

__weak uint32_t  pmc_aongpio2_init(uint32_t arg, void *Data)
{
	UNUSED(Data);
	UNUSED(arg);

#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	/* GPIO init */
	GPIO_InitTypeDef GPIO_InitStruct_A2 = {0};
	GPIO_InitStruct_A2.GPIO_Pin = _PA_2;
	GPIO_InitStruct_A2.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct_A2.GPIO_Mode = GPIO_Mode_INT;
	GPIO_InitStruct_A2.GPIO_ITTrigger  = GPIO_INT_Trigger_EDGE;
	GPIO_InitStruct_A2.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
	GPIO_InitStruct_A2.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_DISABLE;
	GPIO_Init(&GPIO_InitStruct_A2);
	GPIO_UserRegIrq(_PA_2, pmc_aongpio2_isr, &GPIO_InitStruct_A2);
	GPIO_INTMode(_PA_2, ENABLE, GPIO_INT_Trigger_EDGE, GPIO_INT_POLARITY_ACTIVE_LOW, GPIO_INT_DEBOUNCE_DISABLE);
	GPIO_INTConfig(_PA_2, DISABLE);
#endif
#endif

	return 0;
}

__weak uint32_t  pmc_aongpio2_start(uint32_t arg, void *Data)
{
	UNUSED(Data);
	UNUSED(arg);

#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	GPIO_INTConfig(_PA_2, ENABLE);
#endif
#endif

	return 0;
}

__weak uint32_t  pmc_aongpio2_wake_handler(uint32_t arg, void *Data)
{
	UNUSED(Data);
	(void)arg;

#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	if (arg == 0) {
		//disable interrupt
		GPIO_INTConfig(_PA_2, DISABLE);
	} else {
		AON_BASE->AON_SLP_WAKE_EVENT_STATUS0 |= (AON_BIT_AON_WEVT_AON_GPIO2_STS);
		AON_NS_BASE->SLP_WAKE_EVENT_STATUS0 |= (AON_NS_BIT_AON_WEVT_AON_GPIO2_STS);
		GPIO_INTConfig(_PA_2, DISABLE);
		//do something for wakeup handle here
		pmu_acquire_wakelock(PMU_OS);
		pmu_set_power_event(EVENT_ENTER_ACTIVE);
	}
#endif
#endif


	return 0;
}

// AON GPIOA3
void pmc_aongpio3_isr(void *id, u32 event)
{
	(void)id;
	(void)event;
#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	DiagPrintf("pmc_aongpio3_isr\r\n");

#endif
#endif
}

__weak uint32_t  pmc_aongpio3_init(uint32_t arg, void *Data)
{
	UNUSED(Data);
	UNUSED(arg);

#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	GPIO_InitTypeDef GPIO_InitStruct_A3 = {0};
	GPIO_InitStruct_A3.GPIO_Pin = _PA_3;
	GPIO_InitStruct_A3.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct_A3.GPIO_Mode = GPIO_Mode_INT;
	GPIO_InitStruct_A3.GPIO_ITTrigger  = GPIO_INT_Trigger_EDGE;
	GPIO_InitStruct_A3.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
	GPIO_InitStruct_A3.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_DISABLE;
	GPIO_Init(&GPIO_InitStruct_A3);
	GPIO_UserRegIrq(_PA_3, pmc_aongpio3_isr, &GPIO_InitStruct_A3);
	GPIO_INTMode(_PA_3, ENABLE, GPIO_INT_Trigger_EDGE, GPIO_INT_POLARITY_ACTIVE_LOW, GPIO_INT_DEBOUNCE_DISABLE);
	GPIO_INTConfig(_PA_3, DISABLE);
#endif
#endif

	return 0;
}

__weak uint32_t  pmc_aongpio3_start(uint32_t arg, void *Data)
{
	UNUSED(Data);
	UNUSED(arg);

#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	GPIO_INTConfig(_PA_3, ENABLE);
#endif
#endif

	return 0;
}

__weak uint32_t  pmc_aongpio3_wake_handler(uint32_t arg, void *Data)
{
	UNUSED(Data);
	(void)arg;

#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	if (arg == 0) {
		//disable interrupt
		GPIO_INTConfig(_PA_3, DISABLE);
	} else {
		AON_BASE->AON_SLP_WAKE_EVENT_STATUS0 |= (AON_BIT_AON_WEVT_AON_GPIO3_STS);
		AON_NS_BASE->SLP_WAKE_EVENT_STATUS0 |= (AON_NS_BIT_AON_WEVT_AON_GPIO3_STS);
		GPIO_INTConfig(_PA_3, DISABLE);
		//do something for wakeup handle here
		pmu_acquire_wakelock(PMU_OS);
		pmu_set_power_event(EVENT_ENTER_ACTIVE);
	}

#endif
#endif


	return 0;
}

// RTC
#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
#include <time.h>
static RTC_TimeTypeDef RTC_TimeStruct;
static RTC_AlarmTypeDef RTC_AlarmStruct;
#endif
#endif

u32 pmc_rtc_isr(void *data)
{
	(void)data;

#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	RTC_INTClear();
	RTC_Enable(DISABLE);
#endif
#endif

	return 0;
}

__weak uint32_t  pmc_rtc_init(uint32_t arg, void *Data)
{
	UNUSED(Data);
	UNUSED(arg);

#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	RCC_PeriphClockCmd(APBPeriph_RTC, APBPeriph_RTC_CLOCK, ENABLE);

	struct tm *t;
	time_t epoch_time, alarm_epoch_time;

	epoch_time = 1735689600;

	t = gmtime(&epoch_time);

	RTC_TimeStruct.RTC_Seconds = t->tm_sec;
	RTC_TimeStruct.RTC_Minutes = t->tm_min;
	RTC_TimeStruct.RTC_Hours = t->tm_hour;
	RTC_TimeStruct.RTC_DayOfMonth = t->tm_mday;
	RTC_TimeStruct.RTC_DayOfWeek = t->tm_wday;
	RTC_TimeStruct.RTC_Days = t->tm_yday;
	RTC_TimeStruct.RTC_Month = t->tm_mon;
	RTC_TimeStruct.RTC_Year = t->tm_year + RTC_BASE_YEAR;

	RTC_SetTime(0, &RTC_TimeStruct);

	alarm_epoch_time = epoch_time + 60;	 // alarm in 60 seconds later

	t = gmtime(&alarm_epoch_time);

	RTC_AlarmStruct.RTC_AlarmTime.RTC_Seconds = t->tm_sec;
	RTC_AlarmStruct.RTC_AlarmTime.RTC_Minutes = t->tm_min;
	RTC_AlarmStruct.RTC_AlarmTime.RTC_Hours = t->tm_hour;
	RTC_AlarmStruct.RTC_AlarmTime.RTC_DayOfMonth = t->tm_mday;
	RTC_AlarmStruct.RTC_AlarmTime.RTC_DayOfWeek = t->tm_wday;
	RTC_AlarmStruct.RTC_AlarmTime.RTC_Days = t->tm_yday;
	RTC_AlarmStruct.RTC_AlarmTime.RTC_Month = t->tm_mon;
	RTC_AlarmStruct.RTC_AlarmTime.RTC_Year = t->tm_year + RTC_BASE_YEAR;

	RTC_SetAlarm((u32)0, &RTC_AlarmStruct);

	RTC_IRQInit(pmc_rtc_isr, (void *)NULL);
#endif
#endif
	return 0;
}

__weak uint32_t  pmc_rtc_start(uint32_t arg, void *Data)
{
	UNUSED(Data);
	UNUSED(arg);

#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	RTC_Enable(ENABLE);
#endif
#endif
	return 0;
}

__weak uint32_t  pmc_rtc_wake_handler(uint32_t arg, void *Data)
{
	UNUSED(Data);
	(void)arg;

#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	if (arg == 0) {
		//disable interrupt
		RTC_Enable(DISABLE);
	} else {
		//do something for wakeup handle here
		pmu_acquire_wakelock(PMU_OS);
		pmu_set_power_event(EVENT_ENTER_ACTIVE);
	}
#endif
#endif

	return 0;
}

// AON TIMER
__weak u32 pmc_aontimer_isr(void *Data)
{
	UNUSED(Data);

#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	AONTimer_INT(DISABLE);
	AONTimer_ClearINT();
	AONTimer_Cmd(DISABLE);
	RCC_PeriphClockCmd(APBPeriph_AON_TIMER, APBPeriph_AONTIM_CLOCK, DISABLE);
#endif
#endif

	return 0;
}

__weak uint32_t  pmc_aontimer_init(uint32_t arg, void *Data)
{
	UNUSED(Data);
	UNUSED(arg);

#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	u32 duration_ms = 10 * 1000;

	InterruptRegister((IRQ_FUN)pmc_aontimer_isr, AON_TIMER_IRQ, (u32)NULL, 3);
	InterruptEn(AON_TIMER_IRQ, 3);

	AONTimer_Setting(duration_ms);
#endif
#endif

	return 0;
}

__weak uint32_t  pmc_aontimer_start(uint32_t arg, void *Data)
{
	UNUSED(Data);
	UNUSED(arg);

#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	AONTimer_Cmd(DISABLE);
	RCC_PeriphClockCmd(APBPeriph_AON_TIMER, APBPeriph_AONTIM_CLOCK, DISABLE);
	RCC_PeriphClockCmd(APBPeriph_AON_TIMER, APBPeriph_AONTIM_CLOCK, ENABLE);
	AONTimer_INT(ENABLE);
	AONTimer_Cmd(ENABLE);
#endif
#endif

	return 0;
}

__weak uint32_t  pmc_aontimer_wake_handler(uint32_t arg, void *Data)
{
	UNUSED(Data);
	(void)arg;

#if RUN_WAKEUP_EVENT_SAMPLE
#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP == 1)
	if (arg == 0) {
		//disable interrupt
		AONTimer_INT(DISABLE);
		AONTimer_Cmd(DISABLE);
		RCC_PeriphClockCmd(APBPeriph_AON_TIMER, APBPeriph_AONTIM_CLOCK, DISABLE);
	} else {
		AON_BASE->AON_SLP_WAKE_EVENT_STATUS0 |= (AON_BIT_AON_WEVT_AON_TIMER_STS);
		AON_NS_BASE->SLP_WAKE_EVENT_STATUS0 |= (AON_NS_BIT_AON_WEVT_AON_TIMER_STS);

		//do something for wakeup handle here
		pmu_acquire_wakelock(PMU_OS);
		pmu_set_power_event(EVENT_ENTER_ACTIVE);
	}
#endif
#endif


	return 0;
}

// PON GPIO
void pmc_pongpio_isr(void *id, u32 event)
{
	(void)id;
	(void)event;

#if RUN_WAKEUP_EVENT_SAMPLE
	DiagPrintf("pmc_pongpio_isr\r\n");

#endif

}

__weak uint32_t  pmc_pongpio_init(uint32_t arg, void *Data)
{
	UNUSED(Data);
	UNUSED(arg);
#if RUN_WAKEUP_EVENT_SAMPLE
	/* GPIO init */
	RCC_PeriphClockCmd(APBPeriph_PON_GPIO, APBPeriph_PON_GPIO_CLOCK, ENABLE);
	Pinmux_Config(_PF_2, PINMUX_FUNCTION_GPIO);
	GPIO_InitTypeDef GPIO_InitStruct_F = {0};
	GPIO_InitStruct_F.GPIO_Pin = _PF_2;
	GPIO_InitStruct_F.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct_F.GPIO_Mode = GPIO_Mode_INT;
	GPIO_InitStruct_F.GPIO_ITTrigger  = GPIO_INT_Trigger_EDGE;
	GPIO_InitStruct_F.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_HIGH;
	GPIO_InitStruct_F.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_DISABLE;
	GPIO_Init(&GPIO_InitStruct_F);
	GPIO_UserRegIrq(_PF_2, pmc_pongpio_isr, &GPIO_InitStruct_F);
	GPIO_INTMode(_PF_2, ENABLE, GPIO_INT_Trigger_EDGE, GPIO_INT_POLARITY_ACTIVE_HIGH, GPIO_INT_DEBOUNCE_DISABLE);
	GPIO_INTConfig(_PF_2, DISABLE);
#endif
	return 0;
}

__weak uint32_t  pmc_pongpio_start(uint32_t arg, void *Data)
{
	UNUSED(Data);
	UNUSED(arg);

#if RUN_WAKEUP_EVENT_SAMPLE
	GPIO_INTConfig(_PF_2, ENABLE);
#endif

	return 0;
}

__weak uint32_t  pmc_pongpio_wake_handler(uint32_t arg, void *Data)
{
	UNUSED(Data);
	(void)arg;

#if RUN_WAKEUP_EVENT_SAMPLE
	if (arg == 0) {
		//disable interrupt
		GPIO_INTConfig(_PF_2, DISABLE);
	} else {
		GPIO_INTConfig(_PF_2, DISABLE);
		//do something for wakeup handle here
		pmu_acquire_wakelock(PMU_OS);
		pmu_set_power_event(EVENT_ENTER_ACTIVE);
	}
#endif

	return 0;
}

// PON Timer
__weak u32 pmc_pontimer_isr(void *Data)
{
	UNUSED(Data);

#if RUN_WAKEUP_EVENT_SAMPLE
	u32 TimerIdx = 2;

	RTIM_Cmd(TimerIdx, DISABLE);
	RTIM_INTClear(TimerIdx);
#endif

	return 0;
}

__weak uint32_t  pmc_pontimer_init(uint32_t arg, void *Data)
{
	UNUSED(Data);
	UNUSED(arg);

#if RUN_WAKEUP_EVENT_SAMPLE
	RTIM_TimeBaseInitTypeDef TIM_InitStruct;
	u32 TimerGrp = 0;
	u32 TimerIdx = 2;
	u32 duration_ms = 20 * 1000;
	u32 passed = (duration_ms << 5) + (((duration_ms << 7) + (duration_ms << 6) + (duration_ms << 2) + duration_ms) >> 8);

	RTIM_GTIMER_Init(TimerGrp);

	RTIM_TimeBaseStructInit(&TIM_InitStruct);
	TIM_InitStruct.TIM_TimerIdx = TimerIdx;
	TIM_InitStruct.TIM_Prescaler = 0;//124;
	TIM_InitStruct.TIM_LoadCnt = passed - 1;
	TIM_InitStruct.TIM_IntMask = ENABLE;
	RTIM_TimeBaseInit(TimerGrp, &TIM_InitStruct);

	RTIM_RegTimeoutIRQ(TimerIdx, (IRQ_FUN)pmc_pontimer_isr, (void *)TimerIdx);
#endif

	return 0;
}

__weak uint32_t  pmc_pontimer_start(uint32_t arg, void *Data)
{
	UNUSED(Data);
	UNUSED(arg);

#if RUN_WAKEUP_EVENT_SAMPLE
	u32 TimerIdx = 2;
	RTIM_Cmd(TimerIdx, DISABLE);
	RTIM_INTConfig(TimerIdx, 0, ENABLE);
	RTIM_Cmd(TimerIdx, ENABLE);
#endif

	return 0;
}

__weak uint32_t  pmc_pontimer_wake_handler(uint32_t arg, void *Data)
{
	UNUSED(Data);
	(void)arg;

#if RUN_WAKEUP_EVENT_SAMPLE
	if (arg == 0) {
		//disable interrupt
		u32 TimerIdx = 2;
		RTIM_Cmd(TimerIdx, DISABLE);
	} else {
		//do something for wakeup handle here
		pmu_acquire_wakelock(PMU_OS);
		pmu_set_power_event(EVENT_ENTER_ACTIVE);
	}

#endif

	return 0;
}

/* wakeup attribute can be set to WAKEUP_NULL/WAKEUP_NP/WAKEUP_FP */
/* FP can only set WAKE_SRC_PON_GPIO/WAKE_SRC_PON_TIM */
WakeEvent_TypeDef sleep_wevent_config[] = {
//  	Module				wakeup     init  start handle
	{WAKE_SRC_GPIOA0,	 WAKEUP_NULL,  pmc_aongpio0_init, pmc_aongpio0_start, pmc_aongpio0_wake_handler},
	{WAKE_SRC_GPIOA1,	 WAKEUP_NULL,  pmc_aongpio1_init, pmc_aongpio1_start, pmc_aongpio1_wake_handler},
	{WAKE_SRC_GPIOA2,	 WAKEUP_NULL,  pmc_aongpio2_init, pmc_aongpio2_start, pmc_aongpio2_wake_handler},
	{WAKE_SRC_GPIOA3,	 WAKEUP_NULL,  pmc_aongpio3_init, pmc_aongpio3_start, pmc_aongpio3_wake_handler},
	{WAKE_SRC_RTC,		 WAKEUP_NULL,  pmc_rtc_init,      pmc_rtc_start,      pmc_rtc_wake_handler},
	{WAKE_SRC_AON_TIM,	 WAKEUP_NULL,  pmc_aontimer_init, pmc_aontimer_start, pmc_aontimer_wake_handler},
	{WAKE_SRC_PON_GPIO,	 WAKEUP_NULL,  pmc_pongpio_init,  pmc_pongpio_start,  pmc_pongpio_wake_handler},
	{WAKE_SRC_PON_TIM,	 WAKEUP_NULL,  pmc_pontimer_init, pmc_pontimer_start, pmc_pontimer_wake_handler},
	{0xFFFFFFFF,		 WAKEUP_NULL,  NULL, NULL, NULL},	/* Table end */
};
