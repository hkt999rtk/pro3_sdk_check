/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

extern HAL_VECTOR_FUN VectorTable[];
//static const char *const TAG = "APP";

__weak void SOCPS_WakeFromPG_NP(void)
{

}

__weak int main(void)
{
	while (1);
	return 0;
}

#if defined (__GNUC__)
/* Add This for C++ support to avoid compile error */
void _init(void) {}
#endif

void bss_init(void)
{
	extern u8 __bss_start__[];
	extern u8 __bss_end__[];
	if ((size_t)(__bss_end__ - __bss_start__) > 0) {
		memset(__bss_start__, 0, (size_t)(__bss_end__ - __bss_start__));
	}

	extern u8 __sram_bss_start__[];
	extern u8 __sram_bss_end__[];
	if ((size_t)(__sram_bss_end__ - __sram_bss_start__) > 0) {
		memset(__sram_bss_start__, 0, (size_t)(__sram_bss_end__ - __sram_bss_start__));
	}
}

void os_init(void)
{
	//rtos_mem_init();
#if defined(CONFIG_PLATFORM_FREERTOS) && (CONFIG_PLATFORM_FREERTOS==1)
	extern void irq_init(void);
	extern void SVC_Handler(void);
	extern void PendSV_Handler(void);
	extern void SysTick_Handler(void);

	irq_init();
	__NVIC_SetVector(SVCall_IRQn, (u32)(void *)SVC_Handler);
	__NVIC_SetVector(PendSV_IRQn, (u32)(void *)PendSV_Handler);
	__NVIC_SetVector(SysTick_IRQn, (u32)(void *)SysTick_Handler);
#endif
}

// The Main App entry point
void app_start(void)
{
	bss_init();

	DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
	ICache_Invalidate();

	SystemCoreClockUpdate();
	RTK_LOGI("NP", "Clock Speed %dMhz\n", CPU_ClkGet() / 1000000);
	DelayClkUpdate(CPU_ClkGet());
	DelayDivUpdate(5);

	/* to initial ROM code using global variable */
	if (SYSCFG_OTP_DisBootLog() == FALSE) {
		/* Set log level for diaplaying */
		rtk_log_level_set("*", RTK_LOG_INFO);
	} else {
		rtk_log_level_set("*", RTK_LOG_ERROR);
	}

	/* configure FreeRTOS interrupt and heap region for printf(RTK_LOGx)*/
	os_init();

	/* low power pin dont need pinmap init again after wake from dslp */
	//pinmap_init(); /* 1.7ms */

#if defined (__GNUC__)
	extern void __libc_init_array(void);
	/* Add This for C++ support */
	__libc_init_array();
#endif

	/* NP can not assign non-cache region which is less than 512M */
	main();
}

IMAGE2_ENTRY_SECTION
RAM_START_FUNCTION Img2EntryFun0 = {
	app_start,
	SOCPS_WakeFromPG_NP,
	(u32)VectorTable,
};
