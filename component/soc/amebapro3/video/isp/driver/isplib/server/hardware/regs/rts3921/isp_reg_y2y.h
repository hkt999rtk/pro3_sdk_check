
//tool_version:1.4.35

#ifndef _Y2Y_H_INC_
#define _Y2Y_H_INC_

#include <isp_reg.h>
#include <isp_reg_base.h>

#define Y2Y_CTRL_EN (Y2Y_BASE_ADDR + 0x00000000) //0x00008000
#define Y2Y_FRAME_SIZE (Y2Y_BASE_ADDR + 0x00000004) //0x00008004
#define Y2Y_MODE (Y2Y_BASE_ADDR + 0x0000000c) //0x0000800c

/* Bits of Y2Y_CTRL_EN (0x00000000) */

#define Y2Y_EN_OFFSET 0
#define Y2Y_EN_BITS 1
#define Y2Y_EN_MASK (((1 << 1) - 1) << 0)

/* Bits of Y2Y_FRAME_SIZE (0x00000004) */

#define FRAME_WIDTH_OFFSET 0
#define FRAME_WIDTH_BITS 12
#define FRAME_WIDTH_MASK (((1 << 12) - 1) << 0)

#define FRAME_HEIGHT_OFFSET 16
#define FRAME_HEIGHT_BITS 12
#define FRAME_HEIGHT_MASK (((1 << 12) - 1) << 16)

/* Bits of Y2Y_MODE (0x0000000c) */

#define YUV420_MODE_OFFSET 0
#define YUV420_MODE_BITS 1
#define YUV420_MODE_MASK (((1 << 1) - 1) << 0)

#endif /* _Y2Y_H_INC_ */
