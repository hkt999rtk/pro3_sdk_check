/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_ipc_api.h"
#include "ameba_ipccfg.h"
#include "FreeRTOS.h"
#include "task.h"
#include "wowlan_ipccfg.h"

static IPC_MSG_STRUCT reason;
static WOWLANCFG setting;
TaskHandle_t WowlanIpcTaskHandle;

void wowlan_report_wakeup_reason(WOWLANCFG *param)
{
	memset(&reason, 0, sizeof(IPC_MSG_STRUCT));
	memcpy(&setting, param, sizeof(WOWLANCFG));

	DCache_Clean(&setting, sizeof(WOWLANCFG));
	reason.msg = &setting;
	reason.msg_len = sizeof(WOWLANCFG);
	reason.msg_type = N2A_RECV_WAKEUP_REASON;
	ipc_send_message(IPC_NP_TO_AP, IPC_N2A_WOWLAN, (PIPC_MSG_STRUCT)&reason);
}

void wowlan_a2n_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	xTaskNotifyFromISR(WowlanIpcTaskHandle, IPC_AP_NOTIFY_BIT, eSetBits, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void wowlan_f2n_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	xTaskNotifyFromISR(WowlanIpcTaskHandle, IPC_FP_NOTIFY_BIT, eSetBits, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_a2nwowlan_table[] = {
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = wowlan_a2n_ipc_int,
		.RxIrqData = (void *) NULL,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_AP_TO_NP,
		.IPC_Channel = IPC_A2N_WOWLAN
	},

};

#ifdef CONFIG_STANDARD_TICKLESS
IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_f2nwowlan_table[] = {
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = wowlan_f2n_ipc_int,
		.RxIrqData = (void *) NULL,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_FP_TO_NP,
		.IPC_Channel = IPC_F2N_TICKLESS_INDICATION
	},

};
#endif
