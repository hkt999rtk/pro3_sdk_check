#include <platform_autoconf.h>

#if !defined(SEGGER_SEMIHOST)
#if defined(CONFIG_UNISTD_SEMIHOSTING) && (CONFIG_UNISTD_SEMIHOSTING==1)
#define SEGGER_SEMIHOST 1
#endif
#endif

#if defined(SEGGER_SEMIHOST) && (SEGGER_SEMIHOST==1)
#include <SEGGER_SEMIHOST.h>
#include <ameba_soc.h>

int _close(int fd)
{
	if (fd <= 2) {
		return 0;
	}

	int ena = DCache_IsEnabled();
	if (ena) {
		DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
		DCache_Disable();
	}
	int ret = SEGGER_SEMIHOST_Close(fd);
	if (ena) {
		DCache_Enable();
	}

	return ret;
}

#endif
