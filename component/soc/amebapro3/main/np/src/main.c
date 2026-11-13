
#include "ameba_soc.h"
#include "main.h"
#include "os_wrapper.h"
#include "FreeRTOS.h"
#include "task.h"
#if defined(CONFIG_BT_COEXIST)
#include "rtw_coex_ipc.h"
#endif
static const char *TAG = "NP_MAIN";

__weak void vApplicationStackOverflowHook(struct tskTaskControlBlock *a, char *b)
{
	(void)a;
	(void)b;
}

// remove from main.c
__weak int pmu_ready_to_sleep(void)
{
	return 0;
}

__weak u32 pmu_get_sleep_type(void)
{
	return 0;
}

__weak void SOCPS_SleepPG(void)
{

}

__weak void SOCPS_SleepCG(void)
{

}

void freertos_pre_sleep_processing(uint32_t *expected_idle_time)
{
	(void)expected_idle_time;
}

void freertos_post_sleep_processing(uint32_t *expected_idle_time)
{
	(void)expected_idle_time;
}

ALIGNMTO(32)
SRAM_ONLY_BSS_SECTION
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];
SRAM_ONLY_BSS_SECTION
static StaticTask_t xIdleTaskTCB;

/* static allocation for IDLE and TIMER task */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
								   StackType_t **ppxIdleTaskStackBuffer,
								   uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

ALIGNMTO(32)
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];
static StaticTask_t xTimerTaskTCB;

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
									StackType_t **ppxTimerTaskStackBuffer,
									uint32_t *pulTimerTaskStackSize)
{
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void app_pmu_init(void)
{
	rtk_log_level_set("NP_PMC", RTK_LOG_INFO);
	pmu_acquire_deepwakelock(PMU_OS);
	pmu_acquire_wakelock(PMU_OS);
	pmu_acquire_wakelock(PMU_AP_RUN);
	pmu_acquire_wakelock(PMU_VP_RUN);
	//pmu_acquire_wakelock(PMU_WLAN_DEVICE);
#if defined(configUSE_TICKLESS_IDLE)
	pmu_systimer_init();
#endif

	pmu_set_sleep_type(SLEEP_PG);
	pmu_power_state_handler_create();
	SOCPS_SleepInit();
#ifdef CONFIG_STANDARD_TICKLESS
	pmu_init_wakeup_timer();
#endif
	//pmu_set_sleep_time_range(1*60*1000,5*60*1000);
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

SRAM_ONLY_DATA_SECTION
char *test_str = "SRAM data is here\n";
SRAM_ONLY_BSS_SECTION
int test_var = 0;

SRAM_ONLY_TEXT_SECTION
void sram_test_func(void)
{
	DiagPrintf("%d %s", test_var, test_str);
	test_var++;
	DiagPrintf("%d %s", test_var, test_str);
}

_WEAK void app_example(void)
{


}

//default main
int main(void)
{
	RTK_LOGI(TAG, "NP KM4 MAIN \n");
	RTK_LOGI(TAG, "NP KM4 START SCHEDULER \n");

	app_pmu_init();

	//sram_test_func();
	//rtos_task_t test;
	//rtos_task_create(&test, "test", test_task, NULL, 512, 1);
#if defined(CONFIG_BT_COEXIST)
	/* init coex ipc */
	coex_ipc_entry();
#endif
#if defined(CONFIG_WLAN) && (CONFIG_WLAN==1)
	rtk_log_level_set(NOTAG, RTK_LOG_DEBUG);
	rtk_log_level_set("WLAN", RTK_LOG_DEBUG);
	extern void wifi_init(void);
	wifi_init();
#endif

	/* Execute application example */
	app_example();

	/* Enable Schedule, Start Kernel */
	rtos_sched_start();
}

