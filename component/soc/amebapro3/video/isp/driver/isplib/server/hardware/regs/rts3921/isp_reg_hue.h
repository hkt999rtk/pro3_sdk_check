
//tool_version:1.4.35

#ifndef _HUE_H_INC_
#define _HUE_H_INC_

#include <isp_reg.h>
#include <isp_reg_base.h>

#define HUE_PARAM (HUE_BASE_ADDR + 0x00000000) //0x00007c00
#define HUE_PARAM_SYNC (HUE_BASE_ADDR + 0x00000004) //0x00007c04

/* Bits of HUE_PARAM (0x00000000) */

#define HUE_SIN_X_OFFSET 0
#define HUE_SIN_X_BITS 8
#define HUE_SIN_X_MASK (((1 << 8) - 1) << 0)

#define HUE_COS_X_OFFSET 8
#define HUE_COS_X_BITS 8
#define HUE_COS_X_MASK (((1 << 8) - 1) << 8)

#define HUE_SIN_Y_OFFSET 16
#define HUE_SIN_Y_BITS 8
#define HUE_SIN_Y_MASK (((1 << 8) - 1) << 16)

#define HUE_COS_Y_OFFSET 24
#define HUE_COS_Y_BITS 8
#define HUE_COS_Y_MASK (((1 << 8) - 1) << 24)

/* Bits of HUE_PARAM_SYNC (0x00000004) */

#define HUE_SIN_X_SYNC_OFFSET 0
#define HUE_SIN_X_SYNC_BITS 8
#define HUE_SIN_X_SYNC_MASK (((1 << 8) - 1) << 0)

#define HUE_COS_X_SYNC_OFFSET 8
#define HUE_COS_X_SYNC_BITS 8
#define HUE_COS_X_SYNC_MASK (((1 << 8) - 1) << 8)

#define HUE_SIN_Y_SYNC_OFFSET 16
#define HUE_SIN_Y_SYNC_BITS 8
#define HUE_SIN_Y_SYNC_MASK (((1 << 8) - 1) << 16)

#define HUE_COS_Y_SYNC_OFFSET 24
#define HUE_COS_Y_SYNC_BITS 8
#define HUE_COS_Y_SYNC_MASK (((1 << 8) - 1) << 24)

#endif /* _HUE_H_INC_ */
