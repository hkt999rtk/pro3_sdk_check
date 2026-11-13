/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2021 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <unistd.h>
#include <time.h>
#include <isp_os_time.h>

uint64_t isp_os_time_get_us(void)
{
	struct timespec time;
	const uint64_t S2US = 1000000;
	const uint32_t US2NS = 1000;

	clock_gettime(CLOCK_MONOTONIC, &time);

	return (unsigned long)time.tv_sec * S2US + time.tv_nsec / US2NS;
}

uint64_t isp_os_time_get_ms(void)
{
	struct timespec time;
	const uint64_t S2MS = 1000;
	const uint32_t MS2NS = 1000000;

	clock_gettime(CLOCK_MONOTONIC, &time);

	return (unsigned long)time.tv_sec * S2MS + time.tv_nsec / MS2NS;
}

void isp_os_usleep(uint32_t us)
{
	usleep(us);
}

void isp_os_msleep(uint32_t ms)
{
	usleep(1000 * ms);
}
