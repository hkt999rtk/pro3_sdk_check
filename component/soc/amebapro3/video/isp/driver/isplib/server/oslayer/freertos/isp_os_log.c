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

static struct isp_log_handle log_handle;

extern int isp_rtos_printf(const char *format, ...);
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

	if (!log_handle.opened) {
		return;
	}

	strncpy(log_handle.buf, level_str[level], LOG_HEADER_SIZE);
	buf = log_handle.buf + LOG_HEADER_SIZE;
	size = vsnprintf(buf, size - LOG_HEADER_SIZE, fmt, ap);
	isp_rtos_printf("%s", log_handle.buf);
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

#define RTK_LOG 0

#if RTK_LOG

#include <ameba_soc.h>
#define LOG_BUF_SIZE 1024
#define TAG "ISP_SW"
static char isp_sw_tag[] = TAG;
char *get_isp_sw_tag(void)
{
	return isp_sw_tag;
}
static rtk_log_level_t isp_log_level_to_rtk_log_level(int level)
{
	switch (level) {
	case ISP_LOG_ERR:
		return RTK_LOG_ERROR;

	case ISP_LOG_WARNING:
		return RTK_LOG_WARN;

	case ISP_LOG_INFO:
		return RTK_LOG_INFO;

	case ISP_LOG_DEBUG:
		return RTK_LOG_DEBUG;

	default:
		return RTK_LOG_INFO;
	}
}

void isp_os_log(int level, const char *fmt, ...)
{
	va_list ap;
	char buf[LOG_BUF_SIZE];
	rtk_log_level_t rtk_level;

	if (fmt == NULL) {
		return;
	}

	rtk_level = isp_log_level_to_rtk_log_level(level);

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	RTK_LOGS(TAG, rtk_level, "%s", buf);
}
#else
void isp_os_log(int level, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	__isp_log(level, fmt, ap);
	va_end(ap);
}
#endif

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
