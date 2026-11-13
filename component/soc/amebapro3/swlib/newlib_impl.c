#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <basic_types.h>
#include <ameba_soc.h>

#if defined(__GNUC__)

__attribute__((constructor))
static void init_stdout_buffer(void)
{
	setbuf(stdout, NULL);
}

_ssize_t  _write_r(struct _reent *r, int file, const void *ptr, size_t len)
{
	(void) file;  /* Not used, avoid warning */
	(void) r;     /* Not used, avoid warning */

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

_ssize_t  _read_r(struct _reent *r, int file, void *ptr, size_t len)
{
	(void) file;  /* Not used, avoid warning */
	(void) r;     /* Not used, avoid warning */

	u8 *msg = (u8 *)ptr;
	size_t recv_len = 0;

	if (file == 0) {	// stdin
		int max_cnt = 1000;
		while (max_cnt-- && (recv_len < len)) {
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
#endif


