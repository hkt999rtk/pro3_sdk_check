
//tool_version:1.4.35

#ifndef _RGB2YUV_H_INC_
#define _RGB2YUV_H_INC_

#include <isp_reg.h>
#include <isp_reg_base.h>

#define RGB2YUV0 (RGB2YUV_BASE_ADDR + 0x00000000) //0x00004800
#define RGB2YUV1 (RGB2YUV_BASE_ADDR + 0x00000004) //0x00004804
#define RGB2YUV2 (RGB2YUV_BASE_ADDR + 0x00000008) //0x00004808
#define RGB2YUV3 (RGB2YUV_BASE_ADDR + 0x0000000c) //0x0000480c
#define RGB2YUV4 (RGB2YUV_BASE_ADDR + 0x00000010) //0x00004810
#define RGB2YUV_YOFFSET (RGB2YUV_BASE_ADDR + 0x00000014) //0x00004814

/* Bits of RGB2YUV0 (0x00000000) */

#define YBT709_YR_OFFSET 0
#define YBT709_YR_BITS 12
#define YBT709_YR_MASK (((1 << 12) - 1) << 0)

#define YBT709_YG_OFFSET 16
#define YBT709_YG_BITS 12
#define YBT709_YG_MASK (((1 << 12) - 1) << 16)

/* Bits of RGB2YUV1 (0x00000004) */

#define YBT709_YB_OFFSET 0
#define YBT709_YB_BITS 12
#define YBT709_YB_MASK (((1 << 12) - 1) << 0)

#define YBT709_UR_OFFSET 16
#define YBT709_UR_BITS 12
#define YBT709_UR_MASK (((1 << 12) - 1) << 16)

/* Bits of RGB2YUV2 (0x00000008) */

#define YBT709_UG_OFFSET 0
#define YBT709_UG_BITS 12
#define YBT709_UG_MASK (((1 << 12) - 1) << 0)

#define YBT709_UB_OFFSET 16
#define YBT709_UB_BITS 12
#define YBT709_UB_MASK (((1 << 12) - 1) << 16)

/* Bits of RGB2YUV3 (0x0000000c) */

#define YBT709_VR_OFFSET 0
#define YBT709_VR_BITS 12
#define YBT709_VR_MASK (((1 << 12) - 1) << 0)

#define YBT709_VG_OFFSET 16
#define YBT709_VG_BITS 12
#define YBT709_VG_MASK (((1 << 12) - 1) << 16)

/* Bits of RGB2YUV4 (0x00000010) */

#define YBT709_VB_OFFSET 0
#define YBT709_VB_BITS 12
#define YBT709_VB_MASK (((1 << 12) - 1) << 0)

/* Bits of RGB2YUV_YOFFSET (0x00000014) */

#define YBT709_OFFSET_OFFSET 0
#define YBT709_OFFSET_BITS 5
#define YBT709_OFFSET_MASK (((1 << 5) - 1) << 0)

#endif /* _RGB2YUV_H_INC_ */
