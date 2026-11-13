#include <diag.h>
#include <fcntl.h>

#include <platform_autoconf.h>

#if !defined(SEGGER_SEMIHOST)
#if defined(CONFIG_UNISTD_SEMIHOSTING) && (CONFIG_UNISTD_SEMIHOSTING==1)
#define SEGGER_SEMIHOST 1
#endif
#endif


#if defined(SEGGER_SEMIHOST) && (SEGGER_SEMIHOST==1)
#include <SEGGER_SEMIHOST.h>
#include <ameba_soc.h>


int _open(char *file, int flags, int mode)
{
	int semiflags = 0;

	switch (flags & (O_RDONLY | O_WRONLY | O_RDWR)) {
	case O_RDONLY:
		semiflags = SYS_FILE_MODE_READBINARY;		/* 'rb' */
		break;
	case O_WRONLY:
		if (flags & O_TRUNC) {
			semiflags = SYS_FILE_MODE_WRITEBINARY;    /* 'wb' */
		} else {
			semiflags = SYS_FILE_MODE_APPENDBINARY;    /* 'ab' */
		}
		break;
	default:
		if (flags & O_TRUNC) {
			semiflags = SYS_FILE_MODE_WRITEREADBINARY;    /* 'wb+' */
		} else {
			semiflags = SYS_FILE_MODE_APPENDREADBINARY;    /* 'ab+' */
		}
		break;
	}

	int ret;
	int ena = DCache_IsEnabled();
	if (ena) {
		DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
		DCache_Disable();
	}
	do {
		ret = SEGGER_SEMIHOST_Open(file, semiflags, _strlen(file));
	} while (0 <= ret && ret <= 2);
	if (ena) {
		DCache_Enable();
	}

	return ret;

}

#endif
