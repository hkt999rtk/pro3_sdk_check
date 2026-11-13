/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYSREG_LSYS_H_
#define _SYSREG_LSYS_H_

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif

//Please add your defination here

#if !defined(__ASSEMBLER__)
enum CHIP_TYPE {
	CHIP_TYPE_ASIC_POSTSIM = 0,
	CHIP_TYPE_FPGA = 1,
	CHIP_TYPE_PALADIUM = 2,
	CHIP_TYPE_RTLSIM_PRESIM = 3
};
#endif

#define APBPeriph_WLON_CLOCK    ((u32)(1 << 30) | (0x00000001<< 0))  /*!<R/W 0x0  1: Enable WLON clock 0: Disable DD_SEC: bpc_wl*/

/** @defgroup
 * @brief ROM AON domain Function/Clock Control
 * @{
 */

#define RCC_IDMASK	0xFF
#define RCC_LOCMASK	0x3F

#define RCC_GET_REGID(x) (((x)>>24) & RCC_IDMASK)
#define RCC_GET_BITLOC(x)  (((x)>>18) & RCC_LOCMASK)
#define RCC_GET_BITLOC1(x) (((x)>>12) & RCC_LOCMASK)
#define RCC_GET_BITLOC2(x) (((x)>>6 ) & RCC_LOCMASK)
#define RCC_GET_BITLOC3(x) (((x)>>0 ) & RCC_LOCMASK)

#define TOKEN(id, loc0, loc1, loc2, loc3)  (((id)<<24) | ((loc0)<<18) | ((loc1)<<12) | ((loc2)<<6) | ((loc3)<<0) )

#define RCCTOKEN5(regid, bitloc, bitloc1, bitloc2, bitloc3)					TOKEN(regid, bitloc, bitloc1, bitloc2, bitloc3)
#define RCCTOKEN4(regid, bitloc, bitloc1, bitloc2)							TOKEN(regid, bitloc, bitloc1, bitloc2, RCC_LOCMASK)
#define RCCTOKEN3(regid, bitloc, bitloc1)       							TOKEN(regid, bitloc, bitloc1, RCC_LOCMASK, RCC_LOCMASK)
#define RCCTOKEN2(regid, bitloc)											TOKEN(regid, bitloc, RCC_LOCMASK, RCC_LOCMASK, RCC_LOCMASK)

#define MKRCCTOKEN(fn,...) MKRCCTOKEN_N(fn,##__VA_ARGS__,9,8,7,6,5,4,3,2,1,0)(__VA_ARGS__)
#define MKRCCTOKEN_N(fn,n0,n1,n2,n3,n4,n5,n6,n7,n8,n,...) fn##n
#define RCCTOKEN(...) MKRCCTOKEN(RCCTOKEN,##__VA_ARGS__)

#define REG_FEN_ID0		(u32*)&AON_BASE->AON_FUNC_CTRL1
#define REG_FEN_ID1		(u32*)&SYSON_BASE->PON_0_FUNC_CTRL
#define REG_FEN_ID2		(u32*)&SYSON_BASE->PON_1_FUNC_CTRL
#define REG_FEN_ID3		(u32*)&SYSON_BASE->PON_2_FUNC_CTRL
#define REG_FEN_ID4		(u32*)&CORE_S_BASE->SYS_1_FUNC_CTRL
#define REG_FEN_ID5		(u32*)&CORE_S_BASE->SYS_2_FUNC_CTRL
#define REG_FEN_ID6		(u32*)&WLAFE_BASE->WLAFE_ANAPAR_POW_MAC_0
#define REG_FEN_ID7		(u32*)&WLOFF_BASE->SYS_FUNC_CTRL

/* alternate name */
#define APBPeriph_WLON 							APBPeriph_WMAC
#define APBPeriph_WLAFE 						APBPeriph_RFAFE

#define APBPeriph_ATIM							APBPeriph_AON_TIMER



#define APBPeriph_WMAC  						RCCTOKEN(0, 20) //AON->REG_AON_FUNC_CTRL1&AON_BIT_AON_FEN_A0
#define APBPeriph_RTC       					RCCTOKEN(0, 19) //AON->REG_AON_FUNC_CTRL1&AON_BIT_AON_FEN_A2_0
#define APBPeriph_AON_GPIO  					RCCTOKEN(0, 18) //AON->REG_AON_FUNC_CTRL1&AON_BIT_AON_FEN_A2_1
#define APBPeriph_AON_TIMER 					RCCTOKEN(0, 17) //AON->REG_AON_FUNC_CTRL1&AON_BIT_AON_FEN_A2_2
#define APBPeriph_AON_LSSDM 					RCCTOKEN(0, 16) //AON->REG_AON_FUNC_CTRL1&AON_BIT_AON_FEN_A2_3
#define APBPeriph_AON_HSSDM 					RCCTOKEN(0, 15) //AON->REG_AON_FUNC_CTRL1&AON_BIT_AON_FEN_A2_4
#define APBPeriph_AON_SIC   					RCCTOKEN(0, 14) //AON->REG_AON_FUNC_CTRL1&AON_BIT_AON_FEN_A2_5
#define APBPeriph_AON_COMP  					RCCTOKEN(0, 13) //AON->REG_AON_FUNC_CTRL1&AON_BIT_AON_FEN_A2_6
#define APBPeriph_PON_REG  						RCCTOKEN(0, 11) //AON->REG_AON_FUNC_CTRL1&AON_BIT_AON_FEN_A1
#define APBPeriph_AON_SIC_BUS					RCCTOKEN(0, 10) //AON->REG_AON_FUNC_CTRL1&AON_BIT_AON_FEN_A1

#define APBPeriph_PLFM_CORE_SYS_CLK				RCCTOKEN(1, 10) //SYSON->PON_0_FUNC_CTRL&SYSON_BIT_CORE_PBUS_SYSMEM_CLK_FEN_P2_4
#define APBPeriph_MPLFM_CUOFF_SYS_CLK			RCCTOKEN(1,  9) //SYSON->PON_0_FUNC_CTRL&SYSON_BIT_MCUOFF_PBUS_SYSMEM_CLK_FEN_P2_4
#define APBPeriph_PLFM_MCUOFF_SLOW_PCLK			RCCTOKEN(1,  8) //SYSON->PON_0_FUNC_CTRL&SYSON_BIT_MCUOFF_PBUS_SLOWPCLK_FEN_P2_6
#define APBPeriph_MCUOFF_REG					RCCTOKEN(1,  5) //SYSON->PON_0_FUNC_CTRL&SYSON_BIT_PON_MCUOFF_REG_FEN_P11
#define APBPeriph_WIFIOFF_REG					RCCTOKEN(1,  4) //SYSON->PON_0_FUNC_CTRL&SYSON_BIT_PON_WLOFF_REG_FEN_P10
#define APBPeriph_BT    						RCCTOKEN(1,  3) //SYSON->PON_0_FUNC_CTRL&SYSON_BIT_PON_BT_FEN_P3
#define APBPeriph_SYSON_REG						RCCTOKEN(1,  2) //SYSON->PON_0_FUNC_CTRL&SYSON_BIT_PON_SYSON_REG_FEN_P4
#define APBPeriph_PLFM_MCUOFF_PSRAM_CLK			RCCTOKEN(1,  1) //SYSON->PON_0_FUNC_CTRL&SYSON_BIT_PON_PBUS_PSRAMC_FEN_P2_0
#define APBPeriph_PLFM_MCUOFF_SEC_CLK			RCCTOKEN(1,  0) //SYSON->PON_0_FUNC_CTRL&SYSON_BIT_PON_PBUS_SEC_FEN_P2_1

#define APBPeriph_KM4_NP_WARM  					RCCTOKEN(2, 31) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_P0
#define APBPeriph_KM4_NP_COLD					RCCTOKEN(2, 30) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_P1
#define APBPeriph_PLFM_WIFI						RCCTOKEN(2, 29) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_P2
#define APBPeriph_PLFM_SYSON_AON_CLK			RCCTOKEN(2, 28) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_P2_10
#define APBPeriph_PLFM_MCUOFF_KM4NP_CLK			RCCTOKEN(2, 25) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_P2_3
#define APBPeriph_PLFM_SYSON_SYS_CLK			RCCTOKEN(2, 24) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_P2_4
#define APBPeriph_PLFM_MCUOFF_TRNG_CLK			RCCTOKEN(2, 23) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_P2_5
#define APBPeriph_PLFM_SYSON_SLOW_PCLK			RCCTOKEN(2, 22) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_P2_6
#define APBPeriph_IPC    						RCCTOKEN(2, 18) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_P5
#define APBPeriph_WDT_NP   						RCCTOKEN(2, 17) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_P6
#define APBPeriph_PON_GPIO						RCCTOKEN(2, 15) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_P6_1
#define APBPeriph_PWM   						RCCTOKEN(2, 14) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_P6_2
#define APBPeriph_PON_TIMER0					RCCTOKEN(2, 13) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_P6_3
#define APBPeriph_PINMUX   						RCCTOKEN(2, 12) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_P6_4
#define APBPeriph_SYS_SRAM 						RCCTOKEN(2, 11) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_P6_5
#define APBPeriph_SH_SRAMM 						RCCTOKEN(2, 10) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_P6_6
#define APBPeriph_GDMA0   						RCCTOKEN(2,  9) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_P6_7
#define APBPeriph_KM0_FP_COLD  					RCCTOKEN(2,  8) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_S4
#define APBPeriph_KM0_FP_WARM					RCCTOKEN(2,  7) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_S5 
#define APBPeriph_WDT_FP   						RCCTOKEN(2,  6) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_S7
#define APBPeriph_SPIC_SPICPHY					RCCTOKEN(2,  5) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_S7_10
#define APBPeriph_ADC   						RCCTOKEN(2,  4) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_S7_11
#define APBPeriph_PSRAMC_PSRAMPHY				RCCTOKEN(2,  3) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_S7_12
#define APBPeriph_PON_TIMER3  					RCCTOKEN(2,  2) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_S7_13
#define APBPeriph_RSIP   						RCCTOKEN(2,  1) //SYSON->PON_1_FUNC_CTRL&SYSON_BIT_PON_FEN_S7_15

#define APBPeriph_I2C0    						RCCTOKEN(3, 31) //SYSON->PON_2_FUNC_CTRL&SYSON_BIT_PON_FEN_S7_18
#define APBPeriph_I2C1    						RCCTOKEN(3, 30) //SYSON->PON_2_FUNC_CTRL&SYSON_BIT_PON_FEN_S7_19
#define APBPeriph_I2C2    						RCCTOKEN(3, 29) //SYSON->PON_2_FUNC_CTRL&SYSON_BIT_PON_FEN_S7_20
#define APBPeriph_I2C3    						RCCTOKEN(3, 28) //SYSON->PON_2_FUNC_CTRL&SYSON_BIT_PON_FEN_S7_21
#define APBPeriph_UART1   						RCCTOKEN(3, 27) //SYSON->PON_2_FUNC_CTRL&SYSON_BIT_PON_FEN_S7_22
#define APBPeriph_BTUART 						RCCTOKEN(3, 26) //SYSON->PON_2_FUNC_CTRL&SYSON_BIT_PON_FEN_S7_23
#define APBPeriph_LOGUART 						RCCTOKEN(3, 25) //SYSON->PON_2_FUNC_CTRL&SYSON_BIT_PON_FEN_S7_24
#define APBPeriph_SPIM0							RCCTOKEN(3, 24) //SYSON->PON_2_FUNC_CTRL&SYSON_BIT_PON_FEN_S7_26	  
#define APBPeriph_SPIM1   						RCCTOKEN(3, 23) //SYSON->PON_2_FUNC_CTRL&SYSON_BIT_PON_FEN_S7_27
#define APBPeriph_SPIS0 						RCCTOKEN(3, 22) //SYSON->PON_2_FUNC_CTRL&SYSON_BIT_PON_FEN_S7_26	  
#define APBPeriph_SPIS1   						RCCTOKEN(3, 21) //SYSON->PON_2_FUNC_CTRL&SYSON_BIT_PON_FEN_S7_27
#define APBPeriph_PON_TIMER1					RCCTOKEN(3, 20) //SYSON->PON_2_FUNC_CTRL&SYSON_BIT_PON_FEN_S7_30
#define APBPeriph_PON_TIMER2					RCCTOKEN(3, 19) //SYSON->PON_2_FUNC_CTRL&SYSON_BIT_PON_FEN_S7_31
#define APBPeriph_TRNG   						RCCTOKEN(3, 18) //SYSON->PON_2_FUNC_CTRL&SYSON_BIT_TRNG_FEN_S7_40
#define APBPeriph_HMAC   						RCCTOKEN(3, 17) //SYSON->PON_2_FUNC_CTRL&SYSON_BIT_HMAC_FEN_S7_39
#define APBPeriph_PKE    						RCCTOKEN(3, 16) //SYSON->PON_2_FUNC_CTRL&SYSON_BIT_PKE_FEN_S7_38
#define APBPeriph_AES   						RCCTOKEN(3, 15) //SYSON->PON_2_FUNC_CTRL&SYSON_BIT_AES_FEN_S7_37


#define APBPeriph_KM4_MP_COLD   				RCCTOKEN(4, 29)
#define APBPeriph_KM4_MP_WARM   				RCCTOKEN(4, 28)
#define APBPeriph_PLFM_CORE_NN_ACLK 			RCCTOKEN(4, 21)
#define APBPeriph_PLFM_CORE_ENC_ACLK 			RCCTOKEN(4, 20)
#define APBPeriph_PLFM_CORE_ISPMBUS_CLK 		RCCTOKEN(4, 19)
#define APBPeriph_PLFM_CORE_MP_CLK 				RCCTOKEN(4, 18)
#define APBPeriph_PLFM_CORE_GE_CLK 				RCCTOKEN(4, 17)
#define APBPeriph_PLFM_CORE_DDRC_CLK 			RCCTOKEN(4, 16)
#define APBPeriph_PLFM_CORE_FAST_CLK 			RCCTOKEN(4, 15)
#define APBPeriph_PLFM_CORE_SEC_CLK 			RCCTOKEN(4, 14)
#define APBPeriph_WDT_CA32		 				RCCTOKEN(4, 13)
#define APBPeriph_NN			 				RCCTOKEN(4, 12)
#define APBPeriph_DDRPHY			 			RCCTOKEN(4, 11)
#define APBPeriph_VIDEO_SRAM			 		RCCTOKEN(4, 10)
#define APBPeriph_GDMA1			 				RCCTOKEN(4, 9)
#define APBPeriph_ENCODER			 			RCCTOKEN(4, 8)
#define APBPeriph_CORE_GPIO			 			RCCTOKEN(4, 6)
#define APBPeriph_ISP			 				RCCTOKEN(4, 5)
#define APBPeriph_SGPIO			 				RCCTOKEN(4, 4)
#define APBPeriph_SGPIO_REG			 			RCCTOKEN(4, 3)
#define APBPeriph_GMAC			 				RCCTOKEN(4, 2)
#define APBPeriph_SDMMC			 				RCCTOKEN(4, 1)
#define APBPeriph_SDIO			 				RCCTOKEN(4, 0)

#define APBPeriph_GE		 					RCCTOKEN(5, 31)
#define APBPeriph_SPORT0						RCCTOKEN(5, 30)
#define APBPeriph_SPORT1						RCCTOKEN(5, 29)
#define APBPeriph_USBOTG						RCCTOKEN(5, 28)
#define APBPeriph_AIDN							RCCTOKEN(5, 27)
#define APBPeriph_AC_MCLK						RCCTOKEN(5, 26)
#define APBPeriph_LX_BUS						RCCTOKEN(5, 25)
#define APBPeriph_WLOFF_DSS						RCCTOKEN(5, 24)
#define APBPeriph_CORE_DSS0						RCCTOKEN(5, 23)
#define APBPeriph_ISP_ADDR_REMAP				RCCTOKEN(5, 22)
#define APBPeriph_CORE_DSS1						RCCTOKEN(5, 21)
#define APBPeriph_CORE_DSS2						RCCTOKEN(5, 20)
#define APBPeriph_CORE_DSS3						RCCTOKEN(5, 19)
#define APBPeriph_CORE_DSS4						RCCTOKEN(5, 18)
#define APBPeriph_ENC_ADDR_REMAP				RCCTOKEN(5, 17)
#define APBPeriph_TILE_SCALER					RCCTOKEN(5, 16)
#define APBPeriph_JTAG_KEY_CTRL					RCCTOKEN(5, 14)
#define APBPeriph_UART2							RCCTOKEN(5, 12)
#define APBPeriph_UART0							RCCTOKEN(5, 11)
#define APBPeriph_WIFI_PLFM						RCCTOKEN(5, 2)
#define APBPeriph_AC_PCLK						RCCTOKEN(5, 1)
#define APBPeriph_WDT_MP  						RCCTOKEN(5, 0)

#define APBPeriph_RFAFE  						RCCTOKEN(6, 9, 8, 6, 5) //WLAFE->WLAFE_ANAPAR_POW_MAC_0&WLAFE_BIT_HW_EN_RFAFE/RFAFE_2REGU

#define APBPeriph_WLOFF_BUS_WMAC				RCCTOKEN(7, 4)
#define APBPeriph_WLOFF_BUS_SYSMEM				RCCTOKEN(7, 3)
#define APBPeriph_WLOFF_BUS_SLOW_PCLK			RCCTOKEN(7, 2)
#define APBPeriph_WLOFF_BUS_KM0_FW				RCCTOKEN(7, 1)
#define APBPeriph_WLOFF_BTUART					RCCTOKEN(7, 0)

/** @} */

/** @defgroup
 * @brief
 * @{
 */

#define REG_CLK_ID0		(u32*)&AON_BASE->AON_CLK_CTRL
#define REG_CLK_ID1		(u32*)&SYSON_BASE->PON_0_CLK_CTRL
#define REG_CLK_ID2		(u32*)&SYSON_BASE->PON_1_CLK_CTRL
#define REG_CLK_ID3		(u32*)&SYSON_BASE->PON_2_CLK_CTRL
#define REG_CLK_ID4		(u32*)&CORE_S_BASE->SYS_0_CLK_CTRL
#define REG_CLK_ID5		(u32*)&CORE_S_BASE->SYS_1_CLK_CTRL
#define REG_CLK_ID6		(u32*)&CORE_S_BASE->SYS_2_CLK_CTRL
#define REG_CLK_ID7		(u32*)&WLOFF_BASE->SYS_CLK_CTRL

#define APBPeriph_ATIM_CLOCK  APBPeriph_AONTIM_CLOCK

#define APBPeriph_REG_PON_CLOCK			RCCTOKEN(0, 22)
#define APBPeriph_AONWDT_CLOCK			RCCTOKEN(0, 20, 21)
#define APBPeriph_RTC_CLOCK		    	RCCTOKEN(0, 18, 19)
#define APBPeriph_AONGPIO_CLOCK			RCCTOKEN(0, 16, 17)
#define APBPeriph_AONTIM_CLOCK			RCCTOKEN(0, 14, 15)
#define APBPeriph_AONLSSDM_CLOCK		RCCTOKEN(0, 11, 12, 13)
#define APBPeriph_AONHSSDM_CLOCK		RCCTOKEN(0, 9, 10)
#define APBPeriph_AONSIC_CLOCK			RCCTOKEN(0, 7, 8)
#define APBPeriph_AONCOMP_CLOCK			RCCTOKEN(0, 5, 6)
#define APBPeriph_AONPMC_CLOCK			RCCTOKEN(0, 4)
#define APBPeriph_AONOTPC_CLOCK			RCCTOKEN(0, 2, 3)
#define APBPeriph_AONWLANA_CLOCK		RCCTOKEN(0, 1)
#define APBPeriph_AONWL32K_CLOCK		RCCTOKEN(0, 0)



#define APBPeriph_AONLSSDM_XTAL_CLOCK	RCCTOKEN(0, 10)
#define APBPeriph_AONLSSDM_OSC_CLOCK	RCCTOKEN(0, 12)
#define APBPeriph_AONLSSDM_PCLOCK       RCCTOKEN(0, 11)
#define APBPeriph_AONHSSDM_XTAL_CLOCK   RCCTOKEN(0, 10)
#define APBPeriph_AONHSSDM_PCLOCK       RCCTOKEN(0, 9)
#define APBPeriph_AONPMC_SCLOCK			RCCTOKEN(0, 4)


// ADC
#define APBPeriph_ADC_CLOCK 			RCCTOKEN(1, 25, 24)
// AUDIO_CODEC
#define APBPeriph_AC_CLOCK  			RCCTOKEN(1, 23)
// RSIP
#define APBPeriph_FLASHSEC_CLOCK		RCCTOKEN(1, 22)
// GDMA
#define APBPeriph_GDMA0_CLOCK 			RCCTOKEN(1, 21)
// I2C0 ~ I2C3
#define APBPeriph_I2C0_CLOCK 			RCCTOKEN(1, 20, 19)
#define APBPeriph_I2C1_CLOCK 			RCCTOKEN(1, 18, 17)
#define APBPeriph_I2C2_CLOCK 			RCCTOKEN(1, 16, 15)
#define APBPeriph_I2C3_CLOCK 			RCCTOKEN(1, 14, 13)
// IPC
#define APBPeriph_IPC_CLOCK 			RCCTOKEN(1, 12)
// FP
#define APBPeriph_KM0_FP_CLOCK 			RCCTOKEN(1, 11, 10)
#define APBPeriph_KM0_FP_PCLOCK 		RCCTOKEN(1, 11)
// NP
#define APBPeriph_KM4_NP_CLOCK 			RCCTOKEN(1, 9, 8)
// PBUS
#define APBPeriph_PBUS_ADC_CLOCK 		RCCTOKEN(1, 7)
#define APBPeriph_PBUS_AON_CLOCK 		RCCTOKEN(1, 6)
#define APBPeriph_PBUS_FP_CLOCK 		RCCTOKEN(1, 5)
#define APBPeriph_PBUS_NP_CLOCK 		RCCTOKEN(1, 4)
#define APBPeriph_PBUS_OTPC_CLOCK 		RCCTOKEN(1, 3)
#define APBPeriph_PBUS_PCLOCK 			RCCTOKEN(1, 2)
#define APBPeriph_PBUS_PSRAMC_CLOCK 	RCCTOKEN(1, 1)
// RSIP ?
#define APBPeriph_PBUS_SEC_CLOCK 		RCCTOKEN(1, 0)

#define APBPeriph_PBUS_SLOW_PCLOCK 		RCCTOKEN(2, 31)
//#define APBPeriph_PBUS_SPIC_CLOCK 	RCCTOKEN(2, 30)
#define APBPeriph_PBUS_SYSTEM_CLOCK		RCCTOKEN(2, 29)
#define APBPeriph_PBUS_TRNG_CLOCK 		RCCTOKEN(2, 28)
#define APBPeriph_PBUS_WMAC_CLOCK 		RCCTOKEN(2, 27)

#define APBPeriph_PINMUX_CLOCK 			RCCTOKEN(2, 26)
#define APBPeriph_PON_GPIO_CLOCK 		RCCTOKEN(2, 25, 24)
#define APBPeriph_PON_TIMER0_CLOCK 		RCCTOKEN(2, 23, 22)

//#define APBPeriph_REG_PON_CLOCK 		RCCTOKEN(2, 21)
//#define APBPeriph_UART0_CLOCK 		RCCTOKEN(2, 20, 19)
#define APBPeriph_PSRAMC_CLOCK 			RCCTOKEN(2, 18)
#define APBPeriph_PSRAMPHY_CLOCK 		RCCTOKEN(2, 17)
#define APBPeriph_PWM_CLOCK 			RCCTOKEN(2, 15, 16)
#define APBPeriph_SHRM_CLOCK 			RCCTOKEN(2, 14)
#define APBPeriph_SIC_HCLOCK 			RCCTOKEN(2, 13)
#define APBPeriph_SPIM0_CLOCK 			RCCTOKEN(2, 12, 11)
#define APBPeriph_SPIM1_CLOCK 			RCCTOKEN(2, 10, 9)
#define APBPeriph_SPIS0_CLOCK 			RCCTOKEN(2, 8, 7)
#define APBPeriph_SPIS1_CLOCK 			RCCTOKEN(2, 6, 5)
#define APBPeriph_SPIC_CLOCK 			RCCTOKEN(2, 4)
#define APBPeriph_SPICPHY_CLOCK 		RCCTOKEN(2, 3)
#define APBPeriph_SYS_SRAM_CLOCK 		RCCTOKEN(2, 2)
#define APBPeriph_PON_TIMER1_CLOCK 		RCCTOKEN(2, 1, 0)


#define APBPeriph_PON_TIMER2_CLOCK 		RCCTOKEN(3, 31, 30)
#define APBPeriph_PON_TIMER3_CLOCK 		RCCTOKEN(3, 29, 28)

#define APBPeriph_UART1_CLOCK 			RCCTOKEN(3, 27, 26)
#define APBPeriph_BTUART_CLOCK 			RCCTOKEN(3, 25, 24)	// UART2
#define APBPeriph_LOGUART_CLOCK 		RCCTOKEN(3, 23, 22)

#define APBPeriph_WDT_FP_CLOCK 			RCCTOKEN(3, 21, 20)
#define APBPeriph_WDT_NP_CLOCK 			RCCTOKEN(3, 19, 18)

#define APBPeriph_WMAC_CLOCK 			RCCTOKEN(3, 16)
#define APBPeriph_WIFIOFF_REG_CLOCK 	RCCTOKEN(3, 15)

#define APBPeriph_PKE_CLOCK 			RCCTOKEN(3, 14)
#define APBPeriph_HMAC_CLOCK 			RCCTOKEN(3, 13)
#define APBPeriph_AES_CLOCK 			RCCTOKEN(3, 12)
//#define APBPeriph_TRNG_CLOCK 			RCCTOKEN(3, 11)
#define APBPeriph_MCUOFF_REG_CLOCK 		RCCTOKEN(3, 10)

#define APBPeriph_ADRM_ENC_CLOCK			RCCTOKEN(4, 31)
#define APBPeriph_ADRM_ISP_CLOCK			RCCTOKEN(4, 30)
#define APBPeriph_AI_CLOCK			        RCCTOKEN(4, 29, 28, 27)
#define APBPeriph_AUDIO_CODEC_CLOCK			RCCTOKEN(4, 26)
#define APBPeriph_CBUS_DDRC_CLOCK			RCCTOKEN(4, 25)
#define APBPeriph_CBUS_ENC_CLOCK			RCCTOKEN(4, 24)
#define APBPeriph_CBUS_FAST_CLOCK			RCCTOKEN(4, 23)
#define APBPeriph_CBUS_GE_CLOCK				RCCTOKEN(4, 22)
#define APBPeriph_CBUS_ISP_BUS_CLOCK		RCCTOKEN(4, 21)
#define APBPeriph_CBUS_KM4_MP_CLOCK			RCCTOKEN(4, 20)
#define APBPeriph_CBUS_NN_CLOCK				RCCTOKEN(4, 19)
#define APBPeriph_CBUS_SEC_CLOCK			RCCTOKEN(4, 17)
#define APBPeriph_CBUS_SLOW_CLOCK			RCCTOKEN(4, 16)
#define APBPeriph_CBUS_SYS_MEM_CLOCK		RCCTOKEN(4, 15)
#define APBPeriph_CORE_DSS0_CLOCK			RCCTOKEN(4, 14)
#define APBPeriph_CORE_DSS1_CLOCK			RCCTOKEN(4, 13)
#define APBPeriph_CORE_DSS2_CLOCK			RCCTOKEN(4, 12)
#define APBPeriph_CORE_DSS3_CLOCK			RCCTOKEN(4, 11)
#define APBPeriph_CORE_DSS4_CLOCK			RCCTOKEN(4, 10)
#define APBPeriph_CORE_GPIO_CLOCK			RCCTOKEN(4, 9, 8)
#define APBPeriph_DDRC_CLOCK			    RCCTOKEN(4, 7)
#define APBPeriph_DDRPHY_CLOCK			    RCCTOKEN(4, 6)
#define APBPeriph_ENC_CLOCK			    	RCCTOKEN(4, 5, 4, 3)
#define APBPeriph_GMAC_CLOCK			    RCCTOKEN(4, 2, 1, 0)

#define	APBPeriph_ISP_MBUS_CLOCK       	    RCCTOKEN(5,  31)
#define	APBPeriph_ISP_MIPI_CLOCK        	RCCTOKEN(5,  30)
#define	APBPeriph_ISP_CLOCK             	RCCTOKEN(5,  29, 28)
#define	APBPeriph_KM4_MP_CLOCK          	RCCTOKEN(5,  27, 26)
#define	APBPeriph_LX_CLOCK              	RCCTOKEN(5,  25)
#define	APBPeriph_LXBUS_CLOCK           	RCCTOKEN(5,  24)
#define	APBPeriph_NN_CLOCK              	RCCTOKEN(5,  23, 22, 21)
#define	APBPeriph_PON_SYSONREG_CLOCK    	RCCTOKEN(5,  20)
#define	APBPeriph_PON_WLOFF_DSS_CLOCK   	RCCTOKEN(5,  19)
#define	APBPeriph_SD_CLOCK            		RCCTOKEN(5,  18, 17, 16)
#define	APBPeriph_SDIO_CLOCK            	RCCTOKEN(5,  15, 14)
#define	APBPeriph_SGPIO_CLOCK           	RCCTOKEN(5,  13, 12)
#define	APBPeriph_SPORT0_CLOCK          	RCCTOKEN(5,  11, 10)
#define	APBPeriph_SPORT1_CLOCK          	RCCTOKEN(5,  9, 8)
#define	APBPeriph_TC_CLOCK              	RCCTOKEN(5,  7)
#define	APBPeriph_USBOTG_CLOCK          	RCCTOKEN(5,  3, 4)
#define	APBPeriph_VID_SRAM_CLOCK        	RCCTOKEN(5,  2)
#define	APBPeriph_WDT_AP_CLOCK          	RCCTOKEN(5,  1, 0)

#define APBPeriph_WDT_MP_CLOCK				RCCTOKEN(6, 31, 30)
#define APBPeriph_GDMA1_CLOCK				RCCTOKEN(6, 28)
#define APBPeriph_GE_CLOCK            		RCCTOKEN(6, 27)
#define APBPeriph_IPSEC_CLOCK         		RCCTOKEN(6, 26)
//#define APBPeriph_WDT_AP1_CLOCK        		RCCTOKEN(6, 23)
#define APBPeriph_AUDIO_CODEC_PCLOCK   		RCCTOKEN(6, 22)
#define APBPeriph_UART2_CLOCK         		RCCTOKEN(6, 20, 19)
#define APBPeriph_UART0_CLOCK         		RCCTOKEN(6, 18, 17)

#define APBPeriph_WLOFF_BT_32K_CLOCK		RCCTOKEN(7, 8)
#define APBPeriph_WLOFF_BT_40M_CLOCK		RCCTOKEN(7, 7)
#define APBPeriph_WLOFF_BB_CLOCK			RCCTOKEN(7, 4)
#define APBPeriph_WLOFF_BTUART_CLOCK		RCCTOKEN(7, 3, 2)
#define APBPeriph_WLOFF_HCLOCK				RCCTOKEN(7, 1)
#define APBPeriph_WLOFF_REG_CLOCK			RCCTOKEN(7, 0)


// PRO3 clock source control

/*
 list of clock source

	 SYS1_PLL
	 SYS2_PLL
	 PERI_PLL
	 DDR_PLL
	 USB2_PLL
	 CPU_PLL
	 WIFI_PLL
	 XTAL40M
	 XTAL40M_DIV
	 XTAL32K
	 REGU100K
	 SDM32K
	 HSSDM
	 UTMICLK
	 OSC131K
	 OSC131K_DIV4
	 OSC4M
 */

/*
 list of clock div type
	NONE - 	no clock divider
	INT  - 	only DivN, ratio = DivN
	FLOAT - DivN and DivS, ratio = DivN/DivS
 */

#define RCC_DIV_NONE				0
#define RCC_DIV_INT					1
#define RCC_DIV_FLOAT				2

/*
 list of clock source

 */

// AON
#define AON_32KCLK_BASE					AON_BASE
#define AON_32KCLK_DIVTYPE				RCC_DIV_INT
#define AON_32KCLK_GFMUX				0
#define AON_32KCLK_SDM32K				0x0
#define AON_32KCLK_HSSDM				0x1
#define AON_32KCLK_XTAL40M_DIV			0x2
#define AON_32KCLK_XTAL32K				0x3
#define AON_32KCLK_OSC131K_DIV4			0x4
#define AON_32KCLK_OSC131K				0x5

#define AON_AONCLK_BASE					AON_BASE
#define AON_AONCLK_DIVTYPE				RCC_DIV_INT
#define AON_AONCLK_GFMUX				0
#define AON_AONCLK_OSC4M				0x0
#define AON_AONCLK_REGU100K				0x1
#define AON_AONCLK_SDM32K				0x2
#define AON_AONCLK_XTAL40M				0x3


// SYSON
#define PON_TRNG_CLK_BASE				SYSON_BASE
#define PON_TRNG_CLK_DIVTYPE			RCC_DIV_NONE
#define PON_TRNG_CLK_GFMUX				0
#define PON_TRNG_CLK_XTAL40M			0x0
#define PON_TRNG_CLK_OSC4M				0x1
#define PON_TRNG_CLK_OSC131K			0x2

#define PON_SEC_SCLK_BASE				SYSON_BASE
#define PON_SEC_SCLK_DIVTYPE			RCC_DIV_FLOAT
#define PON_SEC_SCLK_GFMUX				0
#define PON_SEC_SCLK_SYS1_PLL 			0x0
#define PON_SEC_SCLK_SYS2_PLL 			0x1
#define PON_SEC_SCLK_PERI_PLL 			0x2
#define PON_SEC_SCLK_DDR_PLL  			0x3
#define PON_SEC_SCLK_USB2_PLL 			0x4
#define PON_SEC_SCLK_XTAL40M 			0x5

#define PON_ADC_CLK_BASE				SYSON_BASE
#define PON_ADC_CLK_DIVTYPE				RCC_DIV_INT
#define PON_ADC_CLK_GFMUX				0
#define PON_ADC_CLK_XTAL40M				0x0
#define PON_ADC_CLK_OSC4M				0x1
#define PON_ADC_CLK_SDM32K				0x2
#define PON_ADC_CLK_OSC131K				0x3

#define PON_KM0_FW_CLK_BASE				SYSON_BASE
#define PON_KM0_FW_CLK_DIVTYPE			RCC_DIV_FLOAT
#define PON_KM0_FW_CLK_GFMUX			1
#define PON_KM0_FW_CLK_XTAL40M			0x0
#define PON_KM0_FW_CLK_OSC4M			0x1
#define PON_KM0_FW_CLK_OSC131K			0x2


#define PON_KM4_NP_CLK_BASE				SYSON_BASE
#define PON_KM4_NP_CLK_DIVTYPE			RCC_DIV_FLOAT
#define PON_KM4_NP_CLK_GFMUX			1
#define PON_KM4_NP_CLK_SYS1_PLL 		0x0
#define PON_KM4_NP_CLK_SYS2_PLL 		0x1
#define PON_KM4_NP_CLK_PERI_PLL 		0x2
#define PON_KM4_NP_CLK_DDR_PLL  		0x3
#define PON_KM4_NP_CLK_USB2_PLL 		0x4
#define PON_KM4_NP_CLK_CPU_PLL 			0x5
#define PON_KM4_NP_CLK_XTAL40M 			0x6
#define PON_KM4_NP_CLK_OSC4M 			0x7

#define PON_PSRAMC_CLK_BASE				SYSON_BASE
#define PON_PSRAMC_CLK_DIVTYPE			RCC_DIV_INT
#define PON_PSRAMC_CLK_GFMUX			0
#define PON_PSRAMC_CLK_SYS1_PLL 		0x0
#define PON_PSRAMC_CLK_SYS2_PLL 		0x1
#define PON_PSRAMC_CLK_PERI_PLL 		0x2
#define PON_PSRAMC_CLK_DDR_PLL  		0x3
#define PON_PSRAMC_CLK_USB2_PLL 		0x4
#define PON_PSRAMC_CLK_XTAL40M 			0x5

#define PON_PWM_CLK_BASE				SYSON_BASE
#define PON_PWM_CLK_DIVTYPE				RCC_DIV_INT
#define PON_PWM_CLK_GFMUX				0
#define PON_PWM_CLK_XTAL40M				0x0
#define PON_PWM_CLK_OSC4M				0x1
#define PON_PWM_CLK_SDM32K				0x2
#define PON_PWM_CLK_OSC131K				0x3

#define PON_SLOW_PCLK_BASE				SYSON_BASE
#define PON_SLOW_PCLK_DIVTYPE			RCC_DIV_FLOAT
#define PON_SLOW_PCLK_GFMUX				1
#define PON_SLOW_PCLK_SYS1_PLL 			0x0
#define PON_SLOW_PCLK_SYS2_PLL 			0x1
#define PON_SLOW_PCLK_PERI_PLL 			0x2
#define PON_SLOW_PCLK_DDR_PLL  			0x3
#define PON_SLOW_PCLK_USB2_PLL 			0x4
#define PON_SLOW_PCLK_XTAL40M 			0x5
#define PON_SLOW_PCLK_OSC4M 			0x6
#define PON_SLOW_PCLK_SDM32K 			0x7

#define PON_SLOW_SCLK_BASE				SYSON_BASE
#define PON_SLOW_SCLK_DIVTYPE			RCC_DIV_INT
#define PON_SLOW_SCLK_GFMUX				0
#define PON_SLOW_SCLK_SYS1_PLL 			0x0
#define PON_SLOW_SCLK_SYS2_PLL 			0x1
#define PON_SLOW_SCLK_PERI_PLL 			0x2
#define PON_SLOW_SCLK_DDR_PLL  			0x3
#define PON_SLOW_SCLK_USB2_PLL 			0x4
#define PON_SLOW_SCLK_XTAL40M 			0x5
#define PON_SLOW_SCLK_OSC4M 			0x6
#define PON_SLOW_SCLK_SDM32K 			0x7

#define PON_SPIC_CLK_BASE				SYSON_BASE
#define PON_SPIC_CLK_DIVTYPE			RCC_DIV_INT
#define PON_SPIC_CLK_GFMUX				0
#define PON_SPIC_CLK_SYS1_PLL 			0x0
#define PON_SPIC_CLK_SYS2_PLL 			0x1
#define PON_SPIC_CLK_PERI_PLL 			0x2
#define PON_SPIC_CLK_DDR_PLL  			0x3
#define PON_SPIC_CLK_USB2_PLL 			0x4
#define PON_SPIC_CLK_XTAL40M 			0x5

#define PON_TIMER0_CLK_BASE				SYSON_BASE
#define PON_TIMER0_CLK_DIVTYPE			RCC_DIV_INT
#define PON_TIMER0_CLK_GFMUX			0
#define PON_TIMER0_CLK_XTAL40M			0x0
#define PON_TIMER0_CLK_OSC4M			0x1
#define PON_TIMER0_CLK_SDM32K			0x2
#define PON_TIMER0_CLK_OSC131K			0x3

#define PON_TIMER123_CLK_BASE			SYSON_BASE
#define PON_TIMER123_CLK_DIVTYPE		RCC_DIV_INT
#define PON_TIMER123_CLK_GFMUX			0
#define PON_TIMER123_CLK_XTAL40M		0x0
#define PON_TIMER123_CLK_OSC4M			0x1
#define PON_TIMER123_CLK_SDM32K			0x2
#define PON_TIMER123_CLK_OSC131K		0x3
#define PON_TIMER123_CLK_PERI_PLL		0x4


// CORE_S

#define SYS_AI_SCLK_BASE				CORE_S_BASE
#define SYS_AI_SCLK_DIVTYPE				RCC_DIV_FLOAT
#define SYS_AI_SCLK_GFMUX				0
#define SYS_AI_SCLK_SYS1_PLL  			0x0
#define SYS_AI_SCLK_SYS2_PLL  			0x1
#define SYS_AI_SCLK_PERI_PLL  			0x2
#define SYS_AI_SCLK_DDR_PLL   			0x3
#define SYS_AI_SCLK_USB2_PLL  			0x4
#define SYS_AI_SCLK_CPU_PLL 			0x5

#define AUDIO_CODEC_CLK_BASE			CORE_S_BASE
#define AUDIO_CODEC_CLK_DIVTYPE			RCC_DIV_INT
#define AUDIO_CODEC_CLK_GFMUX			0
#define AUDIO_CODEC_CLK_SYS1_PLL 		0x0
#define AUDIO_CODEC_CLK_SYS2_PLL 		0x1
#define AUDIO_CODEC_CLK_PERI_PLL 		0x2
#define AUDIO_CODEC_CLK_DDR_PLL  		0x3
#define AUDIO_CODEC_CLK_USB2_PLL 		0x4
#define AUDIO_CODEC_CLK_XTAL40M 		0x5

#define SYS_ENC_ACLK_BASE				CORE_S_BASE
#define SYS_ENC_ACLK_DIVTYPE			RCC_DIV_FLOAT
#define SYS_ENC_ACLK_GFMUX				0
#define SYS_ENC_ACLK_SYS1_PLL  			0x0
#define SYS_ENC_ACLK_SYS2_PLL  			0x1
#define SYS_ENC_ACLK_PERI_PLL  			0x2
#define SYS_ENC_ACLK_DDR_PLL   			0x3
#define SYS_ENC_ACLK_USB2_PLL  			0x4
#define SYS_ENC_ACLK_CPU_PLL 			0x5

#define SYS_ENC_SCLK_BASE				CORE_S_BASE
#define SYS_ENC_SCLK_DIVTYPE			RCC_DIV_FLOAT
#define SYS_ENC_SCLK_GFMUX				0
#define SYS_ENC_SCLK_SYS1_PLL  			0x0
#define SYS_ENC_SCLK_SYS2_PLL  			0x1
#define SYS_ENC_SCLK_PERI_PLL  			0x2
#define SYS_ENC_SCLK_DDR_PLL   			0x3
#define SYS_ENC_SCLK_USB2_PLL  			0x4
#define SYS_ENC_SCLK_CPU_PLL 			0x5

#define SYS_FAST_PCLK_BASE				CORE_S_BASE
#define SYS_FAST_PCLK_DIVTYPE			RCC_DIV_FLOAT
#define SYS_FAST_PCLK_GFMUX				0
#define SYS_FAST_PCLK_SYS1_PLL 			0x0
#define SYS_FAST_PCLK_SYS2_PLL 			0x1
#define SYS_FAST_PCLK_PERI_PLL 			0x2
#define SYS_FAST_PCLK_DDR_PLL  			0x3
#define SYS_FAST_PCLK_USB2_PLL 			0x4
#define SYS_FAST_PCLK_XTAL40M 			0x5

#define SYS_GE_SCLK_BASE				CORE_S_BASE
#define SYS_GE_SCLK_DIVTYPE				RCC_DIV_FLOAT
#define SYS_GE_SCLK_GFMUX				0
#define SYS_GE_SCLK_SYS1_PLL  			0x0
#define SYS_GE_SCLK_SYS2_PLL  			0x1
#define SYS_GE_SCLK_PERI_PLL  			0x2
#define SYS_GE_SCLK_DDR_PLL   			0x3
#define SYS_GE_SCLK_USB2_PLL  			0x4
#define SYS_GE_SCLK_CPU_PLL 			0x5

#define SYS_GMAC_CLK_BASE				CORE_S_BASE
#define SYS_GMAC_CLK_DIVTYPE			RCC_DIV_INT
#define SYS_GMAC_CLK_GFMUX				0
#define SYS_GMAC_CLK_SYS1_PLL  			0x0
#define SYS_GMAC_CLK_SYS2_PLL  			0x1
#define SYS_GMAC_CLK_PERI_PLL  			0x2
#define SYS_GMAC_CLK_DDR_PLL   			0x3
#define SYS_GMAC_CLK_USB2_PLL  			0x4
#define SYS_GMAC_CLK_CPU_PLL 			0x5

#define SYS_ISP_MIPICLK_BASE			CORE_S_BASE
#define SYS_ISP_MIPICLK_DIVTYPE			RCC_DIV_INT
#define SYS_ISP_MIPICLK_GFMUX			0
#define SYS_ISP_MIPICLK_SYS1_PLL  		0x0
#define SYS_ISP_MIPICLK_SYS2_PLL  		0x1
#define SYS_ISP_MIPICLK_PERI_PLL  		0x2
#define SYS_ISP_MIPICLK_DDR_PLL   		0x3
#define SYS_ISP_MIPICLK_USB2_PLL  		0x4
#define SYS_ISP_MIPICLK_CPU_PLL 		0x5

#define SYS_ISP_BUSCLK_BASE				CORE_S_BASE
#define SYS_ISP_BUSCLK_DIVTYPE			RCC_DIV_FLOAT
#define SYS_ISP_BUSCLK_GFMUX			0
#define SYS_ISP_BUSCLK_SYS1_PLL  		0x0
#define SYS_ISP_BUSCLK_SYS2_PLL  		0x1
#define SYS_ISP_BUSCLK_PERI_PLL  		0x2
#define SYS_ISP_BUSCLK_DDR_PLL   		0x3
#define SYS_ISP_BUSCLK_USB2_PLL  		0x4
#define SYS_ISP_BUSCLK_CPU_PLL 			0x5

#define SYS_ISP_SCLK_BASE				CORE_S_BASE
#define SYS_ISP_SCLK_DIVTYPE			RCC_DIV_FLOAT
#define SYS_ISP_SCLK_GFMUX				0
#define SYS_ISP_SCLK_SYS1_PLL  			0x0
#define SYS_ISP_SCLK_SYS2_PLL  			0x1
#define SYS_ISP_SCLK_PERI_PLL  			0x2
#define SYS_ISP_SCLK_DDR_PLL   			0x3
#define SYS_ISP_SCLK_USB2_PLL  			0x4
#define SYS_ISP_SCLK_CPU_PLL 			0x5


#define SYS_KM4_MP_CLK_BASE				CORE_S_BASE
#define SYS_KM4_MP_CLK_DIVTYPE			RCC_DIV_FLOAT
#define SYS_KM4_MP_CLK_GFMUX			1
#define SYS_KM4_MP_CLK_SYS1_PLL 		0x0
#define SYS_KM4_MP_CLK_SYS2_PLL 		0x1
#define SYS_KM4_MP_CLK_PERI_PLL 		0x2
#define SYS_KM4_MP_CLK_DDR_PLL  		0x3
#define SYS_KM4_MP_CLK_USB2_PLL 		0x4
#define SYS_KM4_MP_CLK_CPU_PLL 			0x5
#define SYS_KM4_MP_CLK_XTAL40M 			0x6
#define SYS_KM4_MP_CLK_OSC4M 			0x7

#define SYS_NN_ACLK_BASE				CORE_S_BASE
#define SYS_NN_ACLK_DIVTYPE				RCC_DIV_FLOAT
#define SYS_NN_ACLK_GFMUX				0
#define SYS_NN_ACLK_SYS1_PLL  			0x0
#define SYS_NN_ACLK_SYS2_PLL  			0x1
#define SYS_NN_ACLK_PERI_PLL  			0x2
#define SYS_NN_ACLK_DDR_PLL   			0x3
#define SYS_NN_ACLK_USB2_PLL  			0x4
#define SYS_NN_ACLK_CPU_PLL 			0x5

#define SYS_NN_SCLK_BASE				CORE_S_BASE
#define SYS_NN_SCLK_DIVTYPE				RCC_DIV_FLOAT
#define SYS_NN_SCLK_GFMUX				0
#define SYS_NN_SCLK_SYS1_PLL  			0x0
#define SYS_NN_SCLK_SYS2_PLL  			0x1
#define SYS_NN_SCLK_PERI_PLL  			0x2
#define SYS_NN_SCLK_DDR_PLL   			0x3
#define SYS_NN_SCLK_USB2_PLL  			0x4
#define SYS_NN_SCLK_CPU_PLL 			0x5


#define SYS_PLF_CLK_BASE				CORE_S_BASE
#define SYS_PLF_CLK_DIVTYPE				RCC_DIV_FLOAT
#define SYS_PLF_CLK_GFMUX				1
#define SYS_PLF_CLK_SYS1_PLL 			0x0
#define SYS_PLF_CLK_SYS2_PLL 			0x1
#define SYS_PLF_CLK_PERI_PLL 			0x2
#define SYS_PLF_CLK_DDR_PLL  			0x3
#define SYS_PLF_CLK_USB2_PLL 			0x4
#define SYS_PLF_CLK_XTAL40M 			0x5
#define SYS_PLF_CLK_OSC4M 				0x6


#define SYS_SD_CLK_BASE					CORE_S_BASE
#define SYS_SD_CLK_DIVTYPE				RCC_DIV_INT
#define SYS_SD_CLK_GFMUX				0
#define SYS_SD_CLK_SYS1_PLL  			0x0
#define SYS_SD_CLK_SYS2_PLL  			0x1
#define SYS_SD_CLK_PERI_PLL  			0x2
#define SYS_SD_CLK_DDR_PLL   			0x3
#define SYS_SD_CLK_USB2_PLL  			0x4
#define SYS_SD_CLK_CPU_PLL 				0x5

#define SYS_SPORT_CLK_BASE				CORE_S_BASE
#define SYS_SPORT_CLK_DIVTYPE			RCC_DIV_FLOAT
#define SYS_SPORT_CLK_GFMUX				0
#define SYS_SPORT_CLK_SYS1_PLL  		0x0
#define SYS_SPORT_CLK_SYS2_PLL  		0x1
#define SYS_SPORT_CLK_PERI_PLL  		0x2
#define SYS_SPORT_CLK_DDR_PLL   		0x3
#define SYS_SPORT_CLK_USB2_PLL  		0x4
#define SYS_SPORT_CLK_XTAL40M 			0x5

#define SYS_SENSOR_CLK_BASE				CORE_S_BASE
#define SYS_SENSOR_CLK_DIVTYPE			RCC_DIV_INT
#define SYS_SENSOR_CLK_GFMUX			0
#define SYS_SENSOR_CLK_SYS1_PLL  		0x0
#define SYS_SENSOR_CLK_SYS2_PLL  		0x1
#define SYS_SENSOR_CLK_PERI_PLL  		0x2
#define SYS_SENSOR_CLK_DDR_PLL   		0x3
#define SYS_SENSOR_CLK_USB2_PLL  		0x4
#define SYS_SENSOR_CLK_XTAL40M 			0x5

#define SYS_USBOTG_CLK_BASE				CORE_S_BASE
#define SYS_USBOTG_CLK_DIVTYPE			RCC_DIV_INT
#define SYS_USBOTG_CLK_GFMUX			0


/* clock source control structure parser */

#define CLKSRC_BIT_XTAL_40M_SEL            ((u32)0x00000001 << 25)
#define CLKSRC_BIT_VLD                     ((u32)0x00000001 << 24)
#define CLKSRC_BIT_EN                      ((u32)0x00000001 << 19)
#define CLKSRC_MASK_SEL                    ((u32)0x00000007 << 16)
#define CLKSRC_SEL(x)                      ((u32)(((x) & 0x00000007) << 16))
#define CLKSRC_GET_SEL(x)                  ((u32)(((x >> 16) & 0x00000007)))
#define CLKSRC_BIT_DIVTRG                  ((u32)0x00000001 << 14)
#define CLKSRC_MASK_DIVS                   ((u32)0x0000003F << 8)
#define CLKSRC_DIVS(x)                     ((u32)(((x) & 0x0000003F) << 8))
#define CLKSRC_GET_DIVS(x)                 ((u32)(((x >> 8) & 0x0000003F)))
#define CLKSRC_MASK_DIVN                   ((u32)0x0000003F << 0)
#define CLKSRC_DIVN(x)                     ((u32)(((x) & 0x0000003F) << 0))
#define CLKSRC_GET_DIVN(x)                 ((u32)(((x >> 0) & 0x0000003F)))


/* clock source control macro */

/*
 some clock select form here
 REG_SYS_2_CLK_CTRL
[29]  SYS_CORE_GPIO_INTRCLK_SEL
[25]  SYS_AI_PSI_CLK_SEL
[21]  DDRC_CLK_SEL

*/

#define RCC_ClockSourceValue(Periph_Name, Clk_Source)	 Periph_Name##_##Clk_Source

/*
programming clock section flow
(1) set XTAL_40M_SEL = 1 (if has GFMUX)
(2) Disable clock.
(3) Poll clock valid = 0.
(4) Program clock select bits.
(5) Enable clock.
(6) set XTAL_40M_SEL = 0 (if has GFMUX)
*/

#define RCC_PeriphClockSourceSet(Periph_Name, Clk_Source) do {          			\
	u32 Temp = 0;																	\
    if( (u32)Periph_Name##_BASE != (u32)AON_BASE ){                                 \
        if( Periph_Name##_GFMUX ){													\
            Periph_Name##_BASE-> Periph_Name##_CTRL |= CLKSRC_BIT_XTAL_40M_SEL; 	\
        }																			\
        Periph_Name##_BASE->Periph_Name##_CTRL &= ~CLKSRC_BIT_EN;					\
        Temp = Periph_Name##_BASE->Periph_Name##_CTRL;								\
        while( (Temp & CLKSRC_BIT_VLD) != 0){ 										\
            Temp = Periph_Name##_BASE->Periph_Name##_CTRL;							\
        }																			\
    }else{                                                                          \
        Temp = Periph_Name##_BASE->Periph_Name##_CTRL;                              \
    }                                                                               \
	Temp &= ~CLKSRC_MASK_SEL;														\
	Temp |= CLKSRC_SEL(Periph_Name##_##Clk_Source);							        \
	Periph_Name##_BASE->Periph_Name##_CTRL = Temp;								    \
    if( (u32)Periph_Name##_BASE != (u32)AON_BASE ){                                 \
        Periph_Name##_BASE->Periph_Name##_CTRL |= CLKSRC_BIT_EN;					\
        if( Periph_Name##_GFMUX ){													\
            Periph_Name##_BASE->Periph_Name##_CTRL &= ~CLKSRC_BIT_XTAL_40M_SEL; 	\
        }																			\
    }                                                                               \
} while(0)

#define RCC_PeriphClockSourceGet(Periph_Name) \
	CLKSRC_GET_SEL(Periph_Name##_BASE->Periph_Name##_CTRL)


//#define RCC_ASSERT(e) typedef char __DIVS_IS_NOT_ZERO__##e[(e)?1:-1]
#define RCC_DIVS_ASSERT1(type, divs) 		_Static_assert( (type != RCC_DIV_FLOAT) || (divs != 0) , "DivS must not be zero when DivType is float" )
#define RCC_DIVS_ASSERT2(type, divs) 		_Static_assert( (type != RCC_DIV_INT) || (divs == 0), "DivS must be zero when DivType is int" )
#define RCC_DIVS_ASSERT3(type, divn, divs) 	_Static_assert( (type != RCC_DIV_FLOAT) || (divn >= divs) , "DivN must larger or equal than DivS when DivType is float" )

/*
clock divider setting flow
(1) Poll clock valid = 1.
(2) Program clock divider factor(s).
(3) Write 1 to clock divider trigger bit (DIVTRG).
*/

#define RCC_PeriphClockDividerSet(Periph_Name, DivN, DivS) do {			\
	RCC_DIVS_ASSERT1( Periph_Name##_DIVTYPE, DivS );					\
	RCC_DIVS_ASSERT2( Periph_Name##_DIVTYPE, DivS );					\
	RCC_DIVS_ASSERT3( Periph_Name##_DIVTYPE, DivN, DivS );				\
	if(Periph_Name##_DIVTYPE != RCC_DIV_NONE){							\
		u32 Temp = Periph_Name##_BASE->Periph_Name##_CTRL;				\
		while( (Temp & CLKSRC_BIT_VLD) ==0 ){ 							\
			Temp = Periph_Name##_BASE->Periph_Name##_CTRL;				\
		}																\
		Temp &= ~CLKSRC_MASK_DIVN;										\
		Temp |= CLKSRC_DIVN(DivN);										\
		if(Periph_Name##_DIVTYPE == RCC_DIV_FLOAT){						\
				Temp &= ~CLKSRC_MASK_DIVS;								\
				Temp |= CLKSRC_DIVS(DivS);								\
		}																\
		Periph_Name##_BASE->Periph_Name##_CTRL = Temp;					\
		Periph_Name##_BASE->Periph_Name##_CTRL |= CLKSRC_BIT_DIVTRG;	\
		__asm volatile ("nop");											\
	}																	\
}while(0)

#define RCC_PeriphClockDividerGetDivN(Periph_Name) \
	CLKSRC_GET_DIVN(Periph_Name##_BASE->Periph_Name##_CTRL)

#define RCC_PeriphClockDividerGetDivS(Periph_Name) \
	( (CLKSRC_GET_DIVS(Periph_Name##_BASE->Periph_Name##_CTRL)!= 0) ? CLKSRC_GET_DIVS(Periph_Name##_BASE->Periph_Name##_CTRL) : 1 )

#define RCC_PeriphClockDividerGet(Periph_Name) \
	(RCC_PeriphClockDividerGetDivN(Periph_Name) / RCC_PeriphClockDividerGetDivS(Periph_Name))




/** @} */

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif


/** @} */

/** @} */
