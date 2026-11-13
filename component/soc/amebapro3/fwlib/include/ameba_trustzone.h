/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_TRUSTZONE_H_
#define _AMEBA_TRUSTZONE_H_

#include "ameba_rxi300.h"
#include "rom/fault_injection_hardening.h"

/** @brief RXI300 port
*/
typedef struct {
	__IO uint32_t IDAU_BAx;  /*!< BASE ADDRESS REGISTER x IN IDAU Register */
	__IO uint32_t IDAU_LAx;  /*!< TOP ADDRESS REGISTER x IN IDAU Register */
} MPC_EntryTypeDef;

/** @brief RXI300 port
*/
typedef struct {
	__IO uint32_t PPC1_REG;  /*!< PPC1 CONFIGURE REGISTER0 Register */
	__IO uint32_t PPC2_REG;  /*!< PPC2 CONFIGURE REGISTER */
} PPC_TypeDef;

typedef struct {
	MPC_EntryTypeDef MPC_Entry1[8]; /*!< RXI300 IP have 8 ports for ram type1 */
	__IO uint32_t IDAU_CTRL;       /*!< IDAU ENABLE CONTROL REGISTER */
	__IO uint32_t IDAU_LOCK;       /*!< IDAU LOCK CONTROL REGISTER */
	MPC_EntryTypeDef MPC_Entry2[8]; /*!< RXI300 IP have 8 ports for ram type2 */
} MPC_TypeDef;

typedef struct {
	u32 Start;
	u32 End;
	u32 CTRL;
} TZ_CFG_TypeDef;

typedef struct {
	u32 Start;
	u32 End;
	u32 NSC;
} SAU_CFG_TypeDef;

/** @brief Register of a PPC config entry, see PPC_CFG_TypeDef
*/
typedef enum {
	PPC_REG1 = 0,		/*!< PPC1_REG: CORE GPR_0, WIFI CTR_AND_SECURE_0 */
	PPC_REG2 = 1,		/*!< PPC2_REG: CORE GPR_1, WIFI SECURE_1 */
	PPC_REG_IDAU = 2,	/*!< CORE IDAU_CTRL: slave without address range is also configured here, bit value 1: non-secure
	                         (opposite of the slave bits in PPC1_REG / PPC2_REG) */
} PPC_RegId;

/** @brief PPC config entry, a config table has one entry per register and ends with Mask == 0.
  * Reg   : register to configure, one of PPC_RegId
  * Mask  : bits to configure, bits outside Mask are never touched (not every bit of these registers is a PPC bit)
  * Secure: bits within Mask to configure as secure, the others within Mask are configured as non-secure
  */
typedef struct {
	u8 Reg;
	u32 Mask;
	u32 Secure;
} PPC_CFG_TypeDef;

/** @brief PPC slave id, one id per slave port that can be configured, grouped by the register holding its bit.
  * id = register group | bit position in that register. The comment of each id is: RXI-300 interface ID, port name.
  */
#define PPC_CORE_REG2	(0 << 8)	/*!< CORE PPC2_REG, RXI_CORE GPR_1 */
#define PPC_CORE_IDAU	(1 << 8)	/*!< CORE IDAU_CTRL */
#define PPC_WIFI_REG1	(2 << 8)	/*!< WIFI PPC1_REG, RXI_WIFI CTR_AND_SECURE_0 */
#define PPC_WIFI_REG2	(3 << 8)	/*!< WIFI PPC2_REG, RXI_WIFI SECURE_1 */

typedef enum {
	/* CORE PPC2_REG (RXI_CORE GPR_1), bit value 1: only secure can reach the slave */
	PPC_CORE_CA32_AUX             = PPC_CORE_REG2 | 2,	/* S3-1   ca32_aux */
	PPC_CORE_ENCODER              = PPC_CORE_REG2 | 4,	/* S4-1   encoder_slv */
	PPC_CORE_NN                   = PPC_CORE_REG2 | 5,	/* S4-2   nn_slv */
	PPC_CORE_ISP                  = PPC_CORE_REG2 | 6,	/* S4-3   isp_slv */
	PPC_CORE_REMAP_ENC            = PPC_CORE_REG2 | 7,	/* S4-4   remap_enc */
	PPC_CORE_REMAP_ISP            = PPC_CORE_REG2 | 8,	/* S4-5   remap_isp */
	PPC_CORE_GE                   = PPC_CORE_REG2 | 9,	/* S4-6   ge_slv */
	PPC_CORE_TILE_SCALER          = PPC_CORE_REG2 | 10,	/* S4-7   tile_scaler_slv */
	PPC_CORE_RXI300_CORE          = PPC_CORE_REG2 | 11,	/* S4-9   rxi300_core */
	PPC_CORE_TRAFFIC_GEN          = PPC_CORE_REG2 | 14,	/* S8-16  traffic_gen */
	PPC_CORE_PHY_CFG              = PPC_CORE_REG2 | 15,	/* S8-15  phy_cfg */
	PPC_CORE_PHY_CAL              = PPC_CORE_REG2 | 16,	/* S8-14  phy_cal */
	PPC_CORE_DRAM_REG             = PPC_CORE_REG2 | 17,	/* S8-13  dram_reg */
	PPC_CORE_DDRC_CFG             = PPC_CORE_REG2 | 18,	/* S8-12  ddrc_cfg */
	PPC_CORE_SYS_CTRL             = PPC_CORE_REG2 | 19,	/* S8-11  sys_ctrl */
	PPC_CORE_GPIO                 = PPC_CORE_REG2 | 20,	/* S8-10  core_gpio_slv */
	PPC_CORE_USB_OTG              = PPC_CORE_REG2 | 21,	/* S8-9   usb_otg_slv */
	PPC_CORE_WDT_MEDIA            = PPC_CORE_REG2 | 22,	/* S8-8   wdt_media */
	PPC_CORE_WDT_CA32             = PPC_CORE_REG2 | 23,	/* S8-7   wdt_ca32 */
	PPC_CORE_NON_SEC_CORE         = PPC_CORE_REG2 | 24,	/* S8-6   non_sec_core_ */
	PPC_CORE_LX                   = PPC_CORE_REG2 | 25,	/* S8-5   lx_slv */
	PPC_CORE_AUDIO_ANA            = PPC_CORE_REG2 | 26,	/* S8-4   audio_ana */
	PPC_CORE_AUDIO_DIG            = PPC_CORE_REG2 | 27,	/* S8-3   audio_dig */
	PPC_CORE_SPORT1               = PPC_CORE_REG2 | 28,	/* S8-2   sport1 */
	PPC_CORE_SPORT0               = PPC_CORE_REG2 | 29,	/* S8-1   sport0 */

	/* CORE IDAU_CTRL, bit value 1: non-secure (opposite of PPC2_REG) */
	PPC_CORE_CA32_ACP             = PPC_CORE_IDAU | 8,	/* S0     ca32_acp */
	PPC_CORE_MEDIA_KM4            = PPC_CORE_IDAU | 9,	/* S1-1   media_km4_slv */
	PPC_CORE_GIC                  = PPC_CORE_IDAU | 10,	/* S3-2   gic */
	PPC_CORE_AI_DNS               = PPC_CORE_IDAU | 11,	/* S1-2   ai_dns */

	/* WIFI PPC1_REG (RXI_WIFI CTR_AND_SECURE_0), bit value 1: only secure can reach the slave */
	PPC_WIFI_SPIC_USER            = PPC_WIFI_REG1 | 8,	/* S13-2  spic_user */
	PPC_WIFI_FLASH_SEC            = PPC_WIFI_REG1 | 9,	/* S16-2  flash_sec */
	PPC_WIFI_PSRAMC_USER          = PPC_WIFI_REG1 | 10,	/* S16-3  psramc_user */
	PPC_WIFI_PSRAM_PHY            = PPC_WIFI_REG1 | 11,	/* S16-4  psram_phy */
	PPC_WIFI_MST_SPI1             = PPC_WIFI_REG1 | 12,	/* S16-5  mst_spi1 */
	PPC_WIFI_SLV_SPI1             = PPC_WIFI_REG1 | 13,	/* S16-6  slv_spi1 */
	PPC_WIFI_I2C2                 = PPC_WIFI_REG1 | 14,	/* S16-8  i2c2 */
	PPC_WIFI_I2C3                 = PPC_WIFI_REG1 | 15,	/* S16-9  i2c3 */
	PPC_WIFI_UART3                = PPC_WIFI_REG1 | 16,	/* S16-10 uart3 */
	PPC_WIFI_UART4                = PPC_WIFI_REG1 | 17,	/* S16-11 uart4 */
	PPC_WIFI_WDT_NETWORK          = PPC_WIFI_REG1 | 18,	/* S16-12 wdt_network */
	PPC_WIFI_NAND_ECC             = PPC_WIFI_REG1 | 19,	/* S16-13 nand_ecc */
	PPC_WIFI_I2C0                 = PPC_WIFI_REG1 | 20,	/* S16-14 i2c0 */
	PPC_WIFI_I2C1                 = PPC_WIFI_REG1 | 21,	/* S16-15 i2c1 */
	PPC_WIFI_UART1                = PPC_WIFI_REG1 | 22,	/* S16-16 uart1 */
	PPC_WIFI_UART2                = PPC_WIFI_REG1 | 23,	/* S16-17 uart2 */
	PPC_WIFI_TIMER1               = PPC_WIFI_REG1 | 24,	/* S16-18 timer1 */
	PPC_WIFI_TIMER2               = PPC_WIFI_REG1 | 25,	/* S16-19 timer2 */
	PPC_WIFI_TIMER3               = PPC_WIFI_REG1 | 26,	/* S16-20 timer3 */
	PPC_WIFI_SGPIO                = PPC_WIFI_REG1 | 27,	/* S16-21 sgpio */
	PPC_WIFI_ADC                  = PPC_WIFI_REG1 | 28,	/* S16-22 adc */
	PPC_WIFI_MST_SPI0             = PPC_WIFI_REG1 | 29,	/* S16-23 mst_spi0 */
	PPC_WIFI_SLV_SPI0             = PPC_WIFI_REG1 | 30,	/* S16-24 slv_spi0 */

	/* WIFI PPC2_REG (RXI_WIFI SECURE_1), bit value 1: only secure can reach the slave */
	PPC_WIFI_WMAC_SLV             = PPC_WIFI_REG2 | 0,	/* S17-1  wmac_slv */
	PPC_WIFI_UART0                = PPC_WIFI_REG2 | 1,	/* S17-5  uart0 */
	PPC_WIFI_TIMER0               = PPC_WIFI_REG2 | 2,	/* S17-8  timer0 */
	PPC_WIFI_WDT_KM0              = PPC_WIFI_REG2 | 3,	/* S17-14 wdt_km0 */
	PPC_WIFI_AON_REG              = PPC_WIFI_REG2 | 4,	/* S17-17 aon_reg */
	PPC_WIFI_SYSON_GPIO           = PPC_WIFI_REG2 | 5,	/* S17-18 syson_gpio */
	PPC_WIFI_PWM                  = PPC_WIFI_REG2 | 6,	/* S17-19 pwm */
	PPC_WIFI_AON_GPIO             = PPC_WIFI_REG2 | 7,	/* S17-20 aon_gpio */
	PPC_WIFI_SYSON_REG            = PPC_WIFI_REG2 | 8,	/* S17-21 syson_reg */
	PPC_WIFI_WIFI_IPC             = PPC_WIFI_REG2 | 9,	/* S17-22 wifi_ipc */
	PPC_WIFI_RXI300_WIFI          = PPC_WIFI_REG2 | 10,	/* S17-23 rxi300_wifi */
} PPC_Id;

#define SECURE				1
#define NON_SECURE			0

/* Not Support MPC_RW/MPC_RO */
/* 0 : only secure reach slave, 1 : secure and non-secure can reach slave */
#define MPC_S				RXI_WIFI_IDAU_CTRL_ENABLE(0)
#define MPC_NS				RXI_WIFI_IDAU_CTRL_ENABLE(1)

/*
  *   <q> Enable SAU
  *   <i> Value for SAU->CTRL register bit ENABLE
  */
#define SAU_INIT_CTRL_ENABLE   1

/*
  *   <o> When SAU is disabled
  *     <0=> All Memory is Secure
  *     <1=> All Memory is Non-Secure
  *   <i> Value for SAU->CTRL register bit ALLNS
  *   <i> When all Memory is Non-Secure (ALLNS is 1), IDAU can override memory map configuration.
  */
#define SAU_INIT_CTRL_ALLNS  0

#define SAU_ENTRY_NUM			8

#define MPC_ENTRY_NUM			8
#define MPC_CNT					3

/**
  * @brief  Configure a slave port as secure or non-secure attribute.
  * @param  Id: slave port, one of PPC_Id. The register and the bit are derived from the id.
  * @param  Status: it can be SECURE or NON_SECURE
  * @note   The bit polarity differs between registers (IDAU_CTRL is the opposite of the PPC registers), it is handled here.
  *     Nothing is written if Id is not a valid PPC_Id.
  * @retval None
  */
__STATIC_FORCEINLINE void TZ_ConfigSlaveSecurity(PPC_Id Id, u32 Status)
{
	volatile u32 *Reg;
	u32 Bit = 1U << (Id & 0xFF);
	u32 NonSecure = (Status == NON_SECURE);

	switch (Id & ~0xFF) {
	case PPC_CORE_REG2:
		Reg = &CORE_PPC->PPC2_REG;
		break;
	case PPC_CORE_IDAU:
		Reg = &CORE_MPC->IDAU_CTRL;
		NonSecure = !NonSecure;			/* IDAU_CTRL: bit value 1 is non-secure, opposite of the PPC registers */
		break;
	case PPC_WIFI_REG1:
		Reg = &WIFI_PPC->PPC1_REG;
		break;
	case PPC_WIFI_REG2:
		Reg = &WIFI_PPC->PPC2_REG;
		break;
	default:
		return;
	}

	/* PPC registers: bit value 1 is secure, 0 is non-secure */
	if (NonSecure) {
		*Reg &= ~Bit;
	} else {
		*Reg |= Bit;
	}
}

/* 	The TT instruction takes a memory address and returns the configuration of the Memory Protection Unit (MPU) at that address.
	The cmse_address_info_t is declared as following:
		typedef union {
			struct cmse_address_info {
				unsigned mpu_region:8;
				unsigned :8;
				unsigned mpu_region_valid:1;
				unsigned :1;
				unsigned read_ok:1;
				unsigned readwrite_ok:1;
				unsigned :12;
			} flags;
			unsigned value;
		} cmse_address_info_t;

	When executed in the secure state the result of TT instruction is extended to return the SAU and IDAU configurations at the specific address.
	The extended cmse_address_info_t is declared as following:
		typedef union {
			struct cmse_address_info {
				unsigned mpu_region:8;
				unsigned sau_region:8;
				unsigned mpu_region_valid:1;
				unsigned sau_region_valid:1;
				unsigned read_ok:1;
				unsigned readwrite_ok:1;
				unsigned nonsecure_read_ok:1;
				unsigned nonsecure_readwrite_ok:1;
				unsigned secure:1;
				unsigned idau_region_valid:1;
				unsigned idau_region:8;
			} flags;
			unsigned value;
		} cmse_address_info_t;

	As a result, these extended bits are only valid when executing in Secure state, and are UNDEFINED if used from Non-secure state.
*/
__STATIC_FORCEINLINE u32 TrustZone_IsSecure(void)
{
#if defined(__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
	cmse_address_info_t cmse_address_info = cmse_TT((void *)DiagPrintf);
	return cmse_address_info.flags.secure;
#else
	return 0;
#endif
}

fih_ret BOOT_CPU_TZCfg(const SAU_CFG_TypeDef *sau_config);
#endif
