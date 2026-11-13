/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 *
 * Top-level Ethernet driver — owns central state and combines:
 *   ameba_eth_gmac.c           — GMAC hardware (register-level)
 *   ameba_eth_phy.c            — PHY framework
 *   ameba_eth_phy_rtl8201f.c   — RTL8201F PHY driver
 *
 * @file ameba_eth.c
 * @brief AmebaPro3 Ethernet HAL Driver
 * @author wongyiekheng@realtek-sg.com
 */

#include "ameba_soc.h"
#include <string.h>

static const char *const TAG = "ETH";

ETH_InitTypeDef eth_dev;

/* ========================================================================== */
/*                     DMA Buffer Allocation                                  */
/* ========================================================================== */

/** @brief Allocate a DMA-aligned buffer and return both the aligned and raw pointers. */
static u8 *Ethernet_AllocAligned(u32 size, u8 **raw_out)
{
	u8 *raw = (u8 *)rtos_mem_malloc(size + ETH_DMA_ALIGN - 1);
	if (!raw) {
		return NULL;
	}
	*raw_out = raw;
	return (u8 *)(((u32)raw + ETH_DMA_ALIGN - 1) & ~(ETH_DMA_ALIGN - 1));
}

/** @brief Free a single buffer node and its associated raw buffer. */
static void Ethernet_FreeBufNode(eth_buf_node_t *node)
{
	if (node->buf_raw) {
		rtos_mem_free(node->buf_raw);
	}
	rtos_mem_free(node);
}

/** @brief Free all buffer nodes in a descriptor ring's linked list. */
static void Ethernet_FreeNodeList(eth_desc_ring_t *ring)
{
	eth_buf_node_t *node = ring->head;
	for (u16 i = 0; i < ring->count; i++) {
		eth_buf_node_t *next = node->next;
		Ethernet_FreeBufNode(node);
		node = next;
	}
	ring->head = NULL;
	ring->cur = NULL;
}

/** @brief Free the hardware descriptor ring memory. */
static void Ethernet_FreeHwRing(eth_desc_ring_t *ring)
{
	if (ring->hw_ring_raw) {
		rtos_mem_free(ring->hw_ring_raw);
		ring->hw_ring = NULL;
		ring->hw_ring_raw = NULL;
	}
}

/** @brief Free all resources (buffer nodes and HW ring) of a descriptor ring. */
static void Ethernet_FreeDescRing(eth_desc_ring_t *ring)
{
	if (ring->head) {
		Ethernet_FreeNodeList(ring);
	}
	Ethernet_FreeHwRing(ring);
	ring->count = 0;
}

/** @brief Allocate and zero-initialize a DMA-aligned hardware descriptor ring. */
static int Ethernet_AllocHwRing(eth_desc_ring_t *ring, u16 count, u32 desc_size)
{
	u8 *raw = NULL;
	ring->hw_ring = Ethernet_AllocAligned(count * desc_size, &raw);
	if (!ring->hw_ring) {
		return -RTK_ERR_NOMEM;
	}
	ring->hw_ring_raw = raw;
	memset(ring->hw_ring, 0, count * desc_size);
	return RTK_SUCCESS;
}

/** @brief Allocate a buffer node with a DMA-aligned packet buffer. */
static eth_buf_node_t *Ethernet_CreateBufNode(u16 buf_size, u16 desc_idx)
{
	u8 *raw = NULL;
	eth_buf_node_t *node = (eth_buf_node_t *)rtos_mem_malloc(sizeof(eth_buf_node_t));
	if (!node) {
		return NULL;
	}
	memset(node, 0, sizeof(*node));

	node->buf = Ethernet_AllocAligned(buf_size, &raw);
	if (!node->buf) {
		rtos_mem_free(node);
		return NULL;
	}
	node->buf_raw = raw;
	node->buf_size = buf_size;
	node->desc_idx = desc_idx;
	return node;
}

/** @brief Append a buffer node to the end of the descriptor ring's linked list. */
static void Ethernet_AppendNode(eth_desc_ring_t *ring, eth_buf_node_t *node, eth_buf_node_t **tail)
{
	if (*tail) {
		(*tail)->next = node;
	} else {
		ring->head = node;
	}
	*tail = node;
}

/** @brief Allocate a circular linked list of buffer nodes for a descriptor ring. */
static int Ethernet_AllocBufNodes(eth_desc_ring_t *ring, u16 count, u16 buf_size)
{
	eth_buf_node_t *tail = NULL;

	for (u16 i = 0; i < count; i++) {
		eth_buf_node_t *node = Ethernet_CreateBufNode(buf_size, i);
		if (!node) {
			return -RTK_ERR_NOMEM;
		}
		Ethernet_AppendNode(ring, node, &tail);
	}

	/* Circular: last node links back to head */
	if (tail) {
		tail->next = ring->head;
	}
	ring->cur = ring->head;
	ring->count = count;
	ring->buf_size = buf_size;
	return RTK_SUCCESS;
}

/** @brief Allocate and initialize a complete descriptor ring (HW ring + buffer nodes). */
static int Ethernet_InitDescRing(eth_desc_ring_t *ring, u16 count, u16 buf_size, u32 desc_size)
{
	if (ring->head) {
		return RTK_SUCCESS;
	}

	int ret = Ethernet_AllocHwRing(ring, count, desc_size);
	if (ret) {
		return ret;
	}

	ret = Ethernet_AllocBufNodes(ring, count, buf_size);
	if (ret) {
		Ethernet_FreeDescRing(ring);
		return ret;
	}

	return RTK_SUCCESS;
}

/**
 * @brief  Allocate and initialize TX/RX DMA descriptor rings and packet buffers.
 * @param  ETH_InitStruct  Pointer to the top-level Ethernet configuration structure.
 * @retval RTK_SUCCESS on success, negative error code on allocation failure.
 */
int Ethernet_InitDmaBuffers(ETH_InitTypeDef *ETH_InitStruct)
{
	int ret;

	if (ETH_InitStruct->gmac.ETH_TxDesc) {
		return RTK_SUCCESS;
	}

	ret = Ethernet_InitDescRing(&ETH_InitStruct->ETH_TxRing,
								ETH_InitStruct->gmac.ETH_TxDescNum,
								ETH_InitStruct->gmac.ETH_TxBufSize,
								sizeof(eth_tx_desc_t));
	if (ret) {
		RTK_LOGE(TAG, "TX ring alloc failed\n");
		return ret;
	}

	ret = Ethernet_InitDescRing(&ETH_InitStruct->ETH_RxRing,
								ETH_InitStruct->gmac.ETH_RxDescNum,
								ETH_InitStruct->gmac.ETH_RxBufSize,
								sizeof(eth_rx_desc_t));
	if (ret) {
		RTK_LOGE(TAG, "RX ring alloc failed\n");
		return ret;
	}

	ETH_InitStruct->gmac.ETH_TxDesc = (volatile eth_tx_desc_t *)ETH_InitStruct->ETH_TxRing.hw_ring;
	ETH_InitStruct->gmac.ETH_RxDesc = (volatile eth_rx_desc_t *)ETH_InitStruct->ETH_RxRing.hw_ring;

	return RTK_SUCCESS;
}


/* ========================================================================== */
/*                     Full HW Init Sequence                                  */
/* ========================================================================== */

/**
 * @brief  Initialize the global eth_dev structure with default GMAC and PHY settings.
 */
void Ethernet_StructInit(void)
{
	memset(&eth_dev, 0, sizeof(ETH_InitTypeDef));

	Ethernet_GMAC_StructInit(&eth_dev.gmac);
	Ethernet_PHY_StructInit();

	eth_dev.phy_dev = &rtl8201f_phy_dev;
}

/**
 * @brief  Perform full Ethernet initialization: struct init, clocks, PHY reset, DMA buffers, and GMAC init.
 * @retval RTK_SUCCESS on success, negative error code on failure.
 */
int Ethernet_Init(void)
{
	Ethernet_StructInit();

	/* RCC reset: stop GMAC DMA before re-init */
	RCC_PeriphClockCmd(APBPeriph_GMAC, APBPeriph_GMAC_CLOCK, DISABLE);
	RCC_PeriphClockCmd(APBPeriph_LX_BUS, APBPeriph_LXBUS_CLOCK, DISABLE);
	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_LX_CLOCK, DISABLE);
	DelayMs(10);

	Ethernet_GMAC_ClkSet();
	Ethernet_PHY_Init();

	int ret = Ethernet_InitDmaBuffers(&eth_dev);
	if (ret) {
		return ret;
	}

	if (Ethernet_GMAC_Init(&eth_dev.gmac, eth_dev.phy_dev,
						   eth_dev.ETH_TxRing.head, eth_dev.ETH_RxRing.head) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Ethernet_GMAC_Init failed\n");
		return RTK_FAIL;
	}

	return RTK_SUCCESS;
}

/* ========================================================================== */
/*                     Blocking Send / Receive                                */
/* ========================================================================== */

/**
 * @brief  Send a raw packet with blocking wait for TX descriptor availability.
 * @param  data        Pointer to packet data.
 * @param  len         Packet length in bytes.
 * @param  timeout_ms  Maximum wait time in milliseconds.
 * @retval RTK_SUCCESS on success, RTK_FAIL on timeout.
 */
int Ethernet_Send(u8 *data, u32 len, u32 timeout_ms)
{
	u8 *tx_buf;

	for (u32 ms = 0; ms < timeout_ms; ms++) {
		tx_buf = Ethernet_GMAC_GetTXPktInfo(&eth_dev.gmac);
		if (tx_buf) {
			memcpy(tx_buf, data, len);
			DCache_Clean((u32)tx_buf, len);
			Ethernet_GMAC_UpdateTXDESCAndSend(&eth_dev.gmac, len);
			return RTK_SUCCESS;
		}
		DelayMs(1);
	}

	return RTK_FAIL;
}

/**
 * @brief  Receive a raw packet with blocking wait.
 * @param  rx_data     Output pointer to received packet buffer.
 * @param  rx_len      Output packet length.
 * @param  timeout_ms  Maximum wait time in milliseconds.
 * @retval RTK_SUCCESS on success, -RTK_ERR_TIMEOUT on timeout.
 */
int Ethernet_Recv(u8 **rx_data, u32 *rx_len, u32 timeout_ms)
{
	u8 *buf;
	u32 pkt_len = 0;

	for (u32 ms = 0; ms < timeout_ms; ms++) {
		buf = Ethernet_GMAC_GetRXPktInfo(&eth_dev.gmac, &pkt_len);
		if (buf && pkt_len > 0) {
			*rx_data = buf;
			*rx_len = pkt_len;
			return RTK_SUCCESS;
		}
		DelayMs(1);
	}

	return -RTK_ERR_TIMEOUT;
}
