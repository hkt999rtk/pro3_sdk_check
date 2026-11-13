
//tool_version:1.4.35

#ifndef _YGAMMA_H_INC_
#define _YGAMMA_H_INC_

#include <isp_reg.h>
#include <isp_reg_base.h>

#define YGAMMA_SYNC (YGAMMA_BASE_ADDR + 0x00000000) //0x00005c00
#define YGAMMA_PARAM0 (YGAMMA_BASE_ADDR + 0x00000010) //0x00005c10
#define YGAMMA_PARAM1 (YGAMMA_BASE_ADDR + 0x00000014) //0x00005c14
#define YGAMMA_PARAM2 (YGAMMA_BASE_ADDR + 0x00000018) //0x00005c18
#define YGAMMA_PARAM3 (YGAMMA_BASE_ADDR + 0x0000001c) //0x00005c1c
#define YGAMMA_PARAM0_SYNC (YGAMMA_BASE_ADDR + 0x00000020) //0x00005c20
#define YGAMMA_PARAM1_SYNC (YGAMMA_BASE_ADDR + 0x00000024) //0x00005c24
#define YGAMMA_PARAM2_SYNC (YGAMMA_BASE_ADDR + 0x00000028) //0x00005c28
#define YGAMMA_PARAM3_SYNC (YGAMMA_BASE_ADDR + 0x0000002c) //0x00005c2c

/* Bits of YGAMMA_SYNC (0x00000000) */

#define YGAMMA_PLOAD_DLY_OFFSET 0
#define YGAMMA_PLOAD_DLY_BITS 1
#define YGAMMA_PLOAD_DLY_MASK (((1 << 1) - 1) << 0)

/* Bits of YGAMMA_PARAM0 (0x00000010) */

#define YGAMMA_P0_OFFSET 0
#define YGAMMA_P0_BITS 8
#define YGAMMA_P0_MASK (((1 << 8) - 1) << 0)

#define YGAMMA_P1_OFFSET 8
#define YGAMMA_P1_BITS 8
#define YGAMMA_P1_MASK (((1 << 8) - 1) << 8)

#define YGAMMA_P2_OFFSET 16
#define YGAMMA_P2_BITS 8
#define YGAMMA_P2_MASK (((1 << 8) - 1) << 16)

#define YGAMMA_P3_OFFSET 24
#define YGAMMA_P3_BITS 8
#define YGAMMA_P3_MASK (((1 << 8) - 1) << 24)

/* Bits of YGAMMA_PARAM1 (0x00000014) */

#define YGAMMA_P4_OFFSET 0
#define YGAMMA_P4_BITS 8
#define YGAMMA_P4_MASK (((1 << 8) - 1) << 0)

#define YGAMMA_P5_OFFSET 8
#define YGAMMA_P5_BITS 8
#define YGAMMA_P5_MASK (((1 << 8) - 1) << 8)

#define YGAMMA_P6_OFFSET 16
#define YGAMMA_P6_BITS 8
#define YGAMMA_P6_MASK (((1 << 8) - 1) << 16)

#define YGAMMA_P7_OFFSET 24
#define YGAMMA_P7_BITS 8
#define YGAMMA_P7_MASK (((1 << 8) - 1) << 24)

/* Bits of YGAMMA_PARAM2 (0x00000018) */

#define YGAMMA_P8_OFFSET 0
#define YGAMMA_P8_BITS 8
#define YGAMMA_P8_MASK (((1 << 8) - 1) << 0)

#define YGAMMA_P9_OFFSET 8
#define YGAMMA_P9_BITS 8
#define YGAMMA_P9_MASK (((1 << 8) - 1) << 8)

#define YGAMMA_PA_OFFSET 16
#define YGAMMA_PA_BITS 8
#define YGAMMA_PA_MASK (((1 << 8) - 1) << 16)

#define YGAMMA_PB_OFFSET 24
#define YGAMMA_PB_BITS 8
#define YGAMMA_PB_MASK (((1 << 8) - 1) << 24)

/* Bits of YGAMMA_PARAM3 (0x0000001c) */

#define YGAMMA_PC_OFFSET 0
#define YGAMMA_PC_BITS 8
#define YGAMMA_PC_MASK (((1 << 8) - 1) << 0)

#define YGAMMA_PD_OFFSET 8
#define YGAMMA_PD_BITS 8
#define YGAMMA_PD_MASK (((1 << 8) - 1) << 8)

#define YGAMMA_PE_OFFSET 16
#define YGAMMA_PE_BITS 8
#define YGAMMA_PE_MASK (((1 << 8) - 1) << 16)

#define YGAMMA_PF_OFFSET 24
#define YGAMMA_PF_BITS 8
#define YGAMMA_PF_MASK (((1 << 8) - 1) << 24)

/* Bits of YGAMMA_PARAM0_SYNC (0x00000020) */

#define YGAMMA_P0_SYNC_OFFSET 0
#define YGAMMA_P0_SYNC_BITS 8
#define YGAMMA_P0_SYNC_MASK (((1 << 8) - 1) << 0)

#define YGAMMA_P1_SYNC_OFFSET 8
#define YGAMMA_P1_SYNC_BITS 8
#define YGAMMA_P1_SYNC_MASK (((1 << 8) - 1) << 8)

#define YGAMMA_P2_SYNC_OFFSET 16
#define YGAMMA_P2_SYNC_BITS 8
#define YGAMMA_P2_SYNC_MASK (((1 << 8) - 1) << 16)

#define YGAMMA_P3_SYNC_OFFSET 24
#define YGAMMA_P3_SYNC_BITS 8
#define YGAMMA_P3_SYNC_MASK (((1 << 8) - 1) << 24)

/* Bits of YGAMMA_PARAM1_SYNC (0x00000024) */

#define YGAMMA_P4_SYNC_OFFSET 0
#define YGAMMA_P4_SYNC_BITS 8
#define YGAMMA_P4_SYNC_MASK (((1 << 8) - 1) << 0)

#define YGAMMA_P5_SYNC_OFFSET 8
#define YGAMMA_P5_SYNC_BITS 8
#define YGAMMA_P5_SYNC_MASK (((1 << 8) - 1) << 8)

#define YGAMMA_P6_SYNC_OFFSET 16
#define YGAMMA_P6_SYNC_BITS 8
#define YGAMMA_P6_SYNC_MASK (((1 << 8) - 1) << 16)

#define YGAMMA_P7_SYNC_OFFSET 24
#define YGAMMA_P7_SYNC_BITS 8
#define YGAMMA_P7_SYNC_MASK (((1 << 8) - 1) << 24)

/* Bits of YGAMMA_PARAM2_SYNC (0x00000028) */

#define YGAMMA_P8_SYNC_OFFSET 0
#define YGAMMA_P8_SYNC_BITS 8
#define YGAMMA_P8_SYNC_MASK (((1 << 8) - 1) << 0)

#define YGAMMA_P9_SYNC_OFFSET 8
#define YGAMMA_P9_SYNC_BITS 8
#define YGAMMA_P9_SYNC_MASK (((1 << 8) - 1) << 8)

#define YGAMMA_PA_SYNC_OFFSET 16
#define YGAMMA_PA_SYNC_BITS 8
#define YGAMMA_PA_SYNC_MASK (((1 << 8) - 1) << 16)

#define YGAMMA_PB_SYNC_OFFSET 24
#define YGAMMA_PB_SYNC_BITS 8
#define YGAMMA_PB_SYNC_MASK (((1 << 8) - 1) << 24)

/* Bits of YGAMMA_PARAM3_SYNC (0x0000002c) */

#define YGAMMA_PC_SYNC_OFFSET 0
#define YGAMMA_PC_SYNC_BITS 8
#define YGAMMA_PC_SYNC_MASK (((1 << 8) - 1) << 0)

#define YGAMMA_PD_SYNC_OFFSET 8
#define YGAMMA_PD_SYNC_BITS 8
#define YGAMMA_PD_SYNC_MASK (((1 << 8) - 1) << 8)

#define YGAMMA_PE_SYNC_OFFSET 16
#define YGAMMA_PE_SYNC_BITS 8
#define YGAMMA_PE_SYNC_MASK (((1 << 8) - 1) << 16)

#define YGAMMA_PF_SYNC_OFFSET 24
#define YGAMMA_PF_SYNC_BITS 8
#define YGAMMA_PF_SYNC_MASK (((1 << 8) - 1) << 24)

#endif /* _YGAMMA_H_INC_ */
