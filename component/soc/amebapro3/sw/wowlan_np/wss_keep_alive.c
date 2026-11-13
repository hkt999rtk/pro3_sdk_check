/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "FreeRTOS.h"
#include "wowlan_ipccfg.h"
#include "keep_alive.h"
#include "wsclient_api.h"

#define MAX_BUFF_LENGTH 2048

static wsclient_context *wsclient = NULL;
static char server_ip[16] = "192.168.50.62";
static uint16_t server_port = 443;
static int waiting_pong = 0;
static int pong_received = 0;
static uint8_t ws_rx_buffer[MAX_BUFF_LENGTH];
static int ws_rx_length = 0;
static int ws_rx_available = 0;

void wss_keep_alive_set_server_ip_port(const char *ip, uint16_t port)
{
	strncpy(server_ip, ip, sizeof(server_ip) - 1);
	server_ip[sizeof(server_ip) - 1] = '\0';
	server_port = port;
}

static void wss_data_cb(wsclient_context **wsc, int len, enum opcode_type opcode)
{
	wsclient_context *ws;

	if (opcode != TEXT_FRAME && opcode != BINARY_FRAME) {
		return;
	}
	if (wsc == NULL || *wsc == NULL) {
		return;
	}

	ws = *wsc;
	if (ws->receivedData == NULL || len <= 0) {
		return;
	}

	if (len <= MAX_BUFF_LENGTH) {
		memcpy(ws_rx_buffer, ws->receivedData, len);
		ws_rx_length = len;
		ws_rx_available = 1;
	}
}

static void wss_pong_cb(wsclient_context **wsc)
{
	(void)wsc;
	waiting_pong = 0;
	pong_received = 1;
}

int wss_keep_alive_reconn(WOWLANCFG *param)
{
	char url[128];

	waiting_pong = 0;
	pong_received = 0;

	DiagPrintf("\r\nwss_keep_alive_reconn\r\n");

	snprintf(url, sizeof(url), "wss://%s", server_ip);

	wsclient = create_wsclient(url, server_port, NULL, NULL, 64, MAX_BUFF_LENGTH, 5);
	if (wsclient == NULL) {
		DiagPrintf("\r\ncreate_wsclient failed\r\n");
		return RTK_FAIL;
	}

	if (ws_connect_url(wsclient) < 0) {
		DiagPrintf("\r\nws_connect_url failed\r\n");
		ws_free(wsclient);
		wsclient = NULL;
		return RTK_FAIL;
	}

	ws_dispatch(wss_data_cb);
	ws_pong(wss_pong_cb);

	DiagPrintf("\r\nwss reconn pass\r\n");
	return RTK_SUCCESS;
}

static int wss_send_ping(void)
{
	int ret;

	if (wsclient == NULL) {
		return RTK_FAIL;
	}

	ret = ws_sendPing(1, wsclient);
	if (ret < 0) {
		DiagPrintf("\r\nwss_sendPing failed\r\n");
		return RTK_FAIL;
	}
	return RTK_SUCCESS;
}

int wss_keep_alive_handler(WOWLANCFG *param)
{
	int ret = RTK_SUCCESS;
	int retry_count = 0;
	TickType_t current_time = 0;
	TickType_t next_event = 0;
	TickType_t ka_next_tx_time = 0;
	TickType_t ka_ack_deadline = 0;
	TickType_t remain_time = 0;
	char url[128];

	waiting_pong = 0;
	pong_received = 0;

	DiagPrintf("\r\nwss_keep_alive_handler\r\n");

	if (param->resume) {
		param->resume = 0;
		DiagPrintf("\r\ngoto resume:\r\n");
		goto resume;
	}

	snprintf(url, sizeof(url), "wss://%s", server_ip);

	wsclient = create_wsclient(url, server_port, NULL, NULL, 64, MAX_BUFF_LENGTH, 5);
	if (wsclient == NULL) {
		DiagPrintf("\r\ncreate_wsclient failed\r\n");
		ret = RTK_FAIL;
		goto exit2;
	}

	if (ws_connect_url(wsclient) < 0) {
		DiagPrintf("\r\nws_connect_url failed\r\n");
		ret = RTK_FAIL;
		goto exit;
	}

	ws_dispatch(wss_data_cb);
	ws_pong(wss_pong_cb);

resume:
	ws_rx_available = 0;
	current_time = xTaskGetTickCount();

	if (wss_send_ping() != RTK_SUCCESS) {
		param->np_wakeup_reason = RX_WSS_CONN_LOST;
		ret = RTK_FAIL;
		goto exit;
	}
	waiting_pong = 1;
	retry_count = 0;
	ka_ack_deadline = current_time + (param->retry_interval * 1000);
	ka_next_tx_time = current_time + (param->interval * 1000);
	DiagPrintf("\r\nsend first wss ping\r\n");

	while (1) {
		current_time = xTaskGetTickCount();

		if (waiting_pong) {
			next_event = ka_ack_deadline;
		} else {
			next_event = ka_next_tx_time;
			ka_ack_deadline = 0;
		}

		remain_time = (next_event > current_time) ? (next_event - current_time) : 1;

		pmu_release_wakelock(PMU_OS);
		pmu_release_wakelock(PMU_WHC_WIFI);

		ws_poll(remain_time, &wsclient);

		current_time = xTaskGetTickCount();

		if (wsclient == NULL || ws_getReadyState(wsclient) != WSC_OPEN) {
			DiagPrintf("\r\nWSS connection closed\r\n");
			param->np_wakeup_reason = RX_WSS_CONN_LOST;
			ret = RTK_FAIL;
			goto exit;
		}

		if (ws_rx_available) {
			ws_rx_available = 0;
			if (wowlan_parser_wakeup_pattern(ws_rx_buffer, ws_rx_length, param) == RTK_SUCCESS) {
				param->np_wakeup_reason = RX_Wakeup_Pattern;
				ret = RTK_SUCCESS;
				DiagPrintf("\r\nreceive wakeup pattern via WSS\r\n");
				goto exit;
			}
			continue;
		}

		if (pong_received) {
			pong_received = 0;
			waiting_pong = 0;
			retry_count = 0;
			ka_ack_deadline = 0;
			ka_next_tx_time = current_time + (param->interval * 1000);
			DiagPrintf("\r\nreceive pong\r\n");
			continue;
		}

		if (current_time >= next_event) {
			if (waiting_pong) {
				retry_count++;
				if (retry_count > param->retry_cnt) {
					DiagPrintf("\r\nWSS ping timeout, retry count exceeded\r\n");
					param->np_wakeup_reason = TX_TCP_APP_Retry_TO;
					ret = RTK_FAIL;
					goto exit;
				}
				if (wss_send_ping() != RTK_SUCCESS) {
					param->np_wakeup_reason = RX_WSS_CONN_LOST;
					ret = RTK_FAIL;
					goto exit;
				}
				ka_ack_deadline = current_time + (param->retry_interval * 1000);
				DiagPrintf("\r\nWSS ping retry %d\r\n", retry_count);
			} else {
				if (wss_send_ping() != RTK_SUCCESS) {
					param->np_wakeup_reason = RX_WSS_CONN_LOST;
					ret = RTK_FAIL;
					goto exit;
				}
				waiting_pong = 1;
				retry_count = 0;
				ka_ack_deadline = current_time + (param->retry_interval * 1000);
				DiagPrintf("\r\nsend wss ping\r\n");
			}
		}
	}

exit:
	if (ret == RTK_FAIL) {
		if (param->fp_wakeup_reason == Rx_DeAuth_DisAssoc
			|| param->fp_wakeup_reason == FWDecisionDisconnect) {
			param->np_wakeup_reason = 0;
		} else {
			if (wsclient) {
				if (ws_getReadyState(wsclient) == WSC_OPEN) {
					wsclient->fun_ops.client_close(wsclient);
				}
				ws_free(wsclient);
				wsclient = NULL;
			}
		}
	}
exit2:
	pmu_acquire_wakelock(PMU_OS);
	pmu_acquire_wakelock(PMU_WHC_WIFI);

	return ret;
}

int wss_keep_alive_error_handler(WOWLANCFG *param)
{
	int reconn_to = 0;
	uint32_t reconn_idle_time = 0;
	int ret = RTK_SUCCESS;

	DiagPrintf("\r\nwss_keep_alive_error_handler\r\n");

	for (reconn_to = 0; reconn_to < SERVER_RECONN_MAX_RETRY; reconn_to++) {
		reconn_idle_time = wowlan_get_server_reconn_idle_ms(reconn_to);
		vTaskDelay(reconn_idle_time);
		ret = wss_keep_alive_reconn(param);
		if (ret == RTK_SUCCESS) {
			DiagPrintf("\r\nWSS reconnection count = %d\r\n", reconn_to);
			break;
		}
	}

	return ret;
}

void wss_keep_alive_close_conn(void)
{
	DiagPrintf("\r\nwss_keep_alive_close_conn\r\n");

	if (wsclient) {
		wsclient->fun_ops.client_close(wsclient);
		ws_free(wsclient);
		wsclient = NULL;
	}
}
