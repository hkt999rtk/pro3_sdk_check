#ifndef _ISP_UTIL_COMMON_H_INC_
#define _ISP_UTIL_COMMON_H_INC_

#include <stddef.h>
#include <stdint.h>

#define ___PASTE(a, b) a##b
#define __PASTE(a, b) ___PASTE(a, b)
#define __UNIQUE_ID(prefix) __PASTE(__PASTE(__UNIQUE_ID_, prefix), __COUNTER__)

#define __min(t1, t2, min1, min2, x, y)    \
	({                                 \
		t1 min1 = (x);             \
		t2 min2 = (y);             \
		(void)(&min1 == &min2);    \
		min1 < min2 ? min1 : min2; \
	})
#define min(x, y)                                                              \
	__min(typeof(x), typeof(y), __UNIQUE_ID(min1_), __UNIQUE_ID(min2_), x, \
	      y)

#define __max(t1, t2, max1, max2, x, y)    \
	({                                 \
		t1 max1 = (x);             \
		t2 max2 = (y);             \
		(void)(&max1 == &max2);    \
		max1 > max2 ? max1 : max2; \
	})
#define max(x, y)                                                              \
	__max(typeof(x), typeof(y), __UNIQUE_ID(max1_), __UNIQUE_ID(max2_), x, \
	      y)

#define min3(x, y, z) min((typeof(x))min(x, y), z)
#define max3(x, y, z) max((typeof(x))max(x, y), z)

#define clamp(val, lo, hi) min((typeof(val))max(val, lo), hi)
#define clamp_t(type, val, lo, hi) min_t(type, max_t(type, val, lo), hi)

#define min_t(type, x, y) \
	__min(type, type, __UNIQUE_ID(min1_), __UNIQUE_ID(min2_), x, y)

#define max_t(type, x, y) \
	__max(type, type, __UNIQUE_ID(min1_), __UNIQUE_ID(min2_), x, y)

#define container_of(ptr, type, member)                            \
	({                                                         \
		const typeof(((type *)0)->member) *__mptr = (ptr); \
		(type *)((char *)__mptr - offsetof(type, member)); \
	})

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define __ARG_PLACEHOLDER_1 0,
#define __take_second_arg(__ignored, val, ...) val
#define __is_defined(arg1_or_junk) __take_second_arg(arg1_or_junk 1, 0)
#define _is_defined(val) __is_defined(__ARG_PLACEHOLDER_##val)
#define ISP_DEFINED(x) _is_defined(x)

#define isp_fourcc(a, b, c, d)                                          \
	((uint32_t)(a) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | \
	 ((uint32_t)(d) << 24))

#define isp_release(ptr, call_back)     \
	do {                            \
		if (ptr)                \
			call_back(ptr); \
		ptr = NULL;             \
	} while (0)

#define ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define ALIGN(x, a) ALIGN_MASK(x, (typeof(x))(a)-1)
#define ALIGN_DOWN(x, a) ALIGN((x) - ((a)-1), (a))
#define PTR_ALIGN(p, a) ((typeof(p))ALIGN((unsigned long)(p), (a)))

#define roundup_div(x, y)                \
	({                               \
		const typeof(y) __y = y; \
		((x) + (__y - 1)) / __y; \
	})
#define roundup(x, y)                            \
	({                                       \
		const typeof(y) __y = y;         \
		(((x) + (__y - 1)) / __y) * __y; \
	})
#define rounddown(x, y)              \
	({                           \
		typeof(x) __x = (x); \
		__x - (__x % (y));   \
	})

#define isp_abs(x)                       \
	({                               \
		const typeof(x) __x = x; \
		__x < 0 ? -__x : __x;    \
	})

#define float_tol 0.0001f
#define float_ne(x, y) (isp_abs(x - y) > float_tol)
#define float_eq(x, y) (isp_abs(x - y) < float_tol)

/* WARNING: do not use it on 8/16 bit int */
#define set_const_var(var, value)                                    \
	do {                                                         \
		typeof((var) + 0) *pvar = (typeof((var) + 0) *)&var; \
		*pvar = value;                                       \
	} while (0)

#define set_const_var_type(type, var, value) \
	do {                                 \
		type *pvar = (type *)&var;   \
		*pvar = value;               \
	} while (0)

#define isp_set_bit(val, bit) ((val) |= (1 << (bit)))
#define isp_clr_bit(val, bit) ((val) &= ~(1 << (bit)))
#define isp_test_bit(val, bit) ((val) & (1 << (bit)))

#define _ISP_AID_NRBITS 8
#define _ISP_AID_TYPEBITS 8
#define _ISP_AID_SIZEBITS 14
#define _ISP_AID_DIRBITS 2

#define _ISP_AID_NRMASK ((1 << _ISP_AID_NRBITS) - 1)
#define _ISP_AID_TYPEMASK ((1 << _ISP_AID_TYPEBITS) - 1)
#define _ISP_AID_SIZEMASK ((1 << _ISP_AID_SIZEBITS) - 1)
#define _ISP_AID_DIRMASK ((1 << _ISP_AID_DIRBITS) - 1)

#define _ISP_AID_NRSHIFT 0
#define _ISP_AID_TYPESHIFT (_ISP_AID_NRSHIFT + _ISP_AID_NRBITS)
#define _ISP_AID_SIZESHIFT (_ISP_AID_TYPESHIFT + _ISP_AID_TYPEBITS)
#define _ISP_AID_DIRSHIFT (_ISP_AID_SIZESHIFT + _ISP_AID_SIZEBITS)

#define ISP_AID_TYPE(id) (((id) >> _ISP_AID_TYPESHIFT) & _ISP_AID_TYPEMASK)
#define ISP_AID_NR(id) (((id) >> _ISP_AID_NRSHIFT) & _ISP_AID_NRMASK)
#define ISP_AID_SIZE(id) (((id) >> _ISP_AID_SIZESHIFT) & _ISP_AID_SIZEMASK)
#define ISP_AID_DIR(id) (((id) >> _ISP_AID_DIRSHIFT) & _ISP_AID_DIRMASK)

#define _AID_NONE 0
#define _AID_READ 1
#define _AID_WRITE 2

#define ISP_AID_IS_NONE(action) (!ISP_AID_DIR(action))
#define ISP_AID_IS_READ(action) (ISP_AID_DIR(action) & _AID_READ)
#define ISP_AID_IS_WRITE(action) (ISP_AID_DIR(action) & _AID_WRITE)

#define ISP_AID_HAS_DATA(action) (!!ISP_AID_SIZE(action))
#define ISP_AID_HAS_READ_DATA(action) \
	(ISP_AID_IS_READ(action) && ISP_AID_SIZE(action))
#define ISP_AID_HAS_WRITE_DATA(action) \
	(ISP_AID_IS_WRITE(action) && ISP_AID_SIZE(action))

#define isp_action_size(dir, type, nr, size)        \
	(uint32_t)(((dir) << _ISP_AID_DIRSHIFT) |   \
		   ((type) << _ISP_AID_TYPESHIFT) | \
		   ((nr) << _ISP_AID_NRSHIFT) |     \
		   ((size) << _ISP_AID_SIZESHIFT))

#define isp_action(type, nr) isp_action_size(_AID_NONE, type, nr, 0)
#define isp_action_r(type, nr, data) \
	isp_action_size(_AID_READ, type, nr, sizeof(data))
#define isp_action_w(type, nr, data) \
	isp_action_size(_AID_WRITE, type, nr, sizeof(data))
#define isp_action_wr(type, nr, data) \
	isp_action_size(_AID_READ | _AID_WRITE, type, nr, sizeof(data))

#define INFO_TYPE 'I'
#define EXEC_TYPE 'E'
#define NOTIFY_TYPE 'N'

#endif /* _ISP_UTIL_COMMON_H_INC_ */
