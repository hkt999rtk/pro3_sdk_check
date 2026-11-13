/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2022 Sherry Cheng <sherry_cheng@realsil.com.cn>
 */

#include <string.h>
#include <isp_os_mem.h>
#include <freertos/FreeRTOS.h>

void *isp_os_malloc(size_t size)
{
	return pvPortMalloc(size);
}

void *isp_os_calloc(size_t nmemb, size_t size)
{
	void *ptr = NULL;

	if (!nmemb || !size) {
		return NULL;
	}
	ptr = pvPortMalloc(nmemb * size);
	if (!ptr) {
		return NULL;
	}
	memset(ptr, 0, nmemb * size);
	return ptr;
}

void *isp_os_realloc(void *ptr, size_t size)
{
	void *ptr_new = NULL;

	if (!ptr) {
		return pvPortMalloc(size);
	}
	if (!size && ptr) {
		vPortFree(ptr);
		return NULL;
	}

	ptr_new = pvPortMalloc(size);
	if (!ptr_new) {
		return NULL;
	}
	memcpy(ptr_new, ptr, size);
	vPortFree(ptr);
	ptr = NULL;
	return ptr_new;
}

void isp_os_free(void *ptr)
{
	vPortFree(ptr);
}

char *isp_os_strdup(const char *s)
{
	void *ptr = NULL;
	size_t len = strlen(s) + 1;

	ptr = pvPortMalloc(len);

	strncpy(ptr, s, len);
	return ptr;
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
