#ifndef _WAIT_QUEUE_H_
#define _WAIT_QUEUE_H_

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "isp_rtos_spinlock.h"

struct isp_rtos_wait_queue_head {
	isp_rtos_spinlock_t lock;
	TaskHandle_t task_handle;
};
typedef struct isp_rtos_wait_queue_head isp_rtos_wait_queue_head_t;

void isp_rtos_wake_up(isp_rtos_wait_queue_head_t *wq);
void isp_rtos_add_wait_queue(isp_rtos_wait_queue_head_t *wq,
							 TaskHandle_t task_handle);
void isp_rtos_remove_wait_queue(isp_rtos_wait_queue_head_t *wq,
								TaskHandle_t task_handle);
#endif /* _WAIT_QUEUE_H_ */
