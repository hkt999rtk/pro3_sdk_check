#ifndef _RTS_ISP_ERRNO_H_INC_
#define _RTS_ISP_ERRNO_H_INC_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

enum rts_isp_boolean {
	RTS_ISP_FALSE = 0,
	RTS_ISP_TRUE,
};

enum rts_isp_errno {
	RTS_ISP_OK = 0,
	RTS_ISP_EFAULT = 256,
	RTS_ISP_EPERM,
	RTS_ISP_EINTR,
	RTS_ISP_EAGAIN,
	RTS_ISP_ENOMEM,
	RTS_ISP_EACCES,
	RTS_ISP_EBUSY,
	RTS_ISP_EEXIST,
	RTS_ISP_EINVAL,
	RTS_ISP_ENOSPC,
	RTS_ISP_ERANGE,
	RTS_ISP_ENOOPS,
	RTS_ISP_ENOITEM,
	RTS_ISP_ENOTREADY,
	RTS_ISP_EPLUGIN,
	RTS_ISP_EPOLLERR,
	RTS_ISP_ECTRL,
	RTS_ISP_ECONFLICT,
	RTS_ISP_ECANCELD,
	RTS_ISP_EPROTOBUF,
	RTS_ISP_ETIMEDOUT,
	_MAX_RTS_ISP_ERRNO,
};

int rts_isp_strerror(int errnum, char *msg_buf, size_t size);
void rts_isp_perror(int errnum, const char *s, ...);
int rts_isp_errno_from_os(int os_errnum);

#ifdef __cplusplus
}
#endif

#endif /* _RTS_ISP_ERRNO_H_INC_ */
