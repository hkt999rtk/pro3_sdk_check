#include <stdio.h>
#include "stdint.h"
#include "ameba_soc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "wowlan_tickless.h"

#if defined (CONFIG_ARM_CORE_CA32)
extern TaskHandle_t APTaskHandle;

void example_wowlan_tickless(void)
{
	DiagPrintf("start wowlan tickless flow\r\n");
	if (xTaskCreate(wowlan_tickless_task, ((const char *)"wowlan_tickless_task"), 4096, NULL, tskIDLE_PRIORITY + 1, &APTaskHandle) != pdPASS) {
		DiagPrintf("\r\n wowlan_tickless_task: Create Task Error\n");
	}
}

#elif defined (CONFIG_ARM_CORE_CM4_NP)

extern TaskHandle_t WowlanIpcTaskHandle;

void example_wowlan_ipc_process(void)
{
	DiagPrintf("\r\nstart ipc handler\r\n");
	if (xTaskCreate(keep_alive_ipc_handler, ((const char *)"keep_alive_ipc_handler"), 1024, NULL, tskIDLE_PRIORITY + 1, &WowlanIpcTaskHandle) != pdPASS) {
		DiagPrintf("\r\n wowlan_ipc_handler_task: Create Task Error\n");
	}
}

#endif
