/**
  ******************************************************************************
  * @file    rtl8721d_delay_rom.c
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

u32 RBSS_UDELAY_CLK;
u8 RBSS_UDELAY_DIV;

/**
  * @brief  Delay x nops.
  * @param  count: nop number
  * @note for AP(default): HF=YES MVE=YES, code in ITCM, 7 cycles per loop. For 300M CPU clock, one loop cost: (1/300)*7=0.023us
  * @note for AP: HF=NO MVE=YES, code in ICache, 6 cycles per loop. For 300M CPU clock, one loop cost: (1/300)*6=0.02us
  * @note for AP FPGA: 40M CPU clock, one loop cost: (1/40)*6=0.15us
  * @note for NP: HF=NO, 4 cycles per loop. For 40M CPU clock, one loop cost: (1/40)*4=0.1us
  */

__attribute__((noinline)) void DelayNop(u32 count)
{
	asm volatile(".align 3				\n"
				 "1:					\n"
				 "sub	%[v1], #1		\n"
				 "cmp	%[v1], #0		\n"
				 "nop					\n"
				 "bne	1b				\n"
				 :[v1]"+r"(count)
				 ::"cc");
}

/**
  * @brief  Delay x us.
  * @param  us: delay time, unit is us
  * @note A minimum delay of 8us is required for read syson register
  * @note the maximum value of us is 0xFFFF_FFFF/CpuClk + 8
  */

void DelayUs(u32 us)
{
	u32 WaitCount, CpuClk;

	u32 ChipType = SYSCFG_CHIPType_Get();

	/* one loop = Div[0]/CpuClk us in np, one loop = Div[1]/CpuClk us in ap */
	if (RBSS_UDELAY_DIV == 0) {
		/* delay div is 6 for unique rom */
		RBSS_UDELAY_DIV = 6;
	}

	/* ROM uses fixed CPU clock delay and other image uses actual clock frequency from RBSS_UDELAY_CLK */
	if (ChipType == CHIP_TYPE_FPGA) {
		RBSS_UDELAY_CLK = 30;
	}

	if (RBSS_UDELAY_CLK == 0) {
		CpuClk = 40;
		WaitCount = (us * CpuClk) / RBSS_UDELAY_DIV;
	} else {
		CpuClk = RBSS_UDELAY_CLK;
		WaitCount = (us * CpuClk) / RBSS_UDELAY_DIV;
	}

	if (WaitCount == 0) {
		return;
	}

	DelayNop(WaitCount);

	return;
}

/**
  * @brief  Delay x ms.
  * @param  ms: delay time, unit is ms
  * @note the maximum value of ms is ~13,000ms when CPU@320M
  */

void DelayMs(u32 ms)
{
	DelayUs(ms * 1000);
}

/**
  * @brief  update CPU Clk used in DelayUs/DelayMs
  * @retval none:
  */

void DelayClkUpdate(u32 CpuClk)
{
	RBSS_UDELAY_CLK = CpuClk / MHZ_TICK_CNT;
}

/**
  * @brief  update Clk Div used in DelayUs/DelayMs
  * @retval none:
  */

void DelayDivUpdate(u32 ClkDiv)
{
	RBSS_UDELAY_DIV = ClkDiv;
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
