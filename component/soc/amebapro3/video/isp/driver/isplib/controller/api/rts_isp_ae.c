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
#include <isp_mod_ae.h>
#include <isp_mod_statis.h>

static void free_ae_setting(struct rts_isp_ae_setting *setting)
{
	isp_free(setting);
}

static struct rts_isp_ae_setting *
alloc_ae_setting(const struct isp_mod_ae_stat_info *info)
{
	int win_num;
	struct rts_isp_ae_setting *setting;

	if (!isp_calloc_item(setting)) {
		return NULL;
	}
	win_num = info->info.win.cols * info->info.win.rows;
	set_const_var(setting->_auto.window_num, win_num);

	return setting;
}

int rts_isp_query_ae_setting(uint32_t isp_id,
							 struct rts_isp_ae_setting **ae_setting)
{
	int ret;
	struct rts_isp_ae_setting *setting;
	struct isp_mod_ae_stat_info info;

	if (isp_id >= _ISP_ID_NUM || !ae_setting) {
		return -RTS_ISP_EINVAL;
	}

	ret = isp_client_message_simple(isp_id, MOD_AE, AE_INFO_STATIS, &info,
									sizeof(info));
	if (ret) {
		return ret;
	}
	if (info.info.win.cols * info.info.win.rows >
		ARRAY_SIZE(setting->_auto.win_weights)) {
		return -RTS_ISP_EINVAL;
	}
	setting = alloc_ae_setting(&info);
	if (!setting) {
		return -RTS_ISP_ENOMEM;
	}
	ret = rts_isp_get_ae_setting(isp_id, setting);
	if (ret) {
		goto err;
	}
	*ae_setting = setting;
	return RTS_ISP_OK;
err:
	isp_perror(ret, "query ae setting fail");
	free_ae_setting(setting);
	return ret;
}

int rts_isp_get_ae_setting(uint32_t isp_id,
						   struct rts_isp_ae_setting *ae_setting)
{
	return isp_client_message_simple(isp_id, MOD_AE, AE_EXEC_GET_SETTING,
									 ae_setting, sizeof(*ae_setting));
}

int rts_isp_set_ae_setting(uint32_t isp_id,
						   struct rts_isp_ae_setting *ae_setting)
{
	return isp_client_message_simple(isp_id, MOD_AE, AE_EXEC_SET_SETTING,
									 ae_setting, sizeof(*ae_setting));
}

void rts_isp_release_ae_setting(uint32_t isp_id,
								struct rts_isp_ae_setting *ae_setting)
{
	free_ae_setting(ae_setting);
}

struct rts_isp_ae_statis_ex {
	struct rts_isp_ae_statis statis;
	struct isp_mod_statis_ae_stat inner;
};

int rts_isp_get_ae_statis_info(uint32_t isp_id,
							   struct rts_isp_ae_stat_info *info)
{
	int ret;
	struct isp_mod_ae_stat_info stat_info;

	if (isp_id > _ISP_ID_NUM || !info) {
		return -RTS_ISP_EINVAL;
	}

	ret = isp_client_message_simple(isp_id, MOD_AE, AE_INFO_STATIS,
									&stat_info, sizeof(stat_info));
	if (ret) {
		return ret;
	}
	*info = stat_info.info;

	return RTS_ISP_OK;
}

int rts_isp_query_ae_statis(uint32_t isp_id,
							const struct rts_isp_ae_statis **statis)
{
	int i;
	int ret;
	void *statis_ptr;
	struct rts_isp_ae_statis_ex *ex;
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

	ex = isp_malloc(sizeof(*ex) + offset.ae.size);
	if (!ex) {
		return -RTS_ISP_ENOMEM;
	}
	isp_memset(ex, 0, sizeof(*ex));
	ex->inner.buffer_size = offset.ae.size;

	statis_ptr = ex + 1;
	for (i = 0; i < RTS_ISP_HDR_CHAN_MAX; i++) {
		ex->statis.hdr[i].hist = statis_ptr +
								 offset.ae.hdr[i].hist_offset -
								 offset.ae.offset;
		ex->statis.hdr[i].y_mean = statis_ptr +
								   offset.ae.hdr[i].y_offset -
								   offset.ae.offset;
	}
	ex->statis.hist = statis_ptr + offset.ae.hist_offset - offset.ae.offset;
	ex->statis.y_mean = statis_ptr + offset.ae.y_offset - offset.ae.offset;

	*statis = &ex->statis;

	return RTS_ISP_OK;
}

int rts_isp_get_ae_statis(uint32_t isp_id,
						  const struct rts_isp_ae_statis *statis)
{
	int ret;
	struct rts_isp_ae_statis_ex *ex;

	if (isp_id > _ISP_ID_NUM || !statis) {
		return -RTS_ISP_EINVAL;
	}

	ex = container_of(statis, struct rts_isp_ae_statis_ex, statis);

	ret = isp_client_message_raw(isp_id, MOD_STATIS, STATIS_INFO_AE_STAT,
								 &ex->inner, sizeof(ex->inner),
								 sizeof(ex->inner) + ex->inner.buffer_size);
	if (ret) {
		return ret;
	}
	set_const_var(statis->frame_count, ex->inner.frame_count);

	return RTS_ISP_OK;
}

int rts_isp_put_ae_statis(uint32_t isp_id,
						  const struct rts_isp_ae_statis *statis)
{
	if (isp_id > _ISP_ID_NUM || !statis) {
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

void rts_isp_release_ae_statis(uint32_t isp_id,
							   const struct rts_isp_ae_statis *statis)
{
	struct rts_isp_ae_statis_ex *ex;

	if (isp_id > _ISP_ID_NUM || !statis) {
		return;
	}

	ex = container_of(statis, struct rts_isp_ae_statis_ex, statis);
	isp_free(ex);
}

int rts_av_isp_get_ae_roi(uint32_t isp_id, struct rts_isp_ae_roi_areas *areas)
{
	return isp_client_message_simple(isp_id, MOD_AE, AE_EXEC_GET_ROI_AREAS,
									 areas, sizeof(*areas));
}

int rts_av_isp_set_ae_roi(uint32_t isp_id, struct rts_isp_ae_roi_areas *areas)
{
	return isp_client_message_simple(isp_id, MOD_AE, AE_EXEC_SET_ROI_AREAS,
									 areas, sizeof(*areas));
}
