/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_ipc_api.h"

static const char *TAG = "PMC";
IPC_MSG_STRUCT sleep_msg;
u32 vp_sleep_suspend = 0;

/**
  * @brief  ap request to enter sleep.
  * @param  none
  * @retval none
  */
void SOCPS_Sleep(void)
{
	RTK_LOGI(TAG, "VP sleep\n");
	//todo: set sleep event and VOE stop
	vp_sleep_suspend = 1;
}

void SOCPS_NPWVP_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);

	ipc_get_message(IPC_NP_TO_VP, IPC_N2V_TICKLESS_INDICATION);

}

void SOCPS_APWVP_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);
	PIPC_MSG_STRUCT psleep_msg;

	psleep_msg = ipc_get_message(IPC_AP_TO_VP, IPC_A2V_TICKLESS_INDICATION);
	if (psleep_msg->msg_type == SLEEP_PG || psleep_msg->msg_type == SLEEP_CG) {
		DiagPrintf("VP SOCPS_Sleep\n\r");
		SOCPS_Sleep();
	} else if (psleep_msg->msg_type == SLEEP_DEEP_SLEEP) {
		DiagPrintf("VP SOCPS_Sleep\n\r");
		SOCPS_Sleep();
	} else if (psleep_msg->msg_type == SLEEP_ACTIVE) {
		RTK_LOGI(TAG, "VP wakeup from AP\n");
		//todo: VP wake
	}
}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_n2vtickless_table[] = {
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = SOCPS_NPWVP_ipc_int,
		.RxIrqData = (void *) NULL,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_NP_TO_VP,
		.IPC_Channel = IPC_N2V_TICKLESS_INDICATION
	},

};

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_a2vtickless_table[] = {
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = SOCPS_APWVP_ipc_int,
		.RxIrqData = (void *) NULL,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_AP_TO_VP,
		.IPC_Channel = IPC_A2V_TICKLESS_INDICATION
	},

};
