#include <string.h>
#include <stdint.h>
#include <ameba.h>
#include <platform_autoconf.h>

extern void SOCPS_WakeFromPG(void);

extern uint8_t __bss_start__[];
extern uint8_t __bss_end__[];
extern HAL_VECTOR_FUN VectorTable[];

static volatile u32 boot_reason = 0;

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

	// leave NS device

	mpu_init();
	device_config.region_base = 0x60000000;
	device_config.region_size = 0x6F000000 - 0x60000000;
	rn = mpu_entry_alloc();
	mpu_region_cfg(rn, &device_config);

	device_config.region_base = 0x29000000;
	device_config.region_size = 0x30000000 - 0x29000000;
	rn = mpu_entry_alloc();
	mpu_region_cfg(rn, &device_config);

}

void startup(void)
{
	AON_NS_TypeDef *AON_NS = (AON_NS_TypeDef *)AON_NS_REG_BASE;
	boot_reason = AON_NS->AON_FW_BOOT_REASON;

	/* Check boot or wake up from power saving */
	if (boot_reason == 0x8) {
		//AONWDG_Enable(1);
		/* Cache enable */
		ICache_Enable();
		DCache_Enable();
		/* enable DIV_0 trap */
		SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;
		SCB->VTOR = (uint32_t)VectorTable;
		SOCPS_WakeFromPG();
	}


	/* Cache enable */
	ICache_Enable();
	DCache_Enable();

	memset(__bss_start__, 0, __bss_end__ - __bss_start__);

	/* to initial ROM code using global variable */
	if (SYSCFG_OTP_DisBootLog() == FALSE) {
		/* Set log level for diaplaying */
		rtk_log_level_set("*", RTK_LOG_INFO);
	} else {
		rtk_log_level_set("*", RTK_LOG_ERROR);
	}

	/* mpu setup */
	mpu_setup();

	/* irq errata */
	irq_errata_disable_core();
	irq_errata_clear_pending_core();

#if 0
	//TODO: should remove in ASIC, boot reason debug check
	DiagPrintf("fp boot reason = 0x%X\r\n", boot_reason);

	boot_reason = AON_NS->AON_FW_BOOT_REASON;

	DiagPrintf("fp boot reason late = 0x%X\r\n", boot_reason);
#endif

	/* enable DIV_0 trap */
	SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;

	/* update core clock frequency */
	SystemCoreClockUpdate();

	/* update delay api */
	RTK_LOGI("FP", "Clock Speed %dMhz\n", CPU_ClkGet() / 1000000);
	DelayClkUpdate(CPU_ClkGet());

#if defined (__GNUC__)
	extern void __libc_init_array(void);
	/* Add This for C++ support */
	__libc_init_array();
#endif

	main();
}
