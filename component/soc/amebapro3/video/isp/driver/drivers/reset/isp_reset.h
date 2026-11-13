#ifndef _RESET_H_
#define _RESET_H_

typedef void *rtscam_rtos_reset_t;

rtscam_rtos_reset_t rtscam_rtos_reset_control_get(const char *id);
void rtscam_rtos_reset_control_put(rtscam_rtos_reset_t reset);
int rtscam_rtos_reset_control_reset(rtscam_rtos_reset_t reset);
int rtscam_rtos_reset_control_assert(rtscam_rtos_reset_t reset);
int rtscam_rtos_reset_control_deassert(rtscam_rtos_reset_t reset);
#endif /* _RESET_H_ */
