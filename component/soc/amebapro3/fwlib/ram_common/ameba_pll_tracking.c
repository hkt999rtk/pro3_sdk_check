/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/* Map table index -> Wi‑Fi channel */
static const u16 pll_table_channel_map[] = {
	1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,
	36,  40,  44,  48,  52,  56,  60,  64,  100, 104, 108, 112, 116, 120,
	124, 128, 132, 136, 140, 144, 149, 153, 157, 161, 165, 169, 173, 177
};

/* Convert user input channel number to PLL LUT index.
 * Returns -1 if the channel is not present in the table.
 */
static int WIFI_ChannelToTableIndex(u32 channel_num)
{
	for (u32 i = 0; i < (sizeof(pll_table_channel_map) / sizeof(pll_table_channel_map[0])); i++) {
		if (pll_table_channel_map[i] == channel_num) {
			return (int)i;
		}
	}
	return -1;
}

/* ================= SYS1 PLL ================= */

void PLLTrack_SYS1_LoadPLLTable(u32 channel_num, u32 divn, u32 f0n, u32 f0f)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	SYS1PLL_TRACK_DEV->ENTRY[idx].entry.divn_sdm = divn;
	SYS1PLL_TRACK_DEV->ENTRY[idx].entry.f0n_sdm = f0n;
	SYS1PLL_TRACK_DEV->ENTRY[idx].entry.f0f_sdm = f0f;
}

void PLLTrack_SYS1_EnablePLLTrackFeature(void)
{
	SYS1PLL_TRACK_DEV->CTRL0 |= PERIPLL_TRACKING_BIT_EN;
}

void PLLTrack_SYS1_SetTrigger(u32 channel_num)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	SYS1PLL_TRACK_DEV->ENTRY[idx].entry.trigger = 1;
}

void PLLTrack_SYS1_UnsetTrigger(u32 channel_num)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	SYS1PLL_TRACK_DEV->ENTRY[idx].entry.trigger = 0;
}

void PLLTrack_SYS1_EnableSSC(u32 channel_num)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	SYS1PLL_TRACK_DEV->ENTRY[idx].entry.en_ssc = 1;
}

void PLLTrack_SYS1_DisableSSC(u32 channel_num)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	SYS1PLL_TRACK_DEV->ENTRY[idx].entry.en_ssc = 0;
}

void PLLTrack_SYS1_SetCNT1(u32 num)
{
	SYS1PLL_TRACK_DEV->CTRL0 &= ~(PERIPLL_TRACKING_MASK_CNT1);
	SYS1PLL_TRACK_DEV->CTRL0 |= PERIPLL_TRACKING_CNT1(num);
}

void PLLTrack_SYS1_SetCNT2(u32 num)
{
	SYS1PLL_TRACK_DEV->CTRL1 &= ~(PERIPLL_TRACKING_MASK_CNT2);
	SYS1PLL_TRACK_DEV->CTRL1 |= PERIPLL_TRACKING_CNT2(num);
}

void PLLTrack_SYS1_SetCNT3(u32 num)
{
	SYS1PLL_TRACK_DEV->CTRL1 &= ~(PERIPLL_TRACKING_MASK_CNT3);
	SYS1PLL_TRACK_DEV->CTRL1 |= PERIPLL_TRACKING_CNT3(num);
}

void PLLTrack_SYS1_SetTBASE(u32 channel_num, u32 tbase)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	SYS1PLL_TRACK_DEV->ENTRY[idx].entry.tbase = tbase;
}

/* ================= SYS2 PLL ================= */

void PLLTrack_SYS2_LoadPLLTable(u32 channel_num, u32 divn, u32 f0n, u32 f0f)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	SYS2PLL_TRACK_DEV->ENTRY[idx].entry.divn_sdm = divn;
	SYS2PLL_TRACK_DEV->ENTRY[idx].entry.f0n_sdm = f0n;
	SYS2PLL_TRACK_DEV->ENTRY[idx].entry.f0f_sdm = f0f;
}

void PLLTrack_SYS2_EnablePLLTrackFeature(void)
{
	SYS2PLL_TRACK_DEV->CTRL0 |= PERIPLL_TRACKING_BIT_EN;
}

void PLLTrack_SYS2_SetTrigger(u32 channel_num)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	SYS2PLL_TRACK_DEV->ENTRY[idx].entry.trigger = 1;
}

void PLLTrack_SYS2_UnsetTrigger(u32 channel_num)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	SYS2PLL_TRACK_DEV->ENTRY[idx].entry.trigger = 0;
}

void PLLTrack_SYS2_EnableSSC(u32 channel_num)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	SYS2PLL_TRACK_DEV->ENTRY[idx].entry.en_ssc = 1;
}

void PLLTrack_SYS2_DisableSSC(u32 channel_num)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	SYS2PLL_TRACK_DEV->ENTRY[idx].entry.en_ssc = 0;
}

void PLLTrack_SYS2_SetCNT1(u32 num)
{
	SYS2PLL_TRACK_DEV->CTRL0 &= ~(PERIPLL_TRACKING_MASK_CNT1);
	SYS2PLL_TRACK_DEV->CTRL0 |= PERIPLL_TRACKING_CNT1(num);
}

void PLLTrack_SYS2_SetCNT2(u32 num)
{
	SYS2PLL_TRACK_DEV->CTRL1 &= ~(PERIPLL_TRACKING_MASK_CNT2);
	SYS2PLL_TRACK_DEV->CTRL1 |= PERIPLL_TRACKING_CNT2(num);
}

void PLLTrack_SYS2_SetCNT3(u32 num)
{
	SYS2PLL_TRACK_DEV->CTRL1 &= ~(PERIPLL_TRACKING_MASK_CNT3);
	SYS2PLL_TRACK_DEV->CTRL1 |= PERIPLL_TRACKING_CNT3(num);
}

void PLLTrack_SYS2_SetTBASE(u32 channel_num, u32 tbase)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	SYS2PLL_TRACK_DEV->ENTRY[idx].entry.tbase = tbase;
}

/* ================= PERI PLL ================= */

void PLLTrack_PERI_LoadPLLTable(u32 channel_num, u32 divn, u32 f0n, u32 f0f)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	PERIPLL_TRACK_DEV->ENTRY[idx].entry.divn_sdm = divn;
	PERIPLL_TRACK_DEV->ENTRY[idx].entry.f0n_sdm = f0n;
	PERIPLL_TRACK_DEV->ENTRY[idx].entry.f0f_sdm = f0f;
}

void PLLTrack_PERI_EnablePLLTrackFeature(void)
{
	PERIPLL_TRACK_DEV->CTRL0 |= PERIPLL_TRACKING_BIT_EN;
}

void PLLTrack_PERI_SetTrigger(u32 channel_num)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	PERIPLL_TRACK_DEV->ENTRY[idx].entry.trigger = 1;
}

void PLLTrack_PERI_UnsetTrigger(u32 channel_num)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	PERIPLL_TRACK_DEV->ENTRY[idx].entry.trigger = 0;
}

void PLLTrack_PERI_SetCNT1(u32 num)
{
	PERIPLL_TRACK_DEV->CTRL0 &= ~(PERIPLL_TRACKING_MASK_CNT1);
	PERIPLL_TRACK_DEV->CTRL0 |= PERIPLL_TRACKING_CNT1(num);
}

void PLLTrack_PERI_SetCNT2(u32 num)
{
	PERIPLL_TRACK_DEV->CTRL1 &= ~(PERIPLL_TRACKING_MASK_CNT2);
	PERIPLL_TRACK_DEV->CTRL1 |= PERIPLL_TRACKING_CNT2(num);
}

void PLLTrack_PERI_SetCNT3(u32 num)
{
	PERIPLL_TRACK_DEV->CTRL1 &= ~(PERIPLL_TRACKING_MASK_CNT3);
	PERIPLL_TRACK_DEV->CTRL1 |= PERIPLL_TRACKING_CNT3(num);
}

void PLLTrack_PERI_SetTBASE(u32 channel_num, u32 tbase)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	PERIPLL_TRACK_DEV->ENTRY[idx].entry.tbase = tbase;
}

void PLLTrack_PERI_EnableSSC(u32 channel_num)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	PERIPLL_TRACK_DEV->ENTRY[idx].entry.en_ssc = 1;
}

void PLLTrack_PERI_DisableSSC(u32 channel_num)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	PERIPLL_TRACK_DEV->ENTRY[idx].entry.en_ssc = 0;
}


/* ================= DDR PLL ================= */

void PLLTrack_DDR_LoadPLLTable(u32 channel_num, u32 n_code, u32 f_code, u32 n_code_t, u32 f_code_t)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	DDRPLL_TRACK_DEV->ENTRY[idx].entry.n_code = n_code;
	DDRPLL_TRACK_DEV->ENTRY[idx].entry.f_code = f_code;
	DDRPLL_TRACK_DEV->ENTRY[idx].entry.n_code_t = n_code_t;
	DDRPLL_TRACK_DEV->ENTRY[idx].entry.f_code_t = f_code_t;
}

void PLLTrack_DDR_EnablePLLTrackFeature(void)
{
	DDRPLL_TRACK_DEV->CTRL0     |= DDRPLL_TRACKING_BIT_EN;
}

void PLLTrack_DDR_DisablePLLTrackFeature(void)
{
	DDRPLL_TRACK_DEV->CTRL0     &= ~DDRPLL_TRACKING_BIT_EN;
}

void PLLTrack_DDR_LoadFromSystemRegs(void)
{
	CORE_S_BASE->DDR_CRT_CTRL1  |= BIT(0);
}

void PLLTrack_DDR_LoadFromCRT(void)
{
	CORE_S_BASE->DDR_CRT_CTRL1  &= ~BIT(0);
}

void PLLTrack_DDR_SetTrigger(u32 channel_num)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	DDRPLL_TRACK_DEV->ENTRY[idx].entry.trigger = 1;
}

void PLLTrack_DDR_UnsetTrigger(u32 channel_num)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	DDRPLL_TRACK_DEV->ENTRY[idx].entry.trigger = 0;
}

void PLLTrack_DDR_EnableSSC(u32 channel_num)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	DDRPLL_TRACK_DEV->ENTRY[idx].entry.en_ssc = 1;
}

void PLLTrack_DDR_DisableSSC(u32 channel_num)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	DDRPLL_TRACK_DEV->ENTRY[idx].entry.en_ssc = 0;
}

void PLLTrack_DDR_SetFlagInitial(u32 channel_num)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	DDRPLL_TRACK_DEV->ENTRY[idx].entry.flag_initial = 1;
}

void PLLTrack_DDR_UnsetFlagInitial(u32 channel_num)
{
	int idx = WIFI_ChannelToTableIndex(channel_num);
	if (idx < 0) {
		return;
	}

	DDRPLL_TRACK_DEV->ENTRY[idx].entry.flag_initial = 0;
}

void PLLTrack_DDR_SetOC_EN(void)
{
	CORE_S_BASE->DDR_CRT_CTRL1 |= BIT(2);
}

void PLLTrack_DDR_UnsetOC_EN(void)
{
	CORE_S_BASE->DDR_CRT_CTRL1 &= ~BIT(2);
}

void PLLTrack_DDR_SetCNT1(u32 num)
{
	DDRPLL_TRACK_DEV->CTRL0 &= ~(DDRPLL_TRACKING_MASK_CNT1);
	DDRPLL_TRACK_DEV->CTRL0 |= DDRPLL_TRACKING_CNT1(num);
}

void PLLTrack_DDR_SetCNT2(u32 num)
{
	DDRPLL_TRACK_DEV->CTRL1 &= ~(DDRPLL_TRACKING_MASK_CNT2);
	DDRPLL_TRACK_DEV->CTRL1 |= DDRPLL_TRACKING_CNT2(num);
}

void PLLTrack_DDR_SetCNT3(u32 num)
{
	DDRPLL_TRACK_DEV->CTRL1 &= ~(DDRPLL_TRACKING_MASK_CNT3);
	DDRPLL_TRACK_DEV->CTRL1 |= DDRPLL_TRACKING_CNT3(num);
}
