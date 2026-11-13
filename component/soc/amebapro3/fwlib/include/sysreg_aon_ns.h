/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_AON_NS_H
#define SYSREG_AON_NS_H

/* AUTO_GEN_START */

/* AON_NS is a part of AON_NS */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup AON_NS
* @brief AON_NS driver modules
* @{
*/


/** @defgroup REG_AON_FW_BOOT_REASON
 * @brief
 * @{
 */
#define AON_NS_BIT_CA32WDT_STS                      ((u32)0x00000001 << 9)                   /*!< R       0 1: Indicate boot reason is CA32 Watchdog. */
#define AON_NS_BIT_FWWDT_STS                        ((u32)0x00000001 << 8)                   /*!< R       0 1: Indicate boot reason is KM0 FW Watchdog. */
#define AON_NS_BIT_MPWDT_STS                        ((u32)0x00000001 << 7)                   /*!< R       0 1: Indicate boot reason is KM4 MP Watchdog. */
#define AON_NS_BIT_NPWDT_STS                        ((u32)0x00000001 << 6)                   /*!< R       0 1: Indicate boot reason is KM4 NP Watchdog. */
#define AON_NS_BIT_BOD_ISR_STS                      ((u32)0x00000001 << 5)                   /*!< R/W1C   0 1: Indicate BOD interrupt and clear by SW. */
#define AON_NS_BIT_BOD_STS                          ((u32)0x00000001 << 4)                   /*!< R       0 1: Indicate reboot reason is BOD detection. */
#define AON_NS_BIT_AON_STANDBY_STS                  ((u32)0x00000001 << 3)                   /*!< R       0 1: Indicated boot reason is wake up event which resumes KM0 from STBY or SNZMCU (except the first boot up from SNZMCU) */
#define AON_NS_BIT_AON_DSLP_STS                     ((u32)0x00000001 << 2)                   /*!< R       0 1: Indicated boot reason is wake up event which resumes KM4NP from DSLP. */
#define AON_NS_BIT_AON_WATCHDOG_STS                 ((u32)0x00000001 << 1)                   /*!< R       0 1: Indicate boot reason is AON Watchdog. */
#define AON_NS_BIT_PWR_ON_STS                       ((u32)0x00000001 << 0)                   /*!< R       1 1: Indicate boot reason is power on reset or SW reset (The first boot up from SNZMCU). */
/** @} */


/** @defgroup REG_AON_MP_BOOT_REASON
 * @brief
 * @{
 */
#define AON_NS_BIT_CA32WDT_STS_004                  ((u32)0x00000001 << 9)                   /*!< R       0 1: Indicate boot reason is CA32 Watchdog. */
#define AON_NS_BIT_FWWDT_STS_004                    ((u32)0x00000001 << 8)                   /*!< R       0 1: Indicate boot reason is KM0 FW Watchdog. */
#define AON_NS_BIT_MPWDT_STS_004                    ((u32)0x00000001 << 7)                   /*!< R       0 1: Indicate boot reason is KM4 MP Watchdog. */
#define AON_NS_BIT_NPWDT_STS_004                    ((u32)0x00000001 << 6)                   /*!< R       0 1: Indicate boot reason is KM4 NP Watchdog. */
#define AON_NS_BIT_BOD_ISR_STS_004                  ((u32)0x00000001 << 5)                   /*!< R/W1C   0 1: Indicate BOD interrupt and clear by SW. */
#define AON_NS_BIT_BOD_STS_004                      ((u32)0x00000001 << 4)                   /*!< R       0 1: Indicate reboot reason is BOD detection. */
#define AON_NS_BIT_AON_STANDBY_STS_004              ((u32)0x00000001 << 3)                   /*!< R       0 NOT USE */
#define AON_NS_BIT_AON_DSLP_STS_004                 ((u32)0x00000001 << 2)                   /*!< R       0 1: Indicated boot reason is wake up event which resumes KM4NP from DSLP. */
#define AON_NS_BIT_AON_WATCHDOG_STS_004             ((u32)0x00000001 << 1)                   /*!< R       0 1: Indicate boot reason is AON Watchdog. */
#define AON_NS_BIT_PWR_ON_STS_004                   ((u32)0x00000001 << 0)                   /*!< R       1 1: Indicate boot reason is power on reset or SW reset. */
/** @} */


/** @defgroup REG_AON_VERSION_STS
 * @brief
 * @{
 */
#define AON_NS_MASK_CHIP_TYPE                       ((u32)0x00000003 << 8)                   /*!< R     0x0
                                                                                              * 00: ASIC
                                                                                              * 01: FPGA
                                                                                              * 10: PXP
                                                                                              * 11: RTL SIM
                                                                                              */
#define AON_NS_CHIP_TYPE(x)                         ((u32)(((x) & 0x00000003) << 8))
#define AON_NS_GET_CHIP_TYPE(x)                     ((u32)(((x >> 8) & 0x00000003)))
#define AON_NS_MASK_MASTER_ID                       ((u32)0x00000007 << 5)                   /*!< R     0x0
                                                                                              * 000: KM4 NP is reading
                                                                                              * 001: CA32 is reading
                                                                                              * 010: KM4MP is reading
                                                                                              * 011: KM0FW is reading
                                                                                              * 100: SIC is reading
                                                                                              */
#define AON_NS_MASTER_ID(x)                         ((u32)(((x) & 0x00000007) << 5))
#define AON_NS_GET_MASTER_ID(x)                     ((u32)(((x >> 5) & 0x00000007)))
#define AON_NS_MASK_CUT_VERSION                     ((u32)0x0000001F << 0)                   /*!< R     0x0 0x0: A-cut */
#define AON_NS_CUT_VERSION(x)                       ((u32)(((x) & 0x0000001F) << 0))
#define AON_NS_GET_CUT_VERSION(x)                   ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_AON_PMC_CTRL
 * @brief
 * @{
 */
#define AON_NS_MASK_AON_PMC_STATE                   ((u32)0x00000007 << 19)                  /*!< R     0x0
                                                                                              * PMC STATE:
                                                                                              * 0: OFF
                                                                                              * 1: SNOZ COMBO
                                                                                              * 2: SNOZ WIFI
                                                                                              * 3: SNOZ MCU
                                                                                              * 4: STANDBY
                                                                                              * 5: DEEP SLEEP
                                                                                              */
#define AON_NS_AON_PMC_STATE(x)                     ((u32)(((x) & 0x00000007) << 19))
#define AON_NS_GET_AON_PMC_STATE(x)                 ((u32)(((x >> 19) & 0x00000007)))
#define AON_NS_BIT_AON_PMC_INT_IMR                  ((u32)0x00000001 << 18)                  /*!< R/W   0x0 1: Enable PMC interrupt */
#define AON_NS_BIT_AON_PMC_INT_STS                  ((u32)0x00000001 << 17)                  /*!< R/W1C 0x0 1: trigger interruput to KM0FW/KM4MP. Clear by SW write 1 */
#define AON_NS_BIT_PWROFF_DLAY_EN                   ((u32)0x00000001 << 2)                   /*!< R/W     0
                                                                                              * 1: Enable latency to turn off WIFIOFF power. 
                                                                                              * The latency is from controlled register to PMC. Current latency is 2 slow_pclk clock cycles.
                                                                                              */
#define AON_NS_BIT_WMAC_100K_SEL                    ((u32)0x00000001 << 1)                   /*!< R/W     0 1: select 100K clock for WMAC ana clock during LPS */
#define AON_NS_BIT_KM0FW_ACTIVE                     ((u32)0x00000001 << 0)                   /*!< R/W     0 1: Indicate KM0 FW is in active; PMC won't turn off WIFIOFF power */
/** @} */


/** @defgroup REG_AON_SW_BOOT0
 * @brief
 * @{
 */
#define AON_NS_MASK_SW0                             ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SW Control */
#define AON_NS_SW0(x)                               ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AON_NS_GET_SW0(x)                           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AON_SW_BOOT1
 * @brief
 * @{
 */
#define AON_NS_MASK_AON_FAST_RESUME                 ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SW Control */
#define AON_NS_AON_FAST_RESUME(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AON_NS_GET_AON_FAST_RESUME(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AON_SW_BOOT2
 * @brief
 * @{
 */
#define AON_NS_MASK_SW1                             ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SW Control */
#define AON_NS_SW1(x)                               ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AON_NS_GET_SW1(x)                           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AON_SW_BOOT3
 * @brief
 * @{
 */
#define AON_NS_MASK_SW2                             ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SW Control */
#define AON_NS_SW2(x)                               ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AON_NS_GET_SW2(x)                           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AON_SW_BOOT4
 * @brief
 * @{
 */
#define AON_NS_MASK_SW3                             ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SW Control */
#define AON_NS_SW3(x)                               ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AON_NS_GET_SW3(x)                           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AON_SW_BOOT5
 * @brief
 * @{
 */
#define AON_NS_MASK_SW4                             ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SW Control */
#define AON_NS_SW4(x)                               ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AON_NS_GET_SW4(x)                           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AON_GPIO_HEART_BEAT
 * @brief
 * @{
 */
#define AON_NS_MASK_GPIO_FORCE                      ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 Use by non sec SW to emulate the GPIO heart beat function */
#define AON_NS_GPIO_FORCE(x)                        ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AON_NS_GET_GPIO_FORCE(x)                    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_AON_SYSMEM_LPS_CTRL
 * @brief
 * @{
 */
#define AON_NS_MASK_KM0_SLP_MEM7_PM_EN              ((u32)0x00000003 << 29)                  /*!< R/W     0
                                                                                              * 0: Enable SRAM7 normal mode when enter into power mode
                                                                                              * 1: Enable SRAM7 RET mode when enter into power mode
                                                                                              * 2: Enable SRAM7 SD mode when enter into power mode
                                                                                              */
#define AON_NS_KM0_SLP_MEM7_PM_EN(x)                ((u32)(((x) & 0x00000003) << 29))
#define AON_NS_GET_KM0_SLP_MEM7_PM_EN(x)            ((u32)(((x >> 29) & 0x00000003)))
#define AON_NS_MASK_KM0_SLP_MEM6_PM_EN              ((u32)0x00000003 << 27)                  /*!< R/W     0
                                                                                              * 0: Enable SRAM6 normal mode when enter into power mode
                                                                                              * 1: Enable SRAM6 RET mode when enter into power mode
                                                                                              * 2: Enable SRAM6 SD mode when enter into power mode
                                                                                              */
#define AON_NS_KM0_SLP_MEM6_PM_EN(x)                ((u32)(((x) & 0x00000003) << 27))
#define AON_NS_GET_KM0_SLP_MEM6_PM_EN(x)            ((u32)(((x >> 27) & 0x00000003)))
#define AON_NS_MASK_KM0_SLP_MEM5_PM_EN              ((u32)0x00000003 << 25)                  /*!< R/W     0
                                                                                              * 0: Enable SRAM5 normal mode when enter into power mode
                                                                                              * 1: Enable SRAM5 RET mode when enter into power mode
                                                                                              * 2: Enable SRAM5 SD mode when enter into power mode
                                                                                              */
#define AON_NS_KM0_SLP_MEM5_PM_EN(x)                ((u32)(((x) & 0x00000003) << 25))
#define AON_NS_GET_KM0_SLP_MEM5_PM_EN(x)            ((u32)(((x >> 25) & 0x00000003)))
#define AON_NS_MASK_KM0_SLP_MEM4_PM_EN              ((u32)0x00000003 << 23)                  /*!< R/W     0
                                                                                              * 0: Enable SRAM4 normal mode when enter into power mode
                                                                                              * 1: Enable SRAM4 RET mode when enter into power mode
                                                                                              * 2: Enable SRAM4 SD mode when enter into power mode
                                                                                              */
#define AON_NS_KM0_SLP_MEM4_PM_EN(x)                ((u32)(((x) & 0x00000003) << 23))
#define AON_NS_GET_KM0_SLP_MEM4_PM_EN(x)            ((u32)(((x >> 23) & 0x00000003)))
#define AON_NS_MASK_KM0_SLP_MEM3_PM_EN              ((u32)0x00000003 << 21)                  /*!< R/W     0
                                                                                              * 0: Enable SRAM3 normal mode when enter into power mode
                                                                                              * 1: Enable SRAM3 RET mode when enter into power mode
                                                                                              * 2: Enable SRAM3 SD mode when enter into power mode
                                                                                              */
#define AON_NS_KM0_SLP_MEM3_PM_EN(x)                ((u32)(((x) & 0x00000003) << 21))
#define AON_NS_GET_KM0_SLP_MEM3_PM_EN(x)            ((u32)(((x >> 21) & 0x00000003)))
#define AON_NS_MASK_KM0_SLP_MEM2_PM_EN              ((u32)0x00000003 << 19)                  /*!< R/W     0
                                                                                              * 0: Enable SRAM2 normal mode when enter into power mode
                                                                                              * 1: Enable SRAM2 RET mode when enter into power mode
                                                                                              * 2: Enable SRAM2 SD mode when enter into power mode
                                                                                              */
#define AON_NS_KM0_SLP_MEM2_PM_EN(x)                ((u32)(((x) & 0x00000003) << 19))
#define AON_NS_GET_KM0_SLP_MEM2_PM_EN(x)            ((u32)(((x >> 19) & 0x00000003)))
#define AON_NS_MASK_KM0_SLP_MEM1_PM_EN              ((u32)0x00000003 << 17)                  /*!< R/W     0
                                                                                              * 0: Enable SRAM1 normal mode when enter into power mode
                                                                                              * 1: Enable SRAM1 RET mode when enter into power mode
                                                                                              * 2: Enable SRAM1 SD mode when enter into power mode
                                                                                              */
#define AON_NS_KM0_SLP_MEM1_PM_EN(x)                ((u32)(((x) & 0x00000003) << 17))
#define AON_NS_GET_KM0_SLP_MEM1_PM_EN(x)            ((u32)(((x >> 17) & 0x00000003)))
#define AON_NS_MASK_KM0_SLP_MEM0_PM_EN              ((u32)0x00000003 << 15)                  /*!< R/W     0
                                                                                              * 0: Enable SRAM0 normal mode when enter into power mode
                                                                                              * 1: Enable SRAM0 RET mode when enter into power mode
                                                                                              * 2: Enable SRAM0 SD mode when enter into power mode
                                                                                              */
#define AON_NS_KM0_SLP_MEM0_PM_EN(x)                ((u32)(((x) & 0x00000003) << 15))
#define AON_NS_GET_KM0_SLP_MEM0_PM_EN(x)            ((u32)(((x >> 15) & 0x00000003)))
/** @} */


/** @defgroup REG_MP_WDT_TIMER
 * @brief
 * @{
 */
#define AON_NS_MASK_PRE_SCALER                      ((u32)0x000003FF << 20)                  /*!< R/W     0
                                                                                              * Pre-scaler factor.
                                                                                              * Real pre-scaler is {pre-scaler factor,6'h3F}
                                                                                              */
#define AON_NS_PRE_SCALER(x)                        ((u32)(((x) & 0x000003FF) << 20))
#define AON_NS_GET_PRE_SCALER(x)                    ((u32)(((x >> 20) & 0x000003FF)))
#define AON_NS_BIT_WDT_RST_TRIG                     ((u32)0x00000001 << 19)                  /*!< R/W     0 1: SW to trigger AON WDT reset. Auto clear by HW */
#define AON_NS_BIT_WDT_CLR                          ((u32)0x00000001 << 17)                  /*!< R/W1P   0 Set 1 to clear the counter; auto clear to 0 by HW */
#define AON_NS_MASK_CNT                             ((u32)0x0000FFFF << 1)                   /*!< R/W   0xFFF WDT Counter value */
#define AON_NS_CNT(x)                               ((u32)(((x) & 0x0000FFFF) << 1))
#define AON_NS_GET_CNT(x)                           ((u32)(((x >> 1) & 0x0000FFFF)))
#define AON_NS_BIT_WDT_EN_BYTE                      ((u32)0x00000001 << 0)                   /*!< R/W     1 Set 1 to enable watch dog timer */
/** @} */


/** @defgroup REG_FW_WDT_TIMER
 * @brief
 * @{
 */
#define AON_NS_MASK_PRE_SCALER_034                  ((u32)0x000003FF << 20)                  /*!< R/W     0
                                                                                              * Pre-scaler factor.
                                                                                              * Real pre-scaler is {pre-scaler factor,6'h3F}
                                                                                              */
#define AON_NS_PRE_SCALER_034(x)                    ((u32)(((x) & 0x000003FF) << 20))
#define AON_NS_GET_PRE_SCALER_034(x)                ((u32)(((x >> 20) & 0x000003FF)))
#define AON_NS_BIT_WDT_RST_TRIG_034                 ((u32)0x00000001 << 19)                  /*!< R/W     0 1: SW to trigger AON WDT reset. Auto clear by HW */
#define AON_NS_BIT_WDT_CLR_034                      ((u32)0x00000001 << 17)                  /*!< R/W1P   0 Set 1 to clear the counter; auto clear to 0 by HW */
#define AON_NS_MASK_CNT_034                         ((u32)0x0000FFFF << 1)                   /*!< R/W   0xFFF WDT Counter value */
#define AON_NS_CNT_034(x)                           ((u32)(((x) & 0x0000FFFF) << 1))
#define AON_NS_GET_CNT_034(x)                       ((u32)(((x >> 1) & 0x0000FFFF)))
#define AON_NS_BIT_WDT_EN_BYTE_034                  ((u32)0x00000001 << 0)                   /*!< R/W     1 Set 1 to enable watch dog timer */
/** @} */


/** @defgroup REG_ANACK_CAL_CTRL
 * @brief
 * @{
 */
#define AON_NS_BIT_ANACK_CAL_CMD                    ((u32)0x00000001 << 31)                  /*!< R/W     0 1: Start the calibration; HW is to clear this bit when the calibration is done. */
#define AON_NS_MASK_ANACK_CAL_SEL                   ((u32)0x00000003 << 24)                  /*!< R/W     0
                                                                                              * Clock to be selected for calibration:
                                                                                              * 0: 4M clk
                                                                                              * 1: 128K clk
                                                                                              * 2: 32K clk
                                                                                              */
#define AON_NS_ANACK_CAL_SEL(x)                     ((u32)(((x) & 0x00000003) << 24))
#define AON_NS_GET_ANACK_CAL_SEL(x)                 ((u32)(((x >> 24) & 0x00000003)))
#define AON_NS_MASK_ANACK_CAL_RPT                   ((u32)0x0000FFFF << 0)                   /*!< R       0 Calibration result */
#define AON_NS_ANACK_CAL_RPT(x)                     ((u32)(((x) & 0x0000FFFF) << 0))
#define AON_NS_GET_ANACK_CAL_RPT(x)                 ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_SLP_WAKE_EVENT_STATUS0
 * @brief
 * @{
 */
#define AON_NS_BIT_SYSON_WEVT_FWIPC_STS             ((u32)0x00000001 << 17)                  /*!< R       0 1: Indicate IPC KM0 FW Wakeup event */
#define AON_NS_BIT_SYSON_WEVT_NPIPC_STS             ((u32)0x00000001 << 16)                  /*!< R       0 1: Indicate IPC KM4 NP Wakeup event */
#define AON_NS_BIT_SYSON_WEVT_SGPIO_STS             ((u32)0x00000001 << 15)                  /*!< R       0 1: Indicate SGPIO Wakeup event */
#define AON_NS_BIT_SYSON_WEVT_ADC_STS               ((u32)0x00000001 << 14)                  /*!< R       0 1: Indicate ADC Wakeup  event */
#define AON_NS_BIT_SYSON_WEVT_USB_STS               ((u32)0x00000001 << 13)                  /*!< R       0 1: Indicate USB Wakeup event */
#define AON_NS_BIT_AON_WEVT_AON_GPIO3_STS           ((u32)0x00000001 << 12)                  /*!< R/W1C   0 1: indicate  AON GPIO3 Wakeup  event;  */
#define AON_NS_BIT_AON_WEVT_AON_GPIO2_STS           ((u32)0x00000001 << 11)                  /*!< R/W1C   0 1: indicate  AON GPIO2 Wakeup  event;  */
#define AON_NS_BIT_AON_WEVT_AON_GPIO1_STS           ((u32)0x00000001 << 10)                  /*!< R/W1C   0 1: indicate  AON GPIO1 Wakeup  event;  */
#define AON_NS_BIT_AON_WEVT_AON_GPIO0_STS           ((u32)0x00000001 << 9)                   /*!< R/W1C   0 1: indicate  AON GPIO0 Wakeup  event; */
#define AON_NS_BIT_AON_WEVT_AON_RTC_STS             ((u32)0x00000001 << 8)                   /*!< R       0 1: indicate  AON RTC Wakeup  event */
#define AON_NS_BIT_AON_WEVT_AON_COMP_STS            ((u32)0x00000001 << 7)                   /*!< R       0 1: indicate  AON Comparator Wakeup  event; */
#define AON_NS_BIT_AON_WEVT_AON_TIMER_STS           ((u32)0x00000001 << 6)                   /*!< R/W1C   0 1: indicate  AON Timer Wakeup  event;  */
#define AON_NS_BIT_PON_WEVT_LOGUART_STS             ((u32)0x00000001 << 5)                   /*!< R       0 1: indicate LOGUART  Wakeup  event; */
#define AON_NS_BIT_PON_WEVT_WLAN1_STS               ((u32)0x00000001 << 4)                   /*!< R       0 1: indicate  WLAN Wakeup  event (INTF1_EN);  */
#define AON_NS_BIT_PON_WEVT_WLAN_STS                ((u32)0x00000001 << 3)                   /*!< R       0 1: indicate  WLAN Wakeup  event (INTF3_EN);  */
#define AON_NS_BIT_PON_WEVT_PWM_STS                 ((u32)0x00000001 << 2)                   /*!< R       0 1: indicate PWM Wakeup event; */
#define AON_NS_BIT_PON_WEVT_GPIO_STS                ((u32)0x00000001 << 1)                   /*!< R       0 1: indicate PON GPIO Wakeup event;  */
#define AON_NS_BIT_PON_WEVT_GTIM0_STS               ((u32)0x00000001 << 0)                   /*!< R       0 1: indicate GTIMER0 Wakeup  event;  */
/** @} */


/** @defgroup REG_AON_NS_TIMER_CTRL
 * @brief
 * @{
 */
#define AON_NS_BIT_AON_TIMER_ISR                    ((u32)0x00000001 << 0)                   /*!< R/W1C   0 1: Indicate  AON Timer Interrupt Status */
/** @} */




/** @defgroup AON_NS_Exported_Types AON_NS Exported Types
* @{
*/
/**
* @brief AON_NS Register Declaration
*/

typedef struct {
	__IO uint32_t AON_FW_BOOT_REASON;                           /*!< Register,  Address offset:0x0000 */
	__IO uint32_t AON_MP_BOOT_REASON;                           /*!< Register,  Address offset:0x0004 */
	__IO uint32_t AON_VERSION_STS;                              /*!< Register,  Address offset:0x0008 */
	__IO uint32_t AON_PMC_CTRL;                                 /*!< Register,  Address offset:0x000C */
	__IO uint32_t AON_SW_BOOT0;                                 /*!< Register,  Address offset:0x0010 */
	__IO uint32_t AON_SW_BOOT1;                                 /*!< Register,  Address offset:0x0014 */
	__IO uint32_t AON_SW_BOOT2;                                 /*!< Register,  Address offset:0x0018 */
	__IO uint32_t AON_SW_BOOT3;                                 /*!< Register,  Address offset:0x001C */
	__IO uint32_t AON_SW_BOOT4;                                 /*!< Register,  Address offset:0x0020 */
	__IO uint32_t AON_SW_BOOT5;                                 /*!< Register,  Address offset:0x0024 */
	__IO uint32_t AON_GPIO_HEART_BEAT;                          /*!< Register,  Address offset:0x0028 */
	__IO uint32_t AON_SYSMEM_LPS_CTRL;                          /*!< Register,  Address offset:0x002C */
	__IO uint32_t MP_WDT_TIMER;                                 /*!< Register,  Address offset:0x0030 */
	__IO uint32_t FW_WDT_TIMER;                                 /*!< Register,  Address offset:0x0034 */
	__IO uint32_t ANACK_CAL_CTRL;                               /*!< Register,  Address offset:0x0038 */
	__IO uint32_t SLP_WAKE_EVENT_STATUS0;                       /*!< Register,  Address offset:0x003C */
	__IO uint32_t AON_NS_TIMER_CTRL;                            /*!< Register,  Address offset:0x0040 */
} AON_NS_TypeDef;

#define AON_NS_BASE ((AON_NS_TypeDef *) (0x40210000))
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
#endif  // SYSREG_AON_NS_H
