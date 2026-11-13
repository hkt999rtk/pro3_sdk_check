#include <stdio.h>
#include <diag.h>
#include <log.h>

#include <FreeRTOS.h>
#include <task.h>

#include "ameba_soc.h"
#include "os_wrapper.h"

static const char *TAG = "VP_MAIN";

__weak void freertos_pre_sleep_processing(uint32_t *expected_idle_time)
{
	(void)expected_idle_time;
}
__weak void freertos_post_sleep_processing(uint32_t *expected_idle_time)
{
	(void)expected_idle_time;
}

__weak void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
	(void)xTask;
	(void)pcTaskName;
}

/* static allocation for IDLE and TIMER task */
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

void vApplicationIdleHook(void)
{
	__WFI();
}

extern u32 vp_sleep_suspend;
void test_task(void *dummy)
{
	(void)dummy;
	int cpuid = SYS_CPUID();
	DiagPrintf("[%d] RTOS test\n\r", cpuid);
	while (1) {
		vTaskDelay(1000);
		DiagPrintf("[%d] test\n\r", cpuid);
		if (vp_sleep_suspend == 1) {
			vTaskSuspendAll();
			while (1);
		}
	}
}

/* bottom half synchronization */
extern void bf_main(void);
void bf_main_task(void *dummy)
{
	(void)dummy;
	/* wait rest firmware ready */
	IPC_SEMTake(IPC_SEM_ID31, 0xffffffff);
	IPC_SEMFree(IPC_SEM_ID31);

	ICache_Invalidate();
	extern void stage2_data_init(void);
	extern void stage2_bss_init(void);
	/* bf bss init */
	stage2_bss_init();
	/* bf data init */
	stage2_data_init();

	RTK_LOGI(TAG, "VP KM4 BF MAIN \n");
#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
	/* Signal stage1 ISP task that stage2 firmware is ready for handoff. */
	extern void video_fcs_isp_stage1_sema_give(void);
	video_fcs_isp_stage1_sema_give();
#endif
	/* User BF main */
	bf_main();

	/* kill self */
	rtos_task_delete(NULL);
}

__weak void app_example(void)
{


}

int main(void)
{
	RTK_LOGI(TAG, "VP KM4 MAIN \n");
	vp_sleep_suspend = 0;

	/* User main init code */


	/* Setup BF main */
	rtos_task_t bfmain;
	rtos_task_create(&bfmain, "bfmain", bf_main_task, NULL, 1024, 1);

#if defined(CONFIG_VIDEO_SW) && (CONFIG_VIDEO_SW == 1)
	/* Start service for receiving AP command */
	extern void vp_ipc_sw_service_init(void);
	vp_ipc_sw_service_init();
#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
	extern void video_app_fast_camera_start(void);
	video_app_fast_camera_start();
#endif /* CONFIG_FAST_CAMERA_START */
#endif /* CONFIG_VIDEO_SW */

	/* Execute application example */
	app_example();

	/* Enable Schedule, Start Kernel */
	RTK_LOGI(TAG, "VP KM4 START SCHEDULER \n");
	rtos_sched_start();
	while (1);
}
