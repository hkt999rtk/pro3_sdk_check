/**
  ******************************************************************************
  * @file    whc_ipc_dev_trx.c
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#define __WHC_IPC_DEV_TX_C__

/* -------------------------------- Includes -------------------------------- */
/* external head files */

/* internal head files */
#include "rtw_inic_common.h"
#include "whc_ipc_dev_trx.h"
#include "wifi_api.h"

#define CONFIG_ENABLE_CACHE
/* -------------------------------- Defines --------------------------------- */

/* -------------------------------- Macros ---------------------------------- */

/* ------------------------------- Data Types ------------------------------- */
/* ipc device tx structure */
struct ipc_dev_priv {
	u32 tx_bytes; /* xmit bytes */
	u32 tx_pkts; /* xmit number of packets */
};

/* -------------------------- Function declaration -------------------------- */

/* ---------------------------- Global Variables ---------------------------- */

/* --------------------------- Private Variables ---------------------------- */
static struct ipc_dev_priv g_ipc_dev_priv;

/* ---------------------------- Public Functions ---------------------------- */
/**
 * @brief  receiving function to send the received data to host.
 * @param  idx[in]: wlan interface index.
 * @return none.
 */
SRAM_WLAN_CRITICAL_CODE_SECTION
void whc_ipc_dev_recv(int idx)
{
	struct sk_buff *skb = NULL;

#if defined(CONFIG_WHC_DEV_TCPIP_KEEPALIVE)
	if (whc_dev_recv_pkt_process((u8 *)&idx, &skb) == 0) {
		return;
	}
#else
	skb = wifi_if_get_recv_skb(idx);
#endif
	if (!skb) {
		return;
	}

#ifdef CONFIG_ENABLE_CACHE
	DCache_CleanInvalidate((u32)skb->head, skbpriv.skb_buf_max_size);
	DCache_CleanInvalidate(((u32)skb), sizeof(struct sk_buff));
#endif /* CONFIG_ENABLE_CACHE */
	whc_ipc_send_msg(IPC_WIFI_EVT_RECV_PKTS, (u32)skb, 0, idx);

}

void whc_ipc_dev_tx_done(int idx)
{
	whc_ipc_send_msg(IPC_WIFI_EVT_TX_DONE, 0, 0, idx);
}

/* ---------------------------- Private Functions --------------------------- */
/**
 * @brief  to handle the message of IPC.
 * @param  event_num[in]: IPC event type.
 * @param  msg_addr[in]: address of the message.
 * @param  wlan_idx[in]: wlan interface index.
 * @return none.
 */
SRAM_WLAN_CRITICAL_CODE_SECTION
void whc_ipc_dev_trx_event_hdl(u8 event_num, u32 msg_addr, u8 wlan_idx)
{
	struct sk_buff *skb = NULL;

	switch (event_num) {
	case IPC_WIFI_CMD_XIMT_PKTS:
		skb = (struct sk_buff *)msg_addr;
#ifdef CONFIG_ENABLE_CACHE
		DCache_Invalidate(((u32)skb), sizeof(struct sk_buff));
		DCache_Invalidate((u32)skb->head, skbpriv.skb_buf_max_size);
#endif /* CONFIG_ENABLE_CACHE */

		if (!wifi_is_running(wlan_idx)) {
			/*free skb and return*/
			dev_kfree_skb_any(skb);
			return;
		}

		g_ipc_dev_priv.tx_bytes += skb->len;
		g_ipc_dev_priv.tx_pkts++;
		wifi_if_send_skb(wlan_idx, skb);
		break;
	case IPC_WIFI_MSG_RECV_DONE:
		skb = (struct sk_buff *)msg_addr;
		kfree_skb(skb);
		break;

	default:
		RTK_LOGE(TAG_WLAN_INIC, "Device Unknown Event(%lx)!\n", event_num);
		break;
	}
}

/**
 * @brief  to handle the ipc message interrupt. If the message queue is
 *     initialized, it will enqueue the ipc message and wake up the message
 *     task to handle the message.
 * @param  Data[in]: IPC data (unused).
 * @param  IrqStatus[in]: interrupt status (unused).
 * @param  ChanNum[in]: IPC channel number (unused).
 * @return none.
 */
SRAM_WLAN_CRITICAL_CODE_SECTION
void whc_ipc_dev_trx_int_hdl(void *Data, u32 IrqStatus, u32 ChanNum)
{
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);

	struct whc_ipc_ex_msg *p_ipc_msg = NULL;
	sint ret = RTK_FAIL;

#ifdef IPC_DIR_MSG_RX
	PIPC_MSG_STRUCT p_ipc_recv_msg = ipc_get_message(IPC_DIR_MSG_RX, \
									 IPC_H2D_WIFI_TRX_TRAN);
	p_ipc_msg = (struct whc_ipc_ex_msg *)p_ipc_recv_msg->msg;
#else
	p_ipc_msg = (struct whc_ipc_ex_msg *)ipc_get_message(IPC_INT_CHAN_WIFI_TRX_TRAN);
#endif /* IPC_DIR_MSG_RX */

#ifdef CONFIG_ENABLE_CACHE
	DCache_Invalidate((u32)p_ipc_msg, sizeof(struct whc_ipc_ex_msg));
#endif /* CONFIG_ENABLE_CACHE */

	if (whc_msg_get_queue_status()) {
		/* put the ipc message to the queue */
		ret = whc_msg_enqueue(p_ipc_msg);
	} else {
		RTK_LOGE(TAG_WLAN_INIC, "inic dev msg queue not working!\n");
		ret = RTK_FAIL;
	}

	if (ret == RTK_SUCCESS) {
		p_ipc_msg->msg_queue_status = 0;
	} else {
		p_ipc_msg->msg_queue_status = IPC_WIFI_MSG_MEMORY_NOT_ENOUGH;
	}
	/* enqueuing message is seccussful, send acknowledgement to another port*/
	p_ipc_msg->event_num = IPC_WIFI_MSG_READ_DONE;
#ifdef CONFIG_ENABLE_CACHE
	DCache_Clean((u32)p_ipc_msg, sizeof(struct whc_ipc_ex_msg));
#endif /* CONFIG_ENABLE_CACHE */
}
