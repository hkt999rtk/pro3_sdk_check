/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_RXI_WIFI_H
#define SYSREG_RXI_WIFI_H

/* AUTO_GEN_START */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup RXI_WIFI
* @brief RXI_WIFI driver modules
* @{
*/


/** @defgroup NAME
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_DESIGN_NAME                   ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x52300 "RXI300" */
#define RXI_WIFI_DESIGN_NAME(x)                     ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_DESIGN_NAME(x)                 ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup VER
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_DESIGN_VER                    ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x300 "RXI300" */
#define RXI_WIFI_DESIGN_VER(x)                      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_DESIGN_VER(x)                  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REV
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_DESIGN_REV                    ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x20240829 "Reversion, it is different for every spec sync" */
#define RXI_WIFI_DESIGN_REV(x)                      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_DESIGN_REV(x)                  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup INST
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_DESIGN_INST                   ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Instance" */
#define RXI_WIFI_DESIGN_INST(x)                     ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_DESIGN_INST(x)                 ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup IMPL_Y
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_DESIGN_IMPL_YEAR              ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x2024 "Implation year" */
#define RXI_WIFI_DESIGN_IMPL_YEAR(x)                ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_DESIGN_IMPL_YEAR(x)            ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup IMPL_D
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_DESIGN_IMPL_DATE              ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x11291114 "Implation date, it is different for every release" */
#define RXI_WIFI_DESIGN_IMPL_DATE(x)                ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_DESIGN_IMPL_DATE(x)            ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup DEV
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_DESIGN_DEV                    ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x522527 "Designer S2527" */
#define RXI_WIFI_DESIGN_DEV(x)                      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_DESIGN_DEV(x)                  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup PRO_NUM
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_DESIGN_PRO_NUM                ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x7013 "Project name" */
#define RXI_WIFI_DESIGN_PRO_NUM(x)                  ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_DESIGN_PRO_NUM(x)              ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup RBARA0
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RBARA0_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x28000
                                                                                              * "MPC Base address 0, it is 4KB align, the default value is for sys_mem;
                                                                                              * Only can select one address range of two address range(0x0 or 0x1, 0x2 or 0x3, 0x4 or 0x5 .etc) to config, all idau must follow this rule;
                                                                                              * For example:
                                                                                              * if config 0x28000, 0x38000 is also available; 
                                                                                              * If config 0x38000,0x28000 is also available;
                                                                                              * If config 0x28000 and 0x38000, there will have error and bus will hang "
                                                                                              */
#define RXI_WIFI_IDAU_RBARA0_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RBARA0_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RBARA0_READ_AS_ZERO      ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RBARA0_READ_AS_ZERO(x)        ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RBARA0_READ_AS_ZERO(x)    ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RLARA0
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RLARA0_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x2807f "MPC Limit address 0, it is 4KB align, the default value is for sys_mem" */
#define RXI_WIFI_IDAU_RLARA0_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RLARA0_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RLARA0_READ_AS_ZERO      ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RLARA0_READ_AS_ZERO(x)        ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RLARA0_READ_AS_ZERO(x)    ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RBARA1
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RBARA1_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x28080 "MPC Base address 1, it is 4KB align, the default value is for share_mem" */
#define RXI_WIFI_IDAU_RBARA1_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RBARA1_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RBARA1_READ_AS_ZERO      ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RBARA1_READ_AS_ZERO(x)        ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RBARA1_READ_AS_ZERO(x)    ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RLARA1
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RLARA1_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x280bf "MPC Limit address 1, it is 4KB align, the default value is for share_mem" */
#define RXI_WIFI_IDAU_RLARA1_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RLARA1_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RLARA1_READ_AS_ZERO      ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RLARA1_READ_AS_ZERO(x)        ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RLARA1_READ_AS_ZERO(x)    ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RBARA2
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RBARA2_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Base address 2, it is 4KB align" */
#define RXI_WIFI_IDAU_RBARA2_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RBARA2_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RBARA2_READ_AS_ZERO      ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RBARA2_READ_AS_ZERO(x)        ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RBARA2_READ_AS_ZERO(x)    ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RLARA2
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RLARA2_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Limit address 2, it is 4KB align" */
#define RXI_WIFI_IDAU_RLARA2_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RLARA2_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RLARA2_READ_AS_ZERO      ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RLARA2_READ_AS_ZERO(x)        ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RLARA2_READ_AS_ZERO(x)    ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RBARA3
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RBARA3_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Base address 3, it is 4KB align" */
#define RXI_WIFI_IDAU_RBARA3_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RBARA3_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RBARA3_READ_AS_ZERO      ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RBARA3_READ_AS_ZERO(x)        ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RBARA3_READ_AS_ZERO(x)    ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RLARA3
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RLARA3_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Limit address 3, it is 4KB align" */
#define RXI_WIFI_IDAU_RLARA3_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RLARA3_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RLARA3_READ_AS_ZERO      ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RLARA3_READ_AS_ZERO(x)        ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RLARA3_READ_AS_ZERO(x)    ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RBARA4
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RBARA4_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Base address 4, it is 4KB align" */
#define RXI_WIFI_IDAU_RBARA4_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RBARA4_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RBARA4_READ_AS_ZERO      ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RBARA4_READ_AS_ZERO(x)        ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RBARA4_READ_AS_ZERO(x)    ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RLARA4
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RLARA4_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Limit address 4, it is 4KB align" */
#define RXI_WIFI_IDAU_RLARA4_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RLARA4_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RLARA4_READ_AS_ZERO      ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RLARA4_READ_AS_ZERO(x)        ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RLARA4_READ_AS_ZERO(x)    ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RBARA5
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RBARA5_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Base address 5, it is 4KB align" */
#define RXI_WIFI_IDAU_RBARA5_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RBARA5_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RBARA5_READ_AS_ZERO      ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RBARA5_READ_AS_ZERO(x)        ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RBARA5_READ_AS_ZERO(x)    ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RLARA5
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RLARA5_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Limit address 5, it is 4KB align" */
#define RXI_WIFI_IDAU_RLARA5_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RLARA5_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RLARA5_READ_AS_ZERO      ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RLARA5_READ_AS_ZERO(x)        ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RLARA5_READ_AS_ZERO(x)    ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RBARA6
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RBARA6_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Base address 6, it is 4KB align" */
#define RXI_WIFI_IDAU_RBARA6_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RBARA6_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RBARA6_READ_AS_ZERO      ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RBARA6_READ_AS_ZERO(x)        ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RBARA6_READ_AS_ZERO(x)    ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RLARA6
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RLARA6_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Limit address 6, it is 4KB align" */
#define RXI_WIFI_IDAU_RLARA6_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RLARA6_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RLARA6_READ_AS_ZERO      ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RLARA6_READ_AS_ZERO(x)        ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RLARA6_READ_AS_ZERO(x)    ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RBARA7
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RBARA7_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Base address 7, it is 4KB align" */
#define RXI_WIFI_IDAU_RBARA7_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RBARA7_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RBARA7_READ_AS_ZERO      ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RBARA7_READ_AS_ZERO(x)        ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RBARA7_READ_AS_ZERO(x)    ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RLARA7
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RLARA7_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Limit address 7, it is 4KB align" */
#define RXI_WIFI_IDAU_RLARA7_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RLARA7_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RLARA7_READ_AS_ZERO      ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RLARA7_READ_AS_ZERO(x)        ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RLARA7_READ_AS_ZERO(x)    ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup CTRL
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_CTRL_READ_AS_ZERO        ((u32)0x0000FFFF << 16)                  /*!< READ_ONLY 0x0000 "" */
#define RXI_WIFI_IDAU_CTRL_READ_AS_ZERO(x)          ((u32)(((x) & 0x0000FFFF) << 16))
#define RXI_WIFI_GET_IDAU_CTRL_READ_AS_ZERO(x)      ((u32)(((x >> 16) & 0x0000FFFF)))
#define RXI_WIFI_MASK_IDAU_CTRL_ENABLE              ((u32)0x0000FFFF << 0)                   /*!< READ_WRITE 0x0000
                                                                                              * "The control register is use to config address range as non_secure, it enable region 15 to 0 respectively;7~0 are for sys_mem and share_mem, 15~8 are for psrmac_auto;
                                                                                              * 1'b1, non_secure and secure can reach slave, and need config non_secure address range; 
                                                                                              * 1'b0, only secure can reach slave "
                                                                                              */
#define RXI_WIFI_IDAU_CTRL_ENABLE(x)                ((u32)(((x) & 0x0000FFFF) << 0))
#define RXI_WIFI_GET_IDAU_CTRL_ENABLE(x)            ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup LOCK
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_LOCK_READ_AS_ZERO        ((u32)0x7FFFFFFF << 1)                   /*!< READ_ONLY 0x00000000 "" */
#define RXI_WIFI_IDAU_LOCK_READ_AS_ZERO(x)          ((u32)(((x) & 0x7FFFFFFF) << 1))
#define RXI_WIFI_GET_IDAU_LOCK_READ_AS_ZERO(x)      ((u32)(((x >> 1) & 0x7FFFFFFF)))
#define RXI_WIFI_BIT_IDAU_LOCK                      ((u32)0x00000001 << 0)                   /*!< READ_WRITE 0x0 "The Lock register. Set this bit to 1 will disable write attribute of all IDAU registers,  and it can't be wrote to 0 again until system reset." */
/** @} */


/** @defgroup RBARA8
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RBARA8_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x20000 "MPC Base address 8, it is 4KB align, the default value is for psramc_auto" */
#define RXI_WIFI_IDAU_RBARA8_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RBARA8_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RBARA8_READ_AS_ZERO      ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RBARA8_READ_AS_ZERO(x)        ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RBARA8_READ_AS_ZERO(x)    ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RLARA8
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RLARA8_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x27fff "MPC Limit address 8, it is 4KB align, the default value is for psramc_auto" */
#define RXI_WIFI_IDAU_RLARA8_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RLARA8_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RLARA8_READ_AS_ZERO      ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RLARA8_READ_AS_ZERO(x)        ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RLARA8_READ_AS_ZERO(x)    ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RBARA9
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RBARA9_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Base address 9, it is 4KB align" */
#define RXI_WIFI_IDAU_RBARA9_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RBARA9_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RBARA9_READ_AS_ZERO      ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RBARA9_READ_AS_ZERO(x)        ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RBARA9_READ_AS_ZERO(x)    ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RLARA9
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RLARA9_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Limit address 9, it is 4KB align" */
#define RXI_WIFI_IDAU_RLARA9_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RLARA9_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RLARA9_READ_AS_ZERO      ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RLARA9_READ_AS_ZERO(x)        ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RLARA9_READ_AS_ZERO(x)    ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RBARA10
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RBARA10_4KB_ALIGN        ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Base address 10, it is 4KB align" */
#define RXI_WIFI_IDAU_RBARA10_4KB_ALIGN(x)          ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RBARA10_4KB_ALIGN(x)      ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RBARA10_READ_AS_ZERO     ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RBARA10_READ_AS_ZERO(x)       ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RBARA10_READ_AS_ZERO(x)   ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RLARA10
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RLARA10_4KB_ALIGN        ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Limit address 10, it is 4KB align" */
#define RXI_WIFI_IDAU_RLARA10_4KB_ALIGN(x)          ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RLARA10_4KB_ALIGN(x)      ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RLARA10_READ_AS_ZERO     ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RLARA10_READ_AS_ZERO(x)       ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RLARA10_READ_AS_ZERO(x)   ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RBARA11
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RBARA11_4KB_ALIGN        ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Base address 11, it is 4KB align" */
#define RXI_WIFI_IDAU_RBARA11_4KB_ALIGN(x)          ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RBARA11_4KB_ALIGN(x)      ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RBARA11_READ_AS_ZERO     ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RBARA11_READ_AS_ZERO(x)       ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RBARA11_READ_AS_ZERO(x)   ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RLARA11
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RLARA11_4KB_ALIGN        ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Limit address 11, it is 4KB align" */
#define RXI_WIFI_IDAU_RLARA11_4KB_ALIGN(x)          ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RLARA11_4KB_ALIGN(x)      ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RLARA11_READ_AS_ZERO     ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RLARA11_READ_AS_ZERO(x)       ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RLARA11_READ_AS_ZERO(x)   ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RBARA12
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RBARA12_4KB_ALIGN        ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Base address 12, it is 4KB align" */
#define RXI_WIFI_IDAU_RBARA12_4KB_ALIGN(x)          ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RBARA12_4KB_ALIGN(x)      ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RBARA12_READ_AS_ZERO     ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RBARA12_READ_AS_ZERO(x)       ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RBARA12_READ_AS_ZERO(x)   ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RLARA12
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RLARA12_4KB_ALIGN        ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Limit address 12, it is 4KB align" */
#define RXI_WIFI_IDAU_RLARA12_4KB_ALIGN(x)          ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RLARA12_4KB_ALIGN(x)      ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RLARA12_READ_AS_ZERO     ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RLARA12_READ_AS_ZERO(x)       ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RLARA12_READ_AS_ZERO(x)   ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RBARA13
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RBARA13_4KB_ALIGN        ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Base address 13, it is 4KB align" */
#define RXI_WIFI_IDAU_RBARA13_4KB_ALIGN(x)          ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RBARA13_4KB_ALIGN(x)      ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RBARA13_READ_AS_ZERO     ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RBARA13_READ_AS_ZERO(x)       ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RBARA13_READ_AS_ZERO(x)   ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RLARA13
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RLARA13_4KB_ALIGN        ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Limit address 13, it is 4KB align" */
#define RXI_WIFI_IDAU_RLARA13_4KB_ALIGN(x)          ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RLARA13_4KB_ALIGN(x)      ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RLARA13_READ_AS_ZERO     ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RLARA13_READ_AS_ZERO(x)       ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RLARA13_READ_AS_ZERO(x)   ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RBARA14
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RBARA14_4KB_ALIGN        ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Base address 14, it is 4KB align" */
#define RXI_WIFI_IDAU_RBARA14_4KB_ALIGN(x)          ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RBARA14_4KB_ALIGN(x)      ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RBARA14_READ_AS_ZERO     ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RBARA14_READ_AS_ZERO(x)       ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RBARA14_READ_AS_ZERO(x)   ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RLARA14
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RLARA14_4KB_ALIGN        ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Limit address 14, it is 4KB align" */
#define RXI_WIFI_IDAU_RLARA14_4KB_ALIGN(x)          ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RLARA14_4KB_ALIGN(x)      ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RLARA14_READ_AS_ZERO     ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RLARA14_READ_AS_ZERO(x)       ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RLARA14_READ_AS_ZERO(x)   ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RBARA15
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RBARA15_4KB_ALIGN        ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Base address 15, it is 4KB align" */
#define RXI_WIFI_IDAU_RBARA15_4KB_ALIGN(x)          ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RBARA15_4KB_ALIGN(x)      ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RBARA15_READ_AS_ZERO     ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RBARA15_READ_AS_ZERO(x)       ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RBARA15_READ_AS_ZERO(x)   ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RLARA15
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_RLARA15_4KB_ALIGN        ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x00000 "MPC Limit address 15, it is 4KB align" */
#define RXI_WIFI_IDAU_RLARA15_4KB_ALIGN(x)          ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_WIFI_GET_IDAU_RLARA15_4KB_ALIGN(x)      ((u32)(((x >> 12) & 0x000FFFFF)))
#define RXI_WIFI_MASK_IDAU_RBARA15_READ_AS_ZERO     ((u32)0x00000FFF << 0)                   /*!< READ_ONLY 0x000 "" */
#define RXI_WIFI_IDAU_RBARA15_READ_AS_ZERO(x)       ((u32)(((x) & 0x00000FFF) << 0))
#define RXI_WIFI_GET_IDAU_RBARA15_READ_AS_ZERO(x)   ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup IDAU_DIR
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_IDAU_DIR                      ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x00100c01 "" */
#define RXI_WIFI_IDAU_DIR(x)                        ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_IDAU_DIR(x)                    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_PLD0_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_BSTINDEX                  ((u32)0x000000FF << 24)                  /*!< READ_ONLY 0x0 "Burst index of the error request" */
#define RXI_WIFI_ERR_BSTINDEX(x)                    ((u32)(((x) & 0x000000FF) << 24))
#define RXI_WIFI_GET_ERR_BSTINDEX(x)                ((u32)(((x >> 24) & 0x000000FF)))
#define RXI_WIFI_MASK_ERR_BST_LEN                   ((u32)0x000000FF << 16)                  /*!< READ_ONLY 0x0 "Burst length of the error request" */
#define RXI_WIFI_ERR_BST_LEN(x)                     ((u32)(((x) & 0x000000FF) << 16))
#define RXI_WIFI_GET_ERR_BST_LEN(x)                 ((u32)(((x >> 16) & 0x000000FF)))
#define RXI_WIFI_MASK_ERR_BSTTYPE                   ((u32)0x00000007 << 11)                  /*!< READ_ONLY 0x0 "BurstSeq of the error request" */
#define RXI_WIFI_ERR_BSTTYPE(x)                     ((u32)(((x) & 0x00000007) << 11))
#define RXI_WIFI_GET_ERR_BSTTYPE(x)                 ((u32)(((x >> 11) & 0x00000007)))
#define RXI_WIFI_MASK_ERR_CMD                       ((u32)0x00000007 << 8)                   /*!< READ_ONLY 0x0 "Read/write cmd of the error request" */
#define RXI_WIFI_ERR_CMD(x)                         ((u32)(((x) & 0x00000007) << 8))
#define RXI_WIFI_GET_ERR_CMD(x)                     ((u32)(((x >> 8) & 0x00000007)))
#define RXI_WIFI_MASK_ERR_SRC                       ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "Error source of the error request
                                                                                              * 0x01:  m11_m12_m15_address_hole
                                                                                              *            network_km4_mst
                                                                                              *            sic_mst
                                                                                              *            otpc_nst
                                                                                              *            km0_mst
                                                                                              *            core_peri_mst
                                                                                              * 0x02:  m13_m14_m16_address_hole
                                                                                              *            secure_mst
                                                                                              *            dmac0_mst
                                                                                              *            core_mem_mst
                                                                                              *            wmac_mst"
                                                                                              */
#define RXI_WIFI_ERR_SRC(x)                         ((u32)(((x) & 0x000000FF) << 0))
#define RXI_WIFI_GET_ERR_SRC(x)                     ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_PLD1_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_MREQINFO                  ((u32)0x000001FF << 23)                  /*!< READ_ONLY 0x0 "Req_info of the error request" */
#define RXI_WIFI_ERR_MREQINFO(x)                    ((u32)(((x) & 0x000001FF) << 23))
#define RXI_WIFI_GET_ERR_MREQINFO(x)                ((u32)(((x >> 23) & 0x000001FF)))
#define RXI_WIFI_MASK_ERR_SIZE                      ((u32)0x00000007 << 16)                  /*!< READ_ONLY 0x0 "Burst size of the error request" */
#define RXI_WIFI_ERR_SIZE(x)                        ((u32)(((x) & 0x00000007) << 16))
#define RXI_WIFI_GET_ERR_SIZE(x)                    ((u32)(((x >> 16) & 0x00000007)))
#define RXI_WIFI_MASK_ERR_BYTEEN                    ((u32)0x0000FFFF << 0)                   /*!< READ_ONLY 0x0 "Byteen of the error request" */
#define RXI_WIFI_ERR_BYTEEN(x)                      ((u32)(((x) & 0x0000FFFF) << 0))
#define RXI_WIFI_GET_ERR_BYTEEN(x)                  ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup ERR_ID_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_TRANS_ID                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "ID of the error request which include master id and tagid
                                                                                              * master id info bit[9:6]:
                                                                                              * 4'b0000  network_km4_mst
                                                                                              * 4'b0001  sic_mst
                                                                                              * 4'b0010  otpc_mst
                                                                                              * 4'b0011  km0_mst
                                                                                              * 4'b0100  secure_mst
                                                                                              * 4'b0101  dmac0_mst
                                                                                              * 4'b0110  core_mem_mst
                                                                                              * 4'b0111  wmac_mst
                                                                                              * 4'b1000  core_peri_mst & core platform other master
                                                                                              * 4'b1001  core_peri_mst & core platform ca32_axim
                                                                                              * 4'b1010  core_peri_mst & core platform media_km4_mst"
                                                                                              */
#define RXI_WIFI_ERR_TRANS_ID(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ERR_TRANS_ID(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR0_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_ADDR_LOW                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [31:0] of request" */
#define RXI_WIFI_ERR_ADDR_LOW(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ERR_ADDR_LOW(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR1_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_ADDR_HIGH                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [63:32] of request" */
#define RXI_WIFI_ERR_ADDR_HIGH(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ERR_ADDR_HIGH(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_CODE_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_CODE_ERR_TYPE             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "Error code of error request; 
                                                                                              * 8'h01 is for address-hole"
                                                                                              */
#define RXI_WIFI_ERR_CODE_ERR_TYPE(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_WIFI_GET_ERR_CODE_ERR_TYPE(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_INTR_CLR_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_WIFI_BIT_ERR_INTR_CLR_ERR_INFO          ((u32)0x00000001 << 0)                   /*!< WRITE_ONLY 0x0 "Error log interrupt clear, writing 1'b1 to clear interrupt" */
/** @} */


/** @defgroup ERR_PLD0_SECURE_FAULT
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_BSTINDEX                  ((u32)0x000000FF << 24)                  /*!< READ_ONLY 0x0 "Burst index of the error request" */
#define RXI_WIFI_ERR_BSTINDEX(x)                    ((u32)(((x) & 0x000000FF) << 24))
#define RXI_WIFI_GET_ERR_BSTINDEX(x)                ((u32)(((x >> 24) & 0x000000FF)))
#define RXI_WIFI_MASK_ERR_BST_LEN                   ((u32)0x000000FF << 16)                  /*!< READ_ONLY 0x0 "Burst length of the error request" */
#define RXI_WIFI_ERR_BST_LEN(x)                     ((u32)(((x) & 0x000000FF) << 16))
#define RXI_WIFI_GET_ERR_BST_LEN(x)                 ((u32)(((x >> 16) & 0x000000FF)))
#define RXI_WIFI_MASK_ERR_BSTTYPE                   ((u32)0x00000007 << 11)                  /*!< READ_ONLY 0x0 "BurstSeq of the error request" */
#define RXI_WIFI_ERR_BSTTYPE(x)                     ((u32)(((x) & 0x00000007) << 11))
#define RXI_WIFI_GET_ERR_BSTTYPE(x)                 ((u32)(((x >> 11) & 0x00000007)))
#define RXI_WIFI_MASK_ERR_CMD                       ((u32)0x00000007 << 8)                   /*!< READ_ONLY 0x0 "Read/write cmd of the error request" */
#define RXI_WIFI_ERR_CMD(x)                         ((u32)(((x) & 0x00000007) << 8))
#define RXI_WIFI_GET_ERR_CMD(x)                     ((u32)(((x >> 8) & 0x00000007)))
#define RXI_WIFI_MASK_ERR_SRC                       ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "Error source of the error request
                                                                                              * 0x03:  s11_secure_fault
                                                                                              *            sys_mem
                                                                                              * 0x04:  s12_secure_fault
                                                                                              *            share_mem
                                                                                              * 0x05:  s13_secure_fault
                                                                                              *            spic_user
                                                                                              * 0x06:  s14_secure_fault
                                                                                              *            psramc_auto
                                                                                              * 0x07:  s15_s16_secure_fault
                                                                                              *            All s16 apb slave
                                                                                              * 0x08:  s17_secure_fault
                                                                                              *            All s17 apb slave"
                                                                                              */
#define RXI_WIFI_ERR_SRC(x)                         ((u32)(((x) & 0x000000FF) << 0))
#define RXI_WIFI_GET_ERR_SRC(x)                     ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_PLD1_SECURE_FAULT
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_MREQINFO                  ((u32)0x000001FF << 23)                  /*!< READ_ONLY 0x0 "Req_info of the error request" */
#define RXI_WIFI_ERR_MREQINFO(x)                    ((u32)(((x) & 0x000001FF) << 23))
#define RXI_WIFI_GET_ERR_MREQINFO(x)                ((u32)(((x >> 23) & 0x000001FF)))
#define RXI_WIFI_MASK_ERR_SIZE                      ((u32)0x00000007 << 16)                  /*!< READ_ONLY 0x0 "Burst size of the error request" */
#define RXI_WIFI_ERR_SIZE(x)                        ((u32)(((x) & 0x00000007) << 16))
#define RXI_WIFI_GET_ERR_SIZE(x)                    ((u32)(((x >> 16) & 0x00000007)))
#define RXI_WIFI_MASK_ERR_BYTEEN                    ((u32)0x0000FFFF << 0)                   /*!< READ_ONLY 0x0 "Byteen of the error request" */
#define RXI_WIFI_ERR_BYTEEN(x)                      ((u32)(((x) & 0x0000FFFF) << 0))
#define RXI_WIFI_GET_ERR_BYTEEN(x)                  ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup ERR_ID_SECURE_FAULT
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_TRANS_ID                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "ID of the error request which include master id and tagid
                                                                                              * master id info bit[9:6]:
                                                                                              * 4'b0000  network_km4_mst
                                                                                              * 4'b0001  sic_mst
                                                                                              * 4'b0010  otpc_mst
                                                                                              * 4'b0011  km0_mst
                                                                                              * 4'b0100  secure_mst
                                                                                              * 4'b0101  dmac0_mst
                                                                                              * 4'b0110  core_mem_mst
                                                                                              * 4'b0111  wmac_mst
                                                                                              * 4'b1000  core_peri_mst & core platform other master
                                                                                              * 4'b1001  core_peri_mst & core platform ca32_axim
                                                                                              * 4'b1010  core_peri_mst & core platform media_km4_mst"
                                                                                              */
#define RXI_WIFI_ERR_TRANS_ID(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ERR_TRANS_ID(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR0_SECURE_FAULT
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_ADDR_LOW                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [31:0] of request" */
#define RXI_WIFI_ERR_ADDR_LOW(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ERR_ADDR_LOW(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR1_SECURE_FAULT
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_ADDR_HIGH                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [63:32] of request" */
#define RXI_WIFI_ERR_ADDR_HIGH(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ERR_ADDR_HIGH(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_CODE_SECURE_FAULT
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_CODE_ERR_TYPE             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0 "Error code of error request , 8'h02 is for secure-fault" */
#define RXI_WIFI_ERR_CODE_ERR_TYPE(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_WIFI_GET_ERR_CODE_ERR_TYPE(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_INTR_CLR_SECURE_FAULT
 * @brief
 * @{
 */
#define RXI_WIFI_BIT_ERR_INTR_CLR_ERR_INFO          ((u32)0x00000001 << 0)                   /*!< WRITE_ONLY 0x0 "Error log interrupt clear, writing 1'b1 to clear interrupt" */
/** @} */


/** @defgroup ERR_PLD0_TMO_S9_S10
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_BSTINDEX                  ((u32)0x000000FF << 24)                  /*!< READ_ONLY 0x0 "Burst index of the error request" */
#define RXI_WIFI_ERR_BSTINDEX(x)                    ((u32)(((x) & 0x000000FF) << 24))
#define RXI_WIFI_GET_ERR_BSTINDEX(x)                ((u32)(((x >> 24) & 0x000000FF)))
#define RXI_WIFI_MASK_ERR_BST_LEN                   ((u32)0x000000FF << 16)                  /*!< READ_ONLY 0x0 "Burst length of the error request" */
#define RXI_WIFI_ERR_BST_LEN(x)                     ((u32)(((x) & 0x000000FF) << 16))
#define RXI_WIFI_GET_ERR_BST_LEN(x)                 ((u32)(((x >> 16) & 0x000000FF)))
#define RXI_WIFI_MASK_ERR_BSTTYPE                   ((u32)0x00000007 << 11)                  /*!< READ_ONLY 0x0 "BurstSeq of the error request" */
#define RXI_WIFI_ERR_BSTTYPE(x)                     ((u32)(((x) & 0x00000007) << 11))
#define RXI_WIFI_GET_ERR_BSTTYPE(x)                 ((u32)(((x >> 11) & 0x00000007)))
#define RXI_WIFI_MASK_ERR_CMD                       ((u32)0x00000007 << 8)                   /*!< READ_ONLY 0x0 "Read/write cmd of the error request" */
#define RXI_WIFI_ERR_CMD(x)                         ((u32)(((x) & 0x00000007) << 8))
#define RXI_WIFI_GET_ERR_CMD(x)                     ((u32)(((x >> 8) & 0x00000007)))
#define RXI_WIFI_MASK_ERR_SRC                       ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "Error source of the error request
                                                                                              * 0x09:  s9_slave_timeout
                                                                                              *            wifi_peri_slave
                                                                                              * 0x0a:  s10_slave_timeout
                                                                                              *            wifi_mem_slave"
                                                                                              */
#define RXI_WIFI_ERR_SRC(x)                         ((u32)(((x) & 0x000000FF) << 0))
#define RXI_WIFI_GET_ERR_SRC(x)                     ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_PLD1_TMO_S9_S10
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_MREQINFO                  ((u32)0x000001FF << 23)                  /*!< READ_ONLY 0x0 "Req_info of the error request" */
#define RXI_WIFI_ERR_MREQINFO(x)                    ((u32)(((x) & 0x000001FF) << 23))
#define RXI_WIFI_GET_ERR_MREQINFO(x)                ((u32)(((x >> 23) & 0x000001FF)))
#define RXI_WIFI_MASK_ERR_SIZE                      ((u32)0x00000007 << 16)                  /*!< READ_ONLY 0x0 "Burst size of the error request" */
#define RXI_WIFI_ERR_SIZE(x)                        ((u32)(((x) & 0x00000007) << 16))
#define RXI_WIFI_GET_ERR_SIZE(x)                    ((u32)(((x >> 16) & 0x00000007)))
#define RXI_WIFI_MASK_ERR_BYTEEN                    ((u32)0x0000FFFF << 0)                   /*!< READ_ONLY 0x0 "Byteen of the error request" */
#define RXI_WIFI_ERR_BYTEEN(x)                      ((u32)(((x) & 0x0000FFFF) << 0))
#define RXI_WIFI_GET_ERR_BYTEEN(x)                  ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup ERR_ID_TMO_S9_S10
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_TRANS_ID                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "ID of the error request which include master id and tagid
                                                                                              * master id info bit[9:6]:
                                                                                              * 4'b0000  network_km4_mst
                                                                                              * 4'b0001  sic_mst
                                                                                              * 4'b0010  otpc_mst
                                                                                              * 4'b0011  km0_mst
                                                                                              * 4'b0100  secure_mst
                                                                                              * 4'b0101  dmac0_mst
                                                                                              * 4'b0110  core_mem_mst
                                                                                              * 4'b0111  wmac_mst
                                                                                              * 4'b1000  core_peri_mst & core platform other master
                                                                                              * 4'b1001  core_peri_mst & core platform ca32_axim
                                                                                              * 4'b1010  core_peri_mst & core platform media_km4_mst"
                                                                                              */
#define RXI_WIFI_ERR_TRANS_ID(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ERR_TRANS_ID(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR0_TMO_S9_S10
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_ADDR_LOW                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [31:0] of request" */
#define RXI_WIFI_ERR_ADDR_LOW(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ERR_ADDR_LOW(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR1_TMO_S9_S10
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_ADDR_HIGH                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [63:32] of request" */
#define RXI_WIFI_ERR_ADDR_HIGH(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ERR_ADDR_HIGH(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_CODE_TMO_S9_S10
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_CODE_ERR_TYPE             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "Error code of error request ,
                                                                                              * 8'h38 is for axi slave arready timeout, 
                                                                                              * 8'h39 is for axi slave rvalid timeout, 
                                                                                              * 8'h3a is for axi slave awready timeout, 
                                                                                              * 8'h3b is for axi slave wready timeout,  
                                                                                              * 8'h3c is for axi slave bvalid timeout "
                                                                                              */
#define RXI_WIFI_ERR_CODE_ERR_TYPE(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_WIFI_GET_ERR_CODE_ERR_TYPE(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_INTR_CLR_TMO_S9_S10
 * @brief
 * @{
 */
#define RXI_WIFI_BIT_ERR_INTR_CLR_ERR_INFO          ((u32)0x00000001 << 0)                   /*!< WRITE_ONLY 0x0 "Error log interrupt clear, writing 1'b1 to clear interrupt" */
/** @} */


/** @defgroup ERR_PLD0_TMO_S11_S12_S13_S14
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_BSTINDEX                  ((u32)0x000000FF << 24)                  /*!< READ_ONLY 0x0 "Burst index of the error request" */
#define RXI_WIFI_ERR_BSTINDEX(x)                    ((u32)(((x) & 0x000000FF) << 24))
#define RXI_WIFI_GET_ERR_BSTINDEX(x)                ((u32)(((x >> 24) & 0x000000FF)))
#define RXI_WIFI_MASK_ERR_BST_LEN                   ((u32)0x000000FF << 16)                  /*!< READ_ONLY 0x0 "Burst length of the error request" */
#define RXI_WIFI_ERR_BST_LEN(x)                     ((u32)(((x) & 0x000000FF) << 16))
#define RXI_WIFI_GET_ERR_BST_LEN(x)                 ((u32)(((x >> 16) & 0x000000FF)))
#define RXI_WIFI_MASK_ERR_BSTTYPE                   ((u32)0x00000007 << 11)                  /*!< READ_ONLY 0x0 "BurstSeq of the error request" */
#define RXI_WIFI_ERR_BSTTYPE(x)                     ((u32)(((x) & 0x00000007) << 11))
#define RXI_WIFI_GET_ERR_BSTTYPE(x)                 ((u32)(((x >> 11) & 0x00000007)))
#define RXI_WIFI_MASK_ERR_CMD                       ((u32)0x00000007 << 8)                   /*!< READ_ONLY 0x0 "Read/write cmd of the error request" */
#define RXI_WIFI_ERR_CMD(x)                         ((u32)(((x) & 0x00000007) << 8))
#define RXI_WIFI_GET_ERR_CMD(x)                     ((u32)(((x >> 8) & 0x00000007)))
#define RXI_WIFI_MASK_ERR_SRC                       ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "Error source of the error request
                                                                                              * 0x0b:  s11_slave_timeout
                                                                                              *            sys_mem
                                                                                              * 0x0c:  s12_slave_tiemout
                                                                                              *            share_mem
                                                                                              * 0x0d:  s13_1_slave_timeout
                                                                                              *            spic_auto
                                                                                              * 0x0e:  s13_2_slave_tiemout
                                                                                              *            spic_user
                                                                                              * 0x0f:   s14_slave_timeout
                                                                                              *            psramc_auto"
                                                                                              */
#define RXI_WIFI_ERR_SRC(x)                         ((u32)(((x) & 0x000000FF) << 0))
#define RXI_WIFI_GET_ERR_SRC(x)                     ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_PLD1_TMO_S11_S12_S13_S14
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_MREQINFO                  ((u32)0x000001FF << 23)                  /*!< READ_ONLY 0x0 "Req_info of the error request" */
#define RXI_WIFI_ERR_MREQINFO(x)                    ((u32)(((x) & 0x000001FF) << 23))
#define RXI_WIFI_GET_ERR_MREQINFO(x)                ((u32)(((x >> 23) & 0x000001FF)))
#define RXI_WIFI_MASK_ERR_SIZE                      ((u32)0x00000007 << 16)                  /*!< READ_ONLY 0x0 "Burst size of the error request" */
#define RXI_WIFI_ERR_SIZE(x)                        ((u32)(((x) & 0x00000007) << 16))
#define RXI_WIFI_GET_ERR_SIZE(x)                    ((u32)(((x >> 16) & 0x00000007)))
#define RXI_WIFI_MASK_ERR_BYTEEN                    ((u32)0x0000FFFF << 0)                   /*!< READ_ONLY 0x0 "Byteen of the error request" */
#define RXI_WIFI_ERR_BYTEEN(x)                      ((u32)(((x) & 0x0000FFFF) << 0))
#define RXI_WIFI_GET_ERR_BYTEEN(x)                  ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup ERR_ID_TMO_S11_S12_S13_S14
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_TRANS_ID                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "ID of the error request which include master id and tagid
                                                                                              * master id info bit[9:6]:
                                                                                              * 4'b0000  network_km4_mst
                                                                                              * 4'b0001  sic_mst
                                                                                              * 4'b0010  otpc_mst
                                                                                              * 4'b0011  km0_mst
                                                                                              * 4'b0100  secure_mst
                                                                                              * 4'b0101  dmac0_mst
                                                                                              * 4'b0110  core_mem_mst
                                                                                              * 4'b0111  wmac_mst
                                                                                              * 4'b1000  core_peri_mst & core platform other master
                                                                                              * 4'b1001  core_peri_mst & core platform ca32_axim
                                                                                              * 4'b1010  core_peri_mst & core platform media_km4_mst"
                                                                                              */
#define RXI_WIFI_ERR_TRANS_ID(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ERR_TRANS_ID(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR0_TMO_S11_S12_S13_S14
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_ADDR_LOW                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [31:0] of request" */
#define RXI_WIFI_ERR_ADDR_LOW(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ERR_ADDR_LOW(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR1_TMO_S11_S12_S13_S14
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_ADDR_HIGH                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [63:32] of request" */
#define RXI_WIFI_ERR_ADDR_HIGH(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ERR_ADDR_HIGH(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_CODE_TMO_S11_S12_S13_S14
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_CODE_ERR_TYPE             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "Error code of error request , 
                                                                                              * 8'h38 is for axi slave arready timeout, 
                                                                                              * 8'h39 is for axi slave rvalid timeout, 
                                                                                              * 8'h3a is for axi slave awready timeout, 
                                                                                              * 8'h3b is for axi slave wready timeout,  
                                                                                              * 8'h3c is for axi slave bvalid timeout "
                                                                                              */
#define RXI_WIFI_ERR_CODE_ERR_TYPE(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_WIFI_GET_ERR_CODE_ERR_TYPE(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_INTR_CLR_TMO_S11_S12_S13_S14
 * @brief
 * @{
 */
#define RXI_WIFI_BIT_ERR_INTR_CLR_ERR_INFO          ((u32)0x00000001 << 0)                   /*!< WRITE_ONLY 0x0 "Error log interrupt clear, writing 1'b1 to clear interrupt" */
/** @} */


/** @defgroup ERR_PLD0_TMO_S15
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_BSTINDEX                  ((u32)0x000000FF << 24)                  /*!< READ_ONLY 0x0 "Burst index of the error request" */
#define RXI_WIFI_ERR_BSTINDEX(x)                    ((u32)(((x) & 0x000000FF) << 24))
#define RXI_WIFI_GET_ERR_BSTINDEX(x)                ((u32)(((x >> 24) & 0x000000FF)))
#define RXI_WIFI_MASK_ERR_BST_LEN                   ((u32)0x000000FF << 16)                  /*!< READ_ONLY 0x0 "Burst length of the error request" */
#define RXI_WIFI_ERR_BST_LEN(x)                     ((u32)(((x) & 0x000000FF) << 16))
#define RXI_WIFI_GET_ERR_BST_LEN(x)                 ((u32)(((x >> 16) & 0x000000FF)))
#define RXI_WIFI_MASK_ERR_BSTTYPE                   ((u32)0x00000007 << 11)                  /*!< READ_ONLY 0x0 "BurstSeq of the error request" */
#define RXI_WIFI_ERR_BSTTYPE(x)                     ((u32)(((x) & 0x00000007) << 11))
#define RXI_WIFI_GET_ERR_BSTTYPE(x)                 ((u32)(((x >> 11) & 0x00000007)))
#define RXI_WIFI_MASK_ERR_CMD                       ((u32)0x00000007 << 8)                   /*!< READ_ONLY 0x0 "Read/write cmd of the error request" */
#define RXI_WIFI_ERR_CMD(x)                         ((u32)(((x) & 0x00000007) << 8))
#define RXI_WIFI_GET_ERR_CMD(x)                     ((u32)(((x >> 8) & 0x00000007)))
#define RXI_WIFI_MASK_ERR_SRC                       ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "Error source of the error request
                                                                                              * 0x10:  s15_secure_clk_domain_slave_timeout
                                                                                              *            S15 apb slave in secure clock domain
                                                                                              * 0x11:  s15_7_slave_timeout
                                                                                              *            trng_slv_s15_7"
                                                                                              */
#define RXI_WIFI_ERR_SRC(x)                         ((u32)(((x) & 0x000000FF) << 0))
#define RXI_WIFI_GET_ERR_SRC(x)                     ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_PLD1_TMO_S15
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_MREQINFO                  ((u32)0x000001FF << 23)                  /*!< READ_ONLY 0x0 "Req_info of the error request" */
#define RXI_WIFI_ERR_MREQINFO(x)                    ((u32)(((x) & 0x000001FF) << 23))
#define RXI_WIFI_GET_ERR_MREQINFO(x)                ((u32)(((x >> 23) & 0x000001FF)))
#define RXI_WIFI_MASK_ERR_SIZE                      ((u32)0x00000007 << 16)                  /*!< READ_ONLY 0x0 "Burst size of the error request" */
#define RXI_WIFI_ERR_SIZE(x)                        ((u32)(((x) & 0x00000007) << 16))
#define RXI_WIFI_GET_ERR_SIZE(x)                    ((u32)(((x >> 16) & 0x00000007)))
#define RXI_WIFI_MASK_ERR_BYTEEN                    ((u32)0x0000FFFF << 0)                   /*!< READ_ONLY 0x0 "Byteen of the error request" */
#define RXI_WIFI_ERR_BYTEEN(x)                      ((u32)(((x) & 0x0000FFFF) << 0))
#define RXI_WIFI_GET_ERR_BYTEEN(x)                  ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup ERR_ID_TMO_S15
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_TRANS_ID                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "ID of the error request which include master id and tagid
                                                                                              * master id info bit[9:6]:
                                                                                              * 4'b0000  network_km4_mst
                                                                                              * 4'b0001  sic_mst
                                                                                              * 4'b0010  otpc_mst
                                                                                              * 4'b0011  km0_mst
                                                                                              * 4'b0100  secure_mst
                                                                                              * 4'b0101  dmac0_mst
                                                                                              * 4'b0110  core_mem_mst
                                                                                              * 4'b0111  wmac_mst
                                                                                              * 4'b1000  core_peri_mst & core platform other master
                                                                                              * 4'b1001  core_peri_mst & core platform ca32_axim
                                                                                              * 4'b1010  core_peri_mst & core platform media_km4_mst"
                                                                                              */
#define RXI_WIFI_ERR_TRANS_ID(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ERR_TRANS_ID(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR0_TMO_S15
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_ADDR_LOW                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [31:0] of request" */
#define RXI_WIFI_ERR_ADDR_LOW(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ERR_ADDR_LOW(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR1_TMO_S15
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_ADDR_HIGH                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [63:32] of request" */
#define RXI_WIFI_ERR_ADDR_HIGH(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ERR_ADDR_HIGH(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_CODE_TMO_S15
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_CODE_ERR_TYPE             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "Error code of error request , 
                                                                                              * 8'h21 is for apb pready timeout"
                                                                                              */
#define RXI_WIFI_ERR_CODE_ERR_TYPE(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_WIFI_GET_ERR_CODE_ERR_TYPE(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_INTR_CLR_TMO_S15
 * @brief
 * @{
 */
#define RXI_WIFI_BIT_ERR_INTR_CLR_ERR_INFO          ((u32)0x00000001 << 0)                   /*!< WRITE_ONLY 0x0 "Error log interrupt clear, writing 1'b1 to clear interrupt" */
/** @} */


/** @defgroup ERR_PLD0_TMO_S16
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_BSTINDEX                  ((u32)0x000000FF << 24)                  /*!< READ_ONLY 0x0 "Burst index of the error request" */
#define RXI_WIFI_ERR_BSTINDEX(x)                    ((u32)(((x) & 0x000000FF) << 24))
#define RXI_WIFI_GET_ERR_BSTINDEX(x)                ((u32)(((x >> 24) & 0x000000FF)))
#define RXI_WIFI_MASK_ERR_BST_LEN                   ((u32)0x000000FF << 16)                  /*!< READ_ONLY 0x0 "Burst length of the error request" */
#define RXI_WIFI_ERR_BST_LEN(x)                     ((u32)(((x) & 0x000000FF) << 16))
#define RXI_WIFI_GET_ERR_BST_LEN(x)                 ((u32)(((x >> 16) & 0x000000FF)))
#define RXI_WIFI_MASK_ERR_BSTTYPE                   ((u32)0x00000007 << 11)                  /*!< READ_ONLY 0x0 "BurstSeq of the error request" */
#define RXI_WIFI_ERR_BSTTYPE(x)                     ((u32)(((x) & 0x00000007) << 11))
#define RXI_WIFI_GET_ERR_BSTTYPE(x)                 ((u32)(((x >> 11) & 0x00000007)))
#define RXI_WIFI_MASK_ERR_CMD                       ((u32)0x00000007 << 8)                   /*!< READ_ONLY 0x0 "Read/write cmd of the error request" */
#define RXI_WIFI_ERR_CMD(x)                         ((u32)(((x) & 0x00000007) << 8))
#define RXI_WIFI_GET_ERR_CMD(x)                     ((u32)(((x >> 8) & 0x00000007)))
#define RXI_WIFI_MASK_ERR_SRC                       ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "Error source of the error request
                                                                                              * 0x12:  s16_3_slave_timeout
                                                                                              *            psramc_user
                                                                                              * 0x13:  s16_4_slave_timeout
                                                                                              *            psram_phy
                                                                                              * 0x14:  s16_slow_pclk_0_domain_slave_timeout
                                                                                              *            S16 apb_slave in slow_pclk domain in mcuoff power domain
                                                                                              * 0x15:  s16_2_slave_timeout
                                                                                              *            flash_sec
                                                                                              * 0x16:  s16_slow_pclk_1_domain_slave_timeout
                                                                                              *            S16 apb slave in slow_pclk domain in core power domain
                                                                                              * 0x17:  s16_13_slave_timeout
                                                                                              *            nand_ecc"
                                                                                              */
#define RXI_WIFI_ERR_SRC(x)                         ((u32)(((x) & 0x000000FF) << 0))
#define RXI_WIFI_GET_ERR_SRC(x)                     ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_PLD1_TMO_S16
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_MREQINFO                  ((u32)0x000001FF << 23)                  /*!< READ_ONLY 0x0 "Req_info of the error request" */
#define RXI_WIFI_ERR_MREQINFO(x)                    ((u32)(((x) & 0x000001FF) << 23))
#define RXI_WIFI_GET_ERR_MREQINFO(x)                ((u32)(((x >> 23) & 0x000001FF)))
#define RXI_WIFI_MASK_ERR_SIZE                      ((u32)0x00000007 << 16)                  /*!< READ_ONLY 0x0 "Burst size of the error request" */
#define RXI_WIFI_ERR_SIZE(x)                        ((u32)(((x) & 0x00000007) << 16))
#define RXI_WIFI_GET_ERR_SIZE(x)                    ((u32)(((x >> 16) & 0x00000007)))
#define RXI_WIFI_MASK_ERR_BYTEEN                    ((u32)0x0000FFFF << 0)                   /*!< READ_ONLY 0x0 "Byteen of the error request" */
#define RXI_WIFI_ERR_BYTEEN(x)                      ((u32)(((x) & 0x0000FFFF) << 0))
#define RXI_WIFI_GET_ERR_BYTEEN(x)                  ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup ERR_ID_TMO_S16
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_TRANS_ID                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "ID of the error request which include master id and tagid
                                                                                              * master id info bit[9:6]:
                                                                                              * 4'b0000  network_km4_mst
                                                                                              * 4'b0001  sic_mst
                                                                                              * 4'b0010  otpc_mst
                                                                                              * 4'b0011  km0_mst
                                                                                              * 4'b0100  secure_mst
                                                                                              * 4'b0101  dmac0_mst
                                                                                              * 4'b0110  core_mem_mst
                                                                                              * 4'b0111  wmac_mst
                                                                                              * 4'b1000  core_peri_mst & core platform other master
                                                                                              * 4'b1001  core_peri_mst & core platform ca32_axim
                                                                                              * 4'b1010  core_peri_mst & core platform media_km4_mst"
                                                                                              */
#define RXI_WIFI_ERR_TRANS_ID(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ERR_TRANS_ID(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR0_TMO_S16
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_ADDR_LOW                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [31:0] of request" */
#define RXI_WIFI_ERR_ADDR_LOW(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ERR_ADDR_LOW(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR1_TMO_S16
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_ADDR_HIGH                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [63:32] of request" */
#define RXI_WIFI_ERR_ADDR_HIGH(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ERR_ADDR_HIGH(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_CODE_TMO_S16
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_CODE_ERR_TYPE             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "Error code of error request ,  
                                                                                              * 8'h21 is for apb pready timeout,  
                                                                                              * 8'h38 is for axi slave arready timeout, 
                                                                                              * 8'h39 is for axi slave rvalid timeout, 
                                                                                              * 8'h3a is for axi slave awready timeout, 
                                                                                              * 8'h3b is for axi slave wready timeout,  
                                                                                              * 8'h3c is for axi slave bvalid timeout "
                                                                                              */
#define RXI_WIFI_ERR_CODE_ERR_TYPE(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_WIFI_GET_ERR_CODE_ERR_TYPE(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_INTR_CLR_TMO_S16
 * @brief
 * @{
 */
#define RXI_WIFI_BIT_ERR_INTR_CLR_ERR_INFO          ((u32)0x00000001 << 0)                   /*!< WRITE_ONLY 0x0 "Error log interrupt clear, writing 1'b1 to clear interrupt" */
/** @} */


/** @defgroup ERR_PLD0_TMO_S17
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_BSTINDEX                  ((u32)0x000000FF << 24)                  /*!< READ_ONLY 0x0 "Burst index of the error request" */
#define RXI_WIFI_ERR_BSTINDEX(x)                    ((u32)(((x) & 0x000000FF) << 24))
#define RXI_WIFI_GET_ERR_BSTINDEX(x)                ((u32)(((x >> 24) & 0x000000FF)))
#define RXI_WIFI_MASK_ERR_BST_LEN                   ((u32)0x000000FF << 16)                  /*!< READ_ONLY 0x0 "Burst length of the error request" */
#define RXI_WIFI_ERR_BST_LEN(x)                     ((u32)(((x) & 0x000000FF) << 16))
#define RXI_WIFI_GET_ERR_BST_LEN(x)                 ((u32)(((x >> 16) & 0x000000FF)))
#define RXI_WIFI_MASK_ERR_BSTTYPE                   ((u32)0x00000007 << 11)                  /*!< READ_ONLY 0x0 "BurstSeq of the error request" */
#define RXI_WIFI_ERR_BSTTYPE(x)                     ((u32)(((x) & 0x00000007) << 11))
#define RXI_WIFI_GET_ERR_BSTTYPE(x)                 ((u32)(((x >> 11) & 0x00000007)))
#define RXI_WIFI_MASK_ERR_CMD                       ((u32)0x00000007 << 8)                   /*!< READ_ONLY 0x0 "Read/write cmd of the error request" */
#define RXI_WIFI_ERR_CMD(x)                         ((u32)(((x) & 0x00000007) << 8))
#define RXI_WIFI_GET_ERR_CMD(x)                     ((u32)(((x >> 8) & 0x00000007)))
#define RXI_WIFI_MASK_ERR_SRC                       ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "Error source of the error request
                                                                                              * 0x18:  s17_1_tslave_imeout
                                                                                              *            wmac_slv
                                                                                              * 0x19:  s17_slow_pclk_timeout
                                                                                              *            S17 apb slave in slow_pclk domain
                                                                                              * 0x1a:  s17_17_slave_timeout
                                                                                              *            aon_reg"
                                                                                              */
#define RXI_WIFI_ERR_SRC(x)                         ((u32)(((x) & 0x000000FF) << 0))
#define RXI_WIFI_GET_ERR_SRC(x)                     ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_PLD1_TMO_S17
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_MREQINFO                  ((u32)0x000001FF << 23)                  /*!< READ_ONLY 0x0 "Req_info of the error request" */
#define RXI_WIFI_ERR_MREQINFO(x)                    ((u32)(((x) & 0x000001FF) << 23))
#define RXI_WIFI_GET_ERR_MREQINFO(x)                ((u32)(((x >> 23) & 0x000001FF)))
#define RXI_WIFI_MASK_ERR_SIZE                      ((u32)0x00000007 << 16)                  /*!< READ_ONLY 0x0 "Burst size of the error request" */
#define RXI_WIFI_ERR_SIZE(x)                        ((u32)(((x) & 0x00000007) << 16))
#define RXI_WIFI_GET_ERR_SIZE(x)                    ((u32)(((x >> 16) & 0x00000007)))
#define RXI_WIFI_MASK_ERR_BYTEEN                    ((u32)0x0000FFFF << 0)                   /*!< READ_ONLY 0x0 "Byteen of the error request" */
#define RXI_WIFI_ERR_BYTEEN(x)                      ((u32)(((x) & 0x0000FFFF) << 0))
#define RXI_WIFI_GET_ERR_BYTEEN(x)                  ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup ERR_ID_TMO_S17
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_TRANS_ID                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "ID of the error request which include master id and tagid
                                                                                              * master id info bit[9:6]:
                                                                                              * 4'b0000  network_km4_mst
                                                                                              * 4'b0001  sic_mst
                                                                                              * 4'b0010  otpc_mst
                                                                                              * 4'b0011  km0_mst
                                                                                              * 4'b0100  secure_mst
                                                                                              * 4'b0101  dmac0_mst
                                                                                              * 4'b0110  core_mem_mst
                                                                                              * 4'b0111  wmac_mst
                                                                                              * 4'b1000  core_peri_mst & core platform other master
                                                                                              * 4'b1001  core_peri_mst & core platform ca32_axim
                                                                                              * 4'b1010  core_peri_mst & core platform media_km4_mst"
                                                                                              */
#define RXI_WIFI_ERR_TRANS_ID(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ERR_TRANS_ID(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR0_TMO_S17
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_ADDR_LOW                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [31:0] of request" */
#define RXI_WIFI_ERR_ADDR_LOW(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ERR_ADDR_LOW(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR1_TMO_S17
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_ADDR_HIGH                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [63:32] of request" */
#define RXI_WIFI_ERR_ADDR_HIGH(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ERR_ADDR_HIGH(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_CODE_TMO_S17
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ERR_CODE_ERR_TYPE             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "Error code of error request ,  8'h21 is for apb pready timeout,
                                                                                              * 8'h22 is for ahb slave adapter timeout"
                                                                                              */
#define RXI_WIFI_ERR_CODE_ERR_TYPE(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_WIFI_GET_ERR_CODE_ERR_TYPE(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_INTR_CLR_TMO_S17
 * @brief
 * @{
 */
#define RXI_WIFI_BIT_ERR_INTR_CLR_ERR_INFO          ((u32)0x00000001 << 0)                   /*!< WRITE_ONLY 0x0 "Error log interrupt clear, writing 1'b1 to clear interrupt" */
/** @} */


/** @defgroup CTR_AND_SECURE_REG_0
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_CTR_REG_SECURE_CFG_GROUP_0    ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x7fffff70
                                                                                              * "bit[31],reserved; 
                                                                                              * bit[30] is for S16_24 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave; 
                                                                                              * bit[29] is for S16_23 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave; 
                                                                                              * bit[28] is for S16_22 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[27] is for S16_21 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[26] is for S16_20 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[25] is for S16_19 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[24] is for S16_18 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[23] is for S16_17 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[22] is for S16_16 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[21] is for S16_15 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[20] is for S16_14 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[19] is for S16_13 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[18] is for S16_12 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[17] is for S16_11 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[16] is for S16_10 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[15] is for S16_9 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[14] is for S16_8 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[13] is for S16_6 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[12] is for S16_5 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[11] is for S16_4 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[10] is for S16_3 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[9] is for S16_2 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[8] is for S13_2 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[7],reversed; 
                                                                                              * bit[6], m16 master ar cmd secureconfig, 1'b1 is for no_secure, 1'b0 is for secure;
                                                                                              * bit[5], m16 master aw cmd secure config, 1'b1 is for no_secure, 1'b0 is for secure;
                                                                                              * bit[4], m12_1 master cmd secure config, 1'b1 is for non_secure, 1'b0 is for secure;
                                                                                              * bit[3:1], reversed;
                                                                                              * bit[0] nand_ecc_icg_disenable, 1'b1 is for not use icg"
                                                                                              */
#define RXI_WIFI_CTR_REG_SECURE_CFG_GROUP_0(x)      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_CTR_REG_SECURE_CFG_GROUP_0(x)  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup SECURE_REG_1
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_SECURE_CFG_GROUP_1            ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x000007ff
                                                                                              * "bit[31:11],reserved; 
                                                                                              * bit[10] is for S17_23 secure config,1 'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[9] is for S17_22 secure config,1 'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[8] is for S17_21 secure config,1 'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave; 
                                                                                              * bit[7] is for S17_20 secure config,1 'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave; 
                                                                                              * bit[6] is for S17_19 secure config,1 'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave; 
                                                                                              * bit[5] is for S17_18 secure config,1 'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave; 
                                                                                              * bit[4] is for S17_17 secure config,1 'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;  
                                                                                              * bit[3] is for S17_14 secure config,1 'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;  
                                                                                              * bit[2] is for S17_8 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[1] is for S17_5 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;
                                                                                              * bit[0] is for S17_1 secure config, 1'b1 is for secure can reach salve, 1'b0 is for non_secure and secure can reach slave;"
                                                                                              */
#define RXI_WIFI_SECURE_CFG_GROUP_1(x)              ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_SECURE_CFG_GROUP_1(x)          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup SRAM_RD_WEIGHT
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_SYS_MEM_RD_WEIGHT             ((u32)0x00007FFF << 17)                  /*!< READ_WRITE 0x01ff "Sys memey read weight for sram ctrl" */
#define RXI_WIFI_SYS_MEM_RD_WEIGHT(x)               ((u32)(((x) & 0x00007FFF) << 17))
#define RXI_WIFI_GET_SYS_MEM_RD_WEIGHT(x)           ((u32)(((x >> 17) & 0x00007FFF)))
#define RXI_WIFI_MASK_SHARE_MEM_RD_WEIGHT           ((u32)0x0001FFFF << 0)                   /*!< READ_WRITE 0x01ff "Share memey read weight for sram ctrl" */
#define RXI_WIFI_SHARE_MEM_RD_WEIGHT(x)             ((u32)(((x) & 0x0001FFFF) << 0))
#define RXI_WIFI_GET_SHARE_MEM_RD_WEIGHT(x)         ((u32)(((x >> 0) & 0x0001FFFF)))
/** @} */


/** @defgroup ERROR_MASTER_ID
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_WIFI_TO_CORE_ERROR_MASTER_ID  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "master id info:
                                                                                              * 4'b0000  network_km4_mst
                                                                                              * 4'b0001  sic_mst
                                                                                              * 4'b0010  otpc_mst
                                                                                              * 4'b0011  km0_mst
                                                                                              * 4'b0100  secure_mst
                                                                                              * 4'b0101  dmac0_mst
                                                                                              * 4'b0110  core_mem_mst
                                                                                              * 4'b0111  wmac_mst
                                                                                              * 4'b1000  core_peri_mst & core platform other master
                                                                                              * 4'b1001  core_peri_mst & core platform ca32_axim
                                                                                              * 4'b1010  core_peri_mst & core platform media_km4_mst
                                                                                              * [31:16] Reserved;
                                                                                              * [15:12] wifi_mem_slv r_error_respone master_id record;
                                                                                              * [11:8]   wifi_mem_slv b_error_respone master_id record;
                                                                                              * [7:4]     wifi_peri_slv r_error_respone master_id record;
                                                                                              * [3:0]     wifi_peri_slv b_error_respone master_id record"
                                                                                              */
#define RXI_WIFI_WIFI_TO_CORE_ERROR_MASTER_ID(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_WIFI_TO_CORE_ERROR_MASTER_ID(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup TIMEOUT_GUARDIAN_EN
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_TIMEOUT_GUARDIAN_EN           ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0
                                                                                              * "Enable timeout check function respectively for slave, 1'b1 is enable,1'b0 is disenable
                                                                                              * bit[17] aon_reg_s17_17;
                                                                                              * bit[16] s17_slow_pclk_apb;
                                                                                              * bit[15] wmac_slv_s17_1;
                                                                                              * bit[14] nand_ecc_sa_lite;
                                                                                              * bit[13] s16-slow_pclk_1;
                                                                                              * bit[12] flash_sec_s16_2;
                                                                                              * bit[11] s16_slow_pclk_0_apb;
                                                                                              * bit[10] psram_phy_s16_4;
                                                                                              * bit[9] psramc_user_s16_3;
                                                                                              * bit[8] trng_slv_s15_7;
                                                                                              * bit[7] s15_secure_clk_apb;
                                                                                              * bit[6] psramc_auto_s14;
                                                                                              * bit[5] spic_user_s13_2;
                                                                                              * bit[4] spic_auto_s13_1;
                                                                                              * bit[3] share_mem_s12;
                                                                                              * bit[2] sys_mem_s11;
                                                                                              * bit[1] wifi_mem_slv_s10;
                                                                                              * bit[0] wifi_peri_slv_s9"
                                                                                              */
#define RXI_WIFI_TIMEOUT_GUARDIAN_EN(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_TIMEOUT_GUARDIAN_EN(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup TIMEOUT_CLEAR_N
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_TIMEOUT_CLEAR_N               ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0xffffffff
                                                                                              * "The timeout clear register:  after timeout event happens, write 0 to reset the corresponding component,  This bit will be automatically set to 1 by hardware
                                                                                              * bit[17] aon_reg_s17_17;
                                                                                              * bit[16] s17_slow_pclk_apb;
                                                                                              * bit[15] wmac_slv_s17_1;
                                                                                              * bit[14] nand_ecc_sa_lite;
                                                                                              * bit[13] s16-slow_pclk_1;
                                                                                              * bit[12] flash_sec_s16_2;
                                                                                              * bit[11] s16_slow_pclk_0_apb;
                                                                                              * bit[10] psram_phy_s16_4;
                                                                                              * bit[9] psramc_user_s16_3;
                                                                                              * bit[8] trng_slv_s15_7;
                                                                                              * bit[7] s15_secure_clk_apb;
                                                                                              * bit[6] psramc_auto_s14;
                                                                                              * bit[5] spic_user_s13_2;
                                                                                              * bit[4] spic_auto_s13_1;
                                                                                              * bit[3] share_mem_s12;
                                                                                              * bit[2] sys_mem_s11;
                                                                                              * bit[1] wifi_mem_slv_s10;
                                                                                              * bit[0] wifi_peri_slv_s9"
                                                                                              */
#define RXI_WIFI_TIMEOUT_CLEAR_N(x)                 ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_TIMEOUT_CLEAR_N(x)             ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup TIMEOUT_MON_CFG_0
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_TIMEOUT_MON_CFG_0             ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x000f00ff
                                                                                              * "The timeout monitor config register for S9 S10, contains one threshold and one grade_select
                                                                                              * threshold = TIMEOUT_MON_CFG[7:0]
                                                                                              * grade_select = TIMEOUT_MON_CFG[19:16]"
                                                                                              */
#define RXI_WIFI_TIMEOUT_MON_CFG_0(x)               ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_TIMEOUT_MON_CFG_0(x)           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup TIMEOUT_MON_CFG_1
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_TIMEOUT_MON_CFG_1             ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x000f00ff
                                                                                              * "The timeout monitor config register for S11 S12 S13 S14, contains one threshold and one grade_select
                                                                                              * threshold = TIMEOUT_MON_CFG[7:0]
                                                                                              * grade_select = TIMEOUT_MON_CFG[19:16]"
                                                                                              */
#define RXI_WIFI_TIMEOUT_MON_CFG_1(x)               ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_TIMEOUT_MON_CFG_1(x)           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup TIMEOUT_MON_CFG_2
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_TIMEOUT_MON_CFG_2             ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x000f00ff
                                                                                              * "The timeout monitor config register for S15, contains one threshold and one grade_select
                                                                                              * threshold = TIMEOUT_MON_CFG[7:0]
                                                                                              * grade_select = TIMEOUT_MON_CFG[19:16]"
                                                                                              */
#define RXI_WIFI_TIMEOUT_MON_CFG_2(x)               ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_TIMEOUT_MON_CFG_2(x)           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup TIMEOUT_MON_CFG_3
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_TIMEOUT_MON_CFG_3             ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x000f00ff
                                                                                              * "The timeout monitor config register for S16, contains one threshold and one grade_select
                                                                                              * threshold = TIMEOUT_MON_CFG[7:0]
                                                                                              * grade_select = TIMEOUT_MON_CFG[19:16]"
                                                                                              */
#define RXI_WIFI_TIMEOUT_MON_CFG_3(x)               ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_TIMEOUT_MON_CFG_3(x)           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup TIMEOUT_MON_CFG_4
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_TIMEOUT_MON_CFG_4             ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x000f00ff
                                                                                              * "The timeout monitor config register for S17, contains one threshold and one grade_select
                                                                                              * threshold = TIMEOUT_MON_CFG[7:0]
                                                                                              * grade_select = TIMEOUT_MON_CFG[19:16]"
                                                                                              */
#define RXI_WIFI_TIMEOUT_MON_CFG_4(x)               ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_TIMEOUT_MON_CFG_4(x)           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup GLOBAL_INTR_RAW_STATUS
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_ELR_INTR_RECORD               ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "It recorde elr_intr which include address hole, secure fault, timeout
                                                                                              * Bit [6]   s17 timeout
                                                                                              * bit [5]   s16 timeout
                                                                                              * bit [4]   s15 timeout
                                                                                              * bit [3]   s11 s12 s13 s14 timeout
                                                                                              * bit [2]   s9 s10 timeout
                                                                                              * bit [1]   secure fault
                                                                                              * bit [0]   address hole"
                                                                                              */
#define RXI_WIFI_ELR_INTR_RECORD(x)                 ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_ELR_INTR_RECORD(x)             ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup MASK_CONTROL
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_MASK_CONTROL                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0
                                                                                              * "The mask control register
                                                                                              * 
                                                                                              * The ELR0_0 is controlled by MASK_CONTROL[1:0], ELR0_1 is controlled by MASK_CONTROL[3:2], and so on.
                                                                                              * 
                                                                                              * MASK_CONTROL[N+1:N] = 2'b00 -> interrupt will not be masked
                                                                                              * 
                                                                                              * MASK_CONTROL[N+1:N] = 2'b01 -> interrupt will be masked depends on ERR_CODE
                                                                                              * "
                                                                                              */
#define RXI_WIFI_MASK_CONTROL(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_MASK_CONTROL(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup MASK_ERR_CODE_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_MASK_ERR_CODE                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0
                                                                                              * "Mask address hole interrupt when ERR_CODE matches.The error_code will be re-encoded to one-hot signals.
                                                                                              * If `ERR_CODE is 8'h1(address_hole) -> re-encode MASK_ERR_CODE as 32'h1;"
                                                                                              */
#define RXI_WIFI_MASK_ERR_CODE(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_MASK_ERR_CODE(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup MASK_ERR_CODE_SECURE_FAULT
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_MASK_ERR_CODE                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0
                                                                                              * "Mask secure fault interrupt when ERR_CODE matches
                                                                                              * If `ERR_CODE is 8'h2(secure_fault) -> re-encode MASK_ERR_CODE as 32'h2;"
                                                                                              */
#define RXI_WIFI_MASK_ERR_CODE(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_MASK_ERR_CODE(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup MASK_ERR_CODE_TMO_S9_S10
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_MASK_ERR_CODE                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0
                                                                                              * "Mask S9 S10 timeout interrupt when ERR_CODE matches
                                                                                              * If `ERR_CODE is 8'h38(arready timeout) -> re-encode MASK_ERR_CODE as 32'h10;
                                                                                              * If `ERR_CODE is 8'h39(rvalid_timeout) -> re-encode MASK_ERR_CODE as 32'h20;
                                                                                              * If `ERR_CODE is 8'h3a(awready_timeout) -> re-encode MASK_ERR_CODE as 32'h40;
                                                                                              * If `ERR_CODE is 8'h3b(wready_timeout) -> re-encode MASK_ERR_CODE as 32'h80;
                                                                                              * If `ERR_CODE is 8'h3c(bvalid_timeout) -> re-encode MASK_ERR_CODE as 32'h100;"
                                                                                              */
#define RXI_WIFI_MASK_ERR_CODE(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_MASK_ERR_CODE(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup MASK_ERR_CODE_TMO_S11_S12_S13_S14
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_MASK_ERR_CODE                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0
                                                                                              * "Mask S11 S12 S13 S14 timeoutinterrupt when ERR_CODE matches
                                                                                              * If `ERR_CODE is 8'h38(arready timeout) -> re-encode MASK_ERR_CODE as 32'h10;
                                                                                              * If `ERR_CODE is 8'h39(rvalid_timeout) -> re-encode MASK_ERR_CODE as 32'h20;
                                                                                              * If `ERR_CODE is 8'h3a(awready_timeout) -> re-encode MASK_ERR_CODE as 32'h40;
                                                                                              * If `ERR_CODE is 8'h3b(wready_timeout) -> re-encode MASK_ERR_CODE as 32'h80;
                                                                                              * If `ERR_CODE is 8'h3c(bvalid_timeout) -> re-encode MASK_ERR_CODE as 32'h100;"
                                                                                              */
#define RXI_WIFI_MASK_ERR_CODE(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_MASK_ERR_CODE(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup MASK_ERR_CODE_TMO_S15
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_MASK_ERR_CODE                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0
                                                                                              * "Mask S15 timeout interrupt when ERR_CODE matches
                                                                                              * If `ERR_CODE is 8'h21(apb_pready_timeout) -> re-encode MASK_ERR_CODE as 32'h4;"
                                                                                              */
#define RXI_WIFI_MASK_ERR_CODE(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_MASK_ERR_CODE(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup MASK_ERR_CODE_TMO_S16
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_MASK_ERR_CODE                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0
                                                                                              * "Mask S16 timeout interrupt when ERR_CODE matches
                                                                                              * If `ERR_CODE is 8'h21(apb_pready_timeout) -> re-encode MASK_ERR_CODE as 32'h4;
                                                                                              * If `ERR_CODE is 8'h38(arready timeout) -> re-encode MASK_ERR_CODE as 32'h10;
                                                                                              * If `ERR_CODE is 8'h39(rvalid_timeout) -> re-encode MASK_ERR_CODE as 32'h20;
                                                                                              * If `ERR_CODE is 8'h3a(awready_timeout) -> re-encode MASK_ERR_CODE as 32'h40;
                                                                                              * If `ERR_CODE is 8'h3b(wready_timeout) -> re-encode MASK_ERR_CODE as 32'h80;
                                                                                              * If `ERR_CODE is 8'h3c(bvalid_timeout) -> re-encode MASK_ERR_CODE as 32'h100;"
                                                                                              */
#define RXI_WIFI_MASK_ERR_CODE(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_MASK_ERR_CODE(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup MASK_ERR_CODE_TMO_S17
 * @brief
 * @{
 */
#define RXI_WIFI_MASK_MASK_ERR_CODE                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0
                                                                                              * "Mask S17 timeout interrupt when ERR_CODE matches
                                                                                              * If `ERR_CODE is 8'h21(apb_pready_timeout) -> re-encode MASK_ERR_CODE as 32'h4;
                                                                                              * If `ERR_CODE is 8'h22(ahb slave adapter timeout) -> re-encode MASK_ERR_CODE as 32'h8;"
                                                                                              */
#define RXI_WIFI_MASK_ERR_CODE(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_WIFI_GET_MASK_ERR_CODE(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */




/** @defgroup RXI_WIFI_Exported_Types RXI_WIFI Exported Types
* @{
*/
/**
* @brief RXI_WIFI Register Declaration
*/

typedef struct {
	__I  uint32_t NAME;                                         /*!< Register,  Address offset:0x0000 */
	__I  uint32_t VER;                                          /*!< Register,  Address offset:0x0004 */
	__I  uint32_t REV;                                          /*!< Register,  Address offset:0x0008 */
	__I  uint32_t INST;                                         /*!< Register,  Address offset:0x000C */
	__I  uint32_t IMPL_Y;                                       /*!< Register,  Address offset:0x0010 */
	__I  uint32_t IMPL_D;                                       /*!< Register,  Address offset:0x0014 */
	__I  uint32_t DEV;                                          /*!< Register,  Address offset:0x0018 */
	__I  uint32_t PRO_NUM;                                      /*!< Register,  Address offset:0x001C */
	uint32_t RSVD_NO_FUNCTION0[56];
	__IO uint32_t RBARA0;                                       /*!< Register,  Address offset:0x0100 */
	__IO uint32_t RLARA0;                                       /*!< Register,  Address offset:0x0104 */
	__IO uint32_t RBARA1;                                       /*!< Register,  Address offset:0x0108 */
	__IO uint32_t RLARA1;                                       /*!< Register,  Address offset:0x010C */
	__IO uint32_t RBARA2;                                       /*!< Register,  Address offset:0x0110 */
	__IO uint32_t RLARA2;                                       /*!< Register,  Address offset:0x0114 */
	__IO uint32_t RBARA3;                                       /*!< Register,  Address offset:0x0118 */
	__IO uint32_t RLARA3;                                       /*!< Register,  Address offset:0x011C */
	__IO uint32_t RBARA4;                                       /*!< Register,  Address offset:0x0120 */
	__IO uint32_t RLARA4;                                       /*!< Register,  Address offset:0x0124 */
	__IO uint32_t RBARA5;                                       /*!< Register,  Address offset:0x0128 */
	__IO uint32_t RLARA5;                                       /*!< Register,  Address offset:0x012C */
	__IO uint32_t RBARA6;                                       /*!< Register,  Address offset:0x0130 */
	__IO uint32_t RLARA6;                                       /*!< Register,  Address offset:0x0134 */
	__IO uint32_t RBARA7;                                       /*!< Register,  Address offset:0x0138 */
	__IO uint32_t RLARA7;                                       /*!< Register,  Address offset:0x013C */
	__IO uint32_t CTRL;                                         /*!< Register,  Address offset:0x0140 */
	__IO uint32_t LOCK;                                         /*!< Register,  Address offset:0x0144 */
	__IO uint32_t RBARA8;                                       /*!< Register,  Address offset:0x0148 */
	__IO uint32_t RLARA8;                                       /*!< Register,  Address offset:0x014C */
	__IO uint32_t RBARA9;                                       /*!< Register,  Address offset:0x0150 */
	__IO uint32_t RLARA9;                                       /*!< Register,  Address offset:0x0154 */
	__IO uint32_t RBARA10;                                      /*!< Register,  Address offset:0x0158 */
	__IO uint32_t RLARA10;                                      /*!< Register,  Address offset:0x015C */
	__IO uint32_t RBARA11;                                      /*!< Register,  Address offset:0x0160 */
	__IO uint32_t RLARA11;                                      /*!< Register,  Address offset:0x0164 */
	__IO uint32_t RBARA12;                                      /*!< Register,  Address offset:0x0168 */
	__IO uint32_t RLARA12;                                      /*!< Register,  Address offset:0x016C */
	__IO uint32_t RBARA13;                                      /*!< Register,  Address offset:0x0170 */
	__IO uint32_t RLARA13;                                      /*!< Register,  Address offset:0x0174 */
	__IO uint32_t RBARA14;                                      /*!< Register,  Address offset:0x0178 */
	__IO uint32_t RLARA14;                                      /*!< Register,  Address offset:0x017C */
	__IO uint32_t RBARA15;                                      /*!< Register,  Address offset:0x0180 */
	__IO uint32_t RLARA15;                                      /*!< Register,  Address offset:0x0184 */
	uint32_t RSVD_NO_FUNCTION1[29];
	__I  uint32_t IDAU_DIR;                                     /*!< Register,  Address offset:0x01FC */
	__I  uint32_t ERR_PLD0_ADDR_HOLE;                           /*!< Register,  Address offset:0x0200 */
	__I  uint32_t ERR_PLD1_ADDR_HOLE;                           /*!< Register,  Address offset:0x0204 */
	__I  uint32_t ERR_ID_ADDR_HOLE;                             /*!< Register,  Address offset:0x0208 */
	__I  uint32_t ERR_ADR0_ADDR_HOLE;                           /*!< Register,  Address offset:0x020C */
	__I  uint32_t ERR_ADR1_ADDR_HOLE;                           /*!< Register,  Address offset:0x0210 */
	uint32_t RSVD_NO_FUNCTION2[7];
	__I  uint32_t ERR_CODE_ADDR_HOLE;                           /*!< Register,  Address offset:0x0230 */
	uint32_t RSVD_NO_FUNCTION3[2];
	__IO uint32_t ERR_INTR_CLR_ADDR_HOLE;                       /*!< Register,  Address offset:0x023C */
	__I  uint32_t ERR_PLD0_SECURE_FAULT;                        /*!< Register,  Address offset:0x0240 */
	__I  uint32_t ERR_PLD1_SECURE_FAULT;                        /*!< Register,  Address offset:0x0244 */
	__I  uint32_t ERR_ID_SECURE_FAULT;                          /*!< Register,  Address offset:0x0248 */
	__I  uint32_t ERR_ADR0_SECURE_FAULT;                        /*!< Register,  Address offset:0x024C */
	__I  uint32_t ERR_ADR1_SECURE_FAULT;                        /*!< Register,  Address offset:0x0250 */
	uint32_t RSVD_NO_FUNCTION4[7];
	__I  uint32_t ERR_CODE_SECURE_FAULT;                        /*!< Register,  Address offset:0x0270 */
	uint32_t RSVD_NO_FUNCTION5[2];
	__IO uint32_t ERR_INTR_CLR_SECURE_FAULT;                    /*!< Register,  Address offset:0x027C */
	__I  uint32_t ERR_PLD0_TMO_S9_S10;                          /*!< Register,  Address offset:0x0280 */
	__I  uint32_t ERR_PLD1_TMO_S9_S10;                          /*!< Register,  Address offset:0x0284 */
	__I  uint32_t ERR_ID_TMO_S9_S10;                            /*!< Register,  Address offset:0x0288 */
	__I  uint32_t ERR_ADR0_TMO_S9_S10;                          /*!< Register,  Address offset:0x028C */
	__I  uint32_t ERR_ADR1_TMO_S9_S10;                          /*!< Register,  Address offset:0x0290 */
	uint32_t RSVD_NO_FUNCTION6[7];
	__I  uint32_t ERR_CODE_TMO_S9_S10;                          /*!< Register,  Address offset:0x02B0 */
	uint32_t RSVD_NO_FUNCTION7[2];
	__IO uint32_t ERR_INTR_CLR_TMO_S9_S10;                      /*!< Register,  Address offset:0x02BC */
	__I  uint32_t ERR_PLD0_TMO_S11_S12_S13_S14;                 /*!< Register,  Address offset:0x02C0 */
	__I  uint32_t ERR_PLD1_TMO_S11_S12_S13_S14;                 /*!< Register,  Address offset:0x02C4 */
	__I  uint32_t ERR_ID_TMO_S11_S12_S13_S14;                   /*!< Register,  Address offset:0x02C8 */
	__I  uint32_t ERR_ADR0_TMO_S11_S12_S13_S14;                 /*!< Register,  Address offset:0x02CC */
	__I  uint32_t ERR_ADR1_TMO_S11_S12_S13_S14;                 /*!< Register,  Address offset:0x02D0 */
	uint32_t RSVD_NO_FUNCTION8[7];
	__I  uint32_t ERR_CODE_TMO_S11_S12_S13_S14;                 /*!< Register,  Address offset:0x02F0 */
	uint32_t RSVD_NO_FUNCTION9[2];
	__IO uint32_t ERR_INTR_CLR_TMO_S11_S12_S13_S14;             /*!< Register,  Address offset:0x02FC */
	__I  uint32_t ERR_PLD0_TMO_S15;                             /*!< Register,  Address offset:0x0300 */
	__I  uint32_t ERR_PLD1_TMO_S15;                             /*!< Register,  Address offset:0x0304 */
	__I  uint32_t ERR_ID_TMO_S15;                               /*!< Register,  Address offset:0x0308 */
	__I  uint32_t ERR_ADR0_TMO_S15;                             /*!< Register,  Address offset:0x030C */
	__I  uint32_t ERR_ADR1_TMO_S15;                             /*!< Register,  Address offset:0x0310 */
	uint32_t RSVD_NO_FUNCTION10[7];
	__I  uint32_t ERR_CODE_TMO_S15;                             /*!< Register,  Address offset:0x0330 */
	uint32_t RSVD_NO_FUNCTION11[2];
	__IO uint32_t ERR_INTR_CLR_TMO_S15;                         /*!< Register,  Address offset:0x033C */
	__I  uint32_t ERR_PLD0_TMO_S16;                             /*!< Register,  Address offset:0x0340 */
	__I  uint32_t ERR_PLD1_TMO_S16;                             /*!< Register,  Address offset:0x0344 */
	__I  uint32_t ERR_ID_TMO_S16;                               /*!< Register,  Address offset:0x0348 */
	__I  uint32_t ERR_ADR0_TMO_S16;                             /*!< Register,  Address offset:0x034C */
	__I  uint32_t ERR_ADR1_TMO_S16;                             /*!< Register,  Address offset:0x0350 */
	uint32_t RSVD_NO_FUNCTION12[7];
	__I  uint32_t ERR_CODE_TMO_S16;                             /*!< Register,  Address offset:0x0370 */
	uint32_t RSVD_NO_FUNCTION13[2];
	__IO uint32_t ERR_INTR_CLR_TMO_S16;                         /*!< Register,  Address offset:0x037C */
	__I  uint32_t ERR_PLD0_TMO_S17;                             /*!< Register,  Address offset:0x0380 */
	__I  uint32_t ERR_PLD1_TMO_S17;                             /*!< Register,  Address offset:0x0384 */
	__I  uint32_t ERR_ID_TMO_S17;                               /*!< Register,  Address offset:0x0388 */
	__I  uint32_t ERR_ADR0_TMO_S17;                             /*!< Register,  Address offset:0x038C */
	__I  uint32_t ERR_ADR1_TMO_S17;                             /*!< Register,  Address offset:0x0390 */
	uint32_t RSVD_NO_FUNCTION14[7];
	__I  uint32_t ERR_CODE_TMO_S17;                             /*!< Register,  Address offset:0x03B0 */
	uint32_t RSVD_NO_FUNCTION15[2];
	__IO uint32_t ERR_INTR_CLR_TMO_S17;                         /*!< Register,  Address offset:0x03BC */
	uint32_t RSVD_NO_FUNCTION16[16];
	__IO uint32_t CTR_AND_SECURE_0;                             /*!< Register,  Address offset:0x0400 */
	__IO uint32_t SECURE_1;                                     /*!< Register,  Address offset:0x0404 */
	__IO uint32_t SRAM_RD_WEIGHT;                               /*!< Register,  Address offset:0x0408 */
	uint32_t RSVD_NO_FUNCTION17[5];
	__I  uint32_t ERROR_MASTER_ID;                              /*!< Register,  Address offset:0x0420 */
	uint32_t RSVD_NO_FUNCTION18[55];
	__IO uint32_t TIMEOUT_GUARDIAN_EN;                          /*!< Register,  Address offset:0x0500 */
	uint32_t RSVD_NO_FUNCTION19[15];
	__IO uint32_t TIMEOUT_CLEAR_N;                              /*!< Register,  Address offset:0x0540 */
	uint32_t RSVD_NO_FUNCTION20[15];
	__IO uint32_t TIMEOUT_MON_CFG_0;                            /*!< Register,  Address offset:0x0580 */
	__IO uint32_t TIMEOUT_MON_CFG_1;                            /*!< Register,  Address offset:0x0584 */
	__IO uint32_t TIMEOUT_MON_CFG_2;                            /*!< Register,  Address offset:0x0588 */
	__IO uint32_t TIMEOUT_MON_CFG_3;                            /*!< Register,  Address offset:0x058C */
	__IO uint32_t TIMEOUT_MON_CFG_4;                            /*!< Register,  Address offset:0x0590 */
	uint32_t RSVD_NO_FUNCTION21[27];
	__I  uint32_t GLOBAL_INTR_RAW_STATUS;                       /*!< Register,  Address offset:0x0600 */
	uint32_t RSVD_NO_FUNCTION22[3];
	__IO uint32_t MASK_CONTROL;                                 /*!< Register,  Address offset:0x0610 */
	uint32_t RSVD_NO_FUNCTION23[3];
	__IO uint32_t MASK_ERR_CODE_ADDR_HOLE;                      /*!< Register,  Address offset:0x0620 */
	__IO uint32_t MASK_ERR_CODE_SECURE_FAULT;                   /*!< Register,  Address offset:0x0624 */
	__IO uint32_t MASK_ERR_CODE_TMO_S9_S10;                     /*!< Register,  Address offset:0x0628 */
	__IO uint32_t MASK_ERR_CODE_TMO_S11_S12_S13_S14;            /*!< Register,  Address offset:0x062C */
	__IO uint32_t MASK_ERR_CODE_TMO_S15;                        /*!< Register,  Address offset:0x0630 */
	__IO uint32_t MASK_ERR_CODE_TMO_S16;                        /*!< Register,  Address offset:0x0634 */
	__IO uint32_t MASK_ERR_CODE_TMO_S17;                        /*!< Register,  Address offset:0x0638 */
} RXI_WIFI_TypeDef;

#define RXI_WIFI_BASE ((RXI_WIFI_TypeDef *) (0x40212000))
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
#endif  // SYSREG_RXI_WIFI_H
