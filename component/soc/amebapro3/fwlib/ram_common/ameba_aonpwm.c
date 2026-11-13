/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_aonpwm.h"

/**
  * @brief  set AON pwm high and low period output
  * @param  low_cnt: Low period of chip_alive output (in 32K clk)
  * @param  high_cnt: High period of chip_alive output (in 32K clk)
  * @note
  * @retval None
  */
void AONPwm_Setting(u32 low_cnt, u32 high_cnt)
{
	AON_TypeDef *AON = (AON_TypeDef *)AON_BASE;

	AON->AON_TIMER_CTR1 = AON_AON_WDT_HIGH_CNT(0);

	AON->AON_TIMER_CTR0 = AON_AON_WDT_LOW_CNT(low_cnt);
	AON->AON_TIMER_CTR1 = AON_AON_WDT_HIGH_CNT(high_cnt);
}
