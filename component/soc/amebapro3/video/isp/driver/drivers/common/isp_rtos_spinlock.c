#include <errno.h>
#include "isp_rtos_spinlock.h"

int isp_rtos_spin_lock_init(isp_rtos_spinlock_t *lock)
{
	if (!lock) {
		return -EINVAL;
	}

	lock->saved_interrupt_state = 0;
	return 0;
}

int isp_rtos_spin_lock(isp_rtos_spinlock_t *lock)
{
	if (!lock) {
		return -EINVAL;
	}

	if (xPortIsInsideInterrupt()) {
		lock->saved_interrupt_state = taskENTER_CRITICAL_FROM_ISR();
	} else {
		taskENTER_CRITICAL();
	}

	return 0;
}

int isp_rtos_spin_unlock(isp_rtos_spinlock_t *lock)
{
	if (!lock) {
		return -EINVAL;
	}

	if (xPortIsInsideInterrupt()) {
		taskEXIT_CRITICAL_FROM_ISR(lock->saved_interrupt_state);
	} else {
		taskEXIT_CRITICAL();
	}

	return 0;
}

/*
 * "flags" is kept for source compatibility with existing callers but unused:
 * the saved interrupt state lives in *lock itself (safe, since by definition
 * only the current holder touches it between lock() and unlock()).
 */
void isp_rtos_spin_lock_irqsave(isp_rtos_spinlock_t *lock, unsigned long flags)
{
	(void)flags;
	isp_rtos_spin_lock(lock);
}

void isp_rtos_spin_unlock_irqrestore(isp_rtos_spinlock_t *lock,
									 unsigned long flags)
{
	(void)flags;
	isp_rtos_spin_unlock(lock);
}
