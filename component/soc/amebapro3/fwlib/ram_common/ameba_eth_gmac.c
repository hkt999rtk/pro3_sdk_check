/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file ameba_eth_gmac.c
 * @brief AmebaPro3 GMAC (Ethernet MAC) Driver
 * @author wongyiekheng@realtek-sg.com
 */

#include "ameba_soc.h"
#include <stdio.h>
#include <string.h>

static const char *const TAG = "ETH";

/* ========================================================================== */
/*                     Interrupt Handling                                      */
/* ========================================================================== */

/**
 * @brief  Clear all pending GMAC interrupt status flags.
 */
void Ethernet_GMAC_ClearAllINT(void)
{
	u32 all_isr_mask = ETH_BIT_ISR_ROK | ETH_BIT_ISR_RER_OVF | ETH_BIT_ISR_RER_RUNT |
					   ETH_BIT_ISR_TOK_TI | ETH_BIT_ISR_TER | ETH_BIT_ISR_LINKCHG |
					   ETH_BIT_ISR_CNT_WRAP | ETH_BIT_ISR_SWINT | ETH_BIT_ISR_TDU |
					   ETH_BIT_ISR_RDU1 | ETH_BIT_ISR_RDU2 | ETH_BIT_ISR_RDU3 |
					   ETH_BIT_ISR_RDU4 | ETH_BIT_ISR_RDU5 | ETH_BIT_ISR_RDU6;

	GMAC_DEV->ETH_ISR_IMR |= all_isr_mask;
}

/**
 * @brief  Get the bitmask of currently pending and enabled GMAC interrupt events.
 * @retval Bitmask of ETH_EVT_* flags indicating active interrupts.
 */
u32 Ethernet_GMAC_GetPendingINT(void)
{
	volatile u32 raw_reg = GMAC_DEV->ETH_ISR_IMR;
	u32 int_status = ETH_EVT_NO_EVENT;

	if ((raw_reg & ETH_BIT_ISR_ROK) && (raw_reg & ETH_BIT_IMR_ROK)) {
		int_status |= ETH_EVT_RX_DONE;
	}

	if ((raw_reg & ETH_BIT_ISR_TOK_TI) && (raw_reg & ETH_BIT_IMR_TOK_TI)) {
		int_status |= ETH_EVT_TX_DONE;
	}

	if ((raw_reg & ETH_BIT_ISR_LINKCHG) && (raw_reg & ETH_BIT_IMR_LINKCHG)) {
		int_status |= ETH_EVT_LINK_CHG;
	}

	if (((raw_reg & ETH_BIT_ISR_RER_OVF) && (raw_reg & ETH_BIT_IMR_RER_OVF)) ||
		((raw_reg & ETH_BIT_ISR_RER_RUNT) && (raw_reg & ETH_BIT_IMR_RER_RUNT))) {
		int_status |= ETH_EVT_RX_ERROR;
	}

	if ((raw_reg & ETH_BIT_ISR_TER) && (raw_reg & ETH_BIT_IMR_TER)) {
		int_status |= ETH_EVT_TX_ERROR;
	}

	if ((raw_reg & ETH_BIT_ISR_RDU1) && (raw_reg & ETH_BIT_IMR_RDU1)) {
		int_status |= ETH_EVT_RDU_RING1;
	}

	return int_status;
}

/**
 * @brief  Clear specific GMAC interrupt status flags by event bitmask.
 * @param  int_events  Bitmask of ETH_EVT_* flags to clear.
 */
void Ethernet_GMAC_ClearINT(u32 int_events)
{
	u32 clear_mask = 0;

	if (int_events & ETH_EVT_RX_DONE) {
		clear_mask |= ETH_BIT_ISR_ROK;
	}
	if (int_events & ETH_EVT_TX_DONE) {
		clear_mask |= ETH_BIT_ISR_TOK_TI;
	}
	if (int_events & ETH_EVT_LINK_CHG) {
		clear_mask |= ETH_BIT_ISR_LINKCHG;
	}
	if (int_events & ETH_EVT_RX_ERROR) {
		clear_mask |= (ETH_BIT_ISR_RER_OVF | ETH_BIT_ISR_RER_RUNT);
	}
	if (int_events & ETH_EVT_TX_ERROR) {
		clear_mask |= ETH_BIT_ISR_TER;
	}
	if (int_events & ETH_EVT_RDU_RING1) {
		clear_mask |= ETH_BIT_ISR_RDU1;
	}

	if (clear_mask != 0) {
		/* Write only target ISR bits (W1C) + preserve IMR bits [31:16].
		 * Writing 0 to other ISR bits is a no-op for W1C. */
		GMAC_DEV->ETH_ISR_IMR = clear_mask | (GMAC_DEV->ETH_ISR_IMR & 0xFFFF0000);
	}
}

/**
 * @brief  Enable or disable GMAC interrupt mask bits for specified events.
 * @param  int_config  Bitmask of ETH_EVT_* flags to configure.
 * @param  enable      ENABLE or DISABLE the specified interrupts.
 */
void Ethernet_GMAC_ConfigINT(u32 int_config, u32 enable)
{
	u32 imr_mask = 0;

	if (int_config & ETH_EVT_RX_DONE) {
		imr_mask |= ETH_BIT_IMR_ROK;
	}
	if (int_config & ETH_EVT_TX_DONE) {
		imr_mask |= ETH_BIT_IMR_TOK_TI;
	}
	if (int_config & ETH_EVT_LINK_CHG) {
		imr_mask |= ETH_BIT_IMR_LINKCHG;
	}
	if (int_config & ETH_EVT_RX_ERROR) {
		imr_mask |= (ETH_BIT_IMR_RER_OVF | ETH_BIT_IMR_RER_RUNT);
	}
	if (int_config & ETH_EVT_TX_ERROR) {
		imr_mask |= ETH_BIT_IMR_TER;
	}
	if (int_config & ETH_EVT_RDU_RING1) {
		imr_mask |= ETH_BIT_IMR_RDU1;
	}

	/* Only modify IMR bits [31:16]; write 0 to ISR bits [15:0] to avoid
	 * inadvertently clearing pending W1C flags via read-modify-write. */
	u32 cur_imr = GMAC_DEV->ETH_ISR_IMR & 0xFFFF0000;
	if (enable) {
		GMAC_DEV->ETH_ISR_IMR = cur_imr | imr_mask;
	} else {
		GMAC_DEV->ETH_ISR_IMR = (cur_imr & ~imr_mask);
	}
}


/* ========================================================================== */
/*                     Descriptor & Address Management                        */
/* ========================================================================== */

/**
 * @brief  Write the TX and RX descriptor ring base addresses to GMAC hardware registers.
 * @param  gmac  Pointer to GMAC configuration with initialized descriptor pointers.
 */
void Ethernet_GMAC_SetDescAddr(GMAC_InitTypeDef *gmac)
{
	if ((gmac == NULL) || (gmac->ETH_TxDesc == NULL) || (gmac->ETH_RxDesc == NULL)) {
		RTK_LOGE(TAG, "Invalid para\n");
		return;
	}

	GMAC_DEV->ETH_TXFDP1 = (u32)(gmac->ETH_TxDesc);
	GMAC_DEV->ETH_RXFDP1 = (u32)(gmac->ETH_RxDesc);
}

/**
 * @brief  Retrieve the MAC address (currently returns a default address).
 * @param  ETH_MacAddr  Buffer of at least 6 bytes to receive the MAC address.
 */
void Ethernet_GMAC_GetMacAddr(u8 *ETH_MacAddr)
{
	static const u8 default_mac[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

	if (ETH_MacAddr == NULL) {
		return;
	}

	memcpy(ETH_MacAddr, default_mac, ETH_MAC_ADDR_LEN);

	/* TODO: read from OTP 0x72A
	for (u8 i = 0; i < ETH_MAC_ADDR_LEN; i++) {
		OTP_Read8(0x72A + i, &ETH_MacAddr[i]);
	}
	*/
}

/** @brief Set the RMII reference clock direction (PHY-to-MAC or MAC-to-PHY). */
void Ethernet_GMAC_SetRefclkDirec(u32 refclk_mode)
{
	if (refclk_mode) {
		GMAC_DEV->ETH_MSR |= ETH_BIT_REFCLK_ON;
	} else {
		GMAC_DEV->ETH_MSR &= ~ETH_BIT_REFCLK_ON;
	}
}

/**
 * @brief  Enable or disable MDIO auto-polling of the PHY.
 * @param  opt  ENABLE or DISABLE.
 */
void Ethernet_GMAC_AutoPolling(u32 opt)
{
	if (opt == DISABLE) {
		GMAC_DEV->ETH_MIIAR |= ETH_BIT_DISABLE_AUTO_POLLING;
	} else {
		GMAC_DEV->ETH_MIIAR &= ~ETH_BIT_DISABLE_AUTO_POLLING;
	}
}

/* ========================================================================== */
/*                     Loopback & Filter Configuration                        */
/* ========================================================================== */

/** @brief Enable or disable GMAC internal loopback mode. */
void Ethernet_GMAC_SetLoopback(u32 mode)
{
	GMAC_DEV->ETH_TCR &= ~ETH_MASK_LBK;
	GMAC_DEV->ETH_TCR |= ETH_LBK(mode);
}

/** @brief Force MAC link speed, duplex, and link-up state for loopback testing. */
void Ethernet_GMAC_ForceLink(u32 speed, u32 duplex)
{
	GMAC_DEV->ETH_MSR &= ~(ETH_BIT_TXFCE | ETH_BIT_RXFCE);
	GMAC_DEV->ETH_MSR &= ~ETH_MASK_FORCE_SPD;
	GMAC_DEV->ETH_MSR |= ETH_BIT_FORCE_SPD_MODE | ETH_FORCE_SPD(speed);
	GMAC_DEV->ETH_MSR |= ETH_FORCEDFULLDUP(duplex);
	GMAC_DEV->ETH_MSR |= ETH_BIT_FORCELINK;
}

/** @brief Enable or disable jumbo frame reception. */
void Ethernet_GMAC_SetRxJumbo(u32 enable)
{
	if (enable) {
		GMAC_DEV->ETH_CR |= ETH_BIT_RXJUMBO;
	} else {
		GMAC_DEV->ETH_CR &= ~ETH_BIT_RXJUMBO;
	}
}

/** @brief Set GMAC RX packet filter mode. */
void Ethernet_GMAC_SetRxFilter(u32 filter_mode)
{
	switch (filter_mode) {
	case ETH_FILTER_WITH_MULTICAST:
		GMAC_DEV->ETH_RCR = ETH_BIT_APM | ETH_BIT_AB | ETH_BIT_AM;
		break;
	case ETH_FILTER_STRICT_UNICAST:
		GMAC_DEV->ETH_RCR = ETH_BIT_APM;
		break;
	case ETH_FILTER_PROMISCUOUS:
		GMAC_DEV->ETH_RCR = ETH_BIT_AAP | ETH_BIT_AB | ETH_BIT_AM;
		break;
	case ETH_FILTER_DIAGNOSTIC_ALL:
		GMAC_DEV->ETH_RCR = ETH_BIT_AAP | ETH_BIT_APM | ETH_BIT_AB | ETH_BIT_AM | ETH_BIT_AR | ETH_BIT_AER;
		break;
	default:
		GMAC_DEV->ETH_RCR = ETH_BIT_APM | ETH_BIT_AB | ETH_BIT_AM;
		break;
	}
}

/* ========================================================================== */
/*                     TX / RX Packet Operations                              */
/* ========================================================================== */

/**
 * @brief  Get the buffer pointer for the next available TX descriptor, if owned by the CPU.
 * @param  gmac  Pointer to the GMAC configuration structure.
 * @retval Pointer to the TX buffer, or NULL if the descriptor is still owned by DMA.
 */
u8 *Ethernet_GMAC_GetTXPktInfo(GMAC_InitTypeDef *gmac)
{
	u8 tx_idx;
	u8 *buf = NULL;

	if (gmac == NULL) {
		RTK_LOGE(TAG, "Invalid para\n");
		return NULL;
	}

	tx_idx = gmac->ETH_TxDescCurrentNum;

	DCache_Invalidate((u32)&gmac->ETH_TxDesc[tx_idx], sizeof(eth_tx_desc_t));

	if ((((u32)(gmac->ETH_TxDesc[tx_idx].dw1)) & FEMAC_TX_DSC_BIT_OWN) == 0) {
		buf = (u8 *)gmac->ETH_TxDesc[tx_idx].addr;
	} else {
		GMAC_DEV->ETH_ISR_IMR = ETH_BIT_ISR_TOK_TI | (GMAC_DEV->ETH_ISR_IMR & 0xFFFF0000);
	}

	return buf;
}

/**
 * @brief  Populate the current TX descriptor with packet metadata and trigger transmission.
 * @param  gmac  Pointer to the GMAC configuration structure.
 * @param  size  Packet size in bytes (must not exceed ETH_TxBufSize).
 */
void Ethernet_GMAC_UpdateTXDESCAndSend(GMAC_InitTypeDef *gmac, u32 size)
{
	u8 tx_idx;

	if (gmac == NULL) {
		RTK_LOGE(TAG, "Invalid para\n");
		return;
	}

	tx_idx = gmac->ETH_TxDescCurrentNum;

	if (size > gmac->ETH_TxBufSize) {
		RTK_LOGE(TAG, "Invalid size !!\r\n");
		return;
	}

	DCache_Clean((u32)gmac->ETH_TxDesc[tx_idx].addr, (u32)size);
	gmac->ETH_TxDesc[tx_idx].dw1 = FEMAC_TX_DSC_BIT_FS | FEMAC_TX_DSC_BIT_IPCS |
								   FEMAC_TX_DSC_BIT_L4CS | FEMAC_TX_DSC_BIT_LS | FEMAC_TX_DSC_BIT_CRC |
								   FEMAC_TX_DSC_BIT_SIZE(size);
	gmac->ETH_TxDesc[tx_idx].dw2 = 0;
	gmac->ETH_TxDesc[tx_idx].dw3 = 0;
	gmac->ETH_TxDesc[tx_idx].dw4 = 0;

	if (tx_idx == ((gmac->ETH_TxDescNum) - 1)) {
		gmac->ETH_TxDesc[tx_idx].dw1 |= FEMAC_TX_DSC_BIT_EOR;
	}

	gmac->ETH_TxDesc[tx_idx].dw1 |= FEMAC_TX_DSC_BIT_OWN;

	/* CleanInvalidate (not Clean): adjacent descriptors share 32B cache lines.
	 * A plain Clean keeps the stale line — later eviction overwrites DMA's OWN=0. */
	DCache_CleanInvalidate((u32)&gmac->ETH_TxDesc[tx_idx], sizeof(eth_tx_desc_t));
	__DSB();

	GMAC_DEV->ETH_ISR_IMR = ETH_BIT_ISR_TOK_TI | (GMAC_DEV->ETH_ISR_IMR & 0xFFFF0000);
	GMAC_DEV->ETH_IO_CMD0 |= ETH_BIT_TXFN1ST;

	gmac->ETH_TxDescCurrentNum = (tx_idx + 1) % (gmac->ETH_TxDescNum);
}

/**
 * @brief  Get the received packet buffer and length from the current RX descriptor.
 * @param  gmac    Pointer to the GMAC configuration structure.
 * @param  rx_len  Pointer to store the received packet length.
 * @retval Pointer to the received packet data, or NULL if no packet is available.
 */
u8 *Ethernet_GMAC_GetRXPktInfo(GMAC_InitTypeDef *gmac, u32 *rx_len)
{
	u32 pkt_size = 0;
	u8 rx_idx = gmac->ETH_RxDescCurrentNum;
	u8 *buf = NULL;

	DCache_Invalidate((u32)&gmac->ETH_RxDesc[rx_idx], sizeof(eth_rx_desc_t));

	if (((gmac->ETH_RxDesc[rx_idx].dw1) & FEMAC_RX_DSC_BIT_OWN) == 0) {
		pkt_size = FEMAC_RX_DSC_LEN(gmac->ETH_RxDesc[rx_idx].dw1);
		buf = (u8 *)(gmac->ETH_RxDesc[rx_idx].addr + 2);
		DCache_Invalidate((u32)(gmac->ETH_RxDesc[rx_idx].addr), (u32)pkt_size);
	}

	*rx_len = pkt_size;
	return buf;
}

/**
 * @brief  Release the current RX descriptor back to DMA and advance the ring index.
 * @param  gmac  Pointer to the GMAC configuration structure.
 */
void Ethernet_GMAC_UpdateRXDESC(GMAC_InitTypeDef *gmac)
{
	u8 read_idx;

	if (gmac == NULL) {
		RTK_LOGE(TAG, "Invalid para!\n");
		return;
	}

	read_idx = gmac->ETH_RxDescCurrentNum;

	gmac->ETH_RxDesc[read_idx].dw1 &= FEMAC_RX_DSC_BIT_EOR;
	gmac->ETH_RxDesc[read_idx].dw1 |= (FEMAC_RX_DSC_BIT_OWN | gmac->ETH_RxBufSize);
	gmac->ETH_RxDesc[read_idx].dw2 = 0;
	gmac->ETH_RxDesc[read_idx].dw3 = 0;

	DCache_CleanInvalidate((u32)&gmac->ETH_RxDesc[read_idx], sizeof(eth_rx_desc_t));

	GMAC_DEV->ETH_ISR_IMR = ETH_BIT_ISR_ROK | (GMAC_DEV->ETH_ISR_IMR & 0xFFFF0000);
	gmac->ETH_RxDescCurrentNum = (read_idx + 1) % (gmac->ETH_RxDescNum);
}

/* ========================================================================== */
/*                     Link Status                                            */
/* ========================================================================== */

/**
 * @brief  Check if the Ethernet link is currently up.
 * @retval 1 if link is up, 0 if link is down.
 */
u32 Ethernet_GMAC_GetLinkStatus(void)
{
	return !ETH_GET_LINKB(GMAC_DEV->ETH_MSR);
}

/** @brief Get the effective duplex mode, accounting for forced-duplex override. */
static u32 Ethernet_GetEffectiveDuplex(void)
{
	if (GMAC_DEV->ETH_MSR & ETH_BIT_FORCEDFULLDUP) {
		return ETH_FULL_DUPLEX;
	}

	return ETH_GET_FULLDUPREG(GMAC_DEV->ETH_MSR);
}

/**
 * @brief  Log the current MAC link speed and duplex mode.
 */
void Ethernet_GMAC_GetSpeedDuplex(void)
{
	u32 speed = ETH_GET_SPEED(GMAC_DEV->ETH_MSR);
	u32 duplex = Ethernet_GetEffectiveDuplex();

	RTK_LOGI(TAG, "MAC Link Info: %s, %s\n",
			 (speed == ETH_SPEED_100M) ? "100 Mb/s" :
			 (speed == ETH_SPEED_10M)  ? "10 Mb/s"  : "Unknown",
			 (duplex == ETH_FULL_DUPLEX) ? "Full Duplex" : "Half Duplex");
}

/** @brief Retrieve the current MAC link speed and duplex into caller-supplied pointers. */
void Ethernet_GMAC_GetLinkInfo(u32 *speed, u32 *duplex)
{
	if (speed) {
		*speed = ETH_GET_SPEED(GMAC_DEV->ETH_MSR);
	}
	if (duplex) {
		*duplex = Ethernet_GetEffectiveDuplex();
	}
}

/**
 * @brief  Force the MAC to a specific speed and duplex setting.
 * @param  speed   ETH_SPEED_100M or ETH_SPEED_10M.
 * @param  duplex  ETH_FULL_DUPLEX or ETH_HALF_DUPLEX.
 */
void Ethernet_GMAC_UpdateMacSpeed(u32 speed, u32 duplex)
{
	GMAC_DEV->ETH_MSR &= ~ETH_MASK_FORCE_SPD;
	GMAC_DEV->ETH_MSR |= ETH_BIT_FORCE_SPD_MODE | ETH_FORCE_SPD(speed);
	GMAC_DEV->ETH_MSR &= ~ETH_BIT_FORCEDFULLDUP;
	GMAC_DEV->ETH_MSR |= ETH_FORCEDFULLDUP(duplex);
}

/**
 * @brief  De-initialize the GMAC: disable TX/RX, clear interrupts, and reset the MAC.
 */
void Ethernet_GMAC_DeInit(void)
{
	GMAC_DEV->ETH_IO_CMD0 &= ~(ETH_BIT_TE | ETH_BIT_RE);
	Ethernet_GMAC_AutoPolling(DISABLE);

	Ethernet_GMAC_ClearAllINT();
	Ethernet_GMAC_ConfigINT(0xFF, DISABLE);

	GMAC_DEV->ETH_CR |= ETH_BIT_RST;
}

/** @brief Enable the GMAC RX ring 1 in the IO command register. */
void Ethernet_GMAC_EnableRx(void)
{
	GMAC_DEV->ETH_IO_CMD1 |= ETH_BIT_RXRING1;
}

/* ========================================================================== */
/*                     MDIO Read / Write (Clause 22)                          */
/* ========================================================================== */

/** @brief Poll the MDIO busy flag until idle or timeout. */
static inline int Ethernet_WaitPhyIdle(ETHERNET_TypeDef *regs, uint32_t timeout_us)
{
	volatile u32 cnt = timeout_us * 10;

	while (cnt--) {
		if (!(regs->ETH_MIIAR & ETH_BIT_MDIO_BUSY)) {
			return RTK_SUCCESS;
		}
	}
	return -RTK_ERR_TIMEOUT;
}

/** @brief Disable auto-polling and wait for MDIO idle before a manual MDIO transaction. */
static int Ethernet_MdioLock(void)
{
	Ethernet_GMAC_AutoPolling(DISABLE);

	if (Ethernet_WaitPhyIdle(GMAC_DEV, ETH_MDIO_WAIT_TIME) != RTK_SUCCESS) {
		return -RTK_ERR_BUSY;
	}

	/* Discard stale auto-poll data */
	(void)GMAC_DEV->ETH_MIIAR;

	return RTK_SUCCESS;
}

/**
 * @brief  Read a PHY register via the GMAC MDIO interface (Clause 22).
 * @param  phy_addr  PHY address (0-31).
 * @param  reg_addr  Register address (0-31).
 * @param  data      Pointer to store the read value.
 * @retval RTK_SUCCESS on success, negative error code on failure.
 */
int Ethernet_GMAC_ReadPhyReg(uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)
{
	if (reg_addr > 31) {
		return -RTK_FAIL;
	}

	if (Ethernet_MdioLock() != RTK_SUCCESS) {
		return -RTK_ERR_BUSY;
	}

	GMAC_DEV->ETH_MIIAR = (ETH_BIT_DISABLE_AUTO_POLLING |
						   ETH_PHYADDRESS(phy_addr) | ETH_REGADDR4_0(reg_addr));

	if (Ethernet_WaitPhyIdle(GMAC_DEV, ETH_MDIO_WAIT_TIME) != RTK_SUCCESS) {
		return -RTK_ERR_BUSY;
	}

	*data = (uint16_t)ETH_GET_DATA15_0(GMAC_DEV->ETH_MIIAR);

	return RTK_SUCCESS;
}

/**
 * @brief  Write a PHY register via the GMAC MDIO interface (Clause 22).
 * @param  phy_addr  PHY address (0-31).
 * @param  reg_addr  Register address (0-31).
 * @param  data      Value to write.
 * @retval RTK_SUCCESS on success, negative error code on failure.
 */
int Ethernet_GMAC_WritePhyReg(uint8_t phy_addr, uint8_t reg_addr, uint16_t data)
{
	if (reg_addr > 31) {
		return -RTK_FAIL;
	}

	if (Ethernet_MdioLock() != RTK_SUCCESS) {
		return -RTK_ERR_BUSY;
	}

	GMAC_DEV->ETH_MIIAR = (ETH_BIT_FLAG | ETH_BIT_DISABLE_AUTO_POLLING |
						   ETH_PHYADDRESS(phy_addr) | ETH_REGADDR4_0(reg_addr) | data);

	return RTK_SUCCESS;
}

/* ========================================================================== */
/*                     MDIO Bus Operations (for PHY driver)                   */
/* ========================================================================== */

const struct eth_mdio_ops Ethernet_GMAC_MdioBus = {
	.mdio_read  = Ethernet_GMAC_ReadPhyReg,
	.mdio_write = Ethernet_GMAC_WritePhyReg,
};

/* ========================================================================== */
/*                     Clock & Pinmux Setup                                   */
/* ========================================================================== */

/**
 * @brief  Enable GMAC clocks, configure the clock divider, and set up RMII pin mux.
 */
void Ethernet_GMAC_ClkSet(void)
{
	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_LX_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_LX_BUS, APBPeriph_LXBUS_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_PBUS_SLOW_PCLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_CBUS_SYS_MEM_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_GMAC, APBPeriph_GMAC_CLOCK, ENABLE);

	RCC_PeriphClockDividerSet(SYS_GMAC_CLK, 9, 0);		// GMAC running 500/10 = 50MHz
	RCC_PeriphClockSourceSet(SYS_GMAC_CLK, SYS2_PLL);	// 50MHz is expected frequency setting from AmebaPro2, confirmed with DV - Hong Giap

	CORE_S_BASE->SYS_0_CLK_CTRL |= (CORE_S_BIT_SYS_GMAC_LCLKEN | CORE_S_BIT_SYS_GMAC_PCLKEN | CORE_S_BIT_SYS_GMAC_SCLKEN);
	CORE_S_BASE->SYS_1_FUNC_CTRL |= CORE_S_BIT_SYS_FEN_S7_34;

	for (u8 pin = _PE_3; pin <= _PE_12; pin++) {
		Pinmux_Config(pin, PINMUX_FUNCTION_RMII);
		PAD_DrvStrength(pin, PAD_DRV_ABILITITY_HIGH);
	}
}

/* ========================================================================== */
/*                     Initialization                                         */
/* ========================================================================== */

/**
 * @brief  Fill a GMAC configuration structure with default values for RMII 10/100 operation.
 * @param  gmac  Pointer to the GMAC configuration structure to initialize.
 */
void Ethernet_GMAC_StructInit(GMAC_InitTypeDef *gmac)
{
	memset(gmac, 0, sizeof(GMAC_InitTypeDef));

	gmac->MacConfig.Bits.MacMode          = ETH_MAC_MODE;
	gmac->MacConfig.Bits.Loopback         = ETH_LPB_NONE;
	gmac->MacConfig.Bits.IFGTime          = ETH_IFG_3;
	gmac->MacConfig.Bits.RefClkDir        = ETH_REFCLK_PHY2MAC;
	gmac->MacConfig.Bits.RefClkPhase      = ETH_SAMPLED_ON_RISING_EDGE;
	gmac->MacConfig.Bits.RxJumbo          = ETH_RX_JUMBO_DISABLE;
	gmac->MacConfig.Bits.PktFilterConfig  = ETH_FILTER_PROMISCUOUS;
	gmac->MacConfig.Bits.EEEEnable        = DISABLE;
	gmac->MacConfig.Bits.AutoNego         = ETH_NWAY_ENABLE;

	gmac->DMA_RxThreshold = ETH_RX_THRESHOLD_256B;
	gmac->DMA_TxThreshold = ETH_TX_THRESHOLD_256B;
	gmac->DMA_RxTriggerLevel = ETH_TRIGGER_LEVEL_1_PKT;
	gmac->DMA_TxTriggerLevel = ETH_TRIGGER_LEVEL_1_PKT;

	gmac->ETH_RxDescNum = ETH_RX_DESC_NUM_DEFAULT;
	gmac->ETH_TxDescNum = ETH_TX_DESC_NUM_DEFAULT;

	gmac->ETH_TxBufSize = ETH_PKT_BUFF_SZ;
	gmac->ETH_RxBufSize = ETH_PKT_BUFF_SZ;

	gmac->ETH_IntMaskAndStatus = ETH_BIT_IMR_LINKCHG | ETH_BIT_IMR_TOK_TI |
								 ETH_BIT_IMR_RER_OVF | ETH_BIT_IMR_ROK | 0xFFFF;

	// FIXME: REMOVE FIXED MAC ADDRESS
	{
		static const u8 default_mac[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
		memcpy(gmac->ETH_MacAddr, default_mac, 6);
	}
}

/** @brief Reset the GMAC MAC core and configure base clock and DMA swap settings. */
static int Ethernet_InitMacReset(u32 smsa_addr)
{
	u32 timeout = 1000;

	/* REF_CLK must be present from PHY before MAC reset */
	DelayMs(50);
	GMAC_DEV->ETH_CR |= ETH_BIT_RST;

	while ((GMAC_DEV->ETH_CR & ETH_BIT_RST) && timeout--) {
		DelayUs(10);
	}
	if (timeout == 0) {
		RTK_LOGE(TAG, "MAC Reset Timeout\r\n");
		return -RTK_ERR_TIMEOUT;
	}

	Ethernet_GMAC_AutoPolling(DISABLE);

	GMAC_DEV->ETH_CLK_CTRL_REG = ETH_BIT_TXCLK_EN | ETH_BIT_RXCLK_EN | ETH_BIT_DMA_DATA_SWAP |
								 ETH_BIT_REV_00E4_3 | ETH_BIT_REV_00E4_4;

	GMAC_DEV->ETH_CONFIG = ETH_RFF_SIZE_SEL(0x2);
	GMAC_DEV->ETH_SMSA = smsa_addr;

	return RTK_SUCCESS;
}

/** @brief Configure PHY speed/duplex from GMAC config and start auto-negotiation if enabled. */
static void Ethernet_InitSpeedDuplex(GMAC_InitTypeDef *gmac, struct eth_phy_dev *phy)
{
	if (gmac->MacConfig.Bits.AutoNego) {
		phy->link_cfg.autoneg_en = TRUE;
		phy->link_cfg.advertise_100m = TRUE;
		phy->link_cfg.advertise_10m = TRUE;
	} else {
		phy->link_cfg.autoneg_en = FALSE;
		if (gmac->MacConfig.Bits.Speed == ETH_SPEED_100M) {
			phy->link_cfg.speed = PHY_SPEED_100M;
			phy->link_cfg.advertise_100m = TRUE;
		} else {
			phy->link_cfg.speed = PHY_SPEED_10M;
			phy->link_cfg.advertise_10m = TRUE;
		}
		GMAC_DEV->ETH_MSR &= ~ETH_MASK_FORCE_SPD;
		GMAC_DEV->ETH_MSR |= ETH_BIT_FORCE_SPD_MODE | ETH_FORCE_SPD(gmac->MacConfig.Bits.Speed);

		if (gmac->MacConfig.Bits.Duplex == ETH_FULL_DUPLEX) {
			phy->link_cfg.duplex = PHY_DUPLEX_FULL;
		} else {
			phy->link_cfg.duplex = PHY_DUPLEX_HALF;
		}
		GMAC_DEV->ETH_MSR &= ~ETH_BIT_FORCEDFULLDUP;
		GMAC_DEV->ETH_MSR |= ETH_FORCEDFULLDUP(gmac->MacConfig.Bits.Duplex);
	}

	if (phy->ops->cfg_link) {
		phy->ops->cfg_link(phy, &phy->link_cfg);
	}

	if (gmac->MacConfig.Bits.AutoNego) {
		if (phy->ops->autoneg_restart) {
			phy->ops->autoneg_restart(phy);
		}
	}
}

/** @brief Apply MAC-level configuration: RMII mode, flow control, IFG, and RX checksum. */
static void Ethernet_InitMacConfig(GMAC_InitTypeDef *gmac)
{
	GMAC_DEV->ETH_MSR |= ETH_BIT_REG_RMII2MII_EN;
	Ethernet_GMAC_SetRefclkDirec(gmac->MacConfig.Bits.RefClkDir);
	GMAC_DEV->ETH_MSR |= ETH_BIT_TXFCE | ETH_BIT_RXFCE;
	GMAC_DEV->ETH_MSR |= ETH_REFCLK_PHASE(gmac->MacConfig.Bits.RefClkPhase);

	GMAC_DEV->ETH_TCR &= ~ETH_MASK_IFG2_0;
	GMAC_DEV->ETH_TCR |= ETH_IFG2_0(gmac->MacConfig.Bits.IFGTime);
	GMAC_DEV->ETH_TCR &= ~ETH_MASK_LBK;

	GMAC_DEV->ETH_CR |= ETH_BIT_RXCHKSUM;
	if (gmac->MacConfig.Bits.RxJumbo == ETH_RX_JUMBO_ENABLE) {
		GMAC_DEV->ETH_CR |= ETH_BIT_RXJUMBO;
	}
}

/** @brief Write the MAC address from the GMAC config into hardware ID registers. */
static void Ethernet_InitMacAddr(GMAC_InitTypeDef *gmac)
{
	u8 *m = gmac->ETH_MacAddr;

	GMAC_DEV->ETH_IDR0 = (m[0] << 24) | (m[1] << 16) | (m[2] << 8) | m[3];
	GMAC_DEV->ETH_IDR4 = (m[4] << 24) | (m[5] << 16);
}

/** @brief Configure the RX packet filter from GMAC config. */
static void Ethernet_InitRxFilter(GMAC_InitTypeDef *gmac)
{
	Ethernet_GMAC_SetRxFilter(gmac->MacConfig.Bits.PktFilterConfig);
}

/** @brief Initialize TX and RX descriptor rings with buffer addresses and ownership flags. */
static void Ethernet_InitDescriptors(GMAC_InitTypeDef *gmac,
									 eth_buf_node_t *tx_ring_head, eth_buf_node_t *rx_ring_head,
									 u8 *tx_pkt_buf, u8 *rx_pkt_buf)
{
	u8 i;

	{
		eth_buf_node_t *tx_node = tx_ring_head;
		for (i = 0; i < gmac->ETH_TxDescNum; i++) {
			gmac->ETH_TxDesc[i].dw1 = 0;
			gmac->ETH_TxDesc[i].addr = tx_node ?
									   (u32)tx_node->buf :
									   (u32)(tx_pkt_buf + i * gmac->ETH_TxBufSize);
			gmac->ETH_TxDesc[i].dw2 = 0;
			gmac->ETH_TxDesc[i].dw3 = 0;
			gmac->ETH_TxDesc[i].dw4 = 0;
			if (tx_node) {
				tx_node = tx_node->next;
			}
		}
	}

	{
		eth_buf_node_t *rx_node = rx_ring_head;
		for (i = 0; i < gmac->ETH_RxDescNum; i++) {
			gmac->ETH_RxDesc[i].dw1 = FEMAC_RX_DSC_BIT_OWN | gmac->ETH_RxBufSize;
			if (i == gmac->ETH_RxDescNum - 1) {
				gmac->ETH_RxDesc[i].dw1 |= FEMAC_RX_DSC_BIT_EOR;
			}
			gmac->ETH_RxDesc[i].addr = rx_node ?
									   (u32)rx_node->buf :
									   (u32)(rx_pkt_buf + i * gmac->ETH_RxBufSize);
			gmac->ETH_RxDesc[i].dw2 = 0;
			gmac->ETH_RxDesc[i].dw3 = 0;
			if (rx_node) {
				rx_node = rx_node->next;
			}
		}
	}

	DCache_Clean((u32)gmac->ETH_TxDesc,
				 gmac->ETH_TxDescNum * sizeof(eth_tx_desc_t));
	DCache_Clean((u32)gmac->ETH_RxDesc,
				 gmac->ETH_RxDescNum * sizeof(eth_rx_desc_t));
}

/** @brief Configure DMA engine registers, initialize descriptors, and enable TX/RX. */
static void Ethernet_InitDma(GMAC_InitTypeDef *gmac,
							 eth_buf_node_t *tx_ring_head, eth_buf_node_t *rx_ring_head,
							 u8 *tx_pkt_buf, u8 *rx_pkt_buf)
{
	GMAC_DEV->ETH_ETNRXCPU1 = ETH_RX_PSE_DES_THRES_OFF_1_7_0(0x01) |
							  ETH_RX_PSE_DES_THRES_ON_1_11_8((gmac->ETH_RxDescNum - 2) >> 8) |
							  ETH_RX_PSE_DES_THRES_ON_1_7_0(gmac->ETH_RxDescNum - 2) |
							  ETH_ETHRNTRXCPU_DES_NUM_1_11_8((gmac->ETH_RxDescNum - 1) >> 8) |
							  ETH_ETHRNTRXCPU_DES_NUM_1_7_0(gmac->ETH_RxDescNum - 1);
	GMAC_DEV->ETH_RX_RINGSIZE1 |= ETH_RXRINGSIZE_1_LOW(gmac->ETH_RxDescNum - 1);

	/* IO_CMD1 unlock sequence required by hardware */
	GMAC_DEV->ETH_IO_CMD1 = 0x7ffffeff;
	(void)GMAC_DEV->ETH_IO_CMD1;
	GMAC_DEV->ETH_IO_CMD1 = ETH_DSC_FORMAT_EXTRA(0x3) | ETH_BIT_EN_4GB | ETH_BIT_RXRING1;

	GMAC_DEV->ETH_IO_CMD0 = ETH_RXFTH(gmac->DMA_RxThreshold) |
							ETH_TSH(gmac->DMA_TxThreshold) |
							ETH_BIT_SHORTDESFORMAT;

	/* Clear unused TX ring 2-5 descriptor pointers (not zeroed by MAC reset) */
	GMAC_DEV->ETH_TXFDP2 = 0;
	GMAC_DEV->ETH_TXFDP3 = 0;
	GMAC_DEV->ETH_TXFDP4 = 0;
	GMAC_DEV->ETH_TXFDP5 = 0;

	Ethernet_GMAC_SetDescAddr(gmac);
	Ethernet_InitDescriptors(gmac, tx_ring_head, rx_ring_head, tx_pkt_buf, rx_pkt_buf);

	GMAC_DEV->ETH_IO_CMD0 |= ETH_BIT_TE | ETH_BIT_RE;

	/* Clear all pending ISR bits (W1C). Caller configures IMR after init. */
	GMAC_DEV->ETH_ISR_IMR = 0x0000FFFF;

	Ethernet_GMAC_AutoPolling(ENABLE);
}

/**
 * @brief  Initialize the GMAC hardware: reset MAC, configure registers, set up DMA descriptors, and start TX/RX.
 * @param  gmac          Pointer to the GMAC configuration (descriptors must be pre-allocated).
 * @param  phy           Pointer to the PHY device for link configuration.
 * @param  tx_ring_head  Head of the TX buffer node list.
 * @param  rx_ring_head  Head of the RX buffer node list.
 * @retval RTK_SUCCESS on success, negative error code on failure.
 */
int Ethernet_GMAC_Init(GMAC_InitTypeDef *gmac, struct eth_phy_dev *phy,
					   eth_buf_node_t *tx_ring_head, eth_buf_node_t *rx_ring_head)
{
	static bool hw_initialized = false;
	int32_t ret;

	if (!hw_initialized) {
#if defined(CONFIG_ARM_CORE_CM4_NP)
		Ethernet_GMAC_ClkSet();
#endif
		hw_initialized = true;
	}

	if (!gmac->ETH_TxDesc || !gmac->ETH_RxDesc) {
		RTK_LOGE(TAG, "DMA buffers not allocated\n");
		return RTK_FAIL;
	}

	u32 smsa = rx_ring_head ? (u32)rx_ring_head->buf : 0;
	ret = Ethernet_InitMacReset(smsa);
	if (ret) {
		return ret;
	}

	Ethernet_InitMacConfig(gmac);
	Ethernet_InitMacAddr(gmac);
	Ethernet_InitRxFilter(gmac);
	Ethernet_InitSpeedDuplex(gmac, phy);

	/* Pass ring heads for descriptor init; use first node's buf as fallback pkt_buf */
	{
		u8 *tx_pkt_buf = tx_ring_head ? tx_ring_head->buf : NULL;
		u8 *rx_pkt_buf = rx_ring_head ? rx_ring_head->buf : NULL;
		Ethernet_InitDma(gmac, tx_ring_head, rx_ring_head, tx_pkt_buf, rx_pkt_buf);
	}

	return RTK_SUCCESS;
}

/**
 * @brief  Dump all GMAC MAC and DMA registers for debugging.
 */
void Ethernet_GMAC_DumpRegs(void)
{
	u32 base = RMII_REG_BASE;
	u32 i;

	RTK_LOGI(TAG, "===== GMAC Register Dump (base=0x%08X) =====\n", base);

	RTK_LOGI(TAG, "--- MAC Registers 0x00~0xFF ---\n");
	for (i = 0x00; i < 0x100; i += 0x10) {
		RTK_LOGI(TAG, "0x%04X: %08X %08X %08X %08X\n", i,
				 HAL_READ32(base, i),
				 HAL_READ32(base, i + 4),
				 HAL_READ32(base, i + 8),
				 HAL_READ32(base, i + 12));
	}

	RTK_LOGI(TAG, "--- DMA Registers 0x1300~0x143F ---\n");
	for (i = 0x1300; i < 0x1440; i += 0x10) {
		RTK_LOGI(TAG, "0x%04X: %08X %08X %08X %08X\n", i,
				 HAL_READ32(base, i),
				 HAL_READ32(base, i + 4),
				 HAL_READ32(base, i + 8),
				 HAL_READ32(base, i + 12));
	}

	RTK_LOGI(TAG, "==============================================\n");
}

