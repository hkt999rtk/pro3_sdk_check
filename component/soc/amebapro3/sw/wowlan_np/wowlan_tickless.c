#include "stdio.h"
#include "ameba_soc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "wowlan_ipccfg.h"
#include "keep_alive.h"

TaskHandle_t wowlanthreadhandle = NULL;

void keep_alive_ipc_handler(void *param)
{
	UNUSED(param);
	PIPC_MSG_STRUCT kacfg = NULL;
	uint32_t notify_value = 0;
	WOWLANCFG wowlan_cfg = {0};
	uint8_t fp_wakeup_reason = 0;

	while (1) {
		//parser ipc message
		xTaskNotifyWait(0, IPC_AP_NOTIFY_BIT | IPC_FP_NOTIFY_BIT, &notify_value, portMAX_DELAY);
		if (notify_value & IPC_AP_NOTIFY_BIT) {
			kacfg = ipc_get_message(IPC_AP_TO_NP, IPC_A2N_WOWLAN);
			if (kacfg == NULL) {
				continue;
			}
			DCache_Invalidate(kacfg->msg, kacfg->msg_len);
			if (kacfg->msg_type == A2N_WOWLAN_CFG) {
				DiagPrintf("\r\nreceive wowlan enable, start wowlan thread\r\n");
				memcpy(&wowlan_cfg, (uint8_t *)kacfg->msg, kacfg->msg_len);
				wowlan_set_wifi_cfg(&wowlan_cfg);
				if (wowlan_cfg.resume) {
					xSemaphoreGive(resumesemaphore);
				} else {
					if (xTaskCreate(keep_alive_thread, ((const char *)"keep_alive_thread"), 4096, NULL, tskIDLE_PRIORITY + 1, &wowlanthreadhandle) != pdPASS) {
						DiagPrintf("\r\n wowlan_example_task: Create Task Error\n");
					}
				}
			} else if (kacfg->msg_type == A2N_AP_WAKEUP) {
				DiagPrintf("\r\nAP already waked up\r\n");
				//send wakeup reason
				wowlan_get_wifi_cfg(&wowlan_cfg);
				wowlan_report_wakeup_reason(&wowlan_cfg);
			}
		} else if (notify_value & IPC_FP_NOTIFY_BIT) {
			extern u8 wifi_hal_get_wowlan_wakeup_reason(void);
			fp_wakeup_reason = wifi_hal_get_wowlan_wakeup_reason();
			if (fp_wakeup_reason == Rx_DeAuth_DisAssoc || fp_wakeup_reason == FWDecisionDisconnect) {
				wowlan_get_wifi_cfg(&wowlan_cfg);
				wowlan_cfg.fp_wakeup_reason = fp_wakeup_reason;
				wowlan_set_wifi_cfg(&wowlan_cfg);
				DiagPrintf("\r\nfp wakeup reason = %d, need to close connection before wifi reconnect\r\n", fp_wakeup_reason);
				wowlan_close_server_conn_cb();
			}
		}
	}
}
