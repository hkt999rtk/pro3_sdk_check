/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef __VIDEO_API_H__
#define __VIDEO_API_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "video_common_type.h"

typedef struct v_out_s {
	uint32_t ch;
	uint32_t type;
	uint32_t timestamp;
	uint32_t addr;
	uint32_t size;
} v_out_t;

void video_open_all(char *v_cmd);
void video_open(char *cmd);
void video_close(int ch);
void video_set_fps(int ch, int fps);
void video_release_buf(int ch);
void video_switch_raw(int format);
void video_set_sensor_mode(int mode, int fps);
void video_set_isp_ctrl(uint32_t id, int *value);
void video_get_isp_ctrl(uint32_t id, int *value);
void video_set_i2c(int addr, int value);
void video_get_i2c(int addr, int *value);
void video_get_sensor_info(uint32_t *snr_info);
void video_set_sensor_iq(uint32_t sensor_idx, uint32_t iq_idx);
void video_get_rc_param(int ch, void *rc);
void video_set_rc_param(int ch, void *rc);
void video_get_rc_ext(int ch, vpap_shared_enc_rc_ext_t *rc);
void video_set_rc_ext(int ch, vpap_shared_enc_rc_ext_t *rc);
void video_set_bitrate(int ch, uint32_t bps);
void video_set_rcmode(int ch, uint32_t rc_mode);
void video_set_qp_hdr(int ch, int32_t qp_hdr);
void video_set_intra_qp_delta(int ch, int32_t intra_qp_delta);
void video_set_fixed_intra_qp(int ch, uint32_t fixed_intra_qp);
void video_set_i_qp(int ch, uint32_t min_i_qp, uint32_t max_i_qp);
void video_set_qp(int ch, uint32_t min_qp, uint32_t max_qp);
void video_set_minqp(int ch, uint32_t min_qp);
void video_set_maxqp(int ch, uint32_t max_qp);
void video_set_pic_rc(int ch, uint32_t enable);
void video_set_ctb_rc(int ch, uint32_t mode);
void video_set_block_rc_size(int ch, uint32_t size);
void video_set_picture_skip(int ch, uint32_t enable);
void video_set_bitrate_window(int ch, uint32_t frames);
void video_set_moving_bitrate(int ch, int32_t tolerance, int32_t frames);
void video_set_crf(int ch, int32_t crf);
void video_set_vbr(int ch, uint32_t enable);
void video_set_gop(int ch, int gop);
int video_get_snapshot(int ch, video_snapshot_t *snapshot);
void video_force_iframe(int ch);
void video_get_coding(int ch, vpap_shared_enc_coding_t *coding);
void video_set_coding(int ch, vpap_shared_enc_coding_t *coding);
void video_set_slice_size(int ch, uint32_t slice_size);
void video_set_sei_user_data(int ch, uint32_t enable);
void video_set_deblock(int ch, uint32_t disable, int32_t tc_offset, int32_t beta_offset);
void video_set_cir(int ch, uint32_t start, uint32_t interval);
void video_set_roi(int ch, int idx, uint32_t enable, uint32_t top, uint32_t left, uint32_t bottom, uint32_t right, int32_t delta_qp, int32_t qp);
int video_set_osd_bitmap(int ch, int idx, video_osd_format_t fmt, uint32_t x_off, uint32_t y_off, uint32_t w, uint32_t h, uint32_t alpha, uint32_t y,
						 uint32_t u, uint32_t v, uint8_t *bitmap);
int video_disable_osd(int ch, int idx);
void video_set_mosaic(int ch, int idx, uint32_t x_off, uint32_t y_off, uint32_t w, uint32_t h, uint32_t size);
void video_disable_mosaic(int ch, int idx);
/* Total hardware OSD/mosaic overlay index slots per ISP channel.
 * OSD and mosaic allocations draw from this shared pool via video_alloc_overlay_index().
 * The encoder supports up to MAX_OVERLAY_NUM (12) OSD regions and MAX_MOSAIC_NUM (12) mosaic
 * regions, but both are backed by the same 12 hardware indices.  Concurrent use of OSD and
 * mosaic must therefore satisfy: (osd layers) + (mosaic regions) <= VIDEO_OVERLAY_INDEX_COUNT.
 * Pool exhaustion is detected at runtime — video_alloc_overlay_index() returns -1, and both
 * nn_osd_group_init() and nn_mosaic_group_init() propagate that failure to the caller. */
#define VIDEO_OVERLAY_INDEX_COUNT 12
int  video_alloc_overlay_index(int ch);
void video_free_overlay_index(int ch, int idx);
void video_ainr_enable(int en);
int video_fcs_attach(int ch);
int video_fcs_get_boot_state(void);
void video_rx_cb_register(int ch, void (*cb_func)(void *v, void *arg), void *cb_arg);
void video_rx_sw_init(void);
void video_rx_sw_deinit(void);

typedef struct _VIDEO_APP_TABLE_ {
	void(*v_func)(void);
	char *v_func_name;
} VIDEO_APP_TABLE, *PVIDEO_APP_TABLE;

#endif
