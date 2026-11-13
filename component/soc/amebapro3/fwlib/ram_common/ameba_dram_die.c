/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* DRAM die descriptors: JEDEC MR values per supported part */

#include "ameba_soc.h"
#include "ameba_dram_die.h"

/**
  * @brief  Control the board DCDC rail that powers the DRAM die (GPIO_C7).
  *         Disabling loses DRAM contents; the next bring-up is a cold one.
  * @param  NewState: ENABLE powers the rail, DISABLE drops it.
  * @note   Weak: boards wiring the DRAM rail differently override this.
  */
__weak void DRAM_Die_Power(u32 NewState)
{
	RCC_PeriphClockCmd(APBPeriph_CORE_GPIO, APBPeriph_CORE_GPIO_CLOCK, ENABLE);
	GPIO_BASE->GPIOA_ODM_EN |= BIT(7);
	if (NewState == ENABLE) {
		GPIO_BASE->GPIOA_ODH_EN |= BIT(7);
	} else {
		GPIO_BASE->GPIOA_ODL_EN |= BIT(7);
	}
}

/** @brief  KVR16S11/8: DDR3L-100 FPGA model, DLL off, CL=6. */
const DRAM_DIE_OPS_TypeDef DRAM_DIE_KVR16S11_8 = {
	.name       = "KVR16S11/8",
	.jedec_spec = "DDR3L-100 (Kingston, x16, 1 die, 1.35V)",
	.mr         = {
		[0] = JEDEC_MR0_BL_8 | JEDEC_MR0_CL_6 | JEDEC_MR0_WR_5 | JEDEC_MR0_PPD_SLOW,
		[1] = JEDEC_MR1_DLL_DISABLED,
		[2] = JEDEC_MR2_CWL_5 | JEDEC_MR2_RTT_WR_RZQ_4,
		[3] = 0,
	},
	.DumpInfo   = NULL,
};

/**
  * @brief  ESMT M15T1G1664A: 1 Gb x16 DDR3L-1866, CL=13, CWL=10.
  */
const DRAM_DIE_OPS_TypeDef DRAM_DIE_M15T1G1664A = {
	.name       = "M15T1G1664A",
	.jedec_spec = "DDR3L-1866 (ESMT, 1Gb x16, 1.35V)",
	.mr         = {
		[0] = JEDEC_MR0_BL_8 | JEDEC_MR0_CL_13 | JEDEC_MR0_WR_14 |
		JEDEC_MR0_DLL_RESET | JEDEC_MR0_PPD_FAST | JEDEC_MR0_RBT_INTERLEAVE,
		[1] = JEDEC_MR1_DLL_ENABLED
#if DRAM_DIE_ODT_ENABLE
		| JEDEC_MR1_RTT_NOM_RZQ_6 | JEDEC_MR1_DIC_RZQ_6
#else
		| JEDEC_MR1_RTT_NOM_DISABLED
#endif
		,
		[2] = JEDEC_MR2_CWL_10 | JEDEC_MR2_SRT_EXTENDED
#if DRAM_DIE_ODT_ENABLE
		| JEDEC_MR2_RTT_WR_RZQ_4
#endif
		,
		[3] = 0,
	},
	.DumpInfo   = NULL,
};
