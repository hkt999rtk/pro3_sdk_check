#ifndef _ISP_RTOS_BARRIER_H_
#define _ISP_RTOS_BARRIER_H_

#define dsb(option) __asm__ __volatile__("dsb " #option : : : "memory")
#define rmb() dsb()
#define wmb() dsb(st)

#ifndef barrier
#define barrier() __asm__ __volatile__("" : : : "memory")
#endif

#endif /* _ISP_RTOS_BARRIER_H_ */
