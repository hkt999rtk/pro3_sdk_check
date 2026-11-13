/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ameba_soc.h"
#include "ameba_ipc.h"
#include "ameba_ipc_api.h"
#include "ipc_video.h"
#include "video_common_type.h"
#include "hal_video.h"

void VIDEO_ISPSwitchRAW(int format)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_ISP_SWITCH_RAW);
	vcmd = SET_CHANNEL(vcmd, 0);
	vcmd = SET_CMD_HIPRIO(vcmd);
	AP2VP_SendCmd(vcmd, (u32)format, 0, 1000);
}

void VIDEO_SetSensorMode(int mode, int fps)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_ISP_SET_SNR_MODE);
	vcmd = SET_CHANNEL(vcmd, 0);
	vcmd = SET_CMD_HIPRIO(vcmd);
	AP2VP_SendCmd(vcmd, (u32)mode, (u32)fps, 1000);
}

void VIDEO_SetISPCtrl(uint32_t id, int *value)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_ISP_SET_CONTROL);
	vcmd = SET_CHANNEL(vcmd, 0);
	vcmd = SET_CMD_HIPRIO(vcmd);
	AP2VP_SendCmd(vcmd, (u32)id, (u32)value, 1000);
}

void VIDEO_GetISPCtrl(uint32_t id, int *value)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_ISP_GET_CONTROL);
	vcmd = SET_CHANNEL(vcmd, 0);
	vcmd = SET_CMD_HIPRIO(vcmd);
	AP2VP_SendCmd(vcmd, (u32)id, (u32)value, 1000);
}

void VIDEO_SetI2C(int addr, int value)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_ISP_SET_I2C);
	vcmd = SET_CHANNEL(vcmd, 0);
	vcmd = SET_CMD_HIPRIO(vcmd);
	AP2VP_SendCmd(vcmd, (u32)addr, (u32)value, 1000);
}

void VIDEO_GetI2C(int addr, int *value)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_ISP_GET_I2C);
	vcmd = SET_CHANNEL(vcmd, 0);
	vcmd = SET_CMD_HIPRIO(vcmd);
	AP2VP_SendCmd(vcmd, (u32)addr, (u32)value, 1000);
}

void VIDEO_GetISPSensorInfo(uint32_t *snr_info)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_ISP_GET_SNR_INFO);
	vcmd = SET_CHANNEL(vcmd, 0);
	vcmd = SET_CMD_HIPRIO(vcmd);
	DCache_Invalidate((u32)snr_info, 64);
	AP2VP_SendCmd(vcmd, (u32)snr_info, 0, 1000);
}

void VIDEO_SetSensorIQ(uint32_t sensor_idx, uint32_t iq_idx)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_ISP_SET_SNR_IQ);
	vcmd = SET_CHANNEL(vcmd, 0);
	vcmd = SET_CMD_HIPRIO(vcmd);
	AP2VP_SendCmd(vcmd, (u32)sensor_idx, (u32)iq_idx, 1000);
}

void VIDEO_Open(char *cmd)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_OPEN);
	vcmd = SET_CHANNEL(vcmd, 0);
	vcmd = SET_CMD_HIPRIO(vcmd);
	uint32_t cmd_len = strlen(cmd) + 1;
	cmd_len = (cmd_len + CACHE_LINE_SIZE) & (~CACHE_LINE_SIZE);
	DCache_CleanInvalidate((u32)cmd, cmd_len);
	AP2VP_SendCmd(vcmd, (u32)cmd, cmd_len, 1000);
}

void VIDEO_OpenAll(char *cmd)
{
	VIDEO_Open(cmd);
}

void VIDEO_ReleaseBuf(int ch)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_BUF_RELEASE);
	vcmd = SET_CHANNEL(vcmd, ch);
	vcmd = SET_CMD_HIPRIO(vcmd);
	AP2VP_SendCmd(vcmd, 0, 0, 1000);
}

void VIDEO_GetRCParam(int ch, rc_param_t *rc_param)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_GET_RC);
	vcmd = SET_CHANNEL(vcmd, ch);
	vcmd = SET_CMD_HIPRIO(vcmd);
	DCache_Invalidate((u32)rc_param, sizeof(rc_param_t));
	AP2VP_SendCmd(vcmd, (u32)rc_param, 0, 1000);
}

void VIDEO_SetRCParam(int ch, rc_param_t *rc_param)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_SET_RC);
	vcmd = SET_CHANNEL(vcmd, ch);
	vcmd = SET_CMD_HIPRIO(vcmd);
	DCache_Clean((u32)rc_param, sizeof(rc_param_t));
	AP2VP_SendCmd(vcmd, (u32)rc_param, 0, 1000);
}

void VIDEO_GetRCExt(int ch, enc_rc_ext_t *rc_param)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_GET_RC_EXT);
	vcmd = SET_CHANNEL(vcmd, ch);
	vcmd = SET_CMD_HIPRIO(vcmd);
	DCache_Invalidate((u32)rc_param, sizeof(enc_rc_ext_t));
	AP2VP_SendCmd(vcmd, (u32)rc_param, 0, 1000);
}

void VIDEO_SetRCExt(int ch, enc_rc_ext_t *rc_param)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_SET_RC_EXT);
	vcmd = SET_CHANNEL(vcmd, ch);
	vcmd = SET_CMD_HIPRIO(vcmd);
	DCache_Clean((u32)rc_param, sizeof(enc_rc_ext_t));
	AP2VP_SendCmd(vcmd, (u32)rc_param, 0, 1000);
}

void VIDEO_GetCoding(int ch, enc_coding_t *coding)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_GET_CODING);
	vcmd = SET_CHANNEL(vcmd, ch);
	vcmd = SET_CMD_HIPRIO(vcmd);
	DCache_Invalidate((u32)coding, sizeof(enc_coding_t));
	AP2VP_SendCmd(vcmd, (u32)coding, 0, 1000);
}

void VIDEO_SetCoding(int ch, enc_coding_t *coding)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_SET_CODING);
	vcmd = SET_CHANNEL(vcmd, ch);
	vcmd = SET_CMD_HIPRIO(vcmd);
	DCache_Clean((u32)coding, sizeof(enc_coding_t));
	AP2VP_SendCmd(vcmd, (u32)coding, 0, 1000);
}

void VIDEO_SetGOP(int ch, int gop)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_SET_GOP);
	vcmd = SET_CHANNEL(vcmd, ch);
	vcmd = SET_CMD_HIPRIO(vcmd);
	AP2VP_SendCmd(vcmd, (u32)gop, 0, 1000);
}

void VIDEO_ForceI(int ch)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_FORCE_I);
	vcmd = SET_CHANNEL(vcmd, ch);
	vcmd = SET_CMD_HIPRIO(vcmd);
	AP2VP_SendCmd(vcmd, 0, 0, 1000);
}

int VIDEO_SetOSDScene(int ch, video_osd_scene_t *scene)
{
	uint32_t cmd = SET_CHANNEL(SET_COMMAND(0, VIDEO_CMD_SET_OSD_SCENE), ch);
	DCache_Clean((u32)scene, sizeof(*scene));
	return AP2VP_SendCmd(SET_CMD_HIPRIO(cmd), (u32)scene, 0, 1000);
}

int VIDEO_GetOSDSceneState(int ch)
{
	uint32_t cmd = SET_CHANNEL(SET_COMMAND(0, VIDEO_CMD_GET_OSD_SCENE_STATE), ch);
	return AP2VP_SendCmd(SET_CMD_HIPRIO(cmd), 0, 0, 1000);
}

int VIDEO_SetOSD(int ch, osd_overlay_t *osd)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_SET_OSD);
	vcmd = SET_CHANNEL(vcmd, ch);
	vcmd = SET_CMD_HIPRIO(vcmd);
	DCache_Clean((u32)osd, sizeof(osd_overlay_t));
	return AP2VP_SendCmd(vcmd, (u32)osd, 0, 1000);
}

void VIDEO_SetMosaic(int ch, mosaic_overlay_t *mosaic)
{
	/* Copy into a cache-line-aligned local so DCache_Clean does not corrupt
	 * adjacent stack data regardless of the caller's alignment.  AP2VP_SendCmd
	 * blocks until VP acknowledges, so &local remains valid throughout.
	 * (VIDEO_SetOSD has the same pre-existing alignment requirement from callers.) */
	ALIGNMTO(CACHE_LINE_SIZE) mosaic_overlay_t local;
	uint32_t vcmd = 0;
	memcpy(&local, mosaic, sizeof(mosaic_overlay_t));
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_SET_MOSAIC);
	vcmd = SET_CHANNEL(vcmd, ch);
	vcmd = SET_CMD_HIPRIO(vcmd);
	DCache_Clean((u32)&local, sizeof(mosaic_overlay_t));
	AP2VP_SendCmd(vcmd, (u32)&local, 0, 1000);
}

void VIDEO_Close(int ch)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_CLOSE);
	vcmd = SET_CHANNEL(vcmd, ch);
	vcmd = SET_CMD_HIPRIO(vcmd);
	AP2VP_SendCmd(vcmd, 0, 0, 1000);
}

void VIDEO_AinrEnable(int en)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_ENA_AINR);
	vcmd = SET_CHANNEL(vcmd, 0);  //don't care
	vcmd = SET_CMD_HIPRIO(vcmd);
	AP2VP_SendCmd(vcmd, (u32)en, 0, 1000);
}

void VIDEO_SetFPS(int ch, int fps)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_SET_FPS);
	vcmd = SET_CHANNEL(vcmd, ch);
	vcmd = SET_CMD_HIPRIO(vcmd);
	AP2VP_SendCmd(vcmd, (u32)fps, 0, 1000);
}

int VIDEO_FcsAttach(int ch)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_FCS_ATTACH);
	vcmd = SET_CHANNEL(vcmd, ch);
	vcmd = SET_CMD_HIPRIO(vcmd);
	return AP2VP_SendCmd(vcmd, 0, 0, 5000);
}

int VIDEO_SnapshotStart(int ch, vp_snapshot_result_t *reply, uint32_t id)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_SNAPSHOT_START);
	vcmd = SET_CHANNEL(vcmd, ch);
	int ret = AP2VP_SendCmd(vcmd, (u32)reply, id, 500);
	return (ret == -1 || ret == -2) ? VIDEO_SNAPSHOT_ERROR : ret;
}

int VIDEO_FcsGetBootState(void)
{
	uint32_t vcmd = 0;
	vcmd = SET_COMMAND(vcmd, VIDEO_CMD_FCS_BOOT_STATE);
	vcmd = SET_CHANNEL(vcmd, 0);
	vcmd = SET_CMD_HIPRIO(vcmd);
	return AP2VP_SendCmd(vcmd, 0, 0, 1000);
}
