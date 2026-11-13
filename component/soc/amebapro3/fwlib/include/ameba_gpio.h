/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AMEBA_GPIO_H
#define AMEBA_GPIO_H

/* AUTO_GEN_START */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup GPIO
* @brief GPIO driver modules
* @{
*/


/** @defgroup REG_GPIO_IT_STS
 * @brief
 * @{
 */
#define GPIO_MASK_IT_STS                            ((u32)0x0000FFFF << 0)                   /*!< R     0.0
                                                                                              * 0: The specific GPIO port is configured to edge sensitive interrupt mode.
                                                                                              * 1: The specific GPIO port is configured to level sensitive interrupt mode.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin. For AON GPIO, only Bit [5:0] are active, as there are only 6 AON GPIO pins.
                                                                                              */
#define GPIO_IT_STS(x)                              ((u32)(((x) & 0x0000FFFF) << 0))
#define GPIO_GET_IT_STS(x)                          ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_GPIO_EI_EN
 * @brief
 * @{
 */
#define GPIO_MASK_EI_EN                             ((u32)0x0000FFFF << 0)                   /*!< WO    0.0
                                                                                              * 0: No operation.
                                                                                              * 1: The controlled GPIO port is configured to edge sensitive interrupt mode and the specific bit of REG_GPIO_IT_STS is ZERO.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin. For AON GPIO, only Bit [5:0] are active, as there are only 6 AON GPIO pins.
                                                                                              */
#define GPIO_EI_EN(x)                               ((u32)(((x) & 0x0000FFFF) << 0))
#define GPIO_GET_EI_EN(x)                           ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_GPIO_LI_EN
 * @brief
 * @{
 */
#define GPIO_MASK_LI_EN                             ((u32)0x0000FFFF << 0)                   /*!< WO    0.0
                                                                                              * 0: No operation.
                                                                                              * 1: The controlled GPIO port is configured to level sensitive interrupt mode and the specific bit of REG_GPIO_IT_STS is ONE.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin. For AON GPIO, only Bit [5:0] are active, as there are only 6 AON GPIO pins.
                                                                                              */
#define GPIO_LI_EN(x)                               ((u32)(((x) & 0x0000FFFF) << 0))
#define GPIO_GET_LI_EN(x)                           ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_GPIO_IP_STS
 * @brief
 * @{
 */
#define GPIO_MASK_GPIO15_IP_STS                     ((u32)0x00000003 << 30)                  /*!< R     0.0
                                                                                              * 00: The specific GPIO port is configured to rising-edge (active-high sensitive)
                                                                                              * 01: The specific GPIO port is configured to falling-edge (active-low sensitive)
                                                                                              * 10: The specific GPIO port is configured to dual-edge
                                                                                              * 11: rsvd
                                                                                              * This is RSVD (Reserved) for AON GPIO.
                                                                                              */
#define GPIO_GPIO15_IP_STS(x)                       ((u32)(((x) & 0x00000003) << 30))
#define GPIO_GET_GPIO15_IP_STS(x)                   ((u32)(((x >> 30) & 0x00000003)))
#define GPIO_MASK_GPIO14_IP_STS                     ((u32)0x00000003 << 28)                  /*!< R     0.0
                                                                                              * 00: The specific GPIO port is configured to rising-edge (active-high sensitive)
                                                                                              * 01: The specific GPIO port is configured to falling-edge (active-low sensitive)
                                                                                              * 10: The specific GPIO port is configured to dual-edge
                                                                                              * 11: rsvd
                                                                                              * This is RSVD (Reserved) for AON GPIO.
                                                                                              */
#define GPIO_GPIO14_IP_STS(x)                       ((u32)(((x) & 0x00000003) << 28))
#define GPIO_GET_GPIO14_IP_STS(x)                   ((u32)(((x >> 28) & 0x00000003)))
#define GPIO_MASK_GPIO13_IP_STS                     ((u32)0x00000003 << 26)                  /*!< R     0.0
                                                                                              * 00: The specific GPIO port is configured to rising-edge (active-high sensitive)
                                                                                              * 01: The specific GPIO port is configured to falling-edge (active-low sensitive)
                                                                                              * 10: The specific GPIO port is configured to dual-edge
                                                                                              * 11: rsvd
                                                                                              * This is RSVD (Reserved) for AON GPIO.
                                                                                              */
#define GPIO_GPIO13_IP_STS(x)                       ((u32)(((x) & 0x00000003) << 26))
#define GPIO_GET_GPIO13_IP_STS(x)                   ((u32)(((x >> 26) & 0x00000003)))
#define GPIO_MASK_GPIO12_IP_STS                     ((u32)0x00000003 << 24)                  /*!< R     0.0
                                                                                              * 00: The specific GPIO port is configured to rising-edge (active-high sensitive)
                                                                                              * 01: The specific GPIO port is configured to falling-edge (active-low sensitive)
                                                                                              * 10: The specific GPIO port is configured to dual-edge
                                                                                              * 11: rsvd
                                                                                              * This is RSVD (Reserved) for AON GPIO.
                                                                                              */
#define GPIO_GPIO12_IP_STS(x)                       ((u32)(((x) & 0x00000003) << 24))
#define GPIO_GET_GPIO12_IP_STS(x)                   ((u32)(((x >> 24) & 0x00000003)))
#define GPIO_MASK_GPIO11_IP_STS                     ((u32)0x00000003 << 22)                  /*!< R     0.0
                                                                                              * 00: The specific GPIO port is configured to rising-edge (active-high sensitive)
                                                                                              * 01: The specific GPIO port is configured to falling-edge (active-low sensitive)
                                                                                              * 10: The specific GPIO port is configured to dual-edge
                                                                                              * 11: rsvd
                                                                                              * This is RSVD (Reserved) for AON GPIO.
                                                                                              */
#define GPIO_GPIO11_IP_STS(x)                       ((u32)(((x) & 0x00000003) << 22))
#define GPIO_GET_GPIO11_IP_STS(x)                   ((u32)(((x >> 22) & 0x00000003)))
#define GPIO_MASK_GPIO10_IP_STS                     ((u32)0x00000003 << 20)                  /*!< R     0.0
                                                                                              * 00: The specific GPIO port is configured to rising-edge (active-high sensitive)
                                                                                              * 01: The specific GPIO port is configured to falling-edge (active-low sensitive)
                                                                                              * 10: The specific GPIO port is configured to dual-edge
                                                                                              * 11: rsvd
                                                                                              * This is RSVD (Reserved) for AON GPIO.
                                                                                              */
#define GPIO_GPIO10_IP_STS(x)                       ((u32)(((x) & 0x00000003) << 20))
#define GPIO_GET_GPIO10_IP_STS(x)                   ((u32)(((x >> 20) & 0x00000003)))
#define GPIO_MASK_GPIO9_IP_STS                      ((u32)0x00000003 << 18)                  /*!< R     0.0
                                                                                              * 00: The specific GPIO port is configured to rising-edge (active-high sensitive)
                                                                                              * 01: The specific GPIO port is configured to falling-edge (active-low sensitive)
                                                                                              * 10: The specific GPIO port is configured to dual-edge
                                                                                              * 11: rsvd
                                                                                              * This is RSVD (Reserved) for AON GPIO.
                                                                                              */
#define GPIO_GPIO9_IP_STS(x)                        ((u32)(((x) & 0x00000003) << 18))
#define GPIO_GET_GPIO9_IP_STS(x)                    ((u32)(((x >> 18) & 0x00000003)))
#define GPIO_MASK_GPIO8_IP_STS                      ((u32)0x00000003 << 16)                  /*!< R     0.0
                                                                                              * 00: The specific GPIO port is configured to rising-edge (active-high sensitive)
                                                                                              * 01: The specific GPIO port is configured to falling-edge (active-low sensitive)
                                                                                              * 10: The specific GPIO port is configured to dual-edge
                                                                                              * 11: rsvd
                                                                                              * This is RSVD (Reserved) for AON GPIO.
                                                                                              */
#define GPIO_GPIO8_IP_STS(x)                        ((u32)(((x) & 0x00000003) << 16))
#define GPIO_GET_GPIO8_IP_STS(x)                    ((u32)(((x >> 16) & 0x00000003)))
#define GPIO_MASK_GPIO7_IP_STS                      ((u32)0x00000003 << 14)                  /*!< R     0.0
                                                                                              * 00: The specific GPIO port is configured to rising-edge (active-high sensitive)
                                                                                              * 01: The specific GPIO port is configured to falling-edge (active-low sensitive)
                                                                                              * 10: The specific GPIO port is configured to dual-edge
                                                                                              * 11: rsvd
                                                                                              * This is RSVD (Reserved) for AON GPIO.
                                                                                              */
#define GPIO_GPIO7_IP_STS(x)                        ((u32)(((x) & 0x00000003) << 14))
#define GPIO_GET_GPIO7_IP_STS(x)                    ((u32)(((x >> 14) & 0x00000003)))
#define GPIO_MASK_GPIO6_IP_STS                      ((u32)0x00000003 << 12)                  /*!< R     0.0
                                                                                              * 00: The specific GPIO port is configured to rising-edge (active-high sensitive)
                                                                                              * 01: The specific GPIO port is configured to falling-edge (active-low sensitive)
                                                                                              * 10: The specific GPIO port is configured to dual-edge
                                                                                              * 11: rsvd
                                                                                              * This is RSVD (Reserved) for AON GPIO.
                                                                                              */
#define GPIO_GPIO6_IP_STS(x)                        ((u32)(((x) & 0x00000003) << 12))
#define GPIO_GET_GPIO6_IP_STS(x)                    ((u32)(((x >> 12) & 0x00000003)))
#define GPIO_MASK_GPIO5_IP_STS                      ((u32)0x00000003 << 10)                  /*!< R     0.0
                                                                                              * 00: The specific GPIO port is configured to rising-edge (active-high sensitive)
                                                                                              * 01: The specific GPIO port is configured to falling-edge (active-low sensitive)
                                                                                              * 10: The specific GPIO port is configured to dual-edge
                                                                                              * 11: rsvd
                                                                                              */
#define GPIO_GPIO5_IP_STS(x)                        ((u32)(((x) & 0x00000003) << 10))
#define GPIO_GET_GPIO5_IP_STS(x)                    ((u32)(((x >> 10) & 0x00000003)))
#define GPIO_MASK_GPIO4_IP_STS                      ((u32)0x00000003 << 8)                   /*!< R     0.0
                                                                                              * 00: The specific GPIO port is configured to rising-edge (active-high sensitive)
                                                                                              * 01: The specific GPIO port is configured to falling-edge (active-low sensitive)
                                                                                              * 10: The specific GPIO port is configured to dual-edge
                                                                                              * 11: rsvd
                                                                                              */
#define GPIO_GPIO4_IP_STS(x)                        ((u32)(((x) & 0x00000003) << 8))
#define GPIO_GET_GPIO4_IP_STS(x)                    ((u32)(((x >> 8) & 0x00000003)))
#define GPIO_MASK_GPIO3_IP_STS                      ((u32)0x00000003 << 6)                   /*!< R     0.0
                                                                                              * 00: The specific GPIO port is configured to rising-edge (active-high sensitive)
                                                                                              * 01: The specific GPIO port is configured to falling-edge (active-low sensitive)
                                                                                              * 10: The specific GPIO port is configured to dual-edge
                                                                                              * 11: rsvd
                                                                                              */
#define GPIO_GPIO3_IP_STS(x)                        ((u32)(((x) & 0x00000003) << 6))
#define GPIO_GET_GPIO3_IP_STS(x)                    ((u32)(((x >> 6) & 0x00000003)))
#define GPIO_MASK_GPIO2_IP_STS                      ((u32)0x00000003 << 4)                   /*!< R     0.0
                                                                                              * 00: The specific GPIO port is configured to rising-edge (active-high sensitive)
                                                                                              * 01: The specific GPIO port is configured to falling-edge (active-low sensitive)
                                                                                              * 10: The specific GPIO port is configured to dual-edge
                                                                                              * 11: rsvd
                                                                                              */
#define GPIO_GPIO2_IP_STS(x)                        ((u32)(((x) & 0x00000003) << 4))
#define GPIO_GET_GPIO2_IP_STS(x)                    ((u32)(((x >> 4) & 0x00000003)))
#define GPIO_MASK_GPIO1_IP_STS                      ((u32)0x00000003 << 2)                   /*!< R     0.0
                                                                                              * 00: The specific GPIO port is configured to rising-edge (active-high sensitive)
                                                                                              * 01: The specific GPIO port is configured to falling-edge (active-low sensitive)
                                                                                              * 10: The specific GPIO port is configured to dual-edge
                                                                                              * 11: rsvd
                                                                                              */
#define GPIO_GPIO1_IP_STS(x)                        ((u32)(((x) & 0x00000003) << 2))
#define GPIO_GET_GPIO1_IP_STS(x)                    ((u32)(((x >> 2) & 0x00000003)))
#define GPIO_MASK_GPIO0_IP_STS                      ((u32)0x00000003 << 0)                   /*!< R     0.0
                                                                                              * 00: The specific GPIO port is configured to rising-edge (active-high sensitive)
                                                                                              * 01: The specific GPIO port is configured to falling-edge (active-low sensitive)
                                                                                              * 10: The specific GPIO port is configured to dual-edge
                                                                                              * 11: rsvd
                                                                                              */
#define GPIO_GPIO0_IP_STS(x)                        ((u32)(((x) & 0x00000003) << 0))
#define GPIO_GET_GPIO0_IP_STS(x)                    ((u32)(((x >> 0) & 0x00000003)))
/** @} */


/** @defgroup REG_GPIO_IR_EN
 * @brief
 * @{
 */
#define GPIO_MASK_IR_EN                             ((u32)0x0000FFFF << 0)                   /*!< WO    0.0
                                                                                              * 0: No operation.
                                                                                              * 1: The controlled GPIO port is configured to rising-edge interrupt mode and the specific bits of REG_GPIO_IP_STS is “00“.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin. For AON GPIO, only Bit [5:0] are active, as there are only 6 AON GPIO pins.
                                                                                              */
#define GPIO_IR_EN(x)                               ((u32)(((x) & 0x0000FFFF) << 0))
#define GPIO_GET_IR_EN(x)                           ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_GPIO_IF_EN
 * @brief
 * @{
 */
#define GPIO_MASK_IF_EN                             ((u32)0x0000FFFF << 0)                   /*!< WO    0.0
                                                                                              * 0: No operation.
                                                                                              * 1: The controlled GPIO port is configured to falling-edge interrupt mode and the specific bits of REG_GPIO_IP_STS is “01“.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin. For AON GPIO, only Bit [5:0] are active, as there are only 6 AON GPIO pins.
                                                                                              */
#define GPIO_IF_EN(x)                               ((u32)(((x) & 0x0000FFFF) << 0))
#define GPIO_GET_IF_EN(x)                           ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_GPIO_ID_EN
 * @brief
 * @{
 */
#define GPIO_MASK_ID_EN                             ((u32)0x0000FFFF << 0)                   /*!< WO    0.0
                                                                                              * 0: No operation.
                                                                                              * 1: The controlled GPIO port is configured to dual-edge interrupt mode and the specific bits of REG_GPIO_IP_STS is “10“.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin. For AON GPIO, only Bit [5:0] are active, as there are only 6 AON GPIO pins.
                                                                                              */
#define GPIO_ID_EN(x)                               ((u32)(((x) & 0x0000FFFF) << 0))
#define GPIO_GET_ID_EN(x)                           ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_GPIO_IM_STS
 * @brief
 * @{
 */
#define GPIO_MASK_IM_STS                            ((u32)0x0000FFFF << 0)                   /*!< R     0.0
                                                                                              * 0: The interrupt mask of the specific GPIO port is disabled.
                                                                                              * 1: The interrupt mask of the specific GPIO port is enabled.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin. For AON GPIO, only Bit [5:0] are active, as there are only 6 AON GPIO pins.
                                                                                              */
#define GPIO_IM_STS(x)                              ((u32)(((x) & 0x0000FFFF) << 0))
#define GPIO_GET_IM_STS(x)                          ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_GPIO_IM_EN
 * @brief
 * @{
 */
#define GPIO_MASK_IM_EN                             ((u32)0x0000FFFF << 0)                   /*!< WO    0.0
                                                                                              * 0: No operation.
                                                                                              * 1: The specific GPIO INT is unmasked and REG_GPIO_IM_STS is ONE.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin. For AON GPIO, only Bit [5:0] are active, as there are only 6 AON GPIO pins.
                                                                                              */
#define GPIO_IM_EN(x)                               ((u32)(((x) & 0x0000FFFF) << 0))
#define GPIO_GET_IM_EN(x)                           ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_GPIO_IM_DIS
 * @brief
 * @{
 */
#define GPIO_MASK_IM_DIS                            ((u32)0x0000FFFF << 0)                   /*!< WO    0.0
                                                                                              * 0: No operation.
                                                                                              * 1: The controlled GPIO INT is masked and the specific bit of REG_GPIO_IM_STS is ZERO.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin. For AON GPIO, only Bit [5:0] are active, as there are only 6 AON GPIO pins.
                                                                                              */
#define GPIO_IM_DIS(x)                              ((u32)(((x) & 0x0000FFFF) << 0))
#define GPIO_GET_IM_DIS(x)                          ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_GPIO_RAW_INT_STS
 * @brief
 * @{
 */
#define GPIO_MASK_RAW_INT_STS                       ((u32)0x0000FFFF << 0)                   /*!< R     0.0
                                                                                              * 0: The specific GPIO port has no interrupt request.
                                                                                              * 1: The specific GPIO port has interrupt request (permasking).
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin. For AON GPIO, only Bit [5:0] are active, as there are only 6 AON GPIO pins.
                                                                                              */
#define GPIO_RAW_INT_STS(x)                         ((u32)(((x) & 0x0000FFFF) << 0))
#define GPIO_GET_RAW_INT_STS(x)                     ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_GPIO_INT_STS
 * @brief
 * @{
 */
#define GPIO_MASK_INT_STS                           ((u32)0x0000FFFF << 0)                   /*!< R     0.0
                                                                                              * 0: The specific GPIO port has no interrupt request.
                                                                                              * 1: The specific GPIO port has interrupt request.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin. For AON GPIO, only Bit [5:0] are active, as there are only 6 AON GPIO pins.
                                                                                              */
#define GPIO_INT_STS(x)                             ((u32)(((x) & 0x0000FFFF) << 0))
#define GPIO_GET_INT_STS(x)                         ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_GPIO_INT_CLR
 * @brief
 * @{
 */
#define GPIO_MASK_INT_CLR                           ((u32)0x0000FFFF << 0)                   /*!< WO    0.0
                                                                                              * 0: No operation
                                                                                              * 1: Clear edge type interrupt request of the controlled GPIO port and the specific bits of REG_GPIOA_INT_STS is ZERO.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin. For AON GPIO, only Bit [5:0] are active, as there are only 6 AON GPIO pins.
                                                                                              */
#define GPIO_INT_CLR(x)                             ((u32)(((x) & 0x0000FFFF) << 0))
#define GPIO_GET_INT_CLR(x)                         ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_GPIO_INT_EN_STS
 * @brief
 * @{
 */
#define GPIO_MASK_INT_EN_STS                        ((u32)0x0000FFFF << 0)                   /*!< R     0.0
                                                                                              * 0: The specific GPIO INT is disabled.
                                                                                              * 1: The specific GPIO INT is enabled.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin. For AON GPIO, only Bit [5:0] are active, as there are only 6 AON GPIO pins.
                                                                                              */
#define GPIO_INT_EN_STS(x)                          ((u32)(((x) & 0x0000FFFF) << 0))
#define GPIO_GET_INT_EN_STS(x)                      ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_GPIO_INT_EN
 * @brief
 * @{
 */
#define GPIO_MASK_INT_EN                            ((u32)0x0000FFFF << 0)                   /*!< WO    0.0
                                                                                              * 0: No operation.
                                                                                              * 1: The specific GPIO INT is enabled and the specific bit of REG_GPIO_INT_EN_STS is ONE.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin. For AON GPIO, only Bit [5:0] are active, as there are only 6 AON GPIO pins.
                                                                                              */
#define GPIO_INT_EN(x)                              ((u32)(((x) & 0x0000FFFF) << 0))
#define GPIO_GET_INT_EN(x)                          ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_GPIO_INT_DIS
 * @brief
 * @{
 */
#define GPIO_MASK_INT_DIS                           ((u32)0x0000FFFF << 0)                   /*!< WO    0.0
                                                                                              * 0: No operation.
                                                                                              * 1: The controlled GPIO INT is disabled and the specific bit of REG_GPIO_INT_EN_STS is ZERO.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin. For AON GPIO, only Bit [5:0] are active, as there are only 6 AON GPIO pins.
                                                                                              */
#define GPIO_INT_DIS(x)                             ((u32)(((x) & 0x0000FFFF) << 0))
#define GPIO_GET_INT_DIS(x)                         ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_GPIO_INT0_SEL
 * @brief
 * @{
 */
#define GPIO_BIT_INT0_SUR_SEL                       ((u32)0x00000001 << 15)                  /*!< R/W   0.0
                                                                                              * 0: Using pin input signal as source.
                                                                                              * 1: Using debounce output signal as source.
                                                                                              */
#define GPIO_MASK_INT0_DEB_SEL                      ((u32)0x0000000F << 8)                   /*!< R/W   0.0 Debounce Output signal selection for GPIO INT0. */
#define GPIO_INT0_DEB_SEL(x)                        ((u32)(((x) & 0x0000000F) << 8))
#define GPIO_GET_INT0_DEB_SEL(x)                    ((u32)(((x >> 8) & 0x0000000F)))
#define GPIO_MASK_INT0_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for GPIO INT0. There are 4 GPIO ports. */
#define GPIO_INT0_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_INT0_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_INT0_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for GPIO INT0. Each member represents one GPIO pin. */
#define GPIO_INT0_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_INT0_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_INT1_SEL
 * @brief
 * @{
 */
#define GPIO_BIT_INT1_SUR_SEL                       ((u32)0x00000001 << 15)                  /*!< R/W   0.0
                                                                                              * 0: Using pin input signal as source.
                                                                                              * 1: Using debounce output signal as source.
                                                                                              */
#define GPIO_MASK_INT1_DEB_SEL                      ((u32)0x0000000F << 8)                   /*!< R/W   0.0 Debounce Output signal selection for GPIO INT1. */
#define GPIO_INT1_DEB_SEL(x)                        ((u32)(((x) & 0x0000000F) << 8))
#define GPIO_GET_INT1_DEB_SEL(x)                    ((u32)(((x >> 8) & 0x0000000F)))
#define GPIO_MASK_INT1_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for GPIO INT1. There are 4 GPIO ports. */
#define GPIO_INT1_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_INT1_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_INT1_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for GPIO INT1. Each member represents one GPIO pin. */
#define GPIO_INT1_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_INT1_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_INT2_SEL
 * @brief
 * @{
 */
#define GPIO_BIT_INT2_SUR_SEL                       ((u32)0x00000001 << 15)                  /*!< R/W   0.0
                                                                                              * 0: Using pin input signal as source.
                                                                                              * 1: Using debounce output signal as source.
                                                                                              */
#define GPIO_MASK_INT2_DEB_SEL                      ((u32)0x0000000F << 8)                   /*!< R/W   0.0 Debounce Output signal selection for GPIO INT2. */
#define GPIO_INT2_DEB_SEL(x)                        ((u32)(((x) & 0x0000000F) << 8))
#define GPIO_GET_INT2_DEB_SEL(x)                    ((u32)(((x >> 8) & 0x0000000F)))
#define GPIO_MASK_INT2_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for GPIO INT2. There are 4 GPIO ports. */
#define GPIO_INT2_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_INT2_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_INT2_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for GPIO INT2. Each member represents one GPIO pin. */
#define GPIO_INT2_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_INT2_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_INT3_SEL
 * @brief
 * @{
 */
#define GPIO_BIT_INT3_SUR_SEL                       ((u32)0x00000001 << 15)                  /*!< R/W   0.0
                                                                                              * 0: Using pin input signal as source.
                                                                                              * 1: Using debounce output signal as source.
                                                                                              */
#define GPIO_MASK_INT3_DEB_SEL                      ((u32)0x0000000F << 8)                   /*!< R/W   0.0 Debounce Output signal selection for GPIO INT3. */
#define GPIO_INT3_DEB_SEL(x)                        ((u32)(((x) & 0x0000000F) << 8))
#define GPIO_GET_INT3_DEB_SEL(x)                    ((u32)(((x >> 8) & 0x0000000F)))
#define GPIO_MASK_INT3_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for GPIO INT3. There are 4 GPIO ports. */
#define GPIO_INT3_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_INT3_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_INT3_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for GPIO INT3. Each member represents one GPIO pin. */
#define GPIO_INT3_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_INT3_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_INT4_SEL
 * @brief
 * @{
 */
#define GPIO_BIT_INT4_SUR_SEL                       ((u32)0x00000001 << 15)                  /*!< R/W   0.0
                                                                                              * 0: Using pin input signal as source.
                                                                                              * 1: Using debounce output signal as source.
                                                                                              */
#define GPIO_MASK_INT4_DEB_SEL                      ((u32)0x0000000F << 8)                   /*!< R/W   0.0 Debounce Output signal selection for GPIO INT4. */
#define GPIO_INT4_DEB_SEL(x)                        ((u32)(((x) & 0x0000000F) << 8))
#define GPIO_GET_INT4_DEB_SEL(x)                    ((u32)(((x >> 8) & 0x0000000F)))
#define GPIO_MASK_INT4_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for GPIO INT4. There are 4 GPIO ports. */
#define GPIO_INT4_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_INT4_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_INT4_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for GPIO INT4. Each member represents one GPIO pin. */
#define GPIO_INT4_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_INT4_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_INT5_SEL
 * @brief
 * @{
 */
#define GPIO_BIT_INT5_SUR_SEL                       ((u32)0x00000001 << 15)                  /*!< R/W   0.0
                                                                                              * 0: Using pin input signal as source.
                                                                                              * 1: Using debounce output signal as source.
                                                                                              */
#define GPIO_MASK_INT5_DEB_SEL                      ((u32)0x0000000F << 8)                   /*!< R/W   0.0 Debounce Output signal selection for GPIO INT5. */
#define GPIO_INT5_DEB_SEL(x)                        ((u32)(((x) & 0x0000000F) << 8))
#define GPIO_GET_INT5_DEB_SEL(x)                    ((u32)(((x >> 8) & 0x0000000F)))
#define GPIO_MASK_INT5_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for GPIO INT5. There are 4 GPIO ports. */
#define GPIO_INT5_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_INT5_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_INT5_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for GPIO INT5. Each member represents one GPIO pin. */
#define GPIO_INT5_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_INT5_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_INT6_SEL
 * @brief
 * @{
 */
#define GPIO_BIT_INT6_SUR_SEL                       ((u32)0x00000001 << 15)                  /*!< R/W   0.0
                                                                                              * 0: Using pin input signal as source.
                                                                                              * 1: Using debounce output signal as source.
                                                                                              */
#define GPIO_MASK_INT6_DEB_SEL                      ((u32)0x0000000F << 8)                   /*!< R/W   0.0 Debounce Output signal selection for GPIO INT6. */
#define GPIO_INT6_DEB_SEL(x)                        ((u32)(((x) & 0x0000000F) << 8))
#define GPIO_GET_INT6_DEB_SEL(x)                    ((u32)(((x >> 8) & 0x0000000F)))
#define GPIO_MASK_INT6_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for GPIO INT6. There are 4 GPIO ports. */
#define GPIO_INT6_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_INT6_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_INT6_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for GPIO INT6. Each member represents one GPIO pin. */
#define GPIO_INT6_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_INT6_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_INT7_SEL
 * @brief
 * @{
 */
#define GPIO_BIT_INT7_SUR_SEL                       ((u32)0x00000001 << 15)                  /*!< R/W   0.0
                                                                                              * 0: Using pin input signal as source.
                                                                                              * 1: Using debounce output signal as source.
                                                                                              */
#define GPIO_MASK_INT7_DEB_SEL                      ((u32)0x0000000F << 8)                   /*!< R/W   0.0 Debounce Output signal selection for GPIO INT7 */
#define GPIO_INT7_DEB_SEL(x)                        ((u32)(((x) & 0x0000000F) << 8))
#define GPIO_GET_INT7_DEB_SEL(x)                    ((u32)(((x >> 8) & 0x0000000F)))
#define GPIO_MASK_INT7_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for GPIO INT7. There are 4 GPIO ports. */
#define GPIO_INT7_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_INT7_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_INT7_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for GPIO INT7. Each member represents one GPIO pin. */
#define GPIO_INT7_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_INT7_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_INT8_SEL
 * @brief
 * @{
 */
#define GPIO_BIT_INT8_SUR_SEL                       ((u32)0x00000001 << 15)                  /*!< R/W   0.0
                                                                                              * 0: Using pin input signal as source.
                                                                                              * 1: Using debounce output signal as source.
                                                                                              */
#define GPIO_MASK_INT8_DEB_SEL                      ((u32)0x0000000F << 8)                   /*!< R/W   0.0 Debounce Output signal selection for GPIO INT8. */
#define GPIO_INT8_DEB_SEL(x)                        ((u32)(((x) & 0x0000000F) << 8))
#define GPIO_GET_INT8_DEB_SEL(x)                    ((u32)(((x >> 8) & 0x0000000F)))
#define GPIO_MASK_INT8_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for GPIO INT8. There are 4 GPIO ports. */
#define GPIO_INT8_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_INT8_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_INT8_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for GPIO INT8. Each member represents one GPIO pin. */
#define GPIO_INT8_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_INT8_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_INT9_SEL
 * @brief
 * @{
 */
#define GPIO_BIT_INT9_SUR_SEL                       ((u32)0x00000001 << 15)                  /*!< R/W   0.0
                                                                                              * 0: Using pin input signal as source.
                                                                                              * 1: Using debounce output signal as source.
                                                                                              */
#define GPIO_MASK_INT9_DEB_SEL                      ((u32)0x0000000F << 8)                   /*!< R/W   0.0 Debounce Output signal selection for GPIO INT9. */
#define GPIO_INT9_DEB_SEL(x)                        ((u32)(((x) & 0x0000000F) << 8))
#define GPIO_GET_INT9_DEB_SEL(x)                    ((u32)(((x >> 8) & 0x0000000F)))
#define GPIO_MASK_INT9_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for GPIO INT9. There are 4 GPIO ports. */
#define GPIO_INT9_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_INT9_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_INT9_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for GPIO INT9. Each member represents one GPIO pin. */
#define GPIO_INT9_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_INT9_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_INTA_SEL
 * @brief
 * @{
 */
#define GPIO_BIT_INTA_SUR_SEL                       ((u32)0x00000001 << 15)                  /*!< R/W   0.0
                                                                                              * 0: Using pin input signal as source.
                                                                                              * 1: Using debounce output signal as source.
                                                                                              */
#define GPIO_MASK_INTA_DEB_SEL                      ((u32)0x0000000F << 8)                   /*!< R/W   0.0 Debounce Output signal selection for GPIO INTA. */
#define GPIO_INTA_DEB_SEL(x)                        ((u32)(((x) & 0x0000000F) << 8))
#define GPIO_GET_INTA_DEB_SEL(x)                    ((u32)(((x >> 8) & 0x0000000F)))
#define GPIO_MASK_INTA_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for GPIO INTA. There are 4 GPIO ports. */
#define GPIO_INTA_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_INTA_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_INTA_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for GPIO INTA. Each member represents one GPIO pin. */
#define GPIO_INTA_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_INTA_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_INTB_SEL
 * @brief
 * @{
 */
#define GPIO_BIT_INTB_SUR_SEL                       ((u32)0x00000001 << 15)                  /*!< R/W   0.0
                                                                                              * 0: Using pin input signal as source.
                                                                                              * 1: Using debounce output signal as source.
                                                                                              */
#define GPIO_MASK_INTB_DEB_SEL                      ((u32)0x0000000F << 8)                   /*!< R/W   0.0 Debounce Output signal selection for GPIO INTB. */
#define GPIO_INTB_DEB_SEL(x)                        ((u32)(((x) & 0x0000000F) << 8))
#define GPIO_GET_INTB_DEB_SEL(x)                    ((u32)(((x >> 8) & 0x0000000F)))
#define GPIO_MASK_INTB_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for GPIO INTB. There are 4 GPIO ports. */
#define GPIO_INTB_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_INTB_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_INTB_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for GPIO INTB. Each member represents one GPIO pin. */
#define GPIO_INTB_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_INTB_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_INTC_SEL
 * @brief
 * @{
 */
#define GPIO_BIT_INTC_SUR_SEL                       ((u32)0x00000001 << 15)                  /*!< R/W   0.0
                                                                                              * 0: Using pin input signal as source.
                                                                                              * 1: Using debounce output signal as source.
                                                                                              */
#define GPIO_MASK_INTC_DEB_SEL                      ((u32)0x0000000F << 8)                   /*!< R/W   0.0 Debounce Output signal selection for GPIO INTC. */
#define GPIO_INTC_DEB_SEL(x)                        ((u32)(((x) & 0x0000000F) << 8))
#define GPIO_GET_INTC_DEB_SEL(x)                    ((u32)(((x >> 8) & 0x0000000F)))
#define GPIO_MASK_INTC_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for GPIO INTC. There are 4 GPIO ports. */
#define GPIO_INTC_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_INTC_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_INTC_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for GPIO INTC. Each member represents one GPIO pin. */
#define GPIO_INTC_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_INTC_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_INTD_SEL
 * @brief
 * @{
 */
#define GPIO_BIT_INTD_SUR_SEL                       ((u32)0x00000001 << 15)                  /*!< R/W   0.0
                                                                                              * 0: Using pin input signal as source.
                                                                                              * 1: Using debounce output signal as source.
                                                                                              */
#define GPIO_MASK_INTD_DEB_SEL                      ((u32)0x0000000F << 8)                   /*!< R/W   0.0 Debounce Output signal selection for GPIO INTD. */
#define GPIO_INTD_DEB_SEL(x)                        ((u32)(((x) & 0x0000000F) << 8))
#define GPIO_GET_INTD_DEB_SEL(x)                    ((u32)(((x >> 8) & 0x0000000F)))
#define GPIO_MASK_INTD_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for GPIO INTD. There are 4 GPIO ports. */
#define GPIO_INTD_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_INTD_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_INTD_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for GPIO INTD. Each member represents one GPIO pin. */
#define GPIO_INTD_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_INTD_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_INTE_SEL
 * @brief
 * @{
 */
#define GPIO_BIT_INTE_SUR_SEL                       ((u32)0x00000001 << 15)                  /*!< R/W   0.0
                                                                                              * 0: Using pin input signal as source.
                                                                                              * 1: Using debounce output signal as source.
                                                                                              */
#define GPIO_MASK_INTE_DEB_SEL                      ((u32)0x0000000F << 8)                   /*!< R/W   0.0 Debounce Output signal selection for GPIO INTE. */
#define GPIO_INTE_DEB_SEL(x)                        ((u32)(((x) & 0x0000000F) << 8))
#define GPIO_GET_INTE_DEB_SEL(x)                    ((u32)(((x >> 8) & 0x0000000F)))
#define GPIO_MASK_INTE_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for GPIO INTE. There are 4 GPIO ports. */
#define GPIO_INTE_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_INTE_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_INTE_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for GPIO INTE. Each member represents one GPIO pin. */
#define GPIO_INTE_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_INTE_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_INTF_SEL
 * @brief
 * @{
 */
#define GPIO_BIT_INTF_SUR_SEL                       ((u32)0x00000001 << 15)                  /*!< R/W   0.0
                                                                                              * 0: Using pin input signal as source.
                                                                                              * 1: Using debounce output signal as source.
                                                                                              */
#define GPIO_MASK_INTF_DEB_SEL                      ((u32)0x0000000F << 8)                   /*!< R/W   0.0 Debounce Output signal selection for GPIO INTF. */
#define GPIO_INTF_DEB_SEL(x)                        ((u32)(((x) & 0x0000000F) << 8))
#define GPIO_GET_INTF_DEB_SEL(x)                    ((u32)(((x >> 8) & 0x0000000F)))
#define GPIO_MASK_INTF_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for GPIO INTF. There are 4 GPIO ports. */
#define GPIO_INTF_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_INTF_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_INTF_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for GPIO INTF. Each member represents one GPIO pin. */
#define GPIO_INTF_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_INTF_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_DEB_STS
 * @brief
 * @{
 */
#define GPIO_MASK_DEB_STS                           ((u32)0x0000FFFF << 0)                   /*!< R     0.0
                                                                                              * 0: The specific GPIO port has its debounce function disabled.
                                                                                              * 1: The specific GPIO port has its debounce function enabled.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin. For AON GPIO, only Bit [5:0] are active, as there are only 6 AON GPIO pins.
                                                                                              */
#define GPIO_DEB_STS(x)                             ((u32)(((x) & 0x0000FFFF) << 0))
#define GPIO_GET_DEB_STS(x)                         ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_GPIO_DEB_EN
 * @brief
 * @{
 */
#define GPIO_MASK_DEB_EN                            ((u32)0x0000FFFF << 0)                   /*!< WO    0.0
                                                                                              * 0: No operation.
                                                                                              * 1: The controlled GPIO debounce port has its debounce function enabled and the specific bit of REG_GPIO_DEB_STS is ONE.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin. For AON GPIO, only Bit [5:0] are active, as there are only 6 AON GPIO pins.
                                                                                              */
#define GPIO_DEB_EN(x)                              ((u32)(((x) & 0x0000FFFF) << 0))
#define GPIO_GET_DEB_EN(x)                          ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_GPIO_DEB_DIS
 * @brief
 * @{
 */
#define GPIO_MASK_DEB_DIS                           ((u32)0x0000FFFF << 0)                   /*!< WO    0.0
                                                                                              * 0: No operation.
                                                                                              * 1: The controlled GPIO debounce port has its debounce function disabled and the specific bit of REG_GPIO_DEB_STS is ZERO.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin. For AON GPIO, only Bit [5:0] are active, as there are only 6 AON GPIO pins.
                                                                                              */
#define GPIO_DEB_DIS(x)                             ((u32)(((x) & 0x0000FFFF) << 0))
#define GPIO_GET_DEB_DIS(x)                         ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_GPIO_DEB_DP_STS
 * @brief
 * @{
 */
#define GPIO_MASK_DEB_DP_STS                        ((u32)0x0000FFFF << 0)                   /*!< R     0.0
                                                                                              * Reading this location reads the values of the signal on the specified pin after debounce.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin. For AON GPIO, only Bit [5:0] are active, as there are only 6 AON GPIO pins.
                                                                                              */
#define GPIO_DEB_DP_STS(x)                          ((u32)(((x) & 0x0000FFFF) << 0))
#define GPIO_GET_DEB_DP_STS(x)                      ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_GPIO_DEB0_SEL
 * @brief
 * @{
 */
#define GPIO_MASK_DEB0_CYC                          ((u32)0x00003FFF << 8)                   /*!< R/W   0.0 The GPIO signal will be filtered by the number of debounce cycles of specified in this register. */
#define GPIO_DEB0_CYC(x)                            ((u32)(((x) & 0x00003FFF) << 8))
#define GPIO_GET_DEB0_CYC(x)                        ((u32)(((x >> 8) & 0x00003FFF)))
#define GPIO_MASK_DEB0_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for debounce0. Supports up to 4 GPIO groups. */
#define GPIO_DEB0_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_DEB0_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_DEB0_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for debounce0. Each member represents one GPIO pin. */
#define GPIO_DEB0_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_DEB0_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_DEB1_SEL
 * @brief
 * @{
 */
#define GPIO_MASK_DEB1_CYC                          ((u32)0x00003FFF << 8)                   /*!< R/W   0.0 The GPIO signal will be filtered by the number of debounce cycles of specified in this register. */
#define GPIO_DEB1_CYC(x)                            ((u32)(((x) & 0x00003FFF) << 8))
#define GPIO_GET_DEB1_CYC(x)                        ((u32)(((x >> 8) & 0x00003FFF)))
#define GPIO_MASK_DEB1_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for debounce1. Supports up to 4 GPIO groups. */
#define GPIO_DEB1_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_DEB1_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_DEB1_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for debounce1. Each member represents one GPIO pin. */
#define GPIO_DEB1_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_DEB1_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_DEB2_SEL
 * @brief
 * @{
 */
#define GPIO_MASK_DEB2_CYC                          ((u32)0x00003FFF << 8)                   /*!< R/W   0.0 The GPIO signal will be filtered by the number of debounce cycles of specified in this register. */
#define GPIO_DEB2_CYC(x)                            ((u32)(((x) & 0x00003FFF) << 8))
#define GPIO_GET_DEB2_CYC(x)                        ((u32)(((x >> 8) & 0x00003FFF)))
#define GPIO_MASK_DEB2_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for debounce2. Supports up to 4 GPIO groups. */
#define GPIO_DEB2_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_DEB2_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_DEB2_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for debounce2. Each member represents one GPIO pin. */
#define GPIO_DEB2_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_DEB2_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_DEB3_SEL
 * @brief
 * @{
 */
#define GPIO_MASK_DEB3_CYC                          ((u32)0x00003FFF << 8)                   /*!< R/W   0.0 The GPIO signal will be filtered by the number of debounce cycles of specified in this register. */
#define GPIO_DEB3_CYC(x)                            ((u32)(((x) & 0x00003FFF) << 8))
#define GPIO_GET_DEB3_CYC(x)                        ((u32)(((x >> 8) & 0x00003FFF)))
#define GPIO_MASK_DEB3_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for debounce3. Supports up to 4 GPIO groups. */
#define GPIO_DEB3_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_DEB3_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_DEB3_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for debounce3. Each member represents one GPIO pin. */
#define GPIO_DEB3_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_DEB3_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_DEB4_SEL
 * @brief
 * @{
 */
#define GPIO_MASK_DEB4_CYC                          ((u32)0x00003FFF << 8)                   /*!< R/W   0.0 The GPIO signal will be filtered by the number of debounce cycles of specified in this register. */
#define GPIO_DEB4_CYC(x)                            ((u32)(((x) & 0x00003FFF) << 8))
#define GPIO_GET_DEB4_CYC(x)                        ((u32)(((x >> 8) & 0x00003FFF)))
#define GPIO_MASK_DEB4_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for debounce4. Supports up to 4 GPIO groups. */
#define GPIO_DEB4_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_DEB4_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_DEB4_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for debounce4. Each member represents one GPIO pin. */
#define GPIO_DEB4_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_DEB4_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_DEB5_SEL
 * @brief
 * @{
 */
#define GPIO_MASK_DEB5_CYC                          ((u32)0x00003FFF << 8)                   /*!< R/W   0.0 The GPIO signal will be filtered by the number of debounce cycles of specified in this register. */
#define GPIO_DEB5_CYC(x)                            ((u32)(((x) & 0x00003FFF) << 8))
#define GPIO_GET_DEB5_CYC(x)                        ((u32)(((x >> 8) & 0x00003FFF)))
#define GPIO_BIT_RSVE                               ((u32)0x00000001 << 7)                   /*!<   nan nan None */
#define GPIO_MASK_DEB5_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for debounce5. Supports up to 4 GPIO groups. */
#define GPIO_DEB5_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_DEB5_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_DEB5_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for debounce5. Each member represents one GPIO pin. */
#define GPIO_DEB5_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_DEB5_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_DEB6_SEL
 * @brief
 * @{
 */
#define GPIO_MASK_DEB6_CYC                          ((u32)0x00003FFF << 8)                   /*!< R/W   0.0 The GPIO signal will be filtered by the number of debounce cycles of specified in this register. */
#define GPIO_DEB6_CYC(x)                            ((u32)(((x) & 0x00003FFF) << 8))
#define GPIO_GET_DEB6_CYC(x)                        ((u32)(((x >> 8) & 0x00003FFF)))
#define GPIO_MASK_DEB6_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for debounce6. Supports up to 4 GPIO groups. */
#define GPIO_DEB6_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_DEB6_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_DEB6_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for debounce6. Each member represents one GPIO pin. */
#define GPIO_DEB6_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_DEB6_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_DEB7_SEL
 * @brief
 * @{
 */
#define GPIO_MASK_DEB7_CYC                          ((u32)0x00003FFF << 8)                   /*!< R/W   0.0 The GPIO signal will be filtered by the number of debounce cycles of specified in this register. */
#define GPIO_DEB7_CYC(x)                            ((u32)(((x) & 0x00003FFF) << 8))
#define GPIO_GET_DEB7_CYC(x)                        ((u32)(((x >> 8) & 0x00003FFF)))
#define GPIO_MASK_DEB7_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for debounce7. Supports up to 4 GPIO groups. */
#define GPIO_DEB7_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_DEB7_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_DEB7_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for debounce7. Each member represents one GPIO pin. */
#define GPIO_DEB7_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_DEB7_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_DEB8_SEL
 * @brief
 * @{
 */
#define GPIO_MASK_DEB8_CYC                          ((u32)0x00003FFF << 8)                   /*!< R/W   0.0 The GPIO signal will be filtered by the number of debounce cycles of specified in this register. */
#define GPIO_DEB8_CYC(x)                            ((u32)(((x) & 0x00003FFF) << 8))
#define GPIO_GET_DEB8_CYC(x)                        ((u32)(((x >> 8) & 0x00003FFF)))
#define GPIO_MASK_DEB8_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for debounce8. Supports up to 4 GPIO groups. */
#define GPIO_DEB8_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_DEB8_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_DEB8_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for debounce8. Each member represents one GPIO pin. */
#define GPIO_DEB8_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_DEB8_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_DEB9_SEL
 * @brief
 * @{
 */
#define GPIO_MASK_DEB9_CYC                          ((u32)0x00003FFF << 8)                   /*!< R/W   0.0 The GPIO signal will be filtered by the number of debounce cycles of specified in this register. */
#define GPIO_DEB9_CYC(x)                            ((u32)(((x) & 0x00003FFF) << 8))
#define GPIO_GET_DEB9_CYC(x)                        ((u32)(((x >> 8) & 0x00003FFF)))
#define GPIO_MASK_DEB9_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for debounce9. Supports up to 4 GPIO groups. */
#define GPIO_DEB9_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_DEB9_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_DEB9_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for debounce9. Each member represents one GPIO pin. */
#define GPIO_DEB9_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_DEB9_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_DEBA_SEL
 * @brief
 * @{
 */
#define GPIO_MASK_DEBA_CYC                          ((u32)0x00003FFF << 8)                   /*!< R/W   0.0 The GPIO signal will be filtered by the number of debounce cycles of specified in this register. */
#define GPIO_DEBA_CYC(x)                            ((u32)(((x) & 0x00003FFF) << 8))
#define GPIO_GET_DEBA_CYC(x)                        ((u32)(((x >> 8) & 0x00003FFF)))
#define GPIO_MASK_DEBA_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for debounceA. Supports up to 4 GPIO groups. */
#define GPIO_DEBA_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_DEBA_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_DEBA_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for debounceA. Each member represents one GPIO pin. */
#define GPIO_DEBA_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_DEBA_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_DEBB_SEL
 * @brief
 * @{
 */
#define GPIO_MASK_DEBB_CYC                          ((u32)0x00003FFF << 8)                   /*!< R/W   0.0 The GPIO signal will be filtered by the number of debounce cycles of specified in this register. */
#define GPIO_DEBB_CYC(x)                            ((u32)(((x) & 0x00003FFF) << 8))
#define GPIO_GET_DEBB_CYC(x)                        ((u32)(((x >> 8) & 0x00003FFF)))
#define GPIO_MASK_DEBB_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for debounceB. Supports up to 4 GPIO groups. */
#define GPIO_DEBB_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_DEBB_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_DEBB_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for debounceB. Each member represents one GPIO pin. */
#define GPIO_DEBB_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_DEBB_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_DEBC_SEL
 * @brief
 * @{
 */
#define GPIO_MASK_DEBC_CYC                          ((u32)0x00003FFF << 8)                   /*!< R/W   0.0 The GPIO signal will be filtered by the number of debounce cycles of specified in this register. */
#define GPIO_DEBC_CYC(x)                            ((u32)(((x) & 0x00003FFF) << 8))
#define GPIO_GET_DEBC_CYC(x)                        ((u32)(((x >> 8) & 0x00003FFF)))
#define GPIO_MASK_DEBC_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for debounceC. Supports up to 4 GPIO groups. */
#define GPIO_DEBC_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_DEBC_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_DEBC_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for debounceC. Each member represents one GPIO pin. */
#define GPIO_DEBC_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_DEBC_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_DEBD_SEL
 * @brief
 * @{
 */
#define GPIO_MASK_DEBD_CYC                          ((u32)0x00003FFF << 8)                   /*!< R/W   0.0 The GPIO signal will be filtered by the number of debounce cycles of specified in this register. */
#define GPIO_DEBD_CYC(x)                            ((u32)(((x) & 0x00003FFF) << 8))
#define GPIO_GET_DEBD_CYC(x)                        ((u32)(((x >> 8) & 0x00003FFF)))
#define GPIO_MASK_DEBD_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for debounceD. Supports up to 4 GPIO groups. */
#define GPIO_DEBD_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_DEBD_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_DEBD_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for debounceD. Each member represents one GPIO pin. */
#define GPIO_DEBD_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_DEBD_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_DEBE_SEL
 * @brief
 * @{
 */
#define GPIO_MASK_DEBE_CYC                          ((u32)0x00003FFF << 8)                   /*!< R/W   0.0 The GPIO signal will be filtered by the number of debounce cycles of specified in this register. */
#define GPIO_DEBE_CYC(x)                            ((u32)(((x) & 0x00003FFF) << 8))
#define GPIO_GET_DEBE_CYC(x)                        ((u32)(((x >> 8) & 0x00003FFF)))
#define GPIO_MASK_DEBE_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for debounceE. Supports up to 4 GPIO groups. */
#define GPIO_DEBE_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_DEBE_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_DEBE_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for debounceE. Each member represents one GPIO pin. */
#define GPIO_DEBE_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_DEBE_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIO_DEBF_SEL
 * @brief
 * @{
 */
#define GPIO_MASK_DEBF_CYC                          ((u32)0x00003FFF << 8)                   /*!< R/W   0.0 The GPIO signal will be filtered by the number of debounce cycles of specified in this register. */
#define GPIO_DEBF_CYC(x)                            ((u32)(((x) & 0x00003FFF) << 8))
#define GPIO_GET_DEBF_CYC(x)                        ((u32)(((x >> 8) & 0x00003FFF)))
#define GPIO_MASK_DEBF_GP_SEL                       ((u32)0x00000003 << 5)                   /*!< R/W   0.0 GPIO port selection for debounceF. Supports up to 4 GPIO groups.. */
#define GPIO_DEBF_GP_SEL(x)                         ((u32)(((x) & 0x00000003) << 5))
#define GPIO_GET_DEBF_GP_SEL(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define GPIO_MASK_DEBF_MER_SEL                      ((u32)0x0000001F << 0)                   /*!< R/W   0.0 Member selection of specified group for debounceF. Each member represents one GPIO pin. */
#define GPIO_DEBF_MER_SEL(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define GPIO_GET_DEBF_MER_SEL(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_GPIOA_DMD_STS
 * @brief
 * @{
 */
#define GPIO_MASK_GPIOA_DMD_STS                     ((u32)0xFFFFFFFF << 0)                   /*!< R     0.0
                                                                                              * 0: The specific GPIO port is configured to INPUT data mode
                                                                                              * 1: The specific GPIO port is configured to OUTPUT data mode.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin.
                                                                                              */
#define GPIO_GPIOA_DMD_STS(x)                       ((u32)(((x) & 0xFFFFFFFF) << 0))
#define GPIO_GET_GPIOA_DMD_STS(x)                   ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_GPIOA_IDM_EN
 * @brief
 * @{
 */
#define GPIO_MASK_GPIOA_IDM_EN                      ((u32)0xFFFFFFFF << 0)                   /*!< WO    0.0
                                                                                              * 0: The controlled GPIO port’s configuration is unchanged.
                                                                                              * 1: The controlled GPIO port is configured to input data mode and the specific bit of REG_GPIOA_DMD_STS is ZERO.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin.
                                                                                              */
#define GPIO_GPIOA_IDM_EN(x)                        ((u32)(((x) & 0xFFFFFFFF) << 0))
#define GPIO_GET_GPIOA_IDM_EN(x)                    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_GPIOA_ODM_EN
 * @brief
 * @{
 */
#define GPIO_MASK_GPIOA_ODM_EN                      ((u32)0xFFFFFFFF << 0)                   /*!< WO    0.0
                                                                                              * 0: The controlled GPIO port’s configuration is unchanged.
                                                                                              * 1: The controlled GPIO port is configured to output data mode and the specific bit of REG_GPIOA_DMD_STS is ONE.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin.
                                                                                              */
#define GPIO_GPIOA_ODM_EN(x)                        ((u32)(((x) & 0xFFFFFFFF) << 0))
#define GPIO_GET_GPIOA_ODM_EN(x)                    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_GPIOA_OD_STS
 * @brief
 * @{
 */
#define GPIO_MASK_GPIOA_OD_STS                      ((u32)0xFFFFFFFF << 0)                   /*!< R     0.0
                                                                                              * 0: The specific GPIO port is configured to output low.
                                                                                              * 1: The specific GPIO port is configured to output high, if the specific GPIO port is output data mode.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin.
                                                                                              */
#define GPIO_GPIOA_OD_STS(x)                        ((u32)(((x) & 0xFFFFFFFF) << 0))
#define GPIO_GET_GPIOA_OD_STS(x)                    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_GPIOA_ODL_EN
 * @brief
 * @{
 */
#define GPIO_MASK_GPIOA_ODL_EN                      ((u32)0xFFFFFFFF << 0)                   /*!< WO    0.0
                                                                                              * 0: No operation.
                                                                                              * 1: The controlled GPIO port is configured to output low and the specific bit of REG_GPIOA_OD_STS is ZERO.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin.
                                                                                              */
#define GPIO_GPIOA_ODL_EN(x)                        ((u32)(((x) & 0xFFFFFFFF) << 0))
#define GPIO_GET_GPIOA_ODL_EN(x)                    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_GPIOA_ODH_EN
 * @brief
 * @{
 */
#define GPIO_MASK_GPIOA_ODH_EN                      ((u32)0xFFFFFFFF << 0)                   /*!< WO    0.0
                                                                                              * 0: No operation.
                                                                                              * 1: The controlled GPIO port is configured to output high and the specific bit of REG_GPIOA_OD_STS is ONE.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin.
                                                                                              */
#define GPIO_GPIOA_ODH_EN(x)                        ((u32)(((x) & 0xFFFFFFFF) << 0))
#define GPIO_GET_GPIOA_ODH_EN(x)                    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_GPIOA_ODT
 * @brief
 * @{
 */
#define GPIO_MASK_GPIOA_ODT                         ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0
                                                                                              * 0: No operation.
                                                                                              * 1: Toggle output of the controlled GPIO port and the specific bit of REG_GPIOA_OD_STS is also toggled.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin.
                                                                                              */
#define GPIO_GPIOA_ODT(x)                           ((u32)(((x) & 0xFFFFFFFF) << 0))
#define GPIO_GET_GPIOA_ODT(x)                       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_GPIOA_DP_STS
 * @brief
 * @{
 */
#define GPIO_MASK_GPIOA_DP_STS                      ((u32)0xFFFFFFFF << 0)                   /*!< R     0.0
                                                                                              * When the specific bit is configured as input mode, reading this register reads the values of the signals on the respective pins.
                                                                                              * 
                                                                                              * When the direction of specific bit is configured as output, then reading this location reads the output data register for group A. However, if an external voltage is applied to the bit (pin) destined for output, the corresponding bit in REG_GPIOA_DP_STS will reflect the value of that external signal.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin.
                                                                                              */
#define GPIO_GPIOA_DP_STS(x)                        ((u32)(((x) & 0xFFFFFFFF) << 0))
#define GPIO_GET_GPIOA_DP_STS(x)                    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_GPIOB_DMD_STS
 * @brief
 * @{
 */
#define GPIO_MASK_GPIOB_DMD_STS                     ((u32)0xFFFFFFFF << 0)                   /*!< R     0.0
                                                                                              * 0: The specific GPIO port is configured to INPUT data mode.
                                                                                              * 1: The specific GPIO port is configured to OUTPUT data mode.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin.
                                                                                              */
#define GPIO_GPIOB_DMD_STS(x)                       ((u32)(((x) & 0xFFFFFFFF) << 0))
#define GPIO_GET_GPIOB_DMD_STS(x)                   ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_GPIOB_IDM_EN
 * @brief
 * @{
 */
#define GPIO_MASK_GPIOB_IDM_EN                      ((u32)0xFFFFFFFF << 0)                   /*!< WO    0.0
                                                                                              * 0: The controlled GPIO port’s configuration is unchanged.
                                                                                              * 1: The controlled GPIO port is configured to input data mode and the specific bit of REG_GPIOB_DMD_STS is ZERO.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin.
                                                                                              */
#define GPIO_GPIOB_IDM_EN(x)                        ((u32)(((x) & 0xFFFFFFFF) << 0))
#define GPIO_GET_GPIOB_IDM_EN(x)                    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_GPIOB_ODM_EN
 * @brief
 * @{
 */
#define GPIO_MASK_GPIOB_ODM_EN                      ((u32)0xFFFFFFFF << 0)                   /*!< WO    0.0
                                                                                              * 0: The controlled GPIO port’s configuration is unchanged.
                                                                                              * 1: The controlled GPIO port is configured to output data mode and the specific bit of REG_GPIOB_DMD_STS is ONE.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin.
                                                                                              */
#define GPIO_GPIOB_ODM_EN(x)                        ((u32)(((x) & 0xFFFFFFFF) << 0))
#define GPIO_GET_GPIOB_ODM_EN(x)                    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_GPIOB_OD_STS
 * @brief
 * @{
 */
#define GPIO_MASK_GPIOB_OD_STS                      ((u32)0xFFFFFFFF << 0)                   /*!< R     0.0
                                                                                              * 0: The specific GPIO port is configured to output low.
                                                                                              * 1: The specific GPIO port is configured to output high, if the specific GPIO port is output data mode.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin.
                                                                                              */
#define GPIO_GPIOB_OD_STS(x)                        ((u32)(((x) & 0xFFFFFFFF) << 0))
#define GPIO_GET_GPIOB_OD_STS(x)                    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_GPIOB_ODL_EN
 * @brief
 * @{
 */
#define GPIO_MASK_GPIOB_ODL_EN                      ((u32)0xFFFFFFFF << 0)                   /*!< WO    0.0
                                                                                              * 0: No operation.
                                                                                              * 1: The controlled GPIO port is configured to output low and the specific bit of REG_GPIOB_OD_STS is ZERO.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin.
                                                                                              */
#define GPIO_GPIOB_ODL_EN(x)                        ((u32)(((x) & 0xFFFFFFFF) << 0))
#define GPIO_GET_GPIOB_ODL_EN(x)                    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_GPIOB_ODH_EN
 * @brief
 * @{
 */
#define GPIO_MASK_GPIOB_ODH_EN                      ((u32)0xFFFFFFFF << 0)                   /*!< WO    0.0
                                                                                              * 0: No operation.
                                                                                              * 1: The controlled GPIO port is configured to output high and the specific bit of REG_GPIOB_OD_STS is ONE.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin.
                                                                                              */
#define GPIO_GPIOB_ODH_EN(x)                        ((u32)(((x) & 0xFFFFFFFF) << 0))
#define GPIO_GET_GPIOB_ODH_EN(x)                    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_GPIOA_ODT
 * @brief
 * @{
 */
#define GPIO_MASK_GPIOB_ODT                         ((u32)0xFFFFFFFF << 0)                   /*!< WO    0.0
                                                                                              * 0: No operation.
                                                                                              * 1: Toggle output of the controlled GPIO port and the specific bit of REG_GPIOB_OD_STS is also toggled.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin.
                                                                                              */
#define GPIO_GPIOB_ODT(x)                           ((u32)(((x) & 0xFFFFFFFF) << 0))
#define GPIO_GET_GPIOB_ODT(x)                       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_GPIOB_DP_STS
 * @brief
 * @{
 */
#define GPIO_MASK_GPIOB_DP_STS                      ((u32)0xFFFFFFFF << 0)                   /*!< R     0.0
                                                                                              * When the specific bit is configured as input mode, then reading this register reads the values of the signal on the respective pins.
                                                                                              * 
                                                                                              * When the direction of specific bit is configured as output, then reading this location reads the output data register for group B. However, if an external voltage is applied to the bit (pin) destined for output, the corresponding bit in REG_GPIOB_DP_STS will reflect the value of that external signal.
                                                                                              * 
                                                                                              * Each bit represents one GPIO pin.
                                                                                              */
#define GPIO_GPIOB_DP_STS(x)                        ((u32)(((x) & 0xFFFFFFFF) << 0))
#define GPIO_GET_GPIOB_DP_STS(x)                    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */




/** @defgroup GPIO_Exported_Types GPIO Exported Types
* @{
*/
/**
* @brief GPIO Register Declaration
*/

typedef struct {
	__I  uint32_t GPIO_IT_STS;                                  /*!< Register,  Address offset:0x0000 */
	__O  uint32_t GPIO_EI_EN;                                   /*!< Register,  Address offset:0x0004 */
	__O  uint32_t GPIO_LI_EN;                                   /*!< Register,  Address offset:0x0008 */
	__I  uint32_t GPIO_IP_STS;                                  /*!< Register,  Address offset:0x000C */
	uint32_t RSVD_NO_FUNCTION0[1];
	__O  uint32_t GPIO_IR_EN;                                   /*!< Register,  Address offset:0x0014 */
	__O  uint32_t GPIO_IF_EN;                                   /*!< Register,  Address offset:0x0018 */
	__O  uint32_t GPIO_ID_EN;                                   /*!< Register,  Address offset:0x001C */
	__I  uint32_t GPIO_IM_STS;                                  /*!< Register,  Address offset:0x0020 */
	__O  uint32_t GPIO_IM_EN;                                   /*!< Register,  Address offset:0x0024 */
	__O  uint32_t GPIO_IM_DIS;                                  /*!< Register,  Address offset:0x0028 */
	__I  uint32_t GPIO_RAW_INT_STS;                             /*!< Register,  Address offset:0x002C */
	__I  uint32_t GPIO_INT_STS;                                 /*!< Register,  Address offset:0x0030 */
	__O  uint32_t GPIO_INT_CLR;                                 /*!< Register,  Address offset:0x0034 */
	__I  uint32_t GPIO_INT_EN_STS;                              /*!< Register,  Address offset:0x0038 */
	__O  uint32_t GPIO_INT_EN;                                  /*!< Register,  Address offset:0x003C */
	__O  uint32_t GPIO_INT_DIS;                                 /*!< Register,  Address offset:0x0040 */
	uint32_t RSVD_NO_FUNCTION1[3];
	__IO uint32_t GPIO_INT0_SEL;                                /*!< Register,  Address offset:0x0050 */
	__IO uint32_t GPIO_INT1_SEL;                                /*!< Register,  Address offset:0x0054 */
	__IO uint32_t GPIO_INT2_SEL;                                /*!< Register,  Address offset:0x0058 */
	__IO uint32_t GPIO_INT3_SEL;                                /*!< Register,  Address offset:0x005C */
	__IO uint32_t GPIO_INT4_SEL;                                /*!< Register,  Address offset:0x0060 */
	__IO uint32_t GPIO_INT5_SEL;                                /*!< Register,  Address offset:0x0064 */
	__IO uint32_t GPIO_INT6_SEL;                                /*!< Register,  Address offset:0x0068 */
	__IO uint32_t GPIO_INT7_SEL;                                /*!< Register,  Address offset:0x006C */
	__IO uint32_t GPIO_INT8_SEL;                                /*!< Register,  Address offset:0x0070 */
	__IO uint32_t GPIO_INT9_SEL;                                /*!< Register,  Address offset:0x0074 */
	__IO uint32_t GPIO_INTA_SEL;                                /*!< Register,  Address offset:0x0078 */
	__IO uint32_t GPIO_INTB_SEL;                                /*!< Register,  Address offset:0x007C */
	__IO uint32_t GPIO_INTC_SEL;                                /*!< Register,  Address offset:0x0080 */
	__IO uint32_t GPIO_INTD_SEL;                                /*!< Register,  Address offset:0x0084 */
	__IO uint32_t GPIO_INTE_SEL;                                /*!< Register,  Address offset:0x0088 */
	__IO uint32_t GPIO_INTF_SEL;                                /*!< Register,  Address offset:0x008C */
	uint32_t RSVD_NO_FUNCTION2[24];
	__I  uint32_t GPIO_DEB_STS;                                 /*!< Register,  Address offset:0x00F0 */
	__O  uint32_t GPIO_DEB_EN;                                  /*!< Register,  Address offset:0x00F4 */
	__O  uint32_t GPIO_DEB_DIS;                                 /*!< Register,  Address offset:0x00F8 */
	__I  uint32_t GPIO_DEB_DP_STS;                              /*!< Register,  Address offset:0x00FC */
	__IO uint32_t GPIO_DEB0_SEL;                                /*!< Register,  Address offset:0x0100 */
	__IO uint32_t GPIO_DEB1_SEL;                                /*!< Register,  Address offset:0x0104 */
	__IO uint32_t GPIO_DEB2_SEL;                                /*!< Register,  Address offset:0x0108 */
	__IO uint32_t GPIO_DEB3_SEL;                                /*!< Register,  Address offset:0x010C */
	__IO uint32_t GPIO_DEB4_SEL;                                /*!< Register,  Address offset:0x0110 */
	__IO uint32_t GPIO_DEB5_SEL;                                /*!< Register,  Address offset:0x0114 */
	__IO uint32_t GPIO_DEB6_SEL;                                /*!< Register,  Address offset:0x0118 */
	__IO uint32_t GPIO_DEB7_SEL;                                /*!< Register,  Address offset:0x011C */
	__IO uint32_t GPIO_DEB8_SEL;                                /*!< Register,  Address offset:0x0120 */
	__IO uint32_t GPIO_DEB9_SEL;                                /*!< Register,  Address offset:0x0124 */
	__IO uint32_t GPIO_DEBA_SEL;                                /*!< Register,  Address offset:0x0128 */
	__IO uint32_t GPIO_DEBB_SEL;                                /*!< Register,  Address offset:0x012C */
	__IO uint32_t GPIO_DEBC_SEL;                                /*!< Register,  Address offset:0x0130 */
	__IO uint32_t GPIO_DEBD_SEL;                                /*!< Register,  Address offset:0x0134 */
	__IO uint32_t GPIO_DEBE_SEL;                                /*!< Register,  Address offset:0x0138 */
	__IO uint32_t GPIO_DEBF_SEL;                                /*!< Register,  Address offset:0x013C */
	uint32_t RSVD_NO_FUNCTION3[48];
	__I  uint32_t GPIOA_DMD_STS;                                /*!< Register,  Address offset:0x0200 */
	__O  uint32_t GPIOA_IDM_EN;                                 /*!< Register,  Address offset:0x0204 */
	__O  uint32_t GPIOA_ODM_EN;                                 /*!< Register,  Address offset:0x0208 */
	__I  uint32_t GPIOA_OD_STS;                                 /*!< Register,  Address offset:0x020C */
	__O  uint32_t GPIOA_ODL_EN;                                 /*!< Register,  Address offset:0x0210 */
	__O  uint32_t GPIOA_ODH_EN;                                 /*!< Register,  Address offset:0x0214 */
	__O  uint32_t GPIOA_ODT;                                    /*!< Register,  Address offset:0x0218 */
	__I  uint32_t GPIOA_DP_STS;                                 /*!< Register,  Address offset:0x021C */
	uint32_t RSVD_NO_FUNCTION4[8];
	__I  uint32_t GPIOB_DMD_STS;                                /*!< Register,  Address offset:0x0240 */
	__O  uint32_t GPIOB_IDM_EN;                                 /*!< Register,  Address offset:0x0244 */
	__O  uint32_t GPIOB_ODM_EN;                                 /*!< Register,  Address offset:0x0248 */
	__I  uint32_t GPIOB_OD_STS;                                 /*!< Register,  Address offset:0x024C */
	__O  uint32_t GPIOB_ODL_EN;                                 /*!< Register,  Address offset:0x0250 */
	__O  uint32_t GPIOB_ODH_EN;                                 /*!< Register,  Address offset:0x0254 */
	__O  uint32_t GPIOB_ODT;                                    /*!< Register,  Address offset:0x0258 */
	__I  uint32_t GPIOB_DP_STS;                                 /*!< Register,  Address offset:0x025C */
} GPIO_TypeDef;

/** @} */


/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your defination here

/* Exported types ------------------------------------------------------------*/
/** @addtogroup GPIO_Exported_Types GPIO Exported Types
  * @{
  */

/**
  * @brief  GPIO Init Structure Definition
  */
typedef struct {
	u32 GPIO_Mode;			/*!< Specifies the operating mode for the selected pins.
								This parameter can be a value of @ref GPIO_Mode */

	u32 GPIO_PuPd;			/*!< Specifies the operating Pull-up/Pull down for the selected pins.
								This parameter can be a value of @ref GPIO_Pull_Type */

	u32 GPIO_ITTrigger;		/*!< Specifies interrupt mode is level or edge trigger
								This parameter can be a value of @ref GPIO_INT_Trigger_Type */

	u32 GPIO_ITPolarity;	/*!< Specifies interrupt mode is high or low active trigger
								This parameter can be a value of @ref GPIO_INT_Polarity_Type */

	u32 GPIO_ITDebounce;	/*!< Specifies enable or disable de-bounce for interrupt
								This parameter can be a value of @ref GPIO_INT_Debounce_Type*/
	u32 GPIO_DebounceUs;	/*!< Debounce period in microseconds for interrupt pin.
								Only used when GPIO_ITDebounce == GPIO_INT_DEBOUNCE_ENABLE.
								Set to 0 to use default. */

	u32 GPIO_Pin;			/*!< Specifies the selected pins.
								This parameter contains two parts: Pin[7:5]: port number; Pin[4:0]: pin number */
} GPIO_InitTypeDef;

/**
  * @brief  GPIO IRQ Function Definition
  */
typedef void (*GPIO_IRQ_FUN)(void *Data, u32 Id);

/**
  * @brief  GPIO USER IRQ Function Definition
  */
typedef void (*GPIO_USER_IRQ_FUN)(u32 Id);

struct GPIO_IrqContext {
	u16 IrqPinName;
	u16 IrqIdleEntryFlag;
	GPIO_IRQ_FUN UsrIrqHandler;
	void *UsrIrqData;
};

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup GPIO_Exported_Constants GPIO Exported Constants
  * @{
  */

/** @defgroup GPIO_Mode
  * @{
  */
#define GPIO_Mode_IN			0x0 /*!< GPIO Input Mode */
#define GPIO_Mode_OUT			0x1 /*!< GPIO Output Mode */
#define GPIO_Mode_INT			0x2 /*!< GPIO Interrupt Mode */
/**
  * @}
  */

/** @defgroup GPIO_Pull_Type
  * @{
  */
#define GPIO_PuPd_NOPULL		0x0 /*!< GPIO Interrnal HIGHZ */
#define GPIO_PuPd_DOWN			0x1 /*!< GPIO Interrnal Pull DOWN */
#define GPIO_PuPd_UP			0x2 /*!< GPIO Interrnal Pull UP */
#define GPIO_PuPd_SHUTDOWN		0x3 /*!< GPIO Interrnal PAD shutdown */
/**
  * @}
  */

/** @defgroup GPIO_INT_Trigger_Type
  * @{
  */
#define GPIO_INT_Trigger_LEVEL		0x0 /*!< This interrupt is level trigger */
#define GPIO_INT_Trigger_EDGE		0x1 /*!< This interrupt is edge trigger */
#define GPIO_INT_Trigger_BOTHEDGE	0x2 /*!< This interrupt is both-edge trigger */

#define IS_GPIOIT_LEVEL_TYPE(TYPE)		(((TYPE) == GPIO_INT_Trigger_LEVEL) || \
										((TYPE) == GPIO_INT_Trigger_EDGE) || \
										((TYPE) == GPIO_INT_Trigger_BOTHEDGE))

/**
  * @}
  */

/** @defgroup GPIO_INT_Polarity_Type
  * @{
  */
#define GPIO_INT_POLARITY_ACTIVE_LOW		0x0 /*!< Setting interrupt to low active: falling edge or low level */
#define GPIO_INT_POLARITY_ACTIVE_HIGH		0x1 /*!< Setting interrupt to high active: rising edge or high level */

#define IS_GPIOIT_POLARITY_TYPE(TYPE)		(((TYPE) == GPIO_INT_POLARITY_ACTIVE_LOW) || \
											((TYPE) == GPIO_INT_POLARITY_ACTIVE_HIGH))
/**
  * @}
  */

/** @defgroup GPIO_INT_Debounce_Type
  * @{
  */
#define GPIO_INT_DEBOUNCE_DISABLE			0x0 /*!< Disable interrupt debounce */
#define GPIO_INT_DEBOUNCE_ENABLE			0x1 /*!< Enable interrupt debounce */

#define IS_GPIOIT_DEBOUNCE_TYPE(TYPE)		(((TYPE) == GPIO_INT_DEBOUNCE_DISABLE) || \
											((TYPE) == GPIO_INT_DEBOUNCE_ENABLE))
/** @defgroup MAX_GPIO_IRQ
  * @{
  */
#define MAX_Aon_GPIO_IRQ 6 // Pro3 can have max 6 AON GPIO IRQ pins
#define MAX_GPIO_IRQ 16 // Pro3 can have max 16 IRQ pins for SYSON/CORE GPIO
#define MAX_GPIO_PORT_NUM 8 // Pro3 has a max of 8 GPIO ports - A, B, C, D, E, F, S, P

/**
  * @}
  */


/** @defgroup GPIO_IRQ_ENTRY_USED_MAGIC_NUMBER
  * @{
  */
#define GPIO_IRQ_ENTRY_USED_MAGIC_NUMBER    0x69A5 /*!< Set IrqIdleEntryFlag to this value when table entry is occupied  */
/**
  * @}
  */

/** @defgroup GPIO_IRQ_PIN_TRACKER_UNUSED
  * @{
  */
#define GPIO_IRQ_PIN_TRACKER_UNUSED         0xA5A5 /*!< Set gpio_irq_pin_tracker[] entry to this value when slot is unoccupied  */
/**
  * @}
  */

/** @defgroup GPIO_Pin_State
  * @{
  */
#define GPIO_PIN_LOW			0x0 /*!< Pin state is low */
#define GPIO_PIN_HIGH			0x1 /*!< Pin state is high */
/**
  * @}
  */

/** @defgroup GPIO_Port_Type
  * @{
  */
#define GPIO_PORT_A				0x0 /*!< Port number A */
#define GPIO_PORT_B				0x1 /*!< Port number B */
#define GPIO_PORT_C				0x2 /*!< Port number C */
#define GPIO_PORT_D				0x3 /*!< Port number D */
#define GPIO_PORT_E				0x4 /*!< Port number E */
#define GPIO_PORT_F				0x5 /*!< Port number F */
#define GPIO_PORT_S				0x6 /*!< Port number S */
#define GPIO_PORT_P				0x7 /*!< Port number P */


#define IS_GPIO_PORT_NUM(PORT)	((PORT) == GPIO_PORT_A || \
								(PORT) == GPIO_PORT_B || \
								(PORT) == GPIO_PORT_C || \
								(PORT) == GPIO_PORT_D || \
								(PORT) == GPIO_PORT_E || \
								(PORT) == GPIO_PORT_F || \
								(PORT) == GPIO_PORT_S || \
								(PORT) == GPIO_PORT_P	 \
								)
/**
  * @}
  */

/** @defgroup GPIO_IRQ_Event
  * @{
  */
#define HAL_IRQ_NONE			0x0 /*!< No interrupt event  */
#define HAL_IRQ_RISE			0x1 /*!< Rising edge or high level interrupt event */
#define HAL_IRQ_FALL			0x2 /*!< Falling edge or low level interrupt event */
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup GPIO_Exported_Functions GPIO Exported Functions
  * @{
  */

_LONG_CALL_ void GPIO_WriteBit(u32 GPIO_Pin, u32 BitVal);
_LONG_CALL_ void GPIO_ToggleBit(u32 GPIO_Pin);
_LONG_CALL_ u32 GPIO_ReadDataBit(u32 GPIO_Pin);
_LONG_CALL_ void GPIO_DeInit(u32 GPIO_Pin);
_LONG_CALL_ void GPIO_UserRegIrq(u32 GPIO_Pin, void *IrqHandler, void *IrqData);
_LONG_CALL_ void GPIO_UserUnRegIrq(u32 GPIO_Pin);
_LONG_CALL_ void GPIO_INTMode(u32 GPIO_Pin, u32 NewState, u32 GPIO_ITTrigger, u32 GPIO_ITPolarity, u32 GPIO_ITDebounce);
_LONG_CALL_ void GPIO_INTConfig(u32 GPIO_Pin, u32 NewState);
_LONG_CALL_ void GPIO_Init(GPIO_InitTypeDef  *GPIO_InitStruct);
_LONG_CALL_ u32 GPIO_INTHandler(void *pData);
_LONG_CALL_ void GPIO_Direction(u32 GPIO_Pin, u32 data_direction);
_LONG_CALL_ u32 GPIO_PortRead(u32 GPIO_Port, u32 GPIO_Mask);
_LONG_CALL_ void GPIO_PortWrite(u32 GPIO_Port, u32 GPIO_Mask, u32 Port_State);
_LONG_CALL_ void GPIO_PortDirection(u32 GPIO_Port, u32 GPIO_Mask, u32 data_direction);
_LONG_CALL_ void GPIO_DebounceClock(u32 GPIO_Port, u32 DivideCount);
_LONG_CALL_ void GPIO_DebounceClock_Pro3(u32 GPIO_Pin, u32 debounce_us);
_LONG_CALL_ void GPIO_LevelSync(u32 GPIO_Port, u32 NewState);
_LONG_CALL_ void GPIO_ODInit(u8 PinName, u32 PUInternal);
_LONG_CALL_ void GPIO_ODWriteBit(u8 PinName, u32 PinState);

/**
  * @}
  */



/* MANUAL_GEN_END */

/** @} */

/** @} */
#endif  // AMEBA_GPIO_H
