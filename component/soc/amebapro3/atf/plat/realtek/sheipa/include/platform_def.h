/*
 * Copyright (c) 2015-2019, Realtek Semiconductor Corp. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLATFORM_DEF_H
#define PLATFORM_DEF_H

#include <arch.h>
#include <common/tbbr/tbbr_img_def.h>
#include <lib/utils_def.h>
#include <plat/common/common_def.h>
#include "hal_platform.h"
#include "sysreg_rcc.h"
#include "platform_autoconf.h"
#include "ameba_layout.h"

/* Special value used to verify platform parameters from BL2 to BL3-1 */
#define SHEIPA_BL31_PLAT_PARAM_VAL	0x0f1e2d3c4b5a6978ULL

#define PLATFORM_STACK_SIZE 0x1000

#define PLATFORM_MAX_CPUS_PER_CLUSTER	1
#define PLATFORM_CLUSTER_COUNT			1
#define PLATFORM_CLUSTER0_CORE_COUNT	PLATFORM_MAX_CPUS_PER_CLUSTER
#define PLATFORM_CLUSTER1_CORE_COUNT	PLATFORM_MAX_CPUS_PER_CLUSTER
#define PLATFORM_CORE_COUNT		(PLATFORM_CLUSTER_COUNT * PLATFORM_MAX_CPUS_PER_CLUSTER)

#define SHEIPA_PRIMARY_CPU		0
#define PLAT_NUM_PWR_DOMAINS		(1 + PLATFORM_CLUSTER_COUNT + \
									PLATFORM_CORE_COUNT)
#define PLAT_MAX_PWR_LVL		MPIDR_AFFLVL2

#define PLAT_MAX_RET_STATE		1
#define PLAT_MAX_OFF_STATE		2

/* Local power state for power domains in Run state. */
#define PLAT_LOCAL_STATE_RUN		0
/* Local power state for retention. Valid only for CPU power domains */
#define PLAT_LOCAL_STATE_RET		1
/*
 * Local power state for OFF/power-down. Valid for CPU and cluster power
 * domains.
 */
#define PLAT_LOCAL_STATE_OFF		2

/*
 * Macros used to parse state information from State-ID if it is using the
 * recommended encoding for State-ID.
 */
#define PLAT_LOCAL_PSTATE_WIDTH		4
#define PLAT_LOCAL_PSTATE_MASK		((1 << PLAT_LOCAL_PSTATE_WIDTH) - 1)

/*
 * Some data must be aligned on the biggest cache line size in the platform.
 * This is known only to the platform as it might have a combination of
 * integrated and external caches.
 */
#define CACHE_WRITEBACK_SHIFT		6
#define CACHE_WRITEBACK_GRANULE		(1 << CACHE_WRITEBACK_SHIFT)

/*
 * Partition memory into secure ROM, non-secure DRAM, secure "SRAM",
 * and secure DRAM. from ameba_layout.h
 */

#define KM0_IPC_RAM				IPC_SSRAM_ORIGIN 		//0x2807E000		// currently not use in ATF, need by sheipa_ipc.c

#define DRAM_START_ADDR			DDR_ORIGIN 				//0x80000000		// TODO

#define SEC_ROM_BASE			0x0						//remap from PSRAM
#define SEC_ROM_SIZE			AP_PSRAM_BL1_LENGTH-32 	//0x0002F000		// same as BL1 size

#define SEC_PSRAM_BASE 			AP_PSRAM_BL1_ORIGIN+32 	//0x303D0000		// extra 32 byte header should reserved
#define SEC_PSRAM_SIZE 			AP_PSRAM_BL1_LENGTH-32 	//0x0002F000

#define SEC_SHARED_RAM_BASE		AP_DDR_SHM_ORIGIN		//0xA0200000
#define SEC_SHARED_RAM_SIZE		(AP_DDR_SHM_LENGTH + AP_DDR_OPTEE_SHM_LENGTH)		// 128K - 32

#define SEC_DRAM_BASE 			AP_DDR_BL2_32_ORIGIN	//0xA0000000
#define SEC_DRAM_SIZE 			AP_DDR_BL2_32_LENGTH	//0x00100000		// 1M then OPTEE 1M

#define SEC_SRAM_BASE			NP_SSRAM_ROMBSS_S_ORIGIN	//0x38000000
#define SEC_SRAM_SIZE			(NP_SSRAM_ROMBSS_S_LENGTH + NP_SSRAM_MSP_S_LENGTH + NP_SSRAM_LOADER_S_LENGTH + 32) //0x10000

#define NS_SRAM_BASE			NP_SSRAM_MSP_ORIGIN			//0x28000000
#define NS_SRAM_SIZE			(NP_SSRAM_MSP_LENGTH + NP_SSRAM_ROMBSS_LENGTH + NP_SSRAM_MAILBX_LENGTH)	//0x10000

#define NS_DRAM0_BASE			AP_DDR_RO_ORIGIN			//0x80220000
#define NS_DRAM0_SIZE			(AP_DDR_LENGTH + AP_DDR_STK_LENGTH) //(0x82000000 - NS_DRAM0_BASE)

/*
 * ARM-TF lives in SRAM, partition it here
 */

#define SHARED_RAM_BASE			SEC_SHARED_RAM_BASE
#define SHARED_RAM_SIZE			AP_DDR_SHM_LENGTH

#define PLAT_SHEIPA_TRUSTED_MAILBOX_BASE	SHARED_RAM_BASE
#define PLAT_SHEIPA_TRUSTED_MAILBOX_SIZE	(8 + PLAT_SHEIPA_HOLD_SIZE)
#define PLAT_SHEIPA_HOLD_BASE		(PLAT_SHEIPA_TRUSTED_MAILBOX_BASE + 8)
#define PLAT_SHEIPA_HOLD_SIZE		(PLATFORM_CORE_COUNT * PLAT_SHEIPA_HOLD_ENTRY_SIZE)
#define PLAT_SHEIPA_HOLD_ENTRY_SIZE	8
#define PLAT_SHEIPA_HOLD_STATE_WAIT	0
#define PLAT_SHEIPA_HOLD_STATE_GO		1

#define PLAT_SHEIPA_WARM_BOOT_BASE	(PLAT_SHEIPA_TRUSTED_MAILBOX_BASE + 0x100)

#define BL_RAM_BASE			(SHARED_RAM_BASE + SHARED_RAM_SIZE)
#define BL_RAM_SIZE			(SEC_SHARED_RAM_SIZE - SHARED_RAM_SIZE)

/*
 * BL1 specific defines.
 *
 * BL1 RW data is relocated from ROM to RAM at runtime so we need 2 sets of
 * addresses.
 * Put BL1 RW at the top of the Secure SRAM. BL1_RW_BASE is calculated using
 * the current BL1 RW debug size plus a little space for growth.
 */
#define BL1_RO_SIZE			(0x20000)
#define BL1_RW_SIZE			(0x10000)

#define BL1_RO_BASE			SEC_PSRAM_BASE		// link to ROM. load to PSRAM and remap to 0x0
#define BL1_RO_LIMIT		(BL1_RO_BASE + BL1_RO_SIZE)
#define BL1_RW_BASE			(BL1_RO_LIMIT)
#define BL1_RW_LIMIT		(BL1_RW_BASE + BL1_RW_SIZE)

/*
 * BL2 specific defines.
 *
 * Put BL2 just below BL3-1. BL2_BASE is calculated using the current BL2 debug
 * size plus a little space for growth.
 */
#define BL2_BASE 			SEC_DRAM_BASE // TODO, check what is "skip SHARED_RAM"?
#define BL2_LIMIT			(BL2_BASE + 0x40000)


#define BL2_RW_BASE			(BL2_BASE)
#define BL2_RW_LIMIT		(BL2_LIMIT)
#define BL2_RW_SIZE			(BL2_RW_LIMIT - BL2_RW_BASE)

/*
 * BL3-1 specific defines.
 *
 * Put BL3-1 at the top of the Trusted SRAM. BL31_BASE is calculated using the
 * current BL3-1 debug size plus a little space for growth.
 */
#define BL31_BASE
#define BL31_LIMIT

/*
 * BL3-2 specific defines.
 *
 * BL3-2 can execute from Secure SRAM, or Secure DRAM.
 */
#define BL32_DRAM_BASE			BL2_LIMIT
#define BL32_DRAM_LIMIT			(SEC_DRAM_BASE + SEC_DRAM_SIZE)

#define SEC_DRAM_ID				1

#define BL32_BASE				BL32_DRAM_BASE
#define BL32_LIMIT				BL32_DRAM_LIMIT
#define BL32_SIZE				(BL32_LIMIT - BL32_BASE)

#define BL32_MEM_BASE			BL32_DRAM_BASE
#define BL32_MEM_SIZE			(BL32_LIMIT - BL32_BASE)

#define NS_IMAGE_OFFSET			NS_DRAM0_BASE
#define NS_IMAGE_MAX_SIZE		NS_DRAM0_SIZE

#define PLAT_PHY_ADDR_SPACE_SIZE	(1ull << 32)
#define PLAT_VIRT_ADDR_SPACE_SIZE	(1ull << 32)
#define MAX_MMAP_REGIONS		16
#define MAX_XLAT_TABLES			10
#define MAX_IO_DEVICES			3
#define MAX_IO_HANDLES			4

#define CA32_FIP_MAX_SIZE				(0x00200000)  /* 2MB */

/* Load pageable part of OP-TEE 2MB above BL32 base, from ameba_layout.h */
#define SHEIPA_OPTEE_PAGEABLE_LOAD_BASE AP_DDR_OPTEE_ORIGIN		//0xA0100000
#define SHEIPA_OPTEE_PAGEABLE_LOAD_SIZE	AP_DDR_OPTEE_LENGTH		//0x00100000

#define NS_DDR_ADDR(x)			((x)&~(BIT(29)))				//bit29 is secure bit, 1 for secure, 0 for non-secure

#define SHEIPA_OPTEE_SHMEM_START		NS_DDR_ADDR(AP_DDR_SHM_ORIGIN) //0x80200000
#define SHEIPA_OPTEE_SHMEM_SIZE			0x00020000

/*******************************************************************************
 * TSP  specific defines.
 ******************************************************************************/
#define TSP_SEC_MEM_BASE					BL32_BASE
#define TSP_SEC_MEM_SIZE					(BL32_LIMIT - BL32_BASE + 1)

/* ID of the secure physical generic timer interrupt used by the TSP */
#define ARM_IRQ_SEC_PHY_TIMER				29
#define TSP_IRQ_SEC_PHY_TIMER				ARM_IRQ_SEC_PHY_TIMER
/*
 * UART related constants
 */
#define UART0_BASE							UARTLOG_REG_BASE
#define UART1_BASE							UARTLOG_REG_BASE
#define UART0_CLK_IN_HZ						40000000
#define UART1_CLK_IN_HZ						40000000

#define PLAT_SHEIPA_BOOT_UART_BASE			UART0_BASE
#define PLAT_SHEIPA_BOOT_UART_CLK_IN_HZ		UART0_CLK_IN_HZ   	// not used

#define PLAT_SHEIPA_CRASH_UART_BASE			UART1_BASE
#define PLAT_SHEIPA_CRASH_UART_CLK_IN_HZ	UART1_CLK_IN_HZ		// ?

#define PLAT_SHEIPA_CONSOLE_BAUDRATE		1500000

#define SHEIPA_FLASH_BASE					SPI_FLASH_BASE
#define SHEIPA_FLASH_SIZE					0x08000000

/* Use for Sync KM4 flash_para, from ameba_layout.h */
#define KM4_ROMBSS_RAM_COM_BASE				NP_SSRAM_ROMBSS_ORIGIN							//0x28001000
#define KM4_ROMBSS_RAM_COM_SIZE				(NP_SSRAM_ROMBSS_LENGTH+NP_SSRAM_MAILBX_LENGTH) //0x1000

#define TZ_IDAU_SEC_DRAM_OFFSET				0x20000000

#define SHEIPA_FIP_BASE						(NS_DRAM0_BASE + TZ_IDAU_SEC_DRAM_OFFSET)
#define SHEIPA_FIP_SIZE						NS_DRAM0_SIZE

#define PLAT_SHEIPA_FIP_BASE				SHEIPA_FIP_BASE
#define PLAT_SHEIPA_FIP_MAX_SIZE			SHEIPA_FIP_SIZE

#define DEVICE0_NS_BASE						0x40000000
#define DEVICE0_NS_SIZE						0x10000000

#define DEVICE0_S_BASE						0x50000000
#define DEVICE0_S_SIZE						0x10000000

#define DEVICE0_BASE						(DEVICE0_NS_BASE)
#define DEVICE0_SIZE						(DEVICE0_NS_SIZE+DEVICE0_S_SIZE)

#define SPIC_BASE							0x29000000
#define SPIC_SIZE							0x00100000

#define FLASH_BASE							0x08000000
#define FLASH_SIZE							0x04000000

/* from ameba_layout.h */
#define KM0_RAM								(FP_SSRAM_ORIGIN - 32)							//0x28040000
#define KM0_RAM_SIZE						(FP_SSRAM_LENGTH + FP_SSRAM_MSP_LENGTH + 32) 	//0x40000

/* TODO: Non-volatile counters for Trusted board boot,
 * Because fast model did't have hw non-volatile counters, so
 * we did't implement this feature.
 */

/*
 * GIC related constants
 */
#if ARM_GIC_ARCH == 3
#define GICD_BASE							0x02100000
#define GICR_BASE							0x02140000
#else
#define GICD_BASE							0x60401000		// PRO3
#define GICC_BASE							0x60402000		// PRO3
#endif

#define DEVICE1_NS_BASE						0x60000000	// PRO3 CORE
#define DEVICE1_NS_SIZE						0x10000000	// PRO3 CORE

#define DEVICE1_S_BASE						0x70000000	// PRO3 CORE
#define DEVICE1_S_SIZE						0x10000000	// PRO3 CORE

#define DEVICE1_BASE						(DEVICE1_NS_BASE)	// PRO3 CORE
#define DEVICE1_SIZE						(DEVICE1_NS_SIZE+DEVICE1_S_SIZE)	// PRO3 CORE

/*
 * DT related constants
 */
#define PLAT_SHEIPA_DT_BASE					0x81f00000
#define PLAT_SHEIPA_DT_MAX_SIZE				0x000c0000

/*
 * System counter
 */
#define SYS_COUNTER_FREQ_IN_TICKS			10*1000*1000 // 500*1000*1000	//SHOULD BE 500Mhz/div and div < 64

#define MPC1_BASE_S          				CORE_MPC							// PRO3-VIDEO RAM

#ifndef BIT
#define BIT(x) (1U << (x))
#endif

#define CA32_CTRL_BASE						0x60490000
#define REG_CA32_CLK_CTRL					0x4
#define BIT_TSCLK_DIVTRG_EN					BIT(0)
#define TSCLK_DIV(x)						(((x) & 0x3F) << 1)
#define BIT_TRCCLK_DIVTRG_EN				BIT(7)
#define TRCCLK_DIV(x)						(((x) & 0x3F) << 8)
/*
 * Boot flag related constants
 */
// PRO3 AP not support PG mode
#define BOOT_REASON_SW_REG					(AON_REG_BASE + 0x0088) 			// PRO3 aka (u32)AON_BASE->AON_CA32_BOOT_REASON
#define BIT_AP_WAKE_FROM_PG_FLAG			0x8									// PRO3

#endif /* PLATFORM_DEF_H */
