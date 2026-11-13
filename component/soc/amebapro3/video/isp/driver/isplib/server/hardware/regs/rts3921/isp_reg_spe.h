
//tool_version:1.4.35

#ifndef _SPE_H_INC_
#define _SPE_H_INC_

#include <isp_reg.h>
#include <isp_reg_base.h>

#define SPE_CTRL_EN (SPE_BASE_ADDR + 0x00000000) //0x00005800
#define SPE_Y_MONO (SPE_BASE_ADDR + 0x00000008) //0x00005808
#define SPE_TUNE (SPE_BASE_ADDR + 0x0000000c) //0x0000580c
#define SPE_UV_TARGET (SPE_BASE_ADDR + 0x00000010) //0x00005810
#define SPE_YCONTRAST (SPE_BASE_ADDR + 0x00000014) //0x00005814
#define SPE_Y_OFFSET (SPE_BASE_ADDR + 0x00000018) //0x00005818
#define SPE_U_OFFSET (SPE_BASE_ADDR + 0x0000001c) //0x0000581c
#define SPE_V_OFFSET (SPE_BASE_ADDR + 0x00000020) //0x00005820
#define SPE_Y_GAIN (SPE_BASE_ADDR + 0x00000024) //0x00005824
#define SPE_U_GAIN (SPE_BASE_ADDR + 0x00000028) //0x00005828
#define SPE_V_GAIN (SPE_BASE_ADDR + 0x0000002c) //0x0000582c
#define SPE_REG_LOAD (SPE_BASE_ADDR + 0x00000030) //0x00005830

/* Bits of SPE_CTRL_EN (0x00000000) */

#define SPE_EN_OFFSET 0
#define SPE_EN_BITS 1
#define SPE_EN_MASK (((1 << 1) - 1) << 0)

#define NEGATIVE_EN_OFFSET 1
#define NEGATIVE_EN_BITS 1
#define NEGATIVE_EN_MASK (((1 << 1) - 1) << 1)

#define MONO_EN_OFFSET 2
#define MONO_EN_BITS 1
#define MONO_EN_MASK (((1 << 1) - 1) << 2)

#define GRAY_EN_OFFSET 3
#define GRAY_EN_BITS 1
#define GRAY_EN_MASK (((1 << 1) - 1) << 3)

/* Bits of SPE_Y_MONO (0x00000008) */

#define MONO_MEAN_Y_OFFSET 0
#define MONO_MEAN_Y_BITS 8
#define MONO_MEAN_Y_MASK (((1 << 8) - 1) << 0)

/* Bits of SPE_TUNE (0x0000000c) */

#define SPE_TUNE_OFFSET 0
#define SPE_TUNE_BITS 6
#define SPE_TUNE_MASK (((1 << 6) - 1) << 0)

/* Bits of SPE_UV_TARGET (0x00000010) */

#define SPE_U_TARGET_OFFSET 0
#define SPE_U_TARGET_BITS 8
#define SPE_U_TARGET_MASK (((1 << 8) - 1) << 0)

#define SPE_V_TARGET_OFFSET 8
#define SPE_V_TARGET_BITS 8
#define SPE_V_TARGET_MASK (((1 << 8) - 1) << 8)

/* Bits of SPE_YCONTRAST (0x00000014) */

#define YCONTRAST_OFFSET 0
#define YCONTRAST_BITS 8
#define YCONTRAST_MASK (((1 << 8) - 1) << 0)

#define YCONTRAST_MEAN_OFFSET 8
#define YCONTRAST_MEAN_BITS 8
#define YCONTRAST_MEAN_MASK (((1 << 8) - 1) << 8)

/* Bits of SPE_Y_OFFSET (0x00000018) */

#define Y_OFFSET_OFFSET 0
#define Y_OFFSET_BITS 8
#define Y_OFFSET_MASK (((1 << 8) - 1) << 0)

/* Bits of SPE_U_OFFSET (0x0000001c) */

#define U_OFFSET_OFFSET 0
#define U_OFFSET_BITS 6
#define U_OFFSET_MASK (((1 << 6) - 1) << 0)

/* Bits of SPE_V_OFFSET (0x00000020) */

#define V_OFFSET_OFFSET 0
#define V_OFFSET_BITS 6
#define V_OFFSET_MASK (((1 << 6) - 1) << 0)

/* Bits of SPE_Y_GAIN (0x00000024) */

#define Y_GAIN_OFFSET 0
#define Y_GAIN_BITS 8
#define Y_GAIN_MASK (((1 << 8) - 1) << 0)

/* Bits of SPE_U_GAIN (0x00000028) */

#define U_GAIN_OFFSET 0
#define U_GAIN_BITS 8
#define U_GAIN_MASK (((1 << 8) - 1) << 0)

/* Bits of SPE_V_GAIN (0x0000002c) */

#define V_GAIN_OFFSET 0
#define V_GAIN_BITS 8
#define V_GAIN_MASK (((1 << 8) - 1) << 0)

/* Bits of SPE_REG_LOAD (0x00000030) */

#define REG_LOAD_OFFSET 0
#define REG_LOAD_BITS 1
#define REG_LOAD_MASK (((1 << 1) - 1) << 0)

#endif /* _SPE_H_INC_ */
