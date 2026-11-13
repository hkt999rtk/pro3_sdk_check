#ifndef __ISP_RTOS_SPINLOCK_H
#define __ISP_RTOS_SPINLOCK_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

/*
 * Backed by taskENTER_CRITICAL()/taskENTER_CRITICAL_FROM_ISR() rather than a
 * FreeRTOS mutex: a real mutex cannot be validly given/taken from interrupt
 * context (it trips configASSERT in xQueueGiveFromISR() the moment a task
 * holds it while an ISR calls unlock — see rts_camera_soc.c's frame-done ISR
 * racing rtscam_buffer_timer_cb()). This lock is used from both task and ISR
 * context, so it needs to be genuinely interrupt-safe.
 */
typedef struct {
	UBaseType_t saved_interrupt_state;
} isp_rtos_spinlock_t;

int isp_rtos_spin_lock_init(isp_rtos_spinlock_t *lock);
int isp_rtos_spin_lock(isp_rtos_spinlock_t *lock);
int isp_rtos_spin_unlock(isp_rtos_spinlock_t *lock);
void isp_rtos_spin_lock_irqsave(isp_rtos_spinlock_t *lock, unsigned long flags);
void isp_rtos_spin_unlock_irqrestore(isp_rtos_spinlock_t *lock,
									 unsigned long flags);

#endif /* __ISP_RTOS_SPINLOCK_H */
