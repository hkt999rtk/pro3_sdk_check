#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "ameba_soc.h"
#include "ipc_video.h"
#include "video_app_main.h"
#include "video_app_mod_ctrl.h"
#include "hal_isp.h"

static const char *TAG = "VP_ISP_CTRL";

static int video_cmd_set_fps(int ch, uint32_t arg1)
{
	mm_context_t *isp_ctx = video_app_isp_get_ctx(ch);
	if (!isp_ctx) {
		RTK_LOGE(TAG, "isp_ctx is NULL\r\n");
		return -1;
	}
	return mm_module_ctrl(isp_ctx, CMD_ISP_SET_FPS, (int)arg1);
}

static int video_cmd_ena_ainr(int ch, uint32_t arg1)
{
	mm_context_t *isp_ctx = video_app_isp_get_ctx(ch);
	if (!isp_ctx) {
		RTK_LOGE(TAG, "isp_ctx is NULL\r\n");
		return -1;
	}
	int ainr_en = (int)arg1;
	int isp_mod_cmd = ainr_en ? CMD_ISP_AINR_START : CMD_ISP_AINR_STOP;
	return mm_module_ctrl(isp_ctx, isp_mod_cmd, 0);
}

int vp_isp_mod_ctrl(int ch, uint32_t cmd, uint32_t arg1, uint32_t arg2)
{
	(void)arg2;
	int ret = 0;

	switch (cmd) {
	case VIDEO_CMD_SET_FPS:
		ret = video_cmd_set_fps(ch, arg1);
		break;
	case VIDEO_CMD_ISP_SWITCH_RAW: {
		ISP_SwitchRAWData(arg1);
		break;
	}
	case VIDEO_CMD_ISP_SET_SNR_MODE: {
		ret = ISP_SetSensorMode(arg1, arg2);
		break;
	}
	case VIDEO_CMD_ISP_SET_CONTROL: {
		ret = ISP_SetISPCtrl(arg1, (int *)arg2);
		break;
	}
	case VIDEO_CMD_ISP_GET_CONTROL: {
		ret = ISP_GetISPCtrl(arg1, (int *)arg2);
		break;
	}
	case VIDEO_CMD_ISP_SET_I2C: {
		ret = ISP_SetI2C(arg1, arg2);
		break;
	}
	case VIDEO_CMD_ISP_GET_I2C: {
		ret = ISP_GetI2C(arg1, (int *)arg2);
		break;
	}
	case VIDEO_CMD_ISP_GET_SNR_INFO: {
		struct sensor_info_t *info = (struct sensor_info_t *)arg1;
		ret = ISP_GetSensorInfo(info);
		break;
	}
	case VIDEO_CMD_ISP_SET_SNR_IQ: {
		ret = ISP_SetSensorIQ(arg1, arg2);
		break;
	}
	case VIDEO_CMD_ENA_AINR:
		ret = video_cmd_ena_ainr(ch, arg1);
		break;
	default:
		RTK_LOGE(TAG, "command not handled\r\n");
		ret = -2;
		break;
	}
	return ret;
}
