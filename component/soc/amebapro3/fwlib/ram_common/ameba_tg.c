#include "ameba_soc.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "ameba_ddrc.h"
#include "ameba_tg.h"

static const char *TAG = "Traffic Gen";

#define TG_DATA_BUS_WIDTH  16
#define TG_WAIT_TIMEOUT    10000000

static int  tg_is_write;
static u32  tg_fixed_pattern[4];
static int  tg_has_pattern;

/**
 * @brief  Initialize TG profile 0 and route the selected DDRC port.
 * @param  port: DDRC port number (4, 5, or 6).
 * @param  write: 1 for write traffic, 0 for read traffic.
 * @param  addr: Base address for traffic (e.g. 0x80000000).
 * @param  size: Address range size in bytes.
 * @param  burst_length: AXI burst length (number of beats per burst).
 * @param  pattern: Pointer to 4x u32 (128-bit write data). NULL = random.
 */
void TG_Init(int port, int write, u32 addr, u32 size, u32 burst_length, u32 *pattern)
{
	TRAFFIC_GEN_TypeDef *tg = TG_DEV;

	u32 bytes_per_burst = burst_length * TG_DATA_BUS_WIDTH;

	assert(port >= 4 && port <= 6);
	assert(burst_length > 0);
	assert(size >= bytes_per_burst);
	assert((size % bytes_per_burst) == 0);
	assert((addr % TG_DATA_BUS_WIDTH) == 0);

	tg_is_write = write;
	tg_has_pattern = (pattern != NULL);
	if (tg_has_pattern) {
		memcpy(tg_fixed_pattern, pattern, sizeof(tg_fixed_pattern));
	}

	tg->TG_SEED_AX                           = rand();
	tg->TG_SEED_WD                           = rand();
	tg->TG_SEED_INIT                         = 1;

	tg->TG_PROF[0].TG_PROF_RDWR              = write;
	tg->TG_PROF[0].TG_PROF_STLVL             = 0;
	tg->TG_PROF[0].TG_PROF_FULL              = 4096;
	tg->TG_PROF[0].TG_PROF_TXNLMT            = write ? 1 : 5;
	tg->TG_PROF[0].TG_PROF_RATE_C            = 1;
	tg->TG_PROF[0].TG_PROF_RATE_S            = 16;
	tg->TG_PROF[0].TG_PROF_AXLEN             = burst_length - 1;
	tg->TG_PROF[0].TG_PROF_FMTIME            = 0;
	tg->TG_PROF[0].TG_PROF_FMSIZE            = size / TG_DATA_BUS_WIDTH;
	tg->TG_PROF[0].TG_PROF_BADR              = addr;
	tg->TG_PROF[0].TG_PROF_YADR_ADRMSK       = addr + size - 1;
	tg->TG_PROF[0].TG_PROF_WBV               = 1;
	tg->TG_PROF[0].TG_PROF_IDM               = 1;
	tg->TG_PROF[0].TG_PROF_ID_MAXID          = ~0;

	HAL_WRITE32(RXI300_CORE_REG_BASE, 0x30c, (port == 4) ? 0x0 : (port == 5) ? 0x1 : 0x2);
}

/**
 * @brief  Throttle TG profile 0 to a target bandwidth.
 *         TG pumps RATE_S bytes every RATE_C DDRC cycles, so
 *             bytes/sec = RATE_S / RATE_C * ddrc_clk
 *         Solving for RATE_C with RATE_S fixed at the 128-bit bus width:
 *             RATE_C = ddrc_clk * RATE_S / target_bps
 *         DDRC clock = DDRPLL / 2.
 * @param  target_bps: Target bandwidth in bytes/sec. 0 restores unthrottled (RATE_C=1).
 */
void TG_SetBandwidth(u32 target_bps)
{
	TRAFFIC_GEN_TypeDef *tg = TG_DEV;
	const u32 rate_s = TG_DATA_BUS_WIDTH;  /* 16 bytes per issuance (128-bit beat) */

	if (target_bps == 0) {
		tg->TG_PROF[0].TG_PROF_RATE_C = 1;
		tg->TG_PROF[0].TG_PROF_RATE_S = rate_s;
		return;
	}

	/* DDRC clock = DDR PLL / 2; reuse the same PLL decode as the BW monitor */
	u32 ssc2   = DDRPHY_DEV->SSC2;
	u32 ssc3   = DDRPHY_DEV->SSC3;
	u32 n_code = (ssc3 >> 8) & 0xFF;
	u32 f_code = (ssc2 >> 16) & 0x7FF;
	u64 ddr_freq_hz  = 40000000ULL * (n_code + 3) + (40000000ULL / 2048) * f_code;
	u64 ddrc_clk_hz  = ddr_freq_hz / 2;

	u64 rate_c = (ddrc_clk_hz * rate_s + target_bps / 2) / target_bps;
	if (rate_c == 0) {
		rate_c = 1;
	}

	tg->TG_PROF[0].TG_PROF_RATE_C = (u32)rate_c;
	tg->TG_PROF[0].TG_PROF_RATE_S = rate_s;

	u64 effective_bps = (ddrc_clk_hz * rate_s) / rate_c;
	RTK_LOGI(TAG, "TG target %u MB/s @ DDRC %u MHz -> RATE_C=%u RATE_S=%u (effective %u MB/s)\n",
			 (u32)(target_bps / 1000000), (u32)(ddrc_clk_hz / 1000000),
			 (u32)rate_c, rate_s, (u32)(effective_bps / 1000000));
}

/**
 * @brief  Override profile 0 to time-frame mode (unlimited size, fixed duration).
 * @param  time_cycles: Frame time in DDRC clock cycles.
 */
void TG_SetTimeMode(u32 time_cycles)
{
	TRAFFIC_GEN_TypeDef *tg = TG_DEV;

	tg->TG_PROF[0].TG_PROF_FMSIZE = 0;
	tg->TG_PROF[0].TG_PROF_FMTIME = time_cycles;
}

/**
 * @brief  Clear interrupt and start traffic generation on profile 0.
 */
void TG_Start(void)
{
	TRAFFIC_GEN_TypeDef *tg = TG_DEV;

	/* Refresh write data on every start: fixed pattern or new random */
	if (tg_is_write) {
		tg->TG_PROF[0].TG_PROF_WD_MAXWD = tg_has_pattern ? tg_fixed_pattern[0] : (u32)rand();
		tg->TG_PROF[0].TG_PROF_WD_MAXWD = tg_has_pattern ? tg_fixed_pattern[1] : (u32)rand();
		tg->TG_PROF[0].TG_PROF_WD_MAXWD = tg_has_pattern ? tg_fixed_pattern[2] : (u32)rand();
		tg->TG_PROF[0].TG_PROF_WD_MAXWD = tg_has_pattern ? tg_fixed_pattern[3] : (u32)rand();
	}

	tg->TG_INTR       = 0;
	tg->TG_PROF_START  = 0x1;

	RTK_LOGD(TAG, "TG started.\n");
}

/**
 * @brief  Stop traffic generation.
 */
void TG_Stop(void)
{
	TRAFFIC_GEN_TypeDef *tg = TG_DEV;

	tg->TG_PROF_STOP = 0x1;
	RTK_LOGD(TAG, "TG stopped.\n");
}

/**
 * @brief  Check if TG has completed (interrupt asserted).
 * @return 1 if done, 0 if still running.
 */
int TG_IsDone(void)
{
	TRAFFIC_GEN_TypeDef *tg = TG_DEV;

	return (tg->TG_INTR & 0x1) ? 1 : 0;
}

/**
 * @brief  Poll for TG completion with timeout.
 * @return 0 on success, -1 on timeout.
 */
int TG_WaitDone(void)
{
	u32 timeout = TG_WAIT_TIMEOUT;

	while (!TG_IsDone()) {
		if (--timeout == 0) {
			RTK_LOGE(TAG, "TG timeout waiting for completion!\n");
			return -1;
		}
	}
	RTK_LOGD(TAG, "TG completed.\n");
	return 0;
}

/**
 * @brief  Dump DDRC PMU AW_TICK / AR_TICK counters for ports 4-6.
 */
void TG_DumpPMU(void)
{
	DDRC_TOP_TypeDef *ddrc_top = DDRC_TOP_DEV;

	for (int port = 4; port <= 6; port++) {
		int idx = (port - 4) * 2;
		RTK_LOGI(TAG, "Port %d - AW_TICK: %u\n", port, ddrc_top->PMU_CNT_VALUE_j_0_to_6[idx]);
		RTK_LOGI(TAG, "Port %d - AR_TICK: %u\n", port, ddrc_top->PMU_CNT_VALUE_j_0_to_6[idx + 1]);
	}
}

/**
 * @brief  Dump all TG profile 0 register values.
 */
void TG_DumpProfile(void)
{
	TRAFFIC_GEN_TypeDef *tg = TG_DEV;
	volatile TG_profile_register_t *p = &tg->TG_PROF[0];

	RTK_LOGI(TAG, "TG Profile Register Dump:\n");
	RTK_LOGI(TAG, "  RDWR         = 0x%x (%s)\n",   p->TG_PROF_RDWR, p->TG_PROF_RDWR ? "Write" : "Read");
	RTK_LOGI(TAG, "  STLVL        = 0x%x\n",         p->TG_PROF_STLVL);
	RTK_LOGI(TAG, "  FULL         = 0x%x\n",         p->TG_PROF_FULL);
	RTK_LOGI(TAG, "  TXNLMT       = 0x%x\n",         p->TG_PROF_TXNLMT);
	RTK_LOGI(TAG, "  RATE_C       = 0x%x\n",         p->TG_PROF_RATE_C);
	RTK_LOGI(TAG, "  RATE_S       = 0x%x\n",         p->TG_PROF_RATE_S);
	RTK_LOGI(TAG, "  AXLEN        = 0x%x (burst=%d)\n", p->TG_PROF_AXLEN, p->TG_PROF_AXLEN + 1);
	RTK_LOGI(TAG, "  FMTIME       = 0x%x\n",         p->TG_PROF_FMTIME);
	RTK_LOGI(TAG, "  FMSIZE       = 0x%x\n",         p->TG_PROF_FMSIZE);
	RTK_LOGI(TAG, "  BADR         = 0x%x\n",         p->TG_PROF_BADR);
	RTK_LOGI(TAG, "  YADR_ADRMSK  = 0x%x\n",         p->TG_PROF_YADR_ADRMSK);
	RTK_LOGI(TAG, "  ADRM         = 0x%x\n",         p->TG_PROF_ADRM);
	RTK_LOGI(TAG, "  IDM          = 0x%x\n",         p->TG_PROF_IDM);
	RTK_LOGI(TAG, "  WDM          = 0x%x\n",         p->TG_PROF_WDM);
	RTK_LOGI(TAG, "  WBV          = 0x%x\n",         p->TG_PROF_WBV);
	RTK_LOGI(TAG, "  INTR         = 0x%x\n",         tg->TG_INTR);
}
