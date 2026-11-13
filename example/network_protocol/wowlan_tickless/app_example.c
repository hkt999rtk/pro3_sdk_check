/******************************************************************************
*
* Copyright(c) 2007 - 2018 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "platform_autoconf.h"
#include "ameba_soc.h"
#include "example_wowlan_tickless.h"

void app_example(void)
{
#if defined (CONFIG_ARM_CORE_CA32)
	example_wowlan_tickless();
#elif defined (CONFIG_ARM_CORE_CM4_NP)
	example_wowlan_ipc_process();
#endif
}
