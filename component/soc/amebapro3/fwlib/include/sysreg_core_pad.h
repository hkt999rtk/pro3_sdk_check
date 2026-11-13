/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_CORE_PAD_H
#define SYSREG_CORE_PAD_H

/* AUTO_GEN_START */

/* CORE_PAD is a part of SYSON */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup CORE_PAD
* @brief CORE_PAD driver modules
* @{
*/


/** @defgroup REG_SYSON_GPIOC_0_1_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO1_SLEW_RATE          ((u32)0x00000001 << 28)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO1_DRIVING           ((u32)0x00000003 << 26)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO1_DRIVING(x)             ((u32)(((x) & 0x00000003) << 26))
#define CORE_PAD_GET_SYSON_GPIO1_DRIVING(x)         ((u32)(((x >> 26) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO1_SMT_EN             ((u32)0x00000001 << 25)                  /*!< R/W   0.0 Enable SYSON GPIO1 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO1_PULL_CTRL_1_0     ((u32)0x00000003 << 22)                  /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO1_PULL_CTRL_1_0(x)       ((u32)(((x) & 0x00000003) << 22))
#define CORE_PAD_GET_SYSON_GPIO1_PULL_CTRL_1_0(x)   ((u32)(((x >> 22) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO1_PINMUX_SEL        ((u32)0x0000000F << 16)                  /*!< R/W   1111.0
                                                                                              * 0000: SPI_M_DATA3 0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO1_PINMUX_SEL(x)          ((u32)(((x) & 0x0000000F) << 16))
#define CORE_PAD_GET_SYSON_GPIO1_PINMUX_SEL(x)      ((u32)(((x >> 16) & 0x0000000F)))
#define CORE_PAD_BIT_SYSON_GPIO0_SLEW_RATE          ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO0_DRIVING           ((u32)0x00000003 << 10)                  /*!< R/W   0.0 0:8mA, 1:16mA */
#define CORE_PAD_SYSON_GPIO0_DRIVING(x)             ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO0_DRIVING(x)         ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO0_SMT_EN             ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO0 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO0_PULL_CTRL_1_0     ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO0_PULL_CTRL_1_0(x)       ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO0_PULL_CTRL_1_0(x)   ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO0_PINMUX_SEL        ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000: SPI_M_CLK      0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                           1011: 
                                                                                              * 1100:                           1101: 
                                                                                              * 1110:                           1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO0_PINMUX_SEL(x)          ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO0_PINMUX_SEL(x)      ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOC_2_3_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO3_SLEW_RATE          ((u32)0x00000001 << 28)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO3_DRIVING           ((u32)0x00000003 << 26)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO3_DRIVING(x)             ((u32)(((x) & 0x00000003) << 26))
#define CORE_PAD_GET_SYSON_GPIO3_DRIVING(x)         ((u32)(((x >> 26) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO3_SMT_EN             ((u32)0x00000001 << 25)                  /*!< R/W   0.0 Enable SYSON GPIO3 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO3_PULL_CTRL_1_0     ((u32)0x00000003 << 22)                  /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO3_PULL_CTRL_1_0(x)       ((u32)(((x) & 0x00000003) << 22))
#define CORE_PAD_GET_SYSON_GPIO3_PULL_CTRL_1_0(x)   ((u32)(((x >> 22) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO3_PINMUX_SEL        ((u32)0x0000000F << 16)                  /*!< R/W   1111.0
                                                                                              * 0000: SPI_M_DATA2 0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO3_PINMUX_SEL(x)          ((u32)(((x) & 0x0000000F) << 16))
#define CORE_PAD_GET_SYSON_GPIO3_PINMUX_SEL(x)      ((u32)(((x >> 16) & 0x0000000F)))
#define CORE_PAD_BIT_SYSON_GPIO2_SLEW_RATE          ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO2_DRIVING           ((u32)0x00000003 << 10)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO2_DRIVING(x)             ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO2_DRIVING(x)         ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO2_SMT_EN             ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO2 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO2_PULL_CTRL_1_0     ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO2_PULL_CTRL_1_0(x)       ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO2_PULL_CTRL_1_0(x)   ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO2_PINMUX_SEL        ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000: SPI_M_DATA0 0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO2_PINMUX_SEL(x)          ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO2_PINMUX_SEL(x)      ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOC_4_5_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO5_SLEW_RATE          ((u32)0x00000001 << 28)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO5_DRIVING           ((u32)0x00000003 << 26)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO5_DRIVING(x)             ((u32)(((x) & 0x00000003) << 26))
#define CORE_PAD_GET_SYSON_GPIO5_DRIVING(x)         ((u32)(((x >> 26) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO5_SMT_EN             ((u32)0x00000001 << 25)                  /*!< R/W   0.0 Enable SYSON GPIO5 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO5_PULL_CTRL_1_0     ((u32)0x00000003 << 22)                  /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO5_PULL_CTRL_1_0(x)       ((u32)(((x) & 0x00000003) << 22))
#define CORE_PAD_GET_SYSON_GPIO5_PULL_CTRL_1_0(x)   ((u32)(((x >> 22) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO5_PINMUX_SEL        ((u32)0x0000000F << 16)                  /*!< R/W   1111.0
                                                                                              * 0000: SPI_M_CS       0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO5_PINMUX_SEL(x)          ((u32)(((x) & 0x0000000F) << 16))
#define CORE_PAD_GET_SYSON_GPIO5_PINMUX_SEL(x)      ((u32)(((x >> 16) & 0x0000000F)))
#define CORE_PAD_BIT_SYSON_GPIO4_SLEW_RATE          ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO4_DRIVING           ((u32)0x00000003 << 10)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO4_DRIVING(x)             ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO4_DRIVING(x)         ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO4_SMT_EN             ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO4 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO4_PULL_CTRL_1_0     ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO4_PULL_CTRL_1_0(x)       ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO4_PULL_CTRL_1_0(x)   ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO4_PINMUX_SEL        ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000: SPI_M_DATA1 0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO4_PINMUX_SEL(x)          ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO4_PINMUX_SEL(x)      ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOC_6_7_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO7_SLEW_RATE          ((u32)0x00000001 << 28)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO7_DRIVING           ((u32)0x00000003 << 26)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO7_DRIVING(x)             ((u32)(((x) & 0x00000003) << 26))
#define CORE_PAD_GET_SYSON_GPIO7_DRIVING(x)         ((u32)(((x >> 26) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO7_SMT_EN             ((u32)0x00000001 << 25)                  /*!< R/W   0.0 Enable SYSON GPIO7 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO7_PULL_CTRL_1_0     ((u32)0x00000003 << 22)                  /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO7_PULL_CTRL_1_0(x)       ((u32)(((x) & 0x00000003) << 22))
#define CORE_PAD_GET_SYSON_GPIO7_PULL_CTRL_1_0(x)   ((u32)(((x >> 22) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO7_PINMUX_SEL        ((u32)0x0000000F << 16)                  /*!< R/W   1111.0
                                                                                              * 0000: SPI_M_DATA2 0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO7_PINMUX_SEL(x)          ((u32)(((x) & 0x0000000F) << 16))
#define CORE_PAD_GET_SYSON_GPIO7_PINMUX_SEL(x)      ((u32)(((x >> 16) & 0x0000000F)))
#define CORE_PAD_BIT_SYSON_GPIO6_SLEW_RATE          ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO6_DRIVING           ((u32)0x00000003 << 10)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO6_DRIVING(x)             ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO6_DRIVING(x)         ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO6_SMT_EN             ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO6 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO6_PULL_CTRL_1_0     ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO6_PULL_CTRL_1_0(x)       ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO6_PULL_CTRL_1_0(x)   ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO6_PINMUX_SEL        ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000: SPI_M_DATA0 0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO6_PINMUX_SEL(x)          ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO6_PINMUX_SEL(x)      ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOC_8_9_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO9_SLEW_RATE          ((u32)0x00000001 << 28)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO9_DRIVING           ((u32)0x00000003 << 26)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO9_DRIVING(x)             ((u32)(((x) & 0x00000003) << 26))
#define CORE_PAD_GET_SYSON_GPIO9_DRIVING(x)         ((u32)(((x >> 26) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO9_SMT_EN             ((u32)0x00000001 << 25)                  /*!< R/W   0.0 Enable SYSON GPIO9 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO9_PULL_CTRL_1_0     ((u32)0x00000003 << 22)                  /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO9_PULL_CTRL_1_0(x)       ((u32)(((x) & 0x00000003) << 22))
#define CORE_PAD_GET_SYSON_GPIO9_PULL_CTRL_1_0(x)   ((u32)(((x >> 22) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO9_PINMUX_SEL        ((u32)0x0000000F << 16)                  /*!< R/W   1111.0
                                                                                              * 0000: SPI_M_CS       0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO9_PINMUX_SEL(x)          ((u32)(((x) & 0x0000000F) << 16))
#define CORE_PAD_GET_SYSON_GPIO9_PINMUX_SEL(x)      ((u32)(((x >> 16) & 0x0000000F)))
#define CORE_PAD_BIT_SYSON_GPIO8_SLEW_RATE          ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO8_DRIVING           ((u32)0x00000003 << 10)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO8_DRIVING(x)             ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO8_DRIVING(x)         ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO8_SMT_EN             ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO8 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO8_PULL_CTRL_1_0     ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO8_PULL_CTRL_1_0(x)       ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO8_PULL_CTRL_1_0(x)   ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO8_PINMUX_SEL        ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000: SPI_M_DATA1 0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO8_PINMUX_SEL(x)          ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO8_PINMUX_SEL(x)      ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOC_10_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO10_SLEW_RATE         ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO10_DRIVING          ((u32)0x00000003 << 10)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO10_DRIVING(x)            ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO10_DRIVING(x)        ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO10_SMT_EN            ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO8 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO10_PULL_CTRL_1_0    ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO10_PULL_CTRL_1_0(x)      ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO10_PULL_CTRL_1_0(x)  ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO10_PINMUX_SEL       ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000: SPI_M_DATA1 0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO10_PINMUX_SEL(x)         ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO10_PINMUX_SEL(x)     ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOD_0_1_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO1_SLEW_RATE_018      ((u32)0x00000001 << 28)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO1_DRIVING_018       ((u32)0x00000003 << 26)                  /*!< R/W   0.0 Wait for pad type */
#define CORE_PAD_SYSON_GPIO1_DRIVING_018(x)         ((u32)(((x) & 0x00000003) << 26))
#define CORE_PAD_GET_SYSON_GPIO1_DRIVING_018(x)     ((u32)(((x >> 26) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO1_SMT_EN_018         ((u32)0x00000001 << 25)                  /*!< R/W   0.0 Enable SYSON GPIO1 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO1_PULL_CTRL_1_0_018 ((u32)0x00000003 << 22)                  /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO1_PULL_CTRL_1_0_018(x)   ((u32)(((x) & 0x00000003) << 22))
#define CORE_PAD_GET_SYSON_GPIO1_PULL_CTRL_1_0_018(x) ((u32)(((x >> 22) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO1_PINMUX_SEL_018    ((u32)0x0000000F << 16)                  /*!< R/W   1111.0
                                                                                              * 0000: MIPI_DATA0_N 0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO1_PINMUX_SEL_018(x)      ((u32)(((x) & 0x0000000F) << 16))
#define CORE_PAD_GET_SYSON_GPIO1_PINMUX_SEL_018(x)  ((u32)(((x >> 16) & 0x0000000F)))
#define CORE_PAD_BIT_SYSON_GPIO0_SLEW_RATE_018      ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO0_DRIVING_018       ((u32)0x00000003 << 10)                  /*!< R/W   0.0 Wait for pad type */
#define CORE_PAD_SYSON_GPIO0_DRIVING_018(x)         ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO0_DRIVING_018(x)     ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO0_SMT_EN_018         ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO0 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO0_PULL_CTRL_1_0_018 ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO0_PULL_CTRL_1_0_018(x)   ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO0_PULL_CTRL_1_0_018(x) ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO0_PINMUX_SEL_018    ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000: MIPI_DATA0_P 0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO0_PINMUX_SEL_018(x)      ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO0_PINMUX_SEL_018(x)  ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOD_2_3_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO3_SLEW_RATE_01C      ((u32)0x00000001 << 28)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO3_DRIVING_01C       ((u32)0x00000003 << 26)                  /*!< R/W   0.0 Wait for pad type */
#define CORE_PAD_SYSON_GPIO3_DRIVING_01C(x)         ((u32)(((x) & 0x00000003) << 26))
#define CORE_PAD_GET_SYSON_GPIO3_DRIVING_01C(x)     ((u32)(((x >> 26) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO3_SMT_EN_01C         ((u32)0x00000001 << 25)                  /*!< R/W   0.0 Enable SYSON GPIO3 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO3_PULL_CTRL_1_0_01C ((u32)0x00000003 << 22)                  /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO3_PULL_CTRL_1_0_01C(x)   ((u32)(((x) & 0x00000003) << 22))
#define CORE_PAD_GET_SYSON_GPIO3_PULL_CTRL_1_0_01C(x) ((u32)(((x >> 22) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO3_PINMUX_SEL_01C    ((u32)0x0000000F << 16)                  /*!< R/W   1111.0
                                                                                              * 0000: MIPI_DATA1_N 0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO3_PINMUX_SEL_01C(x)      ((u32)(((x) & 0x0000000F) << 16))
#define CORE_PAD_GET_SYSON_GPIO3_PINMUX_SEL_01C(x)  ((u32)(((x >> 16) & 0x0000000F)))
#define CORE_PAD_BIT_SYSON_GPIO2_SLEW_RATE_01C      ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO2_DRIVING_01C       ((u32)0x00000003 << 10)                  /*!< R/W   0.0 Wait for pad type */
#define CORE_PAD_SYSON_GPIO2_DRIVING_01C(x)         ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO2_DRIVING_01C(x)     ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO2_SMT_EN_01C         ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO2 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO2_PULL_CTRL_1_0_01C ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO2_PULL_CTRL_1_0_01C(x)   ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO2_PULL_CTRL_1_0_01C(x) ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO2_PINMUX_SEL_01C    ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000: MIPI_DATA1_P 0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                           0111: 
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO2_PINMUX_SEL_01C(x)      ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO2_PINMUX_SEL_01C(x)  ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOD_4_5_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO5_SLEW_RATE_020      ((u32)0x00000001 << 28)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO5_DRIVING_020       ((u32)0x00000003 << 26)                  /*!< R/W   0.0 Wait for pad type */
#define CORE_PAD_SYSON_GPIO5_DRIVING_020(x)         ((u32)(((x) & 0x00000003) << 26))
#define CORE_PAD_GET_SYSON_GPIO5_DRIVING_020(x)     ((u32)(((x >> 26) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO5_SMT_EN_020         ((u32)0x00000001 << 25)                  /*!< R/W   0.0 Enable SYSON GPIO5 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO5_PULL_CTRL_1_0_020 ((u32)0x00000003 << 22)                  /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO5_PULL_CTRL_1_0_020(x)   ((u32)(((x) & 0x00000003) << 22))
#define CORE_PAD_GET_SYSON_GPIO5_PULL_CTRL_1_0_020(x) ((u32)(((x >> 22) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO5_PINMUX_SEL_020    ((u32)0x0000000F << 16)                  /*!< R/W   1111.0
                                                                                              * 0000: MIPI_CKI_N     0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                          0111: 
                                                                                              * 1000:                          1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO5_PINMUX_SEL_020(x)      ((u32)(((x) & 0x0000000F) << 16))
#define CORE_PAD_GET_SYSON_GPIO5_PINMUX_SEL_020(x)  ((u32)(((x >> 16) & 0x0000000F)))
#define CORE_PAD_BIT_SYSON_GPIO4_SLEW_RATE_020      ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO4_DRIVING_020       ((u32)0x00000003 << 10)                  /*!< R/W   0.0 Wait for pad type */
#define CORE_PAD_SYSON_GPIO4_DRIVING_020(x)         ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO4_DRIVING_020(x)     ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO4_SMT_EN_020         ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO4 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO4_PULL_CTRL_1_0_020 ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO4_PULL_CTRL_1_0_020(x)   ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO4_PULL_CTRL_1_0_020(x) ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO4_PINMUX_SEL_020    ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000: MIPI_CKI_P     0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                          0111: 
                                                                                              * 1000:                          1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO4_PINMUX_SEL_020(x)      ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO4_PINMUX_SEL_020(x)  ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOD_6_7_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO7_SLEW_RATE_024      ((u32)0x00000001 << 28)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO7_DRIVING_024       ((u32)0x00000003 << 26)                  /*!< R/W   0.0 Wait for pad type */
#define CORE_PAD_SYSON_GPIO7_DRIVING_024(x)         ((u32)(((x) & 0x00000003) << 26))
#define CORE_PAD_GET_SYSON_GPIO7_DRIVING_024(x)     ((u32)(((x >> 26) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO7_SMT_EN_024         ((u32)0x00000001 << 25)                  /*!< R/W   0.0 Enable SYSON GPIO7 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO7_PULL_CTRL_1_0_024 ((u32)0x00000003 << 22)                  /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO7_PULL_CTRL_1_0_024(x)   ((u32)(((x) & 0x00000003) << 22))
#define CORE_PAD_GET_SYSON_GPIO7_PULL_CTRL_1_0_024(x) ((u32)(((x >> 22) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO7_PINMUX_SEL_024    ((u32)0x0000000F << 16)                  /*!< R/W   1111.0
                                                                                              * 0000: MIPI_DATA2_N 0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                          0111: 
                                                                                              * 1000:                          1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO7_PINMUX_SEL_024(x)      ((u32)(((x) & 0x0000000F) << 16))
#define CORE_PAD_GET_SYSON_GPIO7_PINMUX_SEL_024(x)  ((u32)(((x >> 16) & 0x0000000F)))
#define CORE_PAD_BIT_SYSON_GPIO6_SLEW_RATE_024      ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO6_DRIVING_024       ((u32)0x00000003 << 10)                  /*!< R/W   0.0 Wait for pad type */
#define CORE_PAD_SYSON_GPIO6_DRIVING_024(x)         ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO6_DRIVING_024(x)     ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO6_SMT_EN_024         ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO6 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO6_PULL_CTRL_1_0_024 ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO6_PULL_CTRL_1_0_024(x)   ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO6_PULL_CTRL_1_0_024(x) ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO6_PINMUX_SEL_024    ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000: MIPI_DATA2_P 0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                          0111: 
                                                                                              * 1000:                          1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO6_PINMUX_SEL_024(x)      ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO6_PINMUX_SEL_024(x)  ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOD_8_9_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO9_SLEW_RATE_028      ((u32)0x00000001 << 28)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO9_DRIVING_028       ((u32)0x00000003 << 26)                  /*!< R/W   0.0 Wait for pad type */
#define CORE_PAD_SYSON_GPIO9_DRIVING_028(x)         ((u32)(((x) & 0x00000003) << 26))
#define CORE_PAD_GET_SYSON_GPIO9_DRIVING_028(x)     ((u32)(((x >> 26) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO9_SMT_EN_028         ((u32)0x00000001 << 25)                  /*!< R/W   0.0 Enable SYSON GPIO9 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO9_PULL_CTRL_1_0_028 ((u32)0x00000003 << 22)                  /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO9_PULL_CTRL_1_0_028(x)   ((u32)(((x) & 0x00000003) << 22))
#define CORE_PAD_GET_SYSON_GPIO9_PULL_CTRL_1_0_028(x) ((u32)(((x >> 22) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO9_PINMUX_SEL_028    ((u32)0x0000000F << 16)                  /*!< R/W   1111.0
                                                                                              * 0000: MIPI_DATA3_N 0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                          0111: 
                                                                                              * 1000:                          1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO9_PINMUX_SEL_028(x)      ((u32)(((x) & 0x0000000F) << 16))
#define CORE_PAD_GET_SYSON_GPIO9_PINMUX_SEL_028(x)  ((u32)(((x >> 16) & 0x0000000F)))
#define CORE_PAD_BIT_SYSON_GPIO8_SLEW_RATE_028      ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO8_DRIVING_028       ((u32)0x00000003 << 10)                  /*!< R/W   0.0 Wait for pad type */
#define CORE_PAD_SYSON_GPIO8_DRIVING_028(x)         ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO8_DRIVING_028(x)     ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO8_SMT_EN_028         ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO8 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO8_PULL_CTRL_1_0_028 ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO8_PULL_CTRL_1_0_028(x)   ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO8_PULL_CTRL_1_0_028(x) ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO8_PINMUX_SEL_028    ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000: MIPI_DATA3_P 0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                          0111: 
                                                                                              * 1000:                          1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO8_PINMUX_SEL_028(x)      ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO8_PINMUX_SEL_028(x)  ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOE_0_1_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO1_SLEW_RATE_02C      ((u32)0x00000001 << 28)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO1_DRIVING_02C       ((u32)0x00000003 << 26)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO1_DRIVING_02C(x)         ((u32)(((x) & 0x00000003) << 26))
#define CORE_PAD_GET_SYSON_GPIO1_DRIVING_02C(x)     ((u32)(((x >> 26) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO1_SMT_EN_02C         ((u32)0x00000001 << 25)                  /*!< R/W   0.0 Enable SYSON GPIO1 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO1_PULL_CTRL_1_0_02C ((u32)0x00000003 << 22)                  /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO1_PULL_CTRL_1_0_02C(x)   ((u32)(((x) & 0x00000003) << 22))
#define CORE_PAD_GET_SYSON_GPIO1_PULL_CTRL_1_0_02C(x) ((u32)(((x >> 22) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO1_PINMUX_SEL_02C    ((u32)0x0000000F << 16)                  /*!< R/W   1111.0
                                                                                              * 0000: SPI_2_SCL  0001:  SPI_2_SCL
                                                                                              * 0010:  WIFI_UART_OUT  0011:
                                                                                              * 0100: BT_AOX_0     0101: SPI_0_SCL
                                                                                              * 0110:UART3_OUT    0111: BT_UART_OUT
                                                                                              * 1000:SPI_2_SCL   1001: SPI_2_SCL
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO1_PINMUX_SEL_02C(x)      ((u32)(((x) & 0x0000000F) << 16))
#define CORE_PAD_GET_SYSON_GPIO1_PINMUX_SEL_02C(x)  ((u32)(((x >> 16) & 0x0000000F)))
#define CORE_PAD_BIT_SYSON_GPIO0_SLEW_RATE_02C      ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO0_DRIVING_02C       ((u32)0x00000003 << 10)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO0_DRIVING_02C(x)         ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO0_DRIVING_02C(x)     ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO0_SMT_EN_02C         ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO0 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO0_PULL_CTRL_1_0_02C ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO0_PULL_CTRL_1_0_02C(x)   ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO0_PULL_CTRL_1_0_02C(x) ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO0_PINMUX_SEL_02C    ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000:                        0001: 
                                                                                              * 0010: WIFI_LED       0011: RFE_CTRL_3
                                                                                              * 0100:                          0101:
                                                                                              * 0110: UART2_OUT    0111: BT_LOG
                                                                                              * 1000:                           1001: 
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO0_PINMUX_SEL_02C(x)      ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO0_PINMUX_SEL_02C(x)  ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOE_2_3_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO3_SLEW_RATE_030      ((u32)0x00000001 << 28)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO3_DRIVING_030       ((u32)0x00000003 << 26)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO3_DRIVING_030(x)         ((u32)(((x) & 0x00000003) << 26))
#define CORE_PAD_GET_SYSON_GPIO3_DRIVING_030(x)     ((u32)(((x >> 26) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO3_SMT_EN_030         ((u32)0x00000001 << 25)                  /*!< R/W   0.0 Enable SYSON GPIO3 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO3_PULL_CTRL_1_0_030 ((u32)0x00000003 << 22)                  /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO3_PULL_CTRL_1_0_030(x)   ((u32)(((x) & 0x00000003) << 22))
#define CORE_PAD_GET_SYSON_GPIO3_PULL_CTRL_1_0_030(x) ((u32)(((x >> 22) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO3_PINMUX_SEL_030    ((u32)0x0000000F << 16)                  /*!< R/W   1111.0
                                                                                              * 0000: TRXIP            0001: I2C2_SCL
                                                                                              * 0010:                          0011: RFE_CTRL_4
                                                                                              * 0100:BT_AOX_2        0101: SPI_0_MOSI
                                                                                              * 0110: UART3_RTS   0111: BT_UART_RTS
                                                                                              * 1000: SPI_2_MOSI   1001:  BTCMD_I2C_CK
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO3_PINMUX_SEL_030(x)      ((u32)(((x) & 0x0000000F) << 16))
#define CORE_PAD_GET_SYSON_GPIO3_PINMUX_SEL_030(x)  ((u32)(((x >> 16) & 0x0000000F)))
#define CORE_PAD_BIT_SYSON_GPIO2_SLEW_RATE_030      ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO2_DRIVING_030       ((u32)0x00000003 << 10)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO2_DRIVING_030(x)         ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO2_DRIVING_030(x)     ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO2_SMT_EN_030         ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO2 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO2_PULL_CTRL_1_0_030 ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO2_PULL_CTRL_1_0_030(x)   ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO2_PULL_CTRL_1_0_030(x) ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO2_PINMUX_SEL_030    ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000: SPI_2_MISO  0001: SPI_2_MISO
                                                                                              * 0010: WIFI_UART_IN  0011:
                                                                                              * 0100:BT_AOX_1       0101: SPI_0_MISO
                                                                                              * 0110:  UART3_IN      0111: BT_UART_IN
                                                                                              * 1000: SPI_2_MISO   1001: SPI_2_MISO
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO2_PINMUX_SEL_030(x)      ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO2_PINMUX_SEL_030(x)  ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOE_4_5_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO5_SLEW_RATE_034      ((u32)0x00000001 << 28)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO5_DRIVING_034       ((u32)0x00000003 << 26)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO5_DRIVING_034(x)         ((u32)(((x) & 0x00000003) << 26))
#define CORE_PAD_GET_SYSON_GPIO5_DRIVING_034(x)     ((u32)(((x >> 26) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO5_SMT_EN_034         ((u32)0x00000001 << 25)                  /*!< R/W   0.0 Enable SYSON GPIO5 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO5_PULL_CTRL_1_0_034 ((u32)0x00000003 << 22)                  /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO5_PULL_CTRL_1_0_034(x)   ((u32)(((x) & 0x00000003) << 22))
#define CORE_PAD_GET_SYSON_GPIO5_PULL_CTRL_1_0_034(x) ((u32)(((x >> 22) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO5_PINMUX_SEL_034    ((u32)0x0000000F << 16)                  /*!< R/W   1111.0
                                                                                              * 0000:TRXQP        0001: I2C2_SCL
                                                                                              * 0010:                        0011:
                                                                                              * 0100:                         0101:SPI_0_CS1
                                                                                              * 0110:                         0111: 
                                                                                              * 1000:                         1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO5_PINMUX_SEL_034(x)      ((u32)(((x) & 0x0000000F) << 16))
#define CORE_PAD_GET_SYSON_GPIO5_PINMUX_SEL_034(x)  ((u32)(((x >> 16) & 0x0000000F)))
#define CORE_PAD_BIT_SYSON_GPIO4_SLEW_RATE_034      ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO4_DRIVING_034       ((u32)0x00000003 << 10)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO4_DRIVING_034(x)         ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO4_DRIVING_034(x)     ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO4_SMT_EN_034         ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO4 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO4_PULL_CTRL_1_0_034 ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO4_PULL_CTRL_1_0_034(x)   ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO4_PULL_CTRL_1_0_034(x) ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO4_PINMUX_SEL_034    ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000: TRXIN             0001: I2C2_SDA 
                                                                                              * 0010:                          0011:RFE_CTRL_5
                                                                                              * 0100:BT_AOX_3     0101:SPI_0_CS0
                                                                                              * 0110: UART3_CTS     0111:BT_UART_CTS
                                                                                              * 1000:SPI_2_CS      1001: BTCMC_I2C_DD
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO4_PINMUX_SEL_034(x)      ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO4_PINMUX_SEL_034(x)  ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOE_6_7_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO7_SLEW_RATE_038      ((u32)0x00000001 << 28)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO7_DRIVING_038       ((u32)0x00000003 << 26)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO7_DRIVING_038(x)         ((u32)(((x) & 0x00000003) << 26))
#define CORE_PAD_GET_SYSON_GPIO7_DRIVING_038(x)     ((u32)(((x >> 26) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO7_SMT_EN_038         ((u32)0x00000001 << 25)                  /*!< R/W   0.0 Enable SYSON GPIO7 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO7_PULL_CTRL_1_0_038 ((u32)0x00000003 << 22)                  /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO7_PULL_CTRL_1_0_038(x)   ((u32)(((x) & 0x00000003) << 22))
#define CORE_PAD_GET_SYSON_GPIO7_PULL_CTRL_1_0_038(x) ((u32)(((x >> 22) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO7_PINMUX_SEL_038    ((u32)0x0000000F << 16)                  /*!< R/W   1111.0
                                                                                              * 0000: TRXIP            0001: I2C2_SCL
                                                                                              * 0010:                          0011: RFE_CTRL_4
                                                                                              * 0100:BT_AOX_2        0101: SPI_0_MOSI
                                                                                              * 0110: UART3_RTS   0111: BT_UART_RTS
                                                                                              * 1000: SPI_2_MOSI   1001:  BTCMD_I2C_CK
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO7_PINMUX_SEL_038(x)      ((u32)(((x) & 0x0000000F) << 16))
#define CORE_PAD_GET_SYSON_GPIO7_PINMUX_SEL_038(x)  ((u32)(((x >> 16) & 0x0000000F)))
#define CORE_PAD_BIT_SYSON_GPIO6_SLEW_RATE_038      ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO6_DRIVING_038       ((u32)0x00000003 << 10)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO6_DRIVING_038(x)         ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO6_DRIVING_038(x)     ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO6_SMT_EN_038         ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO6 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO6_PULL_CTRL_1_0_038 ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO6_PULL_CTRL_1_0_038(x)   ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO6_PULL_CTRL_1_0_038(x) ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO6_PINMUX_SEL_038    ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000:TRXQN           0001: I2C2_SDA
                                                                                              * 0010:                           0011:
                                                                                              * 0100:                             0101:SPI_0_CS2
                                                                                              * 0110:                            0111: 
                                                                                              * 1000:                          1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO6_PINMUX_SEL_038(x)      ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO6_PINMUX_SEL_038(x)  ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOE_8_9_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO9_SLEW_RATE_03C      ((u32)0x00000001 << 28)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO9_DRIVING_03C       ((u32)0x00000003 << 26)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO9_DRIVING_03C(x)         ((u32)(((x) & 0x00000003) << 26))
#define CORE_PAD_GET_SYSON_GPIO9_DRIVING_03C(x)     ((u32)(((x >> 26) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO9_SMT_EN_03C         ((u32)0x00000001 << 25)                  /*!< R/W   0.0 Enable SYSON GPIO9 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO9_PULL_CTRL_1_0_03C ((u32)0x00000003 << 22)                  /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO9_PULL_CTRL_1_0_03C(x)   ((u32)(((x) & 0x00000003) << 22))
#define CORE_PAD_GET_SYSON_GPIO9_PULL_CTRL_1_0_03C(x) ((u32)(((x >> 22) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO9_PINMUX_SEL_03C    ((u32)0x0000000F << 16)                  /*!< R/W   1111.0
                                                                                              * 0000:TRXQP        0001: I2C2_SCL
                                                                                              * 0010:                        0011:
                                                                                              * 0100:                         0101:SPI_0_CS1
                                                                                              * 0110:                         0111: 
                                                                                              * 1000:                         1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO9_PINMUX_SEL_03C(x)      ((u32)(((x) & 0x0000000F) << 16))
#define CORE_PAD_GET_SYSON_GPIO9_PINMUX_SEL_03C(x)  ((u32)(((x >> 16) & 0x0000000F)))
#define CORE_PAD_BIT_SYSON_GPIO8_SLEW_RATE_03C      ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO8_DRIVING_03C       ((u32)0x00000003 << 10)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO8_DRIVING_03C(x)         ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO8_DRIVING_03C(x)     ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO8_SMT_EN_03C         ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO8 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO8_PULL_CTRL_1_0_03C ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO8_PULL_CTRL_1_0_03C(x)   ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO8_PULL_CTRL_1_0_03C(x) ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO8_PINMUX_SEL_03C    ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000: TRXIN             0001: I2C2_SDA 
                                                                                              * 0010:                          0011:RFE_CTRL_5
                                                                                              * 0100:BT_AOX_3     0101:SPI_0_CS0
                                                                                              * 0110: UART3_CTS     0111:BT_UART_CTS
                                                                                              * 1000:SPI_2_CS      1001: BTCMC_I2C_DD
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO8_PINMUX_SEL_03C(x)      ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO8_PINMUX_SEL_03C(x)  ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOE_10_11_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO11_SLEW_RATE         ((u32)0x00000001 << 28)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO11_DRIVING          ((u32)0x00000003 << 26)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO11_DRIVING(x)            ((u32)(((x) & 0x00000003) << 26))
#define CORE_PAD_GET_SYSON_GPIO11_DRIVING(x)        ((u32)(((x >> 26) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO11_SMT_EN            ((u32)0x00000001 << 25)                  /*!< R/W   0.0 Enable SYSON GPIO11 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO11_PULL_CTRL_1_0    ((u32)0x00000003 << 22)                  /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO11_PULL_CTRL_1_0(x)      ((u32)(((x) & 0x00000003) << 22))
#define CORE_PAD_GET_SYSON_GPIO11_PULL_CTRL_1_0(x)  ((u32)(((x >> 22) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO11_PINMUX_SEL       ((u32)0x0000000F << 16)                  /*!< R/W   1111.0
                                                                                              * 0000: TRXIP            0001: I2C2_SCL
                                                                                              * 0010:                          0011: RFE_CTRL_4
                                                                                              * 0100:BT_AOX_2        0101: SPI_0_MOSI
                                                                                              * 0110: UART3_RTS   0111: BT_UART_RTS
                                                                                              * 1000: SPI_2_MOSI   1001:  BTCMD_I2C_CK
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO11_PINMUX_SEL(x)         ((u32)(((x) & 0x0000000F) << 16))
#define CORE_PAD_GET_SYSON_GPIO11_PINMUX_SEL(x)     ((u32)(((x >> 16) & 0x0000000F)))
#define CORE_PAD_BIT_SYSON_GPIO10_SLEW_RATE_040     ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO10_DRIVING_040      ((u32)0x00000003 << 10)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO10_DRIVING_040(x)        ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO10_DRIVING_040(x)    ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO10_SMT_EN_040        ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO10 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO10_PULL_CTRL_1_0_040 ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO10_PULL_CTRL_1_0_040(x)  ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO10_PULL_CTRL_1_0_040(x) ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO10_PINMUX_SEL_040   ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000:TRXQN           0001: I2C2_SDA
                                                                                              * 0010:                           0011:
                                                                                              * 0100:                             0101:SPI_0_CS2
                                                                                              * 0110:                            0111: 
                                                                                              * 1000:                          1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO10_PINMUX_SEL_040(x)     ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO10_PINMUX_SEL_040(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOE_12_13_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO13_SLEW_RATE         ((u32)0x00000001 << 28)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO13_DRIVING          ((u32)0x00000003 << 26)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO13_DRIVING(x)            ((u32)(((x) & 0x00000003) << 26))
#define CORE_PAD_GET_SYSON_GPIO13_DRIVING(x)        ((u32)(((x >> 26) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO13_SMT_EN            ((u32)0x00000001 << 25)                  /*!< R/W   0.0 Enable SYSON GPIO13 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO13_PULL_CTRL_1_0    ((u32)0x00000003 << 22)                  /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO13_PULL_CTRL_1_0(x)      ((u32)(((x) & 0x00000003) << 22))
#define CORE_PAD_GET_SYSON_GPIO13_PULL_CTRL_1_0(x)  ((u32)(((x >> 22) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO13_PINMUX_SEL       ((u32)0x0000000F << 16)                  /*!< R/W   1111.0
                                                                                              * 0000:TRXQP        0001: I2C2_SCL
                                                                                              * 0010:                        0011:
                                                                                              * 0100:                         0101:SPI_0_CS1
                                                                                              * 0110:                         0111: 
                                                                                              * 1000:                         1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO13_PINMUX_SEL(x)         ((u32)(((x) & 0x0000000F) << 16))
#define CORE_PAD_GET_SYSON_GPIO13_PINMUX_SEL(x)     ((u32)(((x >> 16) & 0x0000000F)))
#define CORE_PAD_BIT_SYSON_GPIO12_SLEW_RATE         ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO12_DRIVING          ((u32)0x00000003 << 10)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO12_DRIVING(x)            ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO12_DRIVING(x)        ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO12_SMT_EN            ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO12 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO12_PULL_CTRL_1_0    ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO12_PULL_CTRL_1_0(x)      ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO12_PULL_CTRL_1_0(x)  ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO12_PINMUX_SEL       ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000: TRXIN             0001: I2C2_SDA 
                                                                                              * 0010:                          0011:RFE_CTRL_5
                                                                                              * 0100:BT_AOX_3     0101:SPI_0_CS0
                                                                                              * 0110: UART3_CTS     0111:BT_UART_CTS
                                                                                              * 1000:SPI_2_CS      1001: BTCMC_I2C_DD
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO12_PINMUX_SEL(x)         ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO12_PINMUX_SEL(x)     ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOE_14_15_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO15_SLEW_RATE         ((u32)0x00000001 << 28)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO15_DRIVING          ((u32)0x00000003 << 26)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO15_DRIVING(x)            ((u32)(((x) & 0x00000003) << 26))
#define CORE_PAD_GET_SYSON_GPIO15_DRIVING(x)        ((u32)(((x >> 26) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO15_SMT_EN            ((u32)0x00000001 << 25)                  /*!< R/W   0.0 Enable SYSON GPIO15 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO15_PULL_CTRL_1_0    ((u32)0x00000003 << 22)                  /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO15_PULL_CTRL_1_0(x)      ((u32)(((x) & 0x00000003) << 22))
#define CORE_PAD_GET_SYSON_GPIO15_PULL_CTRL_1_0(x)  ((u32)(((x >> 22) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO15_PINMUX_SEL       ((u32)0x0000000F << 16)                  /*!< R/W   1111.0
                                                                                              * 0000: TRXIP            0001: I2C2_SCL
                                                                                              * 0010:                          0011: RFE_CTRL_4
                                                                                              * 0100:BT_AOX_2        0101: SPI_0_MOSI
                                                                                              * 0110: UART3_RTS   0111: BT_UART_RTS
                                                                                              * 1000: SPI_2_MOSI   1001:  BTCMD_I2C_CK
                                                                                              * 1010:                            1011: 
                                                                                              * 1100:                            1101: 
                                                                                              * 1110:                            1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO15_PINMUX_SEL(x)         ((u32)(((x) & 0x0000000F) << 16))
#define CORE_PAD_GET_SYSON_GPIO15_PINMUX_SEL(x)     ((u32)(((x >> 16) & 0x0000000F)))
#define CORE_PAD_BIT_SYSON_GPIO14_SLEW_RATE         ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO14_DRIVING          ((u32)0x00000003 << 10)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO14_DRIVING(x)            ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO14_DRIVING(x)        ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO14_SMT_EN            ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO14 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO14_PULL_CTRL_1_0    ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO14_PULL_CTRL_1_0(x)      ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO14_PULL_CTRL_1_0(x)  ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO14_PINMUX_SEL       ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000:TRXQN           0001: I2C2_SDA
                                                                                              * 0010:                           0011:
                                                                                              * 0100:                             0101:SPI_0_CS2
                                                                                              * 0110:                            0111: 
                                                                                              * 1000:                          1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO14_PINMUX_SEL(x)         ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO14_PINMUX_SEL(x)     ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOE_16_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO16_SLEW_RATE         ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO16_DRIVING          ((u32)0x00000003 << 10)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO16_DRIVING(x)            ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO16_DRIVING(x)        ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO16_SMT_EN            ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO16 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO16_PULL_CTRL_1_0    ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO16_PULL_CTRL_1_0(x)      ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO16_PULL_CTRL_1_0(x)  ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO16_PINMUX_SEL       ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000: TRXIN             0001: I2C2_SDA 
                                                                                              * 0010:                          0011:RFE_CTRL_5
                                                                                              * 0100:BT_AOX_3     0101:SPI_0_CS0
                                                                                              * 0110: UART3_CTS     0111:BT_UART_CTS
                                                                                              * 1000:SPI_2_CS      1001: BTCMC_I2C_DD
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO16_PINMUX_SEL(x)         ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO16_PINMUX_SEL(x)     ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_DUMMY0
 * @brief
 * @{
 */
#define CORE_PAD_MASK_DUMMY                         ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 None */
#define CORE_PAD_DUMMY(x)                           ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_PAD_GET_DUMMY(x)                       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYSON_DUMMY1
 * @brief
 * @{
 */
#define CORE_PAD_MASK_DUMMY_054                     ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 None */
#define CORE_PAD_DUMMY_054(x)                       ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_PAD_GET_DUMMY_054(x)                   ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYSON_DUMMY2
 * @brief
 * @{
 */
#define CORE_PAD_MASK_DUMMY_058                     ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 None */
#define CORE_PAD_DUMMY_058(x)                       ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_PAD_GET_DUMMY_058(x)                   ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYSON_DUMMY3
 * @brief
 * @{
 */
#define CORE_PAD_MASK_DUMMY_05C                     ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 None */
#define CORE_PAD_DUMMY_05C(x)                       ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_PAD_GET_DUMMY_05C(x)                   ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYSON_DUMMY4
 * @brief
 * @{
 */
#define CORE_PAD_MASK_DUMMY_060                     ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 None */
#define CORE_PAD_DUMMY_060(x)                       ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_PAD_GET_DUMMY_060(x)                   ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYSON_DUMMY5
 * @brief
 * @{
 */
#define CORE_PAD_MASK_DUMMY_064                     ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 None */
#define CORE_PAD_DUMMY_064(x)                       ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_PAD_GET_DUMMY_064(x)                   ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYSON_GPIOS_0_1_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO1_SLEW_RATE_068      ((u32)0x00000001 << 28)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO1_DRIVING_068       ((u32)0x00000003 << 26)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO1_DRIVING_068(x)         ((u32)(((x) & 0x00000003) << 26))
#define CORE_PAD_GET_SYSON_GPIO1_DRIVING_068(x)     ((u32)(((x >> 26) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO1_SMT_EN_068         ((u32)0x00000001 << 25)                  /*!< R/W   0.0 Enable SYSON GPIO1 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO1_PULL_CTRL_1_0_068 ((u32)0x00000003 << 22)                  /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO1_PULL_CTRL_1_0_068(x)   ((u32)(((x) & 0x00000003) << 22))
#define CORE_PAD_GET_SYSON_GPIO1_PULL_CTRL_1_0_068(x) ((u32)(((x >> 22) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO1_PINMUX_SEL_068    ((u32)0x0000000F << 16)                  /*!< R/W   1111.0
                                                                                              * 0000: SD_D1            0001: 
                                                                                              * 0010:  PWM8             0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                          0111: 
                                                                                              * 1000:                          1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO1_PINMUX_SEL_068(x)      ((u32)(((x) & 0x0000000F) << 16))
#define CORE_PAD_GET_SYSON_GPIO1_PINMUX_SEL_068(x)  ((u32)(((x >> 16) & 0x0000000F)))
#define CORE_PAD_BIT_SYSON_GPIO0_SLEW_RATE_068      ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO0_DRIVING_068       ((u32)0x00000003 << 10)                  /*!< R/W   0.0 0:8mA, 1:16mA */
#define CORE_PAD_SYSON_GPIO0_DRIVING_068(x)         ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO0_DRIVING_068(x)     ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO0_SMT_EN_068         ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO0 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO0_PULL_CTRL_1_0_068 ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO0_PULL_CTRL_1_0_068(x)   ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO0_PULL_CTRL_1_0_068(x) ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO0_PINMUX_SEL_068    ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000: SD_CLK          0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                          0111: 
                                                                                              * 1000:                          1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO0_PINMUX_SEL_068(x)      ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO0_PINMUX_SEL_068(x)  ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOS_2_3_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO3_SLEW_RATE_06C      ((u32)0x00000001 << 28)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO3_DRIVING_06C       ((u32)0x00000003 << 26)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO3_DRIVING_06C(x)         ((u32)(((x) & 0x00000003) << 26))
#define CORE_PAD_GET_SYSON_GPIO3_DRIVING_06C(x)     ((u32)(((x >> 26) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO3_SMT_EN_06C         ((u32)0x00000001 << 25)                  /*!< R/W   0.0 Enable SYSON GPIO3 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO3_PULL_CTRL_1_0_06C ((u32)0x00000003 << 22)                  /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO3_PULL_CTRL_1_0_06C(x)   ((u32)(((x) & 0x00000003) << 22))
#define CORE_PAD_GET_SYSON_GPIO3_PULL_CTRL_1_0_06C(x) ((u32)(((x >> 22) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO3_PINMUX_SEL_06C    ((u32)0x0000000F << 16)                  /*!< R/W   1111.0
                                                                                              * 0000: SD_CMD         0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                          0111: 
                                                                                              * 1000:                          1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO3_PINMUX_SEL_06C(x)      ((u32)(((x) & 0x0000000F) << 16))
#define CORE_PAD_GET_SYSON_GPIO3_PINMUX_SEL_06C(x)  ((u32)(((x >> 16) & 0x0000000F)))
#define CORE_PAD_BIT_SYSON_GPIO2_SLEW_RATE_06C      ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO2_DRIVING_06C       ((u32)0x00000003 << 10)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO2_DRIVING_06C(x)         ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO2_DRIVING_06C(x)     ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO2_SMT_EN_06C         ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO2 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO2_PULL_CTRL_1_0_06C ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO2_PULL_CTRL_1_0_06C(x)   ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO2_PULL_CTRL_1_0_06C(x) ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO2_PINMUX_SEL_06C    ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000: SD_D0            0001: 
                                                                                              * 0010:                          0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                          0111: 
                                                                                              * 1000:                          1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO2_PINMUX_SEL_06C(x)      ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO2_PINMUX_SEL_06C(x)  ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOS_4_5_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO5_SLEW_RATE_070      ((u32)0x00000001 << 28)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO5_DRIVING_070       ((u32)0x00000003 << 26)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO5_DRIVING_070(x)         ((u32)(((x) & 0x00000003) << 26))
#define CORE_PAD_GET_SYSON_GPIO5_DRIVING_070(x)     ((u32)(((x >> 26) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO5_SMT_EN_070         ((u32)0x00000001 << 25)                  /*!< R/W   0.0 Enable SYSON GPIO5 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO5_PULL_CTRL_1_0_070 ((u32)0x00000003 << 22)                  /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO5_PULL_CTRL_1_0_070(x)   ((u32)(((x) & 0x00000003) << 22))
#define CORE_PAD_GET_SYSON_GPIO5_PULL_CTRL_1_0_070(x) ((u32)(((x >> 22) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO5_PINMUX_SEL_070    ((u32)0x0000000F << 16)                  /*!< R/W   1111.0
                                                                                              * 0000: SD_D3            0001: 
                                                                                              * 0010:  PWM10           0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                          0111: 
                                                                                              * 1000:                          1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO5_PINMUX_SEL_070(x)      ((u32)(((x) & 0x0000000F) << 16))
#define CORE_PAD_GET_SYSON_GPIO5_PINMUX_SEL_070(x)  ((u32)(((x >> 16) & 0x0000000F)))
#define CORE_PAD_BIT_SYSON_GPIO4_SLEW_RATE_070      ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO4_DRIVING_070       ((u32)0x00000003 << 10)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO4_DRIVING_070(x)         ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO4_DRIVING_070(x)     ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO4_SMT_EN_070         ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO4 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO4_PULL_CTRL_1_0_070 ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO4_PULL_CTRL_1_0_070(x)   ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO4_PULL_CTRL_1_0_070(x) ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO4_PINMUX_SEL_070    ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000: SD_CD           0001: 
                                                                                              * 0010:PWM9               0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                          0111: 
                                                                                              * 1000:                          1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO4_PINMUX_SEL_070(x)      ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO4_PINMUX_SEL_070(x)  ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYSON_GPIOS_6_CTRL
 * @brief
 * @{
 */
#define CORE_PAD_BIT_SYSON_GPIO6_SLEW_RATE_074      ((u32)0x00000001 << 12)                  /*!< R/W   0.0 0:Disable,1:Enable */
#define CORE_PAD_MASK_SYSON_GPIO6_DRIVING_074       ((u32)0x00000003 << 10)                  /*!< R/W   0.0 0:4mA, 1:8mA */
#define CORE_PAD_SYSON_GPIO6_DRIVING_074(x)         ((u32)(((x) & 0x00000003) << 10))
#define CORE_PAD_GET_SYSON_GPIO6_DRIVING_074(x)     ((u32)(((x >> 10) & 0x00000003)))
#define CORE_PAD_BIT_SYSON_GPIO6_SMT_EN_074         ((u32)0x00000001 << 9)                   /*!< R/W   0.0 Enable SYSON GPIO6 Schmitt trigger;  1: enable */
#define CORE_PAD_MASK_SYSON_GPIO6_PULL_CTRL_1_0_074 ((u32)0x00000003 << 6)                   /*!< R/W   0.0 2b'00: high impedence; 2b'01: pull low; 2b'10: pull high; 2b'11: reserved */
#define CORE_PAD_SYSON_GPIO6_PULL_CTRL_1_0_074(x)   ((u32)(((x) & 0x00000003) << 6))
#define CORE_PAD_GET_SYSON_GPIO6_PULL_CTRL_1_0_074(x) ((u32)(((x >> 6) & 0x00000003)))
#define CORE_PAD_MASK_SYSON_GPIO6_PINMUX_SEL_074    ((u32)0x0000000F << 0)                   /*!< R/W   1111.0
                                                                                              * 0000: SD_D2            0001: 
                                                                                              * 0010: PWM11            0011:
                                                                                              * 0100:                          0101:
                                                                                              * 0110:                          0111: 
                                                                                              * 1000:                          1001: 
                                                                                              * 1010:                          1011: 
                                                                                              * 1100:                          1101: 
                                                                                              * 1110:                          1111: GPIO
                                                                                              */
#define CORE_PAD_SYSON_GPIO6_PINMUX_SEL_074(x)      ((u32)(((x) & 0x0000000F) << 0))
#define CORE_PAD_GET_SYSON_GPIO6_PINMUX_SEL_074(x)  ((u32)(((x >> 0) & 0x0000000F)))
/** @} */




/** @defgroup CORE_PAD_Exported_Types CORE_PAD Exported Types
* @{
*/
/**
* @brief CORE_PAD Register Declaration
*/

typedef struct {
	__IO uint32_t SYSON_GPIOC_0_1_CTRL;                         /*!< Register,  Address offset:0x0000 */
	__IO uint32_t SYSON_GPIOC_2_3_CTRL;                         /*!< Register,  Address offset:0x0004 */
	__IO uint32_t SYSON_GPIOC_4_5_CTRL;                         /*!< Register,  Address offset:0x0008 */
	__IO uint32_t SYSON_GPIOC_6_7_CTRL;                         /*!< Register,  Address offset:0x000C */
	__IO uint32_t SYSON_GPIOC_8_9_CTRL;                         /*!< Register,  Address offset:0x0010 */
	__IO uint32_t SYSON_GPIOC_10_CTRL;                          /*!< Register,  Address offset:0x0014 */
	__IO uint32_t SYSON_GPIOD_0_1_CTRL;                         /*!< Register,  Address offset:0x0018 */
	__IO uint32_t SYSON_GPIOD_2_3_CTRL;                         /*!< Register,  Address offset:0x001C */
	__IO uint32_t SYSON_GPIOD_4_5_CTRL;                         /*!< Register,  Address offset:0x0020 */
	__IO uint32_t SYSON_GPIOD_6_7_CTRL;                         /*!< Register,  Address offset:0x0024 */
	__IO uint32_t SYSON_GPIOD_8_9_CTRL;                         /*!< Register,  Address offset:0x0028 */
	__IO uint32_t SYSON_GPIOE_0_1_CTRL;                         /*!< Register,  Address offset:0x002C */
	__IO uint32_t SYSON_GPIOE_2_3_CTRL;                         /*!< Register,  Address offset:0x0030 */
	__IO uint32_t SYSON_GPIOE_4_5_CTRL;                         /*!< Register,  Address offset:0x0034 */
	__IO uint32_t SYSON_GPIOE_6_7_CTRL;                         /*!< Register,  Address offset:0x0038 */
	__IO uint32_t SYSON_GPIOE_8_9_CTRL;                         /*!< Register,  Address offset:0x003C */
	__IO uint32_t SYSON_GPIOE_10_11_CTRL;                       /*!< Register,  Address offset:0x0040 */
	__IO uint32_t SYSON_GPIOE_12_13_CTRL;                       /*!< Register,  Address offset:0x0044 */
	__IO uint32_t SYSON_GPIOE_14_15_CTRL;                       /*!< Register,  Address offset:0x0048 */
	__IO uint32_t SYSON_GPIOE_16_CTRL;                          /*!< Register,  Address offset:0x004C */
	__IO uint32_t SYSON_DUMMY0;                                 /*!< Register,  Address offset:0x0050 */
	__IO uint32_t SYSON_DUMMY1;                                 /*!< Register,  Address offset:0x0054 */
	__IO uint32_t SYSON_DUMMY2;                                 /*!< Register,  Address offset:0x0058 */
	__IO uint32_t SYSON_DUMMY3;                                 /*!< Register,  Address offset:0x005C */
	__IO uint32_t SYSON_DUMMY4;                                 /*!< Register,  Address offset:0x0060 */
	__IO uint32_t SYSON_DUMMY5;                                 /*!< Register,  Address offset:0x0064 */
	__IO uint32_t SYSON_GPIOS_0_1_CTRL;                         /*!< Register,  Address offset:0x0068 */
	__IO uint32_t SYSON_GPIOS_2_3_CTRL;                         /*!< Register,  Address offset:0x006C */
	__IO uint32_t SYSON_GPIOS_4_5_CTRL;                         /*!< Register,  Address offset:0x0070 */
	__IO uint32_t SYSON_GPIOS_6_CTRL;                           /*!< Register,  Address offset:0x0074 */
} CORE_PAD_TypeDef;

#define CORE_PAD_BASE ((CORE_PAD_TypeDef *) (0x6020C400))
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
#endif  // SYSREG_CORE_PAD_H
