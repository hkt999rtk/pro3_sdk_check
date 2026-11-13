/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_ipc_api.h"
#include "FreeRTOS.h"
#include "task.h"

static const char *TAG = "PMC";
IPC_MSG_STRUCT sleep_msg;

/**
  * @brief  get aon wake reason.
  * @param  None
  * @retval aon wake up reason
  *		This parameter can be one or combination of the following values:
  *		 @arg AON_BIT_RTC_ISR_EVT
  *		 @arg AON_BIT_GPIO_PIN3_WAKDET_EVT
  *		 @arg AON_BIT_GPIO_PIN2_WAKDET_EVT
  *		 @arg AON_BIT_GPIO_PIN1_WAKDET_EVT
  *		 @arg AON_BIT_GPIO_PIN0_WAKDET_EVT
  *		 @arg AON_BIT_TIM_ISR_EVT
  *		 @arg AON_BIT_BOR_ISR_EVT
  *		 @arg AON_BIT_CHIPEN_LP_ISR_EVT
  *		 @arg AON_BIT_CHIPEN_SP_ISR_EVT
  */
int SOCPS_AONWakeReason(void)
{
	int reason = 0;
#if 0
	reason = HAL_READ32(SYSTEM_CTRL_BASE_NP, REG_AON_AON_WAK_EVT);
#endif
	return reason;
}

/**
  * @brief  ap configure wakeup source before sleep.
  * @param  wake_src_mask: OR of WAKE_SRC_xxx bits defined in ameba_pmc.h
  * @param  wake_src_pol:  AON GPIO polarity bitmask; bit N=1 means GPIOAN is high-active
  * @param  timer_ms:      wakeup timer duration in ms; PMU_SLEEP_FOREVER = no timer
  * @retval none
  */
void SOCPS_AP_SetWakeSrc(u32 wake_src_mask, u32 wake_src_pol, u32 timer_ms)
{
	sleep_msg.msg     = wake_src_mask;
	sleep_msg.msg_len = timer_ms;
	sleep_msg.rsvd    = wake_src_pol;
}

/**
  * @brief  ap request to enter sleep.
  * @param  none
  * @retval none
  * @note   call SOCPS_AP_SetWakeSrc() before this function to configure wakeup sources.
  */
void SOCPS_Sleep(void)
{
	RTK_LOGI(TAG, "CA32 sleep\n");
	sleep_msg.msg_type = pmu_get_sleep_type();

	ipc_send_message(IPC_AP_TO_VP, IPC_A2V_TICKLESS_INDICATION, (PIPC_MSG_STRUCT)&sleep_msg);
	vTaskDelay(100);

	ipc_send_message(IPC_AP_TO_NP, IPC_A2N_TICKLESS_INDICATION, (PIPC_MSG_STRUCT)&sleep_msg);

	vTaskSuspendAll();
	while (1);
}

void SOCPS_DeepSleep(void)
{
	RTK_LOGI(TAG, "CA32 deepsleep\n");
	sleep_msg.msg_type = SLEEP_DEEP_SLEEP;

	ipc_send_message(IPC_AP_TO_VP, IPC_A2V_TICKLESS_INDICATION, (PIPC_MSG_STRUCT)&sleep_msg);
	vTaskDelay(100);

	ipc_send_message(IPC_AP_TO_NP, IPC_A2N_TICKLESS_INDICATION, (PIPC_MSG_STRUCT)&sleep_msg);

	vTaskSuspendAll();
	while (1);
}

void SOCPS_NPWAP_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);

	ipc_get_message(IPC_NP_TO_AP, IPC_N2A_TICKLESS_INDICATION);

}

void SOCPS_VPWAP_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);

	ipc_get_message(IPC_VP_TO_AP, IPC_V2A_TICKLESS_INDICATION);

}

void SOCPS_FPWAP_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);

	ipc_get_message(IPC_FP_TO_AP, IPC_F2A_TICKLESS_INDICATION);

}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_n2atickless_table[] = {
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = SOCPS_NPWAP_ipc_int,
		.RxIrqData = (void *) NULL,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_NP_TO_AP,
		.IPC_Channel = IPC_N2A_TICKLESS_INDICATION
	},

};

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_v2atickless_table[] = {
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = SOCPS_VPWAP_ipc_int,
		.RxIrqData = (void *) NULL,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_VP_TO_AP,
		.IPC_Channel = IPC_V2A_TICKLESS_INDICATION
	},

};

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_f2atickless_table[] = {
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = SOCPS_FPWAP_ipc_int,
		.RxIrqData = (void *) NULL,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_FP_TO_AP,
		.IPC_Channel = IPC_F2A_TICKLESS_INDICATION
	},

};
