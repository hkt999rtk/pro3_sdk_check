
//tool_version:1.4.35

#ifndef _ISP_SYS_H_INC_
#define _ISP_SYS_H_INC_

#include <isp_reg.h>
#include <isp_reg_base.h>

#define SYS_CONTROL0 (ISP_SYS_BASE_ADDR + 0x00000000) //0x00000000
#define SYS_CONTROL1 (ISP_SYS_BASE_ADDR + 0x00000004) //0x00000004
#define SYS_CONTROL2 (ISP_SYS_BASE_ADDR + 0x00000008) //0x00000008
#define SYS_CONTROL3 (ISP_SYS_BASE_ADDR + 0x0000000c) //0x0000000c
#define SYS_INT_EN0 (ISP_SYS_BASE_ADDR + 0x00000010) //0x00000010
#define SYS_INT_CLR0 (ISP_SYS_BASE_ADDR + 0x00000014) //0x00000014
#define SYS_INT_EN1 (ISP_SYS_BASE_ADDR + 0x00000018) //0x00000018
#define SYS_INT_CLR1 (ISP_SYS_BASE_ADDR + 0x0000001c) //0x0000001c
#define SYS_FRAME_CNT (ISP_SYS_BASE_ADDR + 0x00000020) //0x00000020
#define SYS_ERROR_CNT (ISP_SYS_BASE_ADDR + 0x00000024) //0x00000024
#define SYS_FRAME_IDLE (ISP_SYS_BASE_ADDR + 0x00000028) //0x00000028
#define SYS_SPEED_CTRL (ISP_SYS_BASE_ADDR + 0x0000002c) //0x0000002c
#define SYS_LAST_BREAK (ISP_SYS_BASE_ADDR + 0x00000030) //0x00000030
#define SYS_LOW_DISABLE (ISP_SYS_BASE_ADDR + 0x00000038) //0x00000038
#define SYS_DATA_ENABLE (ISP_SYS_BASE_ADDR + 0x0000003c) //0x0000003c
#define SYS_INPUT_CTRL (ISP_SYS_BASE_ADDR + 0x00000044) //0x00000044
#define SYS_IMAGE_SEL0 (ISP_SYS_BASE_ADDR + 0x00000048) //0x00000048
#define SYS_IMAGE_SEL1 (ISP_SYS_BASE_ADDR + 0x0000004c) //0x0000004c
#define SYS_PATH_SEL (ISP_SYS_BASE_ADDR + 0x00000050) //0x00000050
#define SYS_IMAGE_MODE_SEL (ISP_SYS_BASE_ADDR + 0x0000005c) //0x0000005c
#define SYS_IMAGE_CHAN_SEL (ISP_SYS_BASE_ADDR + 0x00000060) //0x00000060
#define SYS_ISP_CLK_CTRL (ISP_SYS_BASE_ADDR + 0x00000064) //0x00000064
#define SYS_LCNT_INT_THD (ISP_SYS_BASE_ADDR + 0x00000068) //0x00000068
#define SYS_HOLE_ADDR_CTRL (ISP_SYS_BASE_ADDR + 0x0000006c) //0x0000006c
#define ISP_SYS_DUMMY0 (ISP_SYS_BASE_ADDR + 0x00000070) //0x00000070
#define ISP_SYS_DUMMY1 (ISP_SYS_BASE_ADDR + 0x00000074) //0x00000074
#define ISP_SYS_DUMMY2 (ISP_SYS_BASE_ADDR + 0x00000078) //0x00000078
#define ISP_SYS_BAYER_PATTERN (ISP_SYS_BASE_ADDR + 0x0000007c) //0x0000007c
#define ISP_PG_MEM (ISP_SYS_BASE_ADDR + 0x00000080) //0x00000080
#define SYS_TPNR_COMP (ISP_SYS_BASE_ADDR + 0x00000084) //0x00000084
#define SYS_SENSOR_COMP (ISP_SYS_BASE_ADDR + 0x00000088) //0x00000088
#define SYS_VHDR_SEL (ISP_SYS_BASE_ADDR + 0x0000008c) //0x0000008c
#define SYS_BIT_CLIP (ISP_SYS_BASE_ADDR + 0x00000090) //0x00000090
#define SYS_POS_SEL (ISP_SYS_BASE_ADDR + 0x00000094) //0x00000094
#define SYS_DATA_DELAY_TRIG (ISP_SYS_BASE_ADDR + 0x00000098) //0x00000098
#define SYS_DATA_DELAY (ISP_SYS_BASE_ADDR + 0x0000009c) //0x0000009c

/* Bits of SYS_CONTROL0 (0x00000000) */

#define ISP_STOP_OFFSET 1
#define ISP_STOP_BITS 1
#define ISP_STOP_MASK (((1 << 1) - 1) << 1)

#define ALL_MEM_IDLE_OFFSET 2
#define ALL_MEM_IDLE_BITS 1
#define ALL_MEM_IDLE_MASK (((1 << 1) - 1) << 2)

#define ISP_START_OFFSET 4
#define ISP_START_BITS 1
#define ISP_START_MASK (((1 << 1) - 1) << 4)

/* Bits of SYS_CONTROL1 (0x00000004) */

#define YGAMMA_EN_OFFSET 0
#define YGAMMA_EN_BITS 1
#define YGAMMA_EN_MASK (((1 << 1) - 1) << 0)

#define GAMMA_EN_OFFSET 1
#define GAMMA_EN_BITS 1
#define GAMMA_EN_MASK (((1 << 1) - 1) << 1)

#define MLSC_EN_OFFSET 2
#define MLSC_EN_BITS 1
#define MLSC_EN_MASK (((1 << 1) - 1) << 2)

#define NLSC_EN_OFFSET 3
#define NLSC_EN_BITS 1
#define NLSC_EN_MASK (((1 << 1) - 1) << 3)

#define UVT_EN_OFFSET 4
#define UVT_EN_BITS 1
#define UVT_EN_MASK (((1 << 1) - 1) << 4)

#define HDR_EN_OFFSET 5
#define HDR_EN_BITS 1
#define HDR_EN_MASK (((1 << 1) - 1) << 5)

#define AE_GAIN_EN_OFFSET 6
#define AE_GAIN_EN_BITS 1
#define AE_GAIN_EN_MASK (((1 << 1) - 1) << 6)

#define AWB_GAIN_EN_OFFSET 7
#define AWB_GAIN_EN_BITS 1
#define AWB_GAIN_EN_MASK (((1 << 1) - 1) << 7)

#define BLC_L_EN_OFFSET 8
#define BLC_L_EN_BITS 1
#define BLC_L_EN_MASK (((1 << 1) - 1) << 8)

#define LDC_EN_PRE_OFFSET 9
#define LDC_EN_PRE_BITS 1
#define LDC_EN_PRE_MASK (((1 << 1) - 1) << 9)

#define YGC_EN_OFFSET 10
#define YGC_EN_BITS 1
#define YGC_EN_MASK (((1 << 1) - 1) << 10)

#define CCM_EN_OFFSET 11
#define CCM_EN_BITS 1
#define CCM_EN_MASK (((1 << 1) - 1) << 11)

#define DPC_L_EN_OFFSET 12
#define DPC_L_EN_BITS 1
#define DPC_L_EN_MASK (((1 << 1) - 1) << 12)

#define EEH_EN_OFFSET 13
#define EEH_EN_BITS 1
#define EEH_EN_MASK (((1 << 1) - 1) << 13)

#define SPE_EN_OFFSET 14
#define SPE_EN_BITS 1
#define SPE_EN_MASK (((1 << 1) - 1) << 14)

#define TNR_EN_OFFSET 15
#define TNR_EN_BITS 1
#define TNR_EN_MASK (((1 << 1) - 1) << 15)

#define DPC_S_EN_OFFSET 16
#define DPC_S_EN_BITS 1
#define DPC_S_EN_MASK (((1 << 1) - 1) << 16)

#define BLC_S_EN_OFFSET 17
#define BLC_S_EN_BITS 1
#define BLC_S_EN_MASK (((1 << 1) - 1) << 17)

#define HUE_EN_OFFSET 18
#define HUE_EN_BITS 1
#define HUE_EN_MASK (((1 << 1) - 1) << 18)

#define TM_EN_OFFSET 19
#define TM_EN_BITS 1
#define TM_EN_MASK (((1 << 1) - 1) << 19)

#define CAC_EN_OFFSET 20
#define CAC_EN_BITS 1
#define CAC_EN_MASK (((1 << 1) - 1) << 20)

#define AE_GAIN_L_EN_OFFSET 21
#define AE_GAIN_L_EN_BITS 1
#define AE_GAIN_L_EN_MASK (((1 << 1) - 1) << 21)

#define AE_GAIN_S_EN_OFFSET 22
#define AE_GAIN_S_EN_BITS 1
#define AE_GAIN_S_EN_MASK (((1 << 1) - 1) << 22)

#define LCE_EN_OFFSET 23
#define LCE_EN_BITS 1
#define LCE_EN_MASK (((1 << 1) - 1) << 23)

#define BLCH_EN_OFFSET 24
#define BLCH_EN_BITS 1
#define BLCH_EN_MASK (((1 << 1) - 1) << 24)

#define MOT_INFO_SYS_EN_OFFSET 25
#define MOT_INFO_SYS_EN_BITS 1
#define MOT_INFO_SYS_EN_MASK (((1 << 1) - 1) << 25)

#define DITHER_EN_OFFSET 26
#define DITHER_EN_BITS 1
#define DITHER_EN_MASK (((1 << 1) - 1) << 26)

/* Bits of SYS_CONTROL2 (0x00000008) */

#define YMODE_OFFSET 0
#define YMODE_BITS 1
#define YMODE_MASK (((1 << 1) - 1) << 0)

#define YBT709_OFFSET 1
#define YBT709_BITS 1
#define YBT709_MASK (((1 << 1) - 1) << 1)

#define LFSR_MODE_OFFSET 2
#define LFSR_MODE_BITS 1
#define LFSR_MODE_MASK (((1 << 1) - 1) << 2)

/* Bits of SYS_CONTROL3 (0x0000000c) */

#define DEBUG_SEL_OFFSET 8
#define DEBUG_SEL_BITS 3
#define DEBUG_SEL_MASK (((1 << 3) - 1) << 8)

/* Bits of SYS_INT_EN0 (0x00000010) */

#define FRAME_START_INT_EN_OFFSET 0
#define FRAME_START_INT_EN_BITS 1
#define FRAME_START_INT_EN_MASK (((1 << 1) - 1) << 0)

#define FRAME_END_INT_EN_OFFSET 1
#define FRAME_END_INT_EN_BITS 1
#define FRAME_END_INT_EN_MASK (((1 << 1) - 1) << 1)

#define FRAME_S_START_INT_EN_OFFSET 2
#define FRAME_S_START_INT_EN_BITS 1
#define FRAME_S_START_INT_EN_MASK (((1 << 1) - 1) << 2)

#define FRAME_S_END_INT_EN_OFFSET 3
#define FRAME_S_END_INT_EN_BITS 1
#define FRAME_S_END_INT_EN_MASK (((1 << 1) - 1) << 3)

#define DATA_START_IN_EN_OFFSET 8
#define DATA_START_IN_EN_BITS 1
#define DATA_START_IN_EN_MASK (((1 << 1) - 1) << 8)

#define DATA_END_INT_EN_OFFSET 9
#define DATA_END_INT_EN_BITS 1
#define DATA_END_INT_EN_MASK (((1 << 1) - 1) << 9)

#define DATA_S_START_INT_EN_OFFSET 10
#define DATA_S_START_INT_EN_BITS 1
#define DATA_S_START_INT_EN_MASK (((1 << 1) - 1) << 10)

#define DATA_S_END_INT_EN_OFFSET 11
#define DATA_S_END_INT_EN_BITS 1
#define DATA_S_END_INT_EN_MASK (((1 << 1) - 1) << 11)

#define CROP_DATA_START_INT_EN_OFFSET 16
#define CROP_DATA_START_INT_EN_BITS 1
#define CROP_DATA_START_INT_EN_MASK (((1 << 1) - 1) << 16)

#define CROP_DATA_END_INT_EN_OFFSET 17
#define CROP_DATA_END_INT_EN_BITS 1
#define CROP_DATA_END_INT_EN_MASK (((1 << 1) - 1) << 17)

#define CROP_DATA_S_START_INT_EN_OFFSET 18
#define CROP_DATA_S_START_INT_EN_BITS 1
#define CROP_DATA_S_START_INT_EN_MASK (((1 << 1) - 1) << 18)

#define CROP_DATA_S_END_INT_EN_OFFSET 19
#define CROP_DATA_S_END_INT_EN_BITS 1
#define CROP_DATA_S_END_INT_EN_MASK (((1 << 1) - 1) << 19)

#define DATA_START_OUT_INT_EN_OFFSET 24
#define DATA_START_OUT_INT_EN_BITS 1
#define DATA_START_OUT_INT_EN_MASK (((1 << 1) - 1) << 24)

#define DATA_END_OUT_INT_EN_OFFSET 25
#define DATA_END_OUT_INT_EN_BITS 1
#define DATA_END_OUT_INT_EN_MASK (((1 << 1) - 1) << 25)

#define LINE_CNT_OUT_INT_EN_OFFSET 26
#define LINE_CNT_OUT_INT_EN_BITS 1
#define LINE_CNT_OUT_INT_EN_MASK (((1 << 1) - 1) << 26)

#define TPNR_FRAME_BROKEN_INT_EN_OFFSET 27
#define TPNR_FRAME_BROKEN_INT_EN_BITS 1
#define TPNR_FRAME_BROKEN_INT_EN_MASK (((1 << 1) - 1) << 27)

/* Bits of SYS_INT_CLR0 (0x00000014) */

#define FRAME_START_INT_CLR_OFFSET 0
#define FRAME_START_INT_CLR_BITS 1
#define FRAME_START_INT_CLR_MASK (((1 << 1) - 1) << 0)

#define FRAME_CLRD_INT_CLR_OFFSET 1
#define FRAME_CLRD_INT_CLR_BITS 1
#define FRAME_CLRD_INT_CLR_MASK (((1 << 1) - 1) << 1)

#define FRAME_S_START_INT_CLR_OFFSET 2
#define FRAME_S_START_INT_CLR_BITS 1
#define FRAME_S_START_INT_CLR_MASK (((1 << 1) - 1) << 2)

#define FRAME_S_CLRD_INT_CLR_OFFSET 3
#define FRAME_S_CLRD_INT_CLR_BITS 1
#define FRAME_S_CLRD_INT_CLR_MASK (((1 << 1) - 1) << 3)

#define DATA_START_IN_CLR_OFFSET 8
#define DATA_START_IN_CLR_BITS 1
#define DATA_START_IN_CLR_MASK (((1 << 1) - 1) << 8)

#define DATA_CLRD_INT_CLR_OFFSET 9
#define DATA_CLRD_INT_CLR_BITS 1
#define DATA_CLRD_INT_CLR_MASK (((1 << 1) - 1) << 9)

#define DATA_S_START_INT_CLR_OFFSET 10
#define DATA_S_START_INT_CLR_BITS 1
#define DATA_S_START_INT_CLR_MASK (((1 << 1) - 1) << 10)

#define DATA_S_CLRD_INT_CLR_OFFSET 11
#define DATA_S_CLRD_INT_CLR_BITS 1
#define DATA_S_CLRD_INT_CLR_MASK (((1 << 1) - 1) << 11)

#define CROP_DATA_START_INT_CLR_OFFSET 16
#define CROP_DATA_START_INT_CLR_BITS 1
#define CROP_DATA_START_INT_CLR_MASK (((1 << 1) - 1) << 16)

#define CROP_DATA_CLRD_INT_CLR_OFFSET 17
#define CROP_DATA_CLRD_INT_CLR_BITS 1
#define CROP_DATA_CLRD_INT_CLR_MASK (((1 << 1) - 1) << 17)

#define CROP_DATA_S_START_INT_CLR_OFFSET 18
#define CROP_DATA_S_START_INT_CLR_BITS 1
#define CROP_DATA_S_START_INT_CLR_MASK (((1 << 1) - 1) << 18)

#define CROP_DATA_S_CLRD_INT_CLR_OFFSET 19
#define CROP_DATA_S_CLRD_INT_CLR_BITS 1
#define CROP_DATA_S_CLRD_INT_CLR_MASK (((1 << 1) - 1) << 19)

#define DATA_START_OUT_INT_CLR_OFFSET 24
#define DATA_START_OUT_INT_CLR_BITS 1
#define DATA_START_OUT_INT_CLR_MASK (((1 << 1) - 1) << 24)

#define DATA_CLRD_OUT_INT_CLR_OFFSET 25
#define DATA_CLRD_OUT_INT_CLR_BITS 1
#define DATA_CLRD_OUT_INT_CLR_MASK (((1 << 1) - 1) << 25)

#define LINE_CNT_OUT_INT_CLR_OFFSET 26
#define LINE_CNT_OUT_INT_CLR_BITS 1
#define LINE_CNT_OUT_INT_CLR_MASK (((1 << 1) - 1) << 26)

#define TPNR_FRAME_BROKEN_INT_CLR_OFFSET 27
#define TPNR_FRAME_BROKEN_INT_CLR_BITS 1
#define TPNR_FRAME_BROKEN_INT_CLR_MASK (((1 << 1) - 1) << 27)

/* Bits of SYS_INT_EN1 (0x00000018) */

#define AE_2_STATIS_INT_EN_OFFSET 0
#define AE_2_STATIS_INT_EN_BITS 1
#define AE_2_STATIS_INT_EN_MASK (((1 << 1) - 1) << 0)

#define AE_1_STATIS_INT_EN_OFFSET 1
#define AE_1_STATIS_INT_EN_BITS 1
#define AE_1_STATIS_INT_EN_MASK (((1 << 1) - 1) << 1)

#define AE_0_STATIS_INT_EN_OFFSET 4
#define AE_0_STATIS_INT_EN_BITS 1
#define AE_0_STATIS_INT_EN_MASK (((1 << 1) - 1) << 4)

#define RAW_STATIS_INT_EN_OFFSET 5
#define RAW_STATIS_INT_EN_BITS 1
#define RAW_STATIS_INT_EN_MASK (((1 << 1) - 1) << 5)

#define FLICK_INT_EN_OFFSET 6
#define FLICK_INT_EN_BITS 1
#define FLICK_INT_EN_MASK (((1 << 1) - 1) << 6)

#define AWB_STATIS_INT_EN_OFFSET 7
#define AWB_STATIS_INT_EN_BITS 1
#define AWB_STATIS_INT_EN_MASK (((1 << 1) - 1) << 7)

#define AF1_STATIS_INT_EN_OFFSET 8
#define AF1_STATIS_INT_EN_BITS 1
#define AF1_STATIS_INT_EN_MASK (((1 << 1) - 1) << 8)

#define YUV_STATIS_INT_EN_OFFSET 9
#define YUV_STATIS_INT_EN_BITS 1
#define YUV_STATIS_INT_EN_MASK (((1 << 1) - 1) << 9)

#define AF0_STATIS_INT_EN_OFFSET 10
#define AF0_STATIS_INT_EN_BITS 1
#define AF0_STATIS_INT_EN_MASK (((1 << 1) - 1) << 10)

#define FLICK_S_INT_EN_OFFSET 11
#define FLICK_S_INT_EN_BITS 1
#define FLICK_S_INT_EN_MASK (((1 << 1) - 1) << 11)

#define STATIS_DONE_INT_EN_OFFSET 31
#define STATIS_DONE_INT_EN_BITS 1
#define STATIS_DONE_INT_EN_MASK (((1 << 1) - 1) << 31)

/* Bits of SYS_INT_CLR1 (0x0000001c) */

#define AE_2_STATIS_INT_CLR_OFFSET 0
#define AE_2_STATIS_INT_CLR_BITS 1
#define AE_2_STATIS_INT_CLR_MASK (((1 << 1) - 1) << 0)

#define AE_1_STATIS_INT_CLR_OFFSET 1
#define AE_1_STATIS_INT_CLR_BITS 1
#define AE_1_STATIS_INT_CLR_MASK (((1 << 1) - 1) << 1)

#define AE_0_STATIS_INT_CLR_OFFSET 4
#define AE_0_STATIS_INT_CLR_BITS 1
#define AE_0_STATIS_INT_CLR_MASK (((1 << 1) - 1) << 4)

#define RAW_STATIS_INT_CLR_OFFSET 5
#define RAW_STATIS_INT_CLR_BITS 1
#define RAW_STATIS_INT_CLR_MASK (((1 << 1) - 1) << 5)

#define FLICK_INT_CLR_OFFSET 6
#define FLICK_INT_CLR_BITS 1
#define FLICK_INT_CLR_MASK (((1 << 1) - 1) << 6)

#define AWB_STATIS_INT_CLR_OFFSET 7
#define AWB_STATIS_INT_CLR_BITS 1
#define AWB_STATIS_INT_CLR_MASK (((1 << 1) - 1) << 7)

#define AF1_STATIS_INT_CLR_OFFSET 8
#define AF1_STATIS_INT_CLR_BITS 1
#define AF1_STATIS_INT_CLR_MASK (((1 << 1) - 1) << 8)

#define YUV_STATIS_INT_CLR_OFFSET 9
#define YUV_STATIS_INT_CLR_BITS 1
#define YUV_STATIS_INT_CLR_MASK (((1 << 1) - 1) << 9)

#define AF0_STATIS_INT_CLR_OFFSET 10
#define AF0_STATIS_INT_CLR_BITS 1
#define AF0_STATIS_INT_CLR_MASK (((1 << 1) - 1) << 10)

#define FLICK_S_INT_CLR_OFFSET 11
#define FLICK_S_INT_CLR_BITS 1
#define FLICK_S_INT_CLR_MASK (((1 << 1) - 1) << 11)

#define STATIS_INT_CLR_OFFSET 31
#define STATIS_INT_CLR_BITS 1
#define STATIS_INT_CLR_MASK (((1 << 1) - 1) << 31)

/* Bits of SYS_FRAME_CNT (0x00000020) */

#define FCNT_CLR_OFFSET 0
#define FCNT_CLR_BITS 1
#define FCNT_CLR_MASK (((1 << 1) - 1) << 0)

#define FSCNT_CLR_OFFSET 8
#define FSCNT_CLR_BITS 1
#define FSCNT_CLR_MASK (((1 << 1) - 1) << 8)

/* Bits of SYS_ERROR_CNT (0x00000024) */

#define ERRCNT_CLR_OFFSET 0
#define ERRCNT_CLR_BITS 1
#define ERRCNT_CLR_MASK (((1 << 1) - 1) << 0)

/* Bits of SYS_FRAME_IDLE (0x00000028) */

#define VHDRTM_MEM_IS_IDLE_OFFSET 0
#define VHDRTM_MEM_IS_IDLE_BITS 1
#define VHDRTM_MEM_IS_IDLE_MASK (((1 << 1) - 1) << 0)

#define VHDRFS_MEM_IS_IDLE_OFFSET 1
#define VHDRFS_MEM_IS_IDLE_BITS 1
#define VHDRFS_MEM_IS_IDLE_MASK (((1 << 1) - 1) << 1)

#define RAW3DNR_PRE_MEM_IS_IDLE_OFFSET 2
#define RAW3DNR_PRE_MEM_IS_IDLE_BITS 1
#define RAW3DNR_PRE_MEM_IS_IDLE_MASK (((1 << 1) - 1) << 2)

#define RAW3DNR_MEM_IS_IDLE_OFFSET 3
#define RAW3DNR_MEM_IS_IDLE_BITS 1
#define RAW3DNR_MEM_IS_IDLE_MASK (((1 << 1) - 1) << 3)

#define MTD_MEM_IS_IDLE_OFFSET 4
#define MTD_MEM_IS_IDLE_BITS 1
#define MTD_MEM_IS_IDLE_MASK (((1 << 1) - 1) << 4)

#define DPC_S_MEM_IS_IDLE_OFFSET 5
#define DPC_S_MEM_IS_IDLE_BITS 1
#define DPC_S_MEM_IS_IDLE_MASK (((1 << 1) - 1) << 5)

#define DPC_L_MEM_IS_IDLE_OFFSET 6
#define DPC_L_MEM_IS_IDLE_BITS 1
#define DPC_L_MEM_IS_IDLE_MASK (((1 << 1) - 1) << 6)

#define INTP_MEM_IS_IDLE_OFFSET 7
#define INTP_MEM_IS_IDLE_BITS 1
#define INTP_MEM_IS_IDLE_MASK (((1 << 1) - 1) << 7)

#define EEH_MEM_IS_IDLE_OFFSET 8
#define EEH_MEM_IS_IDLE_BITS 1
#define EEH_MEM_IS_IDLE_MASK (((1 << 1) - 1) << 8)

/* Bits of SYS_SPEED_CTRL (0x0000002c) */

#define RAW_SPEED_DIV_OFFSET 0
#define RAW_SPEED_DIV_BITS 5
#define RAW_SPEED_DIV_MASK (((1 << 5) - 1) << 0)

#define RAW_SPEED_MULT_OFFSET 5
#define RAW_SPEED_MULT_BITS 3
#define RAW_SPEED_MULT_MASK (((1 << 3) - 1) << 5)

#define RGB_SPEED_DIV_OFFSET 8
#define RGB_SPEED_DIV_BITS 5
#define RGB_SPEED_DIV_MASK (((1 << 5) - 1) << 8)

#define RGB_SPEED_MULT_OFFSET 13
#define RGB_SPEED_MULT_BITS 3
#define RGB_SPEED_MULT_MASK (((1 << 3) - 1) << 13)

#define YUV_SPEED_DIV_OFFSET 16
#define YUV_SPEED_DIV_BITS 5
#define YUV_SPEED_DIV_MASK (((1 << 5) - 1) << 16)

#define YUV_SPEED_MULT_OFFSET 21
#define YUV_SPEED_MULT_BITS 3
#define YUV_SPEED_MULT_MASK (((1 << 3) - 1) << 21)

/* Bits of SYS_LAST_BREAK (0x00000030) */

#define RAW_LAST_BREAK_OFFSET 0
#define RAW_LAST_BREAK_BITS 10
#define RAW_LAST_BREAK_MASK (((1 << 10) - 1) << 0)

#define RGB_LAST_BREAK_OFFSET 10
#define RGB_LAST_BREAK_BITS 10
#define RGB_LAST_BREAK_MASK (((1 << 10) - 1) << 10)

#define YUV_LAST_BREAK_OFFSET 20
#define YUV_LAST_BREAK_BITS 10
#define YUV_LAST_BREAK_MASK (((1 << 10) - 1) << 20)

/* Bits of SYS_LOW_DISABLE (0x00000038) */

#define LOW_DISABLE_OFFSET 0
#define LOW_DISABLE_BITS 1
#define LOW_DISABLE_MASK (((1 << 1) - 1) << 0)

#define LOW_DISABLE2_OFFSET 1
#define LOW_DISABLE2_BITS 1
#define LOW_DISABLE2_MASK (((1 << 1) - 1) << 1)

#define RAW_UV_DISABLE_OFFSET 2
#define RAW_UV_DISABLE_BITS 1
#define RAW_UV_DISABLE_MASK (((1 << 1) - 1) << 2)

/* Bits of SYS_DATA_ENABLE (0x0000003c) */

#define DATA_EN_OFFSET 0
#define DATA_EN_BITS 1
#define DATA_EN_MASK (((1 << 1) - 1) << 0)

/* Bits of SYS_INPUT_CTRL (0x00000044) */

#define MIPI_EN_OFFSET 0
#define MIPI_EN_BITS 1
#define MIPI_EN_MASK (((1 << 1) - 1) << 0)

#define FP_SEL_OFFSET 1
#define FP_SEL_BITS 1
#define FP_SEL_MASK (((1 << 1) - 1) << 1)

/* Bits of SYS_IMAGE_SEL0 (0x00000048) */

#define IMAGE_SEL0_OFFSET 0
#define IMAGE_SEL0_BITS 5
#define IMAGE_SEL0_MASK (((1 << 5) - 1) << 0)

/* Bits of SYS_IMAGE_SEL1 (0x0000004c) */

#define IMG_SEL_OFFSET 0
#define IMG_SEL_BITS 5
#define IMG_SEL_MASK (((1 << 5) - 1) << 0)

#define IMG_SEL_EN_OFFSET 8
#define IMG_SEL_EN_BITS 1
#define IMG_SEL_EN_MASK (((1 << 1) - 1) << 8)

/* Bits of SYS_PATH_SEL (0x00000050) */

#define PATH_B_SEL_OFFSET 0
#define PATH_B_SEL_BITS 1
#define PATH_B_SEL_MASK (((1 << 1) - 1) << 0)

#define PATH_C_SEL_OFFSET 1
#define PATH_C_SEL_BITS 1
#define PATH_C_SEL_MASK (((1 << 1) - 1) << 1)

#define PATH_D_SEL_OFFSET 2
#define PATH_D_SEL_BITS 1
#define PATH_D_SEL_MASK (((1 << 1) - 1) << 2)

#define IMAGE_SOURCE_OFFSET 7
#define IMAGE_SOURCE_BITS 1
#define IMAGE_SOURCE_MASK (((1 << 1) - 1) << 7)

/* Bits of SYS_IMAGE_MODE_SEL (0x0000005c) */

#define DOUT_MODE0_OFFSET 0
#define DOUT_MODE0_BITS 1
#define DOUT_MODE0_MASK (((1 << 1) - 1) << 0)

/* Bits of SYS_IMAGE_CHAN_SEL (0x00000060) */

#define DOUT_CHAN_SEL0_OFFSET 0
#define DOUT_CHAN_SEL0_BITS 2
#define DOUT_CHAN_SEL0_MASK (((1 << 2) - 1) << 0)

/* Bits of SYS_ISP_CLK_CTRL (0x00000064) */

#define DUMMY_CLK_ON_OFFSET 0
#define DUMMY_CLK_ON_BITS 1
#define DUMMY_CLK_ON_MASK (((1 << 1) - 1) << 0)

#define MTD_FUNC_CLK_ON_OFFSET 1
#define MTD_FUNC_CLK_ON_BITS 1
#define MTD_FUNC_CLK_ON_MASK (((1 << 1) - 1) << 1)

#define HDR_FUNC_CLK_ON_OFFSET 2
#define HDR_FUNC_CLK_ON_BITS 1
#define HDR_FUNC_CLK_ON_MASK (((1 << 1) - 1) << 2)

#define UVT_FUNC_CLK_ON_OFFSET 3
#define UVT_FUNC_CLK_ON_BITS 1
#define UVT_FUNC_CLK_ON_MASK (((1 << 1) - 1) << 3)

#define LDC_FUNC_CLK_ON_OFFSET 4
#define LDC_FUNC_CLK_ON_BITS 1
#define LDC_FUNC_CLK_ON_MASK (((1 << 1) - 1) << 4)

#define YUVSIN_FUNC_CLK_ON_OFFSET 5
#define YUVSIN_FUNC_CLK_ON_BITS 1
#define YUVSIN_FUNC_CLK_ON_MASK (((1 << 1) - 1) << 5)

#define YUV2RGB_FUNC_CLK_ON_OFFSET 6
#define YUV2RGB_FUNC_CLK_ON_BITS 1
#define YUV2RGB_FUNC_CLK_ON_MASK (((1 << 1) - 1) << 6)

/* Bits of SYS_LCNT_INT_THD (0x00000068) */

#define LCNT_INT_THD_OFFSET 0
#define LCNT_INT_THD_BITS 12
#define LCNT_INT_THD_MASK (((1 << 12) - 1) << 0)

/* Bits of SYS_HOLE_ADDR_CTRL (0x0000006c) */

#define HOLE_ADDR_CLR_OFFSET 0
#define HOLE_ADDR_CLR_BITS 1
#define HOLE_ADDR_CLR_MASK (((1 << 1) - 1) << 0)

/* Bits of ISP_SYS_DUMMY0 (0x00000070) */

#define ISP_DUMMY_0_OFFSET 0
#define ISP_DUMMY_0_BITS 8
#define ISP_DUMMY_0_MASK (((1 << 8) - 1) << 0)

#define ISP_DUMMY_1_OFFSET 8
#define ISP_DUMMY_1_BITS 8
#define ISP_DUMMY_1_MASK (((1 << 8) - 1) << 8)

#define ISP_DUMMY_2_OFFSET 16
#define ISP_DUMMY_2_BITS 8
#define ISP_DUMMY_2_MASK (((1 << 8) - 1) << 16)

#define ISP_DUMMY_3_OFFSET 24
#define ISP_DUMMY_3_BITS 8
#define ISP_DUMMY_3_MASK (((1 << 8) - 1) << 24)

/* Bits of ISP_SYS_DUMMY1 (0x00000074) */

#define ISP_DUMMY_4_OFFSET 0
#define ISP_DUMMY_4_BITS 8
#define ISP_DUMMY_4_MASK (((1 << 8) - 1) << 0)

#define ISP_DUMMY_5_OFFSET 8
#define ISP_DUMMY_5_BITS 8
#define ISP_DUMMY_5_MASK (((1 << 8) - 1) << 8)

#define ISP_DUMMY_6_OFFSET 16
#define ISP_DUMMY_6_BITS 8
#define ISP_DUMMY_6_MASK (((1 << 8) - 1) << 16)

#define ISP_DUMMY_7_OFFSET 24
#define ISP_DUMMY_7_BITS 8
#define ISP_DUMMY_7_MASK (((1 << 8) - 1) << 24)

/* Bits of ISP_SYS_DUMMY2 (0x00000078) */

#define ISP_DUMMY_8_OFFSET 0
#define ISP_DUMMY_8_BITS 8
#define ISP_DUMMY_8_MASK (((1 << 8) - 1) << 0)

#define ISP_DUMMY_9_OFFSET 8
#define ISP_DUMMY_9_BITS 8
#define ISP_DUMMY_9_MASK (((1 << 8) - 1) << 8)

#define ISP_DUMMY_A_OFFSET 16
#define ISP_DUMMY_A_BITS 8
#define ISP_DUMMY_A_MASK (((1 << 8) - 1) << 16)

#define ISP_DUMMY_B_OFFSET 24
#define ISP_DUMMY_B_BITS 8
#define ISP_DUMMY_B_MASK (((1 << 8) - 1) << 24)

/* Bits of ISP_SYS_BAYER_PATTERN (0x0000007c) */

#define SENSOR_START_CH_OFFSET 0
#define SENSOR_START_CH_BITS 2
#define SENSOR_START_CH_MASK (((1 << 2) - 1) << 0)

/* Bits of ISP_PG_MEM (0x00000080) */

#define TPNR_PG_SD_OFFSET 0
#define TPNR_PG_SD_BITS 1
#define TPNR_PG_SD_MASK (((1 << 1) - 1) << 0)

#define TPNR_PG_DS_OFFSET 1
#define TPNR_PG_DS_BITS 1
#define TPNR_PG_DS_MASK (((1 << 1) - 1) << 1)

/* Bits of SYS_TPNR_COMP (0x00000084) */

#define TPNR_COMP_EN_OFFSET 0
#define TPNR_COMP_EN_BITS 1
#define TPNR_COMP_EN_MASK (((1 << 1) - 1) << 0)

#define TPNR_DCOMP_EN_OFFSET 1
#define TPNR_DCOMP_EN_BITS 1
#define TPNR_DCOMP_EN_MASK (((1 << 1) - 1) << 1)

/* Bits of SYS_SENSOR_COMP (0x00000088) */

#define SENSOR_COMP_EN_OFFSET 0
#define SENSOR_COMP_EN_BITS 1
#define SENSOR_COMP_EN_MASK (((1 << 1) - 1) << 0)

#define SENSOR_DCOMP_EN_OFFSET 1
#define SENSOR_DCOMP_EN_BITS 1
#define SENSOR_DCOMP_EN_MASK (((1 << 1) - 1) << 1)

/* Bits of SYS_VHDR_SEL (0x0000008c) */

#define VHDR_INL_SEL_OFFSET 0
#define VHDR_INL_SEL_BITS 1
#define VHDR_INL_SEL_MASK (((1 << 1) - 1) << 0)

#define VHDR_INS_SEL_OFFSET 1
#define VHDR_INS_SEL_BITS 1
#define VHDR_INS_SEL_MASK (((1 << 1) - 1) << 1)

/* Bits of SYS_BIT_CLIP (0x00000090) */

#define CLIP_18BIT_OFFSET 0
#define CLIP_18BIT_BITS 1
#define CLIP_18BIT_MASK (((1 << 1) - 1) << 0)

#define CLIP_16BIT_OFFSET 1
#define CLIP_16BIT_BITS 1
#define CLIP_16BIT_MASK (((1 << 1) - 1) << 1)

#define CLIP_14BIT_OFFSET 2
#define CLIP_14BIT_BITS 1
#define CLIP_14BIT_MASK (((1 << 1) - 1) << 2)

#define CLIP_12BIT_OFFSET 3
#define CLIP_12BIT_BITS 1
#define CLIP_12BIT_MASK (((1 << 1) - 1) << 3)

/* Bits of SYS_POS_SEL (0x00000094) */

#define DECOMP_SEL_OFFSET 0
#define DECOMP_SEL_BITS 1
#define DECOMP_SEL_MASK (((1 << 1) - 1) << 0)

#define EEH_POS_SEL_OFFSET 1
#define EEH_POS_SEL_BITS 1
#define EEH_POS_SEL_MASK (((1 << 1) - 1) << 1)

#define TPNR_POS_SEL_OFFSET 2
#define TPNR_POS_SEL_BITS 2
#define TPNR_POS_SEL_MASK (((1 << 2) - 1) << 2)

#define AF_IN_SEL_OFFSET 4
#define AF_IN_SEL_BITS 1
#define AF_IN_SEL_MASK (((1 << 1) - 1) << 4)

#define AE_GAIN_L_SEL_OFFSET 5
#define AE_GAIN_L_SEL_BITS 1
#define AE_GAIN_L_SEL_MASK (((1 << 1) - 1) << 5)

#define BLC_S_POS_SEL_OFFSET 6
#define BLC_S_POS_SEL_BITS 1
#define BLC_S_POS_SEL_MASK (((1 << 1) - 1) << 6)

#define BLC_L_POS_SEL_OFFSET 7
#define BLC_L_POS_SEL_BITS 1
#define BLC_L_POS_SEL_MASK (((1 << 1) - 1) << 7)

/* Bits of SYS_DATA_DELAY_TRIG (0x00000098) */

#define ISP_DATA_TRIG_OFFSET 0
#define ISP_DATA_TRIG_BITS 1
#define ISP_DATA_TRIG_MASK (((1 << 1) - 1) << 0)

/* Bits of SYS_DATA_DELAY (0x0000009c) */

#define ISP_DATA_NUM_OFFSET 0
#define ISP_DATA_NUM_BITS 7
#define ISP_DATA_NUM_MASK (((1 << 7) - 1) << 0)

#define ISP_DATA_EN_OFFSET 8
#define ISP_DATA_EN_BITS 1
#define ISP_DATA_EN_MASK (((1 << 1) - 1) << 8)

#endif /* _ISP_SYS_H_INC_ */
