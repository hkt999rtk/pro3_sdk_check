/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_WOWLAN_IPCCFG_H_
#define _AMEBA_WOWLAN_IPCCFG_H_

#include "ameba_soc.h"

#define MAX_WAKEUP_PATTERNS_NUM		8
#define MAX_WAKEUP_PATTERN_LENGTH	64
//fp wakeup reason
#define Rx_DeAuth_DisAssoc		0x08
#define FWDecisionDisconnect	0x10
//np wakeup reason
#define RX_TCP_FIN				0xFD
#define RX_Wakeup_Pattern		0xFE
#define TX_TCP_APP_Retry_TO		0xFC
#define RX_WSS_CONN_LOST		0xFB

enum keep_alive_type {
	TCP_PROTOCOL_KA = 0,
	TCP_TLS_KA,
	MQTT_KA,
	WSS_KA,
	TOTAL_KA_NUM
};

enum kaipc_msg_type {
	A2N_WOWLAN_CFG = 0,
	A2N_AP_WAKEUP,
	N2A_RECV_WAKEUP_REASON,
	TOTAL_MSG_TYPE_NUM
};

typedef struct {
	const char *pattern;
	uint8_t len;
	uint8_t offset;
} WOWLANPATTERN;

typedef struct {
	uint8_t dtim;
	uint8_t type;
	uint32_t interval;
	uint32_t retry_interval;
	uint32_t retry_cnt;
	char wakeup_pattern[MAX_WAKEUP_PATTERNS_NUM][MAX_WAKEUP_PATTERN_LENGTH];
	uint8_t pattern_num;
	uint8_t pattern_len[MAX_WAKEUP_PATTERNS_NUM];
	uint32_t pattern_offset[MAX_WAKEUP_PATTERNS_NUM];
	uint8_t fp_wakeup_reason;
	uint8_t np_wakeup_reason;
	uint8_t resume;
} WOWLANCFG;

void wowlan_get_wakeup_reason(WOWLANCFG *param);
void wowlan_set_default_parameter(WOWLANCFG *param);
void wowlan_enable(WOWLANCFG *param);
void wowlan_ap_wakeup_notification(void);

#endif
