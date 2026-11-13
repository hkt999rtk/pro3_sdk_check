/**************************************************
 * malloc/free/realloc wrap for gcc compiler
 *
 **************************************************/
#if defined(__GNUC__)
#include <stdint.h>
#include <stddef.h> /* for size_t */
#include <stdarg.h>
#include "os_wrapper.h"

/* For GCC stdlib */
void *__wrap__malloc_r(void *reent, size_t size)
{
	(void) reent;
	return rtos_mem_malloc(size);
}

void *__wrap__realloc_r(void *reent, void *p, size_t size)
{
	(void) reent;
	return rtos_mem_realloc(p, size);
}

void __wrap__free_r(void *reent, void *p)
{
	(void) reent;
	rtos_mem_free(p);
}

void *__wrap__calloc_r(void *reent, size_t xWantedCnt, size_t xWantedSize)
{
	(void) reent;
	return rtos_mem_calloc(xWantedCnt, xWantedSize);
}

// for general use case
void *__wrap_malloc(size_t size)
{
	return rtos_mem_malloc(size);
}

void *__wrap_realloc(void *p, size_t size)
{
	return rtos_mem_realloc(p, size);
}

void __wrap_free(void *p)
{
	rtos_mem_free(p);
}

void *__wrap_calloc(size_t xWantedCnt, size_t xWantedSize)
{
	return rtos_mem_calloc(xWantedCnt, xWantedSize);
}

#endif
