/*
 * Copyright (c) 2013-2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include <drivers/console.h>

#define OTPC_REG_BASE            	  0x40230600        /* OTP LOGCIAL autoload base */
#define SEC_OTP_SYSCFG0               0x00				/* ROM BOOT CONFIG0 */
#define SEC_BIT_DIS_BOOT_LOG_EN       ((uint32_t)0x00000001 << 18)           /*!< R/W 0x0  Boot log disable*/

#define CHK_BOOT_LOG_DIS() (*(uint32_t *)(OTPC_REG_BASE + SEC_OTP_SYSCFG0) & SEC_BIT_DIS_BOOT_LOG_EN)

static uint32_t put_char_enable = 0xFF;

int putchar(int c)
{
	/* actually for in CHIP_TYPE_SIM, do not print */
	if ((put_char_enable == 0xFF) && CHK_BOOT_LOG_DIS() ) {
		put_char_enable = 0;
	} else {
		put_char_enable = 1;
	}

	if (put_char_enable == 0) {
		return c;
	}

	int res;
	if (console_putc((unsigned char)c) >= 0)
		res = c;
	else
		res = EOF;

	return res;
}
