#include "ameba_soc.h"

static const u16 ddr_pll_channel_map[] = {
	1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,
	36,  40,  44,  48,  52,  56,  60,  64,  100, 104, 108, 112, 116, /*120,*/
	124, 128, 132, 136, 140, 144, 149, 153, 157, 161, 165, 169, 173, 177
};

void DDR_PLLTrackFeature(void)
{
	u8 channel_count, i, channel;

	// DDRPLL
	PLLTrack_DDR_SetCNT1(1);
	PLLTrack_DDR_SetCNT2(20);
	PLLTrack_DDR_SetCNT3(400);

	// other channel => Freq = 933MHz
	channel_count = sizeof(ddr_pll_channel_map) / sizeof(ddr_pll_channel_map[0]);
	for (i = 0; i < channel_count; i++) {
		channel = ddr_pll_channel_map[i];
		//printf("PLL channel: %d\n\r", channel);
		PLLTrack_DDR_LoadPLLTable(channel, 0x14, 0x029A, 0x14, 0x029A);  //other channel, Freq = 933MHz
		PLLTrack_DDR_SetTrigger(channel);
		PLLTrack_DDR_SetFlagInitial(channel);
	}
	// ch120 => Freq = 930MHz
	PLLTrack_DDR_LoadPLLTable(120, 0x14, 0x0200, 0x14, 0x0200);  //CH120, Freq = 930MHz
	PLLTrack_DDR_SetTrigger(120);
	PLLTrack_DDR_SetFlagInitial(120);

	PLLTrack_DDR_EnablePLLTrackFeature();
	PLLTrack_DDR_LoadFromSystemRegs();
}

__attribute__((constructor))
void pll_track_init(void)
{
#if defined(CONFIG_WLAN) && (CONFIG_WLAN==1)
	DDR_PLLTrackFeature();
	RTK_LOGI("INIT", "PLL track init\n");
#endif
}
