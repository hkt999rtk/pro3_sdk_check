#include <platform_autoconf.h>

#if !defined(SEGGER_SEMIHOST)
#if defined(CONFIG_UNISTD_SEMIHOSTING) && (CONFIG_UNISTD_SEMIHOSTING==1)
#define SEGGER_SEMIHOST 1
#endif
#endif

#if defined(SEGGER_SEMIHOST) && (SEGGER_SEMIHOST==1)
#include <SEGGER_SEMIHOST.h>
#include <ameba_soc.h>
#include <stdio.h>
#include <stdint.h>

int _write(int fd, char *ptr, int len)
{
	if (fd == 1 /*stdout*/ || fd == 2 /*stderr*/) {
		extern void LOGUART_PutChar(uint8_t c);
		int ena = DCache_IsEnabled();
		if (ena) {
			DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
			DCache_Disable();
		}
		for (int i = 0; i < len; i++) {
			LOGUART_PutChar(ptr[i]);
		}
		if (ena) {
			DCache_Enable();
		}
		return 0;
	}

	int ena = DCache_IsEnabled();
	if (ena) {
		DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
		DCache_Disable();
	}
	int r = SEGGER_SEMIHOST_Write(fd, ptr, len);
	if (ena) {
		DCache_Enable();
	}

	return len - r;
}

#endif

