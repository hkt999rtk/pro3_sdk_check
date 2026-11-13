#include <stdlib.h>
#include <string.h>
#include <isp_os_mem.h>

void *isp_os_malloc(size_t size)
{
	return malloc(size);
}

void *isp_os_calloc(size_t nmemb, size_t size)
{
	return calloc(nmemb, size);
}

void *isp_os_realloc(void *ptr, size_t size)
{
	return realloc(ptr, size);
}

void isp_os_free(void *ptr)
{
	free(ptr);
}

char *isp_os_strdup(const char *s)
{
	return strdup(s);
}

void *isp_os_memcpy(void *dest, const void *src, size_t n)
{
	return memcpy(dest, src, n);
}

void *isp_os_memmove(void *dest, const void *src, size_t n)
{
	return memmove(dest, src, n);
}

void *isp_os_memset(void *s, int c, size_t n)
{
	return memset(s, c, n);
}

int isp_os_memcmp(const void *s1, const void *s2, size_t n)
{
	return memcmp(s1, s2, n);
}

int isp_os_strcmp(const char *s1, const char *s2)
{
	return strcmp(s1, s2);
}

size_t isp_os_strlen(const char *s)
{
	return strlen(s);
}

char *isp_os_strchr(const char *s, int c)
{
	return strchr(s, c);
}
