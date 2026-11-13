/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAL_REMAP_ISP_H
#define HAL_REMAP_ISP_H

/* AUTO_GEN_START */

/* REMAPADR_ISP is a part of REMAPADR */


/** @addtogroup Ameba_Periph_Driver
* @{
*/

/** @defgroup REMAPADR_ISP
* @brief REMAPADR_ISP driver modules
* @{
*/


/** @defgroup INT_ENB
 * @brief
 * @{
 */
#define REMAPADR_ISP_BIT_INTBUSERR                  ((u32)0x00000001 << 7)                   /*!< R/W1C   0 1: set when the APB address points to an empty address.  Pslverr is flagged at the same time.0: no interrupt.Write 1 to clear. Write 0 has no effect. */
#define REMAPADR_ISP_BIT_HSKISPENCDERR              ((u32)0x00000001 << 6)                   /*!< R/W1C   0 1: Error when ISP overwrite the data of encoder.0: No interruptWrite 1 to clear. Write 0 has no effectThe handshake error will only happen between ISP and encoder.  Between tile scaler and encoder, the design guarantees that no overwrite will happen.Use by ISP remap */
#define REMAPADR_ISP_BIT_SLICETHRESHOLD             ((u32)0x00000001 << 5)                   /*!< R/W1C   0 1: From SOF, the module will count slice number from 0.  Assert when the number of slice reaches the value of slice threshold in register setting.0: no interruptWrite 1 to clear. Write 0 has no effect.Use by ISP remap */
#define REMAPADR_ISP_BIT_SLICEWRAPEOF               ((u32)0x00000001 << 4)                   /*!< R/W1C   0 1: Assert after the final slice of a frame is written.0: no interruptWrite 1 to clear. Write 0 has no effect.Use by ISP remap. */
#define REMAPADR_ISP_BIT_HDREOF                     ((u32)0x00000001 << 3)                   /*!< R/W1C   0 1: Assert when ISP send the final last HDR read of a frame.0: no interruptWrite 1 to clear. Write 0 has no effect.Use by ISP remap */
#define REMAPADR_ISP_BIT_SLICEOFPICTUREERROR        ((u32)0x00000001 << 2)                   /*!< R/W1C   0 1: Total slice number in the picture is different from the register setting.0: no errorWrite 1 to clear. Write 0 has no effect.Use by ISP remap (encoder side detection is TBD) */
#define REMAPADR_ISP_BIT_SLICEWRAPERROR             ((u32)0x00000001 << 1)                   /*!< R/W1C   0 1: Burst size of slice is larger than slice wrap burst setting.0: no errorWrite 1 to clear. Write 0 has no effect.Use by ISP remap */
#define REMAPADR_ISP_BIT_HDRERROR                   ((u32)0x00000001 << 0)                   /*!< R/W1C   0 1: Burst size of HDR is larger than HDR burst setting.0: no errorWrite 1 to clear. Write 0 has no effect.Use by ISP remap */
/** @} */


/** @defgroup INT_MASK
 * @brief
 * @{
 */
#define REMAPADR_ISP_BIT_INTBUSERRMASK              ((u32)0x00000001 << 7)                   /*!< R/W     0 1: Mask the interrupt.0: Generate the interrupt */
#define REMAPADR_ISP_BIT_HSKISPENCDMASK             ((u32)0x00000001 << 6)                   /*!< R/W   0   1: Mask the interrupt.0: Generate the interrupt */
#define REMAPADR_ISP_BIT_SLICETHRESHOLDMASK         ((u32)0x00000001 << 5)                   /*!< R/W   0   1: Mask the interrupt.0: Generate the interrupt */
#define REMAPADR_ISP_BIT_SLICEWRAPEOFMASK           ((u32)0x00000001 << 4)                   /*!< R/W   0   1: Mask the interrupt.0: Generate the interrupt */
#define REMAPADR_ISP_BIT_HDREOFMASK                 ((u32)0x00000001 << 3)                   /*!< R/W   0   1: Mask the interrupt.0: Generate the interrupt */
#define REMAPADR_ISP_BIT_SLICEOFPICTUREERRORMASK    ((u32)0x00000001 << 2)                   /*!< R/W   0   1: Mask the interrupt.0: Generate the interrupt */
#define REMAPADR_ISP_BIT_SLICEWRAPERRORMASK         ((u32)0x00000001 << 1)                   /*!< R/W   0   1: Mask the interrupt.0: Generate the interruptFor encoder remap, user should always mask the interrupt. */
#define REMAPADR_ISP_BIT_HDRERRORMASK               ((u32)0x00000001 << 0)                   /*!< R/W   0   1: Mask the interrupt.0: Generate the interrupt */
/** @} */


/** @defgroup HANDSHAKE_CTRL
 * @brief
 * @{
 */
#define REMAPADR_ISP_BIT_TSCENCDENABLE              ((u32)0x00000001 << 1)                   /*!< R/W     0 1: enable the handshake between TSC and Encoder0: disableUse by encoder remap */
#define REMAPADR_ISP_BIT_ISPENCDENABLE              ((u32)0x00000001 << 0)                   /*!< R/W     0 1: enable the handshake between ISP and Encoder0: disableUse by ISP and encoder remap */
/** @} */


/** @defgroup FUNC_ENB
 * @brief
 * @{
 */
#define REMAPADR_ISP_BIT_PARENB                     ((u32)0x00000001 << 24)                  /*!< R/W     0 1: Partial SRAM func enable0: Partial SRAM func disableUse by ISP and encoder remap */
#define REMAPADR_ISP_BIT_RTTENB                     ((u32)0x00000001 << 16)                  /*!< R/W     0 1: Raster to Tile func enable0: Raster to Tile func disableUse by ISP and encoder remap */
#define REMAPADR_ISP_BIT_PARSWRAPENB                ((u32)0x00000001 << 9)                   /*!< R/W     0 1: partial slice wrap function enable0: partial slice wrap function disableUse by encoder remap */
#define REMAPADR_ISP_BIT_SWRAPENB                   ((u32)0x00000001 << 8)                   /*!< R/W     0 1: Slice wrap func enable. For encoder remap, user should always set it to 0.0: Slice wrap func disableBelow are the supported combination of various features.- slice wrap- slice wrap + raster to tile- slice wrap + compression- slice wrap + partial SRAM- slice wrap + partial SRAM + compression- compressionUse by ISP and encoder remap */
#define REMAPADR_ISP_BIT_HDRENB                     ((u32)0x00000001 << 0)                   /*!< R/W     0 1: HDR func enable0: HDR func disableUse by ISP remap */
/** @} */


/** @defgroup COMP_ENB
 * @brief
 * @{
 */
#define REMAPADR_ISP_BIT_COMPENB                    ((u32)0x00000001 << 0)                   /*!< R/W   0   1: Compress func enable  0: Compress func disableUse by ISP remap */
/** @} */


/** @defgroup HDR_WID
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_HDR_WSID                  ((u32)0x0000FFFF << 16)                  /*!< R/W   0   AXI write ID for ISP remap writing HDR data to SRAM.  Use by ISP remap */
#define REMAPADR_ISP_HDR_WSID(x)                    ((u32)(((x) & 0x0000FFFF << 16))
#define REMAPADR_ISP_GET_HDR_WSID(x)                ((u32)(((x >> 16) & 0x0000FFFF)))
#define REMAPADR_ISP_MASK_HDR_WDID                  ((u32)0x0000FFFF << 0)                   /*!< R/W   0   AXI write ID for ISP remap writing HDR data to DRAM.  The module will also use this ID to determine the HDR data write from ISP.Use by ISP remap */
#define REMAPADR_ISP_HDR_WDID(x)                    ((u32)(((x) & 0x0000FFFF << 0))
#define REMAPADR_ISP_GET_HDR_WDID(x)                ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup HDR_RID
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_HDR_RID                   ((u32)0x0000FFFF << 0)                   /*!< R/W   0   AXI read ID for ISP HDR read operation.  The module will use this ID to determine the HDR data read from ISP.Use by ISP remap */
#define REMAPADR_ISP_HDR_RID(x)                     ((u32)(((x) & 0x0000FFFF << 0))
#define REMAPADR_ISP_GET_HDR_RID(x)                 ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup HDR_INFO
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_HDRBURSTSIZE              ((u32)0x00000003 << 24)                  /*!< R/W     0 HDR burst size for both read and write operation'b00: 64 bytes'b01: 128 bytes'b10: 256 bytes'b11: 512 bytesUse by ISP remap */
#define REMAPADR_ISP_HDRBURSTSIZE(x)                ((u32)(((x) & 0x00000003 << 24))
#define REMAPADR_ISP_GET_HDRBURSTSIZE(x)            ((u32)(((x >> 24) & 0x00000003)))
#define REMAPADR_ISP_MASK_HDRMAXBURST               ((u32)0x00000FFF << 12)                  /*!< R/W     0 HDR max burst count for wrap around'd0: 1'd1: 2Use by ISP remap */
#define REMAPADR_ISP_HDRMAXBURST(x)                 ((u32)(((x) & 0x00000FFF << 12))
#define REMAPADR_ISP_GET_HDRMAXBURST(x)             ((u32)(((x >> 12) & 0x00000FFF)))
#define REMAPADR_ISP_MASK_HDRDENOMINATOR            ((u32)0x0000003F << 6)                   /*!< R/W     0 HDR ratio denominatorUse by ISP remap */
#define REMAPADR_ISP_HDRDENOMINATOR(x)              ((u32)(((x) & 0x0000003F << 6))
#define REMAPADR_ISP_GET_HDRDENOMINATOR(x)          ((u32)(((x >> 6) & 0x0000003F)))
#define REMAPADR_ISP_MASK_HDRNUMERATOR              ((u32)0x0000003F << 0)                   /*!< R/W     0 HDR ratio numeratorUse by ISP remap */
#define REMAPADR_ISP_HDRNUMERATOR(x)                ((u32)(((x) & 0x0000003F << 0))
#define REMAPADR_ISP_GET_HDRNUMERATOR(x)            ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup HDR_RESET
 * @brief
 * @{
 */
#define REMAPADR_ISP_BIT_HDRCNTRST                  ((u32)0x00000001 << 0)                   /*!< R/W   0   1: Reset HDR counter0: Write 0 doesn't effectThis bit has no auto clear function.  After writing 1, user needs to write 0 to prepare for next reset operation.  To support auto clear is TBD.Use by ISP remap */
/** @} */


/** @defgroup HDR_SRAM_SADDR
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_HDRADRBASE                ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0   Base Address of SRAM for HDR dataUse by ISP remap */
#define REMAPADR_ISP_HDRADRBASE(x)                  ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_HDRADRBASE(x)              ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup SLW_IN_ID
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_SLICEWRAPDIDC2            ((u32)0x000000FF << 24)                  /*!< R/W   0   AXI Write ID of ISP Remap unit for component2 to DRAMUse by ISP remap */
#define REMAPADR_ISP_SLICEWRAPDIDC2(x)              ((u32)(((x) & 0x000000FF << 24))
#define REMAPADR_ISP_GET_SLICEWRAPDIDC2(x)          ((u32)(((x >> 24) & 0x000000FF)))
#define REMAPADR_ISP_MASK_SLICEWRAPDIDC1            ((u32)0x000000FF << 16)                  /*!< R/W   0   AXI Write ID of ISP Remap unit for component1 to DRAMUse by ISP remap */
#define REMAPADR_ISP_SLICEWRAPDIDC1(x)              ((u32)(((x) & 0x000000FF << 16))
#define REMAPADR_ISP_GET_SLICEWRAPDIDC1(x)          ((u32)(((x >> 16) & 0x000000FF)))
#define REMAPADR_ISP_MASK_SLICEWRAPDIDC0            ((u32)0x000000FF << 8)                   /*!< R/W   0   AXI Write ID of ISP Remap unit for component0 to DRAMUse by ISP remap */
#define REMAPADR_ISP_SLICEWRAPDIDC0(x)              ((u32)(((x) & 0x000000FF << 8))
#define REMAPADR_ISP_GET_SLICEWRAPDIDC0(x)          ((u32)(((x >> 8) & 0x000000FF)))
#define REMAPADR_ISP_MASK_SLICEWRAPDINID            ((u32)0x000000FF << 0)                   /*!< R/W   0   AXI Write ID for ISP video data output.  The module will use this ID to determine the video data write from ISP.Use by ISP remap */
#define REMAPADR_ISP_SLICEWRAPDINID(x)              ((u32)(((x) & 0x000000FF << 0))
#define REMAPADR_ISP_GET_SLICEWRAPDINID(x)          ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup SLW_OUT_ID
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_SLICEWRAPSIDC2            ((u32)0x000000FF << 24)                  /*!< R/W   0   AXI Write ID of ISP Remap unit for component2 to SRAMUse by ISP remap */
#define REMAPADR_ISP_SLICEWRAPSIDC2(x)              ((u32)(((x) & 0x000000FF << 24))
#define REMAPADR_ISP_GET_SLICEWRAPSIDC2(x)          ((u32)(((x >> 24) & 0x000000FF)))
#define REMAPADR_ISP_MASK_SLICEWRAPSIDC1            ((u32)0x000000FF << 16)                  /*!< R/W   0   AXI Write ID of ISP Remap unit for component1 to SRAMUse by ISP remap */
#define REMAPADR_ISP_SLICEWRAPSIDC1(x)              ((u32)(((x) & 0x000000FF << 16))
#define REMAPADR_ISP_GET_SLICEWRAPSIDC1(x)          ((u32)(((x >> 16) & 0x000000FF)))
#define REMAPADR_ISP_MASK_SLICEWRAPSIDC0            ((u32)0x000000FF << 8)                   /*!< R/W   0   AXI Write ID of ISP Remap unit for component0 to SRAMUse by ISP remap */
#define REMAPADR_ISP_SLICEWRAPSIDC0(x)              ((u32)(((x) & 0x000000FF << 8))
#define REMAPADR_ISP_GET_SLICEWRAPSIDC0(x)          ((u32)(((x >> 8) & 0x000000FF)))
#define REMAPADR_ISP_MASK_HDRSRAMID                 ((u32)0x000000FF << 0)                   /*!< R/W   0   AXI read ID for the ISP remap reading HDR data from SRAMAXI write ID for the ISP remap writing compression header to memory.Use by ISP remap */
#define REMAPADR_ISP_HDRSRAMID(x)                   ((u32)(((x) & 0x000000FF << 0))
#define REMAPADR_ISP_GET_HDRSRAMID(x)               ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup SLW_INFO
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_TOTALSLICENUM             ((u32)0x000001FF << 5)                   /*!< R/W   0   Total slice number of picture0: 1 slice1: 2 slice….Use by ISP remap */
#define REMAPADR_ISP_TOTALSLICENUM(x)               ((u32)(((x) & 0x000001FF << 5))
#define REMAPADR_ISP_GET_TOTALSLICENUM(x)           ((u32)(((x >> 5) & 0x000001FF)))
#define REMAPADR_ISP_MASK_SLICENUMWRAP              ((u32)0x00000007 << 2)                   /*!< R/W   0   Wrap around after this number of slice. For slice wrap + compression, the compression header will wrap together with the slice data.0: wrap after 1 slice1: wrap after 2 slice...Use by ISP remap */
#define REMAPADR_ISP_SLICENUMWRAP(x)                ((u32)(((x) & 0x00000007 << 2))
#define REMAPADR_ISP_GET_SLICENUMWRAP(x)            ((u32)(((x >> 2) & 0x00000007)))
#define REMAPADR_ISP_MASK_SLICEBURSTSIZE            ((u32)0x00000003 << 0)                   /*!< R/W     0 Burst size of slice wrap0: 64 byte1: 128 byte2: 256 byte3: 512 byteUse by ISP remap */
#define REMAPADR_ISP_SLICEBURSTSIZE(x)              ((u32)(((x) & 0x00000003 << 0))
#define REMAPADR_ISP_GET_SLICEBURSTSIZE(x)          ((u32)(((x >> 0) & 0x00000003)))
/** @} */


/** @defgroup SLW_DRAM_CP0_SADDR
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_SLICE_COMP0_DRAM_SADDR    ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 Start address of video data Component 0.  For ISP remap, the address range covers the full frame,which is befor slice wrap.  The ISP remap will do the slice wrap operation.For encoder remap, if partial SRAM or raster to tile conversion is enabled together with slice wrap, the address range should cover the ring buffer, which is after slice wrap.  If only slice wrap is enabled, this field is not used, as the encoder handles the slice wrap operation by itself.For encoder remap, if tile scaler handshake is enabled, the address range should cover the whole frame range, as encoder remap does not use the slice operation.Use by ISP and encoder remap */
#define REMAPADR_ISP_SLICE_COMP0_DRAM_SADDR(x)      ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_SLICE_COMP0_DRAM_SADDR(x)  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup SLW_DRAM_CP0_EADDR
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_SLICE_COMP0_DRAM_EADDR    ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 End address of video data Component 0Use by ISP and encoder remap */
#define REMAPADR_ISP_SLICE_COMP0_DRAM_EADDR(x)      ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_SLICE_COMP0_DRAM_EADDR(x)  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup SLICE_CP0_INFO
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_SLICE_COMP0_SLICEHEIGHT   ((u32)0x00000003 << 13)                  /*!< R/W     0 Component 0 slice height0: 8 rows1: 16 rows2: 32 rows3: 64 rowsUse by ISP remap */
#define REMAPADR_ISP_SLICE_COMP0_SLICEHEIGHT(x)     ((u32)(((x) & 0x00000003 << 13))
#define REMAPADR_ISP_GET_SLICE_COMP0_SLICEHEIGHT(x) ((u32)(((x >> 13) & 0x00000003)))
#define REMAPADR_ISP_MASK_SLICE_COMP0_STRIDE        ((u32)0x00000FFF << 1)                   /*!< R/W     0 Component 0 stride. The unit is burst size.'d0: 1 burst'd1: 2 burstsUse by ISP remap */
#define REMAPADR_ISP_SLICE_COMP0_STRIDE(x)          ((u32)(((x) & 0x00000FFF << 1))
#define REMAPADR_ISP_GET_SLICE_COMP0_STRIDE(x)      ((u32)(((x >> 1) & 0x00000FFF)))
#define REMAPADR_ISP_BIT_COMPENABLE0                ((u32)0x00000001 << 0)                   /*!< R/W     0 1: Component 0 is involved in the slice wrap handshake process.0: Component 0 is not involved in the slice wrap handshake process.Use by ISP and encoder remap */
/** @} */


/** @defgroup SLW_DRAM_CP1_SADDR
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_SLICE_COMP1_DRAM_SADDR    ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 Start address of video data Component 1.  For details, please refer to component 0.Use by ISP and encoder remap */
#define REMAPADR_ISP_SLICE_COMP1_DRAM_SADDR(x)      ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_SLICE_COMP1_DRAM_SADDR(x)  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup SLW_DRAM_CP1_EADDR
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_SLICE_COMP1_DRAM_EADDR    ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 End address of video data Component 1.  Use by ISP and encoder remap */
#define REMAPADR_ISP_SLICE_COMP1_DRAM_EADDR(x)      ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_SLICE_COMP1_DRAM_EADDR(x)  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup SLICE_CP1_INFO
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_SLICE_COMP1_SLICEHEIGHT   ((u32)0x00000003 << 13)                  /*!< R/W     0 Component 1 slice height0: 8 rows1: 16 rows2: 32 rows3: 64 rowsUse by ISP remap */
#define REMAPADR_ISP_SLICE_COMP1_SLICEHEIGHT(x)     ((u32)(((x) & 0x00000003 << 13))
#define REMAPADR_ISP_GET_SLICE_COMP1_SLICEHEIGHT(x) ((u32)(((x >> 13) & 0x00000003)))
#define REMAPADR_ISP_MASK_SLICE_COMP1_STRIDE        ((u32)0x00000FFF << 1)                   /*!< R/W     0 Component 1 stride. The unit is burst size.'d0: 1 burst'd1: 2 burstsUse by ISP remap */
#define REMAPADR_ISP_SLICE_COMP1_STRIDE(x)          ((u32)(((x) & 0x00000FFF << 1))
#define REMAPADR_ISP_GET_SLICE_COMP1_STRIDE(x)      ((u32)(((x >> 1) & 0x00000FFF)))
#define REMAPADR_ISP_BIT_COMPENABLE1                ((u32)0x00000001 << 0)                   /*!< R/W     0 1: Component 1 is involved in the slice wrap handshake process.0: Component 1 is not involved in the slice wrap handshake process.Use by ISP and encoder remap */
/** @} */


/** @defgroup SLW_DRAM_CP2_SADDR
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_SLICE_COMP2_DRAM_SADDR    ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 Start address of video data Component 2.  For details, please refer to component 0.Use by ISP and encoder remap */
#define REMAPADR_ISP_SLICE_COMP2_DRAM_SADDR(x)      ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_SLICE_COMP2_DRAM_SADDR(x)  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup SLW_DRAM_CP2_EADDR
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_SLICE_COMP2_DRAM_EADDR    ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 End address of video data Component 2.  Use by ISP and encoder remap */
#define REMAPADR_ISP_SLICE_COMP2_DRAM_EADDR(x)      ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_SLICE_COMP2_DRAM_EADDR(x)  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup SLICE_CP2_INFO
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_SLICE_COMP2_SLICEHEIGHT   ((u32)0x00000003 << 13)                  /*!< R/W     0 Component 2 slice height0: 8 rows1: 16 rows2: 32 rows3: 64 rowsUse by ISP remap */
#define REMAPADR_ISP_SLICE_COMP2_SLICEHEIGHT(x)     ((u32)(((x) & 0x00000003 << 13))
#define REMAPADR_ISP_GET_SLICE_COMP2_SLICEHEIGHT(x) ((u32)(((x >> 13) & 0x00000003)))
#define REMAPADR_ISP_MASK_SLICE_COMP2_STRIDE        ((u32)0x00000FFF << 1)                   /*!< R/W     0 Component 2 stride. The unit is burst size.'d0: 1 burst'd1: 2 burstsUse by ISP remap */
#define REMAPADR_ISP_SLICE_COMP2_STRIDE(x)          ((u32)(((x) & 0x00000FFF << 1))
#define REMAPADR_ISP_GET_SLICE_COMP2_STRIDE(x)      ((u32)(((x >> 1) & 0x00000FFF)))
#define REMAPADR_ISP_BIT_COMPENABLE2                ((u32)0x00000001 << 0)                   /*!< R/W     0 1: Component 2 is involved in the slice wrap handshake process.0: Component 2 is not involved in the slice wrap handshake process.Use by ISP and encoder remap */
/** @} */


/** @defgroup PARTIAL_SRAM_INFO
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_SRAMSIZEPART              ((u32)0x0000FFFF << 4)                   /*!< R/W     0 RSVD */
#define REMAPADR_ISP_SRAMSIZEPART(x)                ((u32)(((x) & 0x0000FFFF << 4))
#define REMAPADR_ISP_GET_SRAMSIZEPART(x)            ((u32)(((x >> 4) & 0x0000FFFF)))
#define REMAPADR_ISP_MASK_SRAMRATIO                 ((u32)0x0000000F << 0)                   /*!< R/W     0 Partial SRAM ratio numerator.  The denominator is fixed at 16.Use by ISP and encoder remap */
#define REMAPADR_ISP_SRAMRATIO(x)                   ((u32)(((x) & 0x0000000F << 0))
#define REMAPADR_ISP_GET_SRAMRATIO(x)               ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup SLICE_NUM_THRESHOLD
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_SLICENUMTHRESHOLD         ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0   The number of slice threshold 'd0: 1 slice'd1: 2 sliceUse by ISP remap */
#define REMAPADR_ISP_SLICENUMTHRESHOLD(x)           ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_SLICENUMTHRESHOLD(x)       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup SLW_SRAM_CP0_SADDR
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_SLICE_COMP0_SRAM_SADDR    ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0x0 Base address of SRAM for storing video data Component 0 during partial SRAM operationUse by ISP and encoder remap */
#define REMAPADR_ISP_SLICE_COMP0_SRAM_SADDR(x)      ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_SLICE_COMP0_SRAM_SADDR(x)  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup SLW_SRAM_CP0_EADDR
 * @brief
 * @{
 */
/** @} */


/** @defgroup SLW_SRAM_CP1_SADDR
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_SLICE_COMP1_SRAM_SADDR    ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0x0 Base address of SRAM for storing video data Component 1 during partial SRAM operationUse by ISP and encoder remap */
#define REMAPADR_ISP_SLICE_COMP1_SRAM_SADDR(x)      ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_SLICE_COMP1_SRAM_SADDR(x)  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup SLW_SRAM_CP1_EADDR
 * @brief
 * @{
 */
/** @} */


/** @defgroup SLW_SRAM_CP2_SADDR
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_SLICE_COMP2_SRAM_SADDR    ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0x0 Base address of SRAM for storing video data Component 2 during partial SRAM operationUse by ISP and encoder remap */
#define REMAPADR_ISP_SLICE_COMP2_SRAM_SADDR(x)      ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_SLICE_COMP2_SRAM_SADDR(x)  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup SLW_SRAM_CP2_EADDR
 * @brief
 * @{
 */
/** @} */


/** @defgroup CMPR_DADR_START0
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_COMPRESS_DADR_SADDR0      ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 DRAM start address of video data component 0.  For compression + partial SRAM, user needs to config both DRAM and SRAM detection.Use by ISP remap */
#define REMAPADR_ISP_COMPRESS_DADR_SADDR0(x)        ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_COMPRESS_DADR_SADDR0(x)    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup CMPR_DADR_END0
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_COMPRESS_DADR_EADDR0      ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0   DRAM end address of video data component 0.  For compression + partial SRAM, user needs to config both DRAM and SRAM detection.Use by ISP remap */
#define REMAPADR_ISP_COMPRESS_DADR_EADDR0(x)        ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_COMPRESS_DADR_EADDR0(x)    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup CMPR_DADR_START1
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_COMPRESS_DADR_SADDR1      ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 DRAM start address of video data component 1.  For compression + partial SRAM, user needs to config both DRAM and SRAM detection.Use by ISP remap */
#define REMAPADR_ISP_COMPRESS_DADR_SADDR1(x)        ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_COMPRESS_DADR_SADDR1(x)    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup CMPR_DADR_END1
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_COMPRESS_DADR_EADDR1      ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0   DRAM end address of video data component 1.  For compression + partial SRAM, user needs to config both DRAM and SRAM detection.Use by ISP remap */
#define REMAPADR_ISP_COMPRESS_DADR_EADDR1(x)        ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_COMPRESS_DADR_EADDR1(x)    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup CMPR_DADR_START2
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_COMPRESS_DADR_SADDR2      ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 DRAM start address of video data component 2.  For compression + partial SRAM, user needs to config both DRAM and SRAM detection.Use by ISP remap */
#define REMAPADR_ISP_COMPRESS_DADR_SADDR2(x)        ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_COMPRESS_DADR_SADDR2(x)    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup CMPR_DADR_END2
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_COMPRESS_DADR_EADDR2      ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0   DRAM end address of video data component 2.  For compression + partial SRAM, user needs to config both DRAM and SRAM detection.Use by ISP remap */
#define REMAPADR_ISP_COMPRESS_DADR_EADDR2(x)        ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_COMPRESS_DADR_EADDR2(x)    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup CMPR_SADR_START0
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_COMPRESS_SADR_SADDR0      ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SRAM start address of video data component 0.  For compression + partial SRAM, user needs to config both DRAM and SRAM detection.Use by ISP remap */
#define REMAPADR_ISP_COMPRESS_SADR_SADDR0(x)        ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_COMPRESS_SADR_SADDR0(x)    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup CMPR_SADR_END0
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_COMPRESS_SADR_EADDR0      ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0   SRAM end address of video data component 0.  For compression + partial SRAM, user needs to config both DRAM and SRAM detection.Use by ISP remap */
#define REMAPADR_ISP_COMPRESS_SADR_EADDR0(x)        ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_COMPRESS_SADR_EADDR0(x)    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup CMPR_SADR_START1
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_COMPRESS_SADR_SADDR1      ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SRAM start address of video data component 1.  For compression + partial SRAM, user needs to config both DRAM and SRAM detection.Use by ISP remap */
#define REMAPADR_ISP_COMPRESS_SADR_SADDR1(x)        ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_COMPRESS_SADR_SADDR1(x)    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup CMPR_SADR_END1
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_COMPRESS_SADR_EADDR1      ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0   SRAM end address of video data component 1.  For compression + partial SRAM, user needs to config both DRAM and SRAM detection.Use by ISP remap */
#define REMAPADR_ISP_COMPRESS_SADR_EADDR1(x)        ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_COMPRESS_SADR_EADDR1(x)    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup CMPR_SADR_START2
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_COMPRESS_SADR_SADDR2      ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 SRAM start address of video data component 2.  For compression + partial SRAM, user needs to config both DRAM and SRAM detection.Use by ISP remap */
#define REMAPADR_ISP_COMPRESS_SADR_SADDR2(x)        ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_COMPRESS_SADR_SADDR2(x)    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup CMPR_SADR_END2
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_COMPRESS_SADR_EADDR2      ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0   SRAM end address of video data component 2.  For compression + partial SRAM, user needs to config both DRAM and SRAM detection.Use by ISP remap */
#define REMAPADR_ISP_COMPRESS_SADR_EADDR2(x)        ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_COMPRESS_SADR_EADDR2(x)    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup CMPR_HEADER_ADR0
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_COMPRESS_HEADER_ADR0      ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 Base address of compression header for video data component 0  Use by ISP remap */
#define REMAPADR_ISP_COMPRESS_HEADER_ADR0(x)        ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_COMPRESS_HEADER_ADR0(x)    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup CMPR_HEADER_ADR1
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_COMPRESS_HEADER_ADR1      ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 Base address of compression header for video data component 1 Use by ISP remap */
#define REMAPADR_ISP_COMPRESS_HEADER_ADR1(x)        ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_COMPRESS_HEADER_ADR1(x)    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup CMPR_HEADER_ADR2
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_COMPRESS_HEADER_ADR2      ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 Base address of compression header for video data component 2Use by ISP remap */
#define REMAPADR_ISP_COMPRESS_HEADER_ADR2(x)        ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_COMPRESS_HEADER_ADR2(x)    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup DBG_RDBURST_CNT
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_DEBUG_RDBURST_CNT         ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 Count the number of Read burst from HDR in a frame. This register will be updated when the Frame endUse by ISP remap */
#define REMAPADR_ISP_DEBUG_RDBURST_CNT(x)           ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_DEBUG_RDBURST_CNT(x)       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup DBG_WRBURST_CNT
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_DEBUG_WRBURST_CNT         ((u32)0xFFFFFFFF << 0)                   /*!< R/W     0 Count the number of write burst from HDR or slice wrap in a frame. This register will be updated when the Frame endUse by ISP remap */
#define REMAPADR_ISP_DEBUG_WRBURST_CNT(x)           ((u32)(((x) & 0xFFFFFFFF << 0))
#define REMAPADR_ISP_GET_DEBUG_WRBURST_CNT(x)       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup PARTIAL_SLICEWRAP0
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_TILEHEIGHT0               ((u32)0x00000003 << 11)                  /*!< R/W     0 The tile height of component 0.'d0: 8 lines'd1: 16 lines'd2: 32 lines'd3: 64 linesCurrently, component 0 only supports 16 and 64 lines.Use by encoder remap */
#define REMAPADR_ISP_TILEHEIGHT0(x)                 ((u32)(((x) & 0x00000003 << 11))
#define REMAPADR_ISP_GET_TILEHEIGHT0(x)             ((u32)(((x >> 11) & 0x00000003)))
#define REMAPADR_ISP_MASK_TILEWIDTH0                ((u32)0x000003FF << 1)                   /*!< R/W     0 The tile width of component 0. */
#define REMAPADR_ISP_TILEWIDTH0(x)                  ((u32)(((x) & 0x000003FF << 1))
#define REMAPADR_ISP_GET_TILEWIDTH0(x)              ((u32)(((x >> 1) & 0x000003FF)))
#define REMAPADR_ISP_BIT_PARSLICEWRAPCOMP0          ((u32)0x00000001 << 0)                   /*!< R/W     0 Partial slice wrap:  1: Component 0 is involved in the partial slice wrap handshake process.0: Component 0 is not involved in the partial slice wrap handshake process.Use by encoder remap */
/** @} */


/** @defgroup PARTIAL_SLICEWRAP1
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_TILEHEIGHT1               ((u32)0x00000003 << 11)                  /*!< R/W     0 The tile height of component 1.'d0: 8 lines'd1: 16 lines'd2: 32 lines'd3: 64 linesCurrently, component 1 only supports 8 and 32 lines.Use by encoder remap */
#define REMAPADR_ISP_TILEHEIGHT1(x)                 ((u32)(((x) & 0x00000003 << 11))
#define REMAPADR_ISP_GET_TILEHEIGHT1(x)             ((u32)(((x >> 11) & 0x00000003)))
#define REMAPADR_ISP_MASK_TILEWIDTH1                ((u32)0x000003FF << 1)                   /*!< R/W     0 The tile width of component 1. */
#define REMAPADR_ISP_TILEWIDTH1(x)                  ((u32)(((x) & 0x000003FF << 1))
#define REMAPADR_ISP_GET_TILEWIDTH1(x)              ((u32)(((x >> 1) & 0x000003FF)))
#define REMAPADR_ISP_BIT_PARSLICEWRAPCOMP1          ((u32)0x00000001 << 0)                   /*!< R/W     0 Partial slice wrap:  1: Component 1 is involved in the partial slice wrap handshake process.0: Component 1 is not involved in the partial slice wrap handshake process.Use by encoder remap */
/** @} */


/** @defgroup PARTIAL_SLICEWRAP2
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_TILEHEIGHT2               ((u32)0x00000003 << 11)                  /*!< R/W     0 The tile height of component 2.'d0: 8 lines'd1: 16 lines'd2: 32 lines'd3: 64 linesCurrently, component is not used.Use by encoder remap */
#define REMAPADR_ISP_TILEHEIGHT2(x)                 ((u32)(((x) & 0x00000003 << 11))
#define REMAPADR_ISP_GET_TILEHEIGHT2(x)             ((u32)(((x >> 11) & 0x00000003)))
#define REMAPADR_ISP_MASK_TILEWIDTH2                ((u32)0x000003FF << 1)                   /*!< R/W     0 The tile width of component 2. */
#define REMAPADR_ISP_TILEWIDTH2(x)                  ((u32)(((x) & 0x000003FF << 1))
#define REMAPADR_ISP_GET_TILEWIDTH2(x)              ((u32)(((x >> 1) & 0x000003FF)))
#define REMAPADR_ISP_BIT_PARSLICEWRAPCOMP2          ((u32)0x00000001 << 0)                   /*!< R/W     0 Partial slice wrap:  1: Component 2 is involved in the partial slice wrap handshake process.0: Component 2 is not involved in the partial slice wrap handshake process.Use by encoder remap */
/** @} */


/** @defgroup PARTIAL_SLICESTRIDE0
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_MEMSTRIDE0                ((u32)0x00000FFF << 0)                   /*!< R/W     0 The stride value of component 0, for partial slice wrap. (TileWidth x ParSliceWrapNum)'d0: 1 byte'd1: 2 bytesUse by encoder remap */
#define REMAPADR_ISP_MEMSTRIDE0(x)                  ((u32)(((x) & 0x00000FFF << 0))
#define REMAPADR_ISP_GET_MEMSTRIDE0(x)              ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup PARTIAL_SLICESTRIDE1
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_MEMSTRIDE1                ((u32)0x00000FFF << 0)                   /*!< R/W     0 The stride value of component 1, for partial slice. (TileWidth x ParSliceWrapNum)'d0: 1 byte'd1: 2 bytes */
#define REMAPADR_ISP_MEMSTRIDE1(x)                  ((u32)(((x) & 0x00000FFF << 0))
#define REMAPADR_ISP_GET_MEMSTRIDE1(x)              ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup PARTIAL_SLICESTRIDE2
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_MEMSTRIDE2                ((u32)0x00000FFF << 0)                   /*!< R/W     0 The stride value of component 2, for partial slice. (TileWidth x ParSliceWrapNum)'d0: 1 byte'd1: 2 bytes */
#define REMAPADR_ISP_MEMSTRIDE2(x)                  ((u32)(((x) & 0x00000FFF << 0))
#define REMAPADR_ISP_GET_MEMSTRIDE2(x)              ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup PARTIAL_SLICETILE
 * @brief
 * @{
 */
#define REMAPADR_ISP_MASK_TILEINROW                 ((u32)0x000003FF << 10)                  /*!< R/W   0   The number of tile in one row.'d0: 1 tile'd1: 2 tileUse by encoder remap */
#define REMAPADR_ISP_TILEINROW(x)                   ((u32)(((x) & 0x000003FF << 10))
#define REMAPADR_ISP_GET_TILEINROW(x)               ((u32)(((x >> 10) & 0x000003FF)))
#define REMAPADR_ISP_MASK_PARSLICEWRAPNUM           ((u32)0x000003FF << 0)                   /*!< R/W     0 Number of tile block to wrap in partial slice wrap.  'd0: 1 tile'd1: 2 tileUse by encoder remap */
#define REMAPADR_ISP_PARSLICEWRAPNUM(x)             ((u32)(((x) & 0x000003FF << 0))
#define REMAPADR_ISP_GET_PARSLICEWRAPNUM(x)         ((u32)(((x >> 0) & 0x000003FF)))
/** @} */




/** @defgroup REMAPADR_ISP_Exported_Types REMAPADR_ISP Exported Types
* @{
*/

/**
* @brief REMAPADR_ISP Register Declaration
*/

typedef struct {
	__IO uint32_t INT_ENB;                                      /*!< Register,  Address offset:0x0000 */
	__IO uint32_t INT_MASK;                                     /*!< Register,  Address offset:0x0004 */
	__IO uint32_t HANDSHAKE_CTRL;                               /*!< Register,  Address offset:0x0008 */
	__IO uint32_t FUNC_ENB;                                     /*!< Register,  Address offset:0x000C */
	__IO uint32_t COMP_ENB;                                     /*!< Register,  Address offset:0x0010 */
	__IO uint32_t HDR_WID;                                      /*!< Register,  Address offset:0x0014 */
	__IO uint32_t HDR_RID;                                      /*!< Register,  Address offset:0x0018 */
	__IO uint32_t HDR_INFO;                                     /*!< Register,  Address offset:0x001C */
	__IO uint32_t HDR_RESET;                                    /*!< Register,  Address offset:0x0020 */
	__IO uint32_t HDR_SRAM_SADDR;                               /*!< Register,  Address offset:0x0024 */
	__IO uint32_t SLW_IN_ID;                                    /*!< Register,  Address offset:0x0028 */
	__IO uint32_t SLW_OUT_ID;                                   /*!< Register,  Address offset:0x002C */
	__IO uint32_t SLW_INFO;                                     /*!< Register,  Address offset:0x0030 */
	__IO uint32_t SLW_DRAM_CP0_SADDR;                           /*!< Register,  Address offset:0x0034 */
	__IO uint32_t SLW_DRAM_CP0_EADDR;                           /*!< Register,  Address offset:0x0038 */
	__IO uint32_t SLICE_CP0_INFO;                               /*!< Register,  Address offset:0x003C */
	__IO uint32_t SLW_DRAM_CP1_SADDR;                           /*!< Register,  Address offset:0x0040 */
	__IO uint32_t SLW_DRAM_CP1_EADDR;                           /*!< Register,  Address offset:0x0044 */
	__IO uint32_t SLICE_CP1_INFO;                               /*!< Register,  Address offset:0x0048 */
	__IO uint32_t SLW_DRAM_CP2_SADDR;                           /*!< Register,  Address offset:0x004C */
	__IO uint32_t SLW_DRAM_CP2_EADDR;                           /*!< Register,  Address offset:0x0050 */
	__IO uint32_t SLICE_CP2_INFO;                               /*!< Register,  Address offset:0x0054 */
	__IO uint32_t PARTIAL_SRAM_INFO;                            /*!< Register,  Address offset:0x0058 */
	__IO uint32_t SLICE_NUM_THRESHOLD;                          /*!< Register,  Address offset:0x005C */
	__IO uint32_t SLW_SRAM_CP0_SADDR;                           /*!< Register,  Address offset:0x0060 */
	__I  uint32_t SLW_SRAM_CP0_EADDR;                           /*!< Register,  Address offset:0x0064 */
	__IO uint32_t SLW_SRAM_CP1_SADDR;                           /*!< Register,  Address offset:0x0068 */
	__I  uint32_t SLW_SRAM_CP1_EADDR;                           /*!< Register,  Address offset:0x006C */
	__IO uint32_t SLW_SRAM_CP2_SADDR;                           /*!< Register,  Address offset:0x0070 */
	__I  uint32_t SLW_SRAM_CP2_EADDR;                           /*!< Register,  Address offset:0x0074 */
	__IO uint32_t CMPR_DADR_START0;                             /*!< Register,  Address offset:0x0078 */
	__IO uint32_t CMPR_DADR_END0;                               /*!< Register,  Address offset:0x007C */
	__IO uint32_t CMPR_DADR_START1;                             /*!< Register,  Address offset:0x0080 */
	__IO uint32_t CMPR_DADR_END1;                               /*!< Register,  Address offset:0x0084 */
	__IO uint32_t CMPR_DADR_START2;                             /*!< Register,  Address offset:0x0088 */
	__IO uint32_t CMPR_DADR_END2;                               /*!< Register,  Address offset:0x008C */
	__IO uint32_t CMPR_SADR_START0;                             /*!< Register,  Address offset:0x0090 */
	__IO uint32_t CMPR_SADR_END0;                               /*!< Register,  Address offset:0x0094 */
	__IO uint32_t CMPR_SADR_START1;                             /*!< Register,  Address offset:0x0098 */
	__IO uint32_t CMPR_SADR_END1;                               /*!< Register,  Address offset:0x009C */
	__IO uint32_t CMPR_SADR_START2;                             /*!< Register,  Address offset:0x00A0 */
	__IO uint32_t CMPR_SADR_END2;                               /*!< Register,  Address offset:0x00A4 */
	__IO uint32_t CMPR_HEADER_ADR0;                             /*!< Register,  Address offset:0x00A8 */
	__IO uint32_t CMPR_HEADER_ADR1;                             /*!< Register,  Address offset:0x00AC */
	__IO uint32_t CMPR_HEADER_ADR2;                             /*!< Register,  Address offset:0x00B0 */
	__IO uint32_t DBG_RDBURST_CNT;                              /*!< Register,  Address offset:0x00B4 */
	__IO uint32_t DBG_WRBURST_CNT;                              /*!< Register,  Address offset:0x00B8 */
	__IO uint32_t PARTIAL_SLICEWRAP0;                           /*!< Register,  Address offset:0x00BC */
	__IO uint32_t PARTIAL_SLICEWRAP1;                           /*!< Register,  Address offset:0x00C0 */
	__IO uint32_t PARTIAL_SLICEWRAP2;                           /*!< Register,  Address offset:0x00C4 */
	__IO uint32_t PARTIAL_SLICESTRIDE0;                         /*!< Register,  Address offset:0x00C8 */
	__IO uint32_t PARTIAL_SLICESTRIDE1;                         /*!< Register,  Address offset:0x00CC */
	__IO uint32_t PARTIAL_SLICESTRIDE2;                         /*!< Register,  Address offset:0x00D0 */
	__IO uint32_t PARTIAL_SLICETILE;                            /*!< Register,  Address offset:0x00D4 */
} REMAPADR_ISP_TypeDef;

#define REMAPADR_ISP_BASE ((REMAPADR_ISP_TypeDef *) (0x60100000))
/** @} */


/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your defination here


/* MANUAL_GEN_END */

/** @} */

/** @} */
#endif  // HAL_REMAP_ISP_H
