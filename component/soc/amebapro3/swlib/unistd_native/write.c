/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * attached _write to native devices
 */

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <basic_types.h>
#include <ameba_soc.h>

int
_write(int   file,
	   char *ptr,
	   int   len)
{

	u8 *msg = (u8 *)ptr;
	size_t rest = len;

	if (file == 1 || file == 2) {	// stdout and stderr
		while (rest--) {
			LOGUART_PutChar(*msg++);
		}
	} else if (file >= 3) {
		/* redirect to file system */
	}
	return len;
}
