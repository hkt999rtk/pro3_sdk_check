#include <errno.h>
#include <isp_rtos_dev_inner.h>
#include "pipeline/mcu/rtscam_isp_mcu.h"
#include "rtscam_isp_pipeline.h"
#include "isp_driver.h"

int isp_mcu_driver_init(void)
{
	int ret;

	isp_rtos_device_init();
	ret = rtscam_isp_probe();
	if (ret) {
		return -ENODEV;
	}
	return ret;
}

void isp_mcu_driver_cleanup(void)
{
	rtscam_isp_remove();
}
