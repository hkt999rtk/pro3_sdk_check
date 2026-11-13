
//tool_version:1.4.35

#ifndef _DPC_L_H_INC_
#define _DPC_L_H_INC_

#include <isp_reg.h>
#include <isp_reg_base.h>

#define DPC_ENABLE (DPC_L_BASE_ADDR + 0x0000) //0x9400
#define DPC_THD0 (DPC_L_BASE_ADDR + 0x0004) //0x9404
#define DPC_RATE (DPC_L_BASE_ADDR + 0x0008) //0x9408
#define DPC_THD1 (DPC_L_BASE_ADDR + 0x000c) //0x940c
#define DPC_BL_OFFSET_12BIT_CH_0 (DPC_L_BASE_ADDR + 0x0010) //0x9410
#define DPC_BL_OFFSET_12BIT_CH_1 (DPC_L_BASE_ADDR + 0x0014) //0x9414
#define DPC_BL_OFFSET_GAIN_12BIT_CH_0 (DPC_L_BASE_ADDR + 0x0018) //0x9418
#define DPC_BL_OFFSET_GAIN_12BIT_CH_1 (DPC_L_BASE_ADDR + 0x001c) //0x941c
#define DPC_REGLOAD (DPC_L_BASE_ADDR + 0x0020) //0x9420

/* Bits of DPC_ENABLE (0x0000) */

#define DDP2_EN_OFFSET 0
#define DDP2_EN_BITS 1
#define DDP2_EN_MASK (((1 << 1) - 1) << 0)

#define DP_RP_EN_OFFSET 1
#define DP_RP_EN_BITS 1
#define DP_RP_EN_MASK (((1 << 1) - 1) << 1)

#define DPC_SRC_SEL_OFFSET 2
#define DPC_SRC_SEL_BITS 1
#define DPC_SRC_SEL_MASK (((1 << 1) - 1) << 2)

#define DP_GROUP_SQUARE_OFFSET 3
#define DP_GROUP_SQUARE_BITS 1
#define DP_GROUP_SQUARE_MASK (((1 << 1) - 1) << 3)

#define DPC_DEBUG_MODE_OFFSET 4
#define DPC_DEBUG_MODE_BITS 2
#define DPC_DEBUG_MODE_MASK (((1 << 2) - 1) << 4)

#define CLIP_MODE_OFFSET 6
#define CLIP_MODE_BITS 1
#define CLIP_MODE_MASK (((1 << 1) - 1) << 6)

/* Bits of DPC_THD0 (0x0004) */

#define DP_BRIGHT_THD_MIN_OFFSET 0
#define DP_BRIGHT_THD_MIN_BITS 4
#define DP_BRIGHT_THD_MIN_MASK (((1 << 4) - 1) << 0)

#define DP_BRIGHT_THD_MAX_OFFSET 8
#define DP_BRIGHT_THD_MAX_BITS 5
#define DP_BRIGHT_THD_MAX_MASK (((1 << 5) - 1) << 8)

#define DP_DARK_THD_MIN_OFFSET 16
#define DP_DARK_THD_MIN_BITS 4
#define DP_DARK_THD_MIN_MASK (((1 << 4) - 1) << 16)

#define DP_DARK_THD_MAX_OFFSET 24
#define DP_DARK_THD_MAX_BITS 5
#define DP_DARK_THD_MAX_MASK (((1 << 5) - 1) << 24)

/* Bits of DPC_RATE (0x0008) */

#define G_DP_BRIGHT_RATE_OFFSET 0
#define G_DP_BRIGHT_RATE_BITS 5
#define G_DP_BRIGHT_RATE_MASK (((1 << 5) - 1) << 0)

#define G_DP_DARK_RATE_OFFSET 8
#define G_DP_DARK_RATE_BITS 5
#define G_DP_DARK_RATE_MASK (((1 << 5) - 1) << 8)

#define RB_DP_BRIGHT_RATE_OFFSET 16
#define RB_DP_BRIGHT_RATE_BITS 5
#define RB_DP_BRIGHT_RATE_MASK (((1 << 5) - 1) << 16)

#define RB_DP_DARK_RATE_OFFSET 24
#define RB_DP_DARK_RATE_BITS 5
#define RB_DP_DARK_RATE_MASK (((1 << 5) - 1) << 24)

/* Bits of DPC_THD1 (0x000c) */

#define DP_THD_D1_OFFSET 0
#define DP_THD_D1_BITS 4
#define DP_THD_D1_MASK (((1 << 4) - 1) << 0)

/* Bits of DPC_BL_OFFSET_12BIT_CH_0 (0x0010) */

#define BL_OFFSET_12BIT_CH_0_OFFSET 0
#define BL_OFFSET_12BIT_CH_0_BITS 10
#define BL_OFFSET_12BIT_CH_0_MASK (((1 << 10) - 1) << 0)

#define BL_OFFSET_12BIT_CH_1_OFFSET 16
#define BL_OFFSET_12BIT_CH_1_BITS 10
#define BL_OFFSET_12BIT_CH_1_MASK (((1 << 10) - 1) << 16)

/* Bits of DPC_BL_OFFSET_12BIT_CH_1 (0x0014) */

#define BL_OFFSET_12BIT_CH_2_OFFSET 0
#define BL_OFFSET_12BIT_CH_2_BITS 10
#define BL_OFFSET_12BIT_CH_2_MASK (((1 << 10) - 1) << 0)

#define BL_OFFSET_12BIT_CH_3_OFFSET 16
#define BL_OFFSET_12BIT_CH_3_BITS 10
#define BL_OFFSET_12BIT_CH_3_MASK (((1 << 10) - 1) << 16)

/* Bits of DPC_BL_OFFSET_GAIN_12BIT_CH_0 (0x0018) */

#define BL_OFFSET_GAIN_12BIT_CH_0_OFFSET 0
#define BL_OFFSET_GAIN_12BIT_CH_0_BITS 12
#define BL_OFFSET_GAIN_12BIT_CH_0_MASK (((1 << 12) - 1) << 0)

#define BL_OFFSET_GAIN_12BIT_CH_1_OFFSET 16
#define BL_OFFSET_GAIN_12BIT_CH_1_BITS 12
#define BL_OFFSET_GAIN_12BIT_CH_1_MASK (((1 << 12) - 1) << 16)

/* Bits of DPC_BL_OFFSET_GAIN_12BIT_CH_1 (0x001c) */

#define BL_OFFSET_GAIN_12BIT_CH_2_OFFSET 0
#define BL_OFFSET_GAIN_12BIT_CH_2_BITS 12
#define BL_OFFSET_GAIN_12BIT_CH_2_MASK (((1 << 12) - 1) << 0)

#define BL_OFFSET_GAIN_12BIT_CH_3_OFFSET 16
#define BL_OFFSET_GAIN_12BIT_CH_3_BITS 12
#define BL_OFFSET_GAIN_12BIT_CH_3_MASK (((1 << 12) - 1) << 16)

/* Bits of DPC_REGLOAD (0x0020) */

#define REG_LOAD_OFFSET 0
#define REG_LOAD_BITS 1
#define REG_LOAD_MASK (((1 << 1) - 1) << 0)

#endif /* _DPC_L_H_INC_ */
