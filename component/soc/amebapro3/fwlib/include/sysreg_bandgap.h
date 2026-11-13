/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_BANDGAP_H
#define SYSREG_BANDGAP_H

/* AUTO_GEN_START */

/* BANDGAP is a part of AON */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup BANDGAP
* @brief BANDGAP driver modules
* @{
*/


/** @defgroup REG_AON_PLL_CTRL0
 * @brief
 * @{
 */
#define BANDGAP_MASK_SETTLE_TIME_FACTOR             ((u32)0x00000003 << 24)                  /*!< R/W   0   0: delay *1, 1:delay *2, 2: delay *4, 3:delay *8 */
#define BANDGAP_SETTLE_TIME_FACTOR(x)               ((u32)(((x) & 0x00000003) << 24))
#define BANDGAP_GET_SETTLE_TIME_FACTOR(x)           ((u32)(((x >> 24) & 0x00000003)))
#define BANDGAP_BIT_SW_LDO2PWRCUT                   ((u32)0x00000001 << 23)                  /*!< R/W   0   0: LDO 1: power cut */
#define BANDGAP_MASK_SW_LDO_OUT_1_0                 ((u32)0x00000003 << 21)                  /*!< R/W   1  
                                                                                              * 00/01/10/11
                                                                                              * 0.85/0.9/0.95/1
                                                                                              */
#define BANDGAP_SW_LDO_OUT_1_0(x)                   ((u32)(((x) & 0x00000003) << 21))
#define BANDGAP_GET_SW_LDO_OUT_1_0(x)               ((u32)(((x >> 21) & 0x00000003)))
#define BANDGAP_BIT_VPULSE_LDO                      ((u32)0x00000001 << 20)                  /*!< R/W   1   None */
#define BANDGAP_BIT_POW_LDO09                       ((u32)0x00000001 << 19)                  /*!< R/W   0   Power on 13to09 LDO */
#define BANDGAP_MASK_VSET_1_0                       ((u32)0x00000003 << 17)                  /*!< R/W   0x2 None */
#define BANDGAP_VSET_1_0(x)                         ((u32)(((x) & 0x00000003) << 17))
#define BANDGAP_GET_VSET_1_0(x)                     ((u32)(((x >> 17) & 0x00000003)))
#define BANDGAP_BIT_POW_LDO18                       ((u32)0x00000001 << 16)                  /*!< R/W   0   Power on 33to18 LDO */
#define BANDGAP_BIT_AON_PLL_CTRL0_DUMMY_0           ((u32)0x00000001 << 15)                  /*!< R/W   0   None */
#define BANDGAP_MASK_PLL_VR1200_TUNE_2_0            ((u32)0x00000007 << 12)                  /*!< R/W   0x4 Add 40k per step fine tune BG */
#define BANDGAP_PLL_VR1200_TUNE_2_0(x)              ((u32)(((x) & 0x00000007) << 12))
#define BANDGAP_GET_PLL_VR1200_TUNE_2_0(x)          ((u32)(((x >> 12) & 0x00000007)))
#define BANDGAP_BIT_AON_PLL_CTRL0_DUMMY_1           ((u32)0x00000001 << 11)                  /*!< R/W   0   None */
#define BANDGAP_MASK_PLL_R2_TUNE_2_0                ((u32)0x00000007 << 8)                   /*!< R/W   0x4 Add 20k per step fine tune BG */
#define BANDGAP_PLL_R2_TUNE_2_0(x)                  ((u32)(((x) & 0x00000007) << 8))
#define BANDGAP_GET_PLL_R2_TUNE_2_0(x)              ((u32)(((x >> 8) & 0x00000007)))
#define BANDGAP_BIT_AON_PLL_CTRL0_DUMMY_2           ((u32)0x00000001 << 7)                   /*!< R/W   0   None */
#define BANDGAP_MASK_PLL_R1_TUNE_2_0                ((u32)0x00000007 << 4)                   /*!< R/W   0x4 Add 200k per step fine tune BG */
#define BANDGAP_PLL_R1_TUNE_2_0(x)                  ((u32)(((x) & 0x00000007) << 4))
#define BANDGAP_GET_PLL_R1_TUNE_2_0(x)              ((u32)(((x >> 4) & 0x00000007)))
#define BANDGAP_BIT_AON_PLL_CTRL0_DUMMY_3           ((u32)0x00000001 << 3)                   /*!< R/W   0   None */
#define BANDGAP_BIT_PLL_POW_MBIAS                   ((u32)0x00000001 << 2)                   /*!< R/W     0 power on mbias */
#define BANDGAP_BIT_PLL_POW_I                       ((u32)0x00000001 << 1)                   /*!< R/W     0 power on BG current */
#define BANDGAP_BIT_PLL_POW_BG                      ((u32)0x00000001 << 0)                   /*!< R/W     0 power on BG */
/** @} */


/** @defgroup REG_AON_PLL_CTRL1
 * @brief
 * @{
 */
#define BANDGAP_MASK_DUMMY                          ((u32)0x00000007 << 5)                   /*!< R/W   0   None */
#define BANDGAP_DUMMY(x)                            ((u32)(((x) & 0x00000007) << 5))
#define BANDGAP_GET_DUMMY(x)                        ((u32)(((x >> 5) & 0x00000007)))
#define BANDGAP_MASK_SW_SDO_OUT1                    ((u32)0x00000007 << 2)                   /*!< R/W   0x6
                                                                                              * 011/100/101/110/111
                                                                                              * 1.2/1.25/1.3/1.35/1.4  for 33to1p35 LDO
                                                                                              */
#define BANDGAP_SW_SDO_OUT1(x)                      ((u32)(((x) & 0x00000007) << 2))
#define BANDGAP_GET_SW_SDO_OUT1(x)                  ((u32)(((x >> 2) & 0x00000007)))
#define BANDGAP_BIT_POW_SOFT_START                  ((u32)0x00000001 << 1)                   /*!< R/W   1   1: Enable LDO soft start */
#define BANDGAP_BIT_POW_LDO_N                       ((u32)0x00000001 << 0)                   /*!< R/W   0   Power on 33to1p35 LDO and IR_REF */
/** @} */


/** @defgroup REG_AON_SD_CTRL0
 * @brief
 * @{
 */
#define BANDGAP_MASK_AON_SD_CTRL0_DUMMY_0           ((u32)0x00000003 << 18)                  /*!< R/W   0   None */
#define BANDGAP_AON_SD_CTRL0_DUMMY_0(x)             ((u32)(((x) & 0x00000003) << 18))
#define BANDGAP_GET_AON_SD_CTRL0_DUMMY_0(x)         ((u32)(((x >> 18) & 0x00000003)))
#define BANDGAP_MASK_SD_VOS_L_2_0                   ((u32)0x00000007 << 15)                  /*!< R/W   0x3 Offset tunning for comparator */
#define BANDGAP_SD_VOS_L_2_0(x)                     ((u32)(((x) & 0x00000007) << 15))
#define BANDGAP_GET_SD_VOS_L_2_0(x)                 ((u32)(((x >> 15) & 0x00000007)))
#define BANDGAP_MASK_SD_VOS_H_2_0                   ((u32)0x00000007 << 12)                  /*!< R/W   0x3 Offset tunning for comparator */
#define BANDGAP_SD_VOS_H_2_0(x)                     ((u32)(((x) & 0x00000007) << 12))
#define BANDGAP_GET_SD_VOS_H_2_0(x)                 ((u32)(((x >> 12) & 0x00000007)))
#define BANDGAP_MASK_SEL_VREFL_4_0                  ((u32)0x0000001F << 7)                   /*!< R/W   0   vref low voltage 0000:25mV, 25mV/step */
#define BANDGAP_SEL_VREFL_4_0(x)                    ((u32)(((x) & 0x0000001F) << 7))
#define BANDGAP_GET_SEL_VREFL_4_0(x)                ((u32)(((x >> 7) & 0x0000001F)))
#define BANDGAP_MASK_SEL_VREFH_4_0                  ((u32)0x0000001F << 2)                   /*!< R/W   0   vref high voltage 0000:25mV, 25mV/step */
#define BANDGAP_SEL_VREFH_4_0(x)                    ((u32)(((x) & 0x0000001F) << 2))
#define BANDGAP_GET_SEL_VREFH_4_0(x)                ((u32)(((x >> 2) & 0x0000001F)))
#define BANDGAP_BIT_SD_POSEDGE                      ((u32)0x00000001 << 1)                   /*!< R/W   1   1: vin>vref output high */
#define BANDGAP_BIT_POW_SD                          ((u32)0x00000001 << 0)                   /*!< R/W   0   Power on signal detector */
/** @} */


/** @defgroup REG_USB2_BG_CTRL
 * @brief
 * @{
 */
#define BANDGAP_BIT_BG_ON_USB2                      ((u32)0x00000001 << 5)                   /*!< R/W   0   None */
#define BANDGAP_BIT_BG_ON_MIPI                      ((u32)0x00000001 << 4)                   /*!< R/W   0   None */
#define BANDGAP_BIT_MAINPWR                         ((u32)0x00000001 << 3)                   /*!< R/W   0   None */
#define BANDGAP_MASK_BG                             ((u32)0x00000003 << 1)                   /*!< R/W   0x0 None */
#define BANDGAP_BG(x)                               ((u32)(((x) & 0x00000003) << 1))
#define BANDGAP_GET_BG(x)                           ((u32)(((x >> 1) & 0x00000003)))
#define BANDGAP_BIT_BG_LPF                          ((u32)0x00000001 << 0)                   /*!< R/W   0x0 None */
/** @} */




/** @defgroup BANDGAP_Exported_Types BANDGAP Exported Types
* @{
*/
/**
* @brief BANDGAP Register Declaration
*/

typedef struct {
	__IO uint32_t AON_PLL_CTRL0;                                /*!< Register,  Address offset:0x0000 */
	__IO uint32_t AON_PLL_CTRL1;                                /*!< Register,  Address offset:0x0004 */
	__IO uint32_t AON_SD_CTRL0;                                 /*!< Register,  Address offset:0x0008 */
	__IO uint32_t USB2_BG_CTRL;                                 /*!< Register,  Address offset:0x000C */
} BANDGAP_TypeDef;

#define BANDGAP_BASE ((BANDGAP_TypeDef *) (0x4022C000))
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
#endif  // SYSREG_BANDGAP_H
