/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _KEEP_ALIVE_HANDLER_H_
#define _KEEP_ALIVE_HANDLER_H_

#include "ameba_soc.h"
#include "wowlan_ipccfg.h"
#include "FreeRTOS.h"
#include "semphr.h"

//fp wakeup reason
#define Rx_DeAuth_DisAssoc		0x08
#define FWDecisionDisconnect	0x10
//np wakeup reason
#define RX_TCP_FIN				0xFD
#define RX_Wakeup_Pattern		0xFE
#define TX_TCP_APP_Retry_TO		0xFC
#define RX_WSS_CONN_LOST		0xFB
//ka server reconnect param
#define SERVER_RECONN_BASE_MS	1000
#define SERVER_RECONN_MAX_MS	300000
#define SERVER_RECONN_MAX_RETRY	10
#define SERVER_RECONN_JITTER	20

typedef struct {
	int (*handler)(WOWLANCFG *config);
	int (*error_recover)(WOWLANCFG *config);
} WOWLANCB;

extern SemaphoreHandle_t resumesemaphore;
extern void (*wowlan_close_server_conn_cb)(void);

void keep_alive_thread(void *param);
int wowlan_parser_wakeup_pattern(uint8_t *payload, uint32_t pkt_len, WOWLANCFG *param);
uint32_t wowlan_get_server_reconn_idle_ms(uint32_t retry_cnt);
int tcp_protocol_keep_alive_handler(WOWLANCFG *param);
int tcp_protocol_keep_alive_error_handler(WOWLANCFG *param);
void tcp_protocol_keep_alive_close_conn(void);
void tcp_protocol_keep_alive_set_server_ip_port(const char *ip, uint16_t port);
int tcp_tls_keep_alive_handler(WOWLANCFG *param);
int tcp_tls_keep_alive_error_handler(WOWLANCFG *param);
void tcp_tls_keep_alive_close_conn(void);
void tcp_tls_keep_alive_set_server_ip_port(const char *ip, uint16_t port);
int mqtt_keep_alive_handler(WOWLANCFG *param);
int mqtt_keep_alive_reconn(WOWLANCFG *param);
int mqtt_keep_alive_error_handler(WOWLANCFG *param);
void mqtt_keep_alive_close_conn(void);
void mqtt_keep_alive_set_server_ip_port(const char *ip, uint16_t port);
int wss_keep_alive_handler(WOWLANCFG *param);
int wss_keep_alive_reconn(WOWLANCFG *param);
int wss_keep_alive_error_handler(WOWLANCFG *param);
void wss_keep_alive_close_conn(void);
void wss_keep_alive_set_server_ip_port(const char *ip, uint16_t port);

#endif
