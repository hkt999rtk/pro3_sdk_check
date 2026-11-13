/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2019 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_os_time.h>

uint64_t rts_isp_get_time_ms(void)
{
	return isp_os_time_get_ms();
}
