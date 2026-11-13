
//tool_version:1.4.35

#ifndef _MLSC_H_INC_
#define _MLSC_H_INC_

#include <isp_reg.h>
#include <isp_reg_base.h>

#define MLSC_MEM_REG_BASE (MLSC_BASE_ADDR + 0x00000000) //0x0000c000
#define MLSC_MEM_REG_TOP (MLSC_BASE_ADDR + 0x00001fdc) //0x0000dfdc
#define MLSC_CTRL (MLSC_BASE_ADDR + 0x00001ff0) //0x0000dff0

/* Bits of MLSC_MEM_REG (0x00000000~0x00001fdc) */

#define RF_MLSC_R_DAT_OFFSET 0
#define RF_MLSC_R_DAT_BITS 6
#define RF_MLSC_R_DAT_MASK (((1 << 6) - 1) << 0)

#define RF_MLSC_GR_DAT_OFFSET 8
#define RF_MLSC_GR_DAT_BITS 6
#define RF_MLSC_GR_DAT_MASK (((1 << 6) - 1) << 8)

#define RF_MLSC_GB_DAT_OFFSET 16
#define RF_MLSC_GB_DAT_BITS 6
#define RF_MLSC_GB_DAT_MASK (((1 << 6) - 1) << 16)

#define RF_MLSC_B_DAT_OFFSET 24
#define RF_MLSC_B_DAT_BITS 6
#define RF_MLSC_B_DAT_MASK (((1 << 6) - 1) << 24)

/* Bits of MLSC_CTRL (0x00001ff0) */

#define RF_MLSC_MODE_OFFSET 0
#define RF_MLSC_MODE_BITS 2
#define RF_MLSC_MODE_MASK (((1 << 2) - 1) << 0)

#endif /* _MLSC_H_INC_ */
