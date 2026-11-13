/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_PMC_H_
#define _AMEBA_PMC_H_

/** @addtogroup AmebaD_Platform
  * @{
  */

/** @defgroup PMC
  * @brief PMC driver modules
  * @{
  */

/** @addtogroup PMC
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * we support  following soc power save functions:
  *		- sleep clock gating
  *		- sleep power gating
  *		- deep standby
  *		- deep sleep
  *
  *****************************************************************************************
  * sleep power gating
  *****************************************************************************************
  * following functions can be used when power gating:
  *		-UART0/UART1
  *		-TIM4/TIM5
  *		-RTC
  *		-WIFI
  *		-SDIO
  *		-USB
  *		-I2C0/I2C1
  *		-ADC
  *		-GPIO
  *		-REGU timer
  *		-normal wakepin
  *		-ANA timer
  * following functions will be closed when power gating:
  *		-UART2 LOGUART
  *		-TIM0-TIM3
  *		-SPIC flash
  *
  *****************************************************************************************
  * deep standby
  *****************************************************************************************
  * following functions can be used when deep standby:
  *		-RTC
  *		-REGU timer
  *		-normal wakepin
  *		-ANA timer
  *
  *****************************************************************************************
  * deep sleep
  *****************************************************************************************
  * following functions can be used when deep standby:
  *		-REGU timer
  *		-REGU wakepin
  *
  *****************************************************************************************
  * wakepin (A18/A5/A22/A23: mux normal wakepin and REGU wakepin)
  *****************************************************************************************
  * normal wakepin:
  *		-SLP_CG
  *		-SLP_PG
  *		-STDBY
  * REGU wakepin:
  *		-just used in DSLP (1.2V closed)
  *		-just support high acive, so this pin should pull low on your board
  *
  *****************************************************************************************
  *****************************************************************************************
  * SLP & SNZ power option
  *****************************************************************************************
  * BIT_SYSON_PMOPT_SLP_EN_SWR & BIT_SYSON_PMOPT_SNZ_EN_SWR
  *		-we have two 1.2V LDO
  *			-BIG LDO: SWR mode or LDO mode (can config )
  *			-LITTLE LDO: a little 1.2v LDO
  *		-BIT_SYSON_PMOPT_SLP_EN_SWR
  *			-ENABLE/DISABLE BIG LDO when SLP
  *		BIT_SYSON_PMOPT_SNZ_EN_SWR
  *			-ENABLE/DISABLE BIG LDO when SNZ, WIFI & ADC need open BIG LDO when SNZ
  *
  * BIT_SYSON_PMOPT_SLP_EN_PWM & BIT_SYSON_PMOPT_SNZ_EN_PWM
  *		-BIT_SYSON_PMOPT_SLP_EN_PWM
  *			-ENABLE/DISABLE LDO heavy loading current mode when SLP
  *		-BIT_SYSON_PMOPT_SNZ_EN_PWM
  *			-ENABLE/DISABLE heavy loading current mode when SNZ, WIFI & ADC need heavy loading when SNZ
  *
  * BIT_SYSON_PMOPT_SLP_XTAL_EN & BIT_SYSON_PMOPT_SNZ_XTAL_EN
  *		-WIFI and SOC both need XTAL when work,
  *		-but WIFI have individual option to control XTAL, so BIT_SYSON_PMOPT_SNZ_XTAL_EN not needed
  *
  * BIT_SYSON_PMOPT_SLP_SYSPLL_EN & BIT_SYSON_PMOPT_SNZ_SYSPLL_EN
  *		-WIFI and SOC both have individual PLL, here is SOC 500M PLL
  *		-So BIT_SYSON_PMOPT_SNZ_SYSPLL_EN not needed
  *
  * BIT_SYSON_SNFEVT_WIFI_MSK = 1 & BIT_SYSON_BYPASS_SNZ_SLP = 1
  *		- after OS suspend, platform will enter SNZ and close CPU, then platform enter sleep mode when WIFI 32K
  *		- BIT_SYSON_PMOPT_SNZ_EN_SOC should never open, or CPU will not close when platform will enter SNZ
  *
  * BIT_SYSON_SNFEVT_WIFI_MSK = 1 & BIT_SYSON_BYPASS_SNZ_SLP = 0 (not use this config)
  *		- after OS suspend, platform will enter sleep mode & close CPU after WIFI 32K
  *****************************************************************************************
  * @endverbatim
  */

/** @defgroup sleep_parameter np & ap sleep parameter
  * @{
  */
#ifndef CONFIG_BUILD_ROM

typedef struct {
	u32	sleep_type;
	u32	sleep_time;
	u32	dlps_enable;
	u32	rsvd[5];
} SLEEP_ParamDef;

#endif

enum SLEEP_TYPE {
	SLEEP_ACTIVE    = 0,
	SLEEP_CG	= 1,
	SLEEP_PG	= 2,
	SLEEP_DEEP_SLEEP = 3,
};

enum pmc_state_e {
	PMC_OFF         = 0x00,
	PMC_SNOZCOMBO   = 0x01,
	PMC_SNOZ_WIFI   = 0x02,
	PMC_SNOZ_MCU    = 0x03,
	PMC_STANDBY     = 0x04,
	PMC_DLSP        = 0x05,
};

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup PMC_Exported_Constants PMC Exported Constants
  * @{
  */


/** @defgroup MEM_Mode_definitions
  * @{
  */
#define MEM_ACT_MODE			((u32)0x00000000)
#define MEM_LS_MODE				((u32)0x00000001)
#define MEM_DS_MODE				((u32)0x00000002)
#define MEM_SD_MODE				((u32)0x00000004)
#define IS_MEM_MODE(MODE) (((MODE) == MEM_ACT_MODE) || \
									   ((MODE) == MEM_LS_MODE) || \
									   ((MODE) == MEM_DS_MODE) || \
									   ((MODE) == MEM_SD_MODE))
/** @defgroup NP_WAKE_AP by IPC list
  * @{
  */
#define FW_NPWAP_IPC			0
#define TIMER_NPWAP_IPC			1

#define NP_STATUS_BIT     (1U << 16)  // bit 16
#define PMC_STATUS_BIT    (1U << 15)  // bit 15

//extern PSCFG_TypeDef ps_config;
extern uint32_t psp_temp;
extern uint32_t psplim_temp;

#define WAKE_SRC_GPIOA3       AON_BIT_AON_WEVT_AON_GPIO3_MSK
#define WAKE_SRC_GPIOA2       AON_BIT_AON_WEVT_AON_GPIO2_MSK
#define WAKE_SRC_GPIOA1       AON_BIT_AON_WEVT_AON_GPIO1_MSK
#define WAKE_SRC_GPIOA0       AON_BIT_AON_WEVT_AON_GPIO0_MSK
#define WAKE_SRC_RTC          AON_BIT_AON_WEVT_AON_RTC_MSK
#define WAKE_SRC_COMP         AON_BIT_AON_WEVT_AON_COMP_MSK
#define WAKE_SRC_AON_TIM      AON_BIT_AON_WEVT_AON_TIMER_MSK
#define WAKE_SRC_LOGUART      AON_BIT_PON_WEVT_LOGUART_MSK
#define WAKE_SRC_WLAN1        AON_BIT_PON_WEVT_WLAN1_MSK
#define WAKE_SRC_WLAN         AON_BIT_PON_WEVT_WLAN_MSK
#define WAKE_SRC_PWM          AON_BIT_PON_WEVT_PWM_MSK
#define WAKE_SRC_PON_GPIO     AON_BIT_PON_WEVT_GPIO_MSK
#define WAKE_SRC_PON_TIM      AON_BIT_PON_WEVT_GTIM0_MSK

uint32_t ap_status_on(void);
uint32_t pmc_status_active(void);
void pmc_set_active(uint32_t type);

void SOCPS_SleepPG_RAM(void);
void SOCPS_SleepCG_RAM(void);
void SOCPS_DeepSleep_RAM(void);
void SOCPS_Snzmcu_To_Snzcombo(void);
void SOCPS_Snzcombo_To_Active(void);
void SOCPS_Active_To_Snzcombo(void);
void SOCPS_Snzcombo_To_Snzmcu(void);
void SOCPS_Snzcombo_To_Snzwifi(void);
void SOCPS_Snzmcu_To_Stanby(void);
void SOCPS_Snzmcu_To_Dlsp(void);
void SOCPS_Snzwifi_To_Snzcombo(void);
void SOCPS_Snzwifi_To_Stanby(void);
void SOCPS_SleepInit(void);
void SOCPS_Sleep_FP(void);
void SOCPS_SetAPWakeEvent(u32 Option, u32 NewStatus);
int SOCPS_AONWakeReason(void);
uint32_t SOCPS_GetWakeReason(uint8_t index);
void SOCPS_Sleep(void);
void SOCPS_DeepSleep(void);
void SOCPS_AP_SetWakeSrc(u32 wake_src_mask, u32 wake_src_pol, u32 timer_ms);

#endif //_AMEBA_PMC_H_
