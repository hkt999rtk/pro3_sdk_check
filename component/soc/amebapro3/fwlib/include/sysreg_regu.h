/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_REGU_H
#define SYSREG_REGU_H

/* AUTO_GEN_START */

/* REGU is a part of AON */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup REGU
* @brief REGU driver modules
* @{
*/


/** @defgroup REG_REGU_LDO_CTRL0
 * @brief
 * @{
 */
#define REGU_BIT_AON_LDO_CTRL                       ((u32)0x00000001 << 24)                  /*!< R/W   0  
                                                                                              * 0:The AON_LDO control was come from AON+0x0[7:4]
                                                                                              * 1:The AON_LDO control was come from AON+0x0[23:20]
                                                                                              */
#define REGU_MASK_AON_LDOH09_V09ADJ_PMC             ((u32)0x0000000F << 20)                  /*!< R/W   0x5
                                                                                              * AONLDO09 voltage select
                                                                                              * VREF 0P600V 0P575V
                                                                                              * 0000: 0.593V 0.568V
                                                                                              * 0001: 0.708V 0.678V
                                                                                              * 0010: 0.739V 0.708V
                                                                                              * 0011: 0.774V 0.742V
                                                                                              * 0100: 0.812V 0.778V
                                                                                              * 0101: 0.833V 0.798V
                                                                                              * 0110: 0.854V 0.819V
                                                                                              * 0111: 0.877V 0.840V
                                                                                              * 1000: 0.901V 0.863V
                                                                                              * 1001: 0.926V 0.888V
                                                                                              * 1010: 0.953V 0.913V
                                                                                              * 1011: 0.981V 0.940V
                                                                                              * 1100: 1.012V 0.969V
                                                                                              * 1101: 1.044V 1.000V
                                                                                              * 1110: 1.077V 1.033V
                                                                                              * 1111: 1.111V 1.067V
                                                                                              */
#define REGU_AON_LDOH09_V09ADJ_PMC(x)               ((u32)(((x) & 0x0000000F) << 20))
#define REGU_GET_AON_LDOH09_V09ADJ_PMC(x)           ((u32)(((x >> 20) & 0x0000000F)))
#define REGU_BIT_AON_BOD_EN                         ((u32)0x00000001 << 19)                  /*!< R/W   1  
                                                                                              * Brown-Out-Detect
                                                                                              * 1 : enable (default)
                                                                                              * 0 : disable
                                                                                              */
#define REGU_MASK_AON_BOD_VTHSEL                    ((u32)0x00000007 << 16)                  /*!< R/W   1  
                                                                                              * Brown-Out-Detect
                                                                                              * VTH selection
                                                                                              * BOD H L
                                                                                              * 000 0.475 0.450
                                                                                              * 001 2.426 2.298
                                                                                              * 010 2.533 2.400
                                                                                              * 011 2.591 2.455
                                                                                              * 100 2.651 2.512
                                                                                              * 101 2.714 2.571
                                                                                              * 110 2.780 2.634
                                                                                              * 111 2.850 2.700
                                                                                              * 
                                                                                              * INT H L
                                                                                              * 000 0.520 0.500
                                                                                              * 001 2.655 2.553
                                                                                              * 010 2.773 2.667
                                                                                              * 011 2.836 2.727
                                                                                              * 100 2.902 2.791
                                                                                              * 101 2.971 2.857
                                                                                              * 110 3.044 2.927
                                                                                              * 111 3.120 3.000
                                                                                              */
#define REGU_AON_BOD_VTHSEL(x)                      ((u32)(((x) & 0x00000007) << 16))
#define REGU_GET_AON_BOD_VTHSEL(x)                  ((u32)(((x >> 16) & 0x00000007)))
#define REGU_MASK_AON_LDO_CTRL0_DUMMY_0             ((u32)0x0000000F << 12)                  /*!< R/W   0   Dummy */
#define REGU_AON_LDO_CTRL0_DUMMY_0(x)               ((u32)(((x) & 0x0000000F) << 12))
#define REGU_GET_AON_LDO_CTRL0_DUMMY_0(x)           ((u32)(((x >> 12) & 0x0000000F)))
#define REGU_BIT_AON_CK12M_EN                       ((u32)0x00000001 << 11)                  /*!< R/W   1   1: enable 12M ring */
#define REGU_BIT_AON_CK_SEL                         ((u32)0x00000001 << 10)                  /*!< R/W   0  
                                                                                              * 1: 12MHz 0:100kHz
                                                                                              * Choose between 12M and 100K, when selecting 100k, keep 12M enabled.
                                                                                              */
#define REGU_BIT_AON_LDO_CTRL0_DUMMY_1              ((u32)0x00000001 << 9)                   /*!< R/W   0   Dummy */
#define REGU_BIT_AON_EN_SLEEP_L                     ((u32)0x00000001 << 8)                   /*!< R/W   0  
                                                                                              * Enable LDO09H sleep mode
                                                                                              * 0 : disable (default)
                                                                                              * 1 : enable
                                                                                              */
#define REGU_MASK_AON_LDOH09_V09ADJ_L_3_0           ((u32)0x0000000F << 4)                   /*!< R/W   0x8
                                                                                              * AONLDO09 voltage select
                                                                                              * VREF 0P600V 0P575V
                                                                                              * 0000: 0.593V 0.568V
                                                                                              * 0001: 0.708V 0.678V
                                                                                              * 0010: 0.739V 0.708V
                                                                                              * 0011: 0.774V 0.742V
                                                                                              * 0100: 0.812V 0.778V
                                                                                              * 0101: 0.833V 0.798V
                                                                                              * 0110: 0.854V 0.819V
                                                                                              * 0111: 0.877V 0.840V
                                                                                              * 1000: 0.901V 0.863V
                                                                                              * 1001: 0.926V 0.888V
                                                                                              * 1010: 0.953V 0.913V
                                                                                              * 1011: 0.981V 0.940V
                                                                                              * 1100: 1.012V 0.969V
                                                                                              * 1101: 1.044V 1.000V
                                                                                              * 1110: 1.077V 1.033V
                                                                                              * 1111: 1.111V 1.067V
                                                                                              */
#define REGU_AON_LDOH09_V09ADJ_L_3_0(x)             ((u32)(((x) & 0x0000000F) << 4))
#define REGU_GET_AON_LDOH09_V09ADJ_L_3_0(x)         ((u32)(((x >> 4) & 0x0000000F)))
#define REGU_MASK_DUMMY_REG_ANAPAR_LDO_B3_B2_1_0    ((u32)0x00000003 << 2)                   /*!< R/W   0   // [REGU][AONLDO] dummy */
#define REGU_DUMMY_REG_ANAPAR_LDO_B3_B2_1_0(x)      ((u32)(((x) & 0x00000003) << 2))
#define REGU_GET_DUMMY_REG_ANAPAR_LDO_B3_B2_1_0(x)  ((u32)(((x >> 2) & 0x00000003)))
#define REGU_BIT_LV_POR_VREF_SEL                    ((u32)0x00000001 << 1)                   /*!< R/W   0  
                                                                                              * LV_POR VREF
                                                                                              * 0 (default) VREF = 0P600V
                                                                                              * 1 VREF = 0P450V
                                                                                              */
#define REGU_BIT_AONLDO_VREF_SEL                    ((u32)0x00000001 << 0)                   /*!< R/W   0  
                                                                                              * AONLDO VREF_SEL
                                                                                              * 0 (default) VREF = 0P600V
                                                                                              * 1 VREF = 0P575V
                                                                                              */
/** @} */


/** @defgroup REG_REGU_WLON_LDO_CTRL0
 * @brief
 * @{
 */
#define REGU_MASK_REG_DMY_LDO_L_1_0                 ((u32)0x00000003 << 14)                  /*!< R/W   0  
                                                                                              * // [REGU][LDO] REG_CTRL
                                                                                              * // LDO extra Power Cut Control
                                                                                              * <1> : large pwrcut mos, <0> : small pwrcut mos
                                                                                              * 0 (default): disable pwrcut mos
                                                                                              * 1 : enable pwrcut mos
                                                                                              * enable sequence should be first enable small then delay >50us to enable large
                                                                                              */
#define REGU_REG_DMY_LDO_L_1_0(x)                   ((u32)(((x) & 0x00000003) << 14))
#define REGU_GET_REG_DMY_LDO_L_1_0(x)               ((u32)(((x >> 14) & 0x00000003)))
#define REGU_BIT_REG_FORCE_DIODE_L                  ((u32)0x00000001 << 13)                  /*!< R/W   0  
                                                                                              * // [REGU][LDO_WLON] REG_CTRL
                                                                                              * // PGATE diode clamp (this is directly force no gating)
                                                                                              * // (purpose for powerCut mode iOCP)
                                                                                              * // 0: disable force diode clamp (default)
                                                                                              * // 1: enable force diode clamp
                                                                                              */
#define REGU_BIT_REG_BUFFERTYPE_SEL_L               ((u32)0x00000001 << 12)                  /*!< R/W   1  
                                                                                              * // [REGU][LDO] REG_CTRL
                                                                                              * // LDO Buffer Type Select
                                                                                              * // 0: PMOS buffer (for HV 3P3V)
                                                                                              * // 1(default): NMOS buffer (for SWR 1P35V)
                                                                                              */
#define REGU_MASK_REG_DMYLOAD_L_1_0                 ((u32)0x00000003 << 10)                  /*!< R/W   0  
                                                                                              * // [REGU][LDO_WLON] REG_CTRL
                                                                                              * // LDO dmy load ctrl
                                                                                              * // 00(default): dmy load disable
                                                                                              * // 01: light dmy load enable (~160kohm)
                                                                                              * // 10: dmy load enable (~1kohm)
                                                                                              * // 11: dmy load + light dmy load both enable
                                                                                              */
#define REGU_REG_DMYLOAD_L_1_0(x)                   ((u32)(((x) & 0x00000003) << 10))
#define REGU_GET_REG_DMYLOAD_L_1_0(x)               ((u32)(((x >> 10) & 0x00000003)))
#define REGU_BIT_REG_ITAIL_HALF_EN_L                ((u32)0x00000001 << 9)                   /*!< R/W   0  
                                                                                              * // [REGU][LDO_WLON] REG_CTRL
                                                                                              * // LDO iq half ctrl
                                                                                              * // 0(default): iq normal (for normal mode)
                                                                                              * // 1: iq half (for standby mode)
                                                                                              */
#define REGU_MASK_REG_STANDBY_L_1_0                 ((u32)0x00000003 << 7)                   /*!< R/W   0  
                                                                                              * // [REGU][LDO_WLON] REG_CTRL
                                                                                              * // LDO standby mode ctrl:
                                                                                              * // 00 (default): normal mode (iq normal + pwrmos 8x)
                                                                                              * // 01: debug mode (iq reduce + pwrmos 1x)
                                                                                              * // 10: debug mode (iq normal + pwrmos 8x)
                                                                                              * // 11: standby mode (iq reduce + pwrmos 1x)
                                                                                              */
#define REGU_REG_STANDBY_L_1_0(x)                   ((u32)(((x) & 0x00000003) << 7))
#define REGU_GET_REG_STANDBY_L_1_0(x)               ((u32)(((x >> 7) & 0x00000003)))
#define REGU_BIT_EN_DIODE_L                         ((u32)0x00000001 << 6)                   /*!< R/W   1  
                                                                                              * // [REGU][LDO_WLON] REG_CTRL
                                                                                              * // PGATE diode clamp (this gating to disable by pd_regu_L)
                                                                                              * // 0: disable clamp
                                                                                              * // 1: enable clamp (default)
                                                                                              */
#define REGU_MASK_VOADJ_L_3_0                       ((u32)0x0000000F << 2)                   /*!< R/W   0x8
                                                                                              * // [REGU][LDO_WLON] REG_CTRL
                                                                                              * // LDO VOSEL
                                                                                              * // 0000 0.722
                                                                                              * // 0001 0.740
                                                                                              * // 0010 0.760
                                                                                              * // 0011 0.780
                                                                                              * // 0100 0.802
                                                                                              * // 0101 0.825
                                                                                              * // 0110 0.850
                                                                                              * // 0111 0.875
                                                                                              * // 1000 0.903
                                                                                              * // 1001 0.932
                                                                                              * // 1010 0.963
                                                                                              * // 1011 0.997
                                                                                              * // 1100 1.032
                                                                                              * // 1101 1.071
                                                                                              * // 1110 1.112
                                                                                              * // 1111 1.157
                                                                                              */
#define REGU_VOADJ_L_3_0(x)                         ((u32)(((x) & 0x0000000F) << 2))
#define REGU_GET_VOADJ_L_3_0(x)                     ((u32)(((x >> 2) & 0x0000000F)))
#define REGU_BIT_PD_REGU_L                          ((u32)0x00000001 << 1)                   /*!< R/W   1  
                                                                                              * // [REGU][LDO_WLON] REG_CTRL
                                                                                              * // PowerDown CTRL
                                                                                              * // 0: normal-Mode
                                                                                              * // 1: power-down Mode (default)
                                                                                              */
#define REGU_BIT_EN_PC_BT_L                         ((u32)0x00000001 << 0)                   /*!< R/W   0  
                                                                                              * // [REGU][LDO_WLON] REG_CTRL
                                                                                              * // PWRCUT enable
                                                                                              * // 0: LDO-Mode (default)
                                                                                              * // 1: PWRCUT-Mode
                                                                                              */
/** @} */


/** @defgroup REG_REGU_SWR_SRC_CTRL0
 * @brief
 * @{
 */
#define REGU_BIT_FORCE_ZCD_BIAS                     ((u32)0x00000001 << 31)                  /*!< R/W   1  
                                                                                              * //0:ZCD bias on after SS
                                                                                              * //1:ZCD bias on when ZCD enable
                                                                                              */
#define REGU_BIT_ZCD_SDZ_L_1                        ((u32)0x00000001 << 30)                  /*!< R/W   0   //Increase ZCD delay cell */
#define REGU_BIT_ZCD_SDZ_L_0                        ((u32)0x00000001 << 29)                  /*!< R/W   1   //Increase ZCD delay cell */
#define REGU_MASK_ZCDC_H                            ((u32)0x00000003 << 27)                  /*!< R     0x2
                                                                                              * //00:0.1u
                                                                                              * //01:0.2u(PFM)
                                                                                              * //10:0.7u (PWM Loading > 50mA)
                                                                                              * //11:0.8u (PWM Loading <= 50mA)
                                                                                              */
#define REGU_ZCDC_H(x)                              ((u32)(((x) & 0x00000003) << 27))
#define REGU_GET_ZCDC_H(x)                          ((u32)(((x >> 27) & 0x00000003)))
#define REGU_BIT_POW_ZCD_L                          ((u32)0x00000001 << 26)                  /*!< R/W   1   //Enable ZCD */
#define REGU_MASK_OCP_L1                            ((u32)0x00000007 << 23)                  /*!< R     0x3
                                                                                              * //PWM current (A) / PFM Current (A)
                                                                                              * //000 0.398 0.178
                                                                                              * //001 0.486 0.222
                                                                                              * //010 0.573 0.266
                                                                                              * //011 0.660 0.310
                                                                                              * //100 0.747 0.354
                                                                                              * //101 0.834 0.398
                                                                                              * //110 0.920 0.442
                                                                                              * //111 1.008 VIN/10
                                                                                              */
#define REGU_OCP_L1(x)                              ((u32)(((x) & 0x00000007) << 23))
#define REGU_GET_OCP_L1(x)                          ((u32)(((x >> 23) & 0x00000007)))
#define REGU_BIT_POWOCP_L1                          ((u32)0x00000001 << 22)                  /*!< R/W   1   //POWOCP_L1=1 : enable over current protection */
#define REGU_MASK_SAW_FREQ_L                        ((u32)0x0000000F << 18)                  /*!< R/W   0x8
                                                                                              * 0000 458.0k
                                                                                              * 0001 680.2k
                                                                                              * 0010 898.9k
                                                                                              * 0011 1.113M
                                                                                              * 0100 1.321M
                                                                                              * 0101 1.522M
                                                                                              * 0110 1.716M
                                                                                              * 0111 1.902M
                                                                                              * 1000 2.087M
                                                                                              * 1001 2.258M
                                                                                              * 1010 2.426M
                                                                                              * 1011 2.582M
                                                                                              * 1100 2.743M
                                                                                              * 1101 2.888M
                                                                                              * 1110 3.035M
                                                                                              * 1111 3.165M
                                                                                              */
#define REGU_SAW_FREQ_L(x)                          ((u32)(((x) & 0x0000000F) << 18))
#define REGU_GET_SAW_FREQ_L(x)                      ((u32)(((x >> 18) & 0x0000000F)))
#define REGU_BIT_BYPASS_L                           ((u32)0x00000001 << 17)                  /*!< R/W   1   1:bypass SSR latch */
#define REGU_BIT_FPWM_L1                            ((u32)0x00000001 << 16)                  /*!< R/W   1  
                                                                                              * FPWM_L1 = 1 : force PWM 
                                                                                              * FPWM_L1 = 0 : force PFM
                                                                                              */
#define REGU_MASK_STD_L1                            ((u32)0x00000003 << 14)                  /*!< R/W   1  
                                                                                              * STD_L1[1:0]  time
                                                                                              * 00 750u
                                                                                              * 01 1.5m
                                                                                              * 10 3m
                                                                                              * 11 12.288m
                                                                                              */
#define REGU_STD_L1(x)                              ((u32)(((x) & 0x00000003) << 14))
#define REGU_GET_STD_L1(x)                          ((u32)(((x >> 14) & 0x00000003)))
#define REGU_MASK_VOL_L1                            ((u32)0x0000000F << 10)                  /*!< R/W   0x8
                                                                                              * // bits SWR / LDO
                                                                                              * //VREF 0P600V 0P575V 0P500V 0P450V
                                                                                              * //0000 1.426 1.367 1.189 1.070
                                                                                              * //0001 1.455 1.394 1.213 1.091
                                                                                              * //0010 1.501 1.438 1.251 1.125
                                                                                              * //0011 1.549 1.484 1.291 1.162
                                                                                              * //0100 1.583 1.517 1.319 1.187
                                                                                              * //0101 1.637 1.569 1.364 1.228
                                                                                              * //0110 1.695 1.624 1.412 1.271
                                                                                              * //0111 1.736 1.664 1.446 1.302
                                                                                              * //1000 1.801 1.726 1.501 1.351 (default)
                                                                                              * //1001 1.871 1.793 1.559 1.403
                                                                                              * //1010 1.921 1.841 1.601 1.441
                                                                                              * //1011 2.001 1.918 1.667 1.501
                                                                                              * //1100 2.088 2.001 1.740 1.566
                                                                                              * //1101 2.150 2.060 1.792 1.613
                                                                                              * //1110 2.251 2.157 1.876 1.688
                                                                                              * //1111 2.362 2.264 1.968 1.771
                                                                                              */
#define REGU_VOL_L1(x)                              ((u32)(((x) & 0x0000000F) << 10))
#define REGU_GET_VOL_L1(x)                          ((u32)(((x >> 10) & 0x0000000F)))
#define REGU_BIT_AON_SRC_ENB_LDO_1P8_DIODE          ((u32)0x00000001 << 3)                   /*!< R/W     0 [no function] LDOMODE need followed sequence of LDOS->LDO */
#define REGU_BIT_AON_SRC_POW_LDOS_1P8               ((u32)0x00000001 << 2)                   /*!< R/W   0   small LDO enable // default 1'b0 disable */
#define REGU_BIT_AON_SRC_POW_LDO_1P8                ((u32)0x00000001 << 1)                   /*!< R/W     0 large LDO enable // default 1'b0 disable */
#define REGU_BIT_AON_SRC_POW_SW                     ((u32)0x00000001 << 0)                   /*!< R/W   0   SWR enable // default 1'b0 disable */
/** @} */


/** @defgroup REG_REGU_SWR_SRC_CTRL1
 * @brief
 * @{
 */
#define REGU_BIT_SRC_SSOVER_L                       ((u32)0x00000001 << 30)                  /*!< R     0   SWR_SRC vout soft-start-over ready signal 1:ready 0:(default) not ready */
#define REGU_MASK_SP_P_L                            ((u32)0x0000000F << 26)                  /*!< R/W   0x3 PGATE PMOS driving */
#define REGU_SP_P_L(x)                              ((u32)(((x) & 0x0000000F) << 26))
#define REGU_GET_SP_P_L(x)                          ((u32)(((x >> 26) & 0x0000000F)))
#define REGU_BIT_VO_DISCHG_PWM_H                    ((u32)0x00000001 << 25)                  /*!< R/W   0   1 : dummy load RES 178ohm (~10mA for 1P8V) */
#define REGU_BIT_MODE_PREDRIVER                     ((u32)0x00000001 << 24)                  /*!< R/W   1   1 : enable 3 level predriver */
#define REGU_MASK_ADJSLDO_L_3_0                     ((u32)0x0000000F << 20)                  /*!< R/W   0x6
                                                                                              * bits / voltage (for VREF0p6V) (as for others VREF is in ratio)
                                                                                              * 4'b0000 1.413
                                                                                              * 4'b0001 1.451
                                                                                              * 4'b0010 1.491
                                                                                              * 4'b0011 1.534
                                                                                              * 4'b0100 1.579
                                                                                              * 4'b0101 1.627
                                                                                              * 4'b0110 1.678
                                                                                              * 4'b0111 1.732
                                                                                              * 4'b1000 1.790
                                                                                              * 4'b1001 1.852
                                                                                              * 4'b1010 1.918
                                                                                              * 4'b1011 1.989
                                                                                              * 4'b1100 2.065
                                                                                              * 4'b1101 2.148
                                                                                              * 4'b1110 2.238
                                                                                              * 4'b1111 2.335
                                                                                              */
#define REGU_ADJSLDO_L_3_0(x)                       ((u32)(((x) & 0x0000000F) << 20))
#define REGU_GET_ADJSLDO_L_3_0(x)                   ((u32)(((x >> 20) & 0x0000000F)))
#define REGU_BIT_EN_LDOS_DIODE                      ((u32)0x00000001 << 19)                  /*!< R/W   1   1 : LDOS diode enable */
#define REGU_BIT_PWM_FORCE                          ((u32)0x00000001 << 18)                  /*!< R/W   0  
                                                                                              * 0 : bias enabled only at PWM mode
                                                                                              * 1 : force bias enable before mode tran from PFM to PWM
                                                                                              */
#define REGU_BIT_PFM_PD_RST                         ((u32)0x00000001 << 17)                  /*!< R/W   1   //1: When leave PFM mode, COT logic is reset */
#define REGU_BIT_VC_PFM_RSTB                        ((u32)0x00000001 << 16)                  /*!< R/W   0  
                                                                                              * //When into PFM, VC is,
                                                                                              * //0: clamped (bit 40 also =1) 1. floating
                                                                                              */
#define REGU_BIT_PFM_IN_SEL                         ((u32)0x00000001 << 15)                  /*!< R/W   1  
                                                                                              * //Power MOS into PFM when PFM CTRL,
                                                                                              * //0: rising 1: falling
                                                                                              */
#define REGU_BIT_VC_RSTB                            ((u32)0x00000001 << 14)                  /*!< R/W   0   //Before SS VC is, 0: pulled low 1: floating */
#define REGU_BIT_FPWMDELAY                          ((u32)0x00000001 << 13)                  /*!< R/W   0  
                                                                                              * //1:PFM[=]PWM 
                                                                                              * //PFM/PWM change by using DFF(CKMOD)
                                                                                              */
#define REGU_BIT_ENFPWMDELAY_H                      ((u32)0x00000001 << 12)                  /*!< R/W   0  
                                                                                              * //1:PFM=]PWM delay
                                                                                              * //PWM enable and wait one clock cycle then PFM off
                                                                                              */
#define REGU_BIT_MOS_HALF_L                         ((u32)0x00000001 << 11)                  /*!< R     0   //1:power mos half */
#define REGU_BIT_CURRENT_SENSE_MOS                  ((u32)0x00000001 << 10)                  /*!< R/W   0   //1: current sensing MOS for large OC current */
#define REGU_BIT_PFMCMP_IQ                          ((u32)0x00000001 << 9)                   /*!< R/W   1  
                                                                                              * //PFM comparator bias,
                                                                                              * //0: 50nA 1:250nA
                                                                                              */
#define REGU_BIT_REG_FORCE_SRC_IMIRROR_EN           ((u32)0x00000001 << 8)                   /*!< R/W   0  
                                                                                              * // Force SWR_SRC IBIAS Generation circuit enable
                                                                                              * 1: force enable (need enable this circuit before enable small LDO for SRC, LDO_SDIO, LDO_WLON)
                                                                                              * 0: non force, IBIAS enable listen to REG_SRC_POW_SW (default)
                                                                                              */
#define REGU_BIT_OFF_END_SEL                        ((u32)0x00000001 << 7)                   /*!< R/W   1  
                                                                                              * //COT off time controlled by,
                                                                                              * //0: ZCD 1: min off time
                                                                                              */
#define REGU_BIT_POW_MINOFF_L                       ((u32)0x00000001 << 6)                   /*!< R/W   1  
                                                                                              * //COT off time controlled by,
                                                                                              * //0: ZCD 1: min off time
                                                                                              */
#define REGU_MASK_COT_I_L_1_0                       ((u32)0x00000003 << 4)                   /*!< R/W   1  
                                                                                              * //COT ON Current
                                                                                              * //00:150nA
                                                                                              * //01:150nA+100nA
                                                                                              * //10:150nA+150nA
                                                                                              * //11:150nA+250nA
                                                                                              */
#define REGU_COT_I_L_1_0(x)                         ((u32)(((x) & 0x00000003) << 4))
#define REGU_GET_COT_I_L_1_0(x)                     ((u32)(((x >> 4) & 0x00000003)))
#define REGU_MASK_VREFPFM_L                         ((u32)0x0000000F << 0)                   /*!< R/W   0x9
                                                                                              * // SWR/LDO
                                                                                              * //0000 0.800
                                                                                              * //0001 0.850
                                                                                              * //0010 0.900
                                                                                              * //0011 0.950
                                                                                              * //0100 1.000
                                                                                              * //0101 1.050
                                                                                              * //0110 1.100
                                                                                              * //0111 1.150
                                                                                              * //1000 1.200
                                                                                              * //1001 1.250 (default)
                                                                                              * //1010 1.300
                                                                                              * //1011 1.350
                                                                                              * //1100 1.400
                                                                                              * //1101 1.450
                                                                                              * //1110 1.500
                                                                                              * //1111 1.550
                                                                                              */
#define REGU_VREFPFM_L(x)                           ((u32)(((x) & 0x0000000F) << 0))
#define REGU_GET_VREFPFM_L(x)                       ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_REGU_SWR_SRC_CTRL2
 * @brief
 * @{
 */
#define REGU_MASK_REG_SWR_RSV_CTRL_1_0              ((u32)0x00000003 << 16)                  /*!< R/W   0  
                                                                                              * // BG current adjustment
                                                                                              * // 00 default 23.9n
                                                                                              * // 01 54.6n
                                                                                              * // 10 117.8n
                                                                                              */
#define REGU_REG_SWR_RSV_CTRL_1_0(x)                ((u32)(((x) & 0x00000003) << 16))
#define REGU_GET_REG_SWR_RSV_CTRL_1_0(x)            ((u32)(((x >> 16) & 0x00000003)))
#define REGU_MASK_REG_SWR_ZCD_CTRL_1_0              ((u32)0x00000003 << 14)                  /*!< R/W   0x2
                                                                                              * // SWR ZCD CTRL control
                                                                                              * <1> 1'b1: extra current add if zcdc is 2'b11, 1'b0 : no extra current (default)
                                                                                              * <0> 1'b1: force in CCM/DCM boundary mode, 1'b0 : no boundary (default)
                                                                                              */
#define REGU_REG_SWR_ZCD_CTRL_1_0(x)                ((u32)(((x) & 0x00000003) << 14))
#define REGU_GET_REG_SWR_ZCD_CTRL_1_0(x)            ((u32)(((x >> 14) & 0x00000003)))
#define REGU_MASK_REG_PWM_VREF_SEL_1_0              ((u32)0x00000003 << 12)                  /*!< R/W   0x3
                                                                                              * PWM_VREF_SEL
                                                                                              * 2'b11 0P450V for SWR1P350 (default)
                                                                                              * 2'b10 0P425V for SWR1P275
                                                                                              * 2'b01 0P420V for SWR1P250
                                                                                              * 2'b00 0P400V for SWR1P200
                                                                                              */
#define REGU_REG_PWM_VREF_SEL_1_0(x)                ((u32)(((x) & 0x00000003) << 12))
#define REGU_GET_REG_PWM_VREF_SEL_1_0(x)            ((u32)(((x >> 12) & 0x00000003)))
#define REGU_MASK_SN_N_L                            ((u32)0x0000000F << 8)                   /*!< R/W   0xB NGATE NMOS driving */
#define REGU_SN_N_L(x)                              ((u32)(((x) & 0x0000000F) << 8))
#define REGU_GET_SN_N_L(x)                          ((u32)(((x >> 8) & 0x0000000F)))
#define REGU_MASK_SP_N_L                            ((u32)0x0000000F << 4)                   /*!< R/W   0x8 NGATE PMOS driving */
#define REGU_SP_N_L(x)                              ((u32)(((x) & 0x0000000F) << 4))
#define REGU_GET_SP_N_L(x)                          ((u32)(((x >> 4) & 0x0000000F)))
#define REGU_MASK_SN_P_L                            ((u32)0x0000000F << 0)                   /*!< R/W   0x3 PGATE NMOS driving */
#define REGU_SN_P_L(x)                              ((u32)(((x) & 0x0000000F) << 0))
#define REGU_GET_SN_P_L(x)                          ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_REGU_SWR_SRC_CTRL3
 * @brief
 * @{
 */
#define REGU_MASK_SDZN_L                            ((u32)0x00000003 << 30)                  /*!< R/W   1   //Nonoverlap delay cell for NMOS */
#define REGU_SDZN_L(x)                              ((u32)(((x) & 0x00000003) << 30))
#define REGU_GET_SDZN_L(x)                          ((u32)(((x >> 30) & 0x00000003)))
#define REGU_BIT_AUTOZCD_L                          ((u32)0x00000001 << 29)                  /*!< R/W   0   //0:register 1:10000 */
#define REGU_BIT_VOFB_SEL                           ((u32)0x00000001 << 28)                  /*!< R/W   0  
                                                                                              * //0:vo18
                                                                                              * //1:LX_SPS
                                                                                              */
#define REGU_MASK_TBOX_L1                           ((u32)0x00000003 << 26)                  /*!< R/W   0  
                                                                                              * //TBOX_L1[1:0] output
                                                                                              * //00    CKOVER/RAMPOVER
                                                                                              * //01    
                                                                                              * // NI/PI
                                                                                              * //10      CKSS/SSOVER
                                                                                              * //11    
                                                                                              * // i/NIOFF_H
                                                                                              */
#define REGU_TBOX_L1(x)                             ((u32)(((x) & 0x00000003) << 26))
#define REGU_GET_TBOX_L1(x)                         ((u32)(((x >> 26) & 0x00000003)))
#define REGU_BIT_ENOCPMUX_L                         ((u32)0x00000001 << 25)                  /*!< R/W   1  
                                                                                              * 1:OCP signal control by MUX
                                                                                              * 0:OCP signal control by I
                                                                                              */
#define REGU_BIT_FORCE_LDOS                         ((u32)0x00000001 << 24)                  /*!< R/W   0  
                                                                                              * //1 : LDOS force into Standby Mode
                                                                                              * //0 : LDOS at normal mode (default)
                                                                                              */
#define REGU_BIT_VO_DISCHG                          ((u32)0x00000001 << 23)                  /*!< R/W   0   //1:VO with 2mA discharge (internal RES ~ 893ohm) */
#define REGU_BIT_LDO_OC_CLAMP                       ((u32)0x00000001 << 22)                  /*!< R/W   1   // no function */
#define REGU_BIT_MINOFF_LIQ                         ((u32)0x00000001 << 21)                  /*!< R/W   1  
                                                                                              * //0:Min. off circuit always on
                                                                                              * //1:Min. off circuit pulse on
                                                                                              */
#define REGU_BIT_MINON_LIQ                          ((u32)0x00000001 << 20)                  /*!< R/W   1  
                                                                                              * //0:Min. on circuit always on
                                                                                              * //1:Min. on circuit pulse on
                                                                                              */
#define REGU_BIT_POW_AUTO_L                         ((u32)0x00000001 << 19)                  /*!< R/W   0  
                                                                                              * //1:auto PFM/PWM
                                                                                              * //0:register control
                                                                                              */
#define REGU_BIT_ARENB_H                            ((u32)0x00000001 << 18)                  /*!< R/W   0  
                                                                                              * // 0(default) : unity gain feedback clamp VC=VREF
                                                                                              * // 1.orignal clamp(roll back option)
                                                                                              */
#define REGU_BIT_NO_OFFTIME_L                       ((u32)0x00000001 << 17)                  /*!< R/W   0  
                                                                                              * //0:from min OFF time(PFM COT)
                                                                                              * //1:always on
                                                                                              */
#define REGU_BIT_EN_ON_END_L                        ((u32)0x00000001 << 16)                  /*!< R/W   1   //1:EN min OFF time (PFM COT) */
#define REGU_BIT_ENCOT_L                            ((u32)0x00000001 << 15)                  /*!< R/W   1   //1:PFM COT */
#define REGU_MASK_CLK_SEL                           ((u32)0x00000003 << 13)                  /*!< R/W   0x3
                                                                                              * //00:out of phase CKSS
                                                                                              * //01:out of phase PWM_CTRIL
                                                                                              * //10:ckout
                                                                                              * //11:HG
                                                                                              */
#define REGU_CLK_SEL(x)                             ((u32)(((x) & 0x00000003) << 13))
#define REGU_GET_CLK_SEL(x)                         ((u32)(((x >> 13) & 0x00000003)))
#define REGU_BIT_TYPE_L                             ((u32)0x00000001 << 12)                  /*!< R/W   1   //1:type3 0:type2 */
#define REGU_MASK_R3_L1                             ((u32)0x00000003 << 10)                  /*!< R/W   0x2
                                                                                              * //00 => 36k
                                                                                              * //01 => 30k
                                                                                              * //10 => 24k
                                                                                              * //11 => 24k
                                                                                              */
#define REGU_R3_L1(x)                               ((u32)(((x) & 0x00000003) << 10))
#define REGU_GET_R3_L1(x)                           ((u32)(((x >> 10) & 0x00000003)))
#define REGU_MASK_R2_L1                             ((u32)0x00000003 << 8)                   /*!< R/W   0x2
                                                                                              * //00 => 2.5M
                                                                                              * //01 => 2.0M
                                                                                              * //10 => 1.5M
                                                                                              * //11 => 1.0M
                                                                                              */
#define REGU_R2_L1(x)                               ((u32)(((x) & 0x00000003) << 8))
#define REGU_GET_R2_L1(x)                           ((u32)(((x >> 8) & 0x00000003)))
#define REGU_MASK_R1_L1                             ((u32)0x00000003 << 6)                   /*!< R/W   0x2
                                                                                              * //00 => 900k
                                                                                              * //01 => 720k
                                                                                              * //10 => 900k
                                                                                              * //11 => 1080k
                                                                                              */
#define REGU_R1_L1(x)                               ((u32)(((x) & 0x00000003) << 6))
#define REGU_GET_R1_L1(x)                           ((u32)(((x >> 6) & 0x00000003)))
#define REGU_MASK_C3_L1                             ((u32)0x00000003 << 4)                   /*!< R/W   0x2
                                                                                              * //00 => 1.59p
                                                                                              * //01 => 3.2p
                                                                                              * //10 => 4.76p
                                                                                              * //11 => 6.34p
                                                                                              */
#define REGU_C3_L1(x)                               ((u32)(((x) & 0x00000003) << 4))
#define REGU_GET_C3_L1(x)                           ((u32)(((x >> 4) & 0x00000003)))
#define REGU_MASK_C2_L1                             ((u32)0x00000003 << 2)                   /*!< R/W   0x2
                                                                                              * //00 => 13f
                                                                                              * //01 => 26f
                                                                                              * //10 => 39f
                                                                                              * //11 => 41f
                                                                                              */
#define REGU_C2_L1(x)                               ((u32)(((x) & 0x00000003) << 2))
#define REGU_GET_C2_L1(x)                           ((u32)(((x >> 2) & 0x00000003)))
#define REGU_MASK_C1_L1                             ((u32)0x00000003 << 0)                   /*!< R/W   0x2
                                                                                              * //00 => 1.03p
                                                                                              * //01 => 2.06p
                                                                                              * //10 => 3.1p
                                                                                              * //11 => 4.12p
                                                                                              */
#define REGU_C1_L1(x)                               ((u32)(((x) & 0x00000003) << 0))
#define REGU_GET_C1_L1(x)                           ((u32)(((x >> 0) & 0x00000003)))
/** @} */


/** @defgroup REG_REGU_SWR_SRC_CTRL4
 * @brief
 * @{
 */
#define REGU_MASK_SRC_DBG                           ((u32)0x00000003 << 28)                  /*!< R     0  
                                                                                              * REG_SRC_ANAPARSW_MAC_OFF[27:26] SRC_DTEST_SWREG_L<1>  SRC_DTEST_SWREG_L<0>
                                                                                              * 2'b00                                                             (1'b1=PWM, 1'b0=PFM)               RAMPOVER
                                                                                              * 2'b01                                                              NI                                                  PI
                                                                                              * 2'b10                                                             CKSS                                            SSOVER
                                                                                              * 2'b11                                                             i                                                      NIOFF_H
                                                                                              * 
                                                                                              */
#define REGU_SRC_DBG(x)                             ((u32)(((x) & 0x00000003) << 28))
#define REGU_GET_SRC_DBG(x)                         ((u32)(((x >> 28) & 0x00000003)))
#define REGU_BIT_TESTCHIP_DIG_ZCD_CNT_SELECT        ((u32)0x00000001 << 13)                  /*!< R/W   0  
                                                                                              * //TOP ZCD counter select
                                                                                              * //0(default) : reset by 10000
                                                                                              * //1 : reset by 11111
                                                                                              */
#define REGU_BIT_DUMMY_0                            ((u32)0x00000001 << 12)                  /*!< R/W   0   //Dummy */
#define REGU_BIT_REG_VC_CLAMP_SEL_L                 ((u32)0x00000001 << 11)                  /*!< R/W   1  
                                                                                              * // 0(default) : unity gain feedback clamp VC=VREF
                                                                                              * // 1.orignal clamp(roll back option)
                                                                                              */
#define REGU_BIT_SRC_LDO_FFWCAP_EN_L                ((u32)0x00000001 << 10)                  /*!< R/W   0  
                                                                                              * //LDO feedforward cap
                                                                                              * //0 : disable
                                                                                              * //1 : enable (default)
                                                                                              */
#define REGU_BIT_SRC_LDO_BUFFER_DIS_L               ((u32)0x00000001 << 9)                   /*!< R/W   0  
                                                                                              * //LDO buffer
                                                                                              * //0 : enable (default)
                                                                                              * //1 : disable
                                                                                              */
#define REGU_BIT_SRC_LDO_STANDBY_EN_L               ((u32)0x00000001 << 8)                   /*!< R/W   1  
                                                                                              * //LDO_MODE
                                                                                              * //0 : Normal Mode (default)
                                                                                              * //1 : Standby Mode
                                                                                              */
#define REGU_MASK_DUMMY_1                           ((u32)0x00000003 << 6)                   /*!< R/W   0   BG TC tune */
#define REGU_DUMMY_1(x)                             ((u32)(((x) & 0x00000003) << 6))
#define REGU_GET_DUMMY_1(x)                         ((u32)(((x >> 6) & 0x00000003)))
#define REGU_BIT_NMOS_OFF_L                         ((u32)0x00000001 << 5)                   /*!< R/W   0  
                                                                                              * //0:normal
                                                                                              * //1:NMOS always off
                                                                                              */
#define REGU_BIT_MUX_PI_L                           ((u32)0x00000001 << 4)                   /*!< R/W   0  
                                                                                              * //1:PI (original)
                                                                                              * //0:i
                                                                                              */
#define REGU_BIT_PWM_CTRL_L                         ((u32)0x00000001 << 3)                   /*!< R/W   0  
                                                                                              * //0:internal PWM
                                                                                              * //1:external PWM
                                                                                              */
#define REGU_BIT_ENSR_L                             ((u32)0x00000001 << 2)                   /*!< R/W   1   //1:with SSR latch */
#define REGU_MASK_SDZP_L                            ((u32)0x00000003 << 0)                   /*!< R/W   1   //Nonoverlap delay cell for PMOS */
#define REGU_SDZP_L(x)                              ((u32)(((x) & 0x00000003) << 0))
#define REGU_GET_SDZP_L(x)                          ((u32)(((x >> 0) & 0x00000003)))
/** @} */


/** @defgroup REG_REGU_SWR_SRC_OCP
 * @brief
 * @{
 */
#define REGU_BIT_SWR_OCP_EN                         ((u32)0x00000001 << 31)                  /*!< R/W   1   SRC OCP function enable */
#define REGU_MASK_SWR_OCP_THD_14_0                  ((u32)0x00007FFF << 16)                  /*!< R/W   0   SRC OCP threshold */
#define REGU_SWR_OCP_THD_14_0(x)                    ((u32)(((x) & 0x00007FFF) << 16))
#define REGU_GET_SWR_OCP_THD_14_0(x)                ((u32)(((x >> 16) & 0x00007FFF)))
#define REGU_MASK_SWR_OCP_WND_15_0                  ((u32)0x0000FFFF << 0)                   /*!< R/W   0   SRC OCP time windows */
#define REGU_SWR_OCP_WND_15_0(x)                    ((u32)(((x) & 0x0000FFFF) << 0))
#define REGU_GET_SWR_OCP_WND_15_0(x)                ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_REGU_SWR_ZCD_CTRL
 * @brief
 * @{
 */
#define REGU_MASK_ZCD_FORCE_VALUE                   ((u32)0x0000001F << 8)                   /*!< R/W   0   ZCD Force Value */
#define REGU_ZCD_FORCE_VALUE(x)                     ((u32)(((x) & 0x0000001F) << 8))
#define REGU_GET_ZCD_FORCE_VALUE(x)                 ((u32)(((x >> 8) & 0x0000001F)))
#define REGU_BIT_ZCD_FORCE_EN                       ((u32)0x00000001 << 7)                   /*!< R/W   0   1:Enable ZCD Force */
#define REGU_MASK_ZCD_VALUE                         ((u32)0x0000003F << 1)                   /*!< R     0x10 Read zero current detection value */
#define REGU_ZCD_VALUE(x)                           ((u32)(((x) & 0x0000003F) << 1))
#define REGU_GET_ZCD_VALUE(x)                       ((u32)(((x >> 1) & 0x0000003F)))
#define REGU_BIT_ZCD_EN                             ((u32)0x00000001 << 0)                   /*!< R/W   0   1:Enable ZCD Circuit */
/** @} */


/** @defgroup REG_REGU_SWR_SRC_LV_CTRL
 * @brief
 * @{
 */
#define REGU_BIT_COUNT_READY                        ((u32)0x00000001 << 6)                   /*!< R     0   1:PMC Done */
#define REGU_BIT_DOWN_EN                            ((u32)0x00000001 << 5)                   /*!< R/W   0   1:Enable PMC to Increase SWR_SRC output Level */
#define REGU_BIT_UP_EN                              ((u32)0x00000001 << 4)                   /*!< R/W   0   1:Enable PMC to Decrease SWR_SRC output Level */
#define REGU_MASK_PFM_LV                            ((u32)0x0000000F << 0)                   /*!< R/W   0x2 PFM Output level under LDO WLON at Bypss Mode */
#define REGU_PFM_LV(x)                              ((u32)(((x) & 0x0000000F) << 0))
#define REGU_GET_PFM_LV(x)                          ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup REG_REGU_LDO_PSRAM_CTRL
 * @brief
 * @{
 */
#define REGU_MASK_REG_DMY_LDO_L_1_0_028             ((u32)0x00000003 << 14)                  /*!< R/W   0x0
                                                                                              * // [REGU][LDO_PSRAM] REG_CTRL
                                                                                              * // LDO dmy REG
                                                                                              */
#define REGU_REG_DMY_LDO_L_1_0_028(x)               ((u32)(((x) & 0x00000003) << 14))
#define REGU_GET_REG_DMY_LDO_L_1_0_028(x)           ((u32)(((x >> 14) & 0x00000003)))
#define REGU_BIT_REG_FORCE_DIODE_L_028              ((u32)0x00000001 << 13)                  /*!< R/W   0x0
                                                                                              * // [REGU][LDO_PSRAM] REG_CTRL
                                                                                              * // PGATE diode clamp (this is directly force no gating)
                                                                                              * // (purpose for powerCut mode iOCP)
                                                                                              * // 0: disable force diode clamp (default)
                                                                                              * // 1: enable force diode clamp
                                                                                              */
#define REGU_BIT_REG_BUFFERTYPE_SEL_L_028           ((u32)0x00000001 << 12)                  /*!< R/W   0x0
                                                                                              * // [REGU][LDO_PSRAM] REG_CTRL
                                                                                              * // LDO Buffer Type Select // LDO_PSRAM should always set PMOS buffer
                                                                                              * // 0(default): PMOS buffer (for HV 3P3V)
                                                                                              * // 1: NMOS buffer (for HV 1P8V)
                                                                                              */
#define REGU_MASK_REG_DMYLOAD_L_1_0_028             ((u32)0x00000003 << 10)                  /*!< R/W   0x0
                                                                                              * // [REGU][LDO_PSRAM] REG_CTRL
                                                                                              * // LDO dmy load ctrl
                                                                                              * // 00(default): dmy load disable
                                                                                              * // 01: light dmy load enable (~160kohm)
                                                                                              * // 10: dmy load enable (~1kohm)
                                                                                              * // 11: dmy load + light dmy load both enable
                                                                                              */
#define REGU_REG_DMYLOAD_L_1_0_028(x)               ((u32)(((x) & 0x00000003) << 10))
#define REGU_GET_REG_DMYLOAD_L_1_0_028(x)           ((u32)(((x >> 10) & 0x00000003)))
#define REGU_BIT_REG_ITAIL_HALF_EN_L_028            ((u32)0x00000001 << 9)                   /*!< R/W   0x0
                                                                                              * // [REGU][LDO_PSRAM] REG_CTRL
                                                                                              * // LDO iq half ctrl
                                                                                              * // 0(default): iq normal (for normal mode)
                                                                                              * // 1: iq half (for standby mode)
                                                                                              */
#define REGU_MASK_REG_STANDBY_L_1_0_028             ((u32)0x00000003 << 7)                   /*!< R/W   0x0
                                                                                              * // [REGU][LDO_PSRAM] REG_CTRL
                                                                                              * // LDO standby mode ctrl:
                                                                                              * // 00 (default): normal mode (iq normal + pwrmos 4x)
                                                                                              * // 01: debug mode (iq reduce + pwrmos 1x)
                                                                                              * // 10: debug mode (iq normal + pwrmos 4x)
                                                                                              * // 11: standby mode (iq reduce + pwrmos 1x)
                                                                                              */
#define REGU_REG_STANDBY_L_1_0_028(x)               ((u32)(((x) & 0x00000003) << 7))
#define REGU_GET_REG_STANDBY_L_1_0_028(x)           ((u32)(((x >> 7) & 0x00000003)))
#define REGU_BIT_EN_DIODE_L_028                     ((u32)0x00000001 << 6)                   /*!< R/W   0x1
                                                                                              * // [REGU][LDO_PSRAM] REG_CTRL
                                                                                              * // PGATE diode clamp (this gating to disable by pd_regu_L)
                                                                                              * // 0: disable clamp
                                                                                              * // 1: enable clamp (default)
                                                                                              */
#define REGU_MASK_VOADJ_L_3_0_028                   ((u32)0x0000000F << 2)                   /*!< R/W   0x8
                                                                                              * // [REGU][LDO_PSRAM] REG_CTRL
                                                                                              * // LDO VOSEL
                                                                                              * // 0000, 0001, 0010, 0011: 1.617, 1.642, 1.667, 1.694
                                                                                              * // 0100, 0101, 0110, 0111: 1.721, 1.749, 1.763, 1.778
                                                                                              * // 1000(default), 1001, 1010, 1011: 1.793, 1.839, 1.887, 1.938
                                                                                              * // 1100, 1101, 1110, 1111: 1.992, 2.905, 2.979, 3.047
                                                                                              */
#define REGU_VOADJ_L_3_0_028(x)                     ((u32)(((x) & 0x0000000F) << 2))
#define REGU_GET_VOADJ_L_3_0_028(x)                 ((u32)(((x >> 2) & 0x0000000F)))
#define REGU_BIT_PD_REGU_L_028                      ((u32)0x00000001 << 1)                   /*!< R/W   0x1
                                                                                              * // [REGU][LDO_PSRAM] REG_CTRL
                                                                                              * // PowerDown CTRL
                                                                                              * // 0: normal-Mode
                                                                                              * // 1: power-down Mode (default)
                                                                                              */
#define REGU_BIT_EN_PC_BT_L_028                     ((u32)0x00000001 << 0)                   /*!< R/W   0x0
                                                                                              * // [REGU][LDO_PSRAM] REG_CTRL
                                                                                              * // PWRCUT enable
                                                                                              * // 0: LDO-Mode (default)
                                                                                              * // 1: PWRCUT-Mode
                                                                                              */
/** @} */


/** @defgroup REG_REGU_LDO_SDIO_CTRL
 * @brief
 * @{
 */
#define REGU_MASK_RESERVED                          ((u32)0x0000000F << 16)                  /*!< R/W   0x0 None */
#define REGU_RESERVED(x)                            ((u32)(((x) & 0x0000000F) << 16))
#define REGU_GET_RESERVED(x)                        ((u32)(((x >> 16) & 0x0000000F)))
#define REGU_MASK_REG_DMY_LDO_L_1_0_02C             ((u32)0x00000003 << 14)                  /*!< R/W   0x0
                                                                                              * // [REGU][LDO_SDIO] REG_CTRL
                                                                                              * // LDO dmy REG
                                                                                              */
#define REGU_REG_DMY_LDO_L_1_0_02C(x)               ((u32)(((x) & 0x00000003) << 14))
#define REGU_GET_REG_DMY_LDO_L_1_0_02C(x)           ((u32)(((x >> 14) & 0x00000003)))
#define REGU_BIT_REG_FORCE_DIODE_L_02C              ((u32)0x00000001 << 13)                  /*!< R/W   0x0
                                                                                              * // [REGU][LDO_SDIO] REG_CTRL
                                                                                              * // PGATE diode clamp (this is directly force no gating)
                                                                                              * // (purpose for powerCut mode iOCP)
                                                                                              * // 0: disable force diode clamp (default)
                                                                                              * // 1: enable force diode clamp
                                                                                              */
#define REGU_BIT_REG_BUFFERTYPE_SEL_L_02C           ((u32)0x00000001 << 12)                  /*!< R/W   0x0
                                                                                              * // [REGU][LDO_SDIO] REG_CTRL
                                                                                              * // LDO Buffer Type Select // LDO_SDIO should always set PMOS buffer
                                                                                              * // 0(default): PMOS buffer (for HV 3P3V)
                                                                                              * // 1: NMOS buffer (for HV 1P8V)
                                                                                              */
#define REGU_MASK_REG_DMYLOAD_L_1_0_02C             ((u32)0x00000003 << 10)                  /*!< R/W   0x0
                                                                                              * // [REGU][LDO_SDIO] REG_CTRL
                                                                                              * // LDO dmy load ctrl
                                                                                              * // 00(default): dmy load disable
                                                                                              * // 01: light dmy load enable (~160kohm)
                                                                                              * // 10: dmy load enable (~1kohm)
                                                                                              * // 11: dmy load + light dmy load both enable
                                                                                              */
#define REGU_REG_DMYLOAD_L_1_0_02C(x)               ((u32)(((x) & 0x00000003) << 10))
#define REGU_GET_REG_DMYLOAD_L_1_0_02C(x)           ((u32)(((x >> 10) & 0x00000003)))
#define REGU_BIT_REG_ITAIL_HALF_EN_L_02C            ((u32)0x00000001 << 9)                   /*!< R/W   0x0
                                                                                              * // [REGU][LDO_SDIO] REG_CTRL
                                                                                              * // LDO iq half ctrl
                                                                                              * // 0(default): iq normal (for normal mode)
                                                                                              * // 1: iq half (for standby mode)
                                                                                              */
#define REGU_MASK_REG_STANDBY_L_1_0_02C             ((u32)0x00000003 << 7)                   /*!< R/W   0x0
                                                                                              * // [REGU][LDO_SDIO] REG_CTRL
                                                                                              * // LDO standby mode ctrl:
                                                                                              * // 00 (default): normal mode (iq normal + pwrmos 4x)
                                                                                              * // 01: debug mode (iq reduce + pwrmos 1x)
                                                                                              * // 10: debug mode (iq normal + pwrmos 4x)
                                                                                              * // 11: standby mode (iq reduce + pwrmos 1x)
                                                                                              */
#define REGU_REG_STANDBY_L_1_0_02C(x)               ((u32)(((x) & 0x00000003) << 7))
#define REGU_GET_REG_STANDBY_L_1_0_02C(x)           ((u32)(((x >> 7) & 0x00000003)))
#define REGU_BIT_EN_DIODE_L_02C                     ((u32)0x00000001 << 6)                   /*!< R/W   0x1
                                                                                              * // [REGU][LDO_SDIO] REG_CTRL
                                                                                              * // PGATE diode clamp (this gating to disable by pd_regu_L)
                                                                                              * // 0: disable clamp
                                                                                              * // 1: enable clamp (default)
                                                                                              */
#define REGU_MASK_VOADJ_L_3_0_02C                   ((u32)0x0000000F << 2)                   /*!< R/W   0x8
                                                                                              * // LDO VOSEL
                                                                                              * // 0000 1.617
                                                                                              * // 0001 1.642
                                                                                              * // 0010 1.667
                                                                                              * // 0011 1.694
                                                                                              * // 0100 1.721
                                                                                              * // 0101 1.749
                                                                                              * // 0110 1.763
                                                                                              * // 0111 1.778
                                                                                              * // 1000 1.793
                                                                                              * // 1001 1.839
                                                                                              * // 1010 1.887
                                                                                              * // 1011 1.938
                                                                                              * // 1100 1.992
                                                                                              * // 1101 2.905
                                                                                              * // 1110 2.979
                                                                                              * // 1111 3.047
                                                                                              */
#define REGU_VOADJ_L_3_0_02C(x)                     ((u32)(((x) & 0x0000000F) << 2))
#define REGU_GET_VOADJ_L_3_0_02C(x)                 ((u32)(((x >> 2) & 0x0000000F)))
#define REGU_BIT_PD_REGU_L_02C                      ((u32)0x00000001 << 1)                   /*!< R/W   0x1
                                                                                              * // [REGU][LDO_SDIO] REG_CTRL
                                                                                              * // PowerDown CTRL
                                                                                              * // 0: normal-Mode
                                                                                              * // 1: power-down Mode (default)
                                                                                              */
#define REGU_BIT_EN_PC_BT_L_02C                     ((u32)0x00000001 << 0)                   /*!< R/W   0x0
                                                                                              * // [REGU][LDO_SDIO] REG_CTRL
                                                                                              * // PWRCUT enable
                                                                                              * // 0: LDO-Mode (default)
                                                                                              * // 1: PWRCUT-Mode
                                                                                              */
/** @} */


/** @defgroup REG_REGU_XTAL32K_CTRL0
 * @brief
 * @{
 */
#define REGU_MASK_AON_XTAL32K_GM_3_0                ((u32)0x0000000F << 28)                  /*!< R/W   0xF gm size */
#define REGU_AON_XTAL32K_GM_3_0(x)                  ((u32)(((x) & 0x0000000F) << 28))
#define REGU_GET_AON_XTAL32K_GM_3_0(x)              ((u32)(((x >> 28) & 0x0000000F)))
#define REGU_MASK_AON_XTAL32K_CUR_REP_1_0           ((u32)0x00000003 << 26)                  /*!< R/W   1   replica buffer current */
#define REGU_AON_XTAL32K_CUR_REP_1_0(x)             ((u32)(((x) & 0x00000003) << 26))
#define REGU_GET_AON_XTAL32K_CUR_REP_1_0(x)         ((u32)(((x >> 26) & 0x00000003)))
#define REGU_MASK_AON_XTAL32K_CUR_GM_3_0            ((u32)0x0000000F << 22)                  /*!< R/W   0x5 awake gm current, 7pXtal: 0100 */
#define REGU_AON_XTAL32K_CUR_GM_3_0(x)              ((u32)(((x) & 0x0000000F) << 22))
#define REGU_GET_AON_XTAL32K_CUR_GM_3_0(x)          ((u32)(((x >> 22) & 0x0000000F)))
#define REGU_MASK_AON_XTAL32K_CUR_GM_INI_3_0        ((u32)0x0000000F << 18)                  /*!< R/W   0x9 initial gm current, 7pXtal: 0100 */
#define REGU_AON_XTAL32K_CUR_GM_INI_3_0(x)          ((u32)(((x) & 0x0000000F) << 18))
#define REGU_GET_AON_XTAL32K_CUR_GM_INI_3_0(x)      ((u32)(((x >> 18) & 0x0000000F)))
#define REGU_MASK_AON_XTAL32K_CUR_MAIN_1_0          ((u32)0x00000003 << 16)                  /*!< R/W   1   xtal main current */
#define REGU_AON_XTAL32K_CUR_MAIN_1_0(x)            ((u32)(((x) & 0x00000003) << 16))
#define REGU_GET_AON_XTAL32K_CUR_MAIN_1_0(x)        ((u32)(((x >> 16) & 0x00000003)))
#define REGU_BIT_AON_POW_XTAL32K                    ((u32)0x00000001 << 0)                   /*!< R/W   0   xtal 32KHz enable */
/** @} */


/** @defgroup REG_REGU_XTAL32K_CTRL1
 * @brief
 * @{
 */
#define REGU_BIT_AON_XTAL32K_GPIO_SEL               ((u32)0x00000001 << 22)                  /*!< R/W   1  
                                                                                              * 0: 32K  1: GPIO
                                                                                              * Should be efuse control
                                                                                              */
#define REGU_BIT_AON_XTAL32K_SC_XO_EXTRA            ((u32)0x00000001 << 21)                  /*!< R/W   0   0: w/i 2.8pF cap 1: w/o 2.8pF cap  */
#define REGU_BIT_AON_XTAL32K_SC_XI_EXTRA            ((u32)0x00000001 << 20)                  /*!< R/W   0   0: w/i 2.8pF cap 1: w/o 2.8pF cap  */
#define REGU_MASK_AON_XTAL32K_GM_REP_2_0            ((u32)0x00000007 << 17)                  /*!< R/W   0x7 repilca size */
#define REGU_AON_XTAL32K_GM_REP_2_0(x)              ((u32)(((x) & 0x00000007) << 17))
#define REGU_GET_AON_XTAL32K_GM_REP_2_0(x)          ((u32)(((x >> 17) & 0x00000007)))
#define REGU_BIT_AON_ENB_XTAL32K_FBRES              ((u32)0x00000001 << 16)                  /*!< R/W   0   0: enable feedback resistor */
#define REGU_MASK_AON_XTAL32K_TOK_1_0               ((u32)0x00000003 << 14)                  /*!< R/W   0x3 00: 100ms 01:200ms 10:400ms 11: 800ms */
#define REGU_AON_XTAL32K_TOK_1_0(x)                 ((u32)(((x) & 0x00000003) << 14))
#define REGU_GET_AON_XTAL32K_TOK_1_0(x)             ((u32)(((x >> 14) & 0x00000003)))
#define REGU_MASK_AON_XTAL32K_SC_XO_5_0             ((u32)0x0000003F << 8)                   /*!< R/W   0x20 xo cap */
#define REGU_AON_XTAL32K_SC_XO_5_0(x)               ((u32)(((x) & 0x0000003F) << 8))
#define REGU_GET_AON_XTAL32K_SC_XO_5_0(x)           ((u32)(((x >> 8) & 0x0000003F)))
#define REGU_MASK_AON_XTAL32K_SC_XI_5_0             ((u32)0x0000003F << 2)                   /*!< R/W   0x20 xi cap */
#define REGU_AON_XTAL32K_SC_XI_5_0(x)               ((u32)(((x) & 0x0000003F) << 2))
#define REGU_GET_AON_XTAL32K_SC_XI_5_0(x)           ((u32)(((x >> 2) & 0x0000003F)))
#define REGU_BIT_AON_EN_XTAL32K_CAP_AWAKE           ((u32)0x00000001 << 1)                   /*!< R/W   1   internal cap enable as awake */
#define REGU_BIT_AON_EN_XTAL32K_CAP_INITIAL         ((u32)0x00000001 << 0)                   /*!< R/W   1   internal cap enable as initial */
/** @} */


/** @defgroup REG_REGU_OSC32K_CTRL0
 * @brief
 * @{
 */
#define REGU_BIT_AON_SWR_SRC_SWITCH                 ((u32)0x00000001 << 30)                  /*!< R/W   0  
                                                                                              * 1: Connect to REGU+0x0C[3:0]
                                                                                              * 0: Connect to swr_ctrl for sleep up/down in Low power mode transition
                                                                                              */
#define REGU_BIT_AON_SEL_LDO_VREF                   ((u32)0x00000001 << 29)                  /*!< R/W   0   0: LDO low valtage setting 1:high voltage */
#define REGU_MASK_AON_RCAL_5_0                      ((u32)0x0000003F << 16)                  /*!< R/W   0x20 osc frequency is highier as RCAL's larger */
#define REGU_AON_RCAL_5_0(x)                        ((u32)(((x) & 0x0000003F) << 16))
#define REGU_GET_AON_RCAL_5_0(x)                    ((u32)(((x >> 16) & 0x0000003F)))
#define REGU_MASK_AON_DUMMY_14_6                    ((u32)0x000001FF << 3)                   /*!< R/W   0   Dummy */
#define REGU_AON_DUMMY_14_6(x)                      ((u32)(((x) & 0x000001FF) << 3))
#define REGU_GET_AON_DUMMY_14_6(x)                  ((u32)(((x >> 3) & 0x000001FF)))
#define REGU_MASK_AON_DUMMY_1_0                     ((u32)0x00000003 << 1)                   /*!< R/W   0   // [RF][32KOSC] dummy */
#define REGU_AON_DUMMY_1_0(x)                       ((u32)(((x) & 0x00000003) << 1))
#define REGU_GET_AON_DUMMY_1_0(x)                   ((u32)(((x >> 1) & 0x00000003)))
#define REGU_BIT_AON_POW_OSC32K                     ((u32)0x00000001 << 0)                   /*!< R/W   0   32k osc enable */
/** @} */


/** @defgroup REG_REGU_OSC4M_CTRL
 * @brief
 * @{
 */
#define REGU_BIT_C_SEL                              ((u32)0x00000001 << 29)                  /*!< R/W     0 None */
#define REGU_BIT_EN_4MOSC                           ((u32)0x00000001 << 28)                  /*!< R/W     0 [4MOSC] 1'b1 : enable */
#define REGU_BIT_POW33_LVSFT                        ((u32)0x00000001 << 27)                  /*!< R/W     0 None */
#define REGU_MASK_IB_BIAS_SEL_4_0                   ((u32)0x0000001F << 22)                  /*!< R/W   0xF None */
#define REGU_IB_BIAS_SEL_4_0(x)                     ((u32)(((x) & 0x0000001F) << 22))
#define REGU_GET_IB_BIAS_SEL_4_0(x)                 ((u32)(((x >> 22) & 0x0000001F)))
#define REGU_MASK_VCM_SEL_H_3_0                     ((u32)0x0000000F << 18)                  /*!< R/W   0xB None */
#define REGU_VCM_SEL_H_3_0(x)                       ((u32)(((x) & 0x0000000F) << 18))
#define REGU_GET_VCM_SEL_H_3_0(x)                   ((u32)(((x >> 18) & 0x0000000F)))
#define REGU_MASK_VCM_SEL_L_3_0                     ((u32)0x0000000F << 14)                  /*!< R/W   0xB None */
#define REGU_VCM_SEL_L_3_0(x)                       ((u32)(((x) & 0x0000000F) << 14))
#define REGU_GET_VCM_SEL_L_3_0(x)                   ((u32)(((x >> 14) & 0x0000000F)))
#define REGU_MASK_FREQ_R_SEL_7_0                    ((u32)0x000000FF << 6)                   /*!< R/W   0x80 None */
#define REGU_FREQ_R_SEL_7_0(x)                      ((u32)(((x) & 0x000000FF) << 6))
#define REGU_GET_FREQ_R_SEL_7_0(x)                  ((u32)(((x >> 6) & 0x000000FF)))
#define REGU_BIT_AON_POW_4MOSC_YCC                  ((u32)0x00000001 << 5)                   /*!< R/W   1   REG_POW_4MOSC_YCC */
#define REGU_MASK_AON_OSC4M_FSET_YCC_3_0            ((u32)0x0000000F << 1)                   /*!< R/W   0x8 osc freq control ,code up freq up */
#define REGU_AON_OSC4M_FSET_YCC_3_0(x)              ((u32)(((x) & 0x0000000F) << 1))
#define REGU_GET_AON_OSC4M_FSET_YCC_3_0(x)          ((u32)(((x >> 1) & 0x0000000F)))
#define REGU_BIT_AON_POW_4MOSC                      ((u32)0x00000001 << 0)                   /*!< R/W     0 REG_POW_4MOSC */
/** @} */


/** @defgroup REG_REGU_PFM_PWM_CFG
 * @brief
 * @{
 */
#define REGU_MASK_PFM_OCP                           ((u32)0x00000007 << 11)                  /*!< R/W   0x3 Mapping to REGU+0x08[25:23] once FPWM_L1=0 */
#define REGU_PFM_OCP(x)                             ((u32)(((x) & 0x00000007) << 11))
#define REGU_GET_PFM_OCP(x)                         ((u32)(((x >> 11) & 0x00000007)))
#define REGU_MASK_PFM_ZCD_H                         ((u32)0x00000003 << 9)                   /*!< R/W   1   Mapping to REGU+0x08[28:27] once FPWM_L1=0 */
#define REGU_PFM_ZCD_H(x)                           ((u32)(((x) & 0x00000003) << 9))
#define REGU_GET_PFM_ZCD_H(x)                       ((u32)(((x >> 9) & 0x00000003)))
#define REGU_BIT_PFM_MOS_HALF                       ((u32)0x00000001 << 8)                   /*!< R/W   1   Mapping to REGU+0x0C[11] once FPWM_L1=0 */
#define REGU_MASK_PWM_OCP                           ((u32)0x00000007 << 3)                   /*!< R/W   0x3 Mapping to REGU+0x08[25:23] once FPWM_L1=1 */
#define REGU_PWM_OCP(x)                             ((u32)(((x) & 0x00000007) << 3))
#define REGU_GET_PWM_OCP(x)                         ((u32)(((x >> 3) & 0x00000007)))
#define REGU_MASK_PWM_ZCD_H                         ((u32)0x00000003 << 1)                   /*!< R/W   0x2 Mapping to REGU+0x08[28:27] once FPWM_L1=1 */
#define REGU_PWM_ZCD_H(x)                           ((u32)(((x) & 0x00000003) << 1))
#define REGU_GET_PWM_ZCD_H(x)                       ((u32)(((x >> 1) & 0x00000003)))
#define REGU_BIT_PWM_MOS_HALF                       ((u32)0x00000001 << 0)                   /*!< R/W     0 Mapping to REGU+0x0C[11] once FPWM_L1=1 */
/** @} */


/** @defgroup REG_REGU_MISC
 * @brief
 * @{
 */
#define REGU_BIT_AUDIO_STB_EN                       ((u32)0x00000001 << 3)                   /*!< R/W   0x0 1: Enable STB  */
#define REGU_BIT_AON_REGU_CLK_RDY                   ((u32)0x00000001 << 2)                   /*!< R     0x1 Regulator Clock 100KHz Clock Ready */
#define REGU_BIT_AON_32KXTAL_RDY                    ((u32)0x00000001 << 1)                   /*!< R     0x0 32KHz XTALClock Ready */
#define REGU_BIT_AON_SYS_AMACRO_EN                  ((u32)0x00000001 << 0)                   /*!< R/W   0x1 Analog IP PWD_B, REGU hw_pdb_12 */
/** @} */


/** @defgroup REG_REGU_SPARE0
 * @brief
 * @{
 */
#define REGU_MASK_SW_USE0                           ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0   RSVD */
#define REGU_SW_USE0(x)                             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define REGU_GET_SW_USE0(x)                         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_REGU_SPARE1
 * @brief
 * @{
 */
#define REGU_MASK_SW_USE1                           ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 RSVD */
#define REGU_SW_USE1(x)                             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define REGU_GET_SW_USE1(x)                         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */




/** @defgroup REGU_Exported_Types REGU Exported Types
* @{
*/
/**
* @brief REGU Register Declaration
*/

typedef struct {
	__IO uint32_t REGU_LDO_CTRL0;                               /*!< Register,  Address offset:0x0000 */
	__IO uint32_t REGU_WLON_LDO_CTRL0;                          /*!< Register,  Address offset:0x0004 */
	__IO uint32_t REGU_SWR_SRC_CTRL0;                           /*!< Register,  Address offset:0x0008 */
	__IO uint32_t REGU_SWR_SRC_CTRL1;                           /*!< Register,  Address offset:0x000C */
	__IO uint32_t REGU_SWR_SRC_CTRL2;                           /*!< Register,  Address offset:0x0010 */
	__IO uint32_t REGU_SWR_SRC_CTRL3;                           /*!< Register,  Address offset:0x0014 */
	__IO uint32_t REGU_SWR_SRC_CTRL4;                           /*!< Register,  Address offset:0x0018 */
	__IO uint32_t REGU_SWR_SRC_OCP;                             /*!< Register,  Address offset:0x001C */
	__IO uint32_t REGU_SWR_ZCD_CTRL;                            /*!< Register,  Address offset:0x0020 */
	__IO uint32_t REGU_SWR_SRC_LV_CTRL;                         /*!< Register,  Address offset:0x0024 */
	__IO uint32_t REGU_LDO_PSRAM_CTRL;                          /*!< Register,  Address offset:0x0028 */
	__IO uint32_t REGU_LDO_SDIO_CTRL;                           /*!< Register,  Address offset:0x002C */
	__IO uint32_t REGU_XTAL32K_CTRL0;                           /*!< Register,  Address offset:0x0030 */
	__IO uint32_t REGU_XTAL32K_CTRL1;                           /*!< Register,  Address offset:0x0034 */
	__IO uint32_t REGU_OSC32K_CTRL0;                            /*!< Register,  Address offset:0x0038 */
	__IO uint32_t REGU_OSC4M_CTRL;                              /*!< Register,  Address offset:0x003C */
	__IO uint32_t REGU_PFM_PWM_CFG;                             /*!< Register,  Address offset:0x0040 */
	__IO uint32_t REGU_MISC;                                    /*!< Register,  Address offset:0x0044 */
	__IO uint32_t REGU_SPARE0;                                  /*!< Register,  Address offset:0x0048 */
	__IO uint32_t REGU_SPARE1;                                  /*!< Register,  Address offset:0x004C */
} REGU_TypeDef;

#define REGU_BASE ((REGU_TypeDef *) (0x40226000))
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
#endif  // SYSREG_REGU_H
