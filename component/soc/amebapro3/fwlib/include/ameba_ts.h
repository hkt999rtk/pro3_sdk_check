/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AMEBA_TS_H
#define AMEBA_TS_H

/* AUTO_GEN_START */

/* TILESCALER_REGIF is a part of TILESCALER */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup TILESCALER_REGIF
* @brief TILESCALER_REGIF driver modules
* @{
*/


/** @defgroup INT_ENB
 * @brief
 * @{
 */
#define TILESCALER_REGIF_BIT_ERRORBUS               ((u32)0x00000001 << 3)                   /*!< R/W1C 0.0
                                                                                              * 1: set when the APB address points to an empty address.  Pslverr is flagged at the same time.
                                                                                              * 0: no interrupt.
                                                                                              * Write 1 to clear. Write 0 has no effect.
                                                                                              */
#define TILESCALER_REGIF_BIT_TILECNTTHRESHOLD       ((u32)0x00000001 << 2)                   /*!< R/W1C 0.0
                                                                                              * 1: set when the tile reaches the threshold number in case of raster output.
                                                                                              * 0: no interrupt.
                                                                                              * Write 1 to clear. Write 0 has no effect.
                                                                                              */
#define TILESCALER_REGIF_BIT_LINECNTTHRESHOLD       ((u32)0x00000001 << 1)                   /*!< R/W1C 0.0
                                                                                              * 1: set when the line reaches the threshold number in case of raster output.
                                                                                              * 0: no interrupt.
                                                                                              * Write 1 to clear. Write 0 has no effect.
                                                                                              */
#define TILESCALER_REGIF_BIT_EOF                    ((u32)0x00000001 << 0)                   /*!< R/W1C 0.0
                                                                                              * 1: Tile Scaler finish processing of 1 frame
                                                                                              * 0: no interrupt.
                                                                                              * Write 1 to clear. Write 0 has no effect.
                                                                                              */
/** @} */


/** @defgroup INT_MASK
 * @brief
 * @{
 */
#define TILESCALER_REGIF_BIT_ERRORBUSMASK           ((u32)0x00000001 << 3)                   /*!< R/W   0.0
                                                                                              * 1: Mask the interrupt.
                                                                                              * 0: Generate the interrupt
                                                                                              */
#define TILESCALER_REGIF_BIT_TILECNTTHRESHOLDMASK   ((u32)0x00000001 << 2)                   /*!< R/W   0.0
                                                                                              * 1: Mask the interrupt.
                                                                                              * 0: Generate the interrupt
                                                                                              */
#define TILESCALER_REGIF_BIT_LINECNTTHRESHOLDMASK   ((u32)0x00000001 << 1)                   /*!< R/W   0.0
                                                                                              * 1: Mask the interrupt.
                                                                                              * 0: Generate the interrupt
                                                                                              */
#define TILESCALER_REGIF_BIT_EOFMASK                ((u32)0x00000001 << 0)                   /*!< R/W   0.0
                                                                                              * 1: Mask the interrupt.
                                                                                              * 0: Generate the interrupt
                                                                                              */
/** @} */


/** @defgroup RESERVE0
 * @brief
 * @{
 */
/** @} */


/** @defgroup OPERATION
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_WRAPNUMBER            ((u32)0x00000FFF << 17)                  /*!< R/W   0.0
                                                                                              * The maximum number of tiles before address wrap takes place.  It is used for handshake between TSC <-> Encoder
                                                                                              * In case of raster output, the handshake is not supported.
                                                                                              * 'd0: 1 tile
                                                                                              * 'd1: 2 tile
                                                                                              */
#define TILESCALER_REGIF_WRAPNUMBER(x)              ((u32)(((x) & 0x00000FFF) << 17))
#define TILESCALER_REGIF_GET_WRAPNUMBER(x)          ((u32)(((x >> 17) & 0x00000FFF)))
#define TILESCALER_REGIF_BIT_WRAPENABLE             ((u32)0x00000001 << 16)                  /*!< R/W   0.0
                                                                                              * Enable the address wrap and the encoder handshake function when tile scaler write data to memory.
                                                                                              * 1: enable
                                                                                              * 0: disable
                                                                                              */
#define TILESCALER_REGIF_BIT_OUTPUT_MODE            ((u32)0x00000001 << 8)                   /*!< R/W   0.0
                                                                                              * 1: Tile order
                                                                                              * 0: Raster order
                                                                                              */
#define TILESCALER_REGIF_BIT_TILESCALE_ENB          ((u32)0x00000001 << 0)                   /*!< R/W   0.0
                                                                                              * Write 1 to enable the scaling operation.  After the operation is done, the bit will be auto cleared to 0.  Read to check the scaler status.
                                                                                              * 1: Scaler is busy
                                                                                              * 0: Scaler is idle
                                                                                              */
/** @} */


/** @defgroup IMAGE_FORMAT
 * @brief
 * @{
 */
#define TILESCALER_REGIF_BIT_CSC_ENABLE             ((u32)0x00000001 << 10)                  /*!< R/W   0.0
                                                                                              * 1: CSC is enable
                                                                                              * 0: CSC is disable, bypass
                                                                                              */
#define TILESCALER_REGIF_BIT_CSC_ORDER              ((u32)0x00000001 << 9)                   /*!< R/W   0.0
                                                                                              * 1: CSC on output side
                                                                                              * 0: CSC on input side
                                                                                              */
#define TILESCALER_REGIF_BIT_SCALE_ORDER            ((u32)0x00000001 << 8)                   /*!< R/W   0.0
                                                                                              * 1: vertical scale first
                                                                                              * 0: horizontal scale first. 
                                                                                              * In tile mode, user needs to program vertical scale first.  If not, the tile mode will not work.
                                                                                              */
#define TILESCALER_REGIF_MASK_IMGOUTFORMAT          ((u32)0x0000000F << 4)                   /*!< R/W   0.0
                                                                                              * 0: Planner
                                                                                              * 4: RGB888
                                                                                              * 5: ARGB8888
                                                                                              * 6: Semi interleave
                                                                                              */
#define TILESCALER_REGIF_IMGOUTFORMAT(x)            ((u32)(((x) & 0x0000000F) << 4))
#define TILESCALER_REGIF_GET_IMGOUTFORMAT(x)        ((u32)(((x >> 4) & 0x0000000F)))
#define TILESCALER_REGIF_MASK_IMGINFORMAT           ((u32)0x0000000F << 0)                   /*!< R/W   0.0
                                                                                              * 0: Planner
                                                                                              * 1: UYVY
                                                                                              * 2: YUYV
                                                                                              * 3: YUV444
                                                                                              * 4: RGB888
                                                                                              * 5: ARGB8888
                                                                                              * 6: Semi interleave
                                                                                              */
#define TILESCALER_REGIF_IMGINFORMAT(x)             ((u32)(((x) & 0x0000000F) << 0))
#define TILESCALER_REGIF_GET_IMGINFORMAT(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup CSC_PAR0
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_CSC_OF0               ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 - 511 (s10.0) */
#define TILESCALER_REGIF_CSC_OF0(x)                 ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_CSC_OF0(x)             ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_CSC_IF0               ((u32)0x000001FF << 0)                   /*!< R/W   0.0 -256 - 255 (s9.0) */
#define TILESCALER_REGIF_CSC_IF0(x)                 ((u32)(((x) & 0x000001FF) << 0))
#define TILESCALER_REGIF_GET_CSC_IF0(x)             ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup CSC_PAR1
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_CSC_OF1               ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 - 511 (s10.0) */
#define TILESCALER_REGIF_CSC_OF1(x)                 ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_CSC_OF1(x)             ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_CSC_IF1               ((u32)0x000001FF << 0)                   /*!< R/W   0.0 -256 - 255 (s9.0) */
#define TILESCALER_REGIF_CSC_IF1(x)                 ((u32)(((x) & 0x000001FF) << 0))
#define TILESCALER_REGIF_GET_CSC_IF1(x)             ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup CSC_PAR2
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_CSC_OF2               ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 - 511 (s10.0) */
#define TILESCALER_REGIF_CSC_OF2(x)                 ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_CSC_OF2(x)             ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_CSC_IF2               ((u32)0x000001FF << 0)                   /*!< R/W   0.0 -256 - 255 (s9.0) */
#define TILESCALER_REGIF_CSC_IF2(x)                 ((u32)(((x) & 0x000001FF) << 0))
#define TILESCALER_REGIF_GET_CSC_IF2(x)             ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/** @defgroup CSC_COEF0
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_CSC_C00               ((u32)0x000007FF << 16)                  /*!< R/W   0.0 -1024 - 1023 (s3.8) */
#define TILESCALER_REGIF_CSC_C00(x)                 ((u32)(((x) & 0x000007FF) << 16))
#define TILESCALER_REGIF_GET_CSC_C00(x)             ((u32)(((x >> 16) & 0x000007FF)))
#define TILESCALER_REGIF_MASK_CSC_C01               ((u32)0x000007FF << 0)                   /*!< R/W   0.0 -1024 - 1023 (s3.8) */
#define TILESCALER_REGIF_CSC_C01(x)                 ((u32)(((x) & 0x000007FF) << 0))
#define TILESCALER_REGIF_GET_CSC_C01(x)             ((u32)(((x >> 0) & 0x000007FF)))
/** @} */


/** @defgroup CSC_COEF1
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_CSC_C02               ((u32)0x000007FF << 16)                  /*!< R/W   0.0 -1024 - 1023 (s3.8) */
#define TILESCALER_REGIF_CSC_C02(x)                 ((u32)(((x) & 0x000007FF) << 16))
#define TILESCALER_REGIF_GET_CSC_C02(x)             ((u32)(((x >> 16) & 0x000007FF)))
#define TILESCALER_REGIF_MASK_CSC_C10               ((u32)0x000007FF << 0)                   /*!< R/W   0.0 -1024 - 1023 (s3.8) */
#define TILESCALER_REGIF_CSC_C10(x)                 ((u32)(((x) & 0x000007FF) << 0))
#define TILESCALER_REGIF_GET_CSC_C10(x)             ((u32)(((x >> 0) & 0x000007FF)))
/** @} */


/** @defgroup CSC_COEF2
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_CSC_C11               ((u32)0x000007FF << 16)                  /*!< R/W   0.0 -1024 - 1023 (s3.8) */
#define TILESCALER_REGIF_CSC_C11(x)                 ((u32)(((x) & 0x000007FF) << 16))
#define TILESCALER_REGIF_GET_CSC_C11(x)             ((u32)(((x >> 16) & 0x000007FF)))
#define TILESCALER_REGIF_MASK_CSC_C12               ((u32)0x000007FF << 0)                   /*!< R/W   0.0 -1024 - 1023 (s3.8) */
#define TILESCALER_REGIF_CSC_C12(x)                 ((u32)(((x) & 0x000007FF) << 0))
#define TILESCALER_REGIF_GET_CSC_C12(x)             ((u32)(((x >> 0) & 0x000007FF)))
/** @} */


/** @defgroup CSC_COEF3
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_CSC_C20               ((u32)0x000007FF << 16)                  /*!< R/W   0.0 -1024 - 1023 (s3.8) */
#define TILESCALER_REGIF_CSC_C20(x)                 ((u32)(((x) & 0x000007FF) << 16))
#define TILESCALER_REGIF_GET_CSC_C20(x)             ((u32)(((x >> 16) & 0x000007FF)))
#define TILESCALER_REGIF_MASK_CSC_C21               ((u32)0x000007FF << 0)                   /*!< R/W   0.0 -1024 - 1023 (s3.8) */
#define TILESCALER_REGIF_CSC_C21(x)                 ((u32)(((x) & 0x000007FF) << 0))
#define TILESCALER_REGIF_GET_CSC_C21(x)             ((u32)(((x >> 0) & 0x000007FF)))
/** @} */


/** @defgroup CSC_COEF4
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_CSC_C22               ((u32)0x000007FF << 0)                   /*!< R/W   0.0 -1024 - 1023 (s3.8) */
#define TILESCALER_REGIF_CSC_C22(x)                 ((u32)(((x) & 0x000007FF) << 0))
#define TILESCALER_REGIF_GET_CSC_C22(x)             ((u32)(((x >> 0) & 0x000007FF)))
/** @} */


/** @defgroup COMPN0_PAR0
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_INHEIGHT           ((u32)0x00000FFF << 16)                  /*!< R/W   0.0
                                                                                              * 0: 1 line
                                                                                              * 0xFFF: 4096 line
                                                                                              */
#define TILESCALER_REGIF_C0_INHEIGHT(x)             ((u32)(((x) & 0x00000FFF) << 16))
#define TILESCALER_REGIF_GET_C0_INHEIGHT(x)         ((u32)(((x >> 16) & 0x00000FFF)))
#define TILESCALER_REGIF_MASK_C0_INWIDTH            ((u32)0x00000FFF << 0)                   /*!< R/W   0.0
                                                                                              * 0: 1 pixel
                                                                                              * 0xFFF: 4096 pixel
                                                                                              */
#define TILESCALER_REGIF_C0_INWIDTH(x)              ((u32)(((x) & 0x00000FFF) << 0))
#define TILESCALER_REGIF_GET_C0_INWIDTH(x)          ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup COMPN0_PAR1
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_OUTHEIGHT          ((u32)0x00000FFF << 16)                  /*!< R/W   0.0
                                                                                              * 0: 1 line
                                                                                              * 0xFFF: 4096 line
                                                                                              */
#define TILESCALER_REGIF_C0_OUTHEIGHT(x)            ((u32)(((x) & 0x00000FFF) << 16))
#define TILESCALER_REGIF_GET_C0_OUTHEIGHT(x)        ((u32)(((x >> 16) & 0x00000FFF)))
#define TILESCALER_REGIF_MASK_C0_OUTWIDTH           ((u32)0x00000FFF << 0)                   /*!< R/W   0.0
                                                                                              * 0: 1 pixel
                                                                                              * 0xFFF: 4096 pixel
                                                                                              */
#define TILESCALER_REGIF_C0_OUTWIDTH(x)             ((u32)(((x) & 0x00000FFF) << 0))
#define TILESCALER_REGIF_GET_C0_OUTWIDTH(x)         ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RESERVE1
 * @brief
 * @{
 */
/** @} */


/** @defgroup COMPN0_LPF0
 * @brief
 * @{
 */
#define TILESCALER_REGIF_BIT_C0_LPF_MODE            ((u32)0x00000001 << 0)                   /*!< R/W   0.0
                                                                                              * 1: enable LPF
                                                                                              * 0: bypass
                                                                                              */
/** @} */


/** @defgroup COMPN0_LPF1
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_LPF_L01            ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_LPF_L01(x)              ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_LPF_L01(x)          ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_LPF_L00            ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_LPF_L00(x)              ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_LPF_L00(x)          ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN0_LPF2
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_LPF_L03            ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_LPF_L03(x)              ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_LPF_L03(x)          ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_LPF_L02            ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_LPF_L02(x)              ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_LPF_L02(x)          ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN0_INT_HOR0
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_HINT_START         ((u32)0x00003FFF << 18)                  /*!< R/W   0.0 5 bit for positive/negative, 9 bit for decimal part (s5.9) */
#define TILESCALER_REGIF_C0_HINT_START(x)           ((u32)(((x) & 0x00003FFF) << 18))
#define TILESCALER_REGIF_GET_C0_HINT_START(x)       ((u32)(((x >> 18) & 0x00003FFF)))
#define TILESCALER_REGIF_MASK_C0_HINT_STEP          ((u32)0x0001FFFF << 1)                   /*!< R/W   0.0 5 bit integer part, 12 bit decimal part (u5.12) */
#define TILESCALER_REGIF_C0_HINT_STEP(x)            ((u32)(((x) & 0x0001FFFF) << 1))
#define TILESCALER_REGIF_GET_C0_HINT_STEP(x)        ((u32)(((x >> 1) & 0x0001FFFF)))
#define TILESCALER_REGIF_BIT_C0_HINT_MODE           ((u32)0x00000001 << 0)                   /*!< R/W   0.0
                                                                                              * 1: enable
                                                                                              * 0: bypass
                                                                                              */
/** @} */


/** @defgroup COMPN0_INT_HOR1
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_HINT00             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_HINT00(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_HINT00(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_HINT01             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_HINT01(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_HINT01(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN0_INT_HOR2
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_HINT02             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_HINT02(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_HINT02(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_HINT03             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_HINT03(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_HINT03(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN0_INT_HOR3
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_HINT10             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_HINT10(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_HINT10(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_HINT11             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_HINT11(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_HINT11(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN0_INT_HOR4
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_HINT12             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_HINT12(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_HINT12(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_HINT13             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_HINT13(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_HINT13(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN0_INT_HOR5
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_HINT20             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_HINT20(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_HINT20(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_HINT21             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_HINT21(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_HINT21(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN0_INT_HOR6
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_HINT22             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_HINT22(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_HINT22(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_HINT23             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_HINT23(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_HINT23(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN0_INT_HOR7
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_HINT30             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_HINT30(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_HINT30(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_HINT31             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_HINT31(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_HINT31(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN0_INT_HOR8
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_HINT32             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_HINT32(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_HINT32(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_HINT33             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_HINT33(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_HINT33(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN0_INT_HOR9
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_HINT40             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_HINT40(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_HINT40(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_HINT41             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_HINT41(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_HINT41(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN0_INT_HOR10
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_HINT42             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_HINT42(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_HINT42(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_HINT43             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_HINT43(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_HINT43(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMP0_INTILEHEIGHT
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_INTILEHEIGHT       ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0.0 Height of input that need to generate output height (C*_VINT_step * OutTileHeight) */
#define TILESCALER_REGIF_C0_INTILEHEIGHT(x)         ((u32)(((x) & 0x1FFFFFFF) << 0))
#define TILESCALER_REGIF_GET_C0_INTILEHEIGHT(x)     ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup COMP0_INTILEWIDTH
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_INTILEWIDTH        ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0.0 Width of input that need to generate output width (C*_HINT_step * OutTileWidth) */
#define TILESCALER_REGIF_C0_INTILEWIDTH(x)          ((u32)(((x) & 0x1FFFFFFF) << 0))
#define TILESCALER_REGIF_GET_C0_INTILEWIDTH(x)      ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup COMPN0_INT_VER0
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_VINT_START         ((u32)0x00003FFF << 18)                  /*!< R/W   0.0 5 bit for positive/negative, 9 bit for decimal part (s5.9) */
#define TILESCALER_REGIF_C0_VINT_START(x)           ((u32)(((x) & 0x00003FFF) << 18))
#define TILESCALER_REGIF_GET_C0_VINT_START(x)       ((u32)(((x >> 18) & 0x00003FFF)))
#define TILESCALER_REGIF_MASK_C0_VINT_STEP          ((u32)0x0001FFFF << 1)                   /*!< R/W   0.0 5 bit integer part, 12 bit decimal part (u5.12) */
#define TILESCALER_REGIF_C0_VINT_STEP(x)            ((u32)(((x) & 0x0001FFFF) << 1))
#define TILESCALER_REGIF_GET_C0_VINT_STEP(x)        ((u32)(((x >> 1) & 0x0001FFFF)))
#define TILESCALER_REGIF_BIT_C0_VINT_MODE           ((u32)0x00000001 << 0)                   /*!< R/W   0.0
                                                                                              * 1: enable
                                                                                              * 0: bypass
                                                                                              */
/** @} */


/** @defgroup COMPN0_INT_VER1
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_VINT00             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_VINT00(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_VINT00(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_VINT01             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_VINT01(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_VINT01(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN0_INT_VER2
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_VINT02             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_VINT02(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_VINT02(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_VINT03             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_VINT03(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_VINT03(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN0_INT_VER3
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_VINT10             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_VINT10(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_VINT10(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_VINT11             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_VINT11(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_VINT11(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN0_INT_VER4
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_VINT12             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_VINT12(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_VINT12(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_VINT13             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_VINT13(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_VINT13(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN0_INT_VER5
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_VINT20             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_VINT20(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_VINT20(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_VINT21             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_VINT21(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_VINT21(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN0_INT_VER6
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_VINT22             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_VINT22(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_VINT22(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_VINT23             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_VINT23(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_VINT23(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN0_INT_VER7
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_VINT30             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_VINT30(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_VINT30(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_VINT31             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_VINT31(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_VINT31(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN0_INT_VER8
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_VINT32             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_VINT32(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_VINT32(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_VINT33             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_VINT33(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_VINT33(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN0_INT_VER9
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_VINT40             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_VINT40(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_VINT40(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_VINT41             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_VINT41(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_VINT41(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN0_INT_VER10
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_VINT42             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_VINT42(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C0_VINT42(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C0_VINT43             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C0_VINT43(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C0_VINT43(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN0_INT_IN_ADDR
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_INADDR             ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 None */
#define TILESCALER_REGIF_C0_INADDR(x)               ((u32)(((x) & 0xFFFFFFFF) << 0))
#define TILESCALER_REGIF_GET_C0_INADDR(x)           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup COMPN0_INT_OUT_ADDR
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_OUTADDR            ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 None */
#define TILESCALER_REGIF_C0_OUTADDR(x)              ((u32)(((x) & 0xFFFFFFFF) << 0))
#define TILESCALER_REGIF_GET_C0_OUTADDR(x)          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup COMPN0_INT_STRIDE
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_OUTSTRIDE          ((u32)0x0000FFFF << 16)                  /*!< R/W   0.0 Unit in bytes.  The value can be either positive or negative.  This field is the 2s complement representation, in format of s16.0 */
#define TILESCALER_REGIF_C0_OUTSTRIDE(x)            ((u32)(((x) & 0x0000FFFF) << 16))
#define TILESCALER_REGIF_GET_C0_OUTSTRIDE(x)        ((u32)(((x >> 16) & 0x0000FFFF)))
#define TILESCALER_REGIF_MASK_C0_INSTRIDE           ((u32)0x0000FFFF << 0)                   /*!< R/W   0.0 Unit in bytes.  The value can be either positive or negative.  This field is the 2s complement representation, in format of s16.0 */
#define TILESCALER_REGIF_C0_INSTRIDE(x)             ((u32)(((x) & 0x0000FFFF) << 0))
#define TILESCALER_REGIF_GET_C0_INSTRIDE(x)         ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup COMPN0_INT_OUT_TILE_SIZE
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_OUTTILEHEIGHT      ((u32)0x00000003 << 16)                  /*!< R/W   0.0
                                                                                              * 0: 8 line
                                                                                              * 1: 16 line
                                                                                              * 2: 32 line
                                                                                              * 3: 64 line
                                                                                              * Currently, component 0 only supports 16 and 64 lines.
                                                                                              */
#define TILESCALER_REGIF_C0_OUTTILEHEIGHT(x)        ((u32)(((x) & 0x00000003) << 16))
#define TILESCALER_REGIF_GET_C0_OUTTILEHEIGHT(x)    ((u32)(((x >> 16) & 0x00000003)))
#define TILESCALER_REGIF_MASK_C0_OUTTILEWIDTH       ((u32)0x00000FFF << 0)                   /*!< R/W   0.0
                                                                                              * 0: 1 pixel
                                                                                              * 0xFFF: 4096 pixel
                                                                                              */
#define TILESCALER_REGIF_C0_OUTTILEWIDTH(x)         ((u32)(((x) & 0x00000FFF) << 0))
#define TILESCALER_REGIF_GET_C0_OUTTILEWIDTH(x)     ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup COMPN0_WRAP_STRIDE
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C0_WRAPSTRIDE         ((u32)0x0000FFFF << 0)                   /*!< R/W   0.0 Unit in bytes.  The value can be either positive or negative.  This field is the 2s complement representation, in format of s16.0 */
#define TILESCALER_REGIF_C0_WRAPSTRIDE(x)           ((u32)(((x) & 0x0000FFFF) << 0))
#define TILESCALER_REGIF_GET_C0_WRAPSTRIDE(x)       ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup RESERVE5
 * @brief
 * @{
 */
/** @} */


/** @defgroup COMPN1_PAR0
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_INHEIGHT           ((u32)0x00000FFF << 16)                  /*!< R/W   0.0
                                                                                              * 0: 1 line
                                                                                              * 0xFFF: 4096 line
                                                                                              */
#define TILESCALER_REGIF_C1_INHEIGHT(x)             ((u32)(((x) & 0x00000FFF) << 16))
#define TILESCALER_REGIF_GET_C1_INHEIGHT(x)         ((u32)(((x >> 16) & 0x00000FFF)))
#define TILESCALER_REGIF_MASK_C1_INWIDTH            ((u32)0x00000FFF << 0)                   /*!< R/W   0.0
                                                                                              * 0: 1 pixel
                                                                                              * 0xFFF: 4096 pixel
                                                                                              */
#define TILESCALER_REGIF_C1_INWIDTH(x)              ((u32)(((x) & 0x00000FFF) << 0))
#define TILESCALER_REGIF_GET_C1_INWIDTH(x)          ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup COMPN1_PAR1
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_OUTHEIGHT          ((u32)0x00000FFF << 16)                  /*!< R/W   0.0
                                                                                              * 0: 1 line
                                                                                              * 0xFFF: 4096 line
                                                                                              */
#define TILESCALER_REGIF_C1_OUTHEIGHT(x)            ((u32)(((x) & 0x00000FFF) << 16))
#define TILESCALER_REGIF_GET_C1_OUTHEIGHT(x)        ((u32)(((x >> 16) & 0x00000FFF)))
#define TILESCALER_REGIF_MASK_C1_OUTWIDTH           ((u32)0x00000FFF << 0)                   /*!< R/W   0.0
                                                                                              * 0: 1 pixel
                                                                                              * 0xFFF: 4096 pixel
                                                                                              */
#define TILESCALER_REGIF_C1_OUTWIDTH(x)             ((u32)(((x) & 0x00000FFF) << 0))
#define TILESCALER_REGIF_GET_C1_OUTWIDTH(x)         ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RESERVE6
 * @brief
 * @{
 */
/** @} */


/** @defgroup COMPN1_LPF0
 * @brief
 * @{
 */
#define TILESCALER_REGIF_BIT_C1_LPF_MODE            ((u32)0x00000001 << 0)                   /*!< R/W   0.0
                                                                                              * 1: enable LPF
                                                                                              * 0: bypass
                                                                                              */
/** @} */


/** @defgroup COMPN1_LPF1
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_LPF_L01            ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_LPF_L01(x)              ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_LPF_L01(x)          ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_LPF_L00            ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_LPF_L00(x)              ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_LPF_L00(x)          ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN1_LPF2
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_LPF_L03            ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_LPF_L03(x)              ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_LPF_L03(x)          ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_LPF_L02            ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_LPF_L02(x)              ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_LPF_L02(x)          ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN1_INT_HOR0
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_HINT_START         ((u32)0x00003FFF << 18)                  /*!< R/W   0.0 5 bit for positive/negative, 9 bit for decimal part (s5.9) */
#define TILESCALER_REGIF_C1_HINT_START(x)           ((u32)(((x) & 0x00003FFF) << 18))
#define TILESCALER_REGIF_GET_C1_HINT_START(x)       ((u32)(((x >> 18) & 0x00003FFF)))
#define TILESCALER_REGIF_MASK_C1_HINT_STEP          ((u32)0x0001FFFF << 1)                   /*!< R/W   0.0 5 bit integer part, 12 bit decimal part (u5.12) */
#define TILESCALER_REGIF_C1_HINT_STEP(x)            ((u32)(((x) & 0x0001FFFF) << 1))
#define TILESCALER_REGIF_GET_C1_HINT_STEP(x)        ((u32)(((x >> 1) & 0x0001FFFF)))
#define TILESCALER_REGIF_BIT_C1_HINT_MODE           ((u32)0x00000001 << 0)                   /*!< R/W   0.0
                                                                                              * 1: enable
                                                                                              * 0: bypass
                                                                                              */
/** @} */


/** @defgroup COMPN1_INT_HOR1
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_HINT00             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_HINT00(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_HINT00(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_HINT01             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_HINT01(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_HINT01(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN1_INT_HOR2
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_HINT02             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_HINT02(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_HINT02(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_HINT03             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_HINT03(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_HINT03(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN1_INT_HOR3
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_HINT10             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_HINT10(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_HINT10(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_HINT11             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_HINT11(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_HINT11(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN1_INT_HOR4
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_HINT12             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_HINT12(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_HINT12(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_HINT13             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_HINT13(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_HINT13(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN1_INT_HOR5
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_HINT20             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_HINT20(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_HINT20(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_HINT21             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_HINT21(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_HINT21(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN1_INT_HOR6
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_HINT22             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_HINT22(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_HINT22(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_HINT23             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_HINT23(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_HINT23(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN1_INT_HOR7
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_HINT30             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_HINT30(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_HINT30(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_HINT31             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_HINT31(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_HINT31(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN1_INT_HOR8
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_HINT32             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_HINT32(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_HINT32(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_HINT33             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_HINT33(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_HINT33(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN1_INT_HOR9
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_HINT40             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_HINT40(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_HINT40(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_HINT41             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_HINT41(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_HINT41(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN1_INT_HOR10
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_HINT42             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_HINT42(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_HINT42(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_HINT43             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_HINT43(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_HINT43(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMP1_INTILEHEIGHT
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_INTILEHEIGHT       ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0.0 Height of input that need to generate output height (C*_VINT_step * OutTileHeight) */
#define TILESCALER_REGIF_C1_INTILEHEIGHT(x)         ((u32)(((x) & 0x1FFFFFFF) << 0))
#define TILESCALER_REGIF_GET_C1_INTILEHEIGHT(x)     ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup COMP1_INTILEWIDTH
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_INTILEWIDTH        ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0.0 Width of input that need to generate output width (C*_HINT_step * OutTileWidth) */
#define TILESCALER_REGIF_C1_INTILEWIDTH(x)          ((u32)(((x) & 0x1FFFFFFF) << 0))
#define TILESCALER_REGIF_GET_C1_INTILEWIDTH(x)      ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup COMPN1_INT_VER0
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_VINT_START         ((u32)0x00003FFF << 18)                  /*!< R/W   0.0 5 bit for positive/negative, 9 bit for decimal part (s5.9) */
#define TILESCALER_REGIF_C1_VINT_START(x)           ((u32)(((x) & 0x00003FFF) << 18))
#define TILESCALER_REGIF_GET_C1_VINT_START(x)       ((u32)(((x >> 18) & 0x00003FFF)))
#define TILESCALER_REGIF_MASK_C1_VINT_STEP          ((u32)0x0001FFFF << 1)                   /*!< R/W   0.0 5 bit integer part, 12 bit decimal part (u5.12) */
#define TILESCALER_REGIF_C1_VINT_STEP(x)            ((u32)(((x) & 0x0001FFFF) << 1))
#define TILESCALER_REGIF_GET_C1_VINT_STEP(x)        ((u32)(((x >> 1) & 0x0001FFFF)))
#define TILESCALER_REGIF_BIT_C1_VINT_MODE           ((u32)0x00000001 << 0)                   /*!< R/W   0.0
                                                                                              * 1: enable
                                                                                              * 0: bypass
                                                                                              */
/** @} */


/** @defgroup COMPN1_INT_VER1
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_VINT00             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_VINT00(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_VINT00(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_VINT01             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_VINT01(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_VINT01(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN1_INT_VER2
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_VINT02             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_VINT02(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_VINT02(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_VINT03             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_VINT03(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_VINT03(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN1_INT_VER3
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_VINT10             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_VINT10(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_VINT10(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_VINT11             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_VINT11(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_VINT11(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN1_INT_VER4
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_VINT12             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_VINT12(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_VINT12(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_VINT13             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_VINT13(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_VINT13(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN1_INT_VER5
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_VINT20             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_VINT20(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_VINT20(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_VINT21             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_VINT21(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_VINT21(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN1_INT_VER6
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_VINT22             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_VINT22(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_VINT22(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_VINT23             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_VINT23(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_VINT23(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN1_INT_VER7
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_VINT30             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_VINT30(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_VINT30(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_VINT31             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_VINT31(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_VINT31(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN1_INT_VER8
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_VINT32             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_VINT32(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_VINT32(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_VINT33             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_VINT33(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_VINT33(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN1_INT_VER9
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_VINT40             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_VINT40(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_VINT40(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_VINT41             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_VINT41(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_VINT41(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN1_INT_VER10
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_VINT42             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_VINT42(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C1_VINT42(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C1_VINT43             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C1_VINT43(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C1_VINT43(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN1_INT_IN_ADDR
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_INADDR             ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 None */
#define TILESCALER_REGIF_C1_INADDR(x)               ((u32)(((x) & 0xFFFFFFFF) << 0))
#define TILESCALER_REGIF_GET_C1_INADDR(x)           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup COMPN1_INT_OUT_ADDR
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_OUTADDR            ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 None */
#define TILESCALER_REGIF_C1_OUTADDR(x)              ((u32)(((x) & 0xFFFFFFFF) << 0))
#define TILESCALER_REGIF_GET_C1_OUTADDR(x)          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup COMPN1_INT_STRIDE
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_OUTSTRIDE          ((u32)0x0000FFFF << 16)                  /*!< R/W   0.0 Unit in bytes.  The value can be either positive or negative.  This field is the 2s complement representation, in format of s16.0 */
#define TILESCALER_REGIF_C1_OUTSTRIDE(x)            ((u32)(((x) & 0x0000FFFF) << 16))
#define TILESCALER_REGIF_GET_C1_OUTSTRIDE(x)        ((u32)(((x >> 16) & 0x0000FFFF)))
#define TILESCALER_REGIF_MASK_C1_INSTRIDE           ((u32)0x0000FFFF << 0)                   /*!< R/W   0.0 Unit in bytes.  The value can be either positive or negative.  This field is the 2s complement representation, in format of s16.0 */
#define TILESCALER_REGIF_C1_INSTRIDE(x)             ((u32)(((x) & 0x0000FFFF) << 0))
#define TILESCALER_REGIF_GET_C1_INSTRIDE(x)         ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup COMPN1_INT_OUT_TILE_SIZE
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_OUTTILEHEIGHT      ((u32)0x00000003 << 16)                  /*!< R/W   0.0
                                                                                              * 0: 8 line
                                                                                              * 1: 16 line
                                                                                              * 2: 32 line
                                                                                              * 3: 64 line
                                                                                              * Currently, component 1 only supports 8 and 32 lines.
                                                                                              */
#define TILESCALER_REGIF_C1_OUTTILEHEIGHT(x)        ((u32)(((x) & 0x00000003) << 16))
#define TILESCALER_REGIF_GET_C1_OUTTILEHEIGHT(x)    ((u32)(((x >> 16) & 0x00000003)))
#define TILESCALER_REGIF_MASK_C1_OUTTILEWIDTH       ((u32)0x00000FFF << 0)                   /*!< R/W   0.0
                                                                                              * 0: 1 pixel
                                                                                              * 0xFFF: 4096 pixel
                                                                                              */
#define TILESCALER_REGIF_C1_OUTTILEWIDTH(x)         ((u32)(((x) & 0x00000FFF) << 0))
#define TILESCALER_REGIF_GET_C1_OUTTILEWIDTH(x)     ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup COMPN1_WRAP_STRIDE
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C1_WRAPSTRIDE         ((u32)0x0000FFFF << 0)                   /*!< R/W   0.0 Unit in bytes.  The value can be either positive or negative.  This field is the 2s complement representation, in format of s16.0 */
#define TILESCALER_REGIF_C1_WRAPSTRIDE(x)           ((u32)(((x) & 0x0000FFFF) << 0))
#define TILESCALER_REGIF_GET_C1_WRAPSTRIDE(x)       ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup RESERVE10
 * @brief
 * @{
 */
/** @} */


/** @defgroup COMPN2_PAR0
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_INHEIGHT           ((u32)0x00000FFF << 16)                  /*!< R/W   0.0
                                                                                              * 0: 1 line
                                                                                              * 0xFFF: 4096 line
                                                                                              */
#define TILESCALER_REGIF_C2_INHEIGHT(x)             ((u32)(((x) & 0x00000FFF) << 16))
#define TILESCALER_REGIF_GET_C2_INHEIGHT(x)         ((u32)(((x >> 16) & 0x00000FFF)))
#define TILESCALER_REGIF_MASK_C2_INWIDTH            ((u32)0x00000FFF << 0)                   /*!< R/W   0.0
                                                                                              * 0: 1 pixel
                                                                                              * 0xFFF: 4096 pixel
                                                                                              */
#define TILESCALER_REGIF_C2_INWIDTH(x)              ((u32)(((x) & 0x00000FFF) << 0))
#define TILESCALER_REGIF_GET_C2_INWIDTH(x)          ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup COMPN2_PAR1
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_OUTHEIGHT          ((u32)0x00000FFF << 16)                  /*!< R/W   0.0
                                                                                              * 0: 1 line
                                                                                              * 0xFFF: 4096 line
                                                                                              */
#define TILESCALER_REGIF_C2_OUTHEIGHT(x)            ((u32)(((x) & 0x00000FFF) << 16))
#define TILESCALER_REGIF_GET_C2_OUTHEIGHT(x)        ((u32)(((x >> 16) & 0x00000FFF)))
#define TILESCALER_REGIF_MASK_C2_OUTWIDTH           ((u32)0x00000FFF << 0)                   /*!< R/W   0.0
                                                                                              * 0: 1 pixel
                                                                                              * 0xFFF: 4096 pixel
                                                                                              */
#define TILESCALER_REGIF_C2_OUTWIDTH(x)             ((u32)(((x) & 0x00000FFF) << 0))
#define TILESCALER_REGIF_GET_C2_OUTWIDTH(x)         ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup RESERVE11
 * @brief
 * @{
 */
/** @} */


/** @defgroup COMPN2_LPF0
 * @brief
 * @{
 */
#define TILESCALER_REGIF_BIT_C2_LPF_MODE            ((u32)0x00000001 << 0)                   /*!< R/W   0.0
                                                                                              * 1: enable LPF
                                                                                              * 0: bypass
                                                                                              */
/** @} */


/** @defgroup COMPN2_LPF1
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_LPF_L01            ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_LPF_L01(x)              ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_LPF_L01(x)          ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_LPF_L00            ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_LPF_L00(x)              ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_LPF_L00(x)          ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN2_LPF2
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_LPF_L03            ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_LPF_L03(x)              ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_LPF_L03(x)          ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_LPF_L02            ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_LPF_L02(x)              ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_LPF_L02(x)          ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN2_INT_HOR0
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_HINT_START         ((u32)0x00003FFF << 18)                  /*!< R/W   0.0 5 bit for positive/negative, 9 bit for decimal part (s5.9) */
#define TILESCALER_REGIF_C2_HINT_START(x)           ((u32)(((x) & 0x00003FFF) << 18))
#define TILESCALER_REGIF_GET_C2_HINT_START(x)       ((u32)(((x >> 18) & 0x00003FFF)))
#define TILESCALER_REGIF_MASK_C2_HINT_STEP          ((u32)0x0001FFFF << 1)                   /*!< R/W   0.0 5 bit integer part, 12 bit decimal part */
#define TILESCALER_REGIF_C2_HINT_STEP(x)            ((u32)(((x) & 0x0001FFFF) << 1))
#define TILESCALER_REGIF_GET_C2_HINT_STEP(x)        ((u32)(((x >> 1) & 0x0001FFFF)))
#define TILESCALER_REGIF_BIT_C2_HINT_MODE           ((u32)0x00000001 << 0)                   /*!< R/W   0.0
                                                                                              * 1: enable
                                                                                              * 0: bypass
                                                                                              */
/** @} */


/** @defgroup COMPN2_INT_HOR1
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_HINT00             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_HINT00(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_HINT00(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_HINT01             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_HINT01(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_HINT01(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN2_INT_HOR2
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_HINT02             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_HINT02(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_HINT02(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_HINT03             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_HINT03(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_HINT03(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN2_INT_HOR3
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_HINT10             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_HINT10(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_HINT10(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_HINT11             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_HINT11(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_HINT11(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN2_INT_HOR4
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_HINT12             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_HINT12(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_HINT12(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_HINT13             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_HINT13(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_HINT13(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN2_INT_HOR5
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_HINT20             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_HINT20(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_HINT20(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_HINT21             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_HINT21(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_HINT21(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN2_INT_HOR6
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_HINT22             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_HINT22(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_HINT22(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_HINT23             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_HINT23(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_HINT23(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN2_INT_HOR7
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_HINT30             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_HINT30(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_HINT30(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_HINT31             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_HINT31(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_HINT31(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN2_INT_HOR8
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_HINT32             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_HINT32(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_HINT32(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_HINT33             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_HINT33(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_HINT33(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN2_INT_HOR9
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_HINT40             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_HINT40(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_HINT40(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_HINT41             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_HINT41(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_HINT41(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN2_INT_HOR10
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_HINT42             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_HINT42(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_HINT42(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_HINT43             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_HINT43(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_HINT43(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMP2_INTILEHEIGHT
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_INTILEHEIGHT       ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0.0 Height of input that need to generate output height (C*_VINT_step * OutTileHeight) */
#define TILESCALER_REGIF_C2_INTILEHEIGHT(x)         ((u32)(((x) & 0x1FFFFFFF) << 0))
#define TILESCALER_REGIF_GET_C2_INTILEHEIGHT(x)     ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup COMP2_INTILEWIDTH
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_INTILEWIDTH        ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0.0 Width of input that need to generate output width (C*_HINT_step * OutTileWidth) */
#define TILESCALER_REGIF_C2_INTILEWIDTH(x)          ((u32)(((x) & 0x1FFFFFFF) << 0))
#define TILESCALER_REGIF_GET_C2_INTILEWIDTH(x)      ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup COMPN2_INT_VER0
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_VINT_START         ((u32)0x00003FFF << 18)                  /*!< R/W   0.0 5 bit for positive/negative, 9 bit for decimal part (s5.9) */
#define TILESCALER_REGIF_C2_VINT_START(x)           ((u32)(((x) & 0x00003FFF) << 18))
#define TILESCALER_REGIF_GET_C2_VINT_START(x)       ((u32)(((x >> 18) & 0x00003FFF)))
#define TILESCALER_REGIF_MASK_C2_VINT_STEP          ((u32)0x0001FFFF << 1)                   /*!< R/W   0.0 5 bit integer part, 12 bit decimal part (u5.12) */
#define TILESCALER_REGIF_C2_VINT_STEP(x)            ((u32)(((x) & 0x0001FFFF) << 1))
#define TILESCALER_REGIF_GET_C2_VINT_STEP(x)        ((u32)(((x >> 1) & 0x0001FFFF)))
#define TILESCALER_REGIF_BIT_C2_VINT_MODE           ((u32)0x00000001 << 0)                   /*!< R/W   0.0
                                                                                              * 1: enable
                                                                                              * 0: bypass
                                                                                              */
/** @} */


/** @defgroup COMPN2_INT_VER1
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_VINT00             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_VINT00(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_VINT00(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_VINT01             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_VINT01(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_VINT01(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN2_INT_VER2
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_VINT02             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_VINT02(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_VINT02(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_VINT03             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_VINT03(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_VINT03(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN2_INT_VER3
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_VINT10             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_VINT10(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_VINT10(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_VINT11             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_VINT11(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_VINT11(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN2_INT_VER4
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_VINT12             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_VINT12(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_VINT12(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_VINT13             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_VINT13(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_VINT13(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN2_INT_VER5
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_VINT20             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_VINT20(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_VINT20(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_VINT21             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_VINT21(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_VINT21(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN2_INT_VER6
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_VINT22             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_VINT22(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_VINT22(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_VINT23             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_VINT23(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_VINT23(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN2_INT_VER7
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_VINT30             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_VINT30(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_VINT30(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_VINT31             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_VINT31(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_VINT31(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN2_INT_VER8
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_VINT32             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_VINT32(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_VINT32(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_VINT33             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_VINT33(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_VINT33(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN2_INT_VER9
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_VINT40             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_VINT40(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_VINT40(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_VINT41             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_VINT41(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_VINT41(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN2_INT_VER10
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_VINT42             ((u32)0x000003FF << 16)                  /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_VINT42(x)               ((u32)(((x) & 0x000003FF) << 16))
#define TILESCALER_REGIF_GET_C2_VINT42(x)           ((u32)(((x >> 16) & 0x000003FF)))
#define TILESCALER_REGIF_MASK_C2_VINT43             ((u32)0x000003FF << 0)                   /*!< R/W   0.0 -512 … 511 (s2.8) */
#define TILESCALER_REGIF_C2_VINT43(x)               ((u32)(((x) & 0x000003FF) << 0))
#define TILESCALER_REGIF_GET_C2_VINT43(x)           ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup COMPN2_INT_IN_ADDR
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_INADDR             ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 None */
#define TILESCALER_REGIF_C2_INADDR(x)               ((u32)(((x) & 0xFFFFFFFF) << 0))
#define TILESCALER_REGIF_GET_C2_INADDR(x)           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup COMPN2_INT_OUT_ADDR
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_OUTADDR            ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 None */
#define TILESCALER_REGIF_C2_OUTADDR(x)              ((u32)(((x) & 0xFFFFFFFF) << 0))
#define TILESCALER_REGIF_GET_C2_OUTADDR(x)          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup COMPN2_INT_STRIDE
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_OUTSTRIDE          ((u32)0x0000FFFF << 16)                  /*!< R/W   0.0 Unit in bytes.  The value can be either positive or negative.  This field is the 2s complement representation, in format of s16.0 */
#define TILESCALER_REGIF_C2_OUTSTRIDE(x)            ((u32)(((x) & 0x0000FFFF) << 16))
#define TILESCALER_REGIF_GET_C2_OUTSTRIDE(x)        ((u32)(((x >> 16) & 0x0000FFFF)))
#define TILESCALER_REGIF_MASK_C2_INSTRIDE           ((u32)0x0000FFFF << 0)                   /*!< R/W   0.0 Unit in bytes.  The value can be either positive or negative.  This field is the 2s complement representation, in format of s16.0 */
#define TILESCALER_REGIF_C2_INSTRIDE(x)             ((u32)(((x) & 0x0000FFFF) << 0))
#define TILESCALER_REGIF_GET_C2_INSTRIDE(x)         ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup COMPN2_INT_OUT_TILE_SIZE
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_OUTTILEHEIGHT      ((u32)0x00000003 << 16)                  /*!< R/W   0.0
                                                                                              * 0: 8 line
                                                                                              * 1: 16 line
                                                                                              * 2: 32 line
                                                                                              * 3: 64 line
                                                                                              * Currently, component 2 is not used.
                                                                                              */
#define TILESCALER_REGIF_C2_OUTTILEHEIGHT(x)        ((u32)(((x) & 0x00000003) << 16))
#define TILESCALER_REGIF_GET_C2_OUTTILEHEIGHT(x)    ((u32)(((x >> 16) & 0x00000003)))
#define TILESCALER_REGIF_MASK_C2_OUTTILEWIDTH       ((u32)0x00000FFF << 0)                   /*!< R/W   0.0
                                                                                              * 0: 1 pixel
                                                                                              * 0xFFF: 4096 pixel
                                                                                              */
#define TILESCALER_REGIF_C2_OUTTILEWIDTH(x)         ((u32)(((x) & 0x00000FFF) << 0))
#define TILESCALER_REGIF_GET_C2_OUTTILEWIDTH(x)     ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup LineCntThreshold
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_LINENUMTHRESHOLD      ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0
                                                                                              * the number of line threshold in raster mode
                                                                                              * 'd0: 1 line
                                                                                              * 'd1: 2 line
                                                                                              */
#define TILESCALER_REGIF_LINENUMTHRESHOLD(x)        ((u32)(((x) & 0xFFFFFFFF) << 0))
#define TILESCALER_REGIF_GET_LINENUMTHRESHOLD(x)    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup TileCntThreshold
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_TILENUMTHRESHOLD      ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0
                                                                                              * the number of tile threshold in tile mode
                                                                                              * 'd0: 1 line
                                                                                              * 'd1: 2 line
                                                                                              */
#define TILESCALER_REGIF_TILENUMTHRESHOLD(x)        ((u32)(((x) & 0xFFFFFFFF) << 0))
#define TILESCALER_REGIF_GET_TILENUMTHRESHOLD(x)    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup COMPN2_WRAP_STRIDE
 * @brief
 * @{
 */
#define TILESCALER_REGIF_MASK_C2_WRAPSTRIDE         ((u32)0x0000FFFF << 0)                   /*!< R/W   0.0 Unit in bytes.  The value can be either positive or negative.  This field is the 2s complement representation, in format of s16.0 */
#define TILESCALER_REGIF_C2_WRAPSTRIDE(x)           ((u32)(((x) & 0x0000FFFF) << 0))
#define TILESCALER_REGIF_GET_C2_WRAPSTRIDE(x)       ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */




/** @defgroup TILESCALER_REGIF_Exported_Types TILESCALER_REGIF Exported Types
* @{
*/
/**
* @brief TILESCALER_REGIF Register Declaration
*/

typedef struct {
	__IO uint32_t INT_ENB;                                      /*!< Register,  Address offset:0x0000 */
	__IO uint32_t INT_MASK;                                     /*!< Register,  Address offset:0x0004 */
	__I  uint32_t RESERVE0;                                     /*!< Register,  Address offset:0x0008 */
	__IO uint32_t OPERATION;                                    /*!< Register,  Address offset:0x000C */
	__IO uint32_t IMAGE_FORMAT;                                 /*!< Register,  Address offset:0x0010 */
	__IO uint32_t CSC_PAR0;                                     /*!< Register,  Address offset:0x0014 */
	__IO uint32_t CSC_PAR1;                                     /*!< Register,  Address offset:0x0018 */
	__IO uint32_t CSC_PAR2;                                     /*!< Register,  Address offset:0x001C */
	__IO uint32_t CSC_COEF0;                                    /*!< Register,  Address offset:0x0020 */
	__IO uint32_t CSC_COEF1;                                    /*!< Register,  Address offset:0x0024 */
	__IO uint32_t CSC_COEF2;                                    /*!< Register,  Address offset:0x0028 */
	__IO uint32_t CSC_COEF3;                                    /*!< Register,  Address offset:0x002C */
	__IO uint32_t CSC_COEF4;                                    /*!< Register,  Address offset:0x0030 */
	__IO uint32_t COMPN0_PAR0;                                  /*!< Register,  Address offset:0x0034 */
	__IO uint32_t COMPN0_PAR1;                                  /*!< Register,  Address offset:0x0038 */
	__I  uint32_t RESERVE1;                                     /*!< Register,  Address offset:0x003C */
	__IO uint32_t COMPN0_LPF0;                                  /*!< Register,  Address offset:0x0040 */
	__IO uint32_t COMPN0_LPF1;                                  /*!< Register,  Address offset:0x0044 */
	__IO uint32_t COMPN0_LPF2;                                  /*!< Register,  Address offset:0x0048 */
	__IO uint32_t COMPN0_INT_HOR0;                              /*!< Register,  Address offset:0x004C */
	__IO uint32_t COMPN0_INT_HOR1;                              /*!< Register,  Address offset:0x0050 */
	__IO uint32_t COMPN0_INT_HOR2;                              /*!< Register,  Address offset:0x0054 */
	__IO uint32_t COMPN0_INT_HOR3;                              /*!< Register,  Address offset:0x0058 */
	__IO uint32_t COMPN0_INT_HOR4;                              /*!< Register,  Address offset:0x005C */
	__IO uint32_t COMPN0_INT_HOR5;                              /*!< Register,  Address offset:0x0060 */
	__IO uint32_t COMPN0_INT_HOR6;                              /*!< Register,  Address offset:0x0064 */
	__IO uint32_t COMPN0_INT_HOR7;                              /*!< Register,  Address offset:0x0068 */
	__IO uint32_t COMPN0_INT_HOR8;                              /*!< Register,  Address offset:0x006C */
	__IO uint32_t COMPN0_INT_HOR9;                              /*!< Register,  Address offset:0x0070 */
	__IO uint32_t COMPN0_INT_HOR10;                             /*!< Register,  Address offset:0x0074 */
	__IO uint32_t COMP0_INTILEHEIGHT;                           /*!< Register,  Address offset:0x0078 */
	__IO uint32_t COMP0_INTILEWIDTH;                            /*!< Register,  Address offset:0x007C */
	__IO uint32_t COMPN0_INT_VER0;                              /*!< Register,  Address offset:0x0080 */
	__IO uint32_t COMPN0_INT_VER1;                              /*!< Register,  Address offset:0x0084 */
	__IO uint32_t COMPN0_INT_VER2;                              /*!< Register,  Address offset:0x0088 */
	__IO uint32_t COMPN0_INT_VER3;                              /*!< Register,  Address offset:0x008C */
	__IO uint32_t COMPN0_INT_VER4;                              /*!< Register,  Address offset:0x0090 */
	__IO uint32_t COMPN0_INT_VER5;                              /*!< Register,  Address offset:0x0094 */
	__IO uint32_t COMPN0_INT_VER6;                              /*!< Register,  Address offset:0x0098 */
	__IO uint32_t COMPN0_INT_VER7;                              /*!< Register,  Address offset:0x009C */
	__IO uint32_t COMPN0_INT_VER8;                              /*!< Register,  Address offset:0x00A0 */
	__IO uint32_t COMPN0_INT_VER9;                              /*!< Register,  Address offset:0x00A4 */
	__IO uint32_t COMPN0_INT_VER10;                             /*!< Register,  Address offset:0x00A8 */
	__IO uint32_t COMPN0_INT_IN_ADDR;                           /*!< Register,  Address offset:0x00AC */
	__IO uint32_t COMPN0_INT_OUT_ADDR;                          /*!< Register,  Address offset:0x00B0 */
	__IO uint32_t COMPN0_INT_STRIDE;                            /*!< Register,  Address offset:0x00B4 */
	__IO uint32_t COMPN0_INT_OUT_TILE_SIZE;                     /*!< Register,  Address offset:0x00B8 */
	__IO uint32_t COMPN0_WRAP_STRIDE;                           /*!< Register,  Address offset:0x00BC */
	__I  uint32_t RESERVE5;                                     /*!< Register,  Address offset:0x00C0 */
	__IO uint32_t COMPN1_PAR0;                                  /*!< Register,  Address offset:0x00C4 */
	__IO uint32_t COMPN1_PAR1;                                  /*!< Register,  Address offset:0x00C8 */
	__I  uint32_t RESERVE6;                                     /*!< Register,  Address offset:0x00CC */
	__IO uint32_t COMPN1_LPF0;                                  /*!< Register,  Address offset:0x00D0 */
	__IO uint32_t COMPN1_LPF1;                                  /*!< Register,  Address offset:0x00D4 */
	__IO uint32_t COMPN1_LPF2;                                  /*!< Register,  Address offset:0x00D8 */
	__IO uint32_t COMPN1_INT_HOR0;                              /*!< Register,  Address offset:0x00DC */
	__IO uint32_t COMPN1_INT_HOR1;                              /*!< Register,  Address offset:0x00E0 */
	__IO uint32_t COMPN1_INT_HOR2;                              /*!< Register,  Address offset:0x00E4 */
	__IO uint32_t COMPN1_INT_HOR3;                              /*!< Register,  Address offset:0x00E8 */
	__IO uint32_t COMPN1_INT_HOR4;                              /*!< Register,  Address offset:0x00EC */
	__IO uint32_t COMPN1_INT_HOR5;                              /*!< Register,  Address offset:0x00F0 */
	__IO uint32_t COMPN1_INT_HOR6;                              /*!< Register,  Address offset:0x00F4 */
	__IO uint32_t COMPN1_INT_HOR7;                              /*!< Register,  Address offset:0x00F8 */
	__IO uint32_t COMPN1_INT_HOR8;                              /*!< Register,  Address offset:0x00FC */
	__IO uint32_t COMPN1_INT_HOR9;                              /*!< Register,  Address offset:0x0100 */
	__IO uint32_t COMPN1_INT_HOR10;                             /*!< Register,  Address offset:0x0104 */
	__IO uint32_t COMP1_INTILEHEIGHT;                           /*!< Register,  Address offset:0x0108 */
	__IO uint32_t COMP1_INTILEWIDTH;                            /*!< Register,  Address offset:0x010C */
	__IO uint32_t COMPN1_INT_VER0;                              /*!< Register,  Address offset:0x0110 */
	__IO uint32_t COMPN1_INT_VER1;                              /*!< Register,  Address offset:0x0114 */
	__IO uint32_t COMPN1_INT_VER2;                              /*!< Register,  Address offset:0x0118 */
	__IO uint32_t COMPN1_INT_VER3;                              /*!< Register,  Address offset:0x011C */
	__IO uint32_t COMPN1_INT_VER4;                              /*!< Register,  Address offset:0x0120 */
	__IO uint32_t COMPN1_INT_VER5;                              /*!< Register,  Address offset:0x0124 */
	__IO uint32_t COMPN1_INT_VER6;                              /*!< Register,  Address offset:0x0128 */
	__IO uint32_t COMPN1_INT_VER7;                              /*!< Register,  Address offset:0x012C */
	__IO uint32_t COMPN1_INT_VER8;                              /*!< Register,  Address offset:0x0130 */
	__IO uint32_t COMPN1_INT_VER9;                              /*!< Register,  Address offset:0x0134 */
	__IO uint32_t COMPN1_INT_VER10;                             /*!< Register,  Address offset:0x0138 */
	__IO uint32_t COMPN1_INT_IN_ADDR;                           /*!< Register,  Address offset:0x013C */
	__IO uint32_t COMPN1_INT_OUT_ADDR;                          /*!< Register,  Address offset:0x0140 */
	__IO uint32_t COMPN1_INT_STRIDE;                            /*!< Register,  Address offset:0x0144 */
	__IO uint32_t COMPN1_INT_OUT_TILE_SIZE;                     /*!< Register,  Address offset:0x0148 */
	__IO uint32_t COMPN1_WRAP_STRIDE;                           /*!< Register,  Address offset:0x014C */
	__I  uint32_t RESERVE10;                                    /*!< Register,  Address offset:0x0150 */
	__IO uint32_t COMPN2_PAR0;                                  /*!< Register,  Address offset:0x0154 */
	__IO uint32_t COMPN2_PAR1;                                  /*!< Register,  Address offset:0x0158 */
	__I  uint32_t RESERVE11;                                    /*!< Register,  Address offset:0x015C */
	__IO uint32_t COMPN2_LPF0;                                  /*!< Register,  Address offset:0x0160 */
	__IO uint32_t COMPN2_LPF1;                                  /*!< Register,  Address offset:0x0164 */
	__IO uint32_t COMPN2_LPF2;                                  /*!< Register,  Address offset:0x0168 */
	__IO uint32_t COMPN2_INT_HOR0;                              /*!< Register,  Address offset:0x016C */
	__IO uint32_t COMPN2_INT_HOR1;                              /*!< Register,  Address offset:0x0170 */
	__IO uint32_t COMPN2_INT_HOR2;                              /*!< Register,  Address offset:0x0174 */
	__IO uint32_t COMPN2_INT_HOR3;                              /*!< Register,  Address offset:0x0178 */
	__IO uint32_t COMPN2_INT_HOR4;                              /*!< Register,  Address offset:0x017C */
	__IO uint32_t COMPN2_INT_HOR5;                              /*!< Register,  Address offset:0x0180 */
	__IO uint32_t COMPN2_INT_HOR6;                              /*!< Register,  Address offset:0x0184 */
	__IO uint32_t COMPN2_INT_HOR7;                              /*!< Register,  Address offset:0x0188 */
	__IO uint32_t COMPN2_INT_HOR8;                              /*!< Register,  Address offset:0x018C */
	__IO uint32_t COMPN2_INT_HOR9;                              /*!< Register,  Address offset:0x0190 */
	__IO uint32_t COMPN2_INT_HOR10;                             /*!< Register,  Address offset:0x0194 */
	__IO uint32_t COMP2_INTILEHEIGHT;                           /*!< Register,  Address offset:0x0198 */
	__IO uint32_t COMP2_INTILEWIDTH;                            /*!< Register,  Address offset:0x019C */
	__IO uint32_t COMPN2_INT_VER0;                              /*!< Register,  Address offset:0x01A0 */
	__IO uint32_t COMPN2_INT_VER1;                              /*!< Register,  Address offset:0x01A4 */
	__IO uint32_t COMPN2_INT_VER2;                              /*!< Register,  Address offset:0x01A8 */
	__IO uint32_t COMPN2_INT_VER3;                              /*!< Register,  Address offset:0x01AC */
	__IO uint32_t COMPN2_INT_VER4;                              /*!< Register,  Address offset:0x01B0 */
	__IO uint32_t COMPN2_INT_VER5;                              /*!< Register,  Address offset:0x01B4 */
	__IO uint32_t COMPN2_INT_VER6;                              /*!< Register,  Address offset:0x01B8 */
	__IO uint32_t COMPN2_INT_VER7;                              /*!< Register,  Address offset:0x01BC */
	__IO uint32_t COMPN2_INT_VER8;                              /*!< Register,  Address offset:0x01C0 */
	__IO uint32_t COMPN2_INT_VER9;                              /*!< Register,  Address offset:0x01C4 */
	__IO uint32_t COMPN2_INT_VER10;                             /*!< Register,  Address offset:0x01C8 */
	__IO uint32_t COMPN2_INT_IN_ADDR;                           /*!< Register,  Address offset:0x01CC */
	__IO uint32_t COMPN2_INT_OUT_ADDR;                          /*!< Register,  Address offset:0x01D0 */
	__IO uint32_t COMPN2_INT_STRIDE;                            /*!< Register,  Address offset:0x01D4 */
	__IO uint32_t COMPN2_INT_OUT_TILE_SIZE;                     /*!< Register,  Address offset:0x01D8 */
	__IO uint32_t LINECNTTHRESHOLD;                             /*!< Register,  Address offset:0x01DC */
	__IO uint32_t TILECNTTHRESHOLD;                             /*!< Register,  Address offset:0x01E0 */
	__IO uint32_t COMPN2_WRAP_STRIDE;                           /*!< Register,  Address offset:0x01E4 */
} TILESCALER_REGIF_TypeDef;

/** @} */


/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your defination here


/* MANUAL_GEN_END */

/** @} */

/** @} */
#endif  // AMEBA_TS_H
