/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_AON_H
#define SYSREG_AON_H

/* AUTO_GEN_START */

/* AON is a part of AON */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup AON
* @brief AON driver modules
* @{
*/


/** @defgroup REG_AON_PWR_CTRL
 * @brief
 * @{
 */
#define AON_BIT_AON_WLOFF_PC_EN                     ((u32)0x00000001 << 2)                   /*!< R/W   0  
                                                                                              * WIFIOFF domain power control
                                                                                              * 0:Disable, 1: Enable
                                                                                              */
#define AON_BIT_AON_SYSON_LPC_EN                    ((u32)0x00000001 << 1)                   /*!< R/W   0  
                                                                                              * Peripheral on domain large power
                                                                                              * 0:Disable, 1: Enable
                                                                                              */
#define AON_BIT_AON_SYSON_SPC_EN                    ((u32)0x00000001 << 0)                   /*!< R/W   0  
                                                                                              * Peripheral on domain small power
                                                                                              * 0:Disable, 1: Enable
                                                                                              */
/** @} */


/** @defgroup REG_AON_ISO_CTRL
 * @brief
 * @{
 */
#define AON_BIT_AON_SYS_ISO_XTAL                    ((u32)0x00000001 << 4)                   /*!< R/W     1
                                                                                              * Enable ISO cell for data path from XTAL PD. 
                                                                                              * + Set to 1 before power off XTAL PD.
                                                                                              * + Set to 0 after power on XTAL PD.
                                                                                              */
#define AON_BIT_AON_SYS_ISO_SYSON                   ((u32)0x00000001 << 3)                   /*!< R/W   1  
                                                                                              * Enable ISO cell for data path from SYSON PD. 
                                                                                              * + Set to 1 before power off SYSON PD.
                                                                                              * + Set to 0 after power on SYSON PD.
                                                                                              */
#define AON_BIT_AON_SYS_ISO_CORE                    ((u32)0x00000001 << 2)                   /*!< R/W   1  
                                                                                              * Enable ISO cell for data path from CORE PD. 
                                                                                              * + Set to 1 before power off CORE PD.
                                                                                              * + Set to 0 after power on CORE PD.
                                                                                              */
#define AON_BIT_SYS_ISO_WLOFF                       ((u32)0x00000001 << 1)                   /*!< R/W   1  
                                                                                              * Enable ISO cell for data path from WLOFF PD. 
                                                                                              * + Set to 1 before power off WLOFF PD.
                                                                                              * + Set to 0 after power on WLOFF PD.
                                                                                              */
#define AON_BIT_SYS_ISO_MCUOFF                      ((u32)0x00000001 << 0)                   /*!< R/W   1  
                                                                                              * Enable ISO cell for data path from MCUOFF PD. 
                                                                                              * + Set to 1 before power off MCUOFF PD.
                                                                                              * + Set to 0 after power on MCUOFF PD.
                                                                                              */
/** @} */


/** @defgroup REG_AON_32KCLK_CTRL
 * @brief
 * @{
 */
#define AON_BIT_SRC_32K_CLK_VLD                     ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define AON_BIT_SRC_32K_CLK_DIV_DONE                ((u32)0x00000001 << 23)                  /*!< R     0x0 Clock divider ratio changing process is done */
#define AON_BIT_SRC_32K_CLK_EN                      ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock enable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Program clock select bits.
                                                                                              */
#define AON_MASK_SRC_32K_CLK_SEL                    ((u32)0x00000007 << 16)                  /*!< R/W   0x4
                                                                                              * Clock select:
                                                                                              * 0x0: SDM_32K
                                                                                              * 0x1: HSSDM
                                                                                              * 0x2: XTAL_40M_div
                                                                                              * 0x3: XTAL_32K
                                                                                              * 0x4: OSC_131K_div4
                                                                                              * 0x5: OSC_131K
                                                                                              * Others: 1'b0
                                                                                              */
#define AON_SRC_32K_CLK_SEL(x)                      ((u32)(((x) & 0x00000007) << 16))
#define AON_GET_SRC_32K_CLK_SEL(x)                  ((u32)(((x >> 16) & 0x00000007)))
#define AON_BIT_SRC_32K_CLK_DIVTRG                  ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define AON_MASK_SRC_32K_CLK_DIVN                   ((u32)0x0000003F << 0)                   /*!< R/W   0x0 Clock divider factor. */
#define AON_SRC_32K_CLK_DIVN(x)                     ((u32)(((x) & 0x0000003F) << 0))
#define AON_GET_SRC_32K_CLK_DIVN(x)                 ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_AON_AONCLK_CTRL
 * @brief
 * @{
 */
#define AON_BIT_AON_CLK_VLD                         ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define AON_BIT_AON_CLK_DIV_DONE                    ((u32)0x00000001 << 23)                  /*!< R     0x0 Clock divider ratio changing process is done */
#define AON_BIT_AON_CLK_EN                          ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock enable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Program clock select bits.
                                                                                              */
#define AON_MASK_AON_CLK_SEL                        ((u32)0x00000007 << 16)                  /*!< R/W   0x1
                                                                                              * Clock select:
                                                                                              * 0x0: OSC 4M
                                                                                              * 0x1: REGU 100K
                                                                                              * 0x2: SDM 32K
                                                                                              * 0x3: XTAL 40M
                                                                                              * Others: 1'b0
                                                                                              */
#define AON_AON_CLK_SEL(x)                          ((u32)(((x) & 0x00000007) << 16))
#define AON_GET_AON_CLK_SEL(x)                      ((u32)(((x >> 16) & 0x00000007)))
#define AON_BIT_AON_CLK_DIVTRG                      ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define AON_MASK_AON_CLK_DIVN                       ((u32)0x0000003F << 0)                   /*!< R/W   0x0 Clock divider factor. */
#define AON_AON_CLK_DIVN(x)                         ((u32)(((x) & 0x0000003F) << 0))
#define AON_GET_AON_CLK_DIVN(x)                     ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_AON_DUMMY0
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_AON_CLK_CTRL
 * @brief
 * @{
 */
#define AON_BIT_AON_RET_MEM_AUTO_GATE               ((u32)0x00000001 << 30)                  /*!< R/W   0x0 1: disable memory clock gating in mbist wrap */
#define AON_BIT_AON_COMP_INT_CLK_SEL                ((u32)0x00000001 << 29)                  /*!< R/W   0x0
                                                                                              * Comparator interrupt clock source selection,
                                                                                              * 0:pclk, 1:sclk
                                                                                              */
#define AON_BIT_AON_GPIO_INT_CLK_SEL                ((u32)0x00000001 << 28)                  /*!< R/W   0x0 0:pclk, 1:sclk */
#define AON_BIT_AON_PON_REG_CLKEN                   ((u32)0x00000001 << 22)                  /*!< R/W   0x0 Clock enable for PON_REG CLK */
#define AON_BIT_AON_WDT_SCLKEN                      ((u32)0x00000001 << 21)                  /*!< R/W   0x0 Clock enable for AON WDT SCLK */
#define AON_BIT_AON_WDT_PCLKEN                      ((u32)0x00000001 << 20)                  /*!< R/W   0x0 Clock enable for AON WDT PCLK */
#define AON_BIT_AON_RTC_SCLKEN                      ((u32)0x00000001 << 19)                  /*!< R/W   0x0 Clock enable for AON RTC SCLK */
#define AON_BIT_AON_RTC_PCLKEN                      ((u32)0x00000001 << 18)                  /*!< R/W   0x0 Clock enable for AON RTC PCLK */
#define AON_BIT_AON_GPIO_SCLKEN                     ((u32)0x00000001 << 17)                  /*!< R/W   0x0 Clock enable for AON GPIO SCLK */
#define AON_BIT_AON_GPIO_PCLKEN                     ((u32)0x00000001 << 16)                  /*!< R/W   0x0 Clock enable for AON GPIO PCLK */
#define AON_BIT_AON_TIMER_SCLKEN                    ((u32)0x00000001 << 15)                  /*!< R/W   0x0 Clock enable for AON TIMER SCLK */
#define AON_BIT_AON_TIMER_PCLKEN                    ((u32)0x00000001 << 14)                  /*!< R/W   0x0 Clock enable for AON TIMER PCLK */
#define AON_BIT_AON_LS_SDM_XCLKEN                   ((u32)0x00000001 << 13)                  /*!< R/W   0x0 Clock enable for AON LS_SDM XTAL CLK */
#define AON_BIT_AON_LS_SDM_OCLKEN                   ((u32)0x00000001 << 12)                  /*!< R/W   0x0 Clock enable for AON LS_SDM OSC CLK */
#define AON_BIT_AON_LS_SDM_PCLKEN                   ((u32)0x00000001 << 11)                  /*!< R/W   0x0 Clock enable for AON LS_SDM PCLK */
#define AON_BIT_AON_HS_SDM_XCLKEN                   ((u32)0x00000001 << 10)                  /*!< R/W   0x0 Clock enable for AON HS_SDM XTAL CLK */
#define AON_BIT_AON_HS_SDM_PCLKEN                   ((u32)0x00000001 << 9)                   /*!< R/W   0x0 Clock enable for AON HS_SDM PCLK */
#define AON_BIT_AON_SIC_SCLKEN                      ((u32)0x00000001 << 8)                   /*!< R/W   0x0 Clock enable for AON SIC SCLK */
#define AON_BIT_AON_SIC_PCLKEN                      ((u32)0x00000001 << 7)                   /*!< R/W   0x0 Clock enable for AON SIC HCLK */
#define AON_BIT_AON_COMP_SCLKEN                     ((u32)0x00000001 << 6)                   /*!< R/W   0x0 Clock enable for AON COMP SCLK */
#define AON_BIT_AON_COMP_PCLKEN                     ((u32)0x00000001 << 5)                   /*!< R/W   0x0 Clock enable for AON COMP PCLK */
#define AON_BIT_AON_PMC_SCLKEN                      ((u32)0x00000001 << 4)                   /*!< R/W   0x0 Clock enable for AON PMC SCLK */
#define AON_BIT_AON_OTPC_SCLKEN                     ((u32)0x00000001 << 3)                   /*!< R/W   0x0 NOT USED */
#define AON_BIT_AON_OTPC_WCLKEN                     ((u32)0x00000001 << 2)                   /*!< R/W   0x0 NOT USED */
#define AON_BIT_AON_WL_ANA_CLKEN                    ((u32)0x00000001 << 1)                   /*!< R/W   0x0 1: Enable ana clock to WLAN MAC */
#define AON_BIT_AON_WL_32K_CLKEN                    ((u32)0x00000001 << 0)                   /*!< R/W   0x0 1: Enable 32KHz clock to WLAN MAC */
/** @} */


/** @defgroup REG_AON_DUMMY1
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_AON_FUNC_CTRL1
 * @brief
 * @{
 */
#define AON_BIT_AON_FEN_A0                          ((u32)0x00000001 << 20)                  /*!< R/W   0  
                                                                                              * WLON enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define AON_BIT_AON_FEN_A2_0                        ((u32)0x00000001 << 19)                  /*!< R/W   0  
                                                                                              * AON RTC Enable (need delay 4ms?)
                                                                                              * 0: Keep AON RTC in reset
                                                                                              * 1: Release reset to AON RTC 
                                                                                              */
#define AON_BIT_AON_FEN_A2_1                        ((u32)0x00000001 << 18)                  /*!< R/W   0  
                                                                                              * AON GPIO function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define AON_BIT_AON_FEN_A2_2                        ((u32)0x00000001 << 17)                  /*!< R/W     0
                                                                                              * AON TIMER function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define AON_BIT_AON_FEN_A2_3                        ((u32)0x00000001 << 16)                  /*!< R/W   0  
                                                                                              * AON LS SDM function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define AON_BIT_AON_FEN_A2_4                        ((u32)0x00000001 << 15)                  /*!< R/W     0
                                                                                              * AON HS SDM function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define AON_BIT_AON_FEN_A2_5                        ((u32)0x00000001 << 14)                  /*!< R/W     0
                                                                                              * AON SIC function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define AON_BIT_AON_FEN_A2_6                        ((u32)0x00000001 << 13)                  /*!< R/W   0  
                                                                                              * AON COMP function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define AON_BIT_AON_FEN_A2_7                        ((u32)0x00000001 << 12)                  /*!< R/W     0 No use */
#define AON_BIT_AON_FEN_A1                          ((u32)0x00000001 << 11)                  /*!< R/W     0
                                                                                              * PON SYSTEM Register function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define AON_BIT_AON_SIC_MST__FEN                    ((u32)0x00000001 << 10)                  /*!< R/W   0  
                                                                                              * AON SIC BUS function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
/** @} */


/** @defgroup REG_AON_PLATFORM_VER
 * @brief
 * @{
 */
#define AON_MASK_AON_PLATFORM_VER                   ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0   Platform Version number */
#define AON_AON_PLATFORM_VER(x)                     ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AON_GET_AON_PLATFORM_VER(x)                 ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AON_DUMMY1
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_AON_DUMMY8
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_AON_LSFIF_CMD
 * @brief
 * @{
 */
#define AON_BIT_AON_LSFIF_POLL                      ((u32)0x00000001 << 31)                  /*!< R/W     0 LSF (low speed function) register access polling bit. Set this bit to do LSF register read or write transfer depend on BIT_AON_LSFIF_WR. When transfer done, this bit will be clear by HW */
#define AON_MASK_LSF_SEL                            ((u32)0x00000007 << 28)                  /*!< R/W     0
                                                                                              * 000: RTC
                                                                                              * 001: LS SDM
                                                                                              * 010: HS SDM
                                                                                              * 011: AON Comparator
                                                                                              * 100: RSVD
                                                                                              */
#define AON_LSF_SEL(x)                              ((u32)(((x) & 0x00000007) << 28))
#define AON_GET_LSF_SEL(x)                          ((u32)(((x >> 28) & 0x00000007)))
#define AON_BIT_AON_LSFIF_WR                        ((u32)0x00000001 << 27)                  /*!< R/W     0 LSF register write transfer indicator.1: LSF register write transfer, 0: LSF register read transfer */
#define AON_MASK_AON_LSFIF_WE                       ((u32)0x0000000F << 23)                  /*!< R/W     0 LSF register byte write enable. This field is valid during LSF register write transfer, and is ignored during LSF register read transfer */
#define AON_AON_LSFIF_WE(x)                         ((u32)(((x) & 0x0000000F) << 23))
#define AON_GET_AON_LSFIF_WE(x)                     ((u32)(((x >> 23) & 0x0000000F)))
#define AON_MASK_AON_LSFIF_CMD_DUMMY_0              ((u32)0x0000007F << 16)                  /*!< R/W     0 Dummy */
#define AON_AON_LSFIF_CMD_DUMMY_0(x)                ((u32)(((x) & 0x0000007F) << 16))
#define AON_GET_AON_LSFIF_CMD_DUMMY_0(x)            ((u32)(((x >> 16) & 0x0000007F)))
#define AON_MASK_AON_LSFIF_AD                       ((u32)0x0000FFFF << 0)                   /*!< R/W     0
                                                                                              * LSF register access address.
                                                                                              * BIT_AON_LSFIF_AD[15:8]=8'h00: Indirect register base address
                                                                                              */
#define AON_AON_LSFIF_AD(x)                         ((u32)(((x) & 0x0000FFFF) << 0))
#define AON_GET_AON_LSFIF_AD(x)                     ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_AON_LSFIF_RWD
 * @brief
 * @{
 */
#define AON_MASK_AON_LSFIF_RWD                      ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0   LSF(low speed function) write */
#define AON_AON_LSFIF_RWD(x)                        ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AON_GET_AON_LSFIF_RWD(x)                    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AON_TEST_MUX_CTRL
 * @brief
 * @{
 */
#define AON_BIT_TEST_MOD31_EN                       ((u32)0x00000001 << 31)                  /*!< R/W     0 1: enable test mode 31 mux */
#define AON_BIT_TEST_MOD30_EN                       ((u32)0x00000001 << 30)                  /*!< R/W     0 1: enable test mode 30 mux */
#define AON_BIT_TEST_MOD29_EN                       ((u32)0x00000001 << 29)                  /*!< R/W     0 1: enable test mode 29 mux */
#define AON_BIT_TEST_MOD28_EN                       ((u32)0x00000001 << 28)                  /*!< R/W     0 1: enable test mode 28 mux */
#define AON_BIT_TEST_MOD27_EN                       ((u32)0x00000001 << 27)                  /*!< R/W     0 1: enable test mode 27 mux */
#define AON_BIT_TEST_MOD26_EN                       ((u32)0x00000001 << 26)                  /*!< R/W     0 1: enable test mode 26 mux */
#define AON_BIT_TEST_MOD25_EN                       ((u32)0x00000001 << 25)                  /*!< R/W     0 1: enable test mode 25 mux */
#define AON_BIT_SDIO_HOST_TEST_MUX_EN               ((u32)0x00000001 << 24)                  /*!< R/W     0 1: enable SDIO HOST test mux */
#define AON_BIT_SD_HOST_TEST_MUX_EN                 ((u32)0x00000001 << 23)                  /*!< R/W     0 1: enable SD HOST test mux */
#define AON_BIT_ETHERNET_TEST_MUX_EN                ((u32)0x00000001 << 22)                  /*!< R/W     0 1: enable Ethernet PHY test mux */
#define AON_BIT_AUDIO_TEST_MUX_EN                   ((u32)0x00000001 << 21)                  /*!< R/W     0 1: enable AUDIO CODEC test mux */
#define AON_BIT_PLL_TEST_MUX_EN                     ((u32)0x00000001 << 20)                  /*!< R/W     0 1: enable PLL test mux */
#define AON_BIT_AUX_TEST_MUX_EN                     ((u32)0x00000001 << 19)                  /*!< R/W     0 1: enable AUX test mux */
#define AON_BIT_MEM_SPEED_MODE                      ((u32)0x00000001 << 18)                  /*!< R/W     0 1: enable MSM test mode */
#define AON_BIT_WL_BT_RF_TEST_MUX_EN                ((u32)0x00000001 << 17)                  /*!< R/W     0 1: enable WLAN BT RF test mux */
#define AON_BIT_REGU_TEST_MUX_EN                    ((u32)0x00000001 << 16)                  /*!< R/W   0   1: enable REGU test mux */
#define AON_BIT_TEST_MOD15_EN                       ((u32)0x00000001 << 15)                  /*!< R/W     0 1: enable test mode 15 mux */
#define AON_BIT_TEST_MOD14_EN                       ((u32)0x00000001 << 14)                  /*!< R/W     0 1: enable test mode 14 mux */
#define AON_BIT_TEST_MOD13_EN                       ((u32)0x00000001 << 13)                  /*!< R/W     0 1: enable test mode 13 mux */
#define AON_BIT_TEST_MOD12_EN                       ((u32)0x00000001 << 12)                  /*!< R/W     0 1: enable test mode 12 mux */
#define AON_BIT_TEST_MOD11_EN                       ((u32)0x00000001 << 11)                  /*!< R/W     0 1: enable test mode 11 mux */
#define AON_BIT_TEST_MOD10_EN                       ((u32)0x00000001 << 10)                  /*!< R/W     0 1: enable test mode 10 mux */
#define AON_BIT_TEST_MOD9_EN                        ((u32)0x00000001 << 9)                   /*!< R/W     0 1: enable test mode 9 mux */
#define AON_BIT_TEST_MOD8_EN                        ((u32)0x00000001 << 8)                   /*!< R/W     0 1: enable test mode 8 mux */
#define AON_BIT_TEST_MOD7_EN                        ((u32)0x00000001 << 7)                   /*!< R/W   0   1: enable test mode 7 mux */
#define AON_BIT_TEST_MOD6_EN                        ((u32)0x00000001 << 6)                   /*!< R/W   0   1: enable test mode 6 mux */
#define AON_BIT_SCAN_MOD5_EN                        ((u32)0x00000001 << 5)                   /*!< R/W     0 1: enable FT3 scan test mux */
#define AON_BIT_SCAN_MOD4_EN                        ((u32)0x00000001 << 4)                   /*!< R/W     0 1: enable FT2 scan test mux */
#define AON_BIT_SCAN_MOD3_EN                        ((u32)0x00000001 << 3)                   /*!< R/W     0 1: enable FT1 scan test mux */
#define AON_BIT_SCAN_MOD2_EN                        ((u32)0x00000001 << 2)                   /*!< R/W     0 1: enable FT0 scan test mux */
#define AON_BIT_SCAN_MOD1_EN                        ((u32)0x00000001 << 1)                   /*!< R/W     0 1: enable CP1 scan test mux */
#define AON_BIT_SCAN_MOD0_EN                        ((u32)0x00000001 << 0)                   /*!< R/W   0   1: enable CP0 scan test mux */
/** @} */


/** @defgroup REG_AON_TEST_MUX_CTRL1
 * @brief
 * @{
 */
#define AON_MASK_TEST                               ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 FOR SCAN/DFT purpose */
#define AON_TEST(x)                                 ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AON_GET_TEST(x)                             ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AON_TEST_MUX_CTRL2
 * @brief
 * @{
 */
#define AON_BIT_CA32_AXIM_SYNC_TDF_EN               ((u32)0x00000001 << 1)                   /*!< R/W     0 1: Enable TDF test for CA32 core vs AXIM sync paths. */
#define AON_BIT_SCAN_MSM_EN                         ((u32)0x00000001 << 0)                   /*!< R/W     0 1: enable MSM test mode */
/** @} */


/** @defgroup REG_AON_ATLOAD_CTRL2
 * @brief
 * @{
 */
#define AON_MASK_OTP2                               ((u32)0x0000FFFF << 16)                  /*!< R/W     0 For OTP Use */
#define AON_OTP2(x)                                 ((u32)(((x) & 0x0000FFFF) << 16))
#define AON_GET_OTP2(x)                             ((u32)(((x >> 16) & 0x0000FFFF)))
#define AON_MASK_EFUSE_ID1                          ((u32)0x000000FF << 8)                   /*!< R/W   0   Efuse ID byte1, expect ID: 0x87 */
#define AON_EFUSE_ID1(x)                            ((u32)(((x) & 0x000000FF) << 8))
#define AON_GET_EFUSE_ID1(x)                        ((u32)(((x >> 8) & 0x000000FF)))
#define AON_MASK_EFUSE_ID0                          ((u32)0x000000FF << 0)                   /*!< R/W   0   Efuse ID byte0, expect ID:  0x35 */
#define AON_EFUSE_ID0(x)                            ((u32)(((x) & 0x000000FF) << 0))
#define AON_GET_EFUSE_ID0(x)                        ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_AON_SYS_INFO0
 * @brief
 * @{
 */
#define AON_BIT_SYSCFG_BD_PKG_SEL                   ((u32)0x00000001 << 31)                  /*!< R       0
                                                                                              * Trapped PKG_ENG_SEL value
                                                                                              * 0: normal package; 1: engineering mode
                                                                                              */
#define AON_BIT_CHIP_EN                             ((u32)0x00000001 << 30)                  /*!< R     1   CHIP_EN PIN input value */
#define AON_BIT_BD_OTP3                             ((u32)0x00000001 << 27)                  /*!< R       0 Report from bonding option pin */
#define AON_BIT_BD_OTP2                             ((u32)0x00000001 << 26)                  /*!< R       0 None */
#define AON_BIT_BD_OTP1                             ((u32)0x00000001 << 25)                  /*!< R       0 None */
#define AON_BIT_BD_OTP0                             ((u32)0x00000001 << 24)                  /*!< R       0 None */
#define AON_BIT_CHIP_TYPE                           ((u32)0x00000001 << 16)                  /*!< R     0   1: Test chip;  0:MP */
#define AON_MASK_VENDOR_ID                          ((u32)0x0000000F << 8)                   /*!< R     0x5
                                                                                              * BIT_VENDOR_ID[3:2] : Process
                                                                                              *         00: TSMC , 01: SMIC, 10: UMC
                                                                                              * BIT_VENDOR_ID[1] : rsve
                                                                                              * BIT_VENDOR_ID[0] :
                                                                                              *          0 : BT do not exist in IC
                                                                                              *          1 : BT exist in IC
                                                                                              */
#define AON_VENDOR_ID(x)                            ((u32)(((x) & 0x0000000F) << 8))
#define AON_GET_VENDOR_ID(x)                        ((u32)(((x >> 8) & 0x0000000F)))
#define AON_MASK_CHIP_VER                           ((u32)0x0000000F << 4)                   /*!< R     0  
                                                                                              * Chip version-8735B
                                                                                              *     0000: A-cut
                                                                                              *     0001: B-cut
                                                                                              *     0010: C-cut
                                                                                              *     0011: D-cut
                                                                                              * 
                                                                                              */
#define AON_CHIP_VER(x)                             ((u32)(((x) & 0x0000000F) << 4))
#define AON_GET_CHIP_VER(x)                         ((u32)(((x >> 4) & 0x0000000F)))
#define AON_MASK_RF_RL_ID                           ((u32)0x0000000F << 0)                   /*!< R     0   Vendor ID defined in RF */
#define AON_RF_RL_ID(x)                             ((u32)(((x) & 0x0000000F) << 0))
#define AON_GET_RF_RL_ID(x)                         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_AON_SYS_INFO1
 * @brief
 * @{
 */
#define AON_BIT_SYSCFG_TRP_SRCLDO_SEL               ((u32)0x00000001 << 27)                  /*!< R       0
                                                                                              * Trapped Selection for SRC
                                                                                              * 0: SWR mode; 1: LDO mode
                                                                                              */
#define AON_BIT_SYSCFG_TRP_TEST_MODE_SEL            ((u32)0x00000001 << 26)                  /*!< R       0
                                                                                              * Trapped Selection for Test mode
                                                                                              * 0: normal mode; 1: test mode 
                                                                                              */
#define AON_BIT_SYSCFG_TRP_EFUSE_AUTOLOAD_DIS       ((u32)0x00000001 << 24)                  /*!< R       0
                                                                                              * 1: disable efuse autoload
                                                                                              * 0: enable efuse autoload
                                                                                              */
#define AON_BIT_SYS_CPUPLL_CLK_RDY                  ((u32)0x00000001 << 22)                  /*!< R       0 CPUPLL  Clock Stable, 1: Clock Stable */
#define AON_BIT_AON_SYS_INFO1_DUMMY_0               ((u32)0x00000001 << 21)                  /*!< R       0 None */
#define AON_BIT_SYS_PERIPLL_CLK_RDY                 ((u32)0x00000001 << 20)                  /*!< R       0 PERIPLL  Clock Stable, 1: Clock Stable */
#define AON_MASK_AON_SYS_INFO1_DUMMY_1              ((u32)0x00000007 << 17)                  /*!< R       0 None */
#define AON_AON_SYS_INFO1_DUMMY_1(x)                ((u32)(((x) & 0x00000007) << 17))
#define AON_GET_AON_SYS_INFO1_DUMMY_1(x)            ((u32)(((x >> 17) & 0x00000007)))
#define AON_BIT_V09_VLD                             ((u32)0x00000001 << 16)                  /*!< R       1 0.9V Power Ready, 1: Power Ready */
#define AON_MASK_AON_SYS_INFO1_DUMMY_2              ((u32)0x0000000F << 12)                  /*!< R       0 None */
#define AON_AON_SYS_INFO1_DUMMY_2(x)                ((u32)(((x) & 0x0000000F) << 12))
#define AON_GET_AON_SYS_INFO1_DUMMY_2(x)            ((u32)(((x >> 12) & 0x0000000F)))
#define AON_BIT_SYS_SYS2PLL_CLK_RDY                 ((u32)0x00000001 << 11)                  /*!< R       0 SYS2PLL  Clock Stable, 1: Clock Stable */
#define AON_BIT_SYS_128K_RDY                        ((u32)0x00000001 << 10)                  /*!< R       0 1: 128K RDY */
#define AON_BIT_SYS_SYS1PLL_CLK_RDY                 ((u32)0x00000001 << 9)                   /*!< R       0 SYS1PLL  Clock Stable, 1: Clock Stable */
#define AON_BIT_SYS_XCLK_VLD                        ((u32)0x00000001 << 8)                   /*!< R       0 Xtal Clock Stable, 1: Clock Stable */
#define AON_MASK_SYSCFG_ROM_INFO                    ((u32)0x000000FF << 0)                   /*!< R/W Once   0 ROM Information */
#define AON_SYSCFG_ROM_INFO(x)                      ((u32)(((x) & 0x000000FF) << 0))
#define AON_GET_SYSCFG_ROM_INFO(x)                  ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_AON_OTP_SYSCFG2
 * @brief
 * @{
 */
#define AON_MASK_DBG_PORT1_SEL                      ((u32)0x0000000F << 28)                  /*!< R/W     0
                                                                                              * Debug port group 1 data sel
                                                                                              * 0000 : debug data 3:0 
                                                                                              * 0001 : debug data 7:4 
                                                                                              * 0010 : debug data 11:8
                                                                                              * 0011 :  debug data 15:12
                                                                                              * 0100 :  debug data 19:16
                                                                                              * 0101 :  debug data 23:20
                                                                                              * 0110 :  debug data 27:24
                                                                                              * 0111 :  debug data 31:28
                                                                                              */
#define AON_DBG_PORT1_SEL(x)                        ((u32)(((x) & 0x0000000F) << 28))
#define AON_GET_DBG_PORT1_SEL(x)                    ((u32)(((x >> 28) & 0x0000000F)))
#define AON_MASK_DBG_PORT0_SEL                      ((u32)0x0000000F << 24)                  /*!< R/W     0
                                                                                              * Debug port group 0 data sel
                                                                                              * 0000 : debug data 3:0 
                                                                                              * 0001 : debug data 7:4 
                                                                                              * 0010 : debug data 11:8
                                                                                              * 0011 :  debug data 15:12
                                                                                              * 0100 :  debug data 19:16
                                                                                              * 0101 :  debug data 23:20
                                                                                              * 0110 :  debug data 27:24
                                                                                              * 0111 :  debug data 31:28
                                                                                              */
#define AON_DBG_PORT0_SEL(x)                        ((u32)(((x) & 0x0000000F) << 24))
#define AON_GET_DBG_PORT0_SEL(x)                    ((u32)(((x >> 24) & 0x0000000F)))
#define AON_BIT_DBG_PORT7_EN                        ((u32)0x00000001 << 23)                  /*!< R/W     0 1:Debug port group 7 enable */
#define AON_BIT_DBG_PORT6_EN                        ((u32)0x00000001 << 22)                  /*!< R/W     0 1:Debug port group 6 enable */
#define AON_BIT_DBG_PORT5_EN                        ((u32)0x00000001 << 21)                  /*!< R/W     0 1:Debug port group 5 enable */
#define AON_BIT_DBG_PORT4_EN                        ((u32)0x00000001 << 20)                  /*!< R/W     0 1:Debug port group 4 enable */
#define AON_BIT_DBG_PORT3_EN                        ((u32)0x00000001 << 19)                  /*!< R/W     0 1:Debug port group 3 enable */
#define AON_BIT_DBG_PORT2_EN                        ((u32)0x00000001 << 18)                  /*!< R/W     0 1:Debug port group 2 enable */
#define AON_BIT_DBG_PORT1_EN                        ((u32)0x00000001 << 17)                  /*!< R/W     0 1:Debug port group 1 enable */
#define AON_BIT_DBG_PORT0_EN                        ((u32)0x00000001 << 16)                  /*!< R/W     0 1:Debug port group 0 enable */
#define AON_MASK_DBG_PORT                           ((u32)0x000000FF << 8)                   /*!< R/W     0
                                                                                              * Debug port function selection:
                                                                                              * 8'h00: OTP
                                                                                              * 8'h01: SIC
                                                                                              * 8'h02: PMC
                                                                                              * 8'h03: Comparator
                                                                                              * 8'h04: HS SDM
                                                                                              * 8'h05: LS SDM
                                                                                              * 8'h06: AON GPIO heart beat
                                                                                              * 8'h08: LOGUART
                                                                                              * 8'h80: SPI_S0
                                                                                              * 8'h81: SPI_S1
                                                                                              * 8'h82: I2C0
                                                                                              * 8'h83: I2C1
                                                                                              * 8'h84: I2C2
                                                                                              * 8'h85: I2C3
                                                                                              * 8'h86: UART1
                                                                                              * 8'h87: UART2
                                                                                              * 8'h88: UART0
                                                                                              * 8'h89: SPORT0
                                                                                              * 8'h8A: SPORT1
                                                                                              * 8'h8B: DDRPHY (Low 32bit)
                                                                                              * 8'h8C: DDRPHY (High 32bit)
                                                                                              * 8'h8D: SD Host
                                                                                              * 8'h8E: Graphic Engine
                                                                                              * 8'h8F: Digital audio codec
                                                                                              * 8'h90: PSRAM PHY
                                                                                              * 8'h91: ADC
                                                                                              * 8'h92: SGPIO
                                                                                              * 8'h93: WLAFE0
                                                                                              * 8'h94: WLAFE1
                                                                                              * 8'h95: RFAFE
                                                                                              * 8'h96: LX arbiter
                                                                                              * 8'h97: BT UART
                                                                                              * 8'h98: CA32MP1
                                                                                              * 8'h99: TRNG
                                                                                              * 8'h9A: BTTOP
                                                                                              * 8'h9B: GMAC
                                                                                              * 8'h9C: USB OTG
                                                                                              * 8'h9D: ISP (Low 32bit)
                                                                                              * 8'h9E: ISP (High 32bit)
                                                                                              * 8'h9F: PLL (SYS1/SYS2/PERI/CPUPLL)
                                                                                              * 8'hA0: WLAN BB
                                                                                              * 8'hA1: WLAN MAC
                                                                                              * 8'hA2: NN
                                                                                              * 8'hA3: USB PHY
                                                                                              * 8'hA4: APWDT
                                                                                              * 8'hA5: MPWDT
                                                                                              * 8'hA6: NPWDT
                                                                                              * 8'hA7: FWWDT
                                                                                              * 8'hA8: video_handshake
                                                                                              */
#define AON_DBG_PORT(x)                             ((u32)(((x) & 0x000000FF) << 8))
#define AON_GET_DBG_PORT(x)                         ((u32)(((x >> 8) & 0x000000FF)))
#define AON_MASK_DBG_PIN_SEL                        ((u32)0x0000000F << 4)                   /*!< R/W     0 Debugger pin sel (TBD) */
#define AON_DBG_PIN_SEL(x)                          ((u32)(((x) & 0x0000000F) << 4))
#define AON_GET_DBG_PIN_SEL(x)                      ((u32)(((x >> 4) & 0x0000000F)))
#define AON_MASK_OTP3                               ((u32)0x00000003 << 2)                   /*!< R/W     0 For OTP Use */
#define AON_OTP3(x)                                 ((u32)(((x) & 0x00000003) << 2))
#define AON_GET_OTP3(x)                             ((u32)(((x >> 2) & 0x00000003)))
#define AON_BIT_DBG_SEL                             ((u32)0x00000001 << 1)                   /*!< R/W     1 Debugger mode sel (1: JTAG, 0: SWD) */
#define AON_BIT_DBG_EN                              ((u32)0x00000001 << 0)                   /*!< R/W     1 Debugger pin enable(1: enable, 0: disable) */
/** @} */


/** @defgroup REG_AON_OTP_SYSCFG3
 * @brief
 * @{
 */
#define AON_MASK_FLASH_PIN_SEL                      ((u32)0x0000000F << 28)                  /*!< R/W     0 Flash pin sel(TBD) */
#define AON_FLASH_PIN_SEL(x)                        ((u32)(((x) & 0x0000000F) << 28))
#define AON_GET_FLASH_PIN_SEL(x)                    ((u32)(((x >> 28) & 0x0000000F)))
#define AON_MASK_FLASH_MODE                         ((u32)0x0000000F << 24)                  /*!< R/W     0 Flash IO mode sel(TBD) */
#define AON_FLASH_MODE(x)                           ((u32)(((x) & 0x0000000F) << 24))
#define AON_GET_FLASH_MODE(x)                       ((u32)(((x >> 24) & 0x0000000F)))
#define AON_MASK_DBG_PORT7_SEL                      ((u32)0x0000000F << 20)                  /*!< R/W     0
                                                                                              * Debug port group 7 data sel
                                                                                              * 0000 : debug data 3:0 
                                                                                              * 0001 : debug data 7:4 
                                                                                              * 0010 : debug data 11:8
                                                                                              * 0011 :  debug data 15:12
                                                                                              * 0100 :  debug data 19:16
                                                                                              * 0101 :  debug data 23:20
                                                                                              * 0110 :  debug data 27:24
                                                                                              * 0111 :  debug data 31:28
                                                                                              */
#define AON_DBG_PORT7_SEL(x)                        ((u32)(((x) & 0x0000000F) << 20))
#define AON_GET_DBG_PORT7_SEL(x)                    ((u32)(((x >> 20) & 0x0000000F)))
#define AON_MASK_DBG_PORT6_SEL                      ((u32)0x0000000F << 16)                  /*!< R/W     0
                                                                                              * Debug port group 6 data sel
                                                                                              * 0000 : debug data 3:0 
                                                                                              * 0001 : debug data 7:4 
                                                                                              * 0010 : debug data 11:8
                                                                                              * 0011 :  debug data 15:12
                                                                                              * 0100 :  debug data 19:16
                                                                                              * 0101 :  debug data 23:20
                                                                                              * 0110 :  debug data 27:24
                                                                                              * 0111 :  debug data 31:28
                                                                                              */
#define AON_DBG_PORT6_SEL(x)                        ((u32)(((x) & 0x0000000F) << 16))
#define AON_GET_DBG_PORT6_SEL(x)                    ((u32)(((x >> 16) & 0x0000000F)))
#define AON_MASK_DBG_PORT5_SEL                      ((u32)0x0000000F << 12)                  /*!< R/W     0
                                                                                              * Debug port group 5 data sel
                                                                                              * 0000 : debug data 3:0 
                                                                                              * 0001 : debug data 7:4 
                                                                                              * 0010 : debug data 11:8
                                                                                              * 0011 :  debug data 15:12
                                                                                              * 0100 :  debug data 19:16
                                                                                              * 0101 :  debug data 23:20
                                                                                              * 0110 :  debug data 27:24
                                                                                              * 0111 :  debug data 31:28
                                                                                              */
#define AON_DBG_PORT5_SEL(x)                        ((u32)(((x) & 0x0000000F) << 12))
#define AON_GET_DBG_PORT5_SEL(x)                    ((u32)(((x >> 12) & 0x0000000F)))
#define AON_MASK_DBG_PORT4_SEL                      ((u32)0x0000000F << 8)                   /*!< R/W     0
                                                                                              * Debug port group 4 data sel
                                                                                              * 0000 : debug data 3:0 
                                                                                              * 0001 : debug data 7:4 
                                                                                              * 0010 : debug data 11:8
                                                                                              * 0011 :  debug data 15:12
                                                                                              * 0100 :  debug data 19:16
                                                                                              * 0101 :  debug data 23:20
                                                                                              * 0110 :  debug data 27:24
                                                                                              * 0111 :  debug data 31:28
                                                                                              */
#define AON_DBG_PORT4_SEL(x)                        ((u32)(((x) & 0x0000000F) << 8))
#define AON_GET_DBG_PORT4_SEL(x)                    ((u32)(((x >> 8) & 0x0000000F)))
#define AON_MASK_DBG_PORT3_SEL                      ((u32)0x0000000F << 4)                   /*!< R/W     0
                                                                                              * Debug port group 3 data sel
                                                                                              * 0000 : debug data 3:0 
                                                                                              * 0001 : debug data 7:4 
                                                                                              * 0010 : debug data 11:8
                                                                                              * 0011 :  debug data 15:12
                                                                                              * 0100 :  debug data 19:16
                                                                                              * 0101 :  debug data 23:20
                                                                                              * 0110 :  debug data 27:24
                                                                                              * 0111 :  debug data 31:28
                                                                                              */
#define AON_DBG_PORT3_SEL(x)                        ((u32)(((x) & 0x0000000F) << 4))
#define AON_GET_DBG_PORT3_SEL(x)                    ((u32)(((x >> 4) & 0x0000000F)))
#define AON_MASK_DBG_PORT2_SEL                      ((u32)0x0000000F << 0)                   /*!< R/W     0
                                                                                              * Debug port group 2 data sel
                                                                                              * 0000 : debug data 3:0 
                                                                                              * 0001 : debug data 7:4 
                                                                                              * 0010 : debug data 11:8
                                                                                              * 0011 :  debug data 15:12
                                                                                              * 0100 :  debug data 19:16
                                                                                              * 0101 :  debug data 23:20
                                                                                              * 0110 :  debug data 27:24
                                                                                              * 0111 :  debug data 31:28
                                                                                              */
#define AON_DBG_PORT2_SEL(x)                        ((u32)(((x) & 0x0000000F) << 0))
#define AON_GET_DBG_PORT2_SEL(x)                    ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_AON_OTP_SYSCFG4
 * @brief
 * @{
 */
#define AON_BIT_LOG_DBG_EN                          ((u32)0x00000001 << 31)                  /*!< R/W     0 Log UART message on/off (0: On, 1: off), Use by SW */
#define AON_MASK_OTP6                               ((u32)0x00000007 << 28)                  /*!< R/W     0 For OTP Use */
#define AON_OTP6(x)                                 ((u32)(((x) & 0x00000007) << 28))
#define AON_GET_OTP6(x)                             ((u32)(((x >> 28) & 0x00000007)))
#define AON_MASK_LOG_TX_PIN_SEL                     ((u32)0x00000003 << 26)                  /*!< R/W     0 Log UART TX pin sel, Use by SW */
#define AON_LOG_TX_PIN_SEL(x)                       ((u32)(((x) & 0x00000003) << 26))
#define AON_GET_LOG_TX_PIN_SEL(x)                   ((u32)(((x >> 26) & 0x00000003)))
#define AON_MASK_LOG_RX_PIN_SEL                     ((u32)0x00000003 << 24)                  /*!< R/W     0 Log UART RX pin sel, Use by SW */
#define AON_LOG_RX_PIN_SEL(x)                       ((u32)(((x) & 0x00000003) << 24))
#define AON_GET_LOG_RX_PIN_SEL(x)                   ((u32)(((x >> 24) & 0x00000003)))
#define AON_MASK_ROM_UART_BAUD                      ((u32)0x0000000F << 20)                  /*!< R/W     0 ROM UART Baud Rate Sel(TBD), Use by SW */
#define AON_ROM_UART_BAUD(x)                        ((u32)(((x) & 0x0000000F) << 20))
#define AON_GET_ROM_UART_BAUD(x)                    ((u32)(((x >> 20) & 0x0000000F)))
#define AON_MASK_OTP5                               ((u32)0x0000000F << 16)                  /*!< R/W     0 For OTP Use */
#define AON_OTP5(x)                                 ((u32)(((x) & 0x0000000F) << 16))
#define AON_GET_OTP5(x)                             ((u32)(((x >> 16) & 0x0000000F)))
#define AON_BIT_ROM_DBG_EN                          ((u32)0x00000001 << 15)                  /*!< R/W     0 ROM UART message on/off (0: On, 1: off), Use by SW */
#define AON_MASK_ROM_UART_SEL                       ((u32)0x00000007 << 12)                  /*!< R/W     0 ROM UART sel (TBD), Use by SW */
#define AON_ROM_UART_SEL(x)                         ((u32)(((x) & 0x00000007) << 12))
#define AON_GET_ROM_UART_SEL(x)                     ((u32)(((x >> 12) & 0x00000007)))
#define AON_MASK_ROM_TX_SEL                         ((u32)0x00000003 << 10)                  /*!< R/W     0 ROM UART TX pin sel, Use by SW */
#define AON_ROM_TX_SEL(x)                           ((u32)(((x) & 0x00000003) << 10))
#define AON_GET_ROM_TX_SEL(x)                       ((u32)(((x >> 10) & 0x00000003)))
#define AON_MASK_ROM_RX_SEL                         ((u32)0x00000003 << 8)                   /*!< R/W     0 ROM UART RX pin sel, Use by SW */
#define AON_ROM_RX_SEL(x)                           ((u32)(((x) & 0x00000003) << 8))
#define AON_GET_ROM_RX_SEL(x)                       ((u32)(((x >> 8) & 0x00000003)))
#define AON_MASK_BOOT_SEL                           ((u32)0x0000000F << 4)                   /*!< R/W     0 Boot selection(TBD), Use by SW */
#define AON_BOOT_SEL(x)                             ((u32)(((x) & 0x0000000F) << 4))
#define AON_GET_BOOT_SEL(x)                         ((u32)(((x >> 4) & 0x0000000F)))
#define AON_MASK_OTP4                               ((u32)0x0000000F << 0)                   /*!< R/W     0 For OTP Use */
#define AON_OTP4(x)                                 ((u32)(((x) & 0x0000000F) << 0))
#define AON_GET_OTP4(x)                             ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_AON_OTP_SYSCFG5
 * @brief
 * @{
 */
#define AON_BIT_OTP9                                ((u32)0x00000001 << 31)                  /*!< R/W     0 For OTP Use */
#define AON_BIT_AACK_EN                             ((u32)0x00000001 << 30)                  /*!< R/W     0 0:Enable AACK Circuit, 1: Disable AACK Circuit */
#define AON_MASK_PFM_OCP                            ((u32)0x00000007 << 27)                  /*!< R/W   0x3 Mapping to AON+0x18[25:23] once FPWM_L1=0 */
#define AON_PFM_OCP(x)                              ((u32)(((x) & 0x00000007) << 27))
#define AON_GET_PFM_OCP(x)                          ((u32)(((x >> 27) & 0x00000007)))
#define AON_MASK_PFM_ZCD_H                          ((u32)0x00000003 << 25)                  /*!< R/W   1   Mapping to AON+0x18[28:27] once FPWM_L1=0 */
#define AON_PFM_ZCD_H(x)                            ((u32)(((x) & 0x00000003) << 25))
#define AON_GET_PFM_ZCD_H(x)                        ((u32)(((x >> 25) & 0x00000003)))
#define AON_BIT_PFM_MOS_HALF                        ((u32)0x00000001 << 24)                  /*!< R/W   1   Mapping to AON+0x1C[11] once FPWM_L1=0 */
#define AON_MASK_OTP8                               ((u32)0x00000003 << 22)                  /*!< R/W     0 For OTP Use */
#define AON_OTP8(x)                                 ((u32)(((x) & 0x00000003) << 22))
#define AON_GET_OTP8(x)                             ((u32)(((x >> 22) & 0x00000003)))
#define AON_MASK_PWM_OCP                            ((u32)0x00000007 << 19)                  /*!< R/W   0x3 Mapping to AON+0x18[25:23] once FPWM_L1=1 */
#define AON_PWM_OCP(x)                              ((u32)(((x) & 0x00000007) << 19))
#define AON_GET_PWM_OCP(x)                          ((u32)(((x >> 19) & 0x00000007)))
#define AON_MASK_PWM_ZCD_H                          ((u32)0x00000003 << 17)                  /*!< R/W   0x2 Mapping to AON+0x18[28:27] once FPWM_L1=1 */
#define AON_PWM_ZCD_H(x)                            ((u32)(((x) & 0x00000003) << 17))
#define AON_GET_PWM_ZCD_H(x)                        ((u32)(((x >> 17) & 0x00000003)))
#define AON_BIT_PWM_MOS_HALF                        ((u32)0x00000001 << 16)                  /*!< R/W     0 Mapping to AON+0x1C[11] once FPWM_L1=1 */
#define AON_MASK_OTP7                               ((u32)0x00000003 << 14)                  /*!< R/W     0
                                                                                              * [1] Not used
                                                                                              * [0] 1: Force KM0 FW CPU as sec master when acessing to defined address range.
                                                                                              * Defined address range:
                                                                                              * 0x4022_00D4 (AON TIMER)
                                                                                              * 0x4022_002C (LSFIF_CMD)
                                                                                              * 0x4022_0030 (LSFIF_RWD)
                                                                                              * 0x4022_2xxx (AON GPIO)
                                                                                              */
#define AON_OTP7(x)                                 ((u32)(((x) & 0x00000003) << 14))
#define AON_GET_OTP7(x)                             ((u32)(((x >> 14) & 0x00000003)))
#define AON_BIT_XTAL_LP_CTRL                        ((u32)0x00000001 << 13)                  /*!< R/W   0   0:Manual Mode */
#define AON_BIT_ISP_CTRL                            ((u32)0x00000001 << 12)                  /*!< R/W Once 1   1:Enable ISP Function  */
#define AON_BIT_ENC_CTRL                            ((u32)0x00000001 << 11)                  /*!< R/W Once 1   1:Enable whole video encoder function  */
#define AON_BIT_H265_CTRL                           ((u32)0x00000001 << 10)                  /*!< R/W Once 1   1:Enable H265 Function  */
#define AON_BIT_NN_CTRL                             ((u32)0x00000001 << 9)                   /*!< R/W Once 1   1:Enable NN Function */
#define AON_BIT_RM3_DBG_MD_SEL                      ((u32)0x00000001 << 8)                   /*!< R/W     0
                                                                                              * Real-M300 SWJ-DP Mode Selection (SW Only)
                                                                                              * 0:SWD-DP Mode
                                                                                              * 1:JTAG-DP Mode
                                                                                              */
#define AON_BIT_RM3_DBG_PIN_EN                      ((u32)0x00000001 << 7)                   /*!< R/W     0
                                                                                              * Real-M300 SWJ-DP Pinmux Function Enable (SW Only)
                                                                                              * 0:SWD/JTAG Pin Disable
                                                                                              * 1:SWD/JTAG Pin Enable
                                                                                              */
#define AON_BIT_DBG_CHAIN_EN                        ((u32)0x00000001 << 6)                   /*!< R/W     0
                                                                                              * Real-M500 and Real-M300 JTAG Chain Mode Enable (SW Only)
                                                                                              * 0:JTAG Chain Mode Disable
                                                                                              * 1:JTAG Chain Mode Enable
                                                                                              */
#define AON_BIT_RM5_DBG_MD_SEL                      ((u32)0x00000001 << 5)                   /*!< R/W     0
                                                                                              * Real-M500 SWJ-DP Mode Selection (SW Only)
                                                                                              * 0:SWD-DP Mode
                                                                                              * 1:JTAG-DP Mode
                                                                                              */
#define AON_BIT_RM5_DBG_PIN_EN                      ((u32)0x00000001 << 4)                   /*!< R/W     0
                                                                                              * Real-M500 SWJ-DP Pinmux Function Enable (SW Only)
                                                                                              * 0:SWD/JTAG Pin Disable
                                                                                              * 1:SWD/JTAG Pin Enable
                                                                                              */
#define AON_MASK_XTAL_SEL                           ((u32)0x0000000F << 0)                   /*!< R/W     0
                                                                                              * xtal selection, Default=40MHz        
                                                                                              * 0000= 40MHz, 0001= 25MHz,
                                                                                              * 0010= 13MHz,    0011= 19.2MHz 
                                                                                              * 0100= 20MHz,  0101= 26MHz,
                                                                                              * 0110= 38.4MHz,  0111= 17.664MHz 
                                                                                              * 1000= 16MHz,  1001= 14.138MHz,
                                                                                              * 1010= 12MHz,    1011= 52MHz 
                                                                                              * 1100= 48MHz,  1101= 27MHz,
                                                                                              * 1110= 24MHz,    1111= none  
                                                                                              */
#define AON_XTAL_SEL(x)                             ((u32)(((x) & 0x0000000F) << 0))
#define AON_GET_XTAL_SEL(x)                         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_AON_OTP_SYSCFG6
 * @brief
 * @{
 */
#define AON_MASK_SYSCFG6                            ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 None */
#define AON_SYSCFG6(x)                              ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AON_GET_SYSCFG6(x)                          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AON_OTP_SYSCFG7
 * @brief
 * @{
 */
#define AON_MASK_SYSCFG7                            ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 None */
#define AON_SYSCFG7(x)                              ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AON_GET_SYSCFG7(x)                          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AON_DBG_REG
 * @brief
 * @{
 */
#define AON_MASK_SYS_DBG_VALUE                      ((u32)0xFFFFFFFF << 0)                   /*!< R       0 SYS debug register value */
#define AON_SYS_DBG_VALUE(x)                        ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AON_GET_SYS_DBG_VALUE(x)                    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AON_SYS_CTRL
 * @brief
 * @{
 */
#define AON_BIT_DEBUGER_EN                          ((u32)0x00000001 << 8)                   /*!< R/W   0   1: swd mux enable */
#define AON_BIT_OTP_RW_REG_SEL                      ((u32)0x00000001 << 7)                   /*!< R/W   0  
                                                                                              * OTP Indirect Read/Write Register Selection
                                                                                              * 0: From AON indirect access register
                                                                                              * 1: From SYSON/SYSON_S indirect access register
                                                                                              */
#define AON_BIT_OCP_IMR                             ((u32)0x00000001 << 6)                   /*!< R/W   0   1: Enable OCP interrupt */
#define AON_BIT_OCP_ISR                             ((u32)0x00000001 << 5)                   /*!< R/W1C 0   1: indicate OCP status */
#define AON_BIT_AON_TIMER_IMR                       ((u32)0x00000001 << 4)                   /*!< R/W   0   1: Enable AON Timer interrupt */
#define AON_BIT_CPU_RET_MODE                        ((u32)0x00000001 << 3)                   /*!< R/W   0   1:Enable RET Mode, 0: Disable RET Mode */
#define AON_BIT_BOD_INT_IMR                         ((u32)0x00000001 << 2)                   /*!< R/W   0   1: indicate BOD  Mask; Vin < BOD_VINTL (Ex: Vin<2.791 and then BOD_ISR_STS was high) */
#define AON_BIT_GPIOA4_OUTPUT_STATUS                ((u32)0x00000001 << 1)                   /*!< R/W   0   1: Output High, 0: Output Low */
#define AON_BIT_GPIOA4_OUTPUT_EN                    ((u32)0x00000001 << 0)                   /*!< R/W   0   1: GPIO Output Enable */
/** @} */


/** @defgroup REG_AON_SIC_DBG_CTRL
 * @brief
 * @{
 */
#define AON_MASK_SIC_DBG_GP_SEL                     ((u32)0x000000FF << 1)                   /*!< R/W   0   Group Sel */
#define AON_SIC_DBG_GP_SEL(x)                       ((u32)(((x) & 0x000000FF) << 1))
#define AON_GET_SIC_DBG_GP_SEL(x)                   ((u32)(((x >> 1) & 0x000000FF)))
#define AON_BIT_SIC_DBG_EN                          ((u32)0x00000001 << 0)                   /*!< R/W   0   1:Enable */
/** @} */


/** @defgroup REG_AON_GPIO1_WAKE_CTRL
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_AON_GPIO2_WAKE_CTRL
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_AON_WL_CTRL
 * @brief
 * @{
 */
#define AON_BIT_SYS_WLAFE_POD125                    ((u32)0x00000001 << 25)                  /*!< R/W     0 0: power down 125 WLAFE  */
#define AON_BIT_SYS_WLAFE_POD33                     ((u32)0x00000001 << 24)                  /*!< R/W     0 0: power down 33 WLAFE  */
#define AON_BIT_WLON_CLK_DIV_DONE                   ((u32)0x00000001 << 20)                  /*!< R       0 Clock dividing processing is done */
#define AON_BIT_WLON_CLK_VLD                        ((u32)0x00000001 << 19)                  /*!< R       0 None */
#define AON_MASK_WLON_CLK_DIVN                      ((u32)0x0000003F << 13)                  /*!< R/W   0x0 Clock divider factor. */
#define AON_WLON_CLK_DIVN(x)                        ((u32)(((x) & 0x0000003F) << 13))
#define AON_GET_WLON_CLK_DIVN(x)                    ((u32)(((x >> 13) & 0x0000003F)))
#define AON_BIT_WLON_CLK_DIVTRG                     ((u32)0x00000001 << 12)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define AON_MASK_WLON_CLK_SEL                       ((u32)0x00000007 << 9)                   /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: OSC 4M
                                                                                              * 0x1: REGU 100K
                                                                                              * 0x2: SDM 32K
                                                                                              * 0x3: XTAL 40M
                                                                                              * Others: 1'b0
                                                                                              */
#define AON_WLON_CLK_SEL(x)                         ((u32)(((x) & 0x00000007) << 9))
#define AON_GET_WLON_CLK_SEL(x)                     ((u32)(((x >> 9) & 0x00000007)))
#define AON_BIT_WLON_CLK_EN                         ((u32)0x00000001 << 8)                   /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock enable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define AON_BIT_WL_DBG_EN                           ((u32)0x00000001 << 5)                   /*!< R/W     0 1: enable WLAN MAC debug interface */
#define AON_BIT_WL_USE_REQ                          ((u32)0x00000001 << 4)                   /*!< R/W     0 None */
#define AON_BIT_WL_REQ2ACT_                         ((u32)0x00000001 << 3)                   /*!< R       0 1: WLAN request to active  */
#define AON_BIT_WL_RDY2ACT_                         ((u32)0x00000001 << 2)                   /*!< R/W     0 1: WLAN ready to active */
#define AON_MASK_WL_PC_STEP                         ((u32)0x00000003 << 0)                   /*!< R/W   0x1 None */
#define AON_WL_PC_STEP(x)                           ((u32)(((x) & 0x00000003) << 0))
#define AON_GET_WL_PC_STEP(x)                       ((u32)(((x >> 0) & 0x00000003)))
/** @} */


/** @defgroup REG_AON_PMC_CTRL
 * @brief
 * @{
 */
#define AON_MASK_AON_PMC_STATE                      ((u32)0x00000007 << 19)                  /*!< R     0x0
                                                                                              * PMC STATE:
                                                                                              * 0: OFF
                                                                                              * 1: SNOZ COMBO
                                                                                              * 2: SNOZ WIFI
                                                                                              * 3: SNOZ MCU
                                                                                              * 4: STANDBY
                                                                                              * 5: DEEP SLEEP
                                                                                              */
#define AON_AON_PMC_STATE(x)                        ((u32)(((x) & 0x00000007) << 19))
#define AON_GET_AON_PMC_STATE(x)                    ((u32)(((x >> 19) & 0x00000007)))
#define AON_BIT_AON_PMC_INT_IMR                     ((u32)0x00000001 << 18)                  /*!< R/W   0x0 1: Enable PMC interrupt */
#define AON_BIT_AON_PMC_INT_STS                     ((u32)0x00000001 << 17)                  /*!< R/W1C 0x0 1: trigger interruput to CPU. Clear by SW write 1 */
#define AON_MASK_AON_GPIO_WAKE_SEL                  ((u32)0x0000000F << 5)                   /*!< R/W     0
                                                                                              * 0000: GPIOA0~GPIOA3 can wake up system individually
                                                                                              * 0001: RSVD
                                                                                              * 0010: RSVD
                                                                                              * 0011: GPIOA0&GPIOA1=1 and then wake up system. GPIOA2 and GPIOA3 can wake up system individually
                                                                                              * 0100: RSVD
                                                                                              * 0101: GPIOA0&GPIOA2=1 and then wake up system. GPIOA1 and GPIOA3 can wake up system individually
                                                                                              * 0110: GPIOA1&GPIOA2=1 and then wake up system. GPIOA0 and GPIOA3 can wake up system individually
                                                                                              * 0111: GPIOA0&GPIOA1&GPIOA2=1 and then wake up system. GPIOA0 can wake up system individually
                                                                                              * 1000: RSVD
                                                                                              * 1001: GPIOA0&GPIOA3=1 and then wake up system. GPIOA1 and GPIOA2 can wake up system individually
                                                                                              * 1010: GPIOA1&GPIOA3=1 and then wake up system. GPIOA0 and GPIOA2 can wake up system individually
                                                                                              * 1011: GPIOA0&GPIOA1&GPIOA3=1 and then wake up system. GPIOA2 can wake up system individually
                                                                                              * 1100: GPIOA2&GPIOA3=1 and then wake up system. GPIOA0 and GPIOA1 can wake up system individually
                                                                                              * 1101: GPIOA0&GPIOA2&GPIOA3=1 and then wake up system. GPIOA1 can wake up system individually
                                                                                              * 1110: GPIOA1&GPIOA2&GPIOA3=1 and then wake up system. GPIOA0 can wake up system individually
                                                                                              * 1111: GPIOA0&GPIOA1&GPIOA2&GPIOA3=1 and then wake up system
                                                                                              * 
                                                                                              */
#define AON_AON_GPIO_WAKE_SEL(x)                    ((u32)(((x) & 0x0000000F) << 5))
#define AON_GET_AON_GPIO_WAKE_SEL(x)                ((u32)(((x >> 5) & 0x0000000F)))
#define AON_MASK_SYSON_PLL_SWITCH                   ((u32)0x00000003 << 3)                   /*!< R/W     0
                                                                                              * [1] Dummy
                                                                                              * [0] for PMC control:
                                                                                              * 1: Enable SYS1PLL during power on sequence.
                                                                                              * 0: Keep SYS1PLL disable during power on sequence.
                                                                                              */
#define AON_SYSON_PLL_SWITCH(x)                     ((u32)(((x) & 0x00000003) << 3))
#define AON_GET_SYSON_PLL_SWITCH(x)                 ((u32)(((x >> 3) & 0x00000003)))
#define AON_BIT_SYSON_PM_CMD_SLP                    ((u32)0x00000001 << 2)                   /*!< R/W     0
                                                                                              * 1: command SYSON SM to enter sleep state, when PMC 
                                                                                              * finishes the process, this bit will be auto clear to 0
                                                                                              */
#define AON_BIT_DSLP_LV_EN                          ((u32)0x00000001 << 1)                   /*!< R/W     0
                                                                                              * 0: LDO_AON output stay 0.9V under deep sleep state
                                                                                              * 1: LDO_AON output reduce to 0.8V under deep sleep state
                                                                                              */
#define AON_BIT_SYSON_PM_CMD_DSLP                   ((u32)0x00000001 << 0)                   /*!< R/W     0
                                                                                              * 1: command SYSON SM to enter deep sleep state,when
                                                                                              *  PMC finishes the process, this bit will be auto clear to 0
                                                                                              */
/** @} */


/** @defgroup REG_AON_PMC_OPT
 * @brief
 * @{
 */
#define AON_BIT_DSLP_ANA_4M_EN                      ((u32)0x00000001 << 10)                  /*!< R/W     1
                                                                                              * When enter dslp, 1: enable clk; 0: disable
                                                                                              * If it's selected to disable in DSLP, HW should enable clock the clock automatically when wake event takes place.
                                                                                              */
#define AON_BIT_DSLP_AON_REG_CLK_EN                 ((u32)0x00000001 << 9)                   /*!< R/W     1
                                                                                              * When enter dslp, 1: enable clk; 0: gated
                                                                                              * If it's selected to disable in DSLP, HW should enable clock the clock automatically when wake event takes place.
                                                                                              * NOT USED IN AMEBAPRO3
                                                                                              */
#define AON_BIT_AON_FSM_DSLP_CLK_SEL                ((u32)0x00000001 << 0)                   /*!< R/W     0
                                                                                              * FSM clock selection in deep sleep mode or standby mode
                                                                                              * 0: ANA_CLK_100K
                                                                                              * 1: OSC 4MHz
                                                                                              */
/** @} */


/** @defgroup REG_AON_NP_BOOT_REASON
 * @brief
 * @{
 */
#define AON_BIT_CA32WDT_STS                         ((u32)0x00000001 << 9)                   /*!< R       0 1: Indicate boot reason is CA32 Watchdog. */
#define AON_BIT_FWWDT_STS                           ((u32)0x00000001 << 8)                   /*!< R       0 1: Indicate boot reason is KM0 FW Watchdog. */
#define AON_BIT_MPWDT_STS                           ((u32)0x00000001 << 7)                   /*!< R       0 1: Indicate boot reason is KM4 MP Watchdog. */
#define AON_BIT_NPWDT_STS                           ((u32)0x00000001 << 6)                   /*!< R       0 1: Indicate boot reason is KM4 NP Watchdog. */
#define AON_BIT_BOD_ISR_STS                         ((u32)0x00000001 << 5)                   /*!< R/W1C   0 1: Indicate BOD interrupt and clear by SW. */
#define AON_BIT_BOD_STS                             ((u32)0x00000001 << 4)                   /*!< R       0 1: Indicate reboot reason is BOD detection. */
#define AON_BIT_AON_STANDBY_STS                     ((u32)0x00000001 << 3)                   /*!< R       0 1: Indicated boot reason is wake up event which resumes KM4NP from STBY or SNZWIFI */
#define AON_BIT_AON_DSLP_STS                        ((u32)0x00000001 << 2)                   /*!< R       0 1: Indicated boot reason is wake up event which resumes KM4NP from DSLP. */
#define AON_BIT_AON_WATCHDOG_STS                    ((u32)0x00000001 << 1)                   /*!< R       0 1: Indicate boot reason is AON Watchdog. */
#define AON_BIT_PWR_ON_STS                          ((u32)0x00000001 << 0)                   /*!< R       1 1: Indicate boot reason is power on reset. */
/** @} */


/** @defgroup REG_AON_CA32_BOOT_REASON
 * @brief
 * @{
 */
#define AON_BIT_CA32WDT_STS_088                     ((u32)0x00000001 << 9)                   /*!< R       0 1: Indicate boot reason is CA32 Watchdog. */
#define AON_BIT_FWWDT_STS_088                       ((u32)0x00000001 << 8)                   /*!< R       0 1: Indicate boot reason is KM0 FW Watchdog. */
#define AON_BIT_MPWDT_STS_088                       ((u32)0x00000001 << 7)                   /*!< R       0 1: Indicate boot reason is KM4 MP Watchdog. */
#define AON_BIT_NPWDT_STS_088                       ((u32)0x00000001 << 6)                   /*!< R       0 1: Indicate boot reason is KM4 NP Watchdog. */
#define AON_BIT_BOD_ISR_STS_088                     ((u32)0x00000001 << 5)                   /*!< R/W1C   0 1: Indicate BOD interrupt and clear by SW. */
#define AON_BIT_BOD_STS_088                         ((u32)0x00000001 << 4)                   /*!< R       0 1: Indicate reboot reason is BOD detection. */
#define AON_BIT_AON_STANDBY_STS_088                 ((u32)0x00000001 << 3)                   /*!< R       0 NOT USE */
#define AON_BIT_AON_DSLP_STS_088                    ((u32)0x00000001 << 2)                   /*!< R       0 1: Indicated boot reason is wake up event which resumes KM4NP from DSLP. */
#define AON_BIT_AON_WATCHDOG_STS_088                ((u32)0x00000001 << 1)                   /*!< R       0 1: Indicate boot reason is AON Watchdog. */
#define AON_BIT_PWR_ON_STS_088                      ((u32)0x00000001 << 0)                   /*!< R       1 1: Indicate boot reason is power on reset or SW reset */
/** @} */


/** @defgroup REG_AON_SLP_WAKE_EVENT_MSK0
 * @brief
 * @{
 */
#define AON_BIT_SYSON_WEVT_FWIPC_MSK                ((u32)0x00000001 << 17)                  /*!< R/W     0 1: enable IPC FW Wakeup event; 0: disable the event to wakeup system */
#define AON_BIT_SYSON_WEVT_NPIPC_MSK                ((u32)0x00000001 << 16)                  /*!< R/W     0 1: enable IPC NP Wakeup event; 0: disable the event to wakeup system */
#define AON_BIT_SYSON_WEVT_SGPIO_MSK                ((u32)0x00000001 << 15)                  /*!< R/W     0 1: enable SGPIO Wakeup event; 0: disable the event to wakeup system */
#define AON_BIT_SYSON_WEVT_ADC_MSK                  ((u32)0x00000001 << 14)                  /*!< R/W     0 1: enable ADC Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_SYSON_WEVT_USB_MSK                  ((u32)0x00000001 << 13)                  /*!< R/W     0 1: enable USB Wakeup event; 0: disable the event to wakeup system */
#define AON_BIT_AON_WEVT_AON_GPIO3_MSK              ((u32)0x00000001 << 12)                  /*!< R/W     0 1: enable  AON GPIO3 Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_AON_WEVT_AON_GPIO2_MSK              ((u32)0x00000001 << 11)                  /*!< R/W     0 1: enable  AON GPIO2 Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_AON_WEVT_AON_GPIO1_MSK              ((u32)0x00000001 << 10)                  /*!< R/W     0 1: enable  AON GPIO1 Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_AON_WEVT_AON_GPIO0_MSK              ((u32)0x00000001 << 9)                   /*!< R/W     0 1: enable  AON GPIO0 Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_AON_WEVT_AON_RTC_MSK                ((u32)0x00000001 << 8)                   /*!< R/W     0 1: enable  AON RTC Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_AON_WEVT_AON_COMP_MSK               ((u32)0x00000001 << 7)                   /*!< R/W     0 1: enable  AON Comparator Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_AON_WEVT_AON_TIMER_MSK              ((u32)0x00000001 << 6)                   /*!< R/W     0 1: enable  AON Timer Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_PON_WEVT_LOGUART_MSK                ((u32)0x00000001 << 5)                   /*!< R/W     0 1: enable LOGUART  Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_PON_WEVT_WLAN1_MSK                  ((u32)0x00000001 << 4)                   /*!< R/W     0 1: enable  WLAN Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_PON_WEVT_WLAN_MSK                   ((u32)0x00000001 << 3)                   /*!< R/W     0 1: enable  WLAN Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_PON_WEVT_PWM_MSK                    ((u32)0x00000001 << 2)                   /*!< R/W     0 1: enable PWM Wakeup event; 0: disable the event to wakeup system */
#define AON_BIT_PON_WEVT_GPIO_MSK                   ((u32)0x00000001 << 1)                   /*!< R/W     0 1: enable PON GPIO Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_PON_WEVT_GTIM0_MSK                  ((u32)0x00000001 << 0)                   /*!< R/W     0 1: enable GTIMER0 Wakeup event; 0: disable the event to wakeup system */
/** @} */


/** @defgroup REG_AON_SLP_WAKE_EVENT_STATUS0
 * @brief
 * @{
 */
#define AON_BIT_SYSON_WEVT_FWIPC_STS                ((u32)0x00000001 << 17)                  /*!< R       0 1: Indicate IPC KM0 FW Wakeup event */
#define AON_BIT_SYSON_WEVT_NPIPC_STS                ((u32)0x00000001 << 16)                  /*!< R       0 1: Indicate IPC KM4 NP Wakeup event */
#define AON_BIT_SYSON_WEVT_SGPIO_STS                ((u32)0x00000001 << 15)                  /*!< R       0 1: Indicate SGPIO Wakeup event */
#define AON_BIT_SYSON_WEVT_ADC_STS                  ((u32)0x00000001 << 14)                  /*!< R       0 1: Indicate ADC Wakeup  event */
#define AON_BIT_SYSON_WEVT_USB_STS                  ((u32)0x00000001 << 13)                  /*!< R       0 1: Indicate USB Wakeup event */
#define AON_BIT_AON_WEVT_AON_GPIO3_STS              ((u32)0x00000001 << 12)                  /*!< R/W1C   0 1: indicate  AON GPIO3 Wakeup  event;  */
#define AON_BIT_AON_WEVT_AON_GPIO2_STS              ((u32)0x00000001 << 11)                  /*!< R/W1C   0 1: indicate  AON GPIO2 Wakeup  event;  */
#define AON_BIT_AON_WEVT_AON_GPIO1_STS              ((u32)0x00000001 << 10)                  /*!< R/W1C   0 1: indicate  AON GPIO1 Wakeup  event;  */
#define AON_BIT_AON_WEVT_AON_GPIO0_STS              ((u32)0x00000001 << 9)                   /*!< R/W1C   0 1: indicate  AON GPIO0 Wakeup  event; */
#define AON_BIT_AON_WEVT_AON_RTC_STS                ((u32)0x00000001 << 8)                   /*!< R       0 1: indicate  AON RTC Wakeup  event */
#define AON_BIT_AON_WEVT_AON_COMP_STS               ((u32)0x00000001 << 7)                   /*!< R       0 1: indicate  AON Comparator Wakeup  event; */
#define AON_BIT_AON_WEVT_AON_TIMER_STS              ((u32)0x00000001 << 6)                   /*!< R/W1C   0 1: indicate  AON Timer Wakeup  event;  */
#define AON_BIT_PON_WEVT_LOGUART_STS                ((u32)0x00000001 << 5)                   /*!< R       0 1: indicate LOGUART  Wakeup  event; */
#define AON_BIT_PON_WEVT_WLAN1_STS                  ((u32)0x00000001 << 4)                   /*!< R       0 1: indicate  WLAN Wakeup  event (INTF1_EN);  */
#define AON_BIT_PON_WEVT_WLAN_STS                   ((u32)0x00000001 << 3)                   /*!< R       0 1: indicate  WLAN Wakeup  event (INTF3_EN);  */
#define AON_BIT_PON_WEVT_PWM_STS                    ((u32)0x00000001 << 2)                   /*!< R       0 1: indicate PWM Wakeup event; */
#define AON_BIT_PON_WEVT_GPIO_STS                   ((u32)0x00000001 << 1)                   /*!< R       0 1: indicate PON GPIO Wakeup event;  */
#define AON_BIT_PON_WEVT_GTIM0_STS                  ((u32)0x00000001 << 0)                   /*!< R       0 1: indicate GTIMER0 Wakeup  event;  */
/** @} */


/** @defgroup REG_AON_SNF_WAKE_EVENT_MSK0
 * @brief
 * @{
 */
#define AON_BIT_WLAN_IMR_MSK                        ((u32)0x00000001 << 1)                   /*!< R/W     0 1: enable wlan interrupt; 0: Disable */
#define AON_BIT_PON_SNFEVT_WLON_PON_MSK             ((u32)0x00000001 << 0)                   /*!< R/W     0 1: enable wlan power on wakeup event interrupt; 0: Disable */
/** @} */


/** @defgroup REG_AON_SNF_WAKE_EVENT_STATUS
 * @brief
 * @{
 */
#define AON_BIT_WLAN_ISR                            ((u32)0x00000001 << 1)                   /*!< R/W1C 0   1: indicate wlan power on status */
#define AON_BIT_PON_SNFEVT_WLON_PON_STS             ((u32)0x00000001 << 0)                   /*!< R     0   1: indicate wlan power on wakeup status */
/** @} */


/** @defgroup REG_AON_PWRMGT_OPTION
 * @brief
 * @{
 */
#define AON_BIT_PON_PMOPT_SNZ_XTAL_NR_EN            ((u32)0x00000001 << 31)                  /*!< R/W     0 1: enable XTAL Normal mode when enter into snooze mode; 1: NR mode, 0: HP mode */
#define AON_MASK_PON_PMOPT_SLP_MEM7_PM_EN           ((u32)0x00000003 << 29)                  /*!< R/W     0
                                                                                              * 0: Enable SRAM7 normal mode when enter into power mode
                                                                                              * 1: Enable SRAM7 RET mode when enter into power mode
                                                                                              * 2: Enable SRAM7 SD mode when enter into power mode
                                                                                              */
#define AON_PON_PMOPT_SLP_MEM7_PM_EN(x)             ((u32)(((x) & 0x00000003) << 29))
#define AON_GET_PON_PMOPT_SLP_MEM7_PM_EN(x)         ((u32)(((x >> 29) & 0x00000003)))
#define AON_MASK_PON_PMOPT_SLP_MEM6_PM_EN           ((u32)0x00000003 << 27)                  /*!< R/W     0
                                                                                              * 0: Enable SRAM6 normal mode when enter into power mode
                                                                                              * 1: Enable SRAM6 RET mode when enter into power mode
                                                                                              * 2: Enable SRAM6 SD mode when enter into power mode
                                                                                              */
#define AON_PON_PMOPT_SLP_MEM6_PM_EN(x)             ((u32)(((x) & 0x00000003) << 27))
#define AON_GET_PON_PMOPT_SLP_MEM6_PM_EN(x)         ((u32)(((x >> 27) & 0x00000003)))
#define AON_MASK_PON_PMOPT_SLP_MEM5_PM_EN           ((u32)0x00000003 << 25)                  /*!< R/W     0
                                                                                              * 0: Enable SRAM5 normal mode when enter into power mode
                                                                                              * 1: Enable SRAM5 RET mode when enter into power mode
                                                                                              * 2: Enable SRAM5 SD mode when enter into power mode
                                                                                              */
#define AON_PON_PMOPT_SLP_MEM5_PM_EN(x)             ((u32)(((x) & 0x00000003) << 25))
#define AON_GET_PON_PMOPT_SLP_MEM5_PM_EN(x)         ((u32)(((x >> 25) & 0x00000003)))
#define AON_MASK_PON_PMOPT_SLP_MEM4_PM_EN           ((u32)0x00000003 << 23)                  /*!< R/W     0
                                                                                              * 0: Enable SRAM4 normal mode when enter into power mode
                                                                                              * 1: Enable SRAM4 RET mode when enter into power mode
                                                                                              * 2: Enable SRAM4 SD mode when enter into power mode
                                                                                              */
#define AON_PON_PMOPT_SLP_MEM4_PM_EN(x)             ((u32)(((x) & 0x00000003) << 23))
#define AON_GET_PON_PMOPT_SLP_MEM4_PM_EN(x)         ((u32)(((x >> 23) & 0x00000003)))
#define AON_MASK_PON_PMOPT_SLP_MEM3_PM_EN           ((u32)0x00000003 << 21)                  /*!< R/W     0
                                                                                              * 0: Enable SRAM3 normal mode when enter into power mode
                                                                                              * 1: Enable SRAM3 RET mode when enter into power mode
                                                                                              * 2: Enable SRAM3 SD mode when enter into power mode
                                                                                              */
#define AON_PON_PMOPT_SLP_MEM3_PM_EN(x)             ((u32)(((x) & 0x00000003) << 21))
#define AON_GET_PON_PMOPT_SLP_MEM3_PM_EN(x)         ((u32)(((x >> 21) & 0x00000003)))
#define AON_MASK_PON_PMOPT_SLP_MEM2_PM_EN           ((u32)0x00000003 << 19)                  /*!< R/W     0
                                                                                              * 0: Enable SRAM2 normal mode when enter into power mode
                                                                                              * 1: Enable SRAM2 RET mode when enter into power mode
                                                                                              * 2: Enable SRAM2 SD mode when enter into power mode
                                                                                              */
#define AON_PON_PMOPT_SLP_MEM2_PM_EN(x)             ((u32)(((x) & 0x00000003) << 19))
#define AON_GET_PON_PMOPT_SLP_MEM2_PM_EN(x)         ((u32)(((x >> 19) & 0x00000003)))
#define AON_MASK_PON_PMOPT_SLP_MEM1_PM_EN           ((u32)0x00000003 << 17)                  /*!< R/W     0
                                                                                              * 0: Enable SRAM1 normal mode when enter into power mode
                                                                                              * 1: Enable SRAM1 RET mode when enter into power mode
                                                                                              * 2: Enable SRAM1 SD mode when enter into power mode
                                                                                              */
#define AON_PON_PMOPT_SLP_MEM1_PM_EN(x)             ((u32)(((x) & 0x00000003) << 17))
#define AON_GET_PON_PMOPT_SLP_MEM1_PM_EN(x)         ((u32)(((x >> 17) & 0x00000003)))
#define AON_MASK_PON_PMOPT_SLP_MEM0_PM_EN           ((u32)0x00000003 << 15)                  /*!< R/W     0
                                                                                              * 0: Enable SRAM0 normal mode when enter into power mode
                                                                                              * 1: Enable SRAM0 RET mode when enter into power mode
                                                                                              * 2: Enable SRAM0 SD mode when enter into power mode
                                                                                              */
#define AON_PON_PMOPT_SLP_MEM0_PM_EN(x)             ((u32)(((x) & 0x00000003) << 15))
#define AON_GET_PON_PMOPT_SLP_MEM0_PM_EN(x)         ((u32)(((x >> 15) & 0x00000003)))
#define AON_BIT_PON_PMOPT_SLP_PERPLL_EN             ((u32)0x00000001 << 14)                  /*!< R/W     1 1: enable Peri PLL when enter into sleep mode; 0: disable PeriPLL */
#define AON_BIT_PON_PMOPT_SLP_SYS1PLL_EN            ((u32)0x00000001 << 13)                  /*!< R/W     1 1: enable syspem PLL when enter into sleep mode; 0: disable SYSPLL */
#define AON_BIT_PON_PMOPT_SLP_SYS2PLL_EN            ((u32)0x00000001 << 12)                  /*!< R/W     1 1: enable syspem PLL when enter into sleep mode; 0: disable SYSPLL */
#define AON_BIT_PON_PMOPT_SLP_XTAL_EN               ((u32)0x00000001 << 11)                  /*!< R/W     1 1: enable XTAL when enter into sleep mode; 0: disable XTAL */
#define AON_BIT_PON_PMOPT_SLP_XTAL_LP_EN            ((u32)0x00000001 << 10)                  /*!< R/W     1 1: enable XTAL Low Power Mode when enter into sleep mode; 0: disable XTAL LPS */
#define AON_BIT_PON_PMOPT_SLP_EN_SOC                ((u32)0x00000001 << 9)                   /*!< R/W     1
                                                                                              * 1: power enable SOC platform when enter into sleep mode; 
                                                                                              * 0: power off SoC domain
                                                                                              * 
                                                                                              * NOT USED IN AMEBAPRO3
                                                                                              */
#define AON_BIT_PON_PMOPT_SLP_EXT_SWR_EN            ((u32)0x00000001 << 8)                   /*!< R/W     1
                                                                                              * 1: Enable External SWR DCORE
                                                                                              * 0: Disable External SWR DCORE
                                                                                              */
#define AON_BIT_PON_PMOPT_SLP_EN_PWM_SRC            ((u32)0x00000001 << 7)                   /*!< R/W     1
                                                                                              * 1: power enable SWR/LDO output heavy loading current mode when enter into sleep mode; 
                                                                                              * 0: disable SWR/LDO output heavy loading mode
                                                                                              */
#define AON_BIT_PON_PMOPT_SLP_EN_SWR_SRC            ((u32)0x00000001 << 6)                   /*!< R/W     1
                                                                                              * 1: power enable SWR/LDO 1.8V when enter into sleep mode; 
                                                                                              * 0: disable SWR/LDO
                                                                                              */
#define AON_MASK_PON_PMOPT_SLP_EN_WLON_MODE         ((u32)0x00000003 << 4)                   /*!< R/W   0x01
                                                                                              * 00: disable LDO_WLON
                                                                                              * 01: power enable  LDO_WLON when enter into sleep mode; 
                                                                                              * 10: enable  LDO_WLON Bypass Mode when enter into sleep mode; 
                                                                                              */
#define AON_PON_PMOPT_SLP_EN_WLON_MODE(x)           ((u32)(((x) & 0x00000003) << 4))
#define AON_GET_PON_PMOPT_SLP_EN_WLON_MODE(x)       ((u32)(((x >> 4) & 0x00000003)))
#define AON_BIT_PON_PWRON_PLL_EN                    ((u32)0x00000001 << 3)                   /*!< R/W     0 NO USE */
#define AON_BIT_PON_SNZ_MCU_EN                      ((u32)0x00000001 << 2)                   /*!< R/W     0 NO USE */
#define AON_BIT_PON_PMOPT_SLP_EN                    ((u32)0x00000001 << 1)                   /*!< R/W     1
                                                                                              * 1: Enable PON Domain when enter into sleep mode;
                                                                                              * 0: Disable PON Domain
                                                                                              */
#define AON_BIT_PON_PMOPT_SLP_EN_4M                 ((u32)0x00000001 << 0)                   /*!< R/W     1
                                                                                              * 1: Enable OSC 4MHz when enter into sleep mode;
                                                                                              * 0: Disable OSC 4MHz
                                                                                              */
/** @} */


/** @defgroup REG_OTP_AL_CTRL_STS
 * @brief
 * @{
 */
#define AON_MASK_OTP_AL_SYSON_STS                   ((u32)0x00000003 << 12)                  /*!< R/W     0
                                                                                              * OTP SYSON Autoload Result Status
                                                                                              * 00: RSVD
                                                                                              * 01: Autoload successful
                                                                                              * 10: RSVD
                                                                                              * 11: Autoload Failed because of OTP ID check failed
                                                                                              */
#define AON_OTP_AL_SYSON_STS(x)                     ((u32)(((x) & 0x00000003) << 12))
#define AON_GET_OTP_AL_SYSON_STS(x)                 ((u32)(((x >> 12) & 0x00000003)))
#define AON_MASK_OTP_AL_PON_STS                     ((u32)0x00000003 << 10)                  /*!< R/W1C   0
                                                                                              * OTP PON Autoload Result Status
                                                                                              * 00: RSVD
                                                                                              * 01: Autoload successful
                                                                                              * 10: RSVD
                                                                                              * 11: Autoload Failed because of OTP ID check failed
                                                                                              */
#define AON_OTP_AL_PON_STS(x)                       ((u32)(((x) & 0x00000003) << 10))
#define AON_GET_OTP_AL_PON_STS(x)                   ((u32)(((x >> 10) & 0x00000003)))
#define AON_MASK_OTP_AL_AON_STS                     ((u32)0x00000003 << 8)                   /*!< R/W1C   0
                                                                                              * OTP AON Autoload Result Status
                                                                                              * 00: RSVD
                                                                                              * 01: Autoload successful
                                                                                              * 10: RSVD
                                                                                              * 11: Autoload Failed because of OTP ID check failed
                                                                                              */
#define AON_OTP_AL_AON_STS(x)                       ((u32)(((x) & 0x00000003) << 8))
#define AON_GET_OTP_AL_AON_STS(x)                   ((u32)(((x >> 8) & 0x00000003)))
#define AON_BIT_OTP_ALDN_SYSON_STS                  ((u32)0x00000001 << 6)                   /*!< R/W   0  
                                                                                              * OTP SYSON Autoload Done Status
                                                                                              * PMC/CPU should clear this bit before enabling autoload process.
                                                                                              * This bit only represents done status but not autoload resutl
                                                                                              * 0: SYSON autoload is not done
                                                                                              * 1: SYSON autoload is done
                                                                                              */
#define AON_BIT_OTP_ALDN_PON_STS                    ((u32)0x00000001 << 5)                   /*!< R/W1C 0  
                                                                                              * OTP PON Autoload Done Status
                                                                                              * PMC/CPU should clear this bit before enabling autoload process.
                                                                                              * This bit only represents done status but not autoload resutl
                                                                                              * 0: PON autoload is not done
                                                                                              * 1: PON autoload is done
                                                                                              */
#define AON_BIT_OTP_ALDN_AON_STS                    ((u32)0x00000001 << 4)                   /*!< R/W1C 0  
                                                                                              * OTP AON Autoload Done Status
                                                                                              * PMC/CPU should clear this bit before enabling autoload process.
                                                                                              * This bit only represents done status but not autoload resutl
                                                                                              * 0: AON autoload is not done
                                                                                              * 1: AON autoload is done
                                                                                              */
#define AON_BIT_OTP_AL_SYSON_EN                     ((u32)0x00000001 << 2)                   /*!< R/W   0  
                                                                                              * OTP SYSON Autoload Control
                                                                                              * If PMC/CPU wants to do this autoload, autoload done status and this bit should be cleared first.
                                                                                              * After cleared, PMC/CPU could set this bit first and then set mode to Aiutolaod in OTP_AS_CTRL
                                                                                              * 0: Enable SYSON autoload
                                                                                              * 1: Enable SYSON autoload
                                                                                              */
#define AON_BIT_OTP_AL_PON_EN                       ((u32)0x00000001 << 1)                   /*!< R/W   0  
                                                                                              * OTP PON Autoload Control
                                                                                              * If PMC/CPU wants to do this autoload, autoload done status and this bit should be cleared first.
                                                                                              * After cleared, PMC/CPU could set this bit first and then set mode to Aiutolaod in OTP_AS_CTRL
                                                                                              * 0: Disable PON autoload
                                                                                              * 1: Enable PON autoload
                                                                                              */
#define AON_BIT_OTP_AL_AON_EN                       ((u32)0x00000001 << 0)                   /*!< R/W   0  
                                                                                              * OTP AON Autoload Control
                                                                                              * If PMC/CPU wants to do this autoload, autoload done status and this bit should be cleared first.
                                                                                              * After cleared, PMC/CPU could set this bit first and then set mode to Aiutolaod in OTP_AS_CTRL
                                                                                              * 0: Disable AON autoload
                                                                                              * 1: Enable AON autoload
                                                                                              */
/** @} */


/** @defgroup REG_OTP_PWR_CTRL
 * @brief
 * @{
 */
#define AON_BIT_OTP_ISO2SYSON_STS                   ((u32)0x00000001 << 21)                  /*!< R       1
                                                                                              * For status of isolation to SYSON
                                                                                              * 0: Isolation to SYSON is disabled.
                                                                                              * 1: Isolation to SYSON is enabled.
                                                                                              */
#define AON_BIT_OTP_ISO2PON_STS                     ((u32)0x00000001 << 20)                  /*!< R       1
                                                                                              * For status of isolation to PON
                                                                                              * 0: Isolation to PON is disabled.
                                                                                              * 1: Isolation to PON is enabled.
                                                                                              */
#define AON_BIT_OTP_ISO2AON_STS                     ((u32)0x00000001 << 19)                  /*!< R       1
                                                                                              * For status of isolation to AON
                                                                                              * 0: Isolation to AON is disabled.
                                                                                              * 1: Isolation to AON is enabled.
                                                                                              */
#define AON_BIT_OTP_PC_BG_STS                       ((u32)0x00000001 << 18)                  /*!< R       0
                                                                                              * Big Power Cut Status
                                                                                              * 0: Big power cut in disabled state
                                                                                              * 1: Big power cut in enabled state.
                                                                                              */
#define AON_BIT_OTP_PC_SL_STS                       ((u32)0x00000001 << 17)                  /*!< R       0
                                                                                              * Small Power Cut Status
                                                                                              * 0: Small power cut in disabled state
                                                                                              * 1: Small power cut in enabled state.
                                                                                              */
#define AON_BIT_OTP_PGCLK_GT_EN                     ((u32)0x00000001 << 7)                   /*!< R/W     1
                                                                                              * For OTP program clock source gate control.
                                                                                              * (40MHz)
                                                                                              * 0: Program clock source is NOT gated.
                                                                                              * 1: Program clock source is gated.
                                                                                              */
#define AON_BIT_OTP_RDCLK_GT_EN                     ((u32)0x00000001 << 6)                   /*!< R/W   0  
                                                                                              * For OTP read clock source gate control.
                                                                                              * (4MHz)
                                                                                              * 0: Read clock source is NOT gated.
                                                                                              * 1: Read clock source is gated.
                                                                                              */
#define AON_BIT_OTP_ISO2SYSON_EN                    ((u32)0x00000001 << 5)                   /*!< R/W     1
                                                                                              * For OTP isolation to SYSON control.
                                                                                              * 0: Isolation to SYSON is disabled.
                                                                                              * 1: Isolation to SYSON is enabled.
                                                                                              */
#define AON_BIT_OTP_ISO2PON_EN                      ((u32)0x00000001 << 4)                   /*!< R/W     1
                                                                                              * For OTP isolation to PON control.
                                                                                              * 0: Isolation to PON is disabled.
                                                                                              * 1: Isolation to PON is enabled.
                                                                                              */
#define AON_BIT_OTP_ISO2AON_EN                      ((u32)0x00000001 << 3)                   /*!< R/W     1
                                                                                              * For OTP isolation to AON control.
                                                                                              * 0: Isolation to AON is disabled.
                                                                                              * 1: Isolation to AON is enabled.
                                                                                              * 
                                                                                              */
#define AON_BIT_OTP_PC_BG_EN                        ((u32)0x00000001 << 2)                   /*!< R/W   0  
                                                                                              * For Big Power cut control of VDD (0.9V) and VDD2 (3.3V)
                                                                                              * 0: Big power cut disabled. Therefore power is invalid
                                                                                              * 1: Big power cut enabled. Therefore power is valid
                                                                                              * 
                                                                                              */
#define AON_BIT_OTP_PC_SL_EN                        ((u32)0x00000001 << 1)                   /*!< R/W   0  
                                                                                              * For small Power cut control of VDD (0.9V) and VDD2 (3.3V)
                                                                                              * 0: Small power cut disabled. Therefore power is invalid
                                                                                              * 1: Small power cut enabled. Therefore power is valid
                                                                                              */
#define AON_BIT_OTP_CTRLER_FEN                      ((u32)0x00000001 << 0)                   /*!< R/W   0  
                                                                                              * OTP Controller (Loader) Enable.
                                                                                              * 0: Disable OTP controller
                                                                                              * 1: Enable OTP controller
                                                                                              */
/** @} */


/** @defgroup REG_OTP_PRCT_CTRL
 * @brief
 * @{
 */
#define AON_BIT_B_OTP_SZ_WL_CTRL                    ((u32)0x00000001 << 31)                  /*!< R/W Once   0
                                                                                              * Secure Zone Write Lock Control
                                                                                              * 0: OTP write lock control is disabed.
                                                                                              * 1: OTP write lock control is enabled.
                                                                                              */
#define AON_MASK_B_OTP_SS_CRC_STS                   ((u32)0x0001FFFF << 13)                  /*!< R/W1C   0
                                                                                              * Physical CRC Status
                                                                                              * Each bit represents one entry CRC status
                                                                                              * 1: CRC check failed
                                                                                              * 0: CRC check passed
                                                                                              * [13] IPSEC_KEY0
                                                                                              * [14] IPSEC_KEY1
                                                                                              * [15] IPSEC_KEY2
                                                                                              * [16] IPSEC_KEY3
                                                                                              * [17] NON-IPSEC_KEY0
                                                                                              * [18] NON-IPSEC_KEY1
                                                                                              * [19] PKE PRIVATE KEY0
                                                                                              * [20] PKE PRIVATE KEY1
                                                                                              * [21] RSIP_ECB
                                                                                              * [22] RSIP_CTR
                                                                                              * [23] SWD_SEC
                                                                                              * [24] SWD_Non_SEC
                                                                                              * [25] SEC_BOOTHASH0
                                                                                              * [26] SEC_BOOTHASH1
                                                                                              * [27] HUK
                                                                                              * [28] SEC_CFG
                                                                                              * [29] RMA_BOOTHASH
                                                                                              * 
                                                                                              */
#define AON_B_OTP_SS_CRC_STS(x)                     ((u32)(((x) & 0x0001FFFF) << 13))
#define AON_GET_B_OTP_SS_CRC_STS(x)                 ((u32)(((x >> 13) & 0x0001FFFF)))
#define AON_MASK_B_OTP_LZ_CRC_ER_ADDR               ((u32)0x000003FF << 3)                   /*!< R       0 OTP physical address of the first byte of the first CRC check error entry */
#define AON_B_OTP_LZ_CRC_ER_ADDR(x)                 ((u32)(((x) & 0x000003FF) << 3))
#define AON_GET_B_OTP_LZ_CRC_ER_ADDR(x)             ((u32)(((x >> 3) & 0x000003FF)))
#define AON_BIT_B_OTP_LZ_CRC_STS                    ((u32)0x00000001 << 2)                   /*!< R/W1C   0
                                                                                              * Logical Zone CRC Status
                                                                                              * 1: CRC check failed
                                                                                              * 0: CRC check passed
                                                                                              * 
                                                                                              */
#define AON_BIT_B_OTP_SC_CRC_STS                    ((u32)0x00000001 << 1)                   /*!< R/W1C   0
                                                                                              * Secure Control Zone CRC Status
                                                                                              * 1: CRC check failed
                                                                                              * 0: CRC check passed
                                                                                              * 
                                                                                              */
/** @} */


/** @defgroup REG_AON_SW_BOOT0
 * @brief
 * @{
 */
#define AON_MASK_SW0                                ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SW Control */
#define AON_SW0(x)                                  ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AON_GET_SW0(x)                              ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AON_FAST_RESUME
 * @brief
 * @{
 */
#define AON_MASK_AON_FAST_RESUME                    ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SW Control */
#define AON_AON_FAST_RESUME(x)                      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AON_GET_AON_FAST_RESUME(x)                  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AON_SW_BOOT1
 * @brief
 * @{
 */
#define AON_MASK_SW1                                ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SW Control */
#define AON_SW1(x)                                  ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AON_GET_SW1(x)                              ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AON_SW_BOOT2
 * @brief
 * @{
 */
#define AON_MASK_SW2                                ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SW Control */
#define AON_SW2(x)                                  ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AON_GET_SW2(x)                              ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AON_SW_BOOT3
 * @brief
 * @{
 */
#define AON_MASK_SW3                                ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SW Control */
#define AON_SW3(x)                                  ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AON_GET_SW3(x)                              ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AON_SW_BOOT4
 * @brief
 * @{
 */
#define AON_MASK_SW4                                ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SW Control */
#define AON_SW4(x)                                  ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AON_GET_SW4(x)                              ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AON_WDT_RST_CTRL
 * @brief
 * @{
 */
#define AON_BIT_PON_GPIO_WDT_RST_MSK                ((u32)0x00000001 << 9)                   /*!< R/W   0   1: Mask this bit and then WDOG_RST and CPU_RST can reset PON GPIO block */
#define AON_BIT_SYSON_GPIO_WDT_RST_MSK              ((u32)0x00000001 << 8)                   /*!< R/W   0   1: Mask this bit and then WDOG_RST and CPU_RST can reset SYSON GPIO block */
#define AON_BIT_AON_GPIO_WDT_RST_MSK                ((u32)0x00000001 << 7)                   /*!< R/W   0   1: Mask this bit and then WDOG_RST and CPU_RST can reset  AON GPIO block */
/** @} */


/** @defgroup REG_AON_WDT_TIMER
 * @brief
 * @{
 */
#define AON_MASK_PRE_SCALER                         ((u32)0x000003FF << 20)                  /*!< R/W     0
                                                                                              * Pre-scaler factor.
                                                                                              * Real pre-scaler is {pre-scaler factor,6'h3F}
                                                                                              */
#define AON_PRE_SCALER(x)                           ((u32)(((x) & 0x000003FF) << 20))
#define AON_GET_PRE_SCALER(x)                       ((u32)(((x >> 20) & 0x000003FF)))
#define AON_BIT_WDT_RST_TRIG                        ((u32)0x00000001 << 19)                  /*!< R/W     0 1: SW to trigger AON WDT reset. Auto clear by HW */
#define AON_BIT_WDT_CLR_EN                          ((u32)0x00000001 << 18)                  /*!< R/W     0 1: enable HW to disable AON WDT */
#define AON_BIT_WDT_CLR                             ((u32)0x00000001 << 17)                  /*!< R/W1P   0 Set 1 to clear the counter; auto clear to 0 by HW */
#define AON_MASK_CNT                                ((u32)0x0000FFFF << 1)                   /*!< R/W   0xFFF WDT Counter value */
#define AON_CNT(x)                                  ((u32)(((x) & 0x0000FFFF) << 1))
#define AON_GET_CNT(x)                              ((u32)(((x >> 1) & 0x0000FFFF)))
#define AON_BIT_WDT_EN_BYTE                         ((u32)0x00000001 << 0)                   /*!< R/W     1 Set 1 to enable watch dog timer */
/** @} */


/** @defgroup REG_AON_TIMER_CTR0
 * @brief
 * @{
 */
#define AON_MASK_AON_WDT_LOW_CNT                    ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0xFFFF_FFFF Low period of chip_alive output (in 32K clk) */
#define AON_AON_WDT_LOW_CNT(x)                      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AON_GET_AON_WDT_LOW_CNT(x)                  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AON_TIMER_CTR1
 * @brief
 * @{
 */
#define AON_MASK_AON_WDT_HIGH_CNT                   ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0xFFFF_FFFF High period of chip_alive output (in 32K clk) */
#define AON_AON_WDT_HIGH_CNT(x)                     ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AON_GET_AON_WDT_HIGH_CNT(x)                 ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AON_WAKE_TIMER
 * @brief
 * @{
 */
#define AON_BIT_AON_WAKE_TM_EN                      ((u32)0x00000001 << 31)                  /*!< R/W     0 AON wake timer Enable */
#define AON_BIT_AON_TIMER_ISR                       ((u32)0x00000001 << 30)                  /*!< R/W1C   0 1: Indicate  AON Timer Interrupt Status */
#define AON_MASK_AON_WAKE_TM_TC                     ((u32)0x3FFFFFFF << 0)                   /*!< R/W     0 AON wake timer counter value */
#define AON_AON_WAKE_TM_TC(x)                       ((u32)(((x) & 0x3FFFFFFF) << 0))
#define AON_GET_AON_WAKE_TM_TC(x)                   ((u32)(((x >> 0) & 0x3FFFFFFF)))
/** @} */


/** @defgroup REG_AON_KM0_WAKE_EVENT_MSK
 * @brief
 * @{
 */
#define AON_BIT_SYSON_WEVT_SGPIO_MSK_0E0            ((u32)0x00000001 << 15)                  /*!< R/W     0 1: enable SGPIO Wakeup event; 0: disable the event to wakeup system */
#define AON_BIT_SYSON_WEVT_ADC_MSK_0E0              ((u32)0x00000001 << 14)                  /*!< R/W     0 1: enable ADC Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_SYSON_WEVT_USB_MSK_0E0              ((u32)0x00000001 << 13)                  /*!< R/W     0 1: enable USB Wakeup event; 0: disable the event to wakeup system */
#define AON_BIT_AON_WEVT_AON_GPIO3_MSK_0E0          ((u32)0x00000001 << 12)                  /*!< R/W     0 1: enable  AON GPIO3 Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_AON_WEVT_AON_GPIO2_MSK_0E0          ((u32)0x00000001 << 11)                  /*!< R/W     0 1: enable  AON GPIO2 Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_AON_WEVT_AON_GPIO1_MSK_0E0          ((u32)0x00000001 << 10)                  /*!< R/W     0 1: enable  AON GPIO1 Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_AON_WEVT_AON_GPIO0_MSK_0E0          ((u32)0x00000001 << 9)                   /*!< R/W     0 1: enable  AON GPIO0 Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_AON_WEVT_AON_RTC_MSK_0E0            ((u32)0x00000001 << 8)                   /*!< R/W     0 1: enable  AON RTC Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_AON_WEVT_AON_COMP_MSK_0E0           ((u32)0x00000001 << 7)                   /*!< R/W     0 1: enable  AON Comparator Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_AON_WEVT_AON_TIMER_MSK_0E0          ((u32)0x00000001 << 6)                   /*!< R/W     0 1: enable  AON Timer Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_PON_WEVT_LOGUART_MSK_0E0            ((u32)0x00000001 << 5)                   /*!< R/W     0 1: enable LOGUART  Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_PON_WEVT_WLAN1_MSK_0E0              ((u32)0x00000001 << 4)                   /*!< R/W     0 1: enable  WLAN Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_PON_WEVT_WLAN_MSK_0E0               ((u32)0x00000001 << 3)                   /*!< R/W     0 1: enable  WLAN Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_PON_WEVT_PWM_MSK_0E0                ((u32)0x00000001 << 2)                   /*!< R/W     0 1: enable PWM Wakeup event; 0: disable the event to wakeup system */
#define AON_BIT_PON_WEVT_GPIO_MSK_0E0               ((u32)0x00000001 << 1)                   /*!< R/W     0 1: enable PON GPIO Wakeup  event; 0: disable the event to wakeup system */
#define AON_BIT_PON_WEVT_GTIM0_MSK_0E0              ((u32)0x00000001 << 0)                   /*!< R/W     0 1: enable GTIMER0 Wakeup event; 0: disable the event to wakeup system */
/** @} */


/** @defgroup REG_AP_WDT_TIMER
 * @brief
 * @{
 */
#define AON_MASK_PRE_SCALER_0E4                     ((u32)0x000003FF << 20)                  /*!< R/W     0
                                                                                              * Pre-scaler factor.
                                                                                              * Real pre-scaler is {pre-scaler factor,6'h3F}
                                                                                              */
#define AON_PRE_SCALER_0E4(x)                       ((u32)(((x) & 0x000003FF) << 20))
#define AON_GET_PRE_SCALER_0E4(x)                   ((u32)(((x >> 20) & 0x000003FF)))
#define AON_BIT_WDT_RST_TRIG_0E4                    ((u32)0x00000001 << 19)                  /*!< R/W     0 1: SW to trigger AON WDT reset. Auto clear by HW */
#define AON_BIT_WDT_CLR_0E4                         ((u32)0x00000001 << 17)                  /*!< R/W1P   0 Set 1 to clear the counter; auto clear to 0 by HW */
#define AON_MASK_CNT_0E4                            ((u32)0x0000FFFF << 1)                   /*!< R/W   0xFFF WDT Counter value */
#define AON_CNT_0E4(x)                              ((u32)(((x) & 0x0000FFFF) << 1))
#define AON_GET_CNT_0E4(x)                          ((u32)(((x >> 1) & 0x0000FFFF)))
#define AON_BIT_WDT_EN_BYTE_0E4                     ((u32)0x00000001 << 0)                   /*!< R/W     1 Set 1 to enable watch dog timer */
/** @} */




/** @defgroup AON_Exported_Types AON Exported Types
* @{
*/
/**
* @brief AON Register Declaration
*/

typedef struct {
	__IO uint32_t AON_PWR_CTRL;                                 /*!< Register,  Address offset:0x0000 */
	__IO uint32_t AON_ISO_CTRL;                                 /*!< Register,  Address offset:0x0004 */
	__IO uint32_t AON_32KCLK_CTRL;                              /*!< Register,  Address offset:0x0008 */
	__IO uint32_t AON_AONCLK_CTRL;                              /*!< Register,  Address offset:0x000C */
	__I  uint32_t AON_DUMMY0;                                   /*!< Register,  Address offset:0x0010 */
	__IO uint32_t AON_CLK_CTRL;                                 /*!< Register,  Address offset:0x0014 */
	__I  uint32_t AON_DUMMY1;                                   /*!< Register,  Address offset:0x0018 */
	__IO uint32_t AON_FUNC_CTRL1;                               /*!< Register,  Address offset:0x001C */
	__IO uint32_t AON_PLATFORM_VER;                             /*!< Register,  Address offset:0x0020 */
	__I  uint32_t AON_DUMMY2;                                   /*!< Register,  Address offset:0x0024 */
	__I  uint32_t AON_DUMMY8;                                   /*!< Register,  Address offset:0x0028 */
	__IO uint32_t AON_LSFIF_CMD;                                /*!< Register,  Address offset:0x002C */
	__IO uint32_t AON_LSFIF_RWD;                                /*!< Register,  Address offset:0x0030 */
	__IO uint32_t AON_TEST_MUX_CTRL;                            /*!< Register,  Address offset:0x0034 */
	__IO uint32_t AON_TEST_MUX_CTRL1;                           /*!< Register,  Address offset:0x0038 */
	__IO uint32_t AON_TEST_MUX_CTRL2;                           /*!< Register,  Address offset:0x003C */
	__IO uint32_t AON_ATLOAD_CTRL2;                             /*!< Register,  Address offset:0x0040 */
	__I  uint32_t AON_SYS_INFO0;                                /*!< Register,  Address offset:0x0044 */
	__IO uint32_t AON_SYS_INFO1;                                /*!< Register,  Address offset:0x0048 */
	__IO uint32_t AON_OTP_SYSCFG2;                              /*!< Register,  Address offset:0x004C */
	__IO uint32_t AON_OTP_SYSCFG3;                              /*!< Register,  Address offset:0x0050 */
	__IO uint32_t AON_OTP_SYSCFG4;                              /*!< Register,  Address offset:0x0054 */
	__IO uint32_t AON_OTP_SYSCFG5;                              /*!< Register,  Address offset:0x0058 */
	__IO uint32_t AON_OTP_SYSCFG6;                              /*!< Register,  Address offset:0x005C */
	__IO uint32_t AON_OTP_SYSCFG7;                              /*!< Register,  Address offset:0x0060 */
	__I  uint32_t AON_DBG_REG;                                  /*!< Register,  Address offset:0x0064 */
	__IO uint32_t AON_SYS_CTRL;                                 /*!< Register,  Address offset:0x0068 */
	__IO uint32_t AON_SIC_DBG_CTRL;                             /*!< Register,  Address offset:0x006C */
	__I  uint32_t AON_GPIO1_WAKE_CTRL;                          /*!< Register,  Address offset:0x0070 */
	__I  uint32_t AON_GPIO2_WAKE_CTRL;                          /*!< Register,  Address offset:0x0074 */
	__IO uint32_t AON_WL_CTRL;                                  /*!< Register,  Address offset:0x0078 */
	__IO uint32_t AON_PMC_CTRL;                                 /*!< Register,  Address offset:0x007C */
	__IO uint32_t AON_PMC_OPT;                                  /*!< Register,  Address offset:0x0080 */
	__IO uint32_t AON_NP_BOOT_REASON;                           /*!< Register,  Address offset:0x0084 */
	__IO uint32_t AON_CA32_BOOT_REASON;                         /*!< Register,  Address offset:0x0088 */
	__IO uint32_t AON_SLP_WAKE_EVENT_MSK0;                      /*!< Register,  Address offset:0x008C */
	__IO uint32_t AON_SLP_WAKE_EVENT_STATUS0;                   /*!< Register,  Address offset:0x0090 */
	__IO uint32_t AON_SNF_WAKE_EVENT_MSK0;                      /*!< Register,  Address offset:0x0094 */
	__IO uint32_t AON_SNF_WAKE_EVENT_STATUS;                    /*!< Register,  Address offset:0x0098 */
	__IO uint32_t AON_PWRMGT_OPTION;                            /*!< Register,  Address offset:0x009C */
	__IO uint32_t OTP_AL_CTRL_STS;                              /*!< Register,  Address offset:0x00A0 */
	__IO uint32_t OTP_PWR_CTRL;                                 /*!< Register,  Address offset:0x00A4 */
	__IO uint32_t OTP_PRCT_CTRL;                                /*!< Register,  Address offset:0x00A8 */
	__IO uint32_t AON_SW_BOOT0;                                 /*!< Register,  Address offset:0x00AC */
	__IO uint32_t AON_FAST_RESUME;                              /*!< Register,  Address offset:0x00B0 */
	__IO uint32_t AON_SW_BOOT1;                                 /*!< Register,  Address offset:0x00B4 */
	__IO uint32_t AON_SW_BOOT2;                                 /*!< Register,  Address offset:0x00B8 */
	__IO uint32_t AON_SW_BOOT3;                                 /*!< Register,  Address offset:0x00BC */
	__IO uint32_t AON_SW_BOOT4;                                 /*!< Register,  Address offset:0x00C0 */
	__IO uint32_t AON_WDT_RST_CTRL;                             /*!< Register,  Address offset:0x00C4 */
	__IO uint32_t AON_WDT_TIMER;                                /*!< Register,  Address offset:0x00C8 */
	__IO uint32_t AON_TIMER_CTR0;                               /*!< Register,  Address offset:0x00CC */
	__IO uint32_t AON_TIMER_CTR1;                               /*!< Register,  Address offset:0x00D0 */
	__IO uint32_t AON_WAKE_TIMER;                               /*!< Register,  Address offset:0x00D4 */
	uint32_t RSVD_NO_FUNCTION0[2];
	__IO uint32_t AON_KM0_WAKE_EVENT_MSK;                       /*!< Register,  Address offset:0x00E0 */
	__IO uint32_t AP_WDT_TIMER;                                 /*!< Register,  Address offset:0x00E4 */
} AON_TypeDef;

#define AON_BASE ((AON_TypeDef *) (0x40220000))
/** @} */


/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif

//We select reserved addres to use from RSVD_NO_FUNCTION0
#define REG_AON_RSVD_FOR_SW1              0x0D8
//Please add your defination here
#define REG_AON_WIFI_IPC		(SYSTEM_CTRL_BASE + REG_AON_RSVD_FOR_SW1)
/** @defgroup REG_AON_RSVD_FOR_SW1
  * @brief
  * @{
  */
#define AON_BIT_WIFI_INIC_NP_READY ((u32)0x00000001 << 31)          /*!< R/W 0x0  Wi-Fi NP initialization ready flag*/
#define AON_MASK_RSVD_SW1          ((u32)0x7FFFFFFF << 0)           /*!< R/W 0x0  */
#define AON_RSVD_SW1(x)            (((u32)((x) & 0x7FFFFFFF) << 0))
#define AON_GET_RSVD_SW1(x)        ((u32)(((x >> 0) & 0x7FFFFFFF)))


#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

/** @} */

/** @} */
#endif  // SYSREG_AON_H
