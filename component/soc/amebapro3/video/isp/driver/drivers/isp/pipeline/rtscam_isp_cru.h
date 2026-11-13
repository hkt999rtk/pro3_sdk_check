#ifndef _RTSCAM_ISP_CRU_H_INC_
#define _RTSCAM_ISP_CRU_H_INC_

#include <stdbool.h>
#include <clock.h>
#include <reset.h>

struct rtscam_isp_cru {
	struct {
		struct rtscam_rtos_clk *isp_clk;
		struct rtscam_rtos_clk *inf_clk;
		bool has_real_clk;
	};

	struct {
		struct rtscam_rtos_reset_control *sysmem;
		struct rtscam_rtos_reset_control *isp_reset;
		struct rtscam_rtos_reset_control *mipi_reset;
	};
};

int rtscam_isp_cru_init(struct rtscam_isp_cru *cru, bool has_real_clk);
void rtscam_isp_cru_cleanup(struct rtscam_isp_cru *cru);
void rtscam_isp_cru_clk_enable(struct rtscam_isp_cru *cru);
void rtscam_isp_cru_clk_disable(struct rtscam_isp_cru *cru);
int rtscam_isp_cru_clk_set_rate(struct rtscam_isp_cru *cru,
								struct rtscam_isp_clock *clk);
void rtscam_isp_cru_reset_device(struct rtscam_isp_cru *cru);
void rtscam_isp_cru_assert_device(struct rtscam_isp_cru *cru);
void rtscam_isp_cru_deassert_device(struct rtscam_isp_cru *cru);

#endif /* _RTSCAM_ISP_CRU_H_INC_ */
