#include <diag.h>
#include <log.h>

#include <FreeRTOS.h>
#include <task.h>

#include "ameba_soc.h"
#include "os_wrapper.h"
#if defined(CONFIG_BT_COEXIST)
#include "rtw_coex_ipc.h"
#endif

static const char *TAG = "AP_MAIN";

void freertos_pre_sleep_processing(uint32_t *expected_idle_time)
{
	(void)expected_idle_time;
}
void freertos_post_sleep_processing(uint32_t *expected_idle_time)
{
	(void)expected_idle_time;
}

uint32_t SystemCoreClock = 20 * 1000 * 1000;

__weak void vApplicationMallocFailedHook(size_t xWantedSize)
{
	(void)xWantedSize;
}

__weak void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
	(void)xTask;
	(void)pcTaskName;
}

__weak void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
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

__weak void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
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

void vApplicationIdleHook(void)
{
	__WFI();
}

void test_task(void *dummy)
{
	(void)dummy;
	int cpuid = SYS_CPUID();
	DiagPrintf("[%d] RTOS test\n\r", cpuid);
	while (1) {
		vTaskDelay(1000);
		DiagPrintf("[%d] test\n\r", cpuid);
	}
}

__weak void app_example(void)
{


}

void app_pmu_init(void)
{
	pmu_acquire_deepwakelock(PMU_OS);
	pmu_acquire_wakelock(PMU_OS);
	pmu_set_sleep_type(SLEEP_PG);
	int ret = pmu_power_state_handler_create();
	if (ret) {
		RTK_LOGD(TAG, "DBG: power handle create fail!\n");
	}
}

int main(void)
{

	RTK_LOGI(TAG, "AP CA32 MAIN\n");

	app_pmu_init();

	//rtos_task_t test;
	//rtos_task_create(&test, "test", test_task, NULL, 512, 1);
#if defined(CONFIG_WLAN)
	extern void wifi_init(void);
	wifi_init();
#endif

#if defined(CONFIG_BT_COEXIST)
	/* init coex ipc */
	coex_ipc_entry();
#endif

	/* execute application example */
	app_example();

	RTK_LOGI(TAG, "AP CA32 START SCHEDULER \n");

	vTaskStartScheduler();
	while (1);
}
