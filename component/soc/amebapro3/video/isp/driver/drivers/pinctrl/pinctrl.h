#ifndef _PINCTRL_H_
#define _PINCTRL_H_

typedef void *rtscam_rtos_pinctrl_t;

rtscam_rtos_pinctrl_t rtscam_rtos_pinctrl_lookup_state(const char *id);
int rtscam_rtos_pinctrl_select_state(rtscam_rtos_pinctrl_t state);
#endif /* _PINCTRL_H_ */
