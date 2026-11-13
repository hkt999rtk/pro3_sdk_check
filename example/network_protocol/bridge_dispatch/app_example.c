/******************************************************************************
*
* Copyright(c) 2007 - 2026 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "platform_autoconf.h"
#include "ameba_soc.h"
#include "example_bridge_dispatch.h"

void app_example(void)
{
#if defined(CONFIG_ARM_CORE_CM4_NP) && defined(CONFIG_WHC_DEV_TCPIP_KEEPALIVE)
	example_bridge_dispatch();
#endif
}
