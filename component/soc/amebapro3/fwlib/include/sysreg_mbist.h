/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_MBIST_H
#define SYSREG_MBIST_H

/* AUTO_GEN_START */

/* MBISTCTRL is a part of OTPC */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup MBISTCTRL
* @brief MBISTCTRL driver modules
* @{
*/


/** @defgroup REG_MBIST_RSTN_RAM_0
 * @brief
 * @{
 */
#define MBISTCTRL_BIT_MBIST_PKE_TMEM_SRAM           ((u32)0x00000001 << 31)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_PKE_MMEM_SRAM           ((u32)0x00000001 << 30)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_USBOTG                  ((u32)0x00000001 << 29)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SDMMC_PPBECC            ((u32)0x00000001 << 28)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SDMMC_DMASB1            ((u32)0x00000001 << 27)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SDIO                    ((u32)0x00000001 << 26)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_GMAC_SRAM               ((u32)0x00000001 << 25)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_TSC_GROUP00             ((u32)0x00000001 << 24)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_GE_ZOOML                ((u32)0x00000001 << 23)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_GE_ZOOM0                ((u32)0x00000001 << 22)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_GE_BT1                  ((u32)0x00000001 << 21)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_GE_BT0                  ((u32)0x00000001 << 20)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP20              ((u32)0x00000001 << 19)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP19              ((u32)0x00000001 << 18)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP18              ((u32)0x00000001 << 17)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP17              ((u32)0x00000001 << 16)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP16              ((u32)0x00000001 << 15)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP15              ((u32)0x00000001 << 14)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP14              ((u32)0x00000001 << 13)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP13              ((u32)0x00000001 << 12)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP12              ((u32)0x00000001 << 11)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUPL1              ((u32)0x00000001 << 10)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP10              ((u32)0x00000001 << 9)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP09              ((u32)0x00000001 << 8)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP08              ((u32)0x00000001 << 7)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP07              ((u32)0x00000001 << 6)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP06              ((u32)0x00000001 << 5)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP05              ((u32)0x00000001 << 4)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP04              ((u32)0x00000001 << 3)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP03              ((u32)0x00000001 << 2)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP02              ((u32)0x00000001 << 1)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP01              ((u32)0x00000001 << 0)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_RSTN_RAM_1
 * @brief
 * @{
 */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP08             ((u32)0x00000001 << 31)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP07             ((u32)0x00000001 << 30)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP06             ((u32)0x00000001 << 29)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENCGROUP05              ((u32)0x00000001 << 28)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP04             ((u32)0x00000001 << 27)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENCGROUP03              ((u32)0x00000001 << 26)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP02             ((u32)0x00000001 << 25)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENCGROUP01              ((u32)0x00000001 << 24)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP00             ((u32)0x00000001 << 23)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISPGROUP_AID_WRAPPER    ((u32)0x00000001 << 22)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_MPRX          ((u32)0x00000001 << 21)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_VIDEO_AMBAL   ((u32)0x00000001 << 20)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_VIDEO_AMBA0   ((u32)0x00000001 << 19)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISPGROUP_ISP_OSD        ((u32)0x00000001 << 18)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISP_DMA       ((u32)0x00000001 << 17)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ZOOM_UP       ((u32)0x00000001 << 16)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ZOOM_DOWN     ((u32)0x00000001 << 15)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPK          ((u32)0x00000001 << 14)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPL          ((u32)0x00000001 << 13)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPJ          ((u32)0x00000001 << 12)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPH          ((u32)0x00000001 << 11)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPD          ((u32)0x00000001 << 10)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPF          ((u32)0x00000001 << 9)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPE          ((u32)0x00000001 << 8)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPC          ((u32)0x00000001 << 7)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPG          ((u32)0x00000001 << 6)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPB          ((u32)0x00000001 << 5)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPA          ((u32)0x00000001 << 4)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM0_FW_DCACHE_TAG_SRAM  ((u32)0x00000001 << 3)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM0_FW_DCACHE_SRAM      ((u32)0x00000001 << 2)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM0_FW_ICACHE_TAG_SRAM  ((u32)0x00000001 << 1)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM0_FW_ICACHE_SRAM      ((u32)0x00000001 << 0)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_RSTN_RAM_2
 * @brief
 * @{
 */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_DCACHE_TAG  ((u32)0x00000001 << 31)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_DCACHE_DATA ((u32)0x00000001 << 30)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_ICACHE_TAG  ((u32)0x00000001 << 29)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_ICACHE_DATA ((u32)0x00000001 << 28)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP36             ((u32)0x00000001 << 27)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP35             ((u32)0x00000001 << 26)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP34             ((u32)0x00000001 << 25)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP33             ((u32)0x00000001 << 24)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENCGROUP32              ((u32)0x00000001 << 23)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP31             ((u32)0x00000001 << 22)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP30             ((u32)0x00000001 << 21)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP29             ((u32)0x00000001 << 20)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP28             ((u32)0x00000001 << 19)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP27             ((u32)0x00000001 << 18)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP26             ((u32)0x00000001 << 17)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP25             ((u32)0x00000001 << 16)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP24             ((u32)0x00000001 << 15)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP23             ((u32)0x00000001 << 14)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP22             ((u32)0x00000001 << 13)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP21             ((u32)0x00000001 << 12)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP20             ((u32)0x00000001 << 11)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP19             ((u32)0x00000001 << 10)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP18             ((u32)0x00000001 << 9)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP17             ((u32)0x00000001 << 8)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP16             ((u32)0x00000001 << 7)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP15             ((u32)0x00000001 << 6)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP14             ((u32)0x00000001 << 5)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP13             ((u32)0x00000001 << 4)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENCGROUP12              ((u32)0x00000001 << 3)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUPLL             ((u32)0x00000001 << 2)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP10             ((u32)0x00000001 << 1)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP09             ((u32)0x00000001 << 0)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_RSTN_RAM_3
 * @brief
 * @{
 */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP03            ((u32)0x00000001 << 31)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP02            ((u32)0x00000001 << 30)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP01            ((u32)0x00000001 << 29)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NANDECC_SRAM            ((u32)0x00000001 << 28)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_VIDEO_SRAM              ((u32)0x00000001 << 27)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SYSTEM_SRAM             ((u32)0x00000001 << 26)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_SHARE_SRAM           ((u32)0x00000001 << 25)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_CPU                  ((u32)0x00000001 << 24)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_07                   ((u32)0x00000001 << 23)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_06                   ((u32)0x00000001 << 22)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT05                    ((u32)0x00000001 << 21)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_04                   ((u32)0x00000001 << 20)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_03                   ((u32)0x00000001 << 19)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_02                   ((u32)0x00000001 << 18)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPON_E0          ((u32)0x00000001 << 17)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_TPRAMX_WPOFF_E5         ((u32)0x00000001 << 16)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPOFF_E4         ((u32)0x00000001 << 15)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPOFF_E3         ((u32)0x00000001 << 14)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_TPRAMX_WPOFF_E2         ((u32)0x00000001 << 13)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPOFF_EL         ((u32)0x00000001 << 12)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPOFF_E0         ((u32)0x00000001 << 11)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WLK4_E0          ((u32)0x00000001 << 10)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_DDRC_1P_SRAM            ((u32)0x00000001 << 9)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_DDRC_2P_SRAM            ((u32)0x00000001 << 8)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_DTCMDATA      ((u32)0x00000001 << 7)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_ITCM_DATA     ((u32)0x00000001 << 6)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_DCACHE_TAG    ((u32)0x00000001 << 5)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_DCACHE_DATA   ((u32)0x00000001 << 4)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_ICACHETAG     ((u32)0x00000001 << 3)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_ICACHE_DATA   ((u32)0x00000001 << 2)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_DTCM_DATA   ((u32)0x00000001 << 1)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_ITCM_DATA   ((u32)0x00000001 << 0)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_RSTN_RAM_4
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED                     ((u32)0x00007FFF << 17)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED(x)                       ((u32)(((x) & 0x00007FFF) << 17))
#define MBISTCTRL_GET_RESERVED(x)                   ((u32)(((x >> 17) & 0x00007FFF)))
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP15            ((u32)0x00000001 << 16)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP14            ((u32)0x00000001 << 15)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP13            ((u32)0x00000001 << 14)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP12            ((u32)0x00000001 << 13)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP11            ((u32)0x00000001 << 12)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP10            ((u32)0x00000001 << 11)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP09            ((u32)0x00000001 << 10)                  /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP08            ((u32)0x00000001 << 9)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP07            ((u32)0x00000001 << 3)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP06            ((u32)0x00000001 << 2)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP05            ((u32)0x00000001 << 1)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP04            ((u32)0x00000001 << 0)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_RSTN_ROM
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_0B4                 ((u32)0x1FFFFFFF << 3)                   /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_0B4(x)                   ((u32)(((x) & 0x1FFFFFFF) << 3))
#define MBISTCTRL_GET_RESERVED_0B4(x)               ((u32)(((x >> 3) & 0x1FFFFFFF)))
#define MBISTCTRL_BIT_MBIST_BT_01                   ((u32)0x00000001 << 2)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_ITCM_ROM0   ((u32)0x00000001 << 1)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
#define MBISTCTRL_BIT_MBIST_PKE_IROM                ((u32)0x00000001 << 0)                   /*!< R/W   0x0
                                                                                              * 1: release rstn for mbist logic, enable this mbist group
                                                                                              * 0: keep this mbist group logic in reset
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_CTRL_0
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_0B8                 ((u32)0x0003FFFF << 14)                  /*!< --    0x0 None */
#define MBISTCTRL_RESERVED_0B8(x)                   ((u32)(((x) & 0x0003FFFF) << 14))
#define MBISTCTRL_GET_RESERVED_0B8(x)               ((u32)(((x >> 14) & 0x0003FFFF)))
#define MBISTCTRL_BIT_SECOND_RUN_EN                 ((u32)0x00000001 << 13)                  /*!< R/W   0x0
                                                                                              * recond run enale register
                                                                                              * 1: enable the second run of repair
                                                                                              * 0: not enble the second run 
                                                                                              */
#define MBISTCTRL_BIT_REMAP_RSTN                    ((u32)0x00000001 << 12)                  /*!< R/W   0x0
                                                                                              * reset remapping register
                                                                                              * 1: not reset remapping register 
                                                                                              * 0: reset remapping register 
                                                                                              */
#define MBISTCTRL_BIT_PWR_RSTN                      ((u32)0x00000001 << 11)                  /*!< R/W   0x0
                                                                                              * power on reset for repair groups
                                                                                              * 1: release PWR_RSTN
                                                                                              * 0: reset
                                                                                              */
#define MBISTCTRL_MASK_ROM_RPT_SEL                  ((u32)0x0000000F << 7)                   /*!< R/W   0x0
                                                                                              * select the BISR from the rom cell:
                                                                                              * //=========mbist_wlmcu_irom============
                                                                                              * 5'b00000 : WLAN_rom_32768X32M32F80
                                                                                              * //=========mbist_bt_01================
                                                                                              * 5'b00001: rom32kx32_0
                                                                                              * 5'b00010:rom32kx32_1
                                                                                              * 5'b00011:rom8kx32
                                                                                              * //=========mbist_cpu_dtcm_rom==========
                                                                                              * 5'b00100:tm9_rom_2048x64_0
                                                                                              * 5'b00101:tm9_rom_2048x64_1
                                                                                              * 5'b00110:tm9_rom_2048x64_2
                                                                                              * 5'b00111:tm9_rom_2048x64_3
                                                                                              * 5'b01000:tm9_rom_2048x64_4
                                                                                              * //=========mbist_cpu_itcm_rom==========
                                                                                              * 5'b01001:tm9_rom_4096x64
                                                                                              * //=========mbist_voemcu_dtcm_rom======
                                                                                              * 5'b01010:VOE_rom_4096x32
                                                                                              * //========mbist _voemcu_itcm_rom=======
                                                                                              * 5'b01011:VOE_rom_256x32m8F250
                                                                                              * //========mbist_platform_rom_grp========
                                                                                              * 5'b01100:platform_rom_16384x64_0
                                                                                              * 5'b01101:platform_rom_16384x64_1
                                                                                              * 5'b01110:platform_rom_16384x64_2
                                                                                              * 5'b01111:platform_rom_16384x64_3
                                                                                              * 5'b10000:platform_rom_16384x64_4
                                                                                              * 5'b10001:platform_rom_16384x64_5
                                                                                              * 
                                                                                              */
#define MBISTCTRL_ROM_RPT_SEL(x)                    ((u32)(((x) & 0x0000000F) << 7))
#define MBISTCTRL_GET_ROM_RPT_SEL(x)                ((u32)(((x >> 7) & 0x0000000F)))
#define MBISTCTRL_BIT_BIST_LOOP_MODE                ((u32)0x00000001 << 6)                   /*!< R/W   0x0
                                                                                              * test looping capability
                                                                                              * 1: enable loop_mode
                                                                                              * 0: disable loop_mode
                                                                                              */
#define MBISTCTRL_BIT_DYN_READ_EN                   ((u32)0x00000001 << 5)                   /*!< R/W   0x0
                                                                                              * VDDR test enable
                                                                                              * 1: enable vddr test
                                                                                              * 0: disable vddr test
                                                                                              */
#define MBISTCTRL_BIT_BIST_DRF_RESUME               ((u32)0x00000001 << 4)                   /*!< R/W   0x0
                                                                                              * resume drf test:
                                                                                              * 1: stop waiting and resume drf test
                                                                                              * 0: keep waiting
                                                                                              */
#define MBISTCTRL_BIT_BIST_DRF_MODE                 ((u32)0x00000001 << 3)                   /*!< R/W   0x0
                                                                                              * enter DRF_MBIST mode
                                                                                              * 1: enable drf_mbist test
                                                                                              * 0: disable drf_mbist test
                                                                                              */
#define MBISTCTRL_BIT_BIST_NRML_MODE                ((u32)0x00000001 << 2)                   /*!< R/W   0x0
                                                                                              * enter normal mbist mode
                                                                                              * 1: enable normal mbist test
                                                                                              * 0: disable normal mbist test
                                                                                              */
#define MBISTCTRL_BIT_BIST_RSTN                     ((u32)0x00000001 << 1)                   /*!< R/W   0x0
                                                                                              * eable bist function or repair function
                                                                                              * 1: release mbist reset
                                                                                              * 0: keep in normal mode
                                                                                              */
#define MBISTCTRL_BIT_BIST_CLK_EN_0                 ((u32)0x00000001 << 0)                   /*!< R/W   0x0
                                                                                              * eable bist clock for the mbist group
                                                                                              * 1: enable the bist clock
                                                                                              * 0: keep gating the clock
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_CTRL_1
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_BIST_GRP_EN                  ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0x0
                                                                                              * enable each memory cell in one mbist group
                                                                                              * 1: enable the cell
                                                                                              * 0: disable the memory cell
                                                                                              */
#define MBISTCTRL_BIST_GRP_EN(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define MBISTCTRL_GET_BIST_GRP_EN(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_MBIST_CTRL_2
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_0C0                 ((u32)0xFFFFFFFF << 0)                   /*!< R     0x0 Reserved */
#define MBISTCTRL_RESERVED_0C0(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define MBISTCTRL_GET_RESERVED_0C0(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_MBIST_STATUS
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_0C4                 ((u32)0x07FFFFFF << 5)                   /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_0C4(x)                   ((u32)(((x) & 0x07FFFFFF) << 5))
#define MBISTCTRL_GET_RESERVED_0C4(x)               ((u32)(((x >> 5) & 0x07FFFFFF)))
#define MBISTCTRL_BIT_MBIST_START_PAUSE             ((u32)0x00000001 << 4)                   /*!< R     0x1
                                                                                              * indicate whether the drf test is paused
                                                                                              * 1: test is paused and wait for the next pattern
                                                                                              * 0: ends waiting and continue the test
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NRML_DONE               ((u32)0x00000001 << 3)                   /*!< R     0x1
                                                                                              * indicate whether the normal mbist test is done
                                                                                              * 1: normal_test is done
                                                                                              * 0: normal_test is not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NRML_FAIL               ((u32)0x00000001 << 2)                   /*!< R     0x0
                                                                                              * indicate whether the normal test fail
                                                                                              * 1: normal_test fail
                                                                                              * 0: normal_test not fail
                                                                                              */
#define MBISTCTRL_BIT_MBIST_DRF_DONE                ((u32)0x00000001 << 1)                   /*!< R     0x1
                                                                                              * indicate whether the drf_mbist test is done
                                                                                              * 1: drf_mbist test is done
                                                                                              * 0: drf_mbist test is not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_DRF_FAIL                ((u32)0x00000001 << 0)                   /*!< R     0x0
                                                                                              * indicate whether the drf_mbist test fail
                                                                                              * 1: drf_mbist test fail
                                                                                              * 0: drf_mbist test not fail
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_DONE_RAM_0
 * @brief
 * @{
 */
#define MBISTCTRL_BIT_MBIST_PKE_TMEM_SRAM_0C8       ((u32)0x00000001 << 31)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_PKE_MMEM_SRAM_0C8       ((u32)0x00000001 << 30)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_USBOTG_0C8              ((u32)0x00000001 << 29)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SDMMC_PPBECC_0C8        ((u32)0x00000001 << 28)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SDMMC_DMASB1_0C8        ((u32)0x00000001 << 27)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SDIO_0C8                ((u32)0x00000001 << 26)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_GMAC_SRAM_0C8           ((u32)0x00000001 << 25)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_TSC_GROUP00_0C8         ((u32)0x00000001 << 24)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_GE_ZOOML_0C8            ((u32)0x00000001 << 23)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_GE_ZOOM0_0C8            ((u32)0x00000001 << 22)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_GE_BT1_0C8              ((u32)0x00000001 << 21)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_GE_BT0_0C8              ((u32)0x00000001 << 20)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP20_0C8          ((u32)0x00000001 << 19)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP19_0C8          ((u32)0x00000001 << 18)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP18_0C8          ((u32)0x00000001 << 17)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP17_0C8          ((u32)0x00000001 << 16)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP16_0C8          ((u32)0x00000001 << 15)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP15_0C8          ((u32)0x00000001 << 14)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP14_0C8          ((u32)0x00000001 << 13)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP13_0C8          ((u32)0x00000001 << 12)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP12_0C8          ((u32)0x00000001 << 11)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUPL1_0C8          ((u32)0x00000001 << 10)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP10_0C8          ((u32)0x00000001 << 9)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP09_0C8          ((u32)0x00000001 << 8)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP08_0C8          ((u32)0x00000001 << 7)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP07_0C8          ((u32)0x00000001 << 6)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP06_0C8          ((u32)0x00000001 << 5)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP05_0C8          ((u32)0x00000001 << 4)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP04_0C8          ((u32)0x00000001 << 3)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP03_0C8          ((u32)0x00000001 << 2)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP02_0C8          ((u32)0x00000001 << 1)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP01_0C8          ((u32)0x00000001 << 0)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_DONE_RAM_1
 * @brief
 * @{
 */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP08_0CC         ((u32)0x00000001 << 31)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP07_0CC         ((u32)0x00000001 << 30)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP06_0CC         ((u32)0x00000001 << 29)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENCGROUP05_0CC          ((u32)0x00000001 << 28)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP04_0CC         ((u32)0x00000001 << 27)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENCGROUP03_0CC          ((u32)0x00000001 << 26)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP02_0CC         ((u32)0x00000001 << 25)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENCGROUP01_0CC          ((u32)0x00000001 << 24)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP00_0CC         ((u32)0x00000001 << 23)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISPGROUP_AID_WRAPPER_0CC ((u32)0x00000001 << 22)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_MPRX_0CC      ((u32)0x00000001 << 21)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_VIDEO_AMBAL_0CC ((u32)0x00000001 << 20)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_VIDEO_AMBA0_0CC ((u32)0x00000001 << 19)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISPGROUP_ISP_OSD_0CC    ((u32)0x00000001 << 18)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISP_DMA_0CC   ((u32)0x00000001 << 17)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ZOOM_UP_0CC   ((u32)0x00000001 << 16)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ZOOM_DOWN_0CC ((u32)0x00000001 << 15)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPK_0CC      ((u32)0x00000001 << 14)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPL_0CC      ((u32)0x00000001 << 13)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPJ_0CC      ((u32)0x00000001 << 12)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPH_0CC      ((u32)0x00000001 << 11)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPD_0CC      ((u32)0x00000001 << 10)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPF_0CC      ((u32)0x00000001 << 9)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPE_0CC      ((u32)0x00000001 << 8)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPC_0CC      ((u32)0x00000001 << 7)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPG_0CC      ((u32)0x00000001 << 6)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPB_0CC      ((u32)0x00000001 << 5)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPA_0CC      ((u32)0x00000001 << 4)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM0_FW_DCACHE_TAG_SRAM_0CC ((u32)0x00000001 << 3)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM0_FW_DCACHE_SRAM_0CC  ((u32)0x00000001 << 2)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM0_FW_ICACHE_TAG_SRAM_0CC ((u32)0x00000001 << 1)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM0_FW_ICACHE_SRAM_0CC  ((u32)0x00000001 << 0)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_DONE_RAM_2
 * @brief
 * @{
 */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_DCACHE_TAG_0D0 ((u32)0x00000001 << 31)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_DCACHE_DATA_0D0 ((u32)0x00000001 << 30)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_ICACHE_TAG_0D0 ((u32)0x00000001 << 29)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_ICACHE_DATA_0D0 ((u32)0x00000001 << 28)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP36_0D0         ((u32)0x00000001 << 27)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP35_0D0         ((u32)0x00000001 << 26)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP34_0D0         ((u32)0x00000001 << 25)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP33_0D0         ((u32)0x00000001 << 24)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENCGROUP32_0D0          ((u32)0x00000001 << 23)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP31_0D0         ((u32)0x00000001 << 22)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP30_0D0         ((u32)0x00000001 << 21)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP29_0D0         ((u32)0x00000001 << 20)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP28_0D0         ((u32)0x00000001 << 19)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP27_0D0         ((u32)0x00000001 << 18)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP26_0D0         ((u32)0x00000001 << 17)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP25_0D0         ((u32)0x00000001 << 16)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP24_0D0         ((u32)0x00000001 << 15)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP23_0D0         ((u32)0x00000001 << 14)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP22_0D0         ((u32)0x00000001 << 13)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP21_0D0         ((u32)0x00000001 << 12)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP20_0D0         ((u32)0x00000001 << 11)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP19_0D0         ((u32)0x00000001 << 10)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP18_0D0         ((u32)0x00000001 << 9)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP17_0D0         ((u32)0x00000001 << 8)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP16_0D0         ((u32)0x00000001 << 7)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP15_0D0         ((u32)0x00000001 << 6)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP14_0D0         ((u32)0x00000001 << 5)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP13_0D0         ((u32)0x00000001 << 4)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENCGROUP12_0D0          ((u32)0x00000001 << 3)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUPLL_0D0         ((u32)0x00000001 << 2)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP10_0D0         ((u32)0x00000001 << 1)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP09_0D0         ((u32)0x00000001 << 0)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_DONE_RAM_3
 * @brief
 * @{
 */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP03_0D4        ((u32)0x00000001 << 31)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP02_0D4        ((u32)0x00000001 << 30)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP01_0D4        ((u32)0x00000001 << 29)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NANDECC_SRAM_0D4        ((u32)0x00000001 << 28)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_VIDEO_SRAM_0D4          ((u32)0x00000001 << 27)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SYSTEM_SRAM_0D4         ((u32)0x00000001 << 26)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_SHARE_SRAM_0D4       ((u32)0x00000001 << 25)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_CPU_0D4              ((u32)0x00000001 << 24)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_07_0D4               ((u32)0x00000001 << 23)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_06_0D4               ((u32)0x00000001 << 22)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT05_0D4                ((u32)0x00000001 << 21)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_04_0D4               ((u32)0x00000001 << 20)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_03_0D4               ((u32)0x00000001 << 19)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_02_0D4               ((u32)0x00000001 << 18)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPON_E0_0D4      ((u32)0x00000001 << 17)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_TPRAMX_WPOFF_E5_0D4     ((u32)0x00000001 << 16)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPOFF_E4_0D4     ((u32)0x00000001 << 15)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPOFF_E3_0D4     ((u32)0x00000001 << 14)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_TPRAMX_WPOFF_E2_0D4     ((u32)0x00000001 << 13)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPOFF_EL_0D4     ((u32)0x00000001 << 12)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPOFF_E0_0D4     ((u32)0x00000001 << 11)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WLK4_E0_0D4      ((u32)0x00000001 << 10)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_DDRC_1P_SRAM_0D4        ((u32)0x00000001 << 9)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_DDRC_2P_SRAM_0D4        ((u32)0x00000001 << 8)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_DTCMDATA_0D4  ((u32)0x00000001 << 7)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_ITCM_DATA_0D4 ((u32)0x00000001 << 6)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_DCACHE_TAG_0D4 ((u32)0x00000001 << 5)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_DCACHE_DATA_0D4 ((u32)0x00000001 << 4)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_ICACHETAG_0D4 ((u32)0x00000001 << 3)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_ICACHE_DATA_0D4 ((u32)0x00000001 << 2)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_DTCM_DATA_0D4 ((u32)0x00000001 << 1)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_ITCM_DATA_0D4 ((u32)0x00000001 << 0)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_DONE_RAM_4
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_0D8                 ((u32)0x00007FFF << 17)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_0D8(x)                   ((u32)(((x) & 0x00007FFF) << 17))
#define MBISTCTRL_GET_RESERVED_0D8(x)               ((u32)(((x >> 17) & 0x00007FFF)))
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP15_0D8        ((u32)0x00000001 << 16)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP14_0D8        ((u32)0x00000001 << 15)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP13_0D8        ((u32)0x00000001 << 14)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP12_0D8        ((u32)0x00000001 << 13)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP11_0D8        ((u32)0x00000001 << 12)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP10_0D8        ((u32)0x00000001 << 11)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP09_0D8        ((u32)0x00000001 << 10)                  /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP08_0D8        ((u32)0x00000001 << 9)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP07_0D8        ((u32)0x00000001 << 3)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP06_0D8        ((u32)0x00000001 << 2)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP05_0D8        ((u32)0x00000001 << 1)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP04_0D8        ((u32)0x00000001 << 0)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_DONE_ROM
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_0DC                 ((u32)0x1FFFFFFF << 3)                   /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_0DC(x)                   ((u32)(((x) & 0x1FFFFFFF) << 3))
#define MBISTCTRL_GET_RESERVED_0DC(x)               ((u32)(((x >> 3) & 0x1FFFFFFF)))
#define MBISTCTRL_BIT_MBIST_BT_01_0DC               ((u32)0x00000001 << 2)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_ITCM_ROM0_0DC ((u32)0x00000001 << 1)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
#define MBISTCTRL_BIT_MBIST_PKE_IROM_0DC            ((u32)0x00000001 << 0)                   /*!< R     0x0
                                                                                              * 1: bist done for this mbist group
                                                                                              * 0: bist not done
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_0
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_0E0                 ((u32)0x00000003 << 30)                  /*!< --    --  None */
#define MBISTCTRL_RESERVED_0E0(x)                   ((u32)(((x) & 0x00000003) << 30))
#define MBISTCTRL_GET_RESERVED_0E0(x)               ((u32)(((x >> 30) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP08_0E0         ((u32)0x0000000F << 26)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP08_0E0(x)           ((u32)(((x) & 0x0000000F) << 26))
#define MBISTCTRL_GET_MBIST_NN_GROUP08_0E0(x)       ((u32)(((x >> 26) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP07_0E0         ((u32)0x00000003 << 24)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP07_0E0(x)           ((u32)(((x) & 0x00000003) << 24))
#define MBISTCTRL_GET_MBIST_NN_GROUP07_0E0(x)       ((u32)(((x >> 24) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP06_0E0         ((u32)0x0000001F << 19)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP06_0E0(x)           ((u32)(((x) & 0x0000001F) << 19))
#define MBISTCTRL_GET_MBIST_NN_GROUP06_0E0(x)       ((u32)(((x >> 19) & 0x0000001F)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP05_0E0         ((u32)0x0000000F << 15)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP05_0E0(x)           ((u32)(((x) & 0x0000000F) << 15))
#define MBISTCTRL_GET_MBIST_NN_GROUP05_0E0(x)       ((u32)(((x >> 15) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP04_0E0         ((u32)0x00000003 << 13)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP04_0E0(x)           ((u32)(((x) & 0x00000003) << 13))
#define MBISTCTRL_GET_MBIST_NN_GROUP04_0E0(x)       ((u32)(((x >> 13) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP03_0E0         ((u32)0x000000FF << 5)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP03_0E0(x)           ((u32)(((x) & 0x000000FF) << 5))
#define MBISTCTRL_GET_MBIST_NN_GROUP03_0E0(x)       ((u32)(((x >> 5) & 0x000000FF)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP02_0E0         ((u32)0x00000003 << 3)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP02_0E0(x)           ((u32)(((x) & 0x00000003) << 3))
#define MBISTCTRL_GET_MBIST_NN_GROUP02_0E0(x)       ((u32)(((x >> 3) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP01_0E0         ((u32)0x00000007 << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP01_0E0(x)           ((u32)(((x) & 0x00000007) << 0))
#define MBISTCTRL_GET_MBIST_NN_GROUP01_0E0(x)       ((u32)(((x >> 0) & 0x00000007)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_1
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_MBIST_NN_GROUP12_0E4         ((u32)0x000003FF << 22)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP12_0E4(x)           ((u32)(((x) & 0x000003FF) << 22))
#define MBISTCTRL_GET_MBIST_NN_GROUP12_0E4(x)       ((u32)(((x >> 22) & 0x000003FF)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP11             ((u32)0x00000FFF << 10)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP11(x)               ((u32)(((x) & 0x00000FFF) << 10))
#define MBISTCTRL_GET_MBIST_NN_GROUP11(x)           ((u32)(((x >> 10) & 0x00000FFF)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP10_0E4         ((u32)0x00000007 << 7)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP10_0E4(x)           ((u32)(((x) & 0x00000007) << 7))
#define MBISTCTRL_GET_MBIST_NN_GROUP10_0E4(x)       ((u32)(((x >> 7) & 0x00000007)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP09_0E4         ((u32)0x0000007F << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP09_0E4(x)           ((u32)(((x) & 0x0000007F) << 0))
#define MBISTCTRL_GET_MBIST_NN_GROUP09_0E4(x)       ((u32)(((x >> 0) & 0x0000007F)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_2
 * @brief
 * @{
 */
#define MBISTCTRL_BIT_RESERVED_0E8                  ((u32)0x00000001 << 31)                  /*!< R     --  None */
#define MBISTCTRL_MASK_MBIST_NN_GROUP18_0E8         ((u32)0x0000007F << 24)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP18_0E8(x)           ((u32)(((x) & 0x0000007F) << 24))
#define MBISTCTRL_GET_MBIST_NN_GROUP18_0E8(x)       ((u32)(((x >> 24) & 0x0000007F)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP17_0E8         ((u32)0x00000003 << 22)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP17_0E8(x)           ((u32)(((x) & 0x00000003) << 22))
#define MBISTCTRL_GET_MBIST_NN_GROUP17_0E8(x)       ((u32)(((x >> 22) & 0x00000003)))
#define MBISTCTRL_BIT_MBIST_NN_GROUP16_0E8          ((u32)0x00000001 << 21)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_NN_GROUP15_0E8         ((u32)0x00000007 << 18)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP15_0E8(x)           ((u32)(((x) & 0x00000007) << 18))
#define MBISTCTRL_GET_MBIST_NN_GROUP15_0E8(x)       ((u32)(((x >> 18) & 0x00000007)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP14_0E8         ((u32)0x000000FF << 10)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP14_0E8(x)           ((u32)(((x) & 0x000000FF) << 10))
#define MBISTCTRL_GET_MBIST_NN_GROUP14_0E8(x)       ((u32)(((x >> 10) & 0x000000FF)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP13_0E8         ((u32)0x000003FF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP13_0E8(x)           ((u32)(((x) & 0x000003FF) << 0))
#define MBISTCTRL_GET_MBIST_NN_GROUP13_0E8(x)       ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_3
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_0EC                 ((u32)0x0000001F << 27)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_0EC(x)                   ((u32)(((x) & 0x0000001F) << 27))
#define MBISTCTRL_GET_RESERVED_0EC(x)               ((u32)(((x >> 27) & 0x0000001F)))
#define MBISTCTRL_MASK_MBIST_GE_BT1_0EC             ((u32)0x000000FF << 19)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_GE_BT1_0EC(x)               ((u32)(((x) & 0x000000FF) << 19))
#define MBISTCTRL_GET_MBIST_GE_BT1_0EC(x)           ((u32)(((x >> 19) & 0x000000FF)))
#define MBISTCTRL_MASK_MBIST_GE_BT0_0EC             ((u32)0x0000001F << 14)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_GE_BT0_0EC(x)               ((u32)(((x) & 0x0000001F) << 14))
#define MBISTCTRL_GET_MBIST_GE_BT0_0EC(x)           ((u32)(((x >> 14) & 0x0000001F)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP20_0EC         ((u32)0x0000007F << 7)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP20_0EC(x)           ((u32)(((x) & 0x0000007F) << 7))
#define MBISTCTRL_GET_MBIST_NN_GROUP20_0EC(x)       ((u32)(((x >> 7) & 0x0000007F)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP19_0EC         ((u32)0x0000007F << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP19_0EC(x)           ((u32)(((x) & 0x0000007F) << 0))
#define MBISTCTRL_GET_MBIST_NN_GROUP19_0EC(x)       ((u32)(((x >> 0) & 0x0000007F)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_4
 * @brief
 * @{
 */
#define MBISTCTRL_BIT_MBIST_KM0_FW_ICACHE_SRAM_0F0  ((u32)0x00000001 << 31)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_PKE_TMEM_SRAM_0F0       ((u32)0x00000001 << 30)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_PKE_MMEM_SRAM_0F0       ((u32)0x00000001 << 29)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_USBOTG_0F0              ((u32)0x00000001 << 28)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_SDMMC_PPBECC_0F0       ((u32)0x00000003 << 26)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_SDMMC_PPBECC_0F0(x)         ((u32)(((x) & 0x00000003) << 26))
#define MBISTCTRL_GET_MBIST_SDMMC_PPBECC_0F0(x)     ((u32)(((x >> 26) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_SDMMC_DMASBL           ((u32)0x00000003 << 24)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_SDMMC_DMASBL(x)             ((u32)(((x) & 0x00000003) << 24))
#define MBISTCTRL_GET_MBIST_SDMMC_DMASBL(x)         ((u32)(((x >> 24) & 0x00000003)))
#define MBISTCTRL_BIT_MBIST_SDIO_0F0                ((u32)0x00000001 << 23)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_GMAC_SRAM_0F0          ((u32)0x00000003 << 21)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_GMAC_SRAM_0F0(x)            ((u32)(((x) & 0x00000003) << 21))
#define MBISTCTRL_GET_MBIST_GMAC_SRAM_0F0(x)        ((u32)(((x >> 21) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_TSC_GROUP00_0F0        ((u32)0x000003FF << 11)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_TSC_GROUP00_0F0(x)          ((u32)(((x) & 0x000003FF) << 11))
#define MBISTCTRL_GET_MBIST_TSC_GROUP00_0F0(x)      ((u32)(((x >> 11) & 0x000003FF)))
#define MBISTCTRL_MASK_MBIST_GE_ZOOML_0F0           ((u32)0x0000001F << 6)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_GE_ZOOML_0F0(x)             ((u32)(((x) & 0x0000001F) << 6))
#define MBISTCTRL_GET_MBIST_GE_ZOOML_0F0(x)         ((u32)(((x >> 6) & 0x0000001F)))
#define MBISTCTRL_MASK_MBIST_GE_ZOOM0_0F0           ((u32)0x0000003F << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_GE_ZOOM0_0F0(x)             ((u32)(((x) & 0x0000003F) << 0))
#define MBISTCTRL_GET_MBIST_GE_ZOOM0_0F0(x)         ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_5
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_0F4                 ((u32)0x0000000F << 28)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_0F4(x)                   ((u32)(((x) & 0x0000000F) << 28))
#define MBISTCTRL_GET_RESERVED_0F4(x)               ((u32)(((x >> 28) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISPB_0F4     ((u32)0x00007FFF << 13)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISPB_0F4(x)       ((u32)(((x) & 0x00007FFF) << 13))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISPB_0F4(x)   ((u32)(((x >> 13) & 0x00007FFF)))
#define MBISTCTRL_MASK_MBIST_ISPGROUP_ISPA          ((u32)0x000003FF << 3)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISPGROUP_ISPA(x)            ((u32)(((x) & 0x000003FF) << 3))
#define MBISTCTRL_GET_MBIST_ISPGROUP_ISPA(x)        ((u32)(((x >> 3) & 0x000003FF)))
#define MBISTCTRL_BIT_MBIST_KM0_FW_DCACHE_TAG_SRAM_0F4 ((u32)0x00000001 << 2)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM0_FW_DCACHE_SRAM_0F4  ((u32)0x00000001 << 1)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM0_FW_ICACHE_TAG_SRAM_0F4 ((u32)0x00000001 << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_6
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISPE_0F8     ((u32)0x00000FFF << 18)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISPE_0F8(x)       ((u32)(((x) & 0x00000FFF) << 18))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISPE_0F8(x)   ((u32)(((x >> 18) & 0x00000FFF)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISPC_0F8     ((u32)0x0000001F << 13)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISPC_0F8(x)       ((u32)(((x) & 0x0000001F) << 13))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISPC_0F8(x)   ((u32)(((x >> 13) & 0x0000001F)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISPG_0F8     ((u32)0x00001FFF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISPG_0F8(x)       ((u32)(((x) & 0x00001FFF) << 0))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISPG_0F8(x)   ((u32)(((x >> 0) & 0x00001FFF)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_7
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISPH_0FC     ((u32)0x0000007F << 16)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISPH_0FC(x)       ((u32)(((x) & 0x0000007F) << 16))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISPH_0FC(x)   ((u32)(((x >> 16) & 0x0000007F)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISPD_0FC     ((u32)0x0000000F << 12)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISPD_0FC(x)       ((u32)(((x) & 0x0000000F) << 12))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISPD_0FC(x)   ((u32)(((x >> 12) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISPF_0FC     ((u32)0x00000FFF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISPF_0FC(x)       ((u32)(((x) & 0x00000FFF) << 0))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISPF_0FC(x)   ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_8
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_100                 ((u32)0x0000000F << 28)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_100(x)                   ((u32)(((x) & 0x0000000F) << 28))
#define MBISTCTRL_GET_RESERVED_100(x)               ((u32)(((x >> 28) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISPL_100     ((u32)0x00003FFF << 14)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISPL_100(x)       ((u32)(((x) & 0x00003FFF) << 14))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISPL_100(x)   ((u32)(((x >> 14) & 0x00003FFF)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISPJ_100     ((u32)0x00003FFF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISPJ_100(x)       ((u32)(((x) & 0x00003FFF) << 0))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISPJ_100(x)   ((u32)(((x >> 0) & 0x00003FFF)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_9
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_104                 ((u32)0xFFFFFFFF << 0)                   /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_104(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define MBISTCTRL_GET_RESERVED_104(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_10
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_108                 ((u32)0x00000003 << 30)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_108(x)                   ((u32)(((x) & 0x00000003) << 30))
#define MBISTCTRL_GET_RESERVED_108(x)               ((u32)(((x >> 30) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_MPRX_108     ((u32)0x00000003 << 28)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_MPRX_108(x)       ((u32)(((x) & 0x00000003) << 28))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_MPRX_108(x)   ((u32)(((x >> 28) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_VIDEO_AMBAL_108 ((u32)0x0000003F << 22)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_VIDEO_AMBAL_108(x) ((u32)(((x) & 0x0000003F) << 22))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_VIDEO_AMBAL_108(x) ((u32)(((x >> 22) & 0x0000003F)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_VIDEO_AMBA0_108 ((u32)0x00000003 << 20)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_VIDEO_AMBA0_108(x) ((u32)(((x) & 0x00000003) << 20))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_VIDEO_AMBA0_108(x) ((u32)(((x >> 20) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISP_OSD      ((u32)0x00000007 << 17)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISP_OSD(x)        ((u32)(((x) & 0x00000007) << 17))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISP_OSD(x)    ((u32)(((x >> 17) & 0x00000007)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISP_DMA_108  ((u32)0x00000007 << 14)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISP_DMA_108(x)    ((u32)(((x) & 0x00000007) << 14))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISP_DMA_108(x) ((u32)(((x >> 14) & 0x00000007)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ZOOM_UP_108  ((u32)0x0000000F << 10)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ZOOM_UP_108(x)    ((u32)(((x) & 0x0000000F) << 10))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ZOOM_UP_108(x) ((u32)(((x >> 10) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ZOOM_DOWN_108 ((u32)0x00000003 << 8)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ZOOM_DOWN_108(x)  ((u32)(((x) & 0x00000003) << 8))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ZOOM_DOWN_108(x) ((u32)(((x >> 8) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISPK_108     ((u32)0x000000FF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISPK_108(x)       ((u32)(((x) & 0x000000FF) << 0))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISPK_108(x)   ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_11
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_10C                 ((u32)0x0000001F << 27)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_10C(x)                   ((u32)(((x) & 0x0000001F) << 27))
#define MBISTCTRL_GET_RESERVED_10C(x)               ((u32)(((x >> 27) & 0x0000001F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP02_10C        ((u32)0x000003FF << 17)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP02_10C(x)          ((u32)(((x) & 0x000003FF) << 17))
#define MBISTCTRL_GET_MBIST_ENC_GROUP02_10C(x)      ((u32)(((x >> 17) & 0x000003FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP01            ((u32)0x0000000F << 13)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP01(x)              ((u32)(((x) & 0x0000000F) << 13))
#define MBISTCTRL_GET_MBIST_ENC_GROUP01(x)          ((u32)(((x >> 13) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP00_10C        ((u32)0x000001FF << 4)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP00_10C(x)          ((u32)(((x) & 0x000001FF) << 4))
#define MBISTCTRL_GET_MBIST_ENC_GROUP00_10C(x)      ((u32)(((x >> 4) & 0x000001FF)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_AID_WRAPPER  ((u32)0x0000000F << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_AID_WRAPPER(x)    ((u32)(((x) & 0x0000000F) << 0))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_AID_WRAPPER(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_12
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_110                 ((u32)0x0000003F << 26)                  /*!< -     nan None */
#define MBISTCTRL_RESERVED_110(x)                   ((u32)(((x) & 0x0000003F) << 26))
#define MBISTCTRL_GET_RESERVED_110(x)               ((u32)(((x >> 26) & 0x0000003F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP05            ((u32)0x000000FF << 18)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP05(x)              ((u32)(((x) & 0x000000FF) << 18))
#define MBISTCTRL_GET_MBIST_ENC_GROUP05(x)          ((u32)(((x >> 18) & 0x000000FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP04_110        ((u32)0x000003FF << 8)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP04_110(x)          ((u32)(((x) & 0x000003FF) << 8))
#define MBISTCTRL_GET_MBIST_ENC_GROUP04_110(x)      ((u32)(((x >> 8) & 0x000003FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP03            ((u32)0x000000FF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP03(x)              ((u32)(((x) & 0x000000FF) << 0))
#define MBISTCTRL_GET_MBIST_ENC_GROUP03(x)          ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_13
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_MBIST_ENC_GROUP10_114        ((u32)0x0000000F << 26)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP10_114(x)          ((u32)(((x) & 0x0000000F) << 26))
#define MBISTCTRL_GET_MBIST_ENC_GROUP10_114(x)      ((u32)(((x >> 26) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP09_114        ((u32)0x00000007 << 23)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP09_114(x)          ((u32)(((x) & 0x00000007) << 23))
#define MBISTCTRL_GET_MBIST_ENC_GROUP09_114(x)      ((u32)(((x >> 23) & 0x00000007)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP08_114        ((u32)0x0000007F << 16)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP08_114(x)          ((u32)(((x) & 0x0000007F) << 16))
#define MBISTCTRL_GET_MBIST_ENC_GROUP08_114(x)      ((u32)(((x >> 16) & 0x0000007F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP07_114        ((u32)0x000001FF << 7)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP07_114(x)          ((u32)(((x) & 0x000001FF) << 7))
#define MBISTCTRL_GET_MBIST_ENC_GROUP07_114(x)      ((u32)(((x >> 7) & 0x000001FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP06_114        ((u32)0x0000007F << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP06_114(x)          ((u32)(((x) & 0x0000007F) << 0))
#define MBISTCTRL_GET_MBIST_ENC_GROUP06_114(x)      ((u32)(((x >> 0) & 0x0000007F)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_14
 * @brief
 * @{
 */
#define MBISTCTRL_BIT_RESERVED_118                  ((u32)0x00000001 << 31)                  /*!< -     nan None */
#define MBISTCTRL_MASK_MBIST_ENC_GROUP14_118        ((u32)0x000000FF << 23)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP14_118(x)          ((u32)(((x) & 0x000000FF) << 23))
#define MBISTCTRL_GET_MBIST_ENC_GROUP14_118(x)      ((u32)(((x >> 23) & 0x000000FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP13_118        ((u32)0x000000FF << 15)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP13_118(x)          ((u32)(((x) & 0x000000FF) << 15))
#define MBISTCTRL_GET_MBIST_ENC_GROUP13_118(x)      ((u32)(((x >> 15) & 0x000000FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP12            ((u32)0x000003FF << 5)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP12(x)              ((u32)(((x) & 0x000003FF) << 5))
#define MBISTCTRL_GET_MBIST_ENC_GROUP12(x)          ((u32)(((x >> 5) & 0x000003FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP11            ((u32)0x0000001F << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP11(x)              ((u32)(((x) & 0x0000001F) << 0))
#define MBISTCTRL_GET_MBIST_ENC_GROUP11(x)          ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_15
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_11C                 ((u32)0x0000007F << 25)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_11C(x)                   ((u32)(((x) & 0x0000007F) << 25))
#define MBISTCTRL_GET_RESERVED_11C(x)               ((u32)(((x >> 25) & 0x0000007F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP17_11C        ((u32)0x000000FF << 17)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP17_11C(x)          ((u32)(((x) & 0x000000FF) << 17))
#define MBISTCTRL_GET_MBIST_ENC_GROUP17_11C(x)      ((u32)(((x >> 17) & 0x000000FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP16_11C        ((u32)0x000000FF << 9)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP16_11C(x)          ((u32)(((x) & 0x000000FF) << 9))
#define MBISTCTRL_GET_MBIST_ENC_GROUP16_11C(x)      ((u32)(((x >> 9) & 0x000000FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP15_11C        ((u32)0x000001FF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP15_11C(x)          ((u32)(((x) & 0x000001FF) << 0))
#define MBISTCTRL_GET_MBIST_ENC_GROUP15_11C(x)      ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_16
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_120                 ((u32)0x0000000F << 28)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_120(x)                   ((u32)(((x) & 0x0000000F) << 28))
#define MBISTCTRL_GET_RESERVED_120(x)               ((u32)(((x >> 28) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP20_120        ((u32)0x000003FF << 18)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP20_120(x)          ((u32)(((x) & 0x000003FF) << 18))
#define MBISTCTRL_GET_MBIST_ENC_GROUP20_120(x)      ((u32)(((x >> 18) & 0x000003FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP19_120        ((u32)0x000003FF << 8)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP19_120(x)          ((u32)(((x) & 0x000003FF) << 8))
#define MBISTCTRL_GET_MBIST_ENC_GROUP19_120(x)      ((u32)(((x >> 8) & 0x000003FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP18_120        ((u32)0x000000FF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP18_120(x)          ((u32)(((x) & 0x000000FF) << 0))
#define MBISTCTRL_GET_MBIST_ENC_GROUP18_120(x)      ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_17
 * @brief
 * @{
 */
#define MBISTCTRL_BIT_RESERVED_124                  ((u32)0x00000001 << 31)                  /*!< R     0x0 None */
#define MBISTCTRL_MASK_MBIST_ENC_GROUP25_124        ((u32)0x000003FF << 21)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP25_124(x)          ((u32)(((x) & 0x000003FF) << 21))
#define MBISTCTRL_GET_MBIST_ENC_GROUP25_124(x)      ((u32)(((x >> 21) & 0x000003FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP24_124        ((u32)0x000001FF << 12)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP24_124(x)          ((u32)(((x) & 0x000001FF) << 12))
#define MBISTCTRL_GET_MBIST_ENC_GROUP24_124(x)      ((u32)(((x >> 12) & 0x000001FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP23_124        ((u32)0x000001FF << 3)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP23_124(x)          ((u32)(((x) & 0x000001FF) << 3))
#define MBISTCTRL_GET_MBIST_ENC_GROUP23_124(x)      ((u32)(((x >> 3) & 0x000001FF)))
#define MBISTCTRL_BIT_MBIST_ENC_GROUP22_124         ((u32)0x00000001 << 2)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_ENC_GROUP21_124        ((u32)0x00000003 << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP21_124(x)          ((u32)(((x) & 0x00000003) << 0))
#define MBISTCTRL_GET_MBIST_ENC_GROUP21_124(x)      ((u32)(((x >> 0) & 0x00000003)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_18
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_128                 ((u32)0x00000003 << 30)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_128(x)                   ((u32)(((x) & 0x00000003) << 30))
#define MBISTCTRL_GET_RESERVED_128(x)               ((u32)(((x >> 30) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP31_128        ((u32)0x0000003F << 24)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP31_128(x)          ((u32)(((x) & 0x0000003F) << 24))
#define MBISTCTRL_GET_MBIST_ENC_GROUP31_128(x)      ((u32)(((x >> 24) & 0x0000003F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP30_128        ((u32)0x0000007F << 17)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP30_128(x)          ((u32)(((x) & 0x0000007F) << 17))
#define MBISTCTRL_GET_MBIST_ENC_GROUP30_128(x)      ((u32)(((x >> 17) & 0x0000007F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP29_128        ((u32)0x0000000F << 13)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP29_128(x)          ((u32)(((x) & 0x0000000F) << 13))
#define MBISTCTRL_GET_MBIST_ENC_GROUP29_128(x)      ((u32)(((x >> 13) & 0x0000000F)))
#define MBISTCTRL_BIT_MBIST_ENC_GROUP28_128         ((u32)0x00000001 << 12)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_ENC_GROUP27_128        ((u32)0x00000003 << 10)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP27_128(x)          ((u32)(((x) & 0x00000003) << 10))
#define MBISTCTRL_GET_MBIST_ENC_GROUP27_128(x)      ((u32)(((x >> 10) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP26_128        ((u32)0x000003FF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP26_128(x)          ((u32)(((x) & 0x000003FF) << 0))
#define MBISTCTRL_GET_MBIST_ENC_GROUP26_128(x)      ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_19
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_12C                 ((u32)0x0000000F << 28)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_12C(x)                   ((u32)(((x) & 0x0000000F) << 28))
#define MBISTCTRL_GET_RESERVED_12C(x)               ((u32)(((x >> 28) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP35_12C        ((u32)0x000003FF << 18)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP35_12C(x)          ((u32)(((x) & 0x000003FF) << 18))
#define MBISTCTRL_GET_MBIST_ENC_GROUP35_12C(x)      ((u32)(((x >> 18) & 0x000003FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP34_12C        ((u32)0x0000003F << 12)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP34_12C(x)          ((u32)(((x) & 0x0000003F) << 12))
#define MBISTCTRL_GET_MBIST_ENC_GROUP34_12C(x)      ((u32)(((x >> 12) & 0x0000003F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP33_12C        ((u32)0x0000003F << 6)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP33_12C(x)          ((u32)(((x) & 0x0000003F) << 6))
#define MBISTCTRL_GET_MBIST_ENC_GROUP33_12C(x)      ((u32)(((x >> 6) & 0x0000003F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP32            ((u32)0x0000003F << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP32(x)              ((u32)(((x) & 0x0000003F) << 0))
#define MBISTCTRL_GET_MBIST_ENC_GROUP32(x)          ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_20
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_130                 ((u32)0x0000001F << 27)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_130(x)                   ((u32)(((x) & 0x0000001F) << 27))
#define MBISTCTRL_GET_RESERVED_130(x)               ((u32)(((x >> 27) & 0x0000001F)))
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPOFF_E0_130     ((u32)0x00000001 << 26)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_SPRAMX_WLK4_E0_130     ((u32)0x00000003 << 24)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_SPRAMX_WLK4_E0_130(x)       ((u32)(((x) & 0x00000003) << 24))
#define MBISTCTRL_GET_MBIST_SPRAMX_WLK4_E0_130(x)   ((u32)(((x >> 24) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_DDRC_1P_SRAM_130       ((u32)0x0000001F << 19)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_DDRC_1P_SRAM_130(x)         ((u32)(((x) & 0x0000001F) << 19))
#define MBISTCTRL_GET_MBIST_DDRC_1P_SRAM_130(x)     ((u32)(((x >> 19) & 0x0000001F)))
#define MBISTCTRL_MASK_MBIST_DDRC_2P_SRAM_130       ((u32)0x0000000F << 15)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_DDRC_2P_SRAM_130(x)         ((u32)(((x) & 0x0000000F) << 15))
#define MBISTCTRL_GET_MBIST_DDRC_2P_SRAM_130(x)     ((u32)(((x >> 15) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_KM4_MEDIA_DTCM_DATA    ((u32)0x00000003 << 13)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_KM4_MEDIA_DTCM_DATA(x)      ((u32)(((x) & 0x00000003) << 13))
#define MBISTCTRL_GET_MBIST_KM4_MEDIA_DTCM_DATA(x)  ((u32)(((x >> 13) & 0x00000003)))
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_ITCM_DATA_130 ((u32)0x00000001 << 12)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_DCACHE_TAG_130 ((u32)0x00000001 << 11)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_DCACHE_DATA_130 ((u32)0x00000001 << 10)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_ICACHE_TAG    ((u32)0x00000001 << 9)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_ICACHE_DATA_130 ((u32)0x00000001 << 8)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_DTCM_DATA_130 ((u32)0x00000001 << 7)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_ITCM_DATA_130 ((u32)0x00000001 << 6)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_DCACHE_TAG_130 ((u32)0x00000001 << 5)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_DCACHE_DATA_130 ((u32)0x00000001 << 4)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_ICACHE_TAG_130 ((u32)0x00000001 << 3)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_ICACHE_DATA_130 ((u32)0x00000001 << 2)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_ENC_GROUP36_130        ((u32)0x00000003 << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP36_130(x)          ((u32)(((x) & 0x00000003) << 0))
#define MBISTCTRL_GET_MBIST_ENC_GROUP36_130(x)      ((u32)(((x >> 0) & 0x00000003)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_21
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_134                 ((u32)0x00000003 << 30)                  /*!< --    --  None */
#define MBISTCTRL_RESERVED_134(x)                   ((u32)(((x) & 0x00000003) << 30))
#define MBISTCTRL_GET_RESERVED_134(x)               ((u32)(((x >> 30) & 0x00000003)))
#define MBISTCTRL_BIT_MBIST_BT_07_134               ((u32)0x00000001 << 29)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_06_134               ((u32)0x00000001 << 28)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_05                   ((u32)0x00000001 << 27)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_BT_04_134              ((u32)0x0000000F << 23)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_BT_04_134(x)                ((u32)(((x) & 0x0000000F) << 23))
#define MBISTCTRL_GET_MBIST_BT_04_134(x)            ((u32)(((x >> 23) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_BT_03_134              ((u32)0x0000001F << 18)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_BT_03_134(x)                ((u32)(((x) & 0x0000001F) << 18))
#define MBISTCTRL_GET_MBIST_BT_03_134(x)            ((u32)(((x >> 18) & 0x0000001F)))
#define MBISTCTRL_BIT_MBIST_BT_02_134               ((u32)0x00000001 << 17)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPON_E0_134      ((u32)0x00000001 << 16)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_TPRAMX_WPOFF_E5_134    ((u32)0x00000003 << 14)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_TPRAMX_WPOFF_E5_134(x)      ((u32)(((x) & 0x00000003) << 14))
#define MBISTCTRL_GET_MBIST_TPRAMX_WPOFF_E5_134(x)  ((u32)(((x >> 14) & 0x00000003)))
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPOFF_E4_134     ((u32)0x00000001 << 13)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPOFF_E3_134     ((u32)0x00000001 << 12)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_TPRAMX_WPOFF_E2_134    ((u32)0x00000003 << 10)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_TPRAMX_WPOFF_E2_134(x)      ((u32)(((x) & 0x00000003) << 10))
#define MBISTCTRL_GET_MBIST_TPRAMX_WPOFF_E2_134(x)  ((u32)(((x >> 10) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_SPRAMX_WPOFF_EL_134    ((u32)0x000003FF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_SPRAMX_WPOFF_EL_134(x)      ((u32)(((x) & 0x000003FF) << 0))
#define MBISTCTRL_GET_MBIST_SPRAMX_WPOFF_EL_134(x)  ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_22
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_MBIST_BT_SHARE_SRAM_138      ((u32)0x000000FF << 4)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_BT_SHARE_SRAM_138(x)        ((u32)(((x) & 0x000000FF) << 4))
#define MBISTCTRL_GET_MBIST_BT_SHARE_SRAM_138(x)    ((u32)(((x >> 4) & 0x000000FF)))
#define MBISTCTRL_MASK_MBIST_BT_CPU_138             ((u32)0x0000000F << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_BT_CPU_138(x)               ((u32)(((x) & 0x0000000F) << 0))
#define MBISTCTRL_GET_MBIST_BT_CPU_138(x)           ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_23
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_MBIST_SYSTEM_SRAM_13C        ((u32)0xFFFFFFFF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_SYSTEM_SRAM_13C(x)          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define MBISTCTRL_GET_MBIST_SYSTEM_SRAM_13C(x)      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_24
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_MBIST_AIDE_GROUP03_140       ((u32)0x000001FF << 23)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_AIDE_GROUP03_140(x)         ((u32)(((x) & 0x000001FF) << 23))
#define MBISTCTRL_GET_MBIST_AIDE_GROUP03_140(x)     ((u32)(((x >> 23) & 0x000001FF)))
#define MBISTCTRL_MASK_MBIST_AIDE_GROUP02_140       ((u32)0x000001FF << 14)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_AIDE_GROUP02_140(x)         ((u32)(((x) & 0x000001FF) << 14))
#define MBISTCTRL_GET_MBIST_AIDE_GROUP02_140(x)     ((u32)(((x >> 14) & 0x000001FF)))
#define MBISTCTRL_MASK_MBIST_AIDE_GROUP01_140       ((u32)0x0000007F << 7)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_AIDE_GROUP01_140(x)         ((u32)(((x) & 0x0000007F) << 7))
#define MBISTCTRL_GET_MBIST_AIDE_GROUP01_140(x)     ((u32)(((x >> 7) & 0x0000007F)))
#define MBISTCTRL_BIT_MBIST_NANDECC_SRAM_140        ((u32)0x00000001 << 6)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_VIDEO_SRAM_140         ((u32)0x0000003F << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_VIDEO_SRAM_140(x)           ((u32)(((x) & 0x0000003F) << 0))
#define MBISTCTRL_GET_MBIST_VIDEO_SRAM_140(x)       ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_25
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_MBIST_AIDE_GROUP07_144       ((u32)0x0000003F << 26)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_AIDE_GROUP07_144(x)         ((u32)(((x) & 0x0000003F) << 26))
#define MBISTCTRL_GET_MBIST_AIDE_GROUP07_144(x)     ((u32)(((x >> 26) & 0x0000003F)))
#define MBISTCTRL_MASK_MBIST_AIDE_GROUP06_144       ((u32)0x000000FF << 18)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_AIDE_GROUP06_144(x)         ((u32)(((x) & 0x000000FF) << 18))
#define MBISTCTRL_GET_MBIST_AIDE_GROUP06_144(x)     ((u32)(((x >> 18) & 0x000000FF)))
#define MBISTCTRL_MASK_MBIST_AIDE_GROUP05_144       ((u32)0x000003FF << 8)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_AIDE_GROUP05_144(x)         ((u32)(((x) & 0x000003FF) << 8))
#define MBISTCTRL_GET_MBIST_AIDE_GROUP05_144(x)     ((u32)(((x >> 8) & 0x000003FF)))
#define MBISTCTRL_MASK_MBIST_AIDE_GROUP04_144       ((u32)0x000000FF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_AIDE_GROUP04_144(x)         ((u32)(((x) & 0x000000FF) << 0))
#define MBISTCTRL_GET_MBIST_AIDE_GROUP04_144(x)     ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_MBIST_NRML_FAIL_RAM_26
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_148                 ((u32)0x0000001F << 27)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_148(x)                   ((u32)(((x) & 0x0000001F) << 27))
#define MBISTCTRL_GET_RESERVED_148(x)               ((u32)(((x >> 27) & 0x0000001F)))
#define MBISTCTRL_MASK_MBIST_TSC_GROUP00_148        ((u32)0x00000FFF << 15)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_TSC_GROUP00_148(x)          ((u32)(((x) & 0x00000FFF) << 15))
#define MBISTCTRL_GET_MBIST_TSC_GROUP00_148(x)      ((u32)(((x >> 15) & 0x00000FFF)))
#define MBISTCTRL_MASK_MBIST_AIDE_GROUP15_148       ((u32)0x0000001F << 10)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_AIDE_GROUP15_148(x)         ((u32)(((x) & 0x0000001F) << 10))
#define MBISTCTRL_GET_MBIST_AIDE_GROUP15_148(x)     ((u32)(((x >> 10) & 0x0000001F)))
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP14_148        ((u32)0x00000001 << 9)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_AIDE_GROUP13_148       ((u32)0x00000007 << 6)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_AIDE_GROUP13_148(x)         ((u32)(((x) & 0x00000007) << 6))
#define MBISTCTRL_GET_MBIST_AIDE_GROUP13_148(x)     ((u32)(((x >> 6) & 0x00000007)))
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP_12           ((u32)0x00000001 << 5)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP_11           ((u32)0x00000001 << 4)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP10_148        ((u32)0x00000001 << 3)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_AIDE_GROUP09_148       ((u32)0x00000003 << 1)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_AIDE_GROUP09_148(x)         ((u32)(((x) & 0x00000003) << 1))
#define MBISTCTRL_GET_MBIST_AIDE_GROUP09_148(x)     ((u32)(((x >> 1) & 0x00000003)))
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP08_148        ((u32)0x00000001 << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_0
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_14C                 ((u32)0x00000003 << 30)                  /*!< --    --  None */
#define MBISTCTRL_RESERVED_14C(x)                   ((u32)(((x) & 0x00000003) << 30))
#define MBISTCTRL_GET_RESERVED_14C(x)               ((u32)(((x >> 30) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP08_14C         ((u32)0x0000000F << 26)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP08_14C(x)           ((u32)(((x) & 0x0000000F) << 26))
#define MBISTCTRL_GET_MBIST_NN_GROUP08_14C(x)       ((u32)(((x >> 26) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP07_14C         ((u32)0x00000003 << 24)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP07_14C(x)           ((u32)(((x) & 0x00000003) << 24))
#define MBISTCTRL_GET_MBIST_NN_GROUP07_14C(x)       ((u32)(((x >> 24) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP06_14C         ((u32)0x0000001F << 19)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP06_14C(x)           ((u32)(((x) & 0x0000001F) << 19))
#define MBISTCTRL_GET_MBIST_NN_GROUP06_14C(x)       ((u32)(((x >> 19) & 0x0000001F)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP05_14C         ((u32)0x0000000F << 15)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP05_14C(x)           ((u32)(((x) & 0x0000000F) << 15))
#define MBISTCTRL_GET_MBIST_NN_GROUP05_14C(x)       ((u32)(((x >> 15) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP04_14C         ((u32)0x00000003 << 13)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP04_14C(x)           ((u32)(((x) & 0x00000003) << 13))
#define MBISTCTRL_GET_MBIST_NN_GROUP04_14C(x)       ((u32)(((x >> 13) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP03_14C         ((u32)0x000000FF << 5)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP03_14C(x)           ((u32)(((x) & 0x000000FF) << 5))
#define MBISTCTRL_GET_MBIST_NN_GROUP03_14C(x)       ((u32)(((x >> 5) & 0x000000FF)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP02_14C         ((u32)0x00000003 << 3)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP02_14C(x)           ((u32)(((x) & 0x00000003) << 3))
#define MBISTCTRL_GET_MBIST_NN_GROUP02_14C(x)       ((u32)(((x >> 3) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP01_14C         ((u32)0x00000007 << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP01_14C(x)           ((u32)(((x) & 0x00000007) << 0))
#define MBISTCTRL_GET_MBIST_NN_GROUP01_14C(x)       ((u32)(((x >> 0) & 0x00000007)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_1
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_MBIST_NN_GROUP12_150         ((u32)0x000003FF << 22)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP12_150(x)           ((u32)(((x) & 0x000003FF) << 22))
#define MBISTCTRL_GET_MBIST_NN_GROUP12_150(x)       ((u32)(((x >> 22) & 0x000003FF)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP11_150         ((u32)0x00000FFF << 10)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP11_150(x)           ((u32)(((x) & 0x00000FFF) << 10))
#define MBISTCTRL_GET_MBIST_NN_GROUP11_150(x)       ((u32)(((x >> 10) & 0x00000FFF)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP10_150         ((u32)0x00000007 << 7)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP10_150(x)           ((u32)(((x) & 0x00000007) << 7))
#define MBISTCTRL_GET_MBIST_NN_GROUP10_150(x)       ((u32)(((x >> 7) & 0x00000007)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP09_150         ((u32)0x0000007F << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP09_150(x)           ((u32)(((x) & 0x0000007F) << 0))
#define MBISTCTRL_GET_MBIST_NN_GROUP09_150(x)       ((u32)(((x >> 0) & 0x0000007F)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_2
 * @brief
 * @{
 */
#define MBISTCTRL_BIT_RESERVED_154                  ((u32)0x00000001 << 31)                  /*!< R     --  None */
#define MBISTCTRL_MASK_MBIST_NN_GROUP18_154         ((u32)0x0000007F << 24)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP18_154(x)           ((u32)(((x) & 0x0000007F) << 24))
#define MBISTCTRL_GET_MBIST_NN_GROUP18_154(x)       ((u32)(((x >> 24) & 0x0000007F)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP17_154         ((u32)0x00000003 << 22)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP17_154(x)           ((u32)(((x) & 0x00000003) << 22))
#define MBISTCTRL_GET_MBIST_NN_GROUP17_154(x)       ((u32)(((x >> 22) & 0x00000003)))
#define MBISTCTRL_BIT_MBIST_NN_GROUP16_154          ((u32)0x00000001 << 21)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_NN_GROUP15_154         ((u32)0x00000007 << 18)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP15_154(x)           ((u32)(((x) & 0x00000007) << 18))
#define MBISTCTRL_GET_MBIST_NN_GROUP15_154(x)       ((u32)(((x >> 18) & 0x00000007)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP14_154         ((u32)0x000000FF << 10)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP14_154(x)           ((u32)(((x) & 0x000000FF) << 10))
#define MBISTCTRL_GET_MBIST_NN_GROUP14_154(x)       ((u32)(((x >> 10) & 0x000000FF)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP13_154         ((u32)0x000003FF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP13_154(x)           ((u32)(((x) & 0x000003FF) << 0))
#define MBISTCTRL_GET_MBIST_NN_GROUP13_154(x)       ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_3
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_158                 ((u32)0x0000001F << 27)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_158(x)                   ((u32)(((x) & 0x0000001F) << 27))
#define MBISTCTRL_GET_RESERVED_158(x)               ((u32)(((x >> 27) & 0x0000001F)))
#define MBISTCTRL_MASK_MBIST_GE_BT1_158             ((u32)0x000000FF << 19)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_GE_BT1_158(x)               ((u32)(((x) & 0x000000FF) << 19))
#define MBISTCTRL_GET_MBIST_GE_BT1_158(x)           ((u32)(((x >> 19) & 0x000000FF)))
#define MBISTCTRL_MASK_MBIST_GE_BT0_158             ((u32)0x0000001F << 14)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_GE_BT0_158(x)               ((u32)(((x) & 0x0000001F) << 14))
#define MBISTCTRL_GET_MBIST_GE_BT0_158(x)           ((u32)(((x >> 14) & 0x0000001F)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP20_158         ((u32)0x0000007F << 7)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP20_158(x)           ((u32)(((x) & 0x0000007F) << 7))
#define MBISTCTRL_GET_MBIST_NN_GROUP20_158(x)       ((u32)(((x >> 7) & 0x0000007F)))
#define MBISTCTRL_MASK_MBIST_NN_GROUP19_158         ((u32)0x0000007F << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_NN_GROUP19_158(x)           ((u32)(((x) & 0x0000007F) << 0))
#define MBISTCTRL_GET_MBIST_NN_GROUP19_158(x)       ((u32)(((x >> 0) & 0x0000007F)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_4
 * @brief
 * @{
 */
#define MBISTCTRL_BIT_MBIST_KM0_FW_ICACHE_SRAM_15C  ((u32)0x00000001 << 31)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_PKE_TMEM_SRAM_15C       ((u32)0x00000001 << 30)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_PKE_MMEM_SRAM_15C       ((u32)0x00000001 << 29)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_USBOTG_15C              ((u32)0x00000001 << 28)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_SDMMC_PPBECC_15C       ((u32)0x00000003 << 26)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_SDMMC_PPBECC_15C(x)         ((u32)(((x) & 0x00000003) << 26))
#define MBISTCTRL_GET_MBIST_SDMMC_PPBECC_15C(x)     ((u32)(((x >> 26) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_SDMMC_DMASBL_15C       ((u32)0x00000003 << 24)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_SDMMC_DMASBL_15C(x)         ((u32)(((x) & 0x00000003) << 24))
#define MBISTCTRL_GET_MBIST_SDMMC_DMASBL_15C(x)     ((u32)(((x >> 24) & 0x00000003)))
#define MBISTCTRL_BIT_MBIST_SDIO_15C                ((u32)0x00000001 << 23)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_GMAC_SRAM_15C          ((u32)0x00000003 << 21)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_GMAC_SRAM_15C(x)            ((u32)(((x) & 0x00000003) << 21))
#define MBISTCTRL_GET_MBIST_GMAC_SRAM_15C(x)        ((u32)(((x >> 21) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_TSC_GROUP00_15C        ((u32)0x000003FF << 11)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_TSC_GROUP00_15C(x)          ((u32)(((x) & 0x000003FF) << 11))
#define MBISTCTRL_GET_MBIST_TSC_GROUP00_15C(x)      ((u32)(((x >> 11) & 0x000003FF)))
#define MBISTCTRL_MASK_MBIST_GE_ZOOML_15C           ((u32)0x0000001F << 6)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_GE_ZOOML_15C(x)             ((u32)(((x) & 0x0000001F) << 6))
#define MBISTCTRL_GET_MBIST_GE_ZOOML_15C(x)         ((u32)(((x >> 6) & 0x0000001F)))
#define MBISTCTRL_MASK_MBIST_GE_ZOOM0_15C           ((u32)0x0000003F << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_GE_ZOOM0_15C(x)             ((u32)(((x) & 0x0000003F) << 0))
#define MBISTCTRL_GET_MBIST_GE_ZOOM0_15C(x)         ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_5
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_160                 ((u32)0x0000000F << 28)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_160(x)                   ((u32)(((x) & 0x0000000F) << 28))
#define MBISTCTRL_GET_RESERVED_160(x)               ((u32)(((x >> 28) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISPB_160     ((u32)0x00007FFF << 13)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISPB_160(x)       ((u32)(((x) & 0x00007FFF) << 13))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISPB_160(x)   ((u32)(((x >> 13) & 0x00007FFF)))
#define MBISTCTRL_MASK_MBIST_ISPGROUP_ISPA_160      ((u32)0x000003FF << 3)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISPGROUP_ISPA_160(x)        ((u32)(((x) & 0x000003FF) << 3))
#define MBISTCTRL_GET_MBIST_ISPGROUP_ISPA_160(x)    ((u32)(((x >> 3) & 0x000003FF)))
#define MBISTCTRL_BIT_MBIST_KM0_FW_DCACHE_TAG_SRAM_160 ((u32)0x00000001 << 2)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM0_FW_DCACHE_SRAM_160  ((u32)0x00000001 << 1)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM0_FW_ICACHE_TAG_SRAM_160 ((u32)0x00000001 << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_6
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISPE_164     ((u32)0x00000FFF << 18)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISPE_164(x)       ((u32)(((x) & 0x00000FFF) << 18))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISPE_164(x)   ((u32)(((x >> 18) & 0x00000FFF)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISPC_164     ((u32)0x0000001F << 13)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISPC_164(x)       ((u32)(((x) & 0x0000001F) << 13))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISPC_164(x)   ((u32)(((x >> 13) & 0x0000001F)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISPG_164     ((u32)0x00001FFF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISPG_164(x)       ((u32)(((x) & 0x00001FFF) << 0))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISPG_164(x)   ((u32)(((x >> 0) & 0x00001FFF)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_7
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISPH_168     ((u32)0x0000007F << 16)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISPH_168(x)       ((u32)(((x) & 0x0000007F) << 16))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISPH_168(x)   ((u32)(((x >> 16) & 0x0000007F)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISPD_168     ((u32)0x0000000F << 12)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISPD_168(x)       ((u32)(((x) & 0x0000000F) << 12))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISPD_168(x)   ((u32)(((x >> 12) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISPF_168     ((u32)0x00000FFF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISPF_168(x)       ((u32)(((x) & 0x00000FFF) << 0))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISPF_168(x)   ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_8
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_16C                 ((u32)0x0000000F << 28)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_16C(x)                   ((u32)(((x) & 0x0000000F) << 28))
#define MBISTCTRL_GET_RESERVED_16C(x)               ((u32)(((x >> 28) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISPL_16C     ((u32)0x00003FFF << 14)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISPL_16C(x)       ((u32)(((x) & 0x00003FFF) << 14))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISPL_16C(x)   ((u32)(((x >> 14) & 0x00003FFF)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISPJ_16C     ((u32)0x00003FFF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISPJ_16C(x)       ((u32)(((x) & 0x00003FFF) << 0))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISPJ_16C(x)   ((u32)(((x >> 0) & 0x00003FFF)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_9
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_170                 ((u32)0xFFFFFFFF << 0)                   /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_170(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define MBISTCTRL_GET_RESERVED_170(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_10
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_174                 ((u32)0x00000003 << 30)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_174(x)                   ((u32)(((x) & 0x00000003) << 30))
#define MBISTCTRL_GET_RESERVED_174(x)               ((u32)(((x >> 30) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_MPRX_174     ((u32)0x00000003 << 28)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_MPRX_174(x)       ((u32)(((x) & 0x00000003) << 28))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_MPRX_174(x)   ((u32)(((x >> 28) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_VIDEO_AMBAL_174 ((u32)0x0000003F << 22)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_VIDEO_AMBAL_174(x) ((u32)(((x) & 0x0000003F) << 22))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_VIDEO_AMBAL_174(x) ((u32)(((x >> 22) & 0x0000003F)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_VIDEO_AMBA0_174 ((u32)0x00000003 << 20)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_VIDEO_AMBA0_174(x) ((u32)(((x) & 0x00000003) << 20))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_VIDEO_AMBA0_174(x) ((u32)(((x >> 20) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISP_OSD_174  ((u32)0x00000007 << 17)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISP_OSD_174(x)    ((u32)(((x) & 0x00000007) << 17))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISP_OSD_174(x) ((u32)(((x >> 17) & 0x00000007)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISP_DMA_174  ((u32)0x00000007 << 14)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISP_DMA_174(x)    ((u32)(((x) & 0x00000007) << 14))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISP_DMA_174(x) ((u32)(((x >> 14) & 0x00000007)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ZOOM_UP_174  ((u32)0x0000000F << 10)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ZOOM_UP_174(x)    ((u32)(((x) & 0x0000000F) << 10))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ZOOM_UP_174(x) ((u32)(((x >> 10) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ZOOM_DOWN_174 ((u32)0x00000003 << 8)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ZOOM_DOWN_174(x)  ((u32)(((x) & 0x00000003) << 8))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ZOOM_DOWN_174(x) ((u32)(((x >> 8) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_ISPK_174     ((u32)0x000000FF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_ISPK_174(x)       ((u32)(((x) & 0x000000FF) << 0))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_ISPK_174(x)   ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_11
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_178                 ((u32)0x0000001F << 27)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_178(x)                   ((u32)(((x) & 0x0000001F) << 27))
#define MBISTCTRL_GET_RESERVED_178(x)               ((u32)(((x >> 27) & 0x0000001F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP02_178        ((u32)0x000003FF << 17)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP02_178(x)          ((u32)(((x) & 0x000003FF) << 17))
#define MBISTCTRL_GET_MBIST_ENC_GROUP02_178(x)      ((u32)(((x >> 17) & 0x000003FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP01_178        ((u32)0x0000000F << 13)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP01_178(x)          ((u32)(((x) & 0x0000000F) << 13))
#define MBISTCTRL_GET_MBIST_ENC_GROUP01_178(x)      ((u32)(((x >> 13) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP00_178        ((u32)0x000001FF << 4)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP00_178(x)          ((u32)(((x) & 0x000001FF) << 4))
#define MBISTCTRL_GET_MBIST_ENC_GROUP00_178(x)      ((u32)(((x >> 4) & 0x000001FF)))
#define MBISTCTRL_MASK_MBIST_ISP_GROUP_AID_WRAPPER_178 ((u32)0x0000000F << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ISP_GROUP_AID_WRAPPER_178(x) ((u32)(((x) & 0x0000000F) << 0))
#define MBISTCTRL_GET_MBIST_ISP_GROUP_AID_WRAPPER_178(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_12
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_17C                 ((u32)0x0000003F << 26)                  /*!< -     nan None */
#define MBISTCTRL_RESERVED_17C(x)                   ((u32)(((x) & 0x0000003F) << 26))
#define MBISTCTRL_GET_RESERVED_17C(x)               ((u32)(((x >> 26) & 0x0000003F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP05_17C        ((u32)0x000000FF << 18)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP05_17C(x)          ((u32)(((x) & 0x000000FF) << 18))
#define MBISTCTRL_GET_MBIST_ENC_GROUP05_17C(x)      ((u32)(((x >> 18) & 0x000000FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP04_17C        ((u32)0x000003FF << 8)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP04_17C(x)          ((u32)(((x) & 0x000003FF) << 8))
#define MBISTCTRL_GET_MBIST_ENC_GROUP04_17C(x)      ((u32)(((x >> 8) & 0x000003FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP03_17C        ((u32)0x000000FF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP03_17C(x)          ((u32)(((x) & 0x000000FF) << 0))
#define MBISTCTRL_GET_MBIST_ENC_GROUP03_17C(x)      ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_13
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_MBIST_ENC_GROUP10_180        ((u32)0x0000000F << 26)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP10_180(x)          ((u32)(((x) & 0x0000000F) << 26))
#define MBISTCTRL_GET_MBIST_ENC_GROUP10_180(x)      ((u32)(((x >> 26) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP09_180        ((u32)0x00000007 << 23)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP09_180(x)          ((u32)(((x) & 0x00000007) << 23))
#define MBISTCTRL_GET_MBIST_ENC_GROUP09_180(x)      ((u32)(((x >> 23) & 0x00000007)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP08_180        ((u32)0x0000007F << 16)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP08_180(x)          ((u32)(((x) & 0x0000007F) << 16))
#define MBISTCTRL_GET_MBIST_ENC_GROUP08_180(x)      ((u32)(((x >> 16) & 0x0000007F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP07_180        ((u32)0x000001FF << 7)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP07_180(x)          ((u32)(((x) & 0x000001FF) << 7))
#define MBISTCTRL_GET_MBIST_ENC_GROUP07_180(x)      ((u32)(((x >> 7) & 0x000001FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP06_180        ((u32)0x0000007F << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP06_180(x)          ((u32)(((x) & 0x0000007F) << 0))
#define MBISTCTRL_GET_MBIST_ENC_GROUP06_180(x)      ((u32)(((x >> 0) & 0x0000007F)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_14
 * @brief
 * @{
 */
#define MBISTCTRL_BIT_RESERVED_184                  ((u32)0x00000001 << 31)                  /*!< -     nan None */
#define MBISTCTRL_MASK_MBIST_ENC_GROUP14_184        ((u32)0x000000FF << 23)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP14_184(x)          ((u32)(((x) & 0x000000FF) << 23))
#define MBISTCTRL_GET_MBIST_ENC_GROUP14_184(x)      ((u32)(((x >> 23) & 0x000000FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP13_184        ((u32)0x000000FF << 15)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP13_184(x)          ((u32)(((x) & 0x000000FF) << 15))
#define MBISTCTRL_GET_MBIST_ENC_GROUP13_184(x)      ((u32)(((x >> 15) & 0x000000FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP12_184        ((u32)0x000003FF << 5)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP12_184(x)          ((u32)(((x) & 0x000003FF) << 5))
#define MBISTCTRL_GET_MBIST_ENC_GROUP12_184(x)      ((u32)(((x >> 5) & 0x000003FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP11_184        ((u32)0x0000001F << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP11_184(x)          ((u32)(((x) & 0x0000001F) << 0))
#define MBISTCTRL_GET_MBIST_ENC_GROUP11_184(x)      ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_15
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_188                 ((u32)0x0000007F << 25)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_188(x)                   ((u32)(((x) & 0x0000007F) << 25))
#define MBISTCTRL_GET_RESERVED_188(x)               ((u32)(((x >> 25) & 0x0000007F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP17_188        ((u32)0x000000FF << 17)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP17_188(x)          ((u32)(((x) & 0x000000FF) << 17))
#define MBISTCTRL_GET_MBIST_ENC_GROUP17_188(x)      ((u32)(((x >> 17) & 0x000000FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP16_188        ((u32)0x000000FF << 9)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP16_188(x)          ((u32)(((x) & 0x000000FF) << 9))
#define MBISTCTRL_GET_MBIST_ENC_GROUP16_188(x)      ((u32)(((x >> 9) & 0x000000FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP15_188        ((u32)0x000001FF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP15_188(x)          ((u32)(((x) & 0x000001FF) << 0))
#define MBISTCTRL_GET_MBIST_ENC_GROUP15_188(x)      ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_16
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_18C                 ((u32)0x0000000F << 28)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_18C(x)                   ((u32)(((x) & 0x0000000F) << 28))
#define MBISTCTRL_GET_RESERVED_18C(x)               ((u32)(((x >> 28) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP20_18C        ((u32)0x000003FF << 18)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP20_18C(x)          ((u32)(((x) & 0x000003FF) << 18))
#define MBISTCTRL_GET_MBIST_ENC_GROUP20_18C(x)      ((u32)(((x >> 18) & 0x000003FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP19_18C        ((u32)0x000003FF << 8)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP19_18C(x)          ((u32)(((x) & 0x000003FF) << 8))
#define MBISTCTRL_GET_MBIST_ENC_GROUP19_18C(x)      ((u32)(((x >> 8) & 0x000003FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP18_18C        ((u32)0x000000FF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP18_18C(x)          ((u32)(((x) & 0x000000FF) << 0))
#define MBISTCTRL_GET_MBIST_ENC_GROUP18_18C(x)      ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_17
 * @brief
 * @{
 */
#define MBISTCTRL_BIT_RESERVED_190                  ((u32)0x00000001 << 31)                  /*!< R     0x0 None */
#define MBISTCTRL_MASK_MBIST_ENC_GROUP25_190        ((u32)0x000003FF << 21)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP25_190(x)          ((u32)(((x) & 0x000003FF) << 21))
#define MBISTCTRL_GET_MBIST_ENC_GROUP25_190(x)      ((u32)(((x >> 21) & 0x000003FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP24_190        ((u32)0x000001FF << 12)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP24_190(x)          ((u32)(((x) & 0x000001FF) << 12))
#define MBISTCTRL_GET_MBIST_ENC_GROUP24_190(x)      ((u32)(((x >> 12) & 0x000001FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP23_190        ((u32)0x000001FF << 3)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP23_190(x)          ((u32)(((x) & 0x000001FF) << 3))
#define MBISTCTRL_GET_MBIST_ENC_GROUP23_190(x)      ((u32)(((x >> 3) & 0x000001FF)))
#define MBISTCTRL_BIT_MBIST_ENC_GROUP22_190         ((u32)0x00000001 << 2)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_ENC_GROUP21_190        ((u32)0x00000003 << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP21_190(x)          ((u32)(((x) & 0x00000003) << 0))
#define MBISTCTRL_GET_MBIST_ENC_GROUP21_190(x)      ((u32)(((x >> 0) & 0x00000003)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_18
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_194                 ((u32)0x00000003 << 30)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_194(x)                   ((u32)(((x) & 0x00000003) << 30))
#define MBISTCTRL_GET_RESERVED_194(x)               ((u32)(((x >> 30) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP31_194        ((u32)0x0000003F << 24)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP31_194(x)          ((u32)(((x) & 0x0000003F) << 24))
#define MBISTCTRL_GET_MBIST_ENC_GROUP31_194(x)      ((u32)(((x >> 24) & 0x0000003F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP30_194        ((u32)0x0000007F << 17)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP30_194(x)          ((u32)(((x) & 0x0000007F) << 17))
#define MBISTCTRL_GET_MBIST_ENC_GROUP30_194(x)      ((u32)(((x >> 17) & 0x0000007F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP29_194        ((u32)0x0000000F << 13)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP29_194(x)          ((u32)(((x) & 0x0000000F) << 13))
#define MBISTCTRL_GET_MBIST_ENC_GROUP29_194(x)      ((u32)(((x >> 13) & 0x0000000F)))
#define MBISTCTRL_BIT_MBIST_ENC_GROUP28_194         ((u32)0x00000001 << 12)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_ENC_GROUP27_194        ((u32)0x00000003 << 10)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP27_194(x)          ((u32)(((x) & 0x00000003) << 10))
#define MBISTCTRL_GET_MBIST_ENC_GROUP27_194(x)      ((u32)(((x >> 10) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP26_194        ((u32)0x000003FF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP26_194(x)          ((u32)(((x) & 0x000003FF) << 0))
#define MBISTCTRL_GET_MBIST_ENC_GROUP26_194(x)      ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_19
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_198                 ((u32)0x0000000F << 28)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_198(x)                   ((u32)(((x) & 0x0000000F) << 28))
#define MBISTCTRL_GET_RESERVED_198(x)               ((u32)(((x >> 28) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP35_198        ((u32)0x000003FF << 18)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP35_198(x)          ((u32)(((x) & 0x000003FF) << 18))
#define MBISTCTRL_GET_MBIST_ENC_GROUP35_198(x)      ((u32)(((x >> 18) & 0x000003FF)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP34_198        ((u32)0x0000003F << 12)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP34_198(x)          ((u32)(((x) & 0x0000003F) << 12))
#define MBISTCTRL_GET_MBIST_ENC_GROUP34_198(x)      ((u32)(((x >> 12) & 0x0000003F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP33_198        ((u32)0x0000003F << 6)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP33_198(x)          ((u32)(((x) & 0x0000003F) << 6))
#define MBISTCTRL_GET_MBIST_ENC_GROUP33_198(x)      ((u32)(((x >> 6) & 0x0000003F)))
#define MBISTCTRL_MASK_MBIST_ENC_GROUP32_198        ((u32)0x0000003F << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP32_198(x)          ((u32)(((x) & 0x0000003F) << 0))
#define MBISTCTRL_GET_MBIST_ENC_GROUP32_198(x)      ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_20
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_19C                 ((u32)0x0000001F << 27)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_19C(x)                   ((u32)(((x) & 0x0000001F) << 27))
#define MBISTCTRL_GET_RESERVED_19C(x)               ((u32)(((x >> 27) & 0x0000001F)))
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPOFF_E0_19C     ((u32)0x00000001 << 26)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_SPRAMX_WLK4_E0_19C     ((u32)0x00000003 << 24)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_SPRAMX_WLK4_E0_19C(x)       ((u32)(((x) & 0x00000003) << 24))
#define MBISTCTRL_GET_MBIST_SPRAMX_WLK4_E0_19C(x)   ((u32)(((x >> 24) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_DDRC_1P_SRAM_19C       ((u32)0x0000001F << 19)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_DDRC_1P_SRAM_19C(x)         ((u32)(((x) & 0x0000001F) << 19))
#define MBISTCTRL_GET_MBIST_DDRC_1P_SRAM_19C(x)     ((u32)(((x >> 19) & 0x0000001F)))
#define MBISTCTRL_MASK_MBIST_DDRC_2P_SRAM_19C       ((u32)0x0000000F << 15)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_DDRC_2P_SRAM_19C(x)         ((u32)(((x) & 0x0000000F) << 15))
#define MBISTCTRL_GET_MBIST_DDRC_2P_SRAM_19C(x)     ((u32)(((x >> 15) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_KM4_MEDIA_DTCM_DATA_19C ((u32)0x00000003 << 13)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_KM4_MEDIA_DTCM_DATA_19C(x)  ((u32)(((x) & 0x00000003) << 13))
#define MBISTCTRL_GET_MBIST_KM4_MEDIA_DTCM_DATA_19C(x) ((u32)(((x >> 13) & 0x00000003)))
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_ITCM_DATA_19C ((u32)0x00000001 << 12)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_DCACHE_TAG_19C ((u32)0x00000001 << 11)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_DCACHE_DATA_19C ((u32)0x00000001 << 10)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_ICACHE_TAG_19C ((u32)0x00000001 << 9)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_ICACHE_DATA_19C ((u32)0x00000001 << 8)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_DTCM_DATA_19C ((u32)0x00000001 << 7)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_ITCM_DATA_19C ((u32)0x00000001 << 6)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_DCACHE_TAG_19C ((u32)0x00000001 << 5)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_DCACHE_DATA_19C ((u32)0x00000001 << 4)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_ICACHE_TAG_19C ((u32)0x00000001 << 3)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_ICACHE_DATA_19C ((u32)0x00000001 << 2)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_ENC_GROUP36_19C        ((u32)0x00000003 << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_ENC_GROUP36_19C(x)          ((u32)(((x) & 0x00000003) << 0))
#define MBISTCTRL_GET_MBIST_ENC_GROUP36_19C(x)      ((u32)(((x >> 0) & 0x00000003)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_21
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_1A0                 ((u32)0x00000003 << 30)                  /*!< --    --  None */
#define MBISTCTRL_RESERVED_1A0(x)                   ((u32)(((x) & 0x00000003) << 30))
#define MBISTCTRL_GET_RESERVED_1A0(x)               ((u32)(((x >> 30) & 0x00000003)))
#define MBISTCTRL_BIT_MBIST_BT_07_1A0               ((u32)0x00000001 << 29)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_06_1A0               ((u32)0x00000001 << 28)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_05_1A0               ((u32)0x00000001 << 27)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_BT_04_1A0              ((u32)0x0000000F << 23)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_BT_04_1A0(x)                ((u32)(((x) & 0x0000000F) << 23))
#define MBISTCTRL_GET_MBIST_BT_04_1A0(x)            ((u32)(((x >> 23) & 0x0000000F)))
#define MBISTCTRL_MASK_MBIST_BT_03_1A0              ((u32)0x0000001F << 18)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_BT_03_1A0(x)                ((u32)(((x) & 0x0000001F) << 18))
#define MBISTCTRL_GET_MBIST_BT_03_1A0(x)            ((u32)(((x >> 18) & 0x0000001F)))
#define MBISTCTRL_BIT_MBIST_BT_02_1A0               ((u32)0x00000001 << 17)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPON_E0_1A0      ((u32)0x00000001 << 16)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_TPRAMX_WPOFF_E5_1A0    ((u32)0x00000003 << 14)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_TPRAMX_WPOFF_E5_1A0(x)      ((u32)(((x) & 0x00000003) << 14))
#define MBISTCTRL_GET_MBIST_TPRAMX_WPOFF_E5_1A0(x)  ((u32)(((x >> 14) & 0x00000003)))
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPOFF_E4_1A0     ((u32)0x00000001 << 13)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPOFF_E3_1A0     ((u32)0x00000001 << 12)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_TPRAMX_WPOFF_E2_1A0    ((u32)0x00000003 << 10)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_TPRAMX_WPOFF_E2_1A0(x)      ((u32)(((x) & 0x00000003) << 10))
#define MBISTCTRL_GET_MBIST_TPRAMX_WPOFF_E2_1A0(x)  ((u32)(((x >> 10) & 0x00000003)))
#define MBISTCTRL_MASK_MBIST_SPRAMX_WPOFF_EL_1A0    ((u32)0x000003FF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_SPRAMX_WPOFF_EL_1A0(x)      ((u32)(((x) & 0x000003FF) << 0))
#define MBISTCTRL_GET_MBIST_SPRAMX_WPOFF_EL_1A0(x)  ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_22
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_MBIST_BT_SHARE_SRAM_1A4      ((u32)0x000000FF << 4)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_BT_SHARE_SRAM_1A4(x)        ((u32)(((x) & 0x000000FF) << 4))
#define MBISTCTRL_GET_MBIST_BT_SHARE_SRAM_1A4(x)    ((u32)(((x >> 4) & 0x000000FF)))
#define MBISTCTRL_MASK_MBIST_BT_CPU_1A4             ((u32)0x0000000F << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_BT_CPU_1A4(x)               ((u32)(((x) & 0x0000000F) << 0))
#define MBISTCTRL_GET_MBIST_BT_CPU_1A4(x)           ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_23
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_MBIST_SYSTEM_SRAM_1A8        ((u32)0xFFFFFFFF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_SYSTEM_SRAM_1A8(x)          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define MBISTCTRL_GET_MBIST_SYSTEM_SRAM_1A8(x)      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_24
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_MBIST_AIDE_GROUP03_1AC       ((u32)0x000001FF << 23)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_AIDE_GROUP03_1AC(x)         ((u32)(((x) & 0x000001FF) << 23))
#define MBISTCTRL_GET_MBIST_AIDE_GROUP03_1AC(x)     ((u32)(((x >> 23) & 0x000001FF)))
#define MBISTCTRL_MASK_MBIST_AIDE_GROUP02_1AC       ((u32)0x000001FF << 14)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_AIDE_GROUP02_1AC(x)         ((u32)(((x) & 0x000001FF) << 14))
#define MBISTCTRL_GET_MBIST_AIDE_GROUP02_1AC(x)     ((u32)(((x >> 14) & 0x000001FF)))
#define MBISTCTRL_MASK_MBIST_AIDE_GROUP01_1AC       ((u32)0x0000007F << 7)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_AIDE_GROUP01_1AC(x)         ((u32)(((x) & 0x0000007F) << 7))
#define MBISTCTRL_GET_MBIST_AIDE_GROUP01_1AC(x)     ((u32)(((x >> 7) & 0x0000007F)))
#define MBISTCTRL_BIT_MBIST_NANDECC_SRAM_1AC        ((u32)0x00000001 << 6)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_VIDEO_SRAM_1AC         ((u32)0x0000003F << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_VIDEO_SRAM_1AC(x)           ((u32)(((x) & 0x0000003F) << 0))
#define MBISTCTRL_GET_MBIST_VIDEO_SRAM_1AC(x)       ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_25
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_MBIST_AIDE_GROUP07_1B0       ((u32)0x0000003F << 26)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_AIDE_GROUP07_1B0(x)         ((u32)(((x) & 0x0000003F) << 26))
#define MBISTCTRL_GET_MBIST_AIDE_GROUP07_1B0(x)     ((u32)(((x >> 26) & 0x0000003F)))
#define MBISTCTRL_MASK_MBIST_AIDE_GROUP06_1B0       ((u32)0x000000FF << 18)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_AIDE_GROUP06_1B0(x)         ((u32)(((x) & 0x000000FF) << 18))
#define MBISTCTRL_GET_MBIST_AIDE_GROUP06_1B0(x)     ((u32)(((x >> 18) & 0x000000FF)))
#define MBISTCTRL_MASK_MBIST_AIDE_GROUP05_1B0       ((u32)0x000003FF << 8)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_AIDE_GROUP05_1B0(x)         ((u32)(((x) & 0x000003FF) << 8))
#define MBISTCTRL_GET_MBIST_AIDE_GROUP05_1B0(x)     ((u32)(((x >> 8) & 0x000003FF)))
#define MBISTCTRL_MASK_MBIST_AIDE_GROUP04_1B0       ((u32)0x000000FF << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_AIDE_GROUP04_1B0(x)         ((u32)(((x) & 0x000000FF) << 0))
#define MBISTCTRL_GET_MBIST_AIDE_GROUP04_1B0(x)     ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_MBIST_DRF_FAIL_RAM_26
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_1B4                 ((u32)0x0000001F << 27)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_1B4(x)                   ((u32)(((x) & 0x0000001F) << 27))
#define MBISTCTRL_GET_RESERVED_1B4(x)               ((u32)(((x >> 27) & 0x0000001F)))
#define MBISTCTRL_MASK_MBIST_TSC_GROUP00_1B4        ((u32)0x00000FFF << 15)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_TSC_GROUP00_1B4(x)          ((u32)(((x) & 0x00000FFF) << 15))
#define MBISTCTRL_GET_MBIST_TSC_GROUP00_1B4(x)      ((u32)(((x >> 15) & 0x00000FFF)))
#define MBISTCTRL_MASK_MBIST_AIDE_GROUP15_1B4       ((u32)0x0000001F << 10)                  /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_AIDE_GROUP15_1B4(x)         ((u32)(((x) & 0x0000001F) << 10))
#define MBISTCTRL_GET_MBIST_AIDE_GROUP15_1B4(x)     ((u32)(((x >> 10) & 0x0000001F)))
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP14_1B4        ((u32)0x00000001 << 9)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_AIDE_GROUP13_1B4       ((u32)0x00000007 << 6)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_AIDE_GROUP13_1B4(x)         ((u32)(((x) & 0x00000007) << 6))
#define MBISTCTRL_GET_MBIST_AIDE_GROUP13_1B4(x)     ((u32)(((x >> 6) & 0x00000007)))
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP12_1B4        ((u32)0x00000001 << 5)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP11_1B4        ((u32)0x00000001 << 4)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP10_1B4        ((u32)0x00000001 << 3)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MASK_MBIST_AIDE_GROUP09_1B4       ((u32)0x00000003 << 1)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
#define MBISTCTRL_MBIST_AIDE_GROUP09_1B4(x)         ((u32)(((x) & 0x00000003) << 1))
#define MBISTCTRL_GET_MBIST_AIDE_GROUP09_1B4(x)     ((u32)(((x >> 1) & 0x00000003)))
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP08_1B4        ((u32)0x00000001 << 0)                   /*!< R     0x0
                                                                                              * 1: memory normal test fail
                                                                                              * 0: memory normal test pass
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_START_PAUSE_RAM_0
 * @brief
 * @{
 */
#define MBISTCTRL_BIT_MBIST_PKE_TMEM_SRAM_1B8       ((u32)0x00000001 << 31)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_PKE_MMEM_SRAM_1B8       ((u32)0x00000001 << 30)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_USBOTG_1B8              ((u32)0x00000001 << 29)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SDMMC_PPBECC_1B8        ((u32)0x00000001 << 28)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SDMMC_DMASB1_1B8        ((u32)0x00000001 << 27)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SDIO_1B8                ((u32)0x00000001 << 26)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_GMAC_SRAM_1B8           ((u32)0x00000001 << 25)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_TSC_GROUP00_1B8         ((u32)0x00000001 << 24)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_GE_ZOOML_1B8            ((u32)0x00000001 << 23)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_GE_ZOOM0_1B8            ((u32)0x00000001 << 22)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_GE_BT1_1B8              ((u32)0x00000001 << 21)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_GE_BT0_1B8              ((u32)0x00000001 << 20)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP20_1B8          ((u32)0x00000001 << 19)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP19_1B8          ((u32)0x00000001 << 18)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP18_1B8          ((u32)0x00000001 << 17)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP17_1B8          ((u32)0x00000001 << 16)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP16_1B8          ((u32)0x00000001 << 15)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP15_1B8          ((u32)0x00000001 << 14)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP14_1B8          ((u32)0x00000001 << 13)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP13_1B8          ((u32)0x00000001 << 12)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP12_1B8          ((u32)0x00000001 << 11)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUPL1_1B8          ((u32)0x00000001 << 10)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP10_1B8          ((u32)0x00000001 << 9)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP09_1B8          ((u32)0x00000001 << 8)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP08_1B8          ((u32)0x00000001 << 7)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP07_1B8          ((u32)0x00000001 << 6)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP06_1B8          ((u32)0x00000001 << 5)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP05_1B8          ((u32)0x00000001 << 4)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP04_1B8          ((u32)0x00000001 << 3)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP03_1B8          ((u32)0x00000001 << 2)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP02_1B8          ((u32)0x00000001 << 1)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NN_GROUP01_1B8          ((u32)0x00000001 << 0)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_START_PAUSE_RAM_1
 * @brief
 * @{
 */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP08_1BC         ((u32)0x00000001 << 31)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP07_1BC         ((u32)0x00000001 << 30)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP06_1BC         ((u32)0x00000001 << 29)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENCGROUP05_1BC          ((u32)0x00000001 << 28)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP04_1BC         ((u32)0x00000001 << 27)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENCGROUP03_1BC          ((u32)0x00000001 << 26)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP02_1BC         ((u32)0x00000001 << 25)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENCGROUP01_1BC          ((u32)0x00000001 << 24)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP00_1BC         ((u32)0x00000001 << 23)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISPGROUP_AID_WRAPPER_1BC ((u32)0x00000001 << 22)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_MPRX_1BC      ((u32)0x00000001 << 21)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_VIDEO_AMBAL_1BC ((u32)0x00000001 << 20)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_VIDEO_AMBA0_1BC ((u32)0x00000001 << 19)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISPGROUP_ISP_OSD_1BC    ((u32)0x00000001 << 18)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISP_DMA_1BC   ((u32)0x00000001 << 17)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ZOOM_UP_1BC   ((u32)0x00000001 << 16)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ZOOM_DOWN_1BC ((u32)0x00000001 << 15)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPK_1BC      ((u32)0x00000001 << 14)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPL_1BC      ((u32)0x00000001 << 13)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPJ_1BC      ((u32)0x00000001 << 12)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPH_1BC      ((u32)0x00000001 << 11)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPD_1BC      ((u32)0x00000001 << 10)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPF_1BC      ((u32)0x00000001 << 9)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPE_1BC      ((u32)0x00000001 << 8)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPC_1BC      ((u32)0x00000001 << 7)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPG_1BC      ((u32)0x00000001 << 6)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPB_1BC      ((u32)0x00000001 << 5)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ISP_GROUP_ISPA_1BC      ((u32)0x00000001 << 4)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM0_FW_DCACHE_TAG_SRAM_1BC ((u32)0x00000001 << 3)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM0_FW_DCACHE_SRAM_1BC  ((u32)0x00000001 << 2)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM0_FW_ICACHE_TAG_SRAM_1BC ((u32)0x00000001 << 1)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM0_FW_ICACHE_SRAM_1BC  ((u32)0x00000001 << 0)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_START_PAUSE_RAM_2
 * @brief
 * @{
 */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_DCACHE_TAG_1C0 ((u32)0x00000001 << 31)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_DCACHE_DATA_1C0 ((u32)0x00000001 << 30)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_ICACHE_TAG_1C0 ((u32)0x00000001 << 29)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_ICACHE_DATA_1C0 ((u32)0x00000001 << 28)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP36_1C0         ((u32)0x00000001 << 27)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP35_1C0         ((u32)0x00000001 << 26)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP34_1C0         ((u32)0x00000001 << 25)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP33_1C0         ((u32)0x00000001 << 24)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENCGROUP32_1C0          ((u32)0x00000001 << 23)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP31_1C0         ((u32)0x00000001 << 22)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP30_1C0         ((u32)0x00000001 << 21)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP29_1C0         ((u32)0x00000001 << 20)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP28_1C0         ((u32)0x00000001 << 19)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP27_1C0         ((u32)0x00000001 << 18)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP26_1C0         ((u32)0x00000001 << 17)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP25_1C0         ((u32)0x00000001 << 16)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP24_1C0         ((u32)0x00000001 << 15)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP23_1C0         ((u32)0x00000001 << 14)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP22_1C0         ((u32)0x00000001 << 13)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP21_1C0         ((u32)0x00000001 << 12)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP20_1C0         ((u32)0x00000001 << 11)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP19_1C0         ((u32)0x00000001 << 10)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP18_1C0         ((u32)0x00000001 << 9)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP17_1C0         ((u32)0x00000001 << 8)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP16_1C0         ((u32)0x00000001 << 7)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP15_1C0         ((u32)0x00000001 << 6)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP14_1C0         ((u32)0x00000001 << 5)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP13_1C0         ((u32)0x00000001 << 4)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENCGROUP12_1C0          ((u32)0x00000001 << 3)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUPLL_1C0         ((u32)0x00000001 << 2)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP10_1C0         ((u32)0x00000001 << 1)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_ENC_GROUP09_1C0         ((u32)0x00000001 << 0)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_START_PAUSE_RAM_3
 * @brief
 * @{
 */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP03_1C4        ((u32)0x00000001 << 31)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP02_1C4        ((u32)0x00000001 << 30)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP01_1C4        ((u32)0x00000001 << 29)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_NANDECC_SRAM_1C4        ((u32)0x00000001 << 28)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_VIDEO_SRAM_1C4          ((u32)0x00000001 << 27)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SYSTEM_SRAM_1C4         ((u32)0x00000001 << 26)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_SHARE_SRAM_1C4       ((u32)0x00000001 << 25)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_CPU_1C4              ((u32)0x00000001 << 24)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_07_1C4               ((u32)0x00000001 << 23)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_06_1C4               ((u32)0x00000001 << 22)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT05_1C4                ((u32)0x00000001 << 21)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_04_1C4               ((u32)0x00000001 << 20)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_03_1C4               ((u32)0x00000001 << 19)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_BT_02_1C4               ((u32)0x00000001 << 18)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPON_E0_1C4      ((u32)0x00000001 << 17)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_TPRAMX_WPOFF_E5_1C4     ((u32)0x00000001 << 16)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPOFF_E4_1C4     ((u32)0x00000001 << 15)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPOFF_E3_1C4     ((u32)0x00000001 << 14)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_TPRAMX_WPOFF_E2_1C4     ((u32)0x00000001 << 13)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPOFF_EL_1C4     ((u32)0x00000001 << 12)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WPOFF_E0_1C4     ((u32)0x00000001 << 11)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_SPRAMX_WLK4_E0_1C4      ((u32)0x00000001 << 10)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_DDRC_1P_SRAM_1C4        ((u32)0x00000001 << 9)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_DDRC_2P_SRAM_1C4        ((u32)0x00000001 << 8)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_DTCMDATA_1C4  ((u32)0x00000001 << 7)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_ITCM_DATA_1C4 ((u32)0x00000001 << 6)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_DCACHE_TAG_1C4 ((u32)0x00000001 << 5)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_DCACHE_DATA_1C4 ((u32)0x00000001 << 4)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_ICACHETAG_1C4 ((u32)0x00000001 << 3)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_MEDIA_ICACHE_DATA_1C4 ((u32)0x00000001 << 2)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_DTCM_DATA_1C4 ((u32)0x00000001 << 1)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_KM4_NETWORK_ITCM_DATA_1C4 ((u32)0x00000001 << 0)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_START_PAUSE_RAM_4
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_RESERVED_1C8                 ((u32)0x00007FFF << 17)                  /*!< R     0x0 None */
#define MBISTCTRL_RESERVED_1C8(x)                   ((u32)(((x) & 0x00007FFF) << 17))
#define MBISTCTRL_GET_RESERVED_1C8(x)               ((u32)(((x >> 17) & 0x00007FFF)))
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP15_1C8        ((u32)0x00000001 << 16)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP14_1C8        ((u32)0x00000001 << 15)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP13_1C8        ((u32)0x00000001 << 14)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP12_1C8        ((u32)0x00000001 << 13)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP11_1C8        ((u32)0x00000001 << 12)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP10_1C8        ((u32)0x00000001 << 11)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP09_1C8        ((u32)0x00000001 << 10)                  /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP08_1C8        ((u32)0x00000001 << 9)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP07_1C8        ((u32)0x00000001 << 3)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP06_1C8        ((u32)0x00000001 << 2)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP05_1C8        ((u32)0x00000001 << 1)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
#define MBISTCTRL_BIT_MBIST_AIDE_GROUP04_1C8        ((u32)0x00000001 << 0)                   /*!< R     0x0
                                                                                              * 1: drf test pause
                                                                                              * 0: drf test not paused
                                                                                              */
/** @} */


/** @defgroup REG_MBIST_ROM_0
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_REG_ROM_MISR_DOUT            ((u32)0xFFFFFFFF << 0)                   /*!< R     0x0 reg_misr_dout */
#define MBISTCTRL_REG_ROM_MISR_DOUT(x)              ((u32)(((x) & 0xFFFFFFFF) << 0))
#define MBISTCTRL_GET_REG_ROM_MISR_DOUT(x)          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_MBIST_ROM_1
 * @brief
 * @{
 */
#define MBISTCTRL_MASK_REG_ROM_MISR_DOUT_1D0        ((u32)0xFFFFFFFF << 0)                   /*!< R     0x0 reg_misr_dout */
#define MBISTCTRL_REG_ROM_MISR_DOUT_1D0(x)          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define MBISTCTRL_GET_REG_ROM_MISR_DOUT_1D0(x)      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */




/** @defgroup MBISTCTRL_Exported_Types MBISTCTRL Exported Types
* @{
*/
/**
* @brief MBISTCTRL Register Declaration
*/

typedef struct {
	uint32_t RSVD_NO_FUNCTION0[40];
	__IO uint32_t MBIST_RSTN_RAM_0;                             /*!< Register,  Address offset:0x00A0 */
	__IO uint32_t MBIST_RSTN_RAM_1;                             /*!< Register,  Address offset:0x00A4 */
	__IO uint32_t MBIST_RSTN_RAM_2;                             /*!< Register,  Address offset:0x00A8 */
	__IO uint32_t MBIST_RSTN_RAM_3;                             /*!< Register,  Address offset:0x00AC */
	__IO uint32_t MBIST_RSTN_RAM_4;                             /*!< Register,  Address offset:0x00B0 */
	__IO uint32_t MBIST_RSTN_ROM;                               /*!< Register,  Address offset:0x00B4 */
	__IO uint32_t MBIST_CTRL_0;                                 /*!< Register,  Address offset:0x00B8 */
	__IO uint32_t MBIST_CTRL_1;                                 /*!< Register,  Address offset:0x00BC */
	__I  uint32_t MBIST_CTRL_2;                                 /*!< Register,  Address offset:0x00C0 */
	__I  uint32_t MBIST_STATUS;                                 /*!< Register,  Address offset:0x00C4 */
	__I  uint32_t MBIST_DONE_RAM_0;                             /*!< Register,  Address offset:0x00C8 */
	__I  uint32_t MBIST_DONE_RAM_1;                             /*!< Register,  Address offset:0x00CC */
	__I  uint32_t MBIST_DONE_RAM_2;                             /*!< Register,  Address offset:0x00D0 */
	__I  uint32_t MBIST_DONE_RAM_3;                             /*!< Register,  Address offset:0x00D4 */
	__I  uint32_t MBIST_DONE_RAM_4;                             /*!< Register,  Address offset:0x00D8 */
	__I  uint32_t MBIST_DONE_ROM;                               /*!< Register,  Address offset:0x00DC */
	__I  uint32_t MBIST_NRML_FAIL_RAM_0;                        /*!< Register,  Address offset:0x00E0 */
	__I  uint32_t MBIST_NRML_FAIL_RAM_1;                        /*!< Register,  Address offset:0x00E4 */
	__I  uint32_t MBIST_NRML_FAIL_RAM_2;                        /*!< Register,  Address offset:0x00E8 */
	__I  uint32_t MBIST_NRML_FAIL_RAM_3;                        /*!< Register,  Address offset:0x00EC */
	__I  uint32_t MBIST_NRML_FAIL_RAM_4;                        /*!< Register,  Address offset:0x00F0 */
	__I  uint32_t MBIST_NRML_FAIL_RAM_5;                        /*!< Register,  Address offset:0x00F4 */
	__I  uint32_t MBIST_NRML_FAIL_RAM_6;                        /*!< Register,  Address offset:0x00F8 */
	__I  uint32_t MBIST_NRML_FAIL_RAM_7;                        /*!< Register,  Address offset:0x00FC */
	__I  uint32_t MBIST_NRML_FAIL_RAM_8;                        /*!< Register,  Address offset:0x0100 */
	__I  uint32_t MBIST_NRML_FAIL_RAM_9;                        /*!< Register,  Address offset:0x0104 */
	__I  uint32_t MBIST_NRML_FAIL_RAM_10;                       /*!< Register,  Address offset:0x0108 */
	__I  uint32_t MBIST_NRML_FAIL_RAM_11;                       /*!< Register,  Address offset:0x010C */
	__I  uint32_t MBIST_NRML_FAIL_RAM_12;                       /*!< Register,  Address offset:0x0110 */
	__I  uint32_t MBIST_NRML_FAIL_RAM_13;                       /*!< Register,  Address offset:0x0114 */
	__I  uint32_t MBIST_NRML_FAIL_RAM_14;                       /*!< Register,  Address offset:0x0118 */
	__I  uint32_t MBIST_NRML_FAIL_RAM_15;                       /*!< Register,  Address offset:0x011C */
	__I  uint32_t MBIST_NRML_FAIL_RAM_16;                       /*!< Register,  Address offset:0x0120 */
	__I  uint32_t MBIST_NRML_FAIL_RAM_17;                       /*!< Register,  Address offset:0x0124 */
	__I  uint32_t MBIST_NRML_FAIL_RAM_18;                       /*!< Register,  Address offset:0x0128 */
	__I  uint32_t MBIST_NRML_FAIL_RAM_19;                       /*!< Register,  Address offset:0x012C */
	__I  uint32_t MBIST_NRML_FAIL_RAM_20;                       /*!< Register,  Address offset:0x0130 */
	__I  uint32_t MBIST_NRML_FAIL_RAM_21;                       /*!< Register,  Address offset:0x0134 */
	__I  uint32_t MBIST_NRML_FAIL_RAM_22;                       /*!< Register,  Address offset:0x0138 */
	__I  uint32_t MBIST_NRML_FAIL_RAM_23;                       /*!< Register,  Address offset:0x013C */
	__I  uint32_t MBIST_NRML_FAIL_RAM_24;                       /*!< Register,  Address offset:0x0140 */
	__I  uint32_t MBIST_NRML_FAIL_RAM_25;                       /*!< Register,  Address offset:0x0144 */
	__I  uint32_t MBIST_NRML_FAIL_RAM_26;                       /*!< Register,  Address offset:0x0148 */
	__I  uint32_t MBIST_DRF_FAIL_RAM_0;                         /*!< Register,  Address offset:0x014C */
	__I  uint32_t MBIST_DRF_FAIL_RAM_1;                         /*!< Register,  Address offset:0x0150 */
	__I  uint32_t MBIST_DRF_FAIL_RAM_2;                         /*!< Register,  Address offset:0x0154 */
	__I  uint32_t MBIST_DRF_FAIL_RAM_3;                         /*!< Register,  Address offset:0x0158 */
	__I  uint32_t MBIST_DRF_FAIL_RAM_4;                         /*!< Register,  Address offset:0x015C */
	__I  uint32_t MBIST_DRF_FAIL_RAM_5;                         /*!< Register,  Address offset:0x0160 */
	__I  uint32_t MBIST_DRF_FAIL_RAM_6;                         /*!< Register,  Address offset:0x0164 */
	__I  uint32_t MBIST_DRF_FAIL_RAM_7;                         /*!< Register,  Address offset:0x0168 */
	__I  uint32_t MBIST_DRF_FAIL_RAM_8;                         /*!< Register,  Address offset:0x016C */
	__I  uint32_t MBIST_DRF_FAIL_RAM_9;                         /*!< Register,  Address offset:0x0170 */
	__I  uint32_t MBIST_DRF_FAIL_RAM_10;                        /*!< Register,  Address offset:0x0174 */
	__I  uint32_t MBIST_DRF_FAIL_RAM_11;                        /*!< Register,  Address offset:0x0178 */
	__I  uint32_t MBIST_DRF_FAIL_RAM_12;                        /*!< Register,  Address offset:0x017C */
	__I  uint32_t MBIST_DRF_FAIL_RAM_13;                        /*!< Register,  Address offset:0x0180 */
	__I  uint32_t MBIST_DRF_FAIL_RAM_14;                        /*!< Register,  Address offset:0x0184 */
	__I  uint32_t MBIST_DRF_FAIL_RAM_15;                        /*!< Register,  Address offset:0x0188 */
	__I  uint32_t MBIST_DRF_FAIL_RAM_16;                        /*!< Register,  Address offset:0x018C */
	__I  uint32_t MBIST_DRF_FAIL_RAM_17;                        /*!< Register,  Address offset:0x0190 */
	__I  uint32_t MBIST_DRF_FAIL_RAM_18;                        /*!< Register,  Address offset:0x0194 */
	__I  uint32_t MBIST_DRF_FAIL_RAM_19;                        /*!< Register,  Address offset:0x0198 */
	__I  uint32_t MBIST_DRF_FAIL_RAM_20;                        /*!< Register,  Address offset:0x019C */
	__I  uint32_t MBIST_DRF_FAIL_RAM_21;                        /*!< Register,  Address offset:0x01A0 */
	__I  uint32_t MBIST_DRF_FAIL_RAM_22;                        /*!< Register,  Address offset:0x01A4 */
	__I  uint32_t MBIST_DRF_FAIL_RAM_23;                        /*!< Register,  Address offset:0x01A8 */
	__I  uint32_t MBIST_DRF_FAIL_RAM_24;                        /*!< Register,  Address offset:0x01AC */
	__I  uint32_t MBIST_DRF_FAIL_RAM_25;                        /*!< Register,  Address offset:0x01B0 */
	__I  uint32_t MBIST_DRF_FAIL_RAM_26;                        /*!< Register,  Address offset:0x01B4 */
	__I  uint32_t MBIST_START_PAUSE_RAM_0;                      /*!< Register,  Address offset:0x01B8 */
	__I  uint32_t MBIST_START_PAUSE_RAM_1;                      /*!< Register,  Address offset:0x01BC */
	__I  uint32_t MBIST_START_PAUSE_RAM_2;                      /*!< Register,  Address offset:0x01C0 */
	__I  uint32_t MBIST_START_PAUSE_RAM_3;                      /*!< Register,  Address offset:0x01C4 */
	__I  uint32_t MBIST_START_PAUSE_RAM_4;                      /*!< Register,  Address offset:0x01C8 */
	__I  uint32_t MBIST_ROM_0;                                  /*!< Register,  Address offset:0x01CC */
	__I  uint32_t MBIST_ROM_1;                                  /*!< Register,  Address offset:0x01D0 */
} MBISTCTRL_TypeDef;

#define MBISTCTRL_BASE ((MBISTCTRL_TypeDef *) (0x6020C800))
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
#endif  // SYSREG_MBIST_H
