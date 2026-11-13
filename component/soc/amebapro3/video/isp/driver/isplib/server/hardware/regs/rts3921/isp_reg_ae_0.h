
//tool_version:1.4.35

#ifndef _AE_0_H_INC_
#define _AE_0_H_INC_

#include <isp_reg.h>
#include <isp_reg_base.h>

#define AE_ROI_REG_LOAD (AE_0_BASE_ADDR + 0x00000000) //0x00016000
#define AE_DYN_CTRL (AE_0_BASE_ADDR + 0x00000004) //0x00016004
#define AE_DYN_WIN_START (AE_0_BASE_ADDR + 0x00000008) //0x00016008
#define AE_DYN_WIN_SIZE (AE_0_BASE_ADDR + 0x0000000c) //0x0001600c
#define AE_DYN_WIN_CAL_NUM (AE_0_BASE_ADDR + 0x00000010) //0x00016010
#define AE_DYN_HWIN_START (AE_0_BASE_ADDR + 0x00000014) //0x00016014
#define AE_DYN_HWIN_END (AE_0_BASE_ADDR + 0x00000018) //0x00016018
#define AE_DYN_BRIGHT_COEF (AE_0_BASE_ADDR + 0x0000001c) //0x0001601c
#define AE_DYN_Y_THD0 (AE_0_BASE_ADDR + 0x00000020) //0x00016020
#define AE_DYN_Y_THD1 (AE_0_BASE_ADDR + 0x00000024) //0x00016024
#define AE_DYN_R_THD0 (AE_0_BASE_ADDR + 0x00000028) //0x00016028
#define AE_DYN_R_THD1 (AE_0_BASE_ADDR + 0x0000002c) //0x0001602c
#define AE_DYN_GR_THD0 (AE_0_BASE_ADDR + 0x00000030) //0x00016030
#define AE_DYN_GR_THD1 (AE_0_BASE_ADDR + 0x00000034) //0x00016034
#define AE_DYN_GB_THD0 (AE_0_BASE_ADDR + 0x00000038) //0x00016038
#define AE_DYN_GB_THD1 (AE_0_BASE_ADDR + 0x0000003c) //0x0001603c
#define AE_DYN_B_THD0 (AE_0_BASE_ADDR + 0x00000040) //0x00016040
#define AE_DYN_B_THD1 (AE_0_BASE_ADDR + 0x00000044) //0x00016044
#define AE_DYN_SUM_MAX_Y (AE_0_BASE_ADDR + 0x00000048) //0x00016048
#define AE_DYN_SUM_MIN_Y (AE_0_BASE_ADDR + 0x0000004c) //0x0001604c
#define AE_DYN_SUM_MAX_R (AE_0_BASE_ADDR + 0x00000050) //0x00016050
#define AE_DYN_SUM_MIN_R (AE_0_BASE_ADDR + 0x00000054) //0x00016054
#define AE_DYN_SUM_MAX_GR (AE_0_BASE_ADDR + 0x00000058) //0x00016058
#define AE_DYN_SUM_MIN_GR (AE_0_BASE_ADDR + 0x0000005c) //0x0001605c
#define AE_DYN_SUM_MAX_GB (AE_0_BASE_ADDR + 0x00000060) //0x00016060
#define AE_DYN_SUM_MIN_GB (AE_0_BASE_ADDR + 0x00000064) //0x00016064
#define AE_DYN_SUM_MAX_B (AE_0_BASE_ADDR + 0x00000068) //0x00016068
#define AE_DYN_SUM_MIN_B (AE_0_BASE_ADDR + 0x0000006c) //0x0001606c
#define AE_DYN_YWIN_START (AE_0_BASE_ADDR + 0x00000070) //0x00016070
#define AE_DYN_YWIN_END (AE_0_BASE_ADDR + 0x00000074) //0x00016074
#define AE_DYN_YSUM (AE_0_BASE_ADDR + 0x00000078) //0x00016078
#define AE_DYN_GAIN (AE_0_BASE_ADDR + 0x0000007c) //0x0001607c
#define AE_DYN_RECT_START (AE_0_BASE_ADDR + 0x00000080) //0x00016080
#define AE_DYN_RECT_SIZE (AE_0_BASE_ADDR + 0x00000084) //0x00016084
#define AE_DYN_RECT_CALC_NUM (AE_0_BASE_ADDR + 0x00000088) //0x00016088
#define AE_DYN_RECT_RESULT (AE_0_BASE_ADDR + 0x0000008c) //0x0001608c
#define AE_DYN_HISTO_MEM_BASE (AE_0_BASE_ADDR + 0x00000100) //0x00016100
#define AE_DYN_HISTO_MEM_TOP (AE_0_BASE_ADDR + 0x000010fc) //0x000170fc
#define AE_DYN_WIN_MEM_BASE (AE_0_BASE_ADDR + 0x00001100) //0x00017100
#define AE_DYN_WIN_MEM_TOP (AE_0_BASE_ADDR + 0x000014fc) //0x000174fc

/* Bits of AE_ROI_REG_LOAD (0x00000000) */

#define ROI_REG_LOAD_OFFSET 0
#define ROI_REG_LOAD_BITS 1
#define ROI_REG_LOAD_MASK (((1 << 1) - 1) << 0)

/* Bits of AE_DYN_CTRL (0x00000004) */

#define AE_MODE_OFFSET 0
#define AE_MODE_BITS 2
#define AE_MODE_MASK (((1 << 2) - 1) << 0)

#define AE_DLY_OFFSET 2
#define AE_DLY_BITS 2
#define AE_DLY_MASK (((1 << 2) - 1) << 2)

#define WIN_STATS_MODE_OFFSET 4
#define WIN_STATS_MODE_BITS 1
#define WIN_STATS_MODE_MASK (((1 << 1) - 1) << 4)

#define AE_GOING_OFFSET 5
#define AE_GOING_BITS 1
#define AE_GOING_MASK (((1 << 1) - 1) << 5)

#define AE_STOP_OFFSET 6
#define AE_STOP_BITS 1
#define AE_STOP_MASK (((1 << 1) - 1) << 6)

#define AE_START_OFFSET 7
#define AE_START_BITS 1
#define AE_START_MASK (((1 << 1) - 1) << 7)

#define AE_IN_SEL_OFFSET 8
#define AE_IN_SEL_BITS 2
#define AE_IN_SEL_MASK (((1 << 2) - 1) << 8)

#define AE_GAIN_POS_OFFSET 10
#define AE_GAIN_POS_BITS 1
#define AE_GAIN_POS_MASK (((1 << 1) - 1) << 10)

#define AE_YSUM_EN_OFFSET 11
#define AE_YSUM_EN_BITS 1
#define AE_YSUM_EN_MASK (((1 << 1) - 1) << 11)

#define AE_RECT_EN_OFFSET 12
#define AE_RECT_EN_BITS 1
#define AE_RECT_EN_MASK (((1 << 1) - 1) << 12)

/* Bits of AE_DYN_WIN_START (0x00000008) */

#define AE_START_X_OFFSET 0
#define AE_START_X_BITS 16
#define AE_START_X_MASK (((1 << 16) - 1) << 0)

#define AE_START_Y_OFFSET 16
#define AE_START_Y_BITS 16
#define AE_START_Y_MASK (((1 << 16) - 1) << 16)

/* Bits of AE_DYN_WIN_SIZE (0x0000000c) */

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

/* Bits of AE_DYN_WIN_CAL_NUM (0x00000010) */

#define WIN_CAL_NUM_OFFSET 0
#define WIN_CAL_NUM_BITS 16
#define WIN_CAL_NUM_MASK (((1 << 16) - 1) << 0)

/* Bits of AE_DYN_HWIN_START (0x00000014) */

#define AE_YST_X_OFFSET 0
#define AE_YST_X_BITS 16
#define AE_YST_X_MASK (((1 << 16) - 1) << 0)

#define AE_YST_Y_OFFSET 16
#define AE_YST_Y_BITS 16
#define AE_YST_Y_MASK (((1 << 16) - 1) << 16)

/* Bits of AE_DYN_HWIN_END (0x00000018) */

#define AE_END_X_OFFSET 0
#define AE_END_X_BITS 16
#define AE_END_X_MASK (((1 << 16) - 1) << 0)

#define AE_END_Y_OFFSET 16
#define AE_END_Y_BITS 16
#define AE_END_Y_MASK (((1 << 16) - 1) << 16)

/* Bits of AE_DYN_BRIGHT_COEF (0x0000001c) */

#define COEF_R_OFFSET 0
#define COEF_R_BITS 4
#define COEF_R_MASK (((1 << 4) - 1) << 0)

#define COEF_GR_OFFSET 4
#define COEF_GR_BITS 4
#define COEF_GR_MASK (((1 << 4) - 1) << 4)

#define COEF_B_OFFSET 8
#define COEF_B_BITS 4
#define COEF_B_MASK (((1 << 4) - 1) << 8)

#define COEF_GB_OFFSET 16
#define COEF_GB_BITS 4
#define COEF_GB_MASK (((1 << 4) - 1) << 16)

/* Bits of AE_DYN_Y_THD0 (0x00000020) */

#define MIN_Y_OFFSET 0
#define MIN_Y_BITS 10
#define MIN_Y_MASK (((1 << 10) - 1) << 0)

/* Bits of AE_DYN_Y_THD1 (0x00000024) */

#define MAX_Y_OFFSET 0
#define MAX_Y_BITS 10
#define MAX_Y_MASK (((1 << 10) - 1) << 0)

/* Bits of AE_DYN_R_THD0 (0x00000028) */

#define MIN_R_OFFSET 0
#define MIN_R_BITS 10
#define MIN_R_MASK (((1 << 10) - 1) << 0)

/* Bits of AE_DYN_R_THD1 (0x0000002c) */

#define MAX_R_OFFSET 0
#define MAX_R_BITS 10
#define MAX_R_MASK (((1 << 10) - 1) << 0)

/* Bits of AE_DYN_GR_THD0 (0x00000030) */

#define MIN_GR_OFFSET 0
#define MIN_GR_BITS 10
#define MIN_GR_MASK (((1 << 10) - 1) << 0)

/* Bits of AE_DYN_GR_THD1 (0x00000034) */

#define MAX_GR_OFFSET 0
#define MAX_GR_BITS 10
#define MAX_GR_MASK (((1 << 10) - 1) << 0)

/* Bits of AE_DYN_GB_THD0 (0x00000038) */

#define MIN_GB_OFFSET 0
#define MIN_GB_BITS 10
#define MIN_GB_MASK (((1 << 10) - 1) << 0)

/* Bits of AE_DYN_GB_THD1 (0x0000003c) */

#define MAX_GB_OFFSET 0
#define MAX_GB_BITS 10
#define MAX_GB_MASK (((1 << 10) - 1) << 0)

/* Bits of AE_DYN_B_THD0 (0x00000040) */

#define MIN_B_OFFSET 0
#define MIN_B_BITS 10
#define MIN_B_MASK (((1 << 10) - 1) << 0)

/* Bits of AE_DYN_B_THD1 (0x00000044) */

#define MAX_B_OFFSET 0
#define MAX_B_BITS 10
#define MAX_B_MASK (((1 << 10) - 1) << 0)

/* Bits of AE_DYN_SUM_MAX_Y (0x00000048) */

#define MAX_SUM_Y_OFFSET 0
#define MAX_SUM_Y_BITS 20
#define MAX_SUM_Y_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_DYN_SUM_MIN_Y (0x0000004c) */

#define MIN_SUM_Y_OFFSET 0
#define MIN_SUM_Y_BITS 20
#define MIN_SUM_Y_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_DYN_SUM_MAX_R (0x00000050) */

#define MAX_SUM_R_OFFSET 0
#define MAX_SUM_R_BITS 20
#define MAX_SUM_R_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_DYN_SUM_MIN_R (0x00000054) */

#define MIN_SUM_R_OFFSET 0
#define MIN_SUM_R_BITS 20
#define MIN_SUM_R_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_DYN_SUM_MAX_GR (0x00000058) */

#define MAX_SUM_GR_OFFSET 0
#define MAX_SUM_GR_BITS 20
#define MAX_SUM_GR_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_DYN_SUM_MIN_GR (0x0000005c) */

#define MIN_SUM_GR_OFFSET 0
#define MIN_SUM_GR_BITS 20
#define MIN_SUM_GR_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_DYN_SUM_MAX_GB (0x00000060) */

#define MAX_SUM_GB_OFFSET 0
#define MAX_SUM_GB_BITS 20
#define MAX_SUM_GB_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_DYN_SUM_MIN_GB (0x00000064) */

#define MIN_SUM_GB_OFFSET 0
#define MIN_SUM_GB_BITS 20
#define MIN_SUM_GB_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_DYN_SUM_MAX_B (0x00000068) */

#define MAX_SUM_B_OFFSET 0
#define MAX_SUM_B_BITS 20
#define MAX_SUM_B_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_DYN_SUM_MIN_B (0x0000006c) */

#define MIN_SUM_B_OFFSET 0
#define MIN_SUM_B_BITS 20
#define MIN_SUM_B_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_DYN_YWIN_START (0x00000070) */

#define YSTA_X_OFFSET 0
#define YSTA_X_BITS 16
#define YSTA_X_MASK (((1 << 16) - 1) << 0)

#define YSTA_Y_OFFSET 16
#define YSTA_Y_BITS 16
#define YSTA_Y_MASK (((1 << 16) - 1) << 16)

/* Bits of AE_DYN_YWIN_END (0x00000074) */

#define YEND_X_OFFSET 0
#define YEND_X_BITS 16
#define YEND_X_MASK (((1 << 16) - 1) << 0)

#define YEND_Y_OFFSET 16
#define YEND_Y_BITS 16
#define YEND_Y_MASK (((1 << 16) - 1) << 16)

/* Bits of AE_DYN_YSUM (0x00000078) */

#define YSUM_OFFSET 0
#define YSUM_BITS 30
#define YSUM_MASK (((1 << 30) - 1) << 0)

/* Bits of AE_DYN_GAIN (0x0000007c) */

#define AE_GAIN_OFFSET 0
#define AE_GAIN_BITS 17
#define AE_GAIN_MASK (((1 << 17) - 1) << 0)

/* Bits of AE_DYN_RECT_START (0x00000080) */

#define AE_RECT_X_START_OFFSET 0
#define AE_RECT_X_START_BITS 16
#define AE_RECT_X_START_MASK (((1 << 16) - 1) << 0)

#define AE_RECT_Y_START_OFFSET 16
#define AE_RECT_Y_START_BITS 16
#define AE_RECT_Y_START_MASK (((1 << 16) - 1) << 16)

/* Bits of AE_DYN_RECT_SIZE (0x00000084) */

#define AE_RECT_WIDTH_OFFSET 0
#define AE_RECT_WIDTH_BITS 16
#define AE_RECT_WIDTH_MASK (((1 << 16) - 1) << 0)

#define AE_RECT_HEIGHT_OFFSET 16
#define AE_RECT_HEIGHT_BITS 16
#define AE_RECT_HEIGHT_MASK (((1 << 16) - 1) << 16)

/* Bits of AE_DYN_RECT_CALC_NUM (0x00000088) */

#define AE_RECT_CAL_NUM_OFFSET 0
#define AE_RECT_CAL_NUM_BITS 20
#define AE_RECT_CAL_NUM_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_DYN_RECT_RESULT (0x0000008c) */

#define AE_RECT_RESULT_OFFSET 0
#define AE_RECT_RESULT_BITS 29
#define AE_RECT_RESULT_MASK (((1 << 29) - 1) << 0)

/* Bits of AE_DYN_HISTO_MEM (0x00000100~0x000010fc) */

#define AE_HISTO_OFFSET 0
#define AE_HISTO_BITS 20
#define AE_HISTO_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_DYN_WIN_MEM (0x00001100~0x000014fc) */

#define AE_WIN_OFFSET 0
#define AE_WIN_BITS 24
#define AE_WIN_MASK (((1 << 24) - 1) << 0)

#endif /* _AE_0_H_INC_ */
