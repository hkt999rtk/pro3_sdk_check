/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _HAL_PLATFORM_
#define _HAL_PLATFORM_

#if !defined(__ASSEMBLER__)
#include "basic_types.h"
#endif

#define ROMVERSION			0x02
#define ROMVERSION_SUB		0 /* ACUT is v1.0, BCUT is v1.1 ... */

#define HAL_READ32(base, addr)				rtk_le32_to_cpu(*((volatile u32*)(base + addr)))
#define HAL_WRITE32(base, addr, value32)		((*((volatile u32*)(base + addr))) = rtk_cpu_to_le32(value32))
#define HAL_READ16(base, addr)				rtk_le16_to_cpu(*((volatile u16*)(base + addr)))
#define HAL_WRITE16(base, addr, value)		((*((volatile u16*)(base + addr))) = rtk_cpu_to_le16(value))
#define HAL_READ8(base, addr)				(*((volatile u8*)(base + addr)))
#define HAL_WRITE8(base, addr, value)		((*((volatile u8*)(base + addr))) = value)


#ifdef __cplusplus
#define __I		volatile             /*!< Defines 'read only' permissions                 */
#else
#define __I		volatile const       /*!< Defines 'read only' permissions                 */
#endif
#define __O		volatile             /*!< Defines 'write only' permissions                */
#define __IO	 	volatile             /*!< Defines 'read / write' permissions              */


#include "hal_core_base.h"
#include "hal_wifi_base.h"

/* TBD AutoGen */

#define SPI_FLASH_BASE			SPIC_AUTO_REG_BASE
#define USB_REG_BASE 			USB_OTG_REG_BASE
#define RSIP_REG_BASE			FLASH_SEC_REG_BASE
#define WIFI_REG_BASE			WMAC_REG_BASE


//#define AON_REG_BASE			0x40220000
#define PON_REG_BASE			(u32)(SYSON_BASE)
#define WLOFF_REG_BASE			(u32)(WIFIOFF_BASE)

// MBIST_CTRL_register_file
#define MBISTCTRL_REG_BASE		0x6020C800		// MBIST

#define GDMA0_REG_BASE			DMAC0_REG_BASE
#define GDMA1_REG_BASE			DMAC1_REG_BASE
#define GDMA0_REG_BASE_S		DMAC0_REG_BASE_S
#define GDMA1_REG_BASE_S		DMAC1_REG_BASE_S

// PAD_register_file
#define AON_PAD_REG_BASE		AON_PAD_BASE		// AON
#define PON_PAD_REG_BASE		PON_PAD_BASE		// PON aka PRO3-SYSON
#define CORE_PAD_REG_BASE		CORE_PAD_BASE		// SYSON aka PRO3-CORE

// AIP_register_file
#define REGU_REG_BASE			REGU_BASE		// AON
#define XTAL_REG_BASE			XTAL_BASE		// AON

// CPU_SUBSYS_register_file_20240710
#define KM4_MP_REG_BASE			KM4_MP_BASE
#define KM4_NP_REG_BASE			KM4_NP_BASE
#define KM0_REG_BASE			KM0_BASE
#define CSSYS_AUX_REG_BASE		CSSYS_AUX_BASE

// CA32_CPU_TOP_register_file_20240627
#define CA32_VSFC_REG_BASE		VSFC_BASE
#define CA32_CTRL_REG_BASE		CA32_CTRL_BASE

// SRAM BASE for os_wrapper_memory.c (that is not good to use this define in os wrapper)
#define SRAM_BASE 				SYS_MEM_BASE
#define PSRAM_BASE 				PSRAMC_AUTO_REG_BASE

/* FIXME start check remove after update config form */
#define OTPC_REG_BASE           			0x40224000
#define OTPC_REG_BASE_S 					0
#define UARTLOG_REG_BASE 					UART0_REG_BASE
#define UARTLOG_REG_BASE_S 					UART0_REG_BASE_S

#define AON_BACKUP_BASE                     (AON_REG_BASE + 0x00AC) //(&(AON_BASE->AON_SW_BOOT0))
#define AON_BACKUP2_BASE                    (AON_NS_REG_BASE + 0x0010) //(&(AON_NS_BASE->AON_SW_BOOT0))

//#define PMC_BASE                           0
//#define CPU_MMU_BASE_S                     0
//#define SDM_REG_BASE                       0
//#define PINMUX_REG_BASE                    0
//#define PINMUX_REG_BASE_S                  0
//#define SYSTEM_MEM_CTRL_BASE               0
//#define RTC_BASE                           0
//#define IWDG_REG_BASE                      0
//#define WDG0_REG_BASE                      0
//#define WDG1_REG_BASE_S                    0
//#define WDG2_REG_BASE                      0
//#define SDIO_REG_BASE					   0


//#define RETENTION_RAM_BASE                 0

//#define DATA_FLASH_BASE          			0x60000000
/* FIXME end check */

/** @} End of group AmebaD_Address_Map */

#define TZ_IDAU_SEC_OFFSET(x)	((x) + 0x10000000)

#if !defined(__ASSEMBLER__)

/**************************************************************************//**
 * @defgroup AmebaD_Peripheral_Declaration  AmebaD HS/LP Peripheral Declarations
 * @{
 *****************************************************************************/

#define UART0_DEV			((UART_TypeDef			*) UART3_REG_BASE)		/*uart0 */
#define UART1_DEV			((UART_TypeDef			*) UART1_REG_BASE)		/*uart1 */
#define UART2_DEV			((UART_TypeDef			*) UART4_REG_BASE)		/*uart2 */
#define UART3_DEV			((UART_TypeDef			*) UART2_REG_BASE)		/*uart3 */
#define UART0_DEV_S			((UART_TypeDef			*) UART3_REG_BASE_S)		/*uart0 secure */
#define UART1_DEV_S			((UART_TypeDef			*) UART1_REG_BASE_S)		/*uart1 secure*/
#define UART2_DEV_S			((UART_TypeDef			*) UART4_REG_BASE_S)		/*uart2 secrue*/
#define UART3_DEV_S			((UART_TypeDef			*) UART2_REG_BASE_S)		/*uart3 secure*/

#define LOGUART_DEV			((LOGUART_TypeDef		*) UARTLOG_REG_BASE)
#define LOGUART_DEV_S		((LOGUART_TypeDef		*) UARTLOG_REG_BASE_S)

#define SPI0_MST_DEV		((SPI_TypeDef			*) MST_SPI0_REG_BASE)		/*spi0 master*/
#define SPI1_MST_DEV		((SPI_TypeDef			*) MST_SPI1_REG_BASE)		/*spi1 master*/
#define SPI0_SLV_DEV		((SPI_TypeDef			*) SLV_SPI0_REG_BASE)		/*spi0 slave*/
#define SPI1_SLV_DEV		((SPI_TypeDef			*) SLV_SPI1_REG_BASE)		/*spi1 slave*/

#define SPI0_DEV_S			((SPI_TypeDef			*) MST_SPI0_REG_BASE_S)		/*spi0 secure */
#define SPI1_DEV_S			((SPI_TypeDef			*) MST_SPI1_REG_BASE_S)		/*spi1 secure */

#define PSRAMC_DEV			((SPIC_TypeDef			*) PSRAMC_USER_REG_BASE)				/*AP */
#define PSRAMPHY_DEV		((PSPHY_TypeDef			*) PSRAM_PHY_REG_BASE)				/*AP */

#define SPIC				((SPIC_TypeDef			*) SPIC_USER_REG_BASE)
#define NAND_ECC			((Lalu_TypeDef 			*) NAND_ECC_REG_BASE) /*Lalu NAND ECC Engine */
//#define SPIC_COMBO			((SPIC_TypeDef			*) PSRAM_REG_BASE)
#define ADC_DEV				((ADC_TypeDef			*) ADC_REG_BASE)
#define ADC_DEV_S			((ADC_TypeDef			*) ADC_REG_BASE_S)
#define I2C0_DEV			((I2C_TypeDef			*) I2C0_REG_BASE)		/*i2c0 */
#define I2C1_DEV			((I2C_TypeDef			*) I2C1_REG_BASE)		/*i2c1 */
#define I2C2_DEV			((I2C_TypeDef			*) I2C2_REG_BASE)		/*i2c2 */
#define I2C3_DEV			((I2C_TypeDef			*) I2C3_REG_BASE)		/*i2c3 */
#define I2C0_DEV_S			((I2C_TypeDef			*) I2C0_REG_BASE_S)		/*i2c0 secure */
#define I2C1_DEV_S			((I2C_TypeDef			*) I2C1_REG_BASE_S)		/*i2c1 secure */
#define I2C2_DEV_S			((I2C_TypeDef			*) I2C2_REG_BASE_S)		/*i2c2 secure */
#define I2C3_DEV_S			((I2C_TypeDef			*) I2C3_REG_BASE_S)		/*i2c3 secure */

#define GMAC_DEV			((ETHERNET_TypeDef		*) RMII_REG_BASE)

#define GTIMER0				((TG_TypeDef			*) TIMER0_REG_BASE)
#define GTIMER1				((TG_TypeDef			*) TIMER1_REG_BASE)
#define GTIMER2				((TG_TypeDef			*) TIMER2_REG_BASE)
#define GTIMER3				((TG_TypeDef			*) TIMER3_REG_BASE)

#define GTIMER0_S			((TG_TypeDef			*) TIMER0_REG_BASE_S)
#define GTIMER1_S			((TG_TypeDef			*) TIMER1_REG_BASE_S)
#define GTIMER2_S			((TG_TypeDef			*) TIMER2_REG_BASE_S)
#define GTIMER3_S			((TG_TypeDef			*) TIMER3_REG_BASE_S)

#define TIM0				((RTIM_TypeDef			*) (TIMER0_REG_BASE + 0x040))
#define TIM1				((RTIM_TypeDef			*) (TIMER0_REG_BASE + 0x080))
#define TIM2				((RTIM_TypeDef			*) (TIMER0_REG_BASE + 0x0C0))
#define TIM3				((RTIM_TypeDef			*) (TIMER0_REG_BASE + 0x100))
#define TIM4				((RTIM_TypeDef			*) (TIMER0_REG_BASE + 0x140))
#define TIM5				((RTIM_TypeDef			*) (TIMER0_REG_BASE + 0x180))
#define TIM6				((RTIM_TypeDef			*) (TIMER0_REG_BASE + 0x1C0))
#define TIM7				((RTIM_TypeDef			*) (TIMER0_REG_BASE + 0x200))
#define TIM8				((RTIM_TypeDef			*) (TIMER1_REG_BASE + 0x040))
#define TIM9				((RTIM_TypeDef			*) (TIMER2_REG_BASE + 0x040))
#define TIM10				((RTIM_TypeDef			*) (TIMER2_REG_BASE + 0x080))
#define TIM11				((RTIM_TypeDef			*) (TIMER2_REG_BASE + 0x0C0))
#define TIM12				((RTIM_TypeDef			*) (TIMER2_REG_BASE + 0x100))
#define TIM13				((RTIM_TypeDef			*) (TIMER2_REG_BASE + 0x140))
#define TIM14				((RTIM_TypeDef			*) (TIMER2_REG_BASE + 0x180))
#define TIM15				((RTIM_TypeDef			*) (TIMER2_REG_BASE + 0x1C0))
#define TIM16				((RTIM_TypeDef			*) (TIMER2_REG_BASE + 0x200))
#define TIM17				((RTIM_TypeDef			*) (TIMER3_REG_BASE + 0x040))

#define TIM0_S				((RTIM_TypeDef			*) (TIMER0_REG_BASE_S + 0x040))
#define TIM1_S				((RTIM_TypeDef			*) (TIMER0_REG_BASE_S + 0x080))
#define TIM2_S				((RTIM_TypeDef			*) (TIMER0_REG_BASE_S + 0x0C0))
#define TIM3_S				((RTIM_TypeDef			*) (TIMER0_REG_BASE_S + 0x100))
#define TIM4_S				((RTIM_TypeDef			*) (TIMER0_REG_BASE_S + 0x140))
#define TIM5_S				((RTIM_TypeDef			*) (TIMER0_REG_BASE_S + 0x180))
#define TIM6_S				((RTIM_TypeDef			*) (TIMER0_REG_BASE_S + 0x1C0))
#define TIM7_S				((RTIM_TypeDef			*) (TIMER0_REG_BASE_S + 0x200))
#define TIM8_S				((RTIM_TypeDef			*) (TIMER1_REG_BASE_S + 0x040))
#define TIM9_S				((RTIM_TypeDef			*) (TIMER2_REG_BASE_S + 0x040))
#define TIM10_S				((RTIM_TypeDef			*) (TIMER2_REG_BASE_S + 0x080))
#define TIM11_S				((RTIM_TypeDef			*) (TIMER2_REG_BASE_S + 0x0C0))
#define TIM12_S				((RTIM_TypeDef			*) (TIMER2_REG_BASE_S + 0x100))
#define TIM13_S				((RTIM_TypeDef			*) (TIMER2_REG_BASE_S + 0x140))
#define TIM14_S				((RTIM_TypeDef			*) (TIMER2_REG_BASE_S + 0x180))
#define TIM15_S				((RTIM_TypeDef			*) (TIMER2_REG_BASE_S + 0x1C0))
#define TIM16_S				((RTIM_TypeDef			*) (TIMER2_REG_BASE_S + 0x200))
#define TIM17_S				((RTIM_TypeDef			*) (TIMER3_REG_BASE_S + 0x040))

#define PWM_DEV             ((PWM_COMM_TypeDef      *) (PWM_REG_BASE))
#define PWM_DEV_S           ((PWM_COMM_TypeDef      *) (PWM_REG_BASE_S))

//#define PMCTIMER_DEV0		((PMCTIMER_TpyeDef		*) PMC_TIMER_REG_BASE)
//#define PMCTIMER_DEV1		((PMCTIMER_TpyeDef		*) (PMC_TIMER_REG_BASE + 0x200))
//#define PMCTIMER_DEV0_S		((PMCTIMER_TpyeDef		*) PMC_TIMER_REG_BASE_S)
//#define PMCTIMER_DEV1_S		((PMCTIMER_TpyeDef		*) (PMC_TIMER_REG_BASE_S + 0x200))

//#define RTC_DEV				((RTC_TypeDef			*) RTC_BASE)					/*NP */
//#define RTC_DEV_S			((RTC_TypeDef			*) TZ_IDAU_SEC_OFFSET(RTC_BASE))
#define PINMUX				((PINMUX_TypeDef		*) PINMUX_REG_BASE)			/*NP */
#define PINMUX_S			((PINMUX_TypeDef		*) PINMUX_REG_BASE_S)		/*NP */

#define AON_GPIO_BASE		((GPIO_TypeDef			*) AON_GPIO_REG_BASE)
#define PON_GPIO_BASE		((GPIO_TypeDef			*) SYSON_GPIO_REG_BASE)
#define GPIO_BASE			((GPIO_TypeDef			*) CORE_GPIO_REG_BASE)

#define AON_GPIO_BASE_S		((GPIO_TypeDef			*) AON_GPIO_REG_BASE_S)
#define PON_GPIO_BASE_S		((GPIO_TypeDef			*) SYSON_GPIO_REG_BASE_S)
#define GPIO_BASE_S			((GPIO_TypeDef			*) CORE_GPIO_REG_BASE_S)


#define BACKUP_REG			((BACKUP_REG_TypeDef	*) AON_BACKUP_BASE)			            /*AON */
#define BACKUP2_REG			((BACKUP_REG_TypeDef	*) AON_BACKUP2_BASE)			        /*AON_NS */

#define WIFI_MPC			((MPC_TypeDef			*) (RXI300_WIFI_REG_BASE + 0x0100))		/* &(RXI_WIFI_BASE->RBARA0), WIFI DOMAIN MPC */
#define CORE_MPC			((MPC_TypeDef			*) (RXI300_CORE_REG_BASE + 0x0500)) 	/* &(RXI_CORE_BASE->RBARA0), CORE DOMAIN MPC */

#define WIFI_PPC			((PPC_TypeDef			*) (RXI300_WIFI_REG_BASE + 0x0400)) 	/* &(RXI_WIFI_BASE->CTR_AND_SECURE_0), WIFI DOMAIN PPC */
#define CORE_PPC			((PPC_TypeDef			*) (RXI300_CORE_REG_BASE + 0x030C))		/* &(RXI_CORE_BASE->GPR_0), CORE DOMAIN PPC */

//#define AP_MPC1			((MPC_TypeDef			*) MPC1_BASE_S)			/*AP */
//#define AP_MPC2			((MPC_TypeDef			*) MPC2_BASE_S)			/*AP */
//#define AP_MPC3			((MPC_TypeDef			*) MPC3_BASE_S)			/*AP */
//#define AP_PPC				((PPC_TypeDef			*) PPC_BASE_S)			/*AP */

#define IPCNP_REG_BASE		(WIFI_IPC_REG_BASE + 0x000)
#define IPCVP_REG_BASE		(WIFI_IPC_REG_BASE + 0x200)
#define IPCAP_REG_BASE		(WIFI_IPC_REG_BASE + 0x400)
#define IPCFP_REG_BASE		(WIFI_IPC_REG_BASE + 0x600)
#define IPCSE_REG_BASE		(WIFI_IPC_REG_BASE + 0x800)

#define IPCNP_REG_BASE_S	(WIFI_IPC_REG_BASE_S + 0x000)
#define IPCVP_REG_BASE_S	(WIFI_IPC_REG_BASE_S + 0x200)
#define IPCAP_REG_BASE_S	(WIFI_IPC_REG_BASE_S + 0x400)
#define IPCFP_REG_BASE_S	(WIFI_IPC_REG_BASE_S + 0x600)

#define IPCNP_DEV 			((IPC_TypeDef 			*) IPCNP_REG_BASE)							/*NP */
#define IPCVP_DEV 			((IPC_TypeDef 			*) IPCVP_REG_BASE)							/*MP */
#define IPCAP_DEV 			((IPC_TypeDef 			*) IPCAP_REG_BASE)							/*AP */
#define IPCFP_DEV 			((IPC_TypeDef 			*) IPCFP_REG_BASE)							/*FP */
#define IPCSE_DEV 			((IPC_SEMA_TypeDef 		*) IPCSE_REG_BASE)							/*PS */
#define IPCNP_DEV_S 		((IPC_TypeDef 			*) IPCNP_REG_BASE_S)						/*NP Secure */
#define IPCVP_DEV_S 		((IPC_TypeDef 			*) IPCVP_REG_BASE_S)						/*NP Secure */
#define IPCAP_DEV_S 		((IPC_TypeDef 			*) IPCAP_REG_BASE_S)						/*AP Secure*/
#define IPCFP_DEV_S 		((IPC_TypeDef 			*) IPCFP_REG_BASE_S)						/*NP Secure */
//#define GDMA_BASE			((GDMA_TypeDef			*) (GDMA0_REG_BASE))

#define GDMA0_BASE			((GDMA_TypeDef			*) (GDMA0_REG_BASE))
#define GDMA1_BASE			((GDMA_TypeDef			*) (GDMA1_REG_BASE))

//#define IWDG_DEV			((WDG_TypeDef			*) (IWDG_REG_BASE))

#define NP_WDG_DEV			((WDG_TypeDef			*) (WDT_NETWORK_REG_BASE))
#define FP_WDG_DEV			((WDG_TypeDef			*) (WDT_KM0_REG_BASE))
#define	AP_WDG_DEV			((WDG_TypeDef			*) (WDT_CA32_REG_BASE))
#define VP_WDG_DEV			((WDG_TypeDef			*) (WDT_MEDIA_REG_BASE))


//#define SDM_DEV				((SDM_TypeDef			*) (SDM_REG_BASE))
//#define SDM_DEV_S			((SDM_TypeDef			*) (SDM_REG_BASE_S))
#define OTPC_DEV			((OTPC_TypeDef			*) (OTPC_REG_BASE))
#define OTPC_DEV_S			((OTPC_TypeDef			*) (OTPC_REG_BASE_S))
//#define DTIMER_DEV			((DEBUGTIMER_TypeDef	*) (DEBUGTIMER_REG_BASE))
//#define DTIMER_DEV_S		((DEBUGTIMER_TypeDef	*) (DEBUGTIMER_REG_BASE_S))
#define TRNG_DEV			((TRNG_TypeDef			*) (TRNG_REG_BASE))
#define TRNG_SDEV			((TRNG_TypeDef			*) (TRNG_REG_BASE_S))

#define RSIP_BASE			((RSIP_REG_TypeDef		*) (RSIP_REG_BASE))

#define AUDIO_SPORT0_DEV	((AUDIO_SPORT_TypeDef	*) SPORT0_REG_BASE)
#define AUDIO_SPORT0_DEV_S	((AUDIO_SPORT_TypeDef	*) SPORT0_REG_BASE_S)	/*SPORT0 secure */
#define AUDIO_SPORT1_DEV	((AUDIO_SPORT_TypeDef	*) SPORT1_REG_BASE)
#define AUDIO_SPORT1_DEV_S	((AUDIO_SPORT_TypeDef	*) SPORT1_REG_BASE_S)	/*SPORT1 secure */

#define SDIO_CORE			((SDIO_TypeDef			*) SDIO_REG_BASE)
//#define SDIO_WIFI			((SDIO_TypeDef			*) SDIO_REG_BASE)

#define SD_HOST_DEV         ((SD_HOST_TypeDef       *) (LX_REG_BASE + 0x4000))

#define DDRPHY_DEV			((DDRPHY_ASIC_TypeDef 	*) PHY_CFG_REG_BASE)
#define DDRPHY_FPGA_DEV		((DDRPHY_FPGA_TypeDef	*) PHY_CFG_REG_BASE)
#define DDRC_DEV			((DDRC_TypeDef 			*) DRAM_REG_BASE)
#define DDRC_FE_DEV			((DDRC_FE_TypeDef		*) (DRAM_REG_BASE + 0x1000))
#define DDRC_TOP_DEV		((DDRC_TOP_TypeDef		*) (DRAM_REG_BASE + 0x2000))
#define DDRC_CFG_DEV		((DDRC_CFG_TypeDef		*) DDRC_CFG_REG_BASE)
#define TG_DEV				((TRAFFIC_GEN_TypeDef   *) TRAFFIC_GEN_REG_BASE)
#define NN_QOS_DEV			((TRAFFIC_QOS_TypeDef   *) (RXI300_CORE_REG_BASE + 0x700))
#define ISP_QOS_DEV			((TRAFFIC_QOS_TypeDef   *) (RXI300_CORE_REG_BASE + 0x740))

#define SYS1PLL_TRACK_DEV	((SYSPLL_TRACKING_CONTROL *) (&SYSON_BASE->SYS1PLL_TRACKING_ENTRY0_0))
#define SYS2PLL_TRACK_DEV	((SYSPLL_TRACKING_CONTROL *) (&SYSON_BASE->SYS2PLL_TRACKING_ENTRY0_0))
#define PERIPLL_TRACK_DEV	((SYSPLL_TRACKING_CONTROL *) (&SYSON_BASE->PERIPLL_TRACKING_ENTRY0_0))
#define DDRPLL_TRACK_DEV	((DDRPLL_TRACKING_CONTROL *) (&CORE_S_BASE->DDRPLL_TRACKING_ENTRY0_0))

#ifdef FEATURE_TODO
// check all address and think a better solution
#endif

extern u8 __StackLim[];
extern u8 __StackTop[];
extern u8 __LoaderTop[];
extern u8 __LoaderLim[];

#define MSPLIM_RAM_NP				(u32)__StackLim					/* rom used, can not change */
#define MSP_RAM_NP					(u32)__StackTop					/* rom used, can not change */
#define LOADERTOP					(u32)__LoaderTop				/* rom used, can not change */
#define LOADERLIM					(u32)__LoaderLim				/* rom used, can not change */
#endif

#if defined(CONFIG_ARM_CORE_CM4_NP) && (CONFIG_ARM_CORE_CM4_NP==1)
#if defined(CONFIG_BUILD_ROM) && (CONFIG_BUILD_ROM==1)
#define MSP_RAM_NP_NS				(0x00100000 + 0x2000)						/* set to DTCM, not use in ROM */
#else
//extern u8 __StackLimNS[];
extern u8 __StackTopNS[];
//#define MSPLIM_RAM_NP_NS			(u32)__StackLimNS
#define MSP_RAM_NP_NS				(u32)__StackTopNS
#endif
#endif

/* compatibility for RS code */
/* HP now is mean bootup processor */
#define MSPLIM_RAM_HP				MSPLIM_RAM_NP
#define MSP_RAM_HP					MSP_RAM_NP
#define MSP_RAM_HP_NS				MSP_RAM_NP_NS

/* BOOT LOADER VALID loading address range, set full SRAM for flexible adjustment */
#define BOOT_LOADER_START			(LOADERTOP)					/* rom used, can not change */
#define BOOT_LOADER_LIMIT(ext)		(LOADERLIM+(ext))			/* rom used, can not change */
/* download image to 0x28004000 */
#define LOADER_DOWNLOAD_ADDR		LOADERTOP 		//(BOOT_LOADER_LIMIT - 64 * 1024)			/* rom used, can not change */
#define LOADER_DOWNLOAD_MAXLEN		(LOADERLIM-LOADERTOP)


#ifdef FEATURE_TODO
// change to fix value after rom freezed
#endif


#define HS_SRAM_ADDR_START			SYS_MEM_BASE 					/* rom used, can not change */
#define HS_SRAM_ADDR_END			(SYS_MEM_BASE + 0x7FFFF) 		/* rom used, can not change */

#define BOOTLOADER_XIP_START		0x0C000000						/* rom used, can not change */
#define BOOTLOADER_XIP_END			0x0C800000						/* rom used, can not change */


/*BT share mem with system*/
#define SHARE_MEM_BT_E0_ADDRESS     SHARE_MEM_BASE /*Size:128KB*/


#if defined(RSICV_CORE_KR4)
#define CONTEXT_SAVE_SIZE	320	/* portCONTEXT_SIZE:66*4 = 288 roundup to 64B aligned */
#elif defined(CONFIG_ARM_CORE_CA32)
#define CONTEXT_SAVE_SIZE	320 /* 15*4 + 32*8: general reg and floating reg */
#elif defined(CONFIG_ARM_CORE_CM4)
#define CONTEXT_SAVE_SIZE	192 /* 15*4 + 16*8: s16~s31 if use float */
#elif defined(CONFIG_ARM_CORE_CM0)
#define CONTEXT_SAVE_SIZE	64	/* not support hw float, 15*4 */
#endif

#define CONTEXT_SAVE_SIZE_WITH_MARGIN		(CONTEXT_SAVE_SIZE + 64)

//Reference Green2 setting
#define SYSTEM_CTRL_BASE                   AON_BASE


/** @} End of group AmebaD_Outline */
#endif //_HAL_PLATFORM_
