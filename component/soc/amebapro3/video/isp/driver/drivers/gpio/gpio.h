#ifndef _GPIO_H_
#define _GPIO_H_

typedef void *rtscam_rtos_gpiod_t;

rtscam_rtos_gpiod_t rtscam_rtos_gpiod_get(const char *id);
void rtscam_rtos_gpiod_put(rtscam_rtos_gpiod_t gpio);
void rtscam_rtos_gpiod_direction_output(rtscam_rtos_gpiod_t gpio, int value);
#endif /* _GPIO_H_ */
