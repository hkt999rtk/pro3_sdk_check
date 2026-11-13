/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2020 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <rts_isp_patch.h>
#include <isp_top.h>
#include <isp_mod.h>
#include <isp_mod_ccm.h>
#include <isp_mod_iq.h>
#include <isp_mod_blc.h>
#include <isp_mod_gamma.h>
#include <isp_mod_ygc.h>
#include <isp_mod_ygamma.h>

static int rts_isp_patch_apply_iq(uint32_t isp_id, uint32_t id)
{
	return isp_mod_execute(isp_top_get_mod(isp_id, MOD_IQ),
						   IQ_EXEC_APPLY_IQ, &id, sizeof(id));
}

static int rts_isp_patch_set_mod_patch_mode(uint32_t isp_id, uint32_t mod_id,
		uint32_t exec_type, int32_t enable)
{
	int ret;

	ret = isp_mod_execute(isp_top_get_mod(isp_id, mod_id), exec_type,
						  &enable, sizeof(enable));
	if (ret) {
		return ret;
	}
	if (!enable) {
		return rts_isp_patch_apply_iq(isp_id, mod_id);
	}

	return RTS_ISP_OK;
}

int rts_isp_patch_set_ccm_patch_mode(uint32_t isp_id, int32_t enable)
{
	return rts_isp_patch_set_mod_patch_mode(
			   isp_id, MOD_CCM, CCM_EXEC_SET_PATCH_MODE, enable);
}

int rts_isp_patch_set_ccm_v1_dyn(uint32_t isp_id,
								 isp_iq_ccm_v1_dyn_item_t *item)
{
	return isp_mod_execute(isp_top_get_mod(isp_id, MOD_CCM),
						   CCM_EXEC_SET_V1_DYN, &item, sizeof(item));
}

int rts_isp_patch_set_blc_patch_mode(uint32_t isp_id, int32_t enable)
{
	return rts_isp_patch_set_mod_patch_mode(
			   isp_id, MOD_BLC, BLC_EXEC_SET_PATCH_MODE, enable);
}

int rts_isp_patch_set_blc_v1_dyn(uint32_t isp_id,
								 isp_iq_blc_v1_dyn_item_t *item)
{
	return isp_mod_execute(isp_top_get_mod(isp_id, MOD_BLC),
						   BLC_EXEC_SET_V1_DYN, &item, sizeof(item));
}

int rts_isp_patch_set_gamma_patch_mode(uint32_t isp_id, int32_t enable)
{
	return rts_isp_patch_set_mod_patch_mode(
			   isp_id, MOD_GAMMA, GAMMA_EXEC_SET_PATCH_MODE, enable);
}

int rts_isp_patch_set_gamma_v1_dyn(uint32_t isp_id,
								   isp_iq_gamma_v1_dyn_item_t *item)
{
	return isp_mod_execute(isp_top_get_mod(isp_id, MOD_GAMMA),
						   GAMMA_EXEC_SET_V1_DYN, &item, sizeof(item));
}

int rts_isp_patch_set_ygc_patch_mode(uint32_t isp_id, int32_t enable)
{
	return rts_isp_patch_set_mod_patch_mode(
			   isp_id, MOD_YGC, YGC_EXEC_SET_PATCH_MODE, enable);
}

int rts_isp_patch_set_ygc_v1_dyn(uint32_t isp_id,
								 isp_iq_ygc_v1_dyn_item_t *item)
{
	return isp_mod_execute(isp_top_get_mod(isp_id, MOD_YGC),
						   YGC_EXEC_SET_V1_DYN, &item, sizeof(item));
}

int rts_isp_patch_set_ygamma_patch_mode(uint32_t isp_id, int32_t enable)
{
	return rts_isp_patch_set_mod_patch_mode(
			   isp_id, MOD_YGAMMA, YGAMMA_EXEC_SET_PATCH_MODE, enable);
}

int rts_isp_patch_set_ygamma_v1_dyn(uint32_t isp_id,
									isp_iq_ygamma_v1_dyn_item_t *item)
{
	return isp_mod_execute(isp_top_get_mod(isp_id, MOD_YGAMMA),
						   YGAMMA_EXEC_SET_V1_DYN, &item, sizeof(item));
}

static uint32_t rts_isp_patch_read_write_reg(uint32_t isp_id, uint32_t value,
		uint32_t offset, uint32_t mask,
		int is_read)
{
	struct isp_mod_iq_read_write_reg reg;

	if (is_read) {
		reg.type = ISP_MOD_IQ_READ_REG;
	} else {
		reg.type = ISP_MOD_IQ_WRITE_REG;
	}
	reg.value = value;
	reg.offset = offset;
	reg.mask = mask;

	isp_mod_execute(isp_top_get_mod(isp_id, MOD_IQ), IQ_EXEC_READ_WRITE_REG,
					&reg, sizeof(reg));
	if (is_read) {
		return reg.value;
	}
	return RTS_ISP_OK;
}

uint32_t rts_isp_patch_read_reg(uint32_t isp_id, uint32_t offset)
{
	return rts_isp_patch_read_write_reg(isp_id, 0, offset, 0, 1);
}

void rts_isp_patch_write_reg(uint32_t isp_id, uint32_t value, uint32_t offset)
{
	rts_isp_patch_read_write_reg(isp_id, value, offset, 0, 0);
}

void rts_isp_patch_write_reg_mask(uint32_t isp_id, uint32_t value,
								  uint32_t offset, uint32_t mask)
{
	rts_isp_patch_read_write_reg(isp_id, value, offset, mask, 0);
}
