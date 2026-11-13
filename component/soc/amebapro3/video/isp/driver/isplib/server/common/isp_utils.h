
#ifndef _ISP_UTIL_H_INC_
#define _ISP_UTIL_H_INC_

#include <isp_utils_common.h>

#define has_op(master, op) ((master)->op)
#define isp_call_op(master, op, ...)                                \
	(has_op(master, op) ? (master)->op(master, ##__VA_ARGS__) : \
			      -RTS_ISP_ENOOPS)
#define isp_call_op_ret(ret, master, op, ...) \
	(ret = isp_call_op(master, op, ##__VA_ARGS__))

long isp_strlcpy(char *dest, const char *src, size_t size);
int isp_get_val_weight(uint32_t val);
uint32_t isp_upper_power_of_2(uint32_t val);

int linear_intp(int x, int x0, int y0, int x1, int y1);
float linear_intp_float(float x, float x0, float y0, float x1, float y1);
int linear_intp_bound(int x, int x0, int y0, int x1, int y1);
#define linear_intp_array(arr, x, x0, arr0, x1, arr1, count)                   \
	({                                                                     \
		int __intp_array_idx;                                          \
		int ret = RTS_ISP_OK;                                          \
		if ((arr) == NULL || (arr0) == NULL || (arr1) == NULL)         \
			ret = -RTS_ISP_EINVAL;                                 \
		else                                                           \
			for (__intp_array_idx = 0; __intp_array_idx < (count); \
			     __intp_array_idx++)                               \
				(arr)[__intp_array_idx] = linear_intp(         \
					(x), (x0), (arr0)[__intp_array_idx],   \
					(x1), (arr1)[__intp_array_idx]);       \
		ret;                                                           \
	})

int calc_smooth(int current, int target, int div, int max);
#define calc_smooth_array(current, target, div, max, count)                \
	do {                                                               \
		int __intp_array_idx;                                      \
		for (__intp_array_idx = 0; __intp_array_idx < (count);     \
		     __intp_array_idx++)                                   \
			(current)[__intp_array_idx] = calc_smooth(         \
				(current)[__intp_array_idx],               \
				(target)[__intp_array_idx], (div), (max)); \
	} while (0)

#endif /* _ISP_UTIL_H_INC_ */
