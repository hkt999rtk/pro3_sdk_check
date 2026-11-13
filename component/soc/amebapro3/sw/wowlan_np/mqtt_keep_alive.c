/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include "FreeRTOS.h"
#include "lwip_netconf.h"
#include "MQTTClient.h"
#include "MQTTFreertos.h"
#include "wowlan_ipccfg.h"
#include "keep_alive.h"

#define MAX_BUFF_LENGTH	2048

static MQTTClient mqtt_client;
static Network mqtt_network;
static unsigned char sendbuf[MAX_BUFF_LENGTH];
static unsigned char readbuf[MAX_BUFF_LENGTH];
static char server_ip[16] = "192.168.50.62";
static uint16_t server_port = 8883;

static uint8_t mqtt_rx_payload[MAX_BUFF_LENGTH];
static int mqtt_rx_len = 0;
static int mqtt_rx_available = 0;

static void mqtt_msg_rx(MessageData *data)
{
	int payload_len = data->message->payloadlen;

	if (payload_len <= MAX_BUFF_LENGTH) {
		memcpy(mqtt_rx_payload, data->message->payload, payload_len);
		mqtt_rx_len = payload_len;
		mqtt_rx_available = 1;
	}
}

void mqtt_keep_alive_set_server_ip_port(const char *ip, uint16_t port)
{
	strncpy(server_ip, ip, sizeof(server_ip) - 1);
	server_ip[sizeof(server_ip) - 1] = '\0';
	server_port = port;
}

int mqtt_keep_alive_reconn(WOWLANCFG *param)
{
	int ret = RTK_SUCCESS;
	MQTTPacket_connectData connect = MQTTPacket_connectData_initializer;

	DiagPrintf("\r\nmqtt_keep_alive_reconn\r\n");

	NetworkInit(&mqtt_network);

	mqtt_network.use_ssl = 1;
	mqtt_network.rootCA = NULL;
	mqtt_network.clientCA = NULL;
	mqtt_network.private_key = NULL;
	mqtt_network.ciphersuites = NULL;

	ret = NetworkConnect(&mqtt_network, server_ip, server_port);
	if (ret != 0) {
		DiagPrintf("\r\nMQTT reconn TCP/SSL connect fail %d\r\n", ret);
		return RTK_FAIL;
	}

	MQTTClientInit(&mqtt_client, &mqtt_network, 30000,
				   sendbuf, sizeof(sendbuf), readbuf, sizeof(readbuf));

	connect.MQTTVersion = 3;
	connect.keepAliveInterval = param->interval;
	connect.clientID.lenstring.data = (char *)"ameba-iot";
	connect.clientID.lenstring.len = 9;
	connect.cleansession = 1;

	ret = MQTTConnect(&mqtt_client, &connect);
	if (ret != 0) {
		DiagPrintf("\r\nMQTT reconn MQTT connect fail %d\r\n", ret);
		mqtt_network.disconnect(&mqtt_network);
		return RTK_FAIL;
	}

	ret = MQTTSubscribe(&mqtt_client, "wakeup", QOS1, mqtt_msg_rx);
	if (ret != 0) {
		DiagPrintf("\r\nMQTT reconn subscribe fail %d\r\n", ret);
		MQTTDisconnect(&mqtt_client);
		mqtt_network.disconnect(&mqtt_network);
		return RTK_FAIL;
	}

	DiagPrintf("\r\nmqtt reconn pass\r\n");
	return RTK_SUCCESS;
}

int mqtt_keep_alive_handler(WOWLANCFG *param)
{
	int ret = RTK_SUCCESS;
	MQTTPacket_connectData connect = MQTTPacket_connectData_initializer;

	DiagPrintf("\r\nmqtt_keep_alive_handler\r\n");

	if (param->resume) {
		param->resume = 0;
		DiagPrintf("\r\ngoto resume:\r\n");
		goto resume;
	}

	NetworkInit(&mqtt_network);

	mqtt_network.use_ssl = 1;
	mqtt_network.rootCA = NULL;
	mqtt_network.clientCA = NULL;
	mqtt_network.private_key = NULL;
	mqtt_network.ciphersuites = NULL;

	ret = NetworkConnect(&mqtt_network, server_ip, server_port);
	if (ret != 0) {
		DiagPrintf("\r\nMQTT initial TCP/SSL connect fail %d\r\n", ret);
		goto exit2;
	}

	MQTTClientInit(&mqtt_client, &mqtt_network, 30000,
				   sendbuf, sizeof(sendbuf), readbuf, sizeof(readbuf));

	connect.MQTTVersion = 3;
	connect.keepAliveInterval = param->interval;
	connect.clientID.lenstring.data = (char *)"ameba-iot";
	connect.clientID.lenstring.len = 9;
	connect.cleansession = 1;

	ret = MQTTConnect(&mqtt_client, &connect);
	if (ret != 0) {
		DiagPrintf("\r\nMQTT initial MQTT connect fail %d\r\n", ret);
		mqtt_network.disconnect(&mqtt_network);
		goto exit2;
	}

	ret = MQTTSubscribe(&mqtt_client, "wakeup", QOS1, mqtt_msg_rx);
	if (ret != 0) {
		DiagPrintf("\r\nMQTT initial subscribe fail %d\r\n", ret);
		MQTTDisconnect(&mqtt_client);
		mqtt_network.disconnect(&mqtt_network);
		goto exit2;
	}

resume:
	mqtt_rx_available = 0;

	DiagPrintf("\r\nmqtt_keep_alive_handler start\r\n");

	pmu_release_wakelock(PMU_OS);
	pmu_release_wakelock(PMU_WHC_WIFI);

	while (1) {
		ret = MQTTYield(&mqtt_client, (param->interval * 1000));
		if (!mqtt_client.isconnected) {
			DiagPrintf("\r\nMQTT disconnected\r\n");
			param->np_wakeup_reason = errno;
			ret = RTK_FAIL;
			goto exit;
		}

		if (mqtt_rx_available) {
			mqtt_rx_available = 0;
			if (wowlan_parser_wakeup_pattern(mqtt_rx_payload, mqtt_rx_len, param) == RTK_SUCCESS) {
				param->np_wakeup_reason = RX_Wakeup_Pattern;
				ret = RTK_SUCCESS;
				DiagPrintf("\r\nreceive wakeup pattern via MQTT\r\n");
				goto exit;
			}
			continue;
		}

		if (mqtt_client.ping_outstanding && TimerLeftMS(&mqtt_client.ping_timer) == 0) {
			DiagPrintf("\r\nMQTT ping timeout\r\n");
			param->np_wakeup_reason = TX_TCP_APP_Retry_TO;
			ret = RTK_FAIL;
			goto exit;
		}
	}

exit:
	if (ret == RTK_FAIL) {
		if (param->fp_wakeup_reason == Rx_DeAuth_DisAssoc || param->fp_wakeup_reason == FWDecisionDisconnect) {
			param->np_wakeup_reason = 0;
		} else {
			MQTTDisconnect(&mqtt_client);
			mqtt_network.disconnect(&mqtt_network);
		}
	}
exit2:
	pmu_acquire_wakelock(PMU_OS);
	pmu_acquire_wakelock(PMU_WHC_WIFI);

	mqtt_rx_available = 0;
	return ret;
}

int mqtt_keep_alive_error_handler(WOWLANCFG *param)
{
	int reconn_to = 0;
	uint32_t reconn_idle_time = 0;
	int ret = RTK_SUCCESS;

	DiagPrintf("\r\nmqtt_keep_alive_error_handler\r\n");

	for (reconn_to = 0; reconn_to < SERVER_RECONN_MAX_RETRY; reconn_to++) {
		reconn_idle_time = wowlan_get_server_reconn_idle_ms(reconn_to);
		vTaskDelay(reconn_idle_time);
		ret = mqtt_keep_alive_reconn(param);
		if (ret == RTK_SUCCESS) {
			DiagPrintf("\r\nMQTT reconnection count = %d\r\n", reconn_to);
			break;
		}
	}

	return ret;
}

void mqtt_keep_alive_close_conn(void)
{
	DiagPrintf("\r\nmqtt_keep_alive_close_conn\r\n");
	MQTTDisconnect(&mqtt_client);
	mqtt_network.disconnect(&mqtt_network);
}
