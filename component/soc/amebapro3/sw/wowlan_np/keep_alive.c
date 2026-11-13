#include "stdio.h"
#include "ameba_soc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "wowlan_ipccfg.h"
#include "keep_alive.h"
#include "lwip_netconf.h"
#include "lwip/prot/dhcp.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "lwip/ip_addr.h"
#include "wifi_api_event.h"
#include "os_wrapper.h"
#include "whc_dev_api.h"

#define WOWLAN_DHCP_ERR_HANDLEER		0
#define WIFI_RECONN_TO_SECOND			50
#define AUTO_TEST						0

static WOWLANCB ka_cb;
void (*wowlan_close_server_conn_cb)(void);
static WOWLANCFG wowlan_setting = {0};
extern TaskHandle_t wowlanthreadhandle;
SemaphoreHandle_t resumesemaphore;
static char server_ip[16] = {0};
static uint16_t server_port = 0;
#if AUTO_TEST
static int goto_sleep = 0;
static int use_default = 0;
static int auto_test_dtim = 0;
#else
static int goto_sleep = 1;
static int use_default = 1;
#endif

extern void wifi_hal_wlan_timer_tickless_process(uint8_t suspend);

#if WOWLAN_DHCP_ERR_HANDLEER
static void dhcp_state_cb(struct netif *netif)
{
	struct dhcp *dhcp = netif_dhcp_data(netif);
	bool ip_valid = !ip4_addr_isany_val(*netif_ip4_addr(netif));

	if (!dhcp || dhcp->state != DHCP_STATE_BOUND || !ip_valid) {
		//dhcp fail
		//need close socket and do dhcp again
		DiagPrintf("\r\ndhcp error\r\n");
	} else {
		//dhcp ok
		DiagPrintf("\r\ndhcp bound, ip=%s\r\n", ip4addr_ntoa(netif_ip4_addr(netif)));
	}
}
#endif

void wowlan_get_wifi_cfg(WOWLANCFG *cfg)
{
	memcpy(cfg, &wowlan_setting, sizeof(WOWLANCFG));
}

void wowlan_set_wifi_cfg(WOWLANCFG *cfg)
{
	memcpy(&wowlan_setting, cfg, sizeof(WOWLANCFG));
}

int wowlan_parser_wakeup_pattern(uint8_t *payload, uint32_t pkt_len, WOWLANCFG *param)
{
	int ret = RTK_SUCCESS;

	for (int i = 0; i < param->pattern_num; i++) {
		if (*(param->pattern_offset + i) + * (param->pattern_len + i) > pkt_len) {
			DiagPrintf("pattern %d invalid\n", i);
			continue;
		}

		if ((ret = memcmp(payload + * (param->pattern_offset + i), *(param->wakeup_pattern + i), *(param->pattern_len + i))) != 0) {
			DiagPrintf("\r\npattern no.%d not match! payload %s pattern %s\r\n", i, payload, *(param->wakeup_pattern + i));
		} else {
			DiagPrintf("\r\npattern no.%d match! payload %s pattern %s\r\n", i, payload, *(param->wakeup_pattern + i));
			return ret;
		}
	}

	return RTK_FAIL;
}

uint32_t wowlan_get_server_reconn_idle_ms(uint32_t retry_cnt)
{
	uint32_t delay_time = 0;
	uint32_t range = 0;
	uint32_t offset = 0;

	if (retry_cnt >= SERVER_RECONN_MAX_RETRY) {
		return SERVER_RECONN_MAX_MS;
	}

	delay_time = SERVER_RECONN_BASE_MS << retry_cnt;

	if (delay_time > SERVER_RECONN_MAX_MS) {
		delay_time = SERVER_RECONN_MAX_MS;
	}

	if (SERVER_RECONN_JITTER == 0) {
		return delay_time;
	}

	range = (delay_time * SERVER_RECONN_JITTER) / 100;

	offset = (rand() % ((range * 2) + 1)) - range;

	return (uint32_t)((int32_t)delay_time + offset);

}

void keep_alive_thread(void *param)
{
	struct netif *pnetif = lwip_idx_get_netif(0);
	int ret = RTK_SUCCESS;
	static int join_status = 0;
	static int wifi_reconn_to = 0;
	PIPC_MSG_STRUCT kacfg = NULL;
	srand((unsigned int)xTaskGetTickCount());

	//waiting for auto test cmd if need
	while (!goto_sleep) {
		vTaskDelay(1000);
	}

	whc_dev_api_set_host_state(WHC_HOST_UNREADY);

	//select keep alive type
	switch (wowlan_setting.type) {
	case TCP_PROTOCOL_KA:
		ka_cb.handler = tcp_protocol_keep_alive_handler;
		ka_cb.error_recover = tcp_protocol_keep_alive_error_handler;
		wowlan_close_server_conn_cb = tcp_protocol_keep_alive_close_conn;
		if (!use_default) {
			tcp_protocol_keep_alive_set_server_ip_port(server_ip, server_port);
		}
		break;
	case TCP_TLS_KA:
		ka_cb.handler = tcp_tls_keep_alive_handler;
		ka_cb.error_recover = tcp_tls_keep_alive_error_handler;
		wowlan_close_server_conn_cb = tcp_tls_keep_alive_close_conn;
		if (!use_default) {
			tcp_tls_keep_alive_set_server_ip_port(server_ip, server_port);
		}
		break;
	case MQTT_KA:
		ka_cb.handler = mqtt_keep_alive_handler;
		ka_cb.error_recover = mqtt_keep_alive_error_handler;
		wowlan_close_server_conn_cb = mqtt_keep_alive_close_conn;
		if (!use_default) {
			mqtt_keep_alive_set_server_ip_port(server_ip, server_port);
		}
		break;
	case WSS_KA:
		ka_cb.handler = wss_keep_alive_handler;
		ka_cb.error_recover = wss_keep_alive_error_handler;
		wowlan_close_server_conn_cb = wss_keep_alive_close_conn;
		if (!use_default) {
			wss_keep_alive_set_server_ip_port(server_ip, server_port);
		}
		break;
	default:
		DiagPrintf("\r\nkeep alive type not support\r\n");
		goto exit;
		break;
	}

	// Delay to check successful WiFi connection and obtain of an IP address
	while (lwip_check_connectivity(NETIF_WLAN_STA_INDEX) != CONNECTION_VALID) {
		vTaskDelay(2000);
	}

#if WOWLAN_DHCP_ERR_HANDLEER
	//set dhcp monitor
	netif_set_status_callback(pnetif, dhcp_state_cb);
#endif

	resumesemaphore = xSemaphoreCreateBinary();
	if (resumesemaphore == NULL) {
		DiagPrintf("\r\ncreate ka semaphore fail\r\n");
		return;
	}
#if AUTO_TEST
	wowlan_setting.dtim = auto_test_dtim;
#endif
	//km4 keep alive handler
	while (1) {
		//set dtim
		wifi_set_lps_listen_interval(wowlan_setting.dtim);
		wifi_hal_wlan_timer_tickless_process(1);
		ret = ka_cb.handler(&wowlan_setting);
		if (ret != RTK_SUCCESS) {
			wifi_hal_wlan_timer_tickless_process(0);
			//change to normal lps mode dtim1
			wifi_set_lps_listen_interval(1);
			//error handler
			if (wowlan_setting.fp_wakeup_reason == 0) {
				ret = ka_cb.error_recover(&wowlan_setting);
				if (ret != RTK_SUCCESS) {
					//wakeup CA32
					//L3 error
					//need close socket
					pmu_set_power_event(EVENT_ENTER_ACTIVE);
					break;
				} else {
					//keep alive recover success
					wowlan_setting.np_wakeup_reason = 0;
					wowlan_setting.resume = 1;
					DiagPrintf("\r\nka server reconnect successfully\r\n");
				}
			} else {
				extern u8 rtw_join_status;
				rtw_join_status = RTW_JOINSTATUS_DISCONNECT;
				while (1) {
					wifi_reconn_to++;
					wifi_get_join_status(&join_status);
					vTaskDelay(1000);
					if (join_status == RTW_JOINSTATUS_SUCCESS) {
						DiagPrintf("\r\nwifi reconnect success\r\n");
						join_status = 0;
						wifi_reconn_to = 0;
						wowlan_setting.fp_wakeup_reason = 0;
						while (lwip_check_connectivity(NETIF_WLAN_STA_INDEX) != CONNECTION_VALID) {
							vTaskDelay(2000);
						}
						break;
					}
					if (wifi_reconn_to > WIFI_RECONN_TO_SECOND) {
						DiagPrintf("\r\nFP wakeup NP, wifi reconnect timeout\r\n");
						wifi_hal_wlan_timer_tickless_process(0);
						//change to normal lps mode dtim1
						wifi_set_lps_listen_interval(1);
						//wakeup ca32
						pmu_set_power_event(EVENT_ENTER_ACTIVE);
						goto exit2;
					}
				}
			}
		} else {
			DiagPrintf("\r\nreceive wakeup packet\r\n");
			wifi_hal_wlan_timer_tickless_process(0);
			//change to normal lps mode dtim1
			wifi_set_lps_listen_interval(1);
			//wakeup ca32
			pmu_set_power_event(EVENT_ENTER_ACTIVE);
			if (xSemaphoreTake(resumesemaphore, portMAX_DELAY) == pdTRUE) {
				whc_dev_api_set_host_state(WHC_HOST_UNREADY);
				DiagPrintf("\r\nconnection resume\r\n");
			}
		}
		vTaskDelay(10);
	}
exit2:
	vSemaphoreDelete(resumesemaphore);
exit:
	vTaskDelete(NULL);
}

u32 cmd_set_server_ip_and_port(u16 argc, u8 *argv[])
{
	if (argc != 2)	{
		DiagPrintf("Usage: <ip address> <port>\n");
		return 0;
	}

	sprintf(server_ip, "%s", argv[0]);
	server_port = strtoul(argv[1], NULL, 10);

	DiagPrintf("\r\nserver_ip %s server_port %d\r\n", server_ip, server_port);
	goto_sleep = 1;

	return 0;
}

#if AUTO_TEST
u32 cmd_set_dtim(u16 argc, u8 *argv[])
{
	auto_test_dtim = strtoul(argv[0], NULL, 10);
	DiagPrintf("\r\nset dtim %d\r\n", auto_test_dtim);
	return 0;
}
#endif

CMD_TABLE_DATA_SECTION
COMMAND_TABLE __attribute__((unused)) shell_np_ka_cmd_table[] = {
#if AUTO_TEST
	{ "dtim", cmd_set_dtim },
#endif
	{ "keepalive", cmd_set_server_ip_and_port }
};

