/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _AMEBA_ETH_PHY_RTL8201F_H_
#define _AMEBA_ETH_PHY_RTL8201F_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file ameba_eth_phy_rtl8201f.h
 * @brief RTL8201F-VB PHY Driver Interface
 *
 * Vendor-specific PHY driver for the RTL8201F. Inherits IEEE 802.3
 * standard operations from ameba_eth_phy.c and adds RTL8201F-specific
 * features (page selection, power saving, RMII clock direction).
 *
 * IEEE 802.3 standard register definitions are in ameba_eth_phy.h.
 */

#define RTL8201F_PHY_ADDR    0x001   /* Default MDIO address */

/* ========================================================================== */
/*                    RTL8201F Specific Definitions                           */
/* ========================================================================== */

/* Page Selection: Register 31, bits [7:0] select page (0x00~0xFF) */
#define RTL8201F_REG_PAGESEL     31          /**< Page Select Register */
#define RTL8201F_PAGE_0          0           /**< Page 0 (Standard Registers) */
#define RTL8201F_PAGE_4          4           /**< Page 4 (EEE Extension Registers) */
#define RTL8201F_PAGE_7          7           /**< Page 7 (Extension Registers) */

/* Page 0, Reg 24: Power Saving Mode Register (PSMR) */
#define RTL8201F_P0_PSMR         24          /**< Power Saving Mode register */
#define RTL8201F_PSMR_DISABLE    0x0310      /**< Vendor-recommended value to fully disable power saving
                                                  (bit 15=0 + undocumented bits 9,8,4 required for PHY loopback) */

/* Page 0, Reg 28: Fiber Mode and Loopback Register */
#define RTL8201F_P0_FMLBR        28          /**< Fiber Mode and Loopback register */
#define RTL8201F_FMLBR_FXMODE    (1 << 5)   /**< 1=Fiber mode enabled */
#define RTL8201F_FMLBR_EN_AUTOMDIX (1 << 2) /**< 1=Auto MDIX enabled (default) */
#define RTL8201F_FMLBR_FORCE_MDI (1 << 1)   /**< When auto-MDIX off: 1=Force MDI, 0=Force MDIX */

/* Page 0, Reg 30: Interrupt Indicators and SNR Display Register */
#define RTL8201F_P0_INTR_SNR     30          /**< Interrupt Indicators / SNR register */
#define RTL8201F_INTR_ANERR      (1 << 15)  /**< Auto-Negotiation Error interrupt (RC) */
#define RTL8201F_INTR_SPDCHG     (1 << 14)  /**< Speed Mode Change interrupt (RC) */
#define RTL8201F_INTR_DUPLEXCHG  (1 << 13)  /**< Duplex Mode Change interrupt (RC) */
#define RTL8201F_INTR_LINKCHG    (1 << 11)  /**< Link Status Change interrupt (RC) */
#define RTL8201F_INTR_SNR_MASK   0x000F     /**< [3:0] Signal to Noise Ratio (RO) */

/* Page 4, Reg 16: EEE Capability Enable Register */
#define RTL8201F_P4_EEE_EN       16          /**< EEE Capability Enable register */
#define RTL8201F_P4_EEE_10M_CAP  (1 << 13)  /**< Enable EEE 10M capability */
#define RTL8201F_P4_EEE_NWAY_EN  (1 << 12)  /**< Enable NWay next-page exchange for EEE 100M */
#define RTL8201F_P4_TX_QUIET_EN  (1 << 9)   /**< Enable TX quiet power-off in EEE */
#define RTL8201F_P4_RX_QUIET_EN  (1 << 8)   /**< Enable RX quiet power-off in EEE */

/* Page 7, Reg 16: RMII Mode Setting Register (RMSR) — datasheet Table 26 */
#define RTL8201F_P7_RMSR         16          /**< RMII Mode Setting Register */
#define RTL8201F_P7_RMII_CLK_DIR (1 << 12)  /**< Rg_rmii_clkdir: 0=TXC output (PHY2MAC), 1=TXC input (MAC2PHY) */
#define RTL8201F_P7_RMII_TX_OFFSET_MASK (0xF << 8) /**< [11:8] Rg_rmii_tx_offset (default 0xF) */
#define RTL8201F_P7_RMII_RX_OFFSET_MASK (0xF << 4) /**< [7:4] Rg_rmii_rx_offset (default 0xF) */
#define RTL8201F_P7_RMII_MODE_EN (1 << 3)   /**< Bit 3: 0=MII mode, 1=RMII mode */
#define RTL8201F_P7_RMII_RXDV_SEL (1 << 2)  /**< Rg_rmii_rxdv_sel: 0=CRS_DV, 1=RXDV */
#define RTL8201F_P7_RMII_RXDSEL (1 << 1)    /**< Rg_rmii_rxdsel: 0=RMII data only, 1=RMII data with SSD Error (default 1) */

/* Page 7, Reg 19: Interrupt, WOL Enable, and LEDs Function Register */
#define RTL8201F_P7_INT_LED      19          /**< Interrupt / WOL / LED function register */
#define RTL8201F_P7_INT_LINKCHG  (1 << 13)  /**< Link Change interrupt mask on INTB pin */
#define RTL8201F_P7_INT_DUPCHG   (1 << 12)  /**< Duplex Change interrupt mask on INTB pin */
#define RTL8201F_P7_INT_ANERR    (1 << 11)  /**< NWay Error interrupt mask on INTB pin */
#define RTL8201F_P7_LED_SEL_MASK (3 << 4)   /**< [5:4] Traditional LED function selection */
#define RTL8201F_P7_CUSTOM_LED   (1 << 3)   /**< 1=Customized LED enabled */

/* Page 7, Reg 24: Spread Spectrum Clock Register */
#define RTL8201F_P7_SSC          24          /**< Spread Spectrum Clock register */
#define RTL8201F_P7_DIS_SSC      (1 << 0)   /**< 1=SSC disabled, 0=SSC enabled (default) */

/* RTL8201F-VB PHY ID: Reg2=0x001C, Reg3=0xC816 (OUI + model 000001 + rev 0110) */
#define RTL8201F_PHY_ID_MATCH    0x001CC816

extern const struct eth_phy_ops Ethernet_PHY_RTL8201F_Ops;
extern struct eth_phy_dev rtl8201f_phy_dev;

void Ethernet_PHY_RTL8201F_DumpRegs(struct eth_phy_dev *phy);

#ifdef __cplusplus
}
#endif

#endif /* _AMEBA_ETH_PHY_RTL8201F_H_ */
