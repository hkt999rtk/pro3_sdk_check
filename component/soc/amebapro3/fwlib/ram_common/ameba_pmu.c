/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "platform_autoconf.h"
#include "ameba_soc.h"
#include "ameba_pmu.h"


static uint32_t wakelock     = 0;//DEFAULT_WAKELOCK;
static uint32_t sleepwakelock_timeout     = 0;
uint32_t system_can_yield = 1; /* default is can */
static uint32_t timer_min_sleep_time = PMU_SLEEP_FOREVER;
static uint32_t timer_max_sleep_time = PMU_SLEEP_FOREVER;
static uint8_t sleep_type = SLEEP_PG; /* 0 is power gate, 1 is clock gate */
//static uint32_t max_sleep_time = 0; /* if user want wakeup peridically, can set this timer*/

static uint32_t deepwakelock     = 0;//DEFAULT_DEEP_WAKELOCK;
static uint32_t deepwakelock_timeout     = 0;
static uint32_t sysactive_timeout_temp = 0;
uint32_t sysactive_timeout_flag = 0;
u8 otp_swr_value = 0xff;

SLEEP_ParamDef sleep_param ALIGNMTO(32); /* cacheline aligned for lp & np */
IPC_MSG_STRUCT sleep_msg_temp;

/* ++++++++ FreeRTOS macro implementation ++++++++ */
/* psm dd hook info */
PSM_DD_HOOK_INFO gPsmDdHookInfo[PMU_MAX];


uint32_t pmu_exec_sleep_hook_funs(void)
{
	PSM_DD_HOOK_INFO *pPsmDdHookInfo = NULL;
	uint32_t nDeviceIdOffset = 0;
	uint32_t ret = TRUE;

	for (nDeviceIdOffset = 0; nDeviceIdOffset < PMU_MAX; nDeviceIdOffset++) {
		pPsmDdHookInfo = &gPsmDdHookInfo[nDeviceIdOffset];

		/*if this device register and sleep_hook_fun not NULL*/
		if (pPsmDdHookInfo && pPsmDdHookInfo->sleep_hook_fun) {
			ret = pPsmDdHookInfo->sleep_hook_fun(0, pPsmDdHookInfo->sleep_param_ptr);

			if (ret == FALSE) {
				break;
			}
		}
	}

	return nDeviceIdOffset;
}

void pmu_exec_wakeup_hook_funs(uint32_t nDeviceIdMax, uint32_t common_param)
{
	PSM_DD_HOOK_INFO *pPsmDdHookInfo = NULL;
	uint32_t nDeviceIdOffset = 0;

	for (nDeviceIdOffset = 0; nDeviceIdOffset < nDeviceIdMax; nDeviceIdOffset++) {
		pPsmDdHookInfo = &gPsmDdHookInfo[nDeviceIdOffset];

		/*if this device register and sleep_hook_fun not NULL*/
		if (pPsmDdHookInfo && pPsmDdHookInfo->wakeup_hook_fun) {
			pPsmDdHookInfo->wakeup_hook_fun(common_param, pPsmDdHookInfo->wakeup_param_ptr);
		}
	}
}

#define SYSTICK_THRES 0x7fffffff
/*
return: TRUE: time1 > time2
*/
static int pmu_systick_check(uint32_t time1, uint32_t time2)
{
	uint32_t delta = time1 > time2 ? time1 - time2 : time2 - time1;
	if (delta < SYSTICK_THRES) {
		return time1 >= time2 ? TRUE : FALSE;
	} else {	//overflow
		return time1 <= time2 ? TRUE : FALSE;
	}
}


uint32_t pmu_set_sysactive_time(uint32_t timeout)
{
	uint32_t TimeOut = 0;

	if (sysactive_timeout_flag) {
		if (timeout > sysactive_timeout_temp) {
			sysactive_timeout_temp = timeout;
		}
		return 1;
	}

	if (sysactive_timeout_temp > timeout) {
		timeout = sysactive_timeout_temp;
	}
	sysactive_timeout_temp = 0;

	TimeOut = rtos_time_get_current_system_time_ms() + timeout;

	if (pmu_systick_check(TimeOut, sleepwakelock_timeout)) {
		sleepwakelock_timeout = TimeOut;
	}

	return 0;
}

void pmu_register_sleep_callback(uint32_t nDeviceId, PSM_HOOK_FUN sleep_hook_fun, void *sleep_param_ptr,
								 PSM_HOOK_FUN wakeup_hook_fun, void *wakeup_param_ptr)
{
	PSM_DD_HOOK_INFO *pPsmDdHookInfo = NULL;

	assert_param(nDeviceId < PMU_MAX);
	assert_param((sleep_hook_fun != NULL) || (wakeup_hook_fun != NULL));

	pPsmDdHookInfo = &gPsmDdHookInfo[nDeviceId];
	pPsmDdHookInfo->nDeviceId			= nDeviceId;
	pPsmDdHookInfo->sleep_hook_fun		= sleep_hook_fun;
	pPsmDdHookInfo->sleep_param_ptr		= sleep_param_ptr;
	pPsmDdHookInfo->wakeup_hook_fun		= wakeup_hook_fun;
	pPsmDdHookInfo->wakeup_param_ptr	= wakeup_param_ptr;
}

void pmu_unregister_sleep_callback(uint32_t nDeviceId)
{
	PSM_DD_HOOK_INFO *pPsmDdHookInfo = NULL;
	assert_param(nDeviceId < PMU_MAX);
	pPsmDdHookInfo = &gPsmDdHookInfo[nDeviceId];
	_memset(pPsmDdHookInfo, 0x00, sizeof(PSM_DD_HOOK_INFO));
}


/* can not yield CPU under suspend/resume process */
uint32_t pmu_yield_os_check(void)
{
	return system_can_yield;
}

#if defined (CONFIG_ARM_CORE_CM0)
uint32_t np_clk_status_on(void)
{
	uint32_t ret = 0;

	if ((AON_NS_BASE->AON_SW_BOOT5 & NP_STATUS_BIT)) {
		ret = 0;
	} else {
		ret = 1;
	}


	return ret;
}

uint32_t np_status_on(void)
{
	uint32_t ret = 0;

	if ((AON_NS_BASE->AON_SW_BOOT5 & NP_STATUS_BIT)) {
		ret = 0;
	} else {
		ret = 1;
	}

	return ret;
}

uint32_t ap_clk_status_on(void)
{
	return np_clk_status_on();
}

uint32_t ap_status_on(void)
{
	return np_status_on();
}
#endif

uint32_t pmc_status_active(void)
{
	uint32_t ret = 0;

	if ((AON_NS_BASE->AON_SW_BOOT5 & PMC_STATUS_BIT)) {
		ret = 1;
	} else {
		ret = 0;
	}

	return ret;
}

void pmc_set_active(uint32_t type)
{
	if (type == 0) {
		AON_NS_BASE->AON_SW_BOOT5 &= ~PMC_STATUS_BIT;
#if defined(CONFIG_WLAN) && (CONFIG_WLAN==1)
#ifdef CONFIG_WHC_NONE
		//Disable wifi fast connect because it will  read/writer flash
		extern void wifi_fast_connect_enable(u8 enable);
		wifi_fast_connect_enable(0);
#endif
#endif
	} else {
		AON_NS_BASE->AON_SW_BOOT5 |= PMC_STATUS_BIT;
#if defined(CONFIG_WLAN) && (CONFIG_WLAN==1)
#ifdef CONFIG_WHC_NONE
		//Enable wifi fast connect because it will  read/writer flash
		extern void wifi_fast_connect_enable(u8 enable);
		wifi_fast_connect_enable(1);
#endif
#endif
	}

}

/*
 *  It is called in idle task.
 *
 *  @return  true  : System is ready to check conditions that if it can enter sleep.
 *           false : System keep awake.
 **/
int pmu_ready_to_sleep(void)
{
	uint32_t current_tick = xTaskGetTickCount();

	/* timeout - use overflow-safe comparison */
	if (!pmu_systick_check(current_tick, sleepwakelock_timeout)) {
		return FALSE;
	}

	/* Active mode*/
	if (pmc_status_active() == 1) {
		return FALSE;
	}

	if (wakelock == 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/*
 *  It is called in freertos pre_sleep_processing.
 *
 *  @return  true  : System is ready to check conditions that if it can enter dsleep.
 *           false : System can't enter deep sleep.
 **/
int pmu_ready_to_dsleep(void)
{
	uint32_t current_tick = xTaskGetTickCount();

	/* timeout - use overflow-safe comparison */
	if (!pmu_systick_check(current_tick, deepwakelock_timeout)) {
		return FALSE;
	}

	if (deepwakelock == 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/*
 *  It is called when freertos is going to sleep.
 *  At this moment, all sleep conditons are satisfied. All freertos' sleep pre-processing are done.
 *
 *  @param  expected_idle_time : The time that FreeRTOS expect to sleep.
 *                               If we set this value to 0 then FreeRTOS will do nothing in its sleep function.
 **/
#if defined (CONFIG_ARM_CORE_CM4)

void pmu_pre_sleep_processing(uint32_t *tick_before_sleep)
{
	(void)tick_before_sleep;
}
#else
void pmu_pre_sleep_processing(uint32_t *tick_before_sleep)
{
	(void)tick_before_sleep;
}
#endif

/* -------- FreeRTOS macro implementation -------- */

void pmu_acquire_wakelock(uint32_t nDeviceId)
{
	wakelock |= BIT(nDeviceId);
}

void pmu_release_wakelock(uint32_t nDeviceId)
{
#if defined(CONFIG_ARM_CORE_CM4) && defined(CONFIG_LINUX_FW_EN)
	return;
#endif

#if defined (CONFIG_ARM_CORE_CM4)
	if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
		if (pmu_get_system_power_state() == PMU_ACTIVE && nDeviceId == PMU_OS) {
#if defined(CONFIG_BT) && CONFIG_BT
#if !defined(CONFIG_BT_PG_VERIFY) || !CONFIG_BT_PG_VERIFY
			extern bool rtk_bt_is_enable(void);
			if (rtk_bt_is_enable()) {
				DiagPrintf("NP enters PG failed because bt is enabled!\r\n");
				return;
			}
#endif
#endif
			pmu_set_power_event(EVENT_ENTER_SNOOZE);
			vTaskDelay(100);
			sleep_msg_temp.msg_type = sleep_type; //sleep
#if defined(CONFIG_BT_PG_VERIFY) && CONFIG_BT_PG_VERIFY
			extern bool rtk_bt_is_enable(void);
			if (!rtk_bt_is_enable()) {
				ipc_send_message(IPC_NP_TO_FP, IPC_N2F_TICKLESS_INDICATION, (PIPC_MSG_STRUCT)&sleep_msg_temp);
			}
#else
			ipc_send_message(IPC_NP_TO_FP, IPC_N2F_TICKLESS_INDICATION, (PIPC_MSG_STRUCT)&sleep_msg_temp);
#endif
			vTaskDelay(100);
		}
	}
#endif
	wakelock &= ~BIT(nDeviceId);
}

uint32_t pmu_get_wakelock_status(void)
{
	return wakelock;
}

uint32_t pmu_set_sleep_type(uint32_t type)
{
	sleep_type = type;
	return 0;
}

uint32_t pmu_get_sleep_type(void)
{
	return sleep_type;
}

void pmu_get_expected_idle_time(uint32_t expected_idle_time)
{
	sleep_param.sleep_time = expected_idle_time;
}

void pmu_set_sleep_time_range(uint32_t min_time, uint32_t max_time)
{
	timer_min_sleep_time = min_time;
	timer_max_sleep_time = max_time;
}

void pmu_set_max_sleep_time(uint32_t timer_ms)
{
	timer_max_sleep_time = timer_ms;
}

uint32_t pmu_get_sleep_time(void)
{
	u32 time = 0;
	/*case 1: max = 0, including [0, 0], [min, 0], invalid settings.
	  case 2.1: min <= max, max != 0, wake up multiple times.
	  case 2.2: min > max, inlcuding [PMU_SLEEP_FOREVER, max], and can only be woken up once.
	*/
	if (timer_max_sleep_time == 0) {
		time = PMU_SLEEP_FOREVER;
		pmu_set_sleep_time_range(PMU_SLEEP_FOREVER, PMU_SLEEP_FOREVER);
	} else {
		if (timer_max_sleep_time >= timer_min_sleep_time) {
			/*min == max, periodic awakening, but [PMU_SLEEP_FOREVER, PMU_SLEEP_FOREVER] is invalid.
			  min < max, randomly wakeup*/
			time = _rand() % (timer_max_sleep_time - timer_min_sleep_time + 1) + timer_min_sleep_time;
			//DiagPrintf("time = 0x%X\r\n", time);
		} else {
			time = timer_max_sleep_time;
			pmu_set_sleep_time_range(PMU_SLEEP_FOREVER, PMU_SLEEP_FOREVER);
		}
	}

	return time;
}

u32 pmc_wakeuptimer_int_hdl(void *Data)
{
	UNUSED(Data);
	u32 TimerIdx = 1;

	RTIM_Cmd(TimerIdx, DISABLE);
	RTIM_INTClear(TimerIdx);
	return TRUE;
}

void pmu_init_wakeup_timer(void)
{
	u32 TimerGrp = 0;
	u32 TimerIdx = 1;
	RTIM_GTIMER_Init(TimerGrp);
	RTIM_RegTimeoutIRQ(TimerIdx, (IRQ_FUN)pmc_wakeuptimer_int_hdl, (void *)TimerIdx);
	RTIM_Cmd(TimerIdx, DISABLE);
	SYSON_BASE->PON_0_CLK_CTRL |= (SYSON_BIT_PON_TIMER_INTRCLK_SEL);
}

void pmu_set_wakeup_timer(uint32_t timeout_ms)
{
	RTIM_TimeBaseInitTypeDef TIM_InitStruct;
	u32 TimerGrp = 0;
	u32 TimerIdx = 1;
	u32 passed = (timeout_ms << 5) + (((timeout_ms << 7) + (timeout_ms << 6) + (timeout_ms << 2) + timeout_ms) >> 8);

	RTIM_TimeBaseStructInit(&TIM_InitStruct);
	TIM_InitStruct.TIM_TimerIdx = TimerIdx;
	TIM_InitStruct.TIM_Prescaler = 0;
	TIM_InitStruct.TIM_LoadCnt = passed - 1;
	TIM_InitStruct.TIM_IntMask = ENABLE;
	RTIM_TimeBaseInit(TimerGrp, &TIM_InitStruct);

	RTIM_INTConfig(TimerIdx, 0, ENABLE);
	RTIM_Cmd(TimerIdx, ENABLE);

	AON_BASE->AON_SLP_WAKE_EVENT_MSK0 |= AON_BIT_PON_WEVT_GTIM0_MSK;
}

void pmu_reset_wakeup_timer(void)
{
	u32 TimerIdx = 1;
	RTIM_Cmd(TimerIdx, DISABLE);

	AON_BASE->AON_SLP_WAKE_EVENT_STATUS0 |= (AON_BIT_PON_WEVT_GTIM0_STS);
	AON_NS_BASE->SLP_WAKE_EVENT_STATUS0 |= (AON_NS_BIT_PON_WEVT_GTIM0_STS);
}

void pmu_set_dsleep_active_time(uint32_t TimeOutMs)
{
	uint32_t timeout = 0;

	timeout = xTaskGetTickCount() + TimeOutMs;

	if (timeout > deepwakelock_timeout) {
		deepwakelock_timeout = timeout;
	}
}

void pmu_acquire_deepwakelock(uint32_t nDeviceId)
{
	deepwakelock |= BIT(nDeviceId);
}

void pmu_release_deepwakelock(uint32_t nDeviceId)
{
	pmc_set_active(0);
	pmu_release_wakelock(PMU_AP_RUN);
	pmu_release_wakelock(PMU_VP_RUN);
	deepwakelock &= ~BIT(nDeviceId);
}

uint32_t pmu_get_deepwakelock_status(void)
{
	return deepwakelock;
}

void pmu_systimer_init(void)
{
	// gtimer 0
	RCC_PeriphClockCmd(APBPeriph_PON_TIMERx[0], APBPeriph_PON_TIMERx_CLOCK[0], ENABLE);
	RCC_PeriphClockSourceSet(PON_TIMER0_CLK, SDM32K);
	SYSTIMER_Init();
}

#if defined (CONFIG_ARM_CORE_CM4 )
static uint32_t system_power_state = PMU_ACTIVE;
volatile uint32_t event_from_irq = EVENT_NONE;
SemaphoreHandle_t power_state_semaphore;
#define STACK_SIZE 512
StaticTask_t xTaskBuffer;
ALIGNMTO(32)
StackType_t xStack[ STACK_SIZE ];

void pmu_set_power_event(uint32_t event)
{
	event_from_irq = event;

	xSemaphoreGive(power_state_semaphore);
}

void pmu_set_power_event_isr(uint32_t event)
{
	event_from_irq = event;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	xSemaphoreGiveFromISR(power_state_semaphore, &xHigherPriorityTaskWoken);

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

uint32_t pmu_get_system_power_state(void)
{
	return system_power_state;
}

void state_machine_run(uint32_t event)
{
	switch (system_power_state) {
	case PMU_ACTIVE:
		if (event == EVENT_ENTER_SNOOZE) {
			//active to snz
			extern void SOCPS_Power_Off(void);
			SOCPS_Power_Off();
			pmc_set_active(0);
			pmu_release_wakelock(PMU_AP_RUN);
			pmu_release_wakelock(PMU_VP_RUN);
			system_power_state = PMU_SNOOZE;
		} else if (event == EVENT_ENTER_SLEEP) {
			//active to snz
			extern void SOCPS_Power_Off(void);
			SOCPS_Power_Off();
			pmc_set_active(0);
			pmu_release_wakelock(PMU_AP_RUN);
			pmu_release_wakelock(PMU_VP_RUN);
			//snz to sleep
			SOCPS_Sleep_FP();
			system_power_state = PMU_SNOOZE;
			pmu_release_wakelock(PMU_OS);
			system_power_state = PMU_SLEEP;
			//active to sleep
		} else if (event == EVENT_ENTER_DEEPSLEEP) {
			//active to deepsleep
			system_power_state = PMU_SLEEP;
			extern void SOCPS_DeepSleep_RAM(void);
			SOCPS_DeepSleep_RAM();
		}
		break;

	case PMU_SLEEP:
		if (event == EVENT_WAKEUP) {
			//wakeup event handle
			system_power_state = PMU_SNOOZE;
			extern void SOCPS_WakeEventHandle(WakeEvent_TypeDef * sleep_wevent_config);
			SOCPS_WakeEventHandle(sleep_wevent_config);
		}
		break;

	case PMU_SNOOZE:
		if (event == EVENT_WAKEUP) {
			//wakeup event handle
			system_power_state = PMU_SNOOZE;
			extern void SOCPS_WakeEventHandle(WakeEvent_TypeDef * sleep_wevent_config);
			SOCPS_WakeEventHandle(sleep_wevent_config);
		} else if (event == EVENT_ENTER_SLEEP) {
			system_power_state = PMU_SLEEP;
		} else if (event == EVENT_ENTER_ACTIVE) {
			//snz to active
			extern void SOCPS_Power_On(void);
			SOCPS_Power_On();
			pmc_set_active(1);
			pmu_acquire_wakelock(PMU_AP_RUN);
			pmu_acquire_wakelock(PMU_VP_RUN);
			system_power_state = PMU_ACTIVE;
		}
		break;
	}
}

void vPowerStateMachineTask(void *pvParameters)
{
	uint32_t event;
	(void)pvParameters;

	while (1) {
		if (xSemaphoreTake(power_state_semaphore, portMAX_DELAY) == pdTRUE) {
			event = event_from_irq;
			event_from_irq = EVENT_NONE;

			state_machine_run(event);
		}
	}
}

void wdt_task(void *dummy)
{
	(void)dummy;
	DiagPrintf("AONWDG_Enable\n\r");
	AONWDG_Init(0, 10000);
	AONWDG_Enable(0);

	while (1) {
		vTaskDelay(1000);

		AONWDG_Refresh(0);

	}
}

int pmu_power_state_handler_create(void)
{
	TaskHandle_t xHandle = NULL;
	power_state_semaphore = xSemaphoreCreateBinary();

	if (power_state_semaphore == NULL) {
		return -1;
	}

	/* Create the task without using any dynamic memory allocation. */
	xHandle = xTaskCreateStatic(
				  vPowerStateMachineTask,
				  "Power State Machine",
				  STACK_SIZE,
				  (void *) 1,
				  3,
				  xStack,
				  &xTaskBuffer);

	if (xHandle == NULL) {
		return -1;
	}

	AON_BASE->AON_WDT_TIMER |= AON_BIT_WDT_CLR_EN;
	//rtos_task_t wdt_control;
	//rtos_task_create(&wdt_control, "wdt", wdt_task, NULL, 512, 3);
	pmc_set_active(1);

	OTP_Read8(0x6E1, &otp_swr_value);

	return 0;
}
#endif

#if defined (CONFIG_ARM_CORE_CM0)
static uint32_t system_power_state = PMU_SNOOZE;
volatile uint32_t event_from_irq = EVENT_NONE;
SemaphoreHandle_t power_state_semaphore;
#define STACK_SIZE 512
StaticTask_t xTaskBuffer;
ALIGNMTO(32)
StackType_t xStack[ STACK_SIZE ];

void pmu_set_power_event(uint32_t event)
{
	event_from_irq = event;

	xSemaphoreGive(power_state_semaphore);
}

void pmu_set_power_event_isr(uint32_t event)
{
	event_from_irq = event;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	xSemaphoreGiveFromISR(power_state_semaphore, &xHigherPriorityTaskWoken);

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

uint32_t pmu_get_system_power_state(void)
{
	return system_power_state;
}

extern uint32_t fp_wevt;
void state_machine_run(uint32_t event)
{
	switch (system_power_state) {
	case PMU_SNOOZE:
		if (event == EVENT_WAKEUP) {
			uint32_t wevt = fp_wevt;
			uint32_t wlan_evt = AON_NS_BIT_PON_WEVT_WLAN_STS |
								AON_NS_BIT_PON_WEVT_WLAN1_STS;
			uint32_t ipc_evt = AON_NS_BIT_SYSON_WEVT_FWIPC_STS |
							   AON_NS_BIT_SYSON_WEVT_NPIPC_STS;

			//DiagPrintf("wevt = 0x%X\r\n", wevt);

			if (wevt == 0) {
				DiagPrintf("wlan wakeup event lose\r\n");
			} else if ((wevt & wlan_evt) != 0) {
				/* WLAN wakeup */
				//DiagPrintf("wakeup by WLAN! (0x%08lx)\r\n", wevt);
			} else if ((wevt & ipc_evt) != 0) {
				/* IPC wakeup */
				DiagPrintf("wakeup by IPC! (0x%08lx)\r\n", wevt);
				pmu_acquire_wakelock(PMU_OS);
			} else {
				extern void SOCPS_WakeEventHandle(WakeEvent_TypeDef * sleep_wevent_config);
				SOCPS_WakeEventHandle(sleep_wevent_config);
				//DiagPrintf("pmu_acquire_wakelock PMU_OS\r\n");
				//pmu_acquire_wakelock(PMU_OS);
			}
		}
		break;
	}
}

void vPowerStateMachineTask(void *pvParameters)
{
	uint32_t event;
	(void)pvParameters;

	while (1) {
		if (xSemaphoreTake(power_state_semaphore, portMAX_DELAY) == pdTRUE) {
			event = event_from_irq;
			event_from_irq = EVENT_NONE;

			state_machine_run(event);
		}
	}
}

int pmu_power_state_handler_create(void)
{
	TaskHandle_t xHandle = NULL;
	power_state_semaphore = xSemaphoreCreateBinary();

	if (power_state_semaphore == NULL) {
		return -1;
	}

	/* Create the task without using any dynamic memory allocation. */
	xHandle = xTaskCreateStatic(
				  vPowerStateMachineTask,
				  "Power State Machine",
				  STACK_SIZE,
				  (void *) 1,
				  3,
				  xStack,
				  &xTaskBuffer);

	if (xHandle == NULL) {
		return -1;
	}

	return 0;
}
#endif

#if defined (CONFIG_ARM_CORE_CA32)
void vPowerStateMachineTask(void *pvParameters)
{
	(void)pvParameters;

	while (1) {
		vTaskDelay(1000);
		if (wakelock == 0 && deepwakelock == 0) {
			if (timer_max_sleep_time != PMU_SLEEP_FOREVER) {
				SOCPS_AP_SetWakeSrc(WAKE_SRC_AON_TIM, 0, timer_max_sleep_time);
			}
			SOCPS_DeepSleep();
		} else if (wakelock == 0) {
			if (timer_max_sleep_time != PMU_SLEEP_FOREVER) {
				SOCPS_AP_SetWakeSrc(WAKE_SRC_AON_TIM, 0, timer_max_sleep_time);
			}
			SOCPS_Sleep();
		}
	}
}

int pmu_power_state_handler_create(void)
{
	if (xTaskCreate(vPowerStateMachineTask, ((const char *)"ap_power_state"), 512, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		return -1;
	}
	return 0;
}
#endif


