/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <rts_isp_sensor.h>
#include <isp_top.h>
#include <isp_mod.h>
#include <isp_core.h>
#include <isp_notify.h>
#include <isp_sensor.h>
#include <isp_hw_input.h>
#include "isp_mod_input.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_INPUT

#define VSNR_HBLANK 72
#define VSNR_VBLANK 22

struct isp_mod_input_vsnr_mode {
	int ddr_mode;

	struct rts_isp_sensor_mode mode;
	uint32_t bayer;
	uint32_t hts;
	uint32_t vts;
	uint32_t clk;
};

struct isp_mod_input {
	struct isp_mod mod;
	const struct isp_hw_input_ops *hw_ops;

	struct rts_isp_sensor_ops vsnr_ops;
	struct isp_mod_input_vsnr_mode vsnr_mode;

	uint32_t large_image_mode : 1;
	uint8_t ic_cut;
	struct rts_isp_sensor_info large_image_info;
	const struct rts_isp_sensor_ops *large_image_ops;
};

static struct isp_mod_input *get_input_mode(uint32_t isp_id)
{
	struct isp_mod *mod;

	mod = isp_top_get_mod(isp_id, MOD_INPUT);
	if (!mod) {
		return NULL;
	}
	return get_mod(mod, input);
}

static int input_get_info(uint32_t isp_id, struct rts_isp_sensor_info *info)
{
	int ret;
	struct isp_mod_input *input;

	input = get_input_mode(isp_id);
	if (!input || !info) {
		return -RTS_ISP_EINVAL;
	}

	info->modes.mode[0] = input->vsnr_mode.mode;
	info->modes.num = 1;

	info->i2c.i2c_id = 0;
	info->i2c.data_len = 1;
	info->i2c.addr_len = 1;

	info->power_up.num = 0;
	info->power_down.num = 0;

	if (input->large_image_mode) {
		ret = input->large_image_ops->get_info(
				  isp_id, &input->large_image_info);
		if (ret) {
			return ret;
		}

		info->modes.mode[0] = input->large_image_info.modes.mode[0];
		info->modes.mode[0].size = input->vsnr_mode.mode.size;
		info->modes.num = 1;

		info->i2c = input->large_image_info.i2c;
		info->power_up = input->large_image_info.power_up;
		info->power_down = input->large_image_info.power_down;
		info->focus = input->large_image_info.focus;
	} else {
		info->modes.mode[0] = input->vsnr_mode.mode;
		info->modes.num = 1;

		info->i2c.i2c_id = 0;
		info->i2c.data_len = 1;
		info->i2c.addr_len = 1;

		info->power_up.num = 0;
		info->power_down.num = 0;
	}

	return RTS_ISP_OK;
}

static int input_get_init_info(uint32_t isp_id,
							   const struct rts_isp_sensor_mode *mode,
							   struct rts_isp_sensor_init_info *info)
{
	int ret;
	struct isp_mod_input *input;

	input = get_input_mode(isp_id);
	if (!input || !info) {
		return -RTS_ISP_EINVAL;
	}

	if (input->large_image_mode) {
		ret = input->large_image_ops->get_init_info(
				  isp_id, &input->large_image_info.modes.mode[0], info);
		if (ret) {
			return ret;
		}

		info->start.x = 0;
		info->start.y = 0;
		info->size = input->vsnr_mode.mode.size;
	} else {
		info->interface.interface = SNR_INTERFACE_NONE;
		info->interface.type_config.raw.bayer = input->vsnr_mode.bayer;

		info->size = input->vsnr_mode.mode.size;
		info->hts = input->vsnr_mode.hts;
		info->pclk = input->vsnr_mode.clk;
		info->min_vts = info->max_vts = input->vsnr_mode.vts;
	}

	return RTS_ISP_OK;
}

static int input_get_tuned_again(uint32_t isp_id,
								 float again[RTS_ISP_HDR_CHAN_MAX])
{
	struct isp_mod_input *input;

	input = get_input_mode(isp_id);
	if (!input) {
		return -RTS_ISP_EINVAL;
	}

	if (input->large_image_mode) {
		return input->large_image_ops->get_tuned_again(isp_id, again);
	}

	return RTS_ISP_OK;
}

static int input_get_tuned_dgain(uint32_t isp_id,
								 float dgain[RTS_ISP_HDR_CHAN_MAX])
{
	struct isp_mod_input *input;

	input = get_input_mode(isp_id);
	if (!input) {
		return -RTS_ISP_EINVAL;
	}

	if (input->large_image_mode) {
		return input->large_image_ops->get_tuned_dgain(isp_id, dgain);
	}

	return RTS_ISP_OK;
}

static int
input_get_exposure_gain_info(uint32_t isp_id,
							 const struct rts_isp_sensor_exp_gain *exp_gain,
							 struct rts_isp_sync_regs *regs)
{
	return RTS_ISP_OK;
}

static void input_init_vsnr_ops(struct isp_mod_input *input)
{
	if (input->large_image_mode && input->large_image_ops) {
		input->vsnr_ops = *input->large_image_ops;
	}
	input->vsnr_ops.api_version = SENSOR_API_VERSION;
	isp_memcpy(input->vsnr_ops.name, "input", sizeof("input"));
	input->vsnr_ops.get_info = input_get_info;
	input->vsnr_ops.get_init_info = input_get_init_info;
	input->vsnr_ops.get_tuned_again = input_get_tuned_again;
	input->vsnr_ops.get_tuned_dgain = input_get_tuned_dgain;
	input->vsnr_ops.get_exposure_gain_info = input_get_exposure_gain_info;
}

static uint32_t __fmt_to_location(uint32_t fmt)
{
	switch (fmt) {
	case ISP_INPUT_SBGGR16:
		return INPUT_CROP_L_LOCATION;
	case ISP_INPUT_SGBRG16:
		return INPUT_CROP_L_LOCATION;
	case ISP_INPUT_SGRBG16:
		return INPUT_CROP_L_LOCATION;
	case ISP_INPUT_SRGGB16:
		return INPUT_CROP_L_LOCATION;
	case ISP_INPUT_SGRBG32:
		return INPUT_FUSION_OUT_LOCATION;
	case ISP_INPUT_RGB:
		return INPUT_RGB_LOCATION;
	case ISP_INPUT_YUV:
		return INPUT_YUV_LOCATION;
	case ISP_INPUT_YUYV:
		return INPUT_YUYV_LOCATION;
	case ISP_INPUT_NV16:
		return INPUT_NV16_LOCATION;
	default:
		return INPUT_CROP_L_LOCATION;
	}
}

static uint32_t __fmt_to_bayer(uint32_t fmt)
{
	switch (fmt) {
	case ISP_INPUT_SBGGR16:
		return SNR_BGGR;
	case ISP_INPUT_SGBRG16:
		return SNR_GBRG;
	case ISP_INPUT_SGRBG16:
		return SNR_GRBG;
	case ISP_INPUT_SRGGB16:
		return SNR_RGGB;
	case ISP_INPUT_SGRBG32:
		return SNR_GRBG;
	default:
		return SNR_GRBG;
	}
}

static int input_check_behavior(struct isp_mod_input *input,
								struct isp_mod_input_behavior *behavior)
{
	float fps;

	if (!behavior) {
		return -RTS_ISP_EINVAL;
	}
	if (!behavior->ddr_mode) {
		return RTS_ISP_OK;
	}

	if (behavior->fmt != ISP_INPUT_SBGGR16 &&
		behavior->fmt != ISP_INPUT_SGBRG16 &&
		behavior->fmt != ISP_INPUT_SGRBG16 &&
		behavior->fmt != ISP_INPUT_SRGGB16 &&
		behavior->fmt != ISP_INPUT_SGRBG32 &&
		behavior->fmt != ISP_INPUT_RGB && behavior->fmt != ISP_INPUT_YUV &&
		behavior->fmt != ISP_INPUT_YUYV && behavior->fmt != ISP_INPUT_NV16) {
		return -RTS_ISP_EINVAL;
	}

	if (behavior->width < 64 || behavior->height < 64) {
		return -RTS_ISP_EINVAL;
	}
	if (behavior->width % 2 || behavior->height % 2) {
		return -RTS_ISP_EINVAL;
	}

	fps = (float)behavior->fival.denominator / behavior->fival.numerator;
	if (fps < 0.1) {
		return -RTS_ISP_EINVAL;
	}
	if (input->vsnr_mode.clk / (behavior->width + VSNR_HBLANK) / fps <
		behavior->height + VSNR_VBLANK) {
		return -RTS_ISP_EINVAL;
	}

	return RTS_ISP_OK;
}

static int mod_input_exec_set_behavior(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_hw_input_behavior hw_behavior;
	struct isp_mod_input_vsnr_mode backup_mode;
	int32_t plug_type = RTS_ISP_PLUG_TYPE_SENSOR;
	struct isp_mod_input_behavior *behavior = data;
	struct isp_mod_input *input = get_mod(mod, input);

	if (input_check_behavior(input, behavior)) {
		return -RTS_ISP_EINVAL;
	}

	if (isp_top_get_status() == RTS_ISP_STREAMING) {
		return -RTS_ISP_EBUSY;
	}

	if (!behavior->ddr_mode && !input->vsnr_mode.ddr_mode) {
		return RTS_ISP_OK;
	}

	backup_mode = input->vsnr_mode;

	input->vsnr_mode.ddr_mode = behavior->ddr_mode;
	if (input->vsnr_mode.ddr_mode) {
		input->vsnr_mode.mode.hdr = RTS_ISP_HDR_NONE;
		input->vsnr_mode.mode.size.w = behavior->width;
		input->vsnr_mode.mode.size.h = behavior->height;
		input->vsnr_mode.mode.fps = (float)behavior->fival.denominator /
									behavior->fival.numerator;
		input->vsnr_mode.bayer = __fmt_to_bayer(behavior->fmt);
		input->vsnr_mode.hts = behavior->width + VSNR_HBLANK;
		input->vsnr_mode.vts = input->vsnr_mode.clk /
							   input->vsnr_mode.mode.fps /
							   input->vsnr_mode.hts / 2 * 2;
		if (input->large_image_mode) {
			ret = isp_os_driver_set_large_image_mode(true);
			if (ret) {
				goto err;
			}
		}
	} else {
		ret = isp_os_driver_set_large_image_mode(false);
		if (ret) {
			goto err;
		}
	}

	ret = isp_core_notify(mod->owner, ISP_NOTIFY_PLUGIN_UPDATE, &plug_type,
						  sizeof(plug_type));
	if (ret) {
		goto err;
	}

	hw_behavior.ddr_mode = behavior->ddr_mode;
	hw_behavior.location = __fmt_to_location(behavior->fmt);
	if (input->large_image_mode) {
		/* Large-image input ignores fmt location/bayer; hardware routing and
		 * silicon cut determine the injection path and use GRBG. */
		hw_behavior.location = input->ic_cut == 0 ? INPUT_TNR_LOCATION :
							   INPUT_CROP_L_LOCATION;
		input->vsnr_mode.bayer = SNR_GRBG;
	}
	hw_behavior.width = behavior->width;
	hw_behavior.height = behavior->height;
	hw_behavior.hblank = VSNR_HBLANK;
	hw_behavior.vblank = VSNR_VBLANK;
	ret = input->hw_ops->set_behavior(mod->owner_id, &hw_behavior);
	if (ret) {
		goto err;
	}

	return RTS_ISP_OK;
err:
	input->vsnr_mode = backup_mode;
	isp_core_notify(mod->owner, ISP_NOTIFY_PLUGIN_UPDATE, &plug_type,
					sizeof(plug_type));
	return ret;
}

static int mod_input_exec_set_buffer(struct isp_mod *mod, void *data)
{
	struct isp_mod_input_buffer *buf = data;
	struct isp_mod_input *input = get_mod(mod, input);

	return input->hw_ops->set_buffer(mod->owner_id, buf->phy_addr);
}

static int mod_input_exec_set_large_image_mode(struct isp_mod *mod, void *data)
{
	int ret;
	uint32_t mode = *(uint32_t *)data;
	struct isp_mod_input *input = get_mod(mod, input);

	if (mode != input->large_image_mode) {
		input->large_image_mode = mode;
		if (input->large_image_mode) {
			ret = isp_sensor_get_ops(mod->owner_id,
									 &input->large_image_ops);
			if (ret) {
				return ret;
			}
		}
	}

	input_init_vsnr_ops(input);
	return RTS_ISP_OK;
}

static struct isp_mod_action_info input_exec_actions[] = {
	ISP_MOD_ACTION(INPUT_EXEC_SET_BEHAVIOR, mod_input_exec_set_behavior),
	ISP_MOD_ACTION(INPUT_EXEC_SET_BUFFER, mod_input_exec_set_buffer),
	ISP_MOD_ACTION(INPUT_EXEC_SET_LARGE_IMAGE_MODE,
				   mod_input_exec_set_large_image_mode),
};

static int mod_input_info_snr_ops(struct isp_mod *mod, void *data)
{
	const struct rts_isp_sensor_ops **ops = data;
	struct isp_mod_input *input = get_mod(mod, input);

	if (input->vsnr_mode.ddr_mode) {
		*ops = &input->vsnr_ops;
		return RTS_ISP_OK;
	} else {
		return isp_sensor_get_ops(mod->owner_id, ops);
	}
}

static int mod_input_info_large_image_mode(struct isp_mod *mod, void *data)
{
	struct isp_mod_input *input = get_mod(mod, input);

	*(uint32_t *)data = input->large_image_mode;
	return RTS_ISP_OK;
}

static struct isp_mod_action_info input_info_actions[] = {
	ISP_MOD_ACTION(INPUT_INFO_SNR_OPS, mod_input_info_snr_ops),
	ISP_MOD_ACTION(INPUT_INFO_LARGE_IMAGE_MODE,
				   mod_input_info_large_image_mode),
};

static int isp_mod_input_init(struct isp_mod *mod)
{
	int ret;
	int is_fpga;
	struct isp_mod_input *input;

	input = get_mod(mod, input);
	input->hw_ops = isp_hw_input_get_ops();
	if (input->hw_ops == NULL) {
		isp_error("get input hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}
	input_init_vsnr_ops(input);
	input->ic_cut = isp_os_driver_get_ic_cut();
	is_fpga = isp_os_driver_is_fpga();
	if (is_fpga) {
		input->vsnr_mode.clk = 48000000UL;
	} else {
		input->vsnr_mode.clk = 200000000UL;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_input_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static struct isp_mod_input input;

int isp_register_mod_input(void)
{
	isp_memset(&input, 0, sizeof(input));
	input.mod.id = MOD_INPUT;
	input.mod.name = "input";
	input.mod.init = isp_mod_input_init;
	input.mod.cleanup = isp_mod_input_cleanup;
	input.mod.exec_actions = input_exec_actions;
	input.mod.exec_actions_num = ARRAY_SIZE(input_exec_actions);
	input.mod.info_actions = input_info_actions;
	input.mod.info_actions_num = ARRAY_SIZE(input_info_actions);
	input.mod.size = sizeof(input);
	return isp_mod_register(&input.mod);
}

void isp_unregister_mod_input(void)
{
	isp_mod_unregister(&input.mod);
}
