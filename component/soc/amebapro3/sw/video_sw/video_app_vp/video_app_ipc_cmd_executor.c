#include <stdio.h>
#include <string.h>

#include "ameba_soc.h"
#include "ipc_video.h"
#include "video_app_mod_ctrl.h"
#include "video_app_main.h"

static const char *TAG = "VP_CMD_EXE";

static EventGroupHandle_t vrelease_event_group = NULL;
void vp_ipc_notify_frame_recycled(int ch);

// add command list here
int vp_ipc_cmd_executer(uint32_t cmd, uint32_t arg1, uint32_t arg2)
{
	int ret = 0;
	int ch = GET_CHANNEL(cmd);
	RTK_LOGD(TAG, "[VP]cmd running 0x%lx\n\r", cmd);
	uint32_t exe_cmd = GET_COMMAND(cmd);
	if (IS_VIDEO_COM_CMD(exe_cmd)) {
		switch (exe_cmd) {
		case VIDEO_CMD_OPEN: {
			RTK_LOGD(TAG, "VP open video\r\n");

			char *video_cmd = (char *)arg1;
			uint32_t cmd_len = arg2;

			DCache_Invalidate((u32)video_cmd, cmd_len);
			RTK_LOGD(TAG, "video_cmd from VP:%s\r\n", video_cmd);

			/* open video */
			ret = video_app_stream_start(video_cmd);

			break;
		}
		case VIDEO_CMD_CLOSE:
			RTK_LOGI(TAG, "VP close video ch%d \r\n", ch);
			ret = video_app_stream_stop(ch);
			break;
		case VIDEO_CMD_BUF_RELEASE:
			RTK_LOGD(TAG, "VP release ch%d buf\r\n", ch);
			vp_ipc_notify_frame_recycled(ch);
			break;
#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
		case VIDEO_CMD_FCS_ATTACH:
			RTK_LOGI(TAG, "VP fcs ch%d attach\r\n", ch);
			ret = video_app_fcs_attach(ch);
			break;
		case VIDEO_CMD_FCS_BOOT_STATE:
			ret = video_app_fcs_get_boot_state();
			break;
#endif
		default:
			break;
		}

	} else if (IS_VIDEO_ISP_CMD(exe_cmd)) {

		ret = vp_isp_mod_ctrl(ch, exe_cmd, arg1, arg2);

	} else if (IS_VIDEO_ENC_CMD(exe_cmd)) {

		ret = vp_enc_mod_ctrl(ch, exe_cmd, arg1, arg2);

	}
	return ret;
}

void vp_ipc_notify_frame_recycled(int ch)
{
	if (!vrelease_event_group) {
		vrelease_event_group = xEventGroupCreate();
	}
	EventBits_t set_event_bit = (1 << ch);
	xEventGroupSetBits(vrelease_event_group, set_event_bit);
}

void vp_ipc_wait_frame_recycled(int ch, uint32_t timeout_ms)
{
	if (!vrelease_event_group) {
		vrelease_event_group = xEventGroupCreate();
	}
	EventBits_t wait_event_bit = (1 << ch);
	EventBits_t bit_state = xEventGroupWaitBits(vrelease_event_group, wait_event_bit, pdTRUE, pdFALSE, timeout_ms);
	if ((bit_state & wait_event_bit) != wait_event_bit) {
		RTK_LOGW(TAG, "Warning: wait AP release ch%d frame timeout (xEventGroupWaitBits ret=0x%08x)\r\n", ch, bit_state);
	}
}
