/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ameba_soc.h"
/**
  * @brief  Get boot reason
  * @param  NA
  * @retval Boot Reason.
  */
u32 BOOT_Reason(void)
{
#if defined (CONFIG_ARM_CORE_CM4_NP)
	AON_TypeDef *AON = (AON_TypeDef *)AON_REG_BASE;
	return AON->AON_NP_BOOT_REASON;
#elif defined (CONFIG_ARM_CORE_CM0)
	AON_NS_TypeDef *AON_NS = (AON_NS_TypeDef *)AON_NS_REG_BASE;
	return AON_NS->AON_FW_BOOT_REASON;
#elif defined (CONFIG_ARM_CORE_CA32)
	AON_TypeDef *AON = (AON_TypeDef *)AON_REG_BASE;
	return AON->AON_CA32_BOOT_REASON;
#elif defined (CONFIG_ARM_CORE_CM4_VP)
	AON_NS_TypeDef *AON_NS = (AON_NS_TypeDef *)AON_NS_REG_BASE;
	return AON_NS->AON_MP_BOOT_REASON;
#endif
}
/**
  * @brief  warm reset the system
  * @param  None
  * @retval None
  */
void System_Reset(void)
{
	AON_BASE->AON_WDT_TIMER |= AON_BIT_WDT_RST_TRIG; //SW to trigger AON WDT reset.
}
/**
  * @brief  peripheral power on
  * @param  None
  * @retval None
  */
void System_Periph_PowerOn(void)
{
	extern u8 PERIPH_POWER_PIN;
	GPIO_InitTypeDef GPIO_InitStruct_Pow;
	GPIO_InitStruct_Pow.GPIO_Pin = PERIPH_POWER_PIN;
	GPIO_InitStruct_Pow.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct_Pow);
	GPIO_WriteBit(PERIPH_POWER_PIN, 0);
	GPIO_WriteBit(PERIPH_POWER_PIN, 1);
}
/**
  * @brief  peripheral power off
  * @param  None
  * @retval None
  */
void System_Periph_PowerOff(void)
{
	extern u8 PERIPH_POWER_PIN;
	GPIO_InitTypeDef GPIO_InitStruct_Pow;
	GPIO_InitStruct_Pow.GPIO_Pin = PERIPH_POWER_PIN;
	GPIO_InitStruct_Pow.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct_Pow);
	GPIO_WriteBit(PERIPH_POWER_PIN, 0);
}
