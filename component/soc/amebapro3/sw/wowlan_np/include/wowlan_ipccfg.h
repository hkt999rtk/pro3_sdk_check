/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_WOWLAN_IPCCFG_H_
#define _AMEBA_WOWLAN_IPCCFG_H_

#include "ameba_soc.h"

#define IPC_AP_NOTIFY_BIT   (1 << 0)
#define IPC_FP_NOTIFY_BIT   (1 << 1)
#define MAX_WAKEUP_PATTERNS_NUM		8
#define MAX_WAKEUP_PATTERN_LENGTH	64

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

void wowlan_report_wakeup_reason(WOWLANCFG *param);
void wowlan_get_wifi_cfg(WOWLANCFG *cfg);
void wowlan_set_wifi_cfg(WOWLANCFG *cfg);

#endif
