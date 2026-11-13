/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_CA32_CTRL_H
#define SYSREG_CA32_CTRL_H

/* AUTO_GEN_START */

/* CA32_CTRL is a part of CA32_REG */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup CA32_CTRL
* @brief CA32_CTRL driver modules
* @{
*/


/** @defgroup REG_CA32_FEN_CTRL
 * @brief
 * @{
 */
#define CA32_CTRL_BIT_HPC_FSSCTRL_EN                ((u32)0x00000001 << 8)                   /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_HPC_FSSREC_FIFO_EN            ((u32)0x00000001 << 7)                   /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_HPC_FSSREC_EN                 ((u32)0x00000001 << 6)                   /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_HPC_FSS_EN                    ((u32)0x00000001 << 5)                   /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_CA32_TOP_FEN                  ((u32)0x00000001 << 4)                   /*!< R/W   0x0 Enable CA32 TOP. This bit works a cold reset to CA32 and other module in CA32 TOP wrapper. */
#define CA32_CTRL_BIT_CA32_CORE_FEN                 ((u32)0x00000001 << 3)                   /*!< R/W   0x0 Enable CA32 CORE. */
#define CA32_CTRL_BIT_CA32_L2_FEN                   ((u32)0x00000001 << 2)                   /*!< R/W   0x0 Enable CA32 L2 memory */
#define CA32_CTRL_BIT_AP_PLT_PERI_FEN               ((u32)0x00000001 << 1)                   /*!< R/W   0x0 Enable AP Platfrom, ca32_peri_clk domain */
#define CA32_CTRL_BIT_AP_PLT_MBUS_FEN               ((u32)0x00000001 << 0)                   /*!< R/W   0x0 Enable AP Platfrom, ca32_mbus_clk domain */
/** @} */


/** @defgroup REG_CA32_CLK_CTRL
 * @brief
 * @{
 */
#define CA32_CTRL_BIT_CLK_TRC_EN                    ((u32)0x00000001 << 17)                  /*!< R/W   0   None */
#define CA32_CTRL_BIT_CLK_TS_EN                     ((u32)0x00000001 << 16)                  /*!< R/W   0   None */
#define CA32_CTRL_BIT_XTAL_40M_SEL                  ((u32)0x00000001 << 14)                  /*!< R/W   0   None */
#define CA32_CTRL_MASK_TRCCLK_DIVN                  ((u32)0x0000003F << 8)                   /*!< R/W   0   Clock divider ratio */
#define CA32_CTRL_TRCCLK_DIVN(x)                    ((u32)(((x) & 0x0000003F) << 8))
#define CA32_CTRL_GET_TRCCLK_DIVN(x)                ((u32)(((x >> 8) & 0x0000003F)))
#define CA32_CTRL_BIT_TRCCLK_DIVTRG                 ((u32)0x00000001 << 7)                   /*!< R/W1P 0   Trigger pulse to start clock divider */
#define CA32_CTRL_MASK_TSCLK_DIVN                   ((u32)0x0000003F << 1)                   /*!< R/W   0   Clock divider ratio */
#define CA32_CTRL_TSCLK_DIVN(x)                     ((u32)(((x) & 0x0000003F) << 1))
#define CA32_CTRL_GET_TSCLK_DIVN(x)                 ((u32)(((x >> 1) & 0x0000003F)))
#define CA32_CTRL_BIT_TSCLK_DIVTRG                  ((u32)0x00000001 << 0)                   /*!< R/W1P 0   Trigger pulse to start clock divider */
/** @} */


/** @defgroup REG_CA32_RST_CTRL
 * @brief
 * @{
 */
#define CA32_CTRL_BIT_SW_PLF_RST_EN                 ((u32)0x00000001 << 18)                  /*!< R/W   0   Enable SW trigger to reset AP Platform */
#define CA32_CTRL_BIT_FWWDT_PLF_RST_EN              ((u32)0x00000001 << 17)                  /*!< R/W   0   Enable FWWDT trigger to reset AP Platform */
#define CA32_CTRL_BIT_MPWDT_PLF_RST_EN              ((u32)0x00000001 << 16)                  /*!< R/W   0   Enable MPWDT trigger to reset AP Platform */
#define CA32_CTRL_BIT_NPWDT_PLF_RST_EN              ((u32)0x00000001 << 15)                  /*!< R/W   0   Enable NPWDT trigger to reset AP Platform */
#define CA32_CTRL_BIT_APWDT_PLF_RST_EN              ((u32)0x00000001 << 14)                  /*!< R/W   0   Enable APWDT trigger to reset AP Platform */
#define CA32_CTRL_BIT_SW_L2_RST_EN                  ((u32)0x00000001 << 13)                  /*!< R/W   0   Enable SW trigger to reset CA32 L2 memory */
#define CA32_CTRL_BIT_SW_WARM_RST_EN                ((u32)0x00000001 << 12)                  /*!< R/W   0   Enable SW trigger to reset CA32 CPU CORE */
#define CA32_CTRL_BIT_SW_COLD_RST_EN                ((u32)0x00000001 << 11)                  /*!< R/W   0   Enable SW trigger to reset whole CA32 CPU TOP */
#define CA32_CTRL_BIT_MPWDT_L2_RST_EN               ((u32)0x00000001 << 10)                  /*!< R/W   0   Enable MP_WDT trigger to reset CA32 L2 memory */
#define CA32_CTRL_BIT_MPWDT_WARM_RST_EN             ((u32)0x00000001 << 9)                   /*!< R/W   0   Enable MP_WDT trigger to reset CA32 CPU CORE */
#define CA32_CTRL_BIT_MPWDT_COLD_RST_EN             ((u32)0x00000001 << 8)                   /*!< R/W   0   Enable MP_WDT trigger to reset whole CA32 CPU TOP */
#define CA32_CTRL_BIT_FWWDT_L2_RST_EN               ((u32)0x00000001 << 7)                   /*!< R/W   0   Enable FW_WDT trigger to reset CA32 L2 memory */
#define CA32_CTRL_BIT_FWWDT_WARM_RST_EN             ((u32)0x00000001 << 6)                   /*!< R/W   0   Enable FW_WDT trigger to reset CA32 CPU CORE */
#define CA32_CTRL_BIT_FWWDT_COLD_RST_EN             ((u32)0x00000001 << 5)                   /*!< R/W   0   Enable FW_WDT trigger to reset whole CA32 CPU TOP */
#define CA32_CTRL_BIT_NPWDT_L2_RST_EN               ((u32)0x00000001 << 4)                   /*!< R/W   0   Enable NP_WDT trigger to reset CA32 L2 memory */
#define CA32_CTRL_BIT_NPWDT_WARM_RST_EN             ((u32)0x00000001 << 3)                   /*!< R/W   0   Enable NP_WDT trigger to reset CA32 CPU CORE */
#define CA32_CTRL_BIT_NPWDT_COLD_RST_EN             ((u32)0x00000001 << 2)                   /*!< R/W   0   Enable NP_WDT trigger to reset whole CA32 CPU TOP */
#define CA32_CTRL_BIT_APWDT_L2_RST_EN               ((u32)0x00000001 << 1)                   /*!< R/W   0   Enable AP_WDT trigger to reset CA32 L2 memory */
#define CA32_CTRL_BIT_APWDT_COLD_RST_EN             ((u32)0x00000001 << 0)                   /*!< R/W   0   Enable AP_WDT trigger to reset whole CA32 CPU TOP */
/** @} */


/** @defgroup REG_CA32_PWR_CTRL
 * @brief
 * @{
 */
#define CA32_CTRL_BIT_EVENTI                        ((u32)0x00000001 << 10)                  /*!< R/W   0   None */
#define CA32_CTRL_BIT_DBGPWRDUP                     ((u32)0x00000001 << 9)                   /*!< R/W   1  
                                                                                              * Core powered up:
                                                                                              * 0: Core powered down
                                                                                              * 1: Core powered up
                                                                                              */
#define CA32_CTRL_BIT_L2FLUSHREQ                    ((u32)0x00000001 << 8)                   /*!< R/W   0   L2 hardware flush request. */
#define CA32_CTRL_BIT_DBGPWRUPREQ                   ((u32)0x00000001 << 6)                   /*!< R     0  
                                                                                              * Core powered up request:
                                                                                              * 0: No not request core power
                                                                                              * 1: Request that core is power powered up
                                                                                              */
#define CA32_CTRL_BIT_DBGNOPWRDWN                   ((u32)0x00000001 << 5)                   /*!< R     0  
                                                                                              * Core no powerdown request:
                                                                                              * 0: Do not request that the core stays powered up.
                                                                                              * 1: Request that the core stays powered up.
                                                                                              */
#define CA32_CTRL_BIT_SMPEN                         ((u32)0x00000001 << 4)                   /*!< R     0   Indicate whether a core is taking part in coherency. */
#define CA32_CTRL_BIT_L2FLUSHDONE                   ((u32)0x00000001 << 3)                   /*!< R     0   L2 hardware flush complete. */
#define CA32_CTRL_BIT_STANDBYWFIL2                  ((u32)0x00000001 << 2)                   /*!< R     0   Indicates whether the L2 memory system is in WFI low-power state. This signal is active in some defined conditions. Refer to ARM implementation document for the details. */
#define CA32_CTRL_BIT_STANDBYWFE                    ((u32)0x00000001 << 1)                   /*!< R     0  
                                                                                              * Indicates whether a core is in WFE low-power state.
                                                                                              * 0: Core not in WFE
                                                                                              * 1: Core in WFE.
                                                                                              */
#define CA32_CTRL_BIT_STANDBYWFI                    ((u32)0x00000001 << 0)                   /*!< R     0  
                                                                                              * Indicates whether a core is in WFI low-power state.
                                                                                              * 0: Core not in WFI
                                                                                              * 1: Core in WFI. This is the reset condition
                                                                                              */
/** @} */


/** @defgroup REG_CA32_CFG0
 * @brief
 * @{
 */
#define CA32_CTRL_BIT_DBGL1RSTDISABLE               ((u32)0x00000001 << 6)                   /*!< R/W   0  
                                                                                              * Disable L1 data cache automatic invalidate on reset functionality:
                                                                                              * 0: Enable automatic invalidation of L1 data cache on reset.
                                                                                              * 1: Disable automatic invalidation of L1 data cache on reset.
                                                                                              * This pin is sampled only during reset of core.
                                                                                              */
#define CA32_CTRL_BIT_L2RSTDISABLE                  ((u32)0x00000001 << 5)                   /*!< R/W   0  
                                                                                              * Disable automatic L2 cache invalidate at reset.
                                                                                              * 0: Enable L2 cache invalidate at reset.
                                                                                              * 1: Disable L2 cache invalidate at reset.
                                                                                              * Only change it when the core is in the reset state.
                                                                                              */
#define CA32_CTRL_BIT_VINITHI                       ((u32)0x00000001 << 4)                   /*!< R/W   0  
                                                                                              * Location of the exception vectors at reset. It sets the initial value of the V bit in the SCTLR and HSCTLR registers:
                                                                                              * 0: Exception vectors start at 0x0
                                                                                              * 1: Exception vectors start at 0xFFFF_0000
                                                                                              * This pin is sampled only during reset.
                                                                                              */
#define CA32_CTRL_BIT_CRYPTODISABLE                 ((u32)0x00000001 << 3)                   /*!< R/W     0
                                                                                              * Disable the Cryptographic Extension. 
                                                                                              * 0: Enable the Cryptographic Extension
                                                                                              * 1: Disable the Cryptographic Extension
                                                                                              * This pin is sampled only during reset.
                                                                                              */
#define CA32_CTRL_BIT_CP15DISABLE2                  ((u32)0x00000001 << 2)                   /*!< R/W   0  
                                                                                              * Disable write access to some Secure CPU15 regsiters
                                                                                              * 0: Enable all CP15 registers
                                                                                              * 1: Disable access to some Secure CP15 register.
                                                                                              */
#define CA32_CTRL_BIT_CFGTE                         ((u32)0x00000001 << 1)                   /*!< R/W   0  
                                                                                              * Enable T32 exceptions. It sets the initial value of teh TE bit in the SCTLR and HSCTLR regsiters:
                                                                                              * 0: TE bit is LOW
                                                                                              * 1: TE bit is HIGH
                                                                                              * This pin is sampled only during reset of the core.
                                                                                              */
#define CA32_CTRL_BIT_CFGEND                        ((u32)0x00000001 << 0)                   /*!< R/W     0
                                                                                              * Endiannes config at reset. It sets the initial value of the EE bits in the SCTLR, HSCTLR, SCTLR_EL1, SCTLR_EL2, and SCTLR_EL3 registers:
                                                                                              * 0: EE bit is LOW
                                                                                              * 1: EE bit is HIGH
                                                                                              * This pin is sampled only during reset.
                                                                                              */
/** @} */


/** @defgroup REG_CA32_SPARE0
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_CA32_SPARE0                  ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0   SPARE for SW */
#define CA32_CTRL_CA32_SPARE0(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CA32_CTRL_GET_CA32_SPARE0(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CA32_PMU_EVENT
 * @brief
 * @{
 */
#define CA32_CTRL_BIT_NPMUIRQ                       ((u32)0x00000001 << 30)                  /*!< R     0x0 None */
#define CA32_CTRL_MASK_PMUEVENT0                    ((u32)0x3FFFFFFF << 0)                   /*!< R     0x0 None */
#define CA32_CTRL_PMUEVENT0(x)                      ((u32)(((x) & 0x3FFFFFFF) << 0))
#define CA32_CTRL_GET_PMUEVENT0(x)                  ((u32)(((x >> 0) & 0x3FFFFFFF)))
/** @} */


/** @defgroup REG_CA32_GIC_CTRL
 * @brief
 * @{
 */
#define CA32_CTRL_BIT_CFGSDISABLE                   ((u32)0x00000001 << 0)                   /*!< R/W Once 0   None */
/** @} */


/** @defgroup REG_CA32_REMAP
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_CA32_REMAP_ADDR              ((u32)0x0000FFFF << 0)                   /*!< RW      0 None */
#define CA32_CTRL_CA32_REMAP_ADDR(x)                ((u32)(((x) & 0x0000FFFF) << 0))
#define CA32_CTRL_GET_CA32_REMAP_ADDR(x)            ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_CA32_SRAM_CTRL
 * @brief
 * @{
 */
#define CA32_CTRL_BIT_TEST1                         ((u32)0x00000001 << 25)                  /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_RM2                           ((u32)0x00000001 << 24)                  /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_RM3                           ((u32)0x00000001 << 23)                  /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_FUNC_LS_ENABLE                ((u32)0x00000001 << 22)                  /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_LS                            ((u32)0x00000001 << 21)                  /*!< R/W   0x0 None */
#define CA32_CTRL_MASK_RM_L2_VIC                    ((u32)0x00000003 << 19)                  /*!< R/W   0x2 None */
#define CA32_CTRL_RM_L2_VIC(x)                      ((u32)(((x) & 0x00000003) << 19))
#define CA32_CTRL_GET_RM_L2_VIC(x)                  ((u32)(((x >> 19) & 0x00000003)))
#define CA32_CTRL_MASK_RM_L2_DATA                   ((u32)0x00000003 << 17)                  /*!< R/W   0x2 None */
#define CA32_CTRL_RM_L2_DATA(x)                     ((u32)(((x) & 0x00000003) << 17))
#define CA32_CTRL_GET_RM_L2_DATA(x)                 ((u32)(((x >> 17) & 0x00000003)))
#define CA32_CTRL_MASK_RM_L2_TAG                    ((u32)0x00000003 << 15)                  /*!< R/W   0x2 None */
#define CA32_CTRL_RM_L2_TAG(x)                      ((u32)(((x) & 0x00000003) << 15))
#define CA32_CTRL_GET_RM_L2_TAG(x)                  ((u32)(((x >> 15) & 0x00000003)))
#define CA32_CTRL_MASK_RM_SCU_TAG                   ((u32)0x00000003 << 13)                  /*!< R/W   0x2 None */
#define CA32_CTRL_RM_SCU_TAG(x)                     ((u32)(((x) & 0x00000003) << 13))
#define CA32_CTRL_GET_RM_SCU_TAG(x)                 ((u32)(((x >> 13) & 0x00000003)))
#define CA32_CTRL_MASK_RM_TLB                       ((u32)0x00000003 << 11)                  /*!< R/W   0x2 None */
#define CA32_CTRL_RM_TLB(x)                         ((u32)(((x) & 0x00000003) << 11))
#define CA32_CTRL_GET_RM_TLB(x)                     ((u32)(((x >> 11) & 0x00000003)))
#define CA32_CTRL_MASK_RM_ITAG                      ((u32)0x00000003 << 9)                   /*!< R/W   0x2 None */
#define CA32_CTRL_RM_ITAG(x)                        ((u32)(((x) & 0x00000003) << 9))
#define CA32_CTRL_GET_RM_ITAG(x)                    ((u32)(((x >> 9) & 0x00000003)))
#define CA32_CTRL_MASK_RM_IDATA                     ((u32)0x00000003 << 7)                   /*!< R/W   0x2 None */
#define CA32_CTRL_RM_IDATA(x)                       ((u32)(((x) & 0x00000003) << 7))
#define CA32_CTRL_GET_RM_IDATA(x)                   ((u32)(((x >> 7) & 0x00000003)))
#define CA32_CTRL_MASK_RM_DDIRTY                    ((u32)0x00000003 << 5)                   /*!< R/W   0x2 None */
#define CA32_CTRL_RM_DDIRTY(x)                      ((u32)(((x) & 0x00000003) << 5))
#define CA32_CTRL_GET_RM_DDIRTY(x)                  ((u32)(((x >> 5) & 0x00000003)))
#define CA32_CTRL_MASK_RM_DTAG                      ((u32)0x00000003 << 3)                   /*!< R/W   0x2 None */
#define CA32_CTRL_RM_DTAG(x)                        ((u32)(((x) & 0x00000003) << 3))
#define CA32_CTRL_GET_RM_DTAG(x)                    ((u32)(((x >> 3) & 0x00000003)))
#define CA32_CTRL_MASK_RM_DDATA                     ((u32)0x00000003 << 1)                   /*!< R/W   0x2 None */
#define CA32_CTRL_RM_DDATA(x)                       ((u32)(((x) & 0x00000003) << 1))
#define CA32_CTRL_GET_RM_DDATA(x)                   ((u32)(((x >> 1) & 0x00000003)))
#define CA32_CTRL_BIT_RME                           ((u32)0x00000001 << 0)                   /*!< RW    0x0 None */
/** @} */


/** @defgroup REG_CA32_FSS_CTRL0
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_FSS_CONFIG2_CDL2             ((u32)0x0000001F << 24)                  /*!< R/W   0x0 None */
#define CA32_CTRL_FSS_CONFIG2_CDL2(x)               ((u32)(((x) & 0x0000001F) << 24))
#define CA32_CTRL_GET_FSS_CONFIG2_CDL2(x)           ((u32)(((x >> 24) & 0x0000001F)))
#define CA32_CTRL_MASK_FSS_CONFIG2_CDL1             ((u32)0x0000001F << 19)                  /*!< R/W   0x0 None */
#define CA32_CTRL_FSS_CONFIG2_CDL1(x)               ((u32)(((x) & 0x0000001F) << 19))
#define CA32_CTRL_GET_FSS_CONFIG2_CDL1(x)           ((u32)(((x >> 19) & 0x0000001F)))
#define CA32_CTRL_MASK_FSS_CONFIG1_CDL2             ((u32)0x0000001F << 14)                  /*!< R/W   0x0 None */
#define CA32_CTRL_FSS_CONFIG1_CDL2(x)               ((u32)(((x) & 0x0000001F) << 14))
#define CA32_CTRL_GET_FSS_CONFIG1_CDL2(x)           ((u32)(((x >> 14) & 0x0000001F)))
#define CA32_CTRL_MASK_FSS_CONFIG1_CDL1             ((u32)0x0000001F << 9)                   /*!< R/W   0x0 None */
#define CA32_CTRL_FSS_CONFIG1_CDL1(x)               ((u32)(((x) & 0x0000001F) << 9))
#define CA32_CTRL_GET_FSS_CONFIG1_CDL1(x)           ((u32)(((x >> 9) & 0x0000001F)))
#define CA32_CTRL_MASK_FSS_CONFIG_CDL0              ((u32)0x0000000F << 5)                   /*!< R/W   0x0 None */
#define CA32_CTRL_FSS_CONFIG_CDL0(x)                ((u32)(((x) & 0x0000000F) << 5))
#define CA32_CTRL_GET_FSS_CONFIG_CDL0(x)            ((u32)(((x >> 5) & 0x0000000F)))
#define CA32_CTRL_BIT_FSS_CAL_EN                    ((u32)0x00000001 << 4)                   /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_FSS_FALL_EDGE_SEL             ((u32)0x00000001 << 3)                   /*!< R/W   0x0 None */
#define CA32_CTRL_MASK_FSS_PATH_SEL                 ((u32)0x00000003 << 1)                   /*!< R/W   0x0 None */
#define CA32_CTRL_FSS_PATH_SEL(x)                   ((u32)(((x) & 0x00000003) << 1))
#define CA32_CTRL_GET_FSS_PATH_SEL(x)               ((u32)(((x >> 1) & 0x00000003)))
#define CA32_CTRL_BIT_FFS_SENSOR_EN                 ((u32)0x00000001 << 0)                   /*!< R/W   0x0 None */
/** @} */


/** @defgroup REG_CA32_FSS_CTRL1
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_RM_FSS                       ((u32)0x00000003 << 25)                  /*!< RW    0x2 None */
#define CA32_CTRL_RM_FSS(x)                         ((u32)(((x) & 0x00000003) << 25))
#define CA32_CTRL_GET_RM_FSS(x)                     ((u32)(((x >> 25) & 0x00000003)))
#define CA32_CTRL_BIT_FSS_LOW_FREQ_MODE             ((u32)0x00000001 << 24)                  /*!< RW    0x0 None */
#define CA32_CTRL_MASK_FSS_DEG_FILTER_L2H           ((u32)0x00000007 << 21)                  /*!< RW    0x0 None */
#define CA32_CTRL_FSS_DEG_FILTER_L2H(x)             ((u32)(((x) & 0x00000007) << 21))
#define CA32_CTRL_GET_FSS_DEG_FILTER_L2H(x)         ((u32)(((x >> 21) & 0x00000007)))
#define CA32_CTRL_MASK_FSS_DEG_FILTER_H2L           ((u32)0x00000007 << 18)                  /*!< RW    0x0 None */
#define CA32_CTRL_FSS_DEG_FILTER_H2L(x)             ((u32)(((x) & 0x00000007) << 18))
#define CA32_CTRL_GET_FSS_DEG_FILTER_H2L(x)         ((u32)(((x >> 18) & 0x00000007)))
#define CA32_CTRL_MASK_FSS_DEG_FILTER_L2H_SLOPE     ((u32)0x00000007 << 15)                  /*!< RW    0x0 None */
#define CA32_CTRL_FSS_DEG_FILTER_L2H_SLOPE(x)       ((u32)(((x) & 0x00000007) << 15))
#define CA32_CTRL_GET_FSS_DEG_FILTER_L2H_SLOPE(x)   ((u32)(((x >> 15) & 0x00000007)))
#define CA32_CTRL_MASK_FSS_DEG_FILTER_H2L_SLOPE     ((u32)0x00000007 << 12)                  /*!< RW    0x0 None */
#define CA32_CTRL_FSS_DEG_FILTER_H2L_SLOPE(x)       ((u32)(((x) & 0x00000007) << 12))
#define CA32_CTRL_GET_FSS_DEG_FILTER_H2L_SLOPE(x)   ((u32)(((x >> 12) & 0x00000007)))
#define CA32_CTRL_MASK_FSS_CONFIG_SLOPE_SAMPLE_CYCLE ((u32)0x00000003 << 10)                  /*!< RW    0x0 None */
#define CA32_CTRL_FSS_CONFIG_SLOPE_SAMPLE_CYCLE(x)  ((u32)(((x) & 0x00000003) << 10))
#define CA32_CTRL_GET_FSS_CONFIG_SLOPE_SAMPLE_CYCLE(x) ((u32)(((x >> 10) & 0x00000003)))
#define CA32_CTRL_MASK_FSS_CONFIG_SLOPE_DETECT_THESHOLD ((u32)0x0000001F << 5)                   /*!< RW    0x0 None */
#define CA32_CTRL_FSS_CONFIG_SLOPE_DETECT_THESHOLD(x) ((u32)(((x) & 0x0000001F) << 5))
#define CA32_CTRL_GET_FSS_CONFIG_SLOPE_DETECT_THESHOLD(x) ((u32)(((x >> 5) & 0x0000001F)))
#define CA32_CTRL_MASK_FSS_CONFIG_SLOPE_RECOVERY_THESHOLD ((u32)0x0000001F << 0)                   /*!< RW    0x0 None */
#define CA32_CTRL_FSS_CONFIG_SLOPE_RECOVERY_THESHOLD(x) ((u32)(((x) & 0x0000001F) << 0))
#define CA32_CTRL_GET_FSS_CONFIG_SLOPE_RECOVERY_THESHOLD(x) ((u32)(((x >> 0) & 0x0000001F)))
/** @} */


/** @defgroup REG_CA32_FSS_STS
 * @brief
 * @{
 */
#define CA32_CTRL_BIT_FSS_SLOPE_DETECT_OUT          ((u32)0x00000001 << 16)                  /*!< R     0   None */
#define CA32_CTRL_BIT_FSS_CAL_DONE                  ((u32)0x00000001 << 15)                  /*!< R     0   None */
#define CA32_CTRL_MASK_FSS_CAL_MIN_CDL1             ((u32)0x0000001F << 10)                  /*!< R     0   None */
#define CA32_CTRL_FSS_CAL_MIN_CDL1(x)               ((u32)(((x) & 0x0000001F) << 10))
#define CA32_CTRL_GET_FSS_CAL_MIN_CDL1(x)           ((u32)(((x >> 10) & 0x0000001F)))
#define CA32_CTRL_MASK_FSS_CAL_CDL0                 ((u32)0x0000000F << 6)                   /*!< R     0   None */
#define CA32_CTRL_FSS_CAL_CDL0(x)                   ((u32)(((x) & 0x0000000F) << 6))
#define CA32_CTRL_GET_FSS_CAL_CDL0(x)               ((u32)(((x >> 6) & 0x0000000F)))
#define CA32_CTRL_BIT_FSS_DETECT_2_OUT              ((u32)0x00000001 << 5)                   /*!< R     0   None */
#define CA32_CTRL_BIT_FSS_DETECT_2_2                ((u32)0x00000001 << 4)                   /*!< R     0   None */
#define CA32_CTRL_BIT_FSS_DETECT_2_1                ((u32)0x00000001 << 3)                   /*!< R     0   None */
#define CA32_CTRL_BIT_FSS_DETECT1_OUT               ((u32)0x00000001 << 2)                   /*!< R     0   None */
#define CA32_CTRL_BIT_FSS_DETECT1_2                 ((u32)0x00000001 << 1)                   /*!< R     0   None */
#define CA32_CTRL_BIT_FSS_DETECT1_1                 ((u32)0x00000001 << 0)                   /*!< R     0   None */
/** @} */


/** @defgroup REG_CA32_FSS_RECODER_CTRL
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_FSS_PRERECORDER_CYCLE_CONFIG ((u32)0x000000FF << 17)                  /*!< R/W   0x0 None */
#define CA32_CTRL_FSS_PRERECORDER_CYCLE_CONFIG(x)   ((u32)(((x) & 0x000000FF) << 17))
#define CA32_CTRL_GET_FSS_PRERECORDER_CYCLE_CONFIG(x) ((u32)(((x >> 17) & 0x000000FF)))
#define CA32_CTRL_BIT_FSS_PRERECORDER_EN            ((u32)0x00000001 << 16)                  /*!< R/W   0x0 None */
#define CA32_CTRL_MASK_FSSCTRL_SEL                  ((u32)0x00000003 << 9)                   /*!< R/W   0x0 None */
#define CA32_CTRL_FSSCTRL_SEL(x)                    ((u32)(((x) & 0x00000003) << 9))
#define CA32_CTRL_GET_FSSCTRL_SEL(x)                ((u32)(((x >> 9) & 0x00000003)))
#define CA32_CTRL_BIT_FSSCTRL_RECORDER_EN           ((u32)0x00000001 << 8)                   /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_FSSREC_USER_DETECT            ((u32)0x00000001 << 7)                   /*!< R/W   0x0 None */
#define CA32_CTRL_MASK_FSSREC_RECORDER_INTERVAL     ((u32)0x0000003F << 1)                   /*!< R/W   0x0 None */
#define CA32_CTRL_FSSREC_RECORDER_INTERVAL(x)       ((u32)(((x) & 0x0000003F) << 1))
#define CA32_CTRL_GET_FSSREC_RECORDER_INTERVAL(x)   ((u32)(((x >> 1) & 0x0000003F)))
#define CA32_CTRL_BIT_FSSREC_RECORDER_EN            ((u32)0x00000001 << 0)                   /*!< RW    0x0 None */
/** @} */


/** @defgroup REG_CA32_FSSCTRL_SRAM
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_FSSCTRL_EXT_READ_DATA        ((u32)0x00FFFFFF << 8)                   /*!< R     0x0 None */
#define CA32_CTRL_FSSCTRL_EXT_READ_DATA(x)          ((u32)(((x) & 0x00FFFFFF) << 8))
#define CA32_CTRL_GET_FSSCTRL_EXT_READ_DATA(x)      ((u32)(((x >> 8) & 0x00FFFFFF)))
#define CA32_CTRL_BIT_FSSCTRL_SRAM_CTRL_DONE        ((u32)0x00000001 << 5)                   /*!< R     0x0 None */
#define CA32_CTRL_BIT_FSSCTRL_EXT_READ_RDY          ((u32)0x00000001 << 4)                   /*!< R     0x0 None */
#define CA32_CTRL_BIT_FSSCTRL_EXT_READ_PULSE        ((u32)0x00000001 << 0)                   /*!< R/W   0x0 None */
/** @} */


/** @defgroup REG_TPIU_CTRL
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_TPMAXDATASIZE                ((u32)0x0000001F << 1)                   /*!< R/W   0x3 None */
#define CA32_CTRL_TPMAXDATASIZE(x)                  ((u32)(((x) & 0x0000001F) << 1))
#define CA32_CTRL_GET_TPMAXDATASIZE(x)              ((u32)(((x >> 1) & 0x0000001F)))
#define CA32_CTRL_BIT_TPCTL                         ((u32)0x00000001 << 0)                   /*!< R/W   0x0 None */
/** @} */


/** @defgroup REG_MBIST_CTRL0
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_HPC_SHRBUS_EN_CPU            ((u32)0x00000003 << 29)                  /*!< R/W   0x0 None */
#define CA32_CTRL_HPC_SHRBUS_EN_CPU(x)              ((u32)(((x) & 0x00000003) << 29))
#define CA32_CTRL_GET_HPC_SHRBUS_EN_CPU(x)          ((u32)(((x >> 29) & 0x00000003)))
#define CA32_CTRL_BIT_HPC_SHRBUS_GO                 ((u32)0x00000001 << 28)                  /*!< R/W   0x0 None */
#define CA32_CTRL_MASK_HPC_MBISTCFG_SEL             ((u32)0x00000003 << 26)                  /*!< R/W   0x0 None */
#define CA32_CTRL_HPC_MBISTCFG_SEL(x)               ((u32)(((x) & 0x00000003) << 26))
#define CA32_CTRL_GET_HPC_MBISTCFG_SEL(x)           ((u32)(((x >> 26) & 0x00000003)))
#define CA32_CTRL_MASK_HPC_MBISTREQ_DBG             ((u32)0x00000003 << 24)                  /*!< R     0x0 None */
#define CA32_CTRL_HPC_MBISTREQ_DBG(x)               ((u32)(((x) & 0x00000003) << 24))
#define CA32_CTRL_GET_HPC_MBISTREQ_DBG(x)           ((u32)(((x >> 24) & 0x00000003)))
#define CA32_CTRL_MASK__HPC_MBISTACK_DBG            ((u32)0x00000003 << 22)                  /*!< R     0x0 None */
#define CA32_CTRL__HPC_MBISTACK_DBG(x)              ((u32)(((x) & 0x00000003) << 22))
#define CA32_CTRL_GET__HPC_MBISTACK_DBG(x)          ((u32)(((x >> 22) & 0x00000003)))
#define CA32_CTRL_MASK_HPC_MBIST_FNSH               ((u32)0x00000003 << 20)                  /*!< R     0x0 None */
#define CA32_CTRL_HPC_MBIST_FNSH(x)                 ((u32)(((x) & 0x00000003) << 20))
#define CA32_CTRL_GET_HPC_MBIST_FNSH(x)             ((u32)(((x >> 20) & 0x00000003)))
#define CA32_CTRL_MASK_HPC_DRF_BIST_MODE            ((u32)0x00000003 << 18)                  /*!< R/W   0x0 None */
#define CA32_CTRL_HPC_DRF_BIST_MODE(x)              ((u32)(((x) & 0x00000003) << 18))
#define CA32_CTRL_GET_HPC_DRF_BIST_MODE(x)          ((u32)(((x >> 18) & 0x00000003)))
#define CA32_CTRL_MASK_HPC_DRF_TEST_RESUME          ((u32)0x00000003 << 16)                  /*!< R/W   0x0 None */
#define CA32_CTRL_HPC_DRF_TEST_RESUME(x)            ((u32)(((x) & 0x00000003) << 16))
#define CA32_CTRL_GET_HPC_DRF_TEST_RESUME(x)        ((u32)(((x >> 16) & 0x00000003)))
#define CA32_CTRL_MASK_HPC_DRF_START_PAUSE          ((u32)0x00000003 << 14)                  /*!< R     0x0 None */
#define CA32_CTRL_HPC_DRF_START_PAUSE(x)            ((u32)(((x) & 0x00000003) << 14))
#define CA32_CTRL_GET_HPC_DRF_START_PAUSE(x)        ((u32)(((x >> 14) & 0x00000003)))
#define CA32_CTRL_MASK_HPC_DRF_BIST_DONE            ((u32)0x00000003 << 12)                  /*!< R     0x0 None */
#define CA32_CTRL_HPC_DRF_BIST_DONE(x)              ((u32)(((x) & 0x00000003) << 12))
#define CA32_CTRL_GET_HPC_DRF_BIST_DONE(x)          ((u32)(((x >> 12) & 0x00000003)))
#define CA32_CTRL_MASK_HPC_DRF_DYN_READ_EN          ((u32)0x00000003 << 10)                  /*!< R/W   0x0 None */
#define CA32_CTRL_HPC_DRF_DYN_READ_EN(x)            ((u32)(((x) & 0x00000003) << 10))
#define CA32_CTRL_GET_HPC_DRF_DYN_READ_EN(x)        ((u32)(((x >> 10) & 0x00000003)))
#define CA32_CTRL_MASK_HPC_DRF_DBUG_HL_WR           ((u32)0x00000003 << 8)                   /*!< R/W   0x0 None */
#define CA32_CTRL_HPC_DRF_DBUG_HL_WR(x)             ((u32)(((x) & 0x00000003) << 8))
#define CA32_CTRL_GET_HPC_DRF_DBUG_HL_WR(x)         ((u32)(((x >> 8) & 0x00000003)))
#define CA32_CTRL_MASK_HPC_BIST_MODE_ALL            ((u32)0x00000003 << 6)                   /*!< R/W   0x0 None */
#define CA32_CTRL_HPC_BIST_MODE_ALL(x)              ((u32)(((x) & 0x00000003) << 6))
#define CA32_CTRL_GET_HPC_BIST_MODE_ALL(x)          ((u32)(((x >> 6) & 0x00000003)))
#define CA32_CTRL_MASK_HPC_BIST_LOOP_MODE           ((u32)0x0000000F << 2)                   /*!< R/W   0x0 None */
#define CA32_CTRL_HPC_BIST_LOOP_MODE(x)             ((u32)(((x) & 0x0000000F) << 2))
#define CA32_CTRL_GET_HPC_BIST_LOOP_MODE(x)         ((u32)(((x >> 2) & 0x0000000F)))
#define CA32_CTRL_MASK_HPC_BIST_REDUCE_ADDR_EN      ((u32)0x00000003 << 0)                   /*!< R/W   0x0 None */
#define CA32_CTRL_HPC_BIST_REDUCE_ADDR_EN(x)        ((u32)(((x) & 0x00000003) << 0))
#define CA32_CTRL_GET_HPC_BIST_REDUCE_ADDR_EN(x)    ((u32)(((x >> 0) & 0x00000003)))
/** @} */


/** @defgroup REG_MBIST_CTRL1
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_HPC_COORD_RSTN               ((u32)0x00000007 << 29)                  /*!< R/W   0x0 None */
#define CA32_CTRL_HPC_COORD_RSTN(x)                 ((u32)(((x) & 0x00000007) << 29))
#define CA32_CTRL_GET_HPC_COORD_RSTN(x)             ((u32)(((x >> 29) & 0x00000007)))
#define CA32_CTRL_MASK_HPC_BIST_ALG_SELECTION       ((u32)0x0000007F << 22)                  /*!< R/W   0x1 None */
#define CA32_CTRL_HPC_BIST_ALG_SELECTION(x)         ((u32)(((x) & 0x0000007F) << 22))
#define CA32_CTRL_GET_HPC_BIST_ALG_SELECTION(x)     ((u32)(((x >> 22) & 0x0000007F)))
#define CA32_CTRL_MASK_HPC_BIST_DONE                ((u32)0x000007FF << 11)                  /*!< R     0x0 None */
#define CA32_CTRL_HPC_BIST_DONE(x)                  ((u32)(((x) & 0x000007FF) << 11))
#define CA32_CTRL_GET_HPC_BIST_DONE(x)              ((u32)(((x >> 11) & 0x000007FF)))
#define CA32_CTRL_MASK_HPC_BIST_FAIL                ((u32)0x000007FF << 0)                   /*!< R     0x0 None */
#define CA32_CTRL_HPC_BIST_FAIL(x)                  ((u32)(((x) & 0x000007FF) << 0))
#define CA32_CTRL_GET_HPC_BIST_FAIL(x)              ((u32)(((x >> 0) & 0x000007FF)))
/** @} */


/** @defgroup REG_MBIST_CTRL2
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_HPC_BIST_BANK_FAIL           ((u32)0xFFFFFFFF << 0)                   /*!< R     0x0 None */
#define CA32_CTRL_HPC_BIST_BANK_FAIL(x)             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CA32_CTRL_GET_HPC_BIST_BANK_FAIL(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_MBIST_CTRL3
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_HPC_BIST_MODE                ((u32)0x000007FF << 15)                  /*!< R/W   0x0 None */
#define CA32_CTRL_HPC_BIST_MODE(x)                  ((u32)(((x) & 0x000007FF) << 15))
#define CA32_CTRL_GET_HPC_BIST_MODE(x)              ((u32)(((x >> 15) & 0x000007FF)))
#define CA32_CTRL_MASK_HPC_BIST_SRAM_DBG_SEL        ((u32)0x0000001F << 10)                  /*!< R/W   0x0 None */
#define CA32_CTRL_HPC_BIST_SRAM_DBG_SEL(x)          ((u32)(((x) & 0x0000001F) << 10))
#define CA32_CTRL_GET_HPC_BIST_SRAM_DBG_SEL(x)      ((u32)(((x >> 10) & 0x0000001F)))
#define CA32_CTRL_BIT_HPC_BIST_SRAM_DBG_EN          ((u32)0x00000001 << 9)                   /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_HPC_BIST_SRAM_DBG_CLR         ((u32)0x00000001 << 8)                   /*!< R/W   0x0 None */
#define CA32_CTRL_MASK_HPC_BIST_SCRMCONFIG          ((u32)0x000000FF << 0)                   /*!< R/W   0x0 None */
#define CA32_CTRL_HPC_BIST_SCRMCONFIG(x)            ((u32)(((x) & 0x000000FF) << 0))
#define CA32_CTRL_GET_HPC_BIST_SCRMCONFIG(x)        ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_MBIST_CTRL4
 * @brief
 * @{
 */
#define CA32_CTRL_BIT_HPC_DIAG_CLK_EN               ((u32)0x00000001 << 24)                  /*!< R/W   0x0 None */
#define CA32_CTRL_MASK_HPC_DIAG_MODE                ((u32)0x00000003 << 22)                  /*!< R/W   0x0 None */
#define CA32_CTRL_HPC_DIAG_MODE(x)                  ((u32)(((x) & 0x00000003) << 22))
#define CA32_CTRL_GET_HPC_DIAG_MODE(x)              ((u32)(((x >> 22) & 0x00000003)))
#define CA32_CTRL_MASK_HPC_DIAG_MEMARRAY_SEL        ((u32)0x000001FF << 13)                  /*!< R/W   0x0 None */
#define CA32_CTRL_HPC_DIAG_MEMARRAY_SEL(x)          ((u32)(((x) & 0x000001FF) << 13))
#define CA32_CTRL_GET_HPC_DIAG_MEMARRAY_SEL(x)      ((u32)(((x >> 13) & 0x000001FF)))
#define CA32_CTRL_BIT_HPC_DIAG_CPUCORE_SEL          ((u32)0x00000001 << 12)                  /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_HPC_DIAG_START                ((u32)0x00000001 << 11)                  /*!< R     0x0 None */
#define CA32_CTRL_BIT_HPC_DIAG_SO                   ((u32)0x00000001 << 10)                  /*!< R     0x0 None */
#define CA32_CTRL_BIT_HPC_SHRBUS_DELAY_ENABLE       ((u32)0x00000001 << 9)                   /*!< R/W   0x0 None */
#define CA32_CTRL_MASK_HPC_SHRBUS_DELAY_NUM         ((u32)0x0000000F << 5)                   /*!< R/W   0x0 None */
#define CA32_CTRL_HPC_SHRBUS_DELAY_NUM(x)           ((u32)(((x) & 0x0000000F) << 5))
#define CA32_CTRL_GET_HPC_SHRBUS_DELAY_NUM(x)       ((u32)(((x >> 5) & 0x0000000F)))
#define CA32_CTRL_BIT_HPC_MBIST_DELAY_ENABLE        ((u32)0x00000001 << 4)                   /*!< R/W   0x0 None */
#define CA32_CTRL_MASK_HPC_MBIST_DELAY_NUM          ((u32)0x00000003 << 2)                   /*!< R/W   0x0 None */
#define CA32_CTRL_HPC_MBIST_DELAY_NUM(x)            ((u32)(((x) & 0x00000003) << 2))
#define CA32_CTRL_GET_HPC_MBIST_DELAY_NUM(x)        ((u32)(((x >> 2) & 0x00000003)))
#define CA32_CTRL_BIT_HPC_MBIST_DMY_WR_ENABLE       ((u32)0x00000001 << 1)                   /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_HPC_MEM_SPEED_MODE            ((u32)0x00000001 << 0)                   /*!< R/W   0x0 None */
/** @} */


/** @defgroup REG_MBIST_CTRL5
 * @brief
 * @{
 */
#define CA32_CTRL_BIT_HPC_BISR_RSTN                 ((u32)0x00000001 << 13)                  /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_HPC_PWR_RSTN                  ((u32)0x00000001 << 12)                  /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_HPC_SECOND_RUN_EN             ((u32)0x00000001 << 11)                  /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_HPC_REMAP_RSTN                ((u32)0x00000001 << 10)                  /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_HPC_BISR_DONE                 ((u32)0x00000001 << 9)                   /*!< R     0x0 None */
#define CA32_CTRL_MASK_HPC_BISR_FAIL                ((u32)0x000000FF << 1)                   /*!< R     0x0 None */
#define CA32_CTRL_HPC_BISR_FAIL(x)                  ((u32)(((x) & 0x000000FF) << 1))
#define CA32_CTRL_GET_HPC_BISR_FAIL(x)              ((u32)(((x >> 1) & 0x000000FF)))
#define CA32_CTRL_BIT_HPC_BISR_REPAIRED             ((u32)0x00000001 << 0)                   /*!< R     0x0 None */
/** @} */


/** @defgroup REG_MBIST_CTRL6
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_HPC_BISR_OUT_56_32           ((u32)0x00FFFFFF << 0)                   /*!< R     0x0 None */
#define CA32_CTRL_HPC_BISR_OUT_56_32(x)             ((u32)(((x) & 0x00FFFFFF) << 0))
#define CA32_CTRL_GET_HPC_BISR_OUT_56_32(x)         ((u32)(((x >> 0) & 0x00FFFFFF)))
/** @} */


/** @defgroup REG_MBIST_CTRL7
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_HPC_BISR_OUT_31_0            ((u32)0xFFFFFFFF << 0)                   /*!< R     0x0 None */
#define CA32_CTRL_HPC_BISR_OUT_31_0(x)              ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CA32_CTRL_GET_HPC_BISR_OUT_31_0(x)          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_MBIST_CTRL8
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_HPC_BIST_GRP_EN              ((u32)0x000000FF << 11)                  /*!< R/W   0x0 None */
#define CA32_CTRL_HPC_BIST_GRP_EN(x)                ((u32)(((x) & 0x000000FF) << 11))
#define CA32_CTRL_GET_HPC_BIST_GRP_EN(x)            ((u32)(((x >> 11) & 0x000000FF)))
#define CA32_CTRL_BIT_HPC_FSS_BIST_LOOP_MODE        ((u32)0x00000001 << 10)                  /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_HPC_FSS_BIST_MODE             ((u32)0x00000001 << 9)                   /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_HPC_FSS_DRF_BIST_MODE         ((u32)0x00000001 << 8)                   /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_HPC_FSS_DRF_TEST_RESUME       ((u32)0x00000001 << 7)                   /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_HPC_FSS_MBIST_RSTN            ((u32)0x00000001 << 6)                   /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_HPC_FSS_DYN_READ_EN           ((u32)0x00000001 << 5)                   /*!< R/W   0x0 None */
#define CA32_CTRL_BIT_HPC_FSS_BIST_DONE             ((u32)0x00000001 << 4)                   /*!< R     0x0 None */
#define CA32_CTRL_BIT_HPC_FSS_BIST_FAIL             ((u32)0x00000001 << 3)                   /*!< R     0x0 None */
#define CA32_CTRL_BIT_HPC_FSS_DRF_BIST_DONE         ((u32)0x00000001 << 2)                   /*!< R     0x0 None */
#define CA32_CTRL_BIT_HPC_FSS_DRF_BIST_FAIL         ((u32)0x00000001 << 1)                   /*!< R     0x0 None */
#define CA32_CTRL_BIT_HPC_FSS_DRF_START_PAUSE       ((u32)0x00000001 << 0)                   /*!< R     0x0 None */
/** @} */


/** @defgroup REG_CA32_DIAG_DATA_CTRL
 * @brief
 * @{
 */
#define CA32_CTRL_BIT_CA32_DIAG_RDY                 ((u32)0x00000001 << 1)                   /*!< R     0   1: SW can read out the diag data from registers (0x068 - 0x084) */
#define CA32_CTRL_BIT_CA32_DIAG_RESTART_PS          ((u32)0x00000001 << 0)                   /*!< R/W1P 0  
                                                                                              * SW is to write 1 to restart another diag data shift out from memory. This bit will be set by SW after finish reading diag data.
                                                                                              * HW is to clear it.
                                                                                              */
/** @} */


/** @defgroup REG_CA32_DIAG_DATA_0
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_DATA0                        ((u32)0xFFFFFFFF << 0)                   /*!< R     0   None */
#define CA32_CTRL_DATA0(x)                          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CA32_CTRL_GET_DATA0(x)                      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CA32_DIAG_DATA_1
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_DATA1                        ((u32)0xFFFFFFFF << 0)                   /*!< R     0   None */
#define CA32_CTRL_DATA1(x)                          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CA32_CTRL_GET_DATA1(x)                      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CA32_DIAG_DATA_2
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_DATA2                        ((u32)0xFFFFFFFF << 0)                   /*!< R     0   None */
#define CA32_CTRL_DATA2(x)                          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CA32_CTRL_GET_DATA2(x)                      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CA32_DIAG_DATA_3
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_DATA3                        ((u32)0xFFFFFFFF << 0)                   /*!< R     0   None */
#define CA32_CTRL_DATA3(x)                          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CA32_CTRL_GET_DATA3(x)                      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CA32_DIAG_DATA_4
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_DATA4                        ((u32)0xFFFFFFFF << 0)                   /*!< R     0   None */
#define CA32_CTRL_DATA4(x)                          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CA32_CTRL_GET_DATA4(x)                      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CA32_DIAG_DATA_5
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_DATA5                        ((u32)0xFFFFFFFF << 0)                   /*!< R     0   None */
#define CA32_CTRL_DATA5(x)                          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CA32_CTRL_GET_DATA5(x)                      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CA32_DIAG_DATA_6
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_DATA6                        ((u32)0xFFFFFFFF << 0)                   /*!< R     0   None */
#define CA32_CTRL_DATA6(x)                          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CA32_CTRL_GET_DATA6(x)                      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CA32_DIAG_DATA_7
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_DATA7                        ((u32)0xFFFFFFFF << 0)                   /*!< R     0   None */
#define CA32_CTRL_DATA7(x)                          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CA32_CTRL_GET_DATA7(x)                      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CA32_SPARE1
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_CA32_SPARE1                  ((u32)0xFFFFFFFF << 0)                   /*!< RW    0   SPARE for SW */
#define CA32_CTRL_CA32_SPARE1(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CA32_CTRL_GET_CA32_SPARE1(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CA32_SPARE2
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_CA32_SPARE2                  ((u32)0xFFFFFFFF << 0)                   /*!< RW    0   SPARE for SW */
#define CA32_CTRL_CA32_SPARE2(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CA32_CTRL_GET_CA32_SPARE2(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CA32_SPARE3
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_CA32_SPARE3                  ((u32)0xFFFFFFFF << 0)                   /*!< RW    0   SPARE for SW */
#define CA32_CTRL_CA32_SPARE3(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CA32_CTRL_GET_CA32_SPARE3(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CA32_SPARE4
 * @brief
 * @{
 */
#define CA32_CTRL_MASK_CA32_SPARE4                  ((u32)0xFFFFFFFF << 0)                   /*!< RW    0   SPARE for SW */
#define CA32_CTRL_CA32_SPARE4(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CA32_CTRL_GET_CA32_SPARE4(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */




/** @defgroup CA32_CTRL_Exported_Types CA32_CTRL Exported Types
* @{
*/
/**
* @brief CA32_CTRL Register Declaration
*/

typedef struct {
	__IO uint32_t CA32_FEN_CTRL;                                /*!< Register,  Address offset:0x0000 */
	__IO uint32_t CA32_CLK_CTRL;                                /*!< Register,  Address offset:0x0004 */
	__IO uint32_t CA32_RST_CTRL;                                /*!< Register,  Address offset:0x0008 */
	__IO uint32_t CA32_PWR_CTRL;                                /*!< Register,  Address offset:0x000C */
	__IO uint32_t CA32_CFG0;                                    /*!< Register,  Address offset:0x0010 */
	__IO uint32_t CA32_SPARE0;                                  /*!< Register,  Address offset:0x0014 */
	__I  uint32_t CA32_PMU_EVENT;                               /*!< Register,  Address offset:0x0018 */
	__IO uint32_t CA32_GIC_CTRL;                                /*!< Register,  Address offset:0x001C */
	__IO uint32_t CA32_REMAP;                                   /*!< Register,  Address offset:0x0020 */
	__IO uint32_t CA32_SRAM_CTRL;                               /*!< Register,  Address offset:0x0024 */
	__IO uint32_t CA32_FSS_CTRL0;                               /*!< Register,  Address offset:0x0028 */
	__IO uint32_t CA32_FSS_CTRL1;                               /*!< Register,  Address offset:0x002C */
	__I  uint32_t CA32_FSS_STS;                                 /*!< Register,  Address offset:0x0030 */
	__IO uint32_t CA32_FSS_RECODER_CTRL;                        /*!< Register,  Address offset:0x0034 */
	__IO uint32_t CA32_FSSCTRL_SRAM;                            /*!< Register,  Address offset:0x0038 */
	__IO uint32_t TPIU_CTRL;                                    /*!< Register,  Address offset:0x003C */
	__IO uint32_t MBIST_CTRL0;                                  /*!< Register,  Address offset:0x0040 */
	__IO uint32_t MBIST_CTRL1;                                  /*!< Register,  Address offset:0x0044 */
	__I  uint32_t MBIST_CTRL2;                                  /*!< Register,  Address offset:0x0048 */
	__IO uint32_t MBIST_CTRL3;                                  /*!< Register,  Address offset:0x004C */
	__IO uint32_t MBIST_CTRL4;                                  /*!< Register,  Address offset:0x0050 */
	__IO uint32_t MBIST_CTRL5;                                  /*!< Register,  Address offset:0x0054 */
	__I  uint32_t MBIST_CTRL6;                                  /*!< Register,  Address offset:0x0058 */
	__I  uint32_t MBIST_CTRL7;                                  /*!< Register,  Address offset:0x005C */
	__IO uint32_t MBIST_CTRL8;                                  /*!< Register,  Address offset:0x0060 */
	__IO uint32_t CA32_DIAG_DATA_CTRL;                          /*!< Register,  Address offset:0x0064 */
	__I  uint32_t CA32_DIAG_DATA_0;                             /*!< Register,  Address offset:0x0068 */
	__I  uint32_t CA32_DIAG_DATA_1;                             /*!< Register,  Address offset:0x006C */
	__I  uint32_t CA32_DIAG_DATA_2;                             /*!< Register,  Address offset:0x0070 */
	__I  uint32_t CA32_DIAG_DATA_3;                             /*!< Register,  Address offset:0x0074 */
	__I  uint32_t CA32_DIAG_DATA_4;                             /*!< Register,  Address offset:0x0078 */
	__I  uint32_t CA32_DIAG_DATA_5;                             /*!< Register,  Address offset:0x007C */
	__I  uint32_t CA32_DIAG_DATA_6;                             /*!< Register,  Address offset:0x0080 */
	__I  uint32_t CA32_DIAG_DATA_7;                             /*!< Register,  Address offset:0x0084 */
	uint32_t RSVD_NO_FUNCTION0[2];
	__IO uint32_t CA32_SPARE1;                                  /*!< Register,  Address offset:0x0090 */
	__IO uint32_t CA32_SPARE2;                                  /*!< Register,  Address offset:0x0094 */
	__IO uint32_t CA32_SPARE3;                                  /*!< Register,  Address offset:0x0098 */
	__IO uint32_t CA32_SPARE4;                                  /*!< Register,  Address offset:0x009C */
} CA32_CTRL_TypeDef;

#define CA32_CTRL_BASE ((CA32_CTRL_TypeDef *) (0x60490000))
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
#endif  // SYSREG_CA32_CTRL_H
