/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_SYSON_H
#define SYSREG_SYSON_H

/* AUTO_GEN_START */

/* SYSON is a part of PON */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup SYSON
* @brief SYSON driver modules
* @{
*/


/** @defgroup REG_PON_SYSON_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_PON_ISO_CORE2CPUPLL               ((u32)0x00000001 << 26)                  /*!< R/W   1   NOT USED */
#define SYSON_BIT_PON_ISO_CPUPLL                    ((u32)0x00000001 << 25)                  /*!< R/W   1  
                                                                                              * Enable ISO cell for data path from CPUPLL. 
                                                                                              * + Set to 1 before power off CPUPLL PD.
                                                                                              * + Set to 0 after power on CPUPLL PD.
                                                                                              */
#define SYSON_BIT_PON_ISO_PERIPLL                   ((u32)0x00000001 << 24)                  /*!< R/W   1  
                                                                                              * Enable ISO cell for data path from PERIPLL. 
                                                                                              * + Set to 1 before power off PERIPLL PD.
                                                                                              * + Set to 0 after power on PERIPLL PD.
                                                                                              */
#define SYSON_BIT_PON_ISO_SYS1PLL                   ((u32)0x00000001 << 23)                  /*!< R/W   1  
                                                                                              * Enable ISO cell for data path from SYS1PLL. 
                                                                                              * + Set to 1 before power off SY1PLL PD.
                                                                                              * + Set to 0 after power on SYS1PLL PD."
                                                                                              */
#define SYSON_BIT_PON_ISO_SYS2PLL                   ((u32)0x00000001 << 22)                  /*!< R/W   1  
                                                                                              * Enable ISO cell for data path from SYS2PLL. 
                                                                                              * + Set to 1 before power off SYS2PLL PD.
                                                                                              * + Set to 0 after power on SYS2PLL PD."
                                                                                              */
#define SYSON_BIT_CPUPLL_CTRL_SEL                   ((u32)0x00000001 << 21)                  /*!< R/W   1  
                                                                                              * 1: CPUPLL control comes from PON register 0x018bit[10:5], 0x020[31:16],
                                                                                              * 0: CPUPLL control comes from hardware logic mux(as what we have done in TEST chip).
                                                                                              */
#define SYSON_BIT_PERIPLL_CTRL_SEL                  ((u32)0x00000001 << 20)                  /*!< R/W   1  
                                                                                              * 1: PERIPLL control comes from PON register 0x018bit[10:5], 0x020[31:16],
                                                                                              * 0: PERIPLL control comes from hardware logic mux(as what we have done in TEST chip).
                                                                                              */
#define SYSON_BIT_SYS1PLL_CTRL_SEL                  ((u32)0x00000001 << 19)                  /*!< R/W   1  
                                                                                              * 1: SYS1PLL control comes from PON register 0x008bit[10:5], 0x010[31:16],
                                                                                              * 0: SYSPLL control comes from hardware logic mux(as what we have done in TEST chip).
                                                                                              */
#define SYSON_BIT_SYS2PLL_CTRL_SEL                  ((u32)0x00000001 << 18)                  /*!< R/W   1  
                                                                                              * 1: SYS1PLL control comes from PON register 0x008bit[10:5], 0x010[31:16],
                                                                                              * 0: SYSPLL control comes from hardware logic mux(as what we have done in TEST chip).
                                                                                              */
#define SYSON_BIT_CPUPLL_GATE                       ((u32)0x00000001 << 17)                  /*!< R/W     0 0: CPUPLL gating */
#define SYSON_BIT_PERIPLL_GATE                      ((u32)0x00000001 << 16)                  /*!< R/W     0 0: PERIPLL gating */
#define SYSON_BIT_SYS1PLL_GATE                      ((u32)0x00000001 << 15)                  /*!< R/W     0 0: SYS1PLL gating */
#define SYSON_BIT_SYS2PLL_GATE                      ((u32)0x00000001 << 14)                  /*!< R/W     0 0: SYS2PLL gating */
#define SYSON_BIT_PON_SYSON_CLK_EN                  ((u32)0x00000001 << 13)                  /*!< R/W   0   1: enable SYSON register clock */
#define SYSON_BIT_PON_CBUS_SLOWPCLK_EN              ((u32)0x00000001 << 12)                  /*!< R/W   0   1: enable SLOW PCLK clock to core platform */
#define SYSON_BIT_PON_CBUS_SLOWPCLK_FEN             ((u32)0x00000001 << 11)                  /*!< R/W   0   1: Release reset to SLOW PCLK clock domain of core platform */
#define SYSON_BIT_PON_CBUS_GLB_FEN                  ((u32)0x00000001 << 10)                  /*!< R/W   0   1: Release glb reset to core platform */
#define SYSON_BIT_PON_CBUS_CLK_EN                   ((u32)0x00000001 << 9)                   /*!< R/W   0   1: Enable core platform clock */
#define SYSON_BIT_PON_CBUS_FEN                      ((u32)0x00000001 << 8)                   /*!< R/W   0   1: Release reset to core platform */
#define SYSON_BIT_CORE_LPC_EN                       ((u32)0x00000001 << 5)                   /*!< R/W   0  
                                                                                              * System on domain large power
                                                                                              * 0:Disable, 1: Enable
                                                                                              */
#define SYSON_BIT_CORE_SPC_EN                       ((u32)0x00000001 << 4)                   /*!< R/W   0  
                                                                                              * System on domain small power
                                                                                              * 0:Disable, 1: Enable
                                                                                              */
#define SYSON_BIT_MUCOFF_LPC_EN                     ((u32)0x00000001 << 1)                   /*!< R/W   0  
                                                                                              * System on domain large power
                                                                                              * 0:Disable, 1: Enable
                                                                                              */
#define SYSON_BIT_MCUOFF_SPC_EN                     ((u32)0x00000001 << 0)                   /*!< R/W   0  
                                                                                              * System on domain small power
                                                                                              * 0:Disable, 1: Enable
                                                                                              */
/** @} */


/** @defgroup REG_PON_DUMMY0
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY1
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY2
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY3
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_WIFI_PLATFORM_DBG
 * @brief
 * @{
 */
#define SYSON_MASK_WIFI_RXI300_DBG_STS              ((u32)0x007FFFFF << 0)                   /*!< R       0 Debug signal from wifi rxi300 */
#define SYSON_WIFI_RXI300_DBG_STS(x)                ((u32)(((x) & 0x007FFFFF) << 0))
#define SYSON_GET_WIFI_RXI300_DBG_STS(x)            ((u32)(((x >> 0) & 0x007FFFFF)))
/** @} */


/** @defgroup REG_PON_DUMMY5
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY6
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PKE_SEC_ATTR_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_PKE_KEY1_SEC_ATTR                 ((u32)0x00000001 << 1)                   /*!< R/W     0 None */
#define SYSON_BIT_PKE_KEY0_SEC_ATTR                 ((u32)0x00000001 << 0)                   /*!< R/W     0 None */
/** @} */


/** @defgroup REG_PON_SEC_KEY_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_FLASH_SEC_KEY_LOAD_TRIG           ((u32)0x00000001 << 1)                   /*!< R/W   0x0 SW to write 1 to trigger key load. HW will clear to 0 when the loading process is done */
#define SYSON_BIT_PKE_KEY_LOAD_TRIG                 ((u32)0x00000001 << 0)                   /*!< R/W   0x0 SW to write 1 to trigger key load. HW will clear to 0 when the loading process is done */
/** @} */


/** @defgroup REG_PON_DUMMY8
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY9
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_BT_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_BT_FUNC_SI_EN                     ((u32)0x00000001 << 20)                  /*!< R/W   0   1: Enable BT SI in Function mode */
#define SYSON_BIT_BT_TST_SI_EN                      ((u32)0x00000001 << 19)                  /*!< R/W   0   1: Enable BT SI in Testmode */
#define SYSON_BIT_BT_POWER_ON                       ((u32)0x00000001 << 18)                  /*!< R/W   0   1: to turn on WIFIOFF; and to indicate BT is power on */
#define SYSON_BIT_BT_PLL_READY                      ((u32)0x00000001 << 17)                  /*!< R     0   None */
#define SYSON_BIT_BT_USE_EXT_BT                     ((u32)0x00000001 << 16)                  /*!< R/W   0   0: Using internal BT; 1; using external BT */
#define SYSON_BIT_BTAFE_ISO_EN                      ((u32)0x00000001 << 15)                  /*!< R/W   1   0: Release isolation; 1: enable isolation */
#define SYSON_BIT_BTRFC_ISO_EN                      ((u32)0x00000001 << 14)                  /*!< R/W   0   NOT USE in AmebaPro3 */
#define SYSON_BIT_HOST_WAKE_BT                      ((u32)0x00000001 << 13)                  /*!< R/W     0 1: CPU Processors to wake up internal/external BT */
#define SYSON_BIT_HOST_WAKE_BT_POL                  ((u32)0x00000001 << 12)                  /*!< R/W     0
                                                                                              * 0: External to wake up internal BT
                                                                                              * 1: Internal CPU Processors to wake up internal/external BT
                                                                                              */
#define SYSON_BIT_BT_WAKE_HOST_POL                  ((u32)0x00000001 << 11)                  /*!< R/W     0
                                                                                              * 0: External BT to wake interal CPU Procesors.
                                                                                              * 1: Internal BT to wake internal/external CPU Processors.
                                                                                              */
#define SYSON_MASK_BIST_RPT_SEL                     ((u32)0x00000007 << 8)                   /*!< R/W     0 None */
#define SYSON_BIST_RPT_SEL(x)                       ((u32)(((x) & 0x00000007) << 8))
#define SYSON_GET_BIST_RPT_SEL(x)                   ((u32)(((x >> 8) & 0x00000007)))
#define SYSON_BIT_BT_HW_EN_BTAFE                    ((u32)0x00000001 << 7)                   /*!< R/W     0 1: Enable BTAFE */
#define SYSON_BIT_BT_USE_WL_RF                      ((u32)0x00000001 << 6)                   /*!< R/W     0 1: Enable BTS1 & WL share RF  */
#define SYSON_BIT_BT_HW_EN_RF                       ((u32)0x00000001 << 5)                   /*!< R/W     0 1: Enable BTS0 RF */
#define SYSON_BIT_BT_USE_EXT_UART                   ((u32)0x00000001 << 4)                   /*!< R/W     0 0: BT uses internal UART, 1:BT uses external UART */
#define SYSON_BIT_BT_MP_I2C_EN                      ((u32)0x00000001 << 3)                   /*!< R/W     0 1: Enable */
#define SYSON_BIT_BT_HW_EN33                        ((u32)0x00000001 << 2)                   /*!< R/W     0 1: Enable BT AFE 3.3V */
#define SYSON_BIT_FORCE_LOGUART_USE_LOGUART_PAD     ((u32)0x00000001 << 1)                   /*!< R/W   0   1: Enable BT to use LOGUART PAD */
#define SYSON_BIT_BT_USE_LOGUART_PAD_STS            ((u32)0x00000001 << 0)                   /*!< R     0   None */
/** @} */


/** @defgroup REG_UART_INTR_STS
 * @brief
 * @{
 */
#define SYSON_BIT_UART0_ISR                         ((u32)0x00000001 << 0)                   /*!< R       0 UART0 Interrupt Status */
/** @} */


/** @defgroup REG_LOGUART_CLK_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_LOGUART_SCLK_SEL                  ((u32)0x00000001 << 0)                   /*!< R/W   0x0 1: sclk; 0: xtal */
/** @} */


/** @defgroup REG_PON_GDMA_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_HS_SPI1_DMA_SEL                   ((u32)0x00000001 << 31)                  /*!< R/W   0  
                                                                                              * To select either SPI1 master or SPI1 slave to use DMA engine:
                                                                                              * 0: SPI1 master to use DMA engine
                                                                                              * 1: SPI1 slave to use DMA engine
                                                                                              */
#define SYSON_BIT_HS_SPI0_DMA_SEL                   ((u32)0x00000001 << 30)                  /*!< R/W     0
                                                                                              * To select either SPI0 master or SPI0 slave to use DMA engine:
                                                                                              * 0: SPI0 master to use DMA engine
                                                                                              * 1: SPI0 slave to use DMA engine
                                                                                              */
#define SYSON_MASK_GDMA0_SEL                        ((u32)0x3FFFFFFF << 0)                   /*!< R/W   0  
                                                                                              * Select which DMA engine (either DMA0 or DMA1) to serve a data transfer
                                                                                              * 0: DMA1 is used
                                                                                              * 1: DMA0 is used
                                                                                              * [0]: Selection for SPIC_TX transfer
                                                                                              * [1]: Selection for SPIC_RX transfer
                                                                                              * [2]: Selection for UART0_TX transfer
                                                                                              * [3]: Selection for UART0_RX transfer
                                                                                              * [4]: Selection for UART1_TX transfer
                                                                                              * [5]: Selection for UART1_RX transfer
                                                                                              * [6]: Selection for UART2_TX transfer
                                                                                              * [7]: Selection for UART2_RX transfer
                                                                                              * [8]: Selection for LOGUART_TX transfer
                                                                                              * [9]: Selection for LOGUART_RX transfer
                                                                                              * [10]: Selection for BTUART_TX transfer
                                                                                              * [11]: Selection for BTUART_RX transfer
                                                                                              * [12]: Selection for I2C0_TX transfer
                                                                                              * [13]: Selection for I2C0_RX transfer
                                                                                              * [14]: Selection for I2C1_TX transfer
                                                                                              * [15]: Selection for I2C1_RX transfer
                                                                                              * [16]: Selection for I2C2_TX transfer
                                                                                              * [17]: Selection for I2C2_RX transfer
                                                                                              * [18]: Selection for I2C3_TX transfer
                                                                                              * [19]: Selection for I2C3_RX transfer
                                                                                              * [20]: Selection for SPI0_TX transfer
                                                                                              * [21]: Selection for SPI0_RX transfer
                                                                                              * [22]: Selection for SPI1_TX transfer
                                                                                              * [23]: Selection for SPI1_RX transfer
                                                                                              * [24]: Selection for ADC_RX transfer
                                                                                              * [25]: Selection for SGPIO_TX transfer
                                                                                              * [26]: Selection for SPORT0_TX0 transfer
                                                                                              * [27]: Selection for SPORT0_RX0 transfer
                                                                                              * [28]: Selection for SPORT0_TX1 transfer
                                                                                              * [29]: Selection for SPORT0_RX1 transfer
                                                                                              */
#define SYSON_GDMA0_SEL(x)                          ((u32)(((x) & 0x3FFFFFFF) << 0))
#define SYSON_GET_GDMA0_SEL(x)                      ((u32)(((x >> 0) & 0x3FFFFFFF)))
/** @} */


/** @defgroup REG_PON_DUMMY10
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY11
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_BOOT_STATE
 * @brief
 * @{
 */
#define SYSON_MASK_BOOT_STATE                       ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0x0  For SW use */
#define SYSON_BOOT_STATE(x)                         ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_BOOT_STATE(x)                     ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_BOOT_STATE1
 * @brief
 * @{
 */
#define SYSON_MASK_UUID3                            ((u32)0x000000FF << 24)                  /*!< R/W Once 0xFF  For SW use */
#define SYSON_UUID3(x)                              ((u32)(((x) & 0x000000FF) << 24))
#define SYSON_GET_UUID3(x)                          ((u32)(((x >> 24) & 0x000000FF)))
#define SYSON_MASK_UUID2                            ((u32)0x000000FF << 16)                  /*!< R/W Once 0xFF  For SW use */
#define SYSON_UUID2(x)                              ((u32)(((x) & 0x000000FF) << 16))
#define SYSON_GET_UUID2(x)                          ((u32)(((x >> 16) & 0x000000FF)))
#define SYSON_MASK_UUID1                            ((u32)0x000000FF << 8)                   /*!< R/W Once 0xFF  For SW use */
#define SYSON_UUID1(x)                              ((u32)(((x) & 0x000000FF) << 8))
#define SYSON_GET_UUID1(x)                          ((u32)(((x >> 8) & 0x000000FF)))
#define SYSON_MASK_UUID0                            ((u32)0x000000FF << 0)                   /*!< R/W Once 0xFF  For SW use */
#define SYSON_UUID0(x)                              ((u32)(((x) & 0x000000FF) << 0))
#define SYSON_GET_UUID0(x)                          ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_PON_DUMMY12
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY13
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY14
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY15
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY16
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY17
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY18
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY19
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY20
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY21
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_MEM_WLAN_CTRL0
 * @brief
 * @{
 */
#define SYSON_BIT_TEST1_WPOFF_E1_SRAM               ((u32)0x00000001 << 25)                  /*!< R/W   0x0 None */
#define SYSON_MASK_RM_WPOFF_E1_SRAM                 ((u32)0x0000000F << 21)                  /*!< R/W   0x0 None */
#define SYSON_RM_WPOFF_E1_SRAM(x)                   ((u32)(((x) & 0x0000000F) << 21))
#define SYSON_GET_RM_WPOFF_E1_SRAM(x)               ((u32)(((x >> 21) & 0x0000000F)))
#define SYSON_BIT_RME_WPOFF_E1_SRAM                 ((u32)0x00000001 << 20)                  /*!< R/W   0x0 None */
#define SYSON_BIT_LS_WPOFF_E1_SRAM                  ((u32)0x00000001 << 19)                  /*!< R/W   0x0 None */
#define SYSON_BIT_TEST1_WPOFF_E0_SRAM               ((u32)0x00000001 << 18)                  /*!< R/W   0x0 None */
#define SYSON_MASK_RM_WPOFF_E0_SRAM                 ((u32)0x0000000F << 14)                  /*!< R/W   0x0 None */
#define SYSON_RM_WPOFF_E0_SRAM(x)                   ((u32)(((x) & 0x0000000F) << 14))
#define SYSON_GET_RM_WPOFF_E0_SRAM(x)               ((u32)(((x >> 14) & 0x0000000F)))
#define SYSON_BIT_RME_WPOFF_E0_SRAM                 ((u32)0x00000001 << 13)                  /*!< R/W   0x0 None */
#define SYSON_BIT_SD_WPOFF_E0_SRAM                  ((u32)0x00000001 << 12)                  /*!< R/W   0x0 None */
#define SYSON_BIT_LS_WPOFF_E0_SRAM                  ((u32)0x00000001 << 11)                  /*!< R/W   0x0 None */
#define SYSON_BIT_DS_WPOFF_E0_SRAM                  ((u32)0x00000001 << 10)                  /*!< R/W   0x0 None */
#define SYSON_BIT_TMCK_WLK4_E0_SRAM                 ((u32)0x00000001 << 9)                   /*!< R/W   0x0 None */
#define SYSON_BIT_DVSE_WLK4_E0_SRAM                 ((u32)0x00000001 << 8)                   /*!< R/W   0x0 None */
#define SYSON_MASK_DVS_WLK4_E0_SRAM                 ((u32)0x00000003 << 6)                   /*!< R/W   0x0 None */
#define SYSON_DVS_WLK4_E0_SRAM(x)                   ((u32)(((x) & 0x00000003) << 6))
#define SYSON_GET_DVS_WLK4_E0_SRAM(x)               ((u32)(((x >> 6) & 0x00000003)))
#define SYSON_BIT_RAE_WLK4_E0_SRAM                  ((u32)0x00000001 << 5)                   /*!< R/W   0x0 None */
#define SYSON_MASK_WAEM_WLK4_E0_SRAM                ((u32)0x0000000F << 1)                   /*!< R/W   0x0 None */
#define SYSON_WAEM_WLK4_E0_SRAM(x)                  ((u32)(((x) & 0x0000000F) << 1))
#define SYSON_GET_WAEM_WLK4_E0_SRAM(x)              ((u32)(((x >> 1) & 0x0000000F)))
#define SYSON_BIT_WAE_WLK4_E0_SRAM                  ((u32)0x00000001 << 0)                   /*!< R/W   0x0 None */
/** @} */


/** @defgroup REG_MEM_WLAN_CTRL1
 * @brief
 * @{
 */
#define SYSON_BIT_TEST1_WPOFF_E4_SRAM               ((u32)0x00000001 << 27)                  /*!< R/W   0x0 None */
#define SYSON_MASK_RM_WPOFF_E4_SRAM                 ((u32)0x0000000F << 23)                  /*!< R/W   0x0 None */
#define SYSON_RM_WPOFF_E4_SRAM(x)                   ((u32)(((x) & 0x0000000F) << 23))
#define SYSON_GET_RM_WPOFF_E4_SRAM(x)               ((u32)(((x >> 23) & 0x0000000F)))
#define SYSON_BIT_RME_WPOFF_E4_SRAM                 ((u32)0x00000001 << 22)                  /*!< R/W   0x0 None */
#define SYSON_BIT_LS_WPOFF_E4_SRAM                  ((u32)0x00000001 << 21)                  /*!< R/W   0x0 None */
#define SYSON_BIT_TEST1_WPOFF_E3_SRAM               ((u32)0x00000001 << 20)                  /*!< R/W   0x0 None */
#define SYSON_MASK_RM_WPOFF_E3_SRAM                 ((u32)0x0000000F << 16)                  /*!< R/W   0x0 None */
#define SYSON_RM_WPOFF_E3_SRAM(x)                   ((u32)(((x) & 0x0000000F) << 16))
#define SYSON_GET_RM_WPOFF_E3_SRAM(x)               ((u32)(((x >> 16) & 0x0000000F)))
#define SYSON_BIT_RME_WPOFF_E3_SRAM                 ((u32)0x00000001 << 15)                  /*!< R/W   0x0 None */
#define SYSON_BIT_LS_WPOFF_E3_SRAM                  ((u32)0x00000001 << 14)                  /*!< R/W   0x0 None */
#define SYSON_BIT_LS_0_WPOFF_E2_SRAM                ((u32)0x00000001 << 13)                  /*!< R/W   0x0 None */
#define SYSON_BIT_LS_1_WPOFF_E2_SRAM                ((u32)0x00000001 << 12)                  /*!< R/W   0x0 None */
#define SYSON_BIT_TEST1B_WPOFF_E2_SRAM              ((u32)0x00000001 << 11)                  /*!< R/W   0x0 None */
#define SYSON_BIT_TEST1A_WPOFF_E2_SRAM              ((u32)0x00000001 << 10)                  /*!< R/W   0x0 None */
#define SYSON_BIT_RMEB_WPOFF_E2_SRAM                ((u32)0x00000001 << 9)                   /*!< R/W   0x0 None */
#define SYSON_MASK_RMB_WPOFF_E2_SRAM                ((u32)0x0000000F << 5)                   /*!< R/W   0x0 None */
#define SYSON_RMB_WPOFF_E2_SRAM(x)                  ((u32)(((x) & 0x0000000F) << 5))
#define SYSON_GET_RMB_WPOFF_E2_SRAM(x)              ((u32)(((x >> 5) & 0x0000000F)))
#define SYSON_BIT_RMEA_WPOFF_E2_SRAM                ((u32)0x00000001 << 4)                   /*!< R/W   0x0 None */
#define SYSON_MASK_RMA_WPOFF_E2_SRAM                ((u32)0x0000000F << 0)                   /*!< R/W   0x0 None */
#define SYSON_RMA_WPOFF_E2_SRAM(x)                  ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_RMA_WPOFF_E2_SRAM(x)              ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_MEM_WLAN_CTRL2
 * @brief
 * @{
 */
#define SYSON_BIT_TMCK_WPON_E0_SRAM                 ((u32)0x00000001 << 27)                  /*!< R/W   0x0 None */
#define SYSON_BIT_DVSE_WPON_E0_SRAM                 ((u32)0x00000001 << 26)                  /*!< R/W   0x0 None */
#define SYSON_MASK_DVS_WPON_E0_SRAM                 ((u32)0x00000003 << 24)                  /*!< R/W   0x0 None */
#define SYSON_DVS_WPON_E0_SRAM(x)                   ((u32)(((x) & 0x00000003) << 24))
#define SYSON_GET_DVS_WPON_E0_SRAM(x)               ((u32)(((x >> 24) & 0x00000003)))
#define SYSON_BIT_RAE_WPON_E0_SRAM                  ((u32)0x00000001 << 23)                  /*!< R/W   0x0 None */
#define SYSON_MASK_WAEM_WPON_E0_SRAM                ((u32)0x0000000F << 19)                  /*!< R/W   0x0 None */
#define SYSON_WAEM_WPON_E0_SRAM(x)                  ((u32)(((x) & 0x0000000F) << 19))
#define SYSON_GET_WAEM_WPON_E0_SRAM(x)              ((u32)(((x >> 19) & 0x0000000F)))
#define SYSON_BIT_WAE_WPON_E0_SRAM                  ((u32)0x00000001 << 18)                  /*!< R/W   0x0 None */
#define SYSON_BIT_LS_0_WPOFF_E5_SRAM                ((u32)0x00000001 << 17)                  /*!< R/W   0x0 None */
#define SYSON_BIT_DS_0_WPOFF_E5_SRAM                ((u32)0x00000001 << 16)                  /*!< R/W   0x0 None */
#define SYSON_BIT_SD_0_WPOFF_E5_SRAM                ((u32)0x00000001 << 15)                  /*!< R/W   0x0 None */
#define SYSON_BIT_LS_1_WPOFF_E5_SRAM                ((u32)0x00000001 << 14)                  /*!< R/W   0x0 None */
#define SYSON_BIT_DS_1_WPOFF_E5_SRAM                ((u32)0x00000001 << 13)                  /*!< R/W   0x0 None */
#define SYSON_BIT_SD_1_WPOFF_E5_SRAM                ((u32)0x00000001 << 12)                  /*!< R/W   0x0 None */
#define SYSON_BIT_TEST1B_WPOFF_E5_SRAM              ((u32)0x00000001 << 11)                  /*!< R/W   0x0 None */
#define SYSON_BIT_TEST1A_WPOFF_E5_SRAM              ((u32)0x00000001 << 10)                  /*!< R/W   0x0 None */
#define SYSON_BIT_RMEB_WPOFF_E5_SRAM                ((u32)0x00000001 << 9)                   /*!< R/W   0x0 None */
#define SYSON_MASK_RMB_WPOFF_E5_SRAM                ((u32)0x0000000F << 5)                   /*!< R/W   0x0 None */
#define SYSON_RMB_WPOFF_E5_SRAM(x)                  ((u32)(((x) & 0x0000000F) << 5))
#define SYSON_GET_RMB_WPOFF_E5_SRAM(x)              ((u32)(((x >> 5) & 0x0000000F)))
#define SYSON_BIT_RMEA_WPOFF_E5_SRAM                ((u32)0x00000001 << 4)                   /*!< R/W   0x0 None */
#define SYSON_MASK_RMA_WPOFF_E5_SRAM                ((u32)0x0000000F << 0)                   /*!< R/W   0x0 None */
#define SYSON_RMA_WPOFF_E5_SRAM(x)                  ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_RMA_WPOFF_E5_SRAM(x)              ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_MEM_BT_CNTRL
 * @brief
 * @{
 */
#define SYSON_BIT_TMCK_BT_ROM                       ((u32)0x00000001 << 11)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * BT_ROM
                                                                                              */
#define SYSON_BIT_DVSE_BT_ROM                       ((u32)0x00000001 << 10)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * BT_ROM
                                                                                              */
#define SYSON_MASK_DVS_BT_ROM                       ((u32)0x0000000F << 6)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * BT_ROM
                                                                                              */
#define SYSON_DVS_BT_ROM(x)                         ((u32)(((x) & 0x0000000F) << 6))
#define SYSON_GET_DVS_BT_ROM(x)                     ((u32)(((x >> 6) & 0x0000000F)))
#define SYSON_BIT_TMCK_BT_RAM                       ((u32)0x00000001 << 5)                   /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * BT_RAM
                                                                                              */
#define SYSON_BIT_DVSE_BT_RAM                       ((u32)0x00000001 << 4)                   /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * BT_RAM
                                                                                              */
#define SYSON_MASK_DVS_BT_RAM                       ((u32)0x0000000F << 0)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * BT_RAM
                                                                                              */
#define SYSON_DVS_BT_RAM(x)                         ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_DVS_BT_RAM(x)                     ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_MEM_GMAC_CNTRL
 * @brief
 * @{
 */
#define SYSON_BIT_NAP_GMAC_SRAM1                    ((u32)0x00000001 << 13)                  /*!< R/W   0x0
                                                                                              * nap: light sleep
                                                                                              * gmac_sram1
                                                                                              */
#define SYSON_BIT_TMCK_GMAC_SRAM1                   ((u32)0x00000001 << 12)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * gmac_sram1
                                                                                              */
#define SYSON_BIT_DVSE_GMAC_SRAM1                   ((u32)0x00000001 << 11)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * gmac_sram1
                                                                                              */
#define SYSON_MASK_DVS_GMAC_SRAM1                   ((u32)0x0000000F << 7)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * gmac_sram1
                                                                                              */
#define SYSON_DVS_GMAC_SRAM1(x)                     ((u32)(((x) & 0x0000000F) << 7))
#define SYSON_GET_DVS_GMAC_SRAM1(x)                 ((u32)(((x >> 7) & 0x0000000F)))
#define SYSON_BIT_NAP_GMAC_SRAM0                    ((u32)0x00000001 << 6)                   /*!< R/W   0x0
                                                                                              * nap: light sleep
                                                                                              * gmac_sram0
                                                                                              */
#define SYSON_BIT_TMCK_GMAC_SRAM0                   ((u32)0x00000001 << 5)                   /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * gmac_sram0
                                                                                              */
#define SYSON_BIT_DVSE_GMAC_SRAM0                   ((u32)0x00000001 << 4)                   /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * gmac_sram0
                                                                                              */
#define SYSON_MASK_DVS_GMAC_SRAM0                   ((u32)0x0000000F << 0)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * gmac_sram0
                                                                                              */
#define SYSON_DVS_GMAC_SRAM0(x)                     ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_DVS_GMAC_SRAM0(x)                 ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_MEM_PON_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_TMCK_PKE_MEM                      ((u32)0x00000001 << 11)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * cpu_dtcm_ram
                                                                                              */
#define SYSON_BIT_DVSE_PKE_MEM                      ((u32)0x00000001 << 10)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * cpu_dtcm_ram
                                                                                              */
#define SYSON_MASK_DVS_PKE_MEM                      ((u32)0x0000000F << 6)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * cpu_dtcm_ram
                                                                                              */
#define SYSON_DVS_PKE_MEM(x)                        ((u32)(((x) & 0x0000000F) << 6))
#define SYSON_GET_DVS_PKE_MEM(x)                    ((u32)(((x >> 6) & 0x0000000F)))
#define SYSON_BIT_TMCK_BTSHARE_MEM                  ((u32)0x00000001 << 5)                   /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * BTSHARE_ram
                                                                                              */
#define SYSON_BIT_DVSE_BTSHARE_MEM                  ((u32)0x00000001 << 4)                   /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * BTSHARE_ram
                                                                                              */
#define SYSON_MASK_DVS_BTSHARE_MEM                  ((u32)0x0000000F << 0)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * BTSHARE_ram
                                                                                              */
#define SYSON_DVS_BTSHARE_MEM(x)                    ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_DVS_BTSHARE_MEM(x)                ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_MEM_BTSHARE_CTRL
 * @brief
 * @{
 */
#define SYSON_MASK_NAP_BTSHARE_MEM                  ((u32)0x000000FF << 16)                  /*!< R/W   0x0 None */
#define SYSON_NAP_BTSHARE_MEM(x)                    ((u32)(((x) & 0x000000FF) << 16))
#define SYSON_GET_NAP_BTSHARE_MEM(x)                ((u32)(((x >> 16) & 0x000000FF)))
#define SYSON_MASK_SLP_BTSHARE_MEM                  ((u32)0x000000FF << 8)                   /*!< R/W   0x0 None */
#define SYSON_SLP_BTSHARE_MEM(x)                    ((u32)(((x) & 0x000000FF) << 8))
#define SYSON_GET_SLP_BTSHARE_MEM(x)                ((u32)(((x >> 8) & 0x000000FF)))
#define SYSON_MASK_RET_BTSHARE_MEM                  ((u32)0x000000FF << 0)                   /*!< R/W   0x0 None */
#define SYSON_RET_BTSHARE_MEM(x)                    ((u32)(((x) & 0x000000FF) << 0))
#define SYSON_GET_RET_BTSHARE_MEM(x)                ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_PON_DUMMY23
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_MEM_COMM_CNTRL
 * @brief
 * @{
 */
#define SYSON_BIT_NAP_USBOTG                        ((u32)0x00000001 << 27)                  /*!< R/W   0x0
                                                                                              * nap: light sleep
                                                                                              * usbotg
                                                                                              */
#define SYSON_BIT_TMCK_USBOTG                       ((u32)0x00000001 << 26)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * usbotg
                                                                                              */
#define SYSON_BIT_DVSE_USBOTG                       ((u32)0x00000001 << 25)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * usbotg
                                                                                              */
#define SYSON_MASK_DVS_USBOTG                       ((u32)0x0000000F << 21)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * usbotg
                                                                                              */
#define SYSON_DVS_USBOTG(x)                         ((u32)(((x) & 0x0000000F) << 21))
#define SYSON_GET_DVS_USBOTG(x)                     ((u32)(((x >> 21) & 0x0000000F)))
#define SYSON_BIT_NAP_SDMMC_PPBECC                  ((u32)0x00000001 << 20)                  /*!< R/W   0x0
                                                                                              * nap: light sleep
                                                                                              * sdmmc_ppbecc
                                                                                              */
#define SYSON_BIT_TMCK_SDMMC_PPBECC                 ((u32)0x00000001 << 19)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * sdmmc_ppbecc
                                                                                              */
#define SYSON_BIT_DVSE_SDMMC_PPBECC                 ((u32)0x00000001 << 18)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * sdmmc_ppbecc
                                                                                              */
#define SYSON_MASK_DVS_SDMMC_PPBECC                 ((u32)0x0000000F << 14)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * sdmmc_ppbecc
                                                                                              */
#define SYSON_DVS_SDMMC_PPBECC(x)                   ((u32)(((x) & 0x0000000F) << 14))
#define SYSON_GET_DVS_SDMMC_PPBECC(x)               ((u32)(((x >> 14) & 0x0000000F)))
#define SYSON_BIT_NAP_SDMMC_DMASB0                  ((u32)0x00000001 << 13)                  /*!< R/W   0x0
                                                                                              * nap: light sleep
                                                                                              * sdmmc_dmasb1
                                                                                              */
#define SYSON_BIT_TMCK_SDMMC_DMASB1                 ((u32)0x00000001 << 12)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * sdmmc_ppbecc
                                                                                              */
#define SYSON_BIT_DVSE_SDMMC_DMASB1                 ((u32)0x00000001 << 11)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * sdmmc_dmasb1
                                                                                              */
#define SYSON_MASK_DVS_SDMMC_DMASB1                 ((u32)0x0000000F << 7)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * sdmmc_dmasb1
                                                                                              */
#define SYSON_DVS_SDMMC_DMASB1(x)                   ((u32)(((x) & 0x0000000F) << 7))
#define SYSON_GET_DVS_SDMMC_DMASB1(x)               ((u32)(((x >> 7) & 0x0000000F)))
#define SYSON_BIT_NAP_SDIO                          ((u32)0x00000001 << 6)                   /*!< R/W   0x0
                                                                                              * nap: light sleep
                                                                                              * sdio
                                                                                              */
#define SYSON_BIT_TMCK_SDIO                         ((u32)0x00000001 << 5)                   /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * sdio
                                                                                              */
#define SYSON_BIT_DVSE_SDIO                         ((u32)0x00000001 << 4)                   /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * sdio
                                                                                              */
#define SYSON_MASK_DVS_SDIO                         ((u32)0x0000000F << 0)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * sdio
                                                                                              */
#define SYSON_DVS_SDIO(x)                           ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_DVS_SDIO(x)                       ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_MEM_CPU_CNTRL
 * @brief
 * @{
 */
#define SYSON_BIT_TMCK_KM4NP_MEM                    ((u32)0x00000001 << 13)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * cpu_dtcm_ram
                                                                                              */
#define SYSON_BIT_DVSE_KM4NP_MEM                    ((u32)0x00000001 << 12)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * cpu_dtcm_ram
                                                                                              */
#define SYSON_MASK_DVS_KM4NP_MEM                    ((u32)0x0000000F << 8)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * cpu_dtcm_ram
                                                                                              */
#define SYSON_DVS_KM4NP_MEM(x)                      ((u32)(((x) & 0x0000000F) << 8))
#define SYSON_GET_DVS_KM4NP_MEM(x)                  ((u32)(((x >> 8) & 0x0000000F)))
#define SYSON_BIT_TMCK_KM0FW_MEM                    ((u32)0x00000001 << 5)                   /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * cpu_dtcm_ram
                                                                                              */
#define SYSON_BIT_DVSE_KM0FW_MEM                    ((u32)0x00000001 << 4)                   /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * cpu_dtcm_ram
                                                                                              */
#define SYSON_MASK_DVS_KM0FW_MEM                    ((u32)0x0000000F << 0)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * cpu_dtcm_ram
                                                                                              */
#define SYSON_DVS_KM0FW_MEM(x)                      ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_DVS_KM0FW_MEM(x)                  ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_MEM_SYSTEM_CTRL0
 * @brief
 * @{
 */
#define SYSON_MASK_RET_SYSTEM_RAM                   ((u32)0x000000FF << 16)                  /*!< R/W   0x0
                                                                                              * 1: To enabe retention mode for SYS MEM on SLP event
                                                                                              * 0: No action
                                                                                              * [0] : The controlled bit for 1st 64KB SRAM region.
                                                                                              * [1] : The controlled bit for 2nd 64KB SRAM region.
                                                                                              * [2] : The controlled bit for 3rd 64KB SRAM region.
                                                                                              * [3] : The controlled bit for 4th 64KB SRAM region.
                                                                                              * [4] : The controlled bit for 5th 64KB SRAM region.
                                                                                              * [5] : The controlled bit for 6th 64KB SRAM region.
                                                                                              * [6] : The controlled bit for 7th 64KB SRAM region.
                                                                                              * [7] : The controlled bit for 8th 64KB SRAM region.
                                                                                              */
#define SYSON_RET_SYSTEM_RAM(x)                     ((u32)(((x) & 0x000000FF) << 16))
#define SYSON_GET_RET_SYSTEM_RAM(x)                 ((u32)(((x >> 16) & 0x000000FF)))
#define SYSON_MASK_NAP_SYSTEM_RAM                   ((u32)0x000000FF << 8)                   /*!< R/W   0x0 None */
#define SYSON_NAP_SYSTEM_RAM(x)                     ((u32)(((x) & 0x000000FF) << 8))
#define SYSON_GET_NAP_SYSTEM_RAM(x)                 ((u32)(((x >> 8) & 0x000000FF)))
#define SYSON_MASK_SLP_SYSTEM_RAM                   ((u32)0x000000FF << 0)                   /*!< R/W   0x0
                                                                                              * 1: To shut down SYS MEM on SLP event
                                                                                              * 0: No action
                                                                                              * [0] : The controlled bit for 1st 64KB SRAM region.
                                                                                              * [1] : The controlled bit for 2nd 64KB SRAM region.
                                                                                              * [2] : The controlled bit for 3rd 64KB SRAM region.
                                                                                              * [3] : The controlled bit for 4th 64KB SRAM region.
                                                                                              * [4] : The controlled bit for 5th 64KB SRAM region.
                                                                                              * [5] : The controlled bit for 6th 64KB SRAM region.
                                                                                              * [6] : The controlled bit for 7th 64KB SRAM region.
                                                                                              * [7] : The controlled bit for 8th 64KB SRAM region.
                                                                                              */
#define SYSON_SLP_SYSTEM_RAM(x)                     ((u32)(((x) & 0x000000FF) << 0))
#define SYSON_GET_SLP_SYSTEM_RAM(x)                 ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_MEM_SYSTEM_CTRL1
 * @brief
 * @{
 */
#define SYSON_BIT_TMCK_SYSTEM_SRAM4                 ((u32)0x00000001 << 29)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM4
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM4                 ((u32)0x00000001 << 28)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM4
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM4                 ((u32)0x0000000F << 24)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM4
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM4(x)                   ((u32)(((x) & 0x0000000F) << 24))
#define SYSON_GET_DVS_SYSTEM_SRAM4(x)               ((u32)(((x >> 24) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM3                 ((u32)0x00000001 << 23)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM3
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM3                 ((u32)0x00000001 << 22)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM3
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM3                 ((u32)0x0000000F << 18)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM3
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM3(x)                   ((u32)(((x) & 0x0000000F) << 18))
#define SYSON_GET_DVS_SYSTEM_SRAM3(x)               ((u32)(((x >> 18) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM2                 ((u32)0x00000001 << 17)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM2
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM2                 ((u32)0x00000001 << 16)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM2
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM2                 ((u32)0x0000000F << 12)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM2
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM2(x)                   ((u32)(((x) & 0x0000000F) << 12))
#define SYSON_GET_DVS_SYSTEM_SRAM2(x)               ((u32)(((x >> 12) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM1                 ((u32)0x00000001 << 11)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM1
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM1                 ((u32)0x00000001 << 10)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM1
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM1                 ((u32)0x0000000F << 6)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM1
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM1(x)                   ((u32)(((x) & 0x0000000F) << 6))
#define SYSON_GET_DVS_SYSTEM_SRAM1(x)               ((u32)(((x >> 6) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM0                 ((u32)0x00000001 << 5)                   /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM0
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM0                 ((u32)0x00000001 << 4)                   /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM0
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM0                 ((u32)0x0000000F << 0)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM0
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM0(x)                   ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_DVS_SYSTEM_SRAM0(x)               ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_MEM_SYSTEM_CTRL2
 * @brief
 * @{
 */
#define SYSON_BIT_TMCK_SYSTEM_SRAM9                 ((u32)0x00000001 << 29)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM9
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM9                 ((u32)0x00000001 << 28)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM9
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM9                 ((u32)0x0000000F << 24)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM9
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM9(x)                   ((u32)(((x) & 0x0000000F) << 24))
#define SYSON_GET_DVS_SYSTEM_SRAM9(x)               ((u32)(((x >> 24) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM8                 ((u32)0x00000001 << 23)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM8
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM8                 ((u32)0x00000001 << 22)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM8
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM8                 ((u32)0x0000000F << 18)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM8
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM8(x)                   ((u32)(((x) & 0x0000000F) << 18))
#define SYSON_GET_DVS_SYSTEM_SRAM8(x)               ((u32)(((x >> 18) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM12                ((u32)0x00000001 << 17)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM12
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM12                ((u32)0x00000001 << 16)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM12
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM12                ((u32)0x0000000F << 12)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM12
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM12(x)                  ((u32)(((x) & 0x0000000F) << 12))
#define SYSON_GET_DVS_SYSTEM_SRAM12(x)              ((u32)(((x >> 12) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM6                 ((u32)0x00000001 << 11)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM6
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM6                 ((u32)0x00000001 << 10)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM6
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM6                 ((u32)0x0000000F << 6)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM6
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM6(x)                   ((u32)(((x) & 0x0000000F) << 6))
#define SYSON_GET_DVS_SYSTEM_SRAM6(x)               ((u32)(((x >> 6) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM5                 ((u32)0x00000001 << 5)                   /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM5
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM5                 ((u32)0x00000001 << 4)                   /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM5
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM5                 ((u32)0x0000000F << 0)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM5
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM5(x)                   ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_DVS_SYSTEM_SRAM5(x)               ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_MEM_SYSTEM_CTRL3
 * @brief
 * @{
 */
#define SYSON_BIT_TMCK_SYSTEM_SRAM14                ((u32)0x00000001 << 29)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM14
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM14                ((u32)0x00000001 << 28)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM14
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM14                ((u32)0x0000000F << 24)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM14
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM14(x)                  ((u32)(((x) & 0x0000000F) << 24))
#define SYSON_GET_DVS_SYSTEM_SRAM14(x)              ((u32)(((x >> 24) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM13                ((u32)0x00000001 << 23)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM13
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM13                ((u32)0x00000001 << 22)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM13
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM13                ((u32)0x0000000F << 18)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM13
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM13(x)                  ((u32)(((x) & 0x0000000F) << 18))
#define SYSON_GET_DVS_SYSTEM_SRAM13(x)              ((u32)(((x >> 18) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM12_0AC            ((u32)0x00000001 << 17)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM12
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM12_0AC            ((u32)0x00000001 << 16)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM12
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM12_0AC            ((u32)0x0000000F << 12)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM12
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM12_0AC(x)              ((u32)(((x) & 0x0000000F) << 12))
#define SYSON_GET_DVS_SYSTEM_SRAM12_0AC(x)          ((u32)(((x >> 12) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM11                ((u32)0x00000001 << 11)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM11
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM11                ((u32)0x00000001 << 10)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM11
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM11                ((u32)0x0000000F << 6)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM11
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM11(x)                  ((u32)(((x) & 0x0000000F) << 6))
#define SYSON_GET_DVS_SYSTEM_SRAM11(x)              ((u32)(((x >> 6) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM10                ((u32)0x00000001 << 5)                   /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM10
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM10                ((u32)0x00000001 << 4)                   /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM10
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM10                ((u32)0x0000000F << 0)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM10
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM10(x)                  ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_DVS_SYSTEM_SRAM10(x)              ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_MEM_SYSTEM_CTRL4
 * @brief
 * @{
 */
#define SYSON_BIT_TMCK_SYSTEM_SRAM19                ((u32)0x00000001 << 29)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM19
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM19                ((u32)0x00000001 << 28)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM19
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM19                ((u32)0x0000000F << 24)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM19
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM19(x)                  ((u32)(((x) & 0x0000000F) << 24))
#define SYSON_GET_DVS_SYSTEM_SRAM19(x)              ((u32)(((x >> 24) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM18                ((u32)0x00000001 << 23)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM18
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM18                ((u32)0x00000001 << 22)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM18
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM18                ((u32)0x0000000F << 18)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM18
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM18(x)                  ((u32)(((x) & 0x0000000F) << 18))
#define SYSON_GET_DVS_SYSTEM_SRAM18(x)              ((u32)(((x >> 18) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM17                ((u32)0x00000001 << 17)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM17
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM17                ((u32)0x00000001 << 16)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM17
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM17                ((u32)0x0000000F << 12)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM17
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM17(x)                  ((u32)(((x) & 0x0000000F) << 12))
#define SYSON_GET_DVS_SYSTEM_SRAM17(x)              ((u32)(((x >> 12) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM16                ((u32)0x00000001 << 11)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM16
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM16                ((u32)0x00000001 << 10)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM16
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM16                ((u32)0x0000000F << 6)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM16
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM16(x)                  ((u32)(((x) & 0x0000000F) << 6))
#define SYSON_GET_DVS_SYSTEM_SRAM16(x)              ((u32)(((x >> 6) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM15                ((u32)0x00000001 << 5)                   /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM15
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM15                ((u32)0x00000001 << 4)                   /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM15
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM15                ((u32)0x0000000F << 0)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM15
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM15(x)                  ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_DVS_SYSTEM_SRAM15(x)              ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_MEM_SYSTEM_CTRL5
 * @brief
 * @{
 */
#define SYSON_BIT_TMCK_SYSTEM_SRAM24                ((u32)0x00000001 << 29)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM24
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM24                ((u32)0x00000001 << 28)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM24
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM24                ((u32)0x0000000F << 24)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM24
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM24(x)                  ((u32)(((x) & 0x0000000F) << 24))
#define SYSON_GET_DVS_SYSTEM_SRAM24(x)              ((u32)(((x >> 24) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM23                ((u32)0x00000001 << 23)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM23
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM23                ((u32)0x00000001 << 22)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM23
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM23                ((u32)0x0000000F << 18)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM23
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM23(x)                  ((u32)(((x) & 0x0000000F) << 18))
#define SYSON_GET_DVS_SYSTEM_SRAM23(x)              ((u32)(((x >> 18) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM22                ((u32)0x00000001 << 17)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM22
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM22                ((u32)0x00000001 << 16)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM22
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM22                ((u32)0x0000000F << 12)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM22
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM22(x)                  ((u32)(((x) & 0x0000000F) << 12))
#define SYSON_GET_DVS_SYSTEM_SRAM22(x)              ((u32)(((x >> 12) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM21                ((u32)0x00000001 << 11)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM21
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM21                ((u32)0x00000001 << 10)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM21
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM21                ((u32)0x0000000F << 6)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM21
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM21(x)                  ((u32)(((x) & 0x0000000F) << 6))
#define SYSON_GET_DVS_SYSTEM_SRAM21(x)              ((u32)(((x >> 6) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM20                ((u32)0x00000001 << 5)                   /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM20
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM20                ((u32)0x00000001 << 4)                   /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM20
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM20                ((u32)0x0000000F << 0)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM20
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM20(x)                  ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_DVS_SYSTEM_SRAM20(x)              ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_MEM_SYSTEM_CTRL6
 * @brief
 * @{
 */
#define SYSON_BIT_TMCK_SYSTEM_SRAM29                ((u32)0x00000001 << 29)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM29
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM29                ((u32)0x00000001 << 28)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM29
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM29                ((u32)0x0000000F << 24)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM29
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM29(x)                  ((u32)(((x) & 0x0000000F) << 24))
#define SYSON_GET_DVS_SYSTEM_SRAM29(x)              ((u32)(((x >> 24) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM28                ((u32)0x00000001 << 23)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM28
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM28                ((u32)0x00000001 << 22)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM28
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM28                ((u32)0x0000000F << 18)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM28
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM28(x)                  ((u32)(((x) & 0x0000000F) << 18))
#define SYSON_GET_DVS_SYSTEM_SRAM28(x)              ((u32)(((x >> 18) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM27                ((u32)0x00000001 << 17)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM27
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM27                ((u32)0x00000001 << 16)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM27
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM27                ((u32)0x0000000F << 12)                  /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM27
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM27(x)                  ((u32)(((x) & 0x0000000F) << 12))
#define SYSON_GET_DVS_SYSTEM_SRAM27(x)              ((u32)(((x >> 12) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM26                ((u32)0x00000001 << 11)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM26
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM26                ((u32)0x00000001 << 10)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM26
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM26                ((u32)0x0000000F << 6)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM26
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM26(x)                  ((u32)(((x) & 0x0000000F) << 6))
#define SYSON_GET_DVS_SYSTEM_SRAM26(x)              ((u32)(((x >> 6) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM25                ((u32)0x00000001 << 5)                   /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM25
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM25                ((u32)0x00000001 << 4)                   /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM25
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM25                ((u32)0x0000000F << 0)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM25
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM25(x)                  ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_DVS_SYSTEM_SRAM25(x)              ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_MEM_SYSTEM_CTRL7
 * @brief
 * @{
 */
#define SYSON_BIT_TMCK_SYSTEM_SRAM31                ((u32)0x00000001 << 11)                  /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM31
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM31                ((u32)0x00000001 << 10)                  /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM31
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM31                ((u32)0x0000000F << 6)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM31
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM31(x)                  ((u32)(((x) & 0x0000000F) << 6))
#define SYSON_GET_DVS_SYSTEM_SRAM31(x)              ((u32)(((x >> 6) & 0x0000000F)))
#define SYSON_BIT_TMCK_SYSTEM_SRAM30                ((u32)0x00000001 << 5)                   /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * SYSTEM_SRAM30
                                                                                              */
#define SYSON_BIT_DVSE_SYSTEM_SRAM30                ((u32)0x00000001 << 4)                   /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * SYSTEM_SRAM30
                                                                                              */
#define SYSON_MASK_DVS_SYSTEM_SRAM30                ((u32)0x0000000F << 0)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * SYSTEM_SRAM30
                                                                                              */
#define SYSON_DVS_SYSTEM_SRAM30(x)                  ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_DVS_SYSTEM_SRAM30(x)              ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_MEM_NANDECC
 * @brief
 * @{
 */
#define SYSON_BIT_TMCK_NANDECC_SRAM                 ((u32)0x00000001 << 5)                   /*!< R/W   0x0
                                                                                              * internal timing bypass
                                                                                              * NANDECC_SRAM
                                                                                              */
#define SYSON_BIT_DVSE_NANDECC_SRAM                 ((u32)0x00000001 << 4)                   /*!< R/W   0x0
                                                                                              * delay option enable
                                                                                              * NANDECC_SRAM
                                                                                              */
#define SYSON_MASK_DVS_NANDECC_SRAM                 ((u32)0x0000000F << 0)                   /*!< R/W   0x0
                                                                                              * delay option
                                                                                              * NANDECC_SRAM
                                                                                              */
#define SYSON_DVS_NANDECC_SRAM(x)                   ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_DVS_NANDECC_SRAM(x)               ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PON_TRNG_CLK_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_TRNG_CLK_VLD                      ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define SYSON_BIT_TRNG_CLK_DIV_DONE                 ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define SYSON_BIT_TRNG_CLK_EN                       ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock enable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define SYSON_MASK_TRNG_CLK_SEL                     ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: XTAL_40M
                                                                                              * 0x1: OSC_4M
                                                                                              * 0x2: OSC_131K
                                                                                              * Others: 1'b0
                                                                                              */
#define SYSON_TRNG_CLK_SEL(x)                       ((u32)(((x) & 0x00000007) << 16))
#define SYSON_GET_TRNG_CLK_SEL(x)                   ((u32)(((x >> 16) & 0x00000007)))
/** @} */


/** @defgroup REG_PON_SEC_SCLK_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_SEC_SCLK_VLD                      ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define SYSON_BIT_SEC_SCLK_DIV_DONE                 ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define SYSON_BIT_SEC_SCLK_EN                       ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock enable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define SYSON_MASK_SEC_SCLK_SEL                     ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1_PLL
                                                                                              * 0x1: SYS2_PLL
                                                                                              * 0x2: PERI_PLL
                                                                                              * 0x3: DDR_PLL
                                                                                              * 0x4: USB2_PLL
                                                                                              * 0x5: XTAL_40M
                                                                                              * Others: 1'b0
                                                                                              */
#define SYSON_SEC_SCLK_SEL(x)                       ((u32)(((x) & 0x00000007) << 16))
#define SYSON_GET_SEC_SCLK_SEL(x)                   ((u32)(((x >> 16) & 0x00000007)))
#define SYSON_BIT_SEC_SCLK_DIVTRG                   ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define SYSON_MASK_SEC_SCLK_DIVS                    ((u32)0x0000003F << 8)                   /*!< R/W   0x1
                                                                                              * Clock divider factor.
                                                                                              * For example, if the requirement div ratio is 7/4, then:
                                                                                              * *_DIVN = 0x7
                                                                                              * *_DIVS = 0x4
                                                                                              */
#define SYSON_SEC_SCLK_DIVS(x)                      ((u32)(((x) & 0x0000003F) << 8))
#define SYSON_GET_SEC_SCLK_DIVS(x)                  ((u32)(((x >> 8) & 0x0000003F)))
#define SYSON_MASK_SEC_SCLK_DIVN                    ((u32)0x0000003F << 0)                   /*!< R/W   0x3
                                                                                              * Clock divider factor.
                                                                                              * For example, if the requirement div ratio is 7/4, then:
                                                                                              * *_DIVN = 0x7
                                                                                              * *_DIVS = 0x4
                                                                                              */
#define SYSON_SEC_SCLK_DIVN(x)                      ((u32)(((x) & 0x0000003F) << 0))
#define SYSON_GET_SEC_SCLK_DIVN(x)                  ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_PON_ADC_CLK_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_ADC_CLK_VLD                       ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define SYSON_BIT_ADC_CLK_DIV_DONE                  ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define SYSON_BIT_ADC_CLK_EN                        ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock enable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define SYSON_MASK_ADC_CLK_SEL                      ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: XTAL_40M
                                                                                              * 0x1: OSC 4M
                                                                                              * 0x2: SDM 32K
                                                                                              * 0x3: OSC 131K
                                                                                              * Others: 1'b0
                                                                                              */
#define SYSON_ADC_CLK_SEL(x)                        ((u32)(((x) & 0x00000007) << 16))
#define SYSON_GET_ADC_CLK_SEL(x)                    ((u32)(((x >> 16) & 0x00000007)))
#define SYSON_BIT_ADC_CLK_DIVTRG                    ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define SYSON_MASK_ADC_CLK_DIVN                     ((u32)0x0000003F << 0)                   /*!< R/W   0x0 Clock divider factor. */
#define SYSON_ADC_CLK_DIVN(x)                       ((u32)(((x) & 0x0000003F) << 0))
#define SYSON_GET_ADC_CLK_DIVN(x)                   ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_PON_RFC_CLK_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_RFC_CLK_DIV_DONE                  ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define SYSON_BIT_RFC_CLK_EN                        ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock enable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define SYSON_BIT_RFC_CLK_DIVTRG                    ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define SYSON_MASK_RFC_CLK_DIVN                     ((u32)0x0000003F << 0)                   /*!< R/W   0x3 Clock divider factor. */
#define SYSON_RFC_CLK_DIVN(x)                       ((u32)(((x) & 0x0000003F) << 0))
#define SYSON_GET_RFC_CLK_DIVN(x)                   ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_PON_KM0_FW_CLK_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_KM0_FW_CLK_XTAL40M_SEL            ((u32)0x00000001 << 25)                  /*!< R/W   0x0 1: Select XTAL 40M; 0: Based on [18:16] */
#define SYSON_BIT_KM0_FW_CLK_VLD                    ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define SYSON_BIT_KM0_FW_CLK_DIV_DONE               ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define SYSON_BIT_KM0_FW_CLK_EN                     ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock enable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) set KM0_FW_CLK_XTAL_40M_SEL = 1
                                                                                              * (2) Disable clock.
                                                                                              * (3) Poll clock valid = 0.
                                                                                              * (4) Program clock select bits.
                                                                                              * (5) Enable clock.
                                                                                              * (6) set KM0_FW_CLK_XTAL_40M_SEL = 0
                                                                                              */
#define SYSON_MASK_KM0_FW_CLK_SEL                   ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: XTAL 40M
                                                                                              * 0x1: OSC 4M
                                                                                              * 0x2: OSC 131K
                                                                                              * Others: 1'b0
                                                                                              */
#define SYSON_KM0_FW_CLK_SEL(x)                     ((u32)(((x) & 0x00000007) << 16))
#define SYSON_GET_KM0_FW_CLK_SEL(x)                 ((u32)(((x >> 16) & 0x00000007)))
#define SYSON_BIT_KM0_FW_CLK_DIVTRG                 ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define SYSON_MASK_KM0_FW_CLK_DIVS                  ((u32)0x0000003F << 8)                   /*!< R/W   0x1 Clock divider factor. */
#define SYSON_KM0_FW_CLK_DIVS(x)                    ((u32)(((x) & 0x0000003F) << 8))
#define SYSON_GET_KM0_FW_CLK_DIVS(x)                ((u32)(((x >> 8) & 0x0000003F)))
#define SYSON_MASK_KM0_FW_CLK_DIVN                  ((u32)0x0000003F << 0)                   /*!< R/W   0x1
                                                                                              * Clock divider factor.
                                                                                              * Example:
                                                                                              * For clkdiv ratio 7/2, below is the setting:
                                                                                              * *_DIVN = 7
                                                                                              * *_DIVS = 2
                                                                                              * 
                                                                                              * Note that DIVN can't be less than DIVS
                                                                                              */
#define SYSON_KM0_FW_CLK_DIVN(x)                    ((u32)(((x) & 0x0000003F) << 0))
#define SYSON_GET_KM0_FW_CLK_DIVN(x)                ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_PON_KM4_NP_CLK_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_KM4_NP_CLK_XTAL40M_SEL            ((u32)0x00000001 << 25)                  /*!< R/W   0x0 1: Select XTAL 40M; 0: Based on [18:16] */
#define SYSON_BIT_KM4_NP_CLK_VLD                    ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define SYSON_BIT_KM4_NP_CLK_DIV_DONE               ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define SYSON_BIT_KM4_NP_CLK_EN                     ((u32)0x00000001 << 19)                  /*!< R/W   0x1
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
#define SYSON_MASK_KM4_NP_CLK_SEL                   ((u32)0x00000007 << 16)                  /*!< R/W   0x6
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: CPUD2 PLL
                                                                                              * 0x6: XTAL40M
                                                                                              * 0x7: OSC 4M
                                                                                              * Others: 1'b0
                                                                                              */
#define SYSON_KM4_NP_CLK_SEL(x)                     ((u32)(((x) & 0x00000007) << 16))
#define SYSON_GET_KM4_NP_CLK_SEL(x)                 ((u32)(((x >> 16) & 0x00000007)))
#define SYSON_BIT_KM4_NP_CLK_DIVTRG                 ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define SYSON_MASK_KM4_NP_CLK_DIVS                  ((u32)0x0000003F << 8)                   /*!< R/W   0x1 Clock divider factor. */
#define SYSON_KM4_NP_CLK_DIVS(x)                    ((u32)(((x) & 0x0000003F) << 8))
#define SYSON_GET_KM4_NP_CLK_DIVS(x)                ((u32)(((x >> 8) & 0x0000003F)))
#define SYSON_MASK_KM4_NP_CLK_DIVN                  ((u32)0x0000003F << 0)                   /*!< R/W   0x1
                                                                                              * Clock divider factor.
                                                                                              * Example:
                                                                                              * For clkdiv ratio 7/2, below is the setting:
                                                                                              * *_DIVN = 7
                                                                                              * *_DIVS = 2
                                                                                              * 
                                                                                              * Note that DIVN can't be less than DIVS
                                                                                              */
#define SYSON_KM4_NP_CLK_DIVN(x)                    ((u32)(((x) & 0x0000003F) << 0))
#define SYSON_GET_KM4_NP_CLK_DIVN(x)                ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_PON_PSRAMC_CLK_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_PSRAMC_CLK_VLD                    ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define SYSON_BIT_PSRAMC_CLK_DIV_DONE               ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define SYSON_BIT_PSRAMC_CLK_EN                     ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock enable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define SYSON_MASK_PSRAMC_CLK_SEL                   ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: XTAL 40M
                                                                                              * Others: 1'b0
                                                                                              */
#define SYSON_PSRAMC_CLK_SEL(x)                     ((u32)(((x) & 0x00000007) << 16))
#define SYSON_GET_PSRAMC_CLK_SEL(x)                 ((u32)(((x >> 16) & 0x00000007)))
#define SYSON_BIT_PSRAMC_CLK_DIVTRG                 ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define SYSON_MASK_PSRAMC_CLK_DIVN                  ((u32)0x0000003F << 0)                   /*!< R/W   0x1 Clock divider factor. */
#define SYSON_PSRAMC_CLK_DIVN(x)                    ((u32)(((x) & 0x0000003F) << 0))
#define SYSON_GET_PSRAMC_CLK_DIVN(x)                ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_PON_PWM_CLK_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_PWM_CLK_VLD                       ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define SYSON_BIT_PWM_CLK_DIV_DONE                  ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define SYSON_BIT_PWM_CLK_EN                        ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock enable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define SYSON_MASK_PWM_CLK_SEL                      ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: XTAL 40M
                                                                                              * 0x1: OSC 4M
                                                                                              * 0x2: SDM 32K
                                                                                              * 0x3: OSC 131K
                                                                                              * Others: 1'b0
                                                                                              */
#define SYSON_PWM_CLK_SEL(x)                        ((u32)(((x) & 0x00000007) << 16))
#define SYSON_GET_PWM_CLK_SEL(x)                    ((u32)(((x >> 16) & 0x00000007)))
#define SYSON_BIT_PWM_CLK_DIVTRG                    ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define SYSON_MASK_PWM_CLK_DIVN                     ((u32)0x0000003F << 0)                   /*!< R/W   0x0 Clock divider factor. */
#define SYSON_PWM_CLK_DIVN(x)                       ((u32)(((x) & 0x0000003F) << 0))
#define SYSON_GET_PWM_CLK_DIVN(x)                   ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_PON_SLOW_PCLK_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_SLOW_PCLK_XTAL_40M_SEL            ((u32)0x00000001 << 25)                  /*!< R/W   0x0 None */
#define SYSON_BIT_SLOW_PCLK_VLD                     ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define SYSON_BIT_SLOW_PCLK_DIV_DONE                ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define SYSON_BIT_SLOW_PCLK_EN                      ((u32)0x00000001 << 19)                  /*!< R/W   0x1
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
#define SYSON_MASK_SLOW_PCLK_SEL                    ((u32)0x00000007 << 16)                  /*!< R/W   0x5
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: XTAL 40M
                                                                                              * 0x6: OSC 4M
                                                                                              * 0x7: SDM 32K
                                                                                              * Others: 1'b0
                                                                                              */
#define SYSON_SLOW_PCLK_SEL(x)                      ((u32)(((x) & 0x00000007) << 16))
#define SYSON_GET_SLOW_PCLK_SEL(x)                  ((u32)(((x >> 16) & 0x00000007)))
#define SYSON_BIT_SLOW_PCLK_DIVTRG                  ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define SYSON_MASK_SLOW_PCLK_DIVS                   ((u32)0x0000003F << 8)                   /*!< R/W   0x1 Clock divider factor. */
#define SYSON_SLOW_PCLK_DIVS(x)                     ((u32)(((x) & 0x0000003F) << 8))
#define SYSON_GET_SLOW_PCLK_DIVS(x)                 ((u32)(((x >> 8) & 0x0000003F)))
#define SYSON_MASK_SLOW_PCLK_DIVN                   ((u32)0x0000003F << 0)                   /*!< R/W   0x1
                                                                                              * Clock divider factor.
                                                                                              * Example:
                                                                                              * For clkdiv ratio 7/2, below is the setting:
                                                                                              * *_DIVN = 7
                                                                                              * *_DIVS = 2
                                                                                              * 
                                                                                              * Note that DIVN can't be less than DIVS
                                                                                              */
#define SYSON_SLOW_PCLK_DIVN(x)                     ((u32)(((x) & 0x0000003F) << 0))
#define SYSON_GET_SLOW_PCLK_DIVN(x)                 ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_PON_SLOW_SCLK_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_SLOW_SCLK_VLD                     ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define SYSON_BIT_SLOW_SCLK_DIV_DONE                ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define SYSON_BIT_SLOW_SCLK_EN                      ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock enable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define SYSON_MASK_SLOW_SCLK_SEL                    ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: XTAL 40M
                                                                                              * 0x6: OSC 4M
                                                                                              * 0x7: SDM 32K
                                                                                              * Others: 1'b0
                                                                                              */
#define SYSON_SLOW_SCLK_SEL(x)                      ((u32)(((x) & 0x00000007) << 16))
#define SYSON_GET_SLOW_SCLK_SEL(x)                  ((u32)(((x >> 16) & 0x00000007)))
#define SYSON_BIT_SLOW_SCLK_DIVTRG                  ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define SYSON_MASK_SLOW_SCLK_DIVN                   ((u32)0x0000003F << 0)                   /*!< R/W   0x4 Clock divider factor. */
#define SYSON_SLOW_SCLK_DIVN(x)                     ((u32)(((x) & 0x0000003F) << 0))
#define SYSON_GET_SLOW_SCLK_DIVN(x)                 ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_PON_SPIC_CLK_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_SPIC_CLK_VLD                      ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define SYSON_BIT_SPIC_CLK_DIV_DONE                 ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define SYSON_BIT_SPIC_CLK_EN                       ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock enable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define SYSON_MASK_SPIC_CLK_SEL                     ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: SYS1 PLL
                                                                                              * 0x1: SYS2 PLL
                                                                                              * 0x2: PERI PLL
                                                                                              * 0x3: DDR PLL
                                                                                              * 0x4: USB2 PLL
                                                                                              * 0x5: XTAL 40M
                                                                                              * Others: 1'b0
                                                                                              */
#define SYSON_SPIC_CLK_SEL(x)                       ((u32)(((x) & 0x00000007) << 16))
#define SYSON_GET_SPIC_CLK_SEL(x)                   ((u32)(((x >> 16) & 0x00000007)))
#define SYSON_BIT_SPIC_CLK_DIVTRG                   ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define SYSON_MASK_SPIC_CLK_DIVN                    ((u32)0x0000003F << 0)                   /*!< R/W   0x1 Clock divider factor. */
#define SYSON_SPIC_CLK_DIVN(x)                      ((u32)(((x) & 0x0000003F) << 0))
#define SYSON_GET_SPIC_CLK_DIVN(x)                  ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_PON_TIMER0_CLK_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_TIMER0_CLK_VLD                    ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define SYSON_BIT_TIMER0_CLK_DIV_DONE               ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define SYSON_BIT_TIMER0_CLK_EN                     ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock enable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define SYSON_MASK_TIMER0_CLK_SEL                   ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: XTAL 40M
                                                                                              * 0x1: OSC 4M
                                                                                              * 0x2: SDM 32K
                                                                                              * 0x3: OSC 131K
                                                                                              * Others: 1'b0
                                                                                              */
#define SYSON_TIMER0_CLK_SEL(x)                     ((u32)(((x) & 0x00000007) << 16))
#define SYSON_GET_TIMER0_CLK_SEL(x)                 ((u32)(((x >> 16) & 0x00000007)))
#define SYSON_BIT_TIMER0_CLK_DIVTRG                 ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define SYSON_MASK_TIMER0_CLK_DIVN                  ((u32)0x0000003F << 0)                   /*!< R/W   0x0 Clock divider factor. */
#define SYSON_TIMER0_CLK_DIVN(x)                    ((u32)(((x) & 0x0000003F) << 0))
#define SYSON_GET_TIMER0_CLK_DIVN(x)                ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_PON_TIMER123_CLK_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_TIMER123_CLK_VLD                  ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define SYSON_BIT_TIMER123_CLK_DIV_DONE             ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define SYSON_BIT_TIMER123_CLK_EN                   ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock enable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define SYSON_MASK_TIMER123_CLK_SEL                 ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Clock select:
                                                                                              * 0x0: XTAL 40M
                                                                                              * 0x1: OSC 4M
                                                                                              * 0x2: SDM 32K
                                                                                              * 0x3: OSC 131K
                                                                                              * 0x4: PERI PLL
                                                                                              * Others: 1'b0
                                                                                              */
#define SYSON_TIMER123_CLK_SEL(x)                   ((u32)(((x) & 0x00000007) << 16))
#define SYSON_GET_TIMER123_CLK_SEL(x)               ((u32)(((x >> 16) & 0x00000007)))
#define SYSON_BIT_TIMER123_CLK_DIVTRG               ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define SYSON_MASK_TIMER123_CLK_DIVN                ((u32)0x0000003F << 0)                   /*!< R/W   0x0 Clock divider factor. */
#define SYSON_TIMER123_CLK_DIVN(x)                  ((u32)(((x) & 0x0000003F) << 0))
#define SYSON_GET_TIMER123_CLK_DIVN(x)              ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_PON_0_CLK_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_PON_GPIO_INTRCLK_SEL              ((u32)0x00000001 << 31)                  /*!< R/W   0x0 INTR select 0: pclk, 1: sclk PON_GPIO_INTRCLK_SEL */
#define SYSON_BIT_PON_TIMER_INTRCLK_SEL             ((u32)0x00000001 << 30)                  /*!< R/W   0x0 INTR select 0: pclk, 1: sclk PON_TIMER_INTRCLK_SEL */
#define SYSON_BIT_PON_PWM_INTRCLK_SEL               ((u32)0x00000001 << 29)                  /*!< R/W   0x0 INTR select 0: pclk, 1: sclk PON_PWM_INTRCLK_SEL */
#define SYSON_BIT_PON_TIMER1_INTR_CLK_SEL           ((u32)0x00000001 << 28)                  /*!< R/W   0x0 INTR select 0: pclk, 1: sclk PON_TIMER1_INTR_CLK_SEL */
#define SYSON_BIT_PON_TIMER2_INTR_CLK_SEL           ((u32)0x00000001 << 27)                  /*!< R/W   0x0 INTR select 0: pclk, 1: sclk PON_TIMER2_INTR_CLK_SEL */
#define SYSON_BIT_PON_TIMER3_INTR_CLK_SEL           ((u32)0x00000001 << 26)                  /*!< R/W   0x0 INTR select 0: pclk, 1: sclk PON_TIMER3_INTR_CLK_SEL */
#define SYSON_BIT_PON_ADC_CLKEN                     ((u32)0x00000001 << 25)                  /*!< R/W   0x0 Clock enable PON_ADC_CLKEN */
#define SYSON_BIT_PON_ADC_PCLKEN                    ((u32)0x00000001 << 24)                  /*!< R/W   0x0 Clock enable PON_ADC_PCLKEN */
#define SYSON_BIT_PON_AUDIO_CODEC_CLKEN             ((u32)0x00000001 << 23)                  /*!< R/W   0x0 Clock enable PON_AUDIO_CODEC_CLKEN */
#define SYSON_BIT_PON_FLASHSEC_CLKEN                ((u32)0x00000001 << 22)                  /*!< R/W   0x0 Clock enable PON_FLASHSEC_CLKEN */
#define SYSON_BIT_PON_GDMA0_PCLKEN                  ((u32)0x00000001 << 21)                  /*!< R/W   0x0 Clock enable PON_GDMA0_PCLKEN */
#define SYSON_BIT_PON_I2C0_PCLKEN                   ((u32)0x00000001 << 20)                  /*!< R/W   0x0 Clock enable PON_I2C0_PCLKEN */
#define SYSON_BIT_PON_I2C0_SCLKEN                   ((u32)0x00000001 << 19)                  /*!< R/W   0x0 Clock enable PON_I2C0_SCLKEN */
#define SYSON_BIT_PON_I2C1_PCLKEN                   ((u32)0x00000001 << 18)                  /*!< R/W   0x0 Clock enable PON_I2C1_PCLKEN */
#define SYSON_BIT_PON_I2C1_SCLKEN                   ((u32)0x00000001 << 17)                  /*!< R/W   0x0 Clock enable PON_I2C1_SCLKEN */
#define SYSON_BIT_PON_I2C2_PCLKEN                   ((u32)0x00000001 << 16)                  /*!< R/W   0x0 Clock enable PON_I2C2_PCLKEN */
#define SYSON_BIT_PON_I2C2_SCLKEN                   ((u32)0x00000001 << 15)                  /*!< R/W   0x0 Clock enable PON_I2C2_SCLKEN */
#define SYSON_BIT_PON_I2C3_PCLKEN                   ((u32)0x00000001 << 14)                  /*!< R/W   0x0 Clock enable PON_I2C3_PCLKEN */
#define SYSON_BIT_PON_I2C3_SCLKEN                   ((u32)0x00000001 << 13)                  /*!< R/W   0x0 Clock enable PON_I2C3_SCLKEN */
#define SYSON_BIT_PON_IPC_PCLKEN                    ((u32)0x00000001 << 12)                  /*!< R/W   0x0 Clock enable PON_IPC_PCLKEN */
#define SYSON_BIT_PON_KM0_FW_PCLKEN                 ((u32)0x00000001 << 11)                  /*!< R/W   0x0 Clock enable PON_KM0_FW_PCLKEN */
#define SYSON_BIT_PON_KM0_FW_SCLKEN                 ((u32)0x00000001 << 10)                  /*!< R/W   0x0 Clock enable PON_KM0_FW_SCLKEN */
#define SYSON_BIT_PON_KM4_NP_PCLKEN                 ((u32)0x00000001 << 9)                   /*!< R/W   0x0 Clock enable PON_KM4_NP_PCLKEN */
#define SYSON_BIT_PON_KM4_NP_SCLKEN                 ((u32)0x00000001 << 8)                   /*!< R/W   0x0 Clock enable PON_KM4_NP_SCLKEN */
#define SYSON_BIT_PON_PBUS_ADC_CLKEN                ((u32)0x00000001 << 7)                   /*!< R/W   0x0 Clock enable PON_PBUS_ADC_CLKEN */
#define SYSON_BIT_PON_PBUS_AON_CLKEN                ((u32)0x00000001 << 6)                   /*!< R/W   0x0 Clock enable PON_PBUS_AON_CLKEN */
#define SYSON_BIT_PON_PBUS_KM0_FW_CLKEN             ((u32)0x00000001 << 5)                   /*!< R/W   0x0 Clock enable PON_PBUS_KM0_FW_CLKEN */
#define SYSON_BIT_PON_PBUS_KM4_NP_CLKEN             ((u32)0x00000001 << 4)                   /*!< R/W   0x0 Clock enable PON_PBUS_KM4_NP_CLKEN */
#define SYSON_BIT_PON_PBUS_OTPC_CLKEN               ((u32)0x00000001 << 3)                   /*!< R/W   0x0 Clock enable PON_PBUS_OTPC_CLKEN */
#define SYSON_BIT_PON_PBUS_PCLKEN                   ((u32)0x00000001 << 2)                   /*!< R/W   0x0 Clock enable PON_PBUS_PCLKEN */
#define SYSON_BIT_PON_PBUS_PSRAMC_CLKEN             ((u32)0x00000001 << 1)                   /*!< R/W   0x0 Clock enable PON_PBUS_PSRAMC_CLKEN */
#define SYSON_BIT_PON_PBUS_SEC_CLKEN                ((u32)0x00000001 << 0)                   /*!< R/W   0x0 Clock enable PON_PBUS_SEC_CLKEN */
/** @} */


/** @defgroup REG_PON_1_CLK_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_PON_PBUS_SLOW_PCLKEN              ((u32)0x00000001 << 31)                  /*!< R/W   0x0 Clock enable PON_PBUS_SLOW_PCLKEN */
#define SYSON_BIT_PON_PBUS_SPIC_CLKEN               ((u32)0x00000001 << 30)                  /*!< R/W   0x0 No used */
#define SYSON_BIT_PON_PBUS_SYSMEM_CLKEN             ((u32)0x00000001 << 29)                  /*!< R/W   0x0 Clock enable PON_PBUS_SYSMEM_CLKEN */
#define SYSON_BIT_PON_PBUS_TRNG_CLKEN               ((u32)0x00000001 << 28)                  /*!< R/W   0x0 Clock enable PON_PBUS_TRNG_CLKEN */
#define SYSON_BIT_PON_PBUS_WMAC_CLKEN               ((u32)0x00000001 << 27)                  /*!< R/W   0x0 Clock enable PON_PBUS_WMAC_CLKEN */
#define SYSON_BIT_PON_PINMUX_PCLKEN                 ((u32)0x00000001 << 26)                  /*!< R/W   0x0 Clock enable PON_PINMUX_PCLKEN */
#define SYSON_BIT_PON_GPIO_PCLKEN                   ((u32)0x00000001 << 25)                  /*!< R/W   0x0 Clock enable PON_GPIO_PCLKEN */
#define SYSON_BIT_PON_GPIO_SCLKEN                   ((u32)0x00000001 << 24)                  /*!< R/W   0x0 Clock enable PON_GPIO_SCLKEN */
#define SYSON_BIT_PON_TIMER_PCLKEN                  ((u32)0x00000001 << 23)                  /*!< R/W   0x0 Clock enable PON_TIMER_PCLKEN */
#define SYSON_BIT_PON_TIMER_SCLKEN                  ((u32)0x00000001 << 22)                  /*!< R/W   0x0 Clock enable PON_TIMER_SCLKEN */
#define SYSON_BIT_PON_REG_CLKEN                     ((u32)0x00000001 << 21)                  /*!< R/W   0x0 No used */
#define SYSON_BIT_PON_UART_PCLKEN                   ((u32)0x00000001 << 20)                  /*!< R/W   0x0 No used */
#define SYSON_BIT_PON_UART_SCLKEN                   ((u32)0x00000001 << 19)                  /*!< R/W   0x0 No used */
#define SYSON_BIT_PON_PSRAMC_CLKEN                  ((u32)0x00000001 << 18)                  /*!< R/W   0x0 Clock enable PON_PSRAMC_CLKEN */
#define SYSON_BIT_PON_PSRAMPHY_CLKEN                ((u32)0x00000001 << 17)                  /*!< R/W   0x0 Clock enable PON_PSRAMPHY_CLKEN */
#define SYSON_BIT_PON_PWM_SCLKEN                    ((u32)0x00000001 << 16)                  /*!< R/W   0x0 Clock enable PON_PWM_SCLKEN */
#define SYSON_BIT_PON_PWM_PCLKEN                    ((u32)0x00000001 << 15)                  /*!< R/W   0x0 Clock enable PON_PWM_PCLKEN */
#define SYSON_BIT_PON_SHARE_SRAM_CLKEN              ((u32)0x00000001 << 14)                  /*!< R/W   0x0 Clock enable PON_SHARE_SRAM_CLKEN */
#define SYSON_BIT_PON_SIC_HCLKEN                    ((u32)0x00000001 << 13)                  /*!< R/W   0x0 Clock enable PON_SIC_HCLKEN */
#define SYSON_BIT_PON_SPI_M0_PCLKEN                 ((u32)0x00000001 << 12)                  /*!< R/W   0x0 Clock enable PON_SPI_M0_PCLKEN */
#define SYSON_BIT_PON_SPI_M0_SCLKEN                 ((u32)0x00000001 << 11)                  /*!< R/W   0x0 Clock enable PON_SPI_M0_SCLKEN */
#define SYSON_BIT_PON_SPI_M1_PCLKEN                 ((u32)0x00000001 << 10)                  /*!< R/W   0x0 Clock enable PON_SPI_M1_PCLKEN */
#define SYSON_BIT_PON_SPI_M1_SCLKEN                 ((u32)0x00000001 << 9)                   /*!< R/W   0x0 Clock enable PON_SPI_M1_SCLKEN */
#define SYSON_BIT_PON_SPI_S0_PCLKEN                 ((u32)0x00000001 << 8)                   /*!< R/W   0x0 Clock enable PON_SPI_S0_PCLKEN */
#define SYSON_BIT_PON_SPI_S0_SCLKEN                 ((u32)0x00000001 << 7)                   /*!< R/W   0x0 Clock enable PON_SPI_S0_SCLKEN */
#define SYSON_BIT_PON_SPI_S1_PCLKEN                 ((u32)0x00000001 << 6)                   /*!< R/W   0x0 Clock enable PON_SPI_S1_PCLKEN */
#define SYSON_BIT_PON_SPI_S1_SCLKEN                 ((u32)0x00000001 << 5)                   /*!< R/W   0x0 Clock enable PON_SPI_S1_SCLKEN */
#define SYSON_BIT_PON_SPIC_CLKEN                    ((u32)0x00000001 << 4)                   /*!< R/W   0x0 Clock enable PON_SPIC_CLKEN */
#define SYSON_BIT_PON_SPICPHY_CLKEN                 ((u32)0x00000001 << 3)                   /*!< R/W   0x0 Clock enable PON_SPICPHY_CLKEN */
#define SYSON_BIT_PON_SYS_SRAM_CLKEN                ((u32)0x00000001 << 2)                   /*!< R/W   0x0 Clock enable PON_SYS_SRAM_CLKEN */
#define SYSON_BIT_PON_TIMER1_PCLKEN                 ((u32)0x00000001 << 1)                   /*!< R/W   0x0 Clock enable PON_TIMER1_PCLKEN */
#define SYSON_BIT_PON_TIMER1_SCLKEN                 ((u32)0x00000001 << 0)                   /*!< R/W   0x0 Clock enable PON_TIMER1_SCLKEN */
/** @} */


/** @defgroup REG_PON_2_CLK_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_PON_TIMER2_PCLKEN                 ((u32)0x00000001 << 31)                  /*!< R/W   0x0 Clock enable PON_TIMER2_PCLKEN */
#define SYSON_BIT_PON_TIMER2_SCLKEN                 ((u32)0x00000001 << 30)                  /*!< R/W   0x0 Clock enable PON_TIMER2_SCLKEN */
#define SYSON_BIT_PON_TIMER3_PCLKEN                 ((u32)0x00000001 << 29)                  /*!< R/W   0x0 Clock enable PON_TIMER3_PCLKEN */
#define SYSON_BIT_PON_TIMER3_SCLKEN                 ((u32)0x00000001 << 28)                  /*!< R/W   0x0 Clock enable PON_TIMER3_SCLKEN */
#define SYSON_BIT_PON_UART1_PCLKEN                  ((u32)0x00000001 << 27)                  /*!< R/W   0x0 Clock enable PON_UART1_PCLKEN */
#define SYSON_BIT_PON_UART1_SCLKEN                  ((u32)0x00000001 << 26)                  /*!< R/W   0x0 Clock enable PON_UART1_SCLKEN */
#define SYSON_BIT_PON_UART2_PCLKEN                  ((u32)0x00000001 << 25)                  /*!< R/W   0x0 Clock enable for BTUART PCLK */
#define SYSON_BIT_PON_UART2_SCLKEN                  ((u32)0x00000001 << 24)                  /*!< R/W   0x0 Clock enable for BTUART SCLK */
#define SYSON_BIT_PON_LOGUART_PCLKEN                ((u32)0x00000001 << 23)                  /*!< R/W   0x0 Clock enable PON_LOGUART_PCLKEN */
#define SYSON_BIT_PON_LOGUART_SCLKEN                ((u32)0x00000001 << 22)                  /*!< R/W   0x0 Clock enable PON_LOGUART_SCLKEN */
#define SYSON_BIT_PON_WDT_FW_PCLKEN                 ((u32)0x00000001 << 21)                  /*!< R/W   0x0 Clock enable PON_WDT_FW_PCLKEN */
#define SYSON_BIT_PON_WDT_FW_SCLKEN                 ((u32)0x00000001 << 20)                  /*!< R/W   0x0 Clock enable PON_WDT_FW_SCLKEN */
#define SYSON_BIT_PON_WDT_NP_PCLKEN                 ((u32)0x00000001 << 19)                  /*!< R/W   0x0 Clock enable PON_WDT_NP_PCLKEN */
#define SYSON_BIT_PON_WDT_NP_SCLKEN                 ((u32)0x00000001 << 18)                  /*!< R/W   0x0 Clock enable PON_WDT_NP_SCLKEN */
#define SYSON_BIT_PON_WMAC_CLKEN                    ((u32)0x00000001 << 16)                  /*!< R/W   0x0 1: Enable WMAC HCI clock */
#define SYSON_BIT_PON_WLOFF_REG_CLKEN               ((u32)0x00000001 << 15)                  /*!< R/W   0x0 Clock enable PON_WLOFF_REG_CLKEN */
#define SYSON_BIT_PON_PKE_CLKEN                     ((u32)0x00000001 << 14)                  /*!< R/W   0x0 Clock enable PON_PKE_CLKEN */
#define SYSON_BIT_PON_HMAC_CLKEN                    ((u32)0x00000001 << 13)                  /*!< R/W   0x0 Clock enable for crypto IP (HMAC + AES) */
#define SYSON_BIT_PON_AES_CLKEN                     ((u32)0x00000001 << 12)                  /*!< R/W   0x0 No used in AmebaPro3 */
#define SYSON_BIT_PON_TRNG_CLKEN                    ((u32)0x00000001 << 11)                  /*!< R/W   0x0
                                                                                              * NOT use in AmebaPro3.
                                                                                              * TRNG clock control is by REG_PON_1_CLK_CTRL[28]
                                                                                              */
#define SYSON_BIT_PON_MCUOFF_REG_CLKEN              ((u32)0x00000001 << 10)                  /*!< R/W   0x0 Clock enable PON_MCUOFF_REG_CLKEN */
/** @} */


/** @defgroup REG_PON_0_FUNC_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_CORE_PBUS_SYSMEM_CLK_FEN_P2_4     ((u32)0x00000001 << 10)                  /*!< R/W   0  
                                                                                              * CORE platfrom function enable (sysmem_clk clock domain)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_MCUOFF_PBUS_SYSMEM_CLK_FEN_P2_4   ((u32)0x00000001 << 9)                   /*!< R/W   0  
                                                                                              * MCUOFF platfrom function enable (sysmem_clk clock domain)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_MCUOFF_PBUS_SLOWPCLK_FEN_P2_6     ((u32)0x00000001 << 8)                   /*!< R/W   0  
                                                                                              * MCUOFF platfrom function enable (slow_pclk clock domain)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_MCUOFF_REG_FEN_P11            ((u32)0x00000001 << 5)                   /*!< R/W   0  
                                                                                              * MCUOFF REG function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_WLOFF_REG_FEN_P10             ((u32)0x00000001 << 4)                   /*!< R/W   0  
                                                                                              * WLOFF REG function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_BT_FEN_P3                     ((u32)0x00000001 << 3)                   /*!< R/W   0  
                                                                                              * BT function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_SYSON_REG_FEN_P4              ((u32)0x00000001 << 2)                   /*!< R/W     0
                                                                                              * SYSON REG function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_PBUS_PSRAMC_FEN_P2_0          ((u32)0x00000001 << 1)                   /*!< R/W   0  
                                                                                              * MCUOFF Platfrom function enable (psram_clk clock domain)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_PBUS_SEC_FEN_P2_1             ((u32)0x00000001 << 0)                   /*!< R/W   0  
                                                                                              * MCUOFF Platfrom function enable (sec_clk clock domain)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
/** @} */


/** @defgroup REG_PON_1_FUNC_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_PON_FEN_P0                        ((u32)0x00000001 << 31)                  /*!< R/W   0  
                                                                                              * Network CPU  function enable (warm reset)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_P1                        ((u32)0x00000001 << 30)                  /*!< R/W     0
                                                                                              * Network CPU function enable (cold reset)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_P2                        ((u32)0x00000001 << 29)                  /*!< R/W   0x0
                                                                                              * WIFI PLATFORM  function enable (global reset)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_P2_10                     ((u32)0x00000001 << 28)                  /*!< R/W   0  
                                                                                              * SYSON PLATFORM  function enable (aon_clk domain reset)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_P2_11                     ((u32)0x00000001 << 27)                  /*!< R/W     0 No used */
#define SYSON_BIT_PON_FEN_P2_2                      ((u32)0x00000001 << 26)                  /*!< R/W     0 No used */
#define SYSON_BIT_PON_FEN_P2_3                      ((u32)0x00000001 << 25)                  /*!< R/W   0  
                                                                                              * MCUOFF PLATFORM  function enable (km4_np_clk domain reset)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_P2_4                      ((u32)0x00000001 << 24)                  /*!< R/W   0  
                                                                                              * SYSON PLATFORM  function enable (system_clk domain reset)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_P2_5                      ((u32)0x00000001 << 23)                  /*!< R/W   0  
                                                                                              * MCUOFF PLATFORM  function enable trng_clk domain reset)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_P2_6                      ((u32)0x00000001 << 22)                  /*!< R/W     0
                                                                                              * SYSON PLATFORM  function enable (slow_pclk domain reset)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_P2_7                      ((u32)0x00000001 << 21)                  /*!< R/W   0   No used */
#define SYSON_BIT_PON_FEN_P2_8                      ((u32)0x00000001 << 20)                  /*!< R/W   0   No used */
#define SYSON_BIT_PON_FEN_P2_9                      ((u32)0x00000001 << 19)                  /*!< R/W   0   No used */
#define SYSON_BIT_PON_FEN_P5                        ((u32)0x00000001 << 18)                  /*!< R/W   0  
                                                                                              * IPC  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_P6                        ((u32)0x00000001 << 17)                  /*!< R/W     0
                                                                                              * Network Processor WDT  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_P6_0                      ((u32)0x00000001 << 16)                  /*!< R/W   0   No used */
#define SYSON_BIT_PON_FEN_P6_1                      ((u32)0x00000001 << 15)                  /*!< R/W     0
                                                                                              * PON GPIO  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_P6_2                      ((u32)0x00000001 << 14)                  /*!< R/W     0
                                                                                              * PWM  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_P6_3                      ((u32)0x00000001 << 13)                  /*!< R/W   0  
                                                                                              * TIMER0  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_P6_4                      ((u32)0x00000001 << 12)                  /*!< R/W     0
                                                                                              * PINMUX  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_P6_5                      ((u32)0x00000001 << 11)                  /*!< R/W     0
                                                                                              * SYSTEM SRAM  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_P6_6                      ((u32)0x00000001 << 10)                  /*!< R/W   0  
                                                                                              * BT SHARE SRAM  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_P6_7                      ((u32)0x00000001 << 9)                   /*!< R/W   0  
                                                                                              * GDMA0  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_S4                        ((u32)0x00000001 << 8)                   /*!< R/W   0  
                                                                                              * KM0 FW Processor  function enable (cold reset)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_S5                        ((u32)0x00000001 << 7)                   /*!< R/W     0
                                                                                              * KM0 FW Processor  function enable (warm reset)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_S7                        ((u32)0x00000001 << 6)                   /*!< R/W   0  
                                                                                              * FW Processor WDT  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_S7_10                     ((u32)0x00000001 << 5)                   /*!< R/W   0  
                                                                                              * SPIC + SPICPHY  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_S7_11                     ((u32)0x00000001 << 4)                   /*!< R/W   0  
                                                                                              * ADC_TOP function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_S7_12                     ((u32)0x00000001 << 3)                   /*!< R/W   0  
                                                                                              * PSRAMC + PSRAMPHY  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_S7_13                     ((u32)0x00000001 << 2)                   /*!< R/W     0
                                                                                              * TIMER3  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_S7_15                     ((u32)0x00000001 << 1)                   /*!< R/W   0  
                                                                                              * FLASH SEC  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
/** @} */


/** @defgroup REG_PON_2_FUNC_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_PON_FEN_S7_18                     ((u32)0x00000001 << 31)                  /*!< R/W   0  
                                                                                              * I2C0  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_S7_19                     ((u32)0x00000001 << 30)                  /*!< R/W     0
                                                                                              * I2C1  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_S7_20                     ((u32)0x00000001 << 29)                  /*!< R/W     0
                                                                                              * I2C2  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_S7_21                     ((u32)0x00000001 << 28)                  /*!< R/W   0  
                                                                                              * I2C3  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_S7_22                     ((u32)0x00000001 << 27)                  /*!< R/W     0
                                                                                              * UART1  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_S7_23                     ((u32)0x00000001 << 26)                  /*!< R/W     0
                                                                                              * BTUART  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_S7_24                     ((u32)0x00000001 << 25)                  /*!< R/W   0  
                                                                                              * LOGUART  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_S7_26                     ((u32)0x00000001 << 24)                  /*!< R/W   0  
                                                                                              * SPI_M0  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_S7_27                     ((u32)0x00000001 << 23)                  /*!< R/W   0  
                                                                                              * SPI_M1  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_S7_27_                    ((u32)0x00000001 << 22)                  /*!< R/W     0
                                                                                              * SPI_S0  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_S7_28                     ((u32)0x00000001 << 21)                  /*!< R/W     0
                                                                                              * SPI_S1  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_S7_30                     ((u32)0x00000001 << 20)                  /*!< R/W   0  
                                                                                              * TIMER1  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PON_FEN_S7_31                     ((u32)0x00000001 << 19)                  /*!< R/W   0  
                                                                                              * TIMER2  function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_TRNG_FEN_S7_40                    ((u32)0x00000001 << 18)                  /*!< R/W   0  
                                                                                              * TRNG function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_HMAC_FEN_S7_39                    ((u32)0x00000001 << 17)                  /*!< R/W     0
                                                                                              * Crypto IP (HMAC + AES) enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_PKE_FEN_S7_38                     ((u32)0x00000001 << 16)                  /*!< R/W   0  
                                                                                              * PKE function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define SYSON_BIT_AES_FEN_S7_37                     ((u32)0x00000001 << 15)                  /*!< R/W   0   No used in AmebaPro3. Both AES and HMAC are enable 0x10C[17] */
/** @} */


/** @defgroup REG_PON_DUMMY24
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_NPWDT_RST_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_NPWDT_SW_RST_TRG                  ((u32)0x00000001 << 16)                  /*!< R/W   0x0 1: Trigger the NPWDT reset by SW. Auto clear by HW */
#define SYSON_BIT_AON_RST_EN                        ((u32)0x00000001 << 14)                  /*!< R/W   0x0 None */
#define SYSON_BIT_PON_RST_EN                        ((u32)0x00000001 << 13)                  /*!< R/W   0x0 None */
#define SYSON_BIT_SYSON_RST_EN                      ((u32)0x00000001 << 12)                  /*!< R/W   0x0 None */
#define SYSON_BIT_NP_COLD_RST_EN                    ((u32)0x00000001 << 11)                  /*!< R/W   0x0 None */
#define SYSON_BIT_MP_COLD_RST_EN                    ((u32)0x00000001 << 10)                  /*!< R/W   0x0 None */
#define SYSON_BIT_AP_WARM_RST_EN                    ((u32)0x00000001 << 9)                   /*!< R/W   0x0 None */
#define SYSON_BIT_MP_WARM_RST_EN                    ((u32)0x00000001 << 8)                   /*!< R/W   0x0 None */
#define SYSON_BIT_FW_COLD_RST_EN                    ((u32)0x00000001 << 7)                   /*!< R/W   0x0 None */
#define SYSON_BIT_FW_WARM_RST_EN                    ((u32)0x00000001 << 6)                   /*!< R/W   0x0 None */
#define SYSON_BIT_PBUS_RST_EN                       ((u32)0x00000001 << 5)                   /*!< R/W   0x0 None */
#define SYSON_BIT_CBUS_RST_EN                       ((u32)0x00000001 << 4)                   /*!< R/W   0x0 None */
#define SYSON_BIT_WL_RST_EN                         ((u32)0x00000001 << 3)                   /*!< R/W   0x0 None */
#define SYSON_BIT_BT_RST_EN                         ((u32)0x00000001 << 2)                   /*!< R/W   0x0 None */
#define SYSON_BIT_AP_COLD_RST_EN                    ((u32)0x00000001 << 1)                   /*!< R/W   0x0 None */
#define SYSON_BIT_MCUOFF_RST_EN                     ((u32)0x00000001 << 0)                   /*!< R/W   0x0 None */
/** @} */


/** @defgroup REG_FWWDT_RST_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_FWWDT_SW_RST_TRG                  ((u32)0x00000001 << 16)                  /*!< R/W   0x0 1: Trigger the FWWDT reset by SW. Auto clear by HW */
#define SYSON_BIT_AON_RST_EN_118                    ((u32)0x00000001 << 14)                  /*!< R/W   0x0 None */
#define SYSON_BIT_PON_RST_EN_118                    ((u32)0x00000001 << 13)                  /*!< R/W   0x0 None */
#define SYSON_BIT_SYSON_RST_EN_118                  ((u32)0x00000001 << 12)                  /*!< R/W   0x0 None */
#define SYSON_BIT_NP_COLD_RST_EN_118                ((u32)0x00000001 << 11)                  /*!< R/W   0x0 None */
#define SYSON_BIT_NP_WARM_RST_EN                    ((u32)0x00000001 << 10)                  /*!< R/W   0x0 None */
#define SYSON_BIT_AP_WARM_RST_EN_118                ((u32)0x00000001 << 9)                   /*!< R/W   0x0 None */
#define SYSON_BIT_MP_COLD_RST_EN_118                ((u32)0x00000001 << 8)                   /*!< R/W   0x0 None */
#define SYSON_BIT_FW_COLD_RST_EN_118                ((u32)0x00000001 << 7)                   /*!< R/W   0x0 None */
#define SYSON_BIT_MP_WARM_RST_EN_118                ((u32)0x00000001 << 6)                   /*!< R/W   0x0 None */
#define SYSON_BIT_PBUS_RST_EN_118                   ((u32)0x00000001 << 5)                   /*!< R/W   0x0 None */
#define SYSON_BIT_CBUS_RST_EN_118                   ((u32)0x00000001 << 4)                   /*!< R/W   0x0 None */
#define SYSON_BIT_WL_RST_EN_118                     ((u32)0x00000001 << 3)                   /*!< R/W   0x0 None */
#define SYSON_BIT_BT_RST_EN_118                     ((u32)0x00000001 << 2)                   /*!< R/W   0x0 None */
#define SYSON_BIT_AP_COLD_RST_EN_118                ((u32)0x00000001 << 1)                   /*!< R/W   0x0 None */
#define SYSON_BIT_MCUOFF_RST_EN_118                 ((u32)0x00000001 << 0)                   /*!< R/W   0x0 None */
/** @} */


/** @defgroup REG_PON_DUMMY29
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY30
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY31
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY32
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY25
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY28
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY26
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_SYS_SPORT0_MUX
 * @brief
 * @{
 */
#define SYSON_BIT_SP0_DIO3_MUXSEL                   ((u32)0x00000001 << 3)                   /*!< R/W   0  
                                                                                              * 0: Enable SPORT0 RX3 transfer via DIO3
                                                                                              * 1: Enable SPORT0 TX0 trnasfer via DIO3
                                                                                              */
#define SYSON_BIT_SP0_DIO2_MUXSEL                   ((u32)0x00000001 << 2)                   /*!< R/W     0
                                                                                              * 0: Enable SPORT0 RX2 transfer via DIO2
                                                                                              * 1: Enable SPORT0 TX1 trnasfer via DIO2
                                                                                              */
#define SYSON_BIT_SP0_DIO1_MUXSEL                   ((u32)0x00000001 << 1)                   /*!< R/W     0
                                                                                              * 0: Enable SPORT0 RX1 transfer via DIO1
                                                                                              * 1: Enable SPORT0 TX2 trnasfer via DIO1
                                                                                              */
#define SYSON_BIT_SP0_DIO0_MUXSEL                   ((u32)0x00000001 << 0)                   /*!< R/W     0
                                                                                              * 0: Enable SPORT0 RX0 transfer via DIO0
                                                                                              * 1: Enable SPORT0 TX3 trnasfer via DIO0
                                                                                              */
/** @} */


/** @defgroup REG_SYS_SPORT1_MUX
 * @brief
 * @{
 */
#define SYSON_BIT_SP1_DIO3_MUXSEL                   ((u32)0x00000001 << 3)                   /*!< R/W   0  
                                                                                              * 0: Enable SPORT1 RX3 transfer via DIO3
                                                                                              * 1: Enable SPORT1 TX0 trnasfer via DIO3
                                                                                              */
#define SYSON_BIT_SP1_DIO2_MUXSEL                   ((u32)0x00000001 << 2)                   /*!< R/W     0
                                                                                              * 0: Enable SPORT1 RX2 transfer via DIO2
                                                                                              * 1: Enable SPORT1 TX1 trnasfer via DIO2
                                                                                              */
#define SYSON_BIT_SP1_DIO1_MUXSEL                   ((u32)0x00000001 << 1)                   /*!< R/W     0
                                                                                              * 0: Enable SPORT1 RX1 transfer via DIO1
                                                                                              * 1: Enable SPORT1 TX2 trnasfer via DIO1
                                                                                              */
#define SYSON_BIT_SP1_DIO0_MUXSEL                   ((u32)0x00000001 << 0)                   /*!< R/W     0
                                                                                              * 0: Enable SPORT1 RX0 transfer via DIO0
                                                                                              * 1: Enable SPORT1 TX3 trnasfer via DIO0
                                                                                              */
/** @} */


/** @defgroup REG_SYS_ADC_CTRL0
 * @brief
 * @{
 */
#define SYSON_MASK_DUMMY_0                          ((u32)0x000000FF << 24)                  /*!< R/W   0x0 None */
#define SYSON_DUMMY_0(x)                            ((u32)(((x) & 0x000000FF) << 24))
#define SYSON_GET_DUMMY_0(x)                        ((u32)(((x >> 24) & 0x000000FF)))
#define SYSON_MASK_BYPASSEN_7_0                     ((u32)0x000000FF << 16)                  /*!< R/W   0x0 1:0.8V mode, 0: 3.3V mode */
#define SYSON_BYPASSEN_7_0(x)                       ((u32)(((x) & 0x000000FF) << 16))
#define SYSON_GET_BYPASSEN_7_0(x)                   ((u32)(((x >> 16) & 0x000000FF)))
#define SYSON_MASK_DUMMY_1                          ((u32)0x00000003 << 14)                  /*!< R/W   00  None */
#define SYSON_DUMMY_1(x)                            ((u32)(((x) & 0x00000003) << 14))
#define SYSON_GET_DUMMY_1(x)                        ((u32)(((x >> 14) & 0x00000003)))
#define SYSON_MASK_REG_VCM_SEL_1_0                  ((u32)0x00000003 << 12)                  /*!< R/W   00  00/01/10/11 0.45/0.4375/0.425/0.4125V */
#define SYSON_REG_VCM_SEL_1_0(x)                    ((u32)(((x) & 0x00000003) << 12))
#define SYSON_GET_REG_VCM_SEL_1_0(x)                ((u32)(((x >> 12) & 0x00000003)))
#define SYSON_BIT_SEL_LNA                           ((u32)0x00000001 << 11)                  /*!< R/W   1   1: comparator low noise for ALL bit */
#define SYSON_BIT_SEL_LN                            ((u32)0x00000001 << 10)                  /*!< R/W   1   1: comparator low noise for LSB 6bit */
#define SYSON_BIT_REG_META                          ((u32)0x00000001 << 9)                   /*!< R/W   0   1: enable meta detect */
#define SYSON_MASK_REG_CMPDEC_1_0                   ((u32)0x00000003 << 7)                   /*!< R/W   01  meta detect time 00: short */
#define SYSON_REG_CMPDEC_1_0(x)                     ((u32)(((x) & 0x00000003) << 7))
#define SYSON_GET_REG_CMPDEC_1_0(x)                 ((u32)(((x >> 7) & 0x00000003)))
#define SYSON_BIT_REG_REV_CLK                       ((u32)0x00000001 << 6)                   /*!< R/W   0   1: rever ckdata */
#define SYSON_MASK_REG_VREF_SEL_1_0                 ((u32)0x00000003 << 4)                   /*!< R/W   01  00:0.75V 01:0.8V 10:0.85V 11:0.9V */
#define SYSON_REG_VREF_SEL_1_0(x)                   ((u32)(((x) & 0x00000003) << 4))
#define SYSON_GET_REG_VREF_SEL_1_0(x)               ((u32)(((x >> 4) & 0x00000003)))
#define SYSON_BIT_REG_DELAY                         ((u32)0x00000001 << 3)                   /*!< R/W   0   clk delay */
#define SYSON_BIT_REG_CK                            ((u32)0x00000001 << 2)                   /*!< R/W   0   1: clk from external */
#define SYSON_BIT_POW_REF                           ((u32)0x00000001 << 1)                   /*!< R/W   0   pow adc refgen */
#define SYSON_BIT_POW                               ((u32)0x00000001 << 0)                   /*!< R/W   0   pow adc */
/** @} */


/** @defgroup REG_SYS_ADC_CTRL1
 * @brief
 * @{
 */
#define SYSON_BIT_SYS_ADC_CTRL_MSR_SEL              ((u32)0x00000001 << 5)                   /*!< R/W     0 0: HW; 1:SW */
#define SYSON_BIT_SYS_ADC_INTR_CK_SEL               ((u32)0x00000001 << 4)                   /*!< R/W     0 0: pclk; 1: sclk */
#define SYSON_BIT_SYS_ISO_ADC                       ((u32)0x00000001 << 3)                   /*!< R/W     1 1: isolation ADC analog to digital domain; 0: disable isolation */
/** @} */


/** @defgroup REG_SYS_PLATFORM_CTRL0
 * @brief
 * @{
 */
#define SYSON_BIT_HS_GDMA0_CLK_ALWS_DIS             ((u32)0x00000001 << 13)                  /*!< R/W     0 gdma_clk_always_disable */
#define SYSON_BIT_HS_GDMA0_CLK_ALWS_EN              ((u32)0x00000001 << 12)                  /*!< R/W   1   gdma_clk_always_enable */
/** @} */


/** @defgroup REG_PON_DUMMY27
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_PON_DUMMY28
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_SYS_PSRAMPHY_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_PSRAM_SPEED_UP_SIM                ((u32)0x00000001 << 4)                   /*!< R/W   0   1: To bypass reset sequence in PSRAMC */
#define SYSON_BIT_PSRAM_FEN                         ((u32)0x00000001 << 1)                   /*!< R/W   0   None */
#define SYSON_BIT_DPHY_DBG_EN                       ((u32)0x00000001 << 0)                   /*!< R/W   0   None */
/** @} */


/** @defgroup REG_SYS_FLASH_CTRL
 * @brief
 * @{
 */
#define SYSON_BIT_HS_FLASH_INIT_OK                  ((u32)0x00000001 << 4)                   /*!< R       0 BOOT_finish  */
#define SYSON_BIT_FLASH_SPEED_UP_SIM                ((u32)0x00000001 << 0)                   /*!< R/W     0 1: to bypass reset sequence in SPIC FLASH */
/** @} */


/** @defgroup REG_SYS_FLASH_PHY_CTRL0
 * @brief
 * @{
 */
#define SYSON_BIT_FLASH_PHYMODE                     ((u32)0x00000001 << 20)                  /*!< R/W     0
                                                                                              * 1: SPIC phy use loopback clock through delay chain from PAD as fetch clk.  
                                                                                              * 0: SPIC phy use spic controller clk trhough delay chain as fetch clk
                                                                                              */
#define SYSON_MASK_FLASH_PHY_FETCH_PHASE            ((u32)0x0000000F << 16)                  /*!< R/W     0
                                                                                              * [0]: delay 0.5 spic_clk period to fetch lower lane data
                                                                                              * [1]: delay 0.5 spic_clk period to fetch upper lane data
                                                                                              * [2]: delay 0.5 spic_clk period to sync lower lane data to spic
                                                                                              * [3]: delay 0.5 spic_clk period to sync upper lane data to spic
                                                                                              */
#define SYSON_FLASH_PHY_FETCH_PHASE(x)              ((u32)(((x) & 0x0000000F) << 16))
#define SYSON_GET_FLASH_PHY_FETCH_PHASE(x)          ((u32)(((x >> 16) & 0x0000000F)))
#define SYSON_BIT_FLASH_PHY_MUX_SEL                 ((u32)0x00000001 << 7)                   /*!< R/W     0
                                                                                              * [0]:NOR FLASH
                                                                                              * [1]: NAND FLASH
                                                                                              */
#define SYSON_MASK_FLASH_PHY_DLY_EN                 ((u32)0x00000003 << 5)                   /*!< R/W     0
                                                                                              * [0]: delay lower lane a spic_clk period
                                                                                              * [1]: delay upper lane a spic_clk period
                                                                                              */
#define SYSON_FLASH_PHY_DLY_EN(x)                   ((u32)(((x) & 0x00000003) << 5))
#define SYSON_GET_FLASH_PHY_DLY_EN(x)               ((u32)(((x >> 5) & 0x00000003)))
#define SYSON_BIT_FLASH_PHY_DQS_DDR_EN              ((u32)0x00000001 << 4)                   /*!< R/W     0 enable DQS to fetch double data rate rx data */
#define SYSON_BIT_FLASH_PHY_DQS_EN                  ((u32)0x00000001 << 3)                   /*!< R/W     0 enable DQS to fetch rx data */
#define SYSON_BIT_FLASH_PHY_MERGE_PHASE_EN          ((u32)0x00000001 << 2)                   /*!< R/W     0 0: posedge rx data first 1: negedge rx data first */
#define SYSON_BIT_FLASH_PHY_OUTPUT_PHASE_EN         ((u32)0x00000001 << 1)                   /*!< R/W     0 delay 0.5 spic_clk period of sclk */
#define SYSON_BIT_FLASH_PHY_EN                      ((u32)0x00000001 << 0)                   /*!< R/W     0 enble rx data */
/** @} */


/** @defgroup REG_SYS_FLASH_PHY_CTRL1
 * @brief
 * @{
 */
#define SYSON_MASK_FLASH_PHY_DQSU_DLY_SEL           ((u32)0x000000FF << 16)                  /*!< R/W     0 select delay level of DQS on the upper lane */
#define SYSON_FLASH_PHY_DQSU_DLY_SEL(x)             ((u32)(((x) & 0x000000FF) << 16))
#define SYSON_GET_FLASH_PHY_DQSU_DLY_SEL(x)         ((u32)(((x >> 16) & 0x000000FF)))
#define SYSON_MASK_FLASH_PHY_DQSL_DLY_SEL           ((u32)0x000000FF << 8)                   /*!< R/W     0 select delay level of DQS on the lower lane */
#define SYSON_FLASH_PHY_DQSL_DLY_SEL(x)             ((u32)(((x) & 0x000000FF) << 8))
#define SYSON_GET_FLASH_PHY_DQSL_DLY_SEL(x)         ((u32)(((x >> 8) & 0x000000FF)))
#define SYSON_MASK_FLASH_PHY_OUTPUT_DLY_SEL         ((u32)0x000000FF << 0)                   /*!< R/W     0 select delay level of sclk delay line  */
#define SYSON_FLASH_PHY_OUTPUT_DLY_SEL(x)           ((u32)(((x) & 0x000000FF) << 0))
#define SYSON_GET_FLASH_PHY_OUTPUT_DLY_SEL(x)       ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_SYS_FLASH_PHY_CTRL2
 * @brief
 * @{
 */
#define SYSON_BIT_DPHY_EN                           ((u32)0x00000001 << 5)                   /*!< R/W   0   None */
#define SYSON_MASK_DPHY_RD_DELAY                    ((u32)0x00000007 << 2)                   /*!< R/W   0   None */
#define SYSON_DPHY_RD_DELAY(x)                      ((u32)(((x) & 0x00000007) << 2))
#define SYSON_GET_DPHY_RD_DELAY(x)                  ((u32)(((x >> 2) & 0x00000007)))
#define SYSON_MASK_DPHY_RD_DELAY_FACTOR             ((u32)0x00000003 << 0)                   /*!< R/W   0   None */
#define SYSON_DPHY_RD_DELAY_FACTOR(x)               ((u32)(((x) & 0x00000003) << 0))
#define SYSON_GET_DPHY_RD_DELAY_FACTOR(x)           ((u32)(((x >> 0) & 0x00000003)))
/** @} */


/** @defgroup REG_SPIC_BPGM_CTR0
 * @brief
 * @{
 */
#define SYSON_MASK_SPIC_BPGM_FIFO_WPTR_INIT         ((u32)0x0000001F << 1)                   /*!< R/W   0x0 FIFO write pointer initial value */
#define SYSON_SPIC_BPGM_FIFO_WPTR_INIT(x)           ((u32)(((x) & 0x0000001F) << 1))
#define SYSON_GET_SPIC_BPGM_FIFO_WPTR_INIT(x)       ((u32)(((x >> 1) & 0x0000001F)))
#define SYSON_BIT_SPIC_BPGM_SPI_EN                  ((u32)0x00000001 << 0)                   /*!< R/W     0 Enable SPIC BPGM flow */
/** @} */


/** @defgroup REG_SPIC_BPGM_CTR1
 * @brief
 * @{
 */
#define SYSON_MASK_SPIC_BPGM_FIFO_INIT_0            ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 First 32 bits of FIFO initial value */
#define SYSON_SPIC_BPGM_FIFO_INIT_0(x)              ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SPIC_BPGM_FIFO_INIT_0(x)          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SPIC_BPGM_CTR2
 * @brief
 * @{
 */
#define SYSON_MASK_SPIC_BPGM_FIFO_INIT_1            ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 First 32 bits of FIFO initial value */
#define SYSON_SPIC_BPGM_FIFO_INIT_1(x)              ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SPIC_BPGM_FIFO_INIT_1(x)          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SPIC_BPGM_CTR3
 * @brief
 * @{
 */
#define SYSON_MASK_SPIC_BPGM_FIFO_INIT_2            ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 First 32 bits of FIFO initial value */
#define SYSON_SPIC_BPGM_FIFO_INIT_2(x)              ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SPIC_BPGM_FIFO_INIT_2(x)          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SPIC_BPGM_CTR4
 * @brief
 * @{
 */
#define SYSON_MASK_SPIC_BPGM_FIFO_INIT_3            ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 First 32 bits of FIFO initial value */
#define SYSON_SPIC_BPGM_FIFO_INIT_3(x)              ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SPIC_BPGM_FIFO_INIT_3(x)          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SPIC_BPGM_CTR5
 * @brief
 * @{
 */
#define SYSON_MASK_SPIC_BPGM_FIFO_INIT_4            ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 First 32 bits of FIFO initial value */
#define SYSON_SPIC_BPGM_FIFO_INIT_4(x)              ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SPIC_BPGM_FIFO_INIT_4(x)          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SPIC_BPGM_CTR6
 * @brief
 * @{
 */
#define SYSON_MASK_SPIC_BPGM_FIFO_INIT_5            ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 First 32 bits of FIFO initial value */
#define SYSON_SPIC_BPGM_FIFO_INIT_5(x)              ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SPIC_BPGM_FIFO_INIT_5(x)          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SPIC_BPGM_CTR7
 * @brief
 * @{
 */
#define SYSON_MASK_SPIC_BPGM_FIFO_INIT_6            ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 First 32 bits of FIFO initial value */
#define SYSON_SPIC_BPGM_FIFO_INIT_6(x)              ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SPIC_BPGM_FIFO_INIT_6(x)          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SPIC_BPGM_CTR8
 * @brief
 * @{
 */
#define SYSON_MASK_SPIC_BPGM_FIFO_INIT_7            ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 First 32 bits of FIFO initial value */
#define SYSON_SPIC_BPGM_FIFO_INIT_7(x)              ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SPIC_BPGM_FIFO_INIT_7(x)          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PON_DUMMY7
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY0_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE               ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 Entry0[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE(x)                 ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE(x)             ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY0_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_194           ((u32)0x0000000F << 0)                   /*!< R/W     0 Entry0[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_194(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_194(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY1_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_198           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY1[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_198(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_198(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY1_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_19C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY1[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_19C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_19C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY2_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1A0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY2[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1A0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1A0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY2_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1A4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY2[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1A4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1A4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY3_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1A8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY3[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1A8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1A8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY3_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1AC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY3[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1AC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1AC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY4_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1B0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY4[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1B0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1B0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY4_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1B4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY4[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1B4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1B4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY5_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1B8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY5[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1B8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1B8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY5_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1BC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY5[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1BC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1BC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY6_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1C0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY6[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1C0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1C0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY6_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1C4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY6[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1C4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1C4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY7_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1C8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY7[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1C8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1C8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY7_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1CC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY7[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1CC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1CC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY8_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1D0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY8[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1D0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1D0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY8_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1D4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY8[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1D4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1D4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY9_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1D8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY9[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1D8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1D8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY9_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1DC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY9[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1DC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1DC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY10_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1E0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY10[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1E0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1E0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY10_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1E4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY10[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1E4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1E4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY11_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1E8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY11[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1E8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1E8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY11_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1EC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY11[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1EC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1EC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY12_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1F0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY12[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1F0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1F0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY12_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1F4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY12[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1F4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1F4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY13_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1F8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY13[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1F8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1F8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY13_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_1FC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY13[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_1FC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_1FC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY14_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_200           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY14[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_200(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_200(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY14_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_204           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY14[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_204(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_204(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY15_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_208           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY15[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_208(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_208(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY15_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_20C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY15[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_20C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_20C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY16_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_210           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY16[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_210(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_210(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY16_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_214           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY16[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_214(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_214(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY17_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_218           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY17[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_218(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_218(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY17_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_21C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY17[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_21C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_21C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY18_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_220           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY18[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_220(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_220(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY18_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_224           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY18[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_224(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_224(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY19_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_228           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY19[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_228(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_228(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY19_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_22C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY19[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_22C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_22C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY20_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_230           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY20[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_230(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_230(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY20_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_234           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY20[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_234(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_234(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY21_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_238           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY21[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_238(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_238(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY21_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_23C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY21[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_23C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_23C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY22_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_240           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY22[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_240(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_240(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY22_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_244           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY22[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_244(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_244(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY23_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_248           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY23[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_248(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_248(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY23_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_24C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY23[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_24C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_24C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY24_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_250           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY24[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_250(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_250(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY24_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_254           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY24[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_254(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_254(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY25_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_258           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY25[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_258(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_258(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY25_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_25C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY25[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_25C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_25C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY26_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_260           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY26[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_260(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_260(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY26_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_264           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY26[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_264(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_264(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY27_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_268           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY27[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_268(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_268(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY27_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_26C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY27[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_26C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_26C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY28_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_270           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY28[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_270(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_270(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY28_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_274           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY28[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_274(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_274(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY29_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_278           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY29[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_278(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_278(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY29_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_27C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY29[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_27C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_27C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY30_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_280           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY30[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_280(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_280(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY30_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_284           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY30[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_284(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_284(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY31_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_288           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY31[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_288(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_288(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY31_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_28C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY31[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_28C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_28C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY32_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_290           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY32[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_290(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_290(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY32_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_294           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY32[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_294(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_294(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY33_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_298           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY33[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_298(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_298(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY33_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_29C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY33[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_29C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_29C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY34_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_2A0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY34[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_2A0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_2A0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY34_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_2A4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY34[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_2A4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_2A4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY35_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_2A8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY35[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_2A8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_2A8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY35_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_2AC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY35[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_2AC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_2AC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY36_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_2B0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY36[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_2B0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_2B0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY36_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_2B4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY36[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_2B4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_2B4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY37_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_2B8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY37[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_2B8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_2B8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY37_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_2BC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY37[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_2BC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_2BC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY38_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_2C0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY38[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_2C0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_2C0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY38_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_2C4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY38[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_2C4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_2C4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY39_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_2C8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY39[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_2C8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_2C8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY39_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_2CC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY39[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_2CC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_2CC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY40_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_2D0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY40[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_2D0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_2D0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY40_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_2D4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY40[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_2D4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_2D4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY41_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_2D8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY41[31:0] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_2D8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_2D8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_ENTRY41_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS1PLL_CTRL_VALUE_2DC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY41[35:32] value when channel number = 1 */
#define SYSON_SYS1PLL_CTRL_VALUE_2DC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS1PLL_CTRL_VALUE_2DC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_CTRL0
 * @brief
 * @{
 */
#define SYSON_MASK_CNT1                             ((u32)0x0000FFFF << 16)                  /*!< R/W     0 None */
#define SYSON_CNT1(x)                               ((u32)(((x) & 0x0000FFFF) << 16))
#define SYSON_GET_CNT1(x)                           ((u32)(((x >> 16) & 0x0000FFFF)))
#define SYSON_BIT_SYS1PLL_TRACKING_EN               ((u32)0x00000001 << 0)                   /*!< R/W     0 Enable PLL tracking function */
/** @} */


/** @defgroup REG_SYS1PLL_TRACKING_CTRL1
 * @brief
 * @{
 */
#define SYSON_MASK_CNT2                             ((u32)0x0000FFFF << 16)                  /*!< R/W     0 None */
#define SYSON_CNT2(x)                               ((u32)(((x) & 0x0000FFFF) << 16))
#define SYSON_GET_CNT2(x)                           ((u32)(((x >> 16) & 0x0000FFFF)))
#define SYSON_MASK_CNT3                             ((u32)0x0000FFFF << 0)                   /*!< R/W     0 None */
#define SYSON_CNT3(x)                               ((u32)(((x) & 0x0000FFFF) << 0))
#define SYSON_GET_CNT3(x)                           ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY0_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE               ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 Entry0[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE(x)                 ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE(x)             ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY0_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_304           ((u32)0x0000000F << 0)                   /*!< R/W     0 Entry0[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_304(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_304(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY1_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_308           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY1[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_308(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_308(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY1_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_30C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY1[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_30C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_30C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY2_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_310           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY2[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_310(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_310(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY2_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_314           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY2[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_314(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_314(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY3_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_318           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY3[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_318(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_318(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY3_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_31C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY3[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_31C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_31C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY4_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_320           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY4[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_320(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_320(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY4_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_324           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY4[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_324(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_324(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY5_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_328           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY5[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_328(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_328(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY5_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_32C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY5[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_32C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_32C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY6_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_330           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY6[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_330(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_330(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY6_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_334           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY6[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_334(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_334(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY7_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_338           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY7[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_338(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_338(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY7_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_33C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY7[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_33C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_33C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY8_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_340           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY8[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_340(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_340(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY8_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_344           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY8[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_344(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_344(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY9_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_348           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY9[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_348(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_348(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY9_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_34C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY9[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_34C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_34C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY10_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_350           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY10[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_350(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_350(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY10_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_354           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY10[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_354(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_354(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY11_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_358           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY11[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_358(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_358(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY11_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_35C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY11[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_35C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_35C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY12_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_360           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY12[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_360(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_360(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY12_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_364           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY12[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_364(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_364(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY13_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_368           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY13[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_368(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_368(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY13_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_36C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY13[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_36C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_36C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY14_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_370           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY14[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_370(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_370(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY14_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_374           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY14[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_374(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_374(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY15_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_378           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY15[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_378(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_378(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY15_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_37C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY15[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_37C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_37C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY16_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_380           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY16[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_380(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_380(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY16_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_384           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY16[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_384(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_384(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY17_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_388           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY17[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_388(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_388(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY17_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_38C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY17[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_38C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_38C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY18_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_390           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY18[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_390(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_390(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY18_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_394           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY18[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_394(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_394(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY19_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_398           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY19[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_398(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_398(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY19_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_39C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY19[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_39C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_39C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY20_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3A0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY20[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3A0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3A0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY20_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3A4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY20[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3A4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3A4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY21_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3A8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY21[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3A8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3A8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY21_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3AC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY21[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3AC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3AC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY22_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3B0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY22[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3B0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3B0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY22_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3B4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY22[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3B4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3B4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY23_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3B8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY23[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3B8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3B8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY23_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3BC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY23[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3BC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3BC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY24_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3C0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY24[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3C0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3C0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY24_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3C4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY24[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3C4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3C4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY25_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3C8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY25[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3C8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3C8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY25_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3CC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY25[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3CC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3CC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY26_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3D0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY26[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3D0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3D0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY26_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3D4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY26[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3D4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3D4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY27_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3D8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY27[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3D8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3D8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY27_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3DC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY27[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3DC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3DC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY28_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3E0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY28[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3E0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3E0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY28_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3E4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY28[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3E4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3E4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY29_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3E8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY29[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3E8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3E8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY29_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3EC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY29[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3EC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3EC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY30_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3F0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY30[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3F0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3F0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY30_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3F4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY30[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3F4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3F4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY31_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3F8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY31[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3F8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3F8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY31_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_3FC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY31[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_3FC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_3FC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY32_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_400           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY32[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_400(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_400(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY32_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_404           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY32[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_404(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_404(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY33_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_408           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY33[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_408(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_408(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY33_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_40C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY33[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_40C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_40C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY34_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_410           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY34[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_410(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_410(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY34_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_414           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY34[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_414(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_414(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY35_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_418           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY35[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_418(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_418(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY35_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_41C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY35[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_41C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_41C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY36_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_420           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY36[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_420(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_420(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY36_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_424           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY36[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_424(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_424(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY37_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_428           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY37[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_428(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_428(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY37_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_42C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY37[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_42C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_42C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY38_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_430           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY38[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_430(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_430(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY38_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_434           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY38[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_434(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_434(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY39_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_438           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY39[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_438(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_438(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY39_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_43C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY39[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_43C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_43C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY40_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_440           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY40[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_440(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_440(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY40_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_444           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY40[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_444(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_444(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY41_0
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_448           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY41[31:0] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_448(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_448(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_ENTRY41_1
 * @brief
 * @{
 */
#define SYSON_MASK_SYS2PLL_CTRL_VALUE_44C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY41[35:32] value when channel number = 1 */
#define SYSON_SYS2PLL_CTRL_VALUE_44C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_SYS2PLL_CTRL_VALUE_44C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_CTRL0
 * @brief
 * @{
 */
#define SYSON_MASK_CNT1_450                         ((u32)0x0000FFFF << 16)                  /*!< R/W     0 None */
#define SYSON_CNT1_450(x)                           ((u32)(((x) & 0x0000FFFF) << 16))
#define SYSON_GET_CNT1_450(x)                       ((u32)(((x >> 16) & 0x0000FFFF)))
#define SYSON_BIT_SYS2PLL_TRACKING_EN               ((u32)0x00000001 << 0)                   /*!< R/W     0 Enable PLL tracking function */
/** @} */


/** @defgroup REG_SYS2PLL_TRACKING_CTRL1
 * @brief
 * @{
 */
#define SYSON_MASK_CNT2_454                         ((u32)0x0000FFFF << 16)                  /*!< R/W     0 None */
#define SYSON_CNT2_454(x)                           ((u32)(((x) & 0x0000FFFF) << 16))
#define SYSON_GET_CNT2_454(x)                       ((u32)(((x >> 16) & 0x0000FFFF)))
#define SYSON_MASK_CNT3_454                         ((u32)0x0000FFFF << 0)                   /*!< R/W     0 None */
#define SYSON_CNT3_454(x)                           ((u32)(((x) & 0x0000FFFF) << 0))
#define SYSON_GET_CNT3_454(x)                       ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY0_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE               ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 Entry0[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE(x)                 ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE(x)             ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY0_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_464           ((u32)0x0000000F << 0)                   /*!< R/W     0 Entry0[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_464(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_464(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY1_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_468           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY1[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_468(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_468(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY1_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_46C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY1[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_46C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_46C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY2_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_470           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY2[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_470(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_470(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY2_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_474           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY2[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_474(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_474(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY3_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_478           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY3[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_478(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_478(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY3_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_47C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY3[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_47C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_47C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY4_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_480           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY4[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_480(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_480(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY4_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_484           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY4[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_484(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_484(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY5_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_488           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY5[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_488(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_488(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY5_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_48C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY5[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_48C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_48C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY6_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_490           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY6[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_490(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_490(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY6_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_494           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY6[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_494(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_494(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY7_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_498           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY7[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_498(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_498(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY7_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_49C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY7[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_49C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_49C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY8_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4A0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY8[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4A0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4A0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY8_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4A4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY8[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4A4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4A4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY9_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4A8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY9[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4A8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4A8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY9_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4AC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY9[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4AC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4AC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY10_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4B0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY10[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4B0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4B0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY10_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4B4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY10[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4B4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4B4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY11_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4B8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY11[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4B8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4B8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY11_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4BC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY11[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4BC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4BC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY12_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4C0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY12[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4C0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4C0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY12_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4C4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY12[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4C4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4C4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY13_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4C8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY13[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4C8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4C8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY13_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4CC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY13[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4CC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4CC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY14_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4D0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY14[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4D0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4D0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY14_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4D4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY14[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4D4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4D4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY15_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4D8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY15[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4D8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4D8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY15_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4DC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY15[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4DC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4DC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY16_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4E0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY16[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4E0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4E0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY16_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4E4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY16[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4E4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4E4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY17_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4E8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY17[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4E8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4E8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY17_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4EC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY17[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4EC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4EC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY18_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4F0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY18[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4F0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4F0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY18_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4F4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY18[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4F4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4F4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY19_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4F8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY19[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4F8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4F8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY19_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_4FC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY19[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_4FC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_4FC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY20_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_500           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY20[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_500(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_500(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY20_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_504           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY20[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_504(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_504(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY21_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_508           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY21[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_508(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_508(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY21_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_50C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY21[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_50C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_50C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY22_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_510           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY22[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_510(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_510(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY22_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_514           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY22[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_514(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_514(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY23_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_518           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY23[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_518(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_518(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY23_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_51C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY23[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_51C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_51C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY24_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_520           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY24[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_520(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_520(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY24_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_524           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY24[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_524(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_524(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY25_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_528           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY25[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_528(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_528(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY25_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_52C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY25[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_52C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_52C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY26_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_530           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY26[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_530(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_530(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY26_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_534           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY26[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_534(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_534(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY27_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_538           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY27[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_538(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_538(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY27_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_53C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY27[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_53C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_53C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY28_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_540           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY28[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_540(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_540(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY28_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_544           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY28[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_544(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_544(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY29_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_548           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY29[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_548(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_548(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY29_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_54C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY29[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_54C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_54C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY30_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_550           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY30[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_550(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_550(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY30_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_554           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY30[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_554(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_554(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY31_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_558           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY31[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_558(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_558(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY31_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_55C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY31[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_55C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_55C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY32_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_560           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY32[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_560(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_560(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY32_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_564           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY32[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_564(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_564(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY33_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_568           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY33[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_568(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_568(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY33_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_56C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY33[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_56C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_56C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY34_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_570           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY34[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_570(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_570(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY34_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_574           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY34[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_574(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_574(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY35_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_578           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY35[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_578(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_578(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY35_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_57C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY35[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_57C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_57C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY36_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_580           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY36[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_580(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_580(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY36_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_584           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY36[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_584(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_584(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY37_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_588           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY37[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_588(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_588(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY37_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_58C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY37[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_58C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_58C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY38_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_590           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY38[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_590(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_590(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY38_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_594           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY38[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_594(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_594(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY39_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_598           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY39[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_598(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_598(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY39_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_59C           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY39[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_59C(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_59C(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY40_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_5A0           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY40[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_5A0(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_5A0(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY40_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_5A4           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY40[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_5A4(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_5A4(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY41_0
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_5A8           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 ENTRY41[31:0] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_5A8(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_5A8(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_ENTRY41_1
 * @brief
 * @{
 */
#define SYSON_MASK_PERIPLL_CTRL_VALUE_5AC           ((u32)0x0000000F << 0)                   /*!< R/W     0 ENTRY41[35:32] value when channel number = 1 */
#define SYSON_PERIPLL_CTRL_VALUE_5AC(x)             ((u32)(((x) & 0x0000000F) << 0))
#define SYSON_GET_PERIPLL_CTRL_VALUE_5AC(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_CTRL0
 * @brief
 * @{
 */
#define SYSON_MASK_CNT1_5B0                         ((u32)0x0000FFFF << 16)                  /*!< R/W     0 None */
#define SYSON_CNT1_5B0(x)                           ((u32)(((x) & 0x0000FFFF) << 16))
#define SYSON_GET_CNT1_5B0(x)                       ((u32)(((x >> 16) & 0x0000FFFF)))
#define SYSON_BIT_PERIPLL_TRACKING_EN               ((u32)0x00000001 << 0)                   /*!< R/W     0 Enable PLL tracking function */
/** @} */


/** @defgroup REG_PERIPLL_TRACKING_CTRL1
 * @brief
 * @{
 */
#define SYSON_MASK_CNT2_5B4                         ((u32)0x0000FFFF << 16)                  /*!< R/W     0 None */
#define SYSON_CNT2_5B4(x)                           ((u32)(((x) & 0x0000FFFF) << 16))
#define SYSON_GET_CNT2_5B4(x)                       ((u32)(((x >> 16) & 0x0000FFFF)))
#define SYSON_MASK_CNT3_5B4                         ((u32)0x0000FFFF << 0)                   /*!< R/W     0 None */
#define SYSON_CNT3_5B4(x)                           ((u32)(((x) & 0x0000FFFF) << 0))
#define SYSON_GET_CNT3_5B4(x)                       ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_MBIST_BISR_CTRL0
 * @brief
 * @{
 */
#define SYSON_BIT_MBISR_KM4_NETWORK_REMAP_RSTN      ((u32)0x00000001 << 31)                  /*!< R/W   0x0 None */
#define SYSON_BIT_MBISR_CA32MP1_LOAD                ((u32)0x00000001 << 15)                  /*!< R/W     1 1: To load the repair info from syson regsiter into mbisr */
#define SYSON_BIT_MBISR_CA32MP1_FAIL                ((u32)0x00000001 << 14)                  /*!< R/W   0x0 1: Repair test result is fail */
#define SYSON_BIT_MBISR_CA32MP1_REPAIRED            ((u32)0x00000001 << 13)                  /*!< R/W   0x0 1: Repair test result is pass or repairable. */
#define SYSON_BIT_MBISR_CA32MP1_DONE                ((u32)0x00000001 << 12)                  /*!< R     0x0 1: Repair test operation is done */
#define SYSON_BIT_MBISR_AIDE_GROUP15_LOAD           ((u32)0x00000001 << 11)                  /*!< R/W     1 1: To load the repair info from syson regsiter into mbisr */
#define SYSON_BIT_MBISR_AIDE_GROUP15_FAIL           ((u32)0x00000001 << 10)                  /*!< R/W   0x0 1: Repair test result is fail */
#define SYSON_BIT_MBISR_AIDE_GROUP15_REPAIRED       ((u32)0x00000001 << 9)                   /*!< R/W   0x0 1: Repair test result is pass or repairable. */
#define SYSON_BIT_MBISR_AIDE_GROUP15_DONE           ((u32)0x00000001 << 8)                   /*!< R     0x0 1: Repair test operation is done */
#define SYSON_BIT_MBISR_AIDE_GROUP14_LOAD           ((u32)0x00000001 << 7)                   /*!< R/W     1 1: To load the repair info from syson regsiter into mbisr */
#define SYSON_BIT_MBISR_AIDE_GROUP14_FAIL           ((u32)0x00000001 << 6)                   /*!< R/W   0x0 1: Repair test result is fail */
#define SYSON_BIT_MBISR_AIDE_GROUP14_REPAIRED       ((u32)0x00000001 << 5)                   /*!< R/W   0x0 1: Repair test result is pass or repairable. */
#define SYSON_BIT_MBISR_AIDE_GROUP14_DONE           ((u32)0x00000001 << 4)                   /*!< R     0x0 1: Repair test operation is done */
#define SYSON_BIT_MBISR_AIDE_GROUP13_LOAD           ((u32)0x00000001 << 3)                   /*!< R/W     1 1: To load the repair info from syson regsiter into mbisr */
#define SYSON_BIT_MBISR_AIDE_GROUP13_FAIL           ((u32)0x00000001 << 2)                   /*!< R/W   0x0 1: Repair test result is fail */
#define SYSON_BIT_MBISR_AIDE_GROUP13_REPAIRED       ((u32)0x00000001 << 1)                   /*!< R/W   0x0 1: Repair test result is pass or repairable. */
#define SYSON_BIT_MBISR_AIDE_GROUP13_DONE           ((u32)0x00000001 << 0)                   /*!< R     0x0 1: Repair test operation is done */
/** @} */


/** @defgroup REG_MBIST_BISR_CTRL1
 * @brief
 * @{
 */
#define SYSON_BIT_MBISR_VIDEO_SRAM_LOAD             ((u32)0x00000001 << 31)                  /*!< R/W     1 1: To load the repair info from syson regsiter into mbisr */
#define SYSON_BIT_MBISR_VIDEO_SRAM_FAIL             ((u32)0x00000001 << 30)                  /*!< R/W   0x0 1: Repair test result is fail */
#define SYSON_BIT_MBISR_VIDEO_SRAM_REPAIRED         ((u32)0x00000001 << 29)                  /*!< R/W   0x0 1: Repair test result is pass or repairable. */
#define SYSON_BIT_MBISR_VIDEO_SRAM_DONE             ((u32)0x00000001 << 28)                  /*!< R     0x0 1: Repair test operation is done */
#define SYSON_BIT_MBISR_KM4_MEDIA_DCACHE_LOAD       ((u32)0x00000001 << 27)                  /*!< R/W     1 1: To load the repair info from syson regsiter into mbisr */
#define SYSON_BIT_MBISR_KM4_MEDIA_DCACHE_FAIL       ((u32)0x00000001 << 26)                  /*!< R/W   0x0 1: Repair test result is fail */
#define SYSON_BIT_MBISR_KM4_MEDIA_DCACHE_REPAIRED   ((u32)0x00000001 << 25)                  /*!< R/W   0x0 1: Repair test result is pass or repairable. */
#define SYSON_BIT_MBISR_KM4_MEDIA_DCACHE_DATA_DONE  ((u32)0x00000001 << 24)                  /*!< R     0x0 1: Repair test operation is done */
#define SYSON_BIT_MBISR_KM4_MEDIA_ICACHE_LOAD       ((u32)0x00000001 << 23)                  /*!< R/W     1 1: To load the repair info from syson regsiter into mbisr */
#define SYSON_BIT_MBISR_KM4_MEDIA_ICACHE_FAIL       ((u32)0x00000001 << 22)                  /*!< R/W   0x0 1: Repair test result is fail */
#define SYSON_BIT_MBISR_KM4_MEDIA_ICACHE_REPAIRED   ((u32)0x00000001 << 21)                  /*!< R/W   0x0 1: Repair test result is pass or repairable. */
#define SYSON_BIT_MBISR_KM4_MEDIA_ICACHE_DATA_DONE  ((u32)0x00000001 << 20)                  /*!< R     0x0 1: Repair test operation is done */
#define SYSON_BIT_MBISR_KM4_NETWORK_DTCM_LOAD       ((u32)0x00000001 << 19)                  /*!< R/W     1 1: To load the repair info from syson regsiter into mbisr */
#define SYSON_BIT_MBISR_KM4_NETWORK_DTCM_FAIL       ((u32)0x00000001 << 18)                  /*!< R/W   0x0 1: Repair test result is fail */
#define SYSON_BIT_MBISR_KM4_NETWORK_DTCM_REPAIRED   ((u32)0x00000001 << 17)                  /*!< R/W   0x0 1: Repair test result is pass or repairable. */
#define SYSON_BIT_MBISR_KM4_NETWORK_DTCM_DATA_DONE  ((u32)0x00000001 << 16)                  /*!< R     0x0 1: Repair test operation is done */
#define SYSON_BIT_MBISR_KM4_NETWORK_ITCM_DATA_LOAD  ((u32)0x00000001 << 15)                  /*!< R/W     1 1: To load the repair info from syson regsiter into mbisr */
#define SYSON_BIT_MBISR_KM4_NETWORK_ITCM_DATA_FAIL  ((u32)0x00000001 << 14)                  /*!< R/W   0x0 1: Repair test result is fail */
#define SYSON_BIT_MBISR_KM4_NETWORK_ITCM_DATA_REPAIRED ((u32)0x00000001 << 13)                  /*!< R/W   0x0 1: Repair test result is pass or repairable. */
#define SYSON_BIT_MBISR_KM4_NETWORK_ITCM_DATA_DONE  ((u32)0x00000001 << 12)                  /*!< R     0x0 1: Repair test operation is done */
#define SYSON_BIT_MBISR_KM4_NETWORK_DCACHE_DATA_LOAD ((u32)0x00000001 << 11)                  /*!< R/W     1 1: To load the repair info from syson regsiter into mbisr */
#define SYSON_BIT_MBISR_KM4_NETWORK_DCACHE_DATA_FAIL ((u32)0x00000001 << 10)                  /*!< R/W   0x0 1: Repair test result is fail */
#define SYSON_BIT_MBISR_KM4_NETWORK_DCACHE_DATA_REPAIRED ((u32)0x00000001 << 9)                   /*!< R/W   0x0 1: Repair test result is pass or repairable. */
#define SYSON_BIT_MBISR_KM4_NETWORK_DCACHE_DATA_DONE ((u32)0x00000001 << 8)                   /*!< R     0x0 1: Repair test operation is done */
#define SYSON_BIT_MBISR_KM4_NETWORK_ICACHE_DATA_LOAD ((u32)0x00000001 << 7)                   /*!< R/W     1 1: To load the repair info from syson regsiter into mbisr */
#define SYSON_BIT_MBISR_KM4_NETWORK_ICACHE_DATA_FAIL ((u32)0x00000001 << 6)                   /*!< R/W   0x0 1: Repair test result is fail */
#define SYSON_BIT_MBISR_KM4_NETWORK_ICACHE_DATA_REPAIRED ((u32)0x00000001 << 5)                   /*!< R/W   0x0 1: Repair test result is pass or repairable. */
#define SYSON_BIT_MBISR_KM4_NETWORK_ICACHE_DATA_DONE ((u32)0x00000001 << 4)                   /*!< R     0x0 1: Repair test operation is done */
#define SYSON_BIT_MBISR_NN_GROUP14_LOAD             ((u32)0x00000001 << 3)                   /*!< R/W     1 1: To load the repair info from syson regsiter into mbisr */
#define SYSON_BIT_MBISR_NN_GROUP14_FAIL             ((u32)0x00000001 << 2)                   /*!< R/W   0x0 1: Repair test result is fail */
#define SYSON_BIT_MBISR_NN_GROUP14_REPAIRED         ((u32)0x00000001 << 1)                   /*!< R/W   0x0 1: Repair test result is pass or repairable. */
#define SYSON_BIT_MBISR_NN_GROUP14_DONE             ((u32)0x00000001 << 0)                   /*!< R     0x0 1: Repair test operation is done */
/** @} */


/** @defgroup REG_MBIST_REMAP_SIG_0
 * @brief
 * @{
 */
#define SYSON_MASK_MBISR_NN_GROUP14_0               ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0x0 mbisr remap_sig of nn_group14 */
#define SYSON_MBISR_NN_GROUP14_0(x)                 ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_MBISR_NN_GROUP14_0(x)             ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_MBIST_REMAP_SIG_1
 * @brief
 * @{
 */
#define SYSON_MASK_MBISR_NN_GROUP14_1               ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0x0 mbisr remap_sig of nn_group14 */
#define SYSON_MBISR_NN_GROUP14_1(x)                 ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_MBISR_NN_GROUP14_1(x)             ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_MBIST_REMAP_SIG_2
 * @brief
 * @{
 */
#define SYSON_MASK_MBISR_VIDEO_SRAM_0               ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0x0 mbisr remap_sig of video_sram */
#define SYSON_MBISR_VIDEO_SRAM_0(x)                 ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_MBISR_VIDEO_SRAM_0(x)             ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_MBIST_REMAP_SIG_3
 * @brief
 * @{
 */
#define SYSON_MASK_MBISR_VIDEO_SRAM_1               ((u32)0x0000FFFF << 0)                   /*!< R/W   0x0 mbisr remap_sig of video_sram */
#define SYSON_MBISR_VIDEO_SRAM_1(x)                 ((u32)(((x) & 0x0000FFFF) << 0))
#define SYSON_GET_MBISR_VIDEO_SRAM_1(x)             ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_MBIST_REMAP_SIG_4
 * @brief
 * @{
 */
#define SYSON_MASK_MBISR_KM4_MEDIA_ICACHE_DATA      ((u32)0x000000FF << 24)                  /*!< R/W   0x0 mbisr_remap_sig of km4_media_icache_data */
#define SYSON_MBISR_KM4_MEDIA_ICACHE_DATA(x)        ((u32)(((x) & 0x000000FF) << 24))
#define SYSON_GET_MBISR_KM4_MEDIA_ICACHE_DATA(x)    ((u32)(((x >> 24) & 0x000000FF)))
#define SYSON_MASK_MBISR_KM4_NETWORK_DTCM_DATA      ((u32)0x0000003F << 18)                  /*!< R/W   0x0 mbisr_remap_sig of km4_network_dtcm_data */
#define SYSON_MBISR_KM4_NETWORK_DTCM_DATA(x)        ((u32)(((x) & 0x0000003F) << 18))
#define SYSON_GET_MBISR_KM4_NETWORK_DTCM_DATA(x)    ((u32)(((x >> 18) & 0x0000003F)))
#define SYSON_MASK_MBISR_KM4_NETWORK_DCACHE_DATA    ((u32)0x000000FF << 8)                   /*!< R/W   0x0 mbisr_remap_sig of km4_network_dcache_data */
#define SYSON_MBISR_KM4_NETWORK_DCACHE_DATA(x)      ((u32)(((x) & 0x000000FF) << 8))
#define SYSON_GET_MBISR_KM4_NETWORK_DCACHE_DATA(x)  ((u32)(((x >> 8) & 0x000000FF)))
#define SYSON_MASK_MBISR_KM4_NETWORK_ICACHE_DATA    ((u32)0x000000FF << 0)                   /*!< R/W   0x0 mbisr_remap_sig of km4_network_icache_data */
#define SYSON_MBISR_KM4_NETWORK_ICACHE_DATA(x)      ((u32)(((x) & 0x000000FF) << 0))
#define SYSON_GET_MBISR_KM4_NETWORK_ICACHE_DATA(x)  ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_MBIST_REMAP_SIG_5
 * @brief
 * @{
 */
#define SYSON_MASK_MBISR_AID_GROUP14                ((u32)0x0000001F << 26)                  /*!< R/W   0x0 mbisr remap_sig of aid_group14 */
#define SYSON_MBISR_AID_GROUP14(x)                  ((u32)(((x) & 0x0000001F) << 26))
#define SYSON_GET_MBISR_AID_GROUP14(x)              ((u32)(((x >> 26) & 0x0000001F)))
#define SYSON_MASK_MBISR_AID_GROUP13                ((u32)0x0003FFFF << 8)                   /*!< R/W   0x0 mbisr remap_sig of aid_group13 */
#define SYSON_MBISR_AID_GROUP13(x)                  ((u32)(((x) & 0x0003FFFF) << 8))
#define SYSON_GET_MBISR_AID_GROUP13(x)              ((u32)(((x >> 8) & 0x0003FFFF)))
#define SYSON_MASK_MBISR_KM4_MEDIA_DCACHE_DATA      ((u32)0x000000FF << 0)                   /*!< R/W   0x0 mbisr_remap_sig of km4_media_dcache_data */
#define SYSON_MBISR_KM4_MEDIA_DCACHE_DATA(x)        ((u32)(((x) & 0x000000FF) << 0))
#define SYSON_GET_MBISR_KM4_MEDIA_DCACHE_DATA(x)    ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_MBIST_REMAP_SIG_6
 * @brief
 * @{
 */
#define SYSON_MASK_MBISR_AIDE_GROUP15               ((u32)0x01FFFFFF << 0)                   /*!< R/W   0x0 mbisr remap_sig of aid_group15 */
#define SYSON_MBISR_AIDE_GROUP15(x)                 ((u32)(((x) & 0x01FFFFFF) << 0))
#define SYSON_GET_MBISR_AIDE_GROUP15(x)             ((u32)(((x >> 0) & 0x01FFFFFF)))
/** @} */


/** @defgroup REG_MBIST_REMAP_SIG_7
 * @brief
 * @{
 */
#define SYSON_MASK_MBISR_NETWORK_ITCM_DATA          ((u32)0x0000003F << 24)                  /*!< R/W   0x0 mbisr_remap_sig of network_itcm_data */
#define SYSON_MBISR_NETWORK_ITCM_DATA(x)            ((u32)(((x) & 0x0000003F) << 24))
#define SYSON_GET_MBISR_NETWORK_ITCM_DATA(x)        ((u32)(((x >> 24) & 0x0000003F)))
#define SYSON_MASK_MBISR_CA32MP1                    ((u32)0x00FFFFFF << 0)                   /*!< R/W   0x0 mbisr remap_sig of ca32mp1 */
#define SYSON_MBISR_CA32MP1(x)                      ((u32)(((x) & 0x00FFFFFF) << 0))
#define SYSON_GET_MBISR_CA32MP1(x)                  ((u32)(((x >> 0) & 0x00FFFFFF)))
/** @} */


/** @defgroup REG_MBIST_REMAP_SIG_8
 * @brief
 * @{
 */
#define SYSON_MASK_MBISR_CA32MP1_5E0                ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0x0 mbisr remap_sig of ca32mp1 */
#define SYSON_MBISR_CA32MP1_5E0(x)                  ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_MBISR_CA32MP1_5E0(x)              ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYSON_SW_BOOT0
 * @brief
 * @{
 */
#define SYSON_MASK_SW                               ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SW Control */
#define SYSON_SW(x)                                 ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SW(x)                             ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYSON_SW_BOOT1
 * @brief
 * @{
 */
#define SYSON_MASK_SW_604                           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SW Control */
#define SYSON_SW_604(x)                             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SW_604(x)                         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYSON_SW_BOOT2
 * @brief
 * @{
 */
#define SYSON_MASK_SW_608                           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SW Control */
#define SYSON_SW_608(x)                             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SW_608(x)                         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYSON_SW_BOOT3
 * @brief
 * @{
 */
#define SYSON_MASK_SW_60C                           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SW Control */
#define SYSON_SW_60C(x)                             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SW_60C(x)                         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYSON_SW_BOOT4
 * @brief
 * @{
 */
#define SYSON_MASK_SW_610                           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SW Control */
#define SYSON_SW_610(x)                             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SW_610(x)                         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYSON_SW_BOOT5
 * @brief
 * @{
 */
#define SYSON_MASK_SW_614                           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SW Control */
#define SYSON_SW_614(x)                             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SW_614(x)                         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYSON_SW_BOOT6
 * @brief
 * @{
 */
#define SYSON_MASK_SW_618                           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SW Control */
#define SYSON_SW_618(x)                             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SW_618(x)                         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYSON_SW_BOOT7
 * @brief
 * @{
 */
#define SYSON_MASK_SW_61C                           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SW Control */
#define SYSON_SW_61C(x)                             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define SYSON_GET_SW_61C(x)                         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS_VERSION_STS
 * @brief
 * @{
 */
#define SYSON_MASK_CHIP_TYPE                        ((u32)0x00000003 << 8)                   /*!< R     0x0
                                                                                              * 00: ASIC
                                                                                              * 01: FPGA
                                                                                              * 10: PXP
                                                                                              * 11: RTL SIM
                                                                                              */
#define SYSON_CHIP_TYPE(x)                          ((u32)(((x) & 0x00000003) << 8))
#define SYSON_GET_CHIP_TYPE(x)                      ((u32)(((x >> 8) & 0x00000003)))
#define SYSON_MASK_MASTER_ID                        ((u32)0x00000007 << 5)                   /*!< R     0x0
                                                                                              * 000: KM4 NP is reading
                                                                                              * 001: CA32 is reading
                                                                                              * 010: KM4MP is reading
                                                                                              * 011: KM0FW is reading
                                                                                              * 100: SIC is reading
                                                                                              */
#define SYSON_MASTER_ID(x)                          ((u32)(((x) & 0x00000007) << 5))
#define SYSON_GET_MASTER_ID(x)                      ((u32)(((x >> 5) & 0x00000007)))
#define SYSON_MASK_CUT_VERSION                      ((u32)0x0000001F << 0)                   /*!< R     0x0 0x0: A-cut */
#define SYSON_CUT_VERSION(x)                        ((u32)(((x) & 0x0000001F) << 0))
#define SYSON_GET_CUT_VERSION(x)                    ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_CPUPLL_CTRL0
 * @brief
 * @{
 */
#define SYSON_MASK_DUMMY                            ((u32)0x0003FFFF << 13)                  /*!< R/W   18'h10000 None */
#define SYSON_DUMMY(x)                              ((u32)(((x) & 0x0003FFFF) << 13))
#define SYSON_GET_DUMMY(x)                          ((u32)(((x >> 13) & 0x0003FFFF)))
#define SYSON_MASK_DIV_SEL                          ((u32)0x000000FF << 5)                   /*!< R/W   8'h0 For SDM debug */
#define SYSON_DIV_SEL(x)                            ((u32)(((x) & 0x000000FF) << 5))
#define SYSON_GET_DIV_SEL(x)                        ((u32)(((x >> 5) & 0x000000FF)))
#define SYSON_MASK_CK_OUT_SEL                       ((u32)0x00000007 << 2)                   /*!< R/W   3'b0
                                                                                              * Output clk divider
                                                                                              * 000=/1
                                                                                              * 100=/4
                                                                                              * 110=/8
                                                                                              * 111=/16
                                                                                              */
#define SYSON_CK_OUT_SEL(x)                         ((u32)(((x) & 0x00000007) << 2))
#define SYSON_GET_CK_OUT_SEL(x)                     ((u32)(((x >> 2) & 0x00000007)))
#define SYSON_BIT_CK_EN_D2                          ((u32)0x00000001 << 1)                   /*!< R/W   1'b1 Enable clk divided by 2 */
#define SYSON_BIT_CK_EN                             ((u32)0x00000001 << 0)                   /*!< R/W   1'b1 Enable clk */
/** @} */


/** @defgroup REG_CPUPLL_CTRL1
 * @brief
 * @{
 */
#define SYSON_BIT_XTAL_SEL                          ((u32)0x00000001 << 29)                  /*!< R/W   1'b0
                                                                                              * Output clk select
                                                                                              * 0= pll output
                                                                                              * 1=xtal
                                                                                              */
#define SYSON_BIT_XTAL_FREQ_SEL                     ((u32)0x00000001 << 28)                  /*!< R/W   1'b0
                                                                                              * 0=XTAL
                                                                                              * 1=XTAL/2
                                                                                              */
#define SYSON_BIT_XTAL_EDGE_SEL                     ((u32)0x00000001 << 27)                  /*!< R/W   1'b0 Xtal edge select */
#define SYSON_BIT_VC_TL                             ((u32)0x00000001 << 26)                  /*!< R/W   1'b0 For debug use */
#define SYSON_BIT_VC_TH                             ((u32)0x00000001 << 25)                  /*!< R/W   1'b0 For debug use */
#define SYSON_MASK_PI_SEL                           ((u32)0x0000001F << 20)                  /*!< R/W   5'b0 For SDM debug */
#define SYSON_PI_SEL(x)                             ((u32)(((x) & 0x0000001F) << 20))
#define SYSON_GET_PI_SEL(x)                         ((u32)(((x >> 20) & 0x0000001F)))
#define SYSON_BIT_PFD_RESET_GATED                   ((u32)0x00000001 << 19)                  /*!< R/W   1'b0
                                                                                              * 0=normal mode
                                                                                              * 1=gated PFD/CP block(for debug use)
                                                                                              */
#define SYSON_MASK_LPF_RS                           ((u32)0x00000007 << 16)                  /*!< R/W   3'b100
                                                                                              * LPF RS parameter
                                                                                              * 000=15k
                                                                                              * 001=13k
                                                                                              * 010=11k
                                                                                              * 011=9k
                                                                                              * 100=7k
                                                                                              * 101=5k
                                                                                              * 110=3k
                                                                                              * 111=1k
                                                                                              */
#define SYSON_LPF_RS(x)                             ((u32)(((x) & 0x00000007) << 16))
#define SYSON_GET_LPF_RS(x)                         ((u32)(((x >> 16) & 0x00000007)))
#define SYSON_MASK_LPF_R3                           ((u32)0x00000007 << 13)                  /*!< R/W   3'b100
                                                                                              * LPF R3 parameter
                                                                                              * 000=1.75k
                                                                                              * 001=1.5k
                                                                                              * 010=1.25k
                                                                                              * 011=1k
                                                                                              * 100=0.75k
                                                                                              * 101=0.5k
                                                                                              * 110=0.25k
                                                                                              * 111=0k
                                                                                              */
#define SYSON_LPF_R3(x)                             ((u32)(((x) & 0x00000007) << 13))
#define SYSON_GET_LPF_R3(x)                         ((u32)(((x >> 13) & 0x00000007)))
#define SYSON_MASK_LPF_CS                           ((u32)0x00000003 << 11)                  /*!< R/W   2'b10
                                                                                              * LPF CS parameter
                                                                                              * 00=30p
                                                                                              * 01=40p
                                                                                              * 10=50p
                                                                                              * 11=60p
                                                                                              */
#define SYSON_LPF_CS(x)                             ((u32)(((x) & 0x00000003) << 11))
#define SYSON_GET_LPF_CS(x)                         ((u32)(((x >> 11) & 0x00000003)))
#define SYSON_MASK_LPF_CP                           ((u32)0x00000003 << 9)                   /*!< R/W   2'b10
                                                                                              * LPF CP parameter
                                                                                              * 00=3p
                                                                                              * 01=4p
                                                                                              * 10=5p
                                                                                              * 11=6p
                                                                                              */
#define SYSON_LPF_CP(x)                             ((u32)(((x) & 0x00000003) << 9))
#define SYSON_GET_LPF_CP(x)                         ((u32)(((x >> 9) & 0x00000003)))
#define SYSON_MASK_LPF_C3                           ((u32)0x00000003 << 7)                   /*!< R/W   2'b10
                                                                                              * LPF C3 parameter
                                                                                              * 00=0.5p
                                                                                              * 01=1p
                                                                                              * 10=1.5p
                                                                                              * 11=2p
                                                                                              */
#define SYSON_LPF_C3(x)                             ((u32)(((x) & 0x00000003) << 7))
#define SYSON_GET_LPF_C3(x)                         ((u32)(((x >> 7) & 0x00000003)))
#define SYSON_MASK_IB_PI                            ((u32)0x00000003 << 5)                   /*!< R/W   2'b0 PI current select */
#define SYSON_IB_PI(x)                              ((u32)(((x) & 0x00000003) << 5))
#define SYSON_GET_IB_PI(x)                          ((u32)(((x >> 5) & 0x00000003)))
#define SYSON_BIT_EN_SSC                            ((u32)0x00000001 << 4)                   /*!< R/W   1'b0 Enable SSC */
#define SYSON_MASK_CP_ICP_SEL                       ((u32)0x00000007 << 1)                   /*!< R/W   3'b0
                                                                                              * Cp current select
                                                                                              * 000=10uA
                                                                                              * 001=15uA,
                                                                                              * 010=20uA,
                                                                                              * 011=25uA,
                                                                                              * 100=30uA,
                                                                                              * 101=35uA,
                                                                                              * 110=40uA,
                                                                                              * 111=45uA
                                                                                              */
#define SYSON_CP_ICP_SEL(x)                         ((u32)(((x) & 0x00000007) << 1))
#define SYSON_GET_CP_ICP_SEL(x)                     ((u32)(((x >> 1) & 0x00000007)))
#define SYSON_BIT_CP_ICPX2                          ((u32)0x00000001 << 0)                   /*!< R/W   1'b0
                                                                                              * Cp current doubler,
                                                                                              * 0=disable,
                                                                                              * 1=enable
                                                                                              */
/** @} */


/** @defgroup REG_CPUPLL_CTRL2
 * @brief
 * @{
 */
#define SYSON_MASK_SDM_F0N                          ((u32)0x0000001F << 19)                  /*!< R/W   5'h0 SDM fractional function */
#define SYSON_SDM_F0N(x)                            ((u32)(((x) & 0x0000001F) << 19))
#define SYSON_GET_SDM_F0N(x)                        ((u32)(((x >> 19) & 0x0000001F)))
#define SYSON_MASK_SDM_F0F                          ((u32)0x000000FF << 11)                  /*!< R/W   8'h0 SDM fraction function */
#define SYSON_SDM_F0F(x)                            ((u32)(((x) & 0x000000FF) << 11))
#define SYSON_GET_SDM_F0F(x)                        ((u32)(((x >> 11) & 0x000000FF)))
#define SYSON_BIT_SDM_EDGE_SEL                      ((u32)0x00000001 << 10)                  /*!< R/W   1'b0 SDM clk edge select */
#define SYSON_MASK_SDM_DIVN                         ((u32)0x000000FF << 2)                   /*!< R/W   8'h1C N= SDM_DIVN_7:0_+2 */
#define SYSON_SDM_DIVN(x)                           ((u32)(((x) & 0x000000FF) << 2))
#define SYSON_GET_SDM_DIVN(x)                       ((u32)(((x >> 2) & 0x000000FF)))
#define SYSON_BIT_SDM_CK_SEL                        ((u32)0x00000001 << 1)                   /*!< R/W   1'b0
                                                                                              * SDM clk select
                                                                                              * 0=ckdiv
                                                                                              * 1=XTAL
                                                                                              */
#define SYSON_BIT_SDM_CK_GATED                      ((u32)0x00000001 << 0)                   /*!< R/W   1'b0
                                                                                              * 0=no gated SDM clk
                                                                                              * 1=gated SDM clk
                                                                                              */
/** @} */


/** @defgroup REG_CPUPLL_CTRL3
 * @brief
 * @{
 */
#define SYSON_BIT_EN_PLL_PWRCUT09                   ((u32)0x00000001 << 31)                  /*!< R/W   0x0 0: power from LDO  1: power from PAD(power cut) */
#define SYSON_BIT_EN_PLL_CCO_PWRCUT09               ((u32)0x00000001 << 30)                  /*!< R/W   0x0
                                                                                              * 0: power from LDO 
                                                                                              * 1: power from PAD(power cut)
                                                                                              */
#define SYSON_MASK_LDO_OUT                          ((u32)0x00000003 << 28)                  /*!< R/W   0x1
                                                                                              * 1.35Vto0.9V LDO Output Selection
                                                                                              * 00 = 0.85V
                                                                                              * 01 = 0.90V
                                                                                              * 10 = 0.95V
                                                                                              * 11 = 1.00V
                                                                                              * 
                                                                                              */
#define SYSON_LDO_OUT(x)                            ((u32)(((x) & 0x00000003) << 28))
#define SYSON_GET_LDO_OUT(x)                        ((u32)(((x >> 28) & 0x00000003)))
#define SYSON_BIT_LDO2PWRCUT                        ((u32)0x00000001 << 27)                  /*!< R/W   0x0 0: LDO 1: power cut */
#define SYSON_BIT_AGPIO_EN                          ((u32)0x00000001 << 26)                  /*!< R/W   0x0 Enable AGPIO */
#define SYSON_BIT_SDM_DEBUG_MODE                    ((u32)0x00000001 << 25)                  /*!< R/W   0x0 For SDM scan mode */
#define SYSON_BIT_SCAN_TEST_MODE                    ((u32)0x00000001 << 24)                  /*!< R/W   0x0 For SDM scan mode */
#define SYSON_BIT_SCAN_CP_RSTN                      ((u32)0x00000001 << 23)                  /*!< R/W   0x0 For SDM scan mode */
#define SYSON_BIT_SCAN_CP_PS                        ((u32)0x00000001 << 22)                  /*!< R/W   0x0 For SDM scan mode */
#define SYSON_BIT_SDM_DELAY_PH                      ((u32)0x00000001 << 21)                  /*!< R/W   0x0 For SDM debug */
#define SYSON_BIT_SDM_ORDER                         ((u32)0x00000001 << 20)                  /*!< R/W   0x0 SDM order */
#define SYSON_MASK_SDM_SSC_TBASE                    ((u32)0x00000FFF << 8)                   /*!< R/W   12'h0 SSC table select */
#define SYSON_SDM_SSC_TBASE(x)                      ((u32)(((x) & 0x00000FFF) << 8))
#define SYSON_GET_SDM_SSC_TBASE(x)                  ((u32)(((x >> 8) & 0x00000FFF)))
#define SYSON_MASK_SDM_SSC_STEP                     ((u32)0x000000FF << 0)                   /*!< R/W   8'h0 SSC step select */
#define SYSON_SDM_SSC_STEP(x)                       ((u32)(((x) & 0x000000FF) << 0))
#define SYSON_GET_SDM_SSC_STEP(x)                   ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_CPUPLL_CTRL4
 * @brief
 * @{
 */
#define SYSON_MASK_DUMMY_710                        ((u32)0x0000FFFF << 16)                  /*!< R/W   0x0 None */
#define SYSON_DUMMY_710(x)                          ((u32)(((x) & 0x0000FFFF) << 16))
#define SYSON_GET_DUMMY_710(x)                      ((u32)(((x >> 16) & 0x0000FFFF)))
#define SYSON_BIT_POW_RSTB                          ((u32)0x00000001 << 6)                   /*!< R/W   0x0 1: Release reset to PLL */
#define SYSON_BIT_VPULSE09_LDO                      ((u32)0x00000001 << 5)                   /*!< R/W   0x0 Enable PLL LDO RC */
#define SYSON_BIT_POW_LDO                           ((u32)0x00000001 << 4)                   /*!< R/W   0x0 Enable PLL LDO */
#define SYSON_BIT_POW_PI                            ((u32)0x00000001 << 3)                   /*!< R/W   0x0 Enable PI function */
#define SYSON_BIT_POW_MBIAS                         ((u32)0x00000001 << 2)                   /*!< R/W   0x0 Enable PLL_MBIAS */
#define SYSON_BIT_POW_PLL                           ((u32)0x00000001 << 1)                   /*!< R/W   0x0 Enable PLL */
#define SYSON_BIT_POW_POWER_CUT                     ((u32)0x00000001 << 0)                   /*!< R/W   0x0 Power cut of PLL */
/** @} */


/** @defgroup REG_PON_SYS1PLL_CTRL0
 * @brief
 * @{
 */
#define SYSON_BIT_POW_ERC_SYS1                      ((u32)0x00000001 << 31)                  /*!< R/W   0   erc enable. 0 -> 1 */
#define SYSON_BIT_POW_DIV_IN_SYS1                   ((u32)0x00000001 << 30)                  /*!< R/W   0   input divider enable  (need pwl signal) 0 ->1 */
#define SYSON_BIT_POW_PLL_SYS1                      ((u32)0x00000001 << 29)                  /*!< R/W   0   (need pwl signal) 0 ->1 */
#define SYSON_MASK_REG_CP_BIA_SYS1_2_0              ((u32)0x00000007 << 26)                  /*!< R/W   0x0
                                                                                              * charge pump current selection . Default=10uA                                                            CP current select :
                                                                                              * 000: Icp=10uA,  001: Icp=15uA,  010: Icp=20uA,  011: Icp=25uA;             100: Icp=30uA,  101: Icp=35uA,  110: Icp=40uA,  111: Icp=45uA;
                                                                                              */
#define SYSON_REG_CP_BIA_SYS1_2_0(x)                ((u32)(((x) & 0x00000007) << 26))
#define SYSON_GET_REG_CP_BIA_SYS1_2_0(x)            ((u32)(((x >> 26) & 0x00000007)))
#define SYSON_BIT_DOGENB_SYS1                       ((u32)0x00000001 << 25)                  /*!< R/W     0 0: watch dog on, 1: watch dog off */
#define SYSON_MASK_REG_PS_SYS1_2_0                  ((u32)0x00000007 << 22)                  /*!< R/W   0x0 None */
#define SYSON_REG_PS_SYS1_2_0(x)                    ((u32)(((x) & 0x00000007) << 22))
#define SYSON_GET_REG_PS_SYS1_2_0(x)                ((u32)(((x >> 22) & 0x00000007)))
#define SYSON_BIT_REG_PSEN_SYS1                     ((u32)0x00000001 << 21)                  /*!< R/W   1   phase selector enable */
#define SYSON_MASK_REG_CP_SET_SYS1_1_0              ((u32)0x00000003 << 19)                  /*!< R/W   0x2
                                                                                              * Cp selection. Default=5p
                                                                                              * 00: Cp=4p,    01: Cp=5p,
                                                                                              * 10: Cp=6p,    11: Cp=7p
                                                                                              * XTAL=40MHz : 10 ,Cp=6p
                                                                                              * XTAL=26MHz : 01 ,Cp=5p
                                                                                              */
#define SYSON_REG_CP_SET_SYS1_1_0(x)                ((u32)(((x) & 0x00000003) << 19))
#define SYSON_GET_REG_CP_SET_SYS1_1_0(x)            ((u32)(((x >> 19) & 0x00000003)))
#define SYSON_MASK_REG_RS_SET_SYS1_2_0              ((u32)0x00000007 << 16)                  /*!< R/W   0x5
                                                                                              * Rs selection. Default=4k                                          
                                                                                              * 000: Rs=15k, 001: Rs=13k, 010: Rs=11k, 011: Rs=9k,             
                                                                                              * 100: Rs=7k,  101: Rs=5k,  110: Rs=3k,  111: Rs=1k;    
                                                                                              * XTAL=40MHz : 101 ,Rs=5K        
                                                                                              */
#define SYSON_REG_RS_SET_SYS1_2_0(x)                ((u32)(((x) & 0x00000007) << 16))
#define SYSON_GET_REG_RS_SET_SYS1_2_0(x)            ((u32)(((x >> 16) & 0x00000007)))
#define SYSON_MASK_REG_CS_SEL_SYS1_1_0              ((u32)0x00000003 << 14)                  /*!< R/W   0x3
                                                                                              * Cs selection. Default=70p
                                                                                              * 00: Cs=40p,   01: Cs=50p,   10: Cs=60p,   11: Cs=70p
                                                                                              * XTAL=40MHz : 11 , Cs=70p
                                                                                              * XTAL=26MHz : 11 , Cs=70p
                                                                                              */
#define SYSON_REG_CS_SEL_SYS1_1_0(x)                ((u32)(((x) & 0x00000003) << 14))
#define SYSON_GET_REG_CS_SEL_SYS1_1_0(x)            ((u32)(((x >> 14) & 0x00000003)))
#define SYSON_BIT_REG_FREF_EDGE_SYS1                ((u32)0x00000001 << 13)                  /*!< R/W     0 0:fref, 1:frefb */
#define SYSON_BIT_REG_CK_EN_SYS1                    ((u32)0x00000001 << 12)                  /*!< R/W     0 output clk enable */
#define SYSON_BIT_REG_CK_EN_D4_SYS1                 ((u32)0x00000001 << 11)                  /*!< R/W   0   output clk div 4 enable */
#define SYSON_BIT_REG_CK_EN_D2_SYS1                 SYSON_BIT_REG_CK_EN_D4_SYS1
#define SYSON_MASK_REG_DOG_SEL_SYS1_2_0             ((u32)0x00000007 << 8)                   /*!< R/W   0x4 bit[8] reserved. Watchdog voltage selection. Default=0.72V. bit[10:9] 00: 0.365V, 01: 0.54V, 10: 0.72V, 11: 0.9V */
#define SYSON_REG_DOG_SEL_SYS1_2_0(x)               ((u32)(((x) & 0x00000007) << 8))
#define SYSON_GET_REG_DOG_SEL_SYS1_2_0(x)           ((u32)(((x >> 8) & 0x00000007)))
#define SYSON_BIT_REG_XTAL_MANUAL_MODE_SYS1         ((u32)0x00000001 << 7)                   /*!< R/W   0x1
                                                                                              * reg_xtal_maunal_mode.  Default=0                                            
                                                                                              * 0: reg_xtal_sel<3:0> is controlled by REG, xtal_div_sel is controlled by mapping table.                      
                                                                                              * 1: reg_xtal_sel<3:0>=4'b1111, xtal_div_sel is controlled by reg_xtal_div_sel_maunal<1:0>
                                                                                              */
#define SYSON_MASK_REG_XTAL_DIV_SEL_MANUAL_SYS1_1_0 ((u32)0x00000003 << 5)                   /*!< R/W   0x0 input divider selection in xtal maunal mode. Default=00  00=/1,  10=/2,  11=/4 */
#define SYSON_REG_XTAL_DIV_SEL_MANUAL_SYS1_1_0(x)   ((u32)(((x) & 0x00000003) << 5))
#define SYSON_GET_REG_XTAL_DIV_SEL_MANUAL_SYS1_1_0(x) ((u32)(((x >> 5) & 0x00000003)))
#define SYSON_MASK_REG_VC_THL_SYS1_1_0              ((u32)0x00000003 << 3)                   /*!< R/W   0x0
                                                                                              * VC voltage, Default=00                                           
                                                                                              * 00: VC=1/4Vdd,  01: VC=1/2Vdd,  10=3/4Vdd,  11: VC=Vdd
                                                                                              */
#define SYSON_REG_VC_THL_SYS1_1_0(x)                ((u32)(((x) & 0x00000003) << 3))
#define SYSON_GET_REG_VC_THL_SYS1_1_0(x)            ((u32)(((x >> 3) & 0x00000003)))
#define SYSON_MASK_C3_SET_SYS1_1_0                  ((u32)0x00000003 << 1)                   /*!< R/W   0x1 C3 selection. Default=1p  00: 0.5p, 01: 1p, 10: Cs=1.5p, 11: 2p */
#define SYSON_C3_SET_SYS1_1_0(x)                    ((u32)(((x) & 0x00000003) << 1))
#define SYSON_GET_C3_SET_SYS1_1_0(x)                ((u32)(((x >> 1) & 0x00000003)))
#define SYSON_BIT_R3_SET_SYS1                       ((u32)0x00000001 << 0)                   /*!< R/W     1 Rs selection. Default=1k  0: 0 Ohm, 1: 1k Ohm */
/** @} */


/** @defgroup REG_PON_SYS1PLL_CTRL1
 * @brief
 * @{
 */
#define SYSON_MASK_REG_XTAL_SEL_SYS1_3_0            ((u32)0x0000000F << 28)                  /*!< R/W   0x0 xtal selection, Default=40MHz  0000= 40MHz, 0101= 26MHz, */
#define SYSON_REG_XTAL_SEL_SYS1_3_0(x)              ((u32)(((x) & 0x0000000F) << 28))
#define SYSON_GET_REG_XTAL_SEL_SYS1_3_0(x)          ((u32)(((x >> 28) & 0x0000000F)))
#define SYSON_MASK_REG_FREQ_SEL_SYS1_3_0            ((u32)0x0000000F << 24)                  /*!< R/W   0x0
                                                                                              * output clk selection, Default=500MHz                            
                                                                                              * 0000= 400MHz,  
                                                                                              * 0001= 300MHz,                
                                                                                              * 0010= X,       
                                                                                              * 0011= X,                    
                                                                                              * 0100= 216MHz,  
                                                                                              * 0101=196.608MHz, 
                                                                                              * 0110=192MHz,   
                                                                                              * 0111= 180.632MHz, 
                                                                                              * 1000= 148.5MHz,
                                                                                              * 1001= X,         
                                                                                              * 1010= X,       
                                                                                              * 1011= X,        
                                                                                              * 1100= X,       
                                                                                              * 1101= X,         
                                                                                              * 1110= X,       
                                                                                              * 1111= 500MHz                                      
                                                                                              */
#define SYSON_REG_FREQ_SEL_SYS1_3_0(x)              ((u32)(((x) & 0x0000000F) << 24))
#define SYSON_GET_REG_FREQ_SEL_SYS1_3_0(x)          ((u32)(((x >> 24) & 0x0000000F)))
#define SYSON_MASK_TBASE_FREQ_SYS1_3_0              ((u32)0x0000000F << 20)                  /*!< R/W   0x8 now is empty */
#define SYSON_TBASE_FREQ_SYS1_3_0(x)                ((u32)(((x) & 0x0000000F) << 20))
#define SYSON_GET_TBASE_FREQ_SYS1_3_0(x)            ((u32)(((x >> 20) & 0x0000000F)))
#define SYSON_MASK_STEP_FREQ_SYS1_3_0               ((u32)0x0000000F << 16)                  /*!< R/W   0x5 step size selection for switching freq.  0000: 2^5  0001: 2^6  0010: 2^7  0011: 2^8  0100: 2^9  0101: 2^10  0110: 2^11  0111: 2^12 */
#define SYSON_STEP_FREQ_SYS1_3_0(x)                 ((u32)(((x) & 0x0000000F) << 16))
#define SYSON_GET_STEP_FREQ_SYS1_3_0(x)             ((u32)(((x >> 16) & 0x0000000F)))
#define SYSON_BIT_TRIG_FREQ_SYS1                    ((u32)0x00000001 << 15)                  /*!< R/W   0   freq. step up or step down enable */
#define SYSON_MASK_BB_DBG_SEL_AFE_SDM_SYS1_3_0      ((u32)0x0000000F << 11)                  /*!< R/W   0x0 None */
#define SYSON_BB_DBG_SEL_AFE_SDM_SYS1_3_0(x)        ((u32)(((x) & 0x0000000F) << 11))
#define SYSON_GET_BB_DBG_SEL_AFE_SDM_SYS1_3_0(x)    ((u32)(((x >> 11) & 0x0000000F)))
#define SYSON_MASK_DIVN_SDM_SYS1_5_0                ((u32)0x0000003F << 5)                   /*!< R/W   0xE Default  320MHz: 000110  600MHz: 001101  660MHz: 001110  700MHz: 001111  For scan-mode ONLY:  600MHz: 001101 */
#define SYSON_DIVN_SDM_SYS1_5_0(x)                  ((u32)(((x) & 0x0000003F) << 5))
#define SYSON_GET_DIVN_SDM_SYS1_5_0(x)              ((u32)(((x >> 5) & 0x0000003F)))
#define SYSON_MASK_REG_CK_OUT_SEL_SYS1_3_0          ((u32)0x0000000F << 1)                   /*!< R/W   0x0 output divider selection. Default=0000(/1)  0000=/1, 1000=/2, 1100=/4, 1110=/8, 1111=/16  300~350MHz PLL output clock, output divider selection. 1000=/2  Ex : 600MHz/2=300MHz  350~700MHz PLL output clock, output divider selection. 0000=/1  Ex : 700MHz/1=700MHz  Spec:/2(mandatory */
#define SYSON_REG_CK_OUT_SEL_SYS1_3_0(x)            ((u32)(((x) & 0x0000000F) << 1))
#define SYSON_GET_REG_CK_OUT_SEL_SYS1_3_0(x)        ((u32)(((x >> 1) & 0x0000000F)))
#define SYSON_BIT_ICP_HALF_SYS1                     ((u32)0x00000001 << 0)                   /*!< R/W   0x1 charge pump current half or not   0: no half    1: half */
/** @} */


/** @defgroup REG_PON_SYS1PLL_CTRL2
 * @brief
 * @{
 */
#define SYSON_MASK_STEP_SYS1_12_0                   ((u32)0x00001FFF << 19)                  /*!< R/W   0x0 ssc step size selection. */
#define SYSON_STEP_SYS1_12_0(x)                     ((u32)(((x) & 0x00001FFF) << 19))
#define SYSON_GET_STEP_SYS1_12_0(x)                 ((u32)(((x >> 19) & 0x00001FFF)))
#define SYSON_MASK_TBASE_SYS1_11_0                  ((u32)0x00000FFF << 7)                   /*!< R/W   0x0 ssc freq selection */
#define SYSON_TBASE_SYS1_11_0(x)                    ((u32)(((x) & 0x00000FFF) << 7))
#define SYSON_GET_TBASE_SYS1_11_0(x)                ((u32)(((x >> 7) & 0x00000FFF)))
#define SYSON_BIT_EN_SSC_SYS1                       ((u32)0x00000001 << 6)                   /*!< R/W   0   ssc enable. Default=0 */
#define SYSON_MASK_RESERVED_SYS1_CTRL2              ((u32)0x0000003F << 0)                   /*!< R/W   0x0 reserved */
#define SYSON_RESERVED_SYS1_CTRL2(x)                ((u32)(((x) & 0x0000003F) << 0))
#define SYSON_GET_RESERVED_SYS1_CTRL2(x)            ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_PON_SYS1PLL_CTRL3
 * @brief
 * @{
 */
#define SYSON_MASK_F0F_SDM_SYS1_12_0                ((u32)0x00001FFF << 19)                  /*!< R/W   0x0 Default 660MHz: 0000000000000  330MHz: 0000000000000  600MHz: 0000000000000  660MHz: 0000000000000 */
#define SYSON_F0F_SDM_SYS1_12_0(x)                  ((u32)(((x) & 0x00001FFF) << 19))
#define SYSON_GET_F0F_SDM_SYS1_12_0(x)              ((u32)(((x >> 19) & 0x00001FFF)))
#define SYSON_MASK_F0N_SDM_SYS1_2_0                 ((u32)0x00000007 << 16)                  /*!< R/W   0x4 Default 660MHz: 100  330MHz: 010  600MHz: 000  660MHz: 100 */
#define SYSON_F0N_SDM_SYS1_2_0(x)                   ((u32)(((x) & 0x00000007) << 16))
#define SYSON_GET_F0N_SDM_SYS1_2_0(x)               ((u32)(((x >> 16) & 0x00000007)))
#define SYSON_BIT_ORDER_SDM_SYS1                    ((u32)0x00000001 << 15)                  /*!< R/W   0   SDM order */
#define SYSON_MASK_RESERVED_SYS1_CTRL3              ((u32)0x00007FFF << 0)                   /*!< R/W   0x0 reserved */
#define SYSON_RESERVED_SYS1_CTRL3(x)                ((u32)(((x) & 0x00007FFF) << 0))
#define SYSON_GET_RESERVED_SYS1_CTRL3(x)            ((u32)(((x >> 0) & 0x00007FFF)))
/** @} */


/** @defgroup REG_PON_SYS1PLL_CTRL4
 * @brief
 * @{
 */
#define SYSON_BIT_DUMMY_724                         ((u32)0x00000001 << 15)                  /*!< R/W     0 None */
#define SYSON_BIT_POW_CKGEN                         ((u32)0x00000001 << 14)                  /*!< R/W     0 pow of ckgen */
#define SYSON_MASK_CKGEN_DIV                        ((u32)0x000001FF << 5)                   /*!< R/W   111010 DIV9B, N=DIV[8:0]+2 */
#define SYSON_CKGEN_DIV(x)                          ((u32)(((x) & 0x000001FF) << 5))
#define SYSON_GET_CKGEN_DIV(x)                      ((u32)(((x >> 5) & 0x000001FF)))
#define SYSON_BIT_EN_CK400_PS                       ((u32)0x00000001 << 4)                   /*!< R/W   0x0 Gating ck400M ps */
#define SYSON_BIT_EN_CK400M                         ((u32)0x00000001 << 3)                   /*!< R/W   0x0 Gating ck400m,ck400m is PLL VCO freq (350M~700M) */
#define SYSON_BIT_EN_CLK400M                        SYSON_BIT_EN_CK400M
#define SYSON_MASK_PHASE_SEL                        ((u32)0x00000007 << 0)                   /*!< R/W   0x0 choose delay phase (n/8 period) for ck400m_ps */
#define SYSON_PHASE_SEL(x)                          ((u32)(((x) & 0x00000007) << 0))
#define SYSON_GET_PHASE_SEL(x)                      ((u32)(((x >> 0) & 0x00000007)))
/** @} */


/** @defgroup REG_PON_SYS2PLL_CTRL0
 * @brief
 * @{
 */
#define SYSON_BIT_POW_ERC_SYS2                      ((u32)0x00000001 << 31)                  /*!< R/W   0   erc enable. 0 -> 1 */
#define SYSON_BIT_POW_DIV_IN_SYS2                   ((u32)0x00000001 << 30)                  /*!< R/W   0   input divider enable  (need pwl signal) 0 ->1 */
#define SYSON_BIT_POW_PLL_SYS2                      ((u32)0x00000001 << 29)                  /*!< R/W   0   (need pwl signal) 0 ->1 */
#define SYSON_MASK_REG_CP_BIA_SYS2_2_0              ((u32)0x00000007 << 26)                  /*!< R/W   0x0
                                                                                              * charge pump current selection . Default=10uA                                                            CP current select :
                                                                                              * 000: Icp=10uA,  001: Icp=15uA,  010: Icp=20uA,  011: Icp=25uA;             100: Icp=30uA,  101: Icp=35uA,  110: Icp=40uA,  111: Icp=45uA;
                                                                                              */
#define SYSON_REG_CP_BIA_SYS2_2_0(x)                ((u32)(((x) & 0x00000007) << 26))
#define SYSON_GET_REG_CP_BIA_SYS2_2_0(x)            ((u32)(((x >> 26) & 0x00000007)))
#define SYSON_BIT_DOGENB_SYS2                       ((u32)0x00000001 << 25)                  /*!< R/W     0 0: watch dog on, 1: watch dog off */
#define SYSON_MASK_REG_PS_SYS2_2_0                  ((u32)0x00000007 << 22)                  /*!< R/W   0x0 None */
#define SYSON_REG_PS_SYS2_2_0(x)                    ((u32)(((x) & 0x00000007) << 22))
#define SYSON_GET_REG_PS_SYS2_2_0(x)                ((u32)(((x >> 22) & 0x00000007)))
#define SYSON_BIT_REG_PSEN_SYS2                     ((u32)0x00000001 << 21)                  /*!< R/W   1   phase selector enable */
#define SYSON_MASK_REG_CP_SET_SYS2_1_0              ((u32)0x00000003 << 19)                  /*!< R/W   0x2
                                                                                              * Cp selection. Default=5p
                                                                                              * 00: Cp=4p,    01: Cp=5p,
                                                                                              * 10: Cp=6p,    11: Cp=7p
                                                                                              * XTAL=40MHz : 10 ,Cp=6p
                                                                                              * XTAL=26MHz : 01 ,Cp=5p
                                                                                              */
#define SYSON_REG_CP_SET_SYS2_1_0(x)                ((u32)(((x) & 0x00000003) << 19))
#define SYSON_GET_REG_CP_SET_SYS2_1_0(x)            ((u32)(((x >> 19) & 0x00000003)))
#define SYSON_MASK_REG_RS_SET_SYS2_2_0              ((u32)0x00000007 << 16)                  /*!< R/W   0x5
                                                                                              * Rs selection. Default=4k                                          
                                                                                              * 000: Rs=15k, 001: Rs=13k, 010: Rs=11k, 011: Rs=9k,             
                                                                                              * 100: Rs=7k,  101: Rs=5k,  110: Rs=3k,  111: Rs=1k;    
                                                                                              * XTAL=40MHz : 101 ,Rs=5K        
                                                                                              */
#define SYSON_REG_RS_SET_SYS2_2_0(x)                ((u32)(((x) & 0x00000007) << 16))
#define SYSON_GET_REG_RS_SET_SYS2_2_0(x)            ((u32)(((x >> 16) & 0x00000007)))
#define SYSON_MASK_REG_CS_SEL_SYS2_1_0              ((u32)0x00000003 << 14)                  /*!< R/W   0x3
                                                                                              * Cs selection. Default=70p
                                                                                              * 00: Cs=40p,   01: Cs=50p,   10: Cs=60p,   11: Cs=70p
                                                                                              * XTAL=40MHz : 11 , Cs=70p
                                                                                              * XTAL=26MHz : 11 , Cs=70p
                                                                                              */
#define SYSON_REG_CS_SEL_SYS2_1_0(x)                ((u32)(((x) & 0x00000003) << 14))
#define SYSON_GET_REG_CS_SEL_SYS2_1_0(x)            ((u32)(((x >> 14) & 0x00000003)))
#define SYSON_BIT_REG_FREF_EDGE_SYS2                ((u32)0x00000001 << 13)                  /*!< R/W     0 0:fref, 1:frefb */
#define SYSON_BIT_REG_CK_EN_SYS2                    ((u32)0x00000001 << 12)                  /*!< R/W     0 output clk enable */
#define SYSON_BIT_REG_CK_EN_D4_SYS2                 ((u32)0x00000001 << 11)                  /*!< R/W   0   output clk div 4 enable */
#define SYSON_BIT_REG_CK_EN_D2_SYS2                 SYSON_BIT_REG_CK_EN_D4_SYS2
#define SYSON_MASK_REG_DOG_SEL_SYS2_2_0             ((u32)0x00000007 << 8)                   /*!< R/W   0x4 bit[8] reserved. Watchdog voltage selection. Default=0.72V. bit[10:9] 00: 0.365V, 01: 0.54V, 10: 0.72V, 11: 0.9V */
#define SYSON_REG_DOG_SEL_SYS2_2_0(x)               ((u32)(((x) & 0x00000007) << 8))
#define SYSON_GET_REG_DOG_SEL_SYS2_2_0(x)           ((u32)(((x >> 8) & 0x00000007)))
#define SYSON_BIT_REG_XTAL_MANUAL_MODE_SYS2         ((u32)0x00000001 << 7)                   /*!< R/W   0x1
                                                                                              * reg_xtal_maunal_mode.  Default=0                                            
                                                                                              * 0: reg_xtal_sel<3:0> is controlled by REG, xtal_div_sel is controlled by mapping table.                      
                                                                                              * 1: reg_xtal_sel<3:0>=4'b1111, xtal_div_sel is controlled by reg_xtal_div_sel_maunal<1:0>
                                                                                              */
#define SYSON_MASK_REG_XTAL_DIV_SEL_MANUAL_SYS2_1_0 ((u32)0x00000003 << 5)                   /*!< R/W   0x0 input divider selection in xtal maunal mode. Default=00  00=/1,  10=/2,  11=/4 */
#define SYSON_REG_XTAL_DIV_SEL_MANUAL_SYS2_1_0(x)   ((u32)(((x) & 0x00000003) << 5))
#define SYSON_GET_REG_XTAL_DIV_SEL_MANUAL_SYS2_1_0(x) ((u32)(((x >> 5) & 0x00000003)))
#define SYSON_MASK_REG_VC_THL_SYS2_1_0              ((u32)0x00000003 << 3)                   /*!< R/W   0x0
                                                                                              * VC voltage, Default=00                                           
                                                                                              * 00: VC=1/4Vdd,  01: VC=1/2Vdd,  10=3/4Vdd,  11: VC=Vdd
                                                                                              */
#define SYSON_REG_VC_THL_SYS2_1_0(x)                ((u32)(((x) & 0x00000003) << 3))
#define SYSON_GET_REG_VC_THL_SYS2_1_0(x)            ((u32)(((x >> 3) & 0x00000003)))
#define SYSON_MASK_C3_SET_SYS2_1_0                  ((u32)0x00000003 << 1)                   /*!< R/W   0x1 C3 selection. Default=1p  00: 0.5p, 01: 1p, 10: Cs=1.5p, 11: 2p */
#define SYSON_C3_SET_SYS2_1_0(x)                    ((u32)(((x) & 0x00000003) << 1))
#define SYSON_GET_C3_SET_SYS2_1_0(x)                ((u32)(((x >> 1) & 0x00000003)))
#define SYSON_BIT_R3_SET_SYS2                       ((u32)0x00000001 << 0)                   /*!< R/W     1 Rs selection. Default=1k  0: 0 Ohm, 1: 1k Ohm */
/** @} */


/** @defgroup REG_PON_SYS2PLL_CTRL1
 * @brief
 * @{
 */
#define SYSON_MASK_REG_XTAL_SEL_SYS2_3_0            ((u32)0x0000000F << 28)                  /*!< R/W   0x0 xtal selection, Default=40MHz  0000= 40MHz, 0101= 26MHz, */
#define SYSON_REG_XTAL_SEL_SYS2_3_0(x)              ((u32)(((x) & 0x0000000F) << 28))
#define SYSON_GET_REG_XTAL_SEL_SYS2_3_0(x)          ((u32)(((x >> 28) & 0x0000000F)))
#define SYSON_MASK_REG_FREQ_SEL_SYS2_3_0            ((u32)0x0000000F << 24)                  /*!< R/W   0x0
                                                                                              * output clk selection, Default=500MHz                            
                                                                                              * 0000= 400MHz,  
                                                                                              * 0001= 300MHz,                
                                                                                              * 0010= X,       
                                                                                              * 0011= X,                    
                                                                                              * 0100= 216MHz,  
                                                                                              * 0101=196.608MHz, 
                                                                                              * 0110=192MHz,   
                                                                                              * 0111= 180.632MHz, 
                                                                                              * 1000= 148.5MHz,
                                                                                              * 1001= X,         
                                                                                              * 1010= X,       
                                                                                              * 1011= X,        
                                                                                              * 1100= X,       
                                                                                              * 1101= X,         
                                                                                              * 1110= X,       
                                                                                              * 1111= 500MHz                                      
                                                                                              */
#define SYSON_REG_FREQ_SEL_SYS2_3_0(x)              ((u32)(((x) & 0x0000000F) << 24))
#define SYSON_GET_REG_FREQ_SEL_SYS2_3_0(x)          ((u32)(((x >> 24) & 0x0000000F)))
#define SYSON_MASK_TBASE_FREQ_SYS2_3_0              ((u32)0x0000000F << 20)                  /*!< R/W   0x8 now is empty */
#define SYSON_TBASE_FREQ_SYS2_3_0(x)                ((u32)(((x) & 0x0000000F) << 20))
#define SYSON_GET_TBASE_FREQ_SYS2_3_0(x)            ((u32)(((x >> 20) & 0x0000000F)))
#define SYSON_MASK_STEP_FREQ_SYS2_3_0               ((u32)0x0000000F << 16)                  /*!< R/W   0x5 step size selection for switching freq.  0000: 2^5  0001: 2^6  0010: 2^7  0011: 2^8  0100: 2^9  0101: 2^10  0110: 2^11  0111: 2^12 */
#define SYSON_STEP_FREQ_SYS2_3_0(x)                 ((u32)(((x) & 0x0000000F) << 16))
#define SYSON_GET_STEP_FREQ_SYS2_3_0(x)             ((u32)(((x >> 16) & 0x0000000F)))
#define SYSON_BIT_TRIG_FREQ_SYS2                    ((u32)0x00000001 << 15)                  /*!< R/W   0   freq. step up or step down enable */
#define SYSON_MASK_BB_DBG_SEL_AFE_SDM_SYS2_3_0      ((u32)0x0000000F << 11)                  /*!< R/W   0x0 None */
#define SYSON_BB_DBG_SEL_AFE_SDM_SYS2_3_0(x)        ((u32)(((x) & 0x0000000F) << 11))
#define SYSON_GET_BB_DBG_SEL_AFE_SDM_SYS2_3_0(x)    ((u32)(((x >> 11) & 0x0000000F)))
#define SYSON_MASK_DIVN_SDM_SYS2_5_0                ((u32)0x0000003F << 5)                   /*!< R/W   0xE Default  320MHz: 000110  600MHz: 001101  660MHz: 001110  700MHz: 001111  For scan-mode ONLY:  640MHz: 001110 */
#define SYSON_DIVN_SDM_SYS2_5_0(x)                  ((u32)(((x) & 0x0000003F) << 5))
#define SYSON_GET_DIVN_SDM_SYS2_5_0(x)              ((u32)(((x >> 5) & 0x0000003F)))
#define SYSON_MASK_REG_CK_OUT_SEL_SYS2_3_0          ((u32)0x0000000F << 1)                   /*!< R/W   0x0 output divider selection. Default=0000(/1)  0000=/1, 1000=/2, 1100=/4, 1110=/8, 1111=/16  300~350MHz PLL output clock, output divider selection. 1000=/2  Ex : 600MHz/2=300MHz  350~700MHz PLL output clock, output divider selection. 0000=/1  Ex : 700MHz/1=700MHz  Spec:/2(mandatory */
#define SYSON_REG_CK_OUT_SEL_SYS2_3_0(x)            ((u32)(((x) & 0x0000000F) << 1))
#define SYSON_GET_REG_CK_OUT_SEL_SYS2_3_0(x)        ((u32)(((x >> 1) & 0x0000000F)))
#define SYSON_BIT_ICP_HALF_SYS2                     ((u32)0x00000001 << 0)                   /*!< R/W   0x1 charge pump current half or not   0: no half    1: half */
/** @} */


/** @defgroup REG_PON_SYS2PLL_CTRL2
 * @brief
 * @{
 */
#define SYSON_MASK_STEP_SYS2_12_0                   ((u32)0x00001FFF << 19)                  /*!< R/W   0x0 ssc step size selection. */
#define SYSON_STEP_SYS2_12_0(x)                     ((u32)(((x) & 0x00001FFF) << 19))
#define SYSON_GET_STEP_SYS2_12_0(x)                 ((u32)(((x >> 19) & 0x00001FFF)))
#define SYSON_MASK_TBASE_SYS2_11_0                  ((u32)0x00000FFF << 7)                   /*!< R/W   0x0 ssc freq selection */
#define SYSON_TBASE_SYS2_11_0(x)                    ((u32)(((x) & 0x00000FFF) << 7))
#define SYSON_GET_TBASE_SYS2_11_0(x)                ((u32)(((x >> 7) & 0x00000FFF)))
#define SYSON_BIT_EN_SSC_SYS2                       ((u32)0x00000001 << 6)                   /*!< R/W   0   ssc enable. Default=0 */
#define SYSON_MASK_RESERVED_SYS2_CTRL2              ((u32)0x0000003F << 0)                   /*!< R/W   0x0 reserved */
#define SYSON_RESERVED_SYS2_CTRL2(x)                ((u32)(((x) & 0x0000003F) << 0))
#define SYSON_GET_RESERVED_SYS2_CTRL2(x)            ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_PON_SYS2PLL_CTRL3
 * @brief
 * @{
 */
#define SYSON_MASK_F0F_SDM_SYS2_12_0                ((u32)0x00001FFF << 19)                  /*!< R/W   0x0 Default 660MHz: 0000000000000  330MHz: 0000000000000  600MHz: 0000000000000  660MHz: 0000000000000 */
#define SYSON_F0F_SDM_SYS2_12_0(x)                  ((u32)(((x) & 0x00001FFF) << 19))
#define SYSON_GET_F0F_SDM_SYS2_12_0(x)              ((u32)(((x >> 19) & 0x00001FFF)))
#define SYSON_MASK_F0N_SDM_SYS2_2_0                 ((u32)0x00000007 << 16)                  /*!< R/W   0x4 Default 660MHz: 100  330MHz: 010  600MHz: 000  660MHz: 100 */
#define SYSON_F0N_SDM_SYS2_2_0(x)                   ((u32)(((x) & 0x00000007) << 16))
#define SYSON_GET_F0N_SDM_SYS2_2_0(x)               ((u32)(((x >> 16) & 0x00000007)))
#define SYSON_BIT_ORDER_SDM_SYS2                    ((u32)0x00000001 << 15)                  /*!< R/W   0   SDM order */
#define SYSON_MASK_RESERVED_SYS2_CTRL3              ((u32)0x00007FFF << 0)                   /*!< R/W   0x0 reserved */
#define SYSON_RESERVED_SYS2_CTRL3(x)                ((u32)(((x) & 0x00007FFF) << 0))
#define SYSON_GET_RESERVED_SYS2_CTRL3(x)            ((u32)(((x >> 0) & 0x00007FFF)))
/** @} */


/** @defgroup REG_PON_SYS2PLL_CTRL4
 * @brief
 * @{
 */
#define SYSON_BIT_RESERVED_738                      ((u32)0x00000001 << 15)                  /*!< R/W     0 Reserved */
#define SYSON_BIT_DUMMY_738                         SYSON_BIT_RESERVED_738
#define SYSON_BIT_POW_CKGEN_738                     ((u32)0x00000001 << 14)                  /*!< R/W     0 pow of ckgen */
#define SYSON_MASK_CKGEN_DIV_738                    ((u32)0x000001FF << 5)                   /*!< R/W   111010 DIV9B, N=DIV[8:0]+2 */
#define SYSON_CKGEN_DIV_738(x)                      ((u32)(((x) & 0x000001FF) << 5))
#define SYSON_GET_CKGEN_DIV_738(x)                  ((u32)(((x >> 5) & 0x000001FF)))
#define SYSON_BIT_EN_CK400_PS_738                   ((u32)0x00000001 << 4)                   /*!< R/W   0x0 Gating ck400M ps */
#define SYSON_BIT_EN_CK400M_738                     ((u32)0x00000001 << 3)                   /*!< R/W   0x0 Gating ck400m,ck400m is PLL VCO freq (350M~700M) */
#define SYSON_BIT_EN_CLK400M_738                    SYSON_BIT_EN_CK400M_738
#define SYSON_MASK_PHASE_SEL_738                    ((u32)0x00000007 << 0)                   /*!< R/W   0x0 choose delay phase (n/8 period) for ck400m_ps */
#define SYSON_PHASE_SEL_738(x)                      ((u32)(((x) & 0x00000007) << 0))
#define SYSON_GET_PHASE_SEL_738(x)                  ((u32)(((x >> 0) & 0x00000007)))
/** @} */


/** @defgroup REG_PON_PERIPLL_CTRL0
 * @brief
 * @{
 */
#define SYSON_BIT_POW_ERC_PERI                      ((u32)0x00000001 << 31)                  /*!< R/W   0   erc enable. */
#define SYSON_BIT_POW_DIV_IN_PERI                   ((u32)0x00000001 << 30)                  /*!< R/W   0   input divider enable  (need pwl signal) 0 ->1 */
#define SYSON_BIT_POW_PLL_PERI                      ((u32)0x00000001 << 29)                  /*!< R/W   0   (need pwl signal) 0 ->1 */
#define SYSON_MASK_REG_CP_BIA_PERI_2_0              ((u32)0x00000007 << 26)                  /*!< R/W   0x0
                                                                                              * charge pump current selection . Default=10uA                                                            CP current select :
                                                                                              * 000: Icp=10uA,  001: Icp=15uA,  010: Icp=20uA,  011: Icp=25uA;             100: Icp=30uA,  101: Icp=35uA,  110: Icp=40uA,  111: Icp=45uA;
                                                                                              */
#define SYSON_REG_CP_BIA_PERI_2_0(x)                ((u32)(((x) & 0x00000007) << 26))
#define SYSON_GET_REG_CP_BIA_PERI_2_0(x)            ((u32)(((x >> 26) & 0x00000007)))
#define SYSON_BIT_DOGENB_PERI                       ((u32)0x00000001 << 25)                  /*!< R/W     0 0: watch dog on, 1: watch dog off */
#define SYSON_MASK_REG_PS_PERI_2_0                  ((u32)0x00000007 << 22)                  /*!< R/W   0x0 None */
#define SYSON_REG_PS_PERI_2_0(x)                    ((u32)(((x) & 0x00000007) << 22))
#define SYSON_GET_REG_PS_PERI_2_0(x)                ((u32)(((x >> 22) & 0x00000007)))
#define SYSON_BIT_REG_PSEN_PERI                     ((u32)0x00000001 << 21)                  /*!< R/W   1   phase selector enable */
#define SYSON_MASK_REG_CP_SET_PERI_1_0              ((u32)0x00000003 << 19)                  /*!< R/W   0x2
                                                                                              * Cp selection. Default=5p
                                                                                              * 00: Cp=4p,    01: Cp=5p,
                                                                                              * 10: Cp=6p,    11: Cp=7p
                                                                                              * XTAL=40MHz : 10 ,Cp=6p
                                                                                              * XTAL=26MHz : 01 ,Cp=5p
                                                                                              */
#define SYSON_REG_CP_SET_PERI_1_0(x)                ((u32)(((x) & 0x00000003) << 19))
#define SYSON_GET_REG_CP_SET_PERI_1_0(x)            ((u32)(((x >> 19) & 0x00000003)))
#define SYSON_MASK_REG_RS_SET_PERI_2_0              ((u32)0x00000007 << 16)                  /*!< R/W   0x5
                                                                                              * Rs selection. Default=4k                                          
                                                                                              * 000: Rs=15k, 001: Rs=13k, 010: Rs=11k, 011: Rs=9k,             
                                                                                              * 100: Rs=7k,  101: Rs=5k,  110: Rs=3k,  111: Rs=1k;    
                                                                                              * XTAL=40MHz : 101 ,Rs=5K        
                                                                                              */
#define SYSON_REG_RS_SET_PERI_2_0(x)                ((u32)(((x) & 0x00000007) << 16))
#define SYSON_GET_REG_RS_SET_PERI_2_0(x)            ((u32)(((x >> 16) & 0x00000007)))
#define SYSON_MASK_REG_CS_SEL_PERI_1_0              ((u32)0x00000003 << 14)                  /*!< R/W   0x3
                                                                                              * Cs selection. Default=70p
                                                                                              * 00: Cs=40p,   01: Cs=50p,   10: Cs=60p,   11: Cs=70p
                                                                                              * XTAL=40MHz : 11 , Cs=70p
                                                                                              * XTAL=26MHz : 11 , Cs=70p
                                                                                              */
#define SYSON_REG_CS_SEL_PERI_1_0(x)                ((u32)(((x) & 0x00000003) << 14))
#define SYSON_GET_REG_CS_SEL_PERI_1_0(x)            ((u32)(((x >> 14) & 0x00000003)))
#define SYSON_BIT_REG_FREF_EDGE_PERI                ((u32)0x00000001 << 13)                  /*!< R/W     0 0:fref, 1:frefb */
#define SYSON_BIT_REG_CK_EN_PERI                    ((u32)0x00000001 << 12)                  /*!< R/W     0 output clk enable */
#define SYSON_BIT_REG_CK_EN_D4_PERI                 ((u32)0x00000001 << 11)                  /*!< R/W   0   output clk div 4 enable */
#define SYSON_BIT_REG_CK_EN_D2_PERI                 SYSON_BIT_REG_CK_EN_D4_PERI
#define SYSON_MASK_REG_DOG_SEL_PERI_2_0             ((u32)0x00000007 << 8)                   /*!< R/W   0x4 bit[8] reserved. Watchdog voltage selection. Default=0.72V. bit[10:9] 00: 0.365V, 01: 0.54V, 10: 0.72V, 11: 0.9V */
#define SYSON_REG_DOG_SEL_PERI_2_0(x)               ((u32)(((x) & 0x00000007) << 8))
#define SYSON_GET_REG_DOG_SEL_PERI_2_0(x)           ((u32)(((x >> 8) & 0x00000007)))
#define SYSON_BIT_REG_XTAL_MANUAL_MODE_PERI         ((u32)0x00000001 << 7)                   /*!< R/W   0x1
                                                                                              * reg_xtal_maunal_mode.  Default=0                                            
                                                                                              * 0: reg_xtal_sel<3:0> is controlled by REG, xtal_div_sel is controlled by mapping table.                      
                                                                                              * 1: reg_xtal_sel<3:0>=4'b1111, xtal_div_sel is controlled by reg_xtal_div_sel_maunal<1:0>
                                                                                              */
#define SYSON_MASK_REG_XTAL_DIV_SEL_MANUAL_PERI_1_0 ((u32)0x00000003 << 5)                   /*!< R/W   0x0 input divider selection in xtal maunal mode. Default=00  00=/1,  10=/2,  11=/4 */
#define SYSON_REG_XTAL_DIV_SEL_MANUAL_PERI_1_0(x)   ((u32)(((x) & 0x00000003) << 5))
#define SYSON_GET_REG_XTAL_DIV_SEL_MANUAL_PERI_1_0(x) ((u32)(((x >> 5) & 0x00000003)))
#define SYSON_MASK_REG_VC_THL_PERI_1_0              ((u32)0x00000003 << 3)                   /*!< R/W   0x0
                                                                                              * VC voltage, Default=00                                           
                                                                                              * 00: VC=1/4Vdd,  01: VC=1/2Vdd,  10=3/4Vdd,  11: VC=Vdd
                                                                                              */
#define SYSON_REG_VC_THL_PERI_1_0(x)                ((u32)(((x) & 0x00000003) << 3))
#define SYSON_GET_REG_VC_THL_PERI_1_0(x)            ((u32)(((x >> 3) & 0x00000003)))
#define SYSON_MASK_C3_SET_PERI_1_0                  ((u32)0x00000003 << 1)                   /*!< R/W   0x1 C3 selection. Default=1p  00: 0.5p, 01:1p, 10: Cs=1.5p, 11: 2p */
#define SYSON_C3_SET_PERI_1_0(x)                    ((u32)(((x) & 0x00000003) << 1))
#define SYSON_GET_C3_SET_PERI_1_0(x)                ((u32)(((x >> 1) & 0x00000003)))
#define SYSON_BIT_R3_SET_PERI                       ((u32)0x00000001 << 0)                   /*!< R/W     1 Rs selection. Default=1k  0: 0 Ohm, 1: 1k Ohm */
/** @} */


/** @defgroup REG_PON_PERIPLL_CTRL1
 * @brief
 * @{
 */
#define SYSON_MASK_REG_XTAL_SEL_PERI_3_0            ((u32)0x0000000F << 28)                  /*!< R     0x0 xtal selection, Default=40MHz  0000= 40MHz, 0101= 26MHz, */
#define SYSON_REG_XTAL_SEL_PERI_3_0(x)              ((u32)(((x) & 0x0000000F) << 28))
#define SYSON_GET_REG_XTAL_SEL_PERI_3_0(x)          ((u32)(((x >> 28) & 0x0000000F)))
#define SYSON_MASK_REG_FREQ_SEL_PERI_3_0            ((u32)0x0000000F << 24)                  /*!< R/W   0x0
                                                                                              * output clk selection, Default=500MHz                            
                                                                                              * 0000= 400MHz,  
                                                                                              * 0001= 300MHz,                
                                                                                              * 0010= X,       
                                                                                              * 0011= X,                    
                                                                                              * 0100= 216MHz,  
                                                                                              * 0101=196.608MHz, 
                                                                                              * 0110=192MHz,   
                                                                                              * 0111= 180.632MHz, 
                                                                                              * 1000= 148.5MHz,
                                                                                              * 1001= X,         
                                                                                              * 1010= X,       
                                                                                              * 1011= X,        
                                                                                              * 1100= X,       
                                                                                              * 1101= X,         
                                                                                              * 1110= X,       
                                                                                              * 1111= 500MHz                                      
                                                                                              */
#define SYSON_REG_FREQ_SEL_PERI_3_0(x)              ((u32)(((x) & 0x0000000F) << 24))
#define SYSON_GET_REG_FREQ_SEL_PERI_3_0(x)          ((u32)(((x >> 24) & 0x0000000F)))
#define SYSON_MASK_TBASE_FREQ_PERI_3_0              ((u32)0x0000000F << 20)                  /*!< R/W   0x8 now is empty */
#define SYSON_TBASE_FREQ_PERI_3_0(x)                ((u32)(((x) & 0x0000000F) << 20))
#define SYSON_GET_TBASE_FREQ_PERI_3_0(x)            ((u32)(((x >> 20) & 0x0000000F)))
#define SYSON_MASK_STEP_FREQ_PERI_3_0               ((u32)0x0000000F << 16)                  /*!< R/W   0x5 step size selection for switching freq.  0000:2^5  0001: 2^6  0010: 2^7  0011: 2^8  0100: 2^9  0101: 2^10  0110: 2^11  0111: 2^12 */
#define SYSON_STEP_FREQ_PERI_3_0(x)                 ((u32)(((x) & 0x0000000F) << 16))
#define SYSON_GET_STEP_FREQ_PERI_3_0(x)             ((u32)(((x >> 16) & 0x0000000F)))
#define SYSON_BIT_TRIG_FREQ_PERI                    ((u32)0x00000001 << 15)                  /*!< R/W   0   freq. step up or step down enable */
#define SYSON_MASK_BB_DBG_SEL_AFE_SDM_PERI_3_0      ((u32)0x0000000F << 11)                  /*!< R/W   0x0 None */
#define SYSON_BB_DBG_SEL_AFE_SDM_PERI_3_0(x)        ((u32)(((x) & 0x0000000F) << 11))
#define SYSON_GET_BB_DBG_SEL_AFE_SDM_PERI_3_0(x)    ((u32)(((x >> 11) & 0x0000000F)))
#define SYSON_MASK_DIVN_SDM_PERI_5_0                ((u32)0x0000003F << 5)                   /*!< R/W   0xE
                                                                                              * Default
                                                                                              * 320MHz: 000110
                                                                                              * 600MHz: 001101
                                                                                              * 660MHz: 001110
                                                                                              * 700MHz: 001111
                                                                                              *
                                                                                              * For scan-mode ONLY:
                                                                                              * 400MHz: 001000
                                                                                              */
#define SYSON_DIVN_SDM_PERI_5_0(x)                  ((u32)(((x) & 0x0000003F) << 5))
#define SYSON_GET_DIVN_SDM_PERI_5_0(x)              ((u32)(((x >> 5) & 0x0000003F)))
#define SYSON_MASK_REG_CK_OUT_SEL_PERI_3_0          ((u32)0x0000000F << 1)                   /*!< R/W   0x0
                                                                                              * output divider selection. Default=0000(/1)                                                                0000=/1,  1000=/2,  1100=/4,  1110=/8,   1111=/16
                                                                                              * 300~350MHz PLL output clock, output divider selection. 1000=/2
                                                                                              * Ex : 600MHz/2=300MHz
                                                                                              * 350~700MHz PLL output clock, output divider selection. 0000=/1
                                                                                              * Ex : 700MHz/1=700MHz
                                                                                              * Spec:/2(mandatory
                                                                                              */
#define SYSON_REG_CK_OUT_SEL_PERI_3_0(x)            ((u32)(((x) & 0x0000000F) << 1))
#define SYSON_GET_REG_CK_OUT_SEL_PERI_3_0(x)        ((u32)(((x >> 1) & 0x0000000F)))
#define SYSON_BIT_ICP_HALF_PERI                     ((u32)0x00000001 << 0)                   /*!< R/W   0x1 charge pump current half or not
                                                                                              * 0: no half    1: half */
/** @} */


/** @defgroup REG_PON_PERIPLL_CTRL2
 * @brief
 * @{
 */
#define SYSON_MASK_STEP_PERI_12_0                   ((u32)0x00001FFF << 19)                  /*!< R/W   0x0 ssc step size selection. */
#define SYSON_STEP_PERI_12_0(x)                     ((u32)(((x) & 0x00001FFF) << 19))
#define SYSON_GET_STEP_PERI_12_0(x)                 ((u32)(((x >> 19) & 0x00001FFF)))
#define SYSON_MASK_TBASE_PERI_11_0                  ((u32)0x00000FFF << 7)                   /*!< R/W   0x0 ssc freq selection */
#define SYSON_TBASE_PERI_11_0(x)                    ((u32)(((x) & 0x00000FFF) << 7))
#define SYSON_GET_TBASE_PERI_11_0(x)                ((u32)(((x >> 7) & 0x00000FFF)))
#define SYSON_BIT_EN_SSC_PERI                       ((u32)0x00000001 << 6)                   /*!< R/W   0   ssc enable. Default=0 */
#define SYSON_MASK_RESERVED_PERI_CTRL2              ((u32)0x0000003F << 0)                   /*!< R/W   0x0 reserved */
#define SYSON_RESERVED_PERI_CTRL2(x)                ((u32)(((x) & 0x0000003F) << 0))
#define SYSON_GET_RESERVED_PERI_CTRL2(x)            ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_PON_PERIPLL_CTRL3
 * @brief
 * @{
 */
#define SYSON_MASK_F0F_SDM_PERI_12_0                ((u32)0x00001FFF << 19)                  /*!< R/W   0x0 Default 660MHz: 0000000000000  330MHz: 0000000000000  600MHz: 0000000000000  660MHz: 0000000000000 */
#define SYSON_F0F_SDM_PERI_12_0(x)                  ((u32)(((x) & 0x00001FFF) << 19))
#define SYSON_GET_F0F_SDM_PERI_12_0(x)              ((u32)(((x >> 19) & 0x00001FFF)))
#define SYSON_MASK_F0N_SDM_PERI_2_0                 ((u32)0x00000007 << 16)                  /*!< R/W   0x4 Default 660MHz: 100  330MHz: 010  600MHz: 000  660MHz: 100 */
#define SYSON_F0N_SDM_PERI_2_0(x)                   ((u32)(((x) & 0x00000007) << 16))
#define SYSON_GET_F0N_SDM_PERI_2_0(x)               ((u32)(((x >> 16) & 0x00000007)))
#define SYSON_BIT_ORDER_SDM_PERI                    ((u32)0x00000001 << 15)                  /*!< R/W   0   SDM order */
#define SYSON_MASK_RESERVED_PERI_CTRL3              ((u32)0x00007FFF << 0)                   /*!< R/W   0x0 reserved */
#define SYSON_RESERVED_PERI_CTRL3(x)                ((u32)(((x) & 0x00007FFF) << 0))
#define SYSON_GET_RESERVED_PERI_CTRL3(x)            ((u32)(((x >> 0) & 0x00007FFF)))
/** @} */


/** @defgroup REG_PON_PERIPLL_CTRL4
 * @brief
 * @{
 */
#define SYSON_BIT_RESERVED_74C                      ((u32)0x00000001 << 15)                  /*!< R/W     0 Reserved */
#define SYSON_BIT_DUMMY_74C                         SYSON_BIT_RESERVED_74C
#define SYSON_BIT_POW_CKGEN_74C                     ((u32)0x00000001 << 14)                  /*!< R/W     0 pow of ckgen */
#define SYSON_MASK_CKGEN_DIV_74C                    ((u32)0x000001FF << 5)                   /*!< R/W   111010 DIV9B, N=DIV[8:0]+2 */
#define SYSON_CKGEN_DIV_74C(x)                      ((u32)(((x) & 0x000001FF) << 5))
#define SYSON_GET_CKGEN_DIV_74C(x)                  ((u32)(((x >> 5) & 0x000001FF)))
#define SYSON_BIT_EN_CK400_PS_74C                   ((u32)0x00000001 << 4)                   /*!< R/W   0x0 Gating ck400M_ps */
#define SYSON_BIT_EN_CK400M_74C                     ((u32)0x00000001 << 3)                   /*!< R/W   0x0 Gating ck400m,ck400m is PLL VCO freq (350M~700M) */
#define SYSON_BIT_EN_CLK400M_74C                    SYSON_BIT_EN_CK400M_74C
#define SYSON_MASK_PHASE_SEL_74C                    ((u32)0x00000007 << 0)                   /*!< R/W   0x0 choose delay phase (n/8 period) for ck400m_ps */
#define SYSON_PHASE_SEL_74C(x)                      ((u32)(((x) & 0x00000007) << 0))
#define SYSON_GET_PHASE_SEL_74C(x)                  ((u32)(((x >> 0) & 0x00000007)))
/** @} */


/** @defgroup REG_PON_PLL_DBG
 * @brief
 * @{
 */
#define SYSON_MASK_PLL_TEST                         ((u32)0x00000007 << 13)                  /*!< R/W   0x0 None */
#define SYSON_PLL_TEST(x)                           ((u32)(((x) & 0x00000007) << 13))
#define SYSON_GET_PLL_TEST(x)                       ((u32)(((x >> 13) & 0x00000007)))
#define SYSON_BIT_REG0X_TEST_12                     ((u32)0x00000001 << 12)                  /*!< R/W   0   0 = ckmon , 1 = ckmon/4 */
#define SYSON_BIT_POW_DIV4                          ((u32)0x00000001 << 11)                  /*!< R/W   1   pow of divider4 */
#define SYSON_BIT_ISO                               ((u32)0x00000001 << 10)                  /*!< R/W   0   1 for ISO ck_fref/ck_frefb */
#define SYSON_BIT_REF_CLK_SEL                       ((u32)0x00000001 << 9)                   /*!< R/W   0   xtal clk from 0 special route 1 APR */
#define SYSON_BIT_CK_MON_EN                         ((u32)0x00000001 << 8)                   /*!< R/W   0   ckmon enable */
#define SYSON_MASK_CK_SEL_3_0                       ((u32)0x0000000F << 4)                   /*!< R/W   0x0
                                                                                              * ckmon clk sel 
                                                                                              * 1111:CK_400M_ISP
                                                                                              * 1110:READY_400M
                                                                                              * 1101:CK_ISPS
                                                                                              * 1100:READY_ISPS
                                                                                              * 1011:CK_300M_SYS
                                                                                              * 1010:READY_300M
                                                                                              * 1001:CK_240M_ENC
                                                                                              * 1000:READY_240M_ENC
                                                                                              * 0111:CK_80M_PERI
                                                                                              * 0110:READY_80M_PERI
                                                                                              * 0101:CK_98P304M_I2S
                                                                                              * 0100:CK_24P576M_I2S
                                                                                              * 0011:READY_I2S
                                                                                              * 0010:CK_45P158M_PCM
                                                                                              * 0001:READY_45P158M_PCM
                                                                                              * 0000:TL
                                                                                              */
#define SYSON_CK_SEL_3_0(x)                         ((u32)(((x) & 0x0000000F) << 4))
#define SYSON_GET_CK_SEL_3_0(x)                     ((u32)(((x >> 4) & 0x0000000F)))
#define SYSON_MASK_APGIO_DRIVE                      ((u32)0x00000003 << 2)                   /*!< R/W   0x3 AGPIO driving sel */
#define SYSON_APGIO_DRIVE(x)                        ((u32)(((x) & 0x00000003) << 2))
#define SYSON_GET_APGIO_DRIVE(x)                    ((u32)(((x >> 2) & 0x00000003)))
#define SYSON_BIT_GPO                               ((u32)0x00000001 << 1)                   /*!< R/W   0   AGPIO CLK SEL 0=external 1=pll */
#define SYSON_BIT_GPE                               ((u32)0x00000001 << 0)                   /*!< R/W   0   AGPIO enable */
/** @} */


/** @defgroup REG_BT_AFE_CTRL0
 * @brief
 * @{
 */
#define SYSON_MASK_DUMMY_760                        ((u32)0x000000FF << 24)                  /*!< R/W   0x0 None */
#define SYSON_DUMMY_760(x)                          ((u32)(((x) & 0x000000FF) << 24))
#define SYSON_GET_DUMMY_760(x)                      ((u32)(((x >> 24) & 0x000000FF)))
#define SYSON_MASK_DITHER_SDM_3_0_                  ((u32)0x0000000F << 20)                  /*!< R/W   0x0 None */
#define SYSON_DITHER_SDM_3_0_(x)                    ((u32)(((x) & 0x0000000F) << 20))
#define SYSON_GET_DITHER_SDM_3_0_(x)                ((u32)(((x >> 20) & 0x0000000F)))
#define SYSON_MASK_C3_SET_1_0_                      ((u32)0x00000003 << 18)                  /*!< R/W   0x1 00: 0.5p  01:1p  10:1.5p  11:2p */
#define SYSON_C3_SET_1_0_(x)                        ((u32)(((x) & 0x00000003) << 18))
#define SYSON_GET_C3_SET_1_0_(x)                    ((u32)(((x >> 18) & 0x00000003)))
#define SYSON_BIT_R3_SET                            ((u32)0x00000001 << 17)                  /*!< R/W   0x1 0: 0k 1: 1k */
#define SYSON_BIT_ICP_HALF                          ((u32)0x00000001 << 16)                  /*!< R/W   0x0 0: icp current same 1: icp current half */
#define SYSON_BIT_ISO_760                           ((u32)0x00000001 << 15)                  /*!< R/W   0x1 1:power on for PLL */
#define SYSON_BIT_ORDER_SDM                         ((u32)0x00000001 << 14)                  /*!< R/W   0x0
                                                                                              * SDM order
                                                                                              * 0:2nd   1:3rd
                                                                                              */
#define SYSON_MASK_DIVN_SDM_5_0_                    ((u32)0x0000003F << 8)                   /*!< R/W   001110 SDM divider sel */
#define SYSON_DIVN_SDM_5_0_(x)                      ((u32)(((x) & 0x0000003F) << 8))
#define SYSON_GET_DIVN_SDM_5_0_(x)                  ((u32)(((x >> 8) & 0x0000003F)))
#define SYSON_MASK_BB_DBG_SEL_AFE_SDM_3_0_          ((u32)0x0000000F << 4)                   /*!< R/W   0x0 SDM debug selection */
#define SYSON_BB_DBG_SEL_AFE_SDM_3_0_(x)            ((u32)(((x) & 0x0000000F) << 4))
#define SYSON_GET_BB_DBG_SEL_AFE_SDM_3_0_(x)        ((u32)(((x >> 4) & 0x0000000F)))
#define SYSON_MASK_REG_CP_SET_1_0_                  ((u32)0x00000003 << 2)                   /*!< R/W   0x1
                                                                                              * LPF cp select
                                                                                              * 00: 0.5p, 01: 1p, 10:1.5p, 11: 2p
                                                                                              */
#define SYSON_REG_CP_SET_1_0_(x)                    ((u32)(((x) & 0x00000003) << 2))
#define SYSON_GET_REG_CP_SET_1_0_(x)                ((u32)(((x >> 2) & 0x00000003)))
#define SYSON_MASK_REG_CS_SET_1_0_                  ((u32)0x00000003 << 0)                   /*!< R/W   0x2
                                                                                              * LPF cs select
                                                                                              * 00: 10p, 01: 15p, 10:20p, 11: 25p
                                                                                              */
#define SYSON_REG_CS_SET_1_0_(x)                    ((u32)(((x) & 0x00000003) << 0))
#define SYSON_GET_REG_CS_SET_1_0_(x)                ((u32)(((x >> 0) & 0x00000003)))
/** @} */


/** @defgroup REG_BT_AFE_CTRL1
 * @brief
 * @{
 */
#define SYSON_MASK_F0N_SDM_2_0_                     ((u32)0x00000007 << 29)                  /*!< R/W   0x0 SDM divider sel */
#define SYSON_F0N_SDM_2_0_(x)                       ((u32)(((x) & 0x00000007) << 29))
#define SYSON_GET_F0N_SDM_2_0_(x)                   ((u32)(((x >> 29) & 0x00000007)))
#define SYSON_MASK_F0F_SDM_12_0_                    ((u32)0x00001FFF << 16)                  /*!< R/W   0x0 SDM divider sel */
#define SYSON_F0F_SDM_12_0_(x)                      ((u32)(((x) & 0x00001FFF) << 16))
#define SYSON_GET_F0F_SDM_12_0_(x)                  ((u32)(((x >> 16) & 0x00001FFF)))
#define SYSON_MASK_REG_VC_TH_TL                     ((u32)0x00000003 << 14)                  /*!< R/W   0x0
                                                                                              * 00:normal  01:vc=0.25
                                                                                              * 10:vc=0.75  11:vc=0.5
                                                                                              */
#define SYSON_REG_VC_TH_TL(x)                       ((u32)(((x) & 0x00000003) << 14))
#define SYSON_GET_REG_VC_TH_TL(x)                   ((u32)(((x >> 14) & 0x00000003)))
#define SYSON_MASK_REG_PS_2_0_                      ((u32)0x00000007 << 11)                  /*!< R/W   0x0
                                                                                              * decide clock phase when reg_ps_enb = 1
                                                                                              * 000: phase 0
                                                                                              * 001: phase 45°
                                                                                              * 010: phase 90°
                                                                                              * 111: phase 315°
                                                                                              */
#define SYSON_REG_PS_2_0_(x)                        ((u32)(((x) & 0x00000007) << 11))
#define SYSON_GET_REG_PS_2_0_(x)                    ((u32)(((x >> 11) & 0x00000007)))
#define SYSON_BIT_REG_PSEN                          ((u32)0x00000001 << 10)                  /*!< R/W   0x1
                                                                                              * PLL phase shift enable
                                                                                              * 1:enable  0:disable
                                                                                              */
#define SYSON_BIT_RFE_SEL_SDM                       ((u32)0x00000001 << 9)                   /*!< R/W   0x1 pll reference clk select 1:table 0:regesters */
#define SYSON_MASK_REG_CK_BT_DA__1_0_               ((u32)0x00000003 << 7)                   /*!< R/W   0x1
                                                                                              * 00:20MHz
                                                                                              * 01:40MHz
                                                                                              * 10:80MHz
                                                                                              * 11:160MHz
                                                                                              */
#define SYSON_REG_CK_BT_DA__1_0_(x)                 ((u32)(((x) & 0x00000003) << 7))
#define SYSON_GET_REG_CK_BT_DA__1_0_(x)             ((u32)(((x >> 7) & 0x00000003)))
#define SYSON_MASK_REG_CK_BT_AD__1_0_               ((u32)0x00000003 << 5)                   /*!< R/W   0x0
                                                                                              * 00:20MHz
                                                                                              * 01:40MHz
                                                                                              * 10:80MHz
                                                                                              * 11:160MHz
                                                                                              */
#define SYSON_REG_CK_BT_AD__1_0_(x)                 ((u32)(((x) & 0x00000003) << 5))
#define SYSON_GET_REG_CK_BT_AD__1_0_(x)             ((u32)(((x >> 5) & 0x00000003)))
#define SYSON_BIT_REG_CK_BT_BB_EN                   ((u32)0x00000001 << 4)                   /*!< R/W   0x1 1:enable digital ck(BT 320M) */
#define SYSON_BIT_REG_CK_BT_AD_EN                   ((u32)0x00000001 << 3)                   /*!< R/W   0x1 1:enable ad ck */
#define SYSON_BIT_REG_CK_BT_DA_EN                   ((u32)0x00000001 << 2)                   /*!< R/W   0x1 1:enable da ck */
#define SYSON_BIT_REG_CK320M_EN                     ((u32)0x00000001 << 1)                   /*!< R/W   0x0
                                                                                              * 1:BT  AFE
                                                                                              * 320M enable
                                                                                              */
#define SYSON_BIT_REG_CK320MB_EN                    ((u32)0x00000001 << 0)                   /*!< R/W   0x0
                                                                                              * 1:BT  AFE
                                                                                              * 320MB enable
                                                                                              */
/** @} */


/** @defgroup REG_BT_AFE_CTRL2
 * @brief
 * @{
 */
#define SYSON_MASK_REG_CK_MON_SEL_2_0_              ((u32)0x00000007 << 13)                  /*!< R/W   0x0
                                                                                              * Set ck_monh clk
                                                                                              * 000 ck_test
                                                                                              * 001 ck_320m
                                                                                              * 010 bbck
                                                                                              * 011 dack
                                                                                              * 100 adck
                                                                                              * 101 ck_ready
                                                                                              * 110 ck_5m
                                                                                              * 111 TH
                                                                                              */
#define SYSON_REG_CK_MON_SEL_2_0_(x)                ((u32)(((x) & 0x00000007) << 13))
#define SYSON_GET_REG_CK_MON_SEL_2_0_(x)            ((u32)(((x >> 13) & 0x00000007)))
#define SYSON_BIT_REG_CK_MON_EN                     ((u32)0x00000001 << 12)                  /*!< R/W   0x0 1:Enable mon sel */
#define SYSON_BIT_REG_TESTEN                        ((u32)0x00000001 << 11)                  /*!< R/W   0x0 1: enable testing pll feedback clk */
#define SYSON_BIT_REG_FREF_EDGE                     ((u32)0x00000001 << 10)                  /*!< R/W   0x0 pll referemce clk edge select 1:negative 0:postive */
#define SYSON_BIT_REG_CK_5M_EN                      ((u32)0x00000001 << 9)                   /*!< R/W   0x1 1:enable */
#define SYSON_BIT_REG_DOGENB                        ((u32)0x00000001 << 8)                   /*!< R/W   0x0 0: enable watch dog 1:disable */
#define SYSON_MASK_REG_RS_SET_2_0_                  ((u32)0x00000007 << 5)                   /*!< R/W   0x4
                                                                                              * Set rs
                                                                                              * 000 15k
                                                                                              * 001 13k
                                                                                              * 010 11k
                                                                                              * 011 9k
                                                                                              * 100 7k
                                                                                              * 101 5k
                                                                                              * 110 3k
                                                                                              * 111 1k
                                                                                              */
#define SYSON_REG_RS_SET_2_0_(x)                    ((u32)(((x) & 0x00000007) << 5))
#define SYSON_GET_REG_RS_SET_2_0_(x)                ((u32)(((x >> 5) & 0x00000007)))
#define SYSON_BIT_REG_FREF_SEL                      ((u32)0x00000001 << 4)                   /*!< R/W   0x1
                                                                                              * 0:PLL clock form analog
                                                                                              * 1:PLL clock from digital
                                                                                              */
#define SYSON_MASK_REG_CP_BIAS_2_0_                 ((u32)0x00000007 << 1)                   /*!< R/W   0x0
                                                                                              * pll CP current selection
                                                                                              * 000 10uA
                                                                                              * 001 15uA
                                                                                              * 010 20uA
                                                                                              * 011 25uA
                                                                                              * 100 30uA
                                                                                              * 101 35uA
                                                                                              * 110 40uA
                                                                                              * 111 45uA
                                                                                              */
#define SYSON_REG_CP_BIAS_2_0_(x)                   ((u32)(((x) & 0x00000007) << 1))
#define SYSON_GET_REG_CP_BIAS_2_0_(x)               ((u32)(((x >> 1) & 0x00000007)))
#define SYSON_BIT_POW_PLL_768                       ((u32)0x00000001 << 0)                   /*!< R/W   0x0 1: PLL enable */
/** @} */


/** @defgroup REG_BT_AFE_CTRL3
 * @brief
 * @{
 */
#define SYSON_MASK_LDO_ADDA_DUMMY1                  ((u32)0x00000003 << 22)                  /*!< R/W   0x0 None */
#define SYSON_LDO_ADDA_DUMMY1(x)                    ((u32)(((x) & 0x00000003) << 22))
#define SYSON_GET_LDO_ADDA_DUMMY1(x)                ((u32)(((x >> 22) & 0x00000003)))
#define SYSON_MASK_LDO_ADDA_SW_1_0_                 ((u32)0x00000003 << 20)                  /*!< R/W   0x1
                                                                                              * 00:0.85V
                                                                                              * 01:0.90V
                                                                                              * 10:0.95V
                                                                                              * 11:1.00V
                                                                                              */
#define SYSON_LDO_ADDA_SW_1_0_(x)                   ((u32)(((x) & 0x00000003) << 20))
#define SYSON_GET_LDO_ADDA_SW_1_0_(x)               ((u32)(((x >> 20) & 0x00000003)))
#define SYSON_BIT_LDO_ADDA_VPULSE                   ((u32)0x00000001 << 19)                  /*!< R/W   0x0 None */
#define SYSON_BIT_LDO_ADDA_DUMMY0                   ((u32)0x00000001 << 18)                  /*!< R/W   0x0 None */
#define SYSON_BIT_LDO_ADDA_POW_LDO                  ((u32)0x00000001 << 17)                  /*!< R/W   0x1 None */
#define SYSON_BIT_LDO_ADDA_SW_LDO2PWRCUT            ((u32)0x00000001 << 16)                  /*!< R/W   0x0 None */
#define SYSON_MASK_LDO_PLL_DUMMY1                   ((u32)0x00000003 << 6)                   /*!< R/W   0x0 None */
#define SYSON_LDO_PLL_DUMMY1(x)                     ((u32)(((x) & 0x00000003) << 6))
#define SYSON_GET_LDO_PLL_DUMMY1(x)                 ((u32)(((x >> 6) & 0x00000003)))
#define SYSON_MASK_LDO_PLL_SW_1_0_                  ((u32)0x00000003 << 4)                   /*!< R/W   0x1
                                                                                              * 00:0.85V
                                                                                              * 01:0.90V
                                                                                              * 10:0.95V
                                                                                              * 11:1.00V
                                                                                              */
#define SYSON_LDO_PLL_SW_1_0_(x)                    ((u32)(((x) & 0x00000003) << 4))
#define SYSON_GET_LDO_PLL_SW_1_0_(x)                ((u32)(((x >> 4) & 0x00000003)))
#define SYSON_BIT_LDO_PLL_VPULSE                    ((u32)0x00000001 << 3)                   /*!< R/W   0x0 None */
#define SYSON_BIT_LDO_PLL_DUMMY0                    ((u32)0x00000001 << 2)                   /*!< R/W   0x0 None */
#define SYSON_BIT_LDO_PLL_POW_LDO                   ((u32)0x00000001 << 1)                   /*!< R/W   0x1 None */
#define SYSON_BIT_LDO_PLL_SW_LDO2PWRCUT             ((u32)0x00000001 << 0)                   /*!< R/W   0x0 None */
/** @} */




/** @defgroup SYSON_Exported_Types SYSON Exported Types
* @{
*/
/**
* @brief SYSON Register Declaration
*/

typedef struct {
	__IO uint32_t PON_SYSON_CTRL;                               /*!< Register,  Address offset:0x0000 */
	__I  uint32_t PON_DUMMY0;                                   /*!< Register,  Address offset:0x0004 */
	__I  uint32_t PON_DUMMY1;                                   /*!< Register,  Address offset:0x0008 */
	__I  uint32_t PON_DUMMY2;                                   /*!< Register,  Address offset:0x000C */
	__I  uint32_t PON_DUMMY3;                                   /*!< Register,  Address offset:0x0010 */
	__I  uint32_t WIFI_PLATFORM_DBG;                            /*!< Register,  Address offset:0x0014 */
	__I  uint32_t PON_DUMMY5;                                   /*!< Register,  Address offset:0x0018 */
	__I  uint32_t PON_DUMMY6;                                   /*!< Register,  Address offset:0x001C */
	__IO uint32_t PKE_SEC_ATTR_CTRL;                            /*!< Register,  Address offset:0x0020 */
	__IO uint32_t PON_SEC_KEY_CTRL;                             /*!< Register,  Address offset:0x0024 */
	__I  uint32_t PON_DUMMY8;                                   /*!< Register,  Address offset:0x0028 */
	__I  uint32_t PON_DUMMY9;                                   /*!< Register,  Address offset:0x002C */
	__IO uint32_t PON_BT_CTRL;                                  /*!< Register,  Address offset:0x0030 */
	__I  uint32_t UART_INTR_STS;                                /*!< Register,  Address offset:0x0034 */
	__IO uint32_t LOGUART_CLK_CTRL;                             /*!< Register,  Address offset:0x0038 */
	__IO uint32_t PON_GDMA_CTRL;                                /*!< Register,  Address offset:0x003C */
	__I  uint32_t PON_DUMMY10;                                  /*!< Register,  Address offset:0x0040 */
	__I  uint32_t PON_DUMMY11;                                  /*!< Register,  Address offset:0x0044 */
	__IO uint32_t BOOT_STATE;                                   /*!< Register,  Address offset:0x0048 */
	__IO uint32_t BOOT_STATE1;                                  /*!< Register,  Address offset:0x004C */
	__IO uint32_t PON_DUMMY12;                                  /*!< Register,  Address offset:0x0050 */
	__IO uint32_t PON_DUMMY13;                                  /*!< Register,  Address offset:0x0054 */
	__IO uint32_t PON_DUMMY14;                                  /*!< Register,  Address offset:0x0058 */
	__IO uint32_t PON_DUMMY15;                                  /*!< Register,  Address offset:0x005C */
	__IO uint32_t PON_DUMMY16;                                  /*!< Register,  Address offset:0x0060 */
	__IO uint32_t PON_DUMMY17;                                  /*!< Register,  Address offset:0x0064 */
	__IO uint32_t PON_DUMMY18;                                  /*!< Register,  Address offset:0x0068 */
	__IO uint32_t PON_DUMMY19;                                  /*!< Register,  Address offset:0x006C */
	__IO uint32_t PON_DUMMY20;                                  /*!< Register,  Address offset:0x0070 */
	__I  uint32_t PON_DUMMY21;                                  /*!< Register,  Address offset:0x0074 */
	__IO uint32_t MEM_WLAN_CTRL0;                               /*!< Register,  Address offset:0x0078 */
	__IO uint32_t MEM_WLAN_CTRL1;                               /*!< Register,  Address offset:0x007C */
	__IO uint32_t MEM_WLAN_CTRL2;                               /*!< Register,  Address offset:0x0080 */
	__IO uint32_t MEM_BT_CNTRL;                                 /*!< Register,  Address offset:0x0084 */
	__IO uint32_t MEM_GMAC_CNTRL;                               /*!< Register,  Address offset:0x0088 */
	__IO uint32_t MEM_PON_CTRL;                                 /*!< Register,  Address offset:0x008C */
	__IO uint32_t MEM_BTSHARE_CTRL;                             /*!< Register,  Address offset:0x0090 */
	__IO uint32_t PON_DUMMY23;                                  /*!< Register,  Address offset:0x0094 */
	__IO uint32_t MEM_COMM_CNTRL;                               /*!< Register,  Address offset:0x0098 */
	__IO uint32_t MEM_CPU_CNTRL;                                /*!< Register,  Address offset:0x009C */
	__IO uint32_t MEM_SYSTEM_CTRL0;                             /*!< Register,  Address offset:0x00A0 */
	__IO uint32_t MEM_SYSTEM_CTRL1;                             /*!< Register,  Address offset:0x00A4 */
	__IO uint32_t MEM_SYSTEM_CTRL2;                             /*!< Register,  Address offset:0x00A8 */
	__IO uint32_t MEM_SYSTEM_CTRL3;                             /*!< Register,  Address offset:0x00AC */
	__IO uint32_t MEM_SYSTEM_CTRL4;                             /*!< Register,  Address offset:0x00B0 */
	__IO uint32_t MEM_SYSTEM_CTRL5;                             /*!< Register,  Address offset:0x00B4 */
	__IO uint32_t MEM_SYSTEM_CTRL6;                             /*!< Register,  Address offset:0x00B8 */
	__IO uint32_t MEM_SYSTEM_CTRL7;                             /*!< Register,  Address offset:0x00BC */
	__IO uint32_t MEM_NANDECC;                                  /*!< Register,  Address offset:0x00C0 */
	__IO uint32_t PON_TRNG_CLK_CTRL;                            /*!< Register,  Address offset:0x00C4 */
	__IO uint32_t PON_SEC_SCLK_CTRL;                            /*!< Register,  Address offset:0x00C8 */
	__IO uint32_t PON_ADC_CLK_CTRL;                             /*!< Register,  Address offset:0x00CC */
	__IO uint32_t PON_RFC_CLK_CTRL;                             /*!< Register,  Address offset:0x00D0 */
	__IO uint32_t PON_KM0_FW_CLK_CTRL;                          /*!< Register,  Address offset:0x00D4 */
	__IO uint32_t PON_KM4_NP_CLK_CTRL;                          /*!< Register,  Address offset:0x00D8 */
	__IO uint32_t PON_PSRAMC_CLK_CTRL;                          /*!< Register,  Address offset:0x00DC */
	__IO uint32_t PON_PWM_CLK_CTRL;                             /*!< Register,  Address offset:0x00E0 */
	__IO uint32_t PON_SLOW_PCLK_CTRL;                           /*!< Register,  Address offset:0x00E4 */
	__IO uint32_t PON_SLOW_SCLK_CTRL;                           /*!< Register,  Address offset:0x00E8 */
	__IO uint32_t PON_SPIC_CLK_CTRL;                            /*!< Register,  Address offset:0x00EC */
	__IO uint32_t PON_TIMER0_CLK_CTRL;                          /*!< Register,  Address offset:0x00F0 */
	__IO uint32_t PON_TIMER123_CLK_CTRL;                        /*!< Register,  Address offset:0x00F4 */
	__IO uint32_t PON_0_CLK_CTRL;                               /*!< Register,  Address offset:0x00F8 */
	__IO uint32_t PON_1_CLK_CTRL;                               /*!< Register,  Address offset:0x00FC */
	__IO uint32_t PON_2_CLK_CTRL;                               /*!< Register,  Address offset:0x0100 */
	__IO uint32_t PON_0_FUNC_CTRL;                              /*!< Register,  Address offset:0x0104 */
	__IO uint32_t PON_1_FUNC_CTRL;                              /*!< Register,  Address offset:0x0108 */
	__IO uint32_t PON_2_FUNC_CTRL;                              /*!< Register,  Address offset:0x010C */
	__I  uint32_t PON_DUMMY24;                                  /*!< Register,  Address offset:0x0110 */
	__IO uint32_t NPWDT_RST_CTRL;                               /*!< Register,  Address offset:0x0114 */
	__IO uint32_t FWWDT_RST_CTRL;                               /*!< Register,  Address offset:0x0118 */
	__I  uint32_t PON_DUMMY29;                                  /*!< Register,  Address offset:0x011C */
	__IO uint32_t PON_DUMMY30;                                  /*!< Register,  Address offset:0x0120 */
	__I  uint32_t PON_DUMMY31;                                  /*!< Register,  Address offset:0x0124 */
	__I  uint32_t PON_DUMMY32;                                  /*!< Register,  Address offset:0x0128 */
	__I  uint32_t PON_DUMMY25;                                  /*!< Register,  Address offset:0x012C */
	__I  uint32_t PON_DUMMY28;                                  /*!< Register,  Address offset:0x0130 */
	__I  uint32_t PON_DUMMY26;                                  /*!< Register,  Address offset:0x0134 */
	__IO uint32_t SYS_SPORT0_MUX;                               /*!< Register,  Address offset:0x0138 */
	__IO uint32_t SYS_SPORT1_MUX;                               /*!< Register,  Address offset:0x013C */
	__IO uint32_t SYS_ADC_CTRL0;                                /*!< Register,  Address offset:0x0140 */
	__IO uint32_t SYS_ADC_CTRL1;                                /*!< Register,  Address offset:0x0144 */
	__IO uint32_t SYS_PLATFORM_CTRL0;                           /*!< Register,  Address offset:0x0148 */
	__I  uint32_t PON_DUMMY27;                                  /*!< Register,  Address offset:0x014C */
	__I  uint32_t PON_DUMMY33;                                  /*!< Register,  Address offset:0x0150 */
	__IO uint32_t SYS_PSRAMPHY_CTRL;                            /*!< Register,  Address offset:0x0154 */
	__IO uint32_t SYS_FLASH_CTRL;                               /*!< Register,  Address offset:0x0158 */
	__IO uint32_t SYS_FLASH_PHY_CTRL0;                          /*!< Register,  Address offset:0x015C */
	__IO uint32_t SYS_FLASH_PHY_CTRL1;                          /*!< Register,  Address offset:0x0160 */
	__IO uint32_t SYS_FLASH_PHY_CTRL2;                          /*!< Register,  Address offset:0x0164 */
	__IO uint32_t SPIC_BPGM_CTR0;                               /*!< Register,  Address offset:0x0168 */
	__IO uint32_t SPIC_BPGM_CTR1;                               /*!< Register,  Address offset:0x016C */
	__IO uint32_t SPIC_BPGM_CTR2;                               /*!< Register,  Address offset:0x0170 */
	__IO uint32_t SPIC_BPGM_CTR3;                               /*!< Register,  Address offset:0x0174 */
	__IO uint32_t SPIC_BPGM_CTR4;                               /*!< Register,  Address offset:0x0178 */
	__IO uint32_t SPIC_BPGM_CTR5;                               /*!< Register,  Address offset:0x017C */
	__IO uint32_t SPIC_BPGM_CTR6;                               /*!< Register,  Address offset:0x0180 */
	__IO uint32_t SPIC_BPGM_CTR7;                               /*!< Register,  Address offset:0x0184 */
	__IO uint32_t SPIC_BPGM_CTR8;                               /*!< Register,  Address offset:0x0188 */
	__I  uint32_t PON_DUMMY7;                                   /*!< Register,  Address offset:0x018C */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY0_0;                    /*!< Register,  Address offset:0x0190 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY0_1;                    /*!< Register,  Address offset:0x0194 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY1_0;                    /*!< Register,  Address offset:0x0198 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY1_1;                    /*!< Register,  Address offset:0x019C */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY2_0;                    /*!< Register,  Address offset:0x01A0 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY2_1;                    /*!< Register,  Address offset:0x01A4 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY3_0;                    /*!< Register,  Address offset:0x01A8 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY3_1;                    /*!< Register,  Address offset:0x01AC */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY4_0;                    /*!< Register,  Address offset:0x01B0 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY4_1;                    /*!< Register,  Address offset:0x01B4 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY5_0;                    /*!< Register,  Address offset:0x01B8 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY5_1;                    /*!< Register,  Address offset:0x01BC */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY6_0;                    /*!< Register,  Address offset:0x01C0 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY6_1;                    /*!< Register,  Address offset:0x01C4 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY7_0;                    /*!< Register,  Address offset:0x01C8 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY7_1;                    /*!< Register,  Address offset:0x01CC */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY8_0;                    /*!< Register,  Address offset:0x01D0 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY8_1;                    /*!< Register,  Address offset:0x01D4 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY9_0;                    /*!< Register,  Address offset:0x01D8 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY9_1;                    /*!< Register,  Address offset:0x01DC */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY10_0;                   /*!< Register,  Address offset:0x01E0 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY10_1;                   /*!< Register,  Address offset:0x01E4 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY11_0;                   /*!< Register,  Address offset:0x01E8 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY11_1;                   /*!< Register,  Address offset:0x01EC */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY12_0;                   /*!< Register,  Address offset:0x01F0 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY12_1;                   /*!< Register,  Address offset:0x01F4 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY13_0;                   /*!< Register,  Address offset:0x01F8 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY13_1;                   /*!< Register,  Address offset:0x01FC */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY14_0;                   /*!< Register,  Address offset:0x0200 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY14_1;                   /*!< Register,  Address offset:0x0204 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY15_0;                   /*!< Register,  Address offset:0x0208 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY15_1;                   /*!< Register,  Address offset:0x020C */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY16_0;                   /*!< Register,  Address offset:0x0210 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY16_1;                   /*!< Register,  Address offset:0x0214 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY17_0;                   /*!< Register,  Address offset:0x0218 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY17_1;                   /*!< Register,  Address offset:0x021C */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY18_0;                   /*!< Register,  Address offset:0x0220 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY18_1;                   /*!< Register,  Address offset:0x0224 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY19_0;                   /*!< Register,  Address offset:0x0228 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY19_1;                   /*!< Register,  Address offset:0x022C */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY20_0;                   /*!< Register,  Address offset:0x0230 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY20_1;                   /*!< Register,  Address offset:0x0234 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY21_0;                   /*!< Register,  Address offset:0x0238 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY21_1;                   /*!< Register,  Address offset:0x023C */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY22_0;                   /*!< Register,  Address offset:0x0240 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY22_1;                   /*!< Register,  Address offset:0x0244 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY23_0;                   /*!< Register,  Address offset:0x0248 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY23_1;                   /*!< Register,  Address offset:0x024C */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY24_0;                   /*!< Register,  Address offset:0x0250 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY24_1;                   /*!< Register,  Address offset:0x0254 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY25_0;                   /*!< Register,  Address offset:0x0258 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY25_1;                   /*!< Register,  Address offset:0x025C */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY26_0;                   /*!< Register,  Address offset:0x0260 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY26_1;                   /*!< Register,  Address offset:0x0264 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY27_0;                   /*!< Register,  Address offset:0x0268 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY27_1;                   /*!< Register,  Address offset:0x026C */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY28_0;                   /*!< Register,  Address offset:0x0270 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY28_1;                   /*!< Register,  Address offset:0x0274 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY29_0;                   /*!< Register,  Address offset:0x0278 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY29_1;                   /*!< Register,  Address offset:0x027C */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY30_0;                   /*!< Register,  Address offset:0x0280 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY30_1;                   /*!< Register,  Address offset:0x0284 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY31_0;                   /*!< Register,  Address offset:0x0288 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY31_1;                   /*!< Register,  Address offset:0x028C */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY32_0;                   /*!< Register,  Address offset:0x0290 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY32_1;                   /*!< Register,  Address offset:0x0294 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY33_0;                   /*!< Register,  Address offset:0x0298 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY33_1;                   /*!< Register,  Address offset:0x029C */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY34_0;                   /*!< Register,  Address offset:0x02A0 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY34_1;                   /*!< Register,  Address offset:0x02A4 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY35_0;                   /*!< Register,  Address offset:0x02A8 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY35_1;                   /*!< Register,  Address offset:0x02AC */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY36_0;                   /*!< Register,  Address offset:0x02B0 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY36_1;                   /*!< Register,  Address offset:0x02B4 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY37_0;                   /*!< Register,  Address offset:0x02B8 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY37_1;                   /*!< Register,  Address offset:0x02BC */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY38_0;                   /*!< Register,  Address offset:0x02C0 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY38_1;                   /*!< Register,  Address offset:0x02C4 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY39_0;                   /*!< Register,  Address offset:0x02C8 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY39_1;                   /*!< Register,  Address offset:0x02CC */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY40_0;                   /*!< Register,  Address offset:0x02D0 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY40_1;                   /*!< Register,  Address offset:0x02D4 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY41_0;                   /*!< Register,  Address offset:0x02D8 */
	__IO uint32_t SYS1PLL_TRACKING_ENTRY41_1;                   /*!< Register,  Address offset:0x02DC */
	__IO uint32_t SYS1PLL_TRACKING_CTRL0;                       /*!< Register,  Address offset:0x02E0 */
	__IO uint32_t SYS1PLL_TRACKING_CTRL1;                       /*!< Register,  Address offset:0x02E4 */
	uint32_t RSVD_NO_FUNCTION0[6];
	__IO uint32_t SYS2PLL_TRACKING_ENTRY0_0;                    /*!< Register,  Address offset:0x0300 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY0_1;                    /*!< Register,  Address offset:0x0304 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY1_0;                    /*!< Register,  Address offset:0x0308 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY1_1;                    /*!< Register,  Address offset:0x030C */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY2_0;                    /*!< Register,  Address offset:0x0310 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY2_1;                    /*!< Register,  Address offset:0x0314 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY3_0;                    /*!< Register,  Address offset:0x0318 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY3_1;                    /*!< Register,  Address offset:0x031C */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY4_0;                    /*!< Register,  Address offset:0x0320 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY4_1;                    /*!< Register,  Address offset:0x0324 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY5_0;                    /*!< Register,  Address offset:0x0328 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY5_1;                    /*!< Register,  Address offset:0x032C */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY6_0;                    /*!< Register,  Address offset:0x0330 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY6_1;                    /*!< Register,  Address offset:0x0334 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY7_0;                    /*!< Register,  Address offset:0x0338 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY7_1;                    /*!< Register,  Address offset:0x033C */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY8_0;                    /*!< Register,  Address offset:0x0340 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY8_1;                    /*!< Register,  Address offset:0x0344 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY9_0;                    /*!< Register,  Address offset:0x0348 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY9_1;                    /*!< Register,  Address offset:0x034C */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY10_0;                   /*!< Register,  Address offset:0x0350 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY10_1;                   /*!< Register,  Address offset:0x0354 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY11_0;                   /*!< Register,  Address offset:0x0358 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY11_1;                   /*!< Register,  Address offset:0x035C */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY12_0;                   /*!< Register,  Address offset:0x0360 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY12_1;                   /*!< Register,  Address offset:0x0364 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY13_0;                   /*!< Register,  Address offset:0x0368 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY13_1;                   /*!< Register,  Address offset:0x036C */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY14_0;                   /*!< Register,  Address offset:0x0370 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY14_1;                   /*!< Register,  Address offset:0x0374 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY15_0;                   /*!< Register,  Address offset:0x0378 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY15_1;                   /*!< Register,  Address offset:0x037C */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY16_0;                   /*!< Register,  Address offset:0x0380 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY16_1;                   /*!< Register,  Address offset:0x0384 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY17_0;                   /*!< Register,  Address offset:0x0388 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY17_1;                   /*!< Register,  Address offset:0x038C */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY18_0;                   /*!< Register,  Address offset:0x0390 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY18_1;                   /*!< Register,  Address offset:0x0394 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY19_0;                   /*!< Register,  Address offset:0x0398 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY19_1;                   /*!< Register,  Address offset:0x039C */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY20_0;                   /*!< Register,  Address offset:0x03A0 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY20_1;                   /*!< Register,  Address offset:0x03A4 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY21_0;                   /*!< Register,  Address offset:0x03A8 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY21_1;                   /*!< Register,  Address offset:0x03AC */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY22_0;                   /*!< Register,  Address offset:0x03B0 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY22_1;                   /*!< Register,  Address offset:0x03B4 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY23_0;                   /*!< Register,  Address offset:0x03B8 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY23_1;                   /*!< Register,  Address offset:0x03BC */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY24_0;                   /*!< Register,  Address offset:0x03C0 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY24_1;                   /*!< Register,  Address offset:0x03C4 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY25_0;                   /*!< Register,  Address offset:0x03C8 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY25_1;                   /*!< Register,  Address offset:0x03CC */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY26_0;                   /*!< Register,  Address offset:0x03D0 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY26_1;                   /*!< Register,  Address offset:0x03D4 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY27_0;                   /*!< Register,  Address offset:0x03D8 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY27_1;                   /*!< Register,  Address offset:0x03DC */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY28_0;                   /*!< Register,  Address offset:0x03E0 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY28_1;                   /*!< Register,  Address offset:0x03E4 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY29_0;                   /*!< Register,  Address offset:0x03E8 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY29_1;                   /*!< Register,  Address offset:0x03EC */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY30_0;                   /*!< Register,  Address offset:0x03F0 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY30_1;                   /*!< Register,  Address offset:0x03F4 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY31_0;                   /*!< Register,  Address offset:0x03F8 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY31_1;                   /*!< Register,  Address offset:0x03FC */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY32_0;                   /*!< Register,  Address offset:0x0400 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY32_1;                   /*!< Register,  Address offset:0x0404 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY33_0;                   /*!< Register,  Address offset:0x0408 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY33_1;                   /*!< Register,  Address offset:0x040C */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY34_0;                   /*!< Register,  Address offset:0x0410 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY34_1;                   /*!< Register,  Address offset:0x0414 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY35_0;                   /*!< Register,  Address offset:0x0418 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY35_1;                   /*!< Register,  Address offset:0x041C */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY36_0;                   /*!< Register,  Address offset:0x0420 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY36_1;                   /*!< Register,  Address offset:0x0424 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY37_0;                   /*!< Register,  Address offset:0x0428 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY37_1;                   /*!< Register,  Address offset:0x042C */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY38_0;                   /*!< Register,  Address offset:0x0430 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY38_1;                   /*!< Register,  Address offset:0x0434 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY39_0;                   /*!< Register,  Address offset:0x0438 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY39_1;                   /*!< Register,  Address offset:0x043C */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY40_0;                   /*!< Register,  Address offset:0x0440 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY40_1;                   /*!< Register,  Address offset:0x0444 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY41_0;                   /*!< Register,  Address offset:0x0448 */
	__IO uint32_t SYS2PLL_TRACKING_ENTRY41_1;                   /*!< Register,  Address offset:0x044C */
	__IO uint32_t SYS2PLL_TRACKING_CTRL0;                       /*!< Register,  Address offset:0x0450 */
	__IO uint32_t SYS2PLL_TRACKING_CTRL1;                       /*!< Register,  Address offset:0x0454 */
	uint32_t RSVD_NO_FUNCTION1[2];
	__IO uint32_t PERIPLL_TRACKING_ENTRY0_0;                    /*!< Register,  Address offset:0x0460 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY0_1;                    /*!< Register,  Address offset:0x0464 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY1_0;                    /*!< Register,  Address offset:0x0468 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY1_1;                    /*!< Register,  Address offset:0x046C */
	__IO uint32_t PERIPLL_TRACKING_ENTRY2_0;                    /*!< Register,  Address offset:0x0470 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY2_1;                    /*!< Register,  Address offset:0x0474 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY3_0;                    /*!< Register,  Address offset:0x0478 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY3_1;                    /*!< Register,  Address offset:0x047C */
	__IO uint32_t PERIPLL_TRACKING_ENTRY4_0;                    /*!< Register,  Address offset:0x0480 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY4_1;                    /*!< Register,  Address offset:0x0484 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY5_0;                    /*!< Register,  Address offset:0x0488 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY5_1;                    /*!< Register,  Address offset:0x048C */
	__IO uint32_t PERIPLL_TRACKING_ENTRY6_0;                    /*!< Register,  Address offset:0x0490 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY6_1;                    /*!< Register,  Address offset:0x0494 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY7_0;                    /*!< Register,  Address offset:0x0498 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY7_1;                    /*!< Register,  Address offset:0x049C */
	__IO uint32_t PERIPLL_TRACKING_ENTRY8_0;                    /*!< Register,  Address offset:0x04A0 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY8_1;                    /*!< Register,  Address offset:0x04A4 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY9_0;                    /*!< Register,  Address offset:0x04A8 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY9_1;                    /*!< Register,  Address offset:0x04AC */
	__IO uint32_t PERIPLL_TRACKING_ENTRY10_0;                   /*!< Register,  Address offset:0x04B0 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY10_1;                   /*!< Register,  Address offset:0x04B4 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY11_0;                   /*!< Register,  Address offset:0x04B8 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY11_1;                   /*!< Register,  Address offset:0x04BC */
	__IO uint32_t PERIPLL_TRACKING_ENTRY12_0;                   /*!< Register,  Address offset:0x04C0 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY12_1;                   /*!< Register,  Address offset:0x04C4 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY13_0;                   /*!< Register,  Address offset:0x04C8 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY13_1;                   /*!< Register,  Address offset:0x04CC */
	__IO uint32_t PERIPLL_TRACKING_ENTRY14_0;                   /*!< Register,  Address offset:0x04D0 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY14_1;                   /*!< Register,  Address offset:0x04D4 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY15_0;                   /*!< Register,  Address offset:0x04D8 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY15_1;                   /*!< Register,  Address offset:0x04DC */
	__IO uint32_t PERIPLL_TRACKING_ENTRY16_0;                   /*!< Register,  Address offset:0x04E0 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY16_1;                   /*!< Register,  Address offset:0x04E4 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY17_0;                   /*!< Register,  Address offset:0x04E8 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY17_1;                   /*!< Register,  Address offset:0x04EC */
	__IO uint32_t PERIPLL_TRACKING_ENTRY18_0;                   /*!< Register,  Address offset:0x04F0 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY18_1;                   /*!< Register,  Address offset:0x04F4 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY19_0;                   /*!< Register,  Address offset:0x04F8 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY19_1;                   /*!< Register,  Address offset:0x04FC */
	__IO uint32_t PERIPLL_TRACKING_ENTRY20_0;                   /*!< Register,  Address offset:0x0500 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY20_1;                   /*!< Register,  Address offset:0x0504 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY21_0;                   /*!< Register,  Address offset:0x0508 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY21_1;                   /*!< Register,  Address offset:0x050C */
	__IO uint32_t PERIPLL_TRACKING_ENTRY22_0;                   /*!< Register,  Address offset:0x0510 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY22_1;                   /*!< Register,  Address offset:0x0514 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY23_0;                   /*!< Register,  Address offset:0x0518 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY23_1;                   /*!< Register,  Address offset:0x051C */
	__IO uint32_t PERIPLL_TRACKING_ENTRY24_0;                   /*!< Register,  Address offset:0x0520 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY24_1;                   /*!< Register,  Address offset:0x0524 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY25_0;                   /*!< Register,  Address offset:0x0528 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY25_1;                   /*!< Register,  Address offset:0x052C */
	__IO uint32_t PERIPLL_TRACKING_ENTRY26_0;                   /*!< Register,  Address offset:0x0530 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY26_1;                   /*!< Register,  Address offset:0x0534 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY27_0;                   /*!< Register,  Address offset:0x0538 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY27_1;                   /*!< Register,  Address offset:0x053C */
	__IO uint32_t PERIPLL_TRACKING_ENTRY28_0;                   /*!< Register,  Address offset:0x0540 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY28_1;                   /*!< Register,  Address offset:0x0544 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY29_0;                   /*!< Register,  Address offset:0x0548 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY29_1;                   /*!< Register,  Address offset:0x054C */
	__IO uint32_t PERIPLL_TRACKING_ENTRY30_0;                   /*!< Register,  Address offset:0x0550 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY30_1;                   /*!< Register,  Address offset:0x0554 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY31_0;                   /*!< Register,  Address offset:0x0558 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY31_1;                   /*!< Register,  Address offset:0x055C */
	__IO uint32_t PERIPLL_TRACKING_ENTRY32_0;                   /*!< Register,  Address offset:0x0560 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY32_1;                   /*!< Register,  Address offset:0x0564 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY33_0;                   /*!< Register,  Address offset:0x0568 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY33_1;                   /*!< Register,  Address offset:0x056C */
	__IO uint32_t PERIPLL_TRACKING_ENTRY34_0;                   /*!< Register,  Address offset:0x0570 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY34_1;                   /*!< Register,  Address offset:0x0574 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY35_0;                   /*!< Register,  Address offset:0x0578 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY35_1;                   /*!< Register,  Address offset:0x057C */
	__IO uint32_t PERIPLL_TRACKING_ENTRY36_0;                   /*!< Register,  Address offset:0x0580 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY36_1;                   /*!< Register,  Address offset:0x0584 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY37_0;                   /*!< Register,  Address offset:0x0588 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY37_1;                   /*!< Register,  Address offset:0x058C */
	__IO uint32_t PERIPLL_TRACKING_ENTRY38_0;                   /*!< Register,  Address offset:0x0590 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY38_1;                   /*!< Register,  Address offset:0x0594 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY39_0;                   /*!< Register,  Address offset:0x0598 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY39_1;                   /*!< Register,  Address offset:0x059C */
	__IO uint32_t PERIPLL_TRACKING_ENTRY40_0;                   /*!< Register,  Address offset:0x05A0 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY40_1;                   /*!< Register,  Address offset:0x05A4 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY41_0;                   /*!< Register,  Address offset:0x05A8 */
	__IO uint32_t PERIPLL_TRACKING_ENTRY41_1;                   /*!< Register,  Address offset:0x05AC */
	__IO uint32_t PERIPLL_TRACKING_CTRL0;                       /*!< Register,  Address offset:0x05B0 */
	__IO uint32_t PERIPLL_TRACKING_CTRL1;                       /*!< Register,  Address offset:0x05B4 */
	__IO uint32_t MBIST_BISR_CTRL0;                             /*!< Register,  Address offset:0x05B8 */
	__IO uint32_t MBIST_BISR_CTRL1;                             /*!< Register,  Address offset:0x05BC */
	__IO uint32_t MBIST_REMAP_SIG_0;                            /*!< Register,  Address offset:0x05C0 */
	__IO uint32_t MBIST_REMAP_SIG_1;                            /*!< Register,  Address offset:0x05C4 */
	__IO uint32_t MBIST_REMAP_SIG_2;                            /*!< Register,  Address offset:0x05C8 */
	__IO uint32_t MBIST_REMAP_SIG_3;                            /*!< Register,  Address offset:0x05CC */
	__IO uint32_t MBIST_REMAP_SIG_4;                            /*!< Register,  Address offset:0x05D0 */
	__IO uint32_t MBIST_REMAP_SIG_5;                            /*!< Register,  Address offset:0x05D4 */
	__IO uint32_t MBIST_REMAP_SIG_6;                            /*!< Register,  Address offset:0x05D8 */
	__IO uint32_t MBIST_REMAP_SIG_7;                            /*!< Register,  Address offset:0x05DC */
	__IO uint32_t MBIST_REMAP_SIG_8;                            /*!< Register,  Address offset:0x05E0 */
	uint32_t RSVD_NO_FUNCTION2[7];
	__IO uint32_t SYSON_SW_BOOT0;                               /*!< Register,  Address offset:0x0600 */
	__IO uint32_t SYSON_SW_BOOT1;                               /*!< Register,  Address offset:0x0604 */
	__IO uint32_t SYSON_SW_BOOT2;                               /*!< Register,  Address offset:0x0608 */
	__IO uint32_t SYSON_SW_BOOT3;                               /*!< Register,  Address offset:0x060C */
	__IO uint32_t SYSON_SW_BOOT4;                               /*!< Register,  Address offset:0x0610 */
	__IO uint32_t SYSON_SW_BOOT5;                               /*!< Register,  Address offset:0x0614 */
	__IO uint32_t SYSON_SW_BOOT6;                               /*!< Register,  Address offset:0x0618 */
	__IO uint32_t SYSON_SW_BOOT7;                               /*!< Register,  Address offset:0x061C */
	__IO uint32_t SYS_VERSION_STS;                              /*!< Register,  Address offset:0x0620 */
	uint32_t RSVD_NO_FUNCTION3[55];
	__IO uint32_t CPUPLL_CTRL0;                                 /*!< Register,  Address offset:0x0700 */
	__IO uint32_t CPUPLL_CTRL1;                                 /*!< Register,  Address offset:0x0704 */
	__IO uint32_t CPUPLL_CTRL2;                                 /*!< Register,  Address offset:0x0708 */
	__IO uint32_t CPUPLL_CTRL3;                                 /*!< Register,  Address offset:0x070C */
	__IO uint32_t CPUPLL_CTRL4;                                 /*!< Register,  Address offset:0x0710 */
	__IO uint32_t PON_SYS1PLL_CTRL0;                            /*!< Register,  Address offset:0x0714 */
	__IO uint32_t PON_SYS1PLL_CTRL1;                            /*!< Register,  Address offset:0x0718 */
	__IO uint32_t PON_SYS1PLL_CTRL2;                            /*!< Register,  Address offset:0x071C */
	__IO uint32_t PON_SYS1PLL_CTRL3;                            /*!< Register,  Address offset:0x0720 */
	__IO uint32_t PON_SYS1PLL_CTRL4;                            /*!< Register,  Address offset:0x0724 */
	__IO uint32_t PON_SYS2PLL_CTRL0;                            /*!< Register,  Address offset:0x0728 */
	__IO uint32_t PON_SYS2PLL_CTRL1;                            /*!< Register,  Address offset:0x072C */
	__IO uint32_t PON_SYS2PLL_CTRL2;                            /*!< Register,  Address offset:0x0730 */
	__IO uint32_t PON_SYS2PLL_CTRL3;                            /*!< Register,  Address offset:0x0734 */
	__IO uint32_t PON_SYS2PLL_CTRL4;                            /*!< Register,  Address offset:0x0738 */
	__IO uint32_t PON_PERIPLL_CTRL0;                            /*!< Register,  Address offset:0x073C */
	__IO uint32_t PON_PERIPLL_CTRL1;                            /*!< Register,  Address offset:0x0740 */
	__IO uint32_t PON_PERIPLL_CTRL2;                            /*!< Register,  Address offset:0x0744 */
	__IO uint32_t PON_PERIPLL_CTRL3;                            /*!< Register,  Address offset:0x0748 */
	__IO uint32_t PON_PERIPLL_CTRL4;                            /*!< Register,  Address offset:0x074C */
	__IO uint32_t PON_PLL_DBG;                                  /*!< Register,  Address offset:0x0750 */
	uint32_t RSVD_NO_FUNCTION4[3];
	__IO uint32_t BT_AFE_CTRL0;                                 /*!< Register,  Address offset:0x0760 */
	__IO uint32_t BT_AFE_CTRL1;                                 /*!< Register,  Address offset:0x0764 */
	__IO uint32_t BT_AFE_CTRL2;                                 /*!< Register,  Address offset:0x0768 */
	__IO uint32_t BT_AFE_CTRL3;                                 /*!< Register,  Address offset:0x076C */
} SYSON_TypeDef;

#define SYSON_BASE ((SYSON_TypeDef *) (0x40230000))
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
#endif  // SYSREG_SYSON_H
