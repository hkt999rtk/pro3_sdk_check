/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_PLATFORM_ROM_H_
#define _AMEBA_PLATFORM_ROM_H_


_LONG_CALL_ void PLATFORM_Init(void);
_LONG_CALL_ void PLATFORM_PERIPLL_Init(void);
_LONG_CALL_ void PLATFORM_SYS2PLL_Init(void);
_LONG_CALL_ void PLATFORM_SYS1PLL_Init(void);
_LONG_CALL_ void PLATFORM_SYS1PLL_SetFreq(u32 DivN_SDM, u32 F0N_SDM, u32 F0F_SDM);
_LONG_CALL_ void PLATFORM_SYS2PLL_SetFreq(u32 DivN_SDM, u32 F0N_SDM, u32 F0F_SDM);
_LONG_CALL_ void PLATFORM_PERIPLL_SetFreq(u32 DivN_SDM, u32 F0N_SDM, u32 F0F_SDM);
_LONG_CALL_ void PLATFORM_CPUPLL_Init(void);
_LONG_CALL_ void PLATFORM_BANDGAP_Init(void);
_LONG_CALL_ void PLATFORM_PADPOWER_Init(void);
_LONG_CALL_ void PLATFORM_COREPOWER_Init(void);
_LONG_CALL_ void PLATFORM_MCUOFF_Init(void);
_LONG_CALL_ void PLATFORM_DEBUGTIMER_Init(void);
_LONG_CALL_ void PLATFORM_Clock_Init(void);
_LONG_CALL_ void PLATFORM_SPIC_Clock_Init(void);
_LONG_CALL_ void PLATFORM_WDG_Init(void);
_LONG_CALL_ void PLATFORM_WDG_Refresh(void);
_LONG_CALL_ void PLATFORM_WDG_Timeout(u32 timeout_ms);

_LONG_CALL_ void PLATFORM_TIM_Init(IRQ_FUN fun, u32 fun_arg);
_LONG_CALL_ void PLATFORM_TIM_DeInit(void);
_LONG_CALL_ void PLATFORM_TIM_INTConfig(int enable);
_LONG_CALL_ void PLATFORM_TIM_Cmd(int cmd);
_LONG_CALL_ void PLATFORM_TIM_ClearINT(void);

_LONG_CALL_ void PLATFORM_CRYPTO_Init(void);
_LONG_CALL_ void PLATFORM_PKE_Init(void);
_LONG_CALL_ void PLATFORM_TRNG_Init(void);
_LONG_CALL_ void PLATFORM_RSIP_Init(void);

_LONG_CALL_ void PLATFORM_MPU_Init(void);
_LONG_CALL_ void PLATFORM_TRAP_Init(void);
_LONG_CALL_ void PLATFORM_TRAP_Dump(void);

_LONG_CALL_ u32 PLATFORM_BOOTLMT_Read(void);
_LONG_CALL_ void PLATFORM_BOOTLMT_Write(uint32_t value);

/*BOOT_OTA_INFO for roll-back*/
/* User should set BOOT_OTA_INFO to zero when bootloader img can work normally */
#define BOOTLMT_BIT_BOOT_CNT_ERR		BIT(4)                   /*!<R/W 0  Set when bootcnt > BOOT_CNT_TRY_LIMIT, means newer bootloader cannot work */
#define BOOTLMT_BIT_BOOT_VER_NUM		BIT(3)                   /*!<R/W 0  boot version, 0 for OTA1, 1 for OTA2 */
#define BOOTLMT_MASK_BOOT_CNT			((u32)0x00000007)        /*!<R/W 0  [2:0] bootcnt value */
#define BOOTLMT_BOOT_CNT(x)				((u32)(((x) & 0x00000007)))
#define BOOTLMT_GET_BOOT_CNT(x)			((u32)(((x) & 0x00000007)))

#define BOOT_CNT_TOTAL_LIMIT			0x06
#define BOOT_CNT_TRY_LIMIT				0x3

#endif
