/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_AON_PAD_H
#define SYSREG_AON_PAD_H

/* AUTO_GEN_START */

/* AON_PAD is a part of AON */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup AON_PAD
* @brief AON_PAD driver modules
* @{
*/


/** @defgroup REG_AON_GPIOA_0_1_CTRL
 * @brief
 * @{
 */
#define AON_PAD_BIT_AON_GPIO1_SLEW_RATE             ((u32)0x00000001 << 28)                  /*!< R/W     0 0:Disable,1:Enable */
#define AON_PAD_MASK_AON_GPIO1_DRIVING              ((u32)0x00000003 << 26)                  /*!< R/W     0 0:4mA, 1:8mA */
#define AON_PAD_AON_GPIO1_DRIVING(x)                ((u32)(((x) & 0x00000003) << 26))
#define AON_PAD_GET_AON_GPIO1_DRIVING(x)            ((u32)(((x >> 26) & 0x00000003)))
#define AON_PAD_BIT_AON_GPIO1_SMT_EN                ((u32)0x00000001 << 25)                  /*!< R/W     0 Enable AON GPIO1 Schmitt trigger;  1: enable */
#define AON_PAD_MASK_AON_GPIO1_PULL_CTRL_1_0        ((u32)0x00000003 << 22)                  /*!< R/W     0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define AON_PAD_AON_GPIO1_PULL_CTRL_1_0(x)          ((u32)(((x) & 0x00000003) << 22))
#define AON_PAD_GET_AON_GPIO1_PULL_CTRL_1_0(x)      ((u32)(((x >> 22) & 0x00000003)))
#define AON_PAD_MASK_AON_GPIO1_PINMUX_SEL           ((u32)0x0000000F << 16)                  /*!< R/W   0xF
                                                                                              * 0000: Comparator_ADC 0001:
                                                                                              * 0010:                               0011:
                                                                                              * 0100:                              0101: 
                                                                                              * 0110: PWM1                   0111: AON_PWM
                                                                                              * 1000: UART0_IN           1001: 
                                                                                              * 1010: SWD_DAT           1011: OSC_4M
                                                                                              * 1100:                              1101: 
                                                                                              * 1110:                              1111: GPIO
                                                                                              */
#define AON_PAD_AON_GPIO1_PINMUX_SEL(x)             ((u32)(((x) & 0x0000000F) << 16))
#define AON_PAD_GET_AON_GPIO1_PINMUX_SEL(x)         ((u32)(((x >> 16) & 0x0000000F)))
#define AON_PAD_BIT_AON_GPIO0_SLEW_RATE             ((u32)0x00000001 << 12)                  /*!< R/W     0 0:Disable,1:Enable */
#define AON_PAD_MASK_AON_GPIO0_DRIVING              ((u32)0x00000003 << 10)                  /*!< R/W     0 0:4mA, 1:8mA */
#define AON_PAD_AON_GPIO0_DRIVING(x)                ((u32)(((x) & 0x00000003) << 10))
#define AON_PAD_GET_AON_GPIO0_DRIVING(x)            ((u32)(((x >> 10) & 0x00000003)))
#define AON_PAD_BIT_AON_GPIO0_SMT_EN                ((u32)0x00000001 << 9)                   /*!< R/W     0 Enable AON GPIO0 Schmitt trigger;  1: enable */
#define AON_PAD_MASK_AON_GPIO0_PULL_CTRL_1_0        ((u32)0x00000003 << 6)                   /*!< R/W   0   2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define AON_PAD_AON_GPIO0_PULL_CTRL_1_0(x)          ((u32)(((x) & 0x00000003) << 6))
#define AON_PAD_GET_AON_GPIO0_PULL_CTRL_1_0(x)      ((u32)(((x >> 6) & 0x00000003)))
#define AON_PAD_MASK_AON_GPIO0_PINMUX_SEL           ((u32)0x0000000F << 0)                   /*!< R/W   0xF
                                                                                              * 0000: Comparator_ADC 0001:  
                                                                                              * 0010:                               0011: 
                                                                                              * 0100:                              0101: 
                                                                                              * 0110: PWM0                   0111: AON_PWM
                                                                                              * 1000: UART0_OUT        1001: 
                                                                                              * 1010: WD_CLK              1011: OSC_131K
                                                                                              * 1100:                              1101: 
                                                                                              * 1110:                              1111: GPIO
                                                                                              */
#define AON_PAD_AON_GPIO0_PINMUX_SEL(x)             ((u32)(((x) & 0x0000000F) << 0))
#define AON_PAD_GET_AON_GPIO0_PINMUX_SEL(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_AON_GPIOA_2_3_CTRL
 * @brief
 * @{
 */
#define AON_PAD_BIT_AON_GPIO3_SLEW_RATE             ((u32)0x00000001 << 28)                  /*!< R/W     0 0:Disable,1:Enable */
#define AON_PAD_MASK_AON_GPIO3_DRIVING              ((u32)0x00000003 << 26)                  /*!< R/W     0 0:4mA, 1:8mA */
#define AON_PAD_AON_GPIO3_DRIVING(x)                ((u32)(((x) & 0x00000003) << 26))
#define AON_PAD_GET_AON_GPIO3_DRIVING(x)            ((u32)(((x >> 26) & 0x00000003)))
#define AON_PAD_BIT_AON_GPIO3_SMT_EN                ((u32)0x00000001 << 25)                  /*!< R/W     0 Enable AON GPIO3 Schmitt trigger;  1: enable */
#define AON_PAD_MASK_AON_GPIO3_PULL_CTRL_1_0        ((u32)0x00000003 << 22)                  /*!< R/W   0   2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define AON_PAD_AON_GPIO3_PULL_CTRL_1_0(x)          ((u32)(((x) & 0x00000003) << 22))
#define AON_PAD_GET_AON_GPIO3_PULL_CTRL_1_0(x)      ((u32)(((x >> 22) & 0x00000003)))
#define AON_PAD_MASK_AON_GPIO3_PINMUX_SEL           ((u32)0x0000000F << 16)                  /*!< R/W   0xF
                                                                                              * 0000:Comparator_ADC  0001:  
                                                                                              * 0010:                               0011: I2C2_SDA
                                                                                              * 0100: SIC_SDA               0101: 
                                                                                              * 0110: UART0_IN             0111: AON_PWM
                                                                                              * 1000:                               1001: LOG_UART_IN
                                                                                              * 1010:                               1011: REGU_100K
                                                                                              * 1100:                               1101: 
                                                                                              * 1110:                               1111: GPIO
                                                                                              */
#define AON_PAD_AON_GPIO3_PINMUX_SEL(x)             ((u32)(((x) & 0x0000000F) << 16))
#define AON_PAD_GET_AON_GPIO3_PINMUX_SEL(x)         ((u32)(((x >> 16) & 0x0000000F)))
#define AON_PAD_BIT_AON_GPIO2_SLEW_RATE             ((u32)0x00000001 << 12)                  /*!< R/W     0 0:Disable,1:Enable */
#define AON_PAD_MASK_AON_GPIO2_DRIVING              ((u32)0x00000003 << 10)                  /*!< R/W     0 0:4mA, 1:8mA */
#define AON_PAD_AON_GPIO2_DRIVING(x)                ((u32)(((x) & 0x00000003) << 10))
#define AON_PAD_GET_AON_GPIO2_DRIVING(x)            ((u32)(((x >> 10) & 0x00000003)))
#define AON_PAD_BIT_AON_GPIO2_SMT_EN                ((u32)0x00000001 << 9)                   /*!< R/W     0 Enable AON GPIO2 Schmitt trigger;  1: enable */
#define AON_PAD_MASK_AON_GPIO2_PULL_CTRL_1_0        ((u32)0x00000003 << 6)                   /*!< R/W   0   2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define AON_PAD_AON_GPIO2_PULL_CTRL_1_0(x)          ((u32)(((x) & 0x00000003) << 6))
#define AON_PAD_GET_AON_GPIO2_PULL_CTRL_1_0(x)      ((u32)(((x >> 6) & 0x00000003)))
#define AON_PAD_MASK_AON_GPIO2_PINMUX_SEL           ((u32)0x0000000F << 0)                   /*!< R/W   0xF
                                                                                              * 0000: Comparator_ADC  0001:  
                                                                                              * 0010:                                0011: I2C2_SCL
                                                                                              * 0100: SIC_SCL                0101: 
                                                                                              * 0110: UART0_OUT            0111: AON_PWM
                                                                                              * 1000:                                1001: LOG_UART_OUT
                                                                                              * 1010:                                1011: SDM_32K
                                                                                              * 1100:                                1101: 
                                                                                              * 1110:                                1111: GPIO
                                                                                              */
#define AON_PAD_AON_GPIO2_PINMUX_SEL(x)             ((u32)(((x) & 0x0000000F) << 0))
#define AON_PAD_GET_AON_GPIO2_PINMUX_SEL(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_AON_GPIOA_4_5_CTRL
 * @brief
 * @{
 */
#define AON_PAD_BIT_AON_GPIO5_SLEW_RATE             ((u32)0x00000001 << 28)                  /*!< R/W     0 0:Disable,1:Enable */
#define AON_PAD_MASK_AON_GPIO5_DRIVING              ((u32)0x00000003 << 26)                  /*!< R/W     0 0:4mA, 1:8mA */
#define AON_PAD_AON_GPIO5_DRIVING(x)                ((u32)(((x) & 0x00000003) << 26))
#define AON_PAD_GET_AON_GPIO5_DRIVING(x)            ((u32)(((x >> 26) & 0x00000003)))
#define AON_PAD_BIT_AON_GPIO5_SMT_EN                ((u32)0x00000001 << 25)                  /*!< R/W     0 Enable AON GPIO5 Schmitt trigger;  1: enable */
#define AON_PAD_MASK_AON_GPIO5_PULL_CTRL_1_0        ((u32)0x00000003 << 22)                  /*!< R/W     1 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define AON_PAD_AON_GPIO5_PULL_CTRL_1_0(x)          ((u32)(((x) & 0x00000003) << 22))
#define AON_PAD_GET_AON_GPIO5_PULL_CTRL_1_0(x)      ((u32)(((x >> 22) & 0x00000003)))
#define AON_PAD_MASK_AON_GPIO5_PINMUX_SEL           ((u32)0x0000000F << 16)                  /*!< R/W   0xF
                                                                                              * 0000:VP_LOW  0001:
                                                                                              * 0010:                  0011: 
                                                                                              * 0100:                  0101: 
                                                                                              * 0110:                  0111: 
                                                                                              * 1000:                  1001: 
                                                                                              * 1010:                  1011: 
                                                                                              * 1100:                  1101: 
                                                                                              * 1110:                  1111: GPIO
                                                                                              */
#define AON_PAD_AON_GPIO5_PINMUX_SEL(x)             ((u32)(((x) & 0x0000000F) << 16))
#define AON_PAD_GET_AON_GPIO5_PINMUX_SEL(x)         ((u32)(((x >> 16) & 0x0000000F)))
#define AON_PAD_BIT_AON_GPIO4_SLEW_RATE             ((u32)0x00000001 << 12)                  /*!< R/W     0 0:Disable,1:Enable */
#define AON_PAD_MASK_AON_GPIO4_DRIVING              ((u32)0x00000003 << 10)                  /*!< R/W     0 0:4mA, 1:8mA */
#define AON_PAD_AON_GPIO4_DRIVING(x)                ((u32)(((x) & 0x00000003) << 10))
#define AON_PAD_GET_AON_GPIO4_DRIVING(x)            ((u32)(((x >> 10) & 0x00000003)))
#define AON_PAD_BIT_AON_GPIO4_SMT_EN                ((u32)0x00000001 << 9)                   /*!< R/W     0 Enable AON GPIO4 Schmitt trigger;  1: enable */
#define AON_PAD_MASK_AON_GPIO4_PULL_CTRL_1_0        ((u32)0x00000003 << 6)                   /*!< R/W   1   2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define AON_PAD_AON_GPIO4_PULL_CTRL_1_0(x)          ((u32)(((x) & 0x00000003) << 6))
#define AON_PAD_GET_AON_GPIO4_PULL_CTRL_1_0(x)      ((u32)(((x >> 6) & 0x00000003)))
#define AON_PAD_MASK_AON_GPIO4_PINMUX_SEL           ((u32)0x0000000F << 0)                   /*!< R/W   0xF
                                                                                              * 0000:VP_HIGH   0001: 
                                                                                              * 0010:                  0011: 
                                                                                              * 0100:                  0101: 
                                                                                              * 0110:                  0111: 
                                                                                              * 1000:                  1001: 
                                                                                              * 1010:                  1011: 
                                                                                              * 1100:                  1101: 
                                                                                              * 1110:                  1111: GPIO
                                                                                              */
#define AON_PAD_AON_GPIO4_PINMUX_SEL(x)             ((u32)(((x) & 0x0000000F) << 0))
#define AON_PAD_GET_AON_GPIO4_PINMUX_SEL(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PWRPAD3N_SHDN_CTRL
 * @brief
 * @{
 */
#define AON_PAD_BIT_PON_GPIOP_PWD33                 ((u32)0x00000001 << 7)                   /*!< R/W   0   0: power down GPIOP */
#define AON_PAD_BIT_SYSON_GPIOS_PWD33               ((u32)0x00000001 << 6)                   /*!< R/W   0   0: power down GPIOS */
#define AON_PAD_BIT_PON_GPIOF_PWD33                 ((u32)0x00000001 << 5)                   /*!< R/W   0   0: power down GPIOF */
#define AON_PAD_BIT_SYSON_GPIOE_PWD33               ((u32)0x00000001 << 4)                   /*!< R/W   0   0: power down GPIOE */
#define AON_PAD_BIT_SYSON_GPIOD_PWD33               ((u32)0x00000001 << 3)                   /*!< R/W   0   0: power down GPIOD */
#define AON_PAD_BIT_SYSON_GPIOC_PWD33               ((u32)0x00000001 << 2)                   /*!< R/W   0   0: power down GPIOC */
#define AON_PAD_BIT_SYSON_GPIOB_PWD33               ((u32)0x00000001 << 1)                   /*!< R/W   0   0: power down GPIOB */
#define AON_PAD_BIT_AON_GPIOA_PWD33                 ((u32)0x00000001 << 0)                   /*!< R/W   1   0: power down GPIOA */
/** @} */


/** @defgroup REG_PWRPAD1N_SHDN_CTRL
 * @brief
 * @{
 */
#define AON_PAD_BIT_PON_GPIOP_PWD09                 ((u32)0x00000001 << 7)                   /*!< R/W   0   0: power down GPIOP */
#define AON_PAD_BIT_DUMMY1                          ((u32)0x00000001 << 6)                   /*!< R/W   1   None */
#define AON_PAD_BIT_PON_GPIOF_PWD09                 ((u32)0x00000001 << 5)                   /*!< R/W   1   0: power down GPIOF */
#define AON_PAD_BIT_DUMMY4                          ((u32)0x00000001 << 4)                   /*!< R/W   1   None */
#define AON_PAD_BIT_DUMMY3                          ((u32)0x00000001 << 3)                   /*!< R/W   1   None */
#define AON_PAD_BIT_DUMMY2                          ((u32)0x00000001 << 2)                   /*!< R/W   1   None */
#define AON_PAD_BIT_PON_GPIOB_PWD09                 ((u32)0x00000001 << 1)                   /*!< R/W   1   0: power down GPIOB */
#define AON_PAD_BIT_DUMMY0                          ((u32)0x00000001 << 0)                   /*!< R/W   1   None */
/** @} */




/** @defgroup AON_PAD_Exported_Types AON_PAD Exported Types
* @{
*/
/**
* @brief AON_PAD Register Declaration
*/

typedef struct {
	__IO uint32_t AON_GPIOA_0_1_CTRL;                           /*!< Register,  Address offset:0x0000 */
	__IO uint32_t AON_GPIOA_2_3_CTRL;                           /*!< Register,  Address offset:0x0004 */
	__IO uint32_t AON_GPIOA_4_5_CTRL;                           /*!< Register,  Address offset:0x0008 */
	__IO uint32_t PWRPAD3N_SHDN_CTRL;                           /*!< Register,  Address offset:0x000C */
	__IO uint32_t PWRPAD1N_SHDN_CTRL;                           /*!< Register,  Address offset:0x0010 */
} AON_PAD_TypeDef;

#define AON_PAD_BASE ((AON_PAD_TypeDef *) (0x4022A000))
/** @} */


/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here


#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

/** @} */

/** @} */
#endif  // SYSREG_AON_PAD_H
