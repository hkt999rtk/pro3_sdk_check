#ifndef _ISP_RTOS_COMPLETION_H_
#define _ISP_RTOS_COMPLETION_H_

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

typedef StaticSemaphore_t isp_rtos_completion_t;

int isp_rtos_init_completion(isp_rtos_completion_t *completion);
int isp_rtos_reinit_completion(isp_rtos_completion_t *completion);
void isp_rtos_wait_for_completion(isp_rtos_completion_t *completion);
void isp_rtos_complete(isp_rtos_completion_t *completion);
int isp_rtos_wait_for_completion_timeout(isp_rtos_completion_t *completion,
		uint32_t ticks);

#endif /* _ISP_RTOS_COMPLETION_H_ */
