#include <limits.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "mbed_printf_implementation.h"
#include "diag.h"

int vsprintf(char *str, const char *format, va_list ap)
{
	return mbed_minimal_formatted_string(str, INT_MAX, format, ap, NULL);
}

int vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
	return mbed_minimal_formatted_string(str, size, format, ap, NULL);
}

int sprintf(char *buf, const char *format, ...)
{
	int i;
	va_list ap;

	va_start(ap, format);
	i = vsprintf(buf, format, ap);
	va_end(ap);

	return i;
}

int snprintf(char *buf, size_t size, const char *format, ...)
{
	int i;
	va_list ap;

	va_start(ap, format);
	i = vsnprintf(buf, size, format, ap);
	va_end(ap);

	return i;
}

int fputc(int c, FILE *stream)
{
	void serial_putc(int c);

	serial_putc(c);
	return 1;
}

int vprintf(const char *format, va_list ap)
{
	int ret;

	ret = mbed_minimal_formatted_string(NULL, INT_MAX, format, ap,
										(void *)1);
	return ret;
}

int printf(const char *format, ...)
{
	int i;
	va_list ap;

	va_start(ap, format);
	i = vprintf(format, ap);
	va_end(ap);

	return i;
}

/*  FIXME: need vendor implamented */
int isp_rtos_printf(const char *format, char *buf)
{
	int i;

	i = DiagPrintf(format, buf);
	return i;
}
