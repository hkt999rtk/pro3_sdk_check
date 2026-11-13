/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "FreeRTOS.h"
#include "lwip_netconf.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/net_sockets.h"
#include "wowlan_ipccfg.h"
#include "keep_alive.h"

#define NEED_RETRY		1
#define NEED_WAIT		2
#define WAKEUP_PKT		3
#define KA_ACK			4
#define	OTHER_PKT		5
#define MAX_BUFF_LENGTH	2048

static int server_socket;
static mbedtls_ssl_context ssl;
static mbedtls_ssl_config conf;
static mbedtls_entropy_context entropy;
static mbedtls_ctr_drbg_context ctr_drbg;
static mbedtls_net_context net;
const char *keepalive_pattern = "keepalive";
static char server_ip[16] = "192.168.50.62";
static uint16_t server_port = 8443;

void tcp_tls_keep_alive_set_server_ip_port(const char *ip, uint16_t port)
{
	strncpy(server_ip, ip, sizeof(server_ip) - 1);
	server_ip[sizeof(server_ip) - 1] = '\0';
	server_port = port;
}

int tcp_tls_keep_alive_reconn(WOWLANCFG *param)
{
	int ret = RTK_SUCCESS;
	struct timeval tv;
	struct sockaddr_in server_addr;

	DiagPrintf("\r\ntcp_tls_keep_alive_reconn\r\n");

#if !LWIP_TCP_KEEPALIVE
	DiagPrintf("\nPlease enable LWIP_TCP_KEEPALIVE\n");
	ret = RTK_FAIL;
#else
	//tcp connection
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	//non-blocking
	fcntl(server_socket, F_SETFL, O_NONBLOCK);
	net.fd = server_socket;

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);
	inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

	ret = connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret < 0) {
		if (errno != EINPROGRESS) {
			DiagPrintf("\r\nconnect fail %d\n", errno);
			param->np_wakeup_reason = errno;
			ret = RTK_FAIL;
			goto exit;
		}

		fd_set wfds;
		FD_ZERO(&wfds);
		FD_SET(server_socket, &wfds);
		struct timeval tv = {5, 0};
		ret = select(server_socket + 1, NULL, &wfds, NULL, &tv);
		if (ret <= 0) {
			DiagPrintf("\r\nconnect timeout\n");
			param->np_wakeup_reason = errno;
			ret = RTK_FAIL;
			goto exit;
		}

		int err;
		socklen_t len = sizeof(err);
		getsockopt(server_socket, SOL_SOCKET, SO_ERROR, &err, &len);

		if (err != 0) {
			DiagPrintf("\r\nconnect error %d\n", err);
			param->np_wakeup_reason = err;
			ret = RTK_FAIL;
			goto exit;
		}
	}

	//mbedtls init
	mbedtls_ssl_init(&ssl);
	mbedtls_ssl_config_init(&conf);
	mbedtls_entropy_init(&entropy);
	mbedtls_ctr_drbg_init(&ctr_drbg);
	mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)"ameba-iot", 9);
	mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
	mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
	mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
	mbedtls_ssl_setup(&ssl, &conf);
	mbedtls_ssl_set_bio(&ssl, &net, mbedtls_net_send, mbedtls_net_recv, NULL);
	mbedtls_ssl_conf_read_timeout(&conf, 0);//non-blocking

	//mbedtls handshake
	while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
		if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(server_socket, &fds);

			if (ret == MBEDTLS_ERR_SSL_WANT_READ) {
				select(server_socket + 1, &fds, NULL, NULL, NULL);
			} else {
				select(server_socket + 1, NULL, &fds, NULL, NULL);
			}
			continue;
		}
		DiagPrintf("\r\nmbedtls handshake fail %d\r\n", ret);
		ret = RTK_FAIL;
		break;
	}
#endif
	if (ret == RTK_FAIL) {
		mbedtls_ssl_free(&ssl);
		mbedtls_ssl_config_free(&conf);
		mbedtls_ctr_drbg_free(&ctr_drbg);
		mbedtls_entropy_free(&entropy);
		mbedtls_net_free(&net);
exit:
		close(server_socket);
	} else {
		DiagPrintf("\r\nreconn pass\n");
	}
	return ret;
}

int tcp_tls_ka_send(mbedtls_ssl_context *ssl)
{
	int ret = RTK_SUCCESS;
	char buf[64];

	ret = mbedtls_ssl_write(ssl, (const unsigned char *)keepalive_pattern, strlen(keepalive_pattern));
	if (ret > 0) {
		return ret;
	} else if (ret == MBEDTLS_ERR_SSL_WANT_WRITE || ret == MBEDTLS_ERR_SSL_WANT_READ) {
		return NEED_RETRY;
	} else {
		return RTK_FAIL;
	}


}

int tcp_tls_ka_recv(mbedtls_ssl_context *ssl, WOWLANCFG *param, uint8_t *buf, int buf_len, int *read_len)
{
	int ret;

	ret = mbedtls_ssl_read(ssl, buf, buf_len - 1);
	if (ret > 0) {
		buf[ret] = '\0';
		*read_len = ret;
		if (memcmp(buf, keepalive_pattern, strlen(keepalive_pattern)) == 0) {
			return KA_ACK;
		}
		if (wowlan_parser_wakeup_pattern(buf, *read_len, param) == RTK_SUCCESS) {
			return WAKEUP_PKT;
		}
		return OTHER_PKT;
	} else if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
		return NEED_RETRY;
	} else if (ret == 0) {
		return NEED_WAIT;
	}

	return RTK_FAIL;
}

int tcp_tls_keep_alive_handler(WOWLANCFG *param)
{
	int ret = RTK_SUCCESS;
	int retry_count = 0;
	TickType_t current_time = 0;
	TickType_t next_event = 0;
	TickType_t ka_next_tx_time = 0;
	TickType_t ka_ack_deadline = 0;
	TickType_t remain_time = 0;
	int waiting_ack = 0;
	fd_set readfds;
	struct timeval tv;
	int sel_ret = 0;
	static uint8_t recv_buff[MAX_BUFF_LENGTH];
	uint32_t buff_len = MAX_BUFF_LENGTH;
	uint32_t data_len = 0;
	struct sockaddr_in server_addr;

	DiagPrintf("\r\ntcp_tls_keep_alive_handler\r\n");

#if !LWIP_TCP_KEEPALIVE
	DiagPrintf("\nPlease enable LWIP_TCP_KEEPALIVE\n");
#else
	if (param->resume) {
		param->resume = 0;
		DiagPrintf("\r\ngoto resume:\r\n");
		goto resume;
	}
	//tcp connection
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	net.fd = server_socket;

	//non-blocking
	fcntl(server_socket, F_SETFL, O_NONBLOCK);

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);
	DiagPrintf("\r\nip = %s port = %d\r\n", server_ip, server_port);
	inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

	ret = connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret < 0) {
		if (errno != EINPROGRESS) {
			DiagPrintf("\r\nconnect fail %d\n", errno);
			param->np_wakeup_reason = errno;
			ret = RTK_FAIL;
			goto exit2;
		}

		fd_set wfds;
		FD_ZERO(&wfds);
		FD_SET(server_socket, &wfds);
		struct timeval tv = {5, 0};
		ret = select(server_socket + 1, NULL, &wfds, NULL, &tv);
		if (ret <= 0) {
			DiagPrintf("\r\nconnect timeout\n");
			param->np_wakeup_reason = errno;
			ret = RTK_FAIL;
			goto exit2;
		}

		int err;
		socklen_t len = sizeof(err);
		getsockopt(server_socket, SOL_SOCKET, SO_ERROR, &err, &len);

		if (err != 0) {
			DiagPrintf("\r\nconnect error %d\n", err);
			param->np_wakeup_reason = err;
			ret = RTK_FAIL;
			goto exit2;
		}
	}

	//mbedtls init
	mbedtls_ssl_init(&ssl);
	mbedtls_ssl_config_init(&conf);
	mbedtls_entropy_init(&entropy);
	mbedtls_ctr_drbg_init(&ctr_drbg);
	mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)"ameba-iot", 9);
	mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
	mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
	mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
	mbedtls_ssl_setup(&ssl, &conf);
	mbedtls_ssl_set_bio(&ssl, &net, mbedtls_net_send, mbedtls_net_recv, NULL);
	mbedtls_ssl_conf_read_timeout(&conf, 0);//non-blocking

	//mbedtls handshake
	while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
		if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(server_socket, &fds);

			if (ret == MBEDTLS_ERR_SSL_WANT_READ) {
				select(server_socket + 1, &fds, NULL, NULL, NULL);
			} else {
				select(server_socket + 1, NULL, &fds, NULL, NULL);
			}
			continue;
		}
		ret = RTK_FAIL;
		goto exit;
	}
resume:
	current_time = xTaskGetTickCount();
	ret = tcp_tls_ka_send(&ssl);
	if (ret == RTK_FAIL) {
		param->np_wakeup_reason = errno;
		goto exit;
	}
	DiagPrintf("\r\nsend first ka\r\n");
	waiting_ack = 1;
	retry_count = 0;
	ka_ack_deadline = current_time + (param->retry_interval * 1000);
	ka_next_tx_time = current_time + (param->interval * 1000);

	while (1) {
		FD_ZERO(&readfds);
		FD_SET(server_socket, &readfds);
		current_time = xTaskGetTickCount();
		if (waiting_ack) {
			next_event = ka_ack_deadline;
		} else {
			next_event = ka_next_tx_time;
			ka_ack_deadline = 0;
		}

		remain_time = (next_event > current_time) ? (next_event - current_time) : 0;
		if (remain_time <= 0) {
			remain_time = 1;
		}
		tv.tv_sec  = remain_time / 1000;
		tv.tv_usec = (remain_time % 1000) * 1000;

		//release wakelock to enter standby mode
		pmu_release_wakelock(PMU_OS);
		pmu_release_wakelock(PMU_WHC_WIFI);

		sel_ret = select(server_socket + 1, &readfds, NULL, NULL, &tv);

		current_time = xTaskGetTickCount();

		if (sel_ret > 0 && FD_ISSET(server_socket, &readfds)) {
			//receive any packets
			ret = tcp_tls_ka_recv(&ssl, param, recv_buff, buff_len, &data_len);
			if (ret == RTK_FAIL) {
				param->np_wakeup_reason = errno;
				goto exit;
			} else if (ret == KA_ACK) {
				waiting_ack = 0;
				retry_count = 0;
				ka_ack_deadline = 0;
				ka_next_tx_time = current_time + (param->interval * 1000);
				DiagPrintf("\r\nreceive ka ack\r\n");
			} else if (ret == WAKEUP_PKT) {
				param->np_wakeup_reason = RX_Wakeup_Pattern;
				ret = RTK_SUCCESS;
				DiagPrintf("\r\nreceive wakeup pattern\r\n");
				goto exit;
			}
			continue;
		} else if (sel_ret == 0) {
			if (waiting_ack) {
				//keep alive retry
				retry_count++;
				if (retry_count > param->retry_cnt) {
					param->np_wakeup_reason = TX_TCP_APP_Retry_TO;
					ret = RTK_FAIL;
					goto exit;
				}
				ret = tcp_tls_ka_send(&ssl);
				if (ret == RTK_FAIL) {
					param->np_wakeup_reason = errno;
					goto exit;
				}
				ka_ack_deadline = current_time + (param->retry_interval * 1000);
			} else {
				//period keep alive
				ret = tcp_tls_ka_send(&ssl);
				if (ret == RTK_FAIL) {
					param->np_wakeup_reason = errno;
					goto exit;
				}
				waiting_ack = 1;
				retry_count = 0;
				ka_ack_deadline = current_time + (param->retry_interval * 1000);
			}
			continue;
		} else {
			if (errno == EINTR) {
				continue;
			}
			param->np_wakeup_reason = errno;
			ret = RTK_FAIL;
			goto exit;
		}
	}

exit:
	if (ret == RTK_FAIL) {
		if (param->fp_wakeup_reason == Rx_DeAuth_DisAssoc ||  param->fp_wakeup_reason == FWDecisionDisconnect) {
			param->np_wakeup_reason = 0;
		} else {
			mbedtls_ssl_free(&ssl);
			mbedtls_ssl_config_free(&conf);
			mbedtls_ctr_drbg_free(&ctr_drbg);
			mbedtls_entropy_free(&entropy);
			mbedtls_net_free(&net);
exit2:
			close(server_socket);
		}
	}
	pmu_acquire_wakelock(PMU_OS);
	pmu_acquire_wakelock(PMU_WHC_WIFI);

#endif
	return ret;
}

int tcp_tls_keep_alive_error_handler(WOWLANCFG *param)
{
	int reconn_to = 0;
	uint32_t reconn_idle_time = 0;
	int ret = RTK_SUCCESS;

	DiagPrintf("\r\ntcp_tls_keep_alive_error_handler\r\n");

	for (reconn_to = 0; reconn_to < SERVER_RECONN_MAX_RETRY; reconn_to++) {
		reconn_idle_time = wowlan_get_server_reconn_idle_ms(reconn_to);
		vTaskDelay(reconn_idle_time);
		ret = tcp_tls_keep_alive_reconn(param);
		if (ret == RTK_SUCCESS) {
			DiagPrintf("\r\nreconnection count = %d\r\n", reconn_to);
			break;
		}
	}

	return ret;
}

void tcp_tls_keep_alive_close_conn(void)
{
	DiagPrintf("\r\ntcp_tls_keep_alive_close_connection\r\n");
	close(server_socket);
	mbedtls_ssl_free(&ssl);
	mbedtls_ssl_config_free(&conf);
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);
	mbedtls_net_free(&net);
}
