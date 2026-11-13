/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _AMEBA_ETH_PHY_H_
#define _AMEBA_ETH_PHY_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file ameba_eth_phy.h
 * @brief IEEE 802.3 Clause 22 Standard PHY Register Definitions
 *        and Generic Base PHY Driver Interface.
 *
 * This header provides:
 *   1. Standard MII register addresses (Clause 22, Reg 0-31)
 *   2. Standard register bit definitions (BMCR, BMSR, ADVERTISE, LPA)
 *   3. MMD (Clause 45 over Clause 22) definitions
 *   4. Generic base PHY driver functions that operate on standard registers
 *
 * Vendor-specific PHY drivers (e.g., RTL8210F) should include this header
 * and call base functions for standard IEEE 802.3 operations.
 */

/* ========================================================================== */
/*           IEEE 802.3 Clause 22 - Basic Register Set (Reg 0-31)             */
/* ========================================================================== */

#define MII_BMCR            0x00    /**< Basic Mode Control Register */
#define MII_BMSR            0x01    /**< Basic Mode Status Register */
#define MII_PHYSID1         0x02    /**< PHY Identifier 1 */
#define MII_PHYSID2         0x03    /**< PHY Identifier 2 */
#define MII_ADVERTISE       0x04    /**< Auto-Negotiation Advertisement */
#define MII_LPA             0x05    /**< Auto-Neg Link Partner Ability */
#define MII_EXPANSION       0x06    /**< Auto-Neg Expansion Register */
#define MII_NEXTPAGE        0x07    /**< Auto-Neg Next Page Transmit */
#define MII_LPNEXTPAGE      0x08    /**< Auto-Neg Link Partner Next Page */
#define MII_CTRL1000        0x09    /**< 1000BASE-T Control (Gigabit PHY only) */
#define MII_STAT1000        0x0A    /**< 1000BASE-T Status (Gigabit PHY only) */
#define MII_ESTATUS         0x0F    /**< Extended Status */
#define MII_MMD_CTRL        0x0D    /**< MMD Access Control (Clause 45 over 22) */
#define MII_MMD_DATA        0x0E    /**< MMD Access Data */

/* ========================================================================== */
/*                        Register Bit Definitions                            */
/* ========================================================================== */

/* --- BMCR: Basic Mode Control Register (Reg 0x00) --- */
#define BMCR_RESET          (1 << 15)   /**< 1=Software Reset */
#define BMCR_LOOPBACK       (1 << 14)   /**< 1=Loopback enabled */
#define BMCR_SPEED100       (1 << 13)   /**< 1=100Mbps, 0=10Mbps (ignored if AN enabled) */
#define BMCR_ANENABLE       (1 << 12)   /**< 1=Auto-Negotiation Enabled */
#define BMCR_PDOWN          (1 << 11)   /**< 1=Power Down */
#define BMCR_ISOLATE        (1 << 10)   /**< 1=Isolate PHY from MII */
#define BMCR_ANRESTART      (1 << 9)    /**< 1=Restart Auto-Negotiation */
#define BMCR_FULLDPLX       (1 << 8)    /**< 1=Full Duplex, 0=Half Duplex */
#define BMCR_CTST           (1 << 7)    /**< 1=Collision Test */
#define BMCR_SPEED1000      (1 << 6)    /**< 1=1000Mbps (MSB of Speed select) */

/* --- BMSR: Basic Mode Status Register (Reg 0x01) --- */
#define BMSR_100T4          (1 << 15)   /**< 100BASE-T4 Capable */
#define BMSR_100FULL        (1 << 14)   /**< 100BASE-TX Full Duplex Capable */
#define BMSR_100HALF        (1 << 13)   /**< 100BASE-TX Half Duplex Capable */
#define BMSR_10FULL         (1 << 12)   /**< 10BASE-T Full Duplex Capable */
#define BMSR_10HALF         (1 << 11)   /**< 10BASE-T Half Duplex Capable */
#define BMSR_ESTATEN        (1 << 8)    /**< Extended Status Support */
#define BMSR_ANEGCOMPLETE   (1 << 5)    /**< 1=Auto-Negotiation Complete */
#define BMSR_RFAULT         (1 << 4)    /**< Remote Fault */
#define BMSR_ANEGCAPABLE    (1 << 3)    /**< Auto-Negotiation Capable */
#define BMSR_LSTATUS        (1 << 2)    /**< Link Status (1=Up, 0=Down) */
#define BMSR_JABBER         (1 << 1)    /**< Jabber Detected */
#define BMSR_ERCAP          (1 << 0)    /**< Extended Register Capable */

/* --- ADVERTISE: Auto-Neg Advertisement (Reg 0x04) --- */
#define ADVERTISE_NPAGE     (1 << 15)   /**< Next Page */
#define ADVERTISE_RFAULT    (1 << 13)   /**< Remote Fault */
#define ADVERTISE_PAUSE_CAP (1 << 10)   /**< Pause Capable */
#define ADVERTISE_PAUSE_ASYM (1 << 11)  /**< Asymmetric Pause Capable */
#define ADVERTISE_100T4     (1 << 9)    /**< 100BASE-T4 */
#define ADVERTISE_100FULL   (1 << 8)    /**< 100BASE-TX Full Duplex */
#define ADVERTISE_100HALF   (1 << 7)    /**< 100BASE-TX Half Duplex */
#define ADVERTISE_10FULL    (1 << 6)    /**< 10BASE-T Full Duplex */
#define ADVERTISE_10HALF    (1 << 5)    /**< 10BASE-T Half Duplex */
#define ADVERTISE_CSMA      (1 << 0)    /**< Selector Field (00001=IEEE 802.3) */

#define ADVERTISE_ALL       (ADVERTISE_10HALF | ADVERTISE_10FULL | \
                             ADVERTISE_100HALF | ADVERTISE_100FULL)

/* --- LPA: Link Partner Ability (Reg 0x05) --- */
#define LPA_NPAGE           (1 << 15)
#define LPA_LPACK           (1 << 14)   /**< Link Partner Acknowledge */
#define LPA_RFAULT          (1 << 13)
#define LPA_PAUSE_ASYM      (1 << 11)
#define LPA_PAUSE_CAP       (1 << 10)
#define LPA_100T4           (1 << 9)
#define LPA_100FULL         (1 << 8)
#define LPA_100HALF         (1 << 7)
#define LPA_10FULL          (1 << 6)
#define LPA_10HALF          (1 << 5)

/* ========================================================================== */
/*                     MMD (MDIO Manageable Device) Definitions               */
/* ========================================================================== */

#define MMD_PMA_PMD         1
#define MMD_WIS             2
#define MMD_PCS             3
#define MMD_PHY_XS          4
#define MMD_DTE_XS          5
#define MMD_AN              7   /**< Auto-Negotiation (EEE Advertisement here) */
#define MMD_VEND1           30
#define MMD_VEND2           31
#define MMD_EEEAR           60  /**< MMD EEE Advertisement Register (Clause 45, Dev 7, Reg 60) */

/* --- MMD Access Control Bits (Reg 0x0D) --- */
#define MMD_FUNC_ADDR       (0x0000)    /**< Function: Address */
#define MMD_FUNC_DATA       (0x4000)    /**< Function: Data, no post increment */
#define MMD_FUNC_DATA_INC   (0x8000)    /**< Function: Data, post increment */
#define MMD_FUNC_DATA_INC_WR (0xC000)   /**< Function: Data, post inc on write only */

/* --- EEE Advertisement (MMD 7, Reg 60) --- */
#define EEE_ADVERT_100      (1 << 1)    /**< Advertise 100BASE-TX EEE */
#define EEE_ADVERT_1000     (1 << 2)    /**< Advertise 1000BASE-T EEE */

/* ========================================================================== */
/*                       PHY Driver Types                                     */
/* ========================================================================== */

typedef enum {
	PHY_SPEED_10M   = 10,
	PHY_SPEED_100M  = 100,
	PHY_SPEED_1000M = 1000
} phy_speed_t;

typedef enum {
	PHY_DUPLEX_HALF = 0,
	PHY_DUPLEX_FULL = 1
} phy_duplex_t;

typedef enum {
	PHY_EEE_DISABLE = 0,
	PHY_EEE_ENABLE_AN,
	PHY_EEE_ENABLE_LPI,
} phy_eee_mode_t;

typedef struct {
	bool autoneg_en;
	phy_speed_t speed;
	phy_duplex_t duplex;
	bool advertise_100m;
	bool advertise_10m;
	bool advertise_pause;
	bool advertise_asym_pause;
} phy_link_config_t;

typedef struct {
	bool link_up;
	phy_speed_t speed;
	phy_duplex_t duplex;
	bool pause_tx;
	bool pause_rx;
} phy_link_state_t;

struct eth_phy_dev;

typedef void (*phy_link_cb_t)(struct eth_phy_dev *dev, phy_link_state_t state, void *user_data);

struct eth_mdio_ops {
	int (*mdio_read)(uint8_t phy_addr, uint8_t reg_addr, uint16_t *data);
	int (*mdio_write)(uint8_t phy_addr, uint8_t reg_addr, uint16_t data);
};

enum eth_refclk_dir {
	ETH_REFCLK_PHY2MAC = 0x00,
	ETH_REFCLK_MAC2PHY = 0x01,
};

struct eth_phy_ops {
	int (*init)(struct eth_phy_dev *dev);
	void (*hw_reset)(void);
	int (*reset)(struct eth_phy_dev *dev);
	int (*cfg_link)(struct eth_phy_dev *dev, const phy_link_config_t *cfg);
	int (*get_link)(struct eth_phy_dev *dev, phy_link_state_t *state);
	int (*autoneg_restart)(struct eth_phy_dev *dev);
	int (*cfg_refclock)(struct eth_phy_dev *dev, enum eth_refclk_dir dir);
	int (*cfg_eee)(struct eth_phy_dev *dev, phy_eee_mode_t mode);
	int (*set_loopback)(struct eth_phy_dev *dev, bool enable);
	int (*set_link_callback)(struct eth_phy_dev *dev, phy_link_cb_t cb, void *user_data);
};

typedef struct {
	uint8_t tx_setup_time;   /* enum eth_phy_tx_setup */
	uint8_t rx_setup_time;   /* enum eth_phy_rx_setup */
	uint8_t refclk_dir;      /* enum eth_refclk_dir */
	uint8_t eee_enable;
} phy_iface_config_t;

struct eth_phy_dev {
	const struct eth_mdio_ops *bus;
	const struct eth_phy_ops *ops;
	phy_link_config_t link_cfg;
	phy_iface_config_t iface_cfg;
	uint8_t addr;
	phy_link_cb_t link_cb;
	void *cb_user_data;
};

enum eth_phy_tx_setup {
	ETH_PHY_TX_SETUP_TIME_2NS = 0,
	ETH_PHY_TX_SETUP_TIME_4NS = 1,
	ETH_PHY_TX_SETUP_TIME_6NS = 2,
	ETH_PHY_TX_SETUP_TIME_8NS = 3,
};

enum eth_phy_rx_setup {
	ETH_PHY_RX_SETUP_TIME_2NS = 0,
	ETH_PHY_RX_SETUP_TIME_4NS = 1,
	ETH_PHY_RX_SETUP_TIME_6NS = 2,
	ETH_PHY_RX_SETUP_TIME_8NS = 3,
};

/* ========================================================================== */
/*                  Generic Base PHY Driver Functions                          */
/* ========================================================================== */

/* PHY struct defaults (base layer) — call before Ethernet_PHY_Init to customize */
void Ethernet_PHY_StructInit(void);

/* PHY hardware reset — weak, override for board-specific GPIO pin */
void Ethernet_PHY_HWReset(void);

/* PHY device init (base layer) — auto-calls Ethernet_PHY_StructInit if needed */
void Ethernet_PHY_Init(void);

/* Apply PHY interface config (refclock direction, EEE) — call after PHY_Init */
void Ethernet_PHY_Configure(struct eth_phy_dev *phy);

/* MDIO read/write through bus ops */
int Ethernet_PHY_Read(struct eth_phy_dev *phy, uint32_t reg, uint16_t *val);
int Ethernet_PHY_Write(struct eth_phy_dev *phy, uint32_t reg, uint16_t val);

/* Error-latching MDIO helpers */
uint16_t Ethernet_PHY_ReadSafe(struct eth_phy_dev *phy, uint32_t reg, int *err);
void Ethernet_PHY_WriteSafe(struct eth_phy_dev *phy, uint32_t reg, uint16_t val, int *err);

/* MMD access (Clause 45 over Clause 22) */
void Ethernet_PHY_MMDWriteSafe(struct eth_phy_dev *phy, uint8_t dev_addr,
							   uint16_t reg_addr, uint16_t data, int *err);
uint16_t Ethernet_PHY_MMDReadSafe(struct eth_phy_dev *phy, uint8_t dev_addr,
								  uint16_t reg_addr, int *err);

/* Standard IEEE 802.3 PHY operations */
int Ethernet_PHY_ReadID(struct eth_phy_dev *phy, uint32_t *phy_id);
int Ethernet_PHY_Reset(struct eth_phy_dev *phy);
int Ethernet_PHY_CfgLink(struct eth_phy_dev *phy, const phy_link_config_t *cfg);
int Ethernet_PHY_GetLink(struct eth_phy_dev *phy, phy_link_state_t *state);
int Ethernet_PHY_AutonegRestart(struct eth_phy_dev *phy);
int Ethernet_PHY_SetLoopback(struct eth_phy_dev *phy, bool enable);

#ifdef __cplusplus
}
#endif

#endif /* _AMEBA_ETH_PHY_H_ */
