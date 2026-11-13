
//tool_version:1.4.35

#ifndef _WDR_H_INC_
#define _WDR_H_INC_

#include <isp_reg.h>
#include <isp_reg_base.h>

#define HDR_ENABLE (WDR_BASE_ADDR + 0x00000000) //0x00004400
#define HDR_MODE (WDR_BASE_ADDR + 0x00000004) //0x00004404
#define HDR_LOAD (WDR_BASE_ADDR + 0x00000008) //0x00004408
#define HDR_STEP (WDR_BASE_ADDR + 0x0000000c) //0x0000440c
#define HDR_LEVEL (WDR_BASE_ADDR + 0x00000010) //0x00004410
#define HDR_LEVEL_NOW (WDR_BASE_ADDR + 0x00000014) //0x00004414
#define HDR_CONT_RATE_BOUND (WDR_BASE_ADDR + 0x00000018) //0x00004418
#define HDR_CONT_PARAM0 (WDR_BASE_ADDR + 0x0000001c) //0x0000441c
#define HDR_CONT_PARAM1 (WDR_BASE_ADDR + 0x00000020) //0x00004420
#define HDR_CONT_PARAM2 (WDR_BASE_ADDR + 0x00000024) //0x00004424
#define HDR_CONT_PARAM3 (WDR_BASE_ADDR + 0x00000028) //0x00004428
#define HDR_PARAM0 (WDR_BASE_ADDR + 0x0000002c) //0x0000442c
#define HDR_PARAM1 (WDR_BASE_ADDR + 0x00000030) //0x00004430
#define HDR_PARAM2 (WDR_BASE_ADDR + 0x00000034) //0x00004434
#define HDR_PARAM3 (WDR_BASE_ADDR + 0x00000038) //0x00004438
#define HDR_PARAM4 (WDR_BASE_ADDR + 0x0000003c) //0x0000443c
#define HDR_PARAM5 (WDR_BASE_ADDR + 0x00000040) //0x00004440
#define HDR_PARAM6 (WDR_BASE_ADDR + 0x00000044) //0x00004444
#define HDR_LPF_COEF0 (WDR_BASE_ADDR + 0x00000048) //0x00004448
#define HDR_LPF_COEF1 (WDR_BASE_ADDR + 0x0000004c) //0x0000444c
#define HDR_LPF_COEF2 (WDR_BASE_ADDR + 0x00000050) //0x00004450
#define HDR_LOC_ADJUST_THD (WDR_BASE_ADDR + 0x00000054) //0x00004454
#define HDR_INC_RATE (WDR_BASE_ADDR + 0x00000058) //0x00004458
#define HDR_HER_DIFF_THD (WDR_BASE_ADDR + 0x0000005c) //0x0000445c
#define HDR_HER_DIFF_INV (WDR_BASE_ADDR + 0x00000060) //0x00004460
#define HDR_HER_ADJ_THD (WDR_BASE_ADDR + 0x00000064) //0x00004464
#define HDR_HER_ADJ_INV (WDR_BASE_ADDR + 0x00000068) //0x00004468
#define HDR_HER_ADJ_MIN (WDR_BASE_ADDR + 0x0000006c) //0x0000446c
#define HDR_DE_THD (WDR_BASE_ADDR + 0x00000070) //0x00004470
#define HDR_DE_INV (WDR_BASE_ADDR + 0x00000074) //0x00004474
#define HDR_AL_PARAM (WDR_BASE_ADDR + 0x00000078) //0x00004478
#define HDR_AL_M (WDR_BASE_ADDR + 0x0000007c) //0x0000447c
#define HDR_AL_RATIO (WDR_BASE_ADDR + 0x00000080) //0x00004480
#define HDR_AL_RATIO_AUTO (WDR_BASE_ADDR + 0x00000084) //0x00004484

/* Bits of HDR_ENABLE (0x00000000) */

#define LPF_EN_OFFSET 1
#define LPF_EN_BITS 1
#define LPF_EN_MASK (((1 << 1) - 1) << 1)

#define CONT_EN_OFFSET 2
#define CONT_EN_BITS 1
#define CONT_EN_MASK (((1 << 1) - 1) << 2)

#define HER_ENABLE_OFFSET 3
#define HER_ENABLE_BITS 1
#define HER_ENABLE_MASK (((1 << 1) - 1) << 3)

#define DE_ENABLE_OFFSET 4
#define DE_ENABLE_BITS 1
#define DE_ENABLE_MASK (((1 << 1) - 1) << 4)

#define HDR_LOC_ADJ_EN_OFFSET 5
#define HDR_LOC_ADJ_EN_BITS 1
#define HDR_LOC_ADJ_EN_MASK (((1 << 1) - 1) << 5)

#define AL_ENABLE_OFFSET 6
#define AL_ENABLE_BITS 1
#define AL_ENABLE_MASK (((1 << 1) - 1) << 6)

#define AL_STEP_EN_OFFSET 7
#define AL_STEP_EN_BITS 1
#define AL_STEP_EN_MASK (((1 << 1) - 1) << 7)

/* Bits of HDR_MODE (0x00000004) */

#define HDR_MODE_OFFSET 0
#define HDR_MODE_BITS 1
#define HDR_MODE_MASK (((1 << 1) - 1) << 0)

#define CONT_MODE_OFFSET 1
#define CONT_MODE_BITS 1
#define CONT_MODE_MASK (((1 << 1) - 1) << 1)

#define AL_MODE_OFFSET 2
#define AL_MODE_BITS 1
#define AL_MODE_MASK (((1 << 1) - 1) << 2)

/* Bits of HDR_LOAD (0x00000008) */

#define ADJ_STOP_OFFSET 0
#define ADJ_STOP_BITS 1
#define ADJ_STOP_MASK (((1 << 1) - 1) << 0)

#define ADJ_TRIG_OFFSET 1
#define ADJ_TRIG_BITS 1
#define ADJ_TRIG_MASK (((1 << 1) - 1) << 1)

#define HDR_REG_LOAD_OFFSET 2
#define HDR_REG_LOAD_BITS 1
#define HDR_REG_LOAD_MASK (((1 << 1) - 1) << 2)

#define ADJ_GOING_OFFSET 3
#define ADJ_GOING_BITS 1
#define ADJ_GOING_MASK (((1 << 1) - 1) << 3)

/* Bits of HDR_STEP (0x0000000c) */

#define HDR_STEP_OFFSET 0
#define HDR_STEP_BITS 6
#define HDR_STEP_MASK (((1 << 6) - 1) << 0)

#define CONT_STEP_OFFSET 8
#define CONT_STEP_BITS 6
#define CONT_STEP_MASK (((1 << 6) - 1) << 8)

/* Bits of HDR_LEVEL (0x00000010) */

#define HDR_LEVEL_OFFSET 0
#define HDR_LEVEL_BITS 8
#define HDR_LEVEL_MASK (((1 << 8) - 1) << 0)

#define CONT_LEVEL_OFFSET 8
#define CONT_LEVEL_BITS 8
#define CONT_LEVEL_MASK (((1 << 8) - 1) << 8)

/* Bits of HDR_LEVEL_NOW (0x00000014) */

#define HDR_LEVEL_SYNC_OFFSET 0
#define HDR_LEVEL_SYNC_BITS 8
#define HDR_LEVEL_SYNC_MASK (((1 << 8) - 1) << 0)

#define CONT_LEVEL_SYNC_OFFSET 8
#define CONT_LEVEL_SYNC_BITS 8
#define CONT_LEVEL_SYNC_MASK (((1 << 8) - 1) << 8)

/* Bits of HDR_CONT_RATE_BOUND (0x00000018) */

#define CONT_RATE_MIN_OFFSET 0
#define CONT_RATE_MIN_BITS 6
#define CONT_RATE_MIN_MASK (((1 << 6) - 1) << 0)

#define CONT_RATE_MAX_OFFSET 8
#define CONT_RATE_MAX_BITS 6
#define CONT_RATE_MAX_MASK (((1 << 6) - 1) << 8)

/* Bits of HDR_CONT_PARAM0 (0x0000001c) */

#define CONT_P0_OFFSET 0
#define CONT_P0_BITS 6
#define CONT_P0_MASK (((1 << 6) - 1) << 0)

#define CONT_P1_OFFSET 8
#define CONT_P1_BITS 6
#define CONT_P1_MASK (((1 << 6) - 1) << 8)

#define CONT_P2_OFFSET 16
#define CONT_P2_BITS 6
#define CONT_P2_MASK (((1 << 6) - 1) << 16)

#define CONT_P3_OFFSET 24
#define CONT_P3_BITS 6
#define CONT_P3_MASK (((1 << 6) - 1) << 24)

/* Bits of HDR_CONT_PARAM1 (0x00000020) */

#define CONT_P4_OFFSET 0
#define CONT_P4_BITS 6
#define CONT_P4_MASK (((1 << 6) - 1) << 0)

#define CONT_P5_OFFSET 8
#define CONT_P5_BITS 6
#define CONT_P5_MASK (((1 << 6) - 1) << 8)

#define CONT_P6_OFFSET 16
#define CONT_P6_BITS 6
#define CONT_P6_MASK (((1 << 6) - 1) << 16)

#define CONT_P7_OFFSET 24
#define CONT_P7_BITS 6
#define CONT_P7_MASK (((1 << 6) - 1) << 24)

/* Bits of HDR_CONT_PARAM2 (0x00000024) */

#define CONT_P8_OFFSET 0
#define CONT_P8_BITS 6
#define CONT_P8_MASK (((1 << 6) - 1) << 0)

#define CONT_P9_OFFSET 8
#define CONT_P9_BITS 6
#define CONT_P9_MASK (((1 << 6) - 1) << 8)

#define CONT_PA_OFFSET 16
#define CONT_PA_BITS 6
#define CONT_PA_MASK (((1 << 6) - 1) << 16)

#define CONT_PB_OFFSET 24
#define CONT_PB_BITS 6
#define CONT_PB_MASK (((1 << 6) - 1) << 24)

/* Bits of HDR_CONT_PARAM3 (0x00000028) */

#define CONT_PC_OFFSET 0
#define CONT_PC_BITS 6
#define CONT_PC_MASK (((1 << 6) - 1) << 0)

#define CONT_PD_OFFSET 8
#define CONT_PD_BITS 6
#define CONT_PD_MASK (((1 << 6) - 1) << 8)

#define CONT_PE_OFFSET 16
#define CONT_PE_BITS 6
#define CONT_PE_MASK (((1 << 6) - 1) << 16)

/* Bits of HDR_PARAM0 (0x0000002c) */

#define HDR_P0_OFFSET 0
#define HDR_P0_BITS 8
#define HDR_P0_MASK (((1 << 8) - 1) << 0)

#define HDR_P1_OFFSET 8
#define HDR_P1_BITS 8
#define HDR_P1_MASK (((1 << 8) - 1) << 8)

#define HDR_P2_OFFSET 16
#define HDR_P2_BITS 8
#define HDR_P2_MASK (((1 << 8) - 1) << 16)

#define HDR_P3_OFFSET 24
#define HDR_P3_BITS 8
#define HDR_P3_MASK (((1 << 8) - 1) << 24)

/* Bits of HDR_PARAM1 (0x00000030) */

#define HDR_P4_OFFSET 0
#define HDR_P4_BITS 8
#define HDR_P4_MASK (((1 << 8) - 1) << 0)

#define HDR_P5_OFFSET 8
#define HDR_P5_BITS 8
#define HDR_P5_MASK (((1 << 8) - 1) << 8)

#define HDR_P6_OFFSET 16
#define HDR_P6_BITS 8
#define HDR_P6_MASK (((1 << 8) - 1) << 16)

#define HDR_P7_OFFSET 24
#define HDR_P7_BITS 8
#define HDR_P7_MASK (((1 << 8) - 1) << 24)

/* Bits of HDR_PARAM2 (0x00000034) */

#define HDR_P8_OFFSET 0
#define HDR_P8_BITS 7
#define HDR_P8_MASK (((1 << 7) - 1) << 0)

#define HDR_P9_OFFSET 8
#define HDR_P9_BITS 7
#define HDR_P9_MASK (((1 << 7) - 1) << 8)

#define HDR_PA_OFFSET 16
#define HDR_PA_BITS 7
#define HDR_PA_MASK (((1 << 7) - 1) << 16)

#define HDR_PB_OFFSET 24
#define HDR_PB_BITS 7
#define HDR_PB_MASK (((1 << 7) - 1) << 24)

/* Bits of HDR_PARAM3 (0x00000038) */

#define HDR_PC_OFFSET 0
#define HDR_PC_BITS 7
#define HDR_PC_MASK (((1 << 7) - 1) << 0)

#define HDR_PD_OFFSET 8
#define HDR_PD_BITS 7
#define HDR_PD_MASK (((1 << 7) - 1) << 8)

#define HDR_PE_OFFSET 16
#define HDR_PE_BITS 7
#define HDR_PE_MASK (((1 << 7) - 1) << 16)

#define HDR_PF_OFFSET 24
#define HDR_PF_BITS 7
#define HDR_PF_MASK (((1 << 7) - 1) << 24)

/* Bits of HDR_PARAM4 (0x0000003c) */

#define HDR_PG_OFFSET 0
#define HDR_PG_BITS 7
#define HDR_PG_MASK (((1 << 7) - 1) << 0)

#define HDR_PH_OFFSET 8
#define HDR_PH_BITS 7
#define HDR_PH_MASK (((1 << 7) - 1) << 8)

#define HDR_PI_OFFSET 16
#define HDR_PI_BITS 7
#define HDR_PI_MASK (((1 << 7) - 1) << 16)

#define HDR_PJ_OFFSET 24
#define HDR_PJ_BITS 7
#define HDR_PJ_MASK (((1 << 7) - 1) << 24)

/* Bits of HDR_PARAM5 (0x00000040) */

#define HDR_PK_OFFSET 0
#define HDR_PK_BITS 7
#define HDR_PK_MASK (((1 << 7) - 1) << 0)

#define HDR_PL_OFFSET 8
#define HDR_PL_BITS 7
#define HDR_PL_MASK (((1 << 7) - 1) << 8)

#define HDR_PM_OFFSET 16
#define HDR_PM_BITS 7
#define HDR_PM_MASK (((1 << 7) - 1) << 16)

#define HDR_PN_OFFSET 24
#define HDR_PN_BITS 7
#define HDR_PN_MASK (((1 << 7) - 1) << 24)

/* Bits of HDR_PARAM6 (0x00000044) */

#define HDR_PO_OFFSET 0
#define HDR_PO_BITS 7
#define HDR_PO_MASK (((1 << 7) - 1) << 0)

#define HDR_PP_OFFSET 8
#define HDR_PP_BITS 7
#define HDR_PP_MASK (((1 << 7) - 1) << 8)

#define HDR_PQ_OFFSET 16
#define HDR_PQ_BITS 7
#define HDR_PQ_MASK (((1 << 7) - 1) << 16)

#define HDR_PR_OFFSET 24
#define HDR_PR_BITS 7
#define HDR_PR_MASK (((1 << 7) - 1) << 24)

/* Bits of HDR_LPF_COEF0 (0x00000048) */

#define LPF_C0_OFFSET 0
#define LPF_C0_BITS 3
#define LPF_C0_MASK (((1 << 3) - 1) << 0)

#define LPF_C1_OFFSET 8
#define LPF_C1_BITS 3
#define LPF_C1_MASK (((1 << 3) - 1) << 8)

#define LPF_C2_OFFSET 16
#define LPF_C2_BITS 3
#define LPF_C2_MASK (((1 << 3) - 1) << 16)

#define LPF_C3_OFFSET 24
#define LPF_C3_BITS 5
#define LPF_C3_MASK (((1 << 5) - 1) << 24)

/* Bits of HDR_LPF_COEF1 (0x0000004c) */

#define LPF_C4_OFFSET 0
#define LPF_C4_BITS 5
#define LPF_C4_MASK (((1 << 5) - 1) << 0)

#define LPF_C5_OFFSET 8
#define LPF_C5_BITS 5
#define LPF_C5_MASK (((1 << 5) - 1) << 8)

#define LPF_C6_OFFSET 16
#define LPF_C6_BITS 3
#define LPF_C6_MASK (((1 << 3) - 1) << 16)

#define LPF_C7_OFFSET 24
#define LPF_C7_BITS 3
#define LPF_C7_MASK (((1 << 3) - 1) << 24)

/* Bits of HDR_LPF_COEF2 (0x00000050) */

#define LPF_C8_OFFSET 0
#define LPF_C8_BITS 3
#define LPF_C8_MASK (((1 << 3) - 1) << 0)

/* Bits of HDR_LOC_ADJUST_THD (0x00000054) */

#define HDR_LOC_ADJ_THD_OFFSET 0
#define HDR_LOC_ADJ_THD_BITS 8
#define HDR_LOC_ADJ_THD_MASK (((1 << 8) - 1) << 0)

#define HDR_LOC_ADJ_THD_INV_OFFSET 8
#define HDR_LOC_ADJ_THD_INV_BITS 8
#define HDR_LOC_ADJ_THD_INV_MASK (((1 << 8) - 1) << 8)

/* Bits of HDR_INC_RATE (0x00000058) */

#define HDR_INC_RATE0_OFFSET 0
#define HDR_INC_RATE0_BITS 5
#define HDR_INC_RATE0_MASK (((1 << 5) - 1) << 0)

#define HDR_INC_RATE1_OFFSET 8
#define HDR_INC_RATE1_BITS 5
#define HDR_INC_RATE1_MASK (((1 << 5) - 1) << 8)

/* Bits of HDR_HER_DIFF_THD (0x0000005c) */

#define HER_DIFF_THD1_OFFSET 0
#define HER_DIFF_THD1_BITS 8
#define HER_DIFF_THD1_MASK (((1 << 8) - 1) << 0)

#define HER_DIFF_THD2_OFFSET 8
#define HER_DIFF_THD2_BITS 8
#define HER_DIFF_THD2_MASK (((1 << 8) - 1) << 8)

/* Bits of HDR_HER_DIFF_INV (0x00000060) */

#define HER_DIFF_INV_OFFSET 0
#define HER_DIFF_INV_BITS 8
#define HER_DIFF_INV_MASK (((1 << 8) - 1) << 0)

/* Bits of HDR_HER_ADJ_THD (0x00000064) */

#define HER_ADJ_THD1_OFFSET 0
#define HER_ADJ_THD1_BITS 8
#define HER_ADJ_THD1_MASK (((1 << 8) - 1) << 0)

#define HER_ADJ_THD2_OFFSET 16
#define HER_ADJ_THD2_BITS 8
#define HER_ADJ_THD2_MASK (((1 << 8) - 1) << 16)

/* Bits of HDR_HER_ADJ_INV (0x00000068) */

#define HER_ADJ_INV_OFFSET 0
#define HER_ADJ_INV_BITS 8
#define HER_ADJ_INV_MASK (((1 << 8) - 1) << 0)

/* Bits of HDR_HER_ADJ_MIN (0x0000006c) */

#define HER_ADJ_MIN_OFFSET 0
#define HER_ADJ_MIN_BITS 7
#define HER_ADJ_MIN_MASK (((1 << 7) - 1) << 0)

/* Bits of HDR_DE_THD (0x00000070) */

#define DE_THD1_OFFSET 0
#define DE_THD1_BITS 8
#define DE_THD1_MASK (((1 << 8) - 1) << 0)

#define DE_THD2_OFFSET 8
#define DE_THD2_BITS 8
#define DE_THD2_MASK (((1 << 8) - 1) << 8)

#define DE_MIN_OFFSET 16
#define DE_MIN_BITS 6
#define DE_MIN_MASK (((1 << 6) - 1) << 16)

#define DE_MAX_OFFSET 24
#define DE_MAX_BITS 6
#define DE_MAX_MASK (((1 << 6) - 1) << 24)

/* Bits of HDR_DE_INV (0x00000074) */

#define DE_INV_OFFSET 0
#define DE_INV_BITS 8
#define DE_INV_MASK (((1 << 8) - 1) << 0)

/* Bits of HDR_AL_PARAM (0x00000078) */

#define AL_RATE_OFFSET 0
#define AL_RATE_BITS 5
#define AL_RATE_MASK (((1 << 5) - 1) << 0)

#define AL_STEP_OFFSET 5
#define AL_STEP_BITS 4
#define AL_STEP_MASK (((1 << 4) - 1) << 5)

#define AL_STEP_TRIGGER_RANGE_OFFSET 9
#define AL_STEP_TRIGGER_RANGE_BITS 3
#define AL_STEP_TRIGGER_RANGE_MASK (((1 << 3) - 1) << 9)

/* Bits of HDR_AL_M (0x0000007c) */

#define AL_M1_REG_OFFSET 0
#define AL_M1_REG_BITS 8
#define AL_M1_REG_MASK (((1 << 8) - 1) << 0)

#define AL_M2_REG_OFFSET 8
#define AL_M2_REG_BITS 8
#define AL_M2_REG_MASK (((1 << 8) - 1) << 8)

/* Bits of HDR_AL_RATIO (0x00000080) */

#define AL_RATIO_REG_OFFSET 0
#define AL_RATIO_REG_BITS 10
#define AL_RATIO_REG_MASK (((1 << 10) - 1) << 0)

/* Bits of HDR_AL_RATIO_AUTO (0x00000084) */

#define AL_RATIO_OFFSET 0
#define AL_RATIO_BITS 10
#define AL_RATIO_MASK (((1 << 10) - 1) << 0)

#endif /* _WDR_H_INC_ */
