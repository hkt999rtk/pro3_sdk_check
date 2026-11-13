/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/

#include "ameba_soc.h"

/**
*  @brief Description of ENC_Init
*         ENC_Init is used to do VCE encoder initialization
*
*  @param[in]  void
*
*  @returns void
*/
void ENC_Init(void)
{
	/***************************************************/
	/* ENC Clock Select, Set Divider                   */
	/***************************************************/
	/* Encoder ACLK 250MHz */
	RCC_PeriphClockDividerSet(SYS_ENC_ACLK, 2, 1);
	RCC_PeriphClockSourceSet(SYS_ENC_ACLK, SYS2_PLL);
	RCC_PeriphClockCmd(APBPeriph_PLFM_CORE_ENC_ACLK, APBPeriph_CBUS_ENC_CLOCK, ENABLE);
	/* Enable ENC SCLK 250MHz (240MHz+5%) */
	RCC_PeriphClockDividerSet(SYS_ENC_SCLK, 2, 1);
	RCC_PeriphClockSourceSet(SYS_ENC_SCLK, SYS2_PLL);

	RCC_PeriphClockCmd(APBPeriph_ENCODER, APBPeriph_ENC_CLOCK, ENABLE);

	/***************************************************/
	/* ENC Address Remap Function Enable, Clock Enable */
	/***************************************************/
	RCC_PeriphClockCmd(APBPeriph_ENC_ADDR_REMAP, APBPeriph_ADRM_ENC_CLOCK, ENABLE);
}
