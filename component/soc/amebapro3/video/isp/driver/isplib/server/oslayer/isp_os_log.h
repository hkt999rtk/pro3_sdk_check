#ifndef _ISP_OS_LOG_H_INC_
#define _ISP_OS_LOG_H_INC_

#include <stdint.h>
#include <rts_isp_errno.h>

enum isp_log_level {
	ISP_LOG_ERR,
	ISP_LOG_WARNING,
	ISP_LOG_INFO,
	ISP_LOG_DEBUG,
};

enum {
	_DEBUG_CORE = 1,
	_DEBUG_MOD,
	_DEBUG_POLL,
	_DEBUG_V4L2,
	_DEBUG_ALGO,
	_DEBUG_STREAM,
	_DEBUG_UDS,
	_DEBUG_DRIVER,
	_DEBUG_NOTIFY,
	_DEBUG_RGB2YUV,
	_DEBUG_CONTROL,
	_DEBUG_VCTRL,
	_DEBUG_SENSOR,
	_DEBUG_GLOBAL,
	_DEBUG_INTERFACE,
	_DEBUG_CROP,
	_DEBUG_BLC,
	_DEBUG_AE,
	_DEBUG_AWB,
	_DEBUG_FLICK,
	_DEBUG_WDR,
	_DEBUG_AF,
};

enum isp_debug_type {
	ISP_DEBUG_CORE = 1 << _DEBUG_CORE,
	ISP_DEBUG_MOD = 1 << _DEBUG_MOD,
	ISP_DEBUG_POLL = 1 << _DEBUG_POLL,
	ISP_DEBUG_V4L2 = 1 << _DEBUG_V4L2,
	ISP_DEBUG_ALGO = 1 << _DEBUG_ALGO,
	ISP_DEBUG_STREAM = 1 << _DEBUG_STREAM,
	ISP_DEBUG_UDS = 1 << _DEBUG_UDS,
	ISP_DEBUG_DRIVER = 1 << _DEBUG_DRIVER,
	ISP_DEBUG_NOTIFY = 1 << _DEBUG_NOTIFY,
	ISP_DEBUG_RGB2YUV = 1 << _DEBUG_RGB2YUV,
	ISP_DEBUG_GLOBAL = 1 << _DEBUG_GLOBAL,
	ISP_DEBUG_CONTROL = 1 << _DEBUG_CONTROL,
	ISP_DEBUG_VCTRL = 1 << _DEBUG_VCTRL,
	ISP_DEBUG_SENSOR = 1 << _DEBUG_SENSOR,
	ISP_DEBUG_INTERFACE = 1 << _DEBUG_INTERFACE,
	ISP_DEBUG_CROP = 1 << _DEBUG_CROP,
	ISP_DEBUG_BLC = 1 << _DEBUG_BLC,
	ISP_DEBUG_AE = 1 << _DEBUG_AE,
	ISP_DEBUG_AWB = 1 << _DEBUG_AWB,
	ISP_DEBUG_FLICK = 1 << _DEBUG_FLICK,
	ISP_DEBUG_WDR = 1 << _DEBUG_WDR,
	ISP_DEBUG_AF = 1 << _DEBUG_AF,
};

#define ISP_DEBUG_CORE_ALL                                                    \
	(ISP_DEBUG_CORE | ISP_DEBUG_MOD | ISP_DEBUG_POLL | ISP_DEBUG_STREAM | \
	 ISP_DEBUG_UDS | ISP_DEBUG_DRIVER | ISP_DEBUG_NOTIFY)

void isp_os_log_open(void);
void isp_os_log_close(void);
void isp_os_log(int level, const char *fmt, ...);
void isp_os_log_set_debug_select(uint32_t debug_sel);

/* Debug logs are compiled out by default. Define ISP_NO_DEBUG=0 via build
 * flags to compile them in; runtime log filters still apply.
 */
#ifndef ISP_NO_DEBUG
#define ISP_NO_DEBUG 1
#endif

#if !ISP_NO_DEBUG
#define isp_debug(fmt, ...) isp_os_log(ISP_LOG_DEBUG, fmt, ##__VA_ARGS__)
#else
#define isp_debug(fmt, ...)
#endif
#define isp_warning(fmt, ...) isp_os_log(ISP_LOG_WARNING, fmt, ##__VA_ARGS__)
#define isp_info(fmt, ...) isp_os_log(ISP_LOG_INFO, fmt, ##__VA_ARGS__)
#define isp_error(fmt, ...)                                           \
	isp_os_log(ISP_LOG_ERR, "%s:%d:%s: " fmt, __FILE__, __LINE__, \
		   __func__, ##__VA_ARGS__)
#define isp_perror(errnum, fmt, ...)                           \
	do {                                                   \
		char buf[256];                                 \
		rts_isp_strerror(errnum, buf, sizeof(buf));    \
		isp_error(fmt " -> %s\n", ##__VA_ARGS__, buf); \
	} while (0)

#define isp_info_cont isp_info

#if !ISP_NO_DEBUG
void isp_os_debug(int type, const char *fmt, ...);
#define debug(fmt, ...) isp_os_debug(ISP_DEBUG_TYPE, fmt, ##__VA_ARGS__)
#else
#define debug(fmt, ...)
#endif

void isp_os_abort(void);
#define isp_assert(expr)                                            \
	do {                                                        \
		if (!(expr)) {                                      \
			isp_error("Failed to assert: %s\n", #expr); \
			isp_os_abort();                             \
		}                                                   \
	} while (0)

#endif /* _ISP_OS_LOG_H_INC_ */
