/******************************************************************************\
|* Copyright (c) 2017-2024 by Vivante Corporation.  All Rights Reserved.      *|
|*                                                                            *|
|* The material in this file is confidential and contains trade secrets of    *|
|* of Vivante Corporation.  This is proprietary information owned by Vivante  *|
|* Corporation.  No part of this work may be disclosed, reproduced, copied,   *|
|* transmitted, or used in any way for any purpose, without the express       *|
|* written permission of Vivante Corporation.                                 *|
|*                                                                            *|
\******************************************************************************/

#ifndef _NB_REGISTER_H
#define _NB_REGISTER_H

/*******************************************************************************
**                            ~~~~~~~~~~~~~~~~~~~~                            **
**                            Module AHBNeuroBrick                            **
**                            ~~~~~~~~~~~~~~~~~~~~                            **
*******************************************************************************/

/* Register gcregAHBNbConfig **
** ~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* AHBNeuroBrick Config Register  */

#define gcregAHBNbConfigRegAddrs                                          0x11E0
#define GCREG_AHB_NB_CONFIG_Address                                      0x04780
#define GCREG_AHB_NB_CONFIG_MSB                                               15
#define GCREG_AHB_NB_CONFIG_LSB                                                0
#define GCREG_AHB_NB_CONFIG_BLK                                                0
#define GCREG_AHB_NB_CONFIG_Count                                              1
#define GCREG_AHB_NB_CONFIG_FieldMask                                 0x00003FFF
#define GCREG_AHB_NB_CONFIG_ReadMask                                  0x00003FFF
#define GCREG_AHB_NB_CONFIG_WriteMask                                 0x00003FFF
#define GCREG_AHB_NB_CONFIG_ResetValue                                0x00000000

/* LayerID which this Module */
#define GCREG_AHB_NB_CONFIG_LAYER_ID                                         9:0
#define GCREG_AHB_NB_CONFIG_LAYER_ID_End                                       9
#define GCREG_AHB_NB_CONFIG_LAYER_ID_Start                                     0
#define GCREG_AHB_NB_CONFIG_LAYER_ID_Type                                    U10

/* Value Type Define */
#define GCREG_AHB_NB_CONFIG_VALUE_TYPE                                     12:10
#define GCREG_AHB_NB_CONFIG_VALUE_TYPE_End                                    12
#define GCREG_AHB_NB_CONFIG_VALUE_TYPE_Start                                  10
#define GCREG_AHB_NB_CONFIG_VALUE_TYPE_Type                                  U03
#define   GCREG_AHB_NB_CONFIG_VALUE_TYPE_COMMAND                             0x0
#define   GCREG_AHB_NB_CONFIG_VALUE_TYPE_KERNEL_BIT_STREAM                   0x1
#define   GCREG_AHB_NB_CONFIG_VALUE_TYPE_KERNEL_BIAS_BIT_STREAM              0x2
#define   GCREG_AHB_NB_CONFIG_VALUE_TYPE_LOOK_UP_TABLE                       0x3
#define   GCREG_AHB_NB_CONFIG_VALUE_TYPE_POST_MULT_SHIFT                     0x4
#define   GCREG_AHB_NB_CONFIG_VALUE_TYPE_KERNEL_BS_ADDRESS                   0x5
#define   GCREG_AHB_NB_CONFIG_VALUE_TYPE_RESERVED0                           0x6
#define   GCREG_AHB_NB_CONFIG_VALUE_TYPE_RESERVED1                           0x7

/* 1:all software state load programming is done */
#define GCREG_AHB_NB_CONFIG_PREPARED                                       13:13
#define GCREG_AHB_NB_CONFIG_PREPARED_End                                      13
#define GCREG_AHB_NB_CONFIG_PREPARED_Start                                    13
#define GCREG_AHB_NB_CONFIG_PREPARED_Type                                    U01

/* Register gcregAHBNbData **
** ~~~~~~~~~~~~~~~~~~~~~~~ */

/* AHBNeuroBrick Config Value  A config value is followd by ConfigReg */

#define gcregAHBNbDataRegAddrs                                            0x11E1
#define GCREG_AHB_NB_DATA_Address                                        0x04784
#define GCREG_AHB_NB_DATA_MSB                                                 15
#define GCREG_AHB_NB_DATA_LSB                                                  0
#define GCREG_AHB_NB_DATA_BLK                                                  0
#define GCREG_AHB_NB_DATA_Count                                                1
#define GCREG_AHB_NB_DATA_FieldMask                                   0xFFFFFFFF
#define GCREG_AHB_NB_DATA_ReadMask                                    0xFFFFFFFF
#define GCREG_AHB_NB_DATA_WriteMask                                   0xFFFFFFFF
#define GCREG_AHB_NB_DATA_ResetValue                                  0x00000000

#define GCREG_AHB_NB_DATA_VALUE                                             31:0
#define GCREG_AHB_NB_DATA_VALUE_End                                           31
#define GCREG_AHB_NB_DATA_VALUE_Start                                          0
#define GCREG_AHB_NB_DATA_VALUE_Type                                         U32

/* Register gcregQueryStatus **
** ~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Register for debugging FPGA */

#define gcregQueryStatusRegAddrs                                          0x11E3
#define GCREG_QUERY_STATUS_Address                                       0x0478C
#define GCREG_QUERY_STATUS_MSB                                                15
#define GCREG_QUERY_STATUS_LSB                                                 0
#define GCREG_QUERY_STATUS_BLK                                                 0
#define GCREG_QUERY_STATUS_Count                                               1
#define GCREG_QUERY_STATUS_FieldMask                                  0x00010FFF
#define GCREG_QUERY_STATUS_ReadMask                                   0x00010FFF
#define GCREG_QUERY_STATUS_WriteMask                                  0x00010FFF
#define GCREG_QUERY_STATUS_ResetValue                                 0x00000000

/* Write only. 0: Frame done. Query if hardware finish one frame, if status   **
** is 1, software can read data. 1: Busy Status of one layer with LayerID     **
** defined in bits[9:0]. 2,3 reserved not used.                               */
#define GCREG_QUERY_STATUS_QUERY_TYPE                                        1:0
#define GCREG_QUERY_STATUS_QUERY_TYPE_End                                      1
#define GCREG_QUERY_STATUS_QUERY_TYPE_Start                                    0
#define GCREG_QUERY_STATUS_QUERY_TYPE_Type                                   U02

/* Write only, LayerID of layed want to process, from 0, max support 1024     **
** layers. When QueryType is Frame Done, LayerID not used.                    */
#define GCREG_QUERY_STATUS_LAYER_ID                                         11:2
#define GCREG_QUERY_STATUS_LAYER_ID_End                                       11
#define GCREG_QUERY_STATUS_LAYER_ID_Start                                      2
#define GCREG_QUERY_STATUS_LAYER_ID_Type                                     U10

/* 1:idle, 0:busy */
/* Value 1: Frame done if QueryType 0. Layed(with LayerID) is busy. Value 0: */
#define GCREG_QUERY_STATUS_STATUS                                          16:16
#define GCREG_QUERY_STATUS_STATUS_End                                         16
#define GCREG_QUERY_STATUS_STATUS_Start                                       16
#define GCREG_QUERY_STATUS_STATUS_Type                                       U01

/* AHBNeuroBrick. Register for HardWare Control */
#define gcregAHBNbHWControlRegAddrs                                       0x11E2
#define GCREG_AHB_NB_HW_CONTROL_Address                                  0x04788
#define GCREG_AHB_NB_HW_CONTROL_MSB                                           15
#define GCREG_AHB_NB_HW_CONTROL_LSB                                            0
#define GCREG_AHB_NB_HW_CONTROL_BLK                                            0
#define GCREG_AHB_NB_HW_CONTROL_Count                                          1
#define GCREG_AHB_NB_HW_CONTROL_FieldMask                             0x00000003
#define GCREG_AHB_NB_HW_CONTROL_ReadMask                              0x00000003
#define GCREG_AHB_NB_HW_CONTROL_WriteMask                             0x00000003
#define GCREG_AHB_NB_HW_CONTROL_ResetValue                            0x00000000

/*write 1 and then write 0, trigger soft reset */
/* Software Reset. Reset all modules when HW gets this bits */
#define GCREG_AHB_NB_HW_CONTROL_SOFT_RESET                                   0:0
#define GCREG_AHB_NB_HW_CONTROL_SOFT_RESET_End                                 0
#define GCREG_AHB_NB_HW_CONTROL_SOFT_RESET_Start                               0
#define GCREG_AHB_NB_HW_CONTROL_SOFT_RESET_Type                              U01

/* Disable Clock Gating for all HW modules debug usage. Default 0. */
#define GCREG_AHB_NB_HW_CONTROL_DISABLE_CLOCK_GATE                           1:1
#define GCREG_AHB_NB_HW_CONTROL_DISABLE_CLOCK_GATE_End                         1
#define GCREG_AHB_NB_HW_CONTROL_DISABLE_CLOCK_GATE_Start                       1
#define GCREG_AHB_NB_HW_CONTROL_DISABLE_CLOCK_GATE_Type                      U01

/*******************************************************************************
**                             ~~~~~~~~~~~~~~~~~~                             **
**                             Command gcregNBDMA                             **
**                             ~~~~~~~~~~~~~~~~~~                             **
*******************************************************************************/

/* Offset gcregNBDMAWord0
** ~~~~~~~~~~~~~~~~~~~~~~ */

/* Tensor X Size in pixel, max support 8192 */
#define GCREG_NBDMA_WORD0_TENSOR_XSIZE_MINUS1                               12:0
#define GCREG_NBDMA_WORD0_TENSOR_XSIZE_MINUS1_End                             12
#define GCREG_NBDMA_WORD0_TENSOR_XSIZE_MINUS1_Start                            0
#define GCREG_NBDMA_WORD0_TENSOR_XSIZE_MINUS1_Type                           U13

/* Tensor Y size in Pixel, max suport 8192 */
#define GCREG_NBDMA_WORD0_TENSOR_YSIZE_MINUS1                              25:13
#define GCREG_NBDMA_WORD0_TENSOR_YSIZE_MINUS1_End                             25
#define GCREG_NBDMA_WORD0_TENSOR_YSIZE_MINUS1_Start                           13
#define GCREG_NBDMA_WORD0_TENSOR_YSIZE_MINUS1_Type                           U13

/* 0: one BPP, 1:two BPP, 2: four BPP */
#define GCREG_NBDMA_WORD0_TENSOR_BPP                                       27:26
#define GCREG_NBDMA_WORD0_TENSOR_BPP_End                                      27
#define GCREG_NBDMA_WORD0_TENSOR_BPP_Start                                    26
#define GCREG_NBDMA_WORD0_TENSOR_BPP_Type                                    U02

/* Offset gcregNBDMAWord1
** ~~~~~~~~~~~~~~~~~~~~~~ */

/* Tensor Z size  */
#define GCREG_NBDMA_WORD1_TENSOR_ZSIZE                                      19:0
#define GCREG_NBDMA_WORD1_TENSOR_ZSIZE_End                                    19
#define GCREG_NBDMA_WORD1_TENSOR_ZSIZE_Start                                   0
#define GCREG_NBDMA_WORD1_TENSOR_ZSIZE_Type                                  U20

/* Offset gcregNBDMAWord2
** ~~~~~~~~~~~~~~~~~~~~~~ */

/* DMA Base Address. */
#define GCREG_NBDMA_WORD2_BASE_ADDR                                         31:0
#define GCREG_NBDMA_WORD2_BASE_ADDR_End                                       31
#define GCREG_NBDMA_WORD2_BASE_ADDR_Start                                      0
#define GCREG_NBDMA_WORD2_BASE_ADDR_Type                                     U32

/* Offset gcregNBDMAWord3
** ~~~~~~~~~~~~~~~~~~~~~~ */

/* DMA Stride. */
#define GCREG_NBDMA_WORD3_DMA_STRIDE                                        31:0
#define GCREG_NBDMA_WORD3_DMA_STRIDE_End                                      31
#define GCREG_NBDMA_WORD3_DMA_STRIDE_Start                                     0
#define GCREG_NBDMA_WORD3_DMA_STRIDE_Type                                    U32


#define __gcmSTART(reg_field) \
    (0 ? reg_field)

#define __gcmEND(reg_field) \
    (1 ? reg_field)

#define __gcmGETSIZE(reg_field) \
    (__gcmEND(reg_field) - __gcmSTART(reg_field) + 1)

#define __gcmALIGN(data, reg_field) \
    (((nb_uint32_t) (data)) << __gcmSTART(reg_field))

#define __gcmMASK(reg_field) \
    ((nb_uint32_t) ((__gcmGETSIZE(reg_field) == 32) \
        ?  ~0U \
        : (~(~0U << __gcmGETSIZE(reg_field)))))

/*******************************************************************************
**
**  gcmSETFIELD
**
**      Set the value of a field within specified data.
**
**  ARGUMENTS:
**
**      data    Data value.
**      reg     Name of register.
**      field   Name of field within register.
**      value   Value for field.
*/
#define gcmSETFIELD(data, reg, field, value) \
    ( \
    (((nb_uint32_t) (data)) \
        & ~__gcmALIGN(__gcmMASK(reg##_##field), reg##_##field)) \
        |  __gcmALIGN((nb_uint32_t) (value) \
            & __gcmMASK(reg##_##field), reg##_##field) \
    )

#endif
