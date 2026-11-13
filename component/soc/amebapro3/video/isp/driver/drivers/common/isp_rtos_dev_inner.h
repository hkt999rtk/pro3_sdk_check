#ifndef _ISP_RTOS_DEV_INNER_H_
#define _ISP_RTOS_DEV_INNER_H_

#include <stdbool.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "isp_rtos_epoll.h"
#include "isp_rtos_dev.h"
#include "isp_rtos_wait_queue.h"

struct isp_rtos_file {
	/* inner used */
	int node;
	/* save driver data */
	void *private_data;
};

/* Interface
 * if need, please make implementation
 */
struct isp_rtos_file_operations {
	unsigned int (*poll)(struct isp_rtos_file *);
	long (*ioctl)(struct isp_rtos_file *, unsigned int, void *);
	int (*open)(struct isp_rtos_file *);
	int (*release)(struct isp_rtos_file *);
};

struct isp_rtos_device {
	/* device name */
	char name[32];
	/* inner used */
	unsigned long flags;
	/* inner used */
	struct isp_rtos_file *filp;
	/* interface */
	struct isp_rtos_file_operations *fops;
	/* if need, please make implementation or used isp_rtos_device_release*/
	void (*release)(struct isp_rtos_device *rdev);
	/* Driver data, set and get with isp_rtos_dev_set/get_drvdata */
	void *driver_data;
};

/* alloc isp_rtos_device */
struct isp_rtos_device *isp_rtos_device_alloc(void);
/* free isp_rtos_device */
void isp_rtos_device_release(struct isp_rtos_device *rdev);
/* get isp_rtos_device from isp_rtos_file */
struct isp_rtos_device *isp_rtos_devdata(struct isp_rtos_file *filp);
/* get driver data from isp_rtos_device */
void *isp_rtos_dev_get_drvdata(const struct isp_rtos_device *rdev);
/* set driver data in isp_rtos_device */
void isp_rtos_dev_set_drvdata(struct isp_rtos_device *rdev, void *data);
/* register isp_rtos_device */
int isp_rtos_register_device(struct isp_rtos_device *rdev);
/* unregister isp_rtos_device */
void isp_rtos_unregister_device(struct isp_rtos_device *rdev);
/* poll wait */
void isp_rtos_poll_wait(struct isp_rtos_file *filp,
						isp_rtos_wait_queue_head_t *wq);
/* use for mcu driver init */
int isp_rtos_device_init(void);

#endif /* _ISP_RTOS_DEV_INNER_H_ */
