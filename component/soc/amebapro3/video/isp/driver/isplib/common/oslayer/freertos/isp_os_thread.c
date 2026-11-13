/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2022 Sherry Cheng <sherry_cheng@realsil.com.cn>
 */

#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <isp_os_mem.h>
#include <isp_os_thread.h>
#include <rts_isp_errno.h>

#define STACK_SIZE 4096

enum freertos_task_priorities {
	TASK_PRIORITY_LOWER = 1,
	TASK_PRIORITY_MID,
	TASK_PRIORITY_HIGHER,
};

struct thread_start_data {
	void (*start)(void *);
	void *arg;
};

typedef struct thread_internal {
	StaticTask_t xTaskBuffer;
	StaticSemaphore_t xSemaphore;
	StackType_t xStack[STACK_SIZE];
} thread_internal_t;

static UBaseType_t os_priority_to_task_priority(int priority)
{
	switch (priority) {
	case ISP_OS_THREAD_PRIORITY_LOWER:
		return TASK_PRIORITY_LOWER;
	case ISP_OS_THREAD_PRIORITY_MID:
		return TASK_PRIORITY_MID;
	case ISP_OS_THREAD_PRIORITY_HIGHER:
		return TASK_PRIORITY_HIGHER;
	}
	return -RTS_ISP_EINVAL;
}

static void pvThreadRun(void *pvParameters)
{
	void *_arg;
	void (*_start)(void *);
	struct thread_start_data *data = pvParameters;

	if (!data || !data->start) {
		return;
	}
	_arg = data->arg;
	_start = data->start;
	isp_free(data);

	_start(_arg);
}

int isp_os_thread_create(isp_os_thread_t *newthread, const char *const name,
						 int priority, void (*start)(void *), void *arg)
{
	int ret = RTS_ISP_OK;
	thread_internal_t *pxThread = NULL;
	int _priority;
	UBaseType_t uxPriority;
	SemaphoreHandle_t xSemaphore;
	TaskHandle_t xHandle = NULL;
	struct thread_start_data *data;

	if (!newthread || !start) {
		return -RTS_ISP_EINVAL;
	}

	_priority = os_priority_to_task_priority(priority);
	if (_priority < 0) {
		return -RTS_ISP_EINVAL;
	}
	uxPriority = _priority | portPRIVILEGE_BIT;

	pxThread = (thread_internal_t *)pvPortMalloc(sizeof(thread_internal_t));
	if (pxThread == NULL) {
		return -RTS_ISP_ENOMEM;
	}
	memset(pxThread, 0, sizeof(thread_internal_t));

	if (!isp_calloc_item(data)) {
		vPortFree(pxThread);
		return -RTS_ISP_ENOMEM;
	}
	data->start = start;
	data->arg = arg;

	xSemaphore = xSemaphoreCreateBinaryStatic(&pxThread->xSemaphore);
	if (xSemaphore != (SemaphoreHandle_t)&pxThread->xSemaphore) {
		vPortFree(pxThread);
		return -RTS_ISP_EFAULT;
	}

	xHandle = xTaskCreateStatic(
				  pvThreadRun, /* Function that implements the task. */
				  name, /* Text name for the task. */
				  STACK_SIZE, /* The number of indexes in the xStack array. */
				  data, /* Parameter passed into the task. */
				  uxPriority, /* Priority at which the task is created. */
				  pxThread->xStack, /* Array to use as the task's stack. */
				  &pxThread->xTaskBuffer); /* Variable to hold the task's data structure. */
	if (xHandle == (TaskHandle_t)&pxThread->xTaskBuffer) {
		*newthread = (isp_os_thread_t)pxThread;
	} else {
		vPortFree(pxThread);
		ret = -RTS_ISP_ENOMEM;
	}

	return ret;
}

void isp_os_thread_exit(void)
{
	thread_internal_t *pxThread =
		(thread_internal_t *)xTaskGetCurrentTaskHandle();

	xSemaphoreGive((SemaphoreHandle_t)&pxThread->xSemaphore);
	vTaskDelete(NULL);
}

isp_os_thread_t isp_os_thread_self(void)
{
	thread_internal_t *pxThread =
		(thread_internal_t *)xTaskGetCurrentTaskHandle();
	return (isp_os_thread_t)pxThread;
}

int isp_os_thread_join(isp_os_thread_t thread)
{
	thread_internal_t *pxThread = (thread_internal_t *)thread;

	if (!thread) {
		return -RTS_ISP_EINVAL;
	}

	if (xSemaphoreTake((SemaphoreHandle_t)&pxThread->xSemaphore,
					   portMAX_DELAY) != pdPASS) {
		return -RTS_ISP_EFAULT;
	}

	vSemaphoreDelete((SemaphoreHandle_t)&pxThread->xSemaphore);
	vPortFree(pxThread);

	return RTS_ISP_OK;
}

int isp_os_thread_set_rt(void)
{
	UBaseType_t uxPriority = configMAX_PRIORITIES - 1; /* FIXME */

	vTaskPrioritySet(NULL, uxPriority);

	return RTS_ISP_OK;
}

int isp_os_thread_mutex_init(isp_os_thread_mutex_t *mutex)
{
	SemaphoreHandle_t xSemaphore;
	StaticSemaphore_t *pxMutexBuffer = (StaticSemaphore_t *)mutex;

	configASSERT(sizeof(isp_os_thread_mutex_t) ==
				 sizeof(StaticSemaphore_t));

	if (sizeof(isp_os_thread_mutex_t) != sizeof(StaticSemaphore_t) ||
		!mutex) {
		return -RTS_ISP_EINVAL;
	}

	xSemaphore = xSemaphoreCreateMutexStatic(pxMutexBuffer);
	if (xSemaphore != (SemaphoreHandle_t)pxMutexBuffer) {
		return -RTS_ISP_EFAULT;
	}

	return RTS_ISP_OK;
}

int isp_os_thread_mutex_lock(isp_os_thread_mutex_t *mutex)
{
	if (!mutex) {
		return -RTS_ISP_EINVAL;
	}

	if (xSemaphoreTake((SemaphoreHandle_t)mutex, portMAX_DELAY) != pdPASS) {
		return -RTS_ISP_EFAULT;
	}

	return RTS_ISP_OK;
}

int isp_os_thread_mutex_trylock(isp_os_thread_mutex_t *mutex)
{
	if (!mutex) {
		return -RTS_ISP_EINVAL;
	}

	if (xSemaphoreTake((SemaphoreHandle_t)mutex, 0) != pdPASS) {
		return -RTS_ISP_EAGAIN;
	}

	return RTS_ISP_OK;
}

int isp_os_thread_mutex_unlock(isp_os_thread_mutex_t *mutex)
{
	if (!mutex) {
		return -RTS_ISP_EINVAL;
	}

	if (xSemaphoreGive((SemaphoreHandle_t)mutex) != pdTRUE) {
		return -RTS_ISP_EFAULT;
	}

	return RTS_ISP_OK;
}

int isp_os_thread_mutex_destroy(isp_os_thread_mutex_t *mutex)
{
	if (!mutex) {
		return -RTS_ISP_EINVAL;
	}

	vSemaphoreDelete((SemaphoreHandle_t)mutex);

	return RTS_ISP_OK;
}

int isp_os_thread_cond_init(isp_os_thread_cond_t *cond)
{
	StaticSemaphore_t *pxSemaphoreBuffer = (StaticSemaphore_t *)cond;
	SemaphoreHandle_t xSemaphore;

	configASSERT(sizeof(isp_os_thread_cond_t) == sizeof(StaticSemaphore_t));
	if (sizeof(isp_os_thread_cond_t) != sizeof(StaticSemaphore_t) || !cond) {
		return -RTS_ISP_EINVAL;
	}

	xSemaphore =
		xSemaphoreCreateCountingStatic(1024, 0U, pxSemaphoreBuffer);
	if (xSemaphore != (SemaphoreHandle_t)pxSemaphoreBuffer) {
		return -RTS_ISP_EFAULT;
	}

	return RTS_ISP_OK;
}

int isp_os_thread_cond_wait(isp_os_thread_cond_t *cond,
							isp_os_thread_mutex_t *mutex)
{
	int ret = RTS_ISP_OK;

	if (!cond || !mutex) {
		return -RTS_ISP_EINVAL;
	}

	ret = isp_os_thread_mutex_unlock(mutex);
	if (ret) {
		return ret;
	}

	if (xSemaphoreTake((SemaphoreHandle_t)cond, portMAX_DELAY) != pdPASS) {
		isp_os_thread_mutex_lock(mutex);
		ret = -RTS_ISP_EFAULT;
	} else {
		ret = isp_os_thread_mutex_lock(mutex);
	}

	return ret;
}

int isp_os_thread_cond_signal(isp_os_thread_cond_t *cond)
{
	if (!cond) {
		return -RTS_ISP_EINVAL;
	}
	if (xSemaphoreGive((SemaphoreHandle_t)cond) != pdTRUE) {
		return -RTS_ISP_EFAULT;
	}
	return RTS_ISP_OK;
}

int isp_os_thread_cond_destroy(isp_os_thread_cond_t *cond)
{
	if (!cond) {
		return -RTS_ISP_EINVAL;
	}
	vSemaphoreDelete((SemaphoreHandle_t)cond);
	return RTS_ISP_OK;
}
