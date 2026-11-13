#include <stdio.h>
#include "stdint.h"
#include "ameba_soc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "wowlan_ipccfg.h"
#include "lwip_netconf.h"
#include "ameba_wificfg.h"

#define AUTO_TEST		0

void wowlan_tickless_task(void *param)
{
	WOWLANCFG wowlan_setting = {0};
	uint8_t pattern_match = 0;
	//notify np for ap ready
	wowlan_ap_wakeup_notification();
	//wait for np response
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	wowlan_get_wakeup_reason(&wowlan_setting);

	if (wowlan_setting.fp_wakeup_reason != 0 || wowlan_setting.np_wakeup_reason != 0) {
		//wowlan wakeup
		DiagPrintf("\r\nwakeup reason np = 0x%x fp = 0x%x\r\n", wowlan_setting.np_wakeup_reason, wowlan_setting.fp_wakeup_reason);
		if (wowlan_setting.np_wakeup_reason == RX_Wakeup_Pattern) {
			pattern_match = 1;
		}
		memset(&wowlan_setting, 0, sizeof(WOWLANCFG));
		if (pattern_match) {
			DiagPrintf("\r\nwlan pattern match wakeup\r\n");
			wowlan_setting.resume = 1;
		}
	}

	// Delay to check successful WiFi connection and obtain of an IP address
	while (lwip_check_connectivity(NETIF_WLAN_STA_INDEX) != CONNECTION_VALID) {
		vTaskDelay(2000);
	}

#if defined(CONFIG_WHC_HOST) && !defined(CONFIG_MP_SHRINK)
	/* Backup IP/connection info to retention SRAM for resume after AP power-down */
	wifi_resume_backup();
#endif

#if AUTO_TEST
	if (wowlan_setting.resume == 1) {
		while (1) {
			//for iperf
			vTaskDelay(1000);
		}
	}
#endif

	wowlan_set_default_parameter(&wowlan_setting);
	wowlan_enable(&wowlan_setting);

	while (1) {
		vTaskDelay(1000);
	}

	vTaskDelete(NULL);
}
