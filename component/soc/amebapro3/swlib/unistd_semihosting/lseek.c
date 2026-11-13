#include <diag.h>
#include <stdio.h>

#include <platform_autoconf.h>

#if !defined(SEGGER_SEMIHOST)
#if defined(CONFIG_UNISTD_SEMIHOSTING) && (CONFIG_UNISTD_SEMIHOSTING==1)
#define SEGGER_SEMIHOST 1
#endif
#endif


#if defined(SEGGER_SEMIHOST) && (SEGGER_SEMIHOST==1)
#include <SEGGER_SEMIHOST.h>
#include <ameba_soc.h>

int _lseek(int fd, int offset, int whence)
{
	if (whence == SEEK_CUR && offset == 0) {
		return 0;
	}

	int ena = DCache_IsEnabled();
	if (ena) {
		DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
		DCache_Disable();
	}

	if (whence == SEEK_END) {
		int flen = SEGGER_SEMIHOST_FLen(fd);
		if (flen >= -1) {
			whence = SEEK_SET;
			offset += flen;
		}
	}

	if (whence != SEEK_SET || fd < 0) {
		if (ena) {
			DCache_Enable();
		}
		return -1;
	}

	int r = SEGGER_SEMIHOST_Seek(fd, offset);
	if (ena) {
		DCache_Enable();
	}
	if (r == 0) {
		return offset;
	}

	return -1;
}

#endif
