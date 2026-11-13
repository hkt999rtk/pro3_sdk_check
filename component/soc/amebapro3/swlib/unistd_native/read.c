/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * attached _read to native devices
 */

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <basic_types.h>
#include <ameba_soc.h>

int
_read(int   file,
	  char *ptr,
	  int   len)
{
	u8 *msg = (u8 *)ptr;
	size_t recv_len = 0;
	if( len < 0) {
		return -1;
	}
	if (file == 0) {	// stdin
		int max_cnt = 1000;
		while (max_cnt-- && (recv_len < (size_t)len)) {
			if (LOGUART_Readable()) {
				u8 c = LOGUART_GetChar(FALSE);
				*msg++ = c;
				recv_len++;
			}
		}
	} else if (file >= 3) {
		/* redirect to file system */
	}
	return recv_len;
}

