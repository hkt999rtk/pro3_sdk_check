#include "ameba_soc.h"
#include "os_wrapper.h"

#ifdef CONFIG_MBEDTLS_ENABLED

#include "ssl_rom_to_ram_map.h"
#include "threading_alt.h"

__attribute__((constructor))
void app_mbedtls_init(void)
{
	ssl_function_map.ssl_calloc = (void *(*)(unsigned int, unsigned int))rtos_mem_calloc;
	ssl_function_map.ssl_free = (void (*)(void *))rtos_mem_free;
	ssl_function_map.ssl_printf = (long unsigned int (*)(const char *, ...))printf;
	ssl_function_map.ssl_snprintf = (int (*)(char *s, size_t n, const char *format, ...))snprintf;
#if defined(CONFIG_MBEDTLS_THREADING)
	mbedtls_threading_init();
#endif
}

#endif
