/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef __HAL_ISP_H__
#define __HAL_ISP_H__

#include "isp_common.h"
#include "rts_isp_sensor.h"
#include "sensor.h"
//#include "streamon.h"

/* Select the boot-time ISP clock and buffer profile. enable must be 0 or 1;
 * default is 0. The sensor is not changed; select it with ISP_SetSensorIQ().
 * Call it before ISP_Init() and before any ISP task starts; the mode is read
 * without locking once ISP_Init() latches it.
 * Returns 0 on success, -1 for invalid input or a mode change after ISP_Init().
 * Repeating the current mode is a no-op, including after initialization.
 */
int ISP_Set4KMode(int enable);
int ISP_Get4KMode(void);
void ISP_Init(void);
void ISP_Stop(struct isp_io *io);

/* ISP buffer and callback interface */
void isp_buf_release(int stream_id, void *buf_addr, int buf_id);
void isp_buf_recycle(int stream_id, int buf_id);
void isp_set_user_release(void);
void isp_ch_handle_register(int (*handle)(int stream_id, int buf_id, uint32_t phy_addr));
void isp_data_start_handle_register(int (*handle)(int buf_id));
void ISP_StreamOn(void *strm_param);
int ISP_StreamOnByChannel(void *strm_param);
int ISP_StreamOffByChannel(int channel);
void ISP_StreamOff(int nv0, int nv1, int nv2, int nv3, int rgb);
int ISP_GetIntrEn(int is_yuv);
int ISP_GetFrameCount(void);
int ISP_GetMipiFrameCountL(void);
int ISP_GetMipiFrameWidthL(void);
int ISP_GetMipiFrameHeightL(void);
int ISP_GetMipiFrameCountS(void);
int ISP_GetMipiFrameWidthS(void);
int ISP_GetMipiFrameHeightS(void);
int ISP_GetBufferCount(int stream_idx);
int ISP_GetBufferStatus(int stream_idx, int buffer_idx);
int ISP_GetBufferSize(int stream_idx, int channel_idx);
int ISP_GetBufferAddr(int stream_idx, int buffer_idx, int channel_idx);
int ISP_GetStreamEn(uint32_t ch);
int ISP_GetYccInterleave(void);
int ISP_GetDataReady(int stream_idx, int buffer_idx);
int ISP_GetRawIdx(void);
int ISP_GetRAWBufSize(void);
int ISP_SetStreamFPS(uint32_t ch, uint32_t fps);
void ISP_SwitchRAWData(int raw_index);
int ISP_SetSensorMode(int mode, int fps);
int ISP_SetISPCtrl(uint32_t id, int *value);
int ISP_GetISPCtrl(uint32_t id, int *value);
int ISP_SetI2C(int addr, int value);
int ISP_GetI2C(int addr, int *value);
const struct rts_isp_sensor_ops *ISP_GetSensorOps(void);
int ISP_GetSensorDimension(uint32_t *sensor_w, uint32_t *sensor_h);
int ISP_GetSensorInfo(struct sensor_info_t *sensor_info);
int ISP_SetSensorIQ(unsigned int sensor_idx, unsigned int iq_idx);

#endif /* __HAL_ISP_H__ */
