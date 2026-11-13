/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_PON_PAD_H
#define SYSREG_PON_PAD_H

/* AUTO_GEN_START */

/* PON_PAD is a part of PON */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup PON_PAD
* @brief PON_PAD driver modules
* @{
*/


/** @defgroup REG_PON_GPIOF_0_1_CTRL
 * @brief
 * @{
 */
#define PON_PAD_BIT_PON_GPIO1_SLEW_RATE             ((u32)0x00000001 << 28)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO1_DRIVING              ((u32)0x00000003 << 26)                  /*!< R/W   0x0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO1_DRIVING(x)                ((u32)(((x) & 0x00000003) << 26))
#define PON_PAD_GET_PON_GPIO1_DRIVING(x)            ((u32)(((x >> 26) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO1_SMT_EN                ((u32)0x00000001 << 25)                  /*!< R/W     0 Enable PON GPIO1 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO1_PULL_CTRL_1_0        ((u32)0x00000003 << 22)                  /*!< R/W   0x0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO1_PULL_CTRL_1_0(x)          ((u32)(((x) & 0x00000003) << 22))
#define PON_PAD_GET_PON_GPIO1_PULL_CTRL_1_0(x)      ((u32)(((x >> 22) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO1_PINMUX_SEL           ((u32)0x0000000F << 16)                  /*!< R/W   0xF
                                                                                              * 0000: ADC1        0001: I2C1_SCL
                                                                                              * 0010:                  0011: RFE_CTRL_0
                                                                                              * 0100:                  0101: 
                                                                                              * 0110:UART1_CTS 0111: 
                                                                                              * 1000:                  1001: 
                                                                                              * 1010:                  1011: 
                                                                                              * 1100:                  1101: 
                                                                                              * 1110:                  1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO1_PINMUX_SEL(x)             ((u32)(((x) & 0x0000000F) << 16))
#define PON_PAD_GET_PON_GPIO1_PINMUX_SEL(x)         ((u32)(((x >> 16) & 0x0000000F)))
#define PON_PAD_BIT_PON_GPIO0_SLEW_RATE             ((u32)0x00000001 << 12)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO0_DRIVING              ((u32)0x00000003 << 10)                  /*!< R/W     0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO0_DRIVING(x)                ((u32)(((x) & 0x00000003) << 10))
#define PON_PAD_GET_PON_GPIO0_DRIVING(x)            ((u32)(((x >> 10) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO0_SMT_EN                ((u32)0x00000001 << 9)                   /*!< R/W     0 Enable PON GPIO0 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO0_PULL_CTRL_1_0        ((u32)0x00000003 << 6)                   /*!< R/W   0   2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO0_PULL_CTRL_1_0(x)          ((u32)(((x) & 0x00000003) << 6))
#define PON_PAD_GET_PON_GPIO0_PULL_CTRL_1_0(x)      ((u32)(((x >> 6) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO0_PINMUX_SEL           ((u32)0x0000000F << 0)                   /*!< R/W   0xf
                                                                                              * 0000: ADC0    0001: 
                                                                                              * 0010:                  0011: 
                                                                                              * 0100:                  0101: 
                                                                                              * 0110:                  0111: 
                                                                                              * 1000:                  1001: 
                                                                                              * 1010:                  1011: 
                                                                                              * 1100:                  1101: 
                                                                                              * 1110:                  1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO0_PINMUX_SEL(x)             ((u32)(((x) & 0x0000000F) << 0))
#define PON_PAD_GET_PON_GPIO0_PINMUX_SEL(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PON_GPIOF_2_3_CTRL
 * @brief
 * @{
 */
#define PON_PAD_BIT_PON_GPIO3_SLEW_RATE             ((u32)0x00000001 << 28)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO3_DRIVING              ((u32)0x00000003 << 26)                  /*!< R/W   0x0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO3_DRIVING(x)                ((u32)(((x) & 0x00000003) << 26))
#define PON_PAD_GET_PON_GPIO3_DRIVING(x)            ((u32)(((x >> 26) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO3_SMT_EN                ((u32)0x00000001 << 25)                  /*!< R/W     0 Enable PON GPIO3 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO3_PULL_CTRL_1_0        ((u32)0x00000003 << 22)                  /*!< R/W   0x0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO3_PULL_CTRL_1_0(x)          ((u32)(((x) & 0x00000003) << 22))
#define PON_PAD_GET_PON_GPIO3_PULL_CTRL_1_0(x)      ((u32)(((x >> 22) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO3_PINMUX_SEL           ((u32)0x0000000F << 16)                  /*!< R/W   0xf
                                                                                              * 0000: ADC3       0001: 
                                                                                              * 0010:                 0011: RFE_CTRL_2
                                                                                              * 0100: SDIO_D2  0101:
                                                                                              * 0110:UART1_IN 0111: 
                                                                                              * 1000:                 1001: 
                                                                                              * 1010:                  1011: 
                                                                                              * 1100:                  1101: 
                                                                                              * 1110:                  1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO3_PINMUX_SEL(x)             ((u32)(((x) & 0x0000000F) << 16))
#define PON_PAD_GET_PON_GPIO3_PINMUX_SEL(x)         ((u32)(((x >> 16) & 0x0000000F)))
#define PON_PAD_BIT_PON_GPIO2_SLEW_RATE             ((u32)0x00000001 << 12)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO2_DRIVING              ((u32)0x00000003 << 10)                  /*!< R/W   0x0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO2_DRIVING(x)                ((u32)(((x) & 0x00000003) << 10))
#define PON_PAD_GET_PON_GPIO2_DRIVING(x)            ((u32)(((x >> 10) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO2_SMT_EN                ((u32)0x00000001 << 9)                   /*!< R/W     0 Enable PON GPIO2 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO2_PULL_CTRL_1_0        ((u32)0x00000003 << 6)                   /*!< R/W   0x0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO2_PULL_CTRL_1_0(x)          ((u32)(((x) & 0x00000003) << 6))
#define PON_PAD_GET_PON_GPIO2_PULL_CTRL_1_0(x)      ((u32)(((x >> 6) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO2_PINMUX_SEL           ((u32)0x0000000F << 0)                   /*!< R/W   0xf
                                                                                              * 0000: ADC2      0001: I2C1_SDA
                                                                                              * 0010:                0011: RFE_CTRL_1
                                                                                              * 0100:                 0101:
                                                                                              * 0110:UART1_RTS 0111: 
                                                                                              * 1000:                 1001: 
                                                                                              * 1010:                  1011: 
                                                                                              * 1100:                  1101: 
                                                                                              * 1110:                  1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO2_PINMUX_SEL(x)             ((u32)(((x) & 0x0000000F) << 0))
#define PON_PAD_GET_PON_GPIO2_PINMUX_SEL(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PON_GPIOF_4_5_CTRL
 * @brief
 * @{
 */
#define PON_PAD_BIT_PON_GPIO5_SLEW_RATE             ((u32)0x00000001 << 28)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO5_DRIVING              ((u32)0x00000003 << 26)                  /*!< R/W   0x0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO5_DRIVING(x)                ((u32)(((x) & 0x00000003) << 26))
#define PON_PAD_GET_PON_GPIO5_DRIVING(x)            ((u32)(((x >> 26) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO5_SMT_EN                ((u32)0x00000001 << 25)                  /*!< R/W     0 Enable PON GPIO5 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO5_PULL_CTRL_1_0        ((u32)0x00000003 << 22)                  /*!< R/W   0x0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO5_PULL_CTRL_1_0(x)          ((u32)(((x) & 0x00000003) << 22))
#define PON_PAD_GET_PON_GPIO5_PULL_CTRL_1_0(x)      ((u32)(((x >> 22) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO5_PINMUX_SEL           ((u32)0x0000000F << 16)                  /*!< R/W   0xf
                                                                                              * 0000:                 0001: 
                                                                                              * 0010:                 0011: 
                                                                                              * 0100:SDIO_D0   0101:SPI_1_MISO
                                                                                              * 0110:TPIU_DATA_0 0111: 
                                                                                              * 1000: SPI_3_MISO  1001: 
                                                                                              * 1010:                  1011: 
                                                                                              * 1100:                  1101: 
                                                                                              * 1110:                  1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO5_PINMUX_SEL(x)             ((u32)(((x) & 0x0000000F) << 16))
#define PON_PAD_GET_PON_GPIO5_PINMUX_SEL(x)         ((u32)(((x >> 16) & 0x0000000F)))
#define PON_PAD_BIT_PON_GPIO4_SLEW_RATE             ((u32)0x00000001 << 12)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO4_DRIVING              ((u32)0x00000003 << 10)                  /*!< R/W   0x0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO4_DRIVING(x)                ((u32)(((x) & 0x00000003) << 10))
#define PON_PAD_GET_PON_GPIO4_DRIVING(x)            ((u32)(((x >> 10) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO4_SMT_EN                ((u32)0x00000001 << 9)                   /*!< R/W     0 Enable PON GPIO4 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO4_PULL_CTRL_1_0        ((u32)0x00000003 << 6)                   /*!< R/W   0x0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO4_PULL_CTRL_1_0(x)          ((u32)(((x) & 0x00000003) << 6))
#define PON_PAD_GET_PON_GPIO4_PULL_CTRL_1_0(x)      ((u32)(((x >> 6) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO4_PINMUX_SEL           ((u32)0x0000000F << 0)                   /*!< R/W   0xf
                                                                                              * 0000:                0001: 
                                                                                              * 0010:                 0011: 
                                                                                              * 0100: SDIO_D1   0101:
                                                                                              * 0110:UART1_OUT 0111: 
                                                                                              * 1000:                 1001: 
                                                                                              * 1010:                  1011: 
                                                                                              * 1100:                  1101: 
                                                                                              * 1110:                  1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO4_PINMUX_SEL(x)             ((u32)(((x) & 0x0000000F) << 0))
#define PON_PAD_GET_PON_GPIO4_PINMUX_SEL(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PON_GPIOF_6_7_CTRL
 * @brief
 * @{
 */
#define PON_PAD_BIT_PON_GPIO7_SLEW_RATE             ((u32)0x00000001 << 28)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO7_DRIVING              ((u32)0x00000003 << 26)                  /*!< R/W   0x0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO7_DRIVING(x)                ((u32)(((x) & 0x00000003) << 26))
#define PON_PAD_GET_PON_GPIO7_DRIVING(x)            ((u32)(((x >> 26) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO7_SMT_EN                ((u32)0x00000001 << 25)                  /*!< R/W     0 Enable PON GPIO7 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO7_PULL_CTRL_1_0        ((u32)0x00000003 << 22)                  /*!< R/W   0x0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO7_PULL_CTRL_1_0(x)          ((u32)(((x) & 0x00000003) << 22))
#define PON_PAD_GET_PON_GPIO7_PULL_CTRL_1_0(x)      ((u32)(((x >> 22) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO7_PINMUX_SEL           ((u32)0x0000000F << 16)                  /*!< R/W   0xf
                                                                                              * 0000:                            0001: 
                                                                                              * 0010: PWM1                 0011: JTAG_TDI
                                                                                              * 0100:SDIO_CMD          0101:SPI_1_MOSI
                                                                                              * 0110:TPIU_DATA_2     0111: BT_JTAG_TDI
                                                                                              * 1000: SPI_3_MOSI        1001: 
                                                                                              * 1010:                             1011: 
                                                                                              * 1100:                             1101: 
                                                                                              * 1110:                             1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO7_PINMUX_SEL(x)             ((u32)(((x) & 0x0000000F) << 16))
#define PON_PAD_GET_PON_GPIO7_PINMUX_SEL(x)         ((u32)(((x >> 16) & 0x0000000F)))
#define PON_PAD_BIT_PON_GPIO6_SLEW_RATE             ((u32)0x00000001 << 12)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO6_DRIVING              ((u32)0x00000003 << 10)                  /*!< R/W   0x0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO6_DRIVING(x)                ((u32)(((x) & 0x00000003) << 10))
#define PON_PAD_GET_PON_GPIO6_DRIVING(x)            ((u32)(((x >> 10) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO6_SMT_EN                ((u32)0x00000001 << 9)                   /*!< R/W     0 Enable PON GPIO6 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO6_PULL_CTRL_1_0        ((u32)0x00000003 << 6)                   /*!< R/W   0x0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO6_PULL_CTRL_1_0(x)          ((u32)(((x) & 0x00000003) << 6))
#define PON_PAD_GET_PON_GPIO6_PULL_CTRL_1_0(x)      ((u32)(((x >> 6) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO6_PINMUX_SEL           ((u32)0x0000000F << 0)                   /*!< R/W   0xf
                                                                                              * 0000:                            0001: 
                                                                                              * 0010: PWM0                  0011: JTAG_TDO
                                                                                              * 0100:SDIO_CLK             0101:SPI_1_SCL
                                                                                              * 0110:TPIU_DATA_1        0111: BT_JTAG_TDO
                                                                                              * 1000: SPI_3_SCL             1001: 
                                                                                              * 1010:                             1011: 
                                                                                              * 1100:                             1101: 
                                                                                              * 1110:                             1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO6_PINMUX_SEL(x)             ((u32)(((x) & 0x0000000F) << 0))
#define PON_PAD_GET_PON_GPIO6_PINMUX_SEL(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PON_GPIOF_8_9_CTRL
 * @brief
 * @{
 */
#define PON_PAD_BIT_PON_GPIO9_SLEW_RATE             ((u32)0x00000001 << 28)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO9_DRIVING              ((u32)0x00000003 << 26)                  /*!< R/W   0x0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO9_DRIVING(x)                ((u32)(((x) & 0x00000003) << 26))
#define PON_PAD_GET_PON_GPIO9_DRIVING(x)            ((u32)(((x >> 26) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO9_SMT_EN                ((u32)0x00000001 << 25)                  /*!< R/W     0 Enable PON GPIO9 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO9_PULL_CTRL_1_0        ((u32)0x00000003 << 22)                  /*!< R/W   0x0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO9_PULL_CTRL_1_0(x)          ((u32)(((x) & 0x00000003) << 22))
#define PON_PAD_GET_PON_GPIO9_PULL_CTRL_1_0(x)      ((u32)(((x >> 22) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO9_PINMUX_SEL           ((u32)0x0000000F << 16)                  /*!< R/W   0xf
                                                                                              * 0000:                            0001: SGPIO
                                                                                              * 0010: PWM3                  0011: JTAG_CLK/SWD_CK
                                                                                              * 0100:                           0101:SPI_1_CS1
                                                                                              * 0110:TPIU_CLK            0111: BT_JTAG_CLK/SWD_CK
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                             1011: 
                                                                                              * 1100:                             1101: 
                                                                                              * 1110:                             1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO9_PINMUX_SEL(x)             ((u32)(((x) & 0x0000000F) << 16))
#define PON_PAD_GET_PON_GPIO9_PINMUX_SEL(x)         ((u32)(((x >> 16) & 0x0000000F)))
#define PON_PAD_BIT_PON_GPIO8_SLEW_RATE             ((u32)0x00000001 << 12)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO8_DRIVING              ((u32)0x00000003 << 10)                  /*!< R/W   0x0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO8_DRIVING(x)                ((u32)(((x) & 0x00000003) << 10))
#define PON_PAD_GET_PON_GPIO8_DRIVING(x)            ((u32)(((x >> 10) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO8_SMT_EN                ((u32)0x00000001 << 9)                   /*!< R/W     0 Enable PON GPIO8 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO8_PULL_CTRL_1_0        ((u32)0x00000003 << 6)                   /*!< R/W   0x0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO8_PULL_CTRL_1_0(x)          ((u32)(((x) & 0x00000003) << 6))
#define PON_PAD_GET_PON_GPIO8_PULL_CTRL_1_0(x)      ((u32)(((x >> 6) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO8_PINMUX_SEL           ((u32)0x0000000F << 0)                   /*!< R/W   0xf
                                                                                              * 0000:                            0001: 
                                                                                              * 0010: PWM2               0011: 
                                                                                              * 0100:SDIO_D3           0101:SPI_1_CS0
                                                                                              * 0110:TPIU_DATA_3       0111: 
                                                                                              * 1000: SPI_3_CS          1001: 
                                                                                              * 1010:                             1011: 
                                                                                              * 1100:                             1101: 
                                                                                              * 1110:                             1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO8_PINMUX_SEL(x)             ((u32)(((x) & 0x0000000F) << 0))
#define PON_PAD_GET_PON_GPIO8_PINMUX_SEL(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PON_GPIOF_10_11_CTRL
 * @brief
 * @{
 */
#define PON_PAD_BIT_PON_GPIO11_SLEW_RATE            ((u32)0x00000001 << 28)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO11_DRIVING             ((u32)0x00000003 << 26)                  /*!< R/W   0x0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO11_DRIVING(x)               ((u32)(((x) & 0x00000003) << 26))
#define PON_PAD_GET_PON_GPIO11_DRIVING(x)           ((u32)(((x >> 26) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO11_SMT_EN               ((u32)0x00000001 << 25)                  /*!< R/W     0 Enable PON GPIO11 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO11_PULL_CTRL_1_0       ((u32)0x00000003 << 22)                  /*!< R/W   0x0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO11_PULL_CTRL_1_0(x)         ((u32)(((x) & 0x00000003) << 22))
#define PON_PAD_GET_PON_GPIO11_PULL_CTRL_1_0(x)     ((u32)(((x >> 22) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO11_PINMUX_SEL          ((u32)0x0000000F << 16)                  /*!< R/W   0xf
                                                                                              * 0000:                            0001: 
                                                                                              * 0010: PWM5               0011: JTAG_TRST
                                                                                              * 0100:                          0101:I2S0_MCK
                                                                                              * 0110:                         0111: BT_JTAG_TRST
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                             1011: 
                                                                                              * 1100:                             1101: 
                                                                                              * 1110:                             1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO11_PINMUX_SEL(x)            ((u32)(((x) & 0x0000000F) << 16))
#define PON_PAD_GET_PON_GPIO11_PINMUX_SEL(x)        ((u32)(((x >> 16) & 0x0000000F)))
#define PON_PAD_BIT_PON_GPIO10_SLEW_RATE            ((u32)0x00000001 << 12)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO10_DRIVING             ((u32)0x00000003 << 10)                  /*!< R/W   0x0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO10_DRIVING(x)               ((u32)(((x) & 0x00000003) << 10))
#define PON_PAD_GET_PON_GPIO10_DRIVING(x)           ((u32)(((x >> 10) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO10_SMT_EN               ((u32)0x00000001 << 9)                   /*!< R/W     0 Enable PON GPIO10 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO10_PULL_CTRL_1_0       ((u32)0x00000003 << 6)                   /*!< R/W   0x0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO10_PULL_CTRL_1_0(x)         ((u32)(((x) & 0x00000003) << 6))
#define PON_PAD_GET_PON_GPIO10_PULL_CTRL_1_0(x)     ((u32)(((x >> 6) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO10_PINMUX_SEL          ((u32)0x0000000F << 0)                   /*!< R/W   0xf
                                                                                              * 0000:                            0001: 
                                                                                              * 0010: PWM4               0011: JTAG_TMS/SWD_IO
                                                                                              * 0100:                          0101:SPI_1_CS2
                                                                                              * 0110: TPIU_CTRL         0111: BT_JTAG_TMS/SWD_IO
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                             1011: 
                                                                                              * 1100:                             1101: 
                                                                                              * 1110:                             1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO10_PINMUX_SEL(x)            ((u32)(((x) & 0x0000000F) << 0))
#define PON_PAD_GET_PON_GPIO10_PINMUX_SEL(x)        ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PON_GPIOF_12_13_CTRL
 * @brief
 * @{
 */
#define PON_PAD_BIT_PON_GPIO13_SLEW_RATE            ((u32)0x00000001 << 28)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO13_DRIVING             ((u32)0x00000003 << 26)                  /*!< R/W   0x0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO13_DRIVING(x)               ((u32)(((x) & 0x00000003) << 26))
#define PON_PAD_GET_PON_GPIO13_DRIVING(x)           ((u32)(((x >> 26) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO13_SMT_EN               ((u32)0x00000001 << 25)                  /*!< R/W     0 Enable PON GPIO13 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO13_PULL_CTRL_1_0       ((u32)0x00000003 << 22)                  /*!< R/W   0x0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO13_PULL_CTRL_1_0(x)         ((u32)(((x) & 0x00000003) << 22))
#define PON_PAD_GET_PON_GPIO13_PULL_CTRL_1_0(x)     ((u32)(((x >> 22) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO13_PINMUX_SEL          ((u32)0x0000000F << 16)                  /*!< R/W   0xf
                                                                                              * 0000:                            0001: 
                                                                                              * 0010: PWM7               0011:
                                                                                              * 0100:                          0101: I2S0_CLK
                                                                                              * 0110:UART1_OUT        0111: 
                                                                                              * 1000:VOE_JTAG_TMS/SWD_IO 1001: 
                                                                                              * 1010:                             1011: 
                                                                                              * 1100:                             1101: 
                                                                                              * 1110:                             1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO13_PINMUX_SEL(x)            ((u32)(((x) & 0x0000000F) << 16))
#define PON_PAD_GET_PON_GPIO13_PINMUX_SEL(x)        ((u32)(((x >> 16) & 0x0000000F)))
#define PON_PAD_BIT_PON_GPIO12_SLEW_RATE            ((u32)0x00000001 << 12)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO12_DRIVING             ((u32)0x00000003 << 10)                  /*!< R/W   0x0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO12_DRIVING(x)               ((u32)(((x) & 0x00000003) << 10))
#define PON_PAD_GET_PON_GPIO12_DRIVING(x)           ((u32)(((x >> 10) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO12_SMT_EN               ((u32)0x00000001 << 9)                   /*!< R/W     0 Enable PON GPIO12 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO12_PULL_CTRL_1_0       ((u32)0x00000003 << 6)                   /*!< R/W   0x0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO12_PULL_CTRL_1_0(x)         ((u32)(((x) & 0x00000003) << 6))
#define PON_PAD_GET_PON_GPIO12_PULL_CTRL_1_0(x)     ((u32)(((x >> 6) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO12_PINMUX_SEL          ((u32)0x0000000F << 0)                   /*!< R/W   0xf
                                                                                              * 0000:                            0001: 
                                                                                              * 0010: PWM6               0011:
                                                                                              * 0100:                          0101:I2S0_SD_RX
                                                                                              * 0110:UART1_IN           0111: 
                                                                                              * 1000:VOE_JTAG_CLK/SWD_CK 1001: 
                                                                                              * 1010:                             1011: 
                                                                                              * 1100:                             1101: 
                                                                                              * 1110:                             1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO12_PINMUX_SEL(x)            ((u32)(((x) & 0x0000000F) << 0))
#define PON_PAD_GET_PON_GPIO12_PINMUX_SEL(x)        ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PON_GPIOF_14_15_CTRL
 * @brief
 * @{
 */
#define PON_PAD_BIT_PON_GPIO15_SLEW_RATE            ((u32)0x00000001 << 28)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO15_DRIVING             ((u32)0x00000003 << 26)                  /*!< R/W   0x0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO15_DRIVING(x)               ((u32)(((x) & 0x00000003) << 26))
#define PON_PAD_GET_PON_GPIO15_DRIVING(x)           ((u32)(((x >> 26) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO15_SMT_EN               ((u32)0x00000001 << 25)                  /*!< R/W     0 Enable PON GPIO15 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO15_PULL_CTRL_1_0       ((u32)0x00000003 << 22)                  /*!< R/W   0x0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO15_PULL_CTRL_1_0(x)         ((u32)(((x) & 0x00000003) << 22))
#define PON_PAD_GET_PON_GPIO15_PULL_CTRL_1_0(x)     ((u32)(((x >> 22) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO15_PINMUX_SEL          ((u32)0x0000000F << 16)                  /*!< R/W   0xf
                                                                                              * 0000:                            0001: SGPIO_TX 
                                                                                              * 0010: PWM9               0011:
                                                                                              * 0100:                          0101:I2S0_WS
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                             1011: 
                                                                                              * 1100:                             1101: 
                                                                                              * 1110:                             1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO15_PINMUX_SEL(x)            ((u32)(((x) & 0x0000000F) << 16))
#define PON_PAD_GET_PON_GPIO15_PINMUX_SEL(x)        ((u32)(((x >> 16) & 0x0000000F)))
#define PON_PAD_BIT_PON_GPIO14_SLEW_RATE            ((u32)0x00000001 << 12)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO14_DRIVING             ((u32)0x00000003 << 10)                  /*!< R/W   0x0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO14_DRIVING(x)               ((u32)(((x) & 0x00000003) << 10))
#define PON_PAD_GET_PON_GPIO14_DRIVING(x)           ((u32)(((x >> 10) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO14_SMT_EN               ((u32)0x00000001 << 9)                   /*!< R/W     0 Enable PON GPIO14 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO14_PULL_CTRL_1_0       ((u32)0x00000003 << 6)                   /*!< R/W   0x0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO14_PULL_CTRL_1_0(x)         ((u32)(((x) & 0x00000003) << 6))
#define PON_PAD_GET_PON_GPIO14_PULL_CTRL_1_0(x)     ((u32)(((x >> 6) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO14_PINMUX_SEL          ((u32)0x0000000F << 0)                   /*!< R/W   0xf
                                                                                              * 0000:                            0001: SGPIO
                                                                                              * 0010: PWM8                0011:
                                                                                              * 0100:                          0101:I2S0_SD_TX0
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                             1011: 
                                                                                              * 1100:                             1101: 
                                                                                              * 1110:                             1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO14_PINMUX_SEL(x)            ((u32)(((x) & 0x0000000F) << 0))
#define PON_PAD_GET_PON_GPIO14_PINMUX_SEL(x)        ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PON_GPIOB_0_1_CTRL
 * @brief
 * @{
 */
#define PON_PAD_BIT_PON_GPIO1_SLEW_RATE_020         ((u32)0x00000001 << 28)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO1_DRIVING_020          ((u32)0x00000003 << 26)                  /*!< R/W     0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO1_DRIVING_020(x)            ((u32)(((x) & 0x00000003) << 26))
#define PON_PAD_GET_PON_GPIO1_DRIVING_020(x)        ((u32)(((x >> 26) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO1_SMT_EN_020            ((u32)0x00000001 << 25)                  /*!< R/W     0 Enable PON GPIO1 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO1_PULL_CTRL_1_0_020    ((u32)0x00000003 << 22)                  /*!< R/W     0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO1_PULL_CTRL_1_0_020(x)      ((u32)(((x) & 0x00000003) << 22))
#define PON_PAD_GET_PON_GPIO1_PULL_CTRL_1_0_020(x)  ((u32)(((x >> 22) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO1_PINMUX_SEL_020       ((u32)0x0000000F << 16)                  /*!< R/W   1111
                                                                                              * 0000: USB_HSDM     0001: I2C0_SDA  
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                             1011: 
                                                                                              * 1100:                             1101: 
                                                                                              * 1110:                             1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO1_PINMUX_SEL_020(x)         ((u32)(((x) & 0x0000000F) << 16))
#define PON_PAD_GET_PON_GPIO1_PINMUX_SEL_020(x)     ((u32)(((x >> 16) & 0x0000000F)))
#define PON_PAD_BIT_PON_GPIO0_SLEW_RATE_020         ((u32)0x00000001 << 12)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO0_DRIVING_020          ((u32)0x00000003 << 10)                  /*!< R/W     0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO0_DRIVING_020(x)            ((u32)(((x) & 0x00000003) << 10))
#define PON_PAD_GET_PON_GPIO0_DRIVING_020(x)        ((u32)(((x >> 10) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO0_SMT_EN_020            ((u32)0x00000001 << 9)                   /*!< R/W     0 Enable PON GPIO0 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO0_PULL_CTRL_1_0_020    ((u32)0x00000003 << 6)                   /*!< R/W     0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO0_PULL_CTRL_1_0_020(x)      ((u32)(((x) & 0x00000003) << 6))
#define PON_PAD_GET_PON_GPIO0_PULL_CTRL_1_0_020(x)  ((u32)(((x >> 6) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO0_PINMUX_SEL_020       ((u32)0x0000000F << 0)                   /*!< R/W   1111
                                                                                              * 0000: USB_HSDP      0001: I2C0_SCL
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                         0101:
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                             1011: 
                                                                                              * 1100:                             1101: 
                                                                                              * 1110:                             1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO0_PINMUX_SEL_020(x)         ((u32)(((x) & 0x0000000F) << 0))
#define PON_PAD_GET_PON_GPIO0_PINMUX_SEL_020(x)     ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PON_GPIOB_2_3_CTRL
 * @brief
 * @{
 */
#define PON_PAD_BIT_PON_GPIO3_SLEW_RATE_024         ((u32)0x00000001 << 28)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO3_DRIVING_024          ((u32)0x00000003 << 26)                  /*!< R/W     0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO3_DRIVING_024(x)            ((u32)(((x) & 0x00000003) << 26))
#define PON_PAD_GET_PON_GPIO3_DRIVING_024(x)        ((u32)(((x >> 26) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO3_SMT_EN_024            ((u32)0x00000001 << 25)                  /*!< R/W     0 Enable PON GPIO3 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO3_PULL_CTRL_1_0_024    ((u32)0x00000003 << 22)                  /*!< R/W     0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO3_PULL_CTRL_1_0_024(x)      ((u32)(((x) & 0x00000003) << 22))
#define PON_PAD_GET_PON_GPIO3_PULL_CTRL_1_0_024(x)  ((u32)(((x >> 22) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO3_PINMUX_SEL_024       ((u32)0x0000000F << 16)                  /*!< R/W   1111
                                                                                              * 0000: USB_HSDM     0001: I2C0_SDA  
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                             1011: 
                                                                                              * 1100:                             1101: 
                                                                                              * 1110:                             1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO3_PINMUX_SEL_024(x)         ((u32)(((x) & 0x0000000F) << 16))
#define PON_PAD_GET_PON_GPIO3_PINMUX_SEL_024(x)     ((u32)(((x >> 16) & 0x0000000F)))
#define PON_PAD_BIT_PON_GPIO2_SLEW_RATE_024         ((u32)0x00000001 << 12)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO2_DRIVING_024          ((u32)0x00000003 << 10)                  /*!< R/W     0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO2_DRIVING_024(x)            ((u32)(((x) & 0x00000003) << 10))
#define PON_PAD_GET_PON_GPIO2_DRIVING_024(x)        ((u32)(((x >> 10) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO2_SMT_EN_024            ((u32)0x00000001 << 9)                   /*!< R/W     0 Enable PON GPIO2 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO2_PULL_CTRL_1_0_024    ((u32)0x00000003 << 6)                   /*!< R/W     0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO2_PULL_CTRL_1_0_024(x)      ((u32)(((x) & 0x00000003) << 6))
#define PON_PAD_GET_PON_GPIO2_PULL_CTRL_1_0_024(x)  ((u32)(((x >> 6) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO2_PINMUX_SEL_024       ((u32)0x0000000F << 0)                   /*!< R/W   1111
                                                                                              * 0000:                         0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                             1011: 
                                                                                              * 1100:                             1101: 
                                                                                              * 1110:                             1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO2_PINMUX_SEL_024(x)         ((u32)(((x) & 0x0000000F) << 0))
#define PON_PAD_GET_PON_GPIO2_PINMUX_SEL_024(x)     ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PON_GPIOB_4_5_CTRL
 * @brief
 * @{
 */
#define PON_PAD_BIT_PON_GPIO4_SLEW_RATE_028         ((u32)0x00000001 << 12)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO4_DRIVING_028          ((u32)0x00000003 << 10)                  /*!< R/W     0 0:8mA, 1:16mA */
#define PON_PAD_PON_GPIO4_DRIVING_028(x)            ((u32)(((x) & 0x00000003) << 10))
#define PON_PAD_GET_PON_GPIO4_DRIVING_028(x)        ((u32)(((x >> 10) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO4_SMT_EN_028            ((u32)0x00000001 << 9)                   /*!< R/W     0 Enable PON GPIO0 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO4_PULL_CTRL_1_0_028    ((u32)0x00000003 << 6)                   /*!< R/W     0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO4_PULL_CTRL_1_0_028(x)      ((u32)(((x) & 0x00000003) << 6))
#define PON_PAD_GET_PON_GPIO4_PULL_CTRL_1_0_028(x)  ((u32)(((x >> 6) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO4_PINMUX_SEL_028       ((u32)0x0000000F << 0)                   /*!< R/W   1111
                                                                                              * 0000: SPI_M_CLK      0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                           1011: 
                                                                                              * 1100:                           1101: 
                                                                                              * 1110:                           1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO4_PINMUX_SEL_028(x)         ((u32)(((x) & 0x0000000F) << 0))
#define PON_PAD_GET_PON_GPIO4_PINMUX_SEL_028(x)     ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PWRPAD1N_SHDN_CTRL
 * @brief
 * @{
 */
#define PON_PAD_BIT_DUMMY3                          ((u32)0x00000001 << 7)                   /*!< R/W   0   None */
#define PON_PAD_BIT_PON_GPIOS_PWD09                 ((u32)0x00000001 << 6)                   /*!< R/W   1   0: power down GPIOS */
#define PON_PAD_BIT_DUMMY2                          ((u32)0x00000001 << 5)                   /*!< R/W   1   None */
#define PON_PAD_BIT_PON_GPIOE_PWD09                 ((u32)0x00000001 << 4)                   /*!< R/W   1   0: power down GPIOE */
#define PON_PAD_BIT_PON_GPIOD_PWD09                 ((u32)0x00000001 << 3)                   /*!< R/W   1   0: power down GPIOD */
#define PON_PAD_BIT_PON_GPIOC_PWD09                 ((u32)0x00000001 << 2)                   /*!< R/W   1   0: power down GPIOC */
#define PON_PAD_BIT_DUMMY1                          ((u32)0x00000001 << 1)                   /*!< R/W   1   None */
#define PON_PAD_BIT_DUMMY0                          ((u32)0x00000001 << 0)                   /*!< R/W   1   None */
/** @} */


/** @defgroup REG_DDRPHY_PWRPAD_CTRL
 * @brief
 * @{
 */
#define PON_PAD_BIT_DDRPHY_GRP12_SPC                ((u32)0x00000001 << 25)                  /*!< R/W   0  
                                                                                              * DDRPHY GRP0 small power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP12_LPC                ((u32)0x00000001 << 24)                  /*!< R/W   0  
                                                                                              * DDRPHY GRP0 large power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP11_SPC                ((u32)0x00000001 << 23)                  /*!< R/W   0  
                                                                                              * DDRPHY GRP0 small power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP11_LPC                ((u32)0x00000001 << 22)                  /*!< R/W   0  
                                                                                              * DDRPHY GRP0 large power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP10_SPC                ((u32)0x00000001 << 21)                  /*!< R/W   0  
                                                                                              * DDRPHY GRP0 small power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP10_LPC                ((u32)0x00000001 << 20)                  /*!< R/W   0  
                                                                                              * DDRPHY GRP0 large power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP9_SPC                 ((u32)0x00000001 << 19)                  /*!< R/W   0  
                                                                                              * DDRPHY GRP0 small power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP9_LPC                 ((u32)0x00000001 << 18)                  /*!< R/W   0  
                                                                                              * DDRPHY GRP0 large power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP8_SPC                 ((u32)0x00000001 << 17)                  /*!< R/W   0  
                                                                                              * DDRPHY GRP0 small power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP8_LPC                 ((u32)0x00000001 << 16)                  /*!< R/W   0  
                                                                                              * DDRPHY GRP0 large power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP7_SPC                 ((u32)0x00000001 << 15)                  /*!< R/W   0  
                                                                                              * DDRPHY GRP0 small power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP7_LPC                 ((u32)0x00000001 << 14)                  /*!< R/W   0  
                                                                                              * DDRPHY GRP0 large power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP6_SPC                 ((u32)0x00000001 << 13)                  /*!< R/W   0  
                                                                                              * DDRPHY GRP0 small power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP6_LPC                 ((u32)0x00000001 << 12)                  /*!< R/W   0  
                                                                                              * DDRPHY GRP0 large power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP5_SPC                 ((u32)0x00000001 << 11)                  /*!< R/W   0  
                                                                                              * DDRPHY GRP0 small power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP5_LPC                 ((u32)0x00000001 << 10)                  /*!< R/W   0  
                                                                                              * DDRPHY GRP0 large power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP4_SPC                 ((u32)0x00000001 << 9)                   /*!< R/W   0  
                                                                                              * DDRPHY GRP0 small power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP4_LPC                 ((u32)0x00000001 << 8)                   /*!< R/W   0  
                                                                                              * DDRPHY GRP0 large power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP3_SPC                 ((u32)0x00000001 << 7)                   /*!< R/W   0  
                                                                                              * DDRPHY GRP0 small power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP3_LPC                 ((u32)0x00000001 << 6)                   /*!< R/W   0  
                                                                                              * DDRPHY GRP0 large power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP2_SPC                 ((u32)0x00000001 << 5)                   /*!< R/W   0  
                                                                                              * DDRPHY GRP0 small power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP2_LPC                 ((u32)0x00000001 << 4)                   /*!< R/W   0  
                                                                                              * DDRPHY GRP0 large power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP1_SPC                 ((u32)0x00000001 << 3)                   /*!< R/W   0  
                                                                                              * DDRPHY GRP0 small power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP1_LPC                 ((u32)0x00000001 << 2)                   /*!< R/W   0  
                                                                                              * DDRPHY GRP0 large power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP0_SPC                 ((u32)0x00000001 << 1)                   /*!< R/W   0  
                                                                                              * DDRPHY GRP0 small power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
#define PON_PAD_BIT_DDRPHY_GRP0_LPC                 ((u32)0x00000001 << 0)                   /*!< R/W   0  
                                                                                              * DDRPHY GRP0 large power cut
                                                                                              * 1: Enable power
                                                                                              * 0: Cut off power
                                                                                              */
/** @} */


/** @defgroup REG_PON_GPIOP_0_1_CTRL
 * @brief
 * @{
 */
#define PON_PAD_BIT_PON_GPIO1_SLEW_RATE_034         ((u32)0x00000001 << 28)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO1_DRIVING_034          ((u32)0x00000003 << 26)                  /*!< R/W     0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO1_DRIVING_034(x)            ((u32)(((x) & 0x00000003) << 26))
#define PON_PAD_GET_PON_GPIO1_DRIVING_034(x)        ((u32)(((x >> 26) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO1_SMT_EN_034            ((u32)0x00000001 << 25)                  /*!< R/W     0 Enable PON GPIO1 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO1_PULL_CTRL_1_0_034    ((u32)0x00000003 << 22)                  /*!< R/W     0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO1_PULL_CTRL_1_0_034(x)      ((u32)(((x) & 0x00000003) << 22))
#define PON_PAD_GET_PON_GPIO1_PULL_CTRL_1_0_034(x)  ((u32)(((x >> 22) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO1_PINMUX_SEL_034       ((u32)0x0000000F << 16)                  /*!< R/W   1111
                                                                                              * 0000: SPI_2_SCL  0001:  SPI_2_SCL
                                                                                              * 0010:  WIFI_UART_OUT  0011:
                                                                                              * 0100: BT_AOX_0     0101: SPI_0_SCL
                                                                                              * 0110:UART3_OUT    0111: BT_UART_OUT
                                                                                              * 1000:SPI_2_SCL   1001: SPI_2_SCL
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO1_PINMUX_SEL_034(x)         ((u32)(((x) & 0x0000000F) << 16))
#define PON_PAD_GET_PON_GPIO1_PINMUX_SEL_034(x)     ((u32)(((x >> 16) & 0x0000000F)))
#define PON_PAD_BIT_PON_GPIO0_SLEW_RATE_034         ((u32)0x00000001 << 12)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO0_DRIVING_034          ((u32)0x00000003 << 10)                  /*!< R/W     0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO0_DRIVING_034(x)            ((u32)(((x) & 0x00000003) << 10))
#define PON_PAD_GET_PON_GPIO0_DRIVING_034(x)        ((u32)(((x >> 10) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO0_SMT_EN_034            ((u32)0x00000001 << 9)                   /*!< R/W     0 Enable PON GPIO0 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO0_PULL_CTRL_1_0_034    ((u32)0x00000003 << 6)                   /*!< R/W     0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO0_PULL_CTRL_1_0_034(x)      ((u32)(((x) & 0x00000003) << 6))
#define PON_PAD_GET_PON_GPIO0_PULL_CTRL_1_0_034(x)  ((u32)(((x >> 6) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO0_PINMUX_SEL_034       ((u32)0x0000000F << 0)                   /*!< R/W   1111
                                                                                              * 0000:                        0001: 
                                                                                              * 0010: WIFI_LED       0011: RFE_CTRL_3
                                                                                              * 0100:                          0101:
                                                                                              * 0110: UART2_OUT    0111: BT_LOG
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO0_PINMUX_SEL_034(x)         ((u32)(((x) & 0x0000000F) << 0))
#define PON_PAD_GET_PON_GPIO0_PINMUX_SEL_034(x)     ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PON_GPIOP_2_3_CTRL
 * @brief
 * @{
 */
#define PON_PAD_BIT_PON_GPIO3_SLEW_RATE_038         ((u32)0x00000001 << 28)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO3_DRIVING_038          ((u32)0x00000003 << 26)                  /*!< R/W     0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO3_DRIVING_038(x)            ((u32)(((x) & 0x00000003) << 26))
#define PON_PAD_GET_PON_GPIO3_DRIVING_038(x)        ((u32)(((x >> 26) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO3_SMT_EN_038            ((u32)0x00000001 << 25)                  /*!< R/W     0 Enable PON GPIO3 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO3_PULL_CTRL_1_0_038    ((u32)0x00000003 << 22)                  /*!< R/W     0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO3_PULL_CTRL_1_0_038(x)      ((u32)(((x) & 0x00000003) << 22))
#define PON_PAD_GET_PON_GPIO3_PULL_CTRL_1_0_038(x)  ((u32)(((x >> 22) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO3_PINMUX_SEL_038       ((u32)0x0000000F << 16)                  /*!< R/W   1111
                                                                                              * 0000: TRXIP            0001: I2C2_SCL
                                                                                              * 0010:                          0011: RFE_CTRL_4
                                                                                              * 0100:BT_AOX_2        0101: SPI_0_MOSI
                                                                                              * 0110: UART3_RTS   0111: BT_UART_RTS
                                                                                              * 1000: SPI_2_MOSI   1001:  BTCMD_I2C_CK
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO3_PINMUX_SEL_038(x)         ((u32)(((x) & 0x0000000F) << 16))
#define PON_PAD_GET_PON_GPIO3_PINMUX_SEL_038(x)     ((u32)(((x >> 16) & 0x0000000F)))
#define PON_PAD_BIT_PON_GPIO2_SLEW_RATE_038         ((u32)0x00000001 << 12)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO2_DRIVING_038          ((u32)0x00000003 << 10)                  /*!< R/W     0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO2_DRIVING_038(x)            ((u32)(((x) & 0x00000003) << 10))
#define PON_PAD_GET_PON_GPIO2_DRIVING_038(x)        ((u32)(((x >> 10) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO2_SMT_EN_038            ((u32)0x00000001 << 9)                   /*!< R/W     0 Enable PON GPIO2 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO2_PULL_CTRL_1_0_038    ((u32)0x00000003 << 6)                   /*!< R/W     0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO2_PULL_CTRL_1_0_038(x)      ((u32)(((x) & 0x00000003) << 6))
#define PON_PAD_GET_PON_GPIO2_PULL_CTRL_1_0_038(x)  ((u32)(((x >> 6) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO2_PINMUX_SEL_038       ((u32)0x0000000F << 0)                   /*!< R/W   1111
                                                                                              * 0000: SPI_2_MISO  0001: SPI_2_MISO
                                                                                              * 0010: WIFI_UART_IN  0011:
                                                                                              * 0100:BT_AOX_1       0101: SPI_0_MISO
                                                                                              * 0110:  UART3_IN      0111: BT_UART_IN
                                                                                              * 1000: SPI_2_MISO   1001: SPI_2_MISO
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO2_PINMUX_SEL_038(x)         ((u32)(((x) & 0x0000000F) << 0))
#define PON_PAD_GET_PON_GPIO2_PINMUX_SEL_038(x)     ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PON_GPIOP_4_5_CTRL
 * @brief
 * @{
 */
#define PON_PAD_BIT_PON_GPIO5_SLEW_RATE_03C         ((u32)0x00000001 << 28)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO5_DRIVING_03C          ((u32)0x00000003 << 26)                  /*!< R/W     0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO5_DRIVING_03C(x)            ((u32)(((x) & 0x00000003) << 26))
#define PON_PAD_GET_PON_GPIO5_DRIVING_03C(x)        ((u32)(((x >> 26) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO5_SMT_EN_03C            ((u32)0x00000001 << 25)                  /*!< R/W     0 Enable PON GPIO5 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO5_PULL_CTRL_1_0_03C    ((u32)0x00000003 << 22)                  /*!< R/W     0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO5_PULL_CTRL_1_0_03C(x)      ((u32)(((x) & 0x00000003) << 22))
#define PON_PAD_GET_PON_GPIO5_PULL_CTRL_1_0_03C(x)  ((u32)(((x >> 22) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO5_PINMUX_SEL_03C       ((u32)0x0000000F << 16)                  /*!< R/W   1111
                                                                                              * 0000:TRXQP        0001: I2C2_SCL
                                                                                              * 0010:                        0011:
                                                                                              * 0100:                         0101:SPI_0_CS1
                                                                                              * 0110:                         0111: 
                                                                                              * 1000:                         1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO5_PINMUX_SEL_03C(x)         ((u32)(((x) & 0x0000000F) << 16))
#define PON_PAD_GET_PON_GPIO5_PINMUX_SEL_03C(x)     ((u32)(((x >> 16) & 0x0000000F)))
#define PON_PAD_BIT_PON_GPIO4_SLEW_RATE_03C         ((u32)0x00000001 << 12)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO4_DRIVING_03C          ((u32)0x00000003 << 10)                  /*!< R/W     0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO4_DRIVING_03C(x)            ((u32)(((x) & 0x00000003) << 10))
#define PON_PAD_GET_PON_GPIO4_DRIVING_03C(x)        ((u32)(((x >> 10) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO4_SMT_EN_03C            ((u32)0x00000001 << 9)                   /*!< R/W     0 Enable PON GPIO4 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO4_PULL_CTRL_1_0_03C    ((u32)0x00000003 << 6)                   /*!< R/W     0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO4_PULL_CTRL_1_0_03C(x)      ((u32)(((x) & 0x00000003) << 6))
#define PON_PAD_GET_PON_GPIO4_PULL_CTRL_1_0_03C(x)  ((u32)(((x >> 6) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO4_PINMUX_SEL_03C       ((u32)0x0000000F << 0)                   /*!< R/W   1111
                                                                                              * 0000: TRXIN             0001: I2C2_SDA 
                                                                                              * 0010:                          0011:RFE_CTRL_5
                                                                                              * 0100:BT_AOX_3     0101:SPI_0_CS0
                                                                                              * 0110: UART3_CTS     0111:BT_UART_CTS
                                                                                              * 1000:SPI_2_CS      1001: BTCMC_I2C_DD
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO4_PINMUX_SEL_03C(x)         ((u32)(((x) & 0x0000000F) << 0))
#define PON_PAD_GET_PON_GPIO4_PINMUX_SEL_03C(x)     ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PON_GPIOP_6_7_CTRL
 * @brief
 * @{
 */
#define PON_PAD_BIT_PON_GPIO7_SLEW_RATE_040         ((u32)0x00000001 << 28)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO7_DRIVING_040          ((u32)0x00000003 << 26)                  /*!< R/W     0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO7_DRIVING_040(x)            ((u32)(((x) & 0x00000003) << 26))
#define PON_PAD_GET_PON_GPIO7_DRIVING_040(x)        ((u32)(((x >> 26) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO7_SMT_EN_040            ((u32)0x00000001 << 25)                  /*!< R/W     0 Enable PON GPIO7 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO7_PULL_CTRL_1_0_040    ((u32)0x00000003 << 22)                  /*!< R/W     0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO7_PULL_CTRL_1_0_040(x)      ((u32)(((x) & 0x00000003) << 22))
#define PON_PAD_GET_PON_GPIO7_PULL_CTRL_1_0_040(x)  ((u32)(((x >> 22) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO7_PINMUX_SEL_040       ((u32)0x0000000F << 16)                  /*!< R/W   1111
                                                                                              * 0000: TRXIP            0001: I2C2_SCL
                                                                                              * 0010:                          0011: RFE_CTRL_4
                                                                                              * 0100:BT_AOX_2        0101: SPI_0_MOSI
                                                                                              * 0110: UART3_RTS   0111: BT_UART_RTS
                                                                                              * 1000: SPI_2_MOSI   1001:  BTCMD_I2C_CK
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO7_PINMUX_SEL_040(x)         ((u32)(((x) & 0x0000000F) << 16))
#define PON_PAD_GET_PON_GPIO7_PINMUX_SEL_040(x)     ((u32)(((x >> 16) & 0x0000000F)))
#define PON_PAD_BIT_PON_GPIO6_SLEW_RATE_040         ((u32)0x00000001 << 12)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO6_DRIVING_040          ((u32)0x00000003 << 10)                  /*!< R/W     0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO6_DRIVING_040(x)            ((u32)(((x) & 0x00000003) << 10))
#define PON_PAD_GET_PON_GPIO6_DRIVING_040(x)        ((u32)(((x >> 10) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO6_SMT_EN_040            ((u32)0x00000001 << 9)                   /*!< R/W     0 Enable PON GPIO6 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO6_PULL_CTRL_1_0_040    ((u32)0x00000003 << 6)                   /*!< R/W     0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO6_PULL_CTRL_1_0_040(x)      ((u32)(((x) & 0x00000003) << 6))
#define PON_PAD_GET_PON_GPIO6_PULL_CTRL_1_0_040(x)  ((u32)(((x >> 6) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO6_PINMUX_SEL_040       ((u32)0x0000000F << 0)                   /*!< R/W   1111
                                                                                              * 0000:TRXQN           0001: I2C2_SDA
                                                                                              * 0010:                           0011:
                                                                                              * 0100:                             0101:SPI_0_CS2
                                                                                              * 0110:                            0111: 
                                                                                              * 1000:                          1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO6_PINMUX_SEL_040(x)         ((u32)(((x) & 0x0000000F) << 0))
#define PON_PAD_GET_PON_GPIO6_PINMUX_SEL_040(x)     ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PON_GPIOP_8_9_CTRL
 * @brief
 * @{
 */
#define PON_PAD_BIT_PON_GPIO9_SLEW_RATE_044         ((u32)0x00000001 << 28)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO9_DRIVING_044          ((u32)0x00000003 << 26)                  /*!< R/W     0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO9_DRIVING_044(x)            ((u32)(((x) & 0x00000003) << 26))
#define PON_PAD_GET_PON_GPIO9_DRIVING_044(x)        ((u32)(((x >> 26) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO9_SMT_EN_044            ((u32)0x00000001 << 25)                  /*!< R/W     0 Enable PON GPIO9 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO9_PULL_CTRL_1_0_044    ((u32)0x00000003 << 22)                  /*!< R/W     0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO9_PULL_CTRL_1_0_044(x)      ((u32)(((x) & 0x00000003) << 22))
#define PON_PAD_GET_PON_GPIO9_PULL_CTRL_1_0_044(x)  ((u32)(((x >> 22) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO9_PINMUX_SEL_044       ((u32)0x0000000F << 16)                  /*!< R/W   1111
                                                                                              * 0000:TRXQP        0001: I2C2_SCL
                                                                                              * 0010:                        0011:
                                                                                              * 0100:                         0101:SPI_0_CS1
                                                                                              * 0110:                         0111: 
                                                                                              * 1000:                         1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO9_PINMUX_SEL_044(x)         ((u32)(((x) & 0x0000000F) << 16))
#define PON_PAD_GET_PON_GPIO9_PINMUX_SEL_044(x)     ((u32)(((x >> 16) & 0x0000000F)))
#define PON_PAD_BIT_PON_GPIO8_SLEW_RATE_044         ((u32)0x00000001 << 12)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO8_DRIVING_044          ((u32)0x00000003 << 10)                  /*!< R/W     0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO8_DRIVING_044(x)            ((u32)(((x) & 0x00000003) << 10))
#define PON_PAD_GET_PON_GPIO8_DRIVING_044(x)        ((u32)(((x >> 10) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO8_SMT_EN_044            ((u32)0x00000001 << 9)                   /*!< R/W     0 Enable PON GPIO8 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO8_PULL_CTRL_1_0_044    ((u32)0x00000003 << 6)                   /*!< R/W     0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO8_PULL_CTRL_1_0_044(x)      ((u32)(((x) & 0x00000003) << 6))
#define PON_PAD_GET_PON_GPIO8_PULL_CTRL_1_0_044(x)  ((u32)(((x >> 6) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO8_PINMUX_SEL_044       ((u32)0x0000000F << 0)                   /*!< R/W   1111
                                                                                              * 0000: TRXIN             0001: I2C2_SDA 
                                                                                              * 0010:                          0011:RFE_CTRL_5
                                                                                              * 0100:BT_AOX_3     0101:SPI_0_CS0
                                                                                              * 0110: UART3_CTS     0111:BT_UART_CTS
                                                                                              * 1000:SPI_2_CS      1001: BTCMC_I2C_DD
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO8_PINMUX_SEL_044(x)         ((u32)(((x) & 0x0000000F) << 0))
#define PON_PAD_GET_PON_GPIO8_PINMUX_SEL_044(x)     ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PON_GPIOP_10_11_CTRL
 * @brief
 * @{
 */
#define PON_PAD_BIT_PON_GPIO11_SLEW_RATE_048        ((u32)0x00000001 << 28)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO11_DRIVING_048         ((u32)0x00000003 << 26)                  /*!< R/W     0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO11_DRIVING_048(x)           ((u32)(((x) & 0x00000003) << 26))
#define PON_PAD_GET_PON_GPIO11_DRIVING_048(x)       ((u32)(((x >> 26) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO11_SMT_EN_048           ((u32)0x00000001 << 25)                  /*!< R/W     0 Enable PON GPIO11 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO11_PULL_CTRL_1_0_048   ((u32)0x00000003 << 22)                  /*!< R/W     0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO11_PULL_CTRL_1_0_048(x)     ((u32)(((x) & 0x00000003) << 22))
#define PON_PAD_GET_PON_GPIO11_PULL_CTRL_1_0_048(x) ((u32)(((x >> 22) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO11_PINMUX_SEL_048      ((u32)0x0000000F << 16)                  /*!< R/W   1111
                                                                                              * 0000: TRXIP            0001: I2C2_SCL
                                                                                              * 0010:                          0011: RFE_CTRL_4
                                                                                              * 0100:BT_AOX_2        0101: SPI_0_MOSI
                                                                                              * 0110: UART3_RTS   0111: BT_UART_RTS
                                                                                              * 1000: SPI_2_MOSI   1001:  BTCMD_I2C_CK
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO11_PINMUX_SEL_048(x)        ((u32)(((x) & 0x0000000F) << 16))
#define PON_PAD_GET_PON_GPIO11_PINMUX_SEL_048(x)    ((u32)(((x >> 16) & 0x0000000F)))
#define PON_PAD_BIT_PON_GPIO10_SLEW_RATE_048        ((u32)0x00000001 << 12)                  /*!< R/W     0 0:Disable,1:Enable */
#define PON_PAD_MASK_PON_GPIO10_DRIVING_048         ((u32)0x00000003 << 10)                  /*!< R/W     0 0:4mA, 1:8mA */
#define PON_PAD_PON_GPIO10_DRIVING_048(x)           ((u32)(((x) & 0x00000003) << 10))
#define PON_PAD_GET_PON_GPIO10_DRIVING_048(x)       ((u32)(((x >> 10) & 0x00000003)))
#define PON_PAD_BIT_PON_GPIO10_SMT_EN_048           ((u32)0x00000001 << 9)                   /*!< R/W     0 Enable PON GPIO10 Schmitt trigger;  1: enable */
#define PON_PAD_MASK_PON_GPIO10_PULL_CTRL_1_0_048   ((u32)0x00000003 << 6)                   /*!< R/W     0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define PON_PAD_PON_GPIO10_PULL_CTRL_1_0_048(x)     ((u32)(((x) & 0x00000003) << 6))
#define PON_PAD_GET_PON_GPIO10_PULL_CTRL_1_0_048(x) ((u32)(((x >> 6) & 0x00000003)))
#define PON_PAD_MASK_PON_GPIO10_PINMUX_SEL_048      ((u32)0x0000000F << 0)                   /*!< R/W   1111
                                                                                              * 0000:TRXQN           0001: I2C2_SDA
                                                                                              * 0010:                           0011:
                                                                                              * 0100:                             0101:SPI_0_CS2
                                                                                              * 0110:                            0111: 
                                                                                              * 1000:                          1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define PON_PAD_PON_GPIO10_PINMUX_SEL_048(x)        ((u32)(((x) & 0x0000000F) << 0))
#define PON_PAD_GET_PON_GPIO10_PINMUX_SEL_048(x)    ((u32)(((x >> 0) & 0x0000000F)))
/** @} */




/** @defgroup PON_PAD_Exported_Types PON_PAD Exported Types
* @{
*/
/**
* @brief PON_PAD Register Declaration
*/

typedef struct {
	__IO uint32_t PON_GPIOF_0_1_CTRL;                           /*!< Register,  Address offset:0x0000 */
	__IO uint32_t PON_GPIOF_2_3_CTRL;                           /*!< Register,  Address offset:0x0004 */
	__IO uint32_t PON_GPIOF_4_5_CTRL;                           /*!< Register,  Address offset:0x0008 */
	__IO uint32_t PON_GPIOF_6_7_CTRL;                           /*!< Register,  Address offset:0x000C */
	__IO uint32_t PON_GPIOF_8_9_CTRL;                           /*!< Register,  Address offset:0x0010 */
	__IO uint32_t PON_GPIOF_10_11_CTRL;                         /*!< Register,  Address offset:0x0014 */
	__IO uint32_t PON_GPIOF_12_13_CTRL;                         /*!< Register,  Address offset:0x0018 */
	__IO uint32_t PON_GPIOF_14_15_CTRL;                         /*!< Register,  Address offset:0x001C */
	__IO uint32_t PON_GPIOB_0_1_CTRL;                           /*!< Register,  Address offset:0x0020 */
	__IO uint32_t PON_GPIOB_2_3_CTRL;                           /*!< Register,  Address offset:0x0024 */
	__IO uint32_t PON_GPIOB_4_5_CTRL;                           /*!< Register,  Address offset:0x0028 */
	__IO uint32_t PWRPAD1N_SHDN_CTRL;                           /*!< Register,  Address offset:0x002C */
	__IO uint32_t DDRPHY_PWRPAD_CTRL;                           /*!< Register,  Address offset:0x0030 */
	__IO uint32_t PON_GPIOP_0_1_CTRL;                           /*!< Register,  Address offset:0x0034 */
	__IO uint32_t PON_GPIOP_2_3_CTRL;                           /*!< Register,  Address offset:0x0038 */
	__IO uint32_t PON_GPIOP_4_5_CTRL;                           /*!< Register,  Address offset:0x003C */
	__IO uint32_t PON_GPIOP_6_7_CTRL;                           /*!< Register,  Address offset:0x0040 */
	__IO uint32_t PON_GPIOP_8_9_CTRL;                           /*!< Register,  Address offset:0x0044 */
	__IO uint32_t PON_GPIOP_10_11_CTRL;                         /*!< Register,  Address offset:0x0048 */
} PON_PAD_TypeDef;

#define PON_PAD_BASE ((PON_PAD_TypeDef *) (0x4023A000))
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
#endif  // SYSREG_PON_PAD_H
