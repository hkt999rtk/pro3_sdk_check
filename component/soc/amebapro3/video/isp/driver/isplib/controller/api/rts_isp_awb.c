/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2019 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <rts_isp.h>
#include <isp_os_log.h>
#include <isp_os_mem.h>
#include <isp_atomic.h>
#include <isp_mod.h>
#include <isp_client_message.h>
#include <isp_mod_awb.h>
#include <isp_mod_statis.h>

int rts_isp_query_awb_setting(uint32_t isp_id,
							  struct rts_isp_awb_setting **awb_setting)
{
	int ret;
	struct rts_isp_awb_setting *setting;

	if (isp_id >= _ISP_ID_NUM || !awb_setting) {
		return -RTS_ISP_EINVAL;
	}

	if (!isp_calloc_item(setting)) {
		return -RTS_ISP_ENOMEM;
	}
	ret = rts_isp_get_awb_setting(isp_id, setting);
	if (ret) {
		isp_free(setting);
		return ret;
	}
	*awb_setting = setting;
	return RTS_ISP_OK;
}

int rts_isp_get_awb_setting(uint32_t isp_id,
							struct rts_isp_awb_setting *awb_setting)
{
	return isp_client_message_simple(isp_id, MOD_AWB, AWB_EXEC_GET_SETTING,
									 awb_setting, sizeof(*awb_setting));
}

int rts_isp_set_awb_setting(uint32_t isp_id,
							struct rts_isp_awb_setting *awb_setting)
{
	return isp_client_message_simple(isp_id, MOD_AWB, AWB_EXEC_SET_SETTING,
									 awb_setting, sizeof(*awb_setting));
}

void rts_isp_release_awb_setting(uint32_t isp_id,
								 struct rts_isp_awb_setting *awb_setting)
{
	isp_free(awb_setting);
}

struct rts_isp_awb_statis_ex {
	struct rts_isp_awb_statis statis;
	struct isp_mod_statis_awb_stat inner;
};

int rts_isp_get_awb_statis_info(uint32_t isp_id,
								struct rts_isp_awb_stat_info *info)
{
	int ret;
	struct isp_mod_awb_stat_info stat_info;

	if (isp_id > _ISP_ID_NUM || !info) {
		return -RTS_ISP_EINVAL;
	}

	ret = isp_client_message_simple(isp_id, MOD_AWB, AWB_INFO_STATIS,
									&stat_info, sizeof(stat_info));
	if (ret) {
		return ret;
	}
	*info = stat_info.info;

	return RTS_ISP_OK;
}

int rts_isp_query_awb_statis(uint32_t isp_id,
							 const struct rts_isp_awb_statis **statis)
{
	int ret;
	void *statis_ptr;
	struct rts_isp_awb_statis_ex *ex;
	struct isp_statis_buf_offset offset;

	if (isp_id > _ISP_ID_NUM || !statis) {
		return -RTS_ISP_EINVAL;
	}

	ret = isp_client_message_simple(isp_id, MOD_STATIS,
									STATIS_INFO_BUF_OFFSET, &offset,
									sizeof(offset));
	if (ret) {
		return ret;
	}

	ex = isp_malloc(sizeof(*ex) + offset.awb.size);
	if (!ex) {
		return -RTS_ISP_ENOMEM;
	}
	isp_memset(ex, 0, sizeof(*ex));
	ex->inner.buffer_size = offset.awb.size;

	statis_ptr = ex + 1;
	ex->statis.r_mean =
		statis_ptr + offset.awb.r_offset - offset.awb.offset;
	ex->statis.g_mean =
		statis_ptr + offset.awb.g_offset - offset.awb.offset;
	ex->statis.b_mean =
		statis_ptr + offset.awb.b_offset - offset.awb.offset;
	ex->statis.y_mean =
		statis_ptr + offset.awb.y_offset - offset.awb.offset;
	ex->statis.rg = statis_ptr + offset.awb.rg_offset - offset.awb.offset;
	ex->statis.bg = statis_ptr + offset.awb.bg_offset - offset.awb.offset;
	ex->statis.pixels =
		statis_ptr + offset.awb.pixels_offset - offset.awb.offset;

	*statis = &ex->statis;

	return RTS_ISP_OK;
}

int rts_isp_get_awb_statis(uint32_t isp_id,
						   const struct rts_isp_awb_statis *statis)
{
	int ret;
	struct rts_isp_awb_statis_ex *ex;

	if (isp_id > _ISP_ID_NUM || !statis) {
		return -RTS_ISP_EINVAL;
	}

	ex = container_of(statis, struct rts_isp_awb_statis_ex, statis);

	ret = isp_client_message_raw(isp_id, MOD_STATIS, STATIS_INFO_AWB_STAT,
								 &ex->inner, sizeof(ex->inner),
								 sizeof(ex->inner) + ex->inner.buffer_size);
	if (ret) {
		return ret;
	}

	set_const_var(statis->frame_count, ex->inner.frame_count);
	isp_memcpy((void *)statis->illum_white_pixels,
			   ex->inner.illum_white_pixels,
			   sizeof(statis->illum_white_pixels));
	set_const_var(statis->fine_r_sum, ex->inner.fine_r_sum);
	set_const_var(statis->fine_g_sum, ex->inner.fine_g_sum);
	set_const_var(statis->fine_b_sum, ex->inner.fine_b_sum);
	set_const_var(statis->fine_white_pixels, ex->inner.fine_white_pixels);

	return RTS_ISP_OK;
}

int rts_isp_put_awb_statis(uint32_t isp_id,
						   const struct rts_isp_awb_statis *statis)
{
	if (isp_id > _ISP_ID_NUM || !statis) {
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

void rts_isp_release_awb_statis(uint32_t isp_id,
								const struct rts_isp_awb_statis *statis)
{
	struct rts_isp_awb_statis_ex *ex;

	if (isp_id > _ISP_ID_NUM || !statis) {
		return;
	}

	ex = container_of(statis, struct rts_isp_awb_statis_ex, statis);
	isp_free(ex);
}
