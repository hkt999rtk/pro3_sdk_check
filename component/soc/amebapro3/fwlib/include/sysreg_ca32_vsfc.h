/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_CA32_VSFC_H
#define SYSREG_CA32_VSFC_H

/* AUTO_GEN_START */

/* VSFC is a part of CA32_REG */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup VSFC
* @brief VSFC driver modules
* @{
*/


/** @defgroup REG_VSFC_CTRL0
 * @brief
 * @{
 */
#define VSFC_MASK_FSS_SLOPE_DEG_FILTER_H2L          ((u32)0x00000007 << 28)                  /*!< R/W   0x0
                                                                                              * Fast speed sensor detect de-glitch filter cycles for High to Low edge
                                                                                              * Valid range : 0~5
                                                                                              */
#define VSFC_FSS_SLOPE_DEG_FILTER_H2L(x)            ((u32)(((x) & 0x00000007) << 28))
#define VSFC_GET_FSS_SLOPE_DEG_FILTER_H2L(x)        ((u32)(((x >> 28) & 0x00000007)))
#define VSFC_MASK_FSS_DEG_FILTER_H2L                ((u32)0x00000007 << 24)                  /*!< R/W   0x0
                                                                                              * FSS signals de-glitch filter cycles for High to Low edge
                                                                                              * Valid range : 0~5
                                                                                              */
#define VSFC_FSS_DEG_FILTER_H2L(x)                  ((u32)(((x) & 0x00000007) << 24))
#define VSFC_GET_FSS_DEG_FILTER_H2L(x)              ((u32)(((x >> 24) & 0x00000007)))
#define VSFC_MASK_CPU_DEG_FILTER                    ((u32)0x00000007 << 20)                  /*!< R/W   0x0
                                                                                              * cpu internal signals de-glitch filter cycles for Low to High edge
                                                                                              * Valid range : 0~5
                                                                                              */
#define VSFC_CPU_DEG_FILTER(x)                      ((u32)(((x) & 0x00000007) << 20))
#define VSFC_GET_CPU_DEG_FILTER(x)                  ((u32)(((x >> 20) & 0x00000007)))
#define VSFC_MASK_FSS_SLOPE_DEG_FILTER              ((u32)0x00000007 << 16)                  /*!< R/W   0x0
                                                                                              * Fast speed sensor slope detect de-glitch filter cycles for Low to High edge
                                                                                              * Valid range : 0~5
                                                                                              */
#define VSFC_FSS_SLOPE_DEG_FILTER(x)                ((u32)(((x) & 0x00000007) << 16))
#define VSFC_GET_FSS_SLOPE_DEG_FILTER(x)            ((u32)(((x >> 16) & 0x00000007)))
#define VSFC_MASK_FSS_DEG_FILTER                    ((u32)0x00000007 << 12)                  /*!< R/W   0x0
                                                                                              * Fast speed sensor detect de-glitch filter cycles for Low to High edge
                                                                                              * Valid range : 0~5
                                                                                              */
#define VSFC_FSS_DEG_FILTER(x)                      ((u32)(((x) & 0x00000007) << 12))
#define VSFC_GET_FSS_DEG_FILTER(x)                  ((u32)(((x >> 12) & 0x00000007)))
#define VSFC_MASK_TRIG_SEL                          ((u32)0x00000007 << 8)                   /*!< R/W   0x0
                                                                                              * [0] : cpu internal
                                                                                              * [1] Fast Speed Sensor detect signal
                                                                                              * [2] Fast Speed Sensor slope detect signal
                                                                                              */
#define VSFC_TRIG_SEL(x)                            ((u32)(((x) & 0x00000007) << 8))
#define VSFC_GET_TRIG_SEL(x)                        ((u32)(((x >> 8) & 0x00000007)))
#define VSFC_BIT_DIV_SEL                            ((u32)0x00000001 << 4)                   /*!< R/W   0x0
                                                                                              * 0 : Digitally mask pulse
                                                                                              * 1 : Analog divide by 1.5
                                                                                              * (Only can be configured when glb_en=0)
                                                                                              */
#define VSFC_BIT_GLB_EN                             ((u32)0x00000001 << 0)                   /*!< R/W   0x0
                                                                                              * global enable
                                                                                              * (Programing glb_en from 0 to 1 will reset H/W shift register.)
                                                                                              */
/** @} */


/** @defgroup REG_VSFC_CTRL1
 * @brief
 * @{
 */
#define VSFC_MASK_FSS_DETECT_EN                     ((u32)0x000001FF << 16)                  /*!< R/W   0x1ff
                                                                                              * 1 : enable the corresponding FSS detect/slope detect
                                                                                              * 0 : disable the corresponding FSS detect/slope detect
                                                                                              * each bit is corresponding to fss_detect_out
                                                                                              */
#define VSFC_FSS_DETECT_EN(x)                       ((u32)(((x) & 0x000001FF) << 16))
#define VSFC_GET_FSS_DETECT_EN(x)                   ((u32)(((x >> 16) & 0x000001FF)))
#define VSFC_MASK_SHFT_ITVL                         ((u32)0x0000000F << 4)                   /*!< R/W   0x0
                                                                                              * Indicate that each bit of shift patterns programmed in lowf/recv1/recv2 will be extened to shft_itvl+1 cycles
                                                                                              * This field is only valid at analog divide mode (div_sel = 1)
                                                                                              */
#define VSFC_SHFT_ITVL(x)                           ((u32)(((x) & 0x0000000F) << 4))
#define VSFC_GET_SHFT_ITVL(x)                       ((u32)(((x >> 4) & 0x0000000F)))
#define VSFC_BIT_FORCE_DIV                          ((u32)0x00000001 << 0)                   /*!< R/W   0x0
                                                                                              * when set 1, VSFC enters FDIV state and shift lowf pattern to achieve frequency division until force_div is clear
                                                                                              * Once force_div is clear, VSFC will process recovery flow
                                                                                              */
/** @} */


/** @defgroup REG_VSFC_STATUS
 * @brief
 * @{
 */
#define VSFC_BIT_NORMAL_ST                          ((u32)0x00000001 << 1)                   /*!< R     0x0
                                                                                              * Indicate VSFC at Normal Active State
                                                                                              * When set glb_en = 1, VSFC will enter NORMAL state and monitor any triggers user set to check.
                                                                                              * Wehn VSFC had finished low frequency and recovery process, VSFC will return to NORMAL state unless glb_en had been clear.
                                                                                              */
#define VSFC_BIT_IDLE_ST                            ((u32)0x00000001 << 0)                   /*!< R     0x1
                                                                                              * Indicate vfsc no action
                                                                                              * (set global enable=0 --> wait idle --> idle status=1)
                                                                                              */
/** @} */


/** @defgroup REG_VSFC_TRIG_CTRL
 * @brief
 * @{
 */
#define VSFC_BIT_CORE_IDLE_DIV_EN                   ((u32)0x00000001 << 16)                  /*!< R/W   0x0 Enable clock dividing at all core idle event */
#define VSFC_MASK_CORE_MIN                          ((u32)0x0000000F << 8)                   /*!< R/W   0x0
                                                                                              * minimum core numbers to trigger VSFC
                                                                                              * Valid range: 0~7
                                                                                              * Stands for 1 core ~ 8 cores
                                                                                              * (Only apply to internal signals)
                                                                                              */
#define VSFC_CORE_MIN(x)                            ((u32)(((x) & 0x0000000F) << 8))
#define VSFC_GET_CORE_MIN(x)                        ((u32)(((x >> 8) & 0x0000000F)))
#define VSFC_MASK_FSS_MIN                           ((u32)0x0000000F << 4)                   /*!< R/W   0x0
                                                                                              * minimum FSS trigger numbers to trigger VSFC
                                                                                              * Valid range: 0~8
                                                                                              * (Stands for 1 FSS signal ~ 9 FSS signals)
                                                                                              */
#define VSFC_FSS_MIN(x)                             ((u32)(((x) & 0x0000000F) << 4))
#define VSFC_GET_FSS_MIN(x)                         ((u32)(((x >> 4) & 0x0000000F)))
/** @} */


/** @defgroup REG_VSFC_RECV_CTRL0
 * @brief
 * @{
 */
#define VSFC_MASK_DET_RECV_SEL                      ((u32)0x00000003 << 0)                   /*!< R/W   0x1
                                                                                              * Recovery signal selection for FSS detectors.
                                                                                              * bit[0] : use lowf cycle counter
                                                                                              * bit[1] : use trigger signals (FSS go low from high)
                                                                                              * When set as 2'b11, VSFC waits the latest signals to enter recovery.
                                                                                              * (Note 1: 2'b00 is an invalid setting.
                                                                                              * Note 2: Only takes effect when trig_sel[1] or trig_sel[2] = 1.)
                                                                                              */
#define VSFC_DET_RECV_SEL(x)                        ((u32)(((x) & 0x00000003) << 0))
#define VSFC_GET_DET_RECV_SEL(x)                    ((u32)(((x >> 0) & 0x00000003)))
/** @} */


/** @defgroup REG_VSFC_RECV_CTRL1
 * @brief
 * @{
 */
#define VSFC_MASK_LOWF                              ((u32)0x0000FFFF << 16)                  /*!< R/W   0x0 16-bit values used to do frequency modification when in <lowf> state */
#define VSFC_LOWF(x)                                ((u32)(((x) & 0x0000FFFF) << 16))
#define VSFC_GET_LOWF(x)                            ((u32)(((x >> 16) & 0x0000FFFF)))
#define VSFC_MASK_CYCLE_LOWF                        ((u32)0x0000FFFF << 0)                   /*!< R/W   0x10
                                                                                              * Define the cycles for which VSFC stays in <lowf> state.
                                                                                              * Note 1 : must >= 16
                                                                                              * Note 2 : When in <lowf> state, VSFC internal counter will count to cycle_lowf+1 and then jump to <recv1> state.
                                                                                              */
#define VSFC_CYCLE_LOWF(x)                          ((u32)(((x) & 0x0000FFFF) << 0))
#define VSFC_GET_CYCLE_LOWF(x)                      ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_VSFC_RECV_CTRL2
 * @brief
 * @{
 */
#define VSFC_MASK_RECV1                             ((u32)0x0000FFFF << 16)                  /*!< R/W   0x0
                                                                                              * 16-bit values used to do frequency modification when in <recv1> state
                                                                                              * also used in s/w recovery mode to gradually increase frequency.
                                                                                              * See section 3.4.3 and 3.5.2 for more details.
                                                                                              * Note: When select digitally mask pulse (div_sel=0), must avoid setting this register to all 1s, that will result in no clocks to CPU.
                                                                                              */
#define VSFC_RECV1(x)                               ((u32)(((x) & 0x0000FFFF) << 16))
#define VSFC_GET_RECV1(x)                           ((u32)(((x >> 16) & 0x0000FFFF)))
#define VSFC_MASK_CYCLE_RECV1                       ((u32)0x0000FFFF << 0)                   /*!< R/W   0x10
                                                                                              * Define the cycles for which VSFC stays in <recv1> state.
                                                                                              * Note 1 :must >= 16
                                                                                              * Note 2 : VSFC will be in <recv1> state for cycle_recv1 + 1 cycles.
                                                                                              */
#define VSFC_CYCLE_RECV1(x)                         ((u32)(((x) & 0x0000FFFF) << 0))
#define VSFC_GET_CYCLE_RECV1(x)                     ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_VSFC_RECV_CTRL3
 * @brief
 * @{
 */
#define VSFC_MASK_RECV2                             ((u32)0x0000FFFF << 16)                  /*!< R/W   0x0
                                                                                              * 16-bit values used to do frequency modification when in <recv2> state
                                                                                              * Note: When select digitally mask pulse (div_sel=0), must avoid setting this register to all 1s, that will result in no clocks to CPU.
                                                                                              */
#define VSFC_RECV2(x)                               ((u32)(((x) & 0x0000FFFF) << 16))
#define VSFC_GET_RECV2(x)                           ((u32)(((x >> 16) & 0x0000FFFF)))
#define VSFC_MASK_CYCLE_RECV2                       ((u32)0x0000FFFF << 0)                   /*!< R/W   0x10
                                                                                              * Define the cycles for which VSFC stays in <recv2> state.
                                                                                              * Note 1 :must >= 16
                                                                                              * Note 2 : VSFC will be in <recv2> state for cycle_recv2 + 1 cycles.
                                                                                              */
#define VSFC_CYCLE_RECV2(x)                         ((u32)(((x) & 0x0000FFFF) << 0))
#define VSFC_GET_CYCLE_RECV2(x)                     ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_VSFC_RECV_CTRL4
 * @brief
 * @{
 */
#define VSFC_MASK_RECV1_SW                          ((u32)0x0000FFFF << 0)                   /*!< R/W   0x0
                                                                                              * Pre-set value used in sw recovery mode.
                                                                                              * 16-bit values used to do frequency modification when in <recv1> state of sw recovery mode
                                                                                              * See section 3.4.3 and 3.5.2 for more details.
                                                                                              * Note: When select digitally mask pulse (div_sel=0), must avoid setting this register to all 1s, that will result in no clocks to CPU.
                                                                                              */
#define VSFC_RECV1_SW(x)                            ((u32)(((x) & 0x0000FFFF) << 0))
#define VSFC_GET_RECV1_SW(x)                        ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_VSFC_RECV_CTRL5
 * @brief
 * @{
 */
#define VSFC_MASK_RECV_CYCLE_CNT                    ((u32)0x0000FFFF << 16)                  /*!< R     0x0
                                                                                              * Recovery cycle counter.
                                                                                              * Start count when entering recovery.
                                                                                              */
#define VSFC_RECV_CYCLE_CNT(x)                      ((u32)(((x) & 0x0000FFFF) << 16))
#define VSFC_GET_RECV_CYCLE_CNT(x)                  ((u32)(((x >> 16) & 0x0000FFFF)))
#define VSFC_BIT_SW_RECV_EN                         ((u32)0x00000001 << 2)                   /*!< R/W   0x0
                                                                                              * Software recovery mode enable.
                                                                                              * Will issue interrupt when entering recovery.
                                                                                              */
#define VSFC_BIT_RECV_ST                            ((u32)0x00000001 << 1)                   /*!< R     0x0
                                                                                              * Indicator for recovery mode.
                                                                                              * 0: not in recovery mode
                                                                                              * 1: in recovery mode
                                                                                              */
#define VSFC_BIT_SW_RECV_FINISH                     ((u32)0x00000001 << 0)                   /*!< R/W1P 0x0
                                                                                              * S/W writes 1'b1 to inform H/W that s/w recovery process has finished.
                                                                                              * H/W auto clear to 1'b0 after S/W writes 1'b1 to it.
                                                                                              */
/** @} */


/** @defgroup REG_VSFC_PROF_CTRL0
 * @brief
 * @{
 */
#define VSFC_BIT_PROF_EN                            ((u32)0x00000001 << 31)                  /*!< R/W   0x0
                                                                                              * Profiling enable
                                                                                              * H/W will clear when profiling done.
                                                                                              * Program prof_en from 0 to 1 clears all profiling status registers below.
                                                                                              */
#define VSFC_BIT_PROF_FNSH_SEL                      ((u32)0x00000001 << 30)                  /*!< R/W   0x0
                                                                                              * Profiling finish condition selection
                                                                                              * 0 : By profiling cycles
                                                                                              * 1 : S/W disable by set prof_en=0
                                                                                              */
#define VSFC_MASK_PROF_WND_CNT                      ((u32)0x000FFFFF << 0)                   /*!< R/W   0x0
                                                                                              * Profiling window count
                                                                                              * Start from (profiling enable && global enable)=1
                                                                                              * wrt internal 24-bit profiling window counter
                                                                                              * Total profiling cycle is (prof_wnd_cnt*2^24)-1
                                                                                              */
#define VSFC_PROF_WND_CNT(x)                        ((u32)(((x) & 0x000FFFFF) << 0))
#define VSFC_GET_PROF_WND_CNT(x)                    ((u32)(((x >> 0) & 0x000FFFFF)))
/** @} */


/** @defgroup REG_VSFC_PROF_STS0
 * @brief
 * @{
 */
#define VSFC_MASK_SLOPE_TRIG_CNT                    ((u32)0x0000FFFF << 16)                  /*!< R     0x0
                                                                                              * FSS slope detect event counter
                                                                                              * Count FSS slope detect triggers within a profiling test
                                                                                              */
#define VSFC_SLOPE_TRIG_CNT(x)                      ((u32)(((x) & 0x0000FFFF) << 16))
#define VSFC_GET_SLOPE_TRIG_CNT(x)                  ((u32)(((x >> 16) & 0x0000FFFF)))
#define VSFC_MASK_TRIG_CNT                          ((u32)0x0000FFFF << 0)                   /*!< R     0x0
                                                                                              * Trigger event counter
                                                                                              * Count all triggers happen in different cycles.
                                                                                              */
#define VSFC_TRIG_CNT(x)                            ((u32)(((x) & 0x0000FFFF) << 0))
#define VSFC_GET_TRIG_CNT(x)                        ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_VSFC_PROF_STS1
 * @brief
 * @{
 */
#define VSFC_MASK_WAKEUP_CNT                        ((u32)0x0000FFFF << 16)                  /*!< R     0x0 Cpu wakeup event counter */
#define VSFC_WAKEUP_CNT(x)                          ((u32)(((x) & 0x0000FFFF) << 16))
#define VSFC_GET_WAKEUP_CNT(x)                      ((u32)(((x >> 16) & 0x0000FFFF)))
#define VSFC_MASK_ALL_IDLE_CNT                      ((u32)0x0000FFFF << 0)                   /*!< R     0x0 Cpu all cores idle event counter */
#define VSFC_ALL_IDLE_CNT(x)                        ((u32)(((x) & 0x0000FFFF) << 0))
#define VSFC_GET_ALL_IDLE_CNT(x)                    ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_VSFC_PROF_STS2
 * @brief
 * @{
 */
#define VSFC_MASK_VOLT_2ND_DROOP_CNT                ((u32)0x0000FFFF << 0)                   /*!< R     0x0
                                                                                              * Voltage 2nd droop event counter
                                                                                              * Only counts for FSS trigger or FSS slope trigger events happen in recovery state.
                                                                                              */
#define VSFC_VOLT_2ND_DROOP_CNT(x)                  ((u32)(((x) & 0x0000FFFF) << 0))
#define VSFC_GET_VOLT_2ND_DROOP_CNT(x)              ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_VSFC_PROF_STS3
 * @brief
 * @{
 */
#define VSFC_MASK_DIV_CYCLE_CNT0                    ((u32)0xFFFFFFFF << 0)                   /*!< R     0x0
                                                                                              * VSFC Divide Cycle Count 0
                                                                                              * Lower 32 bits of vsfc divide cycle
                                                                                              */
#define VSFC_DIV_CYCLE_CNT0(x)                      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define VSFC_GET_DIV_CYCLE_CNT0(x)                  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_VSFC_PROF_STS4
 * @brief
 * @{
 */
#define VSFC_MASK_DIV_CYCLE_CNT1                    ((u32)0x00000FFF << 0)                   /*!< R     0x0
                                                                                              * VSFC Divide Cycle Count 1
                                                                                              * upper 12 bits of vsfc divide cycle
                                                                                              */
#define VSFC_DIV_CYCLE_CNT1(x)                      ((u32)(((x) & 0x00000FFF) << 0))
#define VSFC_GET_DIV_CYCLE_CNT1(x)                  ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup REG_VSFC_PROF_STS5
 * @brief
 * @{
 */
#define VSFC_MASK_FSS_MIN_DURATION                  ((u32)0x0000FFFF << 16)                  /*!< R     0xffff
                                                                                              * FSS detect minimum event duration
                                                                                              * (Not for cpu internal signals)
                                                                                              */
#define VSFC_FSS_MIN_DURATION(x)                    ((u32)(((x) & 0x0000FFFF) << 16))
#define VSFC_GET_FSS_MIN_DURATION(x)                ((u32)(((x >> 16) & 0x0000FFFF)))
#define VSFC_MASK_FSS_MAX_DURATION                  ((u32)0x0000FFFF << 0)                   /*!< R     0x0
                                                                                              * FSS detect maximum event duration
                                                                                              * (Not for cpu internal signals)
                                                                                              */
#define VSFC_FSS_MAX_DURATION(x)                    ((u32)(((x) & 0x0000FFFF) << 0))
#define VSFC_GET_FSS_MAX_DURATION(x)                ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_VSFC_PROF_STS6
 * @brief
 * @{
 */
#define VSFC_MASK_FSS_SLOPE_MIN_DURATION            ((u32)0x0000FFFF << 16)                  /*!< R     0xffff FSS slope detect minimum event duration */
#define VSFC_FSS_SLOPE_MIN_DURATION(x)              ((u32)(((x) & 0x0000FFFF) << 16))
#define VSFC_GET_FSS_SLOPE_MIN_DURATION(x)          ((u32)(((x >> 16) & 0x0000FFFF)))
#define VSFC_MASK_FSS_SLOPE_MAX_DURATION            ((u32)0x0000FFFF << 0)                   /*!< R     0x0 FSS slope detect maximum event duration */
#define VSFC_FSS_SLOPE_MAX_DURATION(x)              ((u32)(((x) & 0x0000FFFF) << 0))
#define VSFC_GET_FSS_SLOPE_MAX_DURATION(x)          ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_VSFC_STS_OVFL
 * @brief
 * @{
 */
#define VSFC_BIT_OVFL_DURATION                      ((u32)0x00000001 << 4)                   /*!< R     0x0
                                                                                              * Overflow of event duration
                                                                                              * Sticky overflow
                                                                                              */
#define VSFC_BIT_OVFL_VOLT_2ND_DROOP                ((u32)0x00000001 << 3)                   /*!< R     0x0
                                                                                              * Overflow of voltage 2nd droop event
                                                                                              * Sticky overflow
                                                                                              */
#define VSFC_BIT_OVFL_ALL_IDLE                      ((u32)0x00000001 << 2)                   /*!< R     0x0 Overflow of cpu all core idle event */
#define VSFC_BIT_OVFL_WAKEUP                        ((u32)0x00000001 << 1)                   /*!< R     0x0
                                                                                              * Overflow of cpu wakeup event
                                                                                              * Sticky overflow
                                                                                              */
#define VSFC_BIT_OVFL_TRIG                          ((u32)0x00000001 << 0)                   /*!< R     0x0
                                                                                              * Overflow of trigger event
                                                                                              * Sticky overflow
                                                                                              */
/** @} */


/** @defgroup REG_VSFC_PROF_INTST
 * @brief
 * @{
 */
#define VSFC_BIT_SW_RECV_INTST                      ((u32)0x00000001 << 2)                   /*!< R/W1C 0x0
                                                                                              * Recovery interrupt status.
                                                                                              * S/W need to clear it when entering ISR.
                                                                                              */
#define VSFC_BIT_PROF_INTST                         ((u32)0x00000001 << 1)                   /*!< R/W1C 0x0
                                                                                              * Profiling interrupt status
                                                                                              * S/W need to clear it when entering ISR
                                                                                              */
/** @} */


/** @defgroup REG_VSFC_INTEN
 * @brief
 * @{
 */
#define VSFC_BIT_SW_RECV_INTEN                      ((u32)0x00000001 << 2)                   /*!< R/W   0x0
                                                                                              * SW recovery mode Interrupt enable
                                                                                              * When it is enabled, VSFC will generate an interrupt once sw_recv_intst=1.
                                                                                              */
#define VSFC_BIT_PROF_INTEN                         ((u32)0x00000001 << 1)                   /*!< R/W   0x0
                                                                                              * Profiling interrupt enable
                                                                                              * (Only when prof_fnsh_sel = 0)
                                                                                              * When it is enabled, VSFC will generate an interrupt once prof_recv_intst=1.
                                                                                              */
/** @} */


/** @defgroup REG_VSFC_MISC
 * @brief
 * @{
 */
#define VSFC_MASK_CPUPLL_CLK_SEL                    ((u32)0x00000003 << 0)                   /*!< R/W   0x0
                                                                                              * 0: div1
                                                                                              * 1: div1p125
                                                                                              * 2: div1p25
                                                                                              * 3: div1p5
                                                                                              */
#define VSFC_CPUPLL_CLK_SEL(x)                      ((u32)(((x) & 0x00000003) << 0))
#define VSFC_GET_CPUPLL_CLK_SEL(x)                  ((u32)(((x >> 0) & 0x00000003)))
/** @} */




/** @defgroup VSFC_Exported_Types VSFC Exported Types
* @{
*/
/**
* @brief VSFC Register Declaration
*/

typedef struct {
	__IO uint32_t VSFC_CTRL0;                                   /*!< Register,  Address offset:0x0000 */
	__IO uint32_t VSFC_CTRL1;                                   /*!< Register,  Address offset:0x0004 */
	__I  uint32_t VSFC_STATUS;                                  /*!< Register,  Address offset:0x0008 */
	__IO uint32_t VSFC_TRIG_CTRL;                               /*!< Register,  Address offset:0x000C */
	__IO uint32_t VSFC_RECV_CTRL0;                              /*!< Register,  Address offset:0x0010 */
	__IO uint32_t VSFC_RECV_CTRL1;                              /*!< Register,  Address offset:0x0014 */
	__IO uint32_t VSFC_RECV_CTRL2;                              /*!< Register,  Address offset:0x0018 */
	__IO uint32_t VSFC_RECV_CTRL3;                              /*!< Register,  Address offset:0x001C */
	__IO uint32_t VSFC_RECV_CTRL4;                              /*!< Register,  Address offset:0x0020 */
	__IO uint32_t VSFC_RECV_CTRL5;                              /*!< Register,  Address offset:0x0024 */
	__IO uint32_t VSFC_PROF_CTRL0;                              /*!< Register,  Address offset:0x0028 */
	__I  uint32_t VSFC_PROF_STS0;                               /*!< Register,  Address offset:0x002C */
	__I  uint32_t VSFC_PROF_STS1;                               /*!< Register,  Address offset:0x0030 */
	__I  uint32_t VSFC_PROF_STS2;                               /*!< Register,  Address offset:0x0034 */
	__I  uint32_t VSFC_PROF_STS3;                               /*!< Register,  Address offset:0x0038 */
	__I  uint32_t VSFC_PROF_STS4;                               /*!< Register,  Address offset:0x003C */
	__I  uint32_t VSFC_PROF_STS5;                               /*!< Register,  Address offset:0x0040 */
	__I  uint32_t VSFC_PROF_STS6;                               /*!< Register,  Address offset:0x0044 */
	__I  uint32_t VSFC_STS_OVFL;                                /*!< Register,  Address offset:0x0048 */
	__IO uint32_t VSFC_PROF_INTST;                              /*!< Register,  Address offset:0x004C */
	__IO uint32_t VSFC_INTEN;                                   /*!< Register,  Address offset:0x0050 */
	__IO uint32_t VSFC_MISC;                                    /*!< Register,  Address offset:0x0054 */
} VSFC_TypeDef;

#define VSFC_BASE ((VSFC_TypeDef *) (0x60480000))
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
#endif  // SYSREG_CA32_VSFC_H
