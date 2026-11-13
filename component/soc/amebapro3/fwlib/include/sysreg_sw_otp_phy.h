/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_SW_OTP_PHY_H
#define SYSREG_SW_OTP_PHY_H

/* AUTO_GEN_START */

/* OTP_SW_PHY is a part of OTP_SW_PHY */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup OTP_SW_PHY
* @brief OTP_SW_PHY driver modules
* @{
*/


/** @defgroup REG_PHYOTP_SW_CONFIG0
 * @brief
 * @{
 */
#define OTP_SW_PHY_MASK_RESV                        ((u32)0x0007FFFF << 13)                  /*!< RW    0x1FFFFFFF None */
#define OTP_SW_PHY_RESV(x)                          ((u32)(((x) & 0x0007FFFF) << 13))
#define OTP_SW_PHY_GET_RESV(x)                      ((u32)(((x >> 13) & 0x0007FFFF)))
#define OTP_SW_PHY_MASK_PHY_ROM_SNZMCU2COMBO        ((u32)0x00000003 << 11)                  /*!< RW    0x3
                                                                                              * 2b'11 or 2b'00 : enable
                                                                                              * 2b'01 or 2b'10 : disable
                                                                                              */
#define OTP_SW_PHY_PHY_ROM_SNZMCU2COMBO(x)          ((u32)(((x) & 0x00000003) << 11))
#define OTP_SW_PHY_GET_PHY_ROM_SNZMCU2COMBO(x)      ((u32)(((x >> 11) & 0x00000003)))
#define OTP_SW_PHY_BIT_PHY_ROM_TESTMODE1            ((u32)0x00000001 << 10)                  /*!< RW    0x1
                                                                                              * 1: enable
                                                                                              * 0: disable
                                                                                              */
#define OTP_SW_PHY_BIT_PHY_ROM_SWD_EN               ((u32)0x00000001 << 9)                   /*!< RW    0x1
                                                                                              * 1: enable
                                                                                              * 0: disable
                                                                                              */
#define OTP_SW_PHY_BIT_PHY_ROM_TESTMODE3            ((u32)0x00000001 << 8)                   /*!< RW    0x1
                                                                                              * 1: enable
                                                                                              * 0: disable
                                                                                              */
#define OTP_SW_PHY_MASK_PHY_ROM_PATCH_ADDR          ((u32)0x00000007 << 5)                   /*!< RW    0x7
                                                                                              * 3b'111' : default off
                                                                                              * 3b'011' : use sys2 pll tracking
                                                                                              * 3b'001': use peri pll tracking
                                                                                              * 3b'000': forever turn off
                                                                                              * 
                                                                                              * 
                                                                                              */
#define OTP_SW_PHY_PHY_ROM_PATCH_ADDR(x)            ((u32)(((x) & 0x00000007) << 5))
#define OTP_SW_PHY_GET_PHY_ROM_PATCH_ADDR(x)        ((u32)(((x >> 5) & 0x00000007)))
#define OTP_SW_PHY_MASK_PHY_ROM_PATCH_EN            ((u32)0x0000001F << 0)                   /*!< RW    0x1F
                                                                                              * 5b'11111' : default off
                                                                                              * 5b'01111' : enable without crc16 (crc1)
                                                                                              * 5b'00111': enable with crc16(crc1)
                                                                                              * 5b'00011': enable without crc16(crc2)
                                                                                              * 5b'00001': enable with crc16(crc2)
                                                                                              * 5b'00000': forever disable
                                                                                              */
#define OTP_SW_PHY_PHY_ROM_PATCH_EN(x)              ((u32)(((x) & 0x0000001F) << 0))
#define OTP_SW_PHY_GET_PHY_ROM_PATCH_EN(x)          ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_PHYOTP_SW_CONFIG1
 * @brief
 * @{
 */
#define OTP_SW_PHY_MASK_PHY_ROM_PATCH_CRC16_2       ((u32)0x0000FFFF << 16)                  /*!< RW    0xFFFF PATCH checksum, only valid if phyical crc16 not set */
#define OTP_SW_PHY_PHY_ROM_PATCH_CRC16_2(x)         ((u32)(((x) & 0x0000FFFF) << 16))
#define OTP_SW_PHY_GET_PHY_ROM_PATCH_CRC16_2(x)     ((u32)(((x >> 16) & 0x0000FFFF)))
#define OTP_SW_PHY_MASK_PHY_ROM_PATCH_CRC16_1       ((u32)0x0000FFFF << 0)                   /*!< RW    0xFFFF PATCH checksum, only valid if phyical crc16 not set */
#define OTP_SW_PHY_PHY_ROM_PATCH_CRC16_1(x)         ((u32)(((x) & 0x0000FFFF) << 0))
#define OTP_SW_PHY_GET_PHY_ROM_PATCH_CRC16_1(x)     ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_SYS_SW_RESV_2
 * @brief
 * @{
 */
#define OTP_SW_PHY_MASK_RESV_008                    ((u32)0xFFFFFFFF << 0)                   /*!< RW    0xFFFFFFFF None */
#define OTP_SW_PHY_RESV_008(x)                      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define OTP_SW_PHY_GET_RESV_008(x)                  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */




/** @defgroup OTP_SW_PHY_Exported_Types OTP_SW_PHY Exported Types
* @{
*/
/**
* @brief OTP_SW_PHY Register Declaration
*/

typedef struct {
	__IO uint32_t PHYOTP_SW_CONFIG0;                            /*!< Register,  Address offset:0x0000 */
	__IO uint32_t PHYOTP_SW_CONFIG1;                            /*!< Register,  Address offset:0x0004 */
	__IO uint32_t SYS_SW_RESV_2;                                /*!< Register,  Address offset:0x0008 */
} OTP_SW_PHY_TypeDef;

#define OTP_SW_PHY_BASE ((OTP_SW_PHY_TypeDef *) (0x4023E090))
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
#endif  // SYSREG_SW_OTP_PHY_H
