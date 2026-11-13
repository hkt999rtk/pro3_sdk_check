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
#include "ameba_arch.h"
#include "FreeRTOS.h"
#include "task.h"

extern u32 SYSTIMER_TickGet(void);
extern u32 SYSTIMER_GetPassTime(u32 start);
extern u8 PERIPH_POWER_PIN;

static const char *TAG = "NP_PMC";

IPC_MSG_STRUCT sleep_msg;

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

/* test code */
int sleep_mode = 0;

int SOCPS_Get_Sleep_mode(void)
{
	return sleep_mode;
}

void SOCPS_Set_Sleep_mode(int sleep_value)
{
	sleep_mode = sleep_value;
}

/* test code end */
#define PMC_WAKE_STATUS_REG_NUM 1
uint32_t SOCPS_GetWakeReason(uint8_t index)
{
	if (index >= PMC_WAKE_STATUS_REG_NUM) {
		return 0xFF;
	}

	return AON_BASE->AON_NP_BOOT_REASON;
}

void SOCPS_Gpio_Clkctr(void)
{
	//todo: check gpio wake source
	int i = 0;
	int aon_gpio_used = 0;
	int pon_gpio_used = 0;
	for (i = 0;;) {
		/*  Check if search to end */
		if (sleep_wevent_config[i].Module == 0xFFFFFFFF) {
			break;
		}

		if (sleep_wevent_config[i].Wakeup == WAKEUP_NP) {
			if (sleep_wevent_config[i].Module == WAKE_SRC_GPIOA3 || sleep_wevent_config[i].Module == WAKE_SRC_GPIOA2 ||
				sleep_wevent_config[i].Module == WAKE_SRC_GPIOA1 || sleep_wevent_config[i].Module == WAKE_SRC_GPIOA0) {
				aon_gpio_used = 1;
			}
		}

		if (sleep_wevent_config[i].Wakeup != WAKEUP_NULL) {
			if (sleep_wevent_config[i].Module == WAKE_SRC_PON_GPIO) {
				pon_gpio_used = 1;
			}
		}

		i++;
	}

	if (aon_gpio_used == 0) {
		//RCC_PeriphClockCmd(APBPeriph_AON_GPIO, APBPeriph_AONGPIO_CLOCK, DISABLE);
	}

	if (pon_gpio_used == 0) {
		RCC_PeriphClockCmd(APBPeriph_PON_GPIO, APBPeriph_PON_GPIO_CLOCK, DISABLE);
	}

	RCC_PeriphClockCmd(APBPeriph_CORE_GPIO, APBPeriph_CORE_GPIO_CLOCK, DISABLE);
}


void SOCPS_SleepCG(void)
{
	u32 nDeviceIdOffset = PMU_MAX;
	uint32_t tick_before_sleep;
	volatile uint32_t ms_passed = 0;

#if defined(CONFIG_BT_PG_VERIFY) && CONFIG_BT_PG_VERIFY
	AON_TypeDef *AON = (AON_TypeDef *)AON_REG_BASE;
	//set wakeup event: IPC
	AON->AON_SLP_WAKE_EVENT_MSK0 |= (AON_BIT_SYSON_WEVT_FWIPC_MSK);
	AON->AON_SLP_WAKE_EVENT_MSK0 |= (AON_BIT_SYSON_WEVT_NPIPC_MSK);
#endif

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
		RTK_LOGD(TAG, "DBG: KM4 Sleep CG blocked because Dev %lx busy\n", nDeviceIdOffset);

		prvStartTickInterruptTimer();

		return;
	}

	//Enter SleepCG
	SOCPS_Set_Sleep_mode(1);

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

}

void SOCPS_DeepSleep(void)
{
	//DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
	RTK_LOGI(TAG, "NP sleep\n");
	pinmap_sleep();

	SOCPS_Gpio_Clkctr();

	//Enter Deep Sleep
	SOCPS_Set_Sleep_mode(1);
	pmu_set_power_event(EVENT_ENTER_DEEPSLEEP);
}

void SOCPS_SleepPG(void)
{
	u32 nDeviceIdOffset = PMU_MAX;
	uint32_t tick_before_sleep;
	volatile uint32_t ms_passed = 0;
	AON_TypeDef *AON = (AON_TypeDef *)AON_REG_BASE;
	RTK_LOGI(TAG, "NPPG\n");

	//set wakeup event: IPC
	AON->AON_SLP_WAKE_EVENT_MSK0 |= (AON_BIT_SYSON_WEVT_FWIPC_MSK);
	AON->AON_SLP_WAKE_EVENT_MSK0 |= (AON_BIT_SYSON_WEVT_NPIPC_MSK);
	// AON->AON_SLP_WAKE_EVENT_MSK0 |= (AON_BIT_PON_WEVT_LOGUART_MSK);

	//set FP wakeup event: wlan
	AON->AON_KM0_WAKE_EVENT_MSK |= (AON_BIT_PON_WEVT_WLAN1_MSK_0E0);
	AON->AON_KM0_WAKE_EVENT_MSK |= (AON_BIT_PON_WEVT_WLAN_MSK_0E0);

	prvStopTickInterruptTimer();

#if defined(configUSE_TICKLESS_IDLE) && (configUSE_TICKLESS_IDLE == 1)
	/*  Store gtimer timestamp before sleep */
	tick_before_sleep = SYSTIMER_TickGet();
#endif

	DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
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
		RTK_LOGD(TAG, "DBG: KM4 Sleep PG blocked because Dev %lx busy\n", nDeviceIdOffset);
		__asm volatile("cpsie i" ::: "memory");
		ICache_Enable();
		DCache_Enable();

		prvStartTickInterruptTimer();
		return;
	}

	//Enter SleepPG
	SOCPS_Set_Sleep_mode(1);

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

	pmu_set_power_event(EVENT_WAKEUP);
	RTK_LOGI(TAG, "NPPW\n");
}


void SOCPS_Enable_AP(u32 vector)
{
	if (vector) {
		RTK_LOGI(TAG, "Enable AP CA32 @ %08x\n", vector);
		RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_CBUS_SYS_MEM_CLOCK, ENABLE);
		RXI_CORE_BASE->IDAU_CTRL |= 0xf00;
		CA32_CTRL_BASE->CA32_FEN_CTRL = 0;
		DelayUs(20);
		CA32_CTRL_BASE->CA32_FEN_CTRL |= 0x1F7;	// enable every thing but not release warn reset
		CA32_CTRL_BASE->CA32_CLK_CTRL |= (CA32_CTRL_BIT_CLK_TRC_EN | CA32_CTRL_BIT_CLK_TS_EN);
		CA32_CTRL_BASE->CA32_REMAP = CA32_CTRL_CA32_REMAP_ADDR(((u32)vector) >> 16);
		CA32_CTRL_BASE->CA32_FEN_CTRL |= CA32_CTRL_BIT_CA32_CORE_FEN;

		CA32_CTRL_BASE->MBIST_CTRL5 |= CA32_CTRL_BIT_HPC_REMAP_RSTN;
	}
}

void SOCPS_Enable_MP(u32 vector)
{
	if (vector) {
		RTK_LOGI(TAG, "Enable MP KM4 @ %08x\n", vector);
		/* COREPOWER enabled in rom, enable clocks need for  */
		/* MP KM4 CPU */
		RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_KM4_MP_CLOCK, ENABLE);
		RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_CBUS_KM4_MP_CLOCK, ENABLE);
		RCC_PeriphClockCmd(APBPeriph_PLFM_CORE_MP_CLK, APBPeriph_CLOCK_NULL, ENABLE);
		RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_CBUS_SYS_MEM_CLOCK, ENABLE);
		/* MP Watch Dog */
		RCC_PeriphClockCmd(APBPeriph_WDT_MP, APBPeriph_WDT_MP_CLOCK, ENABLE);
		/* release MP KM4 CPU*/
		RCC_PeriphClockCmd(APBPeriph_KM4_MP_COLD, APBPeriph_CLOCK_NULL, ENABLE);

		/* setup VTOR for MP KM4*/
		KM4_MP_BASE->KM4_CFG1 = vector;
		KM4_MP_BASE->KM4_CFG2 = vector;
		/* release MP */
		RCC_PeriphClockCmd(APBPeriph_KM4_MP_WARM, APBPeriph_CLOCK_NULL, ENABLE);
	}
}

void SOCPS_Power_On(void)
{
	u32 power_state = AON_BASE->AON_PMC_CTRL;
	if (AON_GET_AON_PMC_STATE(power_state) == PMC_SNOZCOMBO) {
		//do some cofig
		RTK_LOGI(TAG, "power_state = PMC_SNOZCOMBO\r\n");
	} else if (AON_GET_AON_PMC_STATE(power_state) == PMC_SNOZ_MCU) {
		//snzmcu to snzcombo
		RTK_LOGI(TAG, "power_state = PMC_SNOZ_MCU\r\n");
		SOCPS_Snzmcu_To_Snzcombo();
	} else {
		RTK_LOGI(TAG, "power state err!\r\n");
	}

	DelayUs(1000);

	//snzcombo to active
	SOCPS_Snzcombo_To_Active();

	if ((AON_BASE->AON_SLP_WAKE_EVENT_MSK0 & AON_BIT_AON_WEVT_AON_GPIO0_MSK) != 0U) {
		//use wakeup source gpioa_0
		RTK_LOGI(TAG, "GPIOA_0 is configured as wake source, skip peripheral power on.\r\n");
	} else {
		//boot peripheral power enable: PERIPH_POWER_PIN
		System_Periph_PowerOn();
	}

#if defined(CONFIG_BT_PG_VERIFY) && CONFIG_BT_PG_VERIFY
	extern bool rtk_bt_is_enable(void);
	if (rtk_bt_is_enable()) {
		extern uint8_t hci_uart_reinit(void);
		if (!hci_uart_reinit()) {
			RTK_LOGI(TAG, "reinit hci uart failed\r\n");
		}

		//BT uses Internal uart, 0x40230030[4] = 0
		SYSON_BASE->PON_BT_CTRL &= ~(SYSON_BIT_BT_USE_EXT_UART);
	}
#endif

	extern u8 __vp_psram_vector__[];
	extern u8 __ap_psram_vector__[];
	SOCPS_Enable_MP((u32)__vp_psram_vector__);
	DelayUs(1000);
	SOCPS_Enable_AP((u32)__ap_psram_vector__);

	//SWD
	CA32_CTRL_BASE->CA32_FEN_CTRL |= (CA32_CTRL_BIT_CA32_TOP_FEN | CA32_CTRL_BIT_AP_PLT_PERI_FEN | CA32_CTRL_BIT_AP_PLT_MBUS_FEN);
	CA32_CTRL_BASE->CA32_CLK_CTRL |= (CA32_CTRL_BIT_CLK_TRC_EN | CA32_CTRL_BIT_CLK_TRC_EN);
	AON_BASE->AON_SYS_CTRL |= AON_BIT_DEBUGER_EN;
	Pinmux_Config(_PE_1, PINMUX_FUNCTION_SWD);
	Pinmux_Config(_PE_2, PINMUX_FUNCTION_SWD);
	/* SWD pull up */
	PAD_PullCtrl(_PE_1, GPIO_PuPd_UP);
	PAD_PullCtrl(_PE_2, GPIO_PuPd_UP);

}

void SOCPS_Power_Off(void)
{
	pinmap_sleep();
	SOCPS_Gpio_Clkctr();

#if defined(CONFIG_BT_PG_VERIFY) && CONFIG_BT_PG_VERIFY
	extern bool rtk_bt_is_enable(void);
	if (rtk_bt_is_enable()) {
		//BT uses External uart, 0x40230030[4] = 1
		SYSON_BASE->PON_BT_CTRL |= SYSON_BIT_BT_USE_EXT_UART;

		extern uint8_t hci_uart_deinit(void);
		if (!hci_uart_deinit()) {
			RTK_LOGI(TAG, "deinit hci uart failed!\r\n");
		}
	}
#endif

	//Active to snzcombo
	SOCPS_Active_To_Snzcombo();
}

void SOCPS_Sleep_FP(void)
{
	sleep_msg.msg_type = SLEEP_PG; //sleep

	ipc_send_message(IPC_NP_TO_FP, IPC_N2F_TICKLESS_INDICATION, (PIPC_MSG_STRUCT)&sleep_msg);
}

void SOCPS_FPWNP_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);

	ipc_get_message(IPC_FP_TO_NP, IPC_F2N_TICKLESS_INDICATION);
}

#if defined(CONFIG_BT_PG_VERIFY) && CONFIG_BT_PG_VERIFY
void SOCPS_FPWNP_bt_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);

	ipc_get_message(IPC_FP_TO_NP, IPC_F2N_TICKLESS_INDICATION_BT);

	extern uint32_t pmu_get_system_power_state(void);
	if (pmu_get_system_power_state() == PMU_SNOOZE) {
		pmu_acquire_wakelock(PMU_OS);
		pmu_set_power_event(EVENT_ENTER_ACTIVE);
	}
}
#endif

void SOCPS_VPWNP_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);

	ipc_get_message(IPC_VP_TO_NP, IPC_V2N_TICKLESS_INDICATION);
}

void SOCPS_APWNP_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);
	PIPC_MSG_STRUCT psleep_msg;

	psleep_msg = ipc_get_message(IPC_AP_TO_NP, IPC_A2N_TICKLESS_INDICATION);

	/* extract wake source config carried in IPC message fields */
	u32 wake_src_mask = psleep_msg->msg;
	u32 timer_ms      = psleep_msg->msg_len;
	u32 wake_src_pol  = psleep_msg->rsvd;

	if (wake_src_mask != 0) {
		int i;

		/* update sleep_wevent_config (from ameba_sleepcfg.c) based on AP's mask */
		for (i = 0; sleep_wevent_config[i].Module != 0xFFFFFFFF; i++) {
			sleep_wevent_config[i].Wakeup =
				(wake_src_mask & sleep_wevent_config[i].Module) ? WAKEUP_NP : WAKEUP_NULL;
		}

		/* (re-)initialize hardware wake sources via sleepcfg hooks */
		extern void SOCPS_WakeSourceSet(WakeEvent_TypeDef * sleep_wevent_config);
		SOCPS_WakeSourceSet(sleep_wevent_config);

		/* reconfigure AON GPIO polarity if AP specified non-default */
		if (wake_src_mask & (WAKE_SRC_GPIOA0 | WAKE_SRC_GPIOA1 | WAKE_SRC_GPIOA2 | WAKE_SRC_GPIOA3)) {
			static const struct {
				u32 src;
				u32 pin;
				u32 pol_bit;
			} aon_gpio_map[4] = {
				{WAKE_SRC_GPIOA0, _PA_0, 0},
				{WAKE_SRC_GPIOA1, _PA_1, 1},
				{WAKE_SRC_GPIOA2, _PA_2, 2},
				{WAKE_SRC_GPIOA3, _PA_3, 3},
			};
			int k;
			for (k = 0; k < 4; k++) {
				if (wake_src_mask & aon_gpio_map[k].src) {
					u32 pol = (wake_src_pol & BIT(aon_gpio_map[k].pol_bit)) ?
							  GPIO_INT_POLARITY_ACTIVE_HIGH : GPIO_INT_POLARITY_ACTIVE_LOW;
					GPIO_INTMode(aon_gpio_map[k].pin, ENABLE,
								 GPIO_INT_Trigger_EDGE, pol, GPIO_INT_DEBOUNCE_DISABLE);
				}
			}
		}

		/* propagate timer duration to NP sleep_param for SOCPS_SleepPG_RAM() */
		if (timer_ms != PMU_SLEEP_FOREVER) {
			extern SLEEP_ParamDef sleep_param;
			sleep_param.sleep_time = timer_ms;

			/* override hardcoded AON timer duration set in pmc_aontimer_init() */
			if (wake_src_mask & WAKE_SRC_AON_TIM) {
				AONTimer_Setting(timer_ms);
			}
		}
	} else {
		RTK_LOGW(TAG, "No wake source set by AP, system may not wake up\n");
	}

	/* use ISR-safe semaphore since this runs in IPC interrupt context */
	if (psleep_msg->msg_type == SLEEP_PG || psleep_msg->msg_type == SLEEP_CG) {
		RTK_LOGI(TAG, "NP sleep from AP\n");
		pmu_set_power_event_isr(EVENT_ENTER_SLEEP);
	} else if (psleep_msg->msg_type == SLEEP_DEEP_SLEEP) {
		RTK_LOGI(TAG, "NP deep sleep from AP\n");
		pmu_set_power_event_isr(EVENT_ENTER_DEEPSLEEP);
	}
}

/* do resume config at wakeup handle or wlan wakeup*/

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_f2ntickless_table[] = {
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = SOCPS_FPWNP_ipc_int,
		.RxIrqData = (void *) NULL,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_FP_TO_NP,
		.IPC_Channel = IPC_F2N_TICKLESS_INDICATION
	},
};

#if defined(CONFIG_BT_PG_VERIFY) && CONFIG_BT_PG_VERIFY
IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_f2ntickless_bt_table[] = {
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = SOCPS_FPWNP_bt_ipc_int,
		.RxIrqData = (void *) NULL,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_FP_TO_NP,
		.IPC_Channel = IPC_F2N_TICKLESS_INDICATION_BT
	},
};
#endif

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_v2ntickless_table[] = {
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = SOCPS_VPWNP_ipc_int,
		.RxIrqData = (void *) NULL,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_VP_TO_NP,
		.IPC_Channel = IPC_V2N_TICKLESS_INDICATION
	},
};

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_a2ntickless_table[] = {
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = SOCPS_APWNP_ipc_int,
		.RxIrqData = (void *) NULL,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_AP_TO_NP,
		.IPC_Channel = IPC_A2N_TICKLESS_INDICATION
	},
};
