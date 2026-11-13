/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2024 Realtek Semiconductor Corp. All rights reserved.
 *
 * This software is a confidential and proprietary property of Realtek
 * Semiconductor Corp. Disclosure, reproduction, redistribution, in
 * whole or in part, of this work and its derivatives without express
 * permission is prohibited.
 *
 * Realtek Semiconductor Corp. reserves the right to update, modify, or
 * discontinue this software at any time without notice. This software is
 * provided "as is" and any express or implied warranties, including, but
 * not limited to, the implied warranties of merchantability and fitness for
 * a particular purpose are disclaimed. In no event shall Realtek
 * Semiconductor Corp. be liable for any direct, indirect, incidental,
 * special, exemplary, or consequential damages (including, but not limited
 * to, procurement of substitute goods or services; loss of use, data, or
 * profits; or business interruption) however caused and on any theory of
 * liability, whether in contract, strict liability, or tort (including
 * negligence or otherwise) arising in any way out of the use of this software,
 * even if advised of the possibility of such damage.
 */


#ifndef __GE_DRIVER_H
#define __GE_DRIVER_H


#include <basic_types.h>
#include "ameba_soc.h"

#if 0

static __u16 rgb2yuv_cnvt_coef[9] = { 0xda,  0x2dc, 0x4a,  0x76, 0x18a,
									  0x200, 0x200, 0x1d1, 0x2f
									};

static __u16 yuv2rgb_cnvt_coef[9] = { 0x100, 0x100, 0x100, 0x0, 0x30,
									  0x1db, 0x193, 0x78,  0x0
									};

static const __u16 zoom_coef[20] = { 0x0,  0x0,	 0x0,  0x0,  0x0,  0x0,	 0x0,
									 0x0,  0x0,	 0x0,  0x0,  0x0,  0x01, 0x20,
									 0x40, 0x60, 0xa0, 0xc0, 0xe0, 0xff
								   };
#endif

#define GE_TIMEOUT_MS 10000

struct ge_surface {
	//__u32 slot;
	void *phy_addr;
	//void *mem_virt;
	//void *mmap_addr;
	__u32 size;
	__u32 file_len;
};

enum SURFACE_NO { SRC = 0, DST, MAX };

struct ge_context {
	int ge_fd;
	struct ge_surface surface[MAX];
};

enum ge_cmd {
	GE_CMD_BITBLT = 0x50,
	GE_CMD_GT,
	GE_CMD_ZOOM,
	GE_CMD_NOP,

	GE_CMD_SET_RGB2YUV = 0x60,
	GE_CMD_SET_YUV2RGB
};

static inline void ge_iowrite32(u32 offset, u32 val)
{
	//iowrite32(val, (__le32 __iomem *)(regmap_base + offset));
	HAL_WRITE32(GE_REG_BASE, offset, val);
}

static inline u32 ge_ioread32(u32 offset)
{
	//return ioread32((__le32 __iomem *)(regmap_base + offset));
	return HAL_READ32(GE_REG_BASE, offset);
}




#endif /* __GE_DRIVER_H */
