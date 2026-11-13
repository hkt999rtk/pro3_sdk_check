#include <errno.h>
#include <rts_camera_isp.h>
#include "rtscam_isp_cru.h"

static int __rtscam_isp_cru_clk_init(struct rtscam_isp_cru *cru)
{
	cru->isp_clk = rtscam_rtos_clk_get("isp_clk");
	if (!cru->isp_clk) {
		return -EINVAL;
	}
	cru->inf_clk = rtscam_rtos_clk_get("inf_clk");
	if (!cru->inf_clk) {
		return -EINVAL;
	}

	return 0;
}

static int __rtscam_isp_cru_reset_init(struct rtscam_isp_cru *cru)
{
	cru->sysmem = rtscam_rtos_reset_control_get("isp-sysmem-up");
	if (!cru->sysmem) {
		return -EINVAL;
	}

	cru->isp_reset = rtscam_rtos_reset_control_get("isp_reset");
	if (!cru->isp_reset) {
		return -EINVAL;
	}
	cru->mipi_reset = rtscam_rtos_reset_control_get("mipi_reset");
	if (!cru->mipi_reset) {
		return -EINVAL;
	}
	return 0;
}

int rtscam_isp_cru_init(struct rtscam_isp_cru *cru, bool has_real_clk)
{
	int ret;

	if (!cru) {
		return -EINVAL;
	}

	cru->has_real_clk = has_real_clk;
	ret = __rtscam_isp_cru_clk_init(cru);
	if (ret) {
		return ret;
	}
	return __rtscam_isp_cru_reset_init(cru);
}

void rtscam_isp_cru_clk_enable(struct rtscam_isp_cru *cru)
{
	if (!cru) {
		return;
	}

	rtscam_rtos_clk_enable(cru->inf_clk);
	rtscam_rtos_clk_enable(cru->isp_clk);
}

void rtscam_isp_cru_clk_disable(struct rtscam_isp_cru *cru)
{
	if (!cru) {
		return;
	}

	rtscam_rtos_clk_disable(cru->inf_clk);
	rtscam_rtos_clk_disable(cru->isp_clk);
}

int rtscam_isp_cru_clk_set_rate(struct rtscam_isp_cru *cru,
								struct rtscam_isp_clock *clock)
{
	int ret;
	struct rtscam_rtos_clk *clk;

	if (!cru || !clock) {
		return -EINVAL;
	}

	switch (clock->type) {
	case RTSCAM_ISP_CLOCK_INTERFACE:
		clk = cru->inf_clk;
		break;
	case RTSCAM_ISP_CLOCK_ISP:
		clk = cru->isp_clk;
		break;
	default:
		return -EINVAL;
	}

	if (cru->has_real_clk) {
		ret = rtscam_rtos_clk_set_rate(
				  clk, rtscam_rtos_clk_round_rate(clk, clock->rate));
		if (ret) {
			return ret;
		}
	}
	clock->rate = rtscam_rtos_clk_get_rate(clk);

	return 0;
}

void rtscam_isp_cru_reset_device(struct rtscam_isp_cru *cru)
{
	if (!cru) {
		return;
	}

	rtscam_rtos_reset_control_reset(cru->isp_reset);
	rtscam_rtos_reset_control_reset(cru->mipi_reset);
}

void rtscam_isp_cru_assert_device(struct rtscam_isp_cru *cru)
{
	if (!cru) {
		return;
	}

	rtscam_rtos_reset_control_assert(cru->sysmem);
}

void rtscam_isp_cru_deassert_device(struct rtscam_isp_cru *cru)
{
	if (!cru) {
		return;
	}

	rtscam_rtos_reset_control_deassert(cru->sysmem);
}

void rtscam_isp_cru_cleanup(struct rtscam_isp_cru *cru)
{
	rtscam_rtos_clk_put(cru->inf_clk);
	rtscam_rtos_clk_put(cru->isp_clk);
	rtscam_rtos_reset_control_put(cru->isp_reset);
	rtscam_rtos_reset_control_put(cru->mipi_reset);
	rtscam_rtos_reset_control_put(cru->sysmem);
}
