
//tool_version:1.4.35

#ifndef _AE_2_H_INC_
#define _AE_2_H_INC_

#include <isp_reg.h>
#include <isp_reg_base.h>

#define AE_CTRL (AE_2_BASE_ADDR + 0x00000000) //0x0001d000
#define AE_WIN_START (AE_2_BASE_ADDR + 0x00000004) //0x0001d004
#define AE_WIN_SIZE (AE_2_BASE_ADDR + 0x00000008) //0x0001d008
#define AE_WIN_CAL_NUM (AE_2_BASE_ADDR + 0x0000000c) //0x0001d00c
#define AE_HWIN_START (AE_2_BASE_ADDR + 0x00000010) //0x0001d010
#define AE_HWIN_END (AE_2_BASE_ADDR + 0x00000014) //0x0001d014
#define AE_BRIGHT_COEF (AE_2_BASE_ADDR + 0x00000018) //0x0001d018
#define AE_Y_THD0 (AE_2_BASE_ADDR + 0x0000001c) //0x0001d01c
#define AE_Y_THD1 (AE_2_BASE_ADDR + 0x00000020) //0x0001d020
#define AE_R_THD0 (AE_2_BASE_ADDR + 0x00000024) //0x0001d024
#define AE_R_THD1 (AE_2_BASE_ADDR + 0x00000028) //0x0001d028
#define AE_GR_THD0 (AE_2_BASE_ADDR + 0x0000002c) //0x0001d02c
#define AE_GR_THD1 (AE_2_BASE_ADDR + 0x00000030) //0x0001d030
#define AE_GB_THD0 (AE_2_BASE_ADDR + 0x00000034) //0x0001d034
#define AE_GB_THD1 (AE_2_BASE_ADDR + 0x00000038) //0x0001d038
#define AE_B_THD0 (AE_2_BASE_ADDR + 0x0000003c) //0x0001d03c
#define AE_B_THD1 (AE_2_BASE_ADDR + 0x00000040) //0x0001d040
#define AE_SUM_MAX_Y (AE_2_BASE_ADDR + 0x00000044) //0x0001d044
#define AE_SUM_MIN_Y (AE_2_BASE_ADDR + 0x00000048) //0x0001d048
#define AE_SUM_MAX_R (AE_2_BASE_ADDR + 0x0000004c) //0x0001d04c
#define AE_SUM_MIN_R (AE_2_BASE_ADDR + 0x00000050) //0x0001d050
#define AE_SUM_MAX_GR (AE_2_BASE_ADDR + 0x00000054) //0x0001d054
#define AE_SUM_MIN_GR (AE_2_BASE_ADDR + 0x00000058) //0x0001d058
#define AE_SUM_MAX_GB (AE_2_BASE_ADDR + 0x0000005c) //0x0001d05c
#define AE_SUM_MIN_GB (AE_2_BASE_ADDR + 0x00000060) //0x0001d060
#define AE_SUM_MAX_B (AE_2_BASE_ADDR + 0x00000064) //0x0001d064
#define AE_SUM_MIN_B (AE_2_BASE_ADDR + 0x00000068) //0x0001d068
#define AE_YWIN_START (AE_2_BASE_ADDR + 0x0000006c) //0x0001d06c
#define AE_YWIN_END (AE_2_BASE_ADDR + 0x00000070) //0x0001d070
#define AE_YSUM (AE_2_BASE_ADDR + 0x00000074) //0x0001d074
#define AE_GAIN_DATA (AE_2_BASE_ADDR + 0x00000080) //0x0001d080
#define AE_GAIN_CTRL (AE_2_BASE_ADDR + 0x00000088) //0x0001d088
#define AE_HISTO_MEM_BASE (AE_2_BASE_ADDR + 0x00000100) //0x0001d100
#define AE_HISTO_MEM_TOP (AE_2_BASE_ADDR + 0x000004fc) //0x0001d4fc
#define AE_WIN_MEM_BASE (AE_2_BASE_ADDR + 0x00000500) //0x0001d500
#define AE_WIN_MEM_TOP (AE_2_BASE_ADDR + 0x000008fc) //0x0001d8fc

/* Bits of AE_CTRL (0x00000000) */

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

#define AE_YSUM_EN_OFFSET 8
#define AE_YSUM_EN_BITS 1
#define AE_YSUM_EN_MASK (((1 << 1) - 1) << 8)

/* Bits of AE_WIN_START (0x00000004) */

#define AE_START_X_OFFSET 0
#define AE_START_X_BITS 16
#define AE_START_X_MASK (((1 << 16) - 1) << 0)

#define AE_START_Y_OFFSET 16
#define AE_START_Y_BITS 16
#define AE_START_Y_MASK (((1 << 16) - 1) << 16)

/* Bits of AE_WIN_SIZE (0x00000008) */

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

/* Bits of AE_WIN_CAL_NUM (0x0000000c) */

#define WIN_CAL_NUM_OFFSET 0
#define WIN_CAL_NUM_BITS 16
#define WIN_CAL_NUM_MASK (((1 << 16) - 1) << 0)

/* Bits of AE_HWIN_START (0x00000010) */

#define AE_YST_X_OFFSET 0
#define AE_YST_X_BITS 16
#define AE_YST_X_MASK (((1 << 16) - 1) << 0)

#define AE_YST_Y_OFFSET 16
#define AE_YST_Y_BITS 16
#define AE_YST_Y_MASK (((1 << 16) - 1) << 16)

/* Bits of AE_HWIN_END (0x00000014) */

#define AE_END_X_OFFSET 0
#define AE_END_X_BITS 16
#define AE_END_X_MASK (((1 << 16) - 1) << 0)

#define AE_END_Y_OFFSET 16
#define AE_END_Y_BITS 16
#define AE_END_Y_MASK (((1 << 16) - 1) << 16)

/* Bits of AE_BRIGHT_COEF (0x00000018) */

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

/* Bits of AE_Y_THD0 (0x0000001c) */

#define MIN_Y_OFFSET 0
#define MIN_Y_BITS 10
#define MIN_Y_MASK (((1 << 10) - 1) << 0)

/* Bits of AE_Y_THD1 (0x00000020) */

#define MAX_Y_OFFSET 0
#define MAX_Y_BITS 10
#define MAX_Y_MASK (((1 << 10) - 1) << 0)

/* Bits of AE_R_THD0 (0x00000024) */

#define MIN_R_OFFSET 0
#define MIN_R_BITS 10
#define MIN_R_MASK (((1 << 10) - 1) << 0)

/* Bits of AE_R_THD1 (0x00000028) */

#define MAX_R_OFFSET 0
#define MAX_R_BITS 10
#define MAX_R_MASK (((1 << 10) - 1) << 0)

/* Bits of AE_GR_THD0 (0x0000002c) */

#define MIN_GR_OFFSET 0
#define MIN_GR_BITS 10
#define MIN_GR_MASK (((1 << 10) - 1) << 0)

/* Bits of AE_GR_THD1 (0x00000030) */

#define MAX_GR_OFFSET 0
#define MAX_GR_BITS 10
#define MAX_GR_MASK (((1 << 10) - 1) << 0)

/* Bits of AE_GB_THD0 (0x00000034) */

#define MIN_GB_OFFSET 0
#define MIN_GB_BITS 10
#define MIN_GB_MASK (((1 << 10) - 1) << 0)

/* Bits of AE_GB_THD1 (0x00000038) */

#define MAX_GB_OFFSET 0
#define MAX_GB_BITS 10
#define MAX_GB_MASK (((1 << 10) - 1) << 0)

/* Bits of AE_B_THD0 (0x0000003c) */

#define MIN_B_OFFSET 0
#define MIN_B_BITS 10
#define MIN_B_MASK (((1 << 10) - 1) << 0)

/* Bits of AE_B_THD1 (0x00000040) */

#define MAX_B_OFFSET 0
#define MAX_B_BITS 10
#define MAX_B_MASK (((1 << 10) - 1) << 0)

/* Bits of AE_SUM_MAX_Y (0x00000044) */

#define MAX_SUM_Y_OFFSET 0
#define MAX_SUM_Y_BITS 20
#define MAX_SUM_Y_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_SUM_MIN_Y (0x00000048) */

#define MIN_SUM_Y_OFFSET 0
#define MIN_SUM_Y_BITS 20
#define MIN_SUM_Y_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_SUM_MAX_R (0x0000004c) */

#define MAX_SUM_R_OFFSET 0
#define MAX_SUM_R_BITS 20
#define MAX_SUM_R_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_SUM_MIN_R (0x00000050) */

#define MIN_SUM_R_OFFSET 0
#define MIN_SUM_R_BITS 20
#define MIN_SUM_R_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_SUM_MAX_GR (0x00000054) */

#define MAX_SUM_GR_OFFSET 0
#define MAX_SUM_GR_BITS 20
#define MAX_SUM_GR_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_SUM_MIN_GR (0x00000058) */

#define MIN_SUM_GR_OFFSET 0
#define MIN_SUM_GR_BITS 20
#define MIN_SUM_GR_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_SUM_MAX_GB (0x0000005c) */

#define MAX_SUM_GB_OFFSET 0
#define MAX_SUM_GB_BITS 20
#define MAX_SUM_GB_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_SUM_MIN_GB (0x00000060) */

#define MIN_SUM_GB_OFFSET 0
#define MIN_SUM_GB_BITS 20
#define MIN_SUM_GB_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_SUM_MAX_B (0x00000064) */

#define MAX_SUM_B_OFFSET 0
#define MAX_SUM_B_BITS 20
#define MAX_SUM_B_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_SUM_MIN_B (0x00000068) */

#define MIN_SUM_B_OFFSET 0
#define MIN_SUM_B_BITS 20
#define MIN_SUM_B_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_YWIN_START (0x0000006c) */

#define YSTA_X_OFFSET 0
#define YSTA_X_BITS 16
#define YSTA_X_MASK (((1 << 16) - 1) << 0)

#define YSTA_Y_OFFSET 16
#define YSTA_Y_BITS 16
#define YSTA_Y_MASK (((1 << 16) - 1) << 16)

/* Bits of AE_YWIN_END (0x00000070) */

#define YEND_X_OFFSET 0
#define YEND_X_BITS 16
#define YEND_X_MASK (((1 << 16) - 1) << 0)

#define YEND_Y_OFFSET 16
#define YEND_Y_BITS 16
#define YEND_Y_MASK (((1 << 16) - 1) << 16)

/* Bits of AE_YSUM (0x00000074) */

#define YSUM_OFFSET 0
#define YSUM_BITS 30
#define YSUM_MASK (((1 << 30) - 1) << 0)

/* Bits of AE_GAIN_DATA (0x00000080) */

#define AE_GAIN_OFFSET 0
#define AE_GAIN_BITS 17
#define AE_GAIN_MASK (((1 << 17) - 1) << 0)

/* Bits of AE_GAIN_CTRL (0x00000088) */

#define AE_GAIN_DELAY_OFFSET 0
#define AE_GAIN_DELAY_BITS 2
#define AE_GAIN_DELAY_MASK (((1 << 2) - 1) << 0)

#define AE_GAIN_TRIG_OFFSET 8
#define AE_GAIN_TRIG_BITS 1
#define AE_GAIN_TRIG_MASK (((1 << 1) - 1) << 8)

/* Bits of AE_HISTO_MEM (0x00000100~0x000004fc) */

#define AE_HISTO_OFFSET 0
#define AE_HISTO_BITS 20
#define AE_HISTO_MASK (((1 << 20) - 1) << 0)

/* Bits of AE_WIN_MEM (0x00000500~0x000008fc) */

#define AE_WIN_OFFSET 0
#define AE_WIN_BITS 24
#define AE_WIN_MASK (((1 << 24) - 1) << 0)

#endif /* _AE_2_H_INC_ */
