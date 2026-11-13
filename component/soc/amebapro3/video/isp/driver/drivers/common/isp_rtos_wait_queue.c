#include "isp_rtos_wait_queue.h"

void isp_rtos_wake_up(isp_rtos_wait_queue_head_t *wq)
{
	BaseType_t xHigherPriorityTaskWoken;
	unsigned long flags = 0;

	if (!wq) {
		return;
	}
	if (xPortIsInsideInterrupt()) {
		isp_rtos_spin_lock(&wq->lock);
		if (!wq->task_handle) {
			isp_rtos_spin_unlock(&wq->lock);
			return;
		}
		xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR(wq->task_handle,
							   &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		isp_rtos_spin_unlock(&wq->lock);
	} else {
		/* FIXME: FreeRTOS API functions must not be called from within a critical section.*/
		isp_rtos_spin_lock_irqsave(&wq->lock, flags);
		if (!wq->task_handle) {
			isp_rtos_spin_unlock_irqrestore(&wq->lock, flags);
			return;
		}
		xTaskNotifyGive(wq->task_handle);
		isp_rtos_spin_unlock_irqrestore(&wq->lock, flags);
	}
}

void isp_rtos_add_wait_queue(isp_rtos_wait_queue_head_t *wq,
							 TaskHandle_t task_handle)
{
	unsigned long flags = 0;

	if (!wq) {
		return;
	}
	isp_rtos_spin_lock_irqsave(&wq->lock, flags);
	wq->task_handle = task_handle;
	isp_rtos_spin_unlock_irqrestore(&wq->lock, flags);
}

void isp_rtos_remove_wait_queue(isp_rtos_wait_queue_head_t *wq,
								TaskHandle_t task_handle)
{
	unsigned long flags = 0;

	if (!wq) {
		return;
	}
	isp_rtos_spin_lock_irqsave(&wq->lock, flags);
	wq->task_handle = NULL;
	isp_rtos_spin_unlock_irqrestore(&wq->lock, flags);
}
