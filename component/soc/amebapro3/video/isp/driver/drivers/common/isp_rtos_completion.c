#include <errno.h>
#include "isp_rtos_completion.h"

int isp_rtos_init_completion(isp_rtos_completion_t *completion)
{
	SemaphoreHandle_t xSemaphore;
	StaticSemaphore_t *xSemaphoreBuffer = completion;

	if (!completion) {
		return -EINVAL;
	}

	xSemaphore = xSemaphoreCreateBinaryStatic(xSemaphoreBuffer);
	if (xSemaphore != (SemaphoreHandle_t)xSemaphoreBuffer) {
		return -EFAULT;
	}
	return 0;
}

int isp_rtos_reinit_completion(isp_rtos_completion_t *completion)
{
	if (!completion) {
		return -EINVAL;
	}

	xSemaphoreTake((SemaphoreHandle_t)completion, 0);
	return 0;
}

void isp_rtos_wait_for_completion(isp_rtos_completion_t *completion)
{
	if (!completion) {
		return;
	}
	xSemaphoreTake((SemaphoreHandle_t)completion, portMAX_DELAY);
}

void isp_rtos_complete(isp_rtos_completion_t *completion)
{
	BaseType_t xHigherPriorityTaskWoken;

	if (!completion) {
		return;
	}

	if (xPortIsInsideInterrupt()) {
		xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR((SemaphoreHandle_t)completion,
							  &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	} else {
		xSemaphoreGive((SemaphoreHandle_t)completion);
	}
}

int isp_rtos_wait_for_completion_timeout(isp_rtos_completion_t *completion,
		uint32_t ticks)
{
	int ret;

	if (!completion) {
		return -EINVAL;
	}
	ret = xSemaphoreTake((SemaphoreHandle_t)completion, ticks);
	if (ret == pdTRUE) {
		return 0;
	} else {
		return -ETIMEDOUT;
	}
}
