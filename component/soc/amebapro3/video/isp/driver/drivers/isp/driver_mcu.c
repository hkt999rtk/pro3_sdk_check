#include "isp_driver.h"
#include "diag.h"

extern int isp_clk_init(void);
extern int reset_init(void);
extern int pinctrl_state_init(void);
extern int gpio_desc_init(void);
extern int rtscam_rtos_i2c_init(void);
extern int rtscam_rtos_i2c_cleanup(void);

int driver_init(void)
{
	DiagPrintf("CLK init..\n");
	isp_clk_init();
	DiagPrintf("reset init..\n");
	reset_init();
	DiagPrintf("pinctrl init..\n");
	pinctrl_state_init();
	DiagPrintf("gpio init..\n");
	gpio_desc_init();
	DiagPrintf("i2c init..\n");
	rtscam_rtos_i2c_init();
	DiagPrintf("driver_init done\n");
	return 0;
}

void driver_cleanup(void)
{
	rtscam_rtos_i2c_cleanup();
}
