
//tool_version:1.4.35

#ifndef _LDC_X_H_INC_
#define _LDC_X_H_INC_

#include <isp_reg.h>
#include <isp_reg_base.h>

#define LDC_ENABLE (LDC_X_BASE_ADDR + 0x00000000) //0x00005000
#define LDC_SCALECOFE0 (LDC_X_BASE_ADDR + 0x00000010) //0x00005010
#define LDC_SCALECOFE1_CX_SQUARE (LDC_X_BASE_ADDR + 0x00000014) //0x00005014
#define LDC_X_INV0 (LDC_X_BASE_ADDR + 0x00000018) //0x00005018
#define LDC_X_INV1_CX (LDC_X_BASE_ADDR + 0x0000001c) //0x0000501c
#define LDC_Y_INV0 (LDC_X_BASE_ADDR + 0x00000020) //0x00005020
#define LDC_Y_INV1_CY (LDC_X_BASE_ADDR + 0x00000024) //0x00005024
#define LDC_CENTER (LDC_X_BASE_ADDR + 0x00000028) //0x00005028
#define LDC_X0_CX_SQUARE (LDC_X_BASE_ADDR + 0x0000002c) //0x0000502c
#define LDC_Y0_CY_SQUARE (LDC_X_BASE_ADDR + 0x00000030) //0x00005030
#define LDC_SYNC (LDC_X_BASE_ADDR + 0x00000034) //0x00005034

/* Bits of LDC_ENABLE (0x00000000) */

#define LDC_EN_OFFSET 0
#define LDC_EN_BITS 1
#define LDC_EN_MASK (((1 << 1) - 1) << 0)

/* Bits of LDC_SCALECOFE0 (0x00000010) */

#define SCALECOFE0_OFFSET 0
#define SCALECOFE0_BITS 22
#define SCALECOFE0_MASK (((1 << 22) - 1) << 0)

/* Bits of LDC_SCALECOFE1_CX_SQUARE (0x00000014) */

#define SCALECOFE1_CX_SQUARE_OFFSET 0
#define SCALECOFE1_CX_SQUARE_BITS 28
#define SCALECOFE1_CX_SQUARE_MASK (((1 << 28) - 1) << 0)

/* Bits of LDC_X_INV0 (0x00000018) */

#define X_INV0_OFFSET 0
#define X_INV0_BITS 18
#define X_INV0_MASK (((1 << 18) - 1) << 0)

/* Bits of LDC_X_INV1_CX (0x0000001c) */

#define X_INV1_CX_OFFSET 0
#define X_INV1_CX_BITS 23
#define X_INV1_CX_MASK (((1 << 23) - 1) << 0)

/* Bits of LDC_Y_INV0 (0x00000020) */

#define Y_INV0_OFFSET 0
#define Y_INV0_BITS 20
#define Y_INV0_MASK (((1 << 20) - 1) << 0)

/* Bits of LDC_Y_INV1_CY (0x00000024) */

#define Y_INV1_CY_OFFSET 0
#define Y_INV1_CY_BITS 19
#define Y_INV1_CY_MASK (((1 << 19) - 1) << 0)

/* Bits of LDC_CENTER (0x00000028) */

#define X_CENTER_OFFSET 0
#define X_CENTER_BITS 12
#define X_CENTER_MASK (((1 << 12) - 1) << 0)

#define Y_CENTER_OFFSET 16
#define Y_CENTER_BITS 12
#define Y_CENTER_MASK (((1 << 12) - 1) << 16)

/* Bits of LDC_X0_CX_SQUARE (0x0000002c) */

#define X0_CX_SQUARE_OFFSET 0
#define X0_CX_SQUARE_BITS 24
#define X0_CX_SQUARE_MASK (((1 << 24) - 1) << 0)

/* Bits of LDC_Y0_CY_SQUARE (0x00000030) */

#define Y0_CY_SQUARE_OFFSET 0
#define Y0_CY_SQUARE_BITS 23
#define Y0_CY_SQUARE_MASK (((1 << 23) - 1) << 0)

/* Bits of LDC_SYNC (0x00000034) */

#define LDC_REG_LOAD_OFFSET 0
#define LDC_REG_LOAD_BITS 1
#define LDC_REG_LOAD_MASK (((1 << 1) - 1) << 0)

#endif /* _LDC_X_H_INC_ */
