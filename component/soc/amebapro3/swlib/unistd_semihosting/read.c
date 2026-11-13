
#include <platform_autoconf.h>

#if !defined(SEGGER_SEMIHOST)
#if defined(CONFIG_UNISTD_SEMIHOSTING) && (CONFIG_UNISTD_SEMIHOSTING==1)
#define SEGGER_SEMIHOST 1
#endif
#endif

#if defined(SEGGER_SEMIHOST) && (SEGGER_SEMIHOST==1)
#include <SEGGER_SEMIHOST.h>
#include <ameba_soc.h>

int
_read(int fd, char *ptr, int len)
{

	int r = 0;
	if (fd == 0) {
		int ena = DCache_IsEnabled();
		if (ena) {
			DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
			DCache_Disable();
		}
		r = SEGGER_SEMIHOST_ReadC();
		if (ena) {
			DCache_Enable();
		}
		*(char *)ptr = r;
		return 1;
	}

	int ena = DCache_IsEnabled();
	if (ena) {
		DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
		DCache_Disable();
	}
	r = SEGGER_SEMIHOST_Read((int)fd, ptr, len);
	if (ena) {
		DCache_Enable();
	}

	return len - r;
}

#endif
