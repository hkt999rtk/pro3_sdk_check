/**
  ******************************************************************************
  * @file    whc_ipc_dev_api.c
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#include "whc_ipc.h"

#include "wifi_intf_drv_to_upper.h"
#include "whc_ipc_cfg.h"
#include "rtw_nan_enum.h"
#ifdef CONFIG_WHC_DEV_TCPIP_KEEPALIVE
#include "lwip_netconf.h"
#endif

/* -------------------------------- Defines --------------------------------- */
#define CONFIG_INIC_IPC_DEV_API_PRIO 3

/* -------------------------------- Macros ---------------------------------- */

/* ---------------------------- Global Variables ---------------------------- */
rtos_sema_t g_dev_inic_api_task_wake_sema = NULL;
rtos_sema_t g_dev_inic_api_message_send_sema = NULL;

rtos_task_t api_dev_task = NULL;

//SRAM_BD_DATA_SECTION
struct whc_ipc_dev_req_msg g_dev_ipc_api_request_info __attribute__((aligned(RTOS_MEM_BYTE_ALIGNMENT)));
u32 latest_enevt_id = 0;  /*for debug*/
#ifdef IPC_DIR_MSG_TX
IPC_MSG_STRUCT g_dev_ipc_api_msg __attribute__((aligned(RTOS_MEM_BYTE_ALIGNMENT)));
#endif

extern u8 whc_ipc_ip_addr[4];
extern u8 whc_ipc_gateway[4];
#ifdef CONFIG_MP_INCLUDED
extern int wext_private_command(char *cmd, int show_msg, char *user_buf);
#endif
extern int wifi_set_ips_internal(u8 enable);
extern int wifi_set_chplan(u8 chplan);
extern u16 call_noused;
void rtw_sme_auth_event(struct rtw_sme_auth_info *auth_info);
void rtw_sme_set_assocreq_ie(u8 *buf, u32 size, u8 wpa_rsn_exist);
void rtw_sme_set_deauth_ie(u8 *ie, u32 ie_len, u16 reason_code);
u8 whc_ipc_dev_nan_api_hdl(struct whc_ipc_host_req_msg *p_ipc_msg);
u8 whc_ipc_dev_p2p_api_hdl(struct whc_ipc_host_req_msg *p_ipc_msg);
u8 whc_ipc_dev_wtn_api_hdl(struct whc_ipc_host_req_msg *p_ipc_msg);
u8 whc_ipc_dev_wtn_ota_api_hdl(struct whc_ipc_host_req_msg *p_ipc_msg);
u8 whc_ipc_dev_xmesh_api_hdl(struct whc_ipc_host_req_msg *p_ipc_msg);
u8 whc_ipc_dev_rmesh_rnat_api_hdl(struct whc_ipc_host_req_msg *p_ipc_msg);
#ifdef CONFIG_WHC_DEV_TCPIP_KEEPALIVE
u8 whc_ipc_dev_dhcp(u8 idx, u32 *ipinfo);
#endif
/* ---------------------------- Private Functions --------------------------- */

/* ---------------------------- Public Functions ---------------------------- */
/**
 * @brief  handle the message of IPC.
 * @param  none.
 * @return none.
 */
void whc_ipc_dev_api_task(void)
{
	int ret = -1;
	struct whc_ipc_host_req_msg *p_ipc_msg = NULL;

	do {
		rtos_sema_take(g_dev_inic_api_task_wake_sema, 0xFFFFFFFF);
#ifdef IPC_DIR_MSG_RX
		PIPC_MSG_STRUCT p_ipc_recv_msg = ipc_get_message(IPC_DIR_MSG_RX, \
										 IPC_H2D_WIFI_API_TRAN);
		p_ipc_msg = (struct whc_ipc_host_req_msg *)p_ipc_recv_msg->msg;
#else
		p_ipc_msg = (struct whc_ipc_host_req_msg *)ipc_get_message(IPC_INT_CHAN_WIFI_API_TRAN);
#endif /* IPC_DIR_MSG_RX */

		DCache_Invalidate((u32)p_ipc_msg, sizeof(struct whc_ipc_host_req_msg));
		if (p_ipc_msg == NULL) {
			RTK_LOGE(TAG_WLAN_INIC, "IPCDEV msg NULL!\n\r");
			continue;
		}

		switch (p_ipc_msg->api_id) {
#if !defined(CONFIG_MP_SHRINK)
		case WHC_API_WIFI_CONNECT: {
			u8 *buf = (u8 *)p_ipc_msg->param_buf[0];
			u32 size = (u32)p_ipc_msg->param_buf[1];
			DCache_Invalidate((u32)buf, size);

			struct rtw_network_info *pWifi = (struct rtw_network_info *)buf;
			/* KVR Roaming from cfg80211 or wpa_lite_wnm. */
			rtw_joinbss_by_roaming(pWifi);
			ret = rtw_joinbss_start_api(pWifi);
			/* rtw_joinbss_start_api() may rewrite pWifi, flush it before host frees buf. RSWLANDIOT-16965 */
			DCache_Clean((u32)buf, size);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_ADD_KEY: {
			struct rtw_crypt_info *crypt = (struct rtw_crypt_info *)p_ipc_msg->param_buf[0];
			DCache_Invalidate((u32)(crypt), sizeof(struct rtw_crypt_info));
			p_ipc_msg->ret = wifi_wpa_add_key(crypt);
			break;
		}

		case WHC_API_WPA_PMKSA_OPS: {
			struct rtw_pmksa_ops_t *pmksa_ops = (struct rtw_pmksa_ops_t *)p_ipc_msg->param_buf[0];
			DCache_Invalidate((u32)(pmksa_ops), sizeof(struct rtw_pmksa_ops_t));
			wifi_wpa_pmksa_ops(pmksa_ops);
			p_ipc_msg->ret = 0;
			break;
		}
#endif
		case WHC_API_WIFI_GET_COUNTRY_CODE: {
			struct rtw_country_code_table *pinfo = (struct rtw_country_code_table *)p_ipc_msg->param_buf[0];

			if (pinfo == NULL) {
				p_ipc_msg->ret = -1;
				break;
			}
			DCache_Invalidate((u32)pinfo, sizeof(struct rtw_country_code_table));
			ret = wifi_get_countrycode(pinfo);
			DCache_Clean((u32)pinfo, sizeof(struct rtw_country_code_table));
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_GET_CHANNEL_LIST: {
			struct rtw_channel_list *ch_list = (struct rtw_channel_list *)p_ipc_msg->param_buf[0];
			DCache_Invalidate((u32)ch_list, sizeof(struct rtw_channel_list));
			ret = wifi_get_channel_list(ch_list);
			DCache_Clean((u32)ch_list, sizeof(struct rtw_channel_list));
			p_ipc_msg->ret = ret;
			break;
		}
#if !defined(CONFIG_MP_SHRINK)
		case WHC_API_WIFI_DISCONNECT: {
#ifdef CONFIG_SUPPLICANT_SME
			u8 *ie = (u8 *)p_ipc_msg->param_buf[0];
			u32 ie_len = p_ipc_msg->param_buf[1];
			u16 reason_code = (u16)p_ipc_msg->param_buf[2];

			if (ie && ie_len) {
				DCache_Invalidate((u32)ie, ie_len);
			}

			rtw_sme_set_deauth_ie(ie, ie_len, reason_code);
#endif
			ret = wifi_disconnect();
			p_ipc_msg->ret = ret;
			break;
		}
#endif
		case WHC_API_WIFI_IS_RUNNING: {
			unsigned char wlan_idx = (unsigned char)p_ipc_msg->param_buf[0];
			ret = wifi_is_running(wlan_idx);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SET_CHANNEL: {
			unsigned char wlan_idx = (unsigned char)p_ipc_msg->param_buf[0];
			u8 channel = (u8)p_ipc_msg->param_buf[1];
			ret = wifi_set_channel(wlan_idx, channel);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SET_USR_CFG: {
			struct wifi_user_conf *pwifi_usrcfg = (struct wifi_user_conf *)p_ipc_msg->param_buf[0];
			DCache_Invalidate((u32)pwifi_usrcfg, sizeof(struct wifi_user_conf));
			memcpy(&wifi_user_config, pwifi_usrcfg, sizeof(struct wifi_user_conf));
			wifi_set_rom2flash_user_config();
			break;
		}
		case WHC_API_WIFI_ON: {
			u8 mode = (u8)p_ipc_msg->param_buf[0];
#ifdef CONFIG_WHC_DEV_TCPIP_KEEPALIVE
			/* host is up right after it turns wifi on; keep default until it sleeps */
			whc_dev_api_set_host_state(WHC_HOST_READY);
#endif
			ret = wifi_on(mode);
			p_ipc_msg->ret = ret;
			break;
		}
#ifdef CONFIG_WHC_DEV_TCPIP_KEEPALIVE
		case WHC_API_WIFI_DEV_DHCP: {
			u8 idx = (u8)p_ipc_msg->param_buf[0];
			u32 *ipinfo = (u32 *)p_ipc_msg->param_buf[1];
			/* host delegates dhcp to dev: run it here (blocking).
			 * ip acquisition is an atomic phase, so holding the api task is intended. */
			lwip_netif_set_link_up(idx);
			DCache_Invalidate((u32)ipinfo, 4 * sizeof(u32));
			p_ipc_msg->ret = whc_ipc_dev_dhcp(idx, ipinfo);
			DCache_Clean((u32)ipinfo, 4 * sizeof(u32));
			break;
		}
#endif
#if !defined(CONFIG_MP_SHRINK)
		case WHC_API_WIFI_INIT_AP: {
			ret = _wifi_on_ap();
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_DEINIT_AP: {
			ret = _wifi_off_ap();
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_START_AP: {
			u8 *buf = (u8 *)p_ipc_msg->param_buf[0];
			u32 size = (u32)p_ipc_msg->param_buf[1];
			DCache_Invalidate((u32)buf, size);

			struct rtw_softap_info *softAP_config = (struct rtw_softap_info *)buf;
			ret = wifi_start_ap(softAP_config);
			/* WEP rewrites password/password_len in place, flush before host frees buf. RSWLANDIOT-16965 */
			DCache_Clean((u32)buf, size);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_STOP_AP: {
			ret = wifi_stop_ap();
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SCAN_NETWROKS: {
			u8 *buf = (u8 *)p_ipc_msg->param_buf[0];
			u32 size = (u8)p_ipc_msg->param_buf[1];
			DCache_Invalidate((u32)buf, size);
			ret = wifi_scan_networks((struct rtw_scan_param *)buf, 0);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SCAN_ABORT: {
			ret = wifi_scan_abort();
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_GET_SCANNED_AP_INFO: {
			u32 *AP_num = (u32 *)p_ipc_msg->param_buf[0];
			struct rtw_scan_result *ap_list = (struct rtw_scan_result *)p_ipc_msg->param_buf[1];
			DCache_Invalidate((u32)AP_num, sizeof(u32));
			DCache_Invalidate((u32)ap_list, (*AP_num)*sizeof(struct rtw_scan_result));
			ret = wifi_get_scan_records(AP_num, ap_list);
			DCache_Clean((u32)AP_num, sizeof(u32));
			DCache_Clean((u32)ap_list, (*AP_num)*sizeof(struct rtw_scan_result));
			p_ipc_msg->ret = ret;
			break;
		}
#endif
		case WHC_API_WIFI_SET_MAC_ADDR: {
			int idx = (int)p_ipc_msg->param_buf[0];
			unsigned char *mac = (unsigned char *)p_ipc_msg->param_buf[1];
			u8 efuse = (u8)p_ipc_msg->param_buf[2];
			DCache_Invalidate((u32)mac, sizeof(struct rtw_mac));
			ret = wifi_set_mac_address(idx, mac, efuse);
			DCache_Clean((u32)mac, sizeof(struct rtw_mac));
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_GET_MAC_ADDR: {
			int idx = (int)p_ipc_msg->param_buf[0];
			struct rtw_mac *mac = (struct rtw_mac *)p_ipc_msg->param_buf[1];
			u8 efuse = (u8)p_ipc_msg->param_buf[2];
			DCache_Invalidate((u32)mac, sizeof(struct rtw_mac));
			ret = wifi_get_mac_address(idx, mac, efuse);
			DCache_Clean((u32)mac, sizeof(struct rtw_mac));
			p_ipc_msg->ret = ret;
			break;
		}
#if !defined(CONFIG_MP_SHRINK)
		case WHC_API_WIFI_SET_WPA_MODE: {
			u8 wpa_mode = p_ipc_msg->param_buf[0];
			ret = wifi_set_wpa_mode(wpa_mode);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WPA_4WAY_REPORT: {
			struct rtw_wpa_4way_status *rpt_4way = (struct rtw_wpa_4way_status *)p_ipc_msg->param_buf[0];
			DCache_Invalidate((u32)rpt_4way, sizeof(struct rtw_wpa_4way_status));
			wifi_wpa_4way_status_indicate(rpt_4way);
			p_ipc_msg->ret = 0;
			break;
		}
		case WHC_API_WIFI_DHCP_SUCCESS_IND: {
			wifi_dhcp_success_indicate();
			p_ipc_msg->ret = 0;
			break;
		}
		case WHC_API_WIFI_SET_PMF_MODE: {
			u8 pmf_mode = p_ipc_msg->param_buf[0];
			ret = wifi_set_pmf_mode(pmf_mode);
			p_ipc_msg->ret = ret;
			break;
		}
#endif
		case WHC_API_WIFI_DRIVE_IS_MP: {
			ret = wifi_driver_is_mp(NULL);
			p_ipc_msg->ret = ret;
			break;
		}
#if !defined(CONFIG_MP_SHRINK)
		case WHC_API_WIFI_AP_GET_CONNECTED_CLIENTS: {
			struct rtw_client_list *client_list_buffer = (struct rtw_client_list *)p_ipc_msg->param_buf[0];
			DCache_Invalidate((u32)client_list_buffer, sizeof(struct rtw_client_list));

			ret = wifi_ap_get_connected_clients(client_list_buffer);
			DCache_Clean((u32)client_list_buffer, sizeof(struct rtw_client_list));
			p_ipc_msg->ret = ret;
			break;
		}
#endif
		case WHC_API_WIFI_GET_SETTING: {
			unsigned char wlan_idx = (unsigned char)p_ipc_msg->param_buf[0];
			struct rtw_wifi_setting *ap_info = (struct rtw_wifi_setting *)p_ipc_msg->param_buf[1];
			DCache_Invalidate((u32)ap_info, sizeof(struct rtw_wifi_setting));

			ret = wifi_get_setting(wlan_idx, ap_info);
			DCache_Clean((u32)ap_info, sizeof(struct rtw_wifi_setting));
			p_ipc_msg->ret = ret;
			break;
		}
#if !defined(CONFIG_MP_SHRINK)
		case WHC_API_WIFI_SET_IPS_EN: {
			u8 ips_en = (u8)p_ipc_msg->param_buf[0];
			ret = wifi_set_ips_internal(ips_en);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SET_LPS_EN: {
			u8 lps_en = (u8)p_ipc_msg->param_buf[0];
			ret = wifi_set_lps_enable(lps_en);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SET_LPS_LISTEN_INTERVAL: {
			u8 interval = (u8)p_ipc_msg->param_buf[0];
			ret = wifi_set_lps_listen_interval(interval);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SET_WOWLAN_RX_BROADCAST: {
			u8 enable = (u8)p_ipc_msg->param_buf[0];
			ret = wifi_set_wowlan_rx_broadcast(enable);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SET_WOWLAN_IPV6_WAKE: {
			u8 enable = (u8)p_ipc_msg->param_buf[0];
			ret = wifi_set_wowlan_ipv6_wake(enable);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SET_LPS_BCN_WINDOW_MANUAL_CONFIG: {
			u8 enable = (u8)p_ipc_msg->param_buf[0];
			u8 bcn_ely_time = (u8)p_ipc_msg->param_buf[1];
			u8 rx_bcn_timeout = (u8)p_ipc_msg->param_buf[2];
			ret = wifi_set_lps_bcn_window(enable, bcn_ely_time, rx_bcn_timeout);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SET_MFP_SUPPORT: {
			unsigned char value = (unsigned char)p_ipc_msg->param_buf[0];
			ret = wifi_set_mfp_support(value);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SET_GROUP_ID: {
			unsigned char value = (unsigned char)p_ipc_msg->param_buf[0];
			ret = wifi_set_group_id(value);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SET_PMK_CACHE_EN: {
			unsigned char value = (unsigned char)p_ipc_msg->param_buf[0];
			ret = wifi_set_pmk_cache_enable(value);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SAE_STATUS: {
			unsigned char wlan_idx = (unsigned char)p_ipc_msg->param_buf[0];
			u16 value = (u16)p_ipc_msg->param_buf[1];
			u8 *mac_addr = (u8 *)p_ipc_msg->param_buf[2];
			if (mac_addr) {
				DCache_Invalidate((u32)mac_addr, 6);
			}
			ret = wifi_sae_status_indicate(wlan_idx, value, mac_addr);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_EXTERNAL_AUTH_START: {
			u8 wlan_idx = (u8)p_ipc_msg->param_buf[0];
			ret = wifi_external_auth_start(wlan_idx);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_START_JOIN_CMD: {
			ret = wifi_start_join_cmd();
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_FT_STATUS: {
			struct rtw_kvr_param_t *kvr_param = (struct rtw_kvr_param_t *)p_ipc_msg->param_buf[0];
			DCache_Invalidate((u32)(kvr_param), sizeof(struct rtw_kvr_param_t));
			u16 value = (u16)p_ipc_msg->param_buf[1];
			ret = wifi_ft_status_indicate(kvr_param, value);
			p_ipc_msg->ret = ret;
			break;
		}
#endif
		case WHC_API_WIFI_GET_TRAFFIC_STATS: {
			u8 wlan_idx = (u8)p_ipc_msg->param_buf[0];
			union rtw_traffic_stats *traffic_stats = (union rtw_traffic_stats *)p_ipc_msg->param_buf[1];
			DCache_Invalidate((u32)traffic_stats, sizeof(union rtw_traffic_stats));
			ret = wifi_get_traffic_stats(wlan_idx, traffic_stats);
			DCache_Clean((u32)traffic_stats, sizeof(union rtw_traffic_stats));
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_GET_PHY_STATS: {
			u8 wlan_idx = (u8)p_ipc_msg->param_buf[0];
			u8 *mac_addr = (u8 *)p_ipc_msg->param_buf[1];
			union rtw_phy_stats *phy_stats = (union rtw_phy_stats *)p_ipc_msg->param_buf[2];
			DCache_Invalidate((u32)phy_stats, sizeof(union rtw_phy_stats));
			if (mac_addr) {
				DCache_Invalidate((u32)mac_addr, 6);
			}
			ret = wifi_get_phy_stats(wlan_idx, mac_addr, phy_stats);
			DCache_Clean((u32)phy_stats, sizeof(union rtw_phy_stats));
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SET_WIRELESS_MODE: {
			u32 mode = (u32)p_ipc_msg->param_buf[0];
			ret = wifi_set_wireless_mode(mode);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_GET_WIRELESS_MODE: {
			u8 *wmode = (u8 *)p_ipc_msg->param_buf[0];
			DCache_Invalidate((u32)wmode, sizeof(u8));
			ret = wifi_get_wireless_mode(wmode);
			DCache_Clean((u32)wmode, sizeof(u8));
			p_ipc_msg->ret = ret;
			break;
		}
#if !defined(CONFIG_MP_SHRINK)
		case WHC_API_WIFI_SET_WPS_PHASE: {
			unsigned char wlan_idx = (unsigned char)p_ipc_msg->param_buf[0];
			unsigned char is_trigger_wps = (unsigned char)p_ipc_msg->param_buf[1];
			ret = wifi_set_wps_phase(wlan_idx, is_trigger_wps);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SET_GEN_IE: {
			unsigned char wlan_idx = (unsigned char)p_ipc_msg->param_buf[0];
			char *buf = (char *)p_ipc_msg->param_buf[1];
			u16 buf_len = (u16)p_ipc_msg->param_buf[2];
			u16 flags = (u16)p_ipc_msg->param_buf[3];

			if (buf != NULL && buf_len != 0) {
				DCache_Invalidate((u32)buf, buf_len);
			}
			ret = wifi_set_gen_ie(wlan_idx, buf, buf_len, flags);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SET_EAP_PHASE: {
			unsigned char is_trigger_eap = (unsigned char)p_ipc_msg->param_buf[0];
			ret = wifi_set_eap_phase(is_trigger_eap);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_GET_EAP_PHASE: {
			ret = wifi_get_eap_phase(NULL);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SET_EAP_METHOD: {
			unsigned char eap_method = (unsigned char)p_ipc_msg->param_buf[0];
			ret = wifi_set_eap_method(eap_method);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SEND_EAPOL: {
			unsigned char wlan_idx = (unsigned char)p_ipc_msg->param_buf[0];
			char *buf = (char *)p_ipc_msg->param_buf[1];
			u16 buf_len = (u16)p_ipc_msg->param_buf[2];
			u16 flags = (u16)p_ipc_msg->param_buf[3];
			DCache_Invalidate((u32)buf, buf_len);
			ret = wifi_if_send_eapol(wlan_idx, buf, buf_len, flags);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_CUS_IE: {
			u8 type = (u8)p_ipc_msg->param_buf[0];
			if (type == 2) {
				unsigned char wlan_idx = p_ipc_msg->param_buf[1];
				ret = wifi_del_custom_ie(wlan_idx);
			} else if (type == 1) {
				u8 *buf = (u8 *)p_ipc_msg->param_buf[1];
				u32 size = (u32)p_ipc_msg->param_buf[2];
				int ie_index = (int)p_ipc_msg->param_buf[3];
				DCache_Invalidate((u32)buf, size);
				ret = wifi_update_custom_ie((struct rtw_custom_ie *)buf, ie_index);
			} else if (type == 0) {
				u8 *buf = (u8 *)p_ipc_msg->param_buf[1];
				u32 size = (u32)p_ipc_msg->param_buf[2];
				int ie_num = (int)p_ipc_msg->param_buf[3];
				DCache_Invalidate((u32)buf, size);
				ret = wifi_add_custom_ie((struct rtw_custom_ie *)buf, ie_num);
			}
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SEND_MGNT: {
			u8 *buf = (u8 *)p_ipc_msg->param_buf[0];
			u32 size = (u32)p_ipc_msg->param_buf[1];
			DCache_Invalidate((u32)buf, size);

			ret = wifi_send_mgnt((struct _raw_data_desc_t *)buf);
			p_ipc_msg->ret = ret;
			break;
		}
#endif
		case WHC_API_WIFI_SET_TXRATE_BY_TOS: {
			unsigned char enable = (unsigned char)p_ipc_msg->param_buf[0];
			unsigned char tos_precedence = (unsigned char)p_ipc_msg->param_buf[1];
			unsigned char tx_rate = (unsigned char)p_ipc_msg->param_buf[2];
			ret = wifi_set_tx_rate_by_tos(enable, tos_precedence, tx_rate);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SET_EDCA_PARAM: {
			struct rtw_edca_param *pedca_param = (struct rtw_edca_param *)p_ipc_msg->param_buf[0];
			DCache_Invalidate((u32)pedca_param, sizeof(struct rtw_edca_param));
			ret = wifi_set_edca_param(pedca_param);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SET_TX_CCA_EN: {
			unsigned char enable = (unsigned char)p_ipc_msg->param_buf[0];
			ret = wifi_set_tx_cca_enable(enable);
			p_ipc_msg->ret = ret;
			break;
		}
#if !defined(CONFIG_MP_SHRINK)

		case WHC_API_WIFI_SET_CTS2SEFL_DUR_AND_SEND: {
			unsigned char wlan_idx = (unsigned char)p_ipc_msg->param_buf[0];
			unsigned short duration = (unsigned short)p_ipc_msg->param_buf[1];
			ret = wifi_set_cts2self_duration_and_send(wlan_idx, duration);
			p_ipc_msg->ret = ret;
			break;
		}
#ifdef CONFIG_WIFI_ANTDIV
		case WHC_API_WIFI_GET_ANTENNA_INFO: {
			u8 *antdiv_mode = (u8 *)p_ipc_msg->param_buf[0];
			u8 *curr_ant = (u8 *)(p_ipc_msg->param_buf[0] + sizeof(unsigned char));
			DCache_Invalidate((u32)antdiv_mode, sizeof(int));
			ret = wifi_get_antdiv_info(antdiv_mode, curr_ant);
			DCache_Clean((u32)antdiv_mode, sizeof(int));
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SET_ANTENNA_INFO: {
			u8 antdiv_mode = (u8)p_ipc_msg->param_buf[0];
			ret = wifi_set_antdiv_info(antdiv_mode);
			p_ipc_msg->ret = ret;
			break;
		}
#endif
#endif
		case WHC_API_WIFI_GET_BAND_TYPE : {
			unsigned char *band_type = (unsigned char *)p_ipc_msg->param_buf[0];
			DCache_Invalidate((u32)band_type, sizeof(unsigned char));
			ret = wifi_get_band_type(band_type);
			DCache_Clean((u32)band_type, sizeof(unsigned char));
			p_ipc_msg->ret = ret;
			break;
		}
#if !defined(CONFIG_MP_SHRINK)
		case WHC_API_WIFI_AP_DEL_CLIENT: {
			u8 *hwaddr = (u8 *)p_ipc_msg->param_buf[1];
			DCache_Invalidate((u32)hwaddr, ETH_ALEN);
			ret = wifi_ap_del_client(hwaddr);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_AP_CH_SWITCH: {
			struct rtw_csa_parm *csa_param = (struct rtw_csa_parm *)p_ipc_msg->param_buf[0];
			DCache_Invalidate((u32)csa_param, sizeof(struct rtw_csa_parm));
			ret = wifi_ap_switch_chl_and_inform(csa_param);
			p_ipc_msg->ret = ret;
			break;
		}
#endif
		case WHC_API_WIFI_IWPRIV_INFO: {
			char *cmd = (char *)p_ipc_msg->param_buf[0];
			int cmd_len = (int)p_ipc_msg->param_buf[1];
			int show_msg = (int)p_ipc_msg->param_buf[2];
			if (cmd) {
				DCache_Invalidate((u32)cmd, cmd_len);
			}
			ret = rtw_iwpriv_command(STA_WLAN_INDEX, cmd, show_msg);
			p_ipc_msg->ret = ret;
			break;
		}
#if !defined(CONFIG_MP_SHRINK)
		case WHC_API_WIFI_CONFIG_CSI: {
			struct rtw_csi_action_parm *act_param = (struct rtw_csi_action_parm *)p_ipc_msg->param_buf[0];
			DCache_Invalidate((u32)act_param, sizeof(struct rtw_csi_action_parm));
			ret = wifi_csi_config(act_param);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_CONFIG_RADAR: {
			struct rtw_radar_action_parm *act_param = (struct rtw_radar_action_parm *)p_ipc_msg->param_buf[0];
			DCache_Invalidate((u32)act_param, sizeof(struct rtw_radar_action_parm));
			ret = wifi_radar_config(act_param);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_RADAR_SEND_DATA: {
			u16 frame_num = (u16)p_ipc_msg->param_buf[0];
			u8 frame_type = (u8)p_ipc_msg->param_buf[1];
			u8 *data = (u8 *)p_ipc_msg->param_buf[2];
			u32 data_size = (frame_type == RTW_RADAR_TYPE_AI_L_NEAR) ?
							(6 * 4 * frame_num) : (44 * 4 * frame_num);
			DCache_Invalidate((u32)data, data_size);
			ret = wifi_radar_send_data(frame_num, frame_type, data);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_IP_UPDATE: {
			u8 *p_ip_addr = (u8 *)p_ipc_msg->param_buf[0];
			DCache_Invalidate((u32)(p_ip_addr), 4);
			memcpy(whc_ipc_ip_addr, p_ip_addr, 4);
			if (!wifi_user_config.cfg80211) {
				/* RTOS: update ap ip addr*/
				u8 *p_gateway = (u8 *)p_ipc_msg->param_buf[1];
				DCache_Invalidate((u32)(p_gateway), 4);
				memcpy(whc_ipc_gateway, p_gateway, 4);
			}
			if (wifi_user_config.cfg80211) {
				pmu_release_wakelock(PMU_OS);
			}
			p_ipc_msg->ret = 0;
			break;
		}
		case WHC_API_WIFI_GET_CCMP_KEY: {
			u8 wlan_idx = (u8)p_ipc_msg->param_buf[0];
			u8 *mac_addr = (u8 *)p_ipc_msg->param_buf[1];
			u8 *uncst_key = (u8 *)p_ipc_msg->param_buf[2];
			u8 *grp_key = (u8 *)p_ipc_msg->param_buf[3];
			if (mac_addr) {
				DCache_Invalidate((u32)mac_addr, 6);
			}
			DCache_Invalidate((u32)uncst_key, 16);
			DCache_Invalidate((u32)grp_key, 16);
			ret = wifi_get_ccmp_key(wlan_idx, mac_addr, uncst_key, grp_key);
			DCache_Clean((u32)uncst_key, 16);
			DCache_Clean((u32)grp_key, 16);
			p_ipc_msg->ret = 0;
			break;
		}
#endif
#ifdef CONFIG_MP_INCLUDED
		case WHC_API_WIFI_MP_CMD: {
			char *cmd = (char *)p_ipc_msg->param_buf[0];
			int cmd_len = (int)p_ipc_msg->param_buf[1];
			int show_msg = (int)p_ipc_msg->param_buf[2];
			char *user_buf = (char *)p_ipc_msg->param_buf[3];
			if (cmd) {
				DCache_Invalidate((u32)cmd, cmd_len);
			}
			if (user_buf) {
				DCache_Invalidate((u32)user_buf, WHC_MP_MSG_BUF_SIZE);
			}
			ret = wext_private_command(cmd, show_msg, user_buf);
			DCache_Clean((u32)user_buf, WHC_MP_MSG_BUF_SIZE);
			p_ipc_msg->ret = ret;
			break;
		}

#endif
#if !defined(CONFIG_MP_SHRINK)
		/* jira: https://jira.realtek.com/browse/RSWLANQC-1036 */
		case WHC_API_WIFI_MSG_TO: {
			RTK_LOGA(TAG_WLAN_INIC, "MSG TO, No need\n");
			break;
		}
		case WHC_API_WIFI_PROMISC_INIT: {
			struct rtw_promisc_para promisc_para;
			u32 status  = p_ipc_msg->param_buf[0];
			memset(&promisc_para, 0, sizeof(struct rtw_promisc_para));
			promisc_para.filter_mode = p_ipc_msg->param_buf[1];
			promisc_para.callback = (void *)p_ipc_msg->param_buf[2]; //true or flase
			wifi_promisc_enable(status, &promisc_para);
			break;
		}
		case WHC_API_WIFI_SPEAKER: {
			u8 set_type = (u8)p_ipc_msg->param_buf[0];
			union rtw_speaker_set *settings = (union rtw_speaker_set *)p_ipc_msg->param_buf[1];
			DCache_Invalidate((u32)(settings), sizeof(union rtw_speaker_set));
			wifi_speaker_setting(set_type, settings);
			break;
		}
#endif
		case WHC_API_WIFI_SET_CHPLAN: {
			u8 chplan = (u8)p_ipc_msg->param_buf[0];
			wifi_set_chplan(chplan);
			break;
		}
		case WHC_API_WIFI_SET_COUNTRY_CODE: {
			u8 *cntcode = (u8 *)p_ipc_msg->param_buf[0];
			if (cntcode) {
				DCache_Invalidate((u32)cntcode, 2);
			}
			wifi_set_countrycode(cntcode);
			break;
		}
#if !defined(CONFIG_MP_SHRINK)
		case WHC_API_WIFI_SET_OWE_PARAM: {
			struct rtw_owe_param_t *owe_param = (struct rtw_owe_param_t *)p_ipc_msg->param_buf[0];
			DCache_Invalidate((u32)(owe_param), sizeof(struct rtw_owe_param_t));
			wifi_set_owe_param(owe_param);
			p_ipc_msg->ret = 0;
			break;
		}
		case WHC_API_WIFI_SET_EDCCA_PARAM: {
			struct rtw_edcca_param_t *param = (struct rtw_edcca_param_t *)p_ipc_msg->param_buf[0];
			if (param) {
				DCache_Invalidate((u32)(param), sizeof(struct rtw_edcca_param_t));
			}

			switch (param->edcca_mode) {
			case RTW_EDCCA_NORM:
			case RTW_EDCCA_ADAPT:
			case RTW_EDCCA_CS:
			case RTW_EDCCA_DISABLE:
				wifi_set_edcca_param(param);
				p_ipc_msg->ret = 0;
				break;
			default:
				p_ipc_msg->ret = -1;
				break;
			}
			break;
		}
		case WHC_API_WIFI_GET_EDCCA_MODE: {
			u8 *edcca_mode = (u8 *)p_ipc_msg->param_buf[0];

			*edcca_mode = wifi_user_config.rtw_edcca_mode;
			DCache_Clean((u32)edcca_mode, sizeof(u8));
			p_ipc_msg->ret = 0;
			break;
		}
		case WHC_API_WIFI_SET_TX_POWER: {
			struct rtw_tx_power_ctl_info *txpwr_ctrl_info = (struct rtw_tx_power_ctl_info *)p_ipc_msg->param_buf[0];
			DCache_Invalidate((u32)(txpwr_ctrl_info), sizeof(struct rtw_tx_power_ctl_info));
			p_ipc_msg->ret = wifi_set_tx_power(txpwr_ctrl_info);
			break;
		}
		case WHC_API_WIFI_GET_TX_POWER: {
			u8 rate = (u8)p_ipc_msg->param_buf[0];
			s8 *txpwr = (s8 *)p_ipc_msg->param_buf[1];
			DCache_Invalidate((u32)txpwr, sizeof(s8));
			ret = wifi_get_tx_power(rate, txpwr);
			DCache_Clean((u32)txpwr, sizeof(s8));
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SET_CONN_STEP_TRY_LIMIT: {
			struct rtw_conn_step_retries *conn_step_retries = (struct rtw_conn_step_retries *)p_ipc_msg->param_buf[0];
			DCache_Invalidate((u32)conn_step_retries, sizeof(struct rtw_conn_step_retries));
			wifi_set_conn_step_try_limit(conn_step_retries);
			p_ipc_msg->ret = 0;
			break;
		}

		case WHC_API_WIFI_AP_TOGGLE_VISIBLE: {
			u8 enable = (u8)p_ipc_msg->param_buf[0];
			wifi_ap_set_invisible(enable);
			p_ipc_msg->ret = 0;
			break;
		}

		case WHC_API_WIFI_WAKE_PLL_RDY_IN_PS: {
			u8 need = (u8)p_ipc_msg->param_buf[0];
			ret = wifi_wake_pll_rdy_in_ps_state(need);
			p_ipc_msg->ret = ret;
			break;
		}

		case WHC_API_WIFI_PS_EN_BY_BT_ON: {
			u8 enable = (u8)p_ipc_msg->param_buf[0];
			wifi_ps_en_by_bt_state(enable);
			p_ipc_msg->ret = 0;
			break;
		}

		case WHC_API_WIFI_SET_TX_ADVANCED_CFG: {
			struct rtw_tx_advanced_cfg *tx_setting = (struct rtw_tx_advanced_cfg *)p_ipc_msg->param_buf[0];
			DCache_Invalidate((u32)tx_setting, sizeof(struct rtw_tx_advanced_cfg));
			ret = wifi_set_tx_advanced_config(tx_setting);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_TSF_SYNC_TO_TARGET: {
			u8 enable = (u8)p_ipc_msg->param_buf[0];
			u8 *mac_addr = (u8 *)p_ipc_msg->param_buf[1];
			DCache_Invalidate((u32)mac_addr, ETH_ALEN);
			ret = wifi_tsf_sync_to_user_target(enable, mac_addr);
			p_ipc_msg->ret = ret;
			break;
		}
		case WHC_API_WIFI_SET_P1_TO_P0_TSF_OFFSET: {
			u32 offset_us = p_ipc_msg->param_buf[0];
			ret = wifi_set_p1_to_p0_tsf_offset(offset_us);
			p_ipc_msg->ret = ret;
			break;
		}
#endif
#ifdef CONFIG_SUPPLICANT_SME
		case WHC_API_WIFI_SME_AUTH: {
			struct rtw_sme_auth_info *pdata = (struct rtw_sme_auth_info *)p_ipc_msg->param_buf[0];
			DCache_Invalidate((u32)(pdata), sizeof(struct rtw_sme_auth_info));
			DCache_Invalidate((u32)(pdata) + sizeof(struct rtw_sme_auth_info), pdata->data_len);
			rtw_sme_auth_event(pdata);
			p_ipc_msg->ret = 0;
			break;
		}
		case WHC_API_WIFI_SME_SET_ASSOCREQ_IE: {
			u8 *ptr = (u8 *)p_ipc_msg->param_buf[0];
			u32 size = p_ipc_msg->param_buf[1];

			DCache_Invalidate((u32)(ptr), size);
			rtw_sme_set_assocreq_ie(ptr, size, (u8)(p_ipc_msg->param_buf[2]));
			p_ipc_msg->ret = 0;
			break;
		}
#endif
#if !defined(CONFIG_MP_SHRINK)
		case WHC_API_WIFI_SET_BROADCAST_WHITE_LIST: {
			u16 *buf = (u16 *)p_ipc_msg->param_buf[0];
			u8 list_count = (u16)p_ipc_msg->param_buf[1];
			ret = wifi_set_broadcast_port_wakeup_white_list(buf, list_count);
			p_ipc_msg->ret = ret;
			break;
		}
#endif
#ifdef CONFIG_WHC_DEV_TCPIP_KEEPALIVE
		case WHC_API_WIFI_HOST_READY: {
			if (p_ipc_msg->param_buf[0] == 0) {
				whc_dev_api_set_host_state(WHC_HOST_UNREADY);
			} else {
				whc_dev_api_set_host_state(WHC_HOST_READY);
			}
			break;
		}
#endif
		default:
			ret = FALSE;
#if !defined(CONFIG_MP_SHRINK)
#ifdef CONFIG_NAN
			if (p_ipc_msg->api_id > NAN_API_BASE && p_ipc_msg->api_id < NAN_API_BASE + 0x1000) {
				ret |= whc_ipc_dev_nan_api_hdl(p_ipc_msg);
			}
#endif
#ifdef CONFIG_P2P
			if (p_ipc_msg->api_id > P2P_API_BASE && p_ipc_msg->api_id < P2P_API_BASE + 0x1000) {
				ret |= whc_ipc_dev_p2p_api_hdl(p_ipc_msg);
			}
#endif
#ifdef CONFIG_WIFI_TUNNEL
			if (p_ipc_msg->api_id > WTN_API_BASE && p_ipc_msg->api_id < WTN_API_BASE + 0x1000) {
				ret |= whc_ipc_dev_wtn_api_hdl(p_ipc_msg);
				ret |= whc_ipc_dev_wtn_ota_api_hdl(p_ipc_msg);
#ifdef CONFIG_WTN_RNAT_INTERCOM
				ret |= whc_ipc_dev_rmesh_rnat_api_hdl(p_ipc_msg);
#endif
			}
#endif
#ifdef CONFIG_WIFI_XMESH
			if (p_ipc_msg->api_id > XMESH_API_BASE && p_ipc_msg->api_id < XMESH_API_BASE + 0x1000) {
				ret |= whc_ipc_dev_xmesh_api_hdl(p_ipc_msg);
			}
#endif
#endif
			if (ret == FALSE) {
				RTK_LOGE(TAG_WLAN_INIC, "Unknown API(0x%x)\n", p_ipc_msg->api_id);
			}
			break;
		}
		if (call_noused) {
			RTK_LOGE(TAG_WLAN_INIC, "Compile NP after AP! %d\n", call_noused);
			while (1);
		}

		/* Set api_id to 0 to notify AP that API is finished. */
		p_ipc_msg->api_id = 0;
		DCache_Clean((u32)p_ipc_msg, sizeof(struct whc_ipc_host_req_msg));

	} while (1);
}

/**
 * @brief  to send an ipc message to host and wait result.
 * @param  ID[in]: api_id.
 * @param  param_buf[inout]: pointer to API parameter.
 * @return result of API.
 */
int whc_ipc_dev_api_message_send(u32 id, u32 *param_buf, u32 buf_len)
{
	int ret = 0;
	latest_enevt_id = ((id << 16) | (param_buf[0] & 0xffff));
#ifdef CONFIG_WHC_DEV_TCPIP_KEEPALIVE
	u8 host_cnt = 200;
	while (!whc_dev_api_get_host_rdy()) {
		rtos_time_delay_ms(1);
		host_cnt--;
		if (host_cnt == 0) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "Host uninit(0x%08x)\n", id);
			return -1;
		}
	}
#endif
	rtos_sema_take(g_dev_inic_api_message_send_sema, 0xFFFFFFFF);
	u32 cnt = 0;

	memset(&g_dev_ipc_api_request_info, 0, sizeof(struct whc_ipc_dev_req_msg));

	g_dev_ipc_api_request_info.enevt_id = id;
	if (param_buf != NULL) {
		memcpy(g_dev_ipc_api_request_info.param_buf, param_buf, buf_len * sizeof(u32));
	}
	DCache_Clean((u32)&g_dev_ipc_api_request_info, sizeof(struct whc_ipc_dev_req_msg));

#ifdef IPC_DIR_MSG_TX
	memset(&g_dev_ipc_api_msg, 0, sizeof(IPC_MSG_STRUCT));
	g_dev_ipc_api_msg.msg_type = IPC_USER_POINT;
	g_dev_ipc_api_msg.msg = (u32)&g_dev_ipc_api_request_info;
	g_dev_ipc_api_msg.msg_len = sizeof(struct whc_ipc_dev_req_msg);
	DCache_Clean((u32)&g_dev_ipc_api_msg, sizeof(IPC_MSG_STRUCT));

	ipc_send_message(IPC_DIR_MSG_TX, IPC_D2H_WIFI_API_TRAN, \
					 &g_dev_ipc_api_msg);
#else
	ipc_send_message(IPC_INT_CHAN_WIFI_API_TRAN, &g_dev_ipc_api_request_info);
#endif /* IPC_DIR_MSG_TX */

	while (g_dev_ipc_api_request_info.enevt_id != WHC_API_PROCESS_DONE) {
		if (id == WHC_API_PROMISC_CALLBACK || id == WHC_API_WTN_OTA_CALLBACK || id == WHC_API_WTN_RNAT_ADV_CALLBACK) {
			DelayUs(5);
			/* warning every 1s */
			cnt = (cnt + 1) % 200000;
		} else if ((id == WHC_API_WIFI_EVENT) && (g_dev_ipc_api_request_info.param_buf[0] == RTW_EVENT_CSI_DONE ||
				   g_dev_ipc_api_request_info.param_buf[0] == RTW_EVENT_RADAR_PROC_RPT)) {
			/* high sounding rate csi/radar requires less processing time, so used us delay instead of ms */
			DelayUs(10);
			/* warning every 1s */
			cnt = (cnt + 1) % 100000;
		} else {
			rtos_time_delay_ms(1);
			/* warning every 1s */
			cnt = (cnt + 1) % 1000;
		}
		if (cnt == 0) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "Dev api ipc timeout: cur id 0x%x, evt 0x%x; latest id 0x%x, evt 0x%x\n", g_dev_ipc_api_request_info.enevt_id,
					 ((g_dev_ipc_api_request_info.enevt_id == WHC_API_WIFI_EVENT) ? g_dev_ipc_api_request_info.param_buf[0] : 0), (latest_enevt_id >> 16),
					 ((latest_enevt_id >> 16) == WHC_API_WIFI_EVENT) ? (latest_enevt_id & 0xffff) : 0);
			if (g_dev_ipc_api_request_info.enevt_id == WHC_API_PROMISC_CALLBACK) {
				RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "Not support call APIs in Promisc callback!\n");
			}
		}
		DCache_Invalidate((u32)&g_dev_ipc_api_request_info, sizeof(struct whc_ipc_dev_req_msg));
	}
	ret = g_dev_ipc_api_request_info.ret;
	rtos_sema_give(g_dev_inic_api_message_send_sema);
	return ret;
}

void whc_ipc_dev_wifi_event_indicate(u32 event_cmd, u8 *evt_info, s32 evt_len)
{
	/*not indicate Beacon event since it is too frequent*/
	/*not indicate scan event since it is handled in device*/
	u32 param_buf[3];
	param_buf[0] = event_cmd;
	param_buf[1] = (u32)evt_info;
	param_buf[2] = evt_len;
	DCache_Clean((u32)evt_info, evt_len);
	whc_ipc_dev_api_message_send(WHC_API_WIFI_EVENT, param_buf, 3);
}

void whc_ipc_dev_scan_user_callback_indicate(unsigned int ap_num, void *user_data)
{
	u32 param_buf[2];
	param_buf[0] = ap_num;
	param_buf[1] = (u32)user_data;

	whc_ipc_dev_api_message_send(WHC_API_SCAN_USER_CALLBACK, param_buf, 2);
}

void whc_ipc_dev_acs_info_indicate(struct rtw_acs_mntr_rpt *acs_mntr_rpt)
{
	u32 param_buf[1];

	if (acs_mntr_rpt) {
		DCache_Clean((u32)acs_mntr_rpt, sizeof(struct rtw_acs_mntr_rpt));
	}
	param_buf[0] = (u32)acs_mntr_rpt;

	whc_ipc_dev_api_message_send(WHC_API_IP_ACS, param_buf, 1);
}

void whc_ipc_dev_scan_each_report_user_callback_indicate(struct rtw_scan_result *scanned_ap_info, void *user_data, u8 *ies, u32 ie_len)
{
	u32 param_buf[4];

	if (scanned_ap_info) {
		DCache_Clean((u32)scanned_ap_info, sizeof(struct rtw_scan_result));
	}

	if (ies) {
		DCache_Clean((u32)ies, ie_len);
	}

	param_buf[0] = (u32)scanned_ap_info;
	param_buf[1] = (u32)user_data;
	param_buf[2] = (u32)ies;
	param_buf[3] = (u32)ie_len;

	whc_ipc_dev_api_message_send(WHC_API_SCAN_EACH_REPORT_USER_CALLBACK, param_buf, sizeof(param_buf) / sizeof(u32));
}

void whc_ipc_dev_update_regd_event_indicate(struct rtw_country_code_table *table)
{
	struct rtw_country_code_table *ptab = NULL;
	u32 param_buf[1];

	ptab = (struct rtw_country_code_table *)rtos_mem_zmalloc(sizeof(struct rtw_country_code_table));
	if (ptab == NULL) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "allocate rtw_country_code_table failed!\n");
		return;
	}
	memcpy((void *)ptab, (void *)table, sizeof(struct rtw_country_code_table));
	DCache_Clean((u32)ptab, sizeof(struct rtw_country_code_table));
	param_buf[0] = (u32)ptab;

	whc_ipc_dev_api_message_send(WHC_API_UPDATE_REGD_EVENT, param_buf, 1);
	rtos_mem_free(ptab);
}

u8 whc_ipc_dev_promisc_callback_indicate(struct rtw_rx_pkt_info *ppktinfo)
{
	u32 param_buf[3];
	uint16_t origin_prio;
	u8 res = RTW_PROMISC_NEED_DRV_HDL;

	/*make whc_ipc_dev_api_task priority higer to process API event during callback*/
	origin_prio = rtos_task_priority_get(api_dev_task);
	rtos_task_priority_set(api_dev_task, 6);

	DCache_Clean((u32)ppktinfo->buf, (u32)ppktinfo->len);
	DCache_CleanInvalidate((u32)ppktinfo, sizeof(struct rtw_rx_pkt_info));
	param_buf[0] = (u32)ppktinfo;
	res = (u8)whc_ipc_dev_api_message_send(WHC_API_PROMISC_CALLBACK, param_buf, 1);
	rtos_task_priority_set(api_dev_task, origin_prio);
	return res;
}

int whc_dev_ip_in_table_indicate(u8 gate, u8 ip)
{
	u32 param_buf[2];
	int ret;
	param_buf[0] = gate;
	param_buf[1] = ip;

	ret = whc_ipc_dev_api_message_send(WHC_API_IP_TABLE_CHK, param_buf, 2);
	return ret;
}

void whc_ipc_dev_ap_ch_switch_callback_indicate(unsigned char channel, s8 ret)
{
	u32 param_buf[2];

	param_buf[0] = (u32)channel;
	param_buf[1] = (u32)ret;
	whc_ipc_dev_api_message_send(WHC_API_AP_CH_SWITCH, param_buf, 2);
}
int whc_ipc_dev_get_lwip_info(u32 type, unsigned char *input, int index)
{
	int ret;
	u32 param_buf[3];
	param_buf[0] = type;
	param_buf[1] = (u32)input;
	param_buf[2] = index;
	if (input && (index == WHC_WLAN_IS_VALID_IP)) {
		DCache_Clean((u32)input, 4);
	}
	ret = whc_ipc_dev_api_message_send(WHC_API_GET_LWIP_INFO, param_buf, 3);

	if (type == WHC_WLAN_GET_IP || type == WHC_WLAN_GET_GW || type == WHC_WLAN_GET_GWMSK) {
		DCache_Invalidate((u32)ret, 4);
	}
	if (type == WHC_WLAN_GET_HW_ADDR) {
		DCache_Invalidate((u32)ret, 6);
	}

	return ret;
}

int whc_ipc_dev_set_netif_info(int idx_wlan, unsigned char *dev_addr)
{
	int ret;
	u32 param_buf[2];
	param_buf[0] = idx_wlan;
	param_buf[1] = (u32)dev_addr;
	if (dev_addr) {
		DCache_Clean((u32)dev_addr, ETH_ALEN);
	}
	ret = whc_ipc_dev_api_message_send(WHC_API_SET_NETIF_INFO, param_buf, 2);

	return ret;
}

void whc_ipc_dev_cfg80211_indicate_scan_report(u32 channel, u32 frame_is_bcn, s32 rssi, u8 *mac_addr, u8 *IEs, u32 ie_len)
{
	u32 param_buf[6];
	param_buf[0] = channel;
	param_buf[1] = frame_is_bcn;
	param_buf[2] = (u32)rssi;
	param_buf[3] = (u32)mac_addr;
	param_buf[4] = (u32)IEs;
	param_buf[5] = ie_len;

	if (mac_addr) {
		DCache_Clean((u32)mac_addr, ETH_ALEN);
	}
	if (IEs && (ie_len > 0)) {
		DCache_Clean((u32)IEs, ie_len);
	}
	whc_ipc_dev_api_message_send(WHC_API_CFG80211_SCAN_REPORT, param_buf, 6);
}

/**
 * @brief  to handle the ipc message interrupt, wakeup event task to process.
 * @param  Data[inout]: IPC data.
 * @param  IrqStatus[in]: interrupt status.
 * @param  ChanNum[in]: IPC channel number.
 * @return none.
 */
void whc_ipc_dev_api_int_hdl(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	(void) Data;
	(void) IrqStatus;
	(void) ChanNum;

	/* wakeup task */
	rtos_sema_give(g_dev_inic_api_task_wake_sema);
}

/**
 * @brief  to initialize the ipc host for WIFI api.
 * @param  none.
 * @return none.
 */
void whc_ipc_dev_api_init(void)
{
	/* initialize the semaphores */
	rtos_sema_create_static(&g_dev_inic_api_task_wake_sema, 0, 0xFFFFFFFF);
	rtos_sema_create_static(&g_dev_inic_api_message_send_sema, 0, 0xFFFFFFFF);
	rtos_sema_give(g_dev_inic_api_message_send_sema);

	/* Initialize the event task */
	if (RTK_SUCCESS != rtos_task_create(&api_dev_task, (const char *const)"whc_ipc_dev_api_task", (rtos_task_function_t)whc_ipc_dev_api_task, NULL,
										g_rtw_task_size.ipc_dev_api_task,
										CONFIG_INIC_IPC_DEV_API_PRIO)) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "init api_dev_task Err!!\n");
	}
}

#ifdef CONFIG_NAN
void whc_ipc_dev_cfg80211_indicate_nan_match(u8 type, u8 inst_id, u8 peer_inst_id, u8 *addr, u32 info_len, u8 *info, u64 cookie)
{
	if (!wifi_user_config.cfg80211) {
		return;
	}

	u32 param_buf[8];
	param_buf[0] = type;
	param_buf[1] = inst_id;
	param_buf[2] = peer_inst_id;
	param_buf[3] = (u32)addr;
	param_buf[4] = (u32)info;
	param_buf[5] = info_len;
	param_buf[6] = cookie & 0xFFFFFFFF;
	param_buf[7] = cookie >> 32 & 0xFFFFFFFF;
	if (addr) {
		DCache_Clean((u32)addr, ETH_ALEN);
	}
	if (info && (info_len > 0)) {
		DCache_Clean((u32)info, info_len);
	}

	whc_ipc_dev_api_message_send(WHC_API_CFG80211_NAN_REPORT_MATCH_EVENT, param_buf, 8);
}

void whc_ipc_dev_cfg80211_nan_func_free(u64 data)
{
	if (!wifi_user_config.cfg80211) {
		return;
	}

	u32 param_buf[2];
	param_buf[0] = (u32)(data & 0xFFFFFFFF);
	param_buf[1] = (u32)((data >> 32) & 0xFFFFFFFF);

	whc_ipc_dev_api_message_send(WHC_API_CFG80211_NAN_DEL_FUNC, param_buf, 2);
}

void whc_ipc_dev_cfg80211_nan_cfgvendor_event_report(u16 event_id, void *event, int event_len)
{
#ifndef CONFIG_WIFI_NAN_HOST_APP
	/* The in-firmware NAN host (km4_tz) consumes these vendor events over IPC
	 * regardless of the cfg80211 (linux-host) flag, which the RTOS host clears
	 * at registration (whc_event_wifi_set_host_rtos). */
	if (!wifi_user_config.cfg80211) {
		return;
	}
#endif

	u32 param_buf[3];
	param_buf[0] = (u32)event_id;
	param_buf[1] = (u32)event;
	param_buf[2] = (u32)event_len;
	if (event && (event_len > 0)) {
		DCache_Clean((u32)event, event_len);
	}

	whc_ipc_dev_api_message_send(WHC_API_CFG80211_NAN_CFGVENDOR_EVENT, param_buf, 3);
}

void whc_ipc_dev_cfg80211_cfgvendor_send_cmd_reply(void *data, int len)
{
#ifndef CONFIG_WIFI_NAN_HOST_APP
	if (!wifi_user_config.cfg80211) {
		return;
	}
#endif

	u32 param_buf[2];
	param_buf[0] = (u32)data;
	param_buf[1] = (u32)len;
	if (data && (len > 0)) {
		DCache_Clean((u32)data, len);
	}

	whc_ipc_dev_api_message_send(WHC_API_CFG80211_NAN_CFGVENDOR_CMD_REPLY, param_buf, 2);
}

__weak u8 whc_ipc_dev_nan_api_hdl(struct whc_ipc_host_req_msg *p_ipc_msg)
{
	u8 ret = TRUE;
	switch (p_ipc_msg->api_id) {
	case WHC_API_NAN_INIT: {
		rtw_nan_intfs_init();
		p_ipc_msg->ret = 0;
		break;
	}
	case WHC_API_NAN_DEINIT: {
		rtw_nan_intfs_deinit();
		p_ipc_msg->ret = 0;
		break;
	}
	case WHC_API_NAN_START: {
		u8 master_pref = (u8)p_ipc_msg->param_buf[0];
		u8 band_support = p_ipc_msg->param_buf[1];
		rtw_nan_start_api(master_pref, band_support, NAN_CMD_TYPE_NATIVE_IW);
		p_ipc_msg->ret = 0;
		break;
	}
	case WHC_API_NAN_STOP: {
		rtw_nan_stop_api();
		p_ipc_msg->ret = 0;
		break;
	}
	case WHC_API_NAN_ADD_FUNC: {
		struct rtw_nan_func_t *func = (void *)p_ipc_msg->param_buf[0];
		u64 nan_func_pointer = ((u64)p_ipc_msg->param_buf[2] << 32) | p_ipc_msg->param_buf[1];
		u32 len = p_ipc_msg->param_buf[3];
		DCache_Invalidate((u32)func, len);
		p_ipc_msg->ret = rtw_nan_func_add(func, nan_func_pointer);
		break;
	}
	case WHC_API_NAN_DEL_FUNC: {
		u64 cookie = 0;
		cookie = (u64)(p_ipc_msg->param_buf[1]);
		cookie = cookie << 32;
		cookie |= p_ipc_msg->param_buf[0];
		p_ipc_msg->ret = rtw_nan_func_del(cookie);
		break;
	}
	case WHC_API_NAN_CFGVENFOR: {
		u16 vendor_cmd = p_ipc_msg->param_buf[0];
		void *data = (void *)p_ipc_msg->param_buf[1];
		u32 len = p_ipc_msg->param_buf[2];
		DCache_Invalidate((u32)data, len);
		rtw_cfgvendor_cmd_process(vendor_cmd, data, len);
		p_ipc_msg->ret = 0;
		break;
	}
	default: {
		ret = FALSE;
		break;
	}
	}
	return ret;
}

#endif

#ifdef CONFIG_P2P
__weak u8 whc_ipc_dev_p2p_api_hdl(struct whc_ipc_host_req_msg *p_ipc_msg)
{
	u8 ret = TRUE;
	switch (p_ipc_msg->api_id) {
	case WHC_API_P2P_ROLE: {
		enum rtw_p2p_role role = (enum rtw_p2p_role)p_ipc_msg->param_buf[0];
		rtw_p2p_set_role(role);
		break;
	}
	case WHC_API_P2P_REMAIN_ON_CH: {
		u8 wlan_idx = (u8)p_ipc_msg->param_buf[0];
		u8 enable = (u8)p_ipc_msg->param_buf[1];
		p_ipc_msg->ret = rtw_p2p_remain_on_ch(wlan_idx, enable);
		break;
	}
	default: {
		ret = FALSE;
		break;
	}
	}
	return ret;
}
#endif

#ifdef CONFIG_WIFI_TUNNEL
#ifdef CONFIG_WTN_SOCKET_APP
int whc_ipc_dev_wtn_socket_send(u8 *buf, u32 len)
{
	int ret = RTK_FAIL;
	u32 param_buf[2];

	param_buf[0] = (u32)buf;
	param_buf[1] = len;

	DCache_Clean((u32)buf, len);

	ret = whc_ipc_dev_api_message_send(WHC_API_WTN_SOCKET_SEND, param_buf, 2);
	return ret;
}

__weak void whc_ipc_dev_wtn_socket_init(u8 enable, u8 rnat_ap_start)
{
	u32 param_buf[2];

	param_buf[0] = (u32)enable;
	param_buf[1] = (u32)rnat_ap_start;
	whc_ipc_dev_api_message_send(WHC_API_WTN_SOCKET_INIT, param_buf, 2);
}
#endif

void whc_ipc_dev_wtn_rnat_ap_init(u8 enable)
{
	u32 param_buf[1];

	param_buf[0] = (u32)enable;
	whc_ipc_dev_api_message_send(WHC_API_WTN_RNAT_AP_INIT, param_buf, 1);
}

void whc_ipc_dev_wtn_rnat_join_group(u8 *bssid)
{
	u32 param_buf[2];

	param_buf[0] = (u32)bssid;
	param_buf[1] = ETH_ALEN;
	DCache_Clean((u32)bssid, ETH_ALEN);
	whc_ipc_dev_api_message_send(WHC_API_WTN_RNAT_JOIN_GROUP, param_buf, 2);
}

__weak u8 whc_ipc_dev_wtn_api_hdl(struct whc_ipc_host_req_msg *p_ipc_msg)
{
	u8 ret = TRUE;
	switch (p_ipc_msg->api_id) {
	case WHC_API_WTN_FIX_FATHER: {
		u8 enable = (u8)p_ipc_msg->param_buf[0];
		u8 *father_mac = (u8 *)p_ipc_msg->param_buf[1];
		DCache_Invalidate((u32)father_mac, ETH_ALEN);
		wifi_rmesh_fix_father(enable, father_mac);
		break;
	}
	default: {
		ret = FALSE;
		break;
	}
	}
	return ret;
}

__weak u8 whc_ipc_dev_wtn_ota_api_hdl(struct whc_ipc_host_req_msg *p_ipc_msg)
{
	u8 ret = TRUE;
	switch (p_ipc_msg->api_id) {

	case WHC_API_WTN_OTA_CALLBACK_REGISTER: {
		wifi_rmesh_register_ota_callback((void *)p_ipc_msg->param_buf[0]);
		break;
	}

	case WHC_API_WTN_GET_NODE_INFO: {
		enum rtw_rmesh_node_type type = p_ipc_msg->param_buf[0];
		struct rtw_rmesh_node_info *node_info = (struct rtw_rmesh_node_info *)p_ipc_msg->param_buf[1];
		DCache_Invalidate((u32)node_info, sizeof(struct rtw_rmesh_node_info));
		p_ipc_msg->ret = wifi_rmesh_get_node_info(type, node_info);
		DCache_Clean((u32)node_info, sizeof(struct rtw_rmesh_node_info));
		break;
	}

	case WHC_API_WTN_GET_CHILD_NUM: {
		p_ipc_msg->ret = wifi_rmesh_get_child_num();
		break;
	}

	case WHC_API_WTN_GET_CHILD_MAC_LIST: {
		u8 *child_num = (u8 *)p_ipc_msg->param_buf[0];
		struct rtw_rmesh_node_info *child_info_list = (struct rtw_rmesh_node_info *)p_ipc_msg->param_buf[1];
		DCache_Invalidate((u32)child_num, sizeof(u32));
		DCache_Invalidate((u32)child_info_list, sizeof(struct rtw_rmesh_node_info) * (*child_num));
		p_ipc_msg->ret = wifi_rmesh_get_child_info_list(child_num, child_info_list);
		DCache_Clean((u32)child_num, sizeof(u32));
		DCache_Clean((u32)child_info_list, sizeof(struct rtw_rmesh_node_info) * (*child_num));
		break;
	}

	case WHC_API_WTN_CHECK_OTA_VER: {
		u8 *ota_ver = (u8 *)p_ipc_msg->param_buf[0];
		u8 ota_ver_len = (u8)p_ipc_msg->param_buf[1];
		enum rtw_rmesh_node_type type = p_ipc_msg->param_buf[2];
		DCache_Invalidate((u32)ota_ver, ota_ver_len);
		p_ipc_msg->ret = wifi_rmesh_check_node_ota_ver(ota_ver, ota_ver_len, type);
		break;
	}

	case WHC_API_WTN_UPDATE_OTA_VER: {
		u8 *ota_ver = (u8 *)p_ipc_msg->param_buf[0];
		u8 ota_ver_len = (u8)p_ipc_msg->param_buf[1];
		u8 ota_ongoing = (u8)p_ipc_msg->param_buf[2];
		DCache_Invalidate((u32)ota_ver, ota_ver_len);
		p_ipc_msg->ret = wifi_rmesh_update_node_ota_ver(ota_ver, ota_ver_len, ota_ongoing);
		break;
	}
	default: {
		ret = FALSE;
		break;
	}
	}
	return ret;
}

__weak int whc_ipc_dev_wtn_ota_callback_indicate(u8 *buf, u16 len)
{
	u32 param_buf[2];
	int ret = RTK_FAIL;

	DCache_Clean((u32)buf, (u32)len);
	param_buf[0] = (u32)buf;
	param_buf[1] = (u32)len;
	ret = (u8)whc_ipc_dev_api_message_send(WHC_API_WTN_OTA_CALLBACK, param_buf, 2);
	return ret;
}

#ifdef CONFIG_WTN_RNAT_INTERCOM
__weak int whc_ipc_dev_wtn_rnat_adv_callback_indicate(u8 *buf, u16 len)
{
	u32 param_buf[2];

	DCache_Clean((u32)buf, (u32)len);
	param_buf[0] = (u32)buf;
	param_buf[1] = (u32)len;
	return (u8)whc_ipc_dev_api_message_send(WHC_API_WTN_RNAT_ADV_CALLBACK, param_buf, 2);
}

__weak u8 whc_ipc_dev_rmesh_rnat_api_hdl(struct whc_ipc_host_req_msg *p_ipc_msg)
{
	u8 ret = TRUE;
	switch (p_ipc_msg->api_id) {
	case WHC_API_WTN_RNAT_GET_NEIGHBOR_INFO: {
		u8 *out = (u8 *)p_ipc_msg->param_buf[0];
		u8 max_count = (u8)p_ipc_msg->param_buf[1];
		DCache_Invalidate((u32)out, sizeof(struct wtn_rnat_neighbor) * max_count);
		p_ipc_msg->ret = wifi_rmesh_rnat_get_neighbor_info((struct wtn_rnat_neighbor *)out, max_count);
		DCache_Clean((u32)out, sizeof(struct wtn_rnat_neighbor) * p_ipc_msg->ret);
		break;
	}
	case WHC_API_WTN_RNAT_SET_GW: {
		u8 *gw = (u8 *)p_ipc_msg->param_buf[0];
		DCache_Invalidate((u32)gw, 4);
		wifi_rmesh_rnat_set_gw(gw);
		break;
	}
	case WHC_API_WTN_RNAT_SET_CLIENT_IP: {
		u8 *mac = (u8 *)p_ipc_msg->param_buf[0];
		u8 ip_byte4 = (u8)p_ipc_msg->param_buf[1];
		DCache_Invalidate((u32)mac, ETH_ALEN);
		wifi_rmesh_rnat_set_client_ip(mac, ip_byte4);
		break;
	}
	case WHC_API_WTN_RNAT_ADV_CB_REGISTER: {
		wifi_rmesh_rnat_register_adv_callback((void *)p_ipc_msg->param_buf[0]);
		break;
	}
	default: {
		ret = FALSE;
		break;
	}
	}
	return ret;
}
#endif /* CONFIG_WTN_RNAT_INTERCOM */

#endif

#ifdef CONFIG_WIFI_XMESH
__weak u8 whc_ipc_dev_xmesh_api_hdl(struct whc_ipc_host_req_msg *p_ipc_msg)
{
	u8 ret = TRUE;
	switch (p_ipc_msg->api_id) {
	case WHC_API_XMESH_INIT: {
		u8 channel = (u8)p_ipc_msg->param_buf[0];
		u16 groupid = (u16)p_ipc_msg->param_buf[1];
		u8 selfid = (u8)p_ipc_msg->param_buf[2];
		u8 boot_epoch = (u8)p_ipc_msg->param_buf[3];
		p_ipc_msg->ret = wifi_xmesh_init(channel, groupid, selfid, boot_epoch);
		break;
	}
	case WHC_API_XMESH_GET_TRX_STATISTIC: {
		/* buf layout: [relay_cnt(u32)] [max_id(u32)] [l2(u16[])] [l3(u16[])] [lm3(u16[])] */
		u32 *relay = (u32 *)p_ipc_msg->param_buf[0];
		u8 *max_id_ptr = (u8 *)p_ipc_msg->param_buf[1];
		DCache_Invalidate((u32)relay, sizeof(u32) * 2);
		u8 max_id_in = *max_id_ptr;
		u32 total_size = sizeof(u32) * 2 + sizeof(u16) * 3 * max_id_in;
		DCache_Invalidate((u32)relay, total_size);
		u16 *l2 = (u16 *)(relay + 2);
		u16 *l3 = l2 + max_id_in;
		u16 *lm3 = l3 + max_id_in;
		p_ipc_msg->ret = wifi_xmesh_get_trx_statistic(relay, l2, l3, lm3, max_id_ptr);
		DCache_Clean((u32)relay, total_size);
		break;
	}
	case WHC_API_XMESH_CLEAR_TRX_STATISTIC: {
		p_ipc_msg->ret = wifi_xmesh_clear_trx_statistic();
		break;
	}
	case WHC_API_XMESH_STOP: {
		u8 enable = (u8)p_ipc_msg->param_buf[0];
		p_ipc_msg->ret = wifi_xmesh_stop_xmesh(enable);
		break;
	}
	case WHC_API_XMESH_GET_RSSI_INFO_LIST: {
		u8 *buf = (u8 *)p_ipc_msg->param_buf[0];
		u8 *max_id = (u8 *)p_ipc_msg->param_buf[1];
		DCache_Invalidate((u32)max_id, sizeof(u8));
		u8 max_num = *max_id;
		DCache_Invalidate((u32)buf, max_num);
		p_ipc_msg->ret = wifi_xmesh_get_rssi_info_list(buf, max_id);
		DCache_Clean((u32)max_id, sizeof(u8));
		DCache_Clean((u32)buf, *max_id);
		break;
	}
	default: {
		ret = FALSE;
		break;
	}
	}
	return ret;
}
#endif

#ifdef CONFIG_WHC_DEV_TCPIP_KEEPALIVE
/**
 * @brief  run/complete the STA dhcp on dev's own netif (blocking).
 * @param  idx: wlan interface index.
 * @param  ipinfo: filled with ip/gw/netmask/dns (raw ip4 addr, network byte order) on success.
 * @return DHCP_ADDRESS_ASSIGNED / DHCP_TIMEOUT / DHCP_STOP.
 */
u8 whc_ipc_dev_dhcp(u8 idx, u32 *ipinfo)
{
	u8 ret;

	if (lwip_idx_get_netif(idx) == NULL) {
		return DHCP_STOP;
	}

	ret = lwip_dhcp(idx, DHCP_START);
	if (ret == DHCP_ADDRESS_ASSIGNED) {
		ipinfo[0] = *(u32 *)lwip_get_ip(idx);
		ipinfo[1] = *(u32 *)lwip_get_gw(idx);
		ipinfo[2] = *(u32 *)lwip_get_mask(idx);
#if LWIP_DNS
		struct ip_addr dns;
		lwip_get_dns(&dns);
		ipinfo[3] = ip_2_ip4(&dns)->addr;
#endif
	}

	return ret;
}

/**
 * @brief  push the dev netif's ip/gw/netmask/dns to host.
 * @param  idx: wlan interface index.
 * @return RTK_SUCCESS when handed to host / RTK_FAIL if netif is invalid or host is not ready.
 */
int whc_dev_api_sync_ip_info(u8 idx)
{
	int ret;
	u32 param_buf[5] = {0};

	if (lwip_idx_get_netif(idx) == NULL) {
		return RTK_FAIL;
	}

	/* {idx, ip, gw, netmask, dns} */
	param_buf[0] = idx;
	param_buf[1] = *(u32 *)lwip_get_ip(idx);
	param_buf[2] = *(u32 *)lwip_get_gw(idx);
	param_buf[3] = *(u32 *)lwip_get_mask(idx);
#if LWIP_DNS
	struct ip_addr dns;
	lwip_get_dns(&dns);
	param_buf[4] = ip_2_ip4(&dns)->addr;
#endif

	ret = whc_ipc_dev_api_message_send(WHC_API_DEV_SYNC_IP_INFO, param_buf, 5);
	return ret;
}
#endif

/* ---------------------------- Global Variables ---------------------------- */
