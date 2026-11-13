/*
 * Copyright (c) 2025 Realtek Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "osif.h"
#include "hci_controller.h"
#include "hci_uart.h"
#include "hci_platform.h"
#include "bt_debug.h"
#include "platform_autoconf.h"
#if defined(CONFIG_WLAN) && CONFIG_WLAN
#include "wifi_api.h"
#include "wifi_intf_drv_to_app_internal.h"
extern int wifi_set_ips_internal(u8 enable);
#endif

#define HCI_PHY_EFUSE_OFFSET       0x588
#define HCI_PHY_EFUSE_LEN          0x80
#define HCI_LGC_EFUSE_OFFSET       0x1B0
#define HCI_LGC_EFUSE_LEN          0x50
#define HCI_MAC_ADDR_LEN           6
#define HCI_CONFIG_SIGNATURE       0x8723ab55
#define HCI_CONFIG_HDR_LEN         6

#define HCI_CFG_BAUDRATE           BIT0
#define HCI_CFG_FLOWCONTROL        BIT1
#define HCI_CFG_BD_ADDR            BIT2
#define LEFUSE(x)                  ((x)-HCI_LGC_EFUSE_OFFSET)
#define PEFUSE(x)                  ((x)-HCI_PHY_EFUSE_OFFSET)

#define DBG_BT_VENDOR              0
#define DBG_BT_ON                  1

uint8_t bt_ant_switch = 0xFF;      /* Select BT RF Patch */

static uint8_t hci_phy_efuse[HCI_PHY_EFUSE_LEN]  = {0};
static uint8_t hci_lgc_efuse[HCI_LGC_EFUSE_LEN]  = {0};
unsigned char hci_init_config_s0[] = {
	/* Header */
	0x55, 0xAB, 0x23, 0x87,

	/* Length */
	0x40, 0x00,

	/* BT MAC Address */
	0x30, 0x00, 0x06, 0x8A, 0xD5, 0x23, 0x4C, 0xE0, 0x00,

	/* LOG Uart Baudrate 500000 */
	0x08, 0x00, 0x04, 0x20, 0xA1, 0x07, 0x00,

	/* HCI Uart Baudrate 3M */
	0x0C, 0x00, 0x04, 0x01, 0x80, 0x92, 0x04,

	/* HCI Uart Flow Control */
	0x18, 0x00, 0x01, 0x5C,

	/* BT Wake Host */
	0x42, 0x00, 0x01, 0x09,

	/* Host Wake BT */
	0x43, 0x00, 0x01, 0x07,

	/* RF(unit:0.5dbm): phy_flow_ctrl_para(1), module_txgaink(1), module_txgain_flatk(4), lbt_threshold_ant_out(1), lbt_ant_gain(1) */
	0x78, 0x02, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00,

	/* RF(unit:0.5dbm): iqm_max_txpower_LE1M(1), iqm_max_txpower_LE2M(1), iqm_max_txpower_LE1M_adv(1), iqm_max_txpower_LE2M_adv(1), iqm_max_txpower_LR(1)*/
	0x85, 0x02, 0x05, 0x10, 0x10, 0x10, 0x10, 0x10,

	/* RF(unit:0.5dbm): iqm_max_txpower_LE1M_2402(1), iqm_max_txpower_LE1M_2480(1), iqm_max_txpower_LE2M_2402(1), iqm_max_txpower_LE2M_2480(1)*/
	0x8A, 0x02, 0x04, 0x10, 0x10, 0x10, 0x10
};
unsigned int hci_init_config_len_s0 = sizeof(hci_init_config_s0);

unsigned char hci_init_config_s1[] = {
	/* Header */
	0x55, 0xAB, 0x23, 0x87,

	/* Length */
	0x40, 0x00,

	/* BT MAC Address */
	0x30, 0x00, 0x06, 0x8A, 0xD5, 0x23, 0x4C, 0xE0, 0x00,

	/* LOG Uart Baudrate 500000 */
	0x08, 0x00, 0x04, 0x20, 0xA1, 0x07, 0x00,

	/* HCI Uart Baudrate 3M */
	0x0C, 0x00, 0x04, 0x01, 0x80, 0x92, 0x04,

	/* HCI Uart Flow Control */
	0x18, 0x00, 0x01, 0x5C,

	/* BT Wake Host */
	0x42, 0x00, 0x01, 0x09,

	/* Host Wake BT */
	0x43, 0x00, 0x01, 0x07,

	/* RF(unit:0.5dbm): phy_flow_ctrl_para(1), module_txgaink(1), module_txgain_flatk(4), lbt_threshold_ant_out(1), lbt_ant_gain(1) */
	0x78, 0x02, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00,

	/* RF(unit:0.5dbm): iqm_max_txpower_LE1M(1), iqm_max_txpower_LE2M(1), iqm_max_txpower_LE1M_adv(1), iqm_max_txpower_LE2M_adv(1), iqm_max_txpower_LR(1)*/
	0x85, 0x02, 0x05, 0x10, 0x10, 0x10, 0x10, 0x10,

	/* RF(unit:0.5dbm): iqm_max_txpower_LE1M_2402(1), iqm_max_txpower_LE1M_2480(1), iqm_max_txpower_LE2M_2402(1), iqm_max_txpower_LE2M_2480(1)*/
	0x8A, 0x02, 0x04, 0x10, 0x10, 0x10, 0x10
};
unsigned int hci_init_config_len_s1 = sizeof(hci_init_config_s1);

unsigned char *hci_init_config = NULL;
unsigned int hci_init_config_len = 0;

static uint8_t  hci_cfg_bd_addr[HCI_MAC_ADDR_LEN] = {0};
static uint32_t hci_cfg_flag                      = 0;

void hci_platform_cfg_bd_addr(uint8_t *bdaddr)
{
	/* bdaddr is little endian, please print in reverse order from high address to low address to confirm habits */
	for (uint8_t i = 0; i < HCI_MAC_ADDR_LEN; i++) {
		hci_cfg_bd_addr[i] = bdaddr[i];
	}

	hci_cfg_flag |= HCI_CFG_BD_ADDR;
}

int hci_platform_get_write_phy_efuse_data(uint8_t *data, uint8_t len)
{
	memcpy(data, hci_phy_efuse, len);
	return HCI_SUCCESS;
}

static uint8_t hci_platform_read_antenna(void)
{
	uint8_t val;

	/* Parse BT RF PATH */
	if (bt_ant_switch == 0xFF) { /* Priority of ATM2=ant is higher than efuse in mp image. */
		/* Read Logic Efuse */
		if (RTK_FAIL == OTP_LogicalRead(&val, 0x133, 1)) {
			BT_LOGE("OTP_LogicalRead 0x133 failed\r\n");
			return HCI_FAIL;
		}

		/* Bit[5]: Radio on/off type
		0: combine with WiFi/BT SEL Shared Port
		1: individual /BT SEL Dedicated Port */
		if (val != 0xff) {
			if (val & BIT5) {
				bt_ant_switch = ANT_S0;
			} else {
				bt_ant_switch = ANT_S1;
			}
		} else {
			bt_ant_switch = ANT_S0; /* default use S0 if efuse is not programed */
		}
	}

	BT_LOGA("bt ant %d\r\n", bt_ant_switch);

	return HCI_SUCCESS;
}

static uint8_t hci_platform_read_efuse(void)
{
	uint8_t i;

	/* Read Logic Efuse */
	if (RTK_FAIL == OTP_LogicalRead(hci_lgc_efuse, HCI_LGC_EFUSE_OFFSET, HCI_LGC_EFUSE_LEN)) {
		BT_LOGE("OTP_LogicalRead failed\r\n");
		return HCI_FAIL;
	}

	/* Read Physical Efuse */
	for (i = 0; i < HCI_PHY_EFUSE_LEN; i++) {
		OTP_Read8((HCI_PHY_EFUSE_OFFSET + i), (hci_phy_efuse + i));
	}
#if 0
	BT_DUMPA("Read Logic Efuse:\r\n", hci_lgc_efuse, HCI_LGC_EFUSE_LEN);
	BT_DUMPA("Read Phy Efuse:\r\n", hci_phy_efuse, HCI_PHY_EFUSE_LEN);
#endif

	return HCI_SUCCESS;
}

static uint8_t bt_manual_tx_power_gain_enable = 0;
static uint8_t bt_manual_gain_index = 0xFE;

void hci_platform_set_tx_power_gain_index(uint32_t index)
{
	bt_manual_tx_power_gain_enable = 1;
	bt_manual_gain_index = (uint8_t)(index & 0xFF);
}

void hci_platform_set_antenna(uint8_t ant)
{
	bt_ant_switch = ant;
}

static uint8_t hci_platform_parse_config(void)
{
	uint8_t *p, i;
	uint16_t entry_offset, entry_len, config_length;
	uint32_t config_header;

	if (bt_ant_switch == ANT_S0) {
		hci_init_config = hci_init_config_s0;
		hci_init_config_len = hci_init_config_len_s0;
	} else {
		hci_init_config = hci_init_config_s1;
		hci_init_config_len = hci_init_config_len_s1;
	}

	if (hci_init_config_len <= HCI_CONFIG_HDR_LEN) {
		return HCI_IGNORE;
	}

	p = hci_init_config;
	memcpy((void *)&config_header, (void *)p, sizeof(uint32_t));
	if (HCI_CONFIG_SIGNATURE != config_header) {
		return HCI_FAIL;
	}

	memcpy((void *)&config_length, (void *)(p + 4), sizeof(uint16_t));
	if (config_length != (uint16_t)(hci_init_config_len - HCI_CONFIG_HDR_LEN)) {
		/* Fix the len, just avoid the length is not corect */
		p[4] = (uint8_t)((hci_init_config_len - HCI_CONFIG_HDR_LEN) >> 0);
		p[5] = (uint8_t)((hci_init_config_len - HCI_CONFIG_HDR_LEN) >> 8);
	}

	p += HCI_CONFIG_HDR_LEN;
	while (p < hci_init_config + hci_init_config_len) {
		memcpy((void *)&entry_offset, (void *)p, sizeof(uint16_t));
		entry_len = *(uint8_t *)(p + 2);
		p += 3;

		switch (entry_offset) {
		case 0x000c:
			hci_set_work_baudrate(p);
			break;
		case 0x0018:
			/* MP Mode, Close Flow Control */
			if (hci_is_mp_mode()) {
				p[0] = p[0] & (~BIT2);
			}
			break;
		case 0x0030:
			/* Set ConfigBuf MacAddr, Use Customer Assign or Efuse */
			if (hci_cfg_flag & HCI_CFG_BD_ADDR) {
				for (i = 0; i < HCI_MAC_ADDR_LEN; i++) {
					p[i] = hci_cfg_bd_addr[i];
				}
			} else {
				if ((hci_lgc_efuse[4] != 0xff) || (hci_lgc_efuse[5] != 0xff) || (hci_lgc_efuse[6] != 0xff) || \
					(hci_lgc_efuse[7] != 0xff) || (hci_lgc_efuse[8] != 0xff) || (hci_lgc_efuse[9] != 0xff)) {
					for (i = 0; i < HCI_MAC_ADDR_LEN; i++) {
						p[i] = hci_lgc_efuse[4 + HCI_MAC_ADDR_LEN - 1 - i];
					}
				}
			}
			BT_LOGA("Bluetooth init BT_ADDR in cfgbuf [%02x:%02x:%02x:%02x:%02x:%02x]\r\n",
					p[5], p[4], p[3], p[2], p[1], p[0]);
			break;
		case 0x0278:
			if (hci_lgc_efuse[LEFUSE(0x1be)] != 0xff) {
				p[0] = hci_lgc_efuse[LEFUSE(0x1be)];
				if (hci_lgc_efuse[LEFUSE(0x1be)] & BIT1) {
					p[1] = hci_lgc_efuse[LEFUSE(0x1bf)];
				}

				if (hci_lgc_efuse[LEFUSE(0x1be)] & BIT2) {
					p[2] = hci_lgc_efuse[LEFUSE(0x1c0)];
					p[3] = hci_lgc_efuse[LEFUSE(0x1c1)];
					p[4] = hci_lgc_efuse[LEFUSE(0x1c2)];
					p[5] = hci_lgc_efuse[LEFUSE(0x1c3)];
				}

				if (hci_lgc_efuse[LEFUSE(0x1be)] & BIT5) {
					p[6] = hci_lgc_efuse[LEFUSE(0x1c4)];
					p[7] = hci_lgc_efuse[LEFUSE(0x1c5)];
				}
			}
			break;
		case 0x0285:
			for (i = 0; i < entry_len; i++) {
				if (bt_manual_tx_power_gain_enable) {
					BT_LOGA("0x285 gain 0x%02x\r\n", bt_manual_gain_index);
					p[i] = bt_manual_gain_index;
				} else {
					if (hci_lgc_efuse[LEFUSE(0x1c7 + i)] != 0xff) {
						p[i] = hci_lgc_efuse[LEFUSE(0x1c7 + i)];
					}
				}
			}
			break;
		case 0x028a:
			for (i = 0; i < entry_len; i++) {
				if (bt_manual_tx_power_gain_enable) {
					BT_LOGA("0x28a gain 0x%02x\r\n", bt_manual_gain_index);
					p[i] = bt_manual_gain_index;
				} else {
					if (hci_lgc_efuse[LEFUSE(0x1cc + i)] != 0xff) {
						p[i] = hci_lgc_efuse[LEFUSE(0x1cc + i)];
					}
				}
			}
			break;
		default:
			break;
		}

		p +=  entry_len;
	}

	return HCI_SUCCESS;
}

static void bt_power_on(void)
{
	u32 TempVal = 0;
	u32 pollingCount = 0;

	set_reg_value(0x40230030, BIT12, 1);                // Enable KM4NP to wake BT: 0x4023_0030[12] = 1
	set_reg_value(0x40230030, BIT18, 1);                // Indicate BT is in operation: 0x4023_0030[18] = 1
	do {                                                // Poll WIFIOFF power is on :0x4022_0000[2] = 1
		TempVal = HAL_READ32(0x40220000, 0);
		TempVal &= BIT2;
		if (TempVal == 0) {
			osif_delay(1);
			if (pollingCount++ > 100) {
				BT_LOGE("Poll WIFIOFF power timeout\n\r");
				break;
			}
		} else {
			break;
		}
	} while (1);
	osif_delay_us(200);
	set_reg_value(0x40230030, BIT2, 1);                 // Enable BT RF 3.3V: Write 0x4023_0030[2]=1
	set_reg_value(0x40230030, BIT7, 1);
	set_reg_value(0x40230030, BIT5, 1);                 // Enable BT RF/AFE 0.9V: Write 0x4023_0030[7]=1, Write 0x4023_0030[5]=1
	if (bt_ant_switch == ANT_S0) {
		set_reg_value(0x40230030, BIT6, 0);             // Enable BT to use BTS0 RF: 0x4023_0030[6]=0
	} else {
		set_reg_value(0x40230030, BIT6, 1);             // Enable BT to use WL RF: 0x4023_0030[6]=1
		set_reg_value(0x40220014, BIT0 | BIT1, 3);      // Enable 4Mhz/32Khz clock to WL RFAFE: 0x4022_0014[1:0] = 2'b11
		set_reg_value(0x4022001C, BIT20, 1);            // Enable WL RFAFE control circuit: 0x4022_001C[20]=1
		set_reg_value(0x4023D000, BIT5 | BIT6, 3);      // Enable WL RFAFE: 0x4023_D000[6:5] = 2'b11
		set_reg_value(0x4023D020, 0xff, 0xff);          // Release WL RFC reset: 0x4023_D020[7:0]=8'hFF
	}
	set_reg_value(0x4023B008, BIT19, 1);                // Switch 32K clock source: Write 0x4023B008[19]=1, Write 0x4023B008[18:16] = 0x1
	set_reg_value(0x4023B008, BIT16 | BIT17 | BIT18, 1);
	set_reg_value(0x4023B010, BIT8, 1);                 // Enable 32K clock: Write 0x4023_B010[8]=1
	set_reg_value(0x4023B010, BIT7, 1);                 // Enable 40M clock: Write 0x4023_B010[7]=1
	set_reg_value(0x402300FC, BIT14, 1);                // Enable BTSHARE memory clock: Write 0x4023_00FC[14]=1
	set_reg_value(0x40230108, BIT10, 1);                // Enable BTSHARE memory reset: Write 0x4023_0108[10] = 1
	set_reg_value(0x40230104, BIT3, 1);                 // Enable BT function: Write 0x4023_0104[3]=1
	do {                                                // Poll BT PLL ready: 0x4023_0030[17]=1
		TempVal = HAL_READ32(0x40230030, 0);
		TempVal &= BIT17;
		if (TempVal == 0) {
			osif_delay(1);
			if (pollingCount++ > 100) {
				BT_LOGE("Poll BT PLL timeout\n\r");
				break;
			}
		} else {
			break;
		}
	} while (1);

	// Wait BT wakeup to receive HCI uart data.
	osif_delay(10);

	set_reg_value(0x40230030, BIT11, 1);            // pull out BT_WAKE_HOST
}

void bt_power_off(void)
{
#if defined(CONFIG_WLAN) && CONFIG_WLAN
	if (!(wifi_is_running(STA_WLAN_INDEX) || wifi_is_running(SOFTAP_WLAN_INDEX))) {
		set_reg_value(0x40220014, BIT0 | BIT1, 0);      // disable WLON clock, 0x4022_0014[1:0]=0
		set_reg_value(0x4022001C, BIT20, 0);            // reset WLON, 0x4022_001C[20]=0
#else
	{
#endif
		if (bt_ant_switch == ANT_S1) {
			set_reg_value(0x4023D000, BIT5 | BIT6, 0);  // disable WL RFAFE(0.9V and 3.3V) (keep 2'b11 if BT use WL RFAFE), 0x4023_D000[6:5]=2'b0, 0x4023_D000[9:8]=2'b0
			set_reg_value(0x4023D020, 0xff, 0x00);      // disable WL RFC, keep 8'hFF if BT USE BTG RFC, 0x4023_D020[7:0]=0
		}
	}

	set_reg_value(0x40230104, BIT3, 0);                 // Disable BT function: write 0x4023_0104[3]=0
	set_reg_value(0x40230108, BIT10, 0);                // Disable BTSHARE memory reset: Write 0x4023_0108[10] = 0
	set_reg_value(0x402300FC, BIT14, 0);                // Disable BTSHARE memory clock: Write 0x4023_00FC[14]=0
	set_reg_value(0x4023B010, BIT8, 0);                 // Disable 32K clock: write to 0x4023_B010[8]=0
	set_reg_value(0x4023B010, BIT7, 0);                 // Disable 40M clock: write to 0x4023_B010[7]=0

	// NP disable BT AFE/PLL
	set_reg_value(0x40230030, BIT15, 1);                // Enable BTAFE ISO: 0x4023_0030[15] = 1
	set_reg_value(0x40230760, BIT15, 1);                // Enable BTPLL ISO: 0x4023_0760[15]=1
	set_reg_value(0x40230768, BIT0, 0);                 // Disable BTPLL: 0x4023_0768[0]=0
	set_reg_value(0x4023076C, BIT1 | BIT3 | BIT17 | BIT19, 0);    // Disable LDOs: 0x4023_076C[19,17,3,1] = {0,0,0,0}
	set_reg_value(0x40230030, BIT2, 0);                 // Disable BT RF/AFE 3.3V: Write to 0x4023_0030[2]=0
	set_reg_value(0x40230030, BIT5 | BIT7, 0);          // Disable BT RF/AFE 3.3V, 0.9V: Write to 0x4023_0030[7,5]={0,0}

	set_reg_value(0x40230030, BIT18, 0);                // Indicate BT is function off: 0x4023_0030[18] = 0
}

void hci_platform_controller_reset(void)
{
	/* BT Controller Power */
	bt_power_on();

	BT_LOGD("BT Reset OK!\r\n");
}

bool rtk_bt_pre_enable(void)
{
	if (!hci_platform_read_antenna()) {
		return false;
	}

#if defined(CONFIG_WLAN) && CONFIG_WLAN
	if (bt_ant_switch == ANT_S1) {
		if (!(wifi_is_running(STA_WLAN_INDEX) || wifi_is_running(SOFTAP_WLAN_INDEX))) {
			BT_LOGE("WiFi is OFF! Please Restart BT after Wifi on!\r\n");
			return false;
		}

		wifi_ps_en_by_bt_state(DISABLE);
	}
#endif

	return true;
}

void rtk_bt_post_enable(void)
{
#if defined(CONFIG_WLAN) && CONFIG_WLAN
	if (bt_ant_switch == ANT_S1) {
		wifi_ps_en_by_bt_state(ENABLE);
	}
#endif
}

void hci_platform_external_fw_log_pin(void)
{
	Pinmux_Config(_PC_10, PINMUX_FUNCTION_BT_DEBUG);
	PAD_PullCtrl(_PC_10, GPIO_PuPd_UP);
}

uint8_t hci_platform_open(void)
{
	if (!CHECK_CFG_SW(CFG_SW_BT_FW_LOG)) {
		rtk_bt_fw_log_open();
		BT_LOGA("FW LOG OPEN\r\n");
#if 0
		hci_platform_external_fw_log_pin();
#endif
	}

	/* Read Efuse and Parse Configbuf */
	if (HCI_FAIL == hci_platform_read_efuse()) {
		return HCI_FAIL;
	}

	if (HCI_FAIL == hci_platform_parse_config()) {
		return HCI_FAIL;
	}

	/* BT Controller Reset */
	hci_platform_controller_reset();

	/* UART Open */
	if (HCI_FAIL == hci_uart_open()) {
		return HCI_FAIL;
	}

	return HCI_SUCCESS;
}

void hci_platform_close(void)
{
	/* BT Controller Power Off */
	bt_power_off();

	/* UART Close */
	hci_uart_close();

	if (!CHECK_CFG_SW(CFG_SW_BT_FW_LOG)) {
		rtk_bt_fw_log_close();
	}
}

void hci_platform_free(void)
{
	/* UART Free */
	hci_uart_free();
}

void hci_platform_get_config(uint8_t **buf, uint16_t *len)
{
	*buf = hci_init_config;
	*len = hci_init_config_len;
}

void hci_platform_debug_enable(void)
{
#if 0 // todo
	uint32_t data;

	/* keep bt wakeup from dlps/lps */
	set_reg_value(0x42008250, BIT14, 1);        /* enable HOST_WAKE_BT No GPIO */
	osif_delay(5);
	set_reg_value(0x42008250, BIT13, 1);        /* HOST_WAKE_BT */
	osif_delay(5);
	while (1) {
		osif_delay(100);
		data = HAL_READ32(0x42008254, 0) & 0x1F; /* 0x42008254 [0:4] */
		if (data == 4) {
			/* bt active */
			BT_LOGD("bt active\r\n");
			break;
		} else {
			BT_LOGD("wait for bt wakeup from sleep\r\n");
		}
	}
	BT_LOGD("BT wakeup from sleep\r\n");

	set_reg_value(0x42008BF8, BIT0, 0);                                     /* swd close */
	osif_delay(5);
	set_reg_value(0x42008A08, BIT0 | BIT1 | BIT2 | BIT3 | BIT4, 24);        /* Uart tx PA2 */
	osif_delay(5);
	set_reg_value(0x42008A0C, BIT0 | BIT1 | BIT2 | BIT3 | BIT4, 24);        /* Uart rx PA3 */
	osif_delay(5);
	set_reg_value(0x42008A10, BIT0 | BIT1 | BIT2 | BIT3 | BIT4, 24);        /* RTS PA4 */
	osif_delay(5);
	set_reg_value(0x42008A14, BIT0 | BIT1 | BIT2 | BIT3 | BIT4, 24);        /* CTS PA5 */
	osif_delay(5);
	set_reg_value(0x42008250, BIT19, 1);                                    /* pinmux */
	osif_delay(5);
#endif
}

static void hci_platform_bt_debug_bit(uint8_t bt_dbg_port, char *pad)
{
	(void)pad;

	AON_TypeDef *AON = (AON_TypeDef *)AON_BASE;
	u32 TempVal;

	switch (bt_dbg_port) {
	case 0:
		/* dbg port0 */
		Pinmux_Config(_PB_0, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PB_1, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PB_2, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PB_3, PINMUX_FUNCTION_GPIO);
		// dbg port0 enable:
		AON->AON_OTP_SYSCFG2 |= AON_BIT_DBG_PORT0_EN; // 0x4022_004C[16] = 1
		// dbg port0 selection:
		TempVal = AON->AON_OTP_SYSCFG2;
		TempVal = (TempVal & ~AON_MASK_DBG_PORT0_SEL) | AON_DBG_PORT0_SEL(0x0); // 0x4022_004C[27:24]
		AON->AON_OTP_SYSCFG2 = TempVal;
		break;
	case 1:
		/* dbg port1 */
		Pinmux_Config(_PB_4, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PC_6, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PC_7, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PC_8, PINMUX_FUNCTION_GPIO);
		// dbg port1 enable:
		AON->AON_OTP_SYSCFG2 |= AON_BIT_DBG_PORT1_EN; // 0x4022_004C[17] = 1
		// dbg port1 selection:
		TempVal = AON->AON_OTP_SYSCFG2;
		TempVal = (TempVal & ~AON_MASK_DBG_PORT1_SEL) | AON_DBG_PORT1_SEL(0x1); // 0x4022_004C[31:28]
		AON->AON_OTP_SYSCFG2 = TempVal;
		break;
	case 2:
		/* dbg port2 */
		Pinmux_Config(_PC_9, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PC_10, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PE_0, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PE_1, PINMUX_FUNCTION_GPIO);
		// dbg port2 enable:
		AON->AON_OTP_SYSCFG2 |= AON_BIT_DBG_PORT2_EN; // 0x4022_004C[18] = 1
		// dbg port2 selection:
		TempVal = AON->AON_OTP_SYSCFG3;
		TempVal = (TempVal & ~AON_MASK_DBG_PORT2_SEL) | AON_DBG_PORT2_SEL(0x2); // 0x4022_0050[3:0]
		AON->AON_OTP_SYSCFG3 = TempVal;
		break;
	case 3:
		/* dbg port3 */
		Pinmux_Config(_PE_2, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PE_3, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PE_4, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PE_5, PINMUX_FUNCTION_GPIO);
		// dbg port3 enable:
		AON->AON_OTP_SYSCFG2 |= AON_BIT_DBG_PORT3_EN; // 0x4022_004C[19] = 1
		// dbg port3 selection:
		TempVal = AON->AON_OTP_SYSCFG3;
		TempVal = (TempVal & ~AON_MASK_DBG_PORT3_SEL) | AON_DBG_PORT3_SEL(0x3); // 0x4022_0050[7:4]
		AON->AON_OTP_SYSCFG3 = TempVal;
		break;
	case 4:
		/* dbg port4 */
		Pinmux_Config(_PE_6, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PE_7, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PE_8, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PE_9, PINMUX_FUNCTION_GPIO);
		// dbg port4 enable:
		AON->AON_OTP_SYSCFG2 |= AON_BIT_DBG_PORT4_EN; // 0x4022_004C[20] = 1
		// dbg port4 selection:
		TempVal = AON->AON_OTP_SYSCFG3;
		TempVal = (TempVal & ~AON_MASK_DBG_PORT4_SEL) | AON_DBG_PORT4_SEL(0x4); // 0x4022_0050[11:8]
		AON->AON_OTP_SYSCFG3 = TempVal;
		break;
	case 5:
		/* dbg port5 */
		Pinmux_Config(_PE_10, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PE_11, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PE_12, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PE_13, PINMUX_FUNCTION_GPIO);
		// dbg port5 enable:
		AON->AON_OTP_SYSCFG2 |= AON_BIT_DBG_PORT5_EN; // 0x4022_004C[21] = 1
		// dbg port5 selection:
		TempVal = AON->AON_OTP_SYSCFG3;
		TempVal = (TempVal & ~AON_MASK_DBG_PORT5_SEL) | AON_DBG_PORT5_SEL(0x5); // 0x4022_0050[15:12]
		AON->AON_OTP_SYSCFG3 = TempVal;
		break;
	case 6:
		/* dbg port6 */
		Pinmux_Config(_PE_14, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PE_15, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PE_16, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PF_1, PINMUX_FUNCTION_GPIO);
		// dbg port6 enable:
		AON->AON_OTP_SYSCFG2 |= AON_BIT_DBG_PORT6_EN; // 0x4022_004C[22] = 1
		// dbg port6 selection:
		TempVal = AON->AON_OTP_SYSCFG3;
		TempVal = (TempVal & ~AON_MASK_DBG_PORT6_SEL) | AON_DBG_PORT6_SEL(0x6); // 0x4022_0050[19:16]
		AON->AON_OTP_SYSCFG3 = TempVal;
		break;
	case 7:
		/* dbg port7 */
		Pinmux_Config(_PF_2, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PF_3, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PF_4, PINMUX_FUNCTION_GPIO);
		Pinmux_Config(_PF_5, PINMUX_FUNCTION_GPIO);
		// dbg port7 enable:
		AON->AON_OTP_SYSCFG2 |= AON_BIT_DBG_PORT7_EN; // 0x4022_004C[23] = 1
		// dbg port7 selection:
		TempVal = AON->AON_OTP_SYSCFG3;
		TempVal = (TempVal & ~AON_MASK_DBG_PORT7_SEL) | AON_DBG_PORT7_SEL(0x7); // 0x4022_0050[23:20]
		AON->AON_OTP_SYSCFG3 = TempVal;
		break;
	default:
		BT_LOGE("Invalid setting BT debug port, wrong BT debug port[%d].\r\n", bt_dbg_port);
		break;
	}
}

static void hci_platform_bt_debug_shift_bit(uint8_t original, uint8_t mapping)
{
	AON_TypeDef *AON = (AON_TypeDef *)AON_BASE;
	u32 TempVal;

	switch (mapping) {
	case 0:
		// dbg port0 selection:
		TempVal = AON->AON_OTP_SYSCFG2;
		TempVal = (TempVal & ~AON_MASK_DBG_PORT0_SEL) | AON_DBG_PORT0_SEL(original); // 0x4022_004C[27:24]
		AON->AON_OTP_SYSCFG2 = TempVal;
		break;
	case 1:
		// dbg port1 selection:
		TempVal = AON->AON_OTP_SYSCFG2;
		TempVal = (TempVal & ~AON_MASK_DBG_PORT1_SEL) | AON_DBG_PORT1_SEL(original); // 0x4022_004C[31:28]
		AON->AON_OTP_SYSCFG2 = TempVal;
		break;
	case 2:
		// dbg port2 selection:
		TempVal = AON->AON_OTP_SYSCFG3;
		TempVal = (TempVal & ~AON_MASK_DBG_PORT2_SEL) | AON_DBG_PORT2_SEL(original); // 0x4022_0050[3:0]
		AON->AON_OTP_SYSCFG3 = TempVal;
		break;
	case 3:
		// dbg port3 selection:
		TempVal = AON->AON_OTP_SYSCFG3;
		TempVal = (TempVal & ~AON_MASK_DBG_PORT3_SEL) | AON_DBG_PORT3_SEL(original); // 0x4022_0050[7:4]
		AON->AON_OTP_SYSCFG3 = TempVal;
		break;
	case 4:
		// dbg port4 selection:
		TempVal = AON->AON_OTP_SYSCFG3;
		TempVal = (TempVal & ~AON_MASK_DBG_PORT4_SEL) | AON_DBG_PORT4_SEL(original); // 0x4022_0050[11:8]
		AON->AON_OTP_SYSCFG3 = TempVal;
		break;
	case 5:
		// dbg port5 selection:
		TempVal = AON->AON_OTP_SYSCFG3;
		TempVal = (TempVal & ~AON_MASK_DBG_PORT5_SEL) | AON_DBG_PORT5_SEL(original); // 0x4022_0050[15:12]
		AON->AON_OTP_SYSCFG3 = TempVal;
		break;
	case 6:
		// dbg port6 selection:
		TempVal = AON->AON_OTP_SYSCFG3;
		TempVal = (TempVal & ~AON_MASK_DBG_PORT6_SEL) | AON_DBG_PORT6_SEL(original); // 0x4022_0050[19:16]
		AON->AON_OTP_SYSCFG3 = TempVal;
		break;
	case 7:
		// dbg port7 selection:
		TempVal = AON->AON_OTP_SYSCFG3;
		TempVal = (TempVal & ~AON_MASK_DBG_PORT7_SEL) | AON_DBG_PORT7_SEL(original); // 0x4022_0050[23:20]
		AON->AON_OTP_SYSCFG3 = TempVal;
		break;
	default:
		BT_LOGE("Invalid setting BT debug port shift, wrong shift mapping (%d).\r\n", mapping);
		break;
	}
}

static void hci_platform_bt_gpio_pad(uint8_t bt_gpio, char *pad)
{
	(void)pad;

	CORE_PAD_TypeDef *CORE_PAD = (CORE_PAD_TypeDef *)CORE_PAD_BASE;
	u32 TempVal;

	switch (bt_gpio) {
	case 1:
		/* BT_GPIO_1 */
		TempVal = CORE_PAD->SYSON_GPIOE_0_1_CTRL;
		TempVal = (TempVal & ~CORE_PAD_MASK_SYSON_GPIO0_PINMUX_SEL_02C) | CORE_PAD_SYSON_GPIO0_PINMUX_SEL_02C(0xD); // 0x6020_C42C[3:0]=0xD
		CORE_PAD->SYSON_GPIOE_0_1_CTRL = TempVal;
		break;
	case 3:
		/* BT_GPIO_3 */
		TempVal = CORE_PAD->SYSON_GPIOE_0_1_CTRL;
		TempVal = (TempVal & ~CORE_PAD_MASK_SYSON_GPIO1_PINMUX_SEL_02C) | CORE_PAD_SYSON_GPIO1_PINMUX_SEL_02C(0xD); // 0x6020_C42C[19:16]=0xD
		CORE_PAD->SYSON_GPIOE_0_1_CTRL = TempVal;
		break;
	case 4:
		/* BT_GPIO_4 */
		TempVal = CORE_PAD->SYSON_GPIOE_10_11_CTRL;
		TempVal = (TempVal & ~CORE_PAD_MASK_SYSON_GPIO10_PINMUX_SEL_040) | CORE_PAD_SYSON_GPIO10_PINMUX_SEL_040(0xD); // 0x6020_C440[3:0]=0xD
		CORE_PAD->SYSON_GPIOE_10_11_CTRL = TempVal;
		break;
	case 5:
		/* BT_GPIO_5 */
		TempVal = CORE_PAD->SYSON_GPIOE_10_11_CTRL;
		TempVal = (TempVal & ~CORE_PAD_MASK_SYSON_GPIO11_PINMUX_SEL) | CORE_PAD_SYSON_GPIO11_PINMUX_SEL(0xD); // 0x6020_C440[19:16]=0xD
		CORE_PAD->SYSON_GPIOE_10_11_CTRL = TempVal;
		break;
	case 6:
		/* BT_GPIO_6 */
		TempVal = CORE_PAD->SYSON_GPIOE_12_13_CTRL;
		TempVal = (TempVal & ~CORE_PAD_MASK_SYSON_GPIO12_PINMUX_SEL) | CORE_PAD_SYSON_GPIO12_PINMUX_SEL(0xD); // 0x6020_C444[3:0]=0xD
		CORE_PAD->SYSON_GPIOE_12_13_CTRL = TempVal;
		break;
	case 8:
		/* BT_GPIO_8 */
		TempVal = CORE_PAD->SYSON_GPIOE_12_13_CTRL;
		TempVal = (TempVal & ~CORE_PAD_MASK_SYSON_GPIO13_PINMUX_SEL) | CORE_PAD_SYSON_GPIO13_PINMUX_SEL(0xD); // 0x6020_C444[19:16]=0xD
		CORE_PAD->SYSON_GPIOE_12_13_CTRL = TempVal;
		break;
	default:
		BT_LOGE("Invalid setting BT GPIO, wrong BT GPIO[%d].\r\n", bt_gpio);
		break;
	}
}

static void hci_platform_debug_port_pre_enable(uint8_t bt_sel)
{
	(void)bt_sel;

	AON_TypeDef *AON = (AON_TypeDef *)AON_BASE;
	u32 TempVal;

	/* AON PAD PWR */
	HAL_WRITE32(0x4022A000, 0x0C, 0xFF); // REG_PWRPAD3N_SHDN_CTRL
	HAL_WRITE32(0x4022A000, 0x10, 0xA2); // REG_PWRPAD1N_SHDN_CTRL
	/* PON (SYSON) PAD PWR */
	HAL_WRITE32(0x4023A000, 0x2C, 0x5C); // REG_PWRPAD1N_SHDN_CTRL

	/* SWD disable */
	AON->AON_SYS_CTRL &= ~AON_BIT_DEBUGER_EN; // 0x4022_0068[8] = 0, SWD disable

	/* DBG_PORT_SEL */
	TempVal = AON->AON_OTP_SYSCFG2;
	TempVal = (TempVal & ~AON_MASK_DBG_PORT) | AON_DBG_PORT(0x9A); // REG_AON_OTP_SYSCFG2: 0x4022_004C[15:8], 0x9A : BTTOP
	AON->AON_OTP_SYSCFG2 = TempVal;
}

void hci_platform_debug_port_mask_enable(uint8_t bt_sel, uint32_t bt_dbg_mask)
{
	uint8_t i = 0;
	uint32_t mask = bt_dbg_mask;

	hci_platform_debug_port_pre_enable(bt_sel);
	for (i = 0; i < 8 && mask != 0; i++) {            /* pinmux, to PINMUX_FUNCTION_DBGPORT */
		if ((mask & ((uint32_t)0x1)) == 1) {
			hci_platform_bt_debug_bit(i, NULL);
			osif_delay(5);
		}
		mask >>= 1;
	}
}

void hci_platform_debug_port_pad_enable(uint8_t bt_sel, uint8_t bt_dbg_port, char *pad)
{
	hci_platform_debug_port_pre_enable(bt_sel);
	hci_platform_bt_debug_bit(bt_dbg_port, pad);       /* pinmux, to PINMUX_FUNCTION_DBGPORT */
	osif_delay(5);
}

void hci_platform_debug_port_shift(uint8_t original, uint8_t mapping)
{
	/* shift from original to mapping */
	hci_platform_bt_debug_shift_bit(original, mapping);
	osif_delay(5);
}

void hci_platform_gpio_enable(uint8_t bt_gpio, char *pad)
{
	AON_TypeDef *AON = (AON_TypeDef *)AON_BASE;
	AON->AON_SYS_CTRL &= ~AON_BIT_DEBUGER_EN;          /* 0x4022_0068[8] = 0, SWD disable */
	osif_delay(5);
	hci_platform_bt_gpio_pad(bt_gpio, pad);
	osif_delay(5);
}
