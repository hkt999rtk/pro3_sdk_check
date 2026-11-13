/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

//static const char *TAG = "SWR";

/**
  * @brief  To use SWR_PFM mode (1.25V output)
  * @retval None
  */
void SWR_PFM_mode(void)
{
	REGU_BASE->REGU_SWR_ZCD_CTRL |= (REGU_BIT_ZCD_EN);

	REGU_BASE->REGU_SWR_SRC_CTRL0 &= ~(REGU_MASK_STD_L1);
	REGU_BASE->REGU_SWR_SRC_CTRL0 |= (REGU_STD_L1(2));

	AON_BASE->AON_OTP_SYSCFG5 &= ~(AON_MASK_PFM_OCP);
	AON_BASE->AON_OTP_SYSCFG5 |= (AON_PFM_OCP(2));

	AON_BASE->AON_OTP_SYSCFG5 &= ~(AON_MASK_PWM_OCP);
	AON_BASE->AON_OTP_SYSCFG5 |= (AON_PWM_OCP(3));

	REGU_BASE->REGU_SWR_SRC_CTRL0 |= (REGU_BIT_AON_SRC_POW_SW);

	while (((REGU_BASE->REGU_SWR_SRC_CTRL1 & (REGU_BIT_SRC_SSOVER_L)) >> 30) == 0);

	REGU_BASE->REGU_OSC32K_CTRL0 |= (REGU_BIT_AON_SWR_SRC_SWITCH);

	REGU_BASE->REGU_SWR_SRC_CTRL0 &= ~(REGU_BIT_FPWM_L1);
}

