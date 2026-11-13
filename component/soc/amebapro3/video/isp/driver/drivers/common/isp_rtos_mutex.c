#include <errno.h>
#include "isp_rtos_mutex.h"

int isp_rtos_mutex_init(isp_rtos_mutex_t *mutex)
{
	SemaphoreHandle_t xSemaphore;
	StaticSemaphore_t *pxMutexBuffer = mutex;

	if (!mutex) {
		return -EINVAL;
	}

	xSemaphore = xSemaphoreCreateMutexStatic(pxMutexBuffer);
	if (xSemaphore != (SemaphoreHandle_t)pxMutexBuffer) {
		return -EFAULT;
	}
	return 0;
}

int isp_rtos_mutex_lock(isp_rtos_mutex_t *mutex)
{
	BaseType_t xHigherPriorityTaskWoken;

	if (!mutex) {
		return -EINVAL;
	}

	if (xPortIsInsideInterrupt()) {
		xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreTakeFromISR((SemaphoreHandle_t)mutex,
							  &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	} else {
		if (xSemaphoreTake((SemaphoreHandle_t)mutex, portMAX_DELAY) !=
			pdPASS) {
			return -EFAULT;
		}
	}

	return 0;
}

int isp_rtos_mutex_unlock(isp_rtos_mutex_t *mutex)
{
	BaseType_t xHigherPriorityTaskWoken;

	if (!mutex) {
		return -EINVAL;
	}

	if (xPortIsInsideInterrupt()) {
		xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR((SemaphoreHandle_t)mutex,
							  &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	} else {
		if (xSemaphoreGive((SemaphoreHandle_t)mutex) != pdTRUE) {
			return -EFAULT;
		}
	}

	return 0;
}
