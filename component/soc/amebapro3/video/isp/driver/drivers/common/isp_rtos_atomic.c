#include "isp_rtos_atomic.h"

void isp_rtos_atomic_set(atomic_t *atomic, int val)
{
	__atomic_store_n(atomic, val, __ATOMIC_RELAXED);
}

int isp_rtos_atomic_read(atomic_t *atomic)
{
	return __atomic_load_n(atomic, __ATOMIC_RELAXED);
}

void isp_rtos_atomic_inc(atomic_t *atomic)
{
	__atomic_add_fetch(atomic, 1, __ATOMIC_RELAXED);
}

int isp_rtos_atomic_inc_return(atomic_t *atomic)
{
	return __atomic_add_fetch(atomic, 1, __ATOMIC_RELAXED);
}

void isp_rtos_atomic_dec(atomic_t *atomic)
{
	__atomic_sub_fetch(atomic, 1, __ATOMIC_RELAXED);
}

int isp_rtos_atomic_dec_return(atomic_t *atomic)
{
	return __atomic_sub_fetch(atomic, 1, __ATOMIC_RELAXED);
}
