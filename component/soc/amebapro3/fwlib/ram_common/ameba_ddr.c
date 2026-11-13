/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define COMPIL_LOG_LEVEL        RTK_LOG_DEBUG

#include "ameba_soc.h"
#include "ameba_ddr.h"

static const char *TAG = "DDR";

DDR_Adapter ddr_adapter = {
	.phy_dev         = DDR_ACTIVE_PHY_OPS,
	.ddrc_dev        = DDR_ACTIVE_DDRC_OPS,
	.timing          = DDR_ACTIVE_TIMING,
	.die             = DDR_ACTIVE_DIE,
	.odt_calibration = DDR_PHY_ODT_CALIBRATION,
	.ca_ocd          = DDR_PHY_CA_OCD,
	.dq_ocd          = DDR_PHY_DQ_OCD,
};

/** @brief Restore any unset ddr_adapter slot to this build's default. */
void DDR_StructInit(void)
{
	if (ddr_adapter.phy_dev == NULL) {
		ddr_adapter.phy_dev = DDR_ACTIVE_PHY_OPS;
	}
	if (ddr_adapter.ddrc_dev == NULL) {
		ddr_adapter.ddrc_dev = DDR_ACTIVE_DDRC_OPS;
	}
	if (ddr_adapter.timing == NULL) {
		ddr_adapter.timing = DDR_ACTIVE_TIMING;
	}
	if (ddr_adapter.die == NULL) {
		ddr_adapter.die = DDR_ACTIVE_DIE;
	}
}

/** @brief Clock the DDR path: DDRC + PHY clocks and resets, PLL frequency.
  *        The resets make DDR unusable until DDR_InitDDRC() raises is_up
  *        again. The die rail is separate: DRAM_Die_Power(). */
void DDR_ClockInit(u32 freq_mhz)
{
	ddr_adapter.is_up = false;

	DDRC_Clock(ENABLE);
	DDRC_Reset();
	DDR_PHY_Clock(ENABLE);
	DDR_PHY_Reset();
	DDR_PHY_SetPLLFreq(freq_mhz);
}

/** @brief Full DDR bring-up: power, clock, PHY, DDRC. Repeatable, and loses
  *        DRAM contents. A live DDR is precharged first: the die is never
  *        reset, so its banks must be closed for the replayed LMR/MRS to be
  *        legal. */
void DDR_Init(u32 freq_mhz, bool odt_calibration)
{
	if (ddr_adapter.is_up) {
		DDRC_PrechargeAll();
	}

	DRAM_Die_Power(ENABLE);
	DDR_ClockInit(freq_mhz);
	ddr_adapter.odt_calibration = odt_calibration;
	DDR_InitPHY();
	DDR_InitDDRC();
	RTK_LOGI(TAG, "DDR initialized\n");
}

/** @brief Enter DRAM self-refresh, then power down the DDR pads and PHY.
  *        The PHY must stop driving the pins: alive, its forced-on CKE driver
  *        overpowers the PWDPAD15N pull-low and the die leaves self-refresh.
  * @note  All-or-nothing - powering down without the SRE leaves the die in
  *        precharge power-down, decaying and unrecoverable without a reboot. */
void DDR_EnterSelfRefresh(void)
{
	if (ddr_adapter.ddrc_dev->EnterSelfRefresh == NULL) {
		RTK_LOGE(TAG, "self refresh not supported on this build\n");
		return;
	}

	ddr_adapter.ddrc_dev->EnterSelfRefresh();

	ddr_adapter.is_up = false;
	CORE_S_BASE->SYS_DDRPHY_CTRL &= ~CORE_S_BIT_PWDPAD15N;
	CORE_S_BASE->SYS_2_CLK_CTRL &= ~CORE_S_BIT_DDRC_CLK_SEL;
	DelayUs(10);

	DDR_PHY_PLLOff();
	DDR_PHY_Clock(DISABLE);
	DDRC_Clock(DISABLE);
}

/** @brief  Select the JEDEC_MR2_PASR_* mode applied via MRS at each
  *         self-refresh entry; banks outside it decay during the dwell.
  *         Exit re-init restores full-array MR2. */
void DDR_SetPASR(u8 pasr)
{
	ddr_adapter.pasr = pasr & JEDEC_MR2_PASR_MASK;
}

static bool DDR_IsValidOCD(u8 ohm)
{
	return (ohm == 40) || (ohm == 60) || (ohm == 80) || (ohm == 100);
}

/** @brief  Select the PHY OCD drive strength applied by the next PHY init.
  *         Only the ODT-calibrated path uses it; ODT off keeps fixed codes.
  * @param  ca_ohm: 40 / 60 / 80 / 100
  * @param  dq_ohm: 40 / 60 / 80 / 100
  * @retval RTK_SUCCESS, or -RTK_ERR_BADARG with the selection unchanged */
int DDR_SetOCD(u8 ca_ohm, u8 dq_ohm)
{
	if (!DDR_IsValidOCD(ca_ohm) || !DDR_IsValidOCD(dq_ohm)) {
		return -RTK_ERR_BADARG;
	}

	ddr_adapter.ca_ocd = ca_ohm;
	ddr_adapter.dq_ocd = dq_ohm;
	return RTK_SUCCESS;
}

/** @brief Exit DRAM self-refresh via the active DDRC device */
void DDR_ExitSelfRefresh(void)
{
	if (ddr_adapter.ddrc_dev->ExitSelfRefresh == NULL) {
		RTK_LOGE(TAG, "self refresh not supported on this build\n");
		return;
	}

	ddr_adapter.ddrc_dev->ExitSelfRefresh();
}

/* -------------------------------------------------------------------------
 * Phase-only entry points
 * ------------------------------------------------------------------------- */

/** @brief Init only the DDR PHY for the active build, with the latched ODT
  *        choice (Kconfig default, or the last DDR_Init() override). */
void DDR_InitPHY(void)
{
	ddr_adapter.phy_dev->Init(ddr_adapter.odt_calibration);
}

/** @brief Init only the DDRC: config, LMR, commit, QoS. */
void DDR_InitDDRC(void)
{
	ddr_adapter.ddrc_dev->InitConfig(ddr_adapter.timing);
	ddr_adapter.ddrc_dev->InitDRAM(ddr_adapter.die->mr);
	ddr_adapter.ddrc_dev->CR_UPDATE();
	ddr_adapter.is_up = true;

#if defined(CONFIG_DDR_ENABLE_SET_PRIORITY) && (CONFIG_DDR_ENABLE_SET_PRIORITY == 1)
	DDR_SetPriority();
#endif
#if defined(CONFIG_DDR_ENABLE_SET_NONCE) && (CONFIG_DDR_ENABLE_SET_NONCE == 1)
	DDR_SetNonce();
#endif
}

/** @brief Apply per-port DDR QoS priority (read + write) */
void DDR_SetPriority(void)
{
	// DDRC_SetReadPriority();
	DDRC_SetWritePriority();
}

/** @brief Seed all 8 SEC_NONCE registers for DRAM address scrambling */
void DDR_SetNonce(void)
{
	DDRC_SetNonce(0);
	DDRC_SetNonce(1);
	DDRC_SetNonce(2);
	DDRC_SetNonce(3);
	DDRC_SetNonce(4);
	DDRC_SetNonce(5);
	DDRC_SetNonce(6);
	DDRC_SetNonce(7);
}
