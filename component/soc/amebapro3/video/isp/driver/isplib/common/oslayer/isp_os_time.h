#ifndef _ISP_OS_TIME_H_INC_
#define _ISP_OS_TIME_H_INC_

#include <stdint.h>

uint64_t isp_os_time_get_us(void);
uint64_t isp_os_time_get_ms(void);
void isp_os_usleep(uint32_t us);
void isp_os_msleep(uint32_t ms);

#endif /* _ISP_OS_TIME_H_INC_ */
