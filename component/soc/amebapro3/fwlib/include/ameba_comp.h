/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AMEBA_COMP_H
#define AMEBA_COMP_H

/* AUTO_GEN_START */

/* COMP is a part of AON-COMP */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup COMP
* @brief COMP driver modules
* @{
*/


/** @defgroup ref_chx
 * @brief
 * @{
 */
#define COMP_BIT_COMP_REF_SEL                       ((u32)0x00000001 << 31)                  /*!< R/W     0 0:Comparator, 1: SEL_VREFL[4:0] and SEL_VREFH[4:0] */
#define COMP_MASK_REF1                              ((u32)0x0000003F << 16)                  /*!< R/W   0x00
                                                                                              * This bit controls the comparator channel 0 internal reference voltage 1.
                                                                                              * Vref1 is equal to (the value of BIT_COMP_REF1+1) * 0.1V.
                                                                                              * Therefore, when BIT_COMP_REF1 is 5, Vref1 is 0.6V. 
                                                                                              * When BIT_COMP_REF1 is 30, Vref1 is 3.1V.
                                                                                              * 0x00: Vref1 is 0.1V.
                                                                                              * 0x01: Vref1 is 0.2V.
                                                                                              * 0x02: Vref1 is 0.3V
                                                                                              *            .
                                                                                              *            .
                                                                                              *            .
                                                                                              */
#define COMP_REF1(x)                                ((u32)(((x) & 0x0000003F) << 16))
#define COMP_GET_REF1(x)                            ((u32)(((x >> 16) & 0x0000003F)))
#define COMP_MASK_REF0                              ((u32)0x0000003F << 0)                   /*!< R/W   0x00
                                                                                              * This bit controls the comparator channel 0 internal reference voltage 0.
                                                                                              * Vref0 is equal to (the value of BIT_COMP_REF0+1) * 0.1V.
                                                                                              * Therefore, when BIT_COMP_REF0 is 5, Vref0 is 0.6V. 
                                                                                              * When BIT_COMP_REF0 is 30, Vref0 is 3.1V.
                                                                                              * 0x00: Vref0 is 0.1V.
                                                                                              * 0x01: Vref0 is 0.2V.
                                                                                              * 0x02: Vref0 is 0.3V
                                                                                              *            .
                                                                                              *            .
                                                                                              *            .
                                                                                              */
#define COMP_REF0(x)                                ((u32)(((x) & 0x0000003F) << 0))
#define COMP_GET_REF0(x)                            ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup intr_ctrl
 * @brief
 * @{
 */
#define COMP_MASK_WK_SYS_CTRL_3                     ((u32)0x00000003 << 26)                  /*!< R/W     0
                                                                                              * This bit defines the criterion of when comparator channel 3 should interrupts (wake up) system. The comparator should send wakeup signal only when BIT_COMP_WK_SYS_EN is 1, if the criterion matches.
                                                                                              * 0: When Vin < Vref0, the comparator interrupts (wake up) system.
                                                                                              * 1: When Vin > Vref1, the Comparator interrupts (wake up) system.
                                                                                              * 2: When Vin > Vref0 and Vin < Vref1, the Comparator interrupts (wake up) system.
                                                                                              * 3: When Vin < Vref0 or Vin > Vref1, the Comparator interrupts (wake up) system.
                                                                                              */
#define COMP_WK_SYS_CTRL_3(x)                       ((u32)(((x) & 0x00000003) << 26))
#define COMP_GET_WK_SYS_CTRL_3(x)                   ((u32)(((x >> 26) & 0x00000003)))
#define COMP_BIT_WK_SYS_EN_3                        ((u32)0x00000001 << 25)                  /*!< R/W     0
                                                                                              * This bit controls whether the comparator channel 3 sends interrupt (wake up) signal or not when the criterion of BIT_COMP_WK_SYS_CTRL matches.
                                                                                              * 0: Disable comparator interrupt (wake up) system.
                                                                                              * 1: Enable comparator interrupt (wake up) system.
                                                                                              */
#define COMP_MASK_WK_SYS_CTRL_2                     ((u32)0x00000003 << 23)                  /*!< R/W     0
                                                                                              * This bit defines the criterion of when comparator channel 2 should interrupt (wake up) system. The comparator should send wakeup signal only when BIT_COMP_WK_SYS_EN is 1, if the criterion matches.
                                                                                              * 0: When Vin < Vref0, the comparator interrupts (wake up) system.
                                                                                              * 1: When Vin >= Vref1, the Comparator interrupts (wake up) system.
                                                                                              * 2: When Vin > Vref0 and Vin < Vref1, the Comparator interrupts (wake up) system.
                                                                                              * 3: When Vin < Vref0 or Vin > Vref1, the Comparator interrupts (wake up) system.
                                                                                              */
#define COMP_WK_SYS_CTRL_2(x)                       ((u32)(((x) & 0x00000003) << 23))
#define COMP_GET_WK_SYS_CTRL_2(x)                   ((u32)(((x >> 23) & 0x00000003)))
#define COMP_BIT_WK_SYS_EN_2                        ((u32)0x00000001 << 22)                  /*!< R/W     0
                                                                                              * This bit controls whether the comparator channel 2 sends interrupt (wake up)  signal or not when the criterion of BIT_COMP_WK_SYS_CTRL matches.
                                                                                              * 0: Disable comparator interrupt (wake up)  system.
                                                                                              * 1: Enable comparator interrupt (wake up)  system.
                                                                                              */
#define COMP_MASK_WK_SYS_CTRL_1                     ((u32)0x00000003 << 20)                  /*!< R/W     0
                                                                                              * This bit defines the criterion of when comparator channel 1 should interrupt (wake up) system. The comparator should send wakeup signal only when BIT_COMP_WK_SYS_EN is 1, if the criterion matches.
                                                                                              * 0: When Vin < Vref0, the comparator interrupt (wake up)  system.
                                                                                              * 1: When Vin >= Vref1, the Comparator interrupt (wake up)  system.
                                                                                              * 2: When Vin > Vref0 and Vin < Vref1, the Comparator interrupts (wake up)  system.
                                                                                              * 3: When Vin < Vref0 or Vin > Vref1, the Comparator interrupts (wake up)  system.
                                                                                              */
#define COMP_WK_SYS_CTRL_1(x)                       ((u32)(((x) & 0x00000003) << 20))
#define COMP_GET_WK_SYS_CTRL_1(x)                   ((u32)(((x >> 20) & 0x00000003)))
#define COMP_BIT_WK_SYS_EN_1                        ((u32)0x00000001 << 19)                  /*!< R/W     0
                                                                                              * This bit controls whether the comparator channel 1 sends interrupt (wake up)  signal or not when the criterion of BIT_COMP_WK_SYS_CTRL matches.
                                                                                              * 0: Disable comparator interrupt (wake up)  system.
                                                                                              * 1: Enable comparator interrupt (wake up)  system.
                                                                                              */
#define COMP_MASK_WK_SYS_CTRL_0                     ((u32)0x00000003 << 17)                  /*!< R/W     0
                                                                                              * This bit defines the criterion of when comparator channel 0 should interrupt (wake up)  system. The comparator should send wakeup signal only when BIT_COMP_WK_SYS_EN is 1, if the criterion matches.
                                                                                              * 0: When Vin < Vref0, the comparator interrupts (wake up)  system.
                                                                                              * 1: When Vin >= Vref1, the Comparator interrupts (wake up)  system.
                                                                                              * 2: When Vin > Vref0 and Vin < Vref1, the Comparator interrupts (wake up)  system.
                                                                                              * 3: When Vin < Vref0 or Vin > Vref1, the Comparator interrupts (wake up)  system.
                                                                                              */
#define COMP_WK_SYS_CTRL_0(x)                       ((u32)(((x) & 0x00000003) << 17))
#define COMP_GET_WK_SYS_CTRL_0(x)                   ((u32)(((x >> 17) & 0x00000003)))
#define COMP_BIT_WK_SYS_EN_0                        ((u32)0x00000001 << 16)                  /*!< R/W     0
                                                                                              * This bit controls whether the comparator channel 0 sends wakeup signal or not when the criterion of BIT_COMP_WK_SYS_CTRL matches.
                                                                                              * 0: Disable comparator interrupt (wake up) system.
                                                                                              * 1: Enable comparator interrupt (wake up) system.
                                                                                              */
/** @} */


/** @defgroup intr_sts
 * @brief
 * @{
 */
#define COMP_BIT_WK_SYS_STS_3                       ((u32)0x00000001 << 25)                  /*!< R/W1C   0
                                                                                              * This bit reflects if channle 3 of comparator module sent a interrupt (wake-up) signal to system or not.
                                                                                              * 0: Channel 3 did NOT send an interrupt (wake-up) signal to System.
                                                                                              * 1: Channel 3 has sent an interrupt (wake-up) signal to System.
                                                                                              */
#define COMP_BIT_WK_SYS_STS_2                       ((u32)0x00000001 << 22)                  /*!< R/W1C   0
                                                                                              * This bit reflects if channle 2 of comparator module sent a interrupt (wake-up) signal to system or not.
                                                                                              * 0: Channel 2 did NOT send an interrupt (wake-up) signal to System.
                                                                                              * 1: Channel 2 has sent an interrupt (wake-up) signal to System.
                                                                                              */
#define COMP_BIT_WK_SYS_STS_1                       ((u32)0x00000001 << 19)                  /*!< R/W1C   0
                                                                                              * This bit reflects if channle 1 of comparator module sent a interrupt (wake-up) signal to system or not.
                                                                                              * 0: Channel 1 did NOT send an interrupt (wake-up) signal to System.
                                                                                              * 1: Channel 1 has sent an interrupt (wake-up) signal to System.
                                                                                              */
#define COMP_BIT_WK_SYS_STS_0                       ((u32)0x00000001 << 16)                  /*!< R/W1C   0
                                                                                              * This bit reflects if channle 0 of comparator module sent a interrupt (wake-up) signal to system or not.
                                                                                              * 0: Channel 0 did NOT send an interrupt (wake-up) signal to System.
                                                                                              * 1: Channel 0 has sent an interrupt (wake-up) signal to System.
                                                                                              */
/** @} */


/** @defgroup intr_raw_sts
 * @brief
 * @{
 */
#define COMP_BIT_WK_SYS_STS_3_018                   ((u32)0x00000001 << 25)                  /*!< R       0
                                                                                              * This bit reflects if channle 3 of comparator module sent a interrupt (wake-up) signal to system or not.
                                                                                              * 0: Channel 3 did NOT send an interrupt (wake-up) signal to System.
                                                                                              * 1: Channel 3 has sent an interrupt (wake-up) signal to System.
                                                                                              */
#define COMP_BIT_WK_SYS_STS_2_018                   ((u32)0x00000001 << 22)                  /*!< R       0
                                                                                              * This bit reflects if channle 2 of comparator module sent a interrupt (wake-up) signal to system or not.
                                                                                              * 0: Channel 2 did NOT send an interrupt (wake-up) signal to System.
                                                                                              * 1: Channel 2 has sent an interrupt (wake-up) signal to System.
                                                                                              */
#define COMP_BIT_WK_SYS_STS_1_018                   ((u32)0x00000001 << 19)                  /*!< R       0
                                                                                              * This bit reflects if channle 1 of comparator module sent a interrupt (wake-up) signal to system or not.
                                                                                              * 0: Channel 1 did NOT send an interrupt (wake-up) signal to System.
                                                                                              * 1: Channel 1 has sent an interrupt (wake-up) signal to System.
                                                                                              */
#define COMP_BIT_WK_SYS_STS_0_018                   ((u32)0x00000001 << 16)                  /*!< R       0
                                                                                              * This bit reflects if channle 0 of comparator module sent a interrupt (wake-up) signal to system or not.
                                                                                              * 0: Channel 0 did NOT send an interrupt (wake-up) signal to System.
                                                                                              * 1: Channel 0 has sent an interrupt (wake-up) signal to System.
                                                                                              */
/** @} */


/** @defgroup itemsw_list
 * @brief
 * @{
 */
#define COMP_MASK_ITEMSW_3                          ((u32)0x0000000F << 12)                  /*!< R/W     0 These bits defines which comparator Channel start comparison at the 4th item to do */
#define COMP_ITEMSW_3(x)                            ((u32)(((x) & 0x0000000F) << 12))
#define COMP_GET_ITEMSW_3(x)                        ((u32)(((x >> 12) & 0x0000000F)))
#define COMP_MASK_ITEMSW_2                          ((u32)0x0000000F << 8)                   /*!< R/W     0 These bits defines which comparator Channel start comparison at the 3rd item to do */
#define COMP_ITEMSW_2(x)                            ((u32)(((x) & 0x0000000F) << 8))
#define COMP_GET_ITEMSW_2(x)                        ((u32)(((x >> 8) & 0x0000000F)))
#define COMP_MASK_ITEMSW_1                          ((u32)0x0000000F << 4)                   /*!< R/W     0 These bits defines which comparator Channel start comparison at the 2nd item to do */
#define COMP_ITEMSW_1(x)                            ((u32)(((x) & 0x0000000F) << 4))
#define COMP_GET_ITEMSW_1(x)                        ((u32)(((x >> 4) & 0x0000000F)))
#define COMP_MASK_ITEMSW_0                          ((u32)0x0000000F << 0)                   /*!< R/W     0 These bits defines which comparator Channel start comparison at the 1st item to do */
#define COMP_ITEMSW_0(x)                            ((u32)(((x) & 0x0000000F) << 0))
#define COMP_GET_ITEMSW_0(x)                        ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup last_item
 * @brief
 * @{
 */
#define COMP_MASK_LAST_ITEM                         ((u32)0x00000003 << 0)                   /*!< R       0 These bits reflects the last used item. The last_item will be round robin from itemsw_0 to itemsw_3 */
#define COMP_LAST_ITEM(x)                           ((u32)(((x) & 0x00000003) << 0))
#define COMP_GET_LAST_ITEM(x)                       ((u32)(((x >> 0) & 0x00000003)))
/** @} */


/** @defgroup busy
 * @brief
 * @{
 */
#define COMP_BIT_BUSY                               ((u32)0x00000001 << 0)                   /*!< R       0
                                                                                              * This bit reflects the comparator is busy or not. If the comparator is processing a comparison of a channel, this bit remains 1 which indicates it's busy. Once a comparison is done, this bit becomes 0 which indicates its' ready to do another comparison.
                                                                                              * 0: The comparator is ready.
                                                                                              * 1: The comparator is busy.
                                                                                              */
/** @} */


/** @defgroup ch_sts
 * @brief
 * @{
 */
#define COMP_MASK_CH_STS_3                          ((u32)0x00000003 << 6)                   /*!< R       0
                                                                                              * This bit reflects the channel 3 comparison result.
                                                                                              * 0: When Vin < Vref0, the bits should be set to 0.
                                                                                              * 1: When Vin >= Vref1, the bits should be set to 1.
                                                                                              * 2:When Vin > Vref0 and Vin < Vref1, the bits should be set to 2.
                                                                                              */
#define COMP_CH_STS_3(x)                            ((u32)(((x) & 0x00000003) << 6))
#define COMP_GET_CH_STS_3(x)                        ((u32)(((x >> 6) & 0x00000003)))
#define COMP_MASK_CH_STS_2                          ((u32)0x00000003 << 4)                   /*!< R       0
                                                                                              * This bit reflects the channel 2 comparison result.
                                                                                              * 0: When Vin < Vref0, the bits should be set to 0.
                                                                                              * 1: When Vin >= Vref1, the bits should be set to 1.
                                                                                              * 2:When Vin > Vref0 and Vin < Vref1, the bits should be set to 2.
                                                                                              */
#define COMP_CH_STS_2(x)                            ((u32)(((x) & 0x00000003) << 4))
#define COMP_GET_CH_STS_2(x)                        ((u32)(((x >> 4) & 0x00000003)))
#define COMP_MASK_CH_STS_1                          ((u32)0x00000003 << 2)                   /*!< R       0
                                                                                              * This bit reflects the channel 1 comparison result.
                                                                                              * 0: When Vin < Vref0, the bits should be set to 0.
                                                                                              * 1: When Vin >= Vref1, the bits should be set to 1.
                                                                                              * 2:When Vin > Vref0 and Vin < Vref1, the bits should be set to 2.
                                                                                              */
#define COMP_CH_STS_1(x)                            ((u32)(((x) & 0x00000003) << 2))
#define COMP_GET_CH_STS_1(x)                        ((u32)(((x >> 2) & 0x00000003)))
#define COMP_MASK_CH_STS_0                          ((u32)0x00000003 << 0)                   /*!< R       0
                                                                                              * This bit reflects the channel 0 comparison result.
                                                                                              * 0: When Vin < Vref0, the bits should be set to 0.
                                                                                              * 1: When Vin >= Vref1, the bits should be set to 1.
                                                                                              * 2:When Vin > Vref0 and Vin < Vref1, the bits should be set to 2.
                                                                                              */
#define COMP_CH_STS_0(x)                            ((u32)(((x) & 0x00000003) << 0))
#define COMP_GET_CH_STS_0(x)                        ((u32)(((x >> 0) & 0x00000003)))
/** @} */


/** @defgroup auto_shut
 * @brief
 * @{
 */
#define COMP_BIT_AUTO_SHUT                          ((u32)0x00000001 << 0)                   /*!< R/W     0
                                                                                              * This bit controls the comparator to disable the analog module and mux automatically or not after the analog conversion is done.
                                                                                              * 0: The analog module and mux will NOT be disabled.The comparator module is always enabled.
                                                                                              * 1: The analog module and mux will be disabled automatically after the analog conversion is done.
                                                                                              */
/** @} */


/** @defgroup wk_timer
 * @brief
 * @{
 */
#define COMP_BIT_WK_TIMER                           ((u32)0x00000001 << 0)                   /*!< R/W     0
                                                                                              * This bit controls the external trigger source of general timer enabled or not.
                                                                                              * If it is enabled, the comparator would execute compare process when the timer event sends to comparator module.
                                                                                              * 0: Disable external timer trigger
                                                                                              * 1: Enable external timer trigger
                                                                                              */
/** @} */


/** @defgroup wk_timer_sel
 * @brief
 * @{
 */
#define COMP_MASK_WK_TIMER_SEL                      ((u32)0x00000007 << 0)                   /*!< R/W     0
                                                                                              * This three bits defines which timer channel would be used to wake up the comparator.
                                                                                              * BIT_COMP_EXT_WK_TIMER_SEL is 
                                                                                              * 0: Timer module 0 is used as the comparator external trigger source.
                                                                                              * 1: Timer module 1 is used as the comparator external trigger source.
                                                                                              *      .
                                                                                              *      .
                                                                                              *      .
                                                                                              * 7: Timer module 7 is used as the comparator external trigger source.
                                                                                              */
#define COMP_WK_TIMER_SEL(x)                        ((u32)(((x) & 0x00000007) << 0))
#define COMP_GET_WK_TIMER_SEL(x)                    ((u32)(((x >> 0) & 0x00000007)))
/** @} */


/** @defgroup rst_list
 * @brief
 * @{
 */
#define COMP_BIT_RST_LIST                           ((u32)0x00000001 << 0)                   /*!< R/W     0
                                                                                              * This bit controls to reset the item switch to default state or not. Once this bit is set, the auto/manual channel switch goes to the first item as a default state. This control bit will trigger from 0 to 1
                                                                                              * 0: Do not reset conversion list to default state.
                                                                                              * 1: Force last_ittem to itemsw_0.
                                                                                              */
/** @} */


/** @defgroup auto_sw_en
 * @brief
 * @{
 */
#define COMP_BIT_AUTO_SW_EN                         ((u32)0x00000001 << 0)                   /*!< R/W     0
                                                                                              * This bit controls the automatic channel swtich enabled or disabled.
                                                                                              * If it's disabled, comparator could be run in software or timer trigger mode.
                                                                                              * 0: Disable the automatic channel switch. 
                                                                                              * 1: Enable the automatic channel switch. 
                                                                                              */
/** @} */


/** @defgroup enable
 * @brief
 * @{
 */
#define COMP_BIT_COMP_DBG_EN                        ((u32)0x00000001 << 2)                   /*!< R/W     0 1:enable debug */
#define COMP_BIT_TRIG_EN                            ((u32)0x00000001 << 1)                   /*!< R/W     0
                                                                                              * This bit is a mulitple trigger control source.
                                                                                              * For software trigger (when comparator auto channel switch register (offset: 0x3C) is NOT set and wake timer control is NOT set), this bit is used to be a software trigger source. Each time this bit is set to 1 from 0, it drives comparator to do a channel comparison.
                                                                                              * For auto trigger, this bit is used to drive comparator to start the auto channel switch operation.
                                                                                              */
#define COMP_BIT_ENABLE                             ((u32)0x00000001 << 0)                   /*!< R/W     0 This bit is for comparator enable control. */
/** @} */


/** @defgroup wk_shut_cnt
 * @brief
 * @{
 */
#define COMP_MASK_POWON_CNT                         ((u32)0x000000FF << 24)                  /*!< R/W   0x64 It is time delay between comparator trigger to read and LPSD[0] */
#define COMP_POWON_CNT(x)                           ((u32)(((x) & 0x000000FF) << 24))
#define COMP_GET_POWON_CNT(x)                       ((u32)(((x >> 24) & 0x000000FF)))
#define COMP_MASK_DATA_VALID_CNT                    ((u32)0x000000FF << 16)                  /*!< R/W   0x6E Data valid counter after turn on REG0X_LPSD[0] */
#define COMP_DATA_VALID_CNT(x)                      ((u32)(((x) & 0x000000FF) << 16))
#define COMP_GET_DATA_VALID_CNT(x)                  ((u32)(((x >> 16) & 0x000000FF)))
#define COMP_MASK_CHSW_CNT                          ((u32)0x000000FF << 8)                   /*!< R/W   0x3C TBD */
#define COMP_CHSW_CNT(x)                            ((u32)(((x) & 0x000000FF) << 8))
#define COMP_GET_CHSW_CNT(x)                        ((u32)(((x >> 8) & 0x000000FF)))
#define COMP_MASK_WK_SHUT_CNT                       ((u32)0x000000FF << 0)                   /*!< R/W   0x96 This eight bits defines how long to disable comparator analog module when an external trigger event is sent to the comparator. The count must include the comparator response time to get a correct compare result. */
#define COMP_WK_SHUT_CNT(x)                         ((u32)(((x) & 0x000000FF) << 0))
#define COMP_GET_WK_SHUT_CNT(x)                     ((u32)(((x >> 0) & 0x000000FF)))
/** @} */




/** @defgroup COMP_Exported_Types COMP Exported Types
* @{
*/
/**
* @brief COMP Register Declaration
*/

typedef struct {
	__IO uint32_t COMP_REF_CHx[4];                                   /*!< Register,  Address offset:0x0000-0x000C */
	__IO uint32_t COMP_INTR_CTRL;                                    /*!< Register,  Address offset:0x0010 */
	__IO uint32_t COMP_INTR_STS;                                     /*!< Register,  Address offset:0x0014 */
	__I  uint32_t COMP_INTR_RAW_STS;                                 /*!< Register,  Address offset:0x0018 */
	__IO uint32_t COMP_ITEMSW_LIST;                                  /*!< Register,  Address offset:0x001C */
	__I  uint32_t COMP_LAST_ITEM;                                    /*!< Register,  Address offset:0x0020 */
	__I  uint32_t COMP_BUSY;                                         /*!< Register,  Address offset:0x0024 */
	__I  uint32_t COMP_CH_STS;                                       /*!< Register,  Address offset:0x0028 */
	__IO uint32_t COMP_AUTO_SHUT;                                    /*!< Register,  Address offset:0x002C */
	__IO uint32_t COMP_WK_TIMER;                                     /*!< Register,  Address offset:0x0030 */
	__IO uint32_t COMP_WK_TIMER_SEL;                                 /*!< Register,  Address offset:0x0034 */
	__IO uint32_t COMP_RST_LIST;                                     /*!< Register,  Address offset:0x0038 */
	__IO uint32_t COMP_AUTO_SW_EN;                                   /*!< Register,  Address offset:0x003C */
	__IO uint32_t COMP_ENABLE;                                       /*!< Register,  Address offset:0x0040 */
	__IO uint32_t COMP_WK_SHUT_CNT;                                  /*!< Register,  Address offset:0x0044 */
} COMP_TypeDef;

// #define COMP_BASE ((COMP_TypeDef *) (?))
/** @} */


/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here

enum comp_reg_access_addr_e {
	Cmp_Ref_Ch0         = 0x00,
	Cmp_Ref_Ch1         = 0x04,
	Cmp_Ref_Ch2         = 0x08,
	Cmp_Ref_Ch3         = 0x0c,
	Cmp_Intr_Ctrl       = 0x10,
	Cmp_Intr_Sts        = 0x14,
	Cmp_Intr_Raw_Sts    = 0x18,
	Cmp_Itemsw_List     = 0x1c,
	Cmp_Last_Item       = 0x20,
	Cmp_Busy            = 0x24,
	Cmp_Ch_Sts          = 0x28,
	Cmp_Auto_Shut       = 0x2c,
	Cmp_Wk_Timer        = 0x30,
	Cmp_Wk_Timer_Sel    = 0x34,
	Cmp_Rst_List        = 0x38,
	Cmp_Auto_Sw_En      = 0x3c,
	Cmp_Enable          = 0x40,
	Cmp_Wk_Shut_Cnt     = 0x44
};

/**
  * @brief	Comparator Channel Init Structure Definition
  */
typedef struct {
	u8 CMP_ChIndex;			/*!< Specify the channel index */

	u8 CMP_Ref0;			/*!< Specify the internal reference voltage0, the value can be 0~31,
									Vref0 = CMP_Ref0*0.1v */
	u8 CMP_Ref1;			/*!< Specify the internal reference voltage1, the value can be 0~31,
									Vref1 = CMP_Ref1*0.1v */
	u8 CMP_WakeType;		/*!< Specify this channel wakeup system or ADC when criterion matches.
									This parameter can be a value or combination of
									@ref COMP_Compare_Wakeup_Type */
	u8 CMP_WakeSysCtrl;		/*!< Specify the criteria of when comparator channel should wakeup
									system, which can be a value of
									@ref COMP_Wakeup_Control */
} CMP_CHTypeDef;

/**
  * @brief Comparator Init Structure Definition
  */
typedef struct {

	CMP_CHTypeDef CMP_ChanCtrl[4];  // Specify the comparator channel control parameters

} CMP_InitTypeDef;

/**
  * @}
  */

/** @defgroup COMP_Channel
  * @{
  */
#define COMP_CH0						((u8)0x00)
#define COMP_CH1						((u8)0x01)
#define COMP_CH2						((u8)0x02)
#define COMP_CH3						((u8)0x03)
#define COMP_CH_NUM						(4)

#define IS_COMP_CHN_SEL(SEL)			(((SEL) == COMP_CH0) || \
										((SEL) == COMP_CH1) || \
										((SEL) == COMP_CH2) || \
										((SEL) == COMP_CH3))

/** @defgroup COMP_Valid_Timer
  * @{
  */
#define IS_COMP_VALID_TIM(idx)				((idx) < 8)
/**
  * @}
  */

/** @defgroup COMP_Channel_List_Shift
  * @{
  */
#define COMP_SHIFT_ITEMSW(x)				(4*x)
/**
  * @}
  */

/** @defgroup COMP_Channel_Status
  * @{
  */
#define COMP_SHIFT_CH_STS(x)			(2*x)
#define COMP_MASK_CH_STS(x)				(u32)(0x3 << COMP_SHIFT_CH_STS(x))
/**
  * @}
  */

/** @defgroup COMP_Compare_Wakeup_Type
 * @{
 */
#define COMP_WK_NONE					((u8)0x00)
#define COMP_WK_ADC						((u8)0x01)
#define COMP_WK_SYS						((u8)0x02)
/**
  * @}
  */

/** @defgroup COMP_Wakeup_Control
 * @{
 */
#define COMP_SHIFT_WK_SYS_CTRL(x)			(17 + 3*x)
#define COMP_MASK_WK_SYS_CTRL(x)			(u32)(0x00000003 << COMP_SHIFT_WK_SYS_CTRL(x))
#define COMP_SHIFT_WK_SYS_EN(x)				(16 + 3*x)
#define COMP_WK_SYS_EN(x)					(u32)(0x00000001 << COMP_SHIFT_WK_SYS_EN(x))
/**
  * @}
  */




/** @defgroup COMP_Compare_Control
  * @{
  */
#define COMP_SMALLER_THAN_REF0			((u8)0x00)	// Vin < Vref0
#define COMP_GREATER_THAN_REF1			((u8)0x01)	// Vin >= Vref1
#define COMP_WITHIN_REF0_AND_REF1		((u8)0x02)	// Vin > Vref0 && Vin < Vref1
#define COMP_OUTSIDE_REF0_AND_REF1		((u8)0x03)	// Vin < Vref0 && Vin > Vref1
/**
  * @}
  */

/** @defgroup COMP_Compare_Status
  * @{
  */

#define IS_COMP_STS_SMALLER_THAN_REF0(STS) 		(((STS) == 0x0))
#define IS_COMP_STS_GREATER_THAN_REF1(STS) 		(((STS) == 0x3))
#define IS_COMP_STS_WITHIN_REF0_AND_REF1(STS)	(((STS) == 0x2))
#define IS_COMP_STS_OUTSIDE_REF0_AND_REF1(STS) 	(((STS) == 0x0) || ((STS) == 0x3))

#define IS_COMP_STS_GREATER_THAN_REF0(STS)		(((STS) == 0x2))

/**
  * @}
  */

/** @defgroup COMP_Intr_Sts
* @{
*/
#define COMP_ALL_INTR_STS 	(COMP_BIT_WK_SYS_STS_0 | \
							COMP_BIT_WK_SYS_STS_1 | \
							COMP_BIT_WK_SYS_STS_2 | \
							COMP_BIT_WK_SYS_STS_3)
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
_LONG_CALL_ u32 CMP_WaitReady(void);
_LONG_CALL_ void CMP_Write(u16 addr, u32 data);
_LONG_CALL_ u32 CMP_Read(u16 addr);
_LONG_CALL_ void CMP_StructInit(CMP_InitTypeDef *CMP_InitStruct);
_LONG_CALL_ void CMP_Init(CMP_InitTypeDef *CMP_InitStruct);
_LONG_CALL_ void CMP_Deinit(void);
_LONG_CALL_ void CMP_Disable(void);
_LONG_CALL_ void CMP_Cmd(u32 NewState);
_LONG_CALL_ u32 CMP_Busy(void);
_LONG_CALL_ u32 CMP_GetISR(void);
_LONG_CALL_ void CMP_INTClearPendingBit(u32 Cmp_IT);
_LONG_CALL_ u32 CMP_GetCompStatus(u8 channel);
_LONG_CALL_ u32 CMP_GetLastChan(void);
_LONG_CALL_ void CMP_ResetCSwList(void);
_LONG_CALL_ void CMP_AutoShutCmd(u32 NewState);
_LONG_CALL_ void CMP_AutoSwCmd(u32 NewState);
_LONG_CALL_ void CMP_SwTrigCmd(u32 NewState);
_LONG_CALL_ void CMP_TimerTrigCmd(u8 Tim_Idx, u32 PeriodMs, u32 NewState);
_LONG_CALL_ void CMP_SetVref(u8 cmp_idx, u8 vref0, u8 vref1);
_LONG_CALL_ void CMP_SetItemList(u8 *list, u8 length);

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

/** @} */

/** @} */
#endif  // AMEBA_COMP_H


