/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2019 Grant Shen <grant_shen@realsil.com.cn>
 * Copyright (C) 2019 Sherry Cheng <sherry_cheng@realsil.com.cn>
 */

#include <rts_isp.h>
#include <isp_os_log.h>
#include <isp_os_mem.h>
#include <isp_atomic.h>
#include <isp_mod.h>
#include <isp_client_message.h>
#include <isp_mod_af.h>
#include <isp_mod_statis.h>

struct rts_isp_af_statis_ex {
	struct rts_isp_af_statis statis;
	struct isp_mod_statis_af_stat inner;
};

int rts_isp_get_af_statis_info(uint32_t isp_id,
							   struct rts_isp_af_stat_info *info)
{
	int ret;
	struct isp_mod_af_stat_info stat_info;

	if (isp_id > _ISP_ID_NUM || !info) {
		return -RTS_ISP_EINVAL;
	}

	ret = isp_client_message_simple(isp_id, MOD_AF, AF_INFO_STATIS,
									&stat_info, sizeof(stat_info));
	if (ret) {
		return ret;
	}
	*info = stat_info.info;

	return RTS_ISP_OK;
}

int rts_isp_query_af_statis(uint32_t isp_id,
							const struct rts_isp_af_statis **statis)
{
	int ret;
	int win_num;
	uint32_t af_size;
	void *statis_ptr;
	struct rts_isp_af_statis_ex *ex;
	struct isp_statis_buf_offset offset;
	struct isp_mod_af_stat_info stat_info;

	if (isp_id > _ISP_ID_NUM || !statis) {
		return -RTS_ISP_EINVAL;
	}

	ret = isp_client_message_simple(isp_id, MOD_STATIS,
									STATIS_INFO_BUF_OFFSET, &offset,
									sizeof(offset));
	if (ret) {
		return ret;
	}
	ret = isp_client_message_simple(isp_id, MOD_AF, AF_INFO_STATIS,
									&stat_info, sizeof(stat_info));
	if (ret) {
		return ret;
	}

	win_num = stat_info.info.win.rows * stat_info.info.win.cols;
	af_size = offset.af.size + win_num * sizeof(*(**statis).y_mean);
	ex = isp_malloc(sizeof(*ex) + af_size);
	if (!ex) {
		return -RTS_ISP_ENOMEM;
	}
	isp_memset(ex, 0, sizeof(*ex));
	ex->inner.buffer_size = af_size;

	statis_ptr = ex + 1;
	ex->statis.high_illum_cnt =
		statis_ptr + offset.af.illum_cnt_offset - offset.af.offset;
	ex->statis.gr_focus_value =
		statis_ptr + offset.af.gr_filter_offset - offset.af.offset;
	ex->statis.gb_focus_value =
		statis_ptr + offset.af.gb_filter_offset - offset.af.offset;
	ex->statis.y_mean = statis_ptr + offset.af.size;

	*statis = &ex->statis;

	return RTS_ISP_OK;
}

int rts_isp_get_af_statis(uint32_t isp_id,
						  const struct rts_isp_af_statis *statis)
{
	int ret;
	struct rts_isp_af_statis_ex *ex;

	if (isp_id > _ISP_ID_NUM || !statis) {
		return -RTS_ISP_EINVAL;
	}

	ex = container_of(statis, struct rts_isp_af_statis_ex, statis);

	ret = isp_client_message_raw(isp_id, MOD_STATIS, STATIS_INFO_AF_STAT,
								 &ex->inner, sizeof(ex->inner),
								 sizeof(ex->inner) + ex->inner.buffer_size);
	if (ret) {
		return ret;
	}
	set_const_var(statis->frame_count, ex->inner.frame_count);

	return RTS_ISP_OK;
}

int rts_isp_put_af_statis(uint32_t isp_id,
						  const struct rts_isp_af_statis *statis)
{
	if (isp_id > _ISP_ID_NUM || !statis) {
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

void rts_isp_release_af_statis(uint32_t isp_id,
							   const struct rts_isp_af_statis *statis)
{
	struct rts_isp_af_statis_ex *ex;

	if (isp_id > _ISP_ID_NUM || !statis) {
		return;
	}

	ex = container_of(statis, struct rts_isp_af_statis_ex, statis);
	isp_free(ex);
}
