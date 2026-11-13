#include <errno.h>
#include <string.h>
#include <isp_rtos_iomem.h>
#include "clock.h"
#include "hal_isp.h"
#include "isp_os_log.h"
#include "ameba_soc.h"

enum rtscam_rtos_clk_id {
	ISP_CLK,
	INF_CLK,
	SSOR_HCLK
};

struct rtscam_rtos_clk {
	enum rtscam_rtos_clk_id id;
	void *clk;
	unsigned long rate;
};

static struct rtscam_rtos_clk clk[3];

#define ISP_SYS 35
#define MIPI_SYS 36
#define ISP_CLK_SIZE 6
#define MIPI_CLK_SIZE 8

/* Fixed by ISP_Init(): ISP SCLK = SYS2_PLL / 2, MIPI = SYS1_PLL / 1. */
#define ISP_4K_DEV_ISP_CLK_RATE  250000000UL
#define ISP_4K_DEV_MIPI_CLK_RATE 600000000UL

/*
 * Ported from the removed others/lib_isp/drv/clk/clk-rts3915.c
 * (SHA-1 594e57fd22ac8953973b0e4af869e4be7741680e), which set ISP/MIPI
 * clock dynamically by sensor setting instead of a fixed rate.
 */
static void hal_video_sys_set_clk(uint8_t id, uint8_t sel_val)
{
	switch (id) {
	case ISP_SYS:
		if (sel_val >= ISP_CLK_SIZE) {
			isp_debug("ISP clock out range: %d\r\n", sel_val);
			return;
		}
		switch (sel_val) {
		case 0: /* ISP SCLK 240M */
			RCC_PeriphClockDividerSet(SYS_ISP_SCLK, 2, 1);
			RCC_PeriphClockSourceSet(SYS_ISP_SCLK, SYS2_PLL);
			RCC_PeriphClockDividerSet(SYS_ISP_BUSCLK, 2, 1);
			RCC_PeriphClockSourceSet(SYS_ISP_BUSCLK, SYS2_PLL);
			break;
		case 1: /* ISP SCLK 200M */
			RCC_PeriphClockDividerSet(SYS_ISP_SCLK, 3, 1);
			RCC_PeriphClockSourceSet(SYS_ISP_SCLK, SYS1_PLL);
			RCC_PeriphClockDividerSet(SYS_ISP_BUSCLK, 3, 1);
			RCC_PeriphClockSourceSet(SYS_ISP_BUSCLK, SYS1_PLL);
			break;
		case 2: /* ISP SCLK 166M */
			RCC_PeriphClockDividerSet(SYS_ISP_SCLK, 3, 1);
			RCC_PeriphClockSourceSet(SYS_ISP_SCLK, SYS2_PLL);
			RCC_PeriphClockDividerSet(SYS_ISP_BUSCLK, 3, 1);
			RCC_PeriphClockSourceSet(SYS_ISP_BUSCLK, SYS2_PLL);
			break;
		case 3: /* ISP SCLK 125M */
			RCC_PeriphClockDividerSet(SYS_ISP_SCLK, 4, 1);
			RCC_PeriphClockSourceSet(SYS_ISP_SCLK, SYS2_PLL);
			RCC_PeriphClockDividerSet(SYS_ISP_BUSCLK, 4, 1);
			RCC_PeriphClockSourceSet(SYS_ISP_BUSCLK, SYS2_PLL);
			break;
		case 4: /* ISP SCLK 100M */
			RCC_PeriphClockDividerSet(SYS_ISP_SCLK, 5, 1);
			RCC_PeriphClockSourceSet(SYS_ISP_SCLK, SYS2_PLL);
			RCC_PeriphClockDividerSet(SYS_ISP_BUSCLK, 5, 1);
			RCC_PeriphClockSourceSet(SYS_ISP_BUSCLK, SYS2_PLL);
			break;
		case 5: /* ISP SCLK 83M */
			RCC_PeriphClockDividerSet(SYS_ISP_SCLK, 6, 1);
			RCC_PeriphClockSourceSet(SYS_ISP_SCLK, SYS2_PLL);
			RCC_PeriphClockDividerSet(SYS_ISP_BUSCLK, 6, 1);
			RCC_PeriphClockSourceSet(SYS_ISP_BUSCLK, SYS2_PLL);
			break;
		}
		break;
	case MIPI_SYS:
		if (sel_val >= MIPI_CLK_SIZE) {
			isp_debug("MIPI clock out range: %d\r\n", sel_val);
			return;
		}
		switch (sel_val) {
		case 0: /* ISP MIPI CLK 600M */
			RCC_PeriphClockDividerSet(SYS_ISP_MIPICLK, 0, 0);
			RCC_PeriphClockSourceSet(SYS_ISP_MIPICLK, SYS1_PLL);
			break;
		case 1: /* ISP MIPI CLK 300M */
			RCC_PeriphClockDividerSet(SYS_ISP_MIPICLK, 1, 0);
			RCC_PeriphClockSourceSet(SYS_ISP_MIPICLK, SYS1_PLL);
			break;
		case 2: /* ISP MIPI CLK 250M */
			RCC_PeriphClockDividerSet(SYS_ISP_MIPICLK, 1, 0);
			RCC_PeriphClockSourceSet(SYS_ISP_MIPICLK, SYS2_PLL);
			break;
		case 3: /* ISP MIPI CLK 200M */
			RCC_PeriphClockDividerSet(SYS_ISP_MIPICLK, 2, 0);
			RCC_PeriphClockSourceSet(SYS_ISP_MIPICLK, SYS1_PLL);
			break;
		case 4: /* ISP MIPI CLK 166M */
			RCC_PeriphClockDividerSet(SYS_ISP_MIPICLK, 2, 0);
			RCC_PeriphClockSourceSet(SYS_ISP_MIPICLK, SYS2_PLL);
			break;
		case 5: /* ISP MIPI CLK 125M */
			RCC_PeriphClockDividerSet(SYS_ISP_MIPICLK, 3, 0);
			RCC_PeriphClockSourceSet(SYS_ISP_MIPICLK, SYS2_PLL);
			break;
		case 6: /* ISP MIPI CLK 100M */
			RCC_PeriphClockDividerSet(SYS_ISP_MIPICLK, 4, 0);
			RCC_PeriphClockSourceSet(SYS_ISP_MIPICLK, SYS2_PLL);
			break;
		case 7: /* ISP MIPI CLK 83M */
			RCC_PeriphClockDividerSet(SYS_ISP_MIPICLK, 5, 0);
			RCC_PeriphClockSourceSet(SYS_ISP_MIPICLK, SYS2_PLL);
			break;
		}
		break;
	default:
		break;
	}
}

static u32 isp_clock_level = 0;
static u32 mipi_clock_level = 0;

int hal_video_set_clock_level(u32 isp_clk_level, u32 mipi_clk_level)
{
	isp_clock_level = isp_clk_level;
	mipi_clock_level = mipi_clk_level;

	return 0;
}

static long rlx_round_rate_isp(unsigned long rate)
{
	static const unsigned long isp_rates[ISP_CLK_SIZE] = {
		83333333, 100000000, 125000000, 166666666, 200000000, 240000000
	};
	int i;

	for (i = 0; i < ISP_CLK_SIZE; i++) {
		if (rate <= isp_rates[i]) {
			break;
		}
	}
	i += isp_clock_level;
	if (i >= ISP_CLK_SIZE) {
		i = ISP_CLK_SIZE - 1;
	}
	return isp_rates[i];
}

static int rlx_set_rate_isp(unsigned long rate)
{
	static const unsigned long isp_rates[ISP_CLK_SIZE] = {
		240000000, 200000000, 166666666, 125000000, 100000000, 83333333
	};
	int i;

	for (i = 0; i < ISP_CLK_SIZE; i++) {
		if (rate == isp_rates[i]) {
			hal_video_sys_set_clk(ISP_SYS, i);
			return 0;
		}
	}
	isp_debug("rlx_set_rate_isp %u not supported\r\n", (unsigned int)rate);
	return -EINVAL;
}

static long rlx_round_rate_mipi(unsigned long rate)
{
	static const unsigned long mipi_rates[MIPI_CLK_SIZE] = {
		83333333, 100000000, 125000000, 166666666, 200000000, 250000000,
		300000000, 600000000
	};
	int i;

	for (i = 0; i < MIPI_CLK_SIZE; i++) {
		if (rate <= mipi_rates[i]) {
			break;
		}
	}
	i += mipi_clock_level;
	if (i >= MIPI_CLK_SIZE) {
		i = MIPI_CLK_SIZE - 1;
	}
	return mipi_rates[i];
}

static int rlx_set_rate_mipi(unsigned long rate)
{
	static const unsigned long mipi_rates[MIPI_CLK_SIZE] = {
		600000000, 300000000, 250000000, 200000000, 166666666, 125000000,
		100000000, 83333333
	};
	int i;

	for (i = 0; i < MIPI_CLK_SIZE; i++) {
		if (rate == mipi_rates[i]) {
			hal_video_sys_set_clk(MIPI_SYS, i);
			return 0;
		}
	}
	isp_debug("rlx_set_rate_mipi %u not supported\r\n", (unsigned int)rate);
	return -EINVAL;
}

void sensor_clk_enable(void *clk)
{
	isp_debug(">>> IN %s line:%d\r\n", __func__, __LINE__);
	//TODO
	return;
}

void sensor_clk_disable(void *clk)
{
	isp_debug(">>> IN %s line:%d\r\n", __func__, __LINE__);
	//TODO
	return;
}

void sensor_hclk_set_rate(unsigned long rate)
{
	isp_debug(">>> IN %s line:%d\r\n", __func__, __LINE__);
	//TODO
	return;
}

int isp_clk_init(void)
{
	isp_debug(">>> IN %s line:%d\r\n", __func__, __LINE__);
	clk[0].id = ISP_CLK;
	clk[0].clk = clk_get("isp_ck");
	clk[0].rate = 240000000;

	clk[1].id = INF_CLK;
	clk[1].clk = clk_get("inf_ck");
	clk[1].rate = 600000000;

	clk[2].id = SSOR_HCLK;
	clk[2].clk = clk_get("hclk");
	clk[2].rate = 200000000;

	return 0;
}

rtscam_rtos_clk_t rtscam_rtos_clk_get(const char *id)
{
	isp_debug(">>> IN %s line:%d\r\n", __func__, __LINE__);
	if (!strcmp("isp_clk", id)) {
		isp_debug(">>> return clk0:%x\r\n", &clk[0]);
		return &clk[0];
	}
	if (!strcmp("inf_clk", id)) {
		isp_debug(">>> return clk1:%x\r\n", &clk[1]);
		return &clk[1];
	}
	if (!strcmp("hclk", id)) {
		isp_debug(">>> return clk2:%x\r\n", &clk[2]);
		return &clk[2];
	}
	return NULL;
}

void rtscam_rtos_clk_put(rtscam_rtos_clk_t clk)
{
	(void)clk;

	return;
}

int rtscam_rtos_clk_enable(rtscam_rtos_clk_t clk)
{
	struct rtscam_rtos_clk *rtos_clk = clk;

	sensor_clk_enable(rtos_clk->clk);

	return 0;
}

int rtscam_rtos_clk_disable(rtscam_rtos_clk_t clk)
{
	struct rtscam_rtos_clk *rtos_clk = clk;

	sensor_clk_disable(rtos_clk->clk);
	return 0;
}

int rtscam_rtos_clk_set_rate(rtscam_rtos_clk_t clk, unsigned long rate)
{
	struct rtscam_rtos_clk *rtos_clk = clk;
	long round;
	int ret;

	if (!rtos_clk) {
		return -EINVAL;
	}
	if (ISP_Get4KMode()) {
		/* 4K DEV keeps the clocks fixed by ISP_Init() and reports the applied
		 * rate, not the requested one. Callers must compare the returned rate
		 * with their requirement: isp_mod_global.c and isp_mod_interface.c fail
		 * with -RTS_ISP_ERANGE if it is lower, so an unmet rate fails loudly.
		 * Do not return an error here: start always requests a rate, and the
		 * fixed rates cover the 4K sensor mode.
		 */
		if (rtos_clk->id == ISP_CLK) {
			rtos_clk->rate = ISP_4K_DEV_ISP_CLK_RATE;
			return 0;
		}
		if (rtos_clk->id == INF_CLK) {
			rtos_clk->rate = ISP_4K_DEV_MIPI_CLK_RATE;
			return 0;
		}
	}
	switch (rtos_clk->id) {
	case ISP_CLK:
		round = rlx_round_rate_isp(rate);
		ret = rlx_set_rate_isp(round);
		if (ret) {
			return ret;
		}
		rtos_clk->rate = round;
		isp_debug("isp clock change to %u\r\n", (unsigned int)round);
		break;
	case INF_CLK:
		round = rlx_round_rate_mipi(rate);
		ret = rlx_set_rate_mipi(round);
		if (ret) {
			return ret;
		}
		rtos_clk->rate = round;
		isp_debug("mipi clock change to %u\r\n", (unsigned int)round);
		break;
	case SSOR_HCLK:
		sensor_hclk_set_rate(rate);
		rtos_clk->rate = rate;
		break;
	default:
		break;
	}
	return 0;
}

unsigned long rtscam_rtos_clk_get_rate(rtscam_rtos_clk_t clk)
{
	struct rtscam_rtos_clk *rtos_clk = clk;

	if (!rtos_clk) {
		return -EINVAL;
	}
	return rtos_clk->rate;
}

long rtscam_rtos_clk_round_rate(rtscam_rtos_clk_t clk, unsigned long rate)
{
	struct rtscam_rtos_clk *rtos_clk = clk;

	if (!rtos_clk) {
		return rate;
	}
	switch (rtos_clk->id) {
	case ISP_CLK:
		return rlx_round_rate_isp(rate);
	case INF_CLK:
		return rlx_round_rate_mipi(rate);
	default:
		return rate;
	}
}
