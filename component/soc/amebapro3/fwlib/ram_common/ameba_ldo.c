/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

//static const char *TAG = "LDO";

/**
  * @brief  Turn ON PSRAM LDO (1.8V output)
  * @retval None
  */
void LDO_PSRAMPowerOn(void)
{
	REGU_BASE->REGU_SWR_SRC_CTRL1 |= (REGU_BIT_REG_FORCE_SRC_IMIRROR_EN);

	REGU_BASE->REGU_LDO_PSRAM_CTRL &= ~(REGU_BIT_REG_BUFFERTYPE_SEL_L_028 | REGU_BIT_EN_PC_BT_L_028 | REGU_MASK_REG_STANDBY_L_1_0_028);
	DelayUs(20);
	REGU_BASE->REGU_LDO_PSRAM_CTRL &= ~(REGU_BIT_PD_REGU_L_028);
	DelayUs(200);
	REGU_BASE->REGU_LDO_PSRAM_CTRL &= ~(REGU_BIT_EN_DIODE_L_028);
}

/**
  * @brief  Turn OFF PSRAM LDO
  * @retval None
  */
void LDO_PSRAMPowerOff(void)
{
	REGU_BASE->REGU_LDO_PSRAM_CTRL |= (REGU_BIT_PD_REGU_L_028 | REGU_BIT_EN_DIODE_L_028);

	DelayMs(5);
}

/**
  * @brief  Turn ON SDIO LDO.
  *			By default, SDIO LDO outputs 3.3V, which is the default voltage for SD cards at the start.
  *			LDO_SDIOPower_Select() can be used to select between an output voltage of 1.8V or 3.3V.
  *			The line which allows SDIO LDO to output 1.8V is for FT2 use ONLY (CONFIG_MP_INCLUDED == 1). Do not remove.
  *			This helps avoid waiting for voltage to drop from 3.3V to 1.8V for FT2.
  * @retval None
  */
void LDO_SDIOPowerOn(void)
{
	REGU_BASE->REGU_SWR_SRC_CTRL1 |= (REGU_BIT_REG_FORCE_SRC_IMIRROR_EN);

#if defined (CONFIG_FT2_VERIFY) && (CONFIG_FT2_VERIFY == 1)
	LDO_SDIOPower_Select(SDIO_HOST_1V8); // LDO mode (1.8V mode), for FT2 use, do not delete
#else
	LDO_SDIOPower_Select(SDIO_HOST_3V3); // Power-Cut Mode (3.3V mode)
#endif

	REGU_BASE->REGU_LDO_SDIO_CTRL &= ~(REGU_BIT_REG_BUFFERTYPE_SEL_L_02C |
									   REGU_MASK_REG_STANDBY_L_1_0_02C);
	DelayUs(20);
	REGU_BASE->REGU_LDO_SDIO_CTRL &= ~(REGU_BIT_PD_REGU_L_02C);
	DelayUs(200);
	REGU_BASE->REGU_LDO_SDIO_CTRL &= ~(REGU_BIT_EN_DIODE_L_02C);
}

/**
  * @brief  SDIO LDO Output Voltage Select - 1.8V or 3.3V.
  * @param  mode: 1: SDIO_HOST_3V3 (for 3.3V (3.15V for Pro3) output); 0: SDIO_HOST_1V8 (for 1.8V output)
  * @retval None
  */
void LDO_SDIOPower_Select(u8 mode)
{
	if (mode == SDIO_HOST_3V3) { // 3.3V mode
		REGU_BASE->REGU_LDO_SDIO_CTRL |= (REGU_BIT_EN_PC_BT_L_02C);
	} else { // SDIO_HOST_1V8; 1.8V mode
		REGU_BASE->REGU_LDO_SDIO_CTRL &= ~(REGU_BIT_EN_PC_BT_L_02C);
	}
}

/**
  * @brief  Turn OFF SDIO LDO
  * @retval None
  */
void LDO_SDIOPowerOff(void)
{
	REGU_BASE->REGU_LDO_SDIO_CTRL |= (REGU_BIT_PD_REGU_L_028 | REGU_BIT_EN_DIODE_L_028);

	DelayMs(5);
}

/**
  * @brief  Turn ON WLON LDO (0.9V output)
  * @retval None
  */
void LDO_WLONPowerOn(void)
{
	REGU_BASE->REGU_SWR_SRC_CTRL1 |= (REGU_BIT_REG_FORCE_SRC_IMIRROR_EN);

	REGU_BASE->REGU_WLON_LDO_CTRL0 |= REGU_BIT_REG_BUFFERTYPE_SEL_L;
	REGU_BASE->REGU_WLON_LDO_CTRL0 &= ~(REGU_MASK_REG_STANDBY_L_1_0);
	DelayUs(20);
	REGU_BASE->REGU_WLON_LDO_CTRL0 &= ~(REGU_BIT_PD_REGU_L);
	DelayUs(400);
	REGU_BASE->REGU_WLON_LDO_CTRL0 &= ~(REGU_BIT_EN_DIODE_L);
	REGU_BASE->REGU_WLON_LDO_CTRL0 &= ~(REGU_MASK_REG_DMY_LDO_L_1_0);
}

/**
  * @brief  Turn OFF WLON LDO
  * @retval None
  */
void LDO_WLONPowerOff(void)
{
	REGU_BASE->REGU_WLON_LDO_CTRL0 |= (REGU_BIT_PD_REGU_L | REGU_BIT_EN_DIODE_L);
}
