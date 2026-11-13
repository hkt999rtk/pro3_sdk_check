/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_PINMUX_H_
#define _AMEBA_PINMUX_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  *		-Every GPIO pin can be set to some function based on pinmux spec.
  *		-Every GPIO pin can set internal pull-up, pull-down based on pinmux spec.
  *
  *====================================================================
  * Pad Control Spec.
  *====================================================================
  *		-[31:18]		reserved
  *		-[17]			pull down resistor enable when system is in sleep
  *		-[16]			pull up resistor enable when system is in sleep
  *		-[13]			srew rate control
  *		-[12]			schmitt trigger enable
  *		-[11]			pad driving strength
  *		-[10]			pull resistor selection
  *		-[9]			pull down resistor enable when system is in active
  *		-[8]			pull up resistor enable when system is in active
  *		-[7:5]			reserved for function id extend
  *		-[4:0]			function id
  *
  *****************************************************************************************
  * How to use Pinmux
  *****************************************************************************************
  *		1. Set the Internal pad function type for  each pin using the follwoing function:
  *			Pinmux_Config(u8 PinName, u32 PinFunc)
  *
  *		2. Set the Internal pad pull type for each pin using the follwoing function:
  *			PAD_PullCtrl(u8 PinName, u8 PullType)
  *			PAD_SleepPullCtrl(u8 PinName, u8 PullType);
  *
  *****************************************************************************************
  * @endverbatim
  */

/** @defgroup PIN
  * @brief PIN driver modules
  * @{
  */


/** @defgroup PINMUX
  * @brief PINMUX driver modules
  * @{
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup PINMUX_Exported_Constants PINMUX Exported Constants
  * @{
  */

/** @defgroup PINMUX_Pin_Name_definitions
  * @note: Pin_Name = (((port)<<5)|(pin))
  * @{
  */
#define _PA_0		(0x00)
#define _PA_1		(0x01)
#define _PA_2		(0x02)
#define _PA_3		(0x03)
#define _PA_4		(0x04)
#define _PA_5		(0x05)

#define _PB_0		(0x20)
#define _PB_1		(0x21)
#define _PB_2		(0x22)
#define _PB_3		(0x23)
#define _PB_4		(0x24)

#define _PC_0		(0x40)
#define _PC_1		(0x41)
#define _PC_2		(0x42)
#define _PC_3		(0x43)
#define _PC_4		(0x44)
#define _PC_5		(0x45)
#define _PC_6		(0x46)
#define _PC_7		(0x47)
#define _PC_8		(0x48)
#define _PC_9		(0x49)
#define _PC_10		(0x4A)

#define _PD_0		(0x60)
#define _PD_1		(0x61)
#define _PD_2		(0x62)
#define _PD_3		(0x63)
#define _PD_4		(0x64)
#define _PD_5		(0x65)
#define _PD_6		(0x66)
#define _PD_7		(0x67)
#define _PD_8		(0x68)
#define _PD_9		(0x69)

#define _PE_0		(0x80)
#define _PE_1		(0x81)
#define _PE_2		(0x82)
#define _PE_3		(0x83)
#define _PE_4		(0x84)
#define _PE_5		(0x85)
#define _PE_6		(0x86)
#define _PE_7		(0x87)
#define _PE_8		(0x88)
#define _PE_9		(0x89)
#define _PE_10		(0x8A)
#define _PE_11		(0x8B)
#define _PE_12		(0x8C)
#define _PE_13		(0x8D)
#define _PE_14		(0x8E)
#define _PE_15		(0x8F)
#define _PE_16		(0x90)

#define _PF_0		(0xA0)
#define _PF_1		(0xA1)
#define _PF_2		(0xA2)
#define _PF_3		(0xA3)
#define _PF_4		(0xA4)
#define _PF_5		(0xA5)
#define _PF_6		(0xA6)
#define _PF_7		(0xA7)
#define _PF_8		(0xA8)
#define _PF_9		(0xA9)
#define _PF_10		(0xAA)
#define _PF_11		(0xAB)
#define _PF_12		(0xAC)
#define _PF_13		(0xAD)
#define _PF_14		(0xAE)
#define _PF_15		(0xAF)

#define _PS_0		(0xC0)
#define _PS_1		(0xC1)
#define _PS_2		(0xC2)
#define _PS_3		(0xC3)
#define _PS_4		(0xC4)
#define _PS_5		(0xC5)
#define _PS_6		(0xC6)

#define _PP_0		(0xE0)
#define _PP_1		(0xE1)
#define _PP_2		(0xE2)
#define _PP_3		(0xE3)
#define _PP_4		(0xE4)
#define _PP_5		(0xE5)
#define _PP_6		(0xE6)
#define _PP_7		(0xE7)
#define _PP_8		(0xE8)
#define _PP_9		(0xE9)
#define _PP_10		(0xEA)
#define _PP_11		(0xEB)

#define _PNC		(0xFFFF)
/** @} */

/** @defgroup PINMUX_Port_definitions
  * @note: Pin_Name = (((port)<<5)|(pin))
  * @{
  */
#define	_PORT_A    0
#define	_PORT_B    1
#define	_PORT_C    2
#define	_PORT_D    3
#define	_PORT_E    4
#define	_PORT_F    5
#define	_PORT_S    6
#define	_PORT_P    7
#define	_PORT_MAX_NUM    8
/** @} */

/** @defgroup PINMUX_Port_and_Pin_definitions
  * @{
  */
#define PORT_NUM(pin)		((pin>>5) & 0x07)
#define PIN_NUM(pin)		(pin & 0x1f)
/** @} */

/** @defgroup PINMUX_Function_definitions
  * @{
  */
#define PINMUX_FUNCTION_GPIO                               15
#define PINMUX_FUNCTION_LOG_UART                           9
#define PINMUX_FUNCTION_SPIC_FLASH                         0
#define PINMUX_FUNCTION_PSRAM                              0
#define PINMUX_FUNCTION_ADC                                0
#define PINMUX_FUNCTION_MIPI                               0
#define PINMUX_FUNCTION_RMII                               7
#define PINMUX_FUNCTION_SPIM0                              1
#define PINMUX_FUNCTION_SPIM1                              1
#define PINMUX_FUNCTION_SPIS0                              2
#define PINMUX_FUNCTION_SPIS1                              2
#define PINMUX_FUNCTION_SWD                                10
#define PINMUX_FUNCTION_SDIO_MST                           0
#define PINMUX_FUNCTION_SDIO_SLV                           0
#define PINMUX_FUNCTION_I2S0                               5
#define PINMUX_FUNCTION_I2S1                               5
#define PINMUX_FUNCTION_DMIC                               4
#define PINMUX_FUNCTION_I2C0                               3
#define PINMUX_FUNCTION_I2C1                               3
#define PINMUX_FUNCTION_I2C2                               3
#define PINMUX_FUNCTION_I2C3                               0
#define PINMUX_FUNCTION_UART0                              8
#define PINMUX_FUNCTION_UART1                              8
#define PINMUX_FUNCTION_UART2                              8
#define PINMUX_FUNCTION_UART3                              8
#define PINMUX_FUNCTION_PWM                                6
#define PINMUX_FUNCTION_AON_PWM                            7


#define PINMUX_FUNCTION_SENSOR_CLK                         0
#define PINMUX_FUNCTION_SD                                 0
#define PINMUX_FUNCTION_MIPI                               0
#define PINMUX_FUNCTION_WIFI_TRX                           0
#define PINMUX_FUNCTION_BT_TRX                             0
#define PINMUX_FUNCTION_SGPIO                              0 // SGPIO in Function Group 0
#define PINMUX_FUNCTION_SIC                                4
#define PINMUX_FUNCTION_PDM                                4
#define PINMUX_FUNCTION_TPIU                               5
#define PINMUX_FUNCTION_SGPIO_FUNC_GRP5                    5 // SGPIO in Function Group 5
#define PINMUX_FUNCTION_AC_I2S                             7
#define PINMUX_FUNCTION_GMAC                               7
#define PINMUX_FUNCTION_COEX                               11
#define PINMUX_FUNCTION_BT_DEBUG                           12
#define PINMUX_FUNCTION_BT_IO                              13
#define PINMUX_FUNCTION_RF_CTRL                            14
#define PINMUX_FUNCTION_UART1_FLOW_CONTROL                 9


// for I2C examples
#define PINMUX_FUNCTION_I2C    PINMUX_FUNCTION_I2C0


// for UART examples
#define	PINMUX_FUNCTION_UART0_TXD PINMUX_FUNCTION_UART0
#define	PINMUX_FUNCTION_UART1_TXD PINMUX_FUNCTION_UART1
#define	PINMUX_FUNCTION_UART2_TXD PINMUX_FUNCTION_UART2
#define	PINMUX_FUNCTION_UART3_TXD PINMUX_FUNCTION_UART3

#define PINMUX_FUNCTION_UART0_RXD PINMUX_FUNCTION_UART0
#define PINMUX_FUNCTION_UART1_RXD PINMUX_FUNCTION_UART1
#define PINMUX_FUNCTION_UART2_RXD PINMUX_FUNCTION_UART2
#define PINMUX_FUNCTION_UART3_RXD PINMUX_FUNCTION_UART3

#define PINMUX_FUNCTION_UART0_CTS NULL
#define PINMUX_FUNCTION_UART3_CTS PINMUX_FUNCTION_UART3

#define PINMUX_FUNCTION_UART0_RTS NULL
#define PINMUX_FUNCTION_UART3_RTS PINMUX_FUNCTION_UART3


/** @} */

/** @defgroup PINMUX_Peripheral_Location_definitions
  * @note just used by function PINMUX_Ctrl
  * @{
  */
#define PINMUX_S0		(0)
#define PINMUX_S1		(1)
#define PINMUX_S2		(2)
#define PINMUX_S3		(3)
#define PINMUX_S4		(4)
#define PINMUX_S5		(5)
/** @} */

/** @} */

/** @defgroup PINMUX_Exported_Functions PINMUX Exported Functions
  * @{
  */
_LONG_CALL_ void Pinmux_Config(u8 PinName, u32 PinFunc);
_LONG_CALL_ u32 Pinmux_ConfigGet(u8 PinName);
_LONG_CALL_ void Pinmux_UartLogCtrl(u32  PinLocation, BOOL   Operation);
_LONG_CALL_ void Pinmux_BootSpicCtrl(u32  PinLocation, BOOL Operation);
_LONG_CALL_ void Pinmux_ComboSpicCtrl(u32  PinLocation, BOOL Operation);
_LONG_CALL_ void Pinmux_Swdoff(void);
/** @} */

/** @} */


/** @defgroup PAD
  * @brief PAD driver modules
  * @{
  */

/** @defgroup PAD_Exported_Constants PAD Exported Constants
  * @{
  */

/** @defgroup PAD_Pull_Resistor_definitions
  * @{
  */
#define PAD_Resistor_LARGE		0x00 /*!< PAD Resistor LARGE */
#define PAD_Resistor_SMALL		0x01 /*!< PAD Resistor SMALL */
/** @} */


/** @defgroup PINMUX_PAD_DrvStrength_definitions
  * @{
  */
#define PAD_DRV_ABILITITY_LOW			(0)
#define PAD_DRV_ABILITITY_HIGH			(1)
/** @} */

/** @defgroup PINMUX_PAD_SlewRate_definitions
  * @{
  */
#define PAD_SlewRate_Fast				(0)
#define PAD_SlewRate_Slow			    	(1)
/** @} */

/** @} */

/** @defgroup PAD_Exported_Functions PAD Exported Functions
  * @{
  */
_LONG_CALL_ void PAD_CMD(u8 PinName, u8 NewStatus);
_LONG_CALL_ void PAD_DrvStrength(u8 PinName, u32 DrvStrength);
_LONG_CALL_ void PAD_SchmitCtrl(u8 PinName, u32 NewState);
_LONG_CALL_ void PAD_SlewRateCtrl(u8 PinName, u32 NewState);
_LONG_CALL_ void PAD_PullCtrl(u8 PinName, u8 PullType);
_LONG_CALL_ void PAD_SleepPullCtrl(u8 PinName, u8 PullType);
_LONG_CALL_ void PAD_SpicCtrl(u32  PinLocation);
_LONG_CALL_ void PAD_ResistorCtrl(u8 PinName, u8 RType);
_LONG_CALL_ void PAD_InputCtrl(u8 PinName, u32 NewState);
/** @} */

/** @} */

/** @} */

/** @} */



/* PINMUX_TESTID_TRAP_ICFG_definitions */
#define TESTID_SIC 								1		// SIC mode
#define TESTID_DELAY5							3       // dealy 8s
#define TESTID_FORCE_USE_S0_ICFG				4       // force boot source select from TEST_MODE SOURCE
#define TESTID_FORCE_HIGH_SPEED_BOOT_ICFG		5       // force high speed boot 
#define TESTID_FORCE_LOW_SPEED_BOOT_ICFG		7       // force low speed boot


#endif   //_AMEBA_PINMUX_H_
