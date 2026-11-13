#ifndef _ISP_OS_THREAD_H_INC_
#define _ISP_OS_THREAD_H_INC_

#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

typedef unsigned long int isp_os_thread_t;
typedef StaticSemaphore_t isp_os_thread_mutex_t;
typedef StaticSemaphore_t isp_os_thread_cond_t;

enum isp_os_thread_priorities {
	ISP_OS_THREAD_PRIORITY_LOWER = -1,
	ISP_OS_THREAD_PRIORITY_MID = 0,
	ISP_OS_THREAD_PRIORITY_HIGHER = 1,
};

int isp_os_thread_create(isp_os_thread_t *newthread, const char *const name,
						 int priority, void (*start)(void *), void *arg);
void isp_os_thread_exit(void);
isp_os_thread_t isp_os_thread_self(void);
int isp_os_thread_join(isp_os_thread_t thread);
int isp_os_thread_set_rt(void);

int isp_os_thread_mutex_init(isp_os_thread_mutex_t *mutex);
int isp_os_thread_mutex_lock(isp_os_thread_mutex_t *mutex);
int isp_os_thread_mutex_trylock(isp_os_thread_mutex_t *mutex);
int isp_os_thread_mutex_unlock(isp_os_thread_mutex_t *mutex);
int isp_os_thread_mutex_destroy(isp_os_thread_mutex_t *mutex);

int isp_os_thread_cond_init(isp_os_thread_cond_t *cond);
int isp_os_thread_cond_wait(isp_os_thread_cond_t *cond,
							isp_os_thread_mutex_t *mutex);
int isp_os_thread_cond_signal(isp_os_thread_cond_t *cond);
int isp_os_thread_cond_destroy(isp_os_thread_cond_t *cond);

#endif /* _ISP_OS_THREAD_H_INC_ */
