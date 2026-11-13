/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AMEBA_ADC_H
#define AMEBA_ADC_H

/* AUTO_GEN_START */

/* ADC is a part of ADC */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup ADC
* @brief ADC driver modules
* @{
*/


/** @defgroup conf
 * @brief
 * @{
 */
#define ADC_BIT_EN                                  ((u32)0x00000001 << 9)                   /*!< R/W   0.0 This bit is for ADC enable control. */
#define ADC_MASK_CVLIST_LEN                         ((u32)0x0000000F << 4)                   /*!< R/W   0.0
                                                                                              * This field defines the number of items in the ADC conversion channel list.
                                                                                              * 0 is for 1 item in list.
                                                                                              * 1 is for 2 items in list.
                                                                                              */
#define ADC_CVLIST_LEN(x)                           ((u32)(((x) & 0x0000000F) << 4))
#define ADC_GET_CVLIST_LEN(x)                       ((u32)(((x >> 4) & 0x0000000F)))
#define ADC_MASK_OP_MOD                             ((u32)0x00000007 << 1)                   /*!< R/W   0.0
                                                                                              * These bits selects ADC operation mode.
                                                                                              * 0: Software Trigger Mode
                                                                                              * 1: Automatic Mode
                                                                                              * 2: Timer-Trigger Mode
                                                                                              */
#define ADC_OP_MOD(x)                               ((u32)(((x) & 0x00000007) << 1))
#define ADC_GET_OP_MOD(x)                           ((u32)(((x >> 1) & 0x00000007)))
#define ADC_BIT_REF_IN_SEL                          ((u32)0x00000001 << 0)                   /*!< R/W   0.0
                                                                                              * This bit selects ADC reference voltage input.
                                                                                              * 0: ADC's reference voltage is from internal source.
                                                                                              * 1: ADC's reference voltage is from external pin.
                                                                                              */
/** @} */


/** @defgroup in_type
 * @brief
 * @{
 */
#define ADC_MASK_CH7_IN_TYPE                        ((u32)0x00000003 << 14)                  /*!< R/W   0.0
                                                                                              * This bit controls channel 7 input type. If it is set to differential type (value of this bit is 1), channel 6 should also be set to differential type since they are hardware-fixed differential pair.
                                                                                              * 0: ADC channel 7 is single-end input.
                                                                                              * 1: ADC channel 7 is differential input which is paired with channel 6.
                                                                                              */
#define ADC_CH7_IN_TYPE(x)                          ((u32)(((x) & 0x00000003) << 14))
#define ADC_GET_CH7_IN_TYPE(x)                      ((u32)(((x >> 14) & 0x00000003)))
#define ADC_MASK_CH6_IN_TYPE                        ((u32)0x00000003 << 12)                  /*!< R/W   0.0
                                                                                              * This bit controls channel 6 input type. If it is set to differential type (value of this bit is 1), channel 7 should also be set to differential type since they are hardware-fixed differential pair.
                                                                                              * 0: ADC channel 6 is single-end input.
                                                                                              * 1: ADC channel 6 is differential input which is paired with channel 7.
                                                                                              */
#define ADC_CH6_IN_TYPE(x)                          ((u32)(((x) & 0x00000003) << 12))
#define ADC_GET_CH6_IN_TYPE(x)                      ((u32)(((x >> 12) & 0x00000003)))
#define ADC_MASK_CH5_IN_TYPE                        ((u32)0x00000003 << 10)                  /*!< R/W   0.0
                                                                                              * This bit controls channel 5 input type. If it is set to differential type (value of this bit is 1), channel 4 should also be set to differential type since they are hardware-fixed differential pair.
                                                                                              * 0: ADC channel 5 is single-end input.
                                                                                              * 1: ADC channel 5 is differential input which is paired with channel 4.
                                                                                              */
#define ADC_CH5_IN_TYPE(x)                          ((u32)(((x) & 0x00000003) << 10))
#define ADC_GET_CH5_IN_TYPE(x)                      ((u32)(((x >> 10) & 0x00000003)))
#define ADC_MASK_CH4_IN_TYPE                        ((u32)0x00000003 << 8)                   /*!< R/W   0.0
                                                                                              * This bit controls channel 4 input type. If it is set to differential type (value of this bit is 1), channel 5 should also be set to differential type since they are hardware-fixed differential pair.
                                                                                              * 0: ADC channel 4 is single-end input.
                                                                                              * 1: ADC channel 4 is differential input which is paired with channel 5.
                                                                                              */
#define ADC_CH4_IN_TYPE(x)                          ((u32)(((x) & 0x00000003) << 8))
#define ADC_GET_CH4_IN_TYPE(x)                      ((u32)(((x >> 8) & 0x00000003)))
#define ADC_MASK_CH3_IN_TYPE                        ((u32)0x00000003 << 6)                   /*!< R/W   0.0
                                                                                              * This bit controls channel 3 input type. If it is set to differential type (value of this bit is 1), channel 2 should also be set to differential type since they are hardware-fixed differential pair.
                                                                                              * 0: ADC channel 3 is single-end input.
                                                                                              * 1: ADC channel 3 is differential input which is paired with channel 2.
                                                                                              */
#define ADC_CH3_IN_TYPE(x)                          ((u32)(((x) & 0x00000003) << 6))
#define ADC_GET_CH3_IN_TYPE(x)                      ((u32)(((x >> 6) & 0x00000003)))
#define ADC_MASK_CH2_IN_TYPE                        ((u32)0x00000003 << 4)                   /*!< R/W   0.0
                                                                                              * This bit controls channel 2 input type. If it is set to differential type (value of this bit is 1), channel 3 should also be set to differential type since they are hardware-fixed differential pair.
                                                                                              * 0: ADC channel 2 is single-end input.
                                                                                              * 1: ADC channel 2 is differential input which is paired with channel 3.
                                                                                              */
#define ADC_CH2_IN_TYPE(x)                          ((u32)(((x) & 0x00000003) << 4))
#define ADC_GET_CH2_IN_TYPE(x)                      ((u32)(((x >> 4) & 0x00000003)))
#define ADC_MASK_CH1_IN_TYPE                        ((u32)0x00000003 << 2)                   /*!< R/W   0.0
                                                                                              * This bit controls channel 1 input type. If it is set to differential type (value of this bit is 1), channel 0 should also be set to differential type since they are hardware-fixed differential pair.
                                                                                              * 0: ADC channel 1 is single-end input.
                                                                                              * 1: ADC channel 1 is differential input which is paired with channel 0.
                                                                                              */
#define ADC_CH1_IN_TYPE(x)                          ((u32)(((x) & 0x00000003) << 2))
#define ADC_GET_CH1_IN_TYPE(x)                      ((u32)(((x >> 2) & 0x00000003)))
#define ADC_MASK_CH0_IN_TYPE                        ((u32)0x00000003 << 0)                   /*!< R/W   0.0
                                                                                              * This bit controls channel 0 input type. If it is set to differential type (value of this bit is 1), channel 1 should also be set to differential type since they are hardware-fixed differential pair.
                                                                                              * 0: ADC channel 0 is single-end input.
                                                                                              * 1: ADC channel 0 is differential input which is paired with channel 1.
                                                                                              */
#define ADC_CH0_IN_TYPE(x)                          ((u32)(((x) & 0x00000003) << 0))
#define ADC_GET_CH0_IN_TYPE(x)                      ((u32)(((x >> 0) & 0x00000003)))
/** @} */


/** @defgroup comp_th_chX
 * @brief
 * @{
 */
#define ADC_MASK_CHx_TH_H                           ((u32)0x00000FFF << 16)                  /*!< R/W   0.0 This field defines the higher threshold of channel 0 for ADC automatic comparison. */
#define ADC_CHx_TH_H(x)                             ((u32)(((x) & 0x00000FFF) << 16))
#define ADC_GET_CHx_TH_H(x)                         ((u32)(((x >> 16) & 0x00000FFF)))
#define ADC_MASK_CHx_TH_L                           ((u32)0x00000FFF << 0)                   /*!< R/W   0.0 This field defines the lower threshold of channel 0 for ADC automatic comparison. */
#define ADC_CHx_TH_L(x)                             ((u32)(((x) & 0x00000FFF) << 0))
#define ADC_GET_CHx_TH_L(x)                         ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup comp_ctrl
 * @brief
 * @{
 */
#define ADC_MASK_CH8_COMP_CTRL                      ((u32)0x00000003 << 16)                  /*!< R/W   0.0
                                                                                              * This field defines ADC channel comparison criteria which would notify system by interrupt when the criterion matches. Once a criterion matches, ADC should send an interrupt signal to system if the related interrupt mask is unmask. ADC also updates comparison results in REG_ADC_COMP_STS.
                                                                                              * 0: When Vin < BIT_ADC_COMP_TH_L_CH8, comparison criterion matches.
                                                                                              * 1: When Vin > BIT_ADC_COMP_TH_H_CH8, comparison criterion matches.
                                                                                              * 2: When Vin >= BIT_ADC_COMP_TH_L_CH8 and Vin <= BIT_ADC_COMP_TH_H_CH8, comparison criterion matches.
                                                                                              * 3: When Vin < BIT_ADC_COMP_TH_L_CH8 or Vin > BIT_ADC_COMP_TH_H_CH8,comparison criterion matches.
                                                                                              */
#define ADC_CH8_COMP_CTRL(x)                        ((u32)(((x) & 0x00000003) << 16))
#define ADC_GET_CH8_COMP_CTRL(x)                    ((u32)(((x >> 16) & 0x00000003)))
#define ADC_MASK_CH7_COMP_CTRL                      ((u32)0x00000003 << 14)                  /*!< R/W   0.0
                                                                                              * This field defines ADC channel comparison criteria which would notify system by interrupt when the criterion matches. Once a criterion matches, ADC should send an interrupt signal to system if the related interrupt mask is unmask. ADC also updates comparison results in REG_ADC_COMP_STS.
                                                                                              * 0: When Vin < BIT_ADC_COMP_TH_L_CH7, comparison criterion matches.
                                                                                              * 1: When Vin > BIT_ADC_COMP_TH_H_CH7, comparison criterion matches.
                                                                                              * 2: When Vin >= BIT_ADC_COMP_TH_L_CH7 and Vin <= BIT_ADC_COMP_TH_H_CH7, comparison criterion matches.
                                                                                              * 3: When Vin < BIT_ADC_COMP_TH_L_CH7 or Vin > BIT_ADC_COMP_TH_H_CH7,comparison criterion matches.
                                                                                              */
#define ADC_CH7_COMP_CTRL(x)                        ((u32)(((x) & 0x00000003) << 14))
#define ADC_GET_CH7_COMP_CTRL(x)                    ((u32)(((x >> 14) & 0x00000003)))
#define ADC_MASK_CH6_COMP_CTRL                      ((u32)0x00000003 << 12)                  /*!< R/W   0.0
                                                                                              * This field defines ADC channel comparison criteria which would notify system by interrupt when the criterion matches. Once a criterion matches, ADC should send an interrupt signal to system if the related interrupt mask is unmask. ADC also updates comparison results in REG_ADC_COMP_STS.
                                                                                              * 0: When Vin < BIT_ADC_COMP_TH_L_CH6, comparison criterion matches.
                                                                                              * 1: When Vin > BIT_ADC_COMP_TH_H_CH6, comparison criterion matches.
                                                                                              * 2: When Vin >= BIT_ADC_COMP_TH_L_CH6 and Vin <= BIT_ADC_COMP_TH_H_CH6, comparison criterion matches.
                                                                                              * 3: When Vin < BIT_ADC_COMP_TH_L_CH6 or Vin > BIT_ADC_COMP_TH_H_CH6,comparison criterion matches.
                                                                                              */
#define ADC_CH6_COMP_CTRL(x)                        ((u32)(((x) & 0x00000003) << 12))
#define ADC_GET_CH6_COMP_CTRL(x)                    ((u32)(((x >> 12) & 0x00000003)))
#define ADC_MASK_CH5_COMP_CTRL                      ((u32)0x00000003 << 10)                  /*!< R/W   0.0
                                                                                              * This field defines ADC channel comparison criteria which would notify system by interrupt when the criterion matches. Once a criterion matches, ADC should send an interrupt signal to system if the related interrupt mask is unmask. ADC also updates comparison results in REG_ADC_COMP_STS.
                                                                                              * 0: When Vin < BIT_ADC_COMP_TH_L_CH5, comparison criterion matches.
                                                                                              * 1: When Vin > BIT_ADC_COMP_TH_H_CH5, comparison criterion matches.
                                                                                              * 2: When Vin >= BIT_ADC_COMP_TH_L_CH5 and Vin <= BIT_ADC_COMP_TH_H_CH5, comparison criterion matches.
                                                                                              * 3: When Vin < BIT_ADC_COMP_TH_L_CH5 or Vin > BIT_ADC_COMP_TH_H_CH5,comparison criterion matches.
                                                                                              */
#define ADC_CH5_COMP_CTRL(x)                        ((u32)(((x) & 0x00000003) << 10))
#define ADC_GET_CH5_COMP_CTRL(x)                    ((u32)(((x >> 10) & 0x00000003)))
#define ADC_MASK_CH4_COMP_CTRL                      ((u32)0x00000003 << 8)                   /*!< R/W   0.0
                                                                                              * This field defines ADC channel comparison criteria which would notify system by interrupt when the criterion matches. Once a criterion matches, ADC should send an interrupt signal to system if the related interrupt mask is unmask. ADC also updates comparison results in REG_ADC_COMP_STS.
                                                                                              * 0: When Vin < BIT_ADC_COMP_TH_L_CH4, comparison criterion matches.
                                                                                              * 1: When Vin > BIT_ADC_COMP_TH_H_CH4, comparison criterion matches.
                                                                                              * 2: When Vin >= BIT_ADC_COMP_TH_L_CH4 and Vin <= BIT_ADC_COMP_TH_H_CH4, comparison criterion matches.
                                                                                              * 3: When Vin < BIT_ADC_COMP_TH_L_CH4 or Vin > BIT_ADC_COMP_TH_H_CH4,comparison criterion matches.
                                                                                              */
#define ADC_CH4_COMP_CTRL(x)                        ((u32)(((x) & 0x00000003) << 8))
#define ADC_GET_CH4_COMP_CTRL(x)                    ((u32)(((x >> 8) & 0x00000003)))
#define ADC_MASK_CH3_COMP_CTRL                      ((u32)0x00000003 << 6)                   /*!< R/W   0.0
                                                                                              * This field defines ADC channel comparison criteria which would notify system by interrupt when the criterion matches. Once a criterion matches, ADC should send an interrupt signal to system if the related interrupt mask is unmask. ADC also updates comparison results in REG_ADC_COMP_STS.
                                                                                              * 0: When Vin < BIT_ADC_COMP_TH_L_CH3, comparison criterion matches.
                                                                                              * 1: When Vin > BIT_ADC_COMP_TH_H_CH3, comparison criterion matches.
                                                                                              * 2: When Vin >= BIT_ADC_COMP_TH_L_CH3 and Vin <= BIT_ADC_COMP_TH_H_CH3, comparison criterion matches.
                                                                                              * 3: When Vin < BIT_ADC_COMP_TH_L_CH3 or Vin > BIT_ADC_COMP_TH_H_CH3,comparison criterion matches.
                                                                                              */
#define ADC_CH3_COMP_CTRL(x)                        ((u32)(((x) & 0x00000003) << 6))
#define ADC_GET_CH3_COMP_CTRL(x)                    ((u32)(((x >> 6) & 0x00000003)))
#define ADC_MASK_CH2_COMP_CTRL                      ((u32)0x00000003 << 4)                   /*!< R/W   0.0
                                                                                              * This field defines ADC channel comparison criteria which would notify system by interrupt when the criterion matches. Once a criterion matches, ADC should send an interrupt signal to system if the related interrupt mask is unmask. ADC also updates comparison results in REG_ADC_COMP_STS.
                                                                                              * 0: When Vin < BIT_ADC_COMP_TH_L_CH2, comparison criterion matches.
                                                                                              * 1: When Vin > BIT_ADC_COMP_TH_H_CH2, comparison criterion matches.
                                                                                              * 2: When Vin >= BIT_ADC_COMP_TH_L_CH2 and Vin <= BIT_ADC_COMP_TH_H_CH2, comparison criterion matches.
                                                                                              * 3: When Vin < BIT_ADC_COMP_TH_L_CH2 or Vin > BIT_ADC_COMP_TH_H_CH2,comparison criterion matches.
                                                                                              */
#define ADC_CH2_COMP_CTRL(x)                        ((u32)(((x) & 0x00000003) << 4))
#define ADC_GET_CH2_COMP_CTRL(x)                    ((u32)(((x >> 4) & 0x00000003)))
#define ADC_MASK_CH1_COMP_CTRL                      ((u32)0x00000003 << 2)                   /*!< R/W   0.0
                                                                                              * This field defines ADC channel comparison criteria which would notify system by interrupt when the criterion matches. Once a criterion matches, ADC should send an interrupt signal to system if the related interrupt mask is unmask. ADC also updates comparison results in REG_ADC_COMP_STS.
                                                                                              * 0: When Vin < BIT_ADC_COMP_TH_L_CH1, comparison criterion matches.
                                                                                              * 1: When Vin > BIT_ADC_COMP_TH_H_CH1, comparison criterion matches.
                                                                                              * 2: When Vin >= BIT_ADC_COMP_TH_L_CH1 and Vin <= BIT_ADC_COMP_TH_H_CH1, comparison criterion matches.
                                                                                              * 3: When Vin < BIT_ADC_COMP_TH_L_CH1 or Vin > BIT_ADC_COMP_TH_H_CH1,comparison criterion matches.
                                                                                              */
#define ADC_CH1_COMP_CTRL(x)                        ((u32)(((x) & 0x00000003) << 2))
#define ADC_GET_CH1_COMP_CTRL(x)                    ((u32)(((x >> 2) & 0x00000003)))
#define ADC_MASK_CH0_COMP_CTRL                      ((u32)0x00000003 << 0)                   /*!< R/W   0.0
                                                                                              * This field defines ADC channel comparison criteria which would notify system by interrupt when the criterion matches. Once a criterion matches, ADC should send an interrupt signal to system if the related interrupt mask is unmask. ADC also updates comparison results in REG_ADC_COMP_STS.
                                                                                              * 0: When Vin < BIT_ADC_COMP_TH_L_CH0, comparison criterion matches.
                                                                                              * 1: When Vin > BIT_ADC_COMP_TH_H_CH0, comparison criterion matches.
                                                                                              * 2: When Vin >= BIT_ADC_COMP_TH_L_CH0 and Vin <= BIT_ADC_COMP_TH_H_CH0, comparison criterion matches.
                                                                                              * 3: When Vin < BIT_ADC_COMP_TH_L_CH0 or Vin > BIT_ADC_COMP_TH_H_CH0,comparison criterion matches.
                                                                                              */
#define ADC_CH0_COMP_CTRL(x)                        ((u32)(((x) & 0x00000003) << 0))
#define ADC_GET_CH0_COMP_CTRL(x)                    ((u32)(((x >> 0) & 0x00000003)))
/** @} */


/** @defgroup comp_sts
 * @brief
 * @{
 */
#define ADC_MASK_CH8_COMP_STS                       ((u32)0x00000003 << 16)                  /*!< R     0.0 This field reflects comparison result of channel 8 immediately. */
#define ADC_CH8_COMP_STS(x)                         ((u32)(((x) & 0x00000003) << 16))
#define ADC_GET_CH8_COMP_STS(x)                     ((u32)(((x >> 16) & 0x00000003)))
#define ADC_MASK_CH7_COMP_STS                       ((u32)0x00000003 << 14)                  /*!< R     0.0 This field reflects comparison result of channel 7 immediately. */
#define ADC_CH7_COMP_STS(x)                         ((u32)(((x) & 0x00000003) << 14))
#define ADC_GET_CH7_COMP_STS(x)                     ((u32)(((x >> 14) & 0x00000003)))
#define ADC_MASK_CH6_COMP_STS                       ((u32)0x00000003 << 12)                  /*!< R     0.0 This field reflects comparison result of channel 6 immediately. */
#define ADC_CH6_COMP_STS(x)                         ((u32)(((x) & 0x00000003) << 12))
#define ADC_GET_CH6_COMP_STS(x)                     ((u32)(((x >> 12) & 0x00000003)))
#define ADC_MASK_CH5_COMP_STS                       ((u32)0x00000003 << 10)                  /*!< R     0.0 This field reflects comparison result of channel 5 immediately. */
#define ADC_CH5_COMP_STS(x)                         ((u32)(((x) & 0x00000003) << 10))
#define ADC_GET_CH5_COMP_STS(x)                     ((u32)(((x >> 10) & 0x00000003)))
#define ADC_MASK_CH4_COMP_STS                       ((u32)0x00000003 << 8)                   /*!< R     0.0 This field reflects comparison result of channel 4 immediately. */
#define ADC_CH4_COMP_STS(x)                         ((u32)(((x) & 0x00000003) << 8))
#define ADC_GET_CH4_COMP_STS(x)                     ((u32)(((x >> 8) & 0x00000003)))
#define ADC_MASK_CH3_COMP_STS                       ((u32)0x00000003 << 6)                   /*!< R     0.0 This field reflects comparison result of channel 3 immediately. */
#define ADC_CH3_COMP_STS(x)                         ((u32)(((x) & 0x00000003) << 6))
#define ADC_GET_CH3_COMP_STS(x)                     ((u32)(((x >> 6) & 0x00000003)))
#define ADC_MASK_CH2_COMP_STS                       ((u32)0x00000003 << 4)                   /*!< R     0.0 This field reflects comparison result of channel 2 immediately. */
#define ADC_CH2_COMP_STS(x)                         ((u32)(((x) & 0x00000003) << 4))
#define ADC_GET_CH2_COMP_STS(x)                     ((u32)(((x >> 4) & 0x00000003)))
#define ADC_MASK_CH1_COMP_STS                       ((u32)0x00000003 << 2)                   /*!< R     0.0 This field reflects comparison result of channel 1 immediately. */
#define ADC_CH1_COMP_STS(x)                         ((u32)(((x) & 0x00000003) << 2))
#define ADC_GET_CH1_COMP_STS(x)                     ((u32)(((x >> 2) & 0x00000003)))
#define ADC_MASK_CH0_COMP_STS                       ((u32)0x00000003 << 0)                   /*!< R     0.0 This field reflects comparison result of channel 0 immediately. */
#define ADC_CH0_COMP_STS(x)                         ((u32)(((x) & 0x00000003) << 0))
#define ADC_GET_CH0_COMP_STS(x)                     ((u32)(((x >> 0) & 0x00000003)))
/** @} */


/** @defgroup chsw_list_0
 * @brief
 * @{
 */
#define ADC_MASK_CHSW_7                             ((u32)0x0000000F << 28)                  /*!< R/W   0.0 This field defines the 8th channel to do a conversion when a conversion event takes place. */
#define ADC_CHSW_7(x)                               ((u32)(((x) & 0x0000000F) << 28))
#define ADC_GET_CHSW_7(x)                           ((u32)(((x >> 28) & 0x0000000F)))
#define ADC_MASK_CHSW_6                             ((u32)0x0000000F << 24)                  /*!< R/W   0.0 This field defines the 7th channel to do a conversion when a conversion event takes place. */
#define ADC_CHSW_6(x)                               ((u32)(((x) & 0x0000000F) << 24))
#define ADC_GET_CHSW_6(x)                           ((u32)(((x >> 24) & 0x0000000F)))
#define ADC_MASK_CHSW_5                             ((u32)0x0000000F << 20)                  /*!< R/W   0.0 This field defines the 6th channel to do a conversion when a conversion event takes place. */
#define ADC_CHSW_5(x)                               ((u32)(((x) & 0x0000000F) << 20))
#define ADC_GET_CHSW_5(x)                           ((u32)(((x >> 20) & 0x0000000F)))
#define ADC_MASK_CHSW_4                             ((u32)0x0000000F << 16)                  /*!< R/W   0.0 This field defines the 5th channel to do a conversion when a conversion event takes place. */
#define ADC_CHSW_4(x)                               ((u32)(((x) & 0x0000000F) << 16))
#define ADC_GET_CHSW_4(x)                           ((u32)(((x >> 16) & 0x0000000F)))
#define ADC_MASK_CHSW_3                             ((u32)0x0000000F << 12)                  /*!< R/W   0.0 This field defines the 4th channel to do a conversion when a conversion event takes place. */
#define ADC_CHSW_3(x)                               ((u32)(((x) & 0x0000000F) << 12))
#define ADC_GET_CHSW_3(x)                           ((u32)(((x >> 12) & 0x0000000F)))
#define ADC_MASK_CHSW_2                             ((u32)0x0000000F << 8)                   /*!< R/W   0.0 This field defines the 3rd channel to do a conversion when a conversion event takes place. */
#define ADC_CHSW_2(x)                               ((u32)(((x) & 0x0000000F) << 8))
#define ADC_GET_CHSW_2(x)                           ((u32)(((x >> 8) & 0x0000000F)))
#define ADC_MASK_CHSW_1                             ((u32)0x0000000F << 4)                   /*!< R/W   0.0 This field defines the 2nd channel to do a conversion when a conversion event takes place. */
#define ADC_CHSW_1(x)                               ((u32)(((x) & 0x0000000F) << 4))
#define ADC_GET_CHSW_1(x)                           ((u32)(((x >> 4) & 0x0000000F)))
#define ADC_MASK_CHSW_0                             ((u32)0x0000000F << 0)                   /*!< R/W   0.0 This field defines the 1st channel to do a conversion when a conversion event takes place. */
#define ADC_CHSW_0(x)                               ((u32)(((x) & 0x0000000F) << 0))
#define ADC_GET_CHSW_0(x)                           ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup chsw_list_1
 * @brief
 * @{
 */
#define ADC_MASK_CHSW_15                            ((u32)0x0000000F << 28)                  /*!< R/W   0.0 This field defines the 16th channel to do a conversion when a conversion event takes place. */
#define ADC_CHSW_15(x)                              ((u32)(((x) & 0x0000000F) << 28))
#define ADC_GET_CHSW_15(x)                          ((u32)(((x >> 28) & 0x0000000F)))
#define ADC_MASK_CHSW_14                            ((u32)0x0000000F << 24)                  /*!< R/W   0.0 This field defines the 15th channel to do a conversion when a conversion event takes place. */
#define ADC_CHSW_14(x)                              ((u32)(((x) & 0x0000000F) << 24))
#define ADC_GET_CHSW_14(x)                          ((u32)(((x >> 24) & 0x0000000F)))
#define ADC_MASK_CHSW_13                            ((u32)0x0000000F << 20)                  /*!< R/W   0.0 This field defines the 14th channel to do a conversion when a conversion event takes place. */
#define ADC_CHSW_13(x)                              ((u32)(((x) & 0x0000000F) << 20))
#define ADC_GET_CHSW_13(x)                          ((u32)(((x >> 20) & 0x0000000F)))
#define ADC_MASK_CHSW_12                            ((u32)0x0000000F << 16)                  /*!< R/W   0.0 This field defines the 13th channel to do a conversion when a conversion event takes place. */
#define ADC_CHSW_12(x)                              ((u32)(((x) & 0x0000000F) << 16))
#define ADC_GET_CHSW_12(x)                          ((u32)(((x >> 16) & 0x0000000F)))
#define ADC_MASK_CHSW_11                            ((u32)0x0000000F << 12)                  /*!< R/W   0.0 This field defines the 12th channel to do a conversion when a conversion event takes place. */
#define ADC_CHSW_11(x)                              ((u32)(((x) & 0x0000000F) << 12))
#define ADC_GET_CHSW_11(x)                          ((u32)(((x >> 12) & 0x0000000F)))
#define ADC_MASK_CHSW_10                            ((u32)0x0000000F << 8)                   /*!< R/W   0.0 This field defines the 11th channel to do a conversion when a conversion event takes place. */
#define ADC_CHSW_10(x)                              ((u32)(((x) & 0x0000000F) << 8))
#define ADC_GET_CHSW_10(x)                          ((u32)(((x >> 8) & 0x0000000F)))
#define ADC_MASK_CHSW_9                             ((u32)0x0000000F << 4)                   /*!< R/W   0.0 This field defines the 10th channel to do a conversion when a conversion event takes place. */
#define ADC_CHSW_9(x)                               ((u32)(((x) & 0x0000000F) << 4))
#define ADC_GET_CHSW_9(x)                           ((u32)(((x >> 4) & 0x0000000F)))
#define ADC_MASK_CHSW_8                             ((u32)0x0000000F << 0)                   /*!< R/W   0.0 This field defines the 9th channel to do a conversion when a conversion event takes place. */
#define ADC_CHSW_8(x)                               ((u32)(((x) & 0x0000000F) << 0))
#define ADC_GET_CHSW_8(x)                           ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup rst_list
 * @brief
 * @{
 */
#define ADC_BIT_RST_LIST                            ((u32)0x00000001 << 0)                   /*!< R/W   0.0
                                                                                              * This bit controls to reset the channel switch to default state or not. Once this bit is set, the auto/manual channel switch goes to the first channel as a default state.
                                                                                              * 0: Do not reset conversion list to default state.
                                                                                              * 1: Reset conversion list to default state.
                                                                                              */
/** @} */


/** @defgroup auto_chsw_ctrl
 * @brief
 * @{
 */
#define ADC_BIT_AUTO_CHSW_EN                        ((u32)0x00000001 << 0)                   /*!< R/W   0.0
                                                                                              * This bit controls the automatic channel swtich enabled or disabled.
                                                                                              * 0: Disable the automatic channel switch. If an automatic channel switch is in progess, writing 0 will terminate the automatic channel switch.
                                                                                              * 1: Enable the automatic channel switch. When setting this bit, an automatic channel switch starts from the first channel in the channel switch list.
                                                                                              */
/** @} */


/** @defgroup sw_trig
 * @brief
 * @{
 */
#define ADC_BIT_SW_TRIG                        		((u32)0x00000001 << 0)                   /*!< R/WS  0.0
                                                                                              * This bit controls the ADC module to do a conversion.
                                                                                              * Every time this bit is set to 1, ADC module would switch to a new channel and do one conversion. Therefore, this bit could be used as a start-convert event which is controlled by software. 
                                                                                              * SW to write this bit to 1. HW clears this bit to 0 when the operation is done (busy=0).
                                                                                              * 0: Disable the analog module and analog mux.
                                                                                              * 1: Enable the analog module and analog mux. And then start a new channel conversion.
                                                                                              */
/** @} */


/** @defgroup last_item
 * @brief
 * @{
 */
#define ADC_MASK_LAST_ITEM                          ((u32)0x0000000F << 0)                   /*!< R     0.0 This field reflects the index of the conversion list, of which the last successful conversion is done. */
#define ADC_LAST_ITEM(x)                            ((u32)(((x) & 0x0000000F) << 0))
#define ADC_GET_LAST_ITEM(x)                        ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup sts
 * @brief
 * @{
 */
#define ADC_BIT_STS_EMPTY                           ((u32)0x00000001 << 2)                   /*!< R     0.0
                                                                                              * This bit directly reflects the FIFO empty status.
                                                                                              * 0: FIFO is empty.
                                                                                              * 1: FIFO is NOT empty.
                                                                                              */
#define ADC_BIT_STS_FULL                            ((u32)0x00000001 << 1)                   /*!< R     0.0
                                                                                              * This bit directly reflects the FIFO full status.
                                                                                              * 0: FIFO is NOT full.
                                                                                              * 1: FIFO is full.
                                                                                              */
#define ADC_BIT_STS_BUSY                            ((u32)0x00000001 << 0)                   /*!< R     0.0
                                                                                              * This bit reflects the ADC is busy or not. If the ADC is processing a conversion of a channel, this bit remains 1 which indicates it's busy. Once a conversion is done, this bit becomes 0 which indicates its' ready to do another conversion.
                                                                                              * 0: The ADC is ready.
                                                                                              * 1: The ADC is busy.
                                                                                              */
/** @} */


/** @defgroup intr_ctrl
 * @brief
 * @{
 */
#define ADC_BIT_IT_COMP_CH8_EN                      ((u32)0x00000001 << 16)                  /*!< R/W   0.0
                                                                                              * This bit controls the interrupt is enabled or not when channel 8 comparison criterion matches.
                                                                                              * 0: This interrupt is disabled.
                                                                                              * 1: This interrupt is enabled.
                                                                                              */
#define ADC_BIT_IT_COMP_CH7_EN                      ((u32)0x00000001 << 15)                  /*!< R/W   0.0
                                                                                              * This bit controls the interrupt is enabled or not when channel 7 comparison criterion matches.
                                                                                              * 0: This interrupt is disabled.
                                                                                              * 1: This interrupt is enabled.
                                                                                              */
#define ADC_BIT_IT_COMP_CH6_EN                      ((u32)0x00000001 << 14)                  /*!< R/W   0.0
                                                                                              * This bit controls the interrupt is enabled or not when channel 6 comparison criterion matches.
                                                                                              * 0: This interrupt is disabled.
                                                                                              * 1: This interrupt is enabled.
                                                                                              */
#define ADC_BIT_IT_COMP_CH5_EN                      ((u32)0x00000001 << 13)                  /*!< R/W   0.0
                                                                                              * This bit controls the interrupt is enabled or not when channel 5 comparison criterion matches.
                                                                                              * 0: This interrupt is disabled.
                                                                                              * 1: This interrupt is enabled.
                                                                                              */
#define ADC_BIT_IT_COMP_CH4_EN                      ((u32)0x00000001 << 12)                  /*!< R/W   0.0
                                                                                              * This bit controls the interrupt is enabled or not when channel 4 comparison criterion matches.
                                                                                              * 0: This interrupt is disabled.
                                                                                              * 1: This interrupt is enabled.
                                                                                              */
#define ADC_BIT_IT_COMP_CH3_EN                      ((u32)0x00000001 << 11)                  /*!< R/W   0.0
                                                                                              * This bit controls the interrupt is enabled or not when channel 3 comparison criterion matches.
                                                                                              * 0: This interrupt is disabled.
                                                                                              * 1: This interrupt is enabled.
                                                                                              */
#define ADC_BIT_IT_COMP_CH2_EN                      ((u32)0x00000001 << 10)                  /*!< R/W   0.0
                                                                                              * This bit controls the interrupt is enabled or not when channel 2 comparison criterion matches.
                                                                                              * 0: This interrupt is disabled.
                                                                                              * 1: This interrupt is enabled.
                                                                                              */
#define ADC_BIT_IT_COMP_CH1_EN                      ((u32)0x00000001 << 9)                   /*!< R/W   0.0
                                                                                              * This bit controls the interrupt is enabled or not when channel 1 comparison criterion matches.
                                                                                              * 0: This interrupt is disabled.
                                                                                              * 1: This interrupt is enabled.
                                                                                              */
#define ADC_BIT_IT_COMP_CH0_EN                      ((u32)0x00000001 << 8)                   /*!< R/W   0.0
                                                                                              * This bit controls the interrupt is enabled or not when channel 0 comparison criterion matches.
                                                                                              * 0: This interrupt is disabled.
                                                                                              * 1: This interrupt is enabled.
                                                                                              */
#define ADC_BIT_IT_ERR_EN                           ((u32)0x00000001 << 7)                   /*!< R/W   0.0
                                                                                              * This bit controls the interrupt is enabled or not when an error state takes place.
                                                                                              * 0: This interrupt is disabled.
                                                                                              * 1: This interrupt is enabled.
                                                                                              */
#define ADC_BIT_IT_DAT_OVW_EN                       ((u32)0x00000001 << 6)                   /*!< R/W   0.0
                                                                                              * This bit controls the interrupt is enabled or not when a data overwritten situation takes place.
                                                                                              * 0: This interrupt is disabled.
                                                                                              * 1: This interrupt is enabled.
                                                                                              */
#define ADC_BIT_IT_FIFO_EMPTY_EN                    ((u32)0x00000001 << 5)                   /*!< R/W   0.0
                                                                                              * This bit controls the interrupt is enabled or not when a FIFO empty state takes place.
                                                                                              * 0: This interrupt is disabled.
                                                                                              * 1: This interrupt is enabled.
                                                                                              */
#define ADC_BIT_IT_FIFO_OVER_EN                     ((u32)0x00000001 << 4)                   /*!< R/W   0.0
                                                                                              * This bit controls the interrupt is enabled or not when a FIFO overflow state takes place.
                                                                                              * 0: This interrupt is disabled.
                                                                                              * 1: This interrupt is enabled.
                                                                                              */
#define ADC_BIT_IT_FIFO_FULL_EN                     ((u32)0x00000001 << 3)                   /*!< R/W   0.0
                                                                                              * This bit controls the interrupt is enabled or not when a FIFO full state takes place.
                                                                                              * 0: This interrupt is disabled.
                                                                                              * 1: This interrupt is enabled.
                                                                                              */
#define ADC_BIT_IT_CHCV_END_EN                      ((u32)0x00000001 << 2)                   /*!< R/W   0.0
                                                                                              * This bit controls the interrupt is enbled or not when a particular channel conversion is done.
                                                                                              * Please refer to REG_ADC_IT_CHNO_CON
                                                                                              * 0: This interrupt is disabled.
                                                                                              * 1: This interrupt is enabled.
                                                                                              */
#define ADC_BIT_IT_CV_END_EN                        ((u32)0x00000001 << 1)                   /*!< R/W   0.0
                                                                                              * This bit controls the interrupt is enbled or not every time a conversion is done.
                                                                                              * No matter ADC module is in what kind of operation mode. Every time a conversion is executed, ADC module would notify system if this bit is set.
                                                                                              * 0: This interrupt is disabled.
                                                                                              * 1: This interrupt is enabled.
                                                                                              */
#define ADC_BIT_IT_CVLIST_END_EN                    ((u32)0x00000001 << 0)                   /*!< R/W   0.0
                                                                                              * This bit controls the interrupt is enbled or not when a conversion of the last channel in the list is done.
                                                                                              * For example, in automatic mode conversions would be executed coninuously. Every time the last channel conversion is done, which means all channel conversions in the list is done, ADC could notify system if this bit is set.
                                                                                              * 0: This interrupt is disabled.
                                                                                              * 1: This interrupt is enabled.
                                                                                              */
/** @} */


/** @defgroup intr_raw_sts
 * @brief
 * @{
 */
#define ADC_BIT_IT_COMP_CH8_RAW_STS                 ((u32)0x00000001 << 16)                  /*!< R     0.0 This bit is the raw status of channel 8 comparison interrupt. */
#define ADC_BIT_IT_COMP_CH7_RAW_STS                 ((u32)0x00000001 << 15)                  /*!< R     0.0 This bit is the raw status of channel 7 comparison interrupt. */
#define ADC_BIT_IT_COMP_CH6_RAW_STS                 ((u32)0x00000001 << 14)                  /*!< R     0.0 This bit is the raw status of channel 6 comparison interrupt. */
#define ADC_BIT_IT_COMP_CH5_RAW_STS                 ((u32)0x00000001 << 13)                  /*!< R     0.0 This bit is the raw status of channel 5 comparison interrupt. */
#define ADC_BIT_IT_COMP_CH4_RAW_STS                 ((u32)0x00000001 << 12)                  /*!< R     0.0 This bit is the raw status of channel 4 comparison interrupt. */
#define ADC_BIT_IT_COMP_CH3_RAW_STS                 ((u32)0x00000001 << 11)                  /*!< R     0.0 This bit is the raw status of channel 3 comparison interrupt. */
#define ADC_BIT_IT_COMP_CH2_RAW_STS                 ((u32)0x00000001 << 10)                  /*!< R     0.0 This bit is the raw status of channel 2 comparison interrupt. */
#define ADC_BIT_IT_COMP_CH1_RAW_STS                 ((u32)0x00000001 << 9)                   /*!< R     0.0 This bit is the raw status of channel 1 comparison interrupt. */
#define ADC_BIT_IT_COMP_CH0_RAW_STS                 ((u32)0x00000001 << 8)                   /*!< R     0.0 This bit is the raw status of channel 0 comparison interrupt. */
#define ADC_BIT_IT_ERR_RAW_STS                      ((u32)0x00000001 << 7)                   /*!< R     0.0 This bit is the raw status of error interrupt. */
#define ADC_BIT_IT_DAT_OVW_RAW_STS                  ((u32)0x00000001 << 6)                   /*!< R     0.0 This bit is the raw status of global data register overwritten interrupt. */
#define ADC_BIT_IT_FIFO_EMPTY_RAW_STS               ((u32)0x00000001 << 5)                   /*!< R     0.0 This bit is the raw status of FIFO empty interrupt. */
#define ADC_BIT_IT_FIFO_OVER_RAW_STS                ((u32)0x00000001 << 4)                   /*!< R     0.0 This bit is the raw status of FIFO overflow interrupt. */
#define ADC_BIT_IT_FIFO_FULL_RAW_STS                ((u32)0x00000001 << 3)                   /*!< R     0.0 This bit is the raw status of FIFO full interrupt. */
#define ADC_BIT_IT_CHCV_END_RAW_STS                 ((u32)0x00000001 << 2)                   /*!< R     0.0 This bit is the raw status of particular channel conversion end interrupt. */
#define ADC_BIT_IT_CV_END_RAW_STS                   ((u32)0x00000001 << 1)                   /*!< R     0.0 This bit is the raw status of each conversion end interrupt. */
#define ADC_BIT_IT_CVLIST_END_RAW_STS               ((u32)0x00000001 << 0)                   /*!< R     0.0 This bit is the raw status of channel conversion list end interrupt. */
/** @} */


/** @defgroup intr_sts
 * @brief
 * @{
 */
#define ADC_BIT_IT_COMP_CH8_STS                     ((u32)0x00000001 << 16)                  /*!< R/W1C 0.0 This bit is the status of channel 8 comparison interrupt. */
#define ADC_BIT_IT_COMP_CH7_STS                     ((u32)0x00000001 << 15)                  /*!< R/W1C 0.0 This bit is the status of channel 7 comparison interrupt. */
#define ADC_BIT_IT_COMP_CH6_STS                     ((u32)0x00000001 << 14)                  /*!< R/W1C 0.0 This bit is the status of channel 6 comparison interrupt. */
#define ADC_BIT_IT_COMP_CH5_STS                     ((u32)0x00000001 << 13)                  /*!< R/W1C 0.0 This bit is the status of channel 5 comparison interrupt. */
#define ADC_BIT_IT_COMP_CH4_STS                     ((u32)0x00000001 << 12)                  /*!< R/W1C 0.0 This bit is the status of channel 4 comparison interrupt. */
#define ADC_BIT_IT_COMP_CH3_STS                     ((u32)0x00000001 << 11)                  /*!< R/W1C 0.0 This bit is the status of channel 3 comparison interrupt. */
#define ADC_BIT_IT_COMP_CH2_STS                     ((u32)0x00000001 << 10)                  /*!< R/W1C 0.0 This bit is the status of channel 2 comparison interrupt. */
#define ADC_BIT_IT_COMP_CH1_STS                     ((u32)0x00000001 << 9)                   /*!< R/W1C 0.0 This bit is the status of channel 1 comparison interrupt. */
#define ADC_BIT_IT_COMP_CH0_STS                     ((u32)0x00000001 << 8)                   /*!< R/W1C 0.0 This bit is the status of channel 0 comparison interrupt. */
#define ADC_BIT_IT_ERR_STS                          ((u32)0x00000001 << 7)                   /*!< R/W1C 0.0 This bit is the status of error interrupt. */
#define ADC_BIT_IT_DAT_OVW_STS                      ((u32)0x00000001 << 6)                   /*!< R/W1C 0.0 This bit is the status of global data register overwritten interrupt. */
#define ADC_BIT_IT_FIFO_EMPTY_STS                   ((u32)0x00000001 << 5)                   /*!< R/W1C 0.0 This bit is the status of FIFO empty interrupt. */
#define ADC_BIT_IT_FIFO_OVER_STS                    ((u32)0x00000001 << 4)                   /*!< R/W1C 0.0 This bit is the status of FIFO overflow interrupt. */
#define ADC_BIT_IT_FIFO_FULL_STS                    ((u32)0x00000001 << 3)                   /*!< R/W1C 0.0 This bit is the status of FIFO full interrupt. */
#define ADC_BIT_IT_CHCV_END_STS                     ((u32)0x00000001 << 2)                   /*!< R/W1C 0.0 This bit is the status of particular channel conversion end interrupt. */
#define ADC_BIT_IT_CV_END_STS                       ((u32)0x00000001 << 1)                   /*!< R/W1C 0.0 This bit is the status of each conversion end interrupt. */
#define ADC_BIT_IT_CVLIST_END_STS                   ((u32)0x00000001 << 0)                   /*!< R/W1C 0.0 This bit is the status of channel conversion list end interrupt. */
/** @} */


/** @defgroup it_chno_con
 * @brief
 * @{
 */
#define ADC_MASK_IT_CHNO_CON                        ((u32)0x00000007 << 0)                   /*!< R/W   0.0
                                                                                              * This field defines that ADC module should send interrupt signal to system when a conversion which of channel number is the same as this field. This register is only valid when BIT_ADC_IT_CHCV_END_EN is set.
                                                                                              * BIT_ADC_IT_CHNO_CON: Channel number.
                                                                                              */
#define ADC_IT_CHNO_CON(x)                          ((u32)(((x) & 0x00000007) << 0))
#define ADC_GET_IT_CHNO_CON(x)                      ((u32)(((x >> 0) & 0x00000007)))
/** @} */


/** @defgroup full_lvl
 * @brief
 * @{
 */
#define ADC_MASK_FULL_LVL                           ((u32)0x0000003F << 0)                   /*!< R/W   0.0
                                                                                              * This field defines the FIFO full entry threshold level and it is used for FIFO full interrupt. It should be set according to ADC FIFO depth.
                                                                                              * A value of 0 sets the threshold for 1 entry, and a value of 15 sets the threshold for
                                                                                              * 16 entries.
                                                                                              */
#define ADC_FULL_LVL(x)                             ((u32)(((x) & 0x0000003F) << 0))
#define ADC_GET_FULL_LVL(x)                         ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup trig_timer_sel
 * @brief
 * @{
 */
#define ADC_MASK_TRIG_TIMER_SEL                     ((u32)0x00000007 << 0)                   /*!< R/W   0.0
                                                                                              * This three bits defines which timer channel would be used to make ADC module do a conversion.
                                                                                              * BIT_ADC_EXT_WK_TIMER_SEL is 
                                                                                              * 0: Timer module 0 is used as the comparator external trigger source.
                                                                                              * 1: Timer module 1 is used as the comparator external trigger source.
                                                                                              *      .
                                                                                              *      .
                                                                                              *      .
                                                                                              * 7: Timer module 7 is used as the comparator external trigger source.
                                                                                              */
#define ADC_TRIG_TIMER_SEL(x)                       ((u32)(((x) & 0x00000007) << 0))
#define ADC_GET_TRIG_TIMER_SEL(x)                   ((u32)(((x >> 0) & 0x00000007)))
/** @} */


/** @defgroup dat_global
 * @brief
 * @{
 */
#define ADC_MASK_DAT_GLOBAL_CH                      ((u32)0x00000007 << 23)                  /*!< R     0.0 This field indicates which channel data is in BIT_ADC_DATA_GLOBAL */
#define ADC_DAT_GLOBAL_CH(x)                        ((u32)(((x) & 0x00000007) << 23))
#define ADC_GET_DAT_GLOBAL_CH(x)                    ((u32)(((x >> 23) & 0x00000007)))
#define ADC_BIT_DAT_GLOBAL_RDY                      ((u32)0x00000001 << 22)                  /*!< R     0.0 This bit indicates that a conversion is done. Every time a conversion is done, this bit should be set to 1 and it would be cleared to 0 when a read operation of REG_ADC_DATA_GLOBAL. */
#define ADC_BIT_DAT_GLOBAL_OVW                      ((u32)0x00000001 << 21)                  /*!< R     0.0
                                                                                              * This bit indicates that there is a data overwritten situation in BIT_ADC_DATA_GLOBAL takes place. A data overwritten situation is that a former conversion data is NOT read before a new conversion is written into data field.
                                                                                              * 0: There is no data overwritten case.
                                                                                              * 1: There is a data overwritten case.
                                                                                              */
#define ADC_MASK_DAT_GLOBAL_DAT                     ((u32)0x00000FFF << 0)                   /*!< R     0.0 This field contains the newest conversion data of a conversion channel in the list. */
#define ADC_DAT_GLOBAL_DAT(x)                       ((u32)(((x) & 0x00000FFF) << 0))
#define ADC_GET_DAT_GLOBAL_DAT(x)                   ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup dat_chX
 * @brief
 * @{
 */
#define ADC_BIT_DAT_CHx_RDY                         ((u32)0x00000001 << 22)                  /*!< R     0.0 This bit indicates that a conversion is done. Every time a conversion is done, this bit should be set to 1 and it would be cleared to 0 when a read operation of dat_ch0. */
#define ADC_BIT_DAT_CHx_OVW                         ((u32)0x00000001 << 21)                  /*!< R     0.0
                                                                                              * This bit indicates that there is a data overwritten situation in dat_ch0 takes place. A data overwritten situation is that a former conversion data is NOT read before a new conversion is written into data field.
                                                                                              * 0: There is no data overwritten case.
                                                                                              * 1: There is a data overwritten case.
                                                                                              */
#define ADC_MASK_DAT_CHx_DAT                        ((u32)0x00000FFF << 0)                   /*!< R     0.0 This field contains the newest conversion data of channel 0. */
#define ADC_DAT_CHx_DAT(x)                          ((u32)(((x) & 0x00000FFF) << 0))
#define ADC_GET_DAT_CHx_DAT(x)                      ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup dma_con
 * @brief
 * @{
 */
#define ADC_MASK_DMA_CON_LVL                        ((u32)0x0000003F << 8)                   /*!< R/W   0.0
                                                                                              * This field defines the FIFO threshold level that ADC module should make a DMA request to DMA module.
                                                                                              * A value of 0 sets the threshold for 1 entry, and a value of 15 sets the threshold for
                                                                                              * 16 entries.
                                                                                              */
#define ADC_DMA_CON_LVL(x)                          ((u32)(((x) & 0x0000003F) << 8))
#define ADC_GET_DMA_CON_LVL(x)                      ((u32)(((x >> 8) & 0x0000003F)))
#define ADC_BIT_DMA_CON_EN                          ((u32)0x00000001 << 0)                   /*!< R/W   0.0
                                                                                              * This bit controls that DMA function of ADC is enabled or not.
                                                                                              * 0: ADC DMA is disabled.
                                                                                              * 1: ADC DMA is enabled.
                                                                                              */
/** @} */


/** @defgroup flr
 * @brief
 * @{
 */
#define ADC_MASK_FLR                                ((u32)0x0000007F << 0)                   /*!< R/W   0.0 This field records the current ADC FIFO entry number. */
#define ADC_FLR(x)                                  ((u32)(((x) & 0x0000007F) << 0))
#define ADC_GET_FLR(x)                              ((u32)(((x >> 0) & 0x0000007F)))
/** @} */


/** @defgroup clr_fifo
 * @brief
 * @{
 */
#define ADC_BIT_CLR_FIFO                            ((u32)0x00000001 << 0)                   /*!< R     0.0
                                                                                              * This bit is used to clear FIFO contents and makes it to the default status.
                                                                                              * Every time this bit is set to 1, FIFO would be cleared to the default status. Software should set this bit to 0 after FIFO is cleared.
                                                                                              * 0: No effect.
                                                                                              * 1: Clear FIFO.
                                                                                              */
/** @} */


/** @defgroup clk_div
 * @brief
 * @{
 */
#define ADC_MASK_CLK_DIV                       		((u32)0x0000FFFF << 0)                   /*!< R/W   0.0 This field defines clock divider level of ADC module. This clock is the SCLK input to the ADC. */
#define ADC_CLK_DIV(x)                         		((u32)(((x) & 0x0000FFFF) << 0))
#define ADC_GET_CLK_DIV(x)                     		((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup dly_cnt
 * @brief
 * @{
 */
#define ADC_MASK_SAMPLE_CLK_PHASE_SEL               ((u32)0x00000003 << 8)                   /*!< R/W   0.0
                                                                                              * This field defines the ADC sample clock phase selection.
                                                                                              * 00, 10: 90 degrees
                                                                                              * 01: 0 degrees
                                                                                              * 11: 180 degrees
                                                                                              */
#define ADC_SAMPLE_CLK_PHASE_SEL(x)                 ((u32)(((x) & 0x00000003) << 8))
#define ADC_GET_SAMPLE_CLK_PHASE_SEL(x)             ((u32)(((x >> 8) & 0x00000003)))
#define ADC_MASK_DATA_DELAY                         ((u32)0x00000003 << 2)                   /*!< R/W   0.0
                                                                                              * This field define the ADC data capture delay selection.
                                                                                              * 00 : Data output from channel is delayed by 0 ADC clock cycle
                                                                                              * 01: Data output from channel is delayed by 1 ADC clock cycles
                                                                                              * 10: Data output from channel is delayed by 2 ADC clock cycles
                                                                                              * 11: Data output from channel is delayed by 3 ADC clock cycles
                                                                                              */
#define ADC_DATA_DELAY(x)                           ((u32)(((x) & 0x00000003) << 2))
#define ADC_GET_DATA_DELAY(x)                       ((u32)(((x >> 2) & 0x00000003)))
/** @} */


/** @defgroup dbg_sel
 * @brief
 * @{
 */
#define ADC_MASK_ADC_DBG_SEL                        ((u32)0x00000003 << 2)                   /*!< R/W   0.0 This field defines the signal select when ADC is in Debug Mode, and only used for debugging by the Designer during chip bring-up. */
#define ADC_ADC_DBG_SEL(x)                          ((u32)(((x) & 0x00000003) << 2))
#define ADC_GET_ADC_DBG_SEL(x)                      ((u32)(((x >> 2) & 0x00000003)))
#define ADC_BIT_ADC_DBG_EN                          ((u32)0x00000001 << 1)                   /*!< R/W   0.0
                                                                                              * This field defines if the ADC is set the ADC operation mode - Normal Mode or Debug Mode.
                                                                                              * 0: ADC is not in Debug Mode. (Normal operation) 
                                                                                              * 1: ADC is in Debug Mode.
                                                                                              */
/** @} */


/** @defgroup RSVD
 * @brief
 * @{
 */
/** @} */


/** @defgroup RSVD
 * @brief
 * @{
 */
/** @} */


/** @defgroup RSVD
 * @brief
 * @{
 */
/** @} */


/** @defgroup RSVD
 * @brief
 * @{
 */
/** @} */


/** @defgroup RSVD
 * @brief
 * @{
 */
/** @} */


/** @defgroup vref_settle_time
 * @brief
 * @{
 */
#define ADC_MASK_VREF_SETTLE_TIME              ((u32)0x0000000F << 0)                   	/*!< R/W   0.0
                                                                                              * This field defines the time required for the reference voltage to settle to ensure stability of ADC operations. The value is calculated based on divider clock, which define by clk_div (0x9C[15:0]). The reference period is 20us, 40us, 80us, 160us, �
                                                                                              * For example:
                                                                                              * . If clk_div = 16'd400  then divider clock = 100KHz.
                                                                                              * . To have 40us delay, need to set vref_settle_time = 4'h4.
                                                                                              */
#define ADC_VREF_SETTLE_TIME(x)                ((u32)(((x) & 0x0000000F) << 0))
#define ADC_GET_VREF_SETTLE_TIME(x)            ((u32)(((x >> 0) & 0x0000000F)))
/** @} */




/** @defgroup ADC_Exported_Types ADC Exported Types
* @{
*/
/**
* @brief ADC Register Declaration
*/

typedef struct {
	__IO uint32_t CONF;                                         /*!< Register,  Address offset:0x0000 */
	__IO uint32_t IN_TYPE;                                      /*!< Register,  Address offset:0x0004 */
	__IO uint32_t COMP_TH_CHx[9];                               /*!< Register,  Address offset:0x0008-0x0028 */
	__IO uint32_t COMP_CTRL;                                    /*!< Register,  Address offset:0x002C */
	__I  uint32_t COMP_STS;                                     /*!< Register,  Address offset:0x0030 */
	__IO uint32_t CHSW_LIST_0;                                  /*!< Register,  Address offset:0x0034 */
	__IO uint32_t CHSW_LIST_1;                                  /*!< Register,  Address offset:0x0038 */
	__IO uint32_t RST_LIST;                                     /*!< Register,  Address offset:0x003C */
	__IO uint32_t AUTO_CHSW_CTRL;                               /*!< Register,  Address offset:0x0040 */
	__IO uint32_t SW_TRIG;                                      /*!< Register,  Address offset:0x0044 */
	__I  uint32_t LAST_ITEM;                                    /*!< Register,  Address offset:0x0048 */
	__I  uint32_t STS;                                          /*!< Register,  Address offset:0x004C */
	__IO uint32_t INTR_CTRL;                                    /*!< Register,  Address offset:0x0050 */
	__I  uint32_t INTR_RAW_STS;                                 /*!< Register,  Address offset:0x0054 */
	__IO uint32_t INTR_STS;                                     /*!< Register,  Address offset:0x0058 */
	__IO uint32_t IT_CHNO_CON;                                  /*!< Register,  Address offset:0x005C */
	__IO uint32_t FULL_LVL;                                     /*!< Register,  Address offset:0x0060 */
	__IO uint32_t TRIG_TIMER_SEL;                               /*!< Register,  Address offset:0x0064 */
	__I  uint32_t DAT_GLOBAL;                                   /*!< Register,  Address offset:0x0068 */
	__I  uint32_t DAT_CHx[9];                                   /*!< Register,  Address offset:0x006C-0x008C */
	__IO uint32_t DMA_CON;                                      /*!< Register,  Address offset:0x0090 */
	__IO uint32_t FLR;                                          /*!< Register,  Address offset:0x0094 */
	__IO uint32_t CLR_FIFO;                                     /*!< Register,  Address offset:0x0098 */
	__IO uint32_t CLK_DIV;                                      /*!< Register,  Address offset:0x009C */
	__IO uint32_t DLY_CNT;                                      /*!< Register,  Address offset:0x00A0 */
	__IO uint32_t DBG_SEL;                                      /*!< Register,  Address offset:0x00A4 */
	__I  uint32_t RSVD;                                         /*!< Register,  Address offset:0x00A8 */
	__I  uint32_t RSVD1;                                        /*!< Register,  Address offset:0x00AC */
	__I  uint32_t RSVD2;                                        /*!< Register,  Address offset:0x00B0 */
	__I  uint32_t RSVD3;                                        /*!< Register,  Address offset:0x00B4 */
	__I  uint32_t RSVD4;                                        /*!< Register,  Address offset:0x00B8 */
	__IO uint32_t VREF_SETTLE_TIME;                             /*!< Register,  Address offset:0x00BC */
} ADC_TypeDef;

#define ADC_BASE ((ADC_TypeDef *) (0x4020A000))
/** @} */


/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif

//Please add your defination here

/* Exported types ------------------------------------------------------------*/
/** @addtogroup ADC_Exported_Types ADC Exported Types
  * @{
  */

/**
  * @brief  ADC Init Structure Definition
  */
typedef struct {
	u8 ADC_OpMode;				/*!< Specifies ADC operation mode.
									This parameter can be a value of @ref ADC_Operation_Mode_Definitions */

	u8 ADC_CvlistLen;				/*!< The number of valid items in the ADC conversion channel list is (ADC_CvlistLen + 1).
									This parameter can be set to 0~15 */

	u8 ADC_Cvlist[16];				/*!< Specifies the ADC channel conversion order. Each member should be
									the channel index */

	u16 ADC_ClkDiv;					/*!< Specifies ADC clock divider.
									This parameter can be a value of @ref ADC_CLK_Divider_Definitions */

	u8 ADC_RxThresholdLevel;		/*!< Specifies the receive FIFO threshold level.
							    		When the number of rx FIFO entries is greater than or equal to this
							     		value +1, the receive FIFO full interrupt is triggered. */

	u8 ADC_DMAThresholdLevel;	/*!< Specifies ADC DMA operation threshold.
									The dma_rx_req is generated when the number of valid data entries in the
									receive FIFO is equal to or above this field value+1 and ADC_DMA_EN=1. */

	u8  ADC_SpecialCh;			/*!< Specifies ADC particular channel. This parameter defines that ADC module
									should send interrupt signal to system when a conversion which of channel
									number is the same as this parameter. Default 0xFF means there is no need
									to set particular channel. */

	u32 ADC_ChanInType; 		/*!< Specifies CH0~5 input type. Default all channels are in single-end mode.
									If some channels need to be set to differential mode, use a value or
									combination of @ref ADC_CH_Input_Type_Definitions. */

	u8 ADC_ChIDEn;				/*!< Specifies whether ADC enables BIT_ADC_DAT_CHID or not. */

} ADC_InitTypeDef;

/**
  * @brief ADC Calibration Parameter Structure Definition
  */
typedef struct {
	s32 cal_a;
	s32 cal_b;
	s32 cal_c;
	BOOL init_done; /* calibration parameter init done */
} ADC_CalParaTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup ADC_Exported_Constants ADC Exported Constants
  * @{
  */

/** @defgroup ADC_Chn_Selection
  * @{
  */
#define ADC_CH0						((u8)0x00)
#define ADC_CH1						((u8)0x01)
#define ADC_CH2						((u8)0x02)
#define ADC_CH3						((u8)0x03)
#define ADC_CH4						((u8)0x04)
#define ADC_CH5						((u8)0x05)
#define ADC_CH6						((u8)0x06)
#define ADC_CH7						((u8)0x07)

#define ADC_CH_NUM					(8) // 8+3
#define ADC_EXT_CH_NUM				(8)
#define ADC_GLOBAL					((u8)0xFF)

#define IS_ADC_CHN_SEL(SEL)			(((SEL) == ADC_CH0) || \
									((SEL) == ADC_CH1) || \
									((SEL) == ADC_CH2) || \
									((SEL) == ADC_CH3) || \
									((SEL) == ADC_CH4) || \
									((SEL) == ADC_CH5) || \
									((SEL) == ADC_CH6) || \
									((SEL) == ADC_CH7))
/**
  * @}
  */

/** @defgroup ADC_Chn_Pad_Selection
  * @{
  */
#define ADC_CH0_PIN					(_PF_12)
#define ADC_CH1_PIN					(_PF_3)
#define ADC_CH2_PIN					(_PF_2)
#define ADC_CH3_PIN					(_PF_1)
#define ADC_CH4_PIN					(_PA_0)
#define ADC_CH5_PIN					(_PA_1)
#define ADC_CH6_PIN					(_PA_2)
#define ADC_CH7_PIN					(_PA_5)
/**
  * @}
  */

/** @defgroup ADC_Clk_Divider
  * @{
  */
#define ADC_CLK_DIV_4			((u16)0x004)
#define ADC_CLK_DIV_8			((u16)0x008)
#define ADC_CLK_DIV_16			((u16)0x010)
#define ADC_CLK_DIV_24			((u16)0x018)
#define ADC_CLK_DIV_32			((u16)0x020)
#define ADC_CLK_DIV_64			((u16)0x040)
#define ADC_CLK_DIV_128			((u16)0x080)

#define ADC_CLK_DIV_640			((u16)0x280)
#define ADC_CLK_DIV_800			((u16)0x320)
#define ADC_CLK_DIV_960			((u16)0x3C0)
#define ADC_CLK_DIV_1120		((u16)0x460)
#define ADC_CLK_DIV_1280		((u16)0x500)

#define IS_ADC_SAMPLE_CLK(CLK)		(((CLK) == ADC_CLK_DIV_640) || \
									((CLK) == ADC_CLK_DIV_800) || \
									((CLK) == ADC_CLK_DIV_960) || \
									((CLK) == ADC_CLK_DIV_1120) || \
									((CLK) == ADC_CLK_DIV_1280))

#define IS_ADC_CLK_DIV(CLK_DIV)		(CLK_DIV < (0x10000))
/**
  * @}
  */

/** @defgroup VREF_SETTLE_TIME
  * @{
  */
#define ADC_VREF_SETTLE_TIME_20_US			(20)
#define ADC_VREF_SETTLE_TIME_40_US			(40)
#define ADC_VREF_SETTLE_TIME_80_US			(80)
#define ADC_VREF_SETTLE_TIME_160_US			(160)

#define IS_ADC_VREF_SETTLE_TIME(TIME)	(((TIME) == ADC_VREF_SETTLE_TIME_20_US) || \
										((TIME) == ADC_VREF_SETTLE_TIME_40_US) || \
										((TIME) == ADC_VREF_SETTLE_TIME_80_US) || \
										((TIME) == ADC_VREF_SETTLE_TIME_160_US))
/**
  * @}
  */

/** @defgroup ADC_Operation_Mode
  * @{
  */
#define ADC_SW_TRI_MODE				((u8)0x00)	// ADC software-trigger mode
#define ADC_AUTO_MODE				((u8)0x01)	// ADC automatic mode
#define ADC_TIM_TRI_MODE			((u8)0x02)	// ADC timer-trigger mode
#define ADC_COMP_ASSIST_MODE		((u8)0x03)	// ADC comparator-assist mode

#define IS_ADC_MODE(mode)		(((mode) == ADC_SW_TRI_MODE) || \
								((mode) == ADC_AUTO_MODE) || \
								((mode) == ADC_TIM_TRI_MODE) || \
								((mode) == ADC_COMP_ASSIST_MODE))
/**
  * @}
  */

/** @defgroup ADC_CH_Input_Type
  * @{
  */
#define ADC_DIFFERENTIAL_CH(x)			((u32)0x00000001 << (2*x))
#define ADC_VDDIO_CH(x)					((u32)0x00000002 << (2*x))
#define IS_ADC_VDDIO_CH(ch)				((ch) == ADC_CH6)
/**
  * @}
  */

/** @defgroup ADC_Compare_Control
  * @{
  */
#define ADC_COMP_SMALLER_THAN_THL			((u8)0x00)	// Vin < ADC_COMP_TH_L
#define ADC_COMP_GREATER_THAN_THH			((u8)0x01)	// Vin > ADC_COMP_TH_H
#define ADC_COMP_WITHIN_THL_AND_THH			((u8)0x02)	// Vin >= ADC_COMP_TH_L &&  Vin <= ADC_COMP_TH_H
#define ADC_COMP_OUTSIDE_THL_AND_THH		((u8)0x03)	// Vin < ADC_COMP_TH_L  || Vin > ADC_COMP_TH_H
#define IS_ADC_COMP_CRITERIA(rule)			(((rule) == ADC_COMP_SMALLER_THAN_THL) || \
											((rule) == ADC_COMP_GREATER_THAN_THH) || \
											((rule) == ADC_COMP_WITHIN_THL_AND_THH) || \
											((rule) == ADC_COMP_OUTSIDE_THL_AND_THH))
/**
  * @}
  */

/** @defgroup ADC_Compare_Threshold
  * @{
  */
#define IS_ADC_VALID_COMP_TH(x)				((x) < 0x1000)
/**
  * @}
  */

/** @defgroup ADC_Valid_Timer
  * @{
  */
#define IS_ADC_VALID_TIM(idx)				((idx) < 8)
/**
  * @}
  */

/** @defgroup ADC_Interrupt_Control
  * @{
  */
#define ADC_BIT_IT_ALL_EN				(ADC_BIT_IT_COMP_CH8_EN   |\
										ADC_BIT_IT_COMP_CH7_EN   |\
										ADC_BIT_IT_COMP_CH6_EN   |\
										ADC_BIT_IT_COMP_CH5_EN   |\
										ADC_BIT_IT_COMP_CH4_EN   |\
										ADC_BIT_IT_COMP_CH3_EN   |\
										ADC_BIT_IT_COMP_CH2_EN   |\
										ADC_BIT_IT_COMP_CH1_EN   |\
										ADC_BIT_IT_COMP_CH0_EN   |\
										ADC_BIT_IT_ERR_EN        |\
										ADC_BIT_IT_DAT_OVW_EN    |\
										ADC_BIT_IT_FIFO_EMPTY_EN |\
										ADC_BIT_IT_FIFO_OVER_EN  |\
										ADC_BIT_IT_FIFO_FULL_EN  |\
										ADC_BIT_IT_CHCV_END_EN   |\
										ADC_BIT_IT_CV_END_EN     |\
										ADC_BIT_IT_CVLIST_END_EN)
/**
  * @}
  */

/** @defgroup ADC_Interrupt_Status
  * @{
  */

#define ADC_BIT_IT_COMP_ALL_STS				(ADC_BIT_IT_COMP_CH0_STS | \
											ADC_BIT_IT_COMP_CH1_STS | \
											ADC_BIT_IT_COMP_CH2_STS | \
											ADC_BIT_IT_COMP_CH3_STS | \
											ADC_BIT_IT_COMP_CH4_STS | \
											ADC_BIT_IT_COMP_CH5_STS | \
											ADC_BIT_IT_COMP_CH6_STS | \
											ADC_BIT_IT_COMP_CH7_STS | \
											ADC_BIT_IT_COMP_CH8_STS)

#define ADC_BIT_IT_ALL_STS					(ADC_BIT_IT_COMP_ALL_STS | \
											ADC_BIT_IT_ERR_STS | \
											ADC_BIT_IT_DAT_OVW_STS |\
											ADC_BIT_IT_FIFO_EMPTY_STS |\
											ADC_BIT_IT_FIFO_OVER_STS |\
											ADC_BIT_IT_FIFO_FULL_STS |\
											ADC_BIT_IT_CHCV_END_STS |\
											ADC_BIT_IT_CV_END_STS |\
											ADC_BIT_IT_CVLIST_END_STS)
/**
  * @}
  */

/** @defgroup ADC_CHSW_List
  * @{
  */
#define ADC_SHIFT_CHSW0(x)						(4*x)
#define ADC_SHIFT_CHSW1(x)						(4*(x - 8))
/**
  * @}
  */

/** @defgroup ADC_Comparison_Setting
  * @{
  */
#define ADC_SHIFT_COMP_CTRL_CH(x)				(2*x)
#define ADC_MASK_COMP_CTRL_CH(x)				(u32)(0x00000003 << ADC_SHIFT_COMP_CTRL_CH(x))

#define ADC_SHIFT_COMP_STS_CH(x)				(2*x)
#define ADC_MASK_COMP_STS_CH(x)					(u32)(0x00000003 << ADC_SHIFT_COMP_STS_CH(x))

#define ADC_IT_COMP_CH_EN(x)					((u32)0x00000001 << ((8+x)))

#define ADC_COMP_LEVEL_DETECT					(0x0)
#define ADC_COMP_EDGE_DETECT					(0x1)

/**
  * @}
  */

/** @defgroup ADC_Data_Setting
  * @{
  */
#define ADC_ID_AND_DATA(x)						((u32)((x) & 0x03800FFF))

/**
  * @}
  */

/** @defgroup ADC_Over_Sample_Setting
  * @{
  */
#define ADC_OSR_2						((u8)0x00)
#define ADC_OSR_4						((u8)0x01)
#define ADC_OSR_8						((u8)0x02)
#define ADC_OSR_16						((u8)0x03)
#define ADC_OSR_32						((u8)0x04)
#define ADC_OSR_64						((u8)0x05)
#define ADC_OSR_128						((u8)0x06)
#define ADC_OSR_256						((u8)0x07)
#define IS_ADC_OSR(x)					((x) < 0x08)

#define ADC_OSF_NONE					((u8)0x00)
#define ADC_OSF_1						((u8)0x01)
#define ADC_OSF_2						((u8)0x02)
#define ADC_OSF_3						((u8)0x03)
#define ADC_OSF_4						((u8)0x04)
#define ADC_OSF_5						((u8)0x05)
#define ADC_OSF_6						((u8)0x06)
#define ADC_OSF_7						((u8)0x07)
#define IS_ADC_OSF(x)					((x) < 0x08)

#define ADC_OS_STAGGERED				((u8)0x00)
#define ADC_OS_REGULAR					((u8)0x01)
#define IS_ADC_OS_MODE(x)				((x) < 0x10)

/**
  * @}
  */

/** @defgroup ADC_OTP_Address_Setting
  * @{
  */
#define DIVIDOR_MODE_ADDR			0x688 // OTP start address for dividor mode channel voltage calibration
#define INTER_R_ADDR			    0x68E // OTP start address for internal resistance
#define BYPASS_MODE_ADDR			0x690 // OTP start address for bypass mode channel voltage calibration
// #define VREF_SEL_ADDR			0x7EB // OTP start address for vref selection
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
_LONG_CALL_ void ADC_StructInit(ADC_InitTypeDef *ADC_InitStruct);
_LONG_CALL_ void ADC_Init(ADC_InitTypeDef *ADC_InitStruct);
_LONG_CALL_ void ADC_Cmd(u32 NewState);
_LONG_CALL_ void ADC_INTConfig(u32 ADC_IT, u32 NewState);
_LONG_CALL_ void ADC_INTClear(void);
_LONG_CALL_ void ADC_INTClearPendingBits(u32 ADC_IT);
_LONG_CALL_ u32 ADC_GetISR(void);
_LONG_CALL_ u32 ADC_GetRawISR(void);
_LONG_CALL_ u32 ADC_GetLastItem(void);
_LONG_CALL_ u32 ADC_GetCompStatus(u8 ADC_Channel);
_LONG_CALL_ u32 ADC_GetRxCount(void);
_LONG_CALL_ u32 ADC_GetLastChan(void);
_LONG_CALL_ void ADC_SetCompMode(u8 det_mode);
_LONG_CALL_ void ADC_SetComp(u8 ADC_channel, u16 CompThresH, u16 CompThresL, u8 CompCtrl);
_LONG_CALL_ void ADC_ResetChSwList(void);
_LONG_CALL_ u32 ADC_Readable(void);
_LONG_CALL_ u32 ADC_Read(void);
_LONG_CALL_ void ADC_ReceiveBuf(u16 *pBuf, u32 len);
_LONG_CALL_ u32 ADC_ChDataReady(u8 ADC_channel, u32 *ADC_data);
_LONG_CALL_ void ADC_ClearFIFO(void);
_LONG_CALL_ u32 ADC_GetStatus(void);
_LONG_CALL_ void ADC_SWTrigCmd(u32 NewState);
_LONG_CALL_ void ADC_SetClkDiv(u16 div);
_LONG_CALL_ void ADC_SetVrefSettleTime(u16 vref_settle_time);
_LONG_CALL_ void ADC_AutoChSwCmd(u32 NewState);
_LONG_CALL_ void ADC_TimerTrigCmd(u8 Tim_Idx, u32 PeriodMs, u32 NewState);
_LONG_CALL_ void ADC_SetDmaEnable(u32 newState);
_LONG_CALL_ u32 ADC_RXGDMA_Init(GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc,
								u8 *pDataBuf, u32 DataLen);
_LONG_CALL_ void ADC_OverSampleCmd(u32 NewState);
_LONG_CALL_ void ADC_SetOverSample(u8 OS_Shift, u8 OS_Ratio, u8 OS_Mode);
_LONG_CALL_ void ADC_InitCalPara(ADC_CalParaTypeDef *CalPara);
_LONG_CALL_ void ADC_InitCalParaBypass(ADC_CalParaTypeDef *CalPara);
_LONG_CALL_ s32 ADC_GetVoltage(u32 chan_data);
_LONG_CALL_ u32 ADC_GetInterR(void);
_LONG_CALL_ void ADC_PowerFromPad(u32 NewState);
_LONG_CALL_ void ADC_PowerFromLDO(u32 NewState);
_LONG_CALL_ void ADC_BypassMode(u32 NewState);

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */
/** @} */

/** @} */
#endif  // AMEBA_ADC_H
