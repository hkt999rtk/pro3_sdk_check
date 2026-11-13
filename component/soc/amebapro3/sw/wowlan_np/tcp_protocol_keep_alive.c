/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "lwip_netconf.h"
#include "wowlan_ipccfg.h"
#include "keep_alive.h"

static int server_socket;
static char server_ip[16] = "192.168.50.62";
static uint16_t server_port = 8899;

void tcp_protocol_keep_alive_set_server_ip_port(const char *ip, uint16_t port)
{
	strncpy(server_ip, ip, sizeof(server_ip) - 1);
	server_ip[sizeof(server_ip) - 1] = '\0';
	server_port = port;
}

int tcp_protocol_keep_alive_reconn(WOWLANCFG *param)
{
	int ret = RTK_SUCCESS;

	DiagPrintf("\r\ntcp_protocol_keep_alive_reconn\r\n");

#if !LWIP_TCP_KEEPALIVE
	DiagPrintf("\nPlease enable LWIP_TCP_KEEPALIVE\n");
	ret = RTK_FAIL;
#else

	int keepalive = 1, keepalive_idle = param->interval, keepalive_interval = param->retry_interval, keepalive_count = param->retry_cnt;
	struct sockaddr_in server_addr;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0) {
		DiagPrintf("create socket fail\n");
		ret = RTK_FAIL;
		goto exit;
	}

	if (setsockopt(server_socket, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive)) != 0) {
		DiagPrintf("ERROR: SO_KEEPALIVE\n");
		ret = RTK_FAIL;
		goto exit;
	}
	if (setsockopt(server_socket, IPPROTO_TCP, TCP_KEEPIDLE, &keepalive_idle, sizeof(keepalive_idle)) != 0) {
		DiagPrintf("ERROR: TCP_KEEPIDLE\n");
		ret = RTK_FAIL;
		goto exit;
	}
	if (setsockopt(server_socket, IPPROTO_TCP, TCP_KEEPINTVL, &keepalive_interval, sizeof(keepalive_interval)) != 0) {
		DiagPrintf("ERROR: TCP_KEEPINTVL\n");
		ret = RTK_FAIL;
		goto exit;
	}
	if (setsockopt(server_socket, IPPROTO_TCP, TCP_KEEPCNT, &keepalive_count, sizeof(keepalive_count)) != 0) {
		DiagPrintf("ERROR: TCP_KEEPCNT\n");
		ret = RTK_FAIL;
		goto exit;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(server_ip);
	server_addr.sin_port = htons(server_port);

	if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0) {
		DiagPrintf("reconnect pass\n");
	} else {
		ret = RTK_FAIL;
		DiagPrintf("reconnect fail\n");
	}
#endif

exit:
	if (ret == RTK_FAIL) {
		close(server_socket);
	}

	return ret;
}

int tcp_protocol_keep_alive_handler(WOWLANCFG *param)
{
	int ret = RTK_SUCCESS;
	unsigned char response_buf[100];
	int read_size = 0;

	DiagPrintf("\r\ntcp_protocol_keep_alive_handler\r\n");

#if !LWIP_TCP_KEEPALIVE
	DiagPrintf("\nPlease enable LWIP_TCP_KEEPALIVE\n");
	ret = RTK_FAIL;
#else

	if (param->resume) {
		param->resume = 0;
		DiagPrintf("\r\ngoto resume:\r\n");
		goto resume;
	}

	int keepalive = 1, keepalive_idle = param->interval, keepalive_interval = param->retry_interval, keepalive_count = param->retry_cnt;
	struct sockaddr_in server_addr;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0) {
		DiagPrintf("create socket fail\n");
		ret = RTK_FAIL;
		goto exit;
	}

	if (setsockopt(server_socket, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive)) != 0) {
		DiagPrintf("ERROR: SO_KEEPALIVE\n");
		ret = RTK_FAIL;
		goto exit;
	}
	if (setsockopt(server_socket, IPPROTO_TCP, TCP_KEEPIDLE, &keepalive_idle, sizeof(keepalive_idle)) != 0) {
		DiagPrintf("ERROR: TCP_KEEPIDLE\n");
		ret = RTK_FAIL;
		goto exit;
	}
	if (setsockopt(server_socket, IPPROTO_TCP, TCP_KEEPINTVL, &keepalive_interval, sizeof(keepalive_interval)) != 0) {
		DiagPrintf("ERROR: TCP_KEEPINTVL\n");
		ret = RTK_FAIL;
		goto exit;
	}
	if (setsockopt(server_socket, IPPROTO_TCP, TCP_KEEPCNT, &keepalive_count, sizeof(keepalive_count)) != 0) {
		DiagPrintf("ERROR: TCP_KEEPCNT\n");
		ret = RTK_FAIL;
		goto exit;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(server_ip);
	server_addr.sin_port = htons(server_port);

	if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0) {
		DiagPrintf("connect OK\n");
resume:
		//release wakelock to enter standby mode
		pmu_release_wakelock(PMU_OS);
		pmu_release_wakelock(PMU_WHC_WIFI);

		while (1) {
			read_size = read(server_socket, response_buf, sizeof(response_buf));
			if (read_size > 0) {
				if (wowlan_parser_wakeup_pattern(response_buf, read_size, param) == RTK_SUCCESS) {
					param->np_wakeup_reason = RX_Wakeup_Pattern;
					break;
				}
			} else if (read_size == -1) {
				if (errno == ETIMEDOUT) {
					DiagPrintf("tcp protocol keep alive timeout...\n");
					param->np_wakeup_reason = errno;
					ret = RTK_FAIL;
					break;
				} else if (errno == ECONNRESET) {
					DiagPrintf("receive tcp reset...\n");
					param->np_wakeup_reason = errno;
					ret = RTK_FAIL;
					break;
				} else {
					DiagPrintf("other unexpected error...\n");
					param->np_wakeup_reason = errno;
					ret = RTK_FAIL;
					break;
				}
			} else if (read_size == 0) {
				DiagPrintf("receive tcp finish\n");
				param->np_wakeup_reason = RX_TCP_FIN;
				ret = RTK_FAIL;
				break;
			} else {
				perror("recv");
				ret = RTK_FAIL;
				break;
			}
		}
	} else {
		DiagPrintf("\r\nconnect fail\r\n");
		param->np_wakeup_reason = errno;
		ret = RTK_FAIL;
	}

exit:
	if (ret == RTK_FAIL) {
		if (param->fp_wakeup_reason == Rx_DeAuth_DisAssoc ||  param->fp_wakeup_reason == FWDecisionDisconnect) {
			param->np_wakeup_reason = 0;
		} else {
			tcp_protocol_keep_alive_close_conn();
		}
	}

	pmu_acquire_wakelock(PMU_OS);
	pmu_acquire_wakelock(PMU_WHC_WIFI);

#endif
	return ret;
}

int tcp_protocol_keep_alive_error_handler(WOWLANCFG *param)
{
	int reconn_to = 0;
	uint32_t reconn_idle_time = 0;
	int ret = RTK_SUCCESS;

	DiagPrintf("\r\ntcp_protocol_keep_alive_error_handler\r\n");

	for (reconn_to = 0; reconn_to < SERVER_RECONN_MAX_RETRY; reconn_to++) {
		reconn_idle_time = wowlan_get_server_reconn_idle_ms(reconn_to);
		vTaskDelay(reconn_idle_time);
		ret = tcp_protocol_keep_alive_reconn(param);
		if (ret == RTK_SUCCESS) {
			DiagPrintf("\r\nreconnection count = %d\r\n", reconn_to);
			break;
		}
	}

	return ret;
}

void tcp_protocol_keep_alive_close_conn(void)
{
	DiagPrintf("\r\ntcp_protocol_keep_alive_close_connection\r\n");
	close(server_socket);
}
