#ifndef _ISP_STATIS_COMMON_H_INC_
#define _ISP_STATIS_COMMON_H_INC_

#include <rts_isp_define.h>
#include <rts_isp_ae.h>
#include <rts_isp_awb.h>
#include <rts_isp_af.h>
#include <rts_isp_other.h>

struct isp_statis_raw_buf_offset {
	uint16_t offset;
	uint16_t r_offset;
	uint16_t g_offset;
	uint16_t b_offset;
	uint16_t y_offset;
	uint16_t rg_offset;
	uint16_t bg_offset;
	uint16_t pixels_offset;
	uint16_t size;
};

struct isp_statis_awb_buf_offset {
	uint16_t offset;
	uint16_t r_offset;
	uint16_t g_offset;
	uint16_t b_offset;
	uint16_t y_offset;
	uint16_t rg_offset;
	uint16_t bg_offset;
	uint16_t pixels_offset;
	uint16_t size;
};

struct isp_statis_ae_hdr_buf_offset {
	uint16_t y_offset;
	uint16_t hist_offset;
};

struct isp_statis_ae_buf_offset {
	uint16_t offset;
	uint16_t y_offset;
	uint16_t hist_offset;
	struct isp_statis_ae_hdr_buf_offset hdr[RTS_ISP_HDR_CHAN_MAX];
	uint16_t size;
};

struct isp_statis_af_buf_offset {
	uint16_t offset;
	uint16_t illum_cnt_offset;
	uint16_t gr_filter_offset;
	uint16_t gb_filter_offset;
	uint16_t size;
};

struct isp_statis_y_buf_offset {
	uint16_t offset;
	uint16_t y_offset;
	uint16_t hist_offset;
	uint16_t size;
};

struct isp_statis_buf_offset {
	struct isp_statis_raw_buf_offset raw;
	struct isp_statis_awb_buf_offset awb;
	struct isp_statis_ae_buf_offset ae;
	struct isp_statis_af_buf_offset af;
	struct isp_statis_y_buf_offset y;
	struct isp_statis_af_buf_offset af1; /* short exp af */
	uint16_t size;
};

struct rts_isp_statis {
	const void *buf;
	struct rts_isp_raw_statis raw;
	struct rts_isp_awb_statis awb;
	struct rts_isp_ae_statis ae;
	struct rts_isp_af_statis af;
	struct rts_isp_flick_statis flick;
	struct rts_isp_y_statis y;
	struct rts_isp_af_statis af1; /* short exp af */
};

struct rts_isp_statis_awb_reg {
	uint32_t illum_white_pixels[6];
	uint32_t fine_r_sum;
	uint32_t fine_g_sum;
	uint32_t fine_b_sum;
	uint32_t fine_white_pixels;
};

struct rts_isp_statis_flick_reg {
	uint32_t fft_sum2_9;
	uint32_t fft_sum2_127;
	uint32_t valid;
};

struct rts_isp_statis_data {
	uint32_t buf_id;

	uint32_t frame_count;
	struct rts_isp_statis_awb_reg awb_reg;
	struct rts_isp_statis_flick_reg flick_reg;
	struct rts_isp_statis_flick_reg flick_s_reg;
};

#endif /* _ISP_STATIS_COMMON_H_INC_ */
