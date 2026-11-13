/**
  ******************************************************************************
  * @file    whc_ipc_device.c
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

/* -------------------------------- Includes -------------------------------- */
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "rtw_inic_common.h"
#include "rtw_skbuff.h"
#include "whc_ipc_dev_trx.h"
#define CONFIG_ENABLE_CACHE

/* ---------------------------- Public Functions ---------------------------- */
/**
 * @brief  to initialize the ipc device for the inic.
 * @param  none.
 * @return none.
 */
void whc_ipc_dev_init(void)
{
	/* Move message queue init after user config set*/
	whc_dev_api_init();

#ifdef CONFIG_WHC_DEV_TCPIP_KEEPALIVE
	whc_dev_pktfilter_init();
#endif

	/* set AON_BIT_WIFI_INIC_NP_READY=1 to indicate inic_ipc_device is ready */
	u32 value = HAL_READ32(REG_AON_WIFI_IPC, 0);
	HAL_WRITE32(REG_AON_WIFI_IPC, 0, value | AON_BIT_WIFI_INIC_NP_READY);

}

/* ---------------------------- Global Variables ---------------------------- */
IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_dev_event_table = {
	.USER_MSG_TYPE = IPC_USER_POINT,
	.Rxfunc = whc_ipc_dev_trx_int_hdl,
	.RxIrqData = (void *) NULL,
	.Txfunc = IPC_TXHandler,
	.TxIrqData = (void *) NULL,
	.IPC_Direction = IPC_DIR_MSG_RX,
	.IPC_Channel = IPC_H2D_WIFI_TRX_TRAN
};

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_api_dev_table = {
	.USER_MSG_TYPE = IPC_USER_POINT,
	.Rxfunc = whc_ipc_dev_api_int_hdl,
	.RxIrqData = (void *) NULL,
	.Txfunc = IPC_TXHandler,
	.TxIrqData = (void *) NULL,
	.IPC_Direction = IPC_DIR_MSG_RX,
	.IPC_Channel = IPC_H2D_WIFI_API_TRAN
};
