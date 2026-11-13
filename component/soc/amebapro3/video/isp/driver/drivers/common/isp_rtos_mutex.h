#ifndef _ISP_RTOS_MUTEX_H_
#define _ISP_RTOS_MUTEX_H_

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

typedef StaticSemaphore_t isp_rtos_mutex_t;
int isp_rtos_mutex_init(isp_rtos_mutex_t *mutex);
int isp_rtos_mutex_lock(isp_rtos_mutex_t *mutex);
int isp_rtos_mutex_unlock(isp_rtos_mutex_t *mutex);

#endif /* _ISP_RTOS_MUTEX_H_ */
