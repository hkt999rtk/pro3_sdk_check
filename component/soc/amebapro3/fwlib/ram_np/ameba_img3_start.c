/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ameba_soc.h"
#include "ameba_secure_boot.h"


/* To Run Init before regular image3 flow
 *
 * Regular Image3 Entry is behind this Entry
 */
extern u8 __image3_real_entry_func__[];
extern u8 __rom_bss_start__[];
extern u8 __rom_bss_end__[];

void STARTUP_IMG3(void)
{
	RAM_START_FUNCTION *entry = (RAM_START_FUNCTION *)__image3_real_entry_func__;

	_memset((void *) __rom_bss_start__, 0, (__rom_bss_end__ - __rom_bss_start__));

	fih_cfi_init();

	/* to initial ROM code using global variable */
	if (SYSCFG_OTP_DisBootLog() == FALSE) {
		/* Set log level for diaplaying */
		rtk_log_level_set("*", RTK_LOG_INFO);
	} else {
		rtk_log_level_set("*", RTK_LOG_ERROR);
	}

	entry->RamStartFun();
}

void WAKEUP_IMG3(void)
{
	RAM_START_FUNCTION *entry = (RAM_START_FUNCTION *)__image3_real_entry_func__;

	/* insert customer resume flow before regular flow */

	entry->RamWakeupFun();
}

IMAGE3_BOOT_SECTION
RAM_START_FUNCTION Img3PreInitEntry = {
	.RamStartFun = STARTUP_IMG3,
	.RamWakeupFun = WAKEUP_IMG3,
};
