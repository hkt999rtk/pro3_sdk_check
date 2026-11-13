#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "ameba_soc.h"
#include "ipc_video.h"
#include "video_app_main.h"
#include "video_app_mod_ctrl.h"

static const char *TAG = "VP_ENC_CTRL";

static int video_cmd_get_rc(int ch, uint32_t arg1)
{
	mm_context_t *enc_ctx = video_app_enc_get_ctx(ch);
	int ret = 0;

	if (!enc_ctx) {
		RTK_LOGE(TAG, "enc_ctx is NULL\r\n");
		return -1;
	}

	if (!arg1) {
		RTK_LOGE(TAG, "rc param is NULL\r\n");
		return -1;
	}

	ret = mm_module_ctrl(enc_ctx, CMD_ENC_GET_RCPARAM, (int)arg1);
	if (ret == 0) {
		/* VP-side: encoder wrote rc params into VP cache; clean to shared memory so AP reads up-to-date values */
		DCache_Clean(arg1, sizeof(rc_param_t));
	}

	return ret;
}

static int video_cmd_set_rc(int ch, uint32_t arg1)
{
	mm_context_t *enc_ctx = video_app_enc_get_ctx(ch);
	if (!enc_ctx) {
		RTK_LOGE(TAG, "enc_ctx is NULL\r\n");
		return -1;
	}

	if (!arg1) {
		RTK_LOGE(TAG, "rc param is NULL\r\n");
		return -1;
	}

	/* VP-side: AP wrote rc params into shared memory; invalidate VP cache before reading to discard any stale copy */
	DCache_Invalidate(arg1, sizeof(rc_param_t));
	return mm_module_ctrl(enc_ctx, CMD_ENC_SET_RCPARAM, (int)arg1);
}

static int video_cmd_get_rc_ext(int ch, uint32_t arg1)
{
	mm_context_t *enc_ctx = video_app_enc_get_ctx(ch);
	int ret = 0;

	if (!enc_ctx) {
		RTK_LOGE(TAG, "enc_ctx is NULL\r\n");
		return -1;
	}

	if (!arg1) {
		RTK_LOGE(TAG, "rc ext param is NULL\r\n");
		return -1;
	}

	ret = mm_module_ctrl(enc_ctx, CMD_ENC_GET_RC_EXT, (int)arg1);
	if (ret == 0) {
		/* VP-side: encoder wrote rc_ext params into VP cache; clean to shared memory so AP reads up-to-date values */
		DCache_Clean(arg1, sizeof(enc_rc_ext_t));
	}

	return ret;
}

static int video_cmd_set_rc_ext(int ch, uint32_t arg1)
{
	mm_context_t *enc_ctx = video_app_enc_get_ctx(ch);
	if (!enc_ctx) {
		RTK_LOGE(TAG, "enc_ctx is NULL\r\n");
		return -1;
	}

	if (!arg1) {
		RTK_LOGE(TAG, "rc ext param is NULL\r\n");
		return -1;
	}

	/* VP-side: AP wrote rc_ext params into shared memory; invalidate VP cache before reading to discard any stale copy */
	DCache_Invalidate(arg1, sizeof(enc_rc_ext_t));
	return mm_module_ctrl(enc_ctx, CMD_ENC_SET_RC_EXT, (int)arg1);
}

static int video_cmd_get_coding(int ch, uint32_t arg1)
{
	mm_context_t *enc_ctx = video_app_enc_get_ctx(ch);
	int ret = 0;

	if (!enc_ctx) {
		RTK_LOGE(TAG, "enc_ctx is NULL\r\n");
		return -1;
	}

	if (!arg1) {
		RTK_LOGE(TAG, "coding param is NULL\r\n");
		return -1;
	}

	ret = mm_module_ctrl(enc_ctx, CMD_ENC_GET_CODING_CTRL, (int)arg1);
	if (ret == 0) {
		/* VP-side: encoder wrote coding params into VP cache; clean to shared memory so AP reads up-to-date values */
		DCache_Clean(arg1, sizeof(enc_coding_t));
	}

	return ret;
}

static int video_cmd_set_coding(int ch, uint32_t arg1)
{
	mm_context_t *enc_ctx = video_app_enc_get_ctx(ch);
	if (!enc_ctx) {
		RTK_LOGE(TAG, "enc_ctx is NULL\r\n");
		return -1;
	}

	if (!arg1) {
		RTK_LOGE(TAG, "coding param is NULL\r\n");
		return -1;
	}

	/* VP-side: AP wrote coding params into shared memory; invalidate VP cache before reading to discard any stale copy */
	DCache_Invalidate(arg1, sizeof(enc_coding_t));
	return mm_module_ctrl(enc_ctx, CMD_ENC_SET_CODING_CTRL, (int)arg1);
}

static int video_cmd_set_gop(int ch, uint32_t arg1)
{
	mm_context_t *enc_ctx = video_app_enc_get_ctx(ch);
	if (!enc_ctx) {
		RTK_LOGE(TAG, "enc_ctx is NULL\r\n");
		return -1;
	}
	return mm_module_ctrl(enc_ctx, CMD_ENC_SET_GOPLEN, (int)arg1);
}

static int video_cmd_force_i(int ch)
{
	mm_context_t *enc_ctx = video_app_enc_get_ctx(ch);
	if (!enc_ctx) {
		RTK_LOGE(TAG, "enc_ctx is NULL\r\n");
		return -1;
	}
	return mm_module_ctrl(enc_ctx, CMD_ENC_FORCE_IFRAME, 0);
}

static int video_cmd_set_osd(int ch, uint32_t arg1)
{
	mm_context_t *enc_ctx = video_app_enc_get_ctx(ch);
	osd_overlay_t osd;

	if (!enc_ctx) {
		RTK_LOGE(TAG, "enc_ctx is NULL\r\n");
		return -1;
	}

	if (!arg1) {
		RTK_LOGE(TAG, "osd is NULL\r\n");
		return -1;
	}

	/* VP-side: AP wrote osd params into shared memory; invalidate VP cache before reading to discard any stale copy */
	DCache_Invalidate(arg1, sizeof(osd_overlay_t));
	memcpy(&osd, (void *)arg1, sizeof(osd_overlay_t));

	return mm_module_ctrl(enc_ctx, CMD_ENC_SET_OSD, (int)&osd);
}

static int video_cmd_set_mosaic(int ch, uint32_t arg1)
{
	mm_context_t *enc_ctx = video_app_enc_get_ctx(ch);
	mosaic_overlay_t mosaic;

	if (!enc_ctx) {
		RTK_LOGE(TAG, "enc_ctx is NULL\r\n");
		return -1;
	}

	if (!arg1) {
		RTK_LOGE(TAG, "mosaic is NULL\r\n");
		return -1;
	}

	/* VP-side: AP wrote mosaic params into shared memory; invalidate VP cache before reading to discard any stale copy */
	DCache_Invalidate(arg1, sizeof(mosaic_overlay_t));
	memcpy(&mosaic, (void *)arg1, sizeof(mosaic_overlay_t));

	return mm_module_ctrl(enc_ctx, CMD_ENC_SET_MOSAIC, (int)&mosaic);
}

int vp_enc_mod_ctrl(int ch, uint32_t cmd, uint32_t arg1, uint32_t arg2)
{
	int ret = 0;

	switch (cmd) {
	case VIDEO_CMD_SNAPSHOT_START:
		ret = video_app_snapshot_start(ch, (vp_snapshot_result_t *)arg1, arg2);
		break;
	case VIDEO_CMD_GET_RC:
		ret = video_cmd_get_rc(ch, arg1);
		break;
	case VIDEO_CMD_SET_RC:
		ret = video_cmd_set_rc(ch, arg1);
		break;
	case VIDEO_CMD_GET_RC_EXT:
		ret = video_cmd_get_rc_ext(ch, arg1);
		break;
	case VIDEO_CMD_SET_RC_EXT:
		ret = video_cmd_set_rc_ext(ch, arg1);
		break;
	case VIDEO_CMD_GET_CODING:
		ret = video_cmd_get_coding(ch, arg1);
		break;
	case VIDEO_CMD_SET_CODING:
		ret = video_cmd_set_coding(ch, arg1);
		break;
	case VIDEO_CMD_SET_GOP:
		ret = video_cmd_set_gop(ch, arg1);
		break;
	case VIDEO_CMD_FORCE_I:
		ret = video_cmd_force_i(ch);
		break;
	case VIDEO_CMD_SET_OSD:
		ret = video_cmd_set_osd(ch, arg1);
		break;
	case VIDEO_CMD_SET_OSD_SCENE: {
		video_osd_scene_t scene;
		mm_context_t *ctx = video_app_enc_get_ctx(ch);
		if (!arg1) {
			return -1;
		}
		DCache_Invalidate(arg1, sizeof(scene));
		memcpy(&scene, (void *)arg1, sizeof(scene));
		ret = ctx ? mm_module_ctrl(ctx, CMD_ENC_SET_OSD_SCENE, (int)&scene) : -1;
		if (ret != 0 && scene.completion) {
			*scene.completion = VIDEO_OSD_SCENE_REJECTED;
			DCache_Clean((uint32_t)scene.completion, CACHE_LINE_SIZE);
		}
		break;
	}
	case VIDEO_CMD_GET_OSD_SCENE_STATE: {
		mm_context_t *ctx = video_app_enc_get_ctx(ch);
		ret = ctx ? mm_module_ctrl(ctx, CMD_ENC_GET_OSD_SCENE_STATE, 0) : -1;
		break;
	}
	case VIDEO_CMD_SET_MOSAIC:
		ret = video_cmd_set_mosaic(ch, arg1);
		break;
	default:
		RTK_LOGE(TAG, "command not handled\r\n");
		ret = -2;
		break;
	}
	return ret;
}
