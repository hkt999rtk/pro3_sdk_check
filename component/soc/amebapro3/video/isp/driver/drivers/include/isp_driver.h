#ifndef _ISP_DRIVER_H_
#define _ISP_DRIVER_H_

int driver_init(void);
void driver_cleanup(void);
int isp_mcu_driver_init(void);
void isp_mcu_driver_cleanup(void);

#endif /* _ISP_DRIVER_H_ */
