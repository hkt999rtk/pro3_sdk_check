
//tool_version:1.4.35

#ifndef _AF1_H_INC_
#define _AF1_H_INC_

#include <isp_reg.h>
#include <isp_reg_base.h>

#define ISP_AF_EN (AF1_BASE_ADDR + 0x00000000) //0x00011000
#define ISP_AF_WIN_START (AF1_BASE_ADDR + 0x00000004) //0x00011004
#define ISP_AF_WIN_SIZE (AF1_BASE_ADDR + 0x00000008) //0x00011008
#define ISP_AF_BLC (AF1_BASE_ADDR + 0x0000000c) //0x0001100c
#define ISP_AF_H1_IIR_PARA_0 (AF1_BASE_ADDR + 0x00000010) //0x00011010
#define ISP_AF_H1_IIR_PARA_1 (AF1_BASE_ADDR + 0x00000014) //0x00011014
#define ISP_AF_H1_IIR_PARA_2 (AF1_BASE_ADDR + 0x00000018) //0x00011018
#define ISP_AF_H1_IIR_SHIFT_0 (AF1_BASE_ADDR + 0x0000001c) //0x0001101c
#define ISP_AF_H1_IIR_SHIFT_1 (AF1_BASE_ADDR + 0x00000020) //0x00011020
#define ISP_AF_H1_LDG_0 (AF1_BASE_ADDR + 0x00000024) //0x00011024
#define ISP_AF_H1_LDG_1 (AF1_BASE_ADDR + 0x00000028) //0x00011028
#define ISP_AF_H1_CORING (AF1_BASE_ADDR + 0x0000002c) //0x0001102c
#define ISP_AF_V1_FIR_PARA_0 (AF1_BASE_ADDR + 0x00000030) //0x00011030
#define ISP_AF_V1_FIR_PARA_1 (AF1_BASE_ADDR + 0x00000034) //0x00011034
#define ISP_AF_V1_LDG_0 (AF1_BASE_ADDR + 0x00000038) //0x00011038
#define ISP_AF_V1_LDG_1 (AF1_BASE_ADDR + 0x0000003c) //0x0001103c
#define ISP_AF_H2_IIR_PARA_0 (AF1_BASE_ADDR + 0x00000040) //0x00011040
#define ISP_AF_H2_IIR_PARA_1 (AF1_BASE_ADDR + 0x00000044) //0x00011044
#define ISP_AF_H2_IIR_PARA_2 (AF1_BASE_ADDR + 0x00000048) //0x00011048
#define ISP_AF_H2_IIR_SHIFT_0 (AF1_BASE_ADDR + 0x0000004c) //0x0001104c
#define ISP_AF_H2_IIR_SHIFT_1 (AF1_BASE_ADDR + 0x00000050) //0x00011050
#define ISP_AF_H2_LDG_0 (AF1_BASE_ADDR + 0x00000054) //0x00011054
#define ISP_AF_H2_LDG_1 (AF1_BASE_ADDR + 0x00000058) //0x00011058
#define ISP_AF_H2_CORING (AF1_BASE_ADDR + 0x0000005c) //0x0001105c
#define ISP_AF_V2_FIR_PARA_0 (AF1_BASE_ADDR + 0x00000060) //0x00011060
#define ISP_AF_V2_FIR_PARA_1 (AF1_BASE_ADDR + 0x00000064) //0x00011064
#define ISP_AF_V2_LDG_0 (AF1_BASE_ADDR + 0x00000068) //0x00011068
#define ISP_AF_V2_LDG_1 (AF1_BASE_ADDR + 0x0000006c) //0x0001106c
#define ISP_AF_CONFIG_0 (AF1_BASE_ADDR + 0x00000070) //0x00011070
#define ISP_AF_BLEND_RATIO (AF1_BASE_ADDR + 0x00000074) //0x00011074
#define ISP_AF_SHIFTER1 (AF1_BASE_ADDR + 0x00000078) //0x00011078
#define ISP_AF_SHIFTER2 (AF1_BASE_ADDR + 0x0000007c) //0x0001107c
#define ISP_AF_V1_CORING (AF1_BASE_ADDR + 0x00000080) //0x00011080
#define ISP_AF_V2_CORING (AF1_BASE_ADDR + 0x00000084) //0x00011084
#define ISP_AF_STATIS_POS (AF1_BASE_ADDR + 0x00000088) //0x00011088
#define ISP_AF_MODE (AF1_BASE_ADDR + 0x0000008c) //0x0001108c
#define ISP_AF_STATUS (AF1_BASE_ADDR + 0x00000090) //0x00011090
#define AF_MEM_RESULT_0_BASE (AF1_BASE_ADDR + 0x00000400) //0x00011400
#define AF_MEM_RESULT_0_TOP (AF1_BASE_ADDR + 0x000007fc) //0x000117fc
#define AF_MEM_RESULT_1_BASE (AF1_BASE_ADDR + 0x00000800) //0x00011800
#define AF_MEM_RESULT_1_TOP (AF1_BASE_ADDR + 0x00000bfc) //0x00011bfc
#define AF_MEM_RESULT_2_BASE (AF1_BASE_ADDR + 0x00000c00) //0x00011c00
#define AF_MEM_RESULT_2_TOP (AF1_BASE_ADDR + 0x00000ffc) //0x00011ffc

/* Bits of ISP_AF_EN (0x00000000) */

#define PRE_FILTER_EN_OFFSET 0
#define PRE_FILTER_EN_BITS 1
#define PRE_FILTER_EN_MASK (((1 << 1) - 1) << 0)

#define GAMMA_EN_OFFSET 1
#define GAMMA_EN_BITS 1
#define GAMMA_EN_MASK (((1 << 1) - 1) << 1)

#define FIR_ROUGH_EN_OFFSET 2
#define FIR_ROUGH_EN_BITS 1
#define FIR_ROUGH_EN_MASK (((1 << 1) - 1) << 2)

#define SQUARE_MODE_EN_OFFSET 3
#define SQUARE_MODE_EN_BITS 1
#define SQUARE_MODE_EN_MASK (((1 << 1) - 1) << 3)

#define PEAK_MODE_EN_OFFSET 4
#define PEAK_MODE_EN_BITS 1
#define PEAK_MODE_EN_MASK (((1 << 1) - 1) << 4)

/* Bits of ISP_AF_WIN_START (0x00000004) */

#define AF_START_X_OFFSET 0
#define AF_START_X_BITS 16
#define AF_START_X_MASK (((1 << 16) - 1) << 0)

#define AF_START_Y_OFFSET 16
#define AF_START_Y_BITS 16
#define AF_START_Y_MASK (((1 << 16) - 1) << 16)

/* Bits of ISP_AF_WIN_SIZE (0x00000008) */

#define WIN_WIDTH_OFFSET 0
#define WIN_WIDTH_BITS 8
#define WIN_WIDTH_MASK (((1 << 8) - 1) << 0)

#define WIN_HEIGHT_OFFSET 8
#define WIN_HEIGHT_BITS 8
#define WIN_HEIGHT_MASK (((1 << 8) - 1) << 8)

#define WIN_NUM_H_OFFSET 16
#define WIN_NUM_H_BITS 5
#define WIN_NUM_H_MASK (((1 << 5) - 1) << 16)

#define WIN_NUM_V_OFFSET 24
#define WIN_NUM_V_BITS 5
#define WIN_NUM_V_MASK (((1 << 5) - 1) << 24)

/* Bits of ISP_AF_BLC (0x0000000c) */

#define BLC_GB_OFFSET_OFFSET 0
#define BLC_GB_OFFSET_BITS 10
#define BLC_GB_OFFSET_MASK (((1 << 10) - 1) << 0)

#define BLC_GR_OFFSET_OFFSET 16
#define BLC_GR_OFFSET_BITS 10
#define BLC_GR_OFFSET_MASK (((1 << 10) - 1) << 16)

#define BLC_EN_OFFSET 31
#define BLC_EN_BITS 1
#define BLC_EN_MASK (((1 << 1) - 1) << 31)

/* Bits of ISP_AF_H1_IIR_PARA_0 (0x00000010) */

#define H1_IIR_PARA_1_OFFSET 0
#define H1_IIR_PARA_1_BITS 10
#define H1_IIR_PARA_1_MASK (((1 << 10) - 1) << 0)

#define H1_IIR_PARA_2_OFFSET 10
#define H1_IIR_PARA_2_BITS 10
#define H1_IIR_PARA_2_MASK (((1 << 10) - 1) << 10)

#define H1_IIR_PARA_3_OFFSET 20
#define H1_IIR_PARA_3_BITS 10
#define H1_IIR_PARA_3_MASK (((1 << 10) - 1) << 20)

/* Bits of ISP_AF_H1_IIR_PARA_1 (0x00000014) */

#define H1_IIR_PARA_4_OFFSET 0
#define H1_IIR_PARA_4_BITS 10
#define H1_IIR_PARA_4_MASK (((1 << 10) - 1) << 0)

#define H1_IIR_PARA_5_OFFSET 10
#define H1_IIR_PARA_5_BITS 10
#define H1_IIR_PARA_5_MASK (((1 << 10) - 1) << 10)

#define H1_IIR_PARA_6_OFFSET 20
#define H1_IIR_PARA_6_BITS 10
#define H1_IIR_PARA_6_MASK (((1 << 10) - 1) << 20)

/* Bits of ISP_AF_H1_IIR_PARA_2 (0x00000018) */

#define H1_IIR_PARA_0_OFFSET 0
#define H1_IIR_PARA_0_BITS 10
#define H1_IIR_PARA_0_MASK (((1 << 10) - 1) << 0)

/* Bits of ISP_AF_H1_IIR_SHIFT_0 (0x0000001c) */

#define H1_IIR_SHIFT_0_OFFSET 0
#define H1_IIR_SHIFT_0_BITS 4
#define H1_IIR_SHIFT_0_MASK (((1 << 4) - 1) << 0)

#define H1_IIR_SHIFT_1_OFFSET 8
#define H1_IIR_SHIFT_1_BITS 4
#define H1_IIR_SHIFT_1_MASK (((1 << 4) - 1) << 8)

#define H1_IIR_SHIFT_2_OFFSET 16
#define H1_IIR_SHIFT_2_BITS 4
#define H1_IIR_SHIFT_2_MASK (((1 << 4) - 1) << 16)

#define H1_IIR_SHIFT_3_OFFSET 24
#define H1_IIR_SHIFT_3_BITS 4
#define H1_IIR_SHIFT_3_MASK (((1 << 4) - 1) << 24)

/* Bits of ISP_AF_H1_IIR_SHIFT_1 (0x00000020) */

#define H1_IIR_SHIFT_4_OFFSET 0
#define H1_IIR_SHIFT_4_BITS 2
#define H1_IIR_SHIFT_4_MASK (((1 << 2) - 1) << 0)

/* Bits of ISP_AF_H1_LDG_0 (0x00000024) */

#define H1_LDG_LOW_TH_OFFSET 0
#define H1_LDG_LOW_TH_BITS 8
#define H1_LDG_LOW_TH_MASK (((1 << 8) - 1) << 0)

#define H1_LDG_LOW_GAIN_OFFSET 8
#define H1_LDG_LOW_GAIN_BITS 8
#define H1_LDG_LOW_GAIN_MASK (((1 << 8) - 1) << 8)

#define H1_LDG_SLOPE1_OFFSET 16
#define H1_LDG_SLOPE1_BITS 8
#define H1_LDG_SLOPE1_MASK (((1 << 8) - 1) << 16)

#define H1_LDG_EN_OFFSET 24
#define H1_LDG_EN_BITS 1
#define H1_LDG_EN_MASK (((1 << 1) - 1) << 24)

/* Bits of ISP_AF_H1_LDG_1 (0x00000028) */

#define H1_LDG_SLOPE2_OFFSET 0
#define H1_LDG_SLOPE2_BITS 8
#define H1_LDG_SLOPE2_MASK (((1 << 8) - 1) << 0)

#define H1_LDG_HIGH_GAIN_OFFSET 8
#define H1_LDG_HIGH_GAIN_BITS 8
#define H1_LDG_HIGH_GAIN_MASK (((1 << 8) - 1) << 8)

#define H1_LDG_HIGH_TH_OFFSET 16
#define H1_LDG_HIGH_TH_BITS 8
#define H1_LDG_HIGH_TH_MASK (((1 << 8) - 1) << 16)

/* Bits of ISP_AF_H1_CORING (0x0000002c) */

#define H1_CORING_LOW_TH_OFFSET 0
#define H1_CORING_LOW_TH_BITS 12
#define H1_CORING_LOW_TH_MASK (((1 << 12) - 1) << 0)

#define H1_CORING_HIGH_TH_OFFSET 12
#define H1_CORING_HIGH_TH_BITS 12
#define H1_CORING_HIGH_TH_MASK (((1 << 12) - 1) << 12)

#define H1_CORING_SLOPE_OFFSET 24
#define H1_CORING_SLOPE_BITS 8
#define H1_CORING_SLOPE_MASK (((1 << 8) - 1) << 24)

/* Bits of ISP_AF_V1_FIR_PARA_0 (0x00000030) */

#define V1_FIR_PARA_0_OFFSET 0
#define V1_FIR_PARA_0_BITS 8
#define V1_FIR_PARA_0_MASK (((1 << 8) - 1) << 0)

#define V1_FIR_PARA_1_OFFSET 8
#define V1_FIR_PARA_1_BITS 8
#define V1_FIR_PARA_1_MASK (((1 << 8) - 1) << 8)

#define V1_FIR_PARA_2_OFFSET 16
#define V1_FIR_PARA_2_BITS 8
#define V1_FIR_PARA_2_MASK (((1 << 8) - 1) << 16)

#define V1_FIR_PARA_3_OFFSET 24
#define V1_FIR_PARA_3_BITS 8
#define V1_FIR_PARA_3_MASK (((1 << 8) - 1) << 24)

/* Bits of ISP_AF_V1_FIR_PARA_1 (0x00000034) */

#define V1_FIR_PARA_4_OFFSET 0
#define V1_FIR_PARA_4_BITS 8
#define V1_FIR_PARA_4_MASK (((1 << 8) - 1) << 0)

/* Bits of ISP_AF_V1_LDG_0 (0x00000038) */

#define V1_LDG_LOW_TH_OFFSET 0
#define V1_LDG_LOW_TH_BITS 8
#define V1_LDG_LOW_TH_MASK (((1 << 8) - 1) << 0)

#define V1_LDG_LOW_GAIN_OFFSET 8
#define V1_LDG_LOW_GAIN_BITS 8
#define V1_LDG_LOW_GAIN_MASK (((1 << 8) - 1) << 8)

#define V1_LDG_SLOPE1_OFFSET 16
#define V1_LDG_SLOPE1_BITS 8
#define V1_LDG_SLOPE1_MASK (((1 << 8) - 1) << 16)

#define V1_LDG_EN_OFFSET 24
#define V1_LDG_EN_BITS 1
#define V1_LDG_EN_MASK (((1 << 1) - 1) << 24)

/* Bits of ISP_AF_V1_LDG_1 (0x0000003c) */

#define V1_LDG_SLOPE2_OFFSET 0
#define V1_LDG_SLOPE2_BITS 8
#define V1_LDG_SLOPE2_MASK (((1 << 8) - 1) << 0)

#define V1_LDG_HIGH_GAIN_OFFSET 8
#define V1_LDG_HIGH_GAIN_BITS 8
#define V1_LDG_HIGH_GAIN_MASK (((1 << 8) - 1) << 8)

#define V1_LDG_HIGH_TH_OFFSET 16
#define V1_LDG_HIGH_TH_BITS 8
#define V1_LDG_HIGH_TH_MASK (((1 << 8) - 1) << 16)

/* Bits of ISP_AF_H2_IIR_PARA_0 (0x00000040) */

#define H2_IIR_PARA_1_OFFSET 0
#define H2_IIR_PARA_1_BITS 10
#define H2_IIR_PARA_1_MASK (((1 << 10) - 1) << 0)

#define H2_IIR_PARA_2_OFFSET 10
#define H2_IIR_PARA_2_BITS 10
#define H2_IIR_PARA_2_MASK (((1 << 10) - 1) << 10)

#define H2_IIR_PARA_3_OFFSET 20
#define H2_IIR_PARA_3_BITS 10
#define H2_IIR_PARA_3_MASK (((1 << 10) - 1) << 20)

/* Bits of ISP_AF_H2_IIR_PARA_1 (0x00000044) */

#define H2_IIR_PARA_4_OFFSET 0
#define H2_IIR_PARA_4_BITS 10
#define H2_IIR_PARA_4_MASK (((1 << 10) - 1) << 0)

#define H2_IIR_PARA_5_OFFSET 10
#define H2_IIR_PARA_5_BITS 10
#define H2_IIR_PARA_5_MASK (((1 << 10) - 1) << 10)

#define H2_IIR_PARA_6_OFFSET 20
#define H2_IIR_PARA_6_BITS 10
#define H2_IIR_PARA_6_MASK (((1 << 10) - 1) << 20)

/* Bits of ISP_AF_H2_IIR_PARA_2 (0x00000048) */

#define H2_IIR_PARA_0_OFFSET 0
#define H2_IIR_PARA_0_BITS 10
#define H2_IIR_PARA_0_MASK (((1 << 10) - 1) << 0)

/* Bits of ISP_AF_H2_IIR_SHIFT_0 (0x0000004c) */

#define H2_IIR_SHIFT_0_OFFSET 0
#define H2_IIR_SHIFT_0_BITS 4
#define H2_IIR_SHIFT_0_MASK (((1 << 4) - 1) << 0)

#define H2_IIR_SHIFT_1_OFFSET 8
#define H2_IIR_SHIFT_1_BITS 4
#define H2_IIR_SHIFT_1_MASK (((1 << 4) - 1) << 8)

#define H2_IIR_SHIFT_2_OFFSET 16
#define H2_IIR_SHIFT_2_BITS 4
#define H2_IIR_SHIFT_2_MASK (((1 << 4) - 1) << 16)

#define H2_IIR_SHIFT_3_OFFSET 24
#define H2_IIR_SHIFT_3_BITS 4
#define H2_IIR_SHIFT_3_MASK (((1 << 4) - 1) << 24)

/* Bits of ISP_AF_H2_IIR_SHIFT_1 (0x00000050) */

#define H2_IIR_SHIFT_4_OFFSET 0
#define H2_IIR_SHIFT_4_BITS 2
#define H2_IIR_SHIFT_4_MASK (((1 << 2) - 1) << 0)

/* Bits of ISP_AF_H2_LDG_0 (0x00000054) */

#define H2_LDG_LOW_TH_OFFSET 0
#define H2_LDG_LOW_TH_BITS 8
#define H2_LDG_LOW_TH_MASK (((1 << 8) - 1) << 0)

#define H2_LDG_LOW_GAIN_OFFSET 8
#define H2_LDG_LOW_GAIN_BITS 8
#define H2_LDG_LOW_GAIN_MASK (((1 << 8) - 1) << 8)

#define H2_LDG_SLOPE1_OFFSET 16
#define H2_LDG_SLOPE1_BITS 8
#define H2_LDG_SLOPE1_MASK (((1 << 8) - 1) << 16)

#define H2_LDG_EN_OFFSET 24
#define H2_LDG_EN_BITS 1
#define H2_LDG_EN_MASK (((1 << 1) - 1) << 24)

/* Bits of ISP_AF_H2_LDG_1 (0x00000058) */

#define H2_LDG_SLOPE2_OFFSET 0
#define H2_LDG_SLOPE2_BITS 8
#define H2_LDG_SLOPE2_MASK (((1 << 8) - 1) << 0)

#define H2_LDG_HIGH_GAIN_OFFSET 8
#define H2_LDG_HIGH_GAIN_BITS 8
#define H2_LDG_HIGH_GAIN_MASK (((1 << 8) - 1) << 8)

#define H2_LDG_HIGH_TH_OFFSET 16
#define H2_LDG_HIGH_TH_BITS 8
#define H2_LDG_HIGH_TH_MASK (((1 << 8) - 1) << 16)

/* Bits of ISP_AF_H2_CORING (0x0000005c) */

#define H2_CORING_LOW_TH_OFFSET 0
#define H2_CORING_LOW_TH_BITS 12
#define H2_CORING_LOW_TH_MASK (((1 << 12) - 1) << 0)

#define H2_CORING_HIGH_TH_OFFSET 12
#define H2_CORING_HIGH_TH_BITS 12
#define H2_CORING_HIGH_TH_MASK (((1 << 12) - 1) << 12)

#define H2_CORING_SLOPE_OFFSET 24
#define H2_CORING_SLOPE_BITS 8
#define H2_CORING_SLOPE_MASK (((1 << 8) - 1) << 24)

/* Bits of ISP_AF_V2_FIR_PARA_0 (0x00000060) */

#define V2_FIR_PARA_0_OFFSET 0
#define V2_FIR_PARA_0_BITS 8
#define V2_FIR_PARA_0_MASK (((1 << 8) - 1) << 0)

#define V2_FIR_PARA_1_OFFSET 8
#define V2_FIR_PARA_1_BITS 8
#define V2_FIR_PARA_1_MASK (((1 << 8) - 1) << 8)

#define V2_FIR_PARA_2_OFFSET 16
#define V2_FIR_PARA_2_BITS 8
#define V2_FIR_PARA_2_MASK (((1 << 8) - 1) << 16)

#define V2_FIR_PARA_3_OFFSET 24
#define V2_FIR_PARA_3_BITS 8
#define V2_FIR_PARA_3_MASK (((1 << 8) - 1) << 24)

/* Bits of ISP_AF_V2_FIR_PARA_1 (0x00000064) */

#define V2_FIR_PARA_4_OFFSET 0
#define V2_FIR_PARA_4_BITS 8
#define V2_FIR_PARA_4_MASK (((1 << 8) - 1) << 0)

/* Bits of ISP_AF_V2_LDG_0 (0x00000068) */

#define V2_LDG_LOW_TH_OFFSET 0
#define V2_LDG_LOW_TH_BITS 8
#define V2_LDG_LOW_TH_MASK (((1 << 8) - 1) << 0)

#define V2_LDG_LOW_GAIN_OFFSET 8
#define V2_LDG_LOW_GAIN_BITS 8
#define V2_LDG_LOW_GAIN_MASK (((1 << 8) - 1) << 8)

#define V2_LDG_SLOPE1_OFFSET 16
#define V2_LDG_SLOPE1_BITS 8
#define V2_LDG_SLOPE1_MASK (((1 << 8) - 1) << 16)

#define V2_LDG_EN_OFFSET 24
#define V2_LDG_EN_BITS 1
#define V2_LDG_EN_MASK (((1 << 1) - 1) << 24)

/* Bits of ISP_AF_V2_LDG_1 (0x0000006c) */

#define V2_LDG_SLOPE2_OFFSET 0
#define V2_LDG_SLOPE2_BITS 8
#define V2_LDG_SLOPE2_MASK (((1 << 8) - 1) << 0)

#define V2_LDG_HIGH_GAIN_OFFSET 8
#define V2_LDG_HIGH_GAIN_BITS 8
#define V2_LDG_HIGH_GAIN_MASK (((1 << 8) - 1) << 8)

#define V2_LDG_HIGH_TH_OFFSET 16
#define V2_LDG_HIGH_TH_BITS 8
#define V2_LDG_HIGH_TH_MASK (((1 << 8) - 1) << 16)

/* Bits of ISP_AF_CONFIG_0 (0x00000070) */

#define HIGH_ILLUM_TH_OFFSET 0
#define HIGH_ILLUM_TH_BITS 12
#define HIGH_ILLUM_TH_MASK (((1 << 12) - 1) << 0)

/* Bits of ISP_AF_BLEND_RATIO (0x00000074) */

#define BLEND_RATIO1_OFFSET 0
#define BLEND_RATIO1_BITS 9
#define BLEND_RATIO1_MASK (((1 << 9) - 1) << 0)

#define BLEND_RATIO2_OFFSET 16
#define BLEND_RATIO2_BITS 9
#define BLEND_RATIO2_MASK (((1 << 9) - 1) << 16)

/* Bits of ISP_AF_SHIFTER1 (0x00000078) */

#define FV_SHIFTE1_0_OFFSET 0
#define FV_SHIFTE1_0_BITS 5
#define FV_SHIFTE1_0_MASK (((1 << 5) - 1) << 0)

#define FV_SHIFTE1_1_OFFSET 8
#define FV_SHIFTE1_1_BITS 5
#define FV_SHIFTE1_1_MASK (((1 << 5) - 1) << 8)

/* Bits of ISP_AF_SHIFTER2 (0x0000007c) */

#define FV_SHIFTE2_0_OFFSET 0
#define FV_SHIFTE2_0_BITS 5
#define FV_SHIFTE2_0_MASK (((1 << 5) - 1) << 0)

#define FV_SHIFTE2_1_OFFSET 8
#define FV_SHIFTE2_1_BITS 5
#define FV_SHIFTE2_1_MASK (((1 << 5) - 1) << 8)

/* Bits of ISP_AF_V1_CORING (0x00000080) */

#define V1_CORING_LOW_TH_OFFSET 0
#define V1_CORING_LOW_TH_BITS 12
#define V1_CORING_LOW_TH_MASK (((1 << 12) - 1) << 0)

#define V1_CORING_HIGH_TH_OFFSET 12
#define V1_CORING_HIGH_TH_BITS 12
#define V1_CORING_HIGH_TH_MASK (((1 << 12) - 1) << 12)

#define V1_CORING_SLOPE_OFFSET 24
#define V1_CORING_SLOPE_BITS 8
#define V1_CORING_SLOPE_MASK (((1 << 8) - 1) << 24)

/* Bits of ISP_AF_V2_CORING (0x00000084) */

#define V2_CORING_LOW_TH_OFFSET 0
#define V2_CORING_LOW_TH_BITS 12
#define V2_CORING_LOW_TH_MASK (((1 << 12) - 1) << 0)

#define V2_CORING_HIGH_TH_OFFSET 12
#define V2_CORING_HIGH_TH_BITS 12
#define V2_CORING_HIGH_TH_MASK (((1 << 12) - 1) << 12)

#define V2_CORING_SLOPE_OFFSET 24
#define V2_CORING_SLOPE_BITS 8
#define V2_CORING_SLOPE_MASK (((1 << 8) - 1) << 24)

/* Bits of ISP_AF_STATIS_POS (0x00000088) */

#define AF_STATIS_POS_OFFSET 0
#define AF_STATIS_POS_BITS 2
#define AF_STATIS_POS_MASK (((1 << 2) - 1) << 0)

/* Bits of ISP_AF_MODE (0x0000008c) */

#define AF_MODE_OFFSET 0
#define AF_MODE_BITS 2
#define AF_MODE_MASK (((1 << 2) - 1) << 0)

/* Bits of ISP_AF_STATUS (0x00000090) */

#define AF_START_OFFSET 0
#define AF_START_BITS 1
#define AF_START_MASK (((1 << 1) - 1) << 0)

#define AF_STOP_OFFSET 8
#define AF_STOP_BITS 1
#define AF_STOP_MASK (((1 << 1) - 1) << 8)

#define AF_GOING_OFFSET 16
#define AF_GOING_BITS 1
#define AF_GOING_MASK (((1 << 1) - 1) << 16)

/* Bits of AF_MEM_RESULT_0 (0x00000400~0x000007fc) */

#define HIGH_ILLUM_OFFSET 0
#define HIGH_ILLUM_BITS 17
#define HIGH_ILLUM_MASK (((1 << 17) - 1) << 0)

/* Bits of AF_MEM_RESULT_1 (0x00000800~0x00000bfc) */

#define FOCUS1_OFFSET 0
#define FOCUS1_BITS 32
#define FOCUS1_MASK (((1 << 32) - 1) << 0)

/* Bits of AF_MEM_RESULT_2 (0x00000c00~0x00000ffc) */

#define FOCUS2_OFFSET 0
#define FOCUS2_BITS 32
#define FOCUS2_MASK (((1 << 32) - 1) << 0)

#endif /* _AF1_H_INC_ */
