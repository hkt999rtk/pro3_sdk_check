#ifndef _ISP_MOD_STATIS_H_INC_
#define _ISP_MOD_STATIS_H_INC_

#include <isp_utils_common.h>
#include <isp_statis_common.h>

/* for STATIS_INFO_AE_STAT */
struct isp_mod_statis_ae_stat {
	uint32_t buffer_size;
	uint32_t frame_count;
	/* statis buffer behind here */
};

/* for STATIS_INFO_AWB_STAT */
struct isp_mod_statis_awb_stat {
	uint32_t buffer_size;
	uint32_t frame_count;
	uint32_t illum_white_pixels[6];
	uint32_t fine_r_sum;
	uint32_t fine_g_sum;
	uint32_t fine_b_sum;
	uint32_t fine_white_pixels;
	/* statis buffer behind here */
};

/* for STATIS_INFO_AF_STAT */
struct isp_mod_statis_af_stat {
	uint32_t buffer_size;
	uint32_t frame_count;
	/* statis buffer behind here */
};

/* for STATIS_INFO_TUNING_STAT */
struct isp_mod_statis_tuning_stat {
	uint32_t buffer_size;
	uint32_t type;
	/* statis buffer behind here */
};

enum isp_mod_statis_get_info_type {
	STATIS_INFO_BUF_OFFSET =
		isp_action_r(INFO_TYPE, 0, struct isp_statis_buf_offset),
	STATIS_INFO_AE_STAT = isp_action_w(INFO_TYPE, 1, void *),
	STATIS_INFO_AWB_STAT = isp_action_w(INFO_TYPE, 2, void *),
	STATIS_INFO_AF_STAT = isp_action_w(INFO_TYPE, 3, void *),
	STATIS_INFO_TUNING_STAT = isp_action_w(INFO_TYPE, 4, void *),
};

#endif /* _ISP_MOD_STATIS_H_INC_ */
