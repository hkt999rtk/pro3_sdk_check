#ifndef _ISP_OS_MEM_H_INC_
#define _ISP_OS_MEM_H_INC_

#include <stddef.h>

void *isp_os_malloc(size_t size);
void *isp_os_calloc(size_t nmemb, size_t size);
void *isp_os_realloc(void *ptr, size_t size);
void isp_os_free(void *ptr);

char *isp_os_strdup(const char *s);

#define isp_malloc(size) isp_os_malloc(size)
#define isp_calloc(nmemb, size) isp_os_calloc(nmemb, size)
#define isp_realloc(ptr, size) isp_os_realloc(ptr, size)
#define isp_free(ptr)                     \
	do {                              \
		if (ptr) {                \
			isp_os_free(ptr); \
			ptr = NULL;       \
		}                         \
	} while (0)

void *isp_os_memcpy(void *dest, const void *src, size_t n);
void *isp_os_memmove(void *dest, const void *src, size_t n);
void *isp_os_memset(void *s, int c, size_t n);
int isp_os_memcmp(const void *s1, const void *s2, size_t n);
int isp_os_strcmp(const char *s1, const char *s2);
size_t isp_os_strlen(const char *s);
char *isp_os_strchr(const char *s, int c);

#define isp_memcpy(dst, src, size) isp_os_memcpy(dst, src, size)
#define isp_memmove(dst, src, size) isp_os_memmove(dst, src, size)
#define isp_memset(s, c, n) isp_os_memset(s, c, n)
#define isp_memcmp(s1, s2, n) isp_os_memcmp(s1, s2, n)
#define isp_strcmp(s1, s2) isp_os_strcmp(s1, s2)
#define isp_strlen(s) isp_os_strlen(s)
#define isp_strchr(s, c) isp_os_strchr(s, c)

#define isp_calloc_item(item) (item = isp_calloc(1, sizeof(*(item))))
#define isp_calloc_array(item, n) (item = isp_calloc(n, sizeof(*(item))))
#define isp_memcpy_array(dst, src, size)                           \
	({                                                         \
		typeof(dst[0]) *_dst = (dst);                      \
		typeof(src[0]) *_src = (src);                      \
		const typeof(dst[0]) *__dst = _dst;                \
		const typeof(src[0]) *__src = _src;                \
		(void)(&__src == &__dst);                          \
		isp_os_memcpy(_dst, _src, (size) * sizeof(*_dst)); \
	})
#define isp_memcmp_array(arr0, arr1, count)                          \
	({                                                           \
		const typeof(arr0[0]) *_arr0 = arr0;                 \
		const typeof(arr1[0]) *_arr1 = arr1;                 \
		(void)(&_arr0 == &_arr1);                            \
		isp_os_memcmp(_arr0, _arr1, count * sizeof(*_arr0)); \
	})

#endif /* _ISP_OS_MEM_H_INC_ */
