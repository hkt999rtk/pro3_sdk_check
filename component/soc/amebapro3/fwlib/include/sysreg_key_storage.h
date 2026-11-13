/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_KEY_STORAGE_H
#define SYSREG_KEY_STORAGE_H

/* AUTO_GEN_START */

/* KEY_STORAGE is a part of KEY_ST */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup KEY_STORAGE
* @brief KEY_STORAGE driver modules
* @{
*/


/** @defgroup REG_KEY_ST_SEC_BOOT_HASH0_3_0
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_SEC_BOOT_HASH0_3_0         ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF
                                                                                              * OTP autoload write, SW read only
                                                                                              * [7  : 0]-OTP_0xXX0
                                                                                              * [15 : 8]-OTP_0xXX1
                                                                                              * [23:16]-OTP_0xXX2
                                                                                              * [31:24]-OTP_0xXX3
                                                                                              */
#define KEY_STORAGE_SEC_BOOT_HASH0_3_0(x)           ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_SEC_BOOT_HASH0_3_0(x)       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_SEC_BOOT_HASH0_7_4
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_SEC_BOOT_HASH0_7_4         ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_SEC_BOOT_HASH0_7_4(x)           ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_SEC_BOOT_HASH0_7_4(x)       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_SEC_BOOT_HASH0_B_8
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_SEC_BOOT_HASH0_B_8         ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_SEC_BOOT_HASH0_B_8(x)           ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_SEC_BOOT_HASH0_B_8(x)       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_SEC_BOOT_HASH0_F_C
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_SEC_BOOT_HASH0_F_C         ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_SEC_BOOT_HASH0_F_C(x)           ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_SEC_BOOT_HASH0_F_C(x)       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_SEC_BOOT_HASH0_13_10
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_SEC_BOOT_HASH0_13_10       ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_SEC_BOOT_HASH0_13_10(x)         ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_SEC_BOOT_HASH0_13_10(x)     ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_SEC_BOOT_HASH0_17_14
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_SEC_BOOT_HASH0_17_14       ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_SEC_BOOT_HASH0_17_14(x)         ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_SEC_BOOT_HASH0_17_14(x)     ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_SEC_BOOT_HASH0_1B_18
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_SEC_BOOT_HASH0_1B_18       ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_SEC_BOOT_HASH0_1B_18(x)         ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_SEC_BOOT_HASH0_1B_18(x)     ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_SEC_BOOT_HASH0_1F_1C
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_SEC_BOOT_HASH0_1F_1C       ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_SEC_BOOT_HASH0_1F_1C(x)         ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_SEC_BOOT_HASH0_1F_1C(x)     ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_SEC_BOOT_HASH1_3_0
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_SEC_BOOT_HASH1_3_0         ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_SEC_BOOT_HASH1_3_0(x)           ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_SEC_BOOT_HASH1_3_0(x)       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_SEC_BOOT_HASH1_7_4
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_SEC_BOOT_HASH1_7_4         ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_SEC_BOOT_HASH1_7_4(x)           ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_SEC_BOOT_HASH1_7_4(x)       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_SEC_BOOT_HASH1_B_8
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_SEC_BOOT_HASH1_B_8         ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_SEC_BOOT_HASH1_B_8(x)           ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_SEC_BOOT_HASH1_B_8(x)       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_SEC_BOOT_HASH1_F_C
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_SEC_BOOT_HASH1_F_C         ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_SEC_BOOT_HASH1_F_C(x)           ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_SEC_BOOT_HASH1_F_C(x)       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_SEC_BOOT_HASH1_13_10
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_SEC_BOOT_HASH1_13_10       ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_SEC_BOOT_HASH1_13_10(x)         ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_SEC_BOOT_HASH1_13_10(x)     ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_SEC_BOOT_HASH1_17_14
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_SEC_BOOT_HASH1_17_14       ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_SEC_BOOT_HASH1_17_14(x)         ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_SEC_BOOT_HASH1_17_14(x)     ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_SEC_BOOT_HASH1_1B_18
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_SEC_BOOT_HASH1_1B_18       ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_SEC_BOOT_HASH1_1B_18(x)         ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_SEC_BOOT_HASH1_1B_18(x)     ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_SEC_BOOT_HASH1_1F_1C
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_SEC_BOOT_HASH1_1F_1C       ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_SEC_BOOT_HASH1_1F_1C(x)         ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_SEC_BOOT_HASH1_1F_1C(x)     ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_HUK_3_0
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_KEY_ST_HUK_3_0             ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_KEY_ST_HUK_3_0(x)               ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_KEY_ST_HUK_3_0(x)           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_HUK_7_4
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_KEY_ST_HUK_7_4             ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_KEY_ST_HUK_7_4(x)               ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_KEY_ST_HUK_7_4(x)           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_HUK_B_8
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_KEY_ST_HUK_B_8             ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_KEY_ST_HUK_B_8(x)               ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_KEY_ST_HUK_B_8(x)           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_HUK_F_C
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_KEY_ST_HUK_F_C             ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_KEY_ST_HUK_F_C(x)               ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_KEY_ST_HUK_F_C(x)           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_454_457
 * @brief
 * @{
 */
#define KEY_STORAGE_BIT_FAULT_LOG_EN                ((u32)0x00000001 << 23)                  /*!< R/W Once 0x0 Fault log enable */
#define KEY_STORAGE_BIT_HUK_DERV_DIS                ((u32)0x00000001 << 22)                  /*!< R/W Once 0x0 HUK derive disable */
#define KEY_STORAGE_BIT_SEC_BOOT_HASH_IDX           ((u32)0x00000001 << 21)                  /*!< R/W Once 0x0 Secure boot hash index */
#define KEY_STORAGE_BIT_SWD_FIXED_KEY_EN            ((u32)0x00000001 << 20)                  /*!< R/W Once 0x0 SWD fixed key enable */
#define KEY_STORAGE_BIT_SEC_SWD_MODE_SEL0           ((u32)0x00000001 << 19)                  /*!< R/W Once 0x0 Secure SWD Mode Select 0 */
#define KEY_STORAGE_BIT_SEC_SWD_MODE_SEL1           ((u32)0x00000001 << 18)                  /*!< R/W Once 0x0 Secure SWD Mode Select 1 */
#define KEY_STORAGE_BIT_NON_SEC_SWD_MODE_SEL0       ((u32)0x00000001 << 17)                  /*!< R/W Once 0x0 Non Secure SWD Mode Select 0 */
#define KEY_STORAGE_BIT_NON_SEC_SWD_MODE_SEL1       ((u32)0x00000001 << 16)                  /*!< R/W Once 0x0 Non Secure SWD Mode Select 1 */
#define KEY_STORAGE_BIT_RSIP_DISABLE                ((u32)0x00000001 << 15)                  /*!< R/W Once 0x0 RSIP disable */
#define KEY_STORAGE_BIT_SEC_BOOT_DIS                ((u32)0x00000001 << 14)                  /*!< R/W Once 0x0 Secure boot disable */
#define KEY_STORAGE_BIT_SEC_BOOT_ECC_SEL            ((u32)0x00000001 << 13)                  /*!< R/W Once 0x0 Secure boot ECC select */
#define KEY_STORAGE_BIT_SEC_BOOT_RANDM_DLY_DIS      ((u32)0x00000001 << 12)                  /*!< R/W Once 0x0 Secure boot random delay disable */
#define KEY_STORAGE_BIT_RDP_DIS                     ((u32)0x00000001 << 11)                  /*!< R/W Once 0x0 RDP disable */
#define KEY_STORAGE_BIT_ANTIROLL_BACK_DIS           ((u32)0x00000001 << 10)                  /*!< R/W Once 0x0 Anti roll back disable */
#define KEY_STORAGE_BIT_HW_UART_DWLD_EN             ((u32)0x00000001 << 9)                   /*!< R/W Once 0x0 hardware uart download enable */
#define KEY_STORAGE_BIT_SW_UART_DWLD_EN             ((u32)0x00000001 << 8)                   /*!< R/W Once 0x0 software uart download enable */
#define KEY_STORAGE_BIT_RSIP_MODE_0                 ((u32)0x00000001 << 7)                   /*!< R/W Once 0x0 RSIP Mode 0 */
#define KEY_STORAGE_BIT_RSIP_MODE_1                 ((u32)0x00000001 << 6)                   /*!< R/W Once 0x0 RSIP Mode 1 */
/** @} */


/** @defgroup REG_KEY_ST_458_45B
 * @brief
 * @{
 */
#define KEY_STORAGE_BIT_SEC_BOOT_AUTH_ALG_0         ((u32)0x00000001 << 7)                   /*!< R/W Once 0x0 Secure boot authentication algorithm 0 */
#define KEY_STORAGE_BIT_SEC_BOOT_AUTH_ALG_1         ((u32)0x00000001 << 6)                   /*!< R/W Once 0x0 Secure boot authentication algorithm 1 */
#define KEY_STORAGE_BIT_SEC_BOOT_AUTH_ALG_2         ((u32)0x00000001 << 5)                   /*!< R/W Once 0x0 Secure boot authentication algorithm 2 */
#define KEY_STORAGE_BIT_SEC_BOOT_AUTH_ALG_3         ((u32)0x00000001 << 4)                   /*!< R/W Once 0x0 Secure boot authentication algorithm 3 */
#define KEY_STORAGE_BIT_SEC_BOOT_HASH_ALG_0         ((u32)0x00000001 << 3)                   /*!< R/W Once 0x0 Secure boot hash algorithm 0 */
#define KEY_STORAGE_BIT_SEC_BOOT_HASH_ALG_1         ((u32)0x00000001 << 2)                   /*!< R/W Once 0x0 Secure boot hash algorithm 1 */
#define KEY_STORAGE_BIT_SEC_BOOT_HASH_ALG_2         ((u32)0x00000001 << 1)                   /*!< R/W Once 0x0 Secure boot hash algorithm 2 */
#define KEY_STORAGE_BIT_SEC_BOOT_HASH_ALG_3         ((u32)0x00000001 << 0)                   /*!< R/W Once 0x0 Secure boot hash algorithm 3 */
/** @} */


/** @defgroup REG_KEY_ST_45C_45F
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_OTA_ADDR_1                 ((u32)0x000000FF << 24)                  /*!< R/W Once 0xFF OTA Address 1 */
#define KEY_STORAGE_OTA_ADDR_1(x)                   ((u32)(((x) & 0x000000FF) << 24))
#define KEY_STORAGE_GET_OTA_ADDR_1(x)               ((u32)(((x >> 24) & 0x000000FF)))
#define KEY_STORAGE_MASK_OTA_ADDR_0                 ((u32)0x000000FF << 16)                  /*!< R/W Once 0xFF OTA Address 0 */
#define KEY_STORAGE_OTA_ADDR_0(x)                   ((u32)(((x) & 0x000000FF) << 16))
#define KEY_STORAGE_GET_OTA_ADDR_0(x)               ((u32)(((x >> 16) & 0x000000FF)))
/** @} */


/** @defgroup REG_KEY_ST_BOOTLOAD_VER_3_0
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_BOOTLOAD_VER_3_0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_BOOTLOAD_VER_3_0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_BOOTLOAD_VER_3_0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_BOOTLOAD_VER_7_4
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_BOOTLOAD_VER_7_4           ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_BOOTLOAD_VER_7_4(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_BOOTLOAD_VER_7_4(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_BOOTLOAD_VER_B_8
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_BOOTLOAD_VER_B_8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_BOOTLOAD_VER_B_8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_BOOTLOAD_VER_B_8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_BOOTLOAD_VER_F_C
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_BOOTLOAD_VER_F_C           ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_BOOTLOAD_VER_F_C(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_BOOTLOAD_VER_F_C(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_RMA_STATE
 * @brief
 * @{
 */
#define KEY_STORAGE_BIT_HUK_READ_DISABLE            ((u32)0x00000001 << 8)                   /*!< R/W   0   Can not write when it being 0x1 */
#define KEY_STORAGE_MASK_RMA_STATE                  ((u32)0x000000FF << 0)                   /*!< R/W Once   0 None */
#define KEY_STORAGE_RMA_STATE(x)                    ((u32)(((x) & 0x000000FF) << 0))
#define KEY_STORAGE_GET_RMA_STATE(x)                ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_RMA_BOOT_HASH_3_0
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_RMA_BOOT_HASH_3_0          ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_RMA_BOOT_HASH_3_0(x)            ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_RMA_BOOT_HASH_3_0(x)        ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_RMA_BOOT_HASH_7_4
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_RMA_BOOT_HASH_7_4          ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_RMA_BOOT_HASH_7_4(x)            ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_RMA_BOOT_HASH_7_4(x)        ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_RMA_BOOT_HASH_B_8
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_RMA_BOOT_HASH_B_8          ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_RMA_BOOT_HASH_B_8(x)            ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_RMA_BOOT_HASH_B_8(x)        ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_RMA_BOOT_HASH_F_C
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_RMA_BOOT_HASH_F_C          ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_RMA_BOOT_HASH_F_C(x)            ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_RMA_BOOT_HASH_F_C(x)        ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_RMA_BOOT_HASH_13_10
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_RMA_BOOT_HASH_13_10        ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_RMA_BOOT_HASH_13_10(x)          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_RMA_BOOT_HASH_13_10(x)      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_RMA_BOOT_HASH_17_14
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_RMA_BOOT_HASH_17_14        ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_RMA_BOOT_HASH_17_14(x)          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_RMA_BOOT_HASH_17_14(x)      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_RMA_BOOT_HASH_1B_18
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_RMA_BOOT_HASH_1B_18        ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_RMA_BOOT_HASH_1B_18(x)          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_RMA_BOOT_HASH_1B_18(x)      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_RMA_BOOT_HASH_1F_1C
 * @brief
 * @{
 */
#define KEY_STORAGE_MASK_RMA_BOOT_HASH_1F_1C        ((u32)0xFFFFFFFF << 0)                   /*!< R/W Once 0xFFFFFFFF None */
#define KEY_STORAGE_RMA_BOOT_HASH_1F_1C(x)          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KEY_STORAGE_GET_RMA_BOOT_HASH_1F_1C(x)      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KEY_ST_RSVD_4D7_4D4
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_KEY_ST_RSVD_4DB_4D8
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_KEY_ST_RSVD_4DF_4DC
 * @brief
 * @{
 */
/** @} */




/** @defgroup KEY_STORAGE_Exported_Types KEY_STORAGE Exported Types
* @{
*/
/**
* @brief KEY_STORAGE Register Declaration
*/

typedef struct {
	__IO uint32_t KEY_ST_SEC_BOOT_HASH0_3_0;                    /*!< Register,  Address offset:0x0000 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH0_7_4;                    /*!< Register,  Address offset:0x0004 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH0_B_8;                    /*!< Register,  Address offset:0x0008 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH0_F_C;                    /*!< Register,  Address offset:0x000C */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH0_13_10;                  /*!< Register,  Address offset:0x0010 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH0_17_14;                  /*!< Register,  Address offset:0x0014 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH0_1B_18;                  /*!< Register,  Address offset:0x0018 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH0_1F_1C;                  /*!< Register,  Address offset:0x001C */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH1_3_0;                    /*!< Register,  Address offset:0x0020 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH1_7_4;                    /*!< Register,  Address offset:0x0024 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH1_B_8;                    /*!< Register,  Address offset:0x0028 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH1_F_C;                    /*!< Register,  Address offset:0x002C */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH1_13_10;                  /*!< Register,  Address offset:0x0030 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH1_17_14;                  /*!< Register,  Address offset:0x0034 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH1_1B_18;                  /*!< Register,  Address offset:0x0038 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH1_1F_1C;                  /*!< Register,  Address offset:0x003C */
	__IO uint32_t KEY_ST_HUK_3_0;                               /*!< Register,  Address offset:0x0040 */
	__IO uint32_t KEY_ST_HUK_7_4;                               /*!< Register,  Address offset:0x0044 */
	__IO uint32_t KEY_ST_HUK_B_8;                               /*!< Register,  Address offset:0x0048 */
	__IO uint32_t KEY_ST_HUK_F_C;                               /*!< Register,  Address offset:0x004C */
	__IO uint32_t KEY_ST_454_457;                               /*!< Register,  Address offset:0x0050 */
	__IO uint32_t KEY_ST_458_45B;                               /*!< Register,  Address offset:0x0054 */
	__IO uint32_t KEY_ST_45C_45F;                               /*!< Register,  Address offset:0x0058 */
	__IO uint32_t KEY_ST_BOOTLOAD_VER_3_0;                      /*!< Register,  Address offset:0x005C */
	__IO uint32_t KEY_ST_BOOTLOAD_VER_7_4;                      /*!< Register,  Address offset:0x0060 */
	__IO uint32_t KEY_ST_BOOTLOAD_VER_B_8;                      /*!< Register,  Address offset:0x0064 */
	__IO uint32_t KEY_ST_BOOTLOAD_VER_F_C;                      /*!< Register,  Address offset:0x0068 */
	__IO uint32_t KEY_ST_RMA_STATE;                             /*!< Register,  Address offset:0x006C */
	__IO uint32_t RMA_BOOT_HASH_3_0;                            /*!< Register,  Address offset:0x0070 */
	__IO uint32_t RMA_BOOT_HASH_7_4;                            /*!< Register,  Address offset:0x0074 */
	__IO uint32_t RMA_BOOT_HASH_B_8;                            /*!< Register,  Address offset:0x0078 */
	__IO uint32_t RMA_BOOT_HASH_F_C;                            /*!< Register,  Address offset:0x007C */
	__IO uint32_t RMA_BOOT_HASH_13_10;                          /*!< Register,  Address offset:0x0080 */
	__IO uint32_t RMA_BOOT_HASH_17_14;                          /*!< Register,  Address offset:0x0084 */
	__IO uint32_t RMA_BOOT_HASH_1B_18;                          /*!< Register,  Address offset:0x0088 */
	__IO uint32_t RMA_BOOT_HASH_1F_1C;                          /*!< Register,  Address offset:0x008C */
	__IO uint32_t KEY_ST_RSVD_4D7_4D4;                          /*!< Register,  Address offset:0x0090 */
	__IO uint32_t KEY_ST_RSVD_4DB_4D8;                          /*!< Register,  Address offset:0x0094 */
	__IO uint32_t KEY_ST_RSVD_4DF_4DC;                          /*!< Register,  Address offset:0x0098 */
} KEY_STORAGE_TypeDef;

#define KEY_STORAGE_BASE ((KEY_STORAGE_TypeDef *) (0x4023E000))
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
#endif  // SYSREG_KEY_STORAGE_H
