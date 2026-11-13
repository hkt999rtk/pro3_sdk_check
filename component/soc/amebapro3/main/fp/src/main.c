#include <stdio.h>
#include <diag.h>

#include <FreeRTOS.h>
#include <task.h>
#include "semphr.h"

#include "ameba_soc.h"
#include "os_wrapper.h"

static const char *TAG = "FP_MAIN";

uint32_t SystemCoreClock = 40 * 1000 * 1000;

__weak void freertos_pre_sleep_processing(uint32_t *expected_idle_time)
{
	(void)expected_idle_time;
}
__weak void freertos_post_sleep_processing(uint32_t *expected_idle_time)
{
	(void)expected_idle_time;
}

__weak int pmu_ready_to_sleep(void)
{
	return 0;
}
__weak u32 pmu_get_sleep_type(void)
{
	return 0;
}
__weak void SOCPS_SleepPG(void) {}
__weak void SOCPS_SleepCG(void) {}

extern void wififw_task_idle(void);
void vApplicationIdleHook(void)
{
#if defined(CONFIG_WIFI_FW_EN) && CONFIG_WIFI_FW_EN && !defined(WIFIFWOFF)
	wififw_task_idle();
#endif
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
	(void)xTask;
	(void)pcTaskName;
}

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
								   StackType_t **ppxIdleTaskStackBuffer,
								   uint32_t *pulIdleTaskStackSize)
{
	/* If the buffers to be provided to the Idle task are declared inside this
	 * function then they must be declared static - otherwise they will be allocated on
	 * the stack and so not exists after this function exits. */
	static StaticTask_t xIdleTaskTCB;
	static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

	/* Pass out a pointer to the StaticTask_t structure in which the Idle
	 * task's state will be stored. */
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

	/* Pass out the array that will be used as the Idle task's stack. */
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;

	/* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
	 * Note that, as the array is necessarily of type StackType_t,
	 * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
									StackType_t **ppxTimerTaskStackBuffer,
									uint32_t *pulTimerTaskStackSize)
{
	/* If the buffers to be provided to the Timer task are declared inside this
	 * function then they must be declared static - otherwise they will be allocated on
	 * the stack and so not exists after this function exits. */
	static StaticTask_t xTimerTaskTCB;
	static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

	/* Pass out a pointer to the StaticTask_t structure in which the Idle
	 * task's state will be stored. */
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

	/* Pass out the array that will be used as the Timer task's stack. */
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;

	/* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
	 * Note that, as the array is necessarily of type StackType_t,
	 * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

#if defined(CONFIG_BT_PG_VERIFY) && CONFIG_BT_PG_VERIFY
SemaphoreHandle_t xInterruptSemaphore = NULL;
static u8 bt_wake_host_task_ipc_pending_flag = 0;
void bt_wake_host_int_handle(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if (xInterruptSemaphore != NULL) {
		if (!bt_wake_host_task_ipc_pending_flag) {
			bt_wake_host_task_ipc_pending_flag = 1;
			xSemaphoreGiveFromISR(xInterruptSemaphore, &xHigherPriorityTaskWoken);
		}
	}
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void bt_wake_host_task_function(void *dummy)
{
	(void)dummy;

	irq_register((IRQ_FUN)bt_wake_host_int_handle, BT_WAKE_HOST_IRQ, NULL, INT_PRI_LOWEST);
	irq_enable(BT_WAKE_HOST_IRQ);

	while (1) {
		if (xSemaphoreTake(xInterruptSemaphore, portMAX_DELAY) == pdTRUE) {
			IPC_MSG_STRUCT ipc_msg_temp;
			ipc_msg_temp.msg_type = IPC_USER_DATA;
			ipc_msg_temp.msg = 0;
			ipc_msg_temp.msg_len = 1;
			ipc_send_message(IPC_FP_TO_NP, IPC_F2N_TICKLESS_INDICATION_BT, &ipc_msg_temp);
			bt_wake_host_task_ipc_pending_flag = 0;
		}
	}
}
#endif

void test_task(void *dummy)
{
	int cpuid = SYS_CPUID();
	(void)dummy;
	DiagPrintf("[%d] RTOS test\n\r", cpuid);
	while (1) {
		vTaskDelay(1000);
		DiagPrintf("[%d] test\n\r", cpuid);
	}
}

void app_pmu_init(void)
{
	AON_NS_TypeDef *AON_NS = (AON_NS_TypeDef *)AON_NS_REG_BASE;

	pmu_acquire_wakelock(PMU_OS);
	//pmu_acquire_wakelock(PMU_WLAN_DEVICE);

	pmu_set_sleep_type(SLEEP_PG);

	AON_NS->SLP_WAKE_EVENT_STATUS0 |= (AON_BIT_AON_WEVT_AON_TIMER_STS);

	pmu_power_state_handler_create();

	SOCPS_SleepInit();
}

void wififw_task_create(void);

__weak void app_example(void)
{


}

int main(void)
{

	app_pmu_init();

	RTK_LOGI(TAG, "FP KM4 MAIN \n");

	//rtos_task_t test;
	//rtos_task_create(&test, "test", test_task, NULL, 512, 1);

#if defined(CONFIG_BT_PG_VERIFY) && CONFIG_BT_PG_VERIFY
	xInterruptSemaphore = xSemaphoreCreateBinary();
	rtos_task_t bt_wake_host_task;
	rtos_task_create(&bt_wake_host_task, "bt_wake_host_task", bt_wake_host_task_function, NULL, 512, 1);
#endif

#if defined(CONFIG_WIFI_FW_EN) && CONFIG_WIFI_FW_EN
	wififw_task_create();
#endif

	/* Execute application example */
	app_example();

	RTK_LOGI(TAG, "FP KM0 START SCHEDULER \n");

	/* Enable Schedule, Start Kernel */
	rtos_sched_start();
	while (1);
}
