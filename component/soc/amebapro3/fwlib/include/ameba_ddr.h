/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_DDR_H
#define _AMEBA_DDR_H

#include "ameba_ddrc.h"
#include "ameba_ddrphy.h"
#include "ameba_dram_die.h"

#if defined(CONFIG_DDR_PHY_FREQ_533MHZ) && (CONFIG_DDR_PHY_FREQ_533MHZ == 1)
#define DDR_PHY_FREQ_MHZ        533
#elif defined(CONFIG_DDR_PHY_FREQ_800MHZ) && (CONFIG_DDR_PHY_FREQ_800MHZ == 1)
#define DDR_PHY_FREQ_MHZ        800
#elif defined(CONFIG_DDR_PHY_FREQ_850MHZ) && (CONFIG_DDR_PHY_FREQ_850MHZ == 1)
#define DDR_PHY_FREQ_MHZ        850
#elif defined(CONFIG_DDR_PHY_FREQ_900MHZ) && (CONFIG_DDR_PHY_FREQ_900MHZ == 1)
#define DDR_PHY_FREQ_MHZ        900
#else
#define DDR_PHY_FREQ_MHZ        933
#endif

#if defined(CONFIG_DRAM_DIE_ODT_ENABLE) && (CONFIG_DRAM_DIE_ODT_ENABLE == 1)
#define DRAM_DIE_ODT_ENABLE      1
#else
#define DRAM_DIE_ODT_ENABLE      0
#endif

#if defined(CONFIG_DISABLE_DDR) && (CONFIG_DISABLE_DDR == 1)
#define DDR_PHY_ODT_ENABLE       1
#elif defined(CONFIG_DDR_PHY_ODT_ENABLE) && (CONFIG_DDR_PHY_ODT_ENABLE == 1)
#define DDR_PHY_ODT_ENABLE       1
#else
#define DDR_PHY_ODT_ENABLE       0
#endif

#if DDR_PHY_ODT_ENABLE
#define DDR_PHY_ODT_CALIBRATION  true
#else
#define DDR_PHY_ODT_CALIBRATION  false
#endif

/* Default PHY OCD drive strength (ohm), ODT-calibrated path only: CA and DQ 40/60/80/100 */
#define DDR_PHY_CA_OCD           40
#define DDR_PHY_DQ_OCD           40

#if defined(CONFIG_FPGA) && (CONFIG_FPGA == 1)
#define DDR_ACTIVE_PHY_OPS      (&DDR_PHY_FPGA_Ops)
#define DDR_ACTIVE_DDRC_OPS     (&DDRC_FPGA_Ops)
#define DDR_ACTIVE_TIMING       (&DDRC_Timing_KVR16S11_8)
#define DDR_ACTIVE_DIE          (&DRAM_DIE_KVR16S11_8)
#else
#define DDR_ACTIVE_PHY_OPS      (&DDR_PHY_ASIC_Ops)
#define DDR_ACTIVE_DDRC_OPS     (&DDRC_ASIC_Ops)
#define DDR_ACTIVE_TIMING       (&DDRC_Timing_M15T1G1664A)
#define DDR_ACTIVE_DIE          (&DRAM_DIE_M15T1G1664A)
#endif

/**
  * @brief  Top-level DDR driver state, statically initialised to this build's
  *         DDR_ACTIVE_*. Tests or alternate boards may overwrite a slot before
  *         the first bring-up to route through a different IP or DRAM part;
  *         bring-up and self-refresh must agree on the same die.
  */
typedef struct {
	const DDR_PHY_OPS_TypeDef  *phy_dev;  /*!< active DDR PHY device       */
	const DDRC_OPS_TypeDef     *ddrc_dev; /*!< active DDR controller       */
	const DDRC_Timing_TypeDef  *timing;   /*!< DRAM part timing profile    */
	const DRAM_DIE_OPS_TypeDef *die;      /*!< DRAM die descriptor (MRs)   */
	bool                       is_up;     /*!< DDR usable; a repeat
	                                           bring-up quiesces the die   */
	bool                       odt_calibration; /*!< replayed by re-init and
	                                           self-refresh exit           */
	u8                         pasr;      /*!< JEDEC_MR2_PASR_*, applied at
	                                           self-refresh entry          */
	u8                         ca_ocd;    /*!< CA drive, ohm (DDR_SetOCD)  */
	u8                         dq_ocd;    /*!< DQ drive, ohm (DDR_SetOCD)  */
} DDR_Adapter;

extern DDR_Adapter ddr_adapter;

/* Top-level API — prefer these over direct phy/ddrc calls in new code. */
_LONG_CALL_ void DDR_StructInit(void);
_LONG_CALL_ void DDR_Init(u32 freq_mhz, bool odt_calibration);
_LONG_CALL_ void DDR_ClockInit(u32 freq_mhz);
_LONG_CALL_ void DDR_EnterSelfRefresh(void);
_LONG_CALL_ void DDR_ExitSelfRefresh(void);
_LONG_CALL_ void DDR_SetPASR(u8 pasr);
_LONG_CALL_ int DDR_SetOCD(u8 ca_ohm, u8 dq_ohm);
_LONG_CALL_ void DDR_SetPriority(void);
_LONG_CALL_ void DDR_SetNonce(void);

/* Phase-only entry points — dispatch to the right variant based on
 * CONFIG_FPGA. Prefer DDR_Init() above for new code. */
_LONG_CALL_ void DDR_InitPHY(void);
_LONG_CALL_ void DDR_InitDDRC(void);

#endif /* _AMEBA_DDR_H */
