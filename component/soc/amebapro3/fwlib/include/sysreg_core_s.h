/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_CORE_S_H
#define SYSREG_CORE_S_H

/* AUTO_GEN_START */

/* CORE_S is a part of SYSON_S */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup CORE_S
* @brief CORE_S driver modules
* @{
*/


/** @defgroup REG_CORE_S_DUMMY0
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_VID_AID_INFO1
 * @brief
 * @{
 */
#define CORE_S_MASK_AID_PIC_HEIGHT                  ((u32)0x00000FFF << 16)                  /*!< R/W     0 Conect to pin isp_pic_height */
#define CORE_S_AID_PIC_HEIGHT(x)                    ((u32)(((x) & 0x00000FFF) << 16))
#define CORE_S_GET_AID_PIC_HEIGHT(x)                ((u32)(((x >> 16) & 0x00000FFF)))
#define CORE_S_MASK_AID_PIC_WIDTH                   ((u32)0x00000FFF << 0)                   /*!< R/W     0 Conect to pin isp_pic_width */
#define CORE_S_AID_PIC_WIDTH(x)                     ((u32)(((x) & 0x00000FFF) << 0))
#define CORE_S_GET_AID_PIC_WIDTH(x)                 ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup REG_VID_AID_INFO2
 * @brief
 * @{
 */
#define CORE_S_MASK_AID_PIX_FMT_CFG                 ((u32)0x000003FF << 2)                   /*!< R/W     0 Conect to pin aid_pix_format_config */
#define CORE_S_AID_PIX_FMT_CFG(x)                   ((u32)(((x) & 0x000003FF) << 2))
#define CORE_S_GET_AID_PIX_FMT_CFG(x)               ((u32)(((x >> 2) & 0x000003FF)))
#define CORE_S_BIT_AID_SWITCH_MODE                  ((u32)0x00000001 << 1)                   /*!< R/W     0 Conect to pin aid_switch_mode */
#define CORE_S_BIT_AID_SWITCH_ENB                   ((u32)0x00000001 << 0)                   /*!< R/W     0 Conect to pin aid_switch_enable */
/** @} */


/** @defgroup REG_AID_DATA_CTRL
 * @brief
 * @{
 */
#define CORE_S_MASK_CMD_CODE                        ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0   None */
#define CORE_S_CMD_CODE(x)                          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_CMD_CODE(x)                      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CORE_S_DUMMY1
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_MEM_CORE_CTRL0
 * @brief
 * @{
 */
#define CORE_S_BIT_TMCK_DDRC_SRAM                   ((u32)0x00000001 << 29)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * DDRC_SRAM
                                                                                              */
#define CORE_S_BIT_DVSE_DDRC_SRAM                   ((u32)0x00000001 << 28)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * DDRC_SRAM
                                                                                              */
#define CORE_S_MASK_DVS_DDRC_SRAM                   ((u32)0x0000000F << 24)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * DDRC_SRAM
                                                                                              */
#define CORE_S_DVS_DDRC_SRAM(x)                     ((u32)(((x) & 0x0000000F) << 24))
#define CORE_S_GET_DVS_DDRC_SRAM(x)                 ((u32)(((x >> 24) & 0x0000000F)))
#define CORE_S_BIT_TMCK_KM4MP_SRAM                  ((u32)0x00000001 << 23)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * KM4MP_SRAM
                                                                                              */
#define CORE_S_BIT_DVSE_KM4MP_SRAM                  ((u32)0x00000001 << 22)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * KM4MP_SRAM
                                                                                              */
#define CORE_S_MASK_DVS_KM4MP_SRAM                  ((u32)0x0000000F << 18)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * KM4MP_SRAM
                                                                                              */
#define CORE_S_DVS_KM4MP_SRAM(x)                    ((u32)(((x) & 0x0000000F) << 18))
#define CORE_S_GET_DVS_KM4MP_SRAM(x)                ((u32)(((x >> 18) & 0x0000000F)))
#define CORE_S_BIT_TMCK_ISP_SRAM                    ((u32)0x00000001 << 17)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * ISP_SRAM
                                                                                              */
#define CORE_S_BIT_DVSE_ISP_SRAM                    ((u32)0x00000001 << 16)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * ISP_SRAM
                                                                                              */
#define CORE_S_MASK_DVS_ISP_SRAM                    ((u32)0x0000000F << 12)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * ISP_SRAM
                                                                                              */
#define CORE_S_DVS_ISP_SRAM(x)                      ((u32)(((x) & 0x0000000F) << 12))
#define CORE_S_GET_DVS_ISP_SRAM(x)                  ((u32)(((x >> 12) & 0x0000000F)))
#define CORE_S_BIT_TMCK_NN_SRAM                     ((u32)0x00000001 << 11)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * NN_SRAM
                                                                                              */
#define CORE_S_BIT_DVSE_NN_SRAM                     ((u32)0x00000001 << 10)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * NN_SRAM
                                                                                              */
#define CORE_S_MASK_DVS_NN_SRAM                     ((u32)0x0000000F << 6)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * NN_SRAM
                                                                                              */
#define CORE_S_DVS_NN_SRAM(x)                       ((u32)(((x) & 0x0000000F) << 6))
#define CORE_S_GET_DVS_NN_SRAM(x)                   ((u32)(((x >> 6) & 0x0000000F)))
#define CORE_S_BIT_TMCK_ENC_SRAM                    ((u32)0x00000001 << 5)                   /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * ENC_SRAM
                                                                                              */
#define CORE_S_BIT_DVSE_ENC_SRAM                    ((u32)0x00000001 << 4)                   /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * ENC_SRAM
                                                                                              */
#define CORE_S_MASK_DVS_ENC_SRAM                    ((u32)0x0000000F << 0)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * ENC_SRAM
                                                                                              */
#define CORE_S_DVS_ENC_SRAM(x)                      ((u32)(((x) & 0x0000000F) << 0))
#define CORE_S_GET_DVS_ENC_SRAM(x)                  ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_MEM_CORE_CTRL1
 * @brief
 * @{
 */
#define CORE_S_BIT_TMCK_GE_SRAM                     ((u32)0x00000001 << 23)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * GE_SRAM
                                                                                              */
#define CORE_S_BIT_DVSE_GE_SRAM                     ((u32)0x00000001 << 22)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * GE_SRAM
                                                                                              */
#define CORE_S_MASK_DVS_GE_SRAM                     ((u32)0x0000000F << 18)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * GE_SRAM
                                                                                              */
#define CORE_S_DVS_GE_SRAM(x)                       ((u32)(((x) & 0x0000000F) << 18))
#define CORE_S_GET_DVS_GE_SRAM(x)                   ((u32)(((x >> 18) & 0x0000000F)))
#define CORE_S_BIT_TMCK_TSC_SRAM                    ((u32)0x00000001 << 17)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * TSC_SRAM
                                                                                              */
#define CORE_S_BIT_DVSE_TSC_SRAM                    ((u32)0x00000001 << 16)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * TSC_SRAM
                                                                                              */
#define CORE_S_MASK_DVS_TSC_SRAM                    ((u32)0x0000000F << 12)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * TSC_SRAM
                                                                                              */
#define CORE_S_DVS_TSC_SRAM(x)                      ((u32)(((x) & 0x0000000F) << 12))
#define CORE_S_GET_DVS_TSC_SRAM(x)                  ((u32)(((x >> 12) & 0x0000000F)))
#define CORE_S_BIT_TMCK_AID_SRAM                    ((u32)0x00000001 << 11)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * AID_SRAM
                                                                                              */
#define CORE_S_BIT_DVSE_AID_SRAM                    ((u32)0x00000001 << 10)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * AID_SRAM
                                                                                              */
#define CORE_S_MASK_DVS_AID_SRAM                    ((u32)0x0000000F << 6)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * AID_SRAM
                                                                                              */
#define CORE_S_DVS_AID_SRAM(x)                      ((u32)(((x) & 0x0000000F) << 6))
#define CORE_S_GET_DVS_AID_SRAM(x)                  ((u32)(((x >> 6) & 0x0000000F)))
#define CORE_S_BIT_TMCK_VID_SRAM                    ((u32)0x00000001 << 5)                   /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * VID_SRAM
                                                                                              */
#define CORE_S_BIT_DVSE_VID_SRAM                    ((u32)0x00000001 << 4)                   /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * VID_SRAM
                                                                                              */
#define CORE_S_MASK_DVS_VID_SRAM                    ((u32)0x0000000F << 0)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * VID_SRAM
                                                                                              */
#define CORE_S_DVS_VID_SRAM(x)                      ((u32)(((x) & 0x0000000F) << 0))
#define CORE_S_GET_DVS_VID_SRAM(x)                  ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_CORE_S_DUMMY2
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY3
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY4
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY5
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY6
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY7
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY8
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY9
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY10
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY11
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY12
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY13
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY14
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY15
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY16
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY17
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY18
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY19
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY20
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY21
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY22
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY23
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY24
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY25
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY26
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY27
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY43
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY44
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY28
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY29
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY30
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_HS_PLATFORM_PARA
 * @brief
 * @{
 */
#define CORE_S_BIT_DRAM_ADAP_STATUS                 ((u32)0x00000001 << 10)                  /*!< R     0  
                                                                                              * dram_adap_idle 
                                                                                              * Note: Tie to 0 in RTL
                                                                                              */
#define CORE_S_BIT_LEXRA_TO_AXI_ADAP7_IDLE          ((u32)0x00000001 << 9)                   /*!< R     1   lxm7_idle */
#define CORE_S_BIT_LEXRA_TO_AXI_ADAP6_IDLE          ((u32)0x00000001 << 8)                   /*!< R     1   lxm6_idle */
/** @} */


/** @defgroup REG_SYS_PLATFORM_CTRL0
 * @brief
 * @{
 */
#define CORE_S_BIT_HS_GDMA1_CLK_ALWS_DIS            ((u32)0x00000001 << 13)                  /*!< R/W     0 gdma_clk_always_disable */
#define CORE_S_BIT_HS_GDMA1_CLK_ALWS_EN             ((u32)0x00000001 << 12)                  /*!< R/W   1   gdma_clk_always_enable */
/** @} */


/** @defgroup REG_CORE_PLATFORM_DBG0
 * @brief
 * @{
 */
#define CORE_S_MASK_CORE_RXI300_DBG_STS0            ((u32)0xFFFFFFFF << 0)                   /*!< R     0   Debug signal from CORE RXI300, core_rxi300_port_status[31:0] */
#define CORE_S_CORE_RXI300_DBG_STS0(x)              ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_CORE_RXI300_DBG_STS0(x)          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CORE_PLATFORM_DBG1
 * @brief
 * @{
 */
#define CORE_S_MASK_CORE_RXI300_DBG_STS1            ((u32)0x000007FF << 0)                   /*!< R     0   Debug signal from CORE RXI300, core_rxi300_port_status[42:32] */
#define CORE_S_CORE_RXI300_DBG_STS1(x)              ((u32)(((x) & 0x000007FF) << 0))
#define CORE_S_GET_CORE_RXI300_DBG_STS1(x)          ((u32)(((x >> 0) & 0x000007FF)))
/** @} */


/** @defgroup REG_CORE_PLATFORM_DBG2
 * @brief
 * @{
 */
#define CORE_S_MASK_CA32_PLATFORM_DBG_STS           ((u32)0x0000000F << 3)                   /*!< R     0   Debug signal from CA32 PLATFORM */
#define CORE_S_CA32_PLATFORM_DBG_STS(x)             ((u32)(((x) & 0x0000000F) << 3))
#define CORE_S_GET_CA32_PLATFORM_DBG_STS(x)         ((u32)(((x >> 3) & 0x0000000F)))
#define CORE_S_MASK_AID_PLATFORM_DBG_STS            ((u32)0x00000007 << 0)                   /*!< R     0   Debug signal from AID PLATFORM */
#define CORE_S_AID_PLATFORM_DBG_STS(x)              ((u32)(((x) & 0x00000007) << 0))
#define CORE_S_GET_AID_PLATFORM_DBG_STS(x)          ((u32)(((x >> 0) & 0x00000007)))
/** @} */


/** @defgroup REG_CORE_S_DUMMY34
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY35
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_SYS_Audio_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_SYS_SPORT_CODEC_SEL              ((u32)0x00000001 << 20)                  /*!< R/W     0
                                                                                              * Codec Select
                                                                                              * 0: Internal Codec
                                                                                              * 1: External Codec
                                                                                              */
#define CORE_S_MASK_SYS_CODEC_LDO_TUNE              ((u32)0x0000007F << 13)                  /*!< R/W   1011111 None */
#define CORE_S_SYS_CODEC_LDO_TUNE(x)                ((u32)(((x) & 0x0000007F) << 13))
#define CORE_S_GET_SYS_CODEC_LDO_TUNE(x)            ((u32)(((x >> 13) & 0x0000007F)))
#define CORE_S_BIT_SYS_CODEC_LDO_EN                 ((u32)0x00000001 << 12)                  /*!< R/W   0  
                                                                                              * LDO power control
                                                                                              * 0: disable LDO
                                                                                              * 1: enable LDO
                                                                                              */
#define CORE_S_BIT_SYS_CODEC_PORB                   ((u32)0x00000001 << 11)                  /*!< R/W   0   0: power down codec */
#define CORE_S_BIT_SYS_CODEC_SCLK_EN                ((u32)0x00000001 << 10)                  /*!< R/W   0   1: enable codec Sclock */
#define CORE_S_BIT_SYS_CODEC_EN                     ((u32)0x00000001 << 8)                   /*!< R/W   0   1: enable codec IP */
#define CORE_S_BIT_SYS_SI_SCLK_EN                   ((u32)0x00000001 << 7)                   /*!< R/W     0 1: enable si Sclock */
#define CORE_S_BIT_SYS_SI_PCLK_EN                   ((u32)0x00000001 << 6)                   /*!< R/W     0 1: enable si  Pclock */
#define CORE_S_BIT_SYS_SI_EN                        ((u32)0x00000001 << 5)                   /*!< R/W     0 1: enable si  IP */
#define CORE_S_MASK_SYS_SPORT_SRC_SEL               ((u32)0x00000003 << 3)                   /*!< R/W   00 
                                                                                              * 00: 40MHz
                                                                                              * 01: 98.304kHz
                                                                                              * 10: 45.158kHz
                                                                                              * 11: rsvd
                                                                                              */
#define CORE_S_SYS_SPORT_SRC_SEL(x)                 ((u32)(((x) & 0x00000003) << 3))
#define CORE_S_GET_SYS_SPORT_SRC_SEL(x)             ((u32)(((x >> 3) & 0x00000003)))
#define CORE_S_BIT_SYS_SPORT_SCLK_EN                ((u32)0x00000001 << 2)                   /*!< R/W     0 1: enable sport Sclock */
#define CORE_S_BIT_SYS_SPORT_PCLK_EN                ((u32)0x00000001 << 1)                   /*!< R/W     0 1: enable sport  Pclock */
#define CORE_S_BIT_SYS_SPORT_EN                     ((u32)0x00000001 << 0)                   /*!< R/W     0 1: enable sport  IP */
/** @} */


/** @defgroup REG_SYS_ISP_CTRL
 * @brief
 * @{
 */
#define CORE_S_MASK_SYS_ISP_DBG_SEL                 ((u32)0x0000003F << 16)                  /*!< R/W     0 select ISP dbg mux */
#define CORE_S_SYS_ISP_DBG_SEL(x)                   ((u32)(((x) & 0x0000003F) << 16))
#define CORE_S_GET_SYS_ISP_DBG_SEL(x)               ((u32)(((x >> 16) & 0x0000003F)))
/** @} */


/** @defgroup REG_CORE_S_DUMMY36
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_SYS_OTG_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_USBPHY_XTAL_SEL                  ((u32)0x00000001 << 30)                  /*!< R/W   0   0:Analog XTAL, 1:Digital XTAL */
#define CORE_S_MASK_USBPHY_CKREF_SEL                ((u32)0x0000000F << 26)                  /*!< R/W   0  
                                                                                              * 0000: 40MHz 
                                                                                              * 0001: 25MHz 
                                                                                              * 0010: 54MHz
                                                                                              * 0011: 19.2MHz 
                                                                                              * 0100: 20MHz 
                                                                                              * 0101: 26MHz
                                                                                              */
#define CORE_S_USBPHY_CKREF_SEL(x)                  ((u32)(((x) & 0x0000000F) << 26))
#define CORE_S_GET_USBPHY_CKREF_SEL(x)              ((u32)(((x >> 26) & 0x0000000F)))
#define CORE_S_BIT_SYS_ISO_USBA_EN                  ((u32)0x00000001 << 25)                  /*!< R/W   1   1: enable  usb analogy isolation */
#define CORE_S_BIT_SYS_ISO_USBD_EN                  ((u32)0x00000001 << 24)                  /*!< R/W   1   1: enable  usb digital isolation */
#define CORE_S_BIT_USB_IBX2MIPI_EN                  ((u32)0x00000001 << 22)                  /*!< R/W   0   1: Enable IBX to MIPI, DDR and EPHY */
#define CORE_S_BIT_USB_IBX2USB_EN                   ((u32)0x00000001 << 21)                  /*!< R/W   0   1: Enable IBX to USB */
#define CORE_S_BIT_USB_PDN33                        ((u32)0x00000001 << 20)                  /*!< R/W   1   1: power up USB */
#define CORE_S_BIT_SYS_UABG_EN                      ((u32)0x00000001 << 19)                  /*!< R/W     0 1. Enable bandgap */
#define CORE_S_BIT_SYS_UAHV_EN                      ((u32)0x00000001 << 18)                  /*!< R/W     0
                                                                                              * This signal is feed to power cut circuit.
                                                                                              * 0: disable USB PHY analog 3.3v power.
                                                                                              * 1: enable USB PHY analog 3.3V power
                                                                                              * 
                                                                                              * note: This is also bandgap power, if want to enable bandgap, this bit is also need be set 1
                                                                                              */
#define CORE_S_BIT_SYS_USBD_EN                      ((u32)0x00000001 << 16)                  /*!< R/W     0 1: USB Digital power cut enable (USB PHY analog 1.2V power cut enable) */
#define CORE_S_BIT_SYS_OTG_UTMI_CLK_EN              ((u32)0x00000001 << 5)                   /*!< R/W     0 1: OTG utmi enable */
#define CORE_S_BIT_SYS_OTG_CLK_EN                   ((u32)0x00000001 << 4)                   /*!< R/W     0 1: OTG bus clock enable */
#define CORE_S_BIT_SYS__LPM_PLL_ALIVE               ((u32)0x00000001 << 3)                   /*!< R/W   0   1: Keep utmi clock active during suspend state */
#define CORE_S_BIT_SYS_OTG_DIGPADEN                 ((u32)0x00000001 << 2)                   /*!< R/W   0   None */
#define CORE_S_BIT_SYS_OTG_RST_MUX                  ((u32)0x00000001 << 1)                   /*!< R/W     0 0: Mask WDOG_RST and  "cpu_r0_rst" for OTG */
#define CORE_S_BIT_SYS_OTG_EN                       ((u32)0x00000001 << 0)                   /*!< R/W     0 1: enable OTG IP */
/** @} */


/** @defgroup REG_SYS_SDIO_SD_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_SYS_SD_CK100_VP1_RDY             ((u32)0x00000001 << 26)                  /*!< R       0 sdh_clk100m_vp1 clock ready */
#define CORE_S_BIT_SYS_SD_CK100_VP0_RDY             ((u32)0x00000001 << 25)                  /*!< R       0 sdh_clk100m_vp0 clock ready */
#define CORE_S_BIT_SYS_SD_CK100_RDY                 ((u32)0x00000001 << 24)                  /*!< R       0 sdh_clk100m clock ready */
#define CORE_S_BIT_SD_PLL_BYPASS_MODE               ((u32)0x00000001 << 23)                  /*!< R/W   0   0: Use Internal clock, 1: Use tester clock */
#define CORE_S_MASK_SYS_SD_CLK2_PSYSEL              ((u32)0x00000007 << 20)                  /*!< R/W     0
                                                                                              * clk100m_vp1 phase shift selection (shift from clk100m)
                                                                                              * 2'b00 :shift 0 degree
                                                                                              * 2'b01 :shift 90 degree
                                                                                              * 2'b10 :shift 180 degree
                                                                                              * 2'b11 :shift 270 degree
                                                                                              */
#define CORE_S_SYS_SD_CLK2_PSYSEL(x)                ((u32)(((x) & 0x00000007) << 20))
#define CORE_S_GET_SYS_SD_CLK2_PSYSEL(x)            ((u32)(((x >> 20) & 0x00000007)))
#define CORE_S_MASK_SYS_SD_CLK1_PSYSEL              ((u32)0x00000007 << 16)                  /*!< R/W     0
                                                                                              * clk100m_vp0 phase shift selection  (shift from clk100m)
                                                                                              * 2'b00 :shift 0 degree
                                                                                              * 2'b01 :shift 90 degree
                                                                                              * 2'b10 :shift 180 degree
                                                                                              * 2'b11 :shift 270 degree
                                                                                              */
#define CORE_S_SYS_SD_CLK1_PSYSEL(x)                ((u32)(((x) & 0x00000007) << 16))
#define CORE_S_GET_SYS_SD_CLK1_PSYSEL(x)            ((u32)(((x >> 16) & 0x00000007)))
#define CORE_S_BIT_SYS_SD_ON_RST_MUX                ((u32)0x00000001 << 12)                  /*!< R/W   0   0: Mask WDOG_RST and  "cpu_r0_rst" for SD ON */
#define CORE_S_BIT_SYS_SD_SYST_SCLK_EN              ((u32)0x00000001 << 11)                  /*!< R/W     0 1: Enable SD host SCLK */
#define CORE_S_BIT_SYS_SD_SYST_100M_EN              ((u32)0x00000001 << 10)                  /*!< R/W     0 SD 100Mhz clock generator function enable */
#define CORE_S_BIT_SYS_SD_SYST_PCLK_EN              ((u32)0x00000001 << 9)                   /*!< R/W     0
                                                                                              * 1: enable SD SYST pclock
                                                                                              * NOT USED
                                                                                              */
#define CORE_S_BIT_SYS_SDH_EN                       ((u32)0x00000001 << 8)                   /*!< R/W     0
                                                                                              * 1: enable SD host IP block
                                                                                              * NOT USED
                                                                                              */
#define CORE_S_BIT_SYS_SDIOD_ON_RST_MUX             ((u32)0x00000001 << 4)                   /*!< R/W     0 0: Mask WDOG_RST and  "cpu_r0_rst"  for SDIO */
#define CORE_S_BIT_SDIO_PLL_BYPASS_MODE             ((u32)0x00000001 << 2)                   /*!< R/W     0 0: Use Internal clock, 1: Use tester clock */
#define CORE_S_BIT_SYS_SDIO_HOST_CLK_EN             ((u32)0x00000001 << 1)                   /*!< R/W     0 1: enable SDIO Host clock */
#define CORE_S_BIT_SYS_SDIOD_HOST_EN                ((u32)0x00000001 << 0)                   /*!< R/W     0 1: enable SDIO Host IP block */
/** @} */


/** @defgroup REG_CORE_S_DUMMY37
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY38
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY39
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY40
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_SYS_SGPIO_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_SYS_SGPIO0_INT_CLK_SEL           ((u32)0x00000001 << 7)                   /*!< R/W     0
                                                                                              * 1: sclk
                                                                                              * 0: apb0
                                                                                              */
/** @} */


/** @defgroup REG_CORE_S_DUMMY41
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_CORE_S_DUMMY42
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_DSS_CPU
 * @brief
 * @{
 */
#define CORE_S_BIT_READY                            ((u32)0x00000001 << 26)                  /*!< R       0 Ready strobe to read count_out value */
#define CORE_S_MASK_COUNT_OUT                       ((u32)0x000FFFFF << 6)                   /*!< R       0
                                                                                              * Counter value indicates the speed of selected ring oscillator
                                                                                              * Cycle time = (count_out*dss_clk period)/ 16384
                                                                                              */
#define CORE_S_COUNT_OUT(x)                         ((u32)(((x) & 0x000FFFFF) << 6))
#define CORE_S_GET_COUNT_OUT(x)                     ((u32)(((x >> 6) & 0x000FFFFF)))
#define CORE_S_MASK_RO_SEL                          ((u32)0x00000007 << 3)                   /*!< R/W     0
                                                                                              * Ring Oscillator selection
                                                                                              * 000: select dss_clk (for test mode)
                                                                                              * 001 : select 1st ring oscillator (RVT NAND)
                                                                                              * 010 : select 2nd ring oscillator (RVT NOR)
                                                                                              * 011 : select 3rd ring oscillator (H/LVT NAND)
                                                                                              * 100 : select 4th ring oscillator (H/LVT NOR)
                                                                                              * 101 : select mixed-library ring oscillator
                                                                                              * Others : not used
                                                                                              */
#define CORE_S_RO_SEL(x)                            ((u32)(((x) & 0x00000007) << 3))
#define CORE_S_GET_RO_SEL(x)                        ((u32)(((x >> 3) & 0x00000007)))
#define CORE_S_BIT_WRITE_SEL                        ((u32)0x00000001 << 2)                   /*!< R/W     0
                                                                                              * Wire selection
                                                                                              * 0 : long wire/delay cell/clock buffer
                                                                                              * 1 : short wire
                                                                                              */
#define CORE_S_BIT_DSS_RST_N                        ((u32)0x00000001 << 1)                   /*!< R/W     0 Reset signal of Speed-Sensor (low active) */
#define CORE_S_BIT_SPEED_EN                         ((u32)0x00000001 << 0)                   /*!< R/W     0
                                                                                              * Speed sensor enable
                                                                                              * 0: disable
                                                                                              * 1: enable
                                                                                              */
/** @} */


/** @defgroup REG_DSS_Encoder
 * @brief
 * @{
 */
#define CORE_S_BIT_READY_148                        ((u32)0x00000001 << 26)                  /*!< R       0 Ready strobe to read count_out value */
#define CORE_S_MASK_COUNT_OUT_148                   ((u32)0x000FFFFF << 6)                   /*!< R       0
                                                                                              * Counter value indicates the speed of selected ring oscillator
                                                                                              * Cycle time = (count_out*dss_clk period)/ 16384
                                                                                              */
#define CORE_S_COUNT_OUT_148(x)                     ((u32)(((x) & 0x000FFFFF) << 6))
#define CORE_S_GET_COUNT_OUT_148(x)                 ((u32)(((x >> 6) & 0x000FFFFF)))
#define CORE_S_MASK_RO_SEL_148                      ((u32)0x00000007 << 3)                   /*!< R/W     0
                                                                                              * Ring Oscillator selection
                                                                                              * 000: select dss_clk (for test mode)
                                                                                              * 001 : select 1st ring oscillator (RVT NAND)
                                                                                              * 010 : select 2nd ring oscillator (RVT NOR)
                                                                                              * 011 : select 3rd ring oscillator (H/LVT NAND)
                                                                                              * 100 : select 4th ring oscillator (H/LVT NOR)
                                                                                              * 101 : select mixed-library ring oscillator
                                                                                              * Others : not used
                                                                                              */
#define CORE_S_RO_SEL_148(x)                        ((u32)(((x) & 0x00000007) << 3))
#define CORE_S_GET_RO_SEL_148(x)                    ((u32)(((x >> 3) & 0x00000007)))
#define CORE_S_BIT_WRITE_SEL_148                    ((u32)0x00000001 << 2)                   /*!< R/W     0
                                                                                              * Wire selection
                                                                                              * 0 : long wire/delay cell/clock buffer
                                                                                              * 1 : short wire
                                                                                              */
#define CORE_S_BIT_DSS_RST_N_148                    ((u32)0x00000001 << 1)                   /*!< R/W     0 Reset signal of Speed-Sensor (low active) */
#define CORE_S_BIT_SPEED_EN_148                     ((u32)0x00000001 << 0)                   /*!< R/W     0
                                                                                              * Speed sensor enable
                                                                                              * 0: disable
                                                                                              * 1: enable
                                                                                              */
/** @} */


/** @defgroup REG_DSS_WLAN_OFF
 * @brief
 * @{
 */
#define CORE_S_BIT_READY_14C                        ((u32)0x00000001 << 26)                  /*!< R       0 Ready strobe to read count_out value */
#define CORE_S_MASK_COUNT_OUT_14C                   ((u32)0x000FFFFF << 6)                   /*!< R       0
                                                                                              * Counter value indicates the speed of selected ring oscillator
                                                                                              * Cycle time = (count_out*dss_clk period)/ 16384
                                                                                              */
#define CORE_S_COUNT_OUT_14C(x)                     ((u32)(((x) & 0x000FFFFF) << 6))
#define CORE_S_GET_COUNT_OUT_14C(x)                 ((u32)(((x >> 6) & 0x000FFFFF)))
#define CORE_S_MASK_RO_SEL_14C                      ((u32)0x00000007 << 3)                   /*!< R/W     0
                                                                                              * Ring Oscillator selection
                                                                                              * 000: select dss_clk (for test mode)
                                                                                              * 001 : select 1st ring oscillator (RVT NAND)
                                                                                              * 010 : select 2nd ring oscillator (RVT NOR)
                                                                                              * 011 : select 3rd ring oscillator (H/LVT NAND)
                                                                                              * 100 : select 4th ring oscillator (H/LVT NOR)
                                                                                              * 101 : select mixed-library ring oscillator
                                                                                              * Others : not used
                                                                                              */
#define CORE_S_RO_SEL_14C(x)                        ((u32)(((x) & 0x00000007) << 3))
#define CORE_S_GET_RO_SEL_14C(x)                    ((u32)(((x >> 3) & 0x00000007)))
#define CORE_S_BIT_WRITE_SEL_14C                    ((u32)0x00000001 << 2)                   /*!< R/W     0
                                                                                              * Wire selection
                                                                                              * 0 : long wire/delay cell/clock buffer
                                                                                              * 1 : short wire
                                                                                              */
#define CORE_S_BIT_DSS_RST_N_14C                    ((u32)0x00000001 << 1)                   /*!< R/W     0 Reset signal of Speed-Sensor (low active) */
#define CORE_S_BIT_SPEED_EN_14C                     ((u32)0x00000001 << 0)                   /*!< R/W     0
                                                                                              * Speed sensor enable
                                                                                              * 0: disable
                                                                                              * 1: enable
                                                                                              */
/** @} */


/** @defgroup REG_DSS_SOC
 * @brief
 * @{
 */
#define CORE_S_BIT_READY_150                        ((u32)0x00000001 << 26)                  /*!< R       0 Ready strobe to read count_out value */
#define CORE_S_MASK_COUNT_OUT_150                   ((u32)0x000FFFFF << 6)                   /*!< R       0
                                                                                              * Counter value indicates the speed of selected ring oscillator
                                                                                              * Cycle time = (count_out*dss_clk period)/ 16384
                                                                                              */
#define CORE_S_COUNT_OUT_150(x)                     ((u32)(((x) & 0x000FFFFF) << 6))
#define CORE_S_GET_COUNT_OUT_150(x)                 ((u32)(((x >> 6) & 0x000FFFFF)))
#define CORE_S_MASK_RO_SEL_150                      ((u32)0x00000007 << 3)                   /*!< R/W     0
                                                                                              * Ring Oscillator selection
                                                                                              * 000: select dss_clk (for test mode)
                                                                                              * 001 : select 1st ring oscillator (RVT NAND)
                                                                                              * 010 : select 2nd ring oscillator (RVT NOR)
                                                                                              * 011 : select 3rd ring oscillator (H/LVT NAND)
                                                                                              * 100 : select 4th ring oscillator (H/LVT NOR)
                                                                                              * 101 : select mixed-library ring oscillator
                                                                                              * Others : not used
                                                                                              */
#define CORE_S_RO_SEL_150(x)                        ((u32)(((x) & 0x00000007) << 3))
#define CORE_S_GET_RO_SEL_150(x)                    ((u32)(((x >> 3) & 0x00000007)))
#define CORE_S_BIT_WRITE_SEL_150                    ((u32)0x00000001 << 2)                   /*!< R/W     0
                                                                                              * Wire selection
                                                                                              * 0 : long wire/delay cell/clock buffer
                                                                                              * 1 : short wire
                                                                                              */
#define CORE_S_BIT_DSS_RST_N_150                    ((u32)0x00000001 << 1)                   /*!< R/W     0 Reset signal of Speed-Sensor (low active) */
#define CORE_S_BIT_SPEED_EN_150                     ((u32)0x00000001 << 0)                   /*!< R/W     0
                                                                                              * Speed sensor enable
                                                                                              * 0: disable
                                                                                              * 1: enable
                                                                                              */
/** @} */


/** @defgroup REG_DSS_REF1
 * @brief
 * @{
 */
#define CORE_S_BIT_READY_154                        ((u32)0x00000001 << 26)                  /*!< R       0 Ready strobe to read count_out value */
#define CORE_S_MASK_COUNT_OUT_154                   ((u32)0x000FFFFF << 6)                   /*!< R       0
                                                                                              * Counter value indicates the speed of selected ring oscillator
                                                                                              * Cycle time = (count_out*dss_clk period)/ 16384
                                                                                              */
#define CORE_S_COUNT_OUT_154(x)                     ((u32)(((x) & 0x000FFFFF) << 6))
#define CORE_S_GET_COUNT_OUT_154(x)                 ((u32)(((x >> 6) & 0x000FFFFF)))
#define CORE_S_MASK_RO_SEL_154                      ((u32)0x00000007 << 3)                   /*!< R/W     0
                                                                                              * Ring Oscillator selection
                                                                                              * 000: select dss_clk (for test mode)
                                                                                              * 001 : select 1st ring oscillator (RVT NAND)
                                                                                              * 010 : select 2nd ring oscillator (RVT NOR)
                                                                                              * 011 : select 3rd ring oscillator (H/LVT NAND)
                                                                                              * 100 : select 4th ring oscillator (H/LVT NOR)
                                                                                              * 101 : select mixed-library ring oscillator
                                                                                              * Others : not used
                                                                                              */
#define CORE_S_RO_SEL_154(x)                        ((u32)(((x) & 0x00000007) << 3))
#define CORE_S_GET_RO_SEL_154(x)                    ((u32)(((x >> 3) & 0x00000007)))
#define CORE_S_BIT_WRITE_SEL_154                    ((u32)0x00000001 << 2)                   /*!< R/W     0
                                                                                              * Wire selection
                                                                                              * 0 : long wire/delay cell/clock buffer
                                                                                              * 1 : short wire
                                                                                              */
#define CORE_S_BIT_DSS_RST_N_154                    ((u32)0x00000001 << 1)                   /*!< R/W     0 Reset signal of Speed-Sensor (low active) */
#define CORE_S_BIT_SPEED_EN_154                     ((u32)0x00000001 << 0)                   /*!< R/W     0
                                                                                              * Speed sensor enable
                                                                                              * 0: disable
                                                                                              * 1: enable
                                                                                              */
/** @} */


/** @defgroup REG_DSS_REF2
 * @brief
 * @{
 */
#define CORE_S_BIT_READY_158                        ((u32)0x00000001 << 26)                  /*!< R       0 Ready strobe to read count_out value */
#define CORE_S_MASK_COUNT_OUT_158                   ((u32)0x000FFFFF << 6)                   /*!< R       0
                                                                                              * Counter value indicates the speed of selected ring oscillator
                                                                                              * Cycle time = (count_out*dss_clk period)/ 16384
                                                                                              */
#define CORE_S_COUNT_OUT_158(x)                     ((u32)(((x) & 0x000FFFFF) << 6))
#define CORE_S_GET_COUNT_OUT_158(x)                 ((u32)(((x >> 6) & 0x000FFFFF)))
#define CORE_S_MASK_RO_SEL_158                      ((u32)0x00000007 << 3)                   /*!< R/W     0
                                                                                              * Ring Oscillator selection
                                                                                              * 000: select dss_clk (for test mode)
                                                                                              * 001 : select 1st ring oscillator (RVT NAND)
                                                                                              * 010 : select 2nd ring oscillator (RVT NOR)
                                                                                              * 011 : select 3rd ring oscillator (H/LVT NAND)
                                                                                              * 100 : select 4th ring oscillator (H/LVT NOR)
                                                                                              * 101 : select mixed-library ring oscillator
                                                                                              * Others : not used
                                                                                              */
#define CORE_S_RO_SEL_158(x)                        ((u32)(((x) & 0x00000007) << 3))
#define CORE_S_GET_RO_SEL_158(x)                    ((u32)(((x >> 3) & 0x00000007)))
#define CORE_S_BIT_WRITE_SEL_158                    ((u32)0x00000001 << 2)                   /*!< R/W     0
                                                                                              * Wire selection
                                                                                              * 0 : long wire/delay cell/clock buffer
                                                                                              * 1 : short wire
                                                                                              */
#define CORE_S_BIT_DSS_RST_N_158                    ((u32)0x00000001 << 1)                   /*!< R/W     0 Reset signal of Speed-Sensor (low active) */
#define CORE_S_BIT_SPEED_EN_158                     ((u32)0x00000001 << 0)                   /*!< R/W     0
                                                                                              * Speed sensor enable
                                                                                              * 0: disable
                                                                                              * 1: enable
                                                                                              */
/** @} */


/** @defgroup REG_VOL_PROBE
 * @brief
 * @{
 */
#define CORE_S_BIT_OE_SOC                           ((u32)0x00000001 << 3)                   /*!< R/W     0 None */
#define CORE_S_BIT_OE_SYSON                         ((u32)0x00000001 << 2)                   /*!< R/W     0 None */
#define CORE_S_BIT_OE_OTP                           ((u32)0x00000001 << 1)                   /*!< R/W     0 None */
#define CORE_S_BIT_OE_AON                           ((u32)0x00000001 << 0)                   /*!< R/W     0 None */
/** @} */


/** @defgroup REG_SYS_AI_SCLK_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_AI_SCLK_VLD                      ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define CORE_S_BIT_AI_SCLK_DIV_DONE                 ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define CORE_S_BIT_AI_SCLK_EN                       ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock disable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define CORE_S_MASK_AI_SCLK_SEL                     ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: CPUD2 PLL
                                                                                              * Others: 1'b0
                                                                                              */
#define CORE_S_AI_SCLK_SEL(x)                       ((u32)(((x) & 0x00000007) << 16))
#define CORE_S_GET_AI_SCLK_SEL(x)                   ((u32)(((x >> 16) & 0x00000007)))
#define CORE_S_BIT_AI_SCLK_DIVTRG                   ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define CORE_S_MASK_AI_SCLK_DIVS                    ((u32)0x0000003F << 8)                   /*!< R/W   0x1 Clock divider factor. */
#define CORE_S_AI_SCLK_DIVS(x)                      ((u32)(((x) & 0x0000003F) << 8))
#define CORE_S_GET_AI_SCLK_DIVS(x)                  ((u32)(((x >> 8) & 0x0000003F)))
#define CORE_S_MASK_AI_SCLK_DIVN                    ((u32)0x0000003F << 0)                   /*!< R/W   0x2
                                                                                              * Clock divider factor.
                                                                                              * Example:
                                                                                              * For clkdiv ratio 7/2, below is the setting:
                                                                                              * *_DIVN = 7
                                                                                              * *_DIVS = 2
                                                                                              * 
                                                                                              * Note that DIVN can't be less than DIVS
                                                                                              */
#define CORE_S_AI_SCLK_DIVN(x)                      ((u32)(((x) & 0x0000003F) << 0))
#define CORE_S_GET_AI_SCLK_DIVN(x)                  ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_AUDIO_CODEC_CLK_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_AUDIO_CODEC_CLK_VLD              ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define CORE_S_BIT_AUDIO_CODEC_CLK_DIV_DONE         ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define CORE_S_BIT_AUDIO_CODEC_CLK_EN               ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock disable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define CORE_S_MASK_AUDIO_CODEC_CLK_SEL             ((u32)0x00000007 << 16)                  /*!< R/W   0x5
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: XTAL 40M
                                                                                              * Others: 1'b0
                                                                                              */
#define CORE_S_AUDIO_CODEC_CLK_SEL(x)               ((u32)(((x) & 0x00000007) << 16))
#define CORE_S_GET_AUDIO_CODEC_CLK_SEL(x)           ((u32)(((x >> 16) & 0x00000007)))
#define CORE_S_BIT_AUDIO_CODEC_CLK_DIVTRG           ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define CORE_S_MASK_AUDIO_CODEC_CLK_DIVN            ((u32)0x0000003F << 0)                   /*!< R/W   0x0 Clock divider factor. */
#define CORE_S_AUDIO_CODEC_CLK_DIVN(x)              ((u32)(((x) & 0x0000003F) << 0))
#define CORE_S_GET_AUDIO_CODEC_CLK_DIVN(x)          ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_SYS_ENC_ACLK_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_ENC_ACLK_VLD                     ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define CORE_S_BIT_ENC_ACLK_DIV_DONE                ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define CORE_S_BIT_ENC_ACLK_EN                      ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock disable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define CORE_S_MASK_ENC_ACLK_SEL                    ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: CPUD2 PLL
                                                                                              * Others: 1'b0
                                                                                              */
#define CORE_S_ENC_ACLK_SEL(x)                      ((u32)(((x) & 0x00000007) << 16))
#define CORE_S_GET_ENC_ACLK_SEL(x)                  ((u32)(((x >> 16) & 0x00000007)))
#define CORE_S_BIT_ENC_ACLK_DIVTRG                  ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define CORE_S_MASK_ENC_ACLK_DIVS                   ((u32)0x0000003F << 8)                   /*!< R/W   0x1 Clock divider factor. */
#define CORE_S_ENC_ACLK_DIVS(x)                     ((u32)(((x) & 0x0000003F) << 8))
#define CORE_S_GET_ENC_ACLK_DIVS(x)                 ((u32)(((x >> 8) & 0x0000003F)))
#define CORE_S_MASK_ENC_ACLK_DIVN                   ((u32)0x0000003F << 0)                   /*!< R/W   0x2
                                                                                              * Clock divider factor.
                                                                                              * Example:
                                                                                              * For clkdiv ratio 7/2, below is the setting:
                                                                                              * *_DIVN = 7
                                                                                              * *_DIVS = 2
                                                                                              * 
                                                                                              * Note that DIVN can't be less than DIVS
                                                                                              */
#define CORE_S_ENC_ACLK_DIVN(x)                     ((u32)(((x) & 0x0000003F) << 0))
#define CORE_S_GET_ENC_ACLK_DIVN(x)                 ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_SYS_ENC_SCLK_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_ENC_SCLK_VLD                     ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define CORE_S_BIT_ENC_SCLK_DIV_DONE                ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define CORE_S_BIT_ENC_SCLK_EN                      ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock disable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define CORE_S_MASK_ENC_SCLK_SEL                    ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: CPUD2 PLL
                                                                                              * Others: 1'b0
                                                                                              */
#define CORE_S_ENC_SCLK_SEL(x)                      ((u32)(((x) & 0x00000007) << 16))
#define CORE_S_GET_ENC_SCLK_SEL(x)                  ((u32)(((x >> 16) & 0x00000007)))
#define CORE_S_BIT_ENC_SCLK_DIVTRG                  ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define CORE_S_MASK_ENC_SCLK_DIVS                   ((u32)0x0000003F << 8)                   /*!< R/W   0x1 Clock divider factor. */
#define CORE_S_ENC_SCLK_DIVS(x)                     ((u32)(((x) & 0x0000003F) << 8))
#define CORE_S_GET_ENC_SCLK_DIVS(x)                 ((u32)(((x >> 8) & 0x0000003F)))
#define CORE_S_MASK_ENC_SCLK_DIVN                   ((u32)0x0000003F << 0)                   /*!< R/W   0x2
                                                                                              * Clock divider factor.
                                                                                              * Example:
                                                                                              * For clkdiv ratio 7/2, below is the setting:
                                                                                              * *_DIVN = 7
                                                                                              * *_DIVS = 2
                                                                                              * 
                                                                                              * Note that DIVN can't be less than DIVS
                                                                                              */
#define CORE_S_ENC_SCLK_DIVN(x)                     ((u32)(((x) & 0x0000003F) << 0))
#define CORE_S_GET_ENC_SCLK_DIVN(x)                 ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_SYS_FAST_PCLK_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_FAST_PCLK_VLD                    ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define CORE_S_BIT_FAST_PCLK_DIV_DONE               ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define CORE_S_BIT_FAST_PCLK_EN                     ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock disable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define CORE_S_MASK_FAST_PCLK_SEL                   ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: XTAL 40M
                                                                                              * Others: 1'b0
                                                                                              */
#define CORE_S_FAST_PCLK_SEL(x)                     ((u32)(((x) & 0x00000007) << 16))
#define CORE_S_GET_FAST_PCLK_SEL(x)                 ((u32)(((x >> 16) & 0x00000007)))
#define CORE_S_BIT_FAST_PCLK_DIVTRG                 ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define CORE_S_MASK_FAST_PCLK_DIVS                  ((u32)0x0000003F << 8)                   /*!< R/W   0x1 Clock divider factor. */
#define CORE_S_FAST_PCLK_DIVS(x)                    ((u32)(((x) & 0x0000003F) << 8))
#define CORE_S_GET_FAST_PCLK_DIVS(x)                ((u32)(((x >> 8) & 0x0000003F)))
#define CORE_S_MASK_FAST_PCLK_DIVN                  ((u32)0x0000003F << 0)                   /*!< R/W   0x2
                                                                                              * Clock divider factor.
                                                                                              * Example:
                                                                                              * For clkdiv ratio 7/2, below is the setting:
                                                                                              * *_DIVN = 7
                                                                                              * *_DIVS = 2
                                                                                              * 
                                                                                              * Note that DIVN can't be less than DIVS
                                                                                              */
#define CORE_S_FAST_PCLK_DIVN(x)                    ((u32)(((x) & 0x0000003F) << 0))
#define CORE_S_GET_FAST_PCLK_DIVN(x)                ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_SYS_GE_SCLK_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_GE_SCLK_VLD                      ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define CORE_S_BIT_GE_SCLK_DIV_DONE                 ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define CORE_S_BIT_GE_SCLK_EN                       ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock disable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define CORE_S_MASK_GE_SCLK_SEL                     ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: CPUD2 PLL
                                                                                              * Others: 1'b0
                                                                                              */
#define CORE_S_GE_SCLK_SEL(x)                       ((u32)(((x) & 0x00000007) << 16))
#define CORE_S_GET_GE_SCLK_SEL(x)                   ((u32)(((x >> 16) & 0x00000007)))
#define CORE_S_BIT_GE_SCLK_DIVTRG                   ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define CORE_S_MASK_GE_SCLK_DIVS                    ((u32)0x0000003F << 8)                   /*!< R/W   0x1 Clock divider factor. */
#define CORE_S_GE_SCLK_DIVS(x)                      ((u32)(((x) & 0x0000003F) << 8))
#define CORE_S_GET_GE_SCLK_DIVS(x)                  ((u32)(((x >> 8) & 0x0000003F)))
#define CORE_S_MASK_GE_SCLK_DIVN                    ((u32)0x0000003F << 0)                   /*!< R/W   0x2
                                                                                              * Clock divider factor.
                                                                                              * Example:
                                                                                              * For clkdiv ratio 7/2, below is the setting:
                                                                                              * *_DIVN = 7
                                                                                              * *_DIVS = 2
                                                                                              * 
                                                                                              * Note that DIVN can't be less than DIVS
                                                                                              */
#define CORE_S_GE_SCLK_DIVN(x)                      ((u32)(((x) & 0x0000003F) << 0))
#define CORE_S_GET_GE_SCLK_DIVN(x)                  ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_SYS_GMAC_CLK_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_GMAC_CLK_VLD                     ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define CORE_S_BIT_GMAC_CLK_DIV_DONE                ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define CORE_S_BIT_GMAC_CLK_EN                      ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock disable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define CORE_S_MASK_GMAC_CLK_SEL                    ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: CPUD2 PLL
                                                                                              * Others: 1'b0
                                                                                              */
#define CORE_S_GMAC_CLK_SEL(x)                      ((u32)(((x) & 0x00000007) << 16))
#define CORE_S_GET_GMAC_CLK_SEL(x)                  ((u32)(((x >> 16) & 0x00000007)))
#define CORE_S_BIT_GMAC_CLK_DIVTRG                  ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define CORE_S_MASK_GMAC_CLK_DIVN                   ((u32)0x0000003F << 0)                   /*!< R/W   0x4 Clock divider factor. */
#define CORE_S_GMAC_CLK_DIVN(x)                     ((u32)(((x) & 0x0000003F) << 0))
#define CORE_S_GET_GMAC_CLK_DIVN(x)                 ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_SYS_ISP_MIPICLK_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_ISP_MIPICLK_VLD                  ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define CORE_S_BIT_ISP_MIPICLK_DIV_DONE             ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define CORE_S_BIT_ISP_MIPICLK_EN                   ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock disable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define CORE_S_MASK_ISP_MIPICLK_SEL                 ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: CPUD2 PLL
                                                                                              * Others: 1'b0
                                                                                              */
#define CORE_S_ISP_MIPICLK_SEL(x)                   ((u32)(((x) & 0x00000007) << 16))
#define CORE_S_GET_ISP_MIPICLK_SEL(x)               ((u32)(((x >> 16) & 0x00000007)))
#define CORE_S_BIT_ISP_MIPICLK_DIVTRG               ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define CORE_S_MASK_ISP_MIPICLK_DIVN                ((u32)0x0000003F << 0)                   /*!< R/W   0x0 Clock divider factor. */
#define CORE_S_ISP_MIPICLK_DIVN(x)                  ((u32)(((x) & 0x0000003F) << 0))
#define CORE_S_GET_ISP_MIPICLK_DIVN(x)              ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_SYS_ISP_BUSCLK_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_ISP_BUSCLK_VLD                   ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define CORE_S_BIT_ISP_BUSCLK_DIV_DONE              ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define CORE_S_BIT_ISP_BUSCLK_EN                    ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock disable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define CORE_S_MASK_ISP_BUSCLK_SEL                  ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: CPUD2 PLL
                                                                                              * Others: 1'b0
                                                                                              */
#define CORE_S_ISP_BUSCLK_SEL(x)                    ((u32)(((x) & 0x00000007) << 16))
#define CORE_S_GET_ISP_BUSCLK_SEL(x)                ((u32)(((x >> 16) & 0x00000007)))
#define CORE_S_BIT_ISP_BUSCLK_DIVTRG                ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define CORE_S_MASK_ISP_BUSCLK_DIVS                 ((u32)0x0000003F << 8)                   /*!< R/W   0x1 Clock divider factor. */
#define CORE_S_ISP_BUSCLK_DIVS(x)                   ((u32)(((x) & 0x0000003F) << 8))
#define CORE_S_GET_ISP_BUSCLK_DIVS(x)               ((u32)(((x >> 8) & 0x0000003F)))
#define CORE_S_MASK_ISP_BUSCLK_DIVN                 ((u32)0x0000003F << 0)                   /*!< R/W   0x1
                                                                                              * Clock divider factor.
                                                                                              * Example:
                                                                                              * For clkdiv ratio 7/2, below is the setting:
                                                                                              * *_DIVN = 7
                                                                                              * *_DIVS = 2
                                                                                              * 
                                                                                              * Note that DIVN can't be less than DIVS
                                                                                              */
#define CORE_S_ISP_BUSCLK_DIVN(x)                   ((u32)(((x) & 0x0000003F) << 0))
#define CORE_S_GET_ISP_BUSCLK_DIVN(x)               ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_SYS_ISP_SCLK_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_ISP_SCLK_VLD                     ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define CORE_S_BIT_ISP_SCLK_DIV_DONE                ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define CORE_S_BIT_ISP_SCLK_EN                      ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock disable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define CORE_S_MASK_ISP_SCLK_SEL                    ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: CPUD2 PLL
                                                                                              * Others: 1'b0
                                                                                              */
#define CORE_S_ISP_SCLK_SEL(x)                      ((u32)(((x) & 0x00000007) << 16))
#define CORE_S_GET_ISP_SCLK_SEL(x)                  ((u32)(((x >> 16) & 0x00000007)))
#define CORE_S_BIT_ISP_SCLK_DIVTRG                  ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define CORE_S_MASK_ISP_SCLK_DIVS                   ((u32)0x0000003F << 8)                   /*!< R/W   0x1 Clock divider factor. */
#define CORE_S_ISP_SCLK_DIVS(x)                     ((u32)(((x) & 0x0000003F) << 8))
#define CORE_S_GET_ISP_SCLK_DIVS(x)                 ((u32)(((x >> 8) & 0x0000003F)))
#define CORE_S_MASK_ISP_SCLK_DIVN                   ((u32)0x0000003F << 0)                   /*!< R/W   0x2
                                                                                              * Clock divider factor.
                                                                                              * Example:
                                                                                              * For clkdiv ratio 7/2, below is the setting:
                                                                                              * *_DIVN = 7
                                                                                              * *_DIVS = 2
                                                                                              * 
                                                                                              * Note that DIVN can't be less than DIVS
                                                                                              */
#define CORE_S_ISP_SCLK_DIVN(x)                     ((u32)(((x) & 0x0000003F) << 0))
#define CORE_S_GET_ISP_SCLK_DIVN(x)                 ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_SYS_KM4_MP_CLK_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_KM4_MP_CLK_XTAL40M_SEL           ((u32)0x00000001 << 25)                  /*!< R/W   0x0 1: Select XTAL 40Mhz; 0: based on [18:16] */
#define CORE_S_BIT_KM4_MP_CLK_VLD                   ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define CORE_S_BIT_KM4_MP_CLK_DIV_DONE              ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define CORE_S_BIT_KM4_MP_CLK_EN                    ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock enable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) set SLOW_PCLK_XTAL_40M_SEL = 1
                                                                                              * (2) Disable clock.
                                                                                              * (3) Poll clock valid = 0.
                                                                                              * (4) Program clock select bits.
                                                                                              * (5) Enable clock.
                                                                                              * (6) set SLOW_PCLK_XTAL_40M_SEL = 0
                                                                                              */
#define CORE_S_MASK_KM4_MP_CLK_SEL                  ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: CPUD2 PLL
                                                                                              * 0x6: XTAL 40M
                                                                                              * 0x7: OSC 4M
                                                                                              * Others: 1'b0
                                                                                              */
#define CORE_S_KM4_MP_CLK_SEL(x)                    ((u32)(((x) & 0x00000007) << 16))
#define CORE_S_GET_KM4_MP_CLK_SEL(x)                ((u32)(((x >> 16) & 0x00000007)))
#define CORE_S_BIT_KM4_MP_CLK_DIVTRG                ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define CORE_S_MASK_KM4_MP_CLK_DIVS                 ((u32)0x0000003F << 8)                   /*!< R/W   0x1 Clock divider factor. */
#define CORE_S_KM4_MP_CLK_DIVS(x)                   ((u32)(((x) & 0x0000003F) << 8))
#define CORE_S_GET_KM4_MP_CLK_DIVS(x)               ((u32)(((x >> 8) & 0x0000003F)))
#define CORE_S_MASK_KM4_MP_CLK_DIVN                 ((u32)0x0000003F << 0)                   /*!< R/W   0x2
                                                                                              * Clock divider factor.
                                                                                              * Example:
                                                                                              * For clkdiv ratio 7/2, below is the setting:
                                                                                              * *_DIVN = 7
                                                                                              * *_DIVS = 2
                                                                                              * 
                                                                                              * Note that DIVN can't be less than DIVS
                                                                                              */
#define CORE_S_KM4_MP_CLK_DIVN(x)                   ((u32)(((x) & 0x0000003F) << 0))
#define CORE_S_GET_KM4_MP_CLK_DIVN(x)               ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_SYS_NN_ACLK_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_NN_ACLK_VLD                      ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define CORE_S_BIT_NN_ACLK_DIV_DONE                 ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define CORE_S_BIT_NN_ACLK_EN                       ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock disable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define CORE_S_MASK_NN_ACLK_SEL                     ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: CPUD2 PLL
                                                                                              * Others: 1'b0
                                                                                              */
#define CORE_S_NN_ACLK_SEL(x)                       ((u32)(((x) & 0x00000007) << 16))
#define CORE_S_GET_NN_ACLK_SEL(x)                   ((u32)(((x >> 16) & 0x00000007)))
#define CORE_S_BIT_NN_ACLK_DIVTRG                   ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define CORE_S_MASK_NN_ACLK_DIVS                    ((u32)0x0000003F << 8)                   /*!< R/W   0x1 Clock divider factor. */
#define CORE_S_NN_ACLK_DIVS(x)                      ((u32)(((x) & 0x0000003F) << 8))
#define CORE_S_GET_NN_ACLK_DIVS(x)                  ((u32)(((x >> 8) & 0x0000003F)))
#define CORE_S_MASK_NN_ACLK_DIVN                    ((u32)0x0000003F << 0)                   /*!< R/W   0x2
                                                                                              * Clock divider factor.
                                                                                              * Example:
                                                                                              * For clkdiv ratio 7/2, below is the setting:
                                                                                              * *_DIVN = 7
                                                                                              * *_DIVS = 2
                                                                                              * 
                                                                                              * Note that DIVN can't be less than DIVS
                                                                                              */
#define CORE_S_NN_ACLK_DIVN(x)                      ((u32)(((x) & 0x0000003F) << 0))
#define CORE_S_GET_NN_ACLK_DIVN(x)                  ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_SYS_NN_SCLK_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_NN_SCLK_VLD                      ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define CORE_S_BIT_NN_SCLK_DIV_DONE                 ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define CORE_S_BIT_NN_SCLK_EN                       ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock disable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define CORE_S_MASK_NN_SCLK_SEL                     ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: CPUD2 PLL
                                                                                              * Others: 1'b0
                                                                                              */
#define CORE_S_NN_SCLK_SEL(x)                       ((u32)(((x) & 0x00000007) << 16))
#define CORE_S_GET_NN_SCLK_SEL(x)                   ((u32)(((x >> 16) & 0x00000007)))
#define CORE_S_BIT_NN_SCLK_DIVTRG                   ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define CORE_S_MASK_NN_SCLK_DIVS                    ((u32)0x0000003F << 8)                   /*!< R/W   0x1 Clock divider factor. */
#define CORE_S_NN_SCLK_DIVS(x)                      ((u32)(((x) & 0x0000003F) << 8))
#define CORE_S_GET_NN_SCLK_DIVS(x)                  ((u32)(((x >> 8) & 0x0000003F)))
#define CORE_S_MASK_NN_SCLK_DIVN                    ((u32)0x0000003F << 0)                   /*!< R/W   0x1
                                                                                              * Clock divider factor.
                                                                                              * Example:
                                                                                              * For clkdiv ratio 7/2, below is the setting:
                                                                                              * *_DIVN = 7
                                                                                              * *_DIVS = 2
                                                                                              * 
                                                                                              * Note that DIVN can't be less than DIVS
                                                                                              */
#define CORE_S_NN_SCLK_DIVN(x)                      ((u32)(((x) & 0x0000003F) << 0))
#define CORE_S_GET_NN_SCLK_DIVN(x)                  ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_SYS_PLF_CLK_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_PLF_CLK_XTAL40M_SEL              ((u32)0x00000001 << 25)                  /*!< R/W   0x1 1: Select XTAL 40Mhz; 0: based on [18:16] */
#define CORE_S_BIT_PLF_CLK_VLD                      ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define CORE_S_BIT_PLF_CLK_DIV_DONE                 ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define CORE_S_BIT_PLF_CLK_EN                       ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock enable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) set PLF_CLK_XTAL_40M_SEL = 1
                                                                                              * (2) Disable clock.
                                                                                              * (3) Poll clock valid = 0.
                                                                                              * (4) Program clock select bits.
                                                                                              * (5) Enable clock.
                                                                                              * (6) set PLF_CLK_XTAL_40M_SEL = 0
                                                                                              */
#define CORE_S_MASK_PLF_CLK_SEL                     ((u32)0x00000007 << 16)                  /*!< R/W   0x5
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: XTAL 40M
                                                                                              * 0x6: OSC 4M
                                                                                              * Others: 1'b0
                                                                                              */
#define CORE_S_PLF_CLK_SEL(x)                       ((u32)(((x) & 0x00000007) << 16))
#define CORE_S_GET_PLF_CLK_SEL(x)                   ((u32)(((x >> 16) & 0x00000007)))
#define CORE_S_BIT_PLF_CLK_DIVTRG                   ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define CORE_S_MASK_PLF_CLK_DIVS                    ((u32)0x0000003F << 8)                   /*!< R/W   0x1 Clock divider factor. */
#define CORE_S_PLF_CLK_DIVS(x)                      ((u32)(((x) & 0x0000003F) << 8))
#define CORE_S_GET_PLF_CLK_DIVS(x)                  ((u32)(((x >> 8) & 0x0000003F)))
#define CORE_S_MASK_PLF_CLK_DIVN                    ((u32)0x0000003F << 0)                   /*!< R/W   0x1
                                                                                              * Clock divider factor.
                                                                                              * Example:
                                                                                              * For clkdiv ratio 7/2, below is the setting:
                                                                                              * *_DIVN = 7
                                                                                              * *_DIVS = 2
                                                                                              * 
                                                                                              * Note that DIVN can't be less than DIVS
                                                                                              */
#define CORE_S_PLF_CLK_DIVN(x)                      ((u32)(((x) & 0x0000003F) << 0))
#define CORE_S_GET_PLF_CLK_DIVN(x)                  ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_SYS_SD_CLK_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_SD_CLK_VLD                       ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define CORE_S_BIT_SD_CLK_DIV_DONE                  ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define CORE_S_BIT_SD_CLK_EN                        ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock disable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define CORE_S_MASK_SD_CLK_SEL                      ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: CPUD2 PLL
                                                                                              * Others: 1'b0
                                                                                              */
#define CORE_S_SD_CLK_SEL(x)                        ((u32)(((x) & 0x00000007) << 16))
#define CORE_S_GET_SD_CLK_SEL(x)                    ((u32)(((x >> 16) & 0x00000007)))
#define CORE_S_BIT_SD_CLK_DIVTRG                    ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define CORE_S_MASK_SD_CLK_DIVN                     ((u32)0x0000003F << 0)                   /*!< R/W   0x0 Clock divider factor. */
#define CORE_S_SD_CLK_DIVN(x)                       ((u32)(((x) & 0x0000003F) << 0))
#define CORE_S_GET_SD_CLK_DIVN(x)                   ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_SYS_SPORT_CLK_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_SPORT_CLK_VLD                    ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define CORE_S_BIT_SPORT_CLK_DIV_DONE               ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define CORE_S_BIT_SPORT_CLK_EN                     ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock disable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define CORE_S_MASK_SPORT_CLK_SEL                   ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: XTAL 40M
                                                                                              * Others: 1'b0
                                                                                              */
#define CORE_S_SPORT_CLK_SEL(x)                     ((u32)(((x) & 0x00000007) << 16))
#define CORE_S_GET_SPORT_CLK_SEL(x)                 ((u32)(((x >> 16) & 0x00000007)))
#define CORE_S_BIT_SPORT_CLK_DIVTRG                 ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define CORE_S_MASK_SPORT_CLK_DIVS                  ((u32)0x0000003F << 8)                   /*!< R/W   0x1 Clock divider factor. */
#define CORE_S_SPORT_CLK_DIVS(x)                    ((u32)(((x) & 0x0000003F) << 8))
#define CORE_S_GET_SPORT_CLK_DIVS(x)                ((u32)(((x >> 8) & 0x0000003F)))
#define CORE_S_MASK_SPORT_CLK_DIVN                  ((u32)0x0000003F << 0)                   /*!< R/W   0x5
                                                                                              * Clock divider factor.
                                                                                              * Example:
                                                                                              * For clkdiv ratio 7/2, below is the setting:
                                                                                              * *_DIVN = 7
                                                                                              * *_DIVS = 2
                                                                                              * 
                                                                                              * Note that DIVN can't be less than DIVS
                                                                                              */
#define CORE_S_SPORT_CLK_DIVN(x)                    ((u32)(((x) & 0x0000003F) << 0))
#define CORE_S_GET_SPORT_CLK_DIVN(x)                ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_SYS_SENSOR_CLK_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_SENSOR_CLK_VLD                   ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define CORE_S_BIT_SENSOR_CLK_DIV_DONE              ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define CORE_S_BIT_SENSOR_CLK_EN                    ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock disable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define CORE_S_MASK_SENSOR_CLK_SEL                  ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: CPU_PLL
                                                                                              * 0x6: XTAL 40M
                                                                                              * 0x7: OSC_4M
                                                                                              * Others: 1'b0
                                                                                              */
#define CORE_S_SENSOR_CLK_SEL(x)                    ((u32)(((x) & 0x00000007) << 16))
#define CORE_S_GET_SENSOR_CLK_SEL(x)                ((u32)(((x >> 16) & 0x00000007)))
#define CORE_S_BIT_SENSOR_CLK_DIVTRG                ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define CORE_S_MASK_SENSOR_CLK_DIVN                 ((u32)0x0000003F << 0)                   /*!< R/W   0x4 Clock divider factor. */
#define CORE_S_SENSOR_CLK_DIVN(x)                   ((u32)(((x) & 0x0000003F) << 0))
#define CORE_S_GET_SENSOR_CLK_DIVN(x)               ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_SYS_USBOTG_CLK_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_USBOTG_CLK_VLD                   ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define CORE_S_BIT_USBOTG_CLK_DIV_DONE              ((u32)0x00000001 << 23)                  /*!< R     0x0 None */
#define CORE_S_BIT_USBOTG_CLK_DIVTRG                ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define CORE_S_MASK_USBOTG_CLK_DIVN                 ((u32)0x0000003F << 0)                   /*!< R/W   0xF Clock divider factor. */
#define CORE_S_USBOTG_CLK_DIVN(x)                   ((u32)(((x) & 0x0000003F) << 0))
#define CORE_S_GET_USBOTG_CLK_DIVN(x)               ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_SYS_0_CLK_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_SYS_ADRM_ENC_SCLKEN              ((u32)0x00000001 << 31)                  /*!< R/W   0x0 Clock enable SYS_ADRM_ENC_SCLKEN */
#define CORE_S_BIT_SYS_ADRM_ISP_SCLKEN              ((u32)0x00000001 << 30)                  /*!< R/W   0x0 Clock enable SYS_ADRM_ISP_SCLKEN */
#define CORE_S_BIT_SYS_AI_PCLKEN                    ((u32)0x00000001 << 29)                  /*!< R/W   0x0 Clock enable SYS_AI_PCLKEN */
#define CORE_S_BIT_SYS_AI_PSI_CLKEN                 ((u32)0x00000001 << 28)                  /*!< R/W   0x0 Clock enable SYS_AI_PSI_CLKEN */
#define CORE_S_BIT_SYS_AI_SCLKEN                    ((u32)0x00000001 << 27)                  /*!< R/W   0x0 Clock enable SYS_AI_SCLKEN */
#define CORE_S_BIT_SYS_AUDIO_CODEC_CLKEN            ((u32)0x00000001 << 26)                  /*!< R/W   0x0 Clock enable SYS_AUDIO_CODEC_CLKEN */
#define CORE_S_BIT_SYS_CBUS_DDRC_CLKEN              ((u32)0x00000001 << 25)                  /*!< R/W   0x0 Clock enable SYS_CBUS_DDRC_CLKEN */
#define CORE_S_BIT_SYS_CBUS_ENC_ACLKEN              ((u32)0x00000001 << 24)                  /*!< R/W   0x0 Clock enable SYS_CBUS_ENC_ACLKEN */
#define CORE_S_BIT_SYS_CBUS_FAST_PCLKEN             ((u32)0x00000001 << 23)                  /*!< R/W   0x0 Clock enable SYS_CBUS_FAST_PCLKEN */
#define CORE_S_BIT_SYS_CBUS_GE_CLKEN                ((u32)0x00000001 << 22)                  /*!< R/W   0x0 Clock enable SYS_CBUS_GE_CLKEN */
#define CORE_S_BIT_SYS_CBUS_ISP_BUS_CLKEN           ((u32)0x00000001 << 21)                  /*!< R/W   0x0 Clock enable SYS_CBUS_ISP_BUS_CLKEN */
#define CORE_S_BIT_SYS_CBUS_KM4_MP_CLKEN            ((u32)0x00000001 << 20)                  /*!< R/W   0x0 Clock enable SYS_CBUS_KM4_MP_CLKEN */
#define CORE_S_BIT_SYS_CBUS_NN_ACLKEN               ((u32)0x00000001 << 19)                  /*!< R/W   0x0 Clock enable SYS_CBUS_NN_ACLKEN */
#define CORE_S_BIT_SYS_CBUS_SEC_CLKEN               ((u32)0x00000001 << 17)                  /*!< R/W   0x0 Clock enable SYS_CBUS_SEC_CLKEN */
#define CORE_S_BIT_SYS_CBUS_SLOW_PCLKEN             ((u32)0x00000001 << 16)                  /*!< R/W   0x0 Clock enable SYS_CBUS_SLOW_PCLKEN */
#define CORE_S_BIT_SYS_CBUS_SYS_MEM_CLKEN           ((u32)0x00000001 << 15)                  /*!< R/W   0x0 Clock enable SYS_CBUS_SYS_MEM_CLKEN */
#define CORE_S_BIT_SYS_CORE_DSS0_CLKEN              ((u32)0x00000001 << 14)                  /*!< R/W   0x0 Clock enable SYS_CORE_DSS0_CLKEN */
#define CORE_S_BIT_SYS_CORE_DSS1_CLKEN              ((u32)0x00000001 << 13)                  /*!< R/W   0x0 Clock enable SYS_CORE_DSS1_CLKEN */
#define CORE_S_BIT_SYS_CORE_DSS2_CLKEN              ((u32)0x00000001 << 12)                  /*!< R/W   0x0 Clock enable SYS_CORE_DSS2_CLKEN */
#define CORE_S_BIT_SYS_CORE_DSS3_CLKEN              ((u32)0x00000001 << 11)                  /*!< R/W   0x0 Clock enable SYS_CORE_DSS3_CLKEN */
#define CORE_S_BIT_SYS_CORE_DSS4_CLKEN              ((u32)0x00000001 << 10)                  /*!< R/W   0x0 Clock enable SYS_CORE_DSS4_CLKEN */
#define CORE_S_BIT_SYS_CORE_GPIO_PCLKEN             ((u32)0x00000001 << 9)                   /*!< R/W   0x0 Clock enable SYS_CORE_GPIO_PCLKEN */
#define CORE_S_BIT_SYS_CORE_GPIO_SCLKEN             ((u32)0x00000001 << 8)                   /*!< R/W   0x0 Clock enable SYS_CORE_GPIO_SCLKEN */
#define CORE_S_BIT_SYS_DDRC_CLKEN                   ((u32)0x00000001 << 7)                   /*!< R/W   0x0 Clock enable SYS_DDRC_CLKEN */
#define CORE_S_BIT_SYS_DDRPHY_CLKEN                 ((u32)0x00000001 << 6)                   /*!< R/W   0x0 Clock enable SYS_DDRPHY_CLKEN */
#define CORE_S_BIT_SYS_ENC_ACLKEN                   ((u32)0x00000001 << 5)                   /*!< R/W   0x0 Clock enable SYS_ENC_ACLKEN */
#define CORE_S_BIT_SYS_ENC_PCLKEN                   ((u32)0x00000001 << 4)                   /*!< R/W   0x0 Clock enable SYS_ENC_PCLKEN */
#define CORE_S_BIT_SYS_ENC_SCLKEN                   ((u32)0x00000001 << 3)                   /*!< R/W   0x0 Clock enable SYS_ENC_SCLKEN */
#define CORE_S_BIT_SYS_GMAC_LCLKEN                  ((u32)0x00000001 << 2)                   /*!< R/W   0x0 Clock enable SYS_GMAC_LCLKEN */
#define CORE_S_BIT_SYS_GMAC_PCLKEN                  ((u32)0x00000001 << 1)                   /*!< R/W   0x0 Clock enable SYS_GMAC_PCLKEN */
#define CORE_S_BIT_SYS_GMAC_SCLKEN                  ((u32)0x00000001 << 0)                   /*!< R/W   0x0 Clock enable SYS_GMAC_SCLKEN */
/** @} */


/** @defgroup REG_SYS_1_CLK_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_SYS_ISP_BUS_CLKEN                ((u32)0x00000001 << 31)                  /*!< R/W   0x0 Clock enable SYS_ISP_BUS_CLKEN */
#define CORE_S_BIT_SYS_ISP_MIPI_CLKEN               ((u32)0x00000001 << 30)                  /*!< R/W   0x0 Clock enable SYS_ISP_MIPI_CLKEN */
#define CORE_S_BIT_SYS_ISP_PCLKEN                   ((u32)0x00000001 << 29)                  /*!< R/W   0x0 NO used */
#define CORE_S_BIT_SYS_ISP_SCLKEN                   ((u32)0x00000001 << 28)                  /*!< R/W   0x0 Clock enable SYS_ISP_SCLKEN */
#define CORE_S_BIT_SYS_KM4_MP_CLKEN                 ((u32)0x00000001 << 27)                  /*!< R/W   0x0 Clock enable SYS_KM4_MP_CLKEN */
#define CORE_S_BIT_SYS_KM4_MP_PCLKEN                ((u32)0x00000001 << 26)                  /*!< R/W   0x0 Clock enable SYS_KM4_MP_PCLKEN */
#define CORE_S_BIT_SYS_LX_CLKEN                     ((u32)0x00000001 << 25)                  /*!< R/W   0x0 Clock enable SYS_LX_CLKEN */
#define CORE_S_BIT_SYS_LXBUS_CLKEN                  ((u32)0x00000001 << 24)                  /*!< R/W   0x0 Clock enable SYS_LXBUS_CLKEN */
#define CORE_S_BIT_SYS_NN_ACLKEN                    ((u32)0x00000001 << 23)                  /*!< R/W   0x0 Clock enable SYS_NN_ACLKEN */
#define CORE_S_BIT_SYS_NN_PCLKEN                    ((u32)0x00000001 << 22)                  /*!< R/W   0x0 Clock enable SYS_NN_PCLKEN */
#define CORE_S_BIT_SYS_NN_SCLKEN                    ((u32)0x00000001 << 21)                  /*!< R/W   0x0 Clock enable SYS_NN_SCLKEN */
#define CORE_S_BIT_SYS_PON_SYSONREG_CLKEN           ((u32)0x00000001 << 20)                  /*!< R/W   0x0 NO used */
#define CORE_S_BIT_SYS_PON_WLOFF_DSS_CLKEN          ((u32)0x00000001 << 19)                  /*!< R/W   0x0 Clock enable SYS_PON_WLOFF_DSS_CLKEN */
#define CORE_S_BIT_SYS_SD_LCLKEN                    ((u32)0x00000001 << 18)                  /*!< R/W   0x0 Clock enable SYS_SD_LCLKEN */
#define CORE_S_BIT_SYS_SD_PCLKEN                    ((u32)0x00000001 << 17)                  /*!< R/W   0x0 Clock enable SYS_SD_PCLKEN */
#define CORE_S_BIT_SYS_SD_SCLKEN                    ((u32)0x00000001 << 16)                  /*!< R/W   0x0 Clock enable SYS_SD_SCLKEN */
#define CORE_S_BIT_SYS_SDIO_PCLKEN                  ((u32)0x00000001 << 15)                  /*!< R/W   0x0 Clock enable SYS_SDIO_PCLKEN */
#define CORE_S_BIT_SYS_SDIO_SCLKEN                  ((u32)0x00000001 << 14)                  /*!< R/W   0x0 Clock enable SYS_SDIO_SCLKEN */
#define CORE_S_BIT_SYS_SGPIO_PCLKEN                 ((u32)0x00000001 << 13)                  /*!< R/W   0x0 Clock enable SYS_SGPIO_PCLKEN */
#define CORE_S_BIT_SYS_SGPIO_SCLKEN                 ((u32)0x00000001 << 12)                  /*!< R/W   0x0 Clock enable SYS_SGPIO_SCLKEN */
#define CORE_S_BIT_SYS_SPORT0_PCLKEN                ((u32)0x00000001 << 11)                  /*!< R/W   0x0 Clock enable SYS_SPORT0_PCLKEN */
#define CORE_S_BIT_SYS_SPORT0_SCLKEN                ((u32)0x00000001 << 10)                  /*!< R/W   0x0 Clock enable SYS_SPORT0_SCLKEN */
#define CORE_S_BIT_SYS_SPORT1_PCLKEN                ((u32)0x00000001 << 9)                   /*!< R/W   0x0 Clock enable SYS_SPORT1_PCLKEN */
#define CORE_S_BIT_SYS_SPORT1_SCLKEN                ((u32)0x00000001 << 8)                   /*!< R/W   0x0 Clock enable SYS_SPORT1_SCLKEN */
#define CORE_S_BIT_SYS_TC_SCLKEN                    ((u32)0x00000001 << 7)                   /*!< R/W   0x0 Clock enable SYS_TC_SCLKEN */
#define CORE_S_BIT_SYS_USBOTG_HCLKEN                ((u32)0x00000001 << 4)                   /*!< R/W   0x0 Clock enable SYS_USBOTG_HCLKEN */
#define CORE_S_BIT_SYS_USBOTG_SCLKEN                ((u32)0x00000001 << 3)                   /*!< R/W   0x0 Clock enable SYS_USBOTG_SCLKEN */
#define CORE_S_BIT_SYS_VID_SRAM_CLKEN               ((u32)0x00000001 << 2)                   /*!< R/W   0x0 Clock enable SYS_VID_SRAM_CLKEN */
#define CORE_S_BIT_SYS_WDT_AP_PCLKEN                ((u32)0x00000001 << 1)                   /*!< R/W   0x0 Clock enable SYS_WDT_AP_PCLKEN */
#define CORE_S_BIT_SYS_WDT_AP_SCLKEN                ((u32)0x00000001 << 0)                   /*!< R/W   0x0 Clock enable SYS_WDT_AP_SCLKEN */
/** @} */


/** @defgroup REG_SYS_2_CLK_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_SYS_WDT_MP_PCLKEN                ((u32)0x00000001 << 31)                  /*!< R/W   0x0 Clock enable SYS_WDT_MP_PCLKEN */
#define CORE_S_BIT_SYS_WDT_MP_SCLKEN                ((u32)0x00000001 << 30)                  /*!< R/W   0x0 Clock enable SYS_WDT_MP_SCLKEN */
#define CORE_S_BIT_SYS_CORE_GPIO_INTRCLK_SEL        ((u32)0x00000001 << 29)                  /*!< R/W   0x0 0 pclk 1 Sclk SYS_CORE_GPIO_INTRCLK_SEL */
#define CORE_S_BIT_SYS_GDMA1_CLKEN                  ((u32)0x00000001 << 28)                  /*!< R/W   0x0 Clock enable SYS_GDMA1_CLKEN */
#define CORE_S_BIT_SYS_GE_SCLKEN                    ((u32)0x00000001 << 27)                  /*!< R/W   0x0 Clock enable SYS_GE_SCLKEN */
#define CORE_S_BIT_SYS_IPSEC_PCLKEN                 ((u32)0x00000001 << 26)                  /*!< R/W   0x0 Clock enable SYS_IPSEC_PCLKEN */
#define CORE_S_BIT_SYS_AI_PSI_CLK_SEL               ((u32)0x00000001 << 25)                  /*!< R/W   0x0
                                                                                              * 0: AID clock clock is isp_core_clk
                                                                                              * 1: AID clock clock is ai_core_clk
                                                                                              */
#define CORE_S_BIT_WDT_AP_SCLKEN                    ((u32)0x00000001 << 23)                  /*!< R/W   0x0 Clock enable WDT_AP_SCLKEN */
#define CORE_S_BIT_AUDIO_CODEC_PCLKEN               ((u32)0x00000001 << 22)                  /*!< R/W   0x0 Clock enable AUDIO_CODEC_PCLKEN */
#define CORE_S_BIT_DDRC_CLK_SEL                     ((u32)0x00000001 << 21)                  /*!< R/W   0x0 0: select XTAL 40M; 1: select DDR PLL */
#define CORE_S_BIT_SYS_UART2_SCLKEN                 ((u32)0x00000001 << 20)                  /*!< R/W   0x0 Clock enable SYS_UART2_SCLKEN */
#define CORE_S_BIT_SYS_UART2_PCLKEN                 ((u32)0x00000001 << 19)                  /*!< R/W   0x0 Clock enable SYS_UART2_PCLKEN */
#define CORE_S_BIT_SYS_UART0_SCLKEN                 ((u32)0x00000001 << 18)                  /*!< R/W   0x0 Clock enable SYS_UART0_SCLKEN */
#define CORE_S_BIT_SYS_UART0_PCLKEN                 ((u32)0x00000001 << 17)                  /*!< R/W   0x0 Clock enable SYS_UART0_PCLKEN */
/** @} */


/** @defgroup REG_SYS_0_FUNC_CTRL
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_SYS_1_FUNC_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_SYS_FEN_S0                       ((u32)0x00000001 << 31)                  /*!< R/W   0   Not used */
#define CORE_S_BIT_SYS_FEN_S1                       ((u32)0x00000001 << 30)                  /*!< R/W     0 Not used */
#define CORE_S_BIT_SYS_FEN_S2                       ((u32)0x00000001 << 29)                  /*!< R/W     0
                                                                                              * Media CPU  function enable (cold reset)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S3                       ((u32)0x00000001 << 28)                  /*!< R/W   0  
                                                                                              * Media CPU  function enable (warm reset)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S6                       ((u32)0x00000001 << 27)                  /*!< R/W     0 Not used */
#define CORE_S_BIT_SYS_FEN_S6_0                     ((u32)0x00000001 << 26)                  /*!< R/W     0 Not used */
#define CORE_S_BIT_SYS_FEN_S6_10                    ((u32)0x00000001 << 24)                  /*!< R/W   0   Not used */
#define CORE_S_BIT_SYS_FEN_S6_11                    ((u32)0x00000001 << 23)                  /*!< R/W   0   Not used */
#define CORE_S_BIT_SYS_FEN_S6_12                    ((u32)0x00000001 << 22)                  /*!< R/W     0 Not used */
#define CORE_S_BIT_SYS_FEN_S6_2                     ((u32)0x00000001 << 21)                  /*!< R/W   0  
                                                                                              * Core Platform function enable (nn_aclk domian reset)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S6_3                     ((u32)0x00000001 << 20)                  /*!< R/W   0  
                                                                                              * Core Platform function enable (enc_aclk domain reset)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S6_4                     ((u32)0x00000001 << 19)                  /*!< R/W   0  
                                                                                              * Core Platform function enable (isp_mbus_clk domain reset)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S6_5                     ((u32)0x00000001 << 18)                  /*!< R/W   0  
                                                                                              * Core Platform function enable (km4_mp_clk domain reset)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S6_6                     ((u32)0x00000001 << 17)                  /*!< R/W     0
                                                                                              * Core Platform function enable (ge_clk domain reset)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S6_7                     ((u32)0x00000001 << 16)                  /*!< R/W   0  
                                                                                              * Core Platform function enable (ddrc_clk domain reset)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S6_8                     ((u32)0x00000001 << 15)                  /*!< R/W     0
                                                                                              * Core Platform function enable (fast_pclk domian reset)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S6_9                     ((u32)0x00000001 << 14)                  /*!< R/W     0
                                                                                              * Core Platform function enable (sec_clk domain reset)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7                       ((u32)0x00000001 << 13)                  /*!< R/W   0  
                                                                                              * CA32 Processor WDT function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_1                     ((u32)0x00000001 << 12)                  /*!< R/W     0
                                                                                              * NN function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_14                    ((u32)0x00000001 << 11)                  /*!< R/W     0
                                                                                              * DDRPHY function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_16                    ((u32)0x00000001 << 10)                  /*!< R/W   0  
                                                                                              * VIDEO SRAM function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_17                    ((u32)0x00000001 << 9)                   /*!< R/W   0  
                                                                                              * GDMA1 function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_2                     ((u32)0x00000001 << 8)                   /*!< R/W   0  
                                                                                              * Encoder function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_29                    ((u32)0x00000001 << 6)                   /*!< R/W   0  
                                                                                              * CORE GPIO function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_3                     ((u32)0x00000001 << 5)                   /*!< R/W   0  
                                                                                              * ISP function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_32                    ((u32)0x00000001 << 4)                   /*!< R/W   0  
                                                                                              * SGPIO function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_33                    ((u32)0x00000001 << 3)                   /*!< R/W   0  
                                                                                              * SGPIO register enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_34                    ((u32)0x00000001 << 2)                   /*!< R/W     0
                                                                                              * GMAC function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_35                    ((u32)0x00000001 << 1)                   /*!< R/W   0  
                                                                                              * SDMMC function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_36                    ((u32)0x00000001 << 0)                   /*!< R/W     0
                                                                                              * SDIO function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
/** @} */


/** @defgroup REG_SYS_2_FUNC_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_SYS_FEN_S7_4                     ((u32)0x00000001 << 31)                  /*!< R/W   0  
                                                                                              * Graphic Engine function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_41                    ((u32)0x00000001 << 30)                  /*!< R/W     0
                                                                                              * SPORT0 function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_42                    ((u32)0x00000001 << 29)                  /*!< R/W     0
                                                                                              * SPORT1 function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_43                    ((u32)0x00000001 << 28)                  /*!< R/W   0  
                                                                                              * USBOTG function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_45                    ((u32)0x00000001 << 27)                  /*!< R/W     0
                                                                                              * AI-Denoise function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_46                    ((u32)0x00000001 << 26)                  /*!< R/W     0
                                                                                              * Audio Codec (mclk) function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_47                    ((u32)0x00000001 << 25)                  /*!< R/W   0  
                                                                                              * LX BUS function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_48                    ((u32)0x00000001 << 24)                  /*!< R/W   0  
                                                                                              * WLOFF_DSS function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_49                    ((u32)0x00000001 << 23)                  /*!< R/W   0  
                                                                                              * core dss0 function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_5                     ((u32)0x00000001 << 22)                  /*!< R/W     0
                                                                                              * ISP Address Map function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_50                    ((u32)0x00000001 << 21)                  /*!< R/W   0  
                                                                                              * Core dss1 function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_51                    ((u32)0x00000001 << 20)                  /*!< R/W   0  
                                                                                              * Core dss2 function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_52                    ((u32)0x00000001 << 19)                  /*!< R/W   0  
                                                                                              * Core dss3 function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_53                    ((u32)0x00000001 << 18)                  /*!< R/W   0  
                                                                                              * Core dss4 function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_6                     ((u32)0x00000001 << 17)                  /*!< R/W     0
                                                                                              * Encoder Address Map function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_7                     ((u32)0x00000001 << 16)                  /*!< R/W   0  
                                                                                              * Tile Scaler function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_8                     ((u32)0x00000001 << 15)                  /*!< R/W   0   Not used */
#define CORE_S_BIT_SYS_FEN_S7_9                     ((u32)0x00000001 << 14)                  /*!< R/W     0
                                                                                              * Jtag Key control function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S8                       ((u32)0x00000001 << 13)                  /*!< R/W   0   Not used */
#define CORE_S_BIT_SYS_FEN_UART2                    ((u32)0x00000001 << 12)                  /*!< R/W   0  
                                                                                              * UART2 function enable
                                                                                              * 0:  Keep IP design in reset
                                                                                              * 1: Release IP design reset
                                                                                              */
#define CORE_S_BIT_SYS_FEN_UART0                    ((u32)0x00000001 << 11)                  /*!< R/W   0  
                                                                                              * UART0 function enable
                                                                                              * 0:  Keep IP design in reset
                                                                                              * 1: Release IP design reset
                                                                                              */
#define CORE_S_BIT_SYS_FEN_S7_60                    ((u32)0x00000001 << 2)                   /*!< R/W   0  
                                                                                              * Wifi Platform function enable (SPIC clock domain reset)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_AUDIO_CODEC_EN_S7_44             ((u32)0x00000001 << 1)                   /*!< R/W     0
                                                                                              * Audio Codec (pclk) function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define CORE_S_BIT_MPWDT_FEN_S7_                    ((u32)0x00000001 << 0)                   /*!< R/W   0  
                                                                                              * Media Processor WDT function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
/** @} */


/** @defgroup REG_APWDT_RST_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_APWDT_SW_RST_TRG                 ((u32)0x00000001 << 16)                  /*!< R/W   0x0 1: Trigger the APWDT reset by SW. Auto clear by HW */
#define CORE_S_BIT_AON_RST_EN                       ((u32)0x00000001 << 14)                  /*!< R/W   0x0 None */
#define CORE_S_BIT_PON_RST_EN                       ((u32)0x00000001 << 13)                  /*!< R/W   0x0 None */
#define CORE_S_BIT_SYSON_RST_EN                     ((u32)0x00000001 << 12)                  /*!< R/W   0x0 None */
#define CORE_S_BIT_NP_COLD_RST_EN                   ((u32)0x00000001 << 11)                  /*!< R/W   0x0 None */
#define CORE_S_BIT_NP_WARM_RST_EN                   ((u32)0x00000001 << 10)                  /*!< R/W   0x0 None */
#define CORE_S_BIT_MP_COLD_RST_EN                   ((u32)0x00000001 << 9)                   /*!< R/W   0x0 None */
#define CORE_S_BIT_MP_WARM_RST_EN                   ((u32)0x00000001 << 8)                   /*!< R/W   0x0 None */
#define CORE_S_BIT_FW_COLD_RST_EN                   ((u32)0x00000001 << 7)                   /*!< R/W   0x0 None */
#define CORE_S_BIT_FW_WARM_RST_EN                   ((u32)0x00000001 << 6)                   /*!< R/W   0x0 None */
#define CORE_S_BIT_PBUS_RST_EN                      ((u32)0x00000001 << 5)                   /*!< R/W   0x0 None */
#define CORE_S_BIT_CBUS_RST_EN                      ((u32)0x00000001 << 4)                   /*!< R/W   0x0 None */
#define CORE_S_BIT_WL_RST_EN                        ((u32)0x00000001 << 3)                   /*!< R/W   0x0 None */
#define CORE_S_BIT_BT_RST_EN                        ((u32)0x00000001 << 2)                   /*!< R/W   0x0 None */
#define CORE_S_BIT_AP_COLD_RST_EN                   ((u32)0x00000001 << 1)                   /*!< R/W   0x0 None */
#define CORE_S_BIT_MCUOFF_RST_EN                    ((u32)0x00000001 << 0)                   /*!< R/W   0x0 None */
/** @} */


/** @defgroup REG_MPWDT_RST_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_MPWDT_SW_RST_TRG                 ((u32)0x00000001 << 16)                  /*!< R/W   0x0 1: Trigger the MPWDT reset by SW. Auto clear by HW */
#define CORE_S_BIT_AON_RST_EN_1C4                   ((u32)0x00000001 << 14)                  /*!< R/W   0x0 None */
#define CORE_S_BIT_PON_RST_EN_1C4                   ((u32)0x00000001 << 13)                  /*!< R/W   0x0 None */
#define CORE_S_BIT_SYSON_RST_EN_1C4                 ((u32)0x00000001 << 12)                  /*!< R/W   0x0 None */
#define CORE_S_BIT_NP_COLD_RST_EN_1C4               ((u32)0x00000001 << 11)                  /*!< R/W   0x0 None */
#define CORE_S_BIT_NP_WARM_RST_EN_1C4               ((u32)0x00000001 << 10)                  /*!< R/W   0x0 None */
#define CORE_S_BIT_AP_WARM_RST_EN                   ((u32)0x00000001 << 9)                   /*!< R/W   0x0 None */
#define CORE_S_BIT_MP_COLD_RST_EN_1C4               ((u32)0x00000001 << 8)                   /*!< R/W   0x0 None */
#define CORE_S_BIT_FW_COLD_RST_EN_1C4               ((u32)0x00000001 << 7)                   /*!< R/W   0x0 None */
#define CORE_S_BIT_FW_WARM_RST_EN_1C4               ((u32)0x00000001 << 6)                   /*!< R/W   0x0 None */
#define CORE_S_BIT_PBUS_RST_EN_1C4                  ((u32)0x00000001 << 5)                   /*!< R/W   0x0 None */
#define CORE_S_BIT_CBUS_RST_EN_1C4                  ((u32)0x00000001 << 4)                   /*!< R/W   0x0 None */
#define CORE_S_BIT_WL_RST_EN_1C4                    ((u32)0x00000001 << 3)                   /*!< R/W   0x0 None */
#define CORE_S_BIT_BT_RST_EN_1C4                    ((u32)0x00000001 << 2)                   /*!< R/W   0x0 None */
#define CORE_S_BIT_AP_COLD_RST_EN_1C4               ((u32)0x00000001 << 1)                   /*!< R/W   0x0 None */
#define CORE_S_BIT_MCUOFF_RST_EN_1C4                ((u32)0x00000001 << 0)                   /*!< R/W   0x0 None */
/** @} */


/** @defgroup REG_LX_CTRL0
 * @brief
 * @{
 */
#define CORE_S_MASK_MII_LX_TIMEOUT_THRESHOLD        ((u32)0x00000003 << 17)                  /*!< R/W   0  
                                                                                              * 2'b00: 2^15 lexra clock cycles
                                                                                              * 2'b01: 2^13 lexra clock cycles
                                                                                              * 2'b10: 2^11 lexra clock cycles
                                                                                              * 2'b11: 2^9 lexra clock cycles
                                                                                              */
#define CORE_S_MII_LX_TIMEOUT_THRESHOLD(x)          ((u32)(((x) & 0x00000003) << 17))
#define CORE_S_GET_MII_LX_TIMEOUT_THRESHOLD(x)      ((u32)(((x >> 17) & 0x00000003)))
#define CORE_S_BIT_MII_LX_TIMEOUT_EN                ((u32)0x00000001 << 16)                  /*!< R/W   1   1:Enable */
#define CORE_S_BIT_LX2_DMA_LOCK_DONE                ((u32)0x00000001 << 9)                   /*!< R     0   1'b1: lock is done. There is no dma transaction now. */
#define CORE_S_BIT_LX2_DMA_LOCK_EN                  ((u32)0x00000001 << 8)                   /*!< R/W   0   1'b1: set gnt=0 to lock lexra dma function. */
#define CORE_S_BIT_PCM1_LX_REQ_EN                   ((u32)0x00000001 << 7)                   /*!< R/W   1   1: Enable */
#define CORE_S_BIT_PCM0_LX_REQ_EN                   ((u32)0x00000001 << 6)                   /*!< R/W   1   1: Enable */
#define CORE_S_BIT_PCM_SPDUP_EN                     ((u32)0x00000001 << 5)                   /*!< R/W   0   PCM speedup enable, only for test mode. */
#define CORE_S_BIT_LX1_DMA_LOCK_DONE                ((u32)0x00000001 << 4)                   /*!< R     0   1'b1: lock is done. There is no dma transaction now. */
#define CORE_S_BIT_LX1_DMA_LOCK_EN                  ((u32)0x00000001 << 3)                   /*!< R/W   0   1'b1: set gnt=0 to lock lexra dma function. */
#define CORE_S_MASK_LX_BUS_TIMEOUT_THRESHOLD        ((u32)0x00000003 << 1)                   /*!< R/W   0  
                                                                                              * 2'b00: 2^15 lexra clock cycles
                                                                                              * 2'b01: 2^13 lexra clock cycles
                                                                                              * 2'b10: 2^11 lexra clock cycles
                                                                                              * 2'b11: 2^9 lexra clock cycles
                                                                                              */
#define CORE_S_LX_BUS_TIMEOUT_THRESHOLD(x)          ((u32)(((x) & 0x00000003) << 1))
#define CORE_S_GET_LX_BUS_TIMEOUT_THRESHOLD(x)      ((u32)(((x >> 1) & 0x00000003)))
#define CORE_S_BIT_LX_BUS_TIMEOUT_EN                ((u32)0x00000001 << 0)                   /*!< R/W   0   1:Enable */
/** @} */


/** @defgroup REG_LX_CTRL1
 * @brief
 * @{
 */
#define CORE_S_BIT_LX_MASTER_RST_CLR_ACK            ((u32)0x00000001 << 8)                   /*!< R     0x1
                                                                                              * 1: Platform is in processing, can't reset LX Master bus
                                                                                              * 0: Platform processing is done, can reset LX Master bus
                                                                                              */
#define CORE_S_BIT_LX_MASTER_RST_CLR_REQ            ((u32)0x00000001 << 0)                   /*!< R/W   0x1
                                                                                              * 1: No request
                                                                                              * 0: Request Platform to do reset clean on LX slave port.
                                                                                              */
/** @} */


/** @defgroup REG_CORE_S_SPARE1
 * @brief
 * @{
 */
#define CORE_S_MASK_DUMMY                           ((u32)0xFFFFFFFF << 0)                   /*!< RW    0x0 None */
#define CORE_S_DUMMY(x)                             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DUMMY(x)                         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CORE_S_SPARE2
 * @brief
 * @{
 */
#define CORE_S_MASK_DUMMY_1D4                       ((u32)0xFFFFFFFF << 0)                   /*!< RW    0x0 None */
#define CORE_S_DUMMY_1D4(x)                         ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DUMMY_1D4(x)                     ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CORE_S_SPARE3
 * @brief
 * @{
 */
#define CORE_S_MASK_DUMMY_1D8                       ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0x0 None */
#define CORE_S_DUMMY_1D8(x)                         ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DUMMY_1D8(x)                     ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CORE_S_SPARE4
 * @brief
 * @{
 */
#define CORE_S_MASK_DUMMY_1DC                       ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0x0 None */
#define CORE_S_DUMMY_1DC(x)                         ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DUMMY_1DC(x)                     ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AUDIO_ANA_CTRL0
 * @brief
 * @{
 */
#define CORE_S_MASK_AUDIO_CODEC_RESERVE             ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0x0
                                                                                              * CODEC_RESERVE<15:0>
                                                                                              * reserved
                                                                                              */
#define CORE_S_AUDIO_CODEC_RESERVE(x)               ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_AUDIO_CODEC_RESERVE(x)           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AUDIO_ANA_CTRL1
 * @brief
 * @{
 */
#define CORE_S_BIT_AUDIO_DAC_CKXEN                  ((u32)0x00000001 << 31)                  /*!< R/W   0x1 DAC chopper clock enable control (0: disable, 1: enable) */
#define CORE_S_BIT_AUDIO_DAC_CKXSEL                 ((u32)0x00000001 << 30)                  /*!< R/W   0x1 DAC chopper clock selection (0: ckx2/4, 1: ckx2/8) */
#define CORE_S_BIT_AUDIO_DAC_POW                    ((u32)0x00000001 << 29)                  /*!< R/W   0x0 DAC power down control (0: power down, 1: power on) */
#define CORE_S_MASK_AUDIO_DPRAMP_CSEL_1_0_          ((u32)0x00000003 << 27)                  /*!< R/W   0x3 Depop C size selection (00: 1x, 01: 2x, 10: 3x, 11: 4x) */
#define CORE_S_AUDIO_DPRAMP_CSEL_1_0_(x)            ((u32)(((x) & 0x00000003) << 27))
#define CORE_S_GET_AUDIO_DPRAMP_CSEL_1_0_(x)        ((u32)(((x >> 27) & 0x00000003)))
#define CORE_S_BIT_AUDIO_DPRAMP_ENRAMP              ((u32)0x00000001 << 26)                  /*!< R/W   0x0 DPRAMP enable ramp control (0: disable, 1: enable) */
#define CORE_S_BIT_AUDIO_DPRAMP_POW                 ((u32)0x00000001 << 25)                  /*!< R/W   0x0 DPRAMP power down control (0: power down, 1: power on) */
#define CORE_S_BIT_AUDIO_DTSDM_CKXEN                ((u32)0x00000001 << 24)                  /*!< R/W   0x1
                                                                                              * ADC integrater 1 OP chopper enable:
                                                                                              * 0 : disable
                                                                                              * 1 : enable (default)
                                                                                              */
#define CORE_S_BIT_AUDIO_DTSDM_POW_                 ((u32)0x00000001 << 23)                  /*!< R/W   0x0
                                                                                              * ADC power on control:
                                                                                              * 0: power down
                                                                                              * 1: power on
                                                                                              */
#define CORE_S_BIT_AUDIO_LDO_COMP_INT_              ((u32)0x00000001 << 22)                  /*!< R/W   0x0
                                                                                              * LDO Miller Compensation
                                                                                              * 0:close 
                                                                                              * 1:short
                                                                                              */
#define CORE_S_BIT_AUDIO_LDO_DISCHARGE_             ((u32)0x00000001 << 21)                  /*!< R/W   0x0
                                                                                              * selection for discharge  path of LDO_CODEC
                                                                                              * 0:discharge  path is turned off (default)
                                                                                              * 1:1.8kohm discharge  path is turned on
                                                                                              */
#define CORE_S_BIT_AUDIO_LDO_EM                     ((u32)0x00000001 << 20)                  /*!< R/W   0x1
                                                                                              * selection for driving  path of LDO_CODEC
                                                                                              * 0:select strong driving path, PMOS w=240um, L=0.44um
                                                                                              * 1:select 1.8kohm driving path(default)
                                                                                              */
#define CORE_S_BIT_AUDIO_LDO_POW                    ((u32)0x00000001 << 19)                  /*!< R/W   0x0
                                                                                              * LDO power control
                                                                                              * 0: disable LDO
                                                                                              * 1: enable LDO
                                                                                              */
#define CORE_S_BIT_AUDIO_LDO_POW_OP9V               ((u32)0x00000001 << 18)                  /*!< R/W   0x0
                                                                                              * LDO Precharge
                                                                                              * 0: LDO
                                                                                              * 1: Unity Gain buffer
                                                                                              */
#define CORE_S_BIT_AUDIO_LDO_PREC                   ((u32)0x00000001 << 17)                  /*!< R/W   0x0 REG_LOD_PREC */
#define CORE_S_MASK_AUDIO_LDO_TUNE_4_0_             ((u32)0x0000001F << 12)                  /*!< R/W   10000
                                                                                              * LDO voltage control
                                                                                              * 10000 Vref 0.9V LDO 1.8V
                                                                                              */
#define CORE_S_AUDIO_LDO_TUNE_4_0_(x)               ((u32)(((x) & 0x0000001F) << 12))
#define CORE_S_GET_AUDIO_LDO_TUNE_4_0_(x)           ((u32)(((x >> 12) & 0x0000001F)))
#define CORE_S_MASK_AUDIO_LO_BIAS_1_0_              ((u32)0x00000003 << 10)                  /*!< R/W   0x0 Lineout mode output MOS quiescent curremt tuning (00: 120uA) */
#define CORE_S_AUDIO_LO_BIAS_1_0_(x)                ((u32)(((x) & 0x00000003) << 10))
#define CORE_S_GET_AUDIO_LO_BIAS_1_0_(x)            ((u32)(((x >> 10) & 0x00000003)))
#define CORE_S_BIT_AUDIO_LO_CAL                     ((u32)0x00000001 << 9)                   /*!< R/W   0x0 Lineout output offset calibration mode selection (1: calibration mode, 0: normal mode) */
#define CORE_S_MASK_AUDIO_LO_CCSEL_1_0_             ((u32)0x00000003 << 7)                   /*!< R/W   0x3 Lineout op 1st stage miller cap size (00: 20pF, 01: 24pF, 10: 28pF, 11: 32pF) */
#define CORE_S_AUDIO_LO_CCSEL_1_0_(x)               ((u32)(((x) & 0x00000003) << 7))
#define CORE_S_GET_AUDIO_LO_CCSEL_1_0_(x)           ((u32)(((x >> 7) & 0x00000003)))
#define CORE_S_BIT_AUDIO_LO_ENDP                    ((u32)0x00000001 << 6)                   /*!< R/W   0x1 Lineout enable depop control (0: disable, 1: enable) */
#define CORE_S_MASK_AUDIO_LO_GSEL_1_0_              ((u32)0x00000003 << 4)                   /*!< R/W   0x0 Lineout  gain selection (00: 0dB, 01: -3dB, 10: -6dB, 11: -9dB) */
#define CORE_S_AUDIO_LO_GSEL_1_0_(x)                ((u32)(((x) & 0x00000003) << 4))
#define CORE_S_GET_AUDIO_LO_GSEL_1_0_(x)            ((u32)(((x >> 4) & 0x00000003)))
#define CORE_S_MASK_AUDIO_LO_M_1_0_                 ((u32)0x00000003 << 2)                   /*!< R/W   0x3 Lineout mute control (0: un-mute, 1: mute), <0>: DAC, <1>: Analog in */
#define CORE_S_AUDIO_LO_M_1_0_(x)                   ((u32)(((x) & 0x00000003) << 2))
#define CORE_S_GET_AUDIO_LO_M_1_0_(x)               ((u32)(((x >> 2) & 0x00000003)))
#define CORE_S_BIT_AUDIO_LO_MDP                     ((u32)0x00000001 << 1)                   /*!< R/W   0x0 Lineout mute depop mode control (0: no depop, 1: depop) */
#define CORE_S_BIT_AUDIO_LO_OPNDP                   ((u32)0x00000001 << 0)                   /*!< R/W   0x0 Lineout op negative depop mode control (0: no depop, 1: depop) */
/** @} */


/** @defgroup REG_AUDIO_ANA_CTRL2
 * @brief
 * @{
 */
#define CORE_S_BIT_AUDIO_LO_OPPDP                   ((u32)0x00000001 << 31)                  /*!< R/W   0x0 Lineout op positive depop mode control (0: no depop, 1: depop) */
#define CORE_S_BIT_AUDIO_LO_POW                     ((u32)0x00000001 << 30)                  /*!< R/W   0x0 Lineout power down control (0: power down, 1: power on) */
#define CORE_S_BIT_AUDIO_LO_SE                      ((u32)0x00000001 << 29)                  /*!< R/W   0x0 Lineout  single-end mode control (0: differential, 1: single-end) */
#define CORE_S_MASK_AUDIO_LO_STBLE_CRTL_1_0_        ((u32)0x00000003 << 27)                  /*!< R/W   0x0
                                                                                              * LO_STBLE_CTRL<0>
                                                                                              * 0: vb<0> use large switch to get better THD performance of Lineout
                                                                                              * 1: vb<0> use small switch
                                                                                              * 
                                                                                              * LO_STBLE_CTRL<1>
                                                                                              * 0: enlarge 2nd stage gm of LO opamp to reduce voltage stress
                                                                                              * 1: 2nd stage gm of LO opamp use default small gm
                                                                                              */
#define CORE_S_AUDIO_LO_STBLE_CRTL_1_0_(x)          ((u32)(((x) & 0x00000003) << 27))
#define CORE_S_GET_AUDIO_LO_STBLE_CRTL_1_0_(x)      ((u32)(((x >> 27) & 0x00000003)))
#define CORE_S_BIT_AUDIO_LPMODE_EN                  ((u32)0x00000001 << 26)                  /*!< R/W   0x0
                                                                                              * low power mode en
                                                                                              * 0: disable , current use 12.5uA
                                                                                              * 1: enable , current use 1uA
                                                                                              */
#define CORE_S_MASK_AUDIO_MBIAS_ISEL_DAC_2_0_       ((u32)0x00000007 << 23)                  /*!< R/W   0x2
                                                                                              * DAC current control:
                                                                                              * default 4uA, 0.5uA per step
                                                                                              */
#define CORE_S_AUDIO_MBIAS_ISEL_DAC_2_0_(x)         ((u32)(((x) & 0x00000007) << 23))
#define CORE_S_GET_AUDIO_MBIAS_ISEL_DAC_2_0_(x)     ((u32)(((x >> 23) & 0x00000007)))
#define CORE_S_MASK_AUDIO_MBIAS_ISEL_DAC_VCM_2_0__  ((u32)0x00000007 << 20)                  /*!< R/W   0x2
                                                                                              * DAC vcm buffer current control:
                                                                                              * default 4uA, 0.5uA per step
                                                                                              */
#define CORE_S_AUDIO_MBIAS_ISEL_DAC_VCM_2_0__(x)    ((u32)(((x) & 0x00000007) << 20))
#define CORE_S_GET_AUDIO_MBIAS_ISEL_DAC_VCM_2_0__(x) ((u32)(((x >> 20) & 0x00000007)))
#define CORE_S_MASK_AUDIO_MBIAS_ISEL_DPRAMP_2_0_    ((u32)0x00000007 << 17)                  /*!< R/W   0x2
                                                                                              * DPRAMP current control:
                                                                                              * default 4uA, 0.5uA per step
                                                                                              */
#define CORE_S_AUDIO_MBIAS_ISEL_DPRAMP_2_0_(x)      ((u32)(((x) & 0x00000007) << 17))
#define CORE_S_GET_AUDIO_MBIAS_ISEL_DPRAMP_2_0_(x)  ((u32)(((x >> 17) & 0x00000007)))
#define CORE_S_MASK_AUDIO_MBIAS_ISEL_DTSDM_2_0_     ((u32)0x00000007 << 14)                  /*!< R/W   0x2
                                                                                              * DTSDM current control:
                                                                                              * default 4uA, 0.5uA per step
                                                                                              */
#define CORE_S_AUDIO_MBIAS_ISEL_DTSDM_2_0_(x)       ((u32)(((x) & 0x00000007) << 14))
#define CORE_S_GET_AUDIO_MBIAS_ISEL_DTSDM_2_0_(x)   ((u32)(((x >> 14) & 0x00000007)))
#define CORE_S_MASK_AUDIO_MBIAS_ISEL_DTSDM_INT1_2_0__ ((u32)0x00000007 << 11)                  /*!< R/W   0x2
                                                                                              * DTSDM_INT1 current control:
                                                                                              * default 8uA, 0.5uA per step
                                                                                              */
#define CORE_S_AUDIO_MBIAS_ISEL_DTSDM_INT1_2_0__(x) ((u32)(((x) & 0x00000007) << 11))
#define CORE_S_GET_AUDIO_MBIAS_ISEL_DTSDM_INT1_2_0__(x) ((u32)(((x >> 11) & 0x00000007)))
#define CORE_S_MASK_AUDIO_MBIAS_ISEL_LO_2_0_        ((u32)0x00000007 << 8)                   /*!< R/W   0x2
                                                                                              * Lineout current control:
                                                                                              * default 4uA, 1uA per step
                                                                                              */
#define CORE_S_AUDIO_MBIAS_ISEL_LO_2_0_(x)          ((u32)(((x) & 0x00000007) << 8))
#define CORE_S_GET_AUDIO_MBIAS_ISEL_LO_2_0_(x)      ((u32)(((x >> 8) & 0x00000007)))
#define CORE_S_MASK_AUDIO_MBIAS_ISEL_MICBIAS_2_0__  ((u32)0x00000007 << 5)                   /*!< R/W   0x2
                                                                                              * MIVBIAS current control:
                                                                                              * default 4uA, 0.5uA per step
                                                                                              */
#define CORE_S_AUDIO_MBIAS_ISEL_MICBIAS_2_0__(x)    ((u32)(((x) & 0x00000007) << 5))
#define CORE_S_GET_AUDIO_MBIAS_ISEL_MICBIAS_2_0__(x) ((u32)(((x >> 5) & 0x00000007)))
#define CORE_S_MASK_AUDIO_MBIAS_ISEL_MICBST_2_0_    ((u32)0x00000007 << 2)                   /*!< R/W   0x2
                                                                                              * MICBST current control:
                                                                                              * default 4uA, 1uA per step
                                                                                              */
#define CORE_S_AUDIO_MBIAS_ISEL_MICBST_2_0_(x)      ((u32)(((x) & 0x00000007) << 2))
#define CORE_S_GET_AUDIO_MBIAS_ISEL_MICBST_2_0_(x)  ((u32)(((x >> 2) & 0x00000007)))
#define CORE_S_BIT_AUDIO_MBIAS_POW                  ((u32)0x00000001 << 1)                   /*!< R/W   0x0
                                                                                              * MICBIAS power control
                                                                                              * 0:power down
                                                                                              * 1:power on
                                                                                              */
#define CORE_S_BIT_AUDIO_MICBIAS_COMP               ((u32)0x00000001 << 0)                   /*!< R/W   0x0
                                                                                              * MICBIAS_COMP
                                                                                              * 0: disable internal miller cap
                                                                                              * 1: enable internal miller cap
                                                                                              */
/** @} */


/** @defgroup REG_AUDIO_ANA_CTRL3
 * @brief
 * @{
 */
#define CORE_S_MASK_AUDIO_MICBIAS_COUNT_1_0_        ((u32)0x00000003 << 23)                  /*!< R/W   0x0
                                                                                              * when OCP happen disable time @312.5kHz
                                                                                              * 00: 819.2us
                                                                                              * 01: 1638.4us
                                                                                              * 10: 3276.8us
                                                                                              * 11: 6553.6us
                                                                                              */
#define CORE_S_AUDIO_MICBIAS_COUNT_1_0_(x)          ((u32)(((x) & 0x00000003) << 23))
#define CORE_S_GET_AUDIO_MICBIAS_COUNT_1_0_(x)      ((u32)(((x >> 23) & 0x00000003)))
#define CORE_S_BIT_AUDIO_MICBIAS_ENCHX              ((u32)0x00000001 << 22)                  /*!< R/W   0x1
                                                                                              * MICBIAS enable chopper clock
                                                                                              * 0:disable
                                                                                              * 1:enable
                                                                                              */
#define CORE_S_BIT_AUDIO_MICBIAS_OC                 ((u32)0x00000001 << 21)                  /*!< R     0x0 OCP happen output signal */
#define CORE_S_MASK_AUDIO_MICBIAS_OCSEL_1_0_        ((u32)0x00000003 << 19)                  /*!< R/W   0x0
                                                                                              * OCP current selection
                                                                                              * 00: 2.5mA
                                                                                              * 01: 5mA
                                                                                              * 10: 7.5mA
                                                                                              * 11: 10mA
                                                                                              */
#define CORE_S_AUDIO_MICBIAS_OCSEL_1_0_(x)          ((u32)(((x) & 0x00000003) << 19))
#define CORE_S_GET_AUDIO_MICBIAS_OCSEL_1_0_(x)      ((u32)(((x >> 19) & 0x00000003)))
#define CORE_S_BIT_AUDIO_MICBIAS_POW                ((u32)0x00000001 << 18)                  /*!< R/W   0x0
                                                                                              * MBIAS power control
                                                                                              * 0: power down
                                                                                              * 1: power on
                                                                                              */
#define CORE_S_BIT_AUDIO_MICBIAS_POWSHDT            ((u32)0x00000001 << 17)                  /*!< R/W   0x0
                                                                                              * MICBIAS OCP power control
                                                                                              * 0: disable OCP
                                                                                              * 1: enable OCP
                                                                                              */
#define CORE_S_MASK_AUDIO_MICBIAS_RESERVE_2_0_      ((u32)0x00000007 << 14)                  /*!< R/W   0x0 MICBIAS reserve */
#define CORE_S_AUDIO_MICBIAS_RESERVE_2_0_(x)        ((u32)(((x) & 0x00000007) << 14))
#define CORE_S_GET_AUDIO_MICBIAS_RESERVE_2_0_(x)    ((u32)(((x >> 14) & 0x00000007)))
#define CORE_S_MASK_AUDIO_MICBIAS_VSET_3_0_         ((u32)0x0000000F << 10)                  /*!< R/W   0xC
                                                                                              * MICBIAS select output voltage level
                                                                                              * 0.05V per step
                                                                                              * 0000:1.2V     1000:1.6V
                                                                                              * 0001:1.25V    1001:1.65V
                                                                                              * 0010:1.3V     1010:1.7V
                                                                                              * 0011:1.35V    1011:1.75V
                                                                                              * 0100:1.4V     1100:1.8V
                                                                                              * 0101:1.45V    1101:1.85V
                                                                                              * 0110:1.5V     1110:1.9V
                                                                                              * 0111:1.55V    1111:1.95V
                                                                                              */
#define CORE_S_AUDIO_MICBIAS_VSET_3_0_(x)           ((u32)(((x) & 0x0000000F) << 10))
#define CORE_S_GET_AUDIO_MICBIAS_VSET_3_0_(x)       ((u32)(((x >> 10) & 0x0000000F)))
#define CORE_S_BIT_AUDIO_MICBST_ENCAL_              ((u32)0x00000001 << 9)                   /*!< R/W   0x0
                                                                                              * MICBST left channel enable calibration path
                                                                                              * 0:disable
                                                                                              * 1:enable
                                                                                              */
#define CORE_S_BIT_AUDIO_MICBST_ENCAL_SWAP_         ((u32)0x00000001 << 8)                   /*!< R/W   0x0
                                                                                              * MICBST left channel swap calibration path
                                                                                              * 0:disable
                                                                                              * 1:enable
                                                                                              */
#define CORE_S_BIT_AUDIO_MICBST_ENDF                ((u32)0x00000001 << 7)                   /*!< R/W   0x0
                                                                                              * MICBST left channel enable differential
                                                                                              * 0:single to differential
                                                                                              * 1:differential to differential
                                                                                              */
#define CORE_S_MASK_AUDIO_MICBST_GSEL_3_0_          ((u32)0x0000000F << 3)                   /*!< R/W   0x0
                                                                                              * MICBST left channel gain select
                                                                                              * 0000: 0dB
                                                                                              * 0001:5dB
                                                                                              * 0010:10dB
                                                                                              * 0011:15dB
                                                                                              * 0100:20dB
                                                                                              * 0101:25dB
                                                                                              * 0110:30dB
                                                                                              * 0111:35dB
                                                                                              * 1XXX:40dB
                                                                                              */
#define CORE_S_AUDIO_MICBST_GSEL_3_0_(x)            ((u32)(((x) & 0x0000000F) << 3))
#define CORE_S_GET_AUDIO_MICBST_GSEL_3_0_(x)        ((u32)(((x >> 3) & 0x0000000F)))
#define CORE_S_BIT_AUDIO_MICBST_MUTE                ((u32)0x00000001 << 2)                   /*!< R/W   0x1
                                                                                              * MICBST left channel  mute control
                                                                                              * 0:unmute  1:mute
                                                                                              */
#define CORE_S_BIT_AUDIO_MICBST_POW                 ((u32)0x00000001 << 1)                   /*!< R/W   0x0
                                                                                              * MICBST power control
                                                                                              * left channel
                                                                                              * 0:power down  1:power on
                                                                                              */
#define CORE_S_BIT_AUDIO_POWADDACK                  ((u32)0x00000001 << 0)                   /*!< R/W   0x0 AD/DA clock power down control (0: power down, 1: power on) */
/** @} */


/** @defgroup REG_DDR_CRT_CTRL0
 * @brief
 * @{
 */
#define CORE_S_MASK_CRT_F_CODE_T                    ((u32)0x000007FF << 11)                  /*!< R/W   0x0 None */
#define CORE_S_CRT_F_CODE_T(x)                      ((u32)(((x) & 0x000007FF) << 11))
#define CORE_S_GET_CRT_F_CODE_T(x)                  ((u32)(((x >> 11) & 0x000007FF)))
#define CORE_S_MASK_CRT_F_CODE                      ((u32)0x000007FF << 0)                   /*!< R/W   0x0 None */
#define CORE_S_CRT_F_CODE(x)                        ((u32)(((x) & 0x000007FF) << 0))
#define CORE_S_GET_CRT_F_CODE(x)                    ((u32)(((x >> 0) & 0x000007FF)))
/** @} */


/** @defgroup REG_DDR_CRT_CTRL1
 * @brief
 * @{
 */
#define CORE_S_MASK_CRT_N_CODE_T                    ((u32)0x000000FF << 16)                  /*!< R/W   0x0 None */
#define CORE_S_CRT_N_CODE_T(x)                      ((u32)(((x) & 0x000000FF) << 16))
#define CORE_S_GET_CRT_N_CODE_T(x)                  ((u32)(((x >> 16) & 0x000000FF)))
#define CORE_S_MASK_CRT_N_CODE                      ((u32)0x000000FF << 8)                   /*!< R/W   0x0 None */
#define CORE_S_CRT_N_CODE(x)                        ((u32)(((x) & 0x000000FF) << 8))
#define CORE_S_GET_CRT_N_CODE(x)                    ((u32)(((x >> 8) & 0x000000FF)))
#define CORE_S_BIT_CRT_EN_SSC                       ((u32)0x00000001 << 3)                   /*!< R/W   0x0 None */
#define CORE_S_BIT_CRT_OC_EN                        ((u32)0x00000001 << 2)                   /*!< R/W   0x1 None */
#define CORE_S_BIT_CRT_SSC_FLOG_INIT                ((u32)0x00000001 << 1)                   /*!< R/W   0x0 None */
#define CORE_S_BIT_CRT_SSC_SEL                      ((u32)0x00000001 << 0)                   /*!< R/W   0x0 1: control by system register; 0: control by CRT register */
/** @} */


/** @defgroup REG_SYS_LPDDR1_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_HS_LPDDR1_SPEEDUP                ((u32)0x00000001 << 8)                   /*!< R/W     0 rxi310_speedup_sim */
/** @} */


/** @defgroup REG_SYS_DDRPHY_CTRL
 * @brief
 * @{
 */
#define CORE_S_BIT_DFI_PLL_REF_TEST_MODE            ((u32)0x00000001 << 10)                  /*!< R/W   0   1:enable */
#define CORE_S_BIT_DFI_PLL_TEST_MODE                ((u32)0x00000001 << 9)                   /*!< R/W   0   1:enable */
#define CORE_S_BIT_DFI_PLL_TEST_CKE                 ((u32)0x00000001 << 8)                   /*!< R/W   0   1:enable */
#define CORE_S_BIT_HS_DDRPHY_CRT_CLK_EN             ((u32)0x00000001 << 7)                   /*!< R/W     0 CRT control register clock gating control */
#define CORE_S_BIT_HS_DDRPHY_RBUS_CLK_EN            ((u32)0x00000001 << 6)                   /*!< R/W     0 Rbus interface clock gating control */
#define CORE_S_BIT_DDRPHY_VCCON                     ((u32)0x00000001 << 4)                   /*!< R/W   0   1: enable core power */
#define CORE_S_BIT_HS_DDRPHY_PLL_RDY                ((u32)0x00000001 << 3)                   /*!< R       0 PLL ready */
#define CORE_S_BIT_DDRPHY_RBUS_EN                   ((u32)0x00000001 << 2)                   /*!< R/W     0 1: reset RBUS */
#define CORE_S_BIT_PWDPAD15N                        ((u32)0x00000001 << 1)                   /*!< R/W     0 0:RESET PAD pull high and CKE PAD pull low (DRAM into seif-refresh mode) */
#define CORE_S_BIT_HS_DDRPHY_CRT_RST                ((u32)0x00000001 << 0)                   /*!< R/W     0 CRT active low asynchronous reset */
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY0_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE               ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 Entry0[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE(x)                 ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE(x)             ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY0_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_204           ((u32)0x000001FF << 0)                   /*!< R/W     0 Entry0[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_204(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_204(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY1_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_208           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY1[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_208(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_208(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY1_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_20C           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY1[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_20C(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_20C(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY2_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_210           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY2[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_210(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_210(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY2_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_214           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY2[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_214(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_214(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY3_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_218           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY3[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_218(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_218(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY3_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_21C           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY3[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_21C(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_21C(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY4_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_220           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY4[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_220(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_220(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY4_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_224           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY4[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_224(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_224(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY5_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_228           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY5[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_228(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_228(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY5_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_22C           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY5[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_22C(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_22C(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY6_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_230           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY6[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_230(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_230(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY6_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_234           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY6[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_234(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_234(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY7_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_238           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY7[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_238(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_238(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY7_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_23C           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY7[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_23C(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_23C(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY8_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_240           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY8[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_240(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_240(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY8_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_244           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY8[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_244(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_244(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY9_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_248           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY9[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_248(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_248(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY9_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_24C           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY9[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_24C(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_24C(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY10_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_250           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY10[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_250(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_250(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY10_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_254           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY10[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_254(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_254(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY11_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_258           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY11[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_258(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_258(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY11_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_25C           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY11[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_25C(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_25C(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY12_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_260           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY12[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_260(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_260(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY12_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_264           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY12[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_264(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_264(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY13_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_268           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY13[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_268(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_268(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY13_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_26C           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY13[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_26C(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_26C(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY14_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_270           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY14[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_270(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_270(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY14_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_274           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY14[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_274(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_274(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY15_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_278           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY15[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_278(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_278(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY15_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_27C           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY15[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_27C(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_27C(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY16_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_280           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY16[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_280(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_280(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY16_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_284           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY16[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_284(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_284(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY17_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_288           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY17[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_288(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_288(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY17_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_28C           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY17[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_28C(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_28C(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY18_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_290           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY18[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_290(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_290(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY18_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_294           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY18[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_294(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_294(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY19_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_298           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY19[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_298(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_298(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY19_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_29C           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY19[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_29C(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_29C(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY20_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2A0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY20[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2A0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2A0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY20_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2A4           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY20[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2A4(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2A4(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY21_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2A8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY21[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2A8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2A8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY21_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2AC           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY21[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2AC(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2AC(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY22_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2B0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY22[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2B0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2B0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY22_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2B4           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY22[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2B4(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2B4(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY23_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2B8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY23[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2B8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2B8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY23_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2BC           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY23[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2BC(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2BC(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY24_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2C0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY24[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2C0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2C0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY24_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2C4           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY24[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2C4(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2C4(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY25_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2C8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY25[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2C8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2C8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY25_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2CC           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY25[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2CC(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2CC(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY26_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2D0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY26[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2D0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2D0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY26_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2D4           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY26[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2D4(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2D4(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY27_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2D8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY27[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2D8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2D8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY27_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2DC           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY27[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2DC(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2DC(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY28_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2E0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY28[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2E0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2E0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY28_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2E4           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY28[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2E4(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2E4(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY29_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2E8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY29[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2E8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2E8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY29_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2EC           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY29[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2EC(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2EC(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY30_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2F0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY30[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2F0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2F0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY30_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2F4           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY30[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2F4(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2F4(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY31_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2F8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY31[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2F8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2F8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY31_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_2FC           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY31[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_2FC(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_2FC(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY32_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_300           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY32[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_300(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_300(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY32_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_304           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY32[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_304(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_304(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY33_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_308           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY33[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_308(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_308(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY33_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_30C           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY33[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_30C(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_30C(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY34_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_310           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY34[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_310(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_310(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY34_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_314           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY34[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_314(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_314(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY35_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_318           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY35[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_318(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_318(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY35_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_31C           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY35[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_31C(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_31C(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY36_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_320           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY36[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_320(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_320(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY36_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_324           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY36[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_324(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_324(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY37_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_328           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY37[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_328(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_328(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY37_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_32C           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY37[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_32C(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_32C(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY38_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_330           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY38[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_330(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_330(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY38_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_334           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY38[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_334(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_334(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY39_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_338           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY39[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_338(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_338(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY39_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_33C           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY39[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_33C(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_33C(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY40_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_340           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY40[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_340(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_340(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY40_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_344           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY40[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_344(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_344(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY41_0
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_348           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY41[31:0] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_348(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_348(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_ENTRY41_1
 * @brief
 * @{
 */
#define CORE_S_MASK_DDRPLL_CTRL_VALUE_34C           ((u32)0x000001FF << 0)                   /*!< R/W     0 ENTRY41[40:32] value when channel number = 1 */
#define CORE_S_DDRPLL_CTRL_VALUE_34C(x)             ((u32)(((x) & 0x000001FF) << 0))
#define CORE_S_GET_DDRPLL_CTRL_VALUE_34C(x)         ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_CTRL0
 * @brief
 * @{
 */
#define CORE_S_MASK_CNT1                            ((u32)0x0000FFFF << 16)                  /*!< R/W     0 None */
#define CORE_S_CNT1(x)                              ((u32)(((x) & 0x0000FFFF) << 16))
#define CORE_S_GET_CNT1(x)                          ((u32)(((x >> 16) & 0x0000FFFF)))
#define CORE_S_BIT_DDRPLL_TRACKING_EN               ((u32)0x00000001 << 0)                   /*!< R/W     0 Enable PLL tracking function */
/** @} */


/** @defgroup REG_DDRPLL_TRACKING_CTRL1
 * @brief
 * @{
 */
#define CORE_S_MASK_CNT2                            ((u32)0x0000FFFF << 16)                  /*!< R/W     0 None */
#define CORE_S_CNT2(x)                              ((u32)(((x) & 0x0000FFFF) << 16))
#define CORE_S_GET_CNT2(x)                          ((u32)(((x >> 16) & 0x0000FFFF)))
#define CORE_S_MASK_CNT3                            ((u32)0x0000FFFF << 0)                   /*!< R/W     0 None */
#define CORE_S_CNT3(x)                              ((u32)(((x) & 0x0000FFFF) << 0))
#define CORE_S_GET_CNT3(x)                          ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */




/** @defgroup CORE_S_Exported_Types CORE_S Exported Types
* @{
*/
/**
* @brief CORE_S Register Declaration
*/

typedef struct {
	__IO uint32_t CORE_S_DUMMY0;                                /*!< Register,  Address offset:0x0000 */
	__IO uint32_t VID_AID_INFO1;                                /*!< Register,  Address offset:0x0004 */
	__IO uint32_t VID_AID_INFO2;                                /*!< Register,  Address offset:0x0008 */
	__IO uint32_t AID_DATA_CTRL;                                /*!< Register,  Address offset:0x000C */
	__IO uint32_t CORE_S_DUMMY1;                                /*!< Register,  Address offset:0x0010 */
	__IO uint32_t MEM_CORE_CTRL0;                               /*!< Register,  Address offset:0x0014 */
	__IO uint32_t MEM_CORE_CTRL1;                               /*!< Register,  Address offset:0x0018 */
	__I  uint32_t CORE_S_DUMMY2;                                /*!< Register,  Address offset:0x001C */
	__I  uint32_t CORE_S_DUMMY3;                                /*!< Register,  Address offset:0x0020 */
	__I  uint32_t CORE_S_DUMMY4;                                /*!< Register,  Address offset:0x0024 */
	__I  uint32_t CORE_S_DUMMY5;                                /*!< Register,  Address offset:0x0028 */
	__I  uint32_t CORE_S_DUMMY6;                                /*!< Register,  Address offset:0x002C */
	__I  uint32_t CORE_S_DUMMY7;                                /*!< Register,  Address offset:0x0030 */
	__I  uint32_t CORE_S_DUMMY8;                                /*!< Register,  Address offset:0x0034 */
	__I  uint32_t CORE_S_DUMMY9;                                /*!< Register,  Address offset:0x0038 */
	__I  uint32_t CORE_S_DUMMY10;                               /*!< Register,  Address offset:0x003C */
	__I  uint32_t CORE_S_DUMMY11;                               /*!< Register,  Address offset:0x0040 */
	__I  uint32_t CORE_S_DUMMY12;                               /*!< Register,  Address offset:0x0044 */
	__I  uint32_t CORE_S_DUMMY13;                               /*!< Register,  Address offset:0x0048 */
	__I  uint32_t CORE_S_DUMMY14;                               /*!< Register,  Address offset:0x004C */
	__I  uint32_t CORE_S_DUMMY15;                               /*!< Register,  Address offset:0x0050 */
	__I  uint32_t CORE_S_DUMMY16;                               /*!< Register,  Address offset:0x0054 */
	__I  uint32_t CORE_S_DUMMY17;                               /*!< Register,  Address offset:0x0058 */
	__I  uint32_t CORE_S_DUMMY18;                               /*!< Register,  Address offset:0x005C */
	__I  uint32_t CORE_S_DUMMY19;                               /*!< Register,  Address offset:0x0060 */
	__I  uint32_t CORE_S_DUMMY20;                               /*!< Register,  Address offset:0x0064 */
	__I  uint32_t CORE_S_DUMMY21;                               /*!< Register,  Address offset:0x0068 */
	__I  uint32_t CORE_S_DUMMY22;                               /*!< Register,  Address offset:0x006C */
	__I  uint32_t CORE_S_DUMMY23;                               /*!< Register,  Address offset:0x0070 */
	__I  uint32_t CORE_S_DUMMY24;                               /*!< Register,  Address offset:0x0074 */
	__I  uint32_t CORE_S_DUMMY25;                               /*!< Register,  Address offset:0x0078 */
	__I  uint32_t CORE_S_DUMMY26;                               /*!< Register,  Address offset:0x007C */
	__IO uint32_t CORE_S_DUMMY27;                               /*!< Register,  Address offset:0x0080 */
	__IO uint32_t CORE_S_DUMMY43;                               /*!< Register,  Address offset:0x0084 */
	__IO uint32_t CORE_S_DUMMY44;                               /*!< Register,  Address offset:0x0088 */
	__I  uint32_t CORE_S_DUMMY28;                               /*!< Register,  Address offset:0x008C */
	__IO uint32_t CORE_S_DUMMY29;                               /*!< Register,  Address offset:0x0090 */
	__IO uint32_t CORE_S_DUMMY30;                               /*!< Register,  Address offset:0x0094 */
	__IO uint32_t HS_PLATFORM_PARA;                             /*!< Register,  Address offset:0x0098 */
	__IO uint32_t SYS_PLATFORM_CTRL0;                           /*!< Register,  Address offset:0x009C */
	uint32_t RSVD_NO_FUNCTION0[24];
	__I  uint32_t CORE_PLATFORM_DBG0;                           /*!< Register,  Address offset:0x0100 */
	__I  uint32_t CORE_PLATFORM_DBG1;                           /*!< Register,  Address offset:0x0104 */
	__I  uint32_t CORE_PLATFORM_DBG2;                           /*!< Register,  Address offset:0x0108 */
	__I  uint32_t CORE_S_DUMMY34;                               /*!< Register,  Address offset:0x010C */
	__I  uint32_t CORE_S_DUMMY35;                               /*!< Register,  Address offset:0x0110 */
	__IO uint32_t SYS_AUDIO_CTRL;                               /*!< Register,  Address offset:0x0114 */
	__IO uint32_t SYS_ISP_CTRL;                                 /*!< Register,  Address offset:0x0118 */
	__I  uint32_t CORE_S_DUMMY36;                               /*!< Register,  Address offset:0x011C */
	__IO uint32_t SYS_OTG_CTRL;                                 /*!< Register,  Address offset:0x0120 */
	__IO uint32_t SYS_SDIO_SD_CTRL;                             /*!< Register,  Address offset:0x0124 */
	__I  uint32_t CORE_S_DUMMY37;                               /*!< Register,  Address offset:0x0128 */
	__I  uint32_t CORE_S_DUMMY38;                               /*!< Register,  Address offset:0x012C */
	__I  uint32_t CORE_S_DUMMY39;                               /*!< Register,  Address offset:0x0130 */
	__I  uint32_t CORE_S_DUMMY40;                               /*!< Register,  Address offset:0x0134 */
	__IO uint32_t SYS_SGPIO_CTRL;                               /*!< Register,  Address offset:0x0138 */
	__I  uint32_t CORE_S_DUMMY41;                               /*!< Register,  Address offset:0x013C */
	__I  uint32_t CORE_S_DUMMY42;                               /*!< Register,  Address offset:0x0140 */
	__IO uint32_t DSS_CPU;                                      /*!< Register,  Address offset:0x0144 */
	__IO uint32_t DSS_ENCODER;                                  /*!< Register,  Address offset:0x0148 */
	__IO uint32_t DSS_WLAN_OFF;                                 /*!< Register,  Address offset:0x014C */
	__IO uint32_t DSS_SOC;                                      /*!< Register,  Address offset:0x0150 */
	__IO uint32_t DSS_REF1;                                     /*!< Register,  Address offset:0x0154 */
	__IO uint32_t DSS_REF2;                                     /*!< Register,  Address offset:0x0158 */
	__IO uint32_t VOL_PROBE;                                    /*!< Register,  Address offset:0x015C */
	__IO uint32_t SYS_AI_SCLK_CTRL;                             /*!< Register,  Address offset:0x0160 */
	__IO uint32_t AUDIO_CODEC_CLK_CTRL;                         /*!< Register,  Address offset:0x0164 */
	__IO uint32_t SYS_ENC_ACLK_CTRL;                            /*!< Register,  Address offset:0x0168 */
	__IO uint32_t SYS_ENC_SCLK_CTRL;                            /*!< Register,  Address offset:0x016C */
	__IO uint32_t SYS_FAST_PCLK_CTRL;                           /*!< Register,  Address offset:0x0170 */
	__IO uint32_t SYS_GE_SCLK_CTRL;                             /*!< Register,  Address offset:0x0174 */
	__IO uint32_t SYS_GMAC_CLK_CTRL;                            /*!< Register,  Address offset:0x0178 */
	__IO uint32_t SYS_ISP_MIPICLK_CTRL;                         /*!< Register,  Address offset:0x017C */
	__IO uint32_t SYS_ISP_BUSCLK_CTRL;                          /*!< Register,  Address offset:0x0180 */
	__IO uint32_t SYS_ISP_SCLK_CTRL;                            /*!< Register,  Address offset:0x0184 */
	__IO uint32_t SYS_KM4_MP_CLK_CTRL;                          /*!< Register,  Address offset:0x0188 */
	__IO uint32_t SYS_NN_ACLK_CTRL;                             /*!< Register,  Address offset:0x018C */
	__IO uint32_t SYS_NN_SCLK_CTRL;                             /*!< Register,  Address offset:0x0190 */
	__IO uint32_t SYS_PLF_CLK_CTRL;                             /*!< Register,  Address offset:0x0194 */
	__IO uint32_t SYS_SD_CLK_CTRL;                              /*!< Register,  Address offset:0x0198 */
	__IO uint32_t SYS_SPORT_CLK_CTRL;                           /*!< Register,  Address offset:0x019C */
	__IO uint32_t SYS_SENSOR_CLK_CTRL;                          /*!< Register,  Address offset:0x01A0 */
	__IO uint32_t SYS_USBOTG_CLK_CTRL;                          /*!< Register,  Address offset:0x01A4 */
	__IO uint32_t SYS_0_CLK_CTRL;                               /*!< Register,  Address offset:0x01A8 */
	__IO uint32_t SYS_1_CLK_CTRL;                               /*!< Register,  Address offset:0x01AC */
	__IO uint32_t SYS_2_CLK_CTRL;                               /*!< Register,  Address offset:0x01B0 */
	__I  uint32_t SYS_0_FUNC_CTRL;                              /*!< Register,  Address offset:0x01B4 */
	__IO uint32_t SYS_1_FUNC_CTRL;                              /*!< Register,  Address offset:0x01B8 */
	__IO uint32_t SYS_2_FUNC_CTRL;                              /*!< Register,  Address offset:0x01BC */
	__IO uint32_t APWDT_RST_CTRL;                               /*!< Register,  Address offset:0x01C0 */
	__IO uint32_t MPWDT_RST_CTRL;                               /*!< Register,  Address offset:0x01C4 */
	__IO uint32_t LX_CTRL0;                                     /*!< Register,  Address offset:0x01C8 */
	__IO uint32_t LX_CTRL1;                                     /*!< Register,  Address offset:0x01CC */
	__IO uint32_t CORE_S_SPARE1;                                /*!< Register,  Address offset:0x01D0 */
	__IO uint32_t CORE_S_SPARE2;                                /*!< Register,  Address offset:0x01D4 */
	__IO uint32_t CORE_S_SPARE3;                                /*!< Register,  Address offset:0x01D8 */
	__IO uint32_t CORE_S_SPARE4;                                /*!< Register,  Address offset:0x01DC */
	__IO uint32_t AUDIO_ANA_CTRL0;                              /*!< Register,  Address offset:0x01E0 */
	__IO uint32_t AUDIO_ANA_CTRL1;                              /*!< Register,  Address offset:0x01E4 */
	__IO uint32_t AUDIO_ANA_CTRL2;                              /*!< Register,  Address offset:0x01E8 */
	__IO uint32_t AUDIO_ANA_CTRL3;                              /*!< Register,  Address offset:0x01EC */
	__IO uint32_t DDR_CRT_CTRL0;                                /*!< Register,  Address offset:0x01F0 */
	__IO uint32_t DDR_CRT_CTRL1;                                /*!< Register,  Address offset:0x01F4 */
	__IO uint32_t SYS_LPDDR1_CTRL;                              /*!< Register,  Address offset:0x01F8 */
	__IO uint32_t SYS_DDRPHY_CTRL;                              /*!< Register,  Address offset:0x01FC */
	__IO uint32_t DDRPLL_TRACKING_ENTRY0_0;                     /*!< Register,  Address offset:0x0200 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY0_1;                     /*!< Register,  Address offset:0x0204 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY1_0;                     /*!< Register,  Address offset:0x0208 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY1_1;                     /*!< Register,  Address offset:0x020C */
	__IO uint32_t DDRPLL_TRACKING_ENTRY2_0;                     /*!< Register,  Address offset:0x0210 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY2_1;                     /*!< Register,  Address offset:0x0214 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY3_0;                     /*!< Register,  Address offset:0x0218 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY3_1;                     /*!< Register,  Address offset:0x021C */
	__IO uint32_t DDRPLL_TRACKING_ENTRY4_0;                     /*!< Register,  Address offset:0x0220 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY4_1;                     /*!< Register,  Address offset:0x0224 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY5_0;                     /*!< Register,  Address offset:0x0228 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY5_1;                     /*!< Register,  Address offset:0x022C */
	__IO uint32_t DDRPLL_TRACKING_ENTRY6_0;                     /*!< Register,  Address offset:0x0230 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY6_1;                     /*!< Register,  Address offset:0x0234 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY7_0;                     /*!< Register,  Address offset:0x0238 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY7_1;                     /*!< Register,  Address offset:0x023C */
	__IO uint32_t DDRPLL_TRACKING_ENTRY8_0;                     /*!< Register,  Address offset:0x0240 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY8_1;                     /*!< Register,  Address offset:0x0244 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY9_0;                     /*!< Register,  Address offset:0x0248 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY9_1;                     /*!< Register,  Address offset:0x024C */
	__IO uint32_t DDRPLL_TRACKING_ENTRY10_0;                    /*!< Register,  Address offset:0x0250 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY10_1;                    /*!< Register,  Address offset:0x0254 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY11_0;                    /*!< Register,  Address offset:0x0258 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY11_1;                    /*!< Register,  Address offset:0x025C */
	__IO uint32_t DDRPLL_TRACKING_ENTRY12_0;                    /*!< Register,  Address offset:0x0260 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY12_1;                    /*!< Register,  Address offset:0x0264 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY13_0;                    /*!< Register,  Address offset:0x0268 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY13_1;                    /*!< Register,  Address offset:0x026C */
	__IO uint32_t DDRPLL_TRACKING_ENTRY14_0;                    /*!< Register,  Address offset:0x0270 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY14_1;                    /*!< Register,  Address offset:0x0274 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY15_0;                    /*!< Register,  Address offset:0x0278 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY15_1;                    /*!< Register,  Address offset:0x027C */
	__IO uint32_t DDRPLL_TRACKING_ENTRY16_0;                    /*!< Register,  Address offset:0x0280 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY16_1;                    /*!< Register,  Address offset:0x0284 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY17_0;                    /*!< Register,  Address offset:0x0288 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY17_1;                    /*!< Register,  Address offset:0x028C */
	__IO uint32_t DDRPLL_TRACKING_ENTRY18_0;                    /*!< Register,  Address offset:0x0290 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY18_1;                    /*!< Register,  Address offset:0x0294 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY19_0;                    /*!< Register,  Address offset:0x0298 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY19_1;                    /*!< Register,  Address offset:0x029C */
	__IO uint32_t DDRPLL_TRACKING_ENTRY20_0;                    /*!< Register,  Address offset:0x02A0 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY20_1;                    /*!< Register,  Address offset:0x02A4 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY21_0;                    /*!< Register,  Address offset:0x02A8 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY21_1;                    /*!< Register,  Address offset:0x02AC */
	__IO uint32_t DDRPLL_TRACKING_ENTRY22_0;                    /*!< Register,  Address offset:0x02B0 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY22_1;                    /*!< Register,  Address offset:0x02B4 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY23_0;                    /*!< Register,  Address offset:0x02B8 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY23_1;                    /*!< Register,  Address offset:0x02BC */
	__IO uint32_t DDRPLL_TRACKING_ENTRY24_0;                    /*!< Register,  Address offset:0x02C0 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY24_1;                    /*!< Register,  Address offset:0x02C4 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY25_0;                    /*!< Register,  Address offset:0x02C8 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY25_1;                    /*!< Register,  Address offset:0x02CC */
	__IO uint32_t DDRPLL_TRACKING_ENTRY26_0;                    /*!< Register,  Address offset:0x02D0 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY26_1;                    /*!< Register,  Address offset:0x02D4 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY27_0;                    /*!< Register,  Address offset:0x02D8 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY27_1;                    /*!< Register,  Address offset:0x02DC */
	__IO uint32_t DDRPLL_TRACKING_ENTRY28_0;                    /*!< Register,  Address offset:0x02E0 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY28_1;                    /*!< Register,  Address offset:0x02E4 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY29_0;                    /*!< Register,  Address offset:0x02E8 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY29_1;                    /*!< Register,  Address offset:0x02EC */
	__IO uint32_t DDRPLL_TRACKING_ENTRY30_0;                    /*!< Register,  Address offset:0x02F0 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY30_1;                    /*!< Register,  Address offset:0x02F4 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY31_0;                    /*!< Register,  Address offset:0x02F8 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY31_1;                    /*!< Register,  Address offset:0x02FC */
	__IO uint32_t DDRPLL_TRACKING_ENTRY32_0;                    /*!< Register,  Address offset:0x0300 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY32_1;                    /*!< Register,  Address offset:0x0304 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY33_0;                    /*!< Register,  Address offset:0x0308 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY33_1;                    /*!< Register,  Address offset:0x030C */
	__IO uint32_t DDRPLL_TRACKING_ENTRY34_0;                    /*!< Register,  Address offset:0x0310 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY34_1;                    /*!< Register,  Address offset:0x0314 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY35_0;                    /*!< Register,  Address offset:0x0318 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY35_1;                    /*!< Register,  Address offset:0x031C */
	__IO uint32_t DDRPLL_TRACKING_ENTRY36_0;                    /*!< Register,  Address offset:0x0320 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY36_1;                    /*!< Register,  Address offset:0x0324 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY37_0;                    /*!< Register,  Address offset:0x0328 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY37_1;                    /*!< Register,  Address offset:0x032C */
	__IO uint32_t DDRPLL_TRACKING_ENTRY38_0;                    /*!< Register,  Address offset:0x0330 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY38_1;                    /*!< Register,  Address offset:0x0334 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY39_0;                    /*!< Register,  Address offset:0x0338 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY39_1;                    /*!< Register,  Address offset:0x033C */
	__IO uint32_t DDRPLL_TRACKING_ENTRY40_0;                    /*!< Register,  Address offset:0x0340 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY40_1;                    /*!< Register,  Address offset:0x0344 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY41_0;                    /*!< Register,  Address offset:0x0348 */
	__IO uint32_t DDRPLL_TRACKING_ENTRY41_1;                    /*!< Register,  Address offset:0x034C */
	__IO uint32_t DDRPLL_TRACKING_CTRL0;                        /*!< Register,  Address offset:0x0350 */
	__IO uint32_t DDRPLL_TRACKING_CTRL1;                        /*!< Register,  Address offset:0x0354 */
} CORE_S_TypeDef;

#define CORE_S_BASE ((CORE_S_TypeDef *) (0x60250000))
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
#endif  // SYSREG_CORE_S_H
