/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_aontimer.h"

//static const char *TAG = "AON_TIMER";

/**
  * @brief  set AON timer for wakeup.
  * @param  SDuration: sleep time, unit is ms
  * @note wakeup state:sleep PG & CG & deep sleep
  * @note This is 32kHz timer, max counter = (0x3FFFFFFF) * (1/32k) / 60 = 559.2405328125 min
  * @note FPGA is using 30MHz clock
  * @retval None
  */
void AONTimer_Setting(u32 SDuration)
{
	AON_TypeDef *AON = (AON_TypeDef *)AON_BASE;

	if (SDuration == 0) {
		return;
	}

	SDuration = SDuration * 32;			// actual setting to be used

	// DiagPrintf("%d\r\n", SDuration);

	AON->AON_WAKE_TIMER &= ~(AON_MASK_AON_WAKE_TM_TC);
	AON->AON_WAKE_TIMER |= AON_AON_WAKE_TM_TC(SDuration);

}

/**
  * @brief  get current AON timer.
  * @param  None
  * @note wakeup state:sleep PG & CG & deep sleep
  * @retval The current Aon Timer counter, the unit is ms.
  */
u32 AONTimer_Get(void)
{
	AON_TypeDef *AON = (AON_TypeDef *)AON_BASE;

	u32 Rtemp = 0;

	/* use 32K */
	Rtemp = AON_GET_AON_WAKE_TM_TC(AON->AON_WAKE_TIMER);

	Rtemp = Rtemp / 32;			// actual setting

	return Rtemp;
}

/**
  * @brief  enable AON timer interrupt.
  * @param  Status: interrupt status
  * @retval None
  */
void AONTimer_INT(u32 Status)
{
	AON_TypeDef *AON = (AON_TypeDef *)AON_BASE;

	if (Status == ENABLE) {
		AON->AON_SYS_CTRL |= AON_BIT_AON_TIMER_IMR;
	} else {
		AON->AON_SYS_CTRL &= ~(AON_BIT_AON_TIMER_IMR);
	}
}

/**
  * @brief  Clear aontimer interrupt.
  * @retval   None
  */
void AONTimer_ClearINT(void)
{
	AON_TypeDef *AON = (AON_TypeDef *)AON_BASE;

	AON->AON_WAKE_TIMER |= AON_BIT_AON_TIMER_ISR;
}

void AONTimer_INT_WAKEUP(void)
{
	// RTK_LOGI(TAG, "wake reason: 0x%x\n", SOCPS_AONWakeReason());
	AONTimer_ClearINT();
	RCC_PeriphClockCmd(APBPeriph_AON_TIMER, APBPeriph_AONTIM_CLOCK, DISABLE);
	// pmu_acquire_wakelock(PMU_OS);
}

// added
void AONTimer_Cmd(u32 Status)
{
	AON_TypeDef *AON = (AON_TypeDef *)AON_BASE;

	if (Status == ENABLE) {
		AON->AON_WAKE_TIMER |= AON_BIT_AON_WAKE_TM_EN;
	} else {
		AON->AON_WAKE_TIMER &= ~(AON_BIT_AON_WAKE_TM_EN);
	}
}
