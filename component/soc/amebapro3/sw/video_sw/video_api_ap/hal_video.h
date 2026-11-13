/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef __HAL_VIDEO_H__
#define __HAL_VIDEO_H__

#include "video_common_type.h"
#include "video_osd_scene.h"

void VIDEO_OpenAll(char *cmd);
void VIDEO_Open(char *cmd);
void VIDEO_Close(int ch);
void VIDEO_ReleaseBuf(int ch);
void VIDEO_GetRCParam(int ch, rc_param_t *rc_param);
void VIDEO_SetRCParam(int ch, rc_param_t *rc_param);
void VIDEO_GetRCExt(int ch, enc_rc_ext_t *rc_param);
void VIDEO_SetRCExt(int ch, enc_rc_ext_t *rc_param);
void VIDEO_GetCoding(int ch, enc_coding_t *coding);
void VIDEO_SetCoding(int ch, enc_coding_t *coding);
void VIDEO_SetGOP(int ch, int gop);
void VIDEO_ForceI(int ch);
int VIDEO_SetOSD(int ch, osd_overlay_t *osd);
int VIDEO_SetOSDScene(int ch, video_osd_scene_t *scene);
/* 1: scene owned, 0: released, negative: encoder/IPC unavailable or fault. */
int VIDEO_GetOSDSceneState(int ch);
void VIDEO_SetMosaic(int ch, mosaic_overlay_t *mosaic);
void VIDEO_AinrEnable(int en);
void VIDEO_SetFPS(int ch, int fps);
void VIDEO_ISPSwitchRAW(int format);
void VIDEO_SetSensorMode(int mode, int fps);
void VIDEO_SetISPCtrl(uint32_t id, int *value);
void VIDEO_GetISPCtrl(uint32_t id, int *value);
void VIDEO_SetI2C(int addr, int value);
void VIDEO_GetI2C(int addr, int *value);
void VIDEO_GetISPSensorDim(uint32_t *width, uint32_t *height);
void VIDEO_GetISPSensorInfo(uint32_t *snr_info);
void VIDEO_SetSensorIQ(uint32_t sensor_idx, uint32_t iq_idx);
int VIDEO_FcsAttach(int ch);
int VIDEO_FcsGetBootState(void);
int VIDEO_SnapshotStart(int ch, vp_snapshot_result_t *reply, uint32_t id);

#endif
