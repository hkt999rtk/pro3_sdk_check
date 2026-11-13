/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "diag.h"
#include "rts_errno.h"
#include "rts_isp.h"
#include "rts_video.h"
#include "rts_raw.h"

#define RTS_RAW_INITIALIZED 0x32

static int s_current_raw_src = RTS_RAW_SRC_DISABLE;

int rts_hal_get_snr_fmt(uint32_t *w, uint32_t *h, uint32_t *fmt)
{
	int ret;
	struct rts_isp_image_size size;

	if (!w || !h || !fmt) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}

	ret = rts_isp_get_image_size(ISP0, &size);
	if (ret < 0) {
		DiagPrintf("[%s] get image size fail\r\n", __func__);
		return ret;
	}

	*w = size.isp_middle_size.w;
	*h = size.isp_middle_size.h;
	*fmt = size.sensor_bit_depth;

	return RTS_OK;
}

int rts_hal_set_raw(uint8_t streamid, uint32_t fmt)
{
	int ret;
	struct isp_v4l2_control ctrl;
	int location = -1;
	int position = -1;

	(void)streamid;

	switch (fmt) {
	case RTS_RAW_SRC_DISABLE:
		location = 0;
		break;
	case RTS_RAW_SRC_S_CROP_A:
		location = 4;
		break;
	case RTS_RAW_SRC_L_CROP_A:
		location = 3;
		break;
	case RTS_RAW_SRC_S_BLC_B:
		position = rts_isp_get_blc_position(ISP0);
		if (position == BLC_BEFORE_RNR) {
			location = 4;
		} else if (position == BLC_AFTER_RNR) {
			location = 6;
		}
		break;
	case RTS_RAW_SRC_L_BLC_B:
		position = rts_isp_get_blc_position(ISP0);
		if (position == BLC_BEFORE_RNR) {
			location = 3;
		} else if (position == BLC_AFTER_RNR) {
			location = 7;
		}
		break;
	case RTS_RAW_SRC_HDR_A:
		location = 9;
		break;
	case RTS_RAW_SRC_CCM_B:
		location = 16;
		break;
	case RTS_RAW_SRC_UVT_B:
		location = 17;
		break;
	case RTS_RAW_SRC_GTM_A:
		location = 13;
		break;
	case RTS_RAW_SRC_LTM_A:
		location = 14;
		break;
	case RTS_RAW_SRC_L_FUSION_B:
		location = 7;
		break;
	case RTS_RAW_SRC_S_FUSION_B:
		location = 6;
		break;
	case RTS_RAW_SRC_3DNR_B:
		location = 5;
		break;
	case RTS_RAW_SRC_FUSION_B:
		location = 8;
		break;
	case RTS_RAW_SRC_AEGAIN_A:
		location = 12;
		break;
	case RTS_RAW_SRC_AWBGAIN_B:
		location = 11;
		break;
	case RTS_RAW_SRC_WDR_A:
		location = 16;
		break;
	case RTS_RAW_SRC_YUV444TO422_B:
		location = 17;
		break;
	case RTS_RAW_SRC_LCE_A:
		location = 18;
		break;
	case RTS_RAW_SRC_EEH_A:
		location = 19;
		break;
	case RTS_RAW_SRC_MASK_A:
		location = 21;
		break;
	default:
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}

	memset(&ctrl, 0, sizeof(ctrl));
	ctrl.id = ISP_V4L2_CID_OUTPUT_POSITION;
	ctrl.value = location;

	ret = rts_isp_v4l2_s_ctrl(ISP0, &ctrl);
	if (ret) {
		DiagPrintf("[%s] set output position fail\r\n", __func__);
		return ret;
	}

	s_current_raw_src = (int)fmt;

	return RTS_OK;
}

int rts_hal_set_bypass(uint8_t streamid, uint32_t fmt)
{
	int ret;
	struct rts_isp_bypass bypass;
	int position = -1;

	(void)streamid;

	memset(&bypass, 0, sizeof(bypass));

	switch (fmt) {
	case RTS_RAW_SRC_DISABLE:
	case RTS_RAW_SRC_S_CROP_A:
	case RTS_RAW_SRC_L_CROP_A:
	case RTS_RAW_SRC_HDR_A:
	case RTS_RAW_SRC_GTM_A:
	case RTS_RAW_SRC_LTM_A:
	case RTS_RAW_SRC_L_FUSION_B:
	case RTS_RAW_SRC_S_FUSION_B:
	case RTS_RAW_SRC_FUSION_B:
	case RTS_RAW_SRC_AEGAIN_A:
	case RTS_RAW_SRC_WDR_A:
	case RTS_RAW_SRC_YUV444TO422_B:
	case RTS_RAW_SRC_EEH_A:
	case RTS_RAW_SRC_LCE_A:
	case RTS_RAW_SRC_MASK_A:
		break;
	case RTS_RAW_SRC_3DNR_B:
		bypass.bypass_rnr_0 = 1;
		break;
	case RTS_RAW_SRC_S_BLC_B:
		position = rts_isp_get_blc_position(ISP0);
		if (position == BLC_AFTER_RNR) {
			bypass.bypass_blc_1 = 1;
			bypass.bypass_ae_1 = 1;
		}
		break;
	case RTS_RAW_SRC_L_BLC_B:
		position = rts_isp_get_blc_position(ISP0);
		if (position == BLC_AFTER_RNR) {
			bypass.bypass_blc_0 = 1;
			bypass.bypass_ae_0 = 1;
		}
		break;
	case RTS_RAW_SRC_CCM_B:
		bypass.bypass_ccm = 1;
		bypass.bypass_gamma = 1;
		bypass.bypass_wdr = 1;
		break;
	case RTS_RAW_SRC_UVT_B:
		bypass.bypass_uvtune = 1;
		break;
	case RTS_RAW_SRC_AWBGAIN_B:
		bypass.bypass_awb = 1;
		break;
	default:
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}

	ret = rts_isp_set_bypass(ISP0, &bypass);
	if (ret) {
		DiagPrintf("[%s] set bypass fail\r\n", __func__);
		return ret;
	}

	return RTS_OK;
}

int rts_hal_pre_streamon(void)
{
	return RTS_OK;
}

int rts_hal_post_streamoff(void)
{
	return RTS_OK;
}

int rts_raw_get_current_src(void)
{
	return s_current_raw_src;
}

/*
 * --------------------------------------------------------------------------
 * Raw capture format descriptor — see the block comment in rts_raw.h.
 * Ported from Linux rtsraw's set_raw_fmt()/make_raw_header(); this port
 * owns no video-channel/buffer/streaming/2A state.
 * --------------------------------------------------------------------------
 */

static void set_raw_fmt(struct rts_raw_t *praw, unsigned int raw_fmt)
{
	praw->raw_fmt = raw_fmt;

	switch (raw_fmt) {
	case RTS_RAW_SRC_DISABLE:
	case RTS_RAW_SRC_EEH_A:
	case RTS_RAW_SRC_LCE_A:
	case RTS_RAW_SRC_MASK_A:
		praw->snr_fmt = RTS_RAW_FMT_RAW16;
		praw->eff_bits = 16;
		praw->data_pattern = RTS_DATA_PATTERN_YUV422;
		praw->data_type = RTS_DATA_TYPE_YUV_INTERLEAVED;
		praw->length = praw->width * praw->height * 2;
		break;
	case RTS_RAW_SRC_S_CROP_A:
	case RTS_RAW_SRC_L_CROP_A:
	case RTS_RAW_SRC_S_BLC_B:
	case RTS_RAW_SRC_L_BLC_B:
	case RTS_RAW_SRC_S_FUSION_B:
	case RTS_RAW_SRC_L_FUSION_B:
	case RTS_RAW_SRC_3DNR_B:
		praw->snr_fmt = RTS_RAW_FMT_RAW16;
		praw->eff_bits = 12;
		praw->data_pattern = RTS_DATA_PATTERN_RAW_GRBG;
		praw->data_type = RTS_DATA_TYPE_UNKNOWN;
		praw->length = praw->width * praw->height * 2;
		break;
	case RTS_RAW_SRC_GTM_A:
		praw->snr_fmt = RTS_RAW_FMT_RAW16;
		praw->eff_bits = 14;
		praw->data_pattern = RTS_DATA_PATTERN_RAW_GRBG;
		praw->data_type = RTS_DATA_TYPE_UNKNOWN;
		praw->length = praw->width * praw->height * 2;
		break;
	case RTS_RAW_SRC_LTM_A:
		praw->snr_fmt = RTS_RAW_FMT_RAW16;
		praw->eff_bits = 12;
		praw->data_pattern = RTS_DATA_PATTERN_RAW_GRBG;
		praw->data_type = RTS_DATA_TYPE_UNKNOWN;
		praw->length = praw->width * praw->height * 2;
		break;
	case RTS_RAW_SRC_FUSION_B:
		praw->snr_fmt = RTS_RAW_FMT_RAW32;
		praw->eff_bits = 32;
		praw->data_pattern = RTS_DATA_PATTERN_HDR;
		praw->data_type = RTS_DATA_TYPE_HDR_INTERLEAVED;
		praw->length = praw->width * praw->height * 4;
		break;
	case RTS_RAW_SRC_CCM_B:
	case RTS_RAW_SRC_WDR_A:
		praw->snr_fmt = RTS_RAW_FMT_RAW128;
		praw->eff_bits = 108;
		praw->data_pattern = RTS_DATA_PATTERN_RGB;
		praw->data_type = RTS_DATA_TYPE_UNKNOWN;
		praw->length = (praw->width * praw->height + 2) / 3 * 128 / 8;
		break;
	case RTS_RAW_SRC_UVT_B:
	case RTS_RAW_SRC_YUV444TO422_B:
		praw->snr_fmt = RTS_RAW_FMT_RAW128;
		praw->eff_bits = 108;
		praw->data_pattern = RTS_DATA_PATTERN_YUV444;
		praw->data_type = RTS_DATA_TYPE_UNKNOWN;
		praw->length = (praw->width * praw->height + 2) / 3 * 128 / 8;
		break;
	case RTS_RAW_SRC_HDR_A:
	case RTS_RAW_SRC_AEGAIN_A:
	case RTS_RAW_SRC_AWBGAIN_B:
		praw->snr_fmt = RTS_RAW_FMT_RAW32;
		praw->eff_bits = 32;
		praw->data_pattern = RTS_DATA_PATTERN_RAW_GRBG;
		praw->data_type = RTS_DATA_TYPE_UNKNOWN;
		praw->length = praw->width * praw->height * 4;
		break;
	default:
		DiagPrintf("[%s] unsupported raw location %d\r\n", __func__, raw_fmt);
	}
}

static void put_u32_le(uint8_t *buf, uint32_t v)
{
	buf[0] = v & 0xff;
	buf[1] = (v >> 8) & 0xff;
	buf[2] = (v >> 16) & 0xff;
	buf[3] = (v >> 24) & 0xff;
}

static void make_raw_header(struct rts_raw_t *praw)
{
	const uint32_t fmt = 0x32595559;
	const uint32_t h_ver = 0x4;

	memset(praw->header, 0, sizeof(praw->header));

	put_u32_le(&praw->header[0], fmt);
	put_u32_le(&praw->header[4], praw->width);
	put_u32_le(&praw->header[8], praw->height);
	put_u32_le(&praw->header[12], praw->length);
	put_u32_le(&praw->header[16], h_ver);
	put_u32_le(&praw->header[20], RTS_RAW_HEADER_LENGTH);
	put_u32_le(&praw->header[24], praw->data_pattern);
	put_u32_le(&praw->header[28], praw->data_type);
	put_u32_le(&praw->header[32], praw->file_fmt);
	put_u32_le(&praw->header[36], praw->snr_fmt);
	put_u32_le(&praw->header[40], praw->eff_bits);
}

int rts_init_raw(unsigned int raw_fmt, unsigned int streamid,
				 struct rts_raw_t **ppraw)
{
	struct rts_raw_t *praw = NULL;
	int ret;

	if (!ppraw) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}

	if (raw_fmt >= RTS_RAW_SRC_RESERVED) {
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}

	praw = (struct rts_raw_t *)calloc(1, sizeof(*praw));
	if (!praw) {
		return RTS_RETURN(RTS_E_NO_MEMORY);
	}

	ret = rts_hal_get_snr_fmt(&praw->width, &praw->height, &praw->snr_fmt);
	if (ret < 0) {
		DiagPrintf("[%s] get sensor fmt fail\r\n", __func__);
		free(praw);
		return ret;
	}

	praw->streamid = streamid;
	praw->fmt = V4L2_PIX_FMT_128BIT_3PIXEL;
	praw->file_fmt = RTS_FILE_BIN;

	set_raw_fmt(praw, raw_fmt);
	make_raw_header(praw);

	praw->initialized = RTS_RAW_INITIALIZED;

	*ppraw = praw;
	return RTS_OK;
}

void rts_release_raw(struct rts_raw_t *praw)
{
	free(praw);
}

int rts_config_raw(struct rts_raw_t *praw)
{
	int ret;

	if (!praw) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}

	if (praw->initialized != RTS_RAW_INITIALIZED) {
		return RTS_RETURN(RTS_E_INITIALIZED);
	}

	ret = rts_hal_set_raw(praw->streamid, praw->raw_fmt);
	if (ret < 0) {
		DiagPrintf("[%s] set raw fmt fail\r\n", __func__);
		return ret;
	}

	ret = rts_hal_set_bypass(praw->streamid, praw->raw_fmt);
	if (ret < 0) {
		DiagPrintf("[%s] set bypass fail\r\n", __func__);
		rts_hal_set_raw(praw->streamid, RTS_RAW_SRC_DISABLE);
		return ret;
	}

	return RTS_OK;
}

int rts_disable_raw(struct rts_raw_t *praw)
{
	int ret;

	if (!praw) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}

	if (praw->initialized != RTS_RAW_INITIALIZED) {
		return RTS_RETURN(RTS_E_INITIALIZED);
	}

	ret = rts_hal_set_raw(praw->streamid, RTS_RAW_SRC_DISABLE);
	if (ret < 0) {
		DiagPrintf("[%s] disable raw fmt fail\r\n", __func__);
	}

	ret = rts_hal_set_bypass(praw->streamid, RTS_RAW_SRC_DISABLE);
	if (ret < 0) {
		DiagPrintf("[%s] disable bypass fail\r\n", __func__);
	}

	return ret;
}

int rts_get_raw_header(struct rts_raw_t *praw)
{
	if (!praw) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}

	if (praw->initialized != RTS_RAW_INITIALIZED) {
		return RTS_RETURN(RTS_E_INITIALIZED);
	}

	make_raw_header(praw);

	return RTS_OK;
}

int rts_update_raw_fmt(unsigned int raw_fmt, struct rts_raw_t *praw)
{
	if (!praw) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}

	if (raw_fmt >= RTS_RAW_SRC_RESERVED) {
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}

	set_raw_fmt(praw, raw_fmt);

	return RTS_OK;
}
