/** mbed Microcontroller Library
 ******************************************************************************
 * @file    timer_api.c
 * @author
 * @version V1.0.0
 * @date    2025-10-23
 * @brief   This file provides mbed API for gtimer.
 ******************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
 ******************************************************************************
 */

#include "objects.h"
#include "timer_api.h"

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_TIMER
 *  @brief      MBED_TIMER driver modules.
 *  @{
 */

/** @defgroup MBED_TIMER_Exported_Functions MBED_TIMER Exported Functions
  * @{
  */

static gtimer_t *data_irq[18] = {NULL,};


static u8 tid_to_tgid(u8 tid)
{
	u8 tgid;

	assert_param(tid < GTIMER_MAX);

	if (tid < 8) {
		tgid = 0;
	} else if (tid == 8) {
		tgid = 1;
	} else if (tid < 17) {
		tgid = 2;
	} else {
		tgid = 3;
	}

	return tgid;
}


static uint32_t get_clk_freq(u8 tid)
{
	uint32_t clk_freq;
	uint8_t clk_source = 0;
	uint8_t tgid;

	assert_param(tid < GTIMER_MAX);

	tgid = tid_to_tgid(tid);

	switch (tgid) {
	// TIMER0 - TIMER7
	case 0:
		clk_source = RCC_PeriphClockSourceGet(PON_TIMER0_CLK);
		break;
	// TIMER8 - TIMER17
	default:
		clk_source = RCC_PeriphClockSourceGet(PON_TIMER123_CLK);
		break;
	}

	// Clock select:
	switch (clk_source) {
	// 0x0: XTAL 40M
	case 0:
		clk_freq = XTAL_ClkGet();
		break;

	// 0x1: OSC 4M
	case 1:
		clk_freq = 4000000;
		break;

	// 0x2: SDM 32K
	case 2:
		clk_freq = 32000;
		break;

	// 0x3: OSC 131K
	case 3:
		clk_freq = 131000;
		break;

	// Others: 1'b0
	default:
		break;
	}

	//DiagPrintf("clock frequency: %ld Hz\n", clk_freq);

	return clk_freq;
}


static uint64_t us_to_cnt(u8 tid, uint64_t us)
{
	uint32_t prescaler;
	uint64_t res;
	uint32_t clk_freq;

	assert_param(tid < GTIMER_MAX);

	prescaler = RTIM_GetPrescaler(tid) + 1;

	clk_freq = get_clk_freq(tid);

	res = (uint64_t)(us * clk_freq / 1000000 / prescaler);

	return res;
}


static uint64_t cnt_to_us(u8 tid, uint32_t cnt)
{
	uint32_t prescaler;
	uint64_t res;
	uint32_t clk_freq;

	assert_param(tid < GTIMER_MAX);

	prescaler = RTIM_GetPrescaler(tid) + 1;

	clk_freq = get_clk_freq(tid);

	res = (uint64_t)(cnt * prescaler * 1000000 / clk_freq);

	return res;
}


/* group: MBED_TIMER_Exported_Functions */
/* Static function in the group should not be extracted to the doxygen doc */
static u32 gtimer_timeout_handler(void *data)
{
	gtimer_t *obj = (gtimer_t *)data;
	uint32_t tid = obj->timer_id;
	gtimer_irq_handler handler;

	RTIM_INTClear(tid);

	// if (!obj->is_periodcal) {
	// 	gtimer_stop(obj);
	// }

	if (obj->handler != NULL) {
		handler = (gtimer_irq_handler)obj->handler;
		handler(obj->hid);
	}

	return 0;
}

// static void print_info(RTIM_TimeBaseInitTypeDef *info)
// {
// 	DiagPrintf("tid: %d\n", info->TIM_TimerIdx);
// 	DiagPrintf("tgid: %d\n", info->TIM_TimerGrp);
// 	DiagPrintf("prescaler: %d\n", info->TIM_Prescaler);
// 	DiagPrintf("load count: %d\n", info->TIM_LoadCnt);
// 	DiagPrintf("count mode: %d\n", info->TIM_CntMode);
// 	DiagPrintf("operate mode: %d\n", info->TIM_OpMode);
// 	DiagPrintf("interrupt mask: %d\n", info->TIM_IntMask);
// }

/**
  * @brief  Initialize the timer device, including timer registers and interrupt.
  * @param  obj: Timer object defined in application software.
  * @param  tid: General timer ID, which can be one of the following parameters:
  *     @arg TIMER0
  *     @arg TIMER1
  *     @arg TIMER2
  *     @arg TIMER3
  *     @arg TIMER4
  *     @arg TIMER5
  *     @arg TIMER6
  *     @arg TIMER7
  *     @arg TIMER8
  *     @arg TIMER9
  *     @arg TIMER10
  * 	@arg TIMER11
  *     @arg TIMER12
  *     @arg TIMER13
  *     @arg TIMER14
  *     @arg TIMER15
  *     @arg TIMER16
  *     @arg TIMER17
  * @retval none
  */
void gtimer_init(gtimer_t *obj, uint32_t tid)
{
	RTIM_TimeBaseInitTypeDef TIM_InitStruct;

	assert_param(tid < GTIMER_MAX);

	// for test purpose, the author said this part will be moved into driver

	// gtimer 0
	RCC_PeriphClockCmd(APBPeriph_PON_TIMER0, APBPeriph_PON_TIMER0_CLOCK, ENABLE);

	// gtimer 1
	RCC_PeriphClockCmd(APBPeriph_PON_TIMER1, APBPeriph_PON_TIMER1_CLOCK, ENABLE);

	// gtimer 2
	RCC_PeriphClockCmd(APBPeriph_PON_TIMER2, APBPeriph_PON_TIMER2_CLOCK, ENABLE);

	// gtimer 3
	RCC_PeriphClockCmd(APBPeriph_PON_TIMER3, APBPeriph_PON_TIMER3_CLOCK, ENABLE);

	RTIM_TimeBaseStructInit(&TIM_InitStruct);

	TIM_InitStruct.TIM_TimerIdx = tid;
	TIM_InitStruct.TIM_TimerGrp = tid_to_tgid(tid);
	TIM_InitStruct.TIM_CntMode = TIM_CNT_MODE_DOWN;
	if (obj->is_periodcal) {
		TIM_InitStruct.TIM_OpMode = TIM_OP_MODE_PERIODICAL;
	} else {
		TIM_InitStruct.TIM_OpMode = TIM_OP_MODE_ONE_SHOT;
	}

	//print_info(&TIM_InitStruct);
	RTIM_TimeBaseInit(tid_to_tgid(tid), &TIM_InitStruct);

	// for interrupts registration
	obj->timer_id = tid;
	data_irq[tid] = obj;
	RTIM_RegTimeoutIRQ(tid, gtimer_timeout_handler, (void *)(data_irq[tid]));
}

/**
  * @brief  Deinitialize the timer device, including interrupt and timer registers.
  * @param  obj: Timer object defined in application software.
  * @retval none
  */
void gtimer_deinit(gtimer_t *obj)
{
	uint32_t tid = obj->timer_id;

	assert_param(tid < GTIMER_MAX);

	RTIM_DeInit(tid);

	// for interrupts deregistration
	data_irq[tid] = NULL;
	RTIM_UnRegTimeoutIRQ(tid);
}

/**
  * @brief  Get counter value of the specified timer.
  * @param  obj: Timer object defined in application software.
  * @return Counter value.
  */
uint32_t gtimer_read_tick(gtimer_t *obj)
{
	uint32_t tid = obj->timer_id;

	assert_param(tid < GTIMER_MAX);

	return (RTIM_GetCurrentCount(tid));
}

/**
  * @brief  Read current timer tick in microsecond.
  * @param  obj: Timer object defined in application software.
  * @return 64b tick time in microsecond(us).
  */
uint64_t gtimer_read_us(gtimer_t *obj)   //need to be test in IAR(64bit computing)
{
	uint32_t cnt;

	assert_param(obj->timer_id < GTIMER_MAX);

	cnt = RTIM_GetCurrentCount(obj->timer_id);

	return cnt_to_us(obj->timer_id, cnt);
}

/**
  * @brief  Change period of the specified timer.
  * @param  obj: Timer object defined in application software.
  * @param  duration_us: Period to be set in microseconds.
  * @retval none
  */
void gtimer_reload(gtimer_t *obj, uint32_t duration_us)
{
	assert_param(obj->timer_id < GTIMER_MAX);
	uint32_t cnt;
	uint32_t tid = obj->timer_id;

	cnt = us_to_cnt(tid, duration_us);

	RTIM_Cmd(tid, DISABLE);
	RTIM_SetLoadCnt(tid, cnt);
	RTIM_Cmd(tid, ENABLE);
}

/**
  * @brief  Start the specified timer and enable update interrupt.
  * @param  obj: Timer object defined in application software.
  * @retval none
  */
void gtimer_start(gtimer_t *obj)
{
	uint32_t tid = obj->timer_id;

	assert_param(tid < GTIMER_MAX);

	RTIM_INTConfig(tid, 0, ENABLE);
	RTIM_Cmd(tid, ENABLE);
}

/**
  * @brief Start the specified timer in one-shot mode with specified period and interrupt handler.
  * @param  obj: Timer object defined in application software.
  * @param  duration_us: Period to be set in microseconds.
  * @param  handler: User-defined IRQ callback function.
  * @param  hid: User-defined IRQ callback parameter.
  * @retval none
  * @note In one-shot mode, timer will stop counting the first time counter overflows.
  */
void gtimer_start_one_shout(gtimer_t *obj, uint32_t duration_us, void *handler, uint32_t hid)
{
	assert_param(obj->timer_id < GTIMER_MAX);
	RTIM_TimeBaseInitTypeDef info;

	info.TIM_TimerIdx = obj->timer_id;
	info.TIM_TimerGrp = tid_to_tgid(obj->timer_id);
	info.TIM_Prescaler = 0;
	info.TIM_LoadCnt = us_to_cnt(obj->timer_id, duration_us);
	info.TIM_PeriodUs = 0;	// simply assign to 0 for this is not used in pro3
	info.TIM_CntMode = TIM_CNT_MODE_DOWN;
	info.TIM_IntMask = ENABLE;

	//print_info(&info);
	RTIM_StartOneShot(info.TIM_TimerGrp, &info, handler, (void *)hid);
}

/**
  * @brief Start the specified timer in periodical mode with specified period and interrupt handler.
  * @param  obj: Timer object defined in application software.
  * @param  duration_us: Period to be set in microseconds.
  * @param  handler: User-defined IRQ callback function.
  * @param  hid: User-defined IRQ callback parameter.
  * @retval none
  * @note In periodical mode, timer will restart from 0 each time the counter overflows.
  */
void gtimer_start_periodical(gtimer_t *obj, uint32_t duration_us, void *handler, uint32_t hid)
{
	assert_param(obj->timer_id < GTIMER_MAX);
	RTIM_TimeBaseInitTypeDef info;

	info.TIM_TimerIdx = obj->timer_id;
	info.TIM_TimerGrp = tid_to_tgid(obj->timer_id);
	info.TIM_Prescaler = 0;
	info.TIM_LoadCnt = us_to_cnt(obj->timer_id, duration_us);
	info.TIM_PeriodUs = 0;	// simply assign to 0 for this is not used in pro3
	info.TIM_CntMode = TIM_CNT_MODE_DOWN;
	info.TIM_IntMask = ENABLE;

	//print_info(&info);
	RTIM_StartPeriodic(info.TIM_TimerGrp, &info, handler, (void *)hid);
}

/**
  * @brief  Disable the specified timer peripheral.
  * @param  obj: Timer object defined in application software.
  * @retval none
  */
void gtimer_stop(gtimer_t *obj)
{
	uint32_t tid = obj->timer_id;

	assert_param(tid < GTIMER_MAX);

	RTIM_Cmd(tid, DISABLE);
}

/** @} */
/** @} */
/** @} */
