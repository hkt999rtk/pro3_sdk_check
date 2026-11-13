/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "platform_autoconf.h"
#include <wifi_api.h>
#include "rtw_skbuff.h"
#if defined(CONFIG_WHC_HOST)
#include "whc_ipc.h"
#endif

struct wifi_user_conf wifi_user_config __attribute__((aligned(64)));

_WEAK void wifi_set_user_config(void)
{
	memset(&wifi_user_config, 0, sizeof(struct wifi_user_conf));

	/* below items for user config, for details, see wifi_user_conf in ameba_wificfg_common.h */
	wifi_user_config.concurrent_enabled = (u8)TRUE;
	wifi_user_config.softap_addr_offset_idx = 1;
	wifi_user_config.fast_reconnect_en = 1;
	wifi_user_config.auto_reconnect_en = 1;
	wifi_user_config.auto_reconnect_count = 10;
	wifi_user_config.auto_reconnect_interval = 5;
	wifi_user_config.no_beacon_disconnect_time = 9; /* unit 2s, default 18s */

#ifdef CONFIG_HIGH_TP_TEST /*enable high tp in make menuconfig*/
	wifi_user_config.skb_num_np = 32;
	wifi_user_config.skb_num_ap = 256;
	wifi_user_config.rx_ampdu_num = 8;
#else
	wifi_user_config.skb_num_np = 10; /*4 for rx_ring_buffer + 4 for rx_ampdu + 2 for mgnt trx*/
	wifi_user_config.skb_num_ap = 4;
	wifi_user_config.rx_ampdu_num = 4;
#endif
#ifdef CONFIG_WHC_NONE
	wifi_user_config.skb_num_ap = 0;
#endif
	wifi_user_config.tx_ampdu_num = 20; /* 0/1: equivalent to wifi_user_config.ampdu_tx_enable = 0, Otherwise: max aggregation number, up to 0x3F*/
	wifi_user_config.skb_buf_size = MAX_SKB_BUF_SIZE;
	wifi_user_config.wifi_wpa_mode_force = RTW_WPA_AUTO_MODE;

	/*Regulatory related*/
	wifi_user_config.country_code[0] = 0;
	wifi_user_config.country_code[1] = 0;
	wifi_user_config.freq_band_support = RTW_SUPPORT_BAND_MAX;
	wifi_user_config.tx_pwr_table_selection = 2;
	wifi_user_config.tpc_enable = 0; /* 1: apply TPC for FCC/MKK/KCC/IC/ETSI(5.25~5.35GHz, 5.47~5.725GHz), CN(5.25~5.35GHz) */
	wifi_user_config.rtw_802_11d_en = DISABLE;
	wifi_user_config.rtw_trp_tis_cert_en = RTW_TRP_TIS_DISABLE;
	wifi_user_config.rtw_edcca_mode = RTW_EDCCA_NORM;
	wifi_user_config.tdma_dig_enable = 0; /*add for customer ctrl tdma dig on/off*/
	wifi_user_config.antdiv_mode = RTW_ANTDIV_DISABLE;
	wifi_user_config.probe_hidden_ap_on_passive_ch = 1;

	/* IPS(Inactive Power Save), power save when wifi unconnected */
	wifi_user_config.ips_enable = 1;
	wifi_user_config.ips_level = RTW_IPS_WIFI_OFF;

	/* LPS(Legacy Power Save), power save when wifi connected */
	wifi_user_config.lps_enable = 1;
	wifi_user_config.lps_listen_interval = 0;
	wifi_user_config.lps_rx_unicast_pkt_timeout = 40;	/* set rx unicast packet timeout in LPS, unit:ms, max_value:100*/
	wifi_user_config.wowlan_rx_bcmc_dis = 0;

	/* Softap related */
	wifi_user_config.ap_sta_num = 5;	/*should not exceed AP_STA_NUM */
	wifi_user_config.ap_polling_sta = 0;
	wifi_user_config.ap_bypass_forwarding = 0;

	/* MISC */
	wifi_user_config.en_mcc = (u8) DISABLE;  /* must select ENABLE_MCC in menuconfig when wifi_user_config.en_mcc=1 */
	wifi_user_config.mcc_force_p1_slot_ratio = 44;
	wifi_user_config.ampdu_rx_enable = (u8)TRUE;
	wifi_user_config.ampdu_tx_enable = (u8)TRUE;
	wifi_user_config.ap_compatibilty_enabled = 0x07;
	wifi_user_config.set_channel_api_do_rfk = 1;
	wifi_user_config.dpk_peak_limit = 0;
	wifi_user_config.rf_calibration_disable = 0;
	wifi_user_config.tx_shortcut_enable = 1;
	wifi_user_config.rx_shortcut_enable = 1;
	wifi_user_config.keepalive_interval = 20;
	wifi_user_config.rx_cca_thresh = 0;
	wifi_user_config.rate_mask_cck = 0x0;
	wifi_user_config.sgi = 1;
	wifi_user_config.he_gi_ltf_cap = RTW_HE_GI_LTF_ALL;

	/* WPS */
	wifi_user_config.wps_retry_count = 4;
	wifi_user_config.wps_retry_interval = 5000;

#if defined(CONFIG_WHC_DEV) && !defined(CONFIG_WHC_INTF_IPC) &&  !defined(CONFIG_WHC_WPA_SUPPLICANT_OFFLOAD)
	/* Linux wifi supports cfg80211 ops. */
	...
	wifi_user_config.cfg80211 = 1;
#else
	wifi_user_config.cfg80211 = 0;
#endif

#if defined(CONFIG_WHC_HOST) && !defined(CONFIG_MP_SHRINK)
	extern int wifi_resume_restore(void);
	wifi_resume_restore();
#endif
}


/**
 * @brief external event handle, customer can add handle functions for wifi events @ref rtw_event_id
 */
__weak struct rtw_event_hdl_func_t event_external_hdl[4] = {
	{RTW_EVENT_MAX,			NULL},
	/* Add customer event handlers here, e.g.: */
	/* {RTW_EVENT_JOIN_STATUS, your_handler}, */
	{RTW_EVENT_MAX,			NULL},
	{RTW_EVENT_MAX,			NULL},
	{RTW_EVENT_MAX,			NULL},
};
__weak u16 array_len_of_event_external_hdl = sizeof(event_external_hdl) / sizeof(struct rtw_event_hdl_func_t);

#if defined(CONFIG_WHC_HOST) && !defined(CONFIG_MP_SHRINK)
#include "lwip_netconf.h"
#include "FreeRTOS.h"
#include "task.h"
#include <wifi_auto_reconnect.h>
#define WIFI_MAKEU32(a,b,c,d) (((u32)((a) & 0xff) << 24) | \
                               ((u32)((b) & 0xff) << 16) | \
                               ((u32)((c) & 0xff) << 8)  | \
                                (u32)((d) & 0xff))

#define WLAN_RESUME_SIG 0x87358735
/* Pro3 persistent data across AP core power-down */
struct wlan_resume_parm {
	u32 resume_sig;
	u32 ip_addr;
	u32 gw_addr;
	u32 mask_addr;
	u32 dns_addr; /* raw ip4 u32 as kept by lwip dns, not byte-swapped */
	u8 wifi_mac_addr[6];
	u8  join_status;
	struct rtw_network_info rtw_reconn_conn_param;
	u8  rtw_reconn_pwd_buf[RTW_MAX_PSK_LEN];
};

struct wlan_resume_parm g_wlan_resume_parm
__attribute__((section(".ret.ssram"), aligned(32)));

int wifi_resume_backup(void)
{
	extern u8 rtw_join_status;
	if (rtw_join_status == RTW_JOINSTATUS_SUCCESS) {
		/* lwip_get_ip() returns a live pointer into netif; the DHCP event may be
		 * delivered before the tcpip thread commits the leased address into netif.
		 * Wait (bounded) until the IP is actually valid, otherwise skip this round
		 * so we never overwrite a previous good backup with 0.0.0.0. */
		u32 ip_ready = 0;
		for (int i = 0; i < 20; i++) {
			ip_ready = *(u32 *)lwip_get_ip(NETIF_WLAN_STA_INDEX);
			if (ip_ready != 0) {
				//DiagPrintf("[WiFi Backup] IP ready(%d)\n\r",i);
				break;
			}
			rtos_time_delay_ms(10);
		}
		if (ip_ready == 0) {
			DiagPrintf("[WiFi Backup] IP not ready, skip backup\n\r");
			return 0;
		}

		memset(&g_wlan_resume_parm, 0, sizeof(struct wlan_resume_parm));

		// store resume join status
		g_wlan_resume_parm.join_status = rtw_join_status;
		// store resume signature
		g_wlan_resume_parm.resume_sig = WLAN_RESUME_SIG;
		// store resume wifi mac
		u8 *mac = lwip_get_mac(0);
		memcpy(g_wlan_resume_parm.wifi_mac_addr, mac, 6);

		// store LWIP IP/GW/MASK
		u8 *ip = lwip_get_ip(NETIF_WLAN_STA_INDEX);
		u8 *gw = lwip_get_gw(NETIF_WLAN_STA_INDEX);
		u8 *msk = lwip_get_mask(NETIF_WLAN_STA_INDEX);
		g_wlan_resume_parm.ip_addr = WIFI_MAKEU32(ip[0], ip[1], ip[2], ip[3]);
		g_wlan_resume_parm.gw_addr = WIFI_MAKEU32(gw[0], gw[1], gw[2], gw[3]);
		g_wlan_resume_parm.mask_addr = WIFI_MAKEU32(msk[0], msk[1], msk[2], msk[3]);
#if LWIP_DNS
		struct ip_addr dns;
		lwip_get_dns(&dns);
		g_wlan_resume_parm.dns_addr = ip_addr_get_ip4_u32(&dns);
#endif
#if CONFIG_AUTO_RECONNECT
		extern struct rtw_auto_reconn_t  rtw_reconn;
		memcpy(&g_wlan_resume_parm.rtw_reconn_conn_param, &rtw_reconn.conn_param, sizeof(struct rtw_network_info));

		/* Backup actual password data to retention memory (pointer alone becomes invalid after power-down) */
		if (rtw_reconn.conn_param.password_len > 0 && rtw_reconn.conn_param.password != NULL) {
			memcpy(g_wlan_resume_parm.rtw_reconn_pwd_buf, rtw_reconn.conn_param.password, rtw_reconn.conn_param.password_len);
			g_wlan_resume_parm.rtw_reconn_conn_param.password = g_wlan_resume_parm.rtw_reconn_pwd_buf;
		} else {
			g_wlan_resume_parm.rtw_reconn_conn_param.password = NULL;
		}
		DiagPrintf("[WiFi Backup] rtw_join_status: %d\n\r", g_wlan_resume_parm.join_status);
#if 0
//Debug
		DiagPrintf("wifi_resume_backup rtw_join_status: %d, mac: "MAC_FMT"\n\r", g_wlan_resume_parm.join_status, MAC_ARG(g_wlan_resume_parm.wifi_mac_addr));
		DiagPrintf("wifi_resume_backup mac: ip: 0x%08X, gw: 0x%08X, msk: 0x%08X\n\r", g_wlan_resume_parm.ip_addr, g_wlan_resume_parm.gw_addr,
				   g_wlan_resume_parm.mask_addr);
		DiagPrintf("wifi_resume_backup ssid=%s, ssid_len=%d, sec_type=%lx, pwd_len=%d, pwd=%s\n\r",
				   rtw_reconn.conn_param.ssid.val,
				   rtw_reconn.conn_param.ssid.len,
				   rtw_reconn.conn_param.security_type,
				   rtw_reconn.conn_param.password_len,
				   rtw_reconn.conn_param.password);
		DiagPrintf("wifi_resume_backup bssid="MAC_FMT", channel=%d, by_reconn=%d\n\r",
				   MAC_ARG(rtw_reconn.conn_param.bssid.octet),
				   rtw_reconn.conn_param.channel,
				   rtw_reconn.conn_param.by_reconn);
#endif
#endif
		DCache_CleanInvalidate((u32)&g_wlan_resume_parm, sizeof(struct wlan_resume_parm));
	}


	return 1;
}

int wifi_resume_restore(void)
{
	u8 join_status;
	join_status = g_wlan_resume_parm.join_status;

	//DiagPrintf("wifi_resume_restore enter\n\r");
	if (join_status == RTW_JOINSTATUS_SUCCESS) {
		if (g_wlan_resume_parm.resume_sig != WLAN_RESUME_SIG) {
			DiagPrintf("WiFi resume signature error! (%x != %x)!\n\r", g_wlan_resume_parm.resume_sig, WLAN_RESUME_SIG);
			return 0;
		} else {
			// clear signature
			DiagPrintf("[WiFi Restore] Successful!\n\r");
			g_wlan_resume_parm.resume_sig = 0;
		}

		wifi_user_config.fast_reconnect_en = 0;

		extern u8 rtw_join_status;
		rtw_join_status = g_wlan_resume_parm.join_status;

		/* restore lwip netif MAC ?X dev is always NULL in WHC host */
		lwip_wlan_set_netif_info(0, NULL, g_wlan_resume_parm.wifi_mac_addr);


		lwip_netif_set_link_up(0);
		lwip_set_ip(0, g_wlan_resume_parm.ip_addr, g_wlan_resume_parm.mask_addr, g_wlan_resume_parm.gw_addr);
#if LWIP_DNS
		if (g_wlan_resume_parm.dns_addr != 0) {
			struct ip_addr dns;
			ip_addr_set_ip4_u32(&dns, g_wlan_resume_parm.dns_addr);
			lwip_set_dns(&dns);
		}
#endif

#if CONFIG_AUTO_RECONNECT
		extern struct rtw_auto_reconn_t  rtw_reconn;
		memcpy(&rtw_reconn.conn_param, &g_wlan_resume_parm.rtw_reconn_conn_param, sizeof(struct rtw_network_info));

		/* Restore password: allocate new buffer and copy from retention memory (like rtw_reconn_new_conn) */
		if (rtw_reconn.conn_param.password_len > 0 && rtw_reconn.conn_param.password != NULL) {
			if (rtw_reconn.pwd_buf != NULL) {
				rtos_mem_free(rtw_reconn.pwd_buf);
			}
			rtw_reconn.pwd_buf = rtos_mem_zmalloc(sizeof(u8) * (rtw_reconn.conn_param.password_len + 1));
			memcpy(rtw_reconn.pwd_buf, rtw_reconn.conn_param.password, rtw_reconn.conn_param.password_len);
			rtw_reconn.pwd_buf[rtw_reconn.conn_param.password_len] = '\0';
			rtw_reconn.conn_param.password = rtw_reconn.pwd_buf;
		} else {
			rtw_reconn.conn_param.password = NULL;
		}
		rtw_reconn.conn_param.by_reconn = 1;
#if 0
		DiagPrintf("wifi_resume_restore rtw_join_status: %d\n\r", rtw_join_status);
		DiagPrintf("wifi_resume_restore rtw_join_status: %d, mac: "MAC_FMT"\n\r", g_wlan_resume_parm.join_status, MAC_ARG(g_wlan_resume_parm.wifi_mac_addr));
		DiagPrintf("wifi_resume_restore mac: ip: 0x%08X, gw: 0x%08X, msk: 0x%08X\n\r", g_wlan_resume_parm.ip_addr, g_wlan_resume_parm.gw_addr,
				   g_wlan_resume_parm.mask_addr);
		DiagPrintf("wifi_resume_restore ssid=%s, ssid_len=%d, sec_type=%lx, pwd_len=%d, pwd=%s\n\r",
				   rtw_reconn.conn_param.ssid.val,
				   rtw_reconn.conn_param.ssid.len,
				   rtw_reconn.conn_param.security_type,
				   rtw_reconn.conn_param.password_len,
				   rtw_reconn.conn_param.password);
		DiagPrintf("wifi_resume_restore bssid="MAC_FMT", channel=%d, by_reconn=%d\n\r",
				   MAC_ARG(rtw_reconn.conn_param.bssid.octet),
				   rtw_reconn.conn_param.channel,
				   rtw_reconn.conn_param.by_reconn);
#endif
#endif
	} else {
		DiagPrintf("[WiFi Restore] Join_status(%d) not connect!\n\r", join_status);
	}

	return 1;
}
#endif
