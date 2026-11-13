/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2019 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <rts_isp_tuning.h>
#include <isp_os_log.h>
#include <isp_os_mem.h>
#include <isp_atomic.h>
#include <isp_mod.h>
#include <isp_client_message.h>
#include <isp_mod_statis.h>
#include <isp_mod_iq.h>

#define TUNING_STATIS_MAX_SIZE (32 * 1024)
#define TUNING_PARAM_MAX_SIZE 2048
#define TUNING_IQ_MAX_SIZE (128 * 1024)

struct rts_isp_tuning_statis_ex {
	struct rts_isp_tuning_statis statis;
	struct isp_mod_statis_tuning_stat inner;
};

struct rts_isp_tuning_param_ex {
	struct rts_isp_tuning_param param;
	struct isp_mod_iq_tuning_buffer inner;
};

struct rts_isp_tuning_iq_ex {
	struct rts_isp_tuning_iq iq;
	struct isp_mod_iq_tuning_buffer inner;
};

static int tuning_regs_serial(struct isp_msg_buf *msg,
							  struct rts_isp_tuning_reg *regs, uint32_t num)
{
	int ret;
	void *buf;
	void *end;
	struct isp_mod_iq_tuning_regs *buf_tuning_regs;

	if (!msg || !regs || !num) {
		return -RTS_ISP_EINVAL;
	}

	buf = msg->data;
	end = buf + isp_msg_data_len(&msg->hdr);

	ret = get_buf_ptr(buf_tuning_regs, buf, end);
	if (ret) {
		return ret;
	}
	buf_tuning_regs->num = num;
	ret = copy_from_src(buf, regs, sizeof(*regs) * num, end);
	if (ret) {
		return ret;
	}

	msg->hdr.msg_len = buf - msg->data + sizeof(msg->hdr);
	msg->hdr.capacity = msg->hdr.msg_len;

	return RTS_ISP_OK;
}

static int tuning_regs_deserial(struct isp_msg_buf *msg,
								struct rts_isp_tuning_reg *regs, uint32_t num)
{
	int ret;
	void *buf;
	void *end;
	struct isp_mod_iq_tuning_regs *buf_tuning_regs;

	if (!msg || !regs || !num) {
		return -RTS_ISP_EINVAL;
	}

	if (msg->hdr.ret_val) {
		return msg->hdr.ret_val;
	}

	if (msg->hdr.action == IQ_EXEC_WRITE_REGS) {
		if (isp_msg_data_len(&msg->hdr) != 0) {
			return -RTS_ISP_EINVAL;
		}
		return RTS_ISP_OK;
	}

	buf = msg->data;
	end = buf + isp_msg_data_len(&msg->hdr);

	ret = get_buf_ptr(buf_tuning_regs, buf, end);
	if (ret) {
		return ret;
	}
	if (buf_tuning_regs->num != num) {
		return -RTS_ISP_EINVAL;
	}

	return copy_to_src(regs, buf, sizeof(*regs) * num, end);
}

static int __process_tuning_regs(uint32_t isp_id, uint32_t action,
								 struct rts_isp_tuning_reg *regs, uint32_t num)
{
	int ret;
	size_t msg_len = 256 + sizeof(*regs) * num;
	struct isp_msg_buf *msg;

	if (isp_id >= _ISP_ID_NUM || !regs) {
		return -RTS_ISP_EINVAL;
	}

	msg = isp_msg_new(NULL, msg_len, 0);
	if (!msg) {
		return -RTS_ISP_ENOMEM;
	}

	ret = tuning_regs_serial(msg, regs, num);
	if (ret) {
		goto out;
	}

	msg->hdr.flags = ISP_MSG_TYPE_RAW;
	msg->hdr.isp_id = isp_id;
	msg->hdr.mod_id = MOD_IQ;
	msg->hdr.action = action;
	msg->hdr.ret_val = -1;

	ret = isp_client_message_process(msg);
	if (ret) {
		goto out;
	}
	ret = tuning_regs_deserial(msg, regs, num);
out:
	isp_release(msg, isp_msg_put);
	return ret;
}

int rts_isp_tuning_query_statis(uint32_t isp_id,
								struct rts_isp_tuning_statis **statis)
{
	struct rts_isp_tuning_statis_ex *ex;

	if (isp_id > _ISP_ID_NUM || !statis) {
		return -RTS_ISP_EINVAL;
	}

	ex = isp_malloc(sizeof(*ex) + TUNING_STATIS_MAX_SIZE);
	if (!ex) {
		return -RTS_ISP_ENOMEM;
	}
	isp_memset(ex, 0, sizeof(*ex));
	ex->statis.type = ex->inner.type = RTS_ISP_TUNING_STATIS_ALL;
	*(void **)&ex->statis.buf = ex + 1;
	set_const_var(ex->statis.len, 0);

	*statis = &ex->statis;

	return RTS_ISP_OK;
}

int rts_isp_tuning_get_statis(uint32_t isp_id,
							  struct rts_isp_tuning_statis *statis)
{
	int ret;
	struct rts_isp_tuning_statis_ex *ex;

	if (isp_id > _ISP_ID_NUM || !statis) {
		return -RTS_ISP_EINVAL;
	}

	ex = container_of(statis, struct rts_isp_tuning_statis_ex, statis);
	if (statis->buf != ex + 1) {
		return -RTS_ISP_EINVAL;
	}

	ex->inner.buffer_size = 0;
	ex->inner.type = statis->type;
	ret = isp_client_message_raw(
			  isp_id, MOD_STATIS, STATIS_INFO_TUNING_STAT, &ex->inner,
			  sizeof(ex->inner), sizeof(ex->inner) + TUNING_STATIS_MAX_SIZE);
	if (ret) {
		return ret;
	}
	set_const_var(ex->statis.len, ex->inner.buffer_size);

	return RTS_ISP_OK;
}

void rts_isp_tuning_release_statis(uint32_t isp_id,
								   struct rts_isp_tuning_statis *statis)
{
	struct rts_isp_tuning_statis_ex *ex;

	if (isp_id > _ISP_ID_NUM || !statis) {
		return;
	}

	ex = container_of(statis, struct rts_isp_tuning_statis_ex, statis);
	if (statis->buf != ex + 1) {
		return;
	}

	isp_free(ex);
}

int rts_isp_tuning_query_param(uint32_t isp_id,
							   struct rts_isp_tuning_param **param)
{
	struct rts_isp_tuning_param_ex *ex;

	if (isp_id > _ISP_ID_NUM || !param) {
		return -RTS_ISP_EINVAL;
	}

	ex = isp_malloc(sizeof(*ex) + TUNING_PARAM_MAX_SIZE);
	if (!ex) {
		return -RTS_ISP_ENOMEM;
	}
	isp_memset(ex, 0, sizeof(*ex));
	ex->param.type = ex->inner.type = RTS_ISP_TUNING_PARAM_ALL;
	*(void **)&ex->param.buf = ex + 1;
	ex->param.len = 0;
	set_const_var(ex->param.max_len, TUNING_PARAM_MAX_SIZE);

	*param = &ex->param;

	return RTS_ISP_OK;
}

int rts_isp_tuning_get_param(uint32_t isp_id,
							 struct rts_isp_tuning_param *param)
{
	int ret;
	struct rts_isp_tuning_param_ex *ex;

	if (isp_id > _ISP_ID_NUM || !param) {
		return -RTS_ISP_EINVAL;
	}

	ex = container_of(param, struct rts_isp_tuning_param_ex, param);
	if (param->buf != ex + 1 || param->max_len != TUNING_PARAM_MAX_SIZE) {
		return -RTS_ISP_EINVAL;
	}

	ex->inner.type = param->type;
	ex->inner.len = 0;
	ret = isp_client_message_raw(isp_id, MOD_IQ, IQ_EXEC_GET_ALGO_PARAM,
								 &ex->inner, sizeof(ex->inner),
								 sizeof(ex->inner) + TUNING_PARAM_MAX_SIZE);
	if (ret) {
		return ret;
	}
	param->len = ex->inner.len;

	return RTS_ISP_OK;
}

int rts_isp_tuning_set_param(uint32_t isp_id,
							 struct rts_isp_tuning_param *param)
{
	struct rts_isp_tuning_param_ex *ex;

	if (isp_id > _ISP_ID_NUM || !param || param->len > param->max_len) {
		return -RTS_ISP_EINVAL;
	}

	ex = container_of(param, struct rts_isp_tuning_param_ex, param);
	if (param->buf != ex + 1 || param->max_len != TUNING_PARAM_MAX_SIZE) {
		return -RTS_ISP_EINVAL;
	}

	ex->inner.type = param->type;
	ex->inner.len = param->len;
	return isp_client_message_raw(isp_id, MOD_IQ, IQ_EXEC_SET_ALGO_PARAM,
								  &ex->inner,
								  sizeof(ex->inner) + param->len,
								  sizeof(ex->inner) + param->len);
}

void rts_isp_tuning_release_param(uint32_t isp_id,
								  struct rts_isp_tuning_param *param)
{
	struct rts_isp_tuning_param_ex *ex;

	if (isp_id > _ISP_ID_NUM || !param) {
		return;
	}

	ex = container_of(param, struct rts_isp_tuning_param_ex, param);
	if (param->buf != ex + 1 || param->max_len != TUNING_PARAM_MAX_SIZE) {
		return;
	}

	isp_free(ex);
}

int rts_isp_tuning_query_iq(uint32_t isp_id, struct rts_isp_tuning_iq **iq)
{
	struct rts_isp_tuning_iq_ex *ex;

	if (isp_id > _ISP_ID_NUM || !iq) {
		return -RTS_ISP_EINVAL;
	}

	ex = isp_malloc(sizeof(*ex) + TUNING_IQ_MAX_SIZE);
	if (!ex) {
		return -RTS_ISP_ENOMEM;
	}
	isp_memset(ex, 0, sizeof(*ex));
	ex->iq.type = ex->inner.type = RTS_ISP_TUNING_IQ_TABLE_ALL;
	*(void **)&ex->iq.buf = ex + 1;
	ex->iq.len = 0;
	set_const_var(ex->iq.max_len, TUNING_IQ_MAX_SIZE);

	*iq = &ex->iq;

	return RTS_ISP_OK;
}

int rts_isp_tuning_get_iq(uint32_t isp_id, struct rts_isp_tuning_iq *iq)
{
	int ret;
	struct rts_isp_tuning_iq_ex *ex;

	if (isp_id > _ISP_ID_NUM || !iq) {
		return -RTS_ISP_EINVAL;
	}

	ex = container_of(iq, struct rts_isp_tuning_iq_ex, iq);
	if (iq->buf != ex + 1 || iq->max_len != TUNING_IQ_MAX_SIZE) {
		return -RTS_ISP_EINVAL;
	}

	ex->inner.type = iq->type;
	ex->inner.len = 0;
	ret = isp_client_message_raw(isp_id, MOD_IQ, IQ_EXEC_GET_IQ_TABLE,
								 &ex->inner, sizeof(ex->inner),
								 sizeof(ex->inner) + TUNING_IQ_MAX_SIZE);
	if (ret) {
		return ret;
	}
	iq->len = ex->inner.len;

	return RTS_ISP_OK;
}

int rts_isp_tuning_set_iq(uint32_t isp_id, struct rts_isp_tuning_iq *iq)
{
	struct rts_isp_tuning_iq_ex *ex;

	if (isp_id > _ISP_ID_NUM || !iq || iq->len > iq->max_len) {
		return -RTS_ISP_EINVAL;
	}

	ex = container_of(iq, struct rts_isp_tuning_iq_ex, iq);
	if (iq->buf != ex + 1 || iq->max_len != TUNING_IQ_MAX_SIZE) {
		return -RTS_ISP_EINVAL;
	}

	ex->inner.type = iq->type;
	ex->inner.len = iq->len;
	return isp_client_message_raw(isp_id, MOD_IQ, IQ_EXEC_SET_IQ_TABLE,
								  &ex->inner, sizeof(ex->inner) + iq->len,
								  sizeof(ex->inner) + iq->len);
}

void rts_isp_tuning_release_iq(uint32_t isp_id, struct rts_isp_tuning_iq *iq)
{
	struct rts_isp_tuning_iq_ex *ex;

	if (isp_id > _ISP_ID_NUM || !iq) {
		return;
	}

	ex = container_of(iq, struct rts_isp_tuning_iq_ex, iq);
	if (iq->buf != ex + 1 || iq->max_len != TUNING_IQ_MAX_SIZE) {
		return;
	}

	isp_free(ex);
}

int rts_isp_tuning_read_regs(uint32_t isp_id, struct rts_isp_tuning_reg *regs,
							 uint32_t num)
{
	if (isp_id > _ISP_ID_NUM || !regs || !num) {
		return -RTS_ISP_EINVAL;
	}

	return __process_tuning_regs(isp_id, IQ_EXEC_READ_REGS, regs, num);
}

int rts_isp_tuning_write_regs(uint32_t isp_id, struct rts_isp_tuning_reg *regs,
							  uint32_t num)
{
	if (isp_id > _ISP_ID_NUM || !regs || !num) {
		return -RTS_ISP_EINVAL;
	}

	return __process_tuning_regs(isp_id, IQ_EXEC_WRITE_REGS, regs, num);
}

const char *rts_isp_tuning_packed_iq_path(uint32_t isp_id)
{
	return NULL;
}

int rts_isp_tuning_reload_packed_iq(uint32_t isp_id)
{
	if (isp_id > _ISP_ID_NUM) {
		return -RTS_ISP_EINVAL;
	}

	return isp_client_message_simple(isp_id, MOD_IQ,
									 IQ_EXEC_RELOAD_PACKED_IQ, NULL, 0);
}

int rts_isp_tuning_get_packed_iq_info(
	uint32_t isp_id, struct rts_isp_tuning_packed_iq_info *info)
{
	int ret;
	struct isp_mod_iq_packed_iq_info mod_iq_info;

	if (isp_id > _ISP_ID_NUM || !info) {
		return -RTS_ISP_EINVAL;
	}

	ret = isp_client_message_simple(isp_id, MOD_IQ, IQ_INFO_PACKED_IQ,
									&mod_iq_info, sizeof(mod_iq_info));
	if (ret) {
		return ret;
	}

	info->version = mod_iq_info.version;
	info->cus_version = mod_iq_info.cus_version;
	info->timestamp = mod_iq_info.timestamp;

	return RTS_ISP_OK;
}
