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

static IPC_MSG_STRUCT kamsg;
static WOWLANCFG setting;
TaskHandle_t APTaskHandle;

void wowlan_get_wakeup_reason(WOWLANCFG *param)
{
	PIPC_MSG_STRUCT kacfg = NULL;

	kacfg = ipc_get_message(IPC_NP_TO_AP, IPC_N2A_WOWLAN);
	if (kacfg == NULL) {
		return;
	}
	DCache_Invalidate(kacfg->msg, kacfg->msg_len);
	memcpy(param, (uint8_t *)kacfg->msg, kacfg->msg_len);
}

void wowlan_enable(WOWLANCFG *param)
{
	memset(&kamsg, 0, sizeof(IPC_MSG_STRUCT));
	memcpy(&setting, param, sizeof(WOWLANCFG));

	DCache_Clean(&setting, sizeof(WOWLANCFG));
	kamsg.msg = &setting;
	kamsg.msg_len = sizeof(WOWLANCFG);
	kamsg.msg_type = A2N_WOWLAN_CFG;
	ipc_send_message(IPC_AP_TO_NP, IPC_A2N_WOWLAN, (PIPC_MSG_STRUCT)&kamsg);
}

void wowlan_ap_wakeup_notification(void)
{
	memset(&kamsg, 0, sizeof(IPC_MSG_STRUCT));

	kamsg.msg_type = A2N_AP_WAKEUP;
	ipc_send_message(IPC_AP_TO_NP, IPC_A2N_WOWLAN, (PIPC_MSG_STRUCT)&kamsg);
}

void wowlan_n2a_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	vTaskNotifyGiveFromISR(APTaskHandle, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_n2awowlan_table[] = {
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = wowlan_n2a_ipc_int,
		.RxIrqData = (void *) NULL,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_NP_TO_AP,
		.IPC_Channel = IPC_N2A_WOWLAN
	},

};
