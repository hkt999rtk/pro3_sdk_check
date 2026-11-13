
//tool_version:1.4.18

#ifndef _ISP_ZOOM_FHD_H_INC_
#define _ISP_ZOOM_FHD_H_INC_
#define ISP_ZOOM_FHD_BASE_ADDR 0xd8c00

#define R_ISP_ZOOM_UP_CTRL     (ISP_ZOOM_FHD_BASE_ADDR + 0x00000000)    //0xb8ad8c00
#define R_ISP_ZOOM_UP_CROP_START     (ISP_ZOOM_FHD_BASE_ADDR + 0x00000004)    //0xb8ad8c04
#define R_ISP_ZOOM_UP_OUTPUT_SIZE     (ISP_ZOOM_FHD_BASE_ADDR + 0x00000008)    //0xb8ad8c08
#define R_ISP_ZOOM_UP_COEF0     (ISP_ZOOM_FHD_BASE_ADDR + 0x00000010)    //0xb8ad8c10
#define R_ISP_ZOOM_UP_COEF1     (ISP_ZOOM_FHD_BASE_ADDR + 0x00000014)    //0xb8ad8c14
#define R_ISP_ZOOM_UP_COEF2     (ISP_ZOOM_FHD_BASE_ADDR + 0x00000018)    //0xb8ad8c18
#define R_ISP_ZOOM_UP_COEF3     (ISP_ZOOM_FHD_BASE_ADDR + 0x0000001c)    //0xb8ad8c1c
#define R_ISP_ZOOM_UP_COEF4     (ISP_ZOOM_FHD_BASE_ADDR + 0x00000020)    //0xb8ad8c20
#define R_ISP_ZOOM_UP_REG_LOAD     (ISP_ZOOM_FHD_BASE_ADDR + 0x00000024)    //0xb8ad8c24
#define R_ISP_ZOOM_UP_LINE_SPACE     (ISP_ZOOM_FHD_BASE_ADDR + 0x00000028)    //0xb8ad8c28
#define R_ISP_ZOOM_UP_STEP_V     (ISP_ZOOM_FHD_BASE_ADDR + 0x00000048)    //0xb8ad8c48
#define R_ISP_ZOOM_UP_STEP_H     (ISP_ZOOM_FHD_BASE_ADDR + 0x0000004c)    //0xb8ad8c4c
#define R_ISP_ZOOM_UP_BUFF_FULL_INT     (ISP_ZOOM_FHD_BASE_ADDR + 0x00000050)    //0xb8ad8c50
#define R_ISP_ZOOM_UP_BUFF_INT_CLR     (ISP_ZOOM_FHD_BASE_ADDR + 0x00000054)    //0xb8ad8c54
#define R_ISP_ZOOM_UP_BUFF_INT_EN     (ISP_ZOOM_FHD_BASE_ADDR + 0x00000058)    //0xb8ad8c58
#define R_ISP_ZOOM_UP_MEM_IDLE     (ISP_ZOOM_FHD_BASE_ADDR + 0x0000005c)    //0xb8ad8c5c

/* Bits of R_ISP_ZOOM_UP_CTRL (0x00000000) */

#define ZOOM_CH_EN_OFFSET 0
#define ZOOM_CH_EN_BITS   1
#define ZOOM_CH_EN_MASK   (((1 << 1) - 1) << 0)
#define ZOOM_CH_EN        (ZOOM_CH_EN_MASK)

/* Bits of R_ISP_ZOOM_UP_CROP_START (0x00000004) */

#define ZOOM_START_X_OFFSET 0
#define ZOOM_START_X_BITS   16
#define ZOOM_START_X_MASK   (((1 << 16) - 1) << 0)
#define ZOOM_START_X        (ZOOM_START_X_MASK)

#define ZOOM_START_Y_OFFSET 16
#define ZOOM_START_Y_BITS   16
#define ZOOM_START_Y_MASK   (((1 << 16) - 1) << 16)
#define ZOOM_START_Y        (ZOOM_START_Y_MASK)

/* Bits of R_ISP_ZOOM_UP_OUTPUT_SIZE (0x00000008) */

#define BK_FRAME_WIDTH_OFFSET 0
#define BK_FRAME_WIDTH_BITS   16
#define BK_FRAME_WIDTH_MASK   (((1 << 16) - 1) << 0)
#define BK_FRAME_WIDTH        (BK_FRAME_WIDTH_MASK)

#define BK_FRAME_HEIGHT_OFFSET 16
#define BK_FRAME_HEIGHT_BITS   16
#define BK_FRAME_HEIGHT_MASK   (((1 << 16) - 1) << 16)
#define BK_FRAME_HEIGHT        (BK_FRAME_HEIGHT_MASK)

/* Bits of R_ISP_ZOOM_UP_COEF0 (0x00000010) */

#define ZOOM_COEF_0_OFFSET 0
#define ZOOM_COEF_0_BITS   8
#define ZOOM_COEF_0_MASK   (((1 << 8) - 1) << 0)
#define ZOOM_COEF_0        (ZOOM_COEF_0_MASK)

#define ZOOM_COEF_1_OFFSET 8
#define ZOOM_COEF_1_BITS   8
#define ZOOM_COEF_1_MASK   (((1 << 8) - 1) << 8)
#define ZOOM_COEF_1        (ZOOM_COEF_1_MASK)

#define ZOOM_COEF_2_OFFSET 16
#define ZOOM_COEF_2_BITS   8
#define ZOOM_COEF_2_MASK   (((1 << 8) - 1) << 16)
#define ZOOM_COEF_2        (ZOOM_COEF_2_MASK)

#define ZOOM_COEF_3_OFFSET 24
#define ZOOM_COEF_3_BITS   8
#define ZOOM_COEF_3_MASK   (((1 << 8) - 1) << 24)
#define ZOOM_COEF_3        (ZOOM_COEF_3_MASK)

/* Bits of R_ISP_ZOOM_UP_COEF1 (0x00000014) */

#define ZOOM_COEF_4_OFFSET 0
#define ZOOM_COEF_4_BITS   8
#define ZOOM_COEF_4_MASK   (((1 << 8) - 1) << 0)
#define ZOOM_COEF_4        (ZOOM_COEF_4_MASK)

#define ZOOM_COEF_5_OFFSET 8
#define ZOOM_COEF_5_BITS   8
#define ZOOM_COEF_5_MASK   (((1 << 8) - 1) << 8)
#define ZOOM_COEF_5        (ZOOM_COEF_5_MASK)

#define ZOOM_COEF_6_OFFSET 16
#define ZOOM_COEF_6_BITS   8
#define ZOOM_COEF_6_MASK   (((1 << 8) - 1) << 16)
#define ZOOM_COEF_6        (ZOOM_COEF_6_MASK)

#define ZOOM_COEF_7_OFFSET 24
#define ZOOM_COEF_7_BITS   8
#define ZOOM_COEF_7_MASK   (((1 << 8) - 1) << 24)
#define ZOOM_COEF_7        (ZOOM_COEF_7_MASK)

/* Bits of R_ISP_ZOOM_UP_COEF2 (0x00000018) */

#define ZOOM_COEF_8_OFFSET 0
#define ZOOM_COEF_8_BITS   8
#define ZOOM_COEF_8_MASK   (((1 << 8) - 1) << 0)
#define ZOOM_COEF_8        (ZOOM_COEF_8_MASK)

#define ZOOM_COEF_9_OFFSET 8
#define ZOOM_COEF_9_BITS   8
#define ZOOM_COEF_9_MASK   (((1 << 8) - 1) << 8)
#define ZOOM_COEF_9        (ZOOM_COEF_9_MASK)

#define ZOOM_COEF_10_OFFSET 16
#define ZOOM_COEF_10_BITS   8
#define ZOOM_COEF_10_MASK   (((1 << 8) - 1) << 16)
#define ZOOM_COEF_10        (ZOOM_COEF_10_MASK)

#define ZOOM_COEF_11_OFFSET 24
#define ZOOM_COEF_11_BITS   8
#define ZOOM_COEF_11_MASK   (((1 << 8) - 1) << 24)
#define ZOOM_COEF_11        (ZOOM_COEF_11_MASK)

/* Bits of R_ISP_ZOOM_UP_COEF3 (0x0000001c) */

#define ZOOM_COEF_12_OFFSET 0
#define ZOOM_COEF_12_BITS   8
#define ZOOM_COEF_12_MASK   (((1 << 8) - 1) << 0)
#define ZOOM_COEF_12        (ZOOM_COEF_12_MASK)

#define ZOOM_COEF_13_OFFSET 8
#define ZOOM_COEF_13_BITS   8
#define ZOOM_COEF_13_MASK   (((1 << 8) - 1) << 8)
#define ZOOM_COEF_13        (ZOOM_COEF_13_MASK)

#define ZOOM_COEF_14_OFFSET 16
#define ZOOM_COEF_14_BITS   8
#define ZOOM_COEF_14_MASK   (((1 << 8) - 1) << 16)
#define ZOOM_COEF_14        (ZOOM_COEF_14_MASK)

#define ZOOM_COEF_15_OFFSET 24
#define ZOOM_COEF_15_BITS   8
#define ZOOM_COEF_15_MASK   (((1 << 8) - 1) << 24)
#define ZOOM_COEF_15        (ZOOM_COEF_15_MASK)

/* Bits of R_ISP_ZOOM_UP_COEF4 (0x00000020) */

#define ZOOM_COEF_16_OFFSET 0
#define ZOOM_COEF_16_BITS   8
#define ZOOM_COEF_16_MASK   (((1 << 8) - 1) << 0)
#define ZOOM_COEF_16        (ZOOM_COEF_16_MASK)

#define ZOOM_COEF_17_OFFSET 8
#define ZOOM_COEF_17_BITS   8
#define ZOOM_COEF_17_MASK   (((1 << 8) - 1) << 8)
#define ZOOM_COEF_17        (ZOOM_COEF_17_MASK)

#define ZOOM_COEF_18_OFFSET 16
#define ZOOM_COEF_18_BITS   8
#define ZOOM_COEF_18_MASK   (((1 << 8) - 1) << 16)
#define ZOOM_COEF_18        (ZOOM_COEF_18_MASK)

#define ZOOM_COEF_19_OFFSET 24
#define ZOOM_COEF_19_BITS   8
#define ZOOM_COEF_19_MASK   (((1 << 8) - 1) << 24)
#define ZOOM_COEF_19        (ZOOM_COEF_19_MASK)

/* Bits of R_ISP_ZOOM_UP_REG_LOAD (0x00000024) */

#define ZOOM_REG_LATCH_OFFSET 0
#define ZOOM_REG_LATCH_BITS   1
#define ZOOM_REG_LATCH_MASK   (((1 << 1) - 1) << 0)
#define ZOOM_REG_LATCH        (ZOOM_REG_LATCH_MASK)

/* Bits of R_ISP_ZOOM_UP_LINE_SPACE (0x00000028) */

#define LINE_SPACE_OFFSET 0
#define LINE_SPACE_BITS   8
#define LINE_SPACE_MASK   (((1 << 8) - 1) << 0)
#define LINE_SPACE        (LINE_SPACE_MASK)

/* Bits of R_ISP_ZOOM_UP_STEP_V (0x00000048) */

#define ZOOM_STEP_V_OFFSET 0
#define ZOOM_STEP_V_BITS   17
#define ZOOM_STEP_V_MASK   (((1 << 17) - 1) << 0)
#define ZOOM_STEP_V        (ZOOM_STEP_V_MASK)

/* Bits of R_ISP_ZOOM_UP_STEP_H (0x0000004c) */

#define ZOOM_STEP_H_OFFSET 0
#define ZOOM_STEP_H_BITS   17
#define ZOOM_STEP_H_MASK   (((1 << 17) - 1) << 0)
#define ZOOM_STEP_H        (ZOOM_STEP_H_MASK)

/* Bits of R_ISP_ZOOM_UP_BUFF_FULL_INT (0x00000050) */

#define ZOOM_BUFF_FULL_INT_N_OFFSET 0
#define ZOOM_BUFF_FULL_INT_N_BITS   1
#define ZOOM_BUFF_FULL_INT_N_MASK   (((1 << 1) - 1) << 0)
#define ZOOM_BUFF_FULL_INT_N        (ZOOM_BUFF_FULL_INT_N_MASK)

/* Bits of R_ISP_ZOOM_UP_BUFF_INT_CLR (0x00000054) */

#define ZOOM_BUFF_INT_CLR_OFFSET 0
#define ZOOM_BUFF_INT_CLR_BITS   1
#define ZOOM_BUFF_INT_CLR_MASK   (((1 << 1) - 1) << 0)
#define ZOOM_BUFF_INT_CLR        (ZOOM_BUFF_INT_CLR_MASK)

/* Bits of R_ISP_ZOOM_UP_BUFF_INT_EN (0x00000058) */

#define ZOOM_BUFF_INT_EN_OFFSET 0
#define ZOOM_BUFF_INT_EN_BITS   1
#define ZOOM_BUFF_INT_EN_MASK   (((1 << 1) - 1) << 0)
#define ZOOM_BUFF_INT_EN        (ZOOM_BUFF_INT_EN_MASK)

/* Bits of R_ISP_ZOOM_UP_MEM_IDLE (0x0000005c) */

#define ZOOM_MEM_IS_IDLE_OFFSET 0
#define ZOOM_MEM_IS_IDLE_BITS   1
#define ZOOM_MEM_IS_IDLE_MASK   (((1 << 1) - 1) << 0)
#define ZOOM_MEM_IS_IDLE        (ZOOM_MEM_IS_IDLE_MASK)

#endif /* _ISP_ZOOM_FHD_H_INC_ */
