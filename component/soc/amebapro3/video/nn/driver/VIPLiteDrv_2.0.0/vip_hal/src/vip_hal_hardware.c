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

#include <vip_hal_hardware.h>
#include <vip_hal_os_port.h>
#include <vip_drv_share.h>


#define vpmdENABLE_TRSP2_LOG      0

typedef struct _trsp2_instr_info
{
    vip_uint32_t wordSize;
    vip_uint32_t inImageXSize;
    vip_uint32_t inImageYSize;
    vip_uint32_t inImageZSize;
    vip_uint32_t inImageStride;
    vip_uint64_t inImageSlice;
    vip_uint32_t inImageAddress;
    vip_int32_t  inImageXOffset;
    vip_int32_t  inImageYOffset;
    vip_uint32_t outImageXSize;
    vip_uint32_t outImageYSize;
    vip_uint32_t outImageZSize;
    vip_uint32_t outImageStride;
    vip_uint64_t outImageSlice;
    vip_uint32_t outImageAddress;
    vip_uint32_t inSramStride;
    vip_uint32_t outSramStride;
    vip_int32_t  sendOverlap2LeftCore;
    vip_int32_t  sendOverlap2RightCore;
    vip_int32_t  receiveOverlapFromLeftCore;
    vip_int32_t  receiveOverlapFromRightCore;

    vip_uint8_t  clearInputEvent;
    vip_uint8_t  waitForInputEvent;
    vip_uint8_t  inputEventID;
    vip_uint8_t  clearOutputEvent;
    vip_uint8_t  waitForOutputEvent;
    vip_uint8_t  outputEventID;
    vip_uint8_t  frameStart;
    vip_uint32_t imageFetchBlockControl;
    vip_uint32_t pad;

    vip_int32_t  inImageBorderConst;
    vip_uint32_t inImageTransposeBufStartAddr;
    vip_uint32_t inImageTransposeBufEndAddr;
    vip_uint32_t outImageTransposeBufStartAddr;
    vip_uint32_t outImageTransposeBufEndAddr;
    vip_uint32_t inImageCircularBufSize;
    vip_uint32_t inImageCircularBufEndAddrPlus1;
    vip_uint32_t outImageCircularBufSize;
    vip_uint32_t outImageCircularBufEndAddrPlus1;
    vip_uint32_t noFlush;
    vip_uint32_t nn_layer_flush;
    vip_uint8_t  hwTileWalkMode;
    vip_uint8_t  nnInImageAxiIdOffset;
    vip_uint8_t  nnOutImageAxiIdOffset;
    vip_uint8_t  nnKernelAxiIdOffset;
    vip_uint8_t  nn2ndImageAxiIdOffset;
    vip_uint32_t inImageBaseAddressInc;
    vip_uint32_t secInimageBaseAddressInc;
    vip_uint32_t outImageBaseAddressInc;
    vip_uint8_t  inImageAddressOffsetId;
    vip_uint8_t  secInimageAddressOffsetId;
    vip_uint8_t  outImageAddressOffsetId;
    vip_uint8_t  kernelAddressOffsetId;
} trsp2_instr_info_t;

static vip_status_e generate_trsp2_instr_info(
    trsp2_instr_info_t *info,
    vip_uint32_t src_physical,
    vip_uint32_t dst_physical,
    vip_uint32_t data_size,
    viphal_hardware_trsp2_type trsp2_type
)
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t wordSize = 1;
    vip_uint32_t inSramStride = 0;
    vip_uint32_t outSramStride = 0;
    vip_uint32_t inImageTransposeBufStartAddr = 0x0;
    vip_uint32_t inImageTransposeBufEndAddr = 0x0;
    vip_uint32_t outImageTransposeBufStartAddr = 0x0;
    vip_uint32_t outImageTransposeBufEndAddr = 0x0;
    vip_uint32_t inImageCircularBufSize = 0x0;
    vip_uint32_t inImageCircularBufEndAddrPlus1 = 0xFFFFFFFF;
    vip_uint32_t outImageCircularBufSize = 0x0;
    vip_uint32_t outImageCircularBufEndAddrPlus1 = 0xFFFFFFFF;
    vip_uint32_t vipsram_width = viphal_query_hw_feature(VIP_HW_FEATURE_VIPSRAM_WIDTH);
    vip_uint32_t vipddr_burst = viphal_query_hw_feature(VIP_HW_FEATURE_DDR_KERNEL_BURST_SIZE);
    vip_uint32_t tmp = ((data_size > vipddr_burst ? data_size : vipddr_burst) + vipsram_width - 1) / vipsram_width;
    vip_uint32_t min_buffer_size = (tmp * 2 > 3 ? tmp * 2 : 3) * vipsram_width;

    /* VIPHAL_HARDWARE_TRSP2_DDR_TO_VIPSRAM: copy data from buffer to sram */
    if (VIPHAL_HARDWARE_TRSP2_DDR_TO_VIPSRAM == trsp2_type) {
        inSramStride = 1;
        outSramStride = 0;
        inImageTransposeBufStartAddr = 0;
        inImageTransposeBufEndAddr = min_buffer_size;
        outImageTransposeBufStartAddr = 0;
        outImageTransposeBufEndAddr = 0;
        inImageCircularBufSize = 0x0;
        inImageCircularBufEndAddrPlus1 = 0xFFFFFFFF;
        outImageCircularBufSize = data_size;
        outImageCircularBufEndAddrPlus1 = 0xFFFFFFFF;
    }
    else if (VIPHAL_HARDWARE_TRSP2_VIPSRAM_TO_DDR == trsp2_type) {
        inSramStride = 0;
        outSramStride = data_size;
        inImageTransposeBufStartAddr = 0x0;
        inImageTransposeBufEndAddr = 0x0;
        outImageTransposeBufStartAddr = 0;
        outImageTransposeBufEndAddr = min_buffer_size;
        inImageCircularBufSize = data_size;
        inImageCircularBufEndAddrPlus1 = 0xFFFFFFFF;
        outImageCircularBufSize = 0x0;
        outImageCircularBufEndAddrPlus1 = 0xFFFFFFFF;
    }
    else {
        vip_error("fail not support this trsp2 trpe=%d\n", trsp2_type);
        return VIP_ERROR_FAILURE;
    }

    info->nn_layer_flush = 1;
    info->wordSize = wordSize;
    info->imageFetchBlockControl = 0;
    info->inImageXSize = 1;
    info->inImageYSize = data_size / 1 / wordSize;
    info->inImageZSize = 1;
    info->inImageStride = 1;
    info->inImageSlice = data_size;
    info->inImageAddress = src_physical;
    info->inImageXOffset = 0;
    info->inImageYOffset = 0;
    info->outImageXSize = data_size / 1 / wordSize;
    info->outImageYSize = 1;
    info->outImageZSize = 1;
    info->outImageStride = 1;
    info->outImageSlice = data_size;
    info->outImageAddress = dst_physical;
    info->inSramStride = inSramStride;
    info->outSramStride = outSramStride;
    info->inImageTransposeBufStartAddr = inImageTransposeBufStartAddr;
    info->inImageTransposeBufEndAddr = inImageTransposeBufEndAddr;
    info->outImageTransposeBufStartAddr = outImageTransposeBufStartAddr;
    info->outImageTransposeBufEndAddr = outImageTransposeBufEndAddr;
    info->inImageCircularBufSize = inImageCircularBufSize;
    info->inImageCircularBufEndAddrPlus1 = inImageCircularBufEndAddrPlus1;
    info->outImageCircularBufSize = outImageCircularBufSize;
    info->outImageCircularBufEndAddrPlus1 = outImageCircularBufEndAddrPlus1;

    vip_info("src_physical=0x%x dst_physical=0x%x data_size=%d instr_type=preload %s\n",
                 src_physical, dst_physical, data_size,
                 (trsp2_type == VIPHAL_HARDWARE_TRSP2_DDR_TO_VIPSRAM) ? "in" :
                 (trsp2_type == VIPHAL_HARDWARE_TRSP2_VIPSRAM_TO_DDR) ? "out" : "unkown");
#if vpmdENABLE_TRSP2_LOG
    {
        vip_print("===========================\n");
        vip_print("**********Show Command Start*****\n");
        vip_print("===========================\n");

        vip_print("wordSize:%d \n", info->wordSize);
        vip_print("imageFetchBlockControl:%d \n", info->imageFetchBlockControl);
        vip_print("inImageXSize:%d \n", info->inImageXSize);
        vip_print("inImageYSize:%d \n", info->inImageYSize);
        vip_print("inImageZSize:%d \n", info->inImageZSize);
        vip_print("inImageStride:%d \n", info->inImageStride);
        vip_print("inImageSlice:%d \n", info->inImageSlice);
        vip_print("inImageAddress:0x%08x \n", info->inImageAddress);
        vip_print("inImageXOffset:%d \n", info->inImageXOffset);
        vip_print("inImageYOffset:%d \n", info->inImageYOffset);
        vip_print("outImageXSize:%d \n", info->outImageXSize);
        vip_print("outImageYSize:%d \n", info->outImageYSize);
        vip_print("outImageZSize:%d \n", info->outImageZSize);
        vip_print("outImageStride:%d \n", info->outImageStride);
        vip_print("outImageSlice:%d \n", info->outImageSlice);
        vip_print("outImageAddress:0x%08x \n", info->outImageAddress);
        vip_print("inSramStride:%d \n", info->inSramStride);
        vip_print("outSramStride:%d \n", info->outSramStride);
        vip_print("inImageBorderConst:%d \n", info->inImageBorderConst);
        vip_print("waitForInputEvent:%d \n", info->waitForInputEvent);
        vip_print("clearInputEvent:%d \n", info->clearInputEvent);
        vip_print("inputEventID:%d \n", info->inputEventID);
        vip_print("waitForOutputEvent:%d \n", info->waitForOutputEvent);
        vip_print("clearOutputEvent:%d \n", info->clearOutputEvent);
        vip_print("outputEventID:%d \n", info->outputEventID);
        vip_print("frameStart:%d \n", info->frameStart);
        vip_print("inImageTransposeBufStartAddr:0x%08x \n", info->inImageTransposeBufStartAddr);
        vip_print("inImageTransposeBufEndAddr:0x%08x \n", info->inImageTransposeBufEndAddr);
        vip_print("outImageTransposeBufStartAddr:0x%08x \n", info->outImageTransposeBufStartAddr);
        vip_print("outImageTransposeBufEndAddr:0x%08x \n", info->outImageTransposeBufEndAddr);
        vip_print("inImageCircularBufSize:%d \n", info->inImageCircularBufSize);
        vip_print("inImageCircularBufEndAddrPlus1:0x%08x \n", info->inImageCircularBufEndAddrPlus1);
        vip_print("outImageCircularBufSize:%d \n", info->outImageCircularBufSize);
        vip_print("outImageCircularBufEndAddrPlus1:0x%08x \n", info->outImageCircularBufEndAddrPlus1);

        vip_print("===============================\n");
        vip_print("**********Show Command End*****\n");
        vip_print("===============================\n");
    }
#endif

    return status;
}

vip_status_e generate_trsp2_instr(
    trsp2_instr_info_t *info,
    vip_uint32_t *buffer,
    vip_uint32_t *size
)
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t *memory = VIP_NULL;
    vip_uint32_t num = 0;

    memory = buffer;

    /* gcregNNInstWord0 */
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 19:6) - (0 ?
 19:6) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:6) - (0 ?
 19:6) + 1))) - 1)))))) << (0 ?
 19:6))) | (((vip_uint32_t) ((vip_uint32_t) (((info->inImageYSize >> 13) & 0x1FFF)) & ((vip_uint32_t) ((((1 ?
 19:6) - (0 ?
 19:6) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 19:6) - (0 ? 19:6) + 1))) - 1)))))) << (0 ? 19:6)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 26:20) - (0 ?
 26:20) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:20) - (0 ?
 26:20) + 1))) - 1)))))) << (0 ?
 26:20))) | (((vip_uint32_t) ((vip_uint32_t) ((info->outImageYSize >> 20 ) & 0x3F) & ((vip_uint32_t) ((((1 ?
 26:20) - (0 ?
 26:20) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 26:20) - (0 ? 26:20) + 1))) - 1)))))) << (0 ? 26:20)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 5:2) - (0 ?
 5:2) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 5:2) - (0 ?
 5:2) + 1))) - 1)))))) << (0 ?
 5:2))) | (((vip_uint32_t) ((vip_uint32_t) ((info->inImageXSize >> 22) & 0xF) & ((vip_uint32_t) ((((1 ?
 5:2) - (0 ?
 5:2) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 5:2) - (0 ? 5:2) + 1))) - 1)))))) << (0 ? 5:2)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:31) - (0 ?
 31:31) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:31) - (0 ?
 31:31) + 1))) - 1)))))) << (0 ?
 31:31))) | (((vip_uint32_t) ((vip_uint32_t) (info->nn_layer_flush) & ((vip_uint32_t) ((((1 ?
 31:31) - (0 ?
 31:31) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:31) - (0 ? 31:31) + 1))) - 1)))))) << (0 ? 31:31)));

    /* gcregNNInstWord1 */
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 18:6) - (0 ?
 18:6) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 18:6) - (0 ?
 18:6) + 1))) - 1)))))) << (0 ?
 18:6))) | (((vip_uint32_t) ((vip_uint32_t) (info->inImageXSize & 0x1FFF) & ((vip_uint32_t) ((((1 ?
 18:6) - (0 ?
 18:6) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 18:6) - (0 ? 18:6) + 1))) - 1)))))) << (0 ? 18:6)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:19) - (0 ?
 31:19) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:19) - (0 ?
 31:19) + 1))) - 1)))))) << (0 ?
 31:19))) | (((vip_uint32_t) ((vip_uint32_t) (info->inImageYSize) & ((vip_uint32_t) ((((1 ?
 31:19) - (0 ?
 31:19) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 31:19) - (0 ? 31:19) + 1))) - 1)))))) << (0 ? 31:19)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 1:1) - (0 ?
 1:1) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 1:1) - (0 ?
 1:1) + 1))) - 1)))))) << (0 ?
 1:1))) | (((vip_uint32_t) ((vip_uint32_t) ((info->inImageXSize >> 18) & 0x1) & ((vip_uint32_t) ((((1 ?
 1:1) - (0 ?
 1:1) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 1:1) - (0 ? 1:1) + 1))) - 1)))))) << (0 ? 1:1)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 0:0) - (0 ?
 0:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 0:0) - (0 ?
 0:0) + 1))) - 1)))))) << (0 ?
 0:0))) | (((vip_uint32_t) ((vip_uint32_t) ((info->inImageXSize >> 17) & 0x1) & ((vip_uint32_t) ((((1 ?
 0:0) - (0 ?
 0:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 0:0) - (0 ? 0:0) + 1))) - 1)))))) << (0 ? 0:0)));
    /* gcregNNInstWord2 */
    *memory++ =  ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 2:0) - (0 ?
 2:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 2:0) - (0 ?
 2:0) + 1))) - 1)))))) << (0 ?
 2:0))) | (((vip_uint32_t) ((vip_uint32_t) (info->inImageXOffset) & ((vip_uint32_t) ((((1 ?
 2:0) - (0 ?
 2:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 2:0) - (0 ? 2:0) + 1))) - 1)))))) << (0 ? 2:0)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 5:3) - (0 ?
 5:3) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 5:3) - (0 ?
 5:3) + 1))) - 1)))))) << (0 ?
 5:3))) | (((vip_uint32_t) ((vip_uint32_t) (info->inImageYOffset) & ((vip_uint32_t) ((((1 ?
 5:3) - (0 ?
 5:3) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 5:3) - (0 ? 5:3) + 1))) - 1)))))) << (0 ? 5:3)));
    /* gcregNNInstWord3 */
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 18:6) - (0 ?
 18:6) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 18:6) - (0 ?
 18:6) + 1))) - 1)))))) << (0 ?
 18:6))) | (((vip_uint32_t) ((vip_uint32_t) (info->outImageXSize) & ((vip_uint32_t) ((((1 ?
 18:6) - (0 ?
 18:6) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 18:6) - (0 ? 18:6) + 1))) - 1)))))) << (0 ? 18:6)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:19) - (0 ?
 31:19) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:19) - (0 ?
 31:19) + 1))) - 1)))))) << (0 ?
 31:19))) | (((vip_uint32_t) ((vip_uint32_t) (info->outImageYSize) & ((vip_uint32_t) ((((1 ?
 31:19) - (0 ?
 31:19) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 31:19) - (0 ? 31:19) + 1))) - 1)))))) << (0 ? 31:19)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 3:3) - (0 ?
 3:3) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 3:3) - (0 ?
 3:3) + 1))) - 1)))))) << (0 ?
 3:3))) | (((vip_uint32_t) ((vip_uint32_t) (info->noFlush) & ((vip_uint32_t) ((((1 ?
 3:3) - (0 ?
 3:3) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 3:3) - (0 ? 3:3) + 1))) - 1)))))) << (0 ? 3:3)));

    /* gcregNNInstWord4 */
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 13:0) - (0 ?
 13:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 13:0) - (0 ?
 13:0) + 1))) - 1)))))) << (0 ?
 13:0))) | (((vip_uint32_t) ((vip_uint32_t) (info->outImageZSize) & ((vip_uint32_t) ((((1 ?
 13:0) - (0 ?
 13:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 13:0) - (0 ? 13:0) + 1))) - 1)))))) << (0 ? 13:0)))
            /* the bit 7 of outImageTileXSize is in word28 bit 31 */
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 24:18) - (0 ?
 24:18) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 24:18) - (0 ?
 24:18) + 1))) - 1)))))) << (0 ?
 24:18))) | (((vip_uint32_t) ((vip_uint32_t) ((info->outImageXSize >> 13) & 0x7F) & ((vip_uint32_t) ((((1 ?
 24:18) - (0 ?
 24:18) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 24:18) - (0 ? 24:18) + 1))) - 1)))))) << (0 ? 24:18)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:25) - (0 ?
 31:25) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:25) - (0 ?
 31:25) + 1))) - 1)))))) << (0 ?
 31:25))) | (((vip_uint32_t) ((vip_uint32_t) ((info->outImageYSize >> 13 )& 0x7F) & ((vip_uint32_t) ((((1 ?
 31:25) - (0 ?
 31:25) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 31:25) - (0 ? 31:25) + 1))) - 1)))))) << (0 ? 31:25)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 16:16) - (0 ?
 16:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 16:16) - (0 ?
 16:16) + 1))) - 1)))))) << (0 ?
 16:16))) | (((vip_uint32_t) ((vip_uint32_t) ((info->inImageXOffset >> 3) & 0x1) & ((vip_uint32_t) ((((1 ?
 16:16) - (0 ?
 16:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 16:16) - (0 ? 16:16) + 1))) - 1)))))) << (0 ? 16:16)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 17:17) - (0 ?
 17:17) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 17:17) - (0 ?
 17:17) + 1))) - 1)))))) << (0 ?
 17:17))) | (((vip_uint32_t) ((vip_uint32_t) ((info->inImageYOffset >> 3) & 0x1) & ((vip_uint32_t) ((((1 ?
 17:17) - (0 ?
 17:17) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 17:17) - (0 ? 17:17) + 1))) - 1)))))) << (0 ? 17:17)));

    /* gcregNNInstWord5 */
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:0) - (0 ?
 25:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:0) - (0 ?
 25:0) + 1))) - 1)))))) << (0 ?
 25:0))) | (((vip_uint32_t) ((vip_uint32_t) ((info->wordSize)) & ((vip_uint32_t) ((((1 ?
 25:0) - (0 ?
 25:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 25:0) - (0 ? 25:0) + 1))) - 1)))))) << (0 ? 25:0)));

    /* gcregNNInstWord6 */
    *memory++ = (vip_uint32_t)info->inImageAddress;
    /* gcregNNInstWord7 */
    *memory++ = (vip_uint32_t)info->outImageAddress;

    /* GCREG_NN_INST_WORD8*/
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 3:2) - (0 ?
 3:2) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 3:2) - (0 ?
 3:2) + 1))) - 1)))))) << (0 ?
 3:2))) | (((vip_uint32_t) ((vip_uint32_t) ((info->inImageXSize) >> 20 & 0x3) & ((vip_uint32_t) ((((1 ?
 3:2) - (0 ?
 3:2) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 3:2) - (0 ? 3:2) + 1))) - 1)))))) << (0 ? 3:2)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:16) - (0 ?
 31:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:16) - (0 ?
 31:16) + 1))) - 1)))))) << (0 ?
 31:16))) | (((vip_uint32_t) ((vip_uint32_t) (info->outImageSlice) & ((vip_uint32_t) ((((1 ?
 31:16) - (0 ?
 31:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:16) - (0 ? 31:16) + 1))) - 1)))))) << (0 ? 31:16)));

    /* 31:0 */
    *memory++ = info->inSramStride;

    /* 31:0 */
    *memory++ = info->outSramStride;

    /* 31:0 [0:11][12:23][24:27]*/
    *memory++ = (((info->inImageStride >> 16) & 0xFFF) | (((info->inImageSlice >> 16) & 0xFFF) << 12) |
            (((info->inImageXSize >> 13) & 0xF) << 24));

    /* CREG_NN_INST_WORD12_KERNEL_CACHE_END_ADDRESS */
    *memory++ = (((info->outImageStride >> 16) & 0xFFF) | (((info->outImageSlice >> 16) & 0xFFF) << 12) |
            (((info->outImageXSize >> 21) & 0xF) <<24));

    /* 31:0 */
    *memory++ = (info->inImageTransposeBufStartAddr>>4);

    /* GCREG_NN_INST_WORD14, IMAGE_END_ADDRESS */
    *memory++ = (info->inImageTransposeBufEndAddr>>4);

    /*1:0*/
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 17:2) - (0 ?
 17:2) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 17:2) - (0 ?
 17:2) + 1))) - 1)))))) << (0 ?
 17:2))) | (((vip_uint32_t) ((vip_uint32_t) (info->inImageBorderConst) & ((vip_uint32_t) ((((1 ?
 17:2) - (0 ?
 17:2) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 17:2) - (0 ? 17:2) + 1))) - 1)))))) << (0 ? 17:2)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 19:19) - (0 ?
 19:19) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:19) - (0 ?
 19:19) + 1))) - 1)))))) << (0 ?
 19:19))) | (((vip_uint32_t) ((vip_uint32_t) ((info->inImageXSize) >> 19 & 0x1) & ((vip_uint32_t) ((((1 ?
 19:19) - (0 ?
 19:19) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:19) - (0 ? 19:19) + 1))) - 1)))))) << (0 ? 19:19)));

    /*GCREG_NN_INST_WORD16*/
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (info->inImageStride & 0xFFFF) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:16) - (0 ?
 31:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:16) - (0 ?
 31:16) + 1))) - 1)))))) << (0 ?
 31:16))) | (((vip_uint32_t) ((vip_uint32_t) (info->inImageSlice & 0xFFFF) & ((vip_uint32_t) ((((1 ?
 31:16) - (0 ?
 31:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:16) - (0 ? 31:16) + 1))) - 1)))))) << (0 ? 31:16)));

    /*GCREG_NN_INST_WORD17*/
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (info->outImageStride) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0)));


    /*GCREG_NN_INST_WORD18*/
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:0) - (0 ?
 25:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:0) - (0 ?
 25:0) + 1))) - 1)))))) << (0 ?
 25:0))) | (((vip_uint32_t) ((vip_uint32_t) (info->outImageCircularBufSize >> 6) & ((vip_uint32_t) ((((1 ?
 25:0) - (0 ?
 25:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 25:0) - (0 ? 25:0) + 1))) - 1)))))) << (0 ? 25:0)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 29:29) - (0 ?
 29:29) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 29:29) - (0 ?
 29:29) + 1))) - 1)))))) << (0 ?
 29:29))) | (((vip_uint32_t) ((vip_uint32_t) ((info->inImageXOffset >> 4) & 0x1) & ((vip_uint32_t) ((((1 ?
 29:29) - (0 ?
 29:29) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 29:29) - (0 ? 29:29) + 1))) - 1)))))) << (0 ? 29:29)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 30:30) - (0 ?
 30:30) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 30:30) - (0 ?
 30:30) + 1))) - 1)))))) << (0 ?
 30:30))) | (((vip_uint32_t) ((vip_uint32_t) ((info->inImageYOffset >> 4) & 0x1) & ((vip_uint32_t) ((((1 ?
 30:30) - (0 ?
 30:30) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 30:30) - (0 ? 30:30) + 1))) - 1)))))) << (0 ? 30:30)));

    /*25:0*/
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:0) - (0 ?
 25:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:0) - (0 ?
 25:0) + 1))) - 1)))))) << (0 ?
 25:0))) | (((vip_uint32_t) ((vip_uint32_t) (info->outImageCircularBufEndAddrPlus1 >> 6) & ((vip_uint32_t) ((((1 ?
 25:0) - (0 ?
 25:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 25:0) - (0 ? 25:0) + 1))) - 1)))))) << (0 ? 25:0)));

    /*GCREG_NN_INST_WORD20*/
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:0) - (0 ?
 25:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:0) - (0 ?
 25:0) + 1))) - 1)))))) << (0 ?
 25:0))) | (((vip_uint32_t) ((vip_uint32_t) (info->inImageCircularBufSize >> 6) & ((vip_uint32_t) ((((1 ?
 25:0) - (0 ?
 25:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 25:0) - (0 ? 25:0) + 1))) - 1)))))) << (0 ? 25:0)));

    /*25:0*/
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:0) - (0 ?
 25:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:0) - (0 ?
 25:0) + 1))) - 1)))))) << (0 ?
 25:0))) | (((vip_uint32_t) ((vip_uint32_t) (info->inImageCircularBufEndAddrPlus1 >> 6) & ((vip_uint32_t) ((((1 ?
 25:0) - (0 ?
 25:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 25:0) - (0 ? 25:0) + 1))) - 1)))))) << (0 ? 25:0)));

    /*GCREG_NN_INST_WORD22*/
    *memory++ = 0;

    /*GCREG_NN_INST_WORD23*/
    *memory++ = 0;

    /*GCREG_NN_INST_WORD24*/
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:4) - (0 ?
 31:4) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:4) - (0 ?
 31:4) + 1))) - 1)))))) << (0 ?
 31:4))) | (((vip_uint32_t) ((vip_uint32_t) (info->outImageTransposeBufStartAddr >> 4) & ((vip_uint32_t) ((((1 ?
 31:4) - (0 ?
 31:4) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 31:4) - (0 ? 31:4) + 1))) - 1)))))) << (0 ? 31:4)));

    /*GCREG_NN_INST_WORD25*/
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 3:0) - (0 ?
 3:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 3:0) - (0 ?
 3:0) + 1))) - 1)))))) << (0 ?
 3:0))) | (((vip_uint32_t) ((vip_uint32_t) (((info->outImageXSize >> 25) & 0x1)) & ((vip_uint32_t) ((((1 ?
 3:0) - (0 ?
 3:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 3:0) - (0 ? 3:0) + 1))) - 1)))))) << (0 ? 3:0)))
                | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:4) - (0 ?
 31:4) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:4) - (0 ?
 31:4) + 1))) - 1)))))) << (0 ?
 31:4))) | (((vip_uint32_t) ((vip_uint32_t) (info->outImageTransposeBufEndAddr >> 4) & ((vip_uint32_t) ((((1 ?
 31:4) - (0 ?
 31:4) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 31:4) - (0 ? 31:4) + 1))) - 1)))))) << (0 ? 31:4)));

    /* GCREG_NN_INST_WORD26, NN_SECOND_INPUT_BASE_ADDRESS */
    *memory++ = 0;

    /* GCREG_NN_INST_WORD27*/
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 13:12) - (0 ?
 13:12) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 13:12) - (0 ?
 13:12) + 1))) - 1)))))) << (0 ?
 13:12))) | (((vip_uint32_t) ((vip_uint32_t) (info->nnInImageAxiIdOffset) & ((vip_uint32_t) ((((1 ?
 13:12) - (0 ?
 13:12) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 13:12) - (0 ? 13:12) + 1))) - 1)))))) << (0 ? 13:12)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:14) - (0 ?
 15:14) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:14) - (0 ?
 15:14) + 1))) - 1)))))) << (0 ?
 15:14))) | (((vip_uint32_t) ((vip_uint32_t) (info->nnOutImageAxiIdOffset) & ((vip_uint32_t) ((((1 ?
 15:14) - (0 ?
 15:14) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:14) - (0 ? 15:14) + 1))) - 1)))))) << (0 ? 15:14)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 17:16) - (0 ?
 17:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 17:16) - (0 ?
 17:16) + 1))) - 1)))))) << (0 ?
 17:16))) | (((vip_uint32_t) ((vip_uint32_t) (info->nnKernelAxiIdOffset) & ((vip_uint32_t) ((((1 ?
 17:16) - (0 ?
 17:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 17:16) - (0 ? 17:16) + 1))) - 1)))))) << (0 ? 17:16)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 22:22) - (0 ?
 22:22) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 22:22) - (0 ?
 22:22) + 1))) - 1)))))) << (0 ?
 22:22))) | (((vip_uint32_t) ((vip_uint32_t) (info->clearInputEvent) & ((vip_uint32_t) ((((1 ?
 22:22) - (0 ?
 22:22) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 22:22) - (0 ? 22:22) + 1))) - 1)))))) << (0 ? 22:22)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:23) - (0 ?
 25:23) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:23) - (0 ?
 25:23) + 1))) - 1)))))) << (0 ?
 25:23))) | (((vip_uint32_t) ((vip_uint32_t) (info->inputEventID) & ((vip_uint32_t) ((((1 ?
 25:23) - (0 ?
 25:23) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:23) - (0 ? 25:23) + 1))) - 1)))))) << (0 ? 25:23)));

    /* GCREG_NN_INST_WORD28*/
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:31) - (0 ?
 31:31) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:31) - (0 ?
 31:31) + 1))) - 1)))))) << (0 ?
 31:31))) | (((vip_uint32_t) ((vip_uint32_t) ((info->outImageXSize >> 20) & 0x1) & ((vip_uint32_t) ((((1 ?
 31:31) - (0 ?
 31:31) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 31:31) - (0 ? 31:31) + 1))) - 1)))))) << (0 ? 31:31)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 30:30) - (0 ?
 30:30) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 30:30) - (0 ?
 30:30) + 1))) - 1)))))) << (0 ?
 30:30))) | (((vip_uint32_t) ((vip_uint32_t) (info->waitForInputEvent) & ((vip_uint32_t) ((((1 ?
 30:30) - (0 ?
 30:30) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 30:30) - (0 ? 30:30) + 1))) - 1)))))) << (0 ? 30:30)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 1:1) - (0 ?
 1:1) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 1:1) - (0 ?
 1:1) + 1))) - 1)))))) << (0 ?
 1:1))) | (((vip_uint32_t) ((vip_uint32_t) (info->waitForOutputEvent) & ((vip_uint32_t) ((((1 ?
 1:1) - (0 ?
 1:1) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 1:1) - (0 ? 1:1) + 1))) - 1)))))) << (0 ? 1:1)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 4:2) - (0 ?
 4:2) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 4:2) - (0 ?
 4:2) + 1))) - 1)))))) << (0 ?
 4:2))) | (((vip_uint32_t) ((vip_uint32_t) (info->outputEventID) & ((vip_uint32_t) ((((1 ?
 4:2) - (0 ?
 4:2) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 4:2) - (0 ? 4:2) + 1))) - 1)))))) << (0 ? 4:2)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 0:0) - (0 ?
 0:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 0:0) - (0 ?
 0:0) + 1))) - 1)))))) << (0 ?
 0:0))) | (((vip_uint32_t) ((vip_uint32_t) (info->waitForOutputEvent) & ((vip_uint32_t) ((((1 ?
 0:0) - (0 ?
 0:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 0:0) - (0 ? 0:0) + 1))) - 1)))))) << (0 ? 0:0)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 5:5) - (0 ?
 5:5) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 5:5) - (0 ?
 5:5) + 1))) - 1)))))) << (0 ?
 5:5))) | (((vip_uint32_t) ((vip_uint32_t) (info->frameStart) & ((vip_uint32_t) ((((1 ?
 5:5) - (0 ?
 5:5) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 5:5) - (0 ? 5:5) + 1))) - 1)))))) << (0 ? 5:5)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 11:8) - (0 ?
 11:8) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 11:8) - (0 ?
 11:8) + 1))) - 1)))))) << (0 ?
 11:8))) | (((vip_uint32_t) ((vip_uint32_t) (info->sendOverlap2LeftCore) & ((vip_uint32_t) ((((1 ?
 11:8) - (0 ?
 11:8) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 11:8) - (0 ? 11:8) + 1))) - 1)))))) << (0 ? 11:8)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:12) - (0 ?
 15:12) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:12) - (0 ?
 15:12) + 1))) - 1)))))) << (0 ?
 15:12))) | (((vip_uint32_t) ((vip_uint32_t) (info->sendOverlap2RightCore) & ((vip_uint32_t) ((((1 ?
 15:12) - (0 ?
 15:12) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:12) - (0 ? 15:12) + 1))) - 1)))))) << (0 ? 15:12)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | (((vip_uint32_t) ((vip_uint32_t) (info->receiveOverlapFromLeftCore) & ((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 19:16) - (0 ? 19:16) + 1))) - 1)))))) << (0 ? 19:16)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 23:20) - (0 ?
 23:20) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 23:20) - (0 ?
 23:20) + 1))) - 1)))))) << (0 ?
 23:20))) | (((vip_uint32_t) ((vip_uint32_t) (info->receiveOverlapFromRightCore) & ((vip_uint32_t) ((((1 ?
 23:20) - (0 ?
 23:20) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 23:20) - (0 ? 23:20) + 1))) - 1)))))) << (0 ? 23:20)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 24:24) - (0 ?
 24:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 24:24) - (0 ?
 24:24) + 1))) - 1)))))) << (0 ?
 24:24))) | (((vip_uint32_t) ((vip_uint32_t) (info->hwTileWalkMode) & ((vip_uint32_t) ((((1 ?
 24:24) - (0 ?
 24:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 24:24) - (0 ? 24:24) + 1))) - 1)))))) << (0 ? 24:24)));

    /* GCREG_NN_INST_WORD29*/
    *memory++ = 0;

    /* GCREG_NN_INST_WORD30*/
    *memory++ = 0;

    /* GCREG_NN_INST_WORD31*/
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 1:1) - (0 ?
 1:1) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 1:1) - (0 ?
 1:1) + 1))) - 1)))))) << (0 ?
 1:1))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 1:1) - (0 ?
 1:1) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 1:1) - (0 ? 1:1) + 1))) - 1)))))) << (0 ? 1:1)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 12:11) - (0 ?
 12:11) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 12:11) - (0 ?
 12:11) + 1))) - 1)))))) << (0 ?
 12:11))) | (((vip_uint32_t) ((vip_uint32_t) ((info->imageFetchBlockControl & 0x3)) & ((vip_uint32_t) ((((1 ?
 12:11) - (0 ?
 12:11) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 12:11) - (0 ? 12:11) + 1))) - 1)))))) << (0 ? 12:11)));

    /* GCREG_NN_INST_WORD32*/
    *memory++ = 0;

    /* GCREG_NN_INST_WORD33*/
    *memory++ = 0;

    /* GCREG_NN_INST_WORD34*/
    *memory++ = 0;

    /* GCREG_NN_INST_WORD35*/
    *memory++ = 0;

    /* GCREG_NN_INST_WORD36*/
    *memory++ = 0;

    /* GCREG_NN_INST_WORD37*/
    *memory++ = 0;

    /* GCREG_NN_INST_WORD38*/
    *memory++ = 0;

    /* GCREG_NN_INST_WORD39*/
    *memory++ = 0;

    /* GCREG_NN_INST_WORD40*/
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 28:27) - (0 ?
 28:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 28:27) - (0 ?
 28:27) + 1))) - 1)))))) << (0 ?
 28:27))) | (((vip_uint32_t) ((vip_uint32_t) (info->kernelAddressOffsetId) & ((vip_uint32_t) ((((1 ?
 28:27) - (0 ?
 28:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 28:27) - (0 ? 28:27) + 1))) - 1)))))) << (0 ? 28:27)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 30:29) - (0 ?
 30:29) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 30:29) - (0 ?
 30:29) + 1))) - 1)))))) << (0 ?
 30:29))) | (((vip_uint32_t) ((vip_uint32_t) (info->outImageAddressOffsetId) & ((vip_uint32_t) ((((1 ?
 30:29) - (0 ?
 30:29) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 30:29) - (0 ? 30:29) + 1))) - 1)))))) << (0 ? 30:29)));

    /* GCREG_NN_INST_WORD41*/
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:19) - (0 ?
 31:19) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:19) - (0 ?
 31:19) + 1))) - 1)))))) << (0 ?
 31:19))) | (((vip_uint32_t) ((vip_uint32_t) (info->inImageBaseAddressInc & 0x1FFF) & ((vip_uint32_t) ((((1 ?
 31:19) - (0 ?
 31:19) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:19) - (0 ? 31:19) + 1))) - 1)))))) << (0 ? 31:19)));

    /* GCREG_NN_INST_WORD42*/
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:30) - (0 ?
 31:30) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:30) - (0 ?
 31:30) + 1))) - 1)))))) << (0 ?
 31:30))) | (((vip_uint32_t) ((vip_uint32_t) (info->secInimageAddressOffsetId) & ((vip_uint32_t) ((((1 ?
 31:30) - (0 ?
 31:30) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:30) - (0 ? 31:30) + 1))) - 1)))))) << (0 ? 31:30)));

    /* GCREG_NN_INST_WORD43*/
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 29:19) - (0 ?
 29:19) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 29:19) - (0 ?
 29:19) + 1))) - 1)))))) << (0 ?
 29:19))) | (((vip_uint32_t) ((vip_uint32_t) (info->inImageBaseAddressInc >> 13) & ((vip_uint32_t) ((((1 ?
 29:19) - (0 ?
 29:19) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 29:19) - (0 ? 29:19) + 1))) - 1)))))) << (0 ? 29:19)));

    /* GCREG_NN_INST_WORD44*/
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 20:19) - (0 ?
 20:19) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 20:19) - (0 ?
 20:19) + 1))) - 1)))))) << (0 ?
 20:19))) | (((vip_uint32_t) ((vip_uint32_t) (info->inImageAddressOffsetId) & ((vip_uint32_t) ((((1 ?
 20:19) - (0 ?
 20:19) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 20:19) - (0 ? 20:19) + 1))) - 1)))))) << (0 ? 20:19)));

    /* GCREG_NN_INST_WORD45*/
    *memory++ = 0;

    /* GCREG_NN_INST_WORD46*/
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:16) - (0 ?
 31:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:16) - (0 ?
 31:16) + 1))) - 1)))))) << (0 ?
 31:16))) | (((vip_uint32_t) ((vip_uint32_t) (info->secInimageBaseAddressInc & 0xFFFF) & ((vip_uint32_t) ((((1 ?
 31:16) - (0 ?
 31:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:16) - (0 ? 31:16) + 1))) - 1)))))) << (0 ? 31:16)));

    /* GCREG_NN_INST_WORD47*/
    *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 7:0) - (0 ?
 7:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 7:0) - (0 ?
 7:0) + 1))) - 1)))))) << (0 ?
 7:0))) | (((vip_uint32_t) ((vip_uint32_t) (info->secInimageBaseAddressInc >> 16) & ((vip_uint32_t) ((((1 ?
 7:0) - (0 ?
 7:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 7:0) - (0 ? 7:0) + 1))) - 1)))))) << (0 ? 7:0)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:8) - (0 ?
 31:8) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:8) - (0 ?
 31:8) + 1))) - 1)))))) << (0 ?
 31:8))) | (((vip_uint32_t) ((vip_uint32_t) (info->outImageBaseAddressInc) & ((vip_uint32_t) ((((1 ?
 31:8) - (0 ?
 31:8) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 31:8) - (0 ? 31:8) + 1))) - 1)))))) << (0 ? 31:8)));

    num = (vip_uint32_t)((vip_uint8_t*)memory - (vip_uint8_t*)buffer);
    if (size != VIP_NULL) {
        *size = num;
    }

    return status;
}

/*
@brief generate trsp2_instr tsp2 instruction command.
@param buffer, instruction command buffer. 192Bytes
@param size, the size of this instruction command buffer.
@param src_physical, address of input in tsp2 instruction command.
@param dst_physical, address of output in tsp2 .instruction command.
@param data_size, the size of src/dst data buffer
@param type, viphal_hardware_trsp2_type.
*/
vip_status_e viphal_hardware_trsp2_instr(
    vip_uint8_t *buffer,
    vip_uint32_t *size,
    vip_uint32_t src_physical,
    vip_uint32_t dst_physical,
    vip_uint32_t data_size,
    viphal_hardware_trsp2_type type
    )
{
    vip_status_e status = VIP_SUCCESS;
    trsp2_instr_info_t info = { 0 };

    if ((VIP_NULL == buffer) || (VIP_NULL == size)) {
        vip_error("fail trsp2 instr buffer or size is NULL\n");
        return VIP_ERROR_INVALID_ARGUMENTS;
    }
    *size = 0;

    status = generate_trsp2_instr_info(&info, src_physical, dst_physical, data_size, type);
    if (status != VIP_SUCCESS) {
        vip_error("fail to generate trsp2 instr info\n");
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    status = generate_trsp2_instr(&info, (vip_uint32_t*)buffer, size);
    if (status != VIP_SUCCESS) {
        vip_error("fail to generate trsp2 instr\n");
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    return status;
}

/*
@brief generate trsp2_instr loadstate command.
@param buffer, loadstate command buffer.
@param size, the size of this loadstate command buffer.
@param instr_physical, address of tsp2 command.
*/
vip_status_e viphal_hardware_trsp2_loadstate(
    vip_uint8_t *buffer,
    vip_uint32_t *size,
    vip_uint32_t instr_physical
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t *memory = VIP_NULL;
    vip_uint32_t num = 0;
    vip_uint32_t NNconfig = 0x00800000;
    vip_uint32_t vipMiscconfig = 0x00000000;
    vip_uint32_t latencyHiding = viphal_query_hw_feature(VIP_HW_FEATURE_LATENCY_HIDING_FULL_AXI_BW);
    vip_uint32_t min_axi_brst_size = viphal_query_hw_feature(VIP_HW_FEATURE_MIN_AXI_BURST_SIZE);
    vip_uint32_t ddr_kel_brust_size = viphal_query_hw_feature(VIP_HW_FEATURE_DDR_KERNEL_BURST_SIZE);
    vip_uint32_t axi_bus_width = viphal_query_hw_feature(VIP_HW_FEATURE_AXI_BUS_WIDTH);
    vip_uint32_t min_burst_size = 0;
    vip_uint32_t burst_size = 0;

    if ((VIP_NULL == buffer) || (VIP_NULL == size)) {
        vip_error("fail trsp2 load states buffer or size is NULL\n");
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    NNconfig |= ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 12:12) - (0 ?
 12:12) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 12:12) - (0 ?
 12:12) + 1))) - 1)))))) << (0 ?
 12:12))) | (((vip_uint32_t) ((vip_uint32_t) (0x1) & ((vip_uint32_t) ((((1 ?
 12:12) - (0 ?
 12:12) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 12:12) - (0 ? 12:12) + 1))) - 1)))))) << (0 ? 12:12)))
             |  ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 16:13) - (0 ?
 16:13) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 16:13) - (0 ?
 16:13) + 1))) - 1)))))) << (0 ?
 16:13))) | (((vip_uint32_t) ((vip_uint32_t) ((latencyHiding/64)-1) & ((vip_uint32_t) ((((1 ?
 16:13) - (0 ?
 16:13) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 16:13) - (0 ? 16:13) + 1))) - 1)))))) << (0 ? 16:13)))
             |  ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 17:17) - (0 ?
 17:17) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 17:17) - (0 ?
 17:17) + 1))) - 1)))))) << (0 ?
 17:17))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 17:17) - (0 ?
 17:17) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 17:17) - (0 ? 17:17) + 1))) - 1)))))) << (0 ? 17:17)))
             |  ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 20:18) - (0 ?
 20:18) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 20:18) - (0 ?
 20:18) + 1))) - 1)))))) << (0 ?
 20:18))) | (((vip_uint32_t) ((vip_uint32_t) (4) & ((vip_uint32_t) ((((1 ?
 20:18) - (0 ?
 20:18) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 20:18) - (0 ? 20:18) + 1))) - 1)))))) << (0 ? 20:18)))
             |  ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 23:22) - (0 ?
 23:22) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 23:22) - (0 ?
 23:22) + 1))) - 1)))))) << (0 ?
 23:22))) | (((vip_uint32_t) ((vip_uint32_t) (2) & ((vip_uint32_t) ((((1 ?
 23:22) - (0 ?
 23:22) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 23:22) - (0 ? 23:22) + 1))) - 1)))))) << (0 ? 23:22)));

    min_burst_size =
       (min_axi_brst_size == axi_bus_width) ? 0x7 :
       (min_axi_brst_size == 16) ? 0x0 :
       (min_axi_brst_size == 32) ? 0x1 :
       (min_axi_brst_size == 64) ? 0x2 :
       (min_axi_brst_size == 128) ? 0x3 :
       (min_axi_brst_size == 256) ? 0x4 :
       (min_axi_brst_size == 512) ? 0x5 :
       (min_axi_brst_size == 1024) ? 0x6:0;
    burst_size =
       (ddr_kel_brust_size == axi_bus_width) ? 0x7 :
       (ddr_kel_brust_size == 64) ? 0x0 :
       (ddr_kel_brust_size == 128) ? 0x1 :
       (ddr_kel_brust_size == 256) ? 0x2 :
       (ddr_kel_brust_size == 512) ? 0x3 :
       (ddr_kel_brust_size == 1024) ? 0x4 :0;

    vipMiscconfig = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 7:5) - (0 ?
 7:5) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 7:5) - (0 ?
 7:5) + 1))) - 1)))))) << (0 ?
 7:5))) | (((vip_uint32_t) ((vip_uint32_t) (burst_size) & ((vip_uint32_t) ((((1 ?
 7:5) - (0 ?
 7:5) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 7:5) - (0 ? 7:5) + 1))) - 1)))))) << (0 ? 7:5)))
                |  ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 10:8) - (0 ?
 10:8) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 10:8) - (0 ?
 10:8) + 1))) - 1)))))) << (0 ?
 10:8))) | (((vip_uint32_t) ((vip_uint32_t) (min_burst_size) & ((vip_uint32_t) ((((1 ?
 10:8) - (0 ?
 10:8) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 10:8) - (0 ? 10:8) + 1))) - 1)))))) << (0 ? 10:8)))
                |  ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 13:11) - (0 ?
 13:11) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 13:11) - (0 ?
 13:11) + 1))) - 1)))))) << (0 ?
 13:11))) | (((vip_uint32_t) ((vip_uint32_t) (burst_size) & ((vip_uint32_t) ((((1 ?
 13:11) - (0 ?
 13:11) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 13:11) - (0 ? 13:11) + 1))) - 1)))))) << (0 ? 13:11)))
                |  ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 16:14) - (0 ?
 16:14) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 16:14) - (0 ?
 16:14) + 1))) - 1)))))) << (0 ?
 16:14))) | (((vip_uint32_t) ((vip_uint32_t) (min_burst_size) & ((vip_uint32_t) ((((1 ?
 16:14) - (0 ?
 16:14) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 16:14) - (0 ? 16:14) + 1))) - 1)))))) << (0 ? 16:14)))
                |  ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 19:17) - (0 ?
 19:17) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:17) - (0 ?
 19:17) + 1))) - 1)))))) << (0 ?
 19:17))) | (((vip_uint32_t) ((vip_uint32_t) (burst_size) & ((vip_uint32_t) ((((1 ?
 19:17) - (0 ?
 19:17) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 19:17) - (0 ? 19:17) + 1))) - 1)))))) << (0 ? 19:17)))
                |  ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 23:20) - (0 ?
 23:20) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 23:20) - (0 ?
 23:20) + 1))) - 1)))))) << (0 ?
 23:20))) | (((vip_uint32_t) ((vip_uint32_t) (min_burst_size) & ((vip_uint32_t) ((((1 ?
 23:20) - (0 ?
 23:20) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 23:20) - (0 ? 23:20) + 1))) - 1)))))) << (0 ? 23:20)));

    memory = (vip_uint32_t*)buffer;

    *memory++ = 0x08010E5E;
    *memory++ = 0x00000000;
    *memory++ = 0x08010E4C;
    *memory++ = NNconfig;
    *memory++ = 0x08010E5A;
    *memory++ = vipMiscconfig;
    *memory++ = 0x08010428;
    *memory++ = instr_physical;
    *memory++ = 0x08010429;
    *memory++ = 0x00000000;

    num = (vip_uint32_t)((vip_uint8_t*)memory - buffer);
    *size = num;

    return status;
}

#if vpmdENABLE_MULTIPLE_VIP
/*
@brief generate multi-vip sync commands.
@param buffer, sync command buffer.
@param size, the size of this sync command buffer.
@param core_count, the number of vip cores in this devices.
@param core_id, the index of vip core in total cores.
*/
vip_status_e viphal_hardware_multivip_sync(
    vip_uint32_t *buffer,
    vip_uint32_t *size,
    vip_uint32_t core_count,
    vip_uint32_t *core_id
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t *memory = buffer;
    vip_int32_t i = 0;
    vip_uint32_t num = 0;

    if (VIP_NULL == buffer) {
        vip_error("fail to get multivip sync cmds, buffer is NULL\n");
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    if (1 >= core_count) {
        if (size != VIP_NULL) {
            *size = 0;
        }
        return VIP_SUCCESS;
    }

    *memory = 0x08010e02; memory++;
    *memory = 0x30000701; memory++;
    *memory = 0x48000000; memory++;
    *memory = 0x30000701; memory++;

    for (i = 0; i < (vip_int32_t)(core_count - 1); i++) {
        do {    do {    *memory++        = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26))) | (((vip_uint32_t) ((vip_uint32_t) (vip_false_e) & ((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (0x0E02) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0)));}while(0);
    do {vip_uint32_t __temp_data32__;
__temp_data32__ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 4:0) - (0 ?
 4:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 4:0) - (0 ?
 4:0) + 1))) - 1)))))) << (0 ?
 4:0))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 4:0) - (0 ?
 4:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 4:0) - (0 ?
 4:0) + 1))) - 1)))))) << (0 ?
 4:0))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 23:20) - (0 ?
 23:20) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 23:20) - (0 ?
 23:20) + 1))) - 1)))))) << (0 ?
 23:20))) | (((vip_uint32_t) ((vip_uint32_t) (core_id[i]) & ((vip_uint32_t) ((((1 ?
 23:20) - (0 ?
 23:20) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 23:20) - (0 ?
 23:20) + 1))) - 1)))))) << (0 ?
 23:20))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | (((vip_uint32_t) ((vip_uint32_t) (0) & ((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 12:8) - (0 ?
 12:8) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 12:8) - (0 ?
 12:8) + 1))) - 1)))))) << (0 ?
 12:8))) | (((vip_uint32_t) (0x0F & ((vip_uint32_t) ((((1 ?
 12:8) - (0 ?
 12:8) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 12:8) - (0 ?
 12:8) + 1))) - 1)))))) << (0 ?
 12:8))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 27:24) - (0 ?
 27:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 27:24) - (0 ?
 27:24) + 1))) - 1)))))) << (0 ?
 27:24))) | (((vip_uint32_t) ((vip_uint32_t) (core_id[i+1]) & ((vip_uint32_t) ((((1 ?
 27:24) - (0 ?
 27:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 27:24) - (0 ? 27:24) + 1))) - 1)))))) << (0 ? 27:24))) ;*memory++ = __temp_data32__;
} while(0);
}while(0);


        *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27))) | (((vip_uint32_t) (0x09 & ((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ? 31:27) + 1))) - 1)))))) << (0 ? 31:27)));

        *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 4:0) - (0 ?
 4:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 4:0) - (0 ?
 4:0) + 1))) - 1)))))) << (0 ?
 4:0))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 4:0) - (0 ?
 4:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 4:0) - (0 ? 4:0) + 1))) - 1)))))) << (0 ? 4:0)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 23:20) - (0 ?
 23:20) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 23:20) - (0 ?
 23:20) + 1))) - 1)))))) << (0 ?
 23:20))) | (((vip_uint32_t) ((vip_uint32_t) (core_id[i]) & ((vip_uint32_t) ((((1 ?
 23:20) - (0 ?
 23:20) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 23:20) - (0 ? 23:20) + 1))) - 1)))))) << (0 ? 23:20)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | (((vip_uint32_t) ((vip_uint32_t) (0) & ((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 19:16) - (0 ? 19:16) + 1))) - 1)))))) << (0 ? 19:16)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 12:8) - (0 ?
 12:8) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 12:8) - (0 ?
 12:8) + 1))) - 1)))))) << (0 ?
 12:8))) | (((vip_uint32_t) (0x0F & ((vip_uint32_t) ((((1 ?
 12:8) - (0 ?
 12:8) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 12:8) - (0 ? 12:8) + 1))) - 1)))))) << (0 ? 12:8)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 27:24) - (0 ?
 27:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 27:24) - (0 ?
 27:24) + 1))) - 1)))))) << (0 ?
 27:24))) | (((vip_uint32_t) ((vip_uint32_t) (core_id[i+1]) & ((vip_uint32_t) ((((1 ?
 27:24) - (0 ?
 27:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 27:24) - (0 ? 27:24) + 1))) - 1)))))) << (0 ? 27:24)));
    }

    for (i = (vip_int32_t)(core_count - 1); i > 0; i--) {
        do {    do {    *memory++        = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26))) | (((vip_uint32_t) ((vip_uint32_t) (vip_false_e) & ((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (0x0E02) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0)));}while(0);
    do {vip_uint32_t __temp_data32__;
__temp_data32__ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 4:0) - (0 ?
 4:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 4:0) - (0 ?
 4:0) + 1))) - 1)))))) << (0 ?
 4:0))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 4:0) - (0 ?
 4:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 4:0) - (0 ?
 4:0) + 1))) - 1)))))) << (0 ?
 4:0))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 23:20) - (0 ?
 23:20) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 23:20) - (0 ?
 23:20) + 1))) - 1)))))) << (0 ?
 23:20))) | (((vip_uint32_t) ((vip_uint32_t) (core_id[i]) & ((vip_uint32_t) ((((1 ?
 23:20) - (0 ?
 23:20) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 23:20) - (0 ?
 23:20) + 1))) - 1)))))) << (0 ?
 23:20))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | (((vip_uint32_t) ((vip_uint32_t) (0) & ((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 12:8) - (0 ?
 12:8) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 12:8) - (0 ?
 12:8) + 1))) - 1)))))) << (0 ?
 12:8))) | (((vip_uint32_t) (0x0F & ((vip_uint32_t) ((((1 ?
 12:8) - (0 ?
 12:8) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 12:8) - (0 ?
 12:8) + 1))) - 1)))))) << (0 ?
 12:8))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 27:24) - (0 ?
 27:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 27:24) - (0 ?
 27:24) + 1))) - 1)))))) << (0 ?
 27:24))) | (((vip_uint32_t) ((vip_uint32_t) (core_id[i-1]) & ((vip_uint32_t) ((((1 ?
 27:24) - (0 ?
 27:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 27:24) - (0 ? 27:24) + 1))) - 1)))))) << (0 ? 27:24))) ;*memory++ = __temp_data32__;
} while(0);
}while(0);

        *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27))) | (((vip_uint32_t) (0x09 & ((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ? 31:27) + 1))) - 1)))))) << (0 ? 31:27)));

        *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 4:0) - (0 ?
 4:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 4:0) - (0 ?
 4:0) + 1))) - 1)))))) << (0 ?
 4:0))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 4:0) - (0 ?
 4:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 4:0) - (0 ? 4:0) + 1))) - 1)))))) << (0 ? 4:0)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 23:20) - (0 ?
 23:20) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 23:20) - (0 ?
 23:20) + 1))) - 1)))))) << (0 ?
 23:20))) | (((vip_uint32_t) ((vip_uint32_t) (core_id[i]) & ((vip_uint32_t) ((((1 ?
 23:20) - (0 ?
 23:20) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 23:20) - (0 ? 23:20) + 1))) - 1)))))) << (0 ? 23:20)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | (((vip_uint32_t) ((vip_uint32_t) (0) & ((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 19:16) - (0 ? 19:16) + 1))) - 1)))))) << (0 ? 19:16)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 12:8) - (0 ?
 12:8) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 12:8) - (0 ?
 12:8) + 1))) - 1)))))) << (0 ?
 12:8))) | (((vip_uint32_t) (0x0F & ((vip_uint32_t) ((((1 ?
 12:8) - (0 ?
 12:8) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 12:8) - (0 ? 12:8) + 1))) - 1)))))) << (0 ? 12:8)))
            | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 27:24) - (0 ?
 27:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 27:24) - (0 ?
 27:24) + 1))) - 1)))))) << (0 ?
 27:24))) | (((vip_uint32_t) ((vip_uint32_t) (core_id[i-1]) & ((vip_uint32_t) ((((1 ?
 27:24) - (0 ?
 27:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 27:24) - (0 ? 27:24) + 1))) - 1)))))) << (0 ? 27:24)));
    }

    num = (vip_uint32_t)((vip_uint8_t*)memory - (vip_uint8_t*)buffer);
    if (size != VIP_NULL) {
        *size = num;
    }

#if vpmdENABLE_DEBUG_LOG >= 4
{
    vip_char_t info_t[256];
    vip_char_t *tmp = info_t;
    for (i = 0; i < (vip_int32_t)core_count; i++) {
        viphal_os_snprint(tmp, 255, "%02d ", core_id[i]);
        tmp += 3;
    }
    vip_debug("multi-vip sync core count=%d, core id[%s], cmds size=%d\n", core_count, info_t, num);
    viphal_dump_data(buffer, num);
}
#endif

    return status;
}
#endif

#if vpmdENABLE_CNN_PROFILING && vpmdENABLE_BW_PROFILING
/*
@brief generate profiling start/end commands.
@param buffer, command buffer.
@param size, the size of this command buffer.
@param core_count, the number of vip cores in this devices.
@param core_id, the index of vip core in this devcies.
@param probe_param, parameters.
*/
vip_status_e viphal_hardware_probe_cmd(
    vip_uint32_t *buffer,
    vip_uint32_t *size,
    vip_uint32_t core_count,
    vip_uint32_t *core_id,
    viphal_probe_param *probe_param
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t *memory = buffer;
    vip_uint32_t enable_cmd = 0, i = 0, j = 0, module = 0, offset = 0, id = 0, num = 0;
    vip_uint32_t physical = probe_param->physical;
    viphal_probe_proflie_cmd_e cmd = probe_param->cmd;
    vip_uint32_t clusterIDWidth = probe_param->clusterIDWidth;
    static const vip_uint32_t moduleXlate[] =
    {
        /* NN MODULE */
        0x0,
        /* TP MODULE */
        0x1,
    };
    static const vip_uint32_t numXlate[] =
    {
        /* NN MODULE */
        MODULE_NN_COUNTER_NUM,
        /* TP MODULE */
        MODULE_TP_COUNTER_NUM,
    };

    enable_cmd = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 20:20) - (0 ?
 20:20) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 20:20) - (0 ?
 20:20) + 1))) - 1)))))) << (0 ?
 20:20))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 20:20) - (0 ?
 20:20) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 20:20) - (0 ? 20:20) + 1))) - 1)))))) << (0 ? 20:20))) |
                 ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 16:16) - (0 ?
 16:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 16:16) - (0 ?
 16:16) + 1))) - 1)))))) << (0 ?
 16:16))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 16:16) - (0 ?
 16:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 16:16) - (0 ? 16:16) + 1))) - 1)))))) << (0 ? 16:16))) |
                 ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 19:19) - (0 ?
 19:19) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:19) - (0 ?
 19:19) + 1))) - 1)))))) << (0 ?
 19:19))) | (((vip_uint32_t) ((vip_uint32_t) (0) & ((vip_uint32_t) ((((1 ?
 19:19) - (0 ?
 19:19) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:19) - (0 ? 19:19) + 1))) - 1)))))) << (0 ? 19:19))) |
                 ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 18:18) - (0 ?
 18:18) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 18:18) - (0 ?
 18:18) + 1))) - 1)))))) << (0 ?
 18:18))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 18:18) - (0 ?
 18:18) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 18:18) - (0 ? 18:18) + 1))) - 1)))))) << (0 ? 18:18))) |
                 ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 17:17) - (0 ?
 17:17) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 17:17) - (0 ?
 17:17) + 1))) - 1)))))) << (0 ?
 17:17))) | (((vip_uint32_t) ((vip_uint32_t) (0) & ((vip_uint32_t) ((((1 ?
 17:17) - (0 ?
 17:17) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 17:17) - (0 ? 17:17) + 1))) - 1)))))) << (0 ? 17:17))) |
                 ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 22:22) - (0 ?
 22:22) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 22:22) - (0 ?
 22:22) + 1))) - 1)))))) << (0 ?
 22:22))) | (((vip_uint32_t) ((vip_uint32_t) (0) & ((vip_uint32_t) ((((1 ?
 22:22) - (0 ?
 22:22) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 22:22) - (0 ? 22:22) + 1))) - 1)))))) << (0 ? 22:22))) |
                 ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 21:21) - (0 ?
 21:21) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 21:21) - (0 ?
 21:21) + 1))) - 1)))))) << (0 ?
 21:21))) | (((vip_uint32_t) ((vip_uint32_t) (0) & ((vip_uint32_t) ((((1 ?
 21:21) - (0 ?
 21:21) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 21:21) - (0 ? 21:21) + 1))) - 1)))))) << (0 ? 21:21))) |
                 ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 23:23) - (0 ?
 23:23) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 23:23) - (0 ?
 23:23) + 1))) - 1)))))) << (0 ?
 23:23))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 23:23) - (0 ?
 23:23) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 23:23) - (0 ? 23:23) + 1))) - 1)))))) << (0 ? 23:23))) |
                 ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 24:24) - (0 ?
 24:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 24:24) - (0 ?
 24:24) + 1))) - 1)))))) << (0 ?
 24:24))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 24:24) - (0 ?
 24:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 24:24) - (0 ? 24:24) + 1))) - 1)))))) << (0 ? 24:24))) |
                 ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 27:27) - (0 ?
 27:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 27:27) - (0 ?
 27:27) + 1))) - 1)))))) << (0 ?
 27:27))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 27:27) - (0 ?
 27:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 27:27) - (0 ? 27:27) + 1))) - 1)))))) << (0 ? 27:27))) |
                 ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 28:28) - (0 ?
 28:28) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 28:28) - (0 ?
 28:28) + 1))) - 1)))))) << (0 ?
 28:28))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 28:28) - (0 ?
 28:28) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 28:28) - (0 ? 28:28) + 1))) - 1)))))) << (0 ? 28:28))) |
                 ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:25) - (0 ?
 25:25) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:25) - (0 ?
 25:25) + 1))) - 1)))))) << (0 ?
 25:25))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 25:25) - (0 ?
 25:25) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:25) - (0 ? 25:25) + 1))) - 1)))))) << (0 ? 25:25)));
    do {    do {    *memory++        = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26))) | (((vip_uint32_t) ((vip_uint32_t) (vip_false_e) & ((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (0x0E12) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0)));}while(0);
    do {vip_uint32_t __temp_data32__;
__temp_data32__ = enable_cmd;
*memory++ = __temp_data32__;
} while(0);
}while(0);


    for (i = 0; i < core_count; i++) {
        id = core_id[i];
        if (1 < core_count) {
            {  *memory++ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27))) | (((vip_uint32_t) (0x0D & ((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ? 31:27) + 1))) - 1)))))) << (0 ? 31:27))) | 1 << id; memory++;
  };

        }
        for (module = 0; module < TOTAL_VIP_MODULE_NUMBER; module++) {
            offset = 0;
            for (j = 0; j < module; j++) {
                offset += numXlate[j];
            }
            for (j = 0; j < numXlate[module]; j++) {

                switch (cmd) {
                case VIPHAL_PROBECMD_BEGIN:
                    /* reset and resume counter */
                    do {    do {    *memory++        = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26))) | (((vip_uint32_t) ((vip_uint32_t) (vip_false_e) & ((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (0x0E55) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0)));}while(0);
    do {vip_uint32_t __temp_data32__;
__temp_data32__ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | (((vip_uint32_t) (0x1 & ((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | (((vip_uint32_t) ((vip_uint32_t) (moduleXlate[module]) & ((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (j) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0))); ;
*memory++ = __temp_data32__;
} while(0);
}while(0);

                    do {    do {    *memory++        = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26))) | (((vip_uint32_t) ((vip_uint32_t) (vip_false_e) & ((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (0x0E55) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0)));}while(0);
    do {vip_uint32_t __temp_data32__;
__temp_data32__ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | (((vip_uint32_t) (0x3 & ((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | (((vip_uint32_t) ((vip_uint32_t) (moduleXlate[module]) & ((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (j) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0))); ;
*memory++ = __temp_data32__;
} while(0);
}while(0);

                    break;
                case VIPHAL_PROBECMD_PAUSE:
                    /* pause counter */
                    do {    do {    *memory++        = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26))) | (((vip_uint32_t) ((vip_uint32_t) (vip_false_e) & ((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (0x0E55) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0)));}while(0);
    do {vip_uint32_t __temp_data32__;
__temp_data32__ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | (((vip_uint32_t) (0x2 & ((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | (((vip_uint32_t) ((vip_uint32_t) (moduleXlate[module]) & ((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (j) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0))); ;
*memory++ = __temp_data32__;
} while(0);
}while(0);

                    break;
                case VIPHAL_PROBECMD_RESUME:
                    /* resume counter */
                    do {    do {    *memory++        = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26))) | (((vip_uint32_t) ((vip_uint32_t) (vip_false_e) & ((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (0x0E55) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0)));}while(0);
    do {vip_uint32_t __temp_data32__;
__temp_data32__ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | (((vip_uint32_t) (0x3 & ((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | (((vip_uint32_t) ((vip_uint32_t) (moduleXlate[module]) & ((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (j) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0))); ;
*memory++ = __temp_data32__;
} while(0);
}while(0);

                    break;
                case VIPHAL_PROBECMD_END:
                    /* pass address to store counter */
                    do {    do {    *memory++        = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26))) | (((vip_uint32_t) ((vip_uint32_t) (vip_false_e) & ((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (0x0E1C) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0)));}while(0);
    do {vip_uint32_t __temp_data32__;
__temp_data32__ = physical + (TOTAL_VIP_PROBE_NUMBER * i + j + offset) * DATA_PER_REGISTER * sizeof(vip_uint32_t) * (vip_uint32_t)(1 << clusterIDWidth);
*memory++ = __temp_data32__;
} while(0);
}while(0);

                    /* probe and pass the counter */
                    do {    do {    *memory++        = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26))) | (((vip_uint32_t) ((vip_uint32_t) (vip_false_e) & ((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (0x0E55) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0)));}while(0);
    do {vip_uint32_t __temp_data32__;
__temp_data32__ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | (((vip_uint32_t) (0x0 & ((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | (((vip_uint32_t) ((vip_uint32_t) (moduleXlate[module]) & ((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (j) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0))); ;
*memory++ = __temp_data32__;
} while(0);
}while(0);

                    break;
                default:
                    vip_error("invalid probe command\n");
                    return VIP_ERROR_INVALID_ARGUMENTS;
                    break;
                }
            }
        }
    }

    num = (vip_uint32_t)((vip_uint8_t*)memory - (vip_uint8_t*)buffer);
    if (size != VIP_NULL) {
        *size = num;
    }
#if 0
    viphal_dump_data(buffer, num);
#endif
    return status;
}
#endif

