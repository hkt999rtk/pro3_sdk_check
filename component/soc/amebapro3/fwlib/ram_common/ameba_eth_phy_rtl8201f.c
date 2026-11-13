/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file ameba_eth_phy_rtl8201f.c
 * @brief RTL8201F-VB PHY Driver (RMII, 10/100 Mbps)
 * @author wongyiekheng@realtek-sg.com
 */

#include "ameba_soc.h"
#include <stdio.h>
#include <string.h>

static const char *const TAG = "ETH_PHY_RTL8201F";

/* ========================================================================== */
/*                    RTL8201F Page Selection Helper                          */
/* ========================================================================== */

/** @brief Select the RTL8201F register page via the page-select register. */
static void Ethernet_PHY_RTL8201F_SelectPage(struct eth_phy_dev *phy, uint16_t page, int *err)
{
	Ethernet_PHY_WriteSafe(phy, RTL8201F_REG_PAGESEL, page, err);
}

/* ========================================================================== */
/*                    RTL8201F Specific Operations                            */
/* ========================================================================== */

/** @brief Configure the RTL8201F for RMII mode with PHY-sourced reference clock. */
static void Ethernet_PHY_RTL8201F_ConfigureRmii(struct eth_phy_dev *phy, int *err)
{
	uint16_t p7r16;

	Ethernet_PHY_RTL8201F_SelectPage(phy, RTL8201F_PAGE_7, err);
	p7r16 = Ethernet_PHY_ReadSafe(phy, RTL8201F_P7_RMSR, err);
	p7r16 |= RTL8201F_P7_RMII_MODE_EN;
	p7r16 &= ~RTL8201F_P7_RMII_CLK_DIR;
	p7r16 &= ~RTL8201F_P7_RMII_RXDSEL;

	Ethernet_PHY_WriteSafe(phy, RTL8201F_P7_RMSR, p7r16, err);
	Ethernet_PHY_RTL8201F_SelectPage(phy, RTL8201F_PAGE_0, err);
}

/** @brief Wake the RTL8201F from power-down mode if BMCR PDOWN is set. */
static int Ethernet_PHY_RTL8201F_WakeFromPdown(struct eth_phy_dev *phy)
{
	int err = RTK_SUCCESS;
	uint16_t bmcr;

	Ethernet_PHY_RTL8201F_SelectPage(phy, RTL8201F_PAGE_0, &err);
	if (err != RTK_SUCCESS) {
		return err;
	}

	bmcr = Ethernet_PHY_ReadSafe(phy, MII_BMCR, &err);
	if (err != RTK_SUCCESS) {
		return err;
	}

	if (bmcr & BMCR_PDOWN) {
		bmcr &= ~BMCR_PDOWN;
		Ethernet_PHY_WriteSafe(phy, MII_BMCR, bmcr, &err);
		if (err != RTK_SUCCESS) {
			return err;
		}
		DelayMs(5);
	}

	return err;
}

/** @brief Read and verify the PHY ID matches the expected RTL8201F identifier. */
static int Ethernet_PHY_RTL8201F_VerifyID(struct eth_phy_dev *phy)
{
	uint32_t phy_id;

	int err = Ethernet_PHY_ReadID(phy, &phy_id);
	if (err != RTK_SUCCESS) {
		RTK_LOGE(TAG, "PHY ID read failed\n");
		return err;
	}

	if (phy_id != RTL8201F_PHY_ID_MATCH) {
		RTK_LOGE(TAG, "ID mismatch: 0x%08X (expected 0x%08X)\n",
				 phy_id, RTL8201F_PHY_ID_MATCH);
		return RTK_FAIL;
	}

	RTK_LOGI(TAG, "eth0: Realtek RTL8201F (OUI 0x%05X, MDIO addr %d)\n",
			 (phy_id >> 10) & 0x3FFFFF, phy->addr);
	return RTK_SUCCESS;
}

/** @brief Disable RTL8201F power-saving mode to keep REF_CLK active. */
static void Ethernet_PHY_RTL8201F_DisablePowerSaving(struct eth_phy_dev *phy, int *err)
{
	Ethernet_PHY_WriteSafe(phy, RTL8201F_P0_PSMR, RTL8201F_PSMR_DISABLE, err);
}

/** @brief Software-reset the RTL8201F and re-apply RMII and power-saving settings. */
static int Ethernet_PHY_RTL8201F_Reset(struct eth_phy_dev *phy)
{
	int err = RTK_SUCCESS;

	Ethernet_PHY_RTL8201F_SelectPage(phy, RTL8201F_PAGE_0, &err);
	if (err != RTK_SUCCESS) {
		return err;
	}

	err = Ethernet_PHY_Reset(phy);
	if (err != RTK_SUCCESS) {
		return err;
	}

	Ethernet_PHY_WriteSafe(phy, RTL8201F_P0_PSMR, RTL8201F_PSMR_DISABLE, &err);

	/* BMCR reset clears Page 7 — re-configure RMII */
	Ethernet_PHY_RTL8201F_ConfigureRmii(phy, &err);

	return err;
}

/** @brief Configure link parameters on the RTL8201F after selecting Page 0. */
static int Ethernet_PHY_RTL8201F_CfgLink(struct eth_phy_dev *phy, const phy_link_config_t *cfg)
{
	int err = RTK_SUCCESS;

	Ethernet_PHY_RTL8201F_SelectPage(phy, RTL8201F_PAGE_0, &err);
	if (err != RTK_SUCCESS) {
		return err;
	}

	return Ethernet_PHY_CfgLink(phy, cfg);
}

/** @brief Read current link state from the RTL8201F after selecting Page 0. */
static int Ethernet_PHY_RTL8201F_GetLink(struct eth_phy_dev *phy, phy_link_state_t *state)
{
	int err = RTK_SUCCESS;

	Ethernet_PHY_RTL8201F_SelectPage(phy, RTL8201F_PAGE_0, &err);
	if (err != RTK_SUCCESS) {
		return err;
	}

	return Ethernet_PHY_GetLink(phy, state);
}

/** @brief Restart auto-negotiation on the RTL8201F after selecting Page 0. */
static int Ethernet_PHY_RTL8201F_AutonegRestart(struct eth_phy_dev *phy)
{
	int err = RTK_SUCCESS;

	Ethernet_PHY_RTL8201F_SelectPage(phy, RTL8201F_PAGE_0, &err);
	if (err != RTK_SUCCESS) {
		return err;
	}

	return Ethernet_PHY_AutonegRestart(phy);
}

/** @brief Enable or disable PHY-level loopback on the RTL8201F. */
static int Ethernet_PHY_RTL8201F_SetLoopback(struct eth_phy_dev *phy, bool enable)
{
	int err = RTK_SUCCESS;
	uint16_t bmcr;

	Ethernet_PHY_RTL8201F_SelectPage(phy, RTL8201F_PAGE_0, &err);
	if (err != RTK_SUCCESS) {
		return err;
	}

	if (enable) {
		/* PSMR must be disabled — power saving gates REF_CLK in loopback */
		Ethernet_PHY_RTL8201F_DisablePowerSaving(phy, &err);
		bmcr = BMCR_LOOPBACK | BMCR_SPEED100 | BMCR_FULLDPLX;
		Ethernet_PHY_WriteSafe(phy, MII_BMCR, bmcr, &err);
		DelayMs(100);
	} else {
		bmcr = Ethernet_PHY_ReadSafe(phy, MII_BMCR, &err);
		bmcr &= ~BMCR_LOOPBACK;
		Ethernet_PHY_WriteSafe(phy, MII_BMCR, bmcr, &err);
	}

	return err;
}

/** @brief Configure the RTL8201F RMII reference clock direction via Page 7. */
static int Ethernet_PHY_RTL8201F_CfgRefclock(struct eth_phy_dev *phy, enum eth_refclk_dir dir)
{
	int err = RTK_SUCCESS;
	uint16_t val;

	Ethernet_PHY_RTL8201F_SelectPage(phy, RTL8201F_PAGE_7, &err);

	val = Ethernet_PHY_ReadSafe(phy, RTL8201F_P7_RMSR, &err);

	if (err == RTK_SUCCESS) {
		if (dir == ETH_REFCLK_MAC2PHY) {
			val |= RTL8201F_P7_RMII_CLK_DIR;
		} else {
			val &= ~RTL8201F_P7_RMII_CLK_DIR;
		}
		Ethernet_PHY_WriteSafe(phy, RTL8201F_P7_RMSR, val, &err);
	}

	Ethernet_PHY_RTL8201F_SelectPage(phy, RTL8201F_PAGE_0, &err);

	return err;
}

/** @brief Configure Energy Efficient Ethernet (EEE) advertisement on the RTL8201F. */
static int Ethernet_PHY_RTL8201F_CfgEEE(struct eth_phy_dev *phy, phy_eee_mode_t mode)
{
	int err = RTK_SUCCESS;
	uint16_t adv_val = 0;

	if (mode == PHY_EEE_ENABLE_AN) {
		adv_val = EEE_ADVERT_100;
	}

	Ethernet_PHY_RTL8201F_SelectPage(phy, RTL8201F_PAGE_0, &err);

	Ethernet_PHY_MMDWriteSafe(phy, MMD_AN, MMD_EEEAR, adv_val, &err);

	if (err == RTK_SUCCESS) {
		err = Ethernet_PHY_RTL8201F_AutonegRestart(phy);
	}

	return err;
}

/** @brief Register a link-change callback (stub, not yet implemented). */
static int Ethernet_PHY_RTL8201F_SetLinkCallback(struct eth_phy_dev *phy, phy_link_cb_t cb, void *user_data)
{
	if (!phy) {
		return RTK_FAIL;
	}
	UNUSED(cb);
	UNUSED(user_data);
	return RTK_SUCCESS;
}

/* ========================================================================== */
/*                    RTL8201F Register Dump                                  */
/* ========================================================================== */

/** @brief Dump a set of PHY registers from a given RTL8201F page for debugging. */
static void Ethernet_PHY_RTL8201F_DumpPage(struct eth_phy_dev *phy, uint16_t page,
		const u8 *regs, u8 num_regs, const char *prefix)
{
	uint16_t val;
	u8 i;

	Ethernet_GMAC_WritePhyReg(phy->addr, RTL8201F_REG_PAGESEL, page);

	for (i = 0; i < num_regs; i++) {
		val = 0xDEAD;
		Ethernet_GMAC_ReadPhyReg(phy->addr, regs[i], &val);
		RTK_LOGI(TAG, "  %s%02d: %04X\n", prefix, regs[i], val);
	}
}

/**
 * @brief  Dump all relevant RTL8201F PHY registers (Page 0, 4, 7) for debugging.
 * @param  phy  Pointer to the PHY device structure.
 */
void Ethernet_PHY_RTL8201F_DumpRegs(struct eth_phy_dev *phy)
{
	if (!phy) {
		return;
	}

	Ethernet_GMAC_AutoPolling(DISABLE);
	DelayMs(10);

	RTK_LOGI(TAG, "===== RTL8201F PHY Register Dump (addr=%d) =====\n", phy->addr);

	{
		u8 reg;
		Ethernet_GMAC_WritePhyReg(phy->addr, RTL8201F_REG_PAGESEL, RTL8201F_PAGE_0);
		RTK_LOGI(TAG, "--- Page 0 (Standard) ---\n");
		for (reg = 0; reg < 32; reg += 4) {
			uint16_t r0 = 0xDEAD, r1 = 0xDEAD, r2 = 0xDEAD, r3 = 0xDEAD;
			Ethernet_GMAC_ReadPhyReg(phy->addr, reg, &r0);
			Ethernet_GMAC_ReadPhyReg(phy->addr, reg + 1, &r1);
			Ethernet_GMAC_ReadPhyReg(phy->addr, reg + 2, &r2);
			Ethernet_GMAC_ReadPhyReg(phy->addr, reg + 3, &r3);
			RTK_LOGI(TAG, "  R%02d: %04X %04X %04X %04X\n", reg, r0, r1, r2, r3);
		}
	}

	{
		static const u8 page4_regs[] = {16, 21};
		RTK_LOGI(TAG, "--- Page 4 (EEE) ---\n");
		Ethernet_PHY_RTL8201F_DumpPage(phy, RTL8201F_PAGE_4, page4_regs, sizeof(page4_regs), "P4R");
	}

	{
		static const u8 page7_regs[] = {16, 17, 18, 19, 20, 24};
		RTK_LOGI(TAG, "--- Page 7 (Extension) ---\n");
		Ethernet_PHY_RTL8201F_DumpPage(phy, RTL8201F_PAGE_7, page7_regs, sizeof(page7_regs), "P7R");
	}

	Ethernet_GMAC_WritePhyReg(phy->addr, RTL8201F_REG_PAGESEL, RTL8201F_PAGE_0);

	RTK_LOGI(TAG, "==============================================\n");
}

/* ========================================================================== */
/*                    RTL8201F Full Init (called via ops->init)               */
/* ========================================================================== */

/** @brief Full RTL8201F initialization: wake, verify ID, configure RMII, and disable power saving. */
static int Ethernet_PHY_RTL8201F_Init(struct eth_phy_dev *phy)
{
	int err;

	if (!phy) {
		return RTK_FAIL;
	}

	err = Ethernet_PHY_RTL8201F_WakeFromPdown(phy);
	if (err != RTK_SUCCESS) {
		return err;
	}

	err = Ethernet_PHY_RTL8201F_VerifyID(phy);
	if (err != RTK_SUCCESS) {
		return err;
	}

	Ethernet_PHY_RTL8201F_ConfigureRmii(phy, &err);
	Ethernet_PHY_RTL8201F_DisablePowerSaving(phy, &err);

	return err;
}

/* ========================================================================== */
/*                    RTL8201F PHY Ops                                        */
/* ========================================================================== */

const struct eth_phy_ops Ethernet_PHY_RTL8201F_Ops = {
	.init               = Ethernet_PHY_RTL8201F_Init,
	.hw_reset           = Ethernet_PHY_HWReset,
	.reset              = Ethernet_PHY_RTL8201F_Reset,
	.cfg_link           = Ethernet_PHY_RTL8201F_CfgLink,
	.get_link           = Ethernet_PHY_RTL8201F_GetLink,
	.autoneg_restart    = Ethernet_PHY_RTL8201F_AutonegRestart,
	.cfg_refclock       = Ethernet_PHY_RTL8201F_CfgRefclock,
	.cfg_eee            = Ethernet_PHY_RTL8201F_CfgEEE,
	.set_loopback       = Ethernet_PHY_RTL8201F_SetLoopback,
	.set_link_callback  = Ethernet_PHY_RTL8201F_SetLinkCallback,
};

struct eth_phy_dev rtl8201f_phy_dev;

