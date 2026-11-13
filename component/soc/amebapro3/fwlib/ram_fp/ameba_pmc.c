/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "ameba_soc.h"
#include "ameba_ipc_api.h"
#include "ameba_pmc.h"
#include "FreeRTOS.h"
#include "task.h"

static const char *TAG = "FP_PMC";
IPC_MSG_STRUCT sleep_msg;
extern u32 SYSTIMER_TickGet(void);
extern u32 SYSTIMER_GetPassTime(u32 start);

#define PMC_SYSTICK_CTRL_REG             ( *( ( volatile uint32_t * ) 0xe000e010 ) )
#define PMC_SYSTICK_ENABLE_BIT           ( 1UL << 0UL )
#define PMC_SYSTICK_INT_BIT              ( 1UL << 1UL )
#define PMC_SYSTICK_CLK_BIT              ( 1UL << 2UL )

void prvStopTickInterruptTimer(void)
{
	PMC_SYSTICK_CTRL_REG &= ~ PMC_SYSTICK_ENABLE_BIT;
}

void prvStartTickInterruptTimer(void)
{
	PMC_SYSTICK_CTRL_REG |= PMC_SYSTICK_ENABLE_BIT;
}

/* ************test code************************************** */
u32 fp_sleep = 0;

int SOCPS_Get_Sleep_mode(void)
{
	return fp_sleep;
}

void SOCPS_Set_Sleep_mode(int sleep_value)
{
	fp_sleep = sleep_value;
}

/* ****************test code end******************************** */


void SOCPS_SleepCG(void)
{
	u32 nDeviceIdOffset = PMU_MAX;
	uint32_t tick_before_sleep;
	volatile uint32_t ms_passed = 0;

	prvStopTickInterruptTimer();

#if defined(configUSE_TICKLESS_IDLE) && (configUSE_TICKLESS_IDLE == 1)
	/*  Store gtimer timestamp before sleep */
	tick_before_sleep = SYSTIMER_TickGet();
#endif

	/* exec sleep hook functions */
	nDeviceIdOffset = pmu_exec_sleep_hook_funs();
	if (nDeviceIdOffset != PMU_MAX) {
#if defined(configUSE_TICKLESS_IDLE) && (configUSE_TICKLESS_IDLE == 1)
		/*  update kernel tick by calculating passed tick from gtimer */
		/*  get current gtimer timestamp */
		ms_passed = SYSTIMER_GetPassTime(tick_before_sleep);
		/* update xTickCount and mark to trigger task list update in xTaskResumeAll */
		vTaskCompTick(ms_passed);
#else
		(void)tick_before_sleep;
		(void)ms_passed;
#endif
		pmu_exec_wakeup_hook_funs(nDeviceIdOffset, ms_passed);
		RTK_LOGD(TAG, "DBG: KM0 Sleep CG blocked because Dev %lx busy\n", nDeviceIdOffset);

		prvStartTickInterruptTimer();
		return;
	}

	SOCPS_Set_Sleep_mode(2);

	SOCPS_SleepCG_RAM();

#if defined(configUSE_TICKLESS_IDLE) && (configUSE_TICKLESS_IDLE == 1)
	/*  update kernel tick by calculating passed tick from gtimer */
	/*  get current gtimer timestamp */
	ms_passed = SYSTIMER_GetPassTime(tick_before_sleep);
	/* update xTickCount and mark to trigger task list update in xTaskResumeAll */
	vTaskCompTick(ms_passed);
#else
	(void)tick_before_sleep;
	(void)ms_passed;
#endif

	/* exec sleep hook functions */
	pmu_exec_wakeup_hook_funs(PMU_MAX, ms_passed);

	prvStartTickInterruptTimer();

	//check wake up reason
}

void np_clock_on(void)
{
	RTK_LOGI(TAG, "FP wakeup np\n");
	ipc_send_message(IPC_FP_TO_NP, IPC_F2N_TICKLESS_INDICATION, (PIPC_MSG_STRUCT)&sleep_msg);
	SOCPS_Set_Sleep_mode(0);
}

void SOCPS_SleepPG(void)
{
	u32 nDeviceIdOffset = PMU_MAX;
	uint32_t tick_before_sleep;
	volatile uint32_t ms_passed = 0;
	RTK_LOGI(TAG, "FPPG\n");

	prvStopTickInterruptTimer();

#if defined(configUSE_TICKLESS_IDLE) && (configUSE_TICKLESS_IDLE == 1)
	/*  Store gtimer timestamp before sleep */
	tick_before_sleep = SYSTIMER_TickGet();
#endif

	DCache_Disable();
	ICache_Disable();

	/* exec sleep hook functions */
	nDeviceIdOffset = pmu_exec_sleep_hook_funs();
	if (nDeviceIdOffset != PMU_MAX) {
#if defined(configUSE_TICKLESS_IDLE) && (configUSE_TICKLESS_IDLE == 1)
		/*  update kernel tick by calculating passed tick from gtimer */
		/*  get current gtimer timestamp */
		ms_passed = SYSTIMER_GetPassTime(tick_before_sleep);
		/* update xTickCount and mark to trigger task list update in xTaskResumeAll */
		vTaskCompTick(ms_passed);
#else
		(void)tick_before_sleep;
		(void)ms_passed;
#endif

		pmu_exec_wakeup_hook_funs(nDeviceIdOffset, ms_passed);
		RTK_LOGD(TAG, "DBG: KM0 Sleep PG blocked because Dev %lx busy\n", nDeviceIdOffset);
		__asm volatile("cpsie i" ::: "memory");
		ICache_Enable();
		DCache_Enable();

		prvStartTickInterruptTimer();
		return;
	}

	SOCPS_Set_Sleep_mode(2);

	SOCPS_SleepPG_RAM();

#if defined(configUSE_TICKLESS_IDLE) && (configUSE_TICKLESS_IDLE == 1)
	/*  update kernel tick by calculating passed tick from gtimer */
	/*  get current gtimer timestamp */
	ms_passed = SYSTIMER_GetPassTime(tick_before_sleep);
	/* update xTickCount and mark to trigger task list update in xTaskResumeAll */
	vTaskCompTick(ms_passed);
#else
	(void)tick_before_sleep;
	(void)ms_passed;
#endif

	/* exec sleep hook functions */
	pmu_exec_wakeup_hook_funs(PMU_MAX, ms_passed);

	prvStartTickInterruptTimer();

	SOCPS_Set_Sleep_mode(0);

	pmu_set_power_event(EVENT_WAKEUP);
	RTK_LOGI(TAG, "FPPW\r\n");
}

void SOCPS_NPWFP_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);
	PIPC_MSG_STRUCT psleep_msg;

	psleep_msg = ipc_get_message(IPC_NP_TO_FP, IPC_N2F_TICKLESS_INDICATION);

	if (psleep_msg->msg_type == SLEEP_PG || psleep_msg->msg_type == SLEEP_CG) {
		RTK_LOGI(TAG, "FP sleep from np\n");
		pmu_set_sleep_type(psleep_msg->msg_type);
		SOCPS_Set_Sleep_mode(1);
		pmu_release_wakelock(PMU_OS);
	} else {
		//NP ask FP to wakeup
		RTK_LOGI(TAG, "FP wakeup from np\n");
	}
}

void SOCPS_APWFP_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);
	PIPC_MSG_STRUCT psleep_msg;

	psleep_msg = ipc_get_message(IPC_AP_TO_FP, IPC_A2F_TICKLESS_INDICATION);

	if (psleep_msg->msg_type == SLEEP_PG) {
		RTK_LOGI(TAG, "FP sleep from ap\n");
		SOCPS_Set_Sleep_mode(1);
		pmu_release_wakelock(PMU_OS);
	} else {
		//NP ask FP to wakeup
		RTK_LOGI(TAG, "FP wakeup from ap\n");
	}
}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_n2ftickless_table[] = {
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = SOCPS_NPWFP_ipc_int,
		.RxIrqData = (void *) NULL,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_NP_TO_FP,
		.IPC_Channel = IPC_N2F_TICKLESS_INDICATION
	},
};

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_a2ftickless_table[] = {
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = SOCPS_APWFP_ipc_int,
		.RxIrqData = (void *) NULL,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_AP_TO_FP,
		.IPC_Channel = IPC_A2F_TICKLESS_INDICATION
	},
};
