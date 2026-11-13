/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Unified Ethernet driver header — includes both the GMAC controller
 * and PHY headers.
 *
 *   ameba_eth_gmac.h         — GMAC hardware registers and API
 *   ameba_eth_phy.h          — PHY framework and drivers
 */

#ifndef _AMEBA_ETH_H_
#define _AMEBA_ETH_H_

#include "ameba_eth_gmac.h"
#include "ameba_eth_phy.h"

#define ETH_DMA_ALIGN 32

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                       Ethernet Adapter                                     */
/* ========================================================================== */

typedef struct {
	GMAC_InitTypeDef    gmac;       /* GMAC HW config + descriptor state */
	struct eth_phy_dev *phy_dev;    /* PHY device */
	eth_desc_ring_t     ETH_TxRing; /* DMA TX ring (managed by ameba_eth.c) */
	eth_desc_ring_t     ETH_RxRing; /* DMA RX ring (managed by ameba_eth.c) */
} ETH_InitTypeDef;

extern ETH_InitTypeDef eth_dev;

/* ========================================================================== */
/*                       Top-level API (ameba_eth.c)                          */
/* ========================================================================== */

void Ethernet_StructInit(void);
int Ethernet_Init(void);
int Ethernet_InitDmaBuffers(ETH_InitTypeDef *eth);
int Ethernet_Send(u8 *data, u32 len, u32 timeout_ms);
int Ethernet_Recv(u8 **rx_data, u32 *rx_len, u32 timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* _AMEBA_ETH_H_ */
