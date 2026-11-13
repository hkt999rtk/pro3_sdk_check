/*
 * Copyright 2024 Realtek
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _U_RTS_CAMERA_OSDI_H
#define _U_RTS_CAMERA_OSDI_H

#include <stdint.h>
#include <errno.h>

#define RTSOSDI_IOC_MAGIC	'O'

#define RTSOSDI_MAX_BLK_NUM	6

enum osdi_fmt {
	RTS_OSDI_PURE_COLOR,
	RTS_OSDI_RGBA_1111,
	RTS_OSDI_RGBA_2222,
	RTS_OSDI_RGBA_5551,
	RTS_OSDI_RGBA_4444,
	RTS_OSDI_RGBA_8888,
	RTS_OSDI_RGBA_MAX,
};

struct osdi_blk_info {
	uint8_t index;
	uint8_t enable;
	uint8_t fmt;
	uint32_t phy_addr;
	uint16_t width;
	uint16_t height;
	uint16_t start_x;
	uint16_t start_y;
	uint16_t mem_ddr;
	uint8_t transfer_length;
	uint8_t get_parameter;
};

struct osdi_all_blk_info {
	struct osdi_blk_info infos[RTSOSDI_MAX_BLK_NUM];
	uint8_t start_flag;
};

struct osdi_frame_info {
	uint16_t width;
	uint16_t height;
};

struct osdi_color_table_info {
	/*do not forget pure_color case*/
	uint8_t fmt;
	uint32_t val; /*from high to low: r, g, b, a*/
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};

#define RTSOSDI_IOC_START		((RTSOSDI_IOC_MAGIC << 16) | 0)
#define RTSOSDI_IOC_FRAME_STOP		((RTSOSDI_IOC_MAGIC << 16) | 1)
#define RTSOSDI_IOC_CONFIG_FRAME	((RTSOSDI_IOC_MAGIC << 16) | 2)
#define RTSOSDI_IOC_SET_ALL_BLK		((RTSOSDI_IOC_MAGIC << 16) | 3)
#define RTSOSDI_IOC_SET_BLK		((RTSOSDI_IOC_MAGIC << 16) | 4)
#define RTSOSDI_IOC_GET_ALL_BLK		((RTSOSDI_IOC_MAGIC << 16) | 5)
#define RTSOSDI_IOC_GET_BLK		((RTSOSDI_IOC_MAGIC << 16) | 6)
#define RTSOSDI_IOC_SET_COLOR_TABLE	((RTSOSDI_IOC_MAGIC << 16) | 7)
#define RTSOSDI_IOC_GET_COLOR_TABLE	((RTSOSDI_IOC_MAGIC << 16) | 8)
#define RTSOSDI_IOC_SET_MEM		((RTSOSDI_IOC_MAGIC << 16) | 9)
#define RTSOSDI_IOC_IMMEDIATE_STOP	((RTSOSDI_IOC_MAGIC << 16) | 10)

int rtscam_osdi_open(int osdi_num);
int rtscam_osdi_close(int osdi_num);
int rtscam_osdi_ioctl(int osdi_num, unsigned int cmd, void *arg);
int rtscam_osdi_init(void);
int rtscam_osdi_suspend(void);
int rtscam_osdi_resume(void);

#endif
