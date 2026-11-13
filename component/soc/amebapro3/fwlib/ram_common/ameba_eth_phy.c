/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file ameba_eth_phy.c
 * @brief IEEE 802.3 Generic Base PHY Driver
 * @author wongyiekheng@realtek-sg.com
 */

#include "ameba_soc.h"

/* ========================================================================== */
/*                    PHY Device Init (base layer)                            */
/* ========================================================================== */

/**
 * @brief  Initialize the RTL8201F PHY device structure with default RMII settings.
 */
void Ethernet_PHY_StructInit(void)
{
	struct eth_phy_dev *phy = &rtl8201f_phy_dev;

	memset(phy, 0, sizeof(*phy));
	phy->bus  = &Ethernet_GMAC_MdioBus;
	phy->ops  = &Ethernet_PHY_RTL8201F_Ops;
	phy->addr = RTL8201F_PHY_ADDR;
	phy->iface_cfg.tx_setup_time = ETH_PHY_TX_SETUP_TIME_6NS;
	phy->iface_cfg.rx_setup_time = ETH_PHY_RX_SETUP_TIME_8NS;
	phy->iface_cfg.refclk_dir = ETH_REFCLK_PHY2MAC;
	phy->iface_cfg.eee_enable = 0;
}

/**
 * @brief  Apply PHY-specific interface configuration (reference clock, EEE).
 * @param  phy  Pointer to the PHY device structure.
 */
void Ethernet_PHY_Configure(struct eth_phy_dev *phy)
{
	if (!phy || !phy->ops) {
		return;
	}
	if (phy->ops->cfg_refclock) {
		phy->ops->cfg_refclock(phy, phy->iface_cfg.refclk_dir);
	}
	if (phy->iface_cfg.eee_enable && phy->ops->cfg_eee) {
		phy->ops->cfg_eee(phy, PHY_EEE_ENABLE_AN);
	}
}

/**
 * @brief  Perform a hardware reset of the PHY via GPIO toggle (weak, override as needed).
 */
__weak void Ethernet_PHY_HWReset(void)
{
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_Pin = _PF_3;
	GPIO_Init(&gpio);
	GPIO_WriteBit(_PF_3, 0);
	DelayMs(20);
	GPIO_WriteBit(_PF_3, 1);
	DelayMs(100);
}

/**
 * @brief  Initialize the PHY: call driver init and apply interface configuration (weak, override as needed).
 */
__weak void Ethernet_PHY_Init(void)
{
	struct eth_phy_dev *phy = &rtl8201f_phy_dev;

	if (!phy->ops) {
		Ethernet_PHY_StructInit();
	}

	if (phy->ops->hw_reset) {
		phy->ops->hw_reset();
	}

	if (phy->ops->init) {
		phy->ops->init(phy);
	}

	Ethernet_PHY_Configure(phy);
}

/**
 * @brief  Enable or disable PHY loopback mode via the PHY driver ops.
 * @param  phy     Pointer to the PHY device structure.
 * @param  enable  true to enable, false to disable.
 * @retval RTK_SUCCESS on success, RTK_FAIL if PHY or ops not available.
 */
int Ethernet_PHY_SetLoopback(struct eth_phy_dev *phy, bool enable)
{
	if (!phy || !phy->ops || !phy->ops->set_loopback) {
		return RTK_FAIL;
	}
	return phy->ops->set_loopback(phy, enable);
}

/* ========================================================================== */
/*                    MDIO Read / Write                                       */
/* ========================================================================== */

/**
 * @brief  Read a PHY register via MDIO.
 * @param  phy  Pointer to the PHY device structure.
 * @param  reg  Register address to read.
 * @param  val  Pointer to store the register value.
 * @retval RTK_SUCCESS on success, RTK_FAIL on invalid parameters.
 */
int Ethernet_PHY_Read(struct eth_phy_dev *phy, uint32_t reg, uint16_t *val)
{
	if (!phy || !phy->bus || !phy->bus->mdio_read) {
		return RTK_FAIL;
	}
	return phy->bus->mdio_read(phy->addr, reg, val);
}

/**
 * @brief  Write a PHY register via MDIO.
 * @param  phy  Pointer to the PHY device structure.
 * @param  reg  Register address to write.
 * @param  val  Value to write.
 * @retval RTK_SUCCESS on success, RTK_FAIL on invalid parameters.
 */
int Ethernet_PHY_Write(struct eth_phy_dev *phy, uint32_t reg, uint16_t val)
{
	if (!phy || !phy->bus || !phy->bus->mdio_write) {
		return RTK_FAIL;
	}
	return phy->bus->mdio_write(phy->addr, reg, val);
}

/* ========================================================================== */
/*                Safe Access Helpers (Error Latching)                        */
/* ========================================================================== */

/**
 * @brief  Read a PHY register with error latching; skips the read if a prior error occurred.
 * @param  phy  Pointer to the PHY device structure.
 * @param  reg  Register address to read.
 * @param  err  Pointer to accumulated error status (set on failure, checked on entry).
 * @retval Register value on success, 0 on error or if a prior error was latched.
 */
uint16_t Ethernet_PHY_ReadSafe(struct eth_phy_dev *phy, uint32_t reg, int *err)
{
	uint16_t val = 0;
	int ret;

	if (*err != RTK_SUCCESS) {
		return 0;
	}

	ret = Ethernet_PHY_Read(phy, reg, &val);
	if (ret != RTK_SUCCESS) {
		*err = ret;
		return 0;
	}

	return val;
}

/**
 * @brief  Write a PHY register with error latching; skips the write if a prior error occurred.
 * @param  phy  Pointer to the PHY device structure.
 * @param  reg  Register address to write.
 * @param  val  Value to write.
 * @param  err  Pointer to accumulated error status (set on failure, checked on entry).
 */
void Ethernet_PHY_WriteSafe(struct eth_phy_dev *phy, uint32_t reg, uint16_t val, int *err)
{
	int ret;

	if (*err != RTK_SUCCESS) {
		return;
	}

	ret = Ethernet_PHY_Write(phy, reg, val);
	if (ret != RTK_SUCCESS) {
		*err = ret;
	}
}

/* ========================================================================== */
/*                MMD Access (Clause 45 over Clause 22)                       */
/* ========================================================================== */

/**
 * @brief  Write an MMD (Clause 45 over Clause 22) register with error latching.
 * @param  phy       Pointer to the PHY device structure.
 * @param  dev_addr  MMD device address.
 * @param  reg_addr  MMD register address.
 * @param  data      Value to write.
 * @param  err       Pointer to accumulated error status.
 */
void Ethernet_PHY_MMDWriteSafe(struct eth_phy_dev *phy, uint8_t dev_addr,
							   uint16_t reg_addr, uint16_t data, int *err)
{
	Ethernet_PHY_WriteSafe(phy, MII_MMD_CTRL, MMD_FUNC_ADDR | dev_addr, err);
	Ethernet_PHY_WriteSafe(phy, MII_MMD_DATA, reg_addr, err);
	Ethernet_PHY_WriteSafe(phy, MII_MMD_CTRL, MMD_FUNC_DATA | dev_addr, err);
	Ethernet_PHY_WriteSafe(phy, MII_MMD_DATA, data, err);
}

/**
 * @brief  Read an MMD (Clause 45 over Clause 22) register with error latching.
 * @param  phy       Pointer to the PHY device structure.
 * @param  dev_addr  MMD device address.
 * @param  reg_addr  MMD register address.
 * @param  err       Pointer to accumulated error status.
 * @retval Register value on success, 0 on error.
 */
uint16_t Ethernet_PHY_MMDReadSafe(struct eth_phy_dev *phy, uint8_t dev_addr,
								  uint16_t reg_addr, int *err)
{
	Ethernet_PHY_WriteSafe(phy, MII_MMD_CTRL, MMD_FUNC_ADDR | dev_addr, err);
	Ethernet_PHY_WriteSafe(phy, MII_MMD_DATA, reg_addr, err);
	Ethernet_PHY_WriteSafe(phy, MII_MMD_CTRL, MMD_FUNC_DATA | dev_addr, err);
	return Ethernet_PHY_ReadSafe(phy, MII_MMD_DATA, err);
}

/* ========================================================================== */
/*                    Link Status Parsing                                     */
/* ========================================================================== */

/** @brief Parse BMSR/BMCR/LPA/ADV registers to determine link speed and duplex. */
static void Ethernet_PHY_ParseLinkStatus(uint16_t bmsr, uint16_t bmcr,
		uint16_t lpa, uint16_t adv,
		phy_link_state_t *state)
{
	uint16_t common = lpa & adv;

	state->link_up = (bmsr & BMSR_LSTATUS) ? true : false;

	if (!state->link_up) {
		state->speed = 0;
		state->duplex = 0;
		return;
	}

	if (!(bmcr & BMCR_ANENABLE)) {
		state->speed = (bmcr & BMCR_SPEED100) ? PHY_SPEED_100M : PHY_SPEED_10M;
		state->duplex = (bmcr & BMCR_FULLDPLX) ? PHY_DUPLEX_FULL : PHY_DUPLEX_HALF;
		return;
	}

	if (common & ADVERTISE_100FULL) {
		state->speed = PHY_SPEED_100M;
		state->duplex = PHY_DUPLEX_FULL;
	} else if (common & ADVERTISE_100HALF) {
		state->speed = PHY_SPEED_100M;
		state->duplex = PHY_DUPLEX_HALF;
	} else if (common & ADVERTISE_10FULL) {
		state->speed = PHY_SPEED_10M;
		state->duplex = PHY_DUPLEX_FULL;
	} else {
		state->speed = PHY_SPEED_10M;
		state->duplex = PHY_DUPLEX_HALF;
	}
}

/* ========================================================================== */
/*                    Standard PHY Operations                                 */
/* ========================================================================== */

/**
 * @brief  Read the 32-bit PHY identifier (PHYSID1 << 16 | PHYSID2).
 * @param  phy     Pointer to the PHY device structure.
 * @param  phy_id  Pointer to store the combined PHY ID.
 * @retval RTK_SUCCESS on success, error code on MDIO failure.
 */
int Ethernet_PHY_ReadID(struct eth_phy_dev *phy, uint32_t *phy_id)
{
	int err = RTK_SUCCESS;
	uint16_t id1, id2;

	id1 = Ethernet_PHY_ReadSafe(phy, MII_PHYSID1, &err);
	id2 = Ethernet_PHY_ReadSafe(phy, MII_PHYSID2, &err);

	if (err != RTK_SUCCESS) {
		return err;
	}

	*phy_id = ((uint32_t)id1 << 16) | id2;
	return RTK_SUCCESS;
}

/**
 * @brief  Issue a software reset to the PHY via BMCR and wait for completion.
 * @param  phy  Pointer to the PHY device structure.
 * @retval RTK_SUCCESS on success, error code on MDIO failure.
 */
int Ethernet_PHY_Reset(struct eth_phy_dev *phy)
{
	int err = RTK_SUCCESS;

	Ethernet_PHY_WriteSafe(phy, MII_BMCR, BMCR_RESET, &err);

	if (err == RTK_SUCCESS) {
		DelayMs(20);
	}

	return err;
}

/** @brief Set the PHY advertisement register and enable auto-negotiation. */
static int Ethernet_PHY_CfgAutoneg(struct eth_phy_dev *phy, const phy_link_config_t *cfg)
{
	int err = RTK_SUCCESS;
	uint16_t bmcr, advertise;

	advertise = ADVERTISE_CSMA;

	if (cfg->advertise_100m) {
		advertise |= (ADVERTISE_100FULL | ADVERTISE_100HALF);
	}
	if (cfg->advertise_10m) {
		advertise |= (ADVERTISE_10FULL | ADVERTISE_10HALF);
	}
	if (cfg->advertise_pause) {
		advertise |= ADVERTISE_PAUSE_CAP;
	}
	if (cfg->advertise_asym_pause) {
		advertise |= ADVERTISE_PAUSE_ASYM;
	}

	Ethernet_PHY_WriteSafe(phy, MII_ADVERTISE, advertise, &err);

	bmcr = Ethernet_PHY_ReadSafe(phy, MII_BMCR, &err);
	bmcr |= (BMCR_ANENABLE | BMCR_ANRESTART);
	Ethernet_PHY_WriteSafe(phy, MII_BMCR, bmcr, &err);

	return err;
}

/** @brief Force the PHY to a specific speed and duplex without auto-negotiation. */
static int Ethernet_PHY_CfgForced(struct eth_phy_dev *phy, const phy_link_config_t *cfg)
{
	int err = RTK_SUCCESS;
	uint16_t bmcr;
	uint16_t bmcr_mask;

	bmcr = Ethernet_PHY_ReadSafe(phy, MII_BMCR, &err);

	bmcr_mask = BMCR_SPEED100 | BMCR_FULLDPLX | BMCR_ANENABLE;
	bmcr &= ~bmcr_mask;

	if (cfg->speed == PHY_SPEED_100M) {
		bmcr |= BMCR_SPEED100;
	}
	if (cfg->duplex == PHY_DUPLEX_FULL) {
		bmcr |= BMCR_FULLDPLX;
	}

	Ethernet_PHY_WriteSafe(phy, MII_BMCR, bmcr, &err);

	return err;
}

/**
 * @brief  Configure PHY link parameters (auto-negotiation or forced speed/duplex).
 * @param  phy  Pointer to the PHY device structure.
 * @param  cfg  Pointer to desired link configuration.
 * @retval RTK_SUCCESS on success, RTK_FAIL on invalid parameters.
 */
int Ethernet_PHY_CfgLink(struct eth_phy_dev *phy, const phy_link_config_t *cfg)
{
	if (!phy || !cfg) {
		return RTK_FAIL;
	}

	if (cfg->autoneg_en) {
		return Ethernet_PHY_CfgAutoneg(phy, cfg);
	} else {
		return Ethernet_PHY_CfgForced(phy, cfg);
	}
}

/**
 * @brief  Read current PHY link status, speed, and duplex from standard MII registers.
 * @param  phy    Pointer to the PHY device structure.
 * @param  state  Pointer to store the parsed link state.
 * @retval RTK_SUCCESS on success, RTK_FAIL on invalid parameters, error code on MDIO failure.
 */
int Ethernet_PHY_GetLink(struct eth_phy_dev *phy, phy_link_state_t *state)
{
	int err = RTK_SUCCESS;
	uint16_t bmsr, bmcr, lpa, adv;

	if (!state) {
		return RTK_FAIL;
	}

	/* BMSR link status is latching-low — read twice to get current state */
	Ethernet_PHY_ReadSafe(phy, MII_BMSR, &err);
	bmsr = Ethernet_PHY_ReadSafe(phy, MII_BMSR, &err);

	bmcr = Ethernet_PHY_ReadSafe(phy, MII_BMCR, &err);
	lpa  = Ethernet_PHY_ReadSafe(phy, MII_LPA, &err);
	adv  = Ethernet_PHY_ReadSafe(phy, MII_ADVERTISE, &err);

	if (err != RTK_SUCCESS) {
		return err;
	}

	Ethernet_PHY_ParseLinkStatus(bmsr, bmcr, lpa, adv, state);
	return RTK_SUCCESS;
}

/**
 * @brief  Restart auto-negotiation on the PHY.
 * @param  phy  Pointer to the PHY device structure.
 * @retval RTK_SUCCESS on success, error code on MDIO failure.
 */
int Ethernet_PHY_AutonegRestart(struct eth_phy_dev *phy)
{
	int err = RTK_SUCCESS;
	uint16_t val;

	val = Ethernet_PHY_ReadSafe(phy, MII_BMCR, &err);
	val |= (BMCR_ANENABLE | BMCR_ANRESTART);
	Ethernet_PHY_WriteSafe(phy, MII_BMCR, val, &err);

	return err;
}


