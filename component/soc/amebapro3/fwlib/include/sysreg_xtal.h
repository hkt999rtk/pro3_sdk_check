/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_XTAL_H
#define SYSREG_XTAL_H

/* AUTO_GEN_START */

/* XTAL is a part of AON */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup XTAL
* @brief XTAL driver modules
* @{
*/


/** @defgroup REG_XTAL_CLK_CTRL0
 * @brief
 * @{
 */
#define XTAL_MASK_XTAL_SC_XO                        ((u32)0x0000007F << 21)                  /*!< R/W   0x40
                                                                                              * 1: reduce cap, 0: increase cap. (160fF per unit)
                                                                                              * [efuse for BT only ]
                                                                                              */
#define XTAL_XTAL_SC_XO(x)                          ((u32)(((x) & 0x0000007F) << 21))
#define XTAL_GET_XTAL_SC_XO(x)                      ((u32)(((x >> 21) & 0x0000007F)))
#define XTAL_MASK_XTAL_SC_XI                        ((u32)0x0000007F << 14)                  /*!< R/W   0x40
                                                                                              * 1: reduce cap, 0: increase cap. (160fF per unit)
                                                                                              * [efuse for BT only ]
                                                                                              */
#define XTAL_XTAL_SC_XI(x)                          ((u32)(((x) & 0x0000007F) << 14))
#define XTAL_GET_XTAL_SC_XI(x)                      ((u32)(((x >> 14) & 0x0000007F)))
#define XTAL_MASK_XTAL_GMN                          ((u32)0x0000001F << 9)                   /*!< R/W   0x1F GMN: 0~31 */
#define XTAL_XTAL_GMN(x)                            ((u32)(((x) & 0x0000001F) << 9))
#define XTAL_GET_XTAL_GMN(x)                        ((u32)(((x >> 9) & 0x0000001F)))
#define XTAL_MASK_XTAL_GMP                          ((u32)0x0000001F << 4)                   /*!< R/W   0x1F GMP: 0~31 */
#define XTAL_XTAL_GMP(x)                            ((u32)(((x) & 0x0000001F) << 4))
#define XTAL_GET_XTAL_GMP(x)                        ((u32)(((x >> 4) & 0x0000001F)))
#define XTAL_BIT_XTAL_POR                           ((u32)0x00000001 << 3)                   /*!< R/W   0   1: Enable xtal 0.9V */
#define XTAL_BIT_XTAL_HW33_EN                       ((u32)0x00000001 << 2)                   /*!< R/W   0   1: Enable xtal 3.3V */
#define XTAL_BIT_POW_XTAL                           ((u32)0x00000001 << 1)                   /*!< R/W   0   0 to 1 Enable xtal  */
#define XTAL_BIT_POW_BG                             ((u32)0x00000001 << 0)                   /*!< R/W   0   Enable BG  (BG for xtal and AFE) */
/** @} */


/** @defgroup REG_XTAL_CLK_CTRL1
 * @brief
 * @{
 */
#define XTAL_BIT_XTAL_LPS_DIVISOR                   ((u32)0x00000001 << 29)                  /*!< R/W   0  
                                                                                              * XTAL LPS clock divider 
                                                                                              * 0: XTAL/128(40M), 1: XTAL/64
                                                                                              */
#define XTAL_BIT_XTAL_CKDIGI_SEL                    ((u32)0x00000001 << 28)                  /*!< R/W   1  
                                                                                              * 1: DIGI CLK from self-bias
                                                                                              * 0: DIGI CLK from Schmitt
                                                                                              */
#define XTAL_BIT_EN_XTAL_LPS_CLK                    ((u32)0x00000001 << 27)                  /*!< R/W   1   1: enable XTAL LPS clock  (312.5kHz enable) */
#define XTAL_BIT_EN_XTAL_SCHMITT                    ((u32)0x00000001 << 26)                  /*!< R/W   1   1: enable XTAL SCHMITT  */
#define XTAL_MASK_XTAL_LDO_VREF_2_0                 ((u32)0x00000007 << 23)                  /*!< R/W   0x3
                                                                                              * 111: 1.25V  110: 1.2V  101:1.15V  100:1.1V
                                                                                              * 011: 1.05V  010: 1V   001: 0.95V  000: 0.9V
                                                                                              */
#define XTAL_XTAL_LDO_VREF_2_0(x)                   ((u32)(((x) & 0x00000007) << 23))
#define XTAL_GET_XTAL_LDO_VREF_2_0(x)               ((u32)(((x >> 23) & 0x00000007)))
#define XTAL_MASK_XTAL_SEL_TOK_2_0                  ((u32)0x00000007 << 20)                  /*!< R/W   0x4
                                                                                              * For cycles / 40MHz / 26MHz
                                                                                              * 000: 8 / 200n / 300n
                                                                                              * 001: 16 / 400n / 600n
                                                                                              * 010: 32 / 800n / 1.2u
                                                                                              * 011: 64 / 1.6u / 2.5u
                                                                                              * 100: 2^14 / 0.4m / 0.6m
                                                                                              * 101: 2^15 / 0.8m / 1.2m
                                                                                              * 110: 2^16 / 1.6m / 2.5m
                                                                                              * 111: 2^17 / 3.2m / 4.9m
                                                                                              * 
                                                                                              */
#define XTAL_XTAL_SEL_TOK_2_0(x)                    ((u32)(((x) & 0x00000007) << 20))
#define XTAL_GET_XTAL_SEL_TOK_2_0(x)                ((u32)(((x >> 20) & 0x00000007)))
#define XTAL_BIT_EN_XTAL_DRV_LPS                    ((u32)0x00000001 << 19)                  /*!< R/W   0   LPS enable clock source control, 1:enable, 0: gated (312.5kHz enable) */
#define XTAL_BIT_EN_XTAL_DRV_DIGI                   ((u32)0x00000001 << 18)                  /*!< R/W   0   DIGI enable clock source control, 1:enable, 0: gated  */
#define XTAL_BIT_EN_XTAL_DRV_USB                    ((u32)0x00000001 << 17)                  /*!< R/W   0   USB enable clock source control, 1:enable, 0: gated  */
#define XTAL_BIT_EN_XTAL_DRV_AFE                    ((u32)0x00000001 << 16)                  /*!< R/W   0   AFE enable clock source control, 1:enable, 0: gated (for WL AFE PLL) */
#define XTAL_BIT_EN_XTAL_DRV_RF2                    ((u32)0x00000001 << 15)                  /*!< R/W   0   RF2 enable clock source control, 1:enable, 0: gated (for BT) */
#define XTAL_BIT_EN_XTAL_DRV_RF1                    ((u32)0x00000001 << 14)                  /*!< R/W   0   RF1 enable clock source control, 1:enable, 0: gated (for WL SYN) */
#define XTAL_MASK_XTAL_SC_LPS_RO                    ((u32)0x0000007F << 7)                   /*!< R     0x3f
                                                                                              * 0: reduce cap, 1: incease cap. (160fF per unit)  efuse for BT only
                                                                                              * Singal from XMD output XTAL_SC_LPS_i<6:0>
                                                                                              */
#define XTAL_XTAL_SC_LPS_RO(x)                      ((u32)(((x) & 0x0000007F) << 7))
#define XTAL_GET_XTAL_SC_LPS_RO(x)                  ((u32)(((x >> 7) & 0x0000007F)))
#define XTAL_MASK_XTAL_SC_INIT                      ((u32)0x0000007F << 0)                   /*!< R/W   0  
                                                                                              * 0: reduce cap, 1: increase cap. (160fF per unit) efuse for BT only
                                                                                              * Set to 0 if external Cap is available
                                                                                              */
#define XTAL_XTAL_SC_INIT(x)                        ((u32)(((x) & 0x0000007F) << 0))
#define XTAL_GET_XTAL_SC_INIT(x)                    ((u32)(((x >> 0) & 0x0000007F)))
/** @} */


/** @defgroup REG_XTAL_CLK_CTRL2
 * @brief
 * @{
 */
#define XTAL_BIT_XTAL_LPMODE                        ((u32)0x00000001 << 31)                  /*!< R/W   0  
                                                                                              * 1: low power mode enable 
                                                                                              * 22C turn off replica path, RF1 no CLK
                                                                                              * RF2 sweep to LPS buf 
                                                                                              */
#define XTAL_BIT_XTAL_DELAY_DIGI                    ((u32)0x00000001 << 30)                  /*!< R/W   1   1: turn on XI/XO Cfix, single side 8pF */
#define XTAL_BIT_XTAL_DELAY_USB                     ((u32)0x00000001 << 29)                  /*!< R/W   1   0: short, 1:long */
#define XTAL_BIT_XTAL_DELAY_AFE                     ((u32)0x00000001 << 28)                  /*!< R/W   1   0: short, 1:long */
#define XTAL_MASK_XTAL_DRV_DIGI_1_0                 ((u32)0x00000003 << 26)                  /*!< R/W   0x3 Driving control of DIGI/LPS clock buffer, 11:large current, 00: small current  */
#define XTAL_XTAL_DRV_DIGI_1_0(x)                   ((u32)(((x) & 0x00000003) << 26))
#define XTAL_GET_XTAL_DRV_DIGI_1_0(x)               ((u32)(((x >> 26) & 0x00000003)))
#define XTAL_MASK_XTAL_DRV_USB_1_0                  ((u32)0x00000003 << 24)                  /*!< R/W   0x3 Driving control of USB clock buffer, 11:large current, 00: small current */
#define XTAL_XTAL_DRV_USB_1_0(x)                    ((u32)(((x) & 0x00000003) << 24))
#define XTAL_GET_XTAL_DRV_USB_1_0(x)                ((u32)(((x >> 24) & 0x00000003)))
#define XTAL_MASK_XTAL_DRV_AFE_1_0                  ((u32)0x00000003 << 22)                  /*!< R/W   0x3 Driving control of AFE clock buffer, 11:large current, 00: small current */
#define XTAL_XTAL_DRV_AFE_1_0(x)                    ((u32)(((x) & 0x00000003) << 22))
#define XTAL_GET_XTAL_DRV_AFE_1_0(x)                ((u32)(((x >> 22) & 0x00000003)))
#define XTAL_MASK_XTAL_DRV_RF2_1_0                  ((u32)0x00000003 << 20)                  /*!< R/W   0x3 Driving control of RF2 clock buffer, 11:large current, 00: small current */
#define XTAL_XTAL_DRV_RF2_1_0(x)                    ((u32)(((x) & 0x00000003) << 20))
#define XTAL_GET_XTAL_DRV_RF2_1_0(x)                ((u32)(((x >> 20) & 0x00000003)))
#define XTAL_MASK_XTAL_DRV_RF1_1_0                  ((u32)0x00000003 << 18)                  /*!< R/W   0x3 Driving control of RF1 clock buffer, 11:large current, 00: small current */
#define XTAL_XTAL_DRV_RF1_1_0(x)                    ((u32)(((x) & 0x00000003) << 18))
#define XTAL_GET_XTAL_DRV_RF1_1_0(x)                ((u32)(((x >> 18) & 0x00000003)))
#define XTAL_BIT_XTAL_DRV_RF_LATCH                  ((u32)0x00000001 << 17)                  /*!< R/W   1   1: latch enable */
#define XTAL_BIT_XTAL_GM_SEP                        ((u32)0x00000001 << 16)                  /*!< R/W   0   0: GMP=GMN */
#define XTAL_BIT_XQSEL_RF_AWAKE                     ((u32)0x00000001 << 15)                  /*!< R/W   1   0: Schmitt trigger, 1:replica INV(for OK0=1) */
#define XTAL_BIT_XQSEL_RF_INITIAL                   ((u32)0x00000001 << 14)                  /*!< R/W   0   0: Schmitt trigger, 1:replica INV(for OK0=0) */
#define XTAL_BIT_XQSEL                              ((u32)0x00000001 << 13)                  /*!< R/W   0   0: Schmitt trigger, 1:replica INV(for AFE) */
#define XTAL_BIT_GATED_XTAL_OK0                     ((u32)0x00000001 << 12)                  /*!< R/W   0   1:Gated xtal OK0 */
#define XTAL_BIT_EN_XTAL_DRV_IQK_BCN                ((u32)0x00000001 << 11)                  /*!< R/W   1   IQKPLL enable clock source control, 1:enable, 0: gate */
#define XTAL_MASK_AAC_MODE_1_0                      ((u32)0x00000003 << 9)                   /*!< R/W   1  
                                                                                              * AAC_MODE for LPS
                                                                                              * 00: all XOXI cap off
                                                                                              * 01: cap from XTAL_SC_LPS
                                                                                              * 10: cap from XTAL_SC_INIT
                                                                                              * 11: cap from XTAL_SC_XI/XO
                                                                                              */
#define XTAL_AAC_MODE_1_0(x)                        ((u32)(((x) & 0x00000003) << 9))
#define XTAL_GET_AAC_MODE_1_0(x)                    ((u32)(((x >> 9) & 0x00000003)))
#define XTAL_MASK_XTAL_CFIX_1_0                     ((u32)0x00000003 << 7)                   /*!< R/W   0x3 11: 7.9pF, 10: 4.5pF, 01: 3.4pF, 00: off */
#define XTAL_XTAL_CFIX_1_0(x)                       ((u32)(((x) & 0x00000003) << 7))
#define XTAL_GET_XTAL_CFIX_1_0(x)                   ((u32)(((x >> 7) & 0x00000003)))
#define XTAL_MASK_XTAL_AAC_OPCUR_1_0                ((u32)0x00000003 << 5)                   /*!< R/W   0x3
                                                                                              * 00: 200nA , 200n/step  
                                                                                              * For AAC robust 11
                                                                                              */
#define XTAL_XTAL_AAC_OPCUR_1_0(x)                  ((u32)(((x) & 0x00000003) << 5))
#define XTAL_GET_XTAL_AAC_OPCUR_1_0(x)              ((u32)(((x >> 5) & 0x00000003)))
#define XTAL_MASK_XTAL_VREF_SEL_4_0                 ((u32)0x0000001F << 0)                   /*!< R/W   0x18
                                                                                              * Analog AAC VREF, 390mV~700mV, 10mV/step
                                                                                              * 11000: Vpp 500mV 
                                                                                              */
#define XTAL_XTAL_VREF_SEL_4_0(x)                   ((u32)(((x) & 0x0000001F) << 0))
#define XTAL_GET_XTAL_VREF_SEL_4_0(x)               ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_XTAL_CLK_CTRL3
 * @brief
 * @{
 */
#define XTAL_MASK_XTAL_SRC_BT_2_0                   ((u32)0x00000007 << 29)                  /*!< R/W   0   Dummy(w/o SRC) */
#define XTAL_XTAL_SRC_BT_2_0(x)                     ((u32)(((x) & 0x00000007) << 29))
#define XTAL_GET_XTAL_SRC_BT_2_0(x)                 ((u32)(((x >> 29) & 0x00000007)))
#define XTAL_MASK_XTAL_SRC_RF_2_0                   ((u32)0x00000007 << 26)                  /*!< R/W   0   Dummy(w/o SRC) */
#define XTAL_XTAL_SRC_RF_2_0(x)                     ((u32)(((x) & 0x00000007) << 26))
#define XTAL_GET_XTAL_SRC_RF_2_0(x)                 ((u32)(((x >> 26) & 0x00000007)))
#define XTAL_MASK_XTAL_DUMMY_3_0                    ((u32)0x0000000F << 22)                  /*!< R/W   0   Dummy control bits  */
#define XTAL_XTAL_DUMMY_3_0(x)                      ((u32)(((x) & 0x0000000F) << 22))
#define XTAL_GET_XTAL_DUMMY_3_0(x)                  ((u32)(((x >> 22) & 0x0000000F)))
#define XTAL_BIT_XTAL_EN_LNBUF                      ((u32)0x00000001 << 21)                  /*!< R/W   0  
                                                                                              * 0: separate bias buffer (lower current higher noise)
                                                                                              * 1: self-bias (higher current lower noise)
                                                                                              */
#define XTAL_BIT_XTAL__AAC_TIE_MID                  ((u32)0x00000001 << 20)                  /*!< R/W   0  
                                                                                              * XTAL_ AAC_TIE_MID=1 & EN_XTAL_PDCK_VREF=0 &
                                                                                              * using XTAL_VREF_SEL<4:0> for lock detect
                                                                                              */
#define XTAL_MASK_XTAL_AAC_IOFFSET_1_0              ((u32)0x00000003 << 18)                  /*!< R/W   0  
                                                                                              * Analog AAC IOFFSET current
                                                                                              * 00: off, 01:2uA, 10:4uA, 11:6uA
                                                                                              */
#define XTAL_XTAL_AAC_IOFFSET_1_0(x)                ((u32)(((x) & 0x00000003) << 18))
#define XTAL_GET_XTAL_AAC_IOFFSET_1_0(x)            ((u32)(((x >> 18) & 0x00000003)))
#define XTAL_MASK_XTAL_AAC_CAP_1_0                  ((u32)0x00000003 << 16)                  /*!< R/W   0x3
                                                                                              * Analog AAC domain pole cap 3pF/step
                                                                                              * 00: no cap,11: max cap
                                                                                              */
#define XTAL_XTAL_AAC_CAP_1_0(x)                    ((u32)(((x) & 0x00000003) << 16))
#define XTAL_GET_XTAL_AAC_CAP_1_0(x)                ((u32)(((x >> 16) & 0x00000003)))
#define XTAL_MASK_XTAL_PDSW_1_0                     ((u32)0x00000003 << 14)                  /*!< R/W   1   XTAL analog AAC peak detector source follower size, 00: small 11: large */
#define XTAL_XTAL_PDSW_1_0(x)                       ((u32)(((x) & 0x00000003) << 14))
#define XTAL_GET_XTAL_PDSW_1_0(x)                   ((u32)(((x >> 14) & 0x00000003)))
#define XTAL_MASK_XTAL_LPS_BUF_VB_1_0               ((u32)0x00000003 << 12)                  /*!< R/W   0   00: small current, 11: large current */
#define XTAL_XTAL_LPS_BUF_VB_1_0(x)                 ((u32)(((x) & 0x00000003) << 12))
#define XTAL_GET_XTAL_LPS_BUF_VB_1_0(x)             ((u32)(((x >> 12) & 0x00000003)))
#define XTAL_BIT_XTAL_PDCK_MANU                     ((u32)0x00000001 << 11)                  /*!< R/W   0   1: open analog AAC loop for PDCK manual  */
#define XTAL_BIT_XTAL_PDCK_OK_MANU                  ((u32)0x00000001 << 10)                  /*!< R/W   1   1: force PDCK_OK=1 for PDCK manual */
#define XTAL_BIT_EN_XTAL_PDCK_VREF                  ((u32)0x00000001 << 9)                   /*!< R/W   1  
                                                                                              * 1: analog AAC VREF from PDCK digital circuit
                                                                                              * 0: analog AAC VREF from XTAL_VREF_SEL for BT only mode
                                                                                              */
#define XTAL_BIT_XTAL_SEL_PWR                       ((u32)0x00000001 << 8)                   /*!< R/W   0   1: external 0.9V, 0: internal 0.9V (FIB) */
#define XTAL_BIT_XTAL_PK_SEL_OFFSET                 ((u32)0x00000001 << 7)                   /*!< R/W   0  
                                                                                              * 1: XI Vpk-to-Vpk offset to
                                                                                              * 00: 0.4V  01: 0.5V  10: 0.6V  11: 0.7V
                                                                                              */
#define XTAL_MASK_XTAL_MANU_PK_SEL_1_0              ((u32)0x00000003 << 5)                   /*!< R/W   0x3
                                                                                              * XI Vpk-to-Vpk
                                                                                              * 00: 0.7V  01: 0.8V  10: 0.9V  11: 1V
                                                                                              */
#define XTAL_XTAL_MANU_PK_SEL_1_0(x)                ((u32)(((x) & 0x00000003) << 5))
#define XTAL_GET_XTAL_MANU_PK_SEL_1_0(x)            ((u32)(((x >> 5) & 0x00000003)))
#define XTAL_BIT_XTAL_AACK_PK_MANU                  ((u32)0x00000001 << 4)                   /*!< R/W   0   1: manual for peak detector peak selection  */
#define XTAL_BIT_EN_XTAL_AAC_PKDET                  ((u32)0x00000001 << 3)                   /*!< R/W   0   1: enable xtal aac peak detector */
#define XTAL_BIT_EN_XTAL_AAC_GM                     ((u32)0x00000001 << 2)                   /*!< R/W   0   1: enable xtal aac gm control ,Note: This bit must be able to write by SW */
#define XTAL_BIT_XTAL_LDO_OPVB_SEL                  ((u32)0x00000001 << 1)                   /*!< R/W   0   0: OP bias from IBG, 1: OP bias from Res divider */
#define XTAL_BIT_XTAL_LDO_NC                        ((u32)0x00000001 << 0)                   /*!< R/W   0  
                                                                                              * 1: low current normal mode (22C)
                                                                                              * 0:high current normal mode
                                                                                              * (BT could be control this pin)
                                                                                              */
/** @} */


/** @defgroup REG_XTAL_CLK_CTRL4
 * @brief
 * @{
 */
#define XTAL_MASK__XTAL_MODE_DEC_1_0                ((u32)0x00000003 << 15)                  /*!< R     0x2 XTAL Dec Mode */
#define XTAL__XTAL_MODE_DEC_1_0(x)                  ((u32)(((x) & 0x00000003) << 15))
#define XTAL_GET__XTAL_MODE_DEC_1_0(x)              ((u32)(((x >> 15) & 0x00000003)))
#define XTAL_MASK_XMD_STATE_4_0                     ((u32)0x0000001F << 10)                  /*!< R     0xB XMD State */
#define XTAL_XMD_STATE_4_0(x)                       ((u32)(((x) & 0x0000001F) << 10))
#define XTAL_GET_XMD_STATE_4_0(x)                   ((u32)(((x >> 10) & 0x0000001F)))
#define XTAL_MASK_XTAL_SC_LPS_6_0                   ((u32)0x0000007F << 3)                   /*!< R/W   0  
                                                                                              * XTAL cap setting @LP 
                                                                                              * 0: reduce cap, 1: increase cap. (160fF per unit)
                                                                                              */
#define XTAL_XTAL_SC_LPS_6_0(x)                     ((u32)(((x) & 0x0000007F) << 3))
#define XTAL_GET_XTAL_SC_LPS_6_0(x)                 ((u32)(((x >> 3) & 0x0000007F)))
#define XTAL_MASK_XTAL_SRC_IQK_BCN_2_0              ((u32)0x00000007 << 0)                   /*!< R/W   0   Dummy(w/o SRC) */
#define XTAL_XTAL_SRC_IQK_BCN_2_0(x)                ((u32)(((x) & 0x00000007) << 0))
#define XTAL_GET_XTAL_SRC_IQK_BCN_2_0(x)            ((u32)(((x >> 0) & 0x00000007)))
/** @} */


/** @defgroup REG_XTAL_CLK_CTRL5
 * @brief
 * @{
 */
#define XTAL_BIT_XAAC_LPOW                          ((u32)0x00000001 << 31)                  /*!< R/W   0   AACK low power mode, 1:gating input clk */
#define XTAL_MASK_AAC_MODE_ON_1_0                   ((u32)0x00000003 << 29)                  /*!< R/W   0x3
                                                                                              * 11: XTAL_AACK_PK_SEL=<11>'s XTAL_GM_OUT3
                                                                                              * 10: XTAL_AACK_PK_SEL=<10>'s XTAL_GM_OUT2
                                                                                              * 01: XTAL_AACK_PK_SEL=<01>'s XTAL_GM_OUT1
                                                                                              * 00: XTAL_AACK_PK_SEL=<00>'s XTAL_GM_OUT0
                                                                                              */
#define XTAL_AAC_MODE_ON_1_0(x)                     ((u32)(((x) & 0x00000003) << 29))
#define XTAL_GET_AAC_MODE_ON_1_0(x)                 ((u32)(((x >> 29) & 0x00000003)))
#define XTAL_BIT_EN_XTAL_AAC_TRIG                   ((u32)0x00000001 << 28)                  /*!< R/W   0   Set by pmc and clr by HW finish calibration */
#define XTAL_BIT_EN_XTAL_AAC                        ((u32)0x00000001 << 27)                  /*!< R/W   0  
                                                                                              * 0:aac off
                                                                                              * 1:aac power on
                                                                                              */
#define XTAL_BIT_EN_XTAL_AAC_DIGI                   ((u32)0x00000001 << 26)                  /*!< R/W   0   1:enable aac digital circuit */
#define XTAL_MASK_GM_MANUAL_4_0                     ((u32)0x0000001F << 21)                  /*!< R/W   0x1F Gm manual at EN_XTAL_AAC_DIGI=0 */
#define XTAL_GM_MANUAL_4_0(x)                       ((u32)(((x) & 0x0000001F) << 21))
#define XTAL_GET_GM_MANUAL_4_0(x)                   ((u32)(((x >> 21) & 0x0000001F)))
#define XTAL_MASK_GM_STUP_4_0                       ((u32)0x0000001F << 16)                  /*!< R/W   0x1F aac gm initial setting */
#define XTAL_GM_STUP_4_0(x)                         ((u32)(((x) & 0x0000001F) << 16))
#define XTAL_GET_GM_STUP_4_0(x)                     ((u32)(((x >> 16) & 0x0000001F)))
#define XTAL_MASK_XTAL_CK_SET_2_0                   ((u32)0x00000007 << 13)                  /*!< R/W   0x3
                                                                                              * aac settling cycles (4M CLK)
                                                                                              * 000:  64us
                                                                                              * 001:  128us
                                                                                              * 010:  256us
                                                                                              * 011:  512us
                                                                                              * 100:  1024us
                                                                                              * 101:  2048us
                                                                                              * 110:  4096us
                                                                                              * 111:  8192us
                                                                                              */
#define XTAL_XTAL_CK_SET_2_0(x)                     ((u32)(((x) & 0x00000007) << 13))
#define XTAL_GET_XTAL_CK_SET_2_0(x)                 ((u32)(((x >> 13) & 0x00000007)))
#define XTAL_MASK_GM_INIT_4_0                       ((u32)0x0000001F << 8)                   /*!< R/W   0x1F aac gm initial setting  */
#define XTAL_GM_INIT_4_0(x)                         ((u32)(((x) & 0x0000001F) << 8))
#define XTAL_GET_GM_INIT_4_0(x)                     ((u32)(((x >> 8) & 0x0000001F)))
#define XTAL_BIT_GM_STEP                            ((u32)0x00000001 << 7)                   /*!< R/W   0   aac gm searching step. 0:step 1, 1:step 2 */
#define XTAL_MASK_XAAC_GM_OFFSET_4_0                ((u32)0x0000001F << 2)                   /*!< R/W   0   aac calibrated gm result +/- offset */
#define XTAL_XAAC_GM_OFFSET_4_0(x)                  ((u32)(((x) & 0x0000001F) << 2))
#define XTAL_GET_XAAC_GM_OFFSET_4_0(x)              ((u32)(((x >> 2) & 0x0000001F)))
#define XTAL_BIT_OFFSET_PLUS                        ((u32)0x00000001 << 1)                   /*!< R/W   1   0: aac calibrated gm result - offset , 1:aac calibrated gm result + offset  */
#define XTAL_BIT_RESET_N                            ((u32)0x00000001 << 0)                   /*!< R/W   0   AAC digital circuit reset (low to high) */
/** @} */


/** @defgroup REG_XTAL_CLK_CTRL6
 * @brief
 * @{
 */
#define XTAL_MASK_XTAL_LDO_LPS_2_0                  ((u32)0x00000007 << 26)                  /*!< R/W   0x2
                                                                                              * (after oscillation, into LPS)
                                                                                              * 111: 1.1V  110: 1.05V  101:1V  100:0.95V
                                                                                              * 011: 0.9V  010: 0.85V   001: 0.8V  000: 0.75V
                                                                                              */
#define XTAL_XTAL_LDO_LPS_2_0(x)                    ((u32)(((x) & 0x00000007) << 26))
#define XTAL_GET_XTAL_LDO_LPS_2_0(x)                ((u32)(((x >> 26) & 0x00000007)))
#define XTAL_MASK_XTAL_WAIT_CYC_5_0                 ((u32)0x0000003F << 20)                  /*!< R/W   0x3 Next state wait cycle, 83ns/unit */
#define XTAL_XTAL_WAIT_CYC_5_0(x)                   ((u32)(((x) & 0x0000003F) << 20))
#define XTAL_GET_XTAL_WAIT_CYC_5_0(x)               ((u32)(((x >> 20) & 0x0000003F)))
#define XTAL_MASK_XTAL_LDO_OK_2_0                   ((u32)0x00000007 << 17)                  /*!< R/W   0x3
                                                                                              * (after oscillation)
                                                                                              * X11: 1.3V  X10: 1.2V  X01:1.1V  X00:0.9V
                                                                                              * XTAL_LDO_OK<2>: no use 
                                                                                              */
#define XTAL_XTAL_LDO_OK_2_0(x)                     ((u32)(((x) & 0x00000007) << 17))
#define XTAL_GET_XTAL_LDO_OK_2_0(x)                 ((u32)(((x >> 17) & 0x00000007)))
#define XTAL_BIT_XTAL_MD_LPOW                       ((u32)0x00000001 << 16)                  /*!< R/W   0  
                                                                                              * XTAL mode decoder low power mode
                                                                                              * 1: gated input clock
                                                                                              */
#define XTAL_MASK_XTAL_OV_RATIO_1_0                 ((u32)0x00000003 << 14)                  /*!< R/W   1  
                                                                                              * overlap unit times (4M ring)
                                                                                              * 00:  128us
                                                                                              * 01:  256us
                                                                                              * 10:  512us
                                                                                              * 11:  1024us
                                                                                              */
#define XTAL_XTAL_OV_RATIO_1_0(x)                   ((u32)(((x) & 0x00000003) << 14))
#define XTAL_GET_XTAL_OV_RATIO_1_0(x)               ((u32)(((x >> 14) & 0x00000003)))
#define XTAL_MASK_XTAL_OV_UNIT_2_0                  ((u32)0x00000007 << 11)                  /*!< R/W   0x3
                                                                                              * overlap time settling cycles (4M ring)
                                                                                              * 000:  2us
                                                                                              * 001:  4us 
                                                                                              * 010:  8us
                                                                                              * 011:  16us  
                                                                                              * 100:  32us  
                                                                                              * 101:  64us
                                                                                              * 110:  128us
                                                                                              * 111:  256us
                                                                                              */
#define XTAL_XTAL_OV_UNIT_2_0(x)                    ((u32)(((x) & 0x00000007) << 11))
#define XTAL_GET_XTAL_OV_UNIT_2_0(x)                ((u32)(((x >> 11) & 0x00000007)))
#define XTAL_MASK_XTAL_MODE_MANUAL_1_0              ((u32)0x00000003 << 9)                   /*!< R/W   0x3
                                                                                              * 01: LPS mode
                                                                                              * 10: Normal mode
                                                                                              * 11: high performance mode
                                                                                              */
#define XTAL_XTAL_MODE_MANUAL_1_0(x)                ((u32)(((x) & 0x00000003) << 9))
#define XTAL_GET_XTAL_MODE_MANUAL_1_0(x)            ((u32)(((x >> 9) & 0x00000003)))
#define XTAL_BIT_XTAL_MANU_SEL                      ((u32)0x00000001 << 8)                   /*!< R/W   0   1: enable xtal mode manual */
#define XTAL_BIT_POW_XTAL_LPS                       ((u32)0x00000001 << 7)                   /*!< R/W   0   POW_XTAL_LPS */
#define XTAL_BIT_XTAL_MODE                          ((u32)0x00000001 << 6)                   /*!< R/W   1  
                                                                                              * xtal mode define:
                                                                                              * 0: NR mode
                                                                                              * 1: HP mode
                                                                                              */
#define XTAL_BIT_XDECODE_RESET_N                    ((u32)0x00000001 << 5)                   /*!< R/W   1   Decoder digital circuit reset (low to high) */
#define XTAL_MASK_PK_END_AR_1_0                     ((u32)0x00000003 << 3)                   /*!< R/W   0x3 AAC peak sel end (star>end) */
#define XTAL_PK_END_AR_1_0(x)                       ((u32)(((x) & 0x00000003) << 3))
#define XTAL_GET_PK_END_AR_1_0(x)                   ((u32)(((x >> 3) & 0x00000003)))
#define XTAL_MASK_PK_START_AR_1_0                   ((u32)0x00000003 << 1)                   /*!< R/W   0x3 AACK peak sel start (star>end) */
#define XTAL_PK_START_AR_1_0(x)                     ((u32)(((x) & 0x00000003) << 1))
#define XTAL_GET_PK_START_AR_1_0(x)                 ((u32)(((x >> 1) & 0x00000003)))
#define XTAL_BIT_XAAC_LUT_MANUAL_EN                 ((u32)0x00000001 << 0)                   /*!< R/W   0  
                                                                                              * 1: enable LUT
                                                                                              * data: GM_MANUAL<4:0> , sel: AAC_MODE<1:0>
                                                                                              */
/** @} */


/** @defgroup REG_XTAL_CLK_CTRL7
 * @brief
 * @{
 */
#define XTAL_BIT_XTAL_LPS_CAP_CTRL                  ((u32)0x00000001 << 30)                  /*!< R/W   1   1:cap step control */
#define XTAL_MASK_XTAL_LPS_CAP_STEP_1_0             ((u32)0x00000003 << 28)                  /*!< R/W   0x2 00:1 step/01:2 step/10:4 step /11:8 step */
#define XTAL_XTAL_LPS_CAP_STEP_1_0(x)               ((u32)(((x) & 0x00000003) << 28))
#define XTAL_GET_XTAL_LPS_CAP_STEP_1_0(x)           ((u32)(((x >> 28) & 0x00000003)))
#define XTAL_MASK_XTAL_LPS_CAP_CYC_1_0              ((u32)0x00000003 << 26)                  /*!< R/W   1   00:8u/01:16u/10:32u/11:64u */
#define XTAL_XTAL_LPS_CAP_CYC_1_0(x)                ((u32)(((x) & 0x00000003) << 26))
#define XTAL_GET_XTAL_LPS_CAP_CYC_1_0(x)            ((u32)(((x >> 26) & 0x00000003)))
#define XTAL_MASK_XMD_SEL_1_0                       ((u32)0x00000003 << 24)                  /*!< R/W   0x2 00:100K/01:500K/10:4M/11:12M */
#define XTAL_XMD_SEL_1_0(x)                         ((u32)(((x) & 0x00000003) << 24))
#define XTAL_GET_XMD_SEL_1_0(x)                     ((u32)(((x >> 24) & 0x00000003)))
#define XTAL_BIT_SCAN_MODE_XMD                      ((u32)0x00000001 << 23)                  /*!< R/W   0   1:scan mode */
#define XTAL_BIT_PDCK_LPOW                          ((u32)0x00000001 << 22)                  /*!< R/W   0  
                                                                                              * PDCK low power mode, 1:gating input clk
                                                                                              * Set to 1 when PDCK finished
                                                                                              */
#define XTAL_MASK_XPDCK_VREF_SEL_4_0                ((u32)0x0000001F << 17)                  /*!< R/W   0x3
                                                                                              * PDCK calibrated result +offset 
                                                                                              * For AAC robust +3
                                                                                              */
#define XTAL_XPDCK_VREF_SEL_4_0(x)                  ((u32)(((x) & 0x0000001F) << 17))
#define XTAL_GET_XPDCK_VREF_SEL_4_0(x)              ((u32)(((x >> 17) & 0x0000001F)))
#define XTAL_MASK_XTAL_PDCK_UNIT_1_0                ((u32)0x00000003 << 15)                  /*!< R/W   1  
                                                                                              * PDCK settling cycles (4M CLK)
                                                                                              * 00: 4us
                                                                                              * 01: 8us 
                                                                                              * 10: 16us 
                                                                                              * 11: 32us
                                                                                              */
#define XTAL_XTAL_PDCK_UNIT_1_0(x)                  ((u32)(((x) & 0x00000003) << 15))
#define XTAL_GET_XTAL_PDCK_UNIT_1_0(x)              ((u32)(((x >> 15) & 0x00000003)))
#define XTAL_MASK_VREF_INIT_4_0                     ((u32)0x0000001F << 10)                  /*!< R/W   0x1F PDCK VREF initial setting */
#define XTAL_VREF_INIT_4_0(x)                       ((u32)(((x) & 0x0000001F) << 10))
#define XTAL_GET_VREF_INIT_4_0(x)                   ((u32)(((x >> 10) & 0x0000001F)))
#define XTAL_MASK_VREF_MANUAL_4_0                   ((u32)0x0000001F << 5)                   /*!< R/W   0x1F VREF manual at EN_XTAL_PDCK_DIGI=0 */
#define XTAL_VREF_MANUAL_4_0(x)                     ((u32)(((x) & 0x0000001F) << 5))
#define XTAL_GET_VREF_MANUAL_4_0(x)                 ((u32)(((x >> 5) & 0x0000001F)))
#define XTAL_MASK_PDCK_WAIT_CYC_1_0                 ((u32)0x00000003 << 3)                   /*!< R/W   1  
                                                                                              * Enable PDCK waiting (4M CLK)
                                                                                              * 00: 64us
                                                                                              * 01:128us 
                                                                                              * 10:256us 
                                                                                              * 11:512us
                                                                                              */
#define XTAL_PDCK_WAIT_CYC_1_0(x)                   ((u32)(((x) & 0x00000003) << 3))
#define XTAL_GET_PDCK_WAIT_CYC_1_0(x)               ((u32)(((x >> 3) & 0x00000003)))
#define XTAL_BIT_PDCK_SEARCH_MODE                   ((u32)0x00000001 << 2)                   /*!< R/W   0   0: binary search, 1: full search from 31  */
#define XTAL_BIT_EN_XTAL_PDCK_DIGI                  ((u32)0x00000001 << 1)                   /*!< R/W   0   Default set 0 due to POW_XTAL will 1 before do PDCK */
#define XTAL_BIT_PDCK_RESET_N                       ((u32)0x00000001 << 0)                   /*!< R/W   0   PDCK digital circuit reset (low to high) */
/** @} */


/** @defgroup REG_XTAL_MISC
 * @brief
 * @{
 */
#define XTAL_BIT_AON_XTAL_RDY                       ((u32)0x00000001 << 0)                   /*!< R     0   High performance XTALClock Ready */
/** @} */


/** @defgroup REG_XTAL_SPARE0
 * @brief
 * @{
 */
#define XTAL_MASK_SW_USE0                           ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0   Spare for SW */
#define XTAL_SW_USE0(x)                             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define XTAL_GET_SW_USE0(x)                         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_XTAL_SPARE1
 * @brief
 * @{
 */
#define XTAL_MASK_SW_USE1                           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 Sapre for SW */
#define XTAL_SW_USE1(x)                             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define XTAL_GET_SW_USE1(x)                         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */




/** @defgroup XTAL_Exported_Types XTAL Exported Types
* @{
*/
/**
* @brief XTAL Register Declaration
*/

typedef struct {
	__IO uint32_t XTAL_CLK_CTRL0;                               /*!< Register,  Address offset:0x0000 */
	__IO uint32_t XTAL_CLK_CTRL1;                               /*!< Register,  Address offset:0x0004 */
	__IO uint32_t XTAL_CLK_CTRL2;                               /*!< Register,  Address offset:0x0008 */
	__IO uint32_t XTAL_CLK_CTRL3;                               /*!< Register,  Address offset:0x000C */
	__IO uint32_t XTAL_CLK_CTRL4;                               /*!< Register,  Address offset:0x0010 */
	__IO uint32_t XTAL_CLK_CTRL5;                               /*!< Register,  Address offset:0x0014 */
	__IO uint32_t XTAL_CLK_CTRL6;                               /*!< Register,  Address offset:0x0018 */
	__IO uint32_t XTAL_CLK_CTRL7;                               /*!< Register,  Address offset:0x001C */
	__I  uint32_t XTAL_MISC;                                    /*!< Register,  Address offset:0x0020 */
	__IO uint32_t XTAL_SPARE0;                                  /*!< Register,  Address offset:0x0024 */
	__IO uint32_t XTAL_SPARE1;                                  /*!< Register,  Address offset:0x0028 */
} XTAL_TypeDef;

#define XTAL_BASE ((XTAL_TypeDef *) (0x40228000))
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
#endif  // SYSREG_XTAL_H
