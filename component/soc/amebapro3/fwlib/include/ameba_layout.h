/**
 * @file ameba_layout.h
 * @brief Memory Layout Configuration Header
 *
 * This file defines the memory layout for AmebaPro3.
 * Modify these values to customize memory regions.
 *
 * NOTE: This header is used by ameba_layout.ld.S via GCC preprocessor
 * Usage: gcc -E -P -I. ameba_layout.ld.S -o ameba_layout.ld
 */

#ifndef AMEBA_LAYOUT_H
#define AMEBA_LAYOUT_H

#include "platform_autoconf.h"

#define KByte 1024
#define MByte 1024 * 1024

/* ========================================================================
 * Base addresses - define these to change memory layout, not used for reference
 * ======================================================================== */
#if 0
#define SYS_SRAM_BASE         0x28000000
#define SYS_SRAM_S_BASE       0x38000000
#define PSRAM_BASE            0x20000000
#define PSRAM_S_BASE          0x30000000
#define DDR_BASE              0x80000000
#define DDR_S_BASE            0xA0000000
#define FLASH_BASE            0x08000000
#define VSRAM_BASE            0x6F000000
#define BT_SHRAM_BASE         0x28080000
#endif

/* ========================================================================
 * ITCM (Instruction Tightly Coupled Memory)
 * ======================================================================== */
#define NP_ITCM_ROM_ORIGIN    0x00000000
#define NP_ITCM_ROM_LENGTH    256 * KByte

#define NP_ITCM_ORIGIN        0x00040000
#define NP_ITCM_LENGTH        16 * KByte

#define VP_ITCM_ORIGIN        0x6F080000
#define VP_ITCM_LENGTH        16 * KByte

/* ========================================================================
 * DTCM (Data Tightly Coupled Memory)
 * ======================================================================== */
#define NP_DTCM_ORIGIN        0x00100000
#define NP_DTCM_LENGTH        16 * KByte

#define NP_DTCM_BSS_ORIGIN    0x00100000
#define NP_DTCM_BSS_LENGTH    4 * KByte

#define NP_DTCM_NOINIT_ORIGIN 0x00101000
#define NP_DTCM_NOINIT_LENGTH 1 * KByte

#define NP_DTCM_RTT_ORIGIN    0x00101400
#define NP_DTCM_RTT_LENGTH    11 * KByte

#define VP_DTCM_ORIGIN        0x6F090000
#define VP_DTCM_LENGTH        16 * KByte

/* ========================================================================
 * System SRAM (SSRAM)
 * ======================================================================== */
#define SYS_SRAM_ORIGIN       0x28000000
#define SYS_SRAM_LENGTH       512 * KByte

/* FP SSRAM */
#define FP_SSRAM_INIT_ORIGIN  0x28000000
#define FP_SSRAM_INIT_LENGTH  4 * KByte

/* NP SSRAM */
#define NP_SSRAM_MSP_ORIGIN   0x28000000
#define NP_SSRAM_MSP_LENGTH   4 * KByte

#define NP_SSRAM_ROMBSS_ORIGIN 0x28001000
#define NP_SSRAM_ROMBSS_LENGTH 3 * KByte

#define NP_SSRAM_MAILBX_ORIGIN 0x28001C00
#define NP_SSRAM_MAILBX_LENGTH 1 * KByte

#if defined(CONFIG_DISABLE_PSRAM)&&(CONFIG_DISABLE_PSRAM==1)
#if !defined(CONFIG_LOADFP)
/* FOR SRAM only, only support NP */

/* Verification only */
#define NP_SSRAM_VERIFY_ORIGIN 0x28010020
#define NP_SSRAM_VERIFY_LENGTH 440 * KByte

#define NP_SSRAM_ORIGIN       0x28010020
#define NP_SSRAM_LENGTH       428 * KByte - 32

#define NP_SSRAM_NC_ORIGIN    0x2807B000
#define NP_SSRAM_NC_LENGTH    8 * KByte

#define AP_SSRAM_RET_ORIGIN   NP_SSRAM_NC_ORIGIN + NP_SSRAM_NC_LENGTH
#define AP_SSRAM_RET_LENGTH   0

/* IPC and SHM */
#define IPC_SSRAM_ORIGIN      0x2807D000
#define IPC_SSRAM_LENGTH      4 * KByte

#define SHM_SSRAM_ORIGIN      0x2807E000
#define SHM_SSRAM_LENGTH      4 * KByte

/* FP SSRAM main */
#define FP_SSRAM_ORIGIN       0x2805F020
#define FP_SSRAM_LENGTH       128 * KByte - 32
#else

/* FOR SRAM only, only support NP and FP */
/* Verification only */
#define NP_SSRAM_VERIFY_ORIGIN 0x2800C020
#define NP_SSRAM_VERIFY_LENGTH 456 * KByte

#define NP_SSRAM_ORIGIN       0x2800C020
#define NP_SSRAM_LENGTH       320 * KByte - 32

#define NP_SSRAM_NC_ORIGIN    0x2805C000
#define NP_SSRAM_NC_LENGTH    8 * KByte

#define AP_SSRAM_RET_ORIGIN   NP_SSRAM_NC_ORIGIN + NP_SSRAM_NC_LENGTH
#define AP_SSRAM_RET_LENGTH   0

/* IPC and SHM */
#define IPC_SSRAM_ORIGIN      0x2805E000
#define IPC_SSRAM_LENGTH      4 * KByte

#define SHM_SSRAM_ORIGIN      0x2805F000
#define SHM_SSRAM_LENGTH      4 * KByte

/* FP SSRAM main */
#define FP_SSRAM_ORIGIN       0x28060020
#define FP_SSRAM_LENGTH       124 * KByte - 32

#endif


#else /* CONFIG_DISABLE_PSRAM not defined, NORMAL case */

/* Verification only */
#define NP_SSRAM_VERIFY_ORIGIN 0x28024000
#define NP_SSRAM_VERIFY_LENGTH 360 * KByte

#define NP_SSRAM_ORIGIN       0x28024020
#define NP_SSRAM_LENGTH       72 * KByte - 32

#define NP_SSRAM_NC_ORIGIN    0x28036000
#define NP_SSRAM_NC_LENGTH    16 * KByte

#define AP_SSRAM_RET_ORIGIN   0x2803A000
#define AP_SSRAM_RET_LENGTH   16 * KByte

/* IPC and SHM */
#define IPC_SSRAM_ORIGIN      0x2803E000
#define IPC_SSRAM_LENGTH      4 * KByte

#define SHM_SSRAM_ORIGIN      0x2803F000
#define SHM_SSRAM_LENGTH      4 * KByte

/* FP SSRAM main */
#define FP_SSRAM_ORIGIN       0x28040020
#define FP_SSRAM_LENGTH       252 * KByte - 32

#define FP_SSRAM_HEAP_EXT_ORIGIN FP_SSRAM_ORIGIN + FP_SSRAM_LENGTH
#define FP_SSRAM_HEAP_EXT_LENGTH 0

#endif /* end if CONFIG_DISABLE_PSRAM */

#define FP_SSRAM_MSP_ORIGIN   0x2807F000
#define FP_SSRAM_MSP_LENGTH   4 * KByte

/* ========================================================================
 * System SRAM (Secure)
 * ======================================================================== */
#define SYS_SRAM_S_ORIGIN     0x38000000
#define SYS_SRAM_S_LENGTH     512 * KByte

#define NP_SSRAM_ROMBSS_S_ORIGIN 0x38002000
#define NP_SSRAM_ROMBSS_S_LENGTH 4 * KByte

#define NP_SSRAM_MSP_S_ORIGIN 0x38003000
#define NP_SSRAM_MSP_S_LENGTH 4 * KByte

#define NP_SSRAM_LOADER_S_ORIGIN 0x38004020
#define NP_SSRAM_LOADER_S_LENGTH 128 * KByte - 32

#define NP_SSRAM_MPBSS_S_ORIGIN 0x38024000
#define NP_SSRAM_MPBSS_S_LENGTH 68 * KByte + 32 * KByte + 256 * KByte		/* only for verification test */


/* ========================================================================
 * BT Share RAM
 * ======================================================================== */
#define BT_SHRAM_ORIGIN       0x28080000
#define BT_SHRAM_LENGTH       128 * KByte

/* ========================================================================
 * PSRAM
 * ======================================================================== */
#define PSRAM_ORIGIN          0x20000000
#define PSRAM_LENGTH          8 * MByte

#if defined(CONFIG_TRUSTZONE_EN) && (CONFIG_TRUSTZONE_EN==1)
/* move 128K to S */
#define NP_PSRAM_ORIGIN       0x20010020
#define NP_PSRAM_LENGTH       2 * MByte - 128 * KByte - 32
#else
#define NP_PSRAM_ORIGIN       0x20000020
#define NP_PSRAM_LENGTH       2 * MByte - 32
#endif

#define NP_PSRAM_HEAP_EXT_ORIGIN NP_PSRAM_ORIGIN + NP_PSRAM_LENGTH
#define NP_PSRAM_HEAP_EXT_LENGTH 0

#define VP_PSRAM_ORIGIN       0x20200020
#define VP_PSRAM_LENGTH       2 * MByte - 192 * KByte - 64

#define AP_PSRAM_RET_ORIGIN   0x20600000
#define AP_PSRAM_RET_LENGTH   1 * MByte
/* AP/VP shared FCS retained data is in AP_PSRAM_RET */
#define AP_PSRAM_RET_FCS_LENGTH   8 * KByte

#define VP_PSRAM_NN_ORIGIN    0x20700000
#define VP_PSRAM_NN_LENGTH    1 * MByte

/* ========================================================================
 * PSRAM (Secure)
 * ======================================================================== */
#define PSRAM_S_ORIGIN        0x30000000
#define PSRAM_S_LENGTH        8 * MByte

#if defined(CONFIG_TRUSTZONE_EN) && (CONFIG_TRUSTZONE_EN==1)
/* move 120K from S, 8K for NSC */
#define NP_PSRAM_NSC_ORIGIN     0x20000020
#define NP_PSRAM_NSC_LENGTH     8 * KByte - 32
#define NP_PSRAM_S_ORIGIN       0x20002020
#define NP_PSRAM_S_LENGTH       120 * KByte - 32
#endif

#define AP_PSRAM_BL1_ORIGIN   0x303CFFE0
#define AP_PSRAM_BL1_LENGTH   188 * KByte + 32

#define AP_PSRAM_SHARE_ORIGIN 0x303FF000
#define AP_PSRAM_SHARE_LENGTH 4 * KByte

/* ========================================================================
 * Video SRAM
 * ======================================================================== */
#define VSRAM_ORIGIN          0x6F000000
#define VSRAM_LENGTH          192 * KByte

#define VP_VSRAM_ORIGIN       0x6F000000
#define VP_VSRAM_LENGTH       192 * KByte

#define TS_VSRAM_ORIGIN       0x6F000000
#define TS_VSRAM_LENGTH       48 * KByte

#define COM_VSRAM_ORIGIN      0x6F00C000
#define COM_VSRAM_LENGTH      144 * KByte

#define SLC_VSRAM_ORIGIN      0x6F000000
#define SLC_VSRAM_LENGTH      192 * KByte

/* ========================================================================
 * DDR
 * ======================================================================== */
#if defined(CONFIG_DISABLE_DDR) && (CONFIG_DISABLE_DDR==1)
/* FAKE DDR */
#define DDR_ORIGIN            0x20400000
#define DDR_LENGTH            4 * MByte
#else
#define DDR_ORIGIN            0x80000000
#if defined(CONFIG_DDR_256M) && (CONFIG_DDR_256M==1)
#define DDR_LENGTH            256 * MByte
#else
#define DDR_LENGTH            128 * MByte
#endif
#endif

#if defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV==1)
#if defined(CONFIG_DISABLE_DDR) && (CONFIG_DISABLE_DDR==1)
#error "Should NOT disable DDR with 4K DEV select"
#endif
#define AP_DDR_RO_ORIGIN      0x80220000
#define AP_DDR_RO_LENGTH      3 * MByte - 128 * KByte

/* 10 MB moved from AP to VP for the 4K V1 + 2K V2 encoders. */
#define AP_DDR_ORIGIN         0x80500000
#define AP_DDR_LENGTH         7 * MByte - 256 * KByte

#define AP_DDR_STK_ORIGIN     0x80BC0000
#define AP_DDR_STK_LENGTH     256 * KByte

#define VP_DDR_ORIGIN         0x80C00000
#define VP_DDR_LENGTH         114 * MByte

#define NN_DDR_ORIGIN         0x87E00000
#define NN_DDR_LENGTH         2 * MByte

#elif defined(CONFIG_4K_ENA) && (CONFIG_4K_ENA==1)
/* TODO: implement when HW ready */
#else	// default or CONFIG_4K_OFF

#define AP_DDR_RO_ORIGIN      0x80220000
#define AP_DDR_RO_LENGTH      3 * MByte - 128 * KByte

#define AP_DDR_ORIGIN         0x80500000
#if defined(CONFIG_LINUX_FW_EN) && (CONFIG_LINUX_FW_EN==1)
#define AP_DDR_LENGTH         45 * MByte - 256 * KByte
#define AP_DDR_STK_ORIGIN     0x831C0000
#else
#define AP_DDR_LENGTH         41 * MByte - 256 * KByte
#define AP_DDR_STK_ORIGIN     0x82DC0000
#endif
#define AP_DDR_STK_LENGTH     256 * KByte

#if defined(CONFIG_DISABLE_DDR) && (CONFIG_DISABLE_DDR==1)
#define VP_DDR_ORIGIN         0x20400000
#define VP_DDR_LENGTH         4 * MByte
#else
#if defined(CONFIG_LINUX_FW_EN) && (CONFIG_LINUX_FW_EN==1)
#define VP_DDR_ORIGIN         0x83200000
#define VP_DDR_LENGTH         66 * MByte
#else
#define VP_DDR_ORIGIN         0x82E00000
#define VP_DDR_LENGTH         70 * MByte
#endif
#endif

#define NN_DDR_ORIGIN         0x87400000
#define NN_DDR_LENGTH         12 * MByte

#endif

#define AP_DDR_HEAP_EXT_ORIGIN  AP_DDR_ORIGIN + AP_DDR_LENGTH
#define AP_DDR_HEAP_EXT_LENGTH  0

#define VP_DDR_HEAP_EXT_ORIGIN VP_DDR_ORIGIN + VP_DDR_LENGTH
#define VP_DDR_HEAP_EXT_LENGTH 0

/* ========================================================================
 * DDR (Secure)
 * ======================================================================== */
#define DDR_S_ORIGIN            0xA0000000

#if defined(CONFIG_DDR_256M) && (CONFIG_DDR_256M==1)
#define DDR_S_LENGTH            256 * MByte
#else
#define DDR_S_LENGTH            128 * MByte
#endif

#if defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV==1)

#define AP_DDR_BL2_32_ORIGIN    0xA0000000
#define AP_DDR_BL2_32_LENGTH    1 * MByte

#define AP_DDR_OPTEE_ORIGIN     0xA0100000
#define AP_DDR_OPTEE_LENGTH     1 * MByte

#define AP_DDR_SHM_ORIGIN       0xA0200000
#define AP_DDR_SHM_LENGTH       4 * KByte

#define AP_DDR_OPTEE_SHM_ORIGIN 0xA0201000
#define AP_DDR_OPTEE_SHM_LENGTH 124 * KByte - 32

/* 4K DEV gives more DDR to VP. This FIP load window (not a flash partition)
 * ends at 0xA0BC0000, the secure alias of AP_DDR_STK_ORIGIN. Keeping the
 * default 30 MiB expression would overlap the AP stack and VP DDR.
 * BL2 and OP-TEE have separate regions above. The 32-byte image header and
 * aligned FIP payload must fit here; recheck their size when images grow.
 */
#define AP_DDR_FIP_ORIGIN       0xA021FFE0
#define AP_DDR_FIP_LENGTH       10 * MByte - 384 * KByte + 32

#elif defined(CONFIG_4K_ENA) && (CONFIG_4K_ENA==1)
/* TBD */
#else	// default or CONFIG_4K_OFF

#define AP_DDR_BL2_32_ORIGIN    0xA0000000
#define AP_DDR_BL2_32_LENGTH    1 * MByte

#define AP_DDR_OPTEE_ORIGIN     0xA0100000
#define AP_DDR_OPTEE_LENGTH     1 * MByte

#define AP_DDR_SHM_ORIGIN       0xA0200000
#define AP_DDR_SHM_LENGTH       4 * KByte

#define AP_DDR_OPTEE_SHM_ORIGIN 0xA0201000
#define AP_DDR_OPTEE_SHM_LENGTH 124 * KByte - 32

#define AP_DDR_FIP_ORIGIN       0xA021FFE0
#define AP_DDR_FIP_LENGTH       30 * MByte - 384 * KByte + 32

#endif
/* ========================================================================
 * XIP (Flash)
 * ======================================================================== */
#define XIP_ORIGIN            0x08000000
#define XIP_LENGTH            64 * MByte

/* ========================================================================
 * XIP Remap
 * ======================================================================== */
#define XIP_REMAP_ORIGIN      0x0C000000
#define XIP_REMAP_LENGTH      64 * MByte

/* ========================================================================
 * Trace Buffers
 * ======================================================================== */
#define BTRACE_ORIGIN         0xC8800000
#define BTRACE_LENGTH         0xC9000000 - 0xC8800000

#define CTRACE_ORIGIN         0xCA000000
#define CTRACE_LENGTH         0xCA7FFFFF - 0xCA000000

/* ========================================================================
 * Flash Configuration (Common)
 * ======================================================================== */
#define FLASH_START_ADDR      0x08000000
#define FLASH_LENGTH          4 * MByte

#endif /* AMEBA_LAYOUT_H */
