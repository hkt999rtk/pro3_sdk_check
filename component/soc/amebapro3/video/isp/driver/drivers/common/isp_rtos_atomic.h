#ifndef _ISP_RTOS_ATOMIC_H
#define _ISP_RTOS_ATOMIC_H

typedef int atomic_t;

void isp_rtos_atomic_set(atomic_t *atomic, int val);
int isp_rtos_atomic_read(atomic_t *atomic);
void isp_rtos_atomic_inc(atomic_t *atomic);
int isp_rtos_atomic_inc_return(atomic_t *atomic);
void isp_rtos_atomic_dec(atomic_t *atomic);
int isp_rtos_atomic_dec_return(atomic_t *atomic);
#endif /* _ISP_RTOS_ATOMIC_H */
