/*
 * Copyright 2024 Realtek
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Ported from Zephyr: removed k_poll_signal (no user-space on MCU).
 */

#ifndef _RTS_CAMERA_MD_H
#define _RTS_CAMERA_MD_H

#include <stdint.h>

/* ioctls */
#define RTSMD_IOC_MAGIC          'm'
#define RTSMD_IOC_SET_ATTR       (RTSMD_IOC_MAGIC << 16 | 1)
#define RTSMD_IOC_GET_ATTR       (RTSMD_IOC_MAGIC << 16 | 2)
#define RTSMD_IOC_SET_FRAME_BUF  (RTSMD_IOC_MAGIC << 16 | 3)
#define RTSMD_IOC_SET_AXI_BUF    (RTSMD_IOC_MAGIC << 16 | 4)
#define RTSMD_IOC_SET_SKIP       (RTSMD_IOC_MAGIC << 16 | 5)
#define RTSMD_IOC_ENABLE         (RTSMD_IOC_MAGIC << 16 | 6)
#define RTSMD_IOC_TRAIN          (RTSMD_IOC_MAGIC << 16 | 7)
#define RTSMD_IOC_GET_RESULT     (RTSMD_IOC_MAGIC << 16 | 8)
#define RTSMD_IOC_RESUME         (RTSMD_IOC_MAGIC << 16 | 9)
#define RTSMD_IOC_DISABLE        (RTSMD_IOC_MAGIC << 16 | 10)

/* interrupt post-treatment flags */
#define MD_POLL_FLAG_OK            0x1
#define MD_POLL_FLAG_NEED_RETRAIN  0x2

struct rts_md_attr {
	struct {
		uint32_t x, y;
		uint32_t w, h;
		uint8_t scale_w, scale_h;
	} md_in, md_out;

	struct {
		uint32_t x, y;
		uint32_t x_end, y_end;
	} roi;

	uint8_t nr_bins;
	uint8_t bin_bits;
	uint8_t train_frames;

	uint8_t learn, forget;
	uint8_t back_thd;
	uint8_t ds_thd;
	uint32_t motion_thd;	/* hardware accepts up to 131071 */

	uint8_t bl_hist_wr;  /* burst length hist write */
	uint8_t bl_res_wr;   /* burst length result write */
	uint8_t bl_isp_rd;   /* burst length isp read */
};

struct rts_md_buf {
	uint32_t hist_addr, hist_size;
	uint32_t res_addr, res_size;
};

struct rts_md_result {
	uint8_t motion_flag;
	uint16_t count;
};

/* MD driver API (FreeRTOS: no device pointer, uses global state) */
int rts_md_init(void);
int rts_md_open(void);
void rts_md_close(void);
int rts_md_ioctl(unsigned int cmd, void *arg);

/* Callback invoked from MD ISR — set by the user to process results */
typedef void (*rts_md_callback_t)(uint32_t poll_flags);
void rts_md_set_callback(rts_md_callback_t cb);

#endif /* _RTS_CAMERA_MD_H */
