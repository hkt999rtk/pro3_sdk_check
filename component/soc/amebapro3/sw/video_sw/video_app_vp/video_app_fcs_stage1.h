/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef __VIDEO_APP_FCS_STAGE1_H__
#define __VIDEO_APP_FCS_STAGE1_H__

/* isp_stream.h is a lightweight header (stdint.h + rts_errno.h only), so it is
 * included directly here for stream_params_t rather than forward-declared. */
#include "isp_stream.h"

/* Forward declaration — avoids pulling in hal_isp.h for callers that only
 * need the sema/stage1 interface (e.g. main.c). */
struct isp_io;

/**
 * @brief  Initialize the ISP semaphore used to synchronize stage1/stage2 handoff.
 *         Must be called once before video_app_fast_camera_start().
 * @return 0 on success, -1 if the semaphore could not be created.
 */
int video_fcs_isp_stage1_sema_init(void);

/**
 * @brief  Release the ISP semaphore to signal stage2 that ISP HW is ready.
 *         Called from bf_main_task after stage2 firmware is loaded.
 */
void video_fcs_isp_stage1_sema_give(void);

/**
 * @brief  Block until the ISP semaphore is available (stage2 firmware loaded).
 *         Called inside the stage1 task before handing off to video_fcs_stream_start_task.
 */
void video_fcs_isp_stage1_sema_take(void);

/**
 * @brief  Query whether ISP stage1 streaming has been started.
 * @return Non-zero if stage1 ISP is active.
 */
int video_fcs_isp_stage1_is_started(void);

/**
 * @brief  Get the isp_io handle initialised during stage1.
 * @return Pointer to the isp_io struct, or NULL if stage1 not yet started.
 */
struct isp_io *video_fcs_isp_stage1_get_io(void);

/**
 * @brief  Copy the stream parameters used in stage1 streaming.
 * @param  param  Output buffer for stream_params_t.
 * @return 0 on success, -1 if stage1 not started or param is NULL.
 */
int video_fcs_isp_stage1_get_stream_params(stream_params_t *param);

/**
 * @brief  Start ISP stage1 streaming on the specified channel.
 *         Initialises ISP HW, registers the frame callback and calls StreamOn.
 * @param  ch  ISP channel index (must be an enabled ISP channel).
 * @return 0 on success, -1 on error.
 */
int video_fcs_isp_stage1_start(int ch);

/**
 * @brief  4K stage1: bring up the ISP driver and server without starting a
 *         stream; the ISP 4K module skips its own bring-up afterwards.
 * @return 0 on success, -1 on error.
 */
int video_fcs_isp_4k_stage1_start(void);

/**
 * @brief  FreeRTOS task: stage2 mmfv2 pipeline setup (defined in video_app_main.c).
 *         Spawned by video_fcs_isp_stage1_task after the ISP semaphore is taken.
 */
void video_fcs_stream_start_task(void *param);

/**
 * @brief  FreeRTOS task: run stage1 ISP start, wait for semaphore, then
 *         spawn video_fcs_stream_start_task for stage2 handoff.
 *         Created by video_app_fast_camera_start().
 */
void video_fcs_isp_stage1_task(void *param);

/* Accessors into video_app_main.c static state — used by stage1.c */
int  video_app_fcs_get_start_ch(void);
void video_app_fcs_set_ch_state(int ch, int state);
void video_app_fcs_set_boot_state(int state);
int  video_app_fcs_isp_ch_enabled(int ch);
int  video_app_fcs_get_isp_params(int ch, stream_params_t *out);

#endif /* __VIDEO_APP_FCS_STAGE1_H__ */
