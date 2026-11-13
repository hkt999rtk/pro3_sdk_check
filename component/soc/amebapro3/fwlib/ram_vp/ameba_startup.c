#include <string.h>
#include <stdint.h>
#include <ameba.h>
#include <platform_autoconf.h>

extern void stage1_data_init(void);
extern void stage1_bss_init(void);

__weak int main(void)
{
	while (1);
	return 0;
}

__weak void _init(void)
{
	return;
}

void mpu_setup(void)
{
	mpu_region_config device_config = {
		.xn = MPU_EXEC_NEVER,
		.ap = MPU_UN_PRIV_RW,
		.sh = MPU_OUT_SHAREABLE,
		.attr_idx = MPU_MEM_ATTR_IDX_NC, //MPU_MEM_ATTR_IDX_DEVICE,
	};

	int rn = 0;

	mpu_init();
	device_config.region_base = 0x60000000;
	device_config.region_size = 0x6F000000 - 0x60000000;
	rn = mpu_entry_alloc();
	mpu_region_cfg(rn, &device_config);

	device_config.region_base = 0x6F100000;		// 0x6f100000 ai-denoise control base
	device_config.region_size = 0x7F000000 - 0x6F100000;
	rn = mpu_entry_alloc();
	mpu_region_cfg(rn, &device_config);

	device_config.region_base = 0x29000000;
	device_config.region_size = 0x30000000 - 0x29000000;
	rn = mpu_entry_alloc();
	mpu_region_cfg(rn, &device_config);

	device_config.region_base = 0x39000000;
	device_config.region_size = 0x40000000 - 0x39000000;
	rn = mpu_entry_alloc();
	mpu_region_cfg(rn, &device_config);
}

void startup(void)
{

	/* Cache enable */
	ICache_Enable();
	DCache_Enable();

	stage1_bss_init();
	stage1_data_init();

	/* to initial ROM code using global variable */
	if (SYSCFG_OTP_DisBootLog() == FALSE) {
		/* Set log level for diaplaying */
		rtk_log_level_set("*", RTK_LOG_INFO);
	} else {
		rtk_log_level_set("*", RTK_LOG_ERROR);
	}

	/* MPU setup */
	mpu_setup();

	/* enable DIV_0 trap */
	SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;

	/* update core clock frequency */
	SystemCoreClockUpdate();

	/* update delay api */
	RTK_LOGI("VP", "Clock Speed %dMhz\n", CPU_ClkGet() / 1000000);
	DelayClkUpdate(CPU_ClkGet());
	DelayDivUpdate(5);

#if defined (__GNUC__)
	extern void __libc_init_array(void);
	/* Add This for C++ support */
	__libc_init_array();
#endif

	main();
}
