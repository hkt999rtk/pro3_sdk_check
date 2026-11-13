/**
  ******************************************************************************
  * @file    ameba_systemer.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the following
  *          functionalities of the systimer & delay:
  *           - SYSTIMER_GetPassTime
  *           - SYSTIMER_TickGet
  *           - DelayUs
  *           - DelayMs
  *           - DelayNop
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#include "ameba_soc.h"

/**
  * @brief  Init TIM0 as systimer.
  * @param  none
  * @note called by rom code when boot, user dont use it.
  */
HAL_ROM_TEXT_SECTION
void SYSTIMER_Init(void)
{
	RTIM_TimeBaseInitTypeDef TIM_InitStruct;
	RTIM_TypeDef *TIMx = (RTIM_TypeDef *)RTIMx[0];

	if (TrustZone_IsSecure()) {
		TIMx = RTIMx_S[0];
	}

	if (TIMx->CTRL & TM_BIT_EN) {
		return;
	}

	RTIM_TimeBaseStructInit(&TIM_InitStruct);

	TIM_InitStruct.TIM_TimerIdx = 0;
	TIM_InitStruct.TIM_Prescaler = 0;
	TIM_InitStruct.TIM_LoadCnt = 0xFFFFFFFF;

	TIM_InitStruct.TIM_OpMode  = TIM_OP_MODE_PERIODICAL;

	RTIM_TimeBaseInit(0, &TIM_InitStruct);
	RTIM_Cmd(0, ENABLE);
}

/**
  * @brief  Get TIM0 tick, unit is 31us.
  * @param  none
  */
HAL_ROM_TEXT_SECTION
u32 SYSTIMER_TickGet(void)
{
	return RTIM_GetCurrentCount(0);
}


/**
  * @brief  Get TIM0 pass tick.
  * @param  none
  */
HAL_ROM_TEXT_SECTION
u32 SYSTIMER_GetPassTick(u32 start)
{
	u32 current = 0;
	u32 passed = 0;

	current = RTIM_GetCurrentCount(0);

	if (current >= start) {
		passed = current - start;
	} else {
		passed = 0xFFFFFFFF - (start - current);
	}

	return passed;
}

/**
  * @brief  Get TIM0 pass time, unit is ms.
  * @param  start: start time stamp (ms)
  * @note resolution is 31us
  */
HAL_ROM_TEXT_SECTION
u32 SYSTIMER_GetPassTime(u32 start)
{
	u32 current = 0;
	u32 passed = 0;

	current = RTIM_GetCurrentCount(0);

	if (current >= start) {
		passed = current - start;
	} else {
		passed = 0xFFFFFFFF - (start - current);
	}

	/* ms =x*1000/32768 = ((u64)x *1000) >> 15 */
	passed = (((passed & 0xFFFF8000) >> 15) * 1000) + (((passed & 0x7FFF) * 1000) >> 15);

	return passed;
}
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
