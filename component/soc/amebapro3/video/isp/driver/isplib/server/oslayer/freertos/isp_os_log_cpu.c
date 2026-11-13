/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2022 Sherry Cheng <sherry_cheng@realsil.com.cn>
 */

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <rts_isp_errno.h>
#include <isp_os_driver.h>
#include <isp_os_log.h>

struct isp_log_handle {
	char buf[256];
	uint32_t debug_sel;
	unsigned int opened : 1;
};

struct rtscam_ispc_log {
	void *buf;
	uint32_t size;
};

static struct isp_log_handle log_handle;

extern int rtscam_ispc_mcu_write_log(struct rtscam_ispc_log *log);

static void __isp_log(int level, const char *fmt, va_list ap)
{
#define LOG_HEADER_SIZE 8
	char *buf;
	uint32_t size = sizeof(log_handle.buf);
	static const char level_str[][LOG_HEADER_SIZE] = {
		"ERROR : ",
		"WARN  : ",
		"INFO  : ",
		"DEBUG : ",
	};
	struct rtscam_ispc_log log;

	if (!log_handle.opened) {
		return;
	}

	strncpy(log_handle.buf, level_str[level], LOG_HEADER_SIZE);
	buf = log_handle.buf + LOG_HEADER_SIZE;
	size = vsnprintf(buf, size - LOG_HEADER_SIZE, fmt, ap);

	log.buf = log_handle.buf;
	log.size = LOG_HEADER_SIZE + size;
	rtscam_ispc_mcu_write_log(&log);
}

void isp_os_log_open(void)
{
	if (log_handle.opened) {
		return;
	}

	log_handle.debug_sel = 0;
	log_handle.opened = RTS_ISP_TRUE;
}

void isp_os_log_close(void)
{
	log_handle.opened = RTS_ISP_FALSE;
}

void isp_os_log(int level, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	__isp_log(level, fmt, ap);
	va_end(ap);
}

void isp_os_debug(int type, const char *fmt, ...)
{
	if (log_handle.debug_sel & type) {
		va_list ap;

		va_start(ap, fmt);
		__isp_log(ISP_LOG_DEBUG, fmt, ap);
		va_end(ap);
	}
}

void isp_os_log_set_debug_select(uint32_t debug_sel)
{
	if (!log_handle.opened) {
		return;
	}
	log_handle.debug_sel = debug_sel;
}

void isp_os_abort(void)
{
	while (1)
		;
}
