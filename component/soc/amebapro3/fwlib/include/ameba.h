/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _HAL_AMEBA_H_
#define _HAL_AMEBA_H_

#include "basic_types.h"
#include "section_config.h"
#include "rom/memproc.h"
#include "rom/strproc.h"
#include "rom/rand.h"
#include "rom/diag.h"
#include "hal_platform.h"
#include "ameba_vector.h"
#include "ameba_boot_mailbox.h"
#include "ameba_loguart.h"
#include "ameba_soc_ps.h"
#include "rom/ameba_shell.h"
#include "rom/ameba_monitor.h"
#include "ameba_syscfg.h"
#include "ameba_pmc.h"
#include "ameba_pll.h"
#include "ameba_clk.h"
#include "ameba_bor.h"
#include "ameba_ldo.h"
#include "ameba_swr.h"
#include "ameba_boot.h"
#include "ameba_rsip.h"
#include "ameba_spic.h"
#include "ameba_spinand.h"
#include "ameba_backup_reg.h"
#include "ameba_pinmap.h"
#include "ameba_ipc.h"
#include "ameba_ipccfg.h"
#include "ameba_sema_rom.h"
#include "sysreg.h"
#include "ameba_pinmux.h"
#include "ameba_otpc.h"
#include "ameba_rsip.h"
#include "ameba_arch.h"
#include "ameba_chipinfo.h"
#include "ameba_dram_die.h"
#include "ameba_ddrphy.h"
#include "ameba_ddrc.h"
#include "ameba_ddr.h"
#include "ameba_pll_tracking.h"
#include "ameba_sd.h"
#include "ameba_aontimer.h"
#include "ameba_aonpwm.h"
#include "ameba_aonwdg.h"
#include "ameba_wakepin.h"
#include "ameba_gdma.h"
#include "ameba_crypto.h"
#include "ameba_key_management_aes.h"
#include "ameba_key_management_hmac.h"
#include "ameba_aes.h"
#include "ameba_sha.h"
#include "ameba_pke.h"
#include "ameba_usb.h"
#include "ameba_platform.h"
#include "sysreg_core_s.h"
#include "sysreg_aon.h"


/* ===========================  Configuration of the ARM ARMV8MML Processor and Core Peripherals  ============================ */
#if defined (CONFIG_ARM_CORE_CM4)
#define __VTOR_PRESENT                 1        /*!< Set to 1 if CPU supports Vector Table Offset Register                     */
#define __MPU_PRESENT                  1         /**< Defines if an MPU is present or not */
#define __NVIC_PRIO_BITS               3         /**< Number of priority bits implemented in the NVIC */
#define __Vendor_SysTickConfig         1         /**< Vendor specific implementation of SysTickConfig is defined *///see vPortSetupTimerInterrupt
#define __SAUREGION_PRESENT            1        /*!< SAU present or not                                                        */
#define DCACHE_4WAY

#define __FPU_PRESENT             1       /*!< FPU present                                   */
#define __DSP_PRESENT             1

#define __PMU_PRESENT             1
#define __PMU_NUM_EVENTCNT        4       /*!< __PMU_NUM_EVENTCNT range is [2, 31]           */

#include <arm_cmse.h>   /* Use CMSE intrinsics */
#include "cmsis/core_armv81mml.h"
#include "cmsis/core_cache.h"
#include "cmsis/mpu_config.h"
#include "ameba_cache.h"
#include "ameba_debugtimer.h"

#elif defined (CONFIG_ARM_CORE_CM0)

#define __ARMV8MBL_REV                 0x0000U  /*!< ARMV8MBL Core Revision                                                    */
#define __NVIC_PRIO_BITS               2        /*!< Number of Bits used for Priority Levels                                   */
#define __Vendor_SysTickConfig         0        /*!< Set to 1 if different SysTick Config is used                              */
#define __VTOR_PRESENT                 1        /*!< Set to 1 if CPU supports Vector Table Offset Register                     */
#define __SAU_REGION_PRESENT           0        /*!< SAU present or not                                                        */
#define DCACHE_4WAY

#define __MPU_PRESENT                  1         /**< Defines if an MPU is present or not */
#include "cmsis/core_armv8mbl.h"
#include "cmsis/core_cache.h"
#include "cmsis/mpu_config.h"
#include "ameba_cache.h"

#elif defined (CONFIG_ARM_CORE_CA32)

#include <string.h>
#include <stdlib.h>

#define __FPU_PRESENT			1
#define __GIC_PRESENT			1
#define __TIM_PRESENT			1
#define __L2C_PRESENT			0
#define __CORTEX_A				7

#define GIC_DISTRIBUTOR_BASE	(0x60401000uL)                        /*!< (GIC DIST  ) Base Address */
#define GIC_INTERFACE_BASE		(0x60402000uL)                        /*!< (GIC CPU IF) Base Address */

/* workaround: basic_types.h has ENABLE define, but core_ca.h has ENABLE field name in structure */
#undef ENABLE

#include "cmsis_ca/core_ca.h"
#include "cmsis_ca/cmsis_cp15.h"
#include "ameba_cache.h"
#include "ameba_debugtimer.h"

#define ENABLE 1

#endif

#include "ameba_trustzone.h"
#include "ameba_gdma.h"
#include "ameba_tg_type.h"
#include "ameba_tm_type.h"
#include "ameba_timer.h"
#include "ameba_pwm_comm.h"
#include "ameba_pwm.h"
#include "ameba_gpio.h"
#include "ameba_spi.h"
#include "ameba_uart.h"
#include "ameba_i2c.h"
#include "ameba_adc.h"
#include "ameba_wdg.h"
#include "ameba_rtc.h"
#include "ameba_delay.h"
#include "ameba_audio.h"
#include "ameba_sport.h"
#include "ameba_trng.h"
#include "ameba_reset.h"
#include "ameba_psram.h"
#include "ameba_psphy.h"
#include "ameba_pmctimer.h"
#include "ameba_eth.h"
#include "ameba_eth_phy.h"
#include "ameba_eth_phy_rtl8201f.h"
#include "ameba_comp.h"

#include "ameba_usrcfg.h"
#include "ameba_rcc.h"
#include "rom/log.h"
#include "rom/sscanf_minimal.h"
#ifndef CONFIG_BUILD_ROM
#include "ameba_ipc_api.h"
#endif

#define FLASH_HS_BOOT_ADDR				(SPI_FLASH_BASE + 0x4000)
#define FLASH_RESERVED_DATA_BASE		0x8000   // reserve 8K for Image1
#define FLASH_SYSTEM_DATA_ADDR			0x13000  // reserve 8K+4K for Image1 + Reserved data
#define FLASH_OTA1_CODE_ADDR			0x6020
#define FLASH_SECTOR_SIZE				0x1000
//BT calibration Data
#define FLASH_BT_PARA_ADDR			0x5FF0

/* Spic_Mode */
#define ReadQuadIOMode			0
#define ReadQuadOMode			1
#define ReadDualIOMode			2
#define ReadDualOMode 			3
#define ReadOneMode				4

/* APP image pattern, can't be modified */
#define APP_IMAGE_PATTERN_1 	0x35393138U
#define APP_IMAGE_PATTERN_2 	0x31313738U

#define IMAGE_HEADER_LEN		0x20
typedef struct {
	u32 signature[2];
	u32 image_size;
	u32 image_addr;

	/* 0x01010101:NP Loader Flag, others are invalid */
	u32 boot_index;
	u32 reserved[3];
} IMAGE_HEADER;

/* cert and manifest common header typedef */
typedef struct {
	u8 Pattern[8];
	u8 Rsvd1[8];
	u8 Ver;
	u8 ImgID;
	u8 AuthAlg;
	u8 HashAlg;
	u16 MajorKeyVer;
	u16 MinorKeyVer;
} AuthHeader_TypeDef;


typedef enum  _HAL_Status {
	HAL_OK            = 0x00,
	HAL_BUSY          = 0x01,
	HAL_TIMEOUT       = 0x02,
	HAL_ERR_PARA      = 0x03,     // error with invaild parameters
	HAL_ERR_MEM       = 0x04,     // error with memory allocation failed
	HAL_ERR_HW        = 0x05,     // error with hardware error

	HAL_ERR_UNKNOWN   = 0xee      // unknown error

} HAL_Status;


#define USE_FULL_ASSERT

#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function which reports
  *         the name of the source file and the source line number of the call
  *         that failed. If expr is true, it returns no value.
  * @retval None
  */
#define assert_param(expr) ((expr) ? (void)0 : io_assert_failed((char *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
/* to tell the compiler that the function won't ever return */
/* used to suppress errors about code paths not being reached */
__NO_RETURN void io_assert_failed(char *function, uint32_t line);
#else
#define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#endif //_HAL_AMEBA_H_
