/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <rts_isp_errno.h>

#define ISP_PRIV_ERR_BASE RTS_ISP_EFAULT
#define pri_err_idx(err) ((err)-ISP_PRIV_ERR_BASE)

const char *rts_isp_err_msg[] = {
	[pri_err_idx(RTS_ISP_EFAULT)] = "Fault or bad address",
	[pri_err_idx(RTS_ISP_EPERM)] = "Operation not permitted",
	[pri_err_idx(RTS_ISP_EINTR)] = "Interrupted system call",
	[pri_err_idx(RTS_ISP_EAGAIN)] = "Resource temporarily unavailable",
	[pri_err_idx(RTS_ISP_ENOMEM)] = "Not enough memory",
	[pri_err_idx(RTS_ISP_EACCES)] = "Permission denied",
	[pri_err_idx(RTS_ISP_EBUSY)] = "Device or resource busy",
	[pri_err_idx(RTS_ISP_EEXIST)] = "File or resource exists",
	[pri_err_idx(RTS_ISP_EINVAL)] = "Invalid argument",
	[pri_err_idx(RTS_ISP_ENOSPC)] = "No space left",
	[pri_err_idx(RTS_ISP_ERANGE)] = "Result too large",
	[pri_err_idx(RTS_ISP_ENOOPS)] = "No such operating fuction",
	[pri_err_idx(RTS_ISP_ENOITEM)] = "No such item",
	[pri_err_idx(RTS_ISP_ENOTREADY)] = "Does not ready",
	[pri_err_idx(RTS_ISP_EPLUGIN)] = "Plugin error",
	[pri_err_idx(RTS_ISP_EPOLLERR)] = "Poll error",
	[pri_err_idx(RTS_ISP_ECTRL)] = "Control error",
	[pri_err_idx(RTS_ISP_ECONFLICT)] = "Conflict",
	[pri_err_idx(RTS_ISP_ECANCELD)] = "Operation Canceled",
	[pri_err_idx(RTS_ISP_EPROTOBUF)] = "Protobuf Operation Fail",
	[pri_err_idx(RTS_ISP_EPROTOBUF)] = "Connection timed out",
};

int rts_isp_strerror(int errnum, char *msg_buf, size_t size)
{
	errnum = -errnum;
	if (errnum < ISP_PRIV_ERR_BASE) {
		return strerror_r(errnum, msg_buf, size);
	} else if (errnum < _MAX_RTS_ISP_ERRNO) {
		strncpy(msg_buf, rts_isp_err_msg[pri_err_idx(errnum)], size);
	} else {
		snprintf(msg_buf, size, "Unknown ISP errno: %d", errnum);
	}
	msg_buf[size - 1] = '\0';
	return RTS_ISP_OK;
}

void rts_isp_perror(int errnum, const char *fmt, ...)
{
	va_list ap;
	static char msg[256];

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	rts_isp_strerror(errnum, msg, sizeof(msg));
	printf(" -> %s\n", msg);
}

int rts_isp_errno_from_os(int os_errnum)
{
	if (os_errnum < 0 || os_errnum >= ISP_PRIV_ERR_BASE) {
		return -_MAX_RTS_ISP_ERRNO;
	}

	switch (os_errnum) {
	case EFAULT:
		return -RTS_ISP_EFAULT;
	case EPERM:
		return -RTS_ISP_EPERM;
	case EINTR:
		return -RTS_ISP_EINTR;
	case EAGAIN:
		return -RTS_ISP_EAGAIN;
	case ENOMEM:
		return -RTS_ISP_ENOMEM;
	case EACCES:
		return -RTS_ISP_EACCES;
	case EBUSY:
		return -RTS_ISP_EBUSY;
	case EEXIST:
		return -RTS_ISP_EEXIST;
	case EINVAL:
		return -RTS_ISP_EINVAL;
	case ENOSPC:
		return -RTS_ISP_ENOSPC;
	case ERANGE:
		return -RTS_ISP_ERANGE;
	case ETIMEDOUT:
		return -RTS_ISP_ETIMEDOUT;
	default:
		return -os_errnum;
	}
}
