
//tool_version:1.4.35

#ifndef _CROP_L_H_INC_
#define _CROP_L_H_INC_

#include <isp_reg.h>
#include <isp_reg_base.h>

#define CROP_START_X (CROP_L_BASE_ADDR + 0x00000000) //0x00001800
#define CROP_START_Y (CROP_L_BASE_ADDR + 0x00000004) //0x00001804
#define CROP_FRAME_WIDTH (CROP_L_BASE_ADDR + 0x00000008) //0x00001808
#define CROP_FRAME_HEIGHT (CROP_L_BASE_ADDR + 0x0000000c) //0x0000180c
#define CROP_SYNC (CROP_L_BASE_ADDR + 0x00000010) //0x00001810
#define CROP_DUMMY_PIXEL (CROP_L_BASE_ADDR + 0x00000014) //0x00001814

/* Bits of CROP_START_X (0x00000000) */

#define BLC_START_X_OFFSET 0
#define BLC_START_X_BITS 16
#define BLC_START_X_MASK (((1 << 16) - 1) << 0)

/* Bits of CROP_START_Y (0x00000004) */

#define BLC_START_Y_OFFSET 0
#define BLC_START_Y_BITS 16
#define BLC_START_Y_MASK (((1 << 16) - 1) << 0)

/* Bits of CROP_FRAME_WIDTH (0x00000008) */

#define FRAME_WIDTH_OFFSET 0
#define FRAME_WIDTH_BITS 16
#define FRAME_WIDTH_MASK (((1 << 16) - 1) << 0)

/* Bits of CROP_FRAME_HEIGHT (0x0000000c) */

#define FRAME_HEIGHT_OFFSET 0
#define FRAME_HEIGHT_BITS 16
#define FRAME_HEIGHT_MASK (((1 << 16) - 1) << 0)

/* Bits of CROP_SYNC (0x00000010) */

#define CROP_REG_LOAD_OFFSET 0
#define CROP_REG_LOAD_BITS 1
#define CROP_REG_LOAD_MASK (((1 << 1) - 1) << 0)

/* Bits of CROP_DUMMY_PIXEL (0x00000014) */

#define DUMMY_PIXEL_OFFSET 0
#define DUMMY_PIXEL_BITS 16
#define DUMMY_PIXEL_MASK (((1 << 16) - 1) << 0)

#endif /* _CROP_L_H_INC_ */
