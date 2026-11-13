/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Verisilicon.                                    --
--                                                                            --
--                   (C) COPYRIGHT 2014 VERISILICON                           --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Description : Preprocessor setup
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include "enccommon.h"
#include "encufbc.h"
#include "ufbcswhwregisters.h"
#include "ewl.h"
#include "encpreprocess.h"

#define HSWREG(n) ((n)*4)

static i32 EncUFBCParaCheck(EncUfbc *ufbc, EncUfbcParam *param);

static void EncUfbcgetAlignedPicSizebyFormat(u32 type, u32 width, u32 height,
                                               u32 alignment, u64 *luma_Size,
                                               u64 *chroma_Size,
                                               u64 *picture_Size) {
  u32 luma_stride = 0, chroma_stride = 0;
  u32 lumaSize = 0, chromaSize = 0, pictureSize = 0;

  EncGetAlignedByteStride(width, type, &luma_stride, &chroma_stride, alignment, 0);
  switch (type) {
    case ENC_PIXFMT_YUV420_PLANAR:
      lumaSize = luma_stride * height;
      chromaSize = chroma_stride * height / 2 * 2;
      break;
    case ENC_PIXFMT_YUV420_SEMIPLANAR:
    case ENC_PIXFMT_YUV420_SEMIPLANAR_VU:
      lumaSize = luma_stride * height;
      chromaSize = chroma_stride * height / 2;
      break;
    case ENC_PIXFMT_YUV422_INTERLEAVED_YUYV:
    case ENC_PIXFMT_YUV422_INTERLEAVED_UYVY:
    case ENC_PIXFMT_YUV422_INTERLEAVED_YVYU:
    case ENC_PIXFMT_YUV422_INTERLEAVED_VYUY:
    case ENC_PIXFMT_RGB565:
    case ENC_PIXFMT_BGR565:
    case ENC_PIXFMT_RGB555:
    case ENC_PIXFMT_BGR555:
    case ENC_PIXFMT_RGB444:
    case ENC_PIXFMT_BGR444:
    case ENC_PIXFMT_RGB888:
    case ENC_PIXFMT_BGR888:
    case ENC_PIXFMT_RGB101010:
    case ENC_PIXFMT_BGR101010:
    case ENC_PIXFMT_RGBX8888:
    case ENC_PIXFMT_BGRX8888:
    case ENC_PIXFMT_RGBX1010102:
    case ENC_PIXFMT_BGRX1010102:
      lumaSize = luma_stride * height;
      chromaSize = 0;
      break;
    case ENC_PIXFMT_YUV420_PLANAR_10BIT_I010:
      lumaSize = luma_stride * height;
      chromaSize = chroma_stride * height / 2 * 2;
      break;
    case ENC_PIXFMT_YUV420_PLANAR_10BIT_P010:
      lumaSize = luma_stride * height;
      chromaSize = chroma_stride * height / 2;
      break;
    case ENC_PIXFMT_YUV420_PLANAR_10BIT_PACKED_PLANAR:
      lumaSize = luma_stride * 10 / 8 * height;
      chromaSize = chroma_stride * 10 / 8 * height / 2 * 2;
      break;
    case ENC_PIXFMT_YUV420_10BIT_PACKED_Y0L2:
      lumaSize = luma_stride * 2 * 2 * height / 2;
      chromaSize = 0;
      break;
    case ENC_PIXFMT_YUV420_PLANAR_8BIT_TILE_32_32:
      lumaSize = luma_stride * ((height + 32 - 1) & (~(32 - 1)));
      chromaSize = lumaSize / 2;
      break;
    case ENC_PIXFMT_YUV420_PLANAR_8BIT_TILE_16_16_PACKED_4:
      lumaSize = luma_stride * height * 2 * 12 / 8;
      chromaSize = 0;
      break;
    case ENC_PIXFMT_YUV420_SEMIPLANAR_8BIT_TILE_4_4:
    case ENC_PIXFMT_YUV420_SEMIPLANAR_VU_8BIT_TILE_4_4:
      lumaSize = luma_stride * ((height + 3) / 4);
      chromaSize = chroma_stride * (((height / 2) + 3) / 4);
      break;
    case ENC_PIXFMT_YUV420_PLANAR_10BIT_P010_TILE_4_4:
      lumaSize = luma_stride * ((height + 3) / 4);
      chromaSize = chroma_stride * (((height / 2) + 3) / 4);
      break;
    case ENC_PIXFMT_YUV420_SEMIPLANAR_101010:
      lumaSize = luma_stride * height;
      chromaSize = chroma_stride * height / 2;
      break;
    case ENC_PIXFMT_YUV420_8BIT_TILE_64_4:
    case ENC_PIXFMT_YUV420_UV_8BIT_TILE_64_4:
      lumaSize = luma_stride * ((height + 3) / 4);
      chromaSize = chroma_stride * (((height / 2) + 3) / 4);
      break;
    case ENC_PIXFMT_YUV420_10BIT_TILE_32_4:
      lumaSize = luma_stride * ((height + 3) / 4);
      chromaSize = chroma_stride * (((height / 2) + 3) / 4);
      break;
    case ENC_PIXFMT_YUV420_10BIT_TILE_48_4:
    case ENC_PIXFMT_YUV420_VU_10BIT_TILE_48_4:
      lumaSize = luma_stride * ((height + 3) / 4);
      chromaSize = chroma_stride * (((height / 2) + 3) / 4);
      break;
    case ENC_PIXFMT_YUV420_8BIT_TILE_128_2:
    case ENC_PIXFMT_YUV420_UV_8BIT_TILE_128_2:
      lumaSize = luma_stride * ((height + 1) / 2);
      chromaSize = chroma_stride * (((height / 2) + 1) / 2);
      break;
    case ENC_PIXFMT_YUV420_10BIT_TILE_96_2:
    case ENC_PIXFMT_YUV420_VU_10BIT_TILE_96_2:
      lumaSize = luma_stride * ((height + 1) / 2);
      chromaSize = chroma_stride * (((height / 2) + 1) / 2);
      break;
    case ENC_PIXFMT_YUV420_8BIT_TILE_8_8:
      lumaSize = luma_stride * ((height + 7) / 8);
      chromaSize = chroma_stride * (((height / 2) + 3) / 4);
      break;
    case ENC_PIXFMT_YUV420_10BIT_TILE_8_8:
      lumaSize = luma_stride * ((height + 7) / 8);
      chromaSize = chroma_stride * (((height / 2) + 3) / 4);
      break;
    default:
      PTRACE_E("Not support this format\n");
      chromaSize = lumaSize = 0;
      break;
  }

  pictureSize = lumaSize + chromaSize;
  if (luma_Size != NULL) *luma_Size = lumaSize;
  if (chroma_Size != NULL) *chroma_Size = chromaSize;
  if (picture_Size != NULL) *picture_Size = pictureSize;
}
#ifdef SUPPORT_UFBC
static void getUFBCBlockParameter(UFBCChannelCfg *cfg) {
  switch (cfg->tile_mode) {
    case UFBC_TM_RASTER_256x1:
      cfg->blockWidth = 256;
      cfg->blockHeight = 1;
      break;
    case UFBC_TM_RASTER_128x1:
      cfg->blockWidth = 128;
      cfg->blockHeight = 1;
      break;
    case UFBC_TM_RASTER_64x1:
      cfg->blockWidth = 64;
      cfg->blockHeight = 1;
      break;
    case UFBC_TM_TILE_16x4:
      cfg->blockWidth = 16;
      cfg->blockHeight = 4;
      break;
    case UFBC_TM_TILE_64x4:
      cfg->blockWidth = 64;
      cfg->blockHeight = 4;
      break;
    case UFBC_TM_TILE_32x4:
      cfg->blockWidth = 32;
      cfg->blockHeight = 4;
      break;
    default:
      APITRACEERR("getUFBCBlockParameter: UFBC not support this tile mode\n");
      break;
  }
}

static void UFBCParseStreamCfg_dec400(EncUfbc *ufbc, EncUfbcParam *ufbcParam) {
  u32 luma_stride, chroma_stride;
  u32 pixelByte = EncGetAlignedByteStride(ufbcParam->width, ufbcParam->format, &luma_stride,
                                      &chroma_stride, ufbcParam->alignment, 0);
  luma_stride = luma_stride / pixelByte;
  chroma_stride = chroma_stride / pixelByte;
  dec400RegParam *dec400 = &ufbc->regParam.dec400;
  dec400Param *dec400_param = &ufbcParam->param.dec400;

  dec400->compAlign = dec400_param->compAlign;
  dec400->planes = 0;

  switch (ufbc->format) {
    case ENC_PIXFMT_YUV420_SEMIPLANAR:
    case ENC_PIXFMT_YUV420_SEMIPLANAR_VU:
        dec400->planes = 2;
        dec400->bit_depth = UFBC_BIT_DEPTH_8;
        dec400->chn_cfg[0].compress_fmt = UFBC_COMP_FMT_Y_ONLY;
        dec400->chn_cfg[0].tile_mode = UFBC_TM_RASTER_256x1;
        dec400_param->stride[0] = luma_stride;
        dec400->chn_cfg[1].compress_fmt = UFBC_COMP_FMT_UV_MIX;
        dec400->chn_cfg[1].tile_mode = UFBC_TM_RASTER_128x1;
        dec400_param->stride[1] = chroma_stride / 2;
        getUFBCBlockParameter(&dec400->chn_cfg[0]);
        getUFBCBlockParameter(&dec400->chn_cfg[1]);
        break;
    case ENC_PIXFMT_YUV420_PLANAR_10BIT_P010:
        dec400->planes = 2;
        dec400->bit_depth = UFBC_BIT_DEPTH_16;
        dec400->chn_cfg[0].compress_fmt = UFBC_COMP_FMT_Y_ONLY;
        dec400->chn_cfg[0].tile_mode = UFBC_TM_RASTER_128x1;
        dec400_param->stride[0] = luma_stride;
        dec400->chn_cfg[1].compress_fmt = UFBC_COMP_FMT_UV_MIX;
        dec400->chn_cfg[1].tile_mode = UFBC_TM_RASTER_64x1;
        dec400_param->stride[1] = chroma_stride / 2;
        getUFBCBlockParameter(&dec400->chn_cfg[0]);
        getUFBCBlockParameter(&dec400->chn_cfg[1]);
        break;
    case ENC_PIXFMT_RGB888:
    case ENC_PIXFMT_BGR888:
    case ENC_PIXFMT_RGB101010:
    case ENC_PIXFMT_BGR101010:
        dec400->planes = 1;
        dec400->bit_depth = UFBC_BIT_DEPTH_8;
        dec400->chn_cfg[0].compress_fmt = UFBC_COMP_FMT_ARGB8;
        dec400->chn_cfg[0].tile_mode = UFBC_TM_RASTER_64x1;
        dec400_param->stride[0] = luma_stride;
        getUFBCBlockParameter(&dec400->chn_cfg[0]);
        break;
    default:
        APITRACEERR("UFBCParseStreamCfg: UFBC not support this format\n");
        break;
  }
}

void ufbcSetParams_dec400(EncUfbc *ufbc, EncUfbcParam *ufbcParam) {
  u32 stride = 0;
  dec400RegParam *dec400 = &ufbc->regParam.dec400;
  dec400Param *dec400_param = &ufbcParam->param.dec400;
  UFBCParseStreamCfg_dec400(ufbc, ufbcParam);
  for (u32 i = 0; i < dec400->planes; i++) {
    stride = STRIDE(dec400_param->stride[i], dec400->chn_cfg[i].blockWidth);
    dec400->chn_cfg[i].headerStride = stride / dec400->chn_cfg[i].blockWidth;
    dec400->chn_cfg[i].headerAddress = dec400_param->headerAddress[i] + (stride * ufbcParam->yOffset + ufbcParam->xOffset)
                                        / dec400_param->tileSize / 2;
    dec400->chn_cfg[i].headerOffset = ((stride * ufbcParam->yOffset + ufbcParam->xOffset)
                                        / dec400_param->tileSize) % 2;
    dec400->chn_cfg[i].blockAddress = dec400_param->blockAddress[i] + stride *
                                            ufbcParam->yOffset + ufbcParam->xOffset;
  }
}

void cfgUfbcReg_dec400(EncUfbc *ufbc) {
  dec400RegParam *dec400 = &ufbc->regParam.dec400;
  UFBC_set_dec400_mirror(dec400->regs, HWIF_ENABLE, 1);
  UFBC_set_dec400_mirror(dec400->regs, HWIF_SURF0_HDR_ADDR_LSB,
                          (u32)(dec400->chn_cfg[0].headerAddress));
  if (sizeof(ptr_t) == 8)
    UFBC_set_dec400_mirror(dec400->regs, HWIF_SURF0_HDR_ADDR_MSB,
                            (u32)((dec400->chn_cfg[0].headerAddress) >> 32));
  //Configure block buffer address
  UFBC_set_dec400_mirror(dec400->regs, HWIF_SURF0_PLD_ADDR_LSB,
                            (u32)(dec400->chn_cfg[0].blockAddress));
  if (sizeof(ptr_t) == 8)
    UFBC_set_dec400_mirror(dec400->regs, HWIF_SURF0_PLD_ADDR_MSB,
                            (u32)((dec400->chn_cfg[0].blockAddress) >> 32));
  UFBC_set_dec400_mirror(dec400->regs, HWIF_SURF0_HDR_OFFSET, dec400->chn_cfg[0].headerOffset);
  UFBC_set_dec400_mirror(dec400->regs, HWIF_SURF0_HDR_STRIDE, dec400->chn_cfg[0].headerStride);
  UFBC_set_dec400_mirror(dec400->regs, HWIF_SURF0_BYTE_ALIGN, dec400->compAlign);
  UFBC_set_dec400_mirror(dec400->regs, HWIF_SURF0_BIT_DEPTH, dec400->bit_depth);
  UFBC_set_dec400_mirror(dec400->regs, HWIF_SURF0_FORMAT, dec400->chn_cfg[0].compress_fmt);
  UFBC_set_dec400_mirror(dec400->regs, HWIF_SURF0_TILE_MODE, dec400->chn_cfg[0].tile_mode);

  if (dec400->planes == 2) {
    UFBC_set_dec400_mirror(dec400->regs, HWIF_SURF1_HDR_ADDR_LSB,
                        (u32)(dec400->chn_cfg[1].headerAddress));
    if (sizeof(ptr_t) == 8)
      UFBC_set_dec400_mirror(dec400->regs, HWIF_SURF1_HDR_ADDR_MSB,
                              (u32)((dec400->chn_cfg[1].headerAddress) >> 32));

    UFBC_set_dec400_mirror(dec400->regs, HWIF_SURF1_PLD_ADDR_LSB,
                              (u32)(dec400->chn_cfg[1].blockAddress));
    if (sizeof(ptr_t) == 8)
      UFBC_set_dec400_mirror(dec400->regs, HWIF_SURF1_PLD_ADDR_MSB,
                              (u32)((dec400->chn_cfg[1].blockAddress) >> 32));

    UFBC_set_dec400_mirror(dec400->regs, HWIF_SURF1_HDR_OFFSET, dec400->chn_cfg[1].headerOffset);
    UFBC_set_dec400_mirror(dec400->regs, HWIF_SURF1_HDR_STRIDE, dec400->chn_cfg[1].headerStride);
    UFBC_set_dec400_mirror(dec400->regs, HWIF_SURF1_BYTE_ALIGN, dec400->compAlign);
    UFBC_set_dec400_mirror(dec400->regs, HWIF_SURF1_BIT_DEPTH, dec400->bit_depth);
    UFBC_set_dec400_mirror(dec400->regs, HWIF_SURF1_FORMAT, dec400->chn_cfg[1].compress_fmt);
    UFBC_set_dec400_mirror(dec400->regs, HWIF_SURF1_TILE_MODE, dec400->chn_cfg[1].tile_mode);
  }

}

void ufbcSetParams_afbc(EncUfbc *ufbc, EncUfbcParam *ufbcParam) {
  afbcRegParam *afbc = &ufbc->regParam.afbc;
  if (ufbcParam->param.afbc.blockType == 0) {
    ufbcParam->param.afbc.blockWidth = 32;
    ufbcParam->param.afbc.blockHeight = 8;
  } else {
    ufbcParam->param.afbc.blockWidth = 16;
    ufbcParam->param.afbc.blockHeight = 16;
  }
  afbc->baseAddress = ufbcParam->param.afbc.baseAddress;
  afbc->yuvTrans = ufbcParam->param.afbc.yuvTrans;
  afbc->blockType = ufbcParam->param.afbc.blockType;
  afbc->blockSplit = ufbcParam->param.afbc.blockSplit;
  afbc->stride = STRIDE(ufbcParam->width, ufbcParam->param.afbc.blockWidth) / ufbcParam->param.afbc.blockWidth;
  afbc->startAddress = afbc->baseAddress + ufbcParam->yOffset / ufbcParam->param.afbc.blockHeight *
  afbc->stride * 16 + ufbcParam->xOffset / ufbcParam->param.afbc.blockWidth * 16;
  ufbcParam->height = STRIDE(ufbcParam->height, ufbcParam->param.afbc.blockHeight);
}

void cfgUfbcReg_afbc(EncUfbc *ufbc) {
  afbcRegParam *afbc = &ufbc->regParam.afbc;
  UFBC_set_afbc_mirror(afbc->regs, HWIF_UFBC_ENABLE, 1);
  //Configure header buffer address
  UFBC_set_afbc_mirror(afbc->regs, HWIF_UFBC_HDR_BASE_ADDR_LSB,
                            (u32)(afbc->baseAddress));
  if (sizeof(ptr_t) == 8)
    UFBC_set_afbc_mirror(afbc->regs, HWIF_UFBC_HDR_BASE_ADDR_MSB,
                            (u32)((afbc->baseAddress) >> 32));
  //Configure payload buffer address
  UFBC_set_afbc_mirror(afbc->regs, HWIF_UFBC_HDR_ADDR_LSB,
                            (u32)(afbc->startAddress));
  if (sizeof(ptr_t) == 8)
    UFBC_set_afbc_mirror(afbc->regs, HWIF_UFBC_HDR_ADDR_MSB,
                            (u32)((afbc->startAddress) >> 32));
  UFBC_set_afbc_mirror(afbc->regs, HWIF_UFBC_HDR_STRIDE, afbc->stride);
  UFBC_set_afbc_mirror(afbc->regs, HWIF_UFBC_YUV_TRANS, afbc->yuvTrans);
  UFBC_set_afbc_mirror(afbc->regs, HWIF_UFBC_BLOCK_TYPE, afbc->blockType);
  UFBC_set_afbc_mirror(afbc->regs, HWIF_UFBC_SPLIT, afbc->blockSplit);
  //Configure ufbc decode error interrupt. 0 means normal interrupt, 1 means abnormal interrupt.
  UFBC_set_afbc_mirror(afbc->regs, HWIF_UFBC_DEC_ERR_TYPE, 0);
  UFBC_set_afbc_mirror(afbc->regs, HWIF_UFBC_DEC_ERR_ENABLE, 1);
  UFBC_set_afbc_mirror(afbc->regs, HWIF_UFBC_CFG_ERR_TYPE, 0);
  UFBC_set_afbc_mirror(afbc->regs, HWIF_UFBC_CFG_ERR_ENABLE, 1);
}

i32 ufbcFormatCheck(u32 format) {
  switch (format) {
    case ASIC_INPUT_YUV420SEMIPLANAR:
    case ASIC_INPUT_P010:
    case ASIC_INPUT_RGB888:
    case ASIC_INPUT_RGB101010:
      return 0;
    default:
      return 1;
  }
}

i32 ufbcVersionCheck(u32 hwId, u32 mode) {
  if (hwId == 100 && mode == UFBC_MODE_AFBC_0) {
    return 0;
  } else if (hwId == 101 && mode == UFBC_MODE_AFBC_1) {
    return 0;
  } else if (hwId == 400 && mode == UFBC_MODE_DEC400) {
    return 0;
  }
  return 1;
}

i32 ufbcSizeCheck(u32 luma_stride, u32 chroma_stride, u32 height, u32 xOffset, u32 yOffset, u32 blockWidth, u32 blockHeight) {
  if (luma_stride % blockWidth != 0 || chroma_stride % blockWidth != 0) {
    APITRACEERR(
        "Error: UFBC not support the ori input buffer width does not meet the %u "
        "alignment.\n\n", blockWidth);
    return 1;
  }
  if (height % blockHeight != 0) {
    APITRACEERR(
        "Error: UFBC not support the ori input buffer height does not meet the %u "
        "alignment.\n\n", blockHeight);
    return 1;
  }
  if (xOffset % blockWidth != 0 || yOffset % blockHeight != 0) {
    APITRACEERR(
        "Error: UFBC not support the boundary of top left corner by CROP does "
        "not meet the %ux%u alignment.\n\n", blockWidth, blockHeight);
    return 1;
  }
  return 0;
}

i32 EncUFBCParaCheck(EncUfbc *ufbc, EncUfbcParam *ufbcParam) {
  FILE *fp = NULL;
  char *file_name;
  u32 luma_stride, chroma_stride, pixelByte;
  u32 blockWidth = 0, blockHeight = 0;
  if (ufbcVersionCheck(ufbc->hwId, ufbcParam->mode)) {
    APITRACEERR("Error: The version of UFBC not support this mode.\n\n");
    goto out;
  }
  int asic_format = EncPreGetHwFormat(ufbcParam->format);
  if (ufbcFormatCheck(asic_format)) {
    APITRACEERR("Error: UFBC not support this format.\n\n");
    goto out;
  }
  pixelByte = EncGetAlignedByteStride(ufbcParam->width, ufbcParam->format, &luma_stride,
                                      &chroma_stride, ufbcParam->alignment, 0);
  luma_stride = luma_stride / pixelByte;
  chroma_stride = chroma_stride / pixelByte;
  if(ufbc->mode == UFBC_MODE_DEC400) {
    blockWidth = ufbc->regParam.dec400.chn_cfg[0].blockWidth;
    blockHeight = ufbc->regParam.dec400.chn_cfg[0].blockHeight;
  } else if (ufbc->mode == UFBC_MODE_AFBC_0 || ufbc->mode == UFBC_MODE_AFBC_1) {
    blockWidth = ufbcParam->param.afbc.blockWidth;
    blockHeight = ufbcParam->param.afbc.blockHeight;
  }
  if (ufbcSizeCheck(luma_stride, chroma_stride, ufbcParam->height, ufbcParam->xOffset, ufbcParam->yOffset,
                      blockWidth, blockHeight)) {
    goto out;
  }
  if (ufbc->mode == UFBC_MODE_AFBC_0 || ufbc->mode == UFBC_MODE_AFBC_1) {
    u32 blockSplit = ufbcParam->param.afbc.blockSplit;
    u32 blockType = ufbcParam->param.afbc.blockType;
    u32 yuvTrans = ufbcParam->param.afbc.yuvTrans;
    if ((asic_format == ASIC_INPUT_YUV420SEMIPLANAR || asic_format == ASIC_INPUT_P010) &&
      blockSplit == 1 && blockType == 0) {
      printf(
          "Error: UFBC not support the blockSplit = %u when blockType = %u and inputFormat is "
          "NV12, NV21, P010 and P010(crcb).\n\n", blockSplit, blockType);
      goto out;
    }
    if ((asic_format == ASIC_INPUT_RGB888 || asic_format == ASIC_INPUT_RGB101010) &&
      blockSplit== 0 && blockType == 0) {
      printf(
          "Error: UFBC not support the blockSplit = %u when blockType = %u and inputFormat is "
          "rgb888 and rgb101010.\n\n", blockSplit, blockType);
      goto out;
    }
    if ((asic_format == ASIC_INPUT_YUV420SEMIPLANAR || asic_format == ASIC_INPUT_P010) &&
      yuvTrans == 1) {
      printf(
          "Error: UFBC not support the yuvTrans = %u when inputFormat is "
          "NV12, NV21, P010 and P010(crcb).\n\n", yuvTrans);
      goto out;
    }
#ifndef SYSTEM_BUILD
  if (ufbc->mode == UFBC_MODE_AFBC_0 && blockType == 1) {
    printf(
        "Error: UFBC version ufbcVersion %x not support ufbcBlockType = %u\n"
        ".\n\n", ufbc->mode, blockType);
    goto out;
  }
#endif
  }
  return 0;
out:
#ifdef __FREERTOS__
  return -1;
#else
  file_name = getenv("TEST_DATA_FILES");
  if (file_name == NULL) {
    fp = fopen("tb.cfg", "r");
    //fprintf(stderr, "Generating traces from default file <%s>.\n", DEFAULT_TB_CFG_FILENAME);
  } else {
    fp = fopen(file_name, "r");  //Mark for klocwork : ignore
    //fprintf(stderr, "Generating traces from <%s>\n", getenv("TEST_DATA_FILES"));
  }

  if (fp == NULL) {
    //fprintf(stderr, "Cannot open trace configuration file.\n");
    //Error(4, ERR, "tb.cfg", ", ", SYSERR);
    return -1;
  }
#ifdef TRACE
  extern FILE *Enc_sw_open_file(FILE * file, char *name);
  FILE *cam_ufbc_fp = Enc_sw_open_file(fp, "trace_Top_cam_data_hex_ufbc.trc");
  if (cam_ufbc_fp != NULL) fclose(cam_ufbc_fp);
#endif
  fclose(fp);
  return -1;
#endif
}

static u32 GetUfbcIOSize(u32 ufbcSupport) {
  if (ufbcSupport == 100 || ufbcSupport == 101) {
    return UFBC_AFBC_REG_MAX;
  } else if (ufbcSupport == 400) {
    return UFBC_DEC400_REG_MAX;
  }
  return 0;
}

static u32 GetUfbcIrqOffset(u32 mode) {
  if (mode == UFBC_MODE_AFBC_0) {
    return 9;
  } else if (mode == UFBC_MODE_AFBC_1) {
    return 8;
  }
  return 0;
}

/*******************************************************************************
 Function name   : EncUfbcInit
 Description     : Config the UFBC HW ID.
 Return type     : void
*******************************************************************************/
void EncUfbcInit(EncUfbc *ufbc, const void *ewl, u32 ufbcSupport) {

  u32 ufbcHwId;
  u32 core_id = -1;
  u32 i;

  ufbc->ewl_inst = ewl;

  if (ufbcSupport != 0) {
    ufbc->has_ufbc = 1;
    ufbc->hwId = ufbcSupport;
    ufbc->ioSize = GetUfbcIOSize(ufbcSupport);
  }
}

i32 EncUfbcSetParams(EncUfbc *ufbc, EncUfbcParam *param) {
#ifndef SYSTEM_BUILD
  u32 irqOffset = GetUfbcIrqOffset(param->mode);
  EWLSetUfbcInfo(ufbc->ewl_inst, param->mode, irqOffset);
#endif
  ufbc->mode = param->mode;
  ufbc->format = param->format;
  ufbc->vcmd = param->vcmd;
  if (ufbc->mode == UFBC_MODE_DEC400) {
    ufbcSetParams_dec400(ufbc, param);
  } else if (ufbc->mode == UFBC_MODE_AFBC_0 || ufbc->mode == UFBC_MODE_AFBC_1) {
    ufbcSetParams_afbc(ufbc, param);
  }
  if (ufbc->mode) {
    if (EncUFBCParaCheck(ufbc, param) != 0) {
      return 1;
    }
  }
  return 0;
}

void EncUfbcAsicStart(EncUfbc *ufbc) {
  u32 vcmd_en = EWLGetVCMDSupport(ufbc->ewl_inst);
  if (ufbc->mode == UFBC_MODE_DEC400) {
    cfgUfbcReg_dec400(ufbc);
    if (vcmd_en == 1) {
#ifdef VCMD_BUILD_SUPPORT
      VcmdbufCollectWriteUFBCRegData(ufbc->ewl_inst, ufbc->vcmd, &ufbc->regParam.dec400.regs[2], 2,
                              UFBC_DEC400_REG_MAX - 2);
#endif
    } else {
      for (u32 i = 2; i < UFBC_DEC400_REG_MAX; i++)
        EWLWriteRegbyClientType(ufbc->ewl_inst, HSWREG(i), ufbc->regParam.dec400.regs[i],
                          EWL_CLIENT_TYPE_UFBC);
    }
  } else if (ufbc->mode == UFBC_MODE_AFBC_0 || ufbc->mode == UFBC_MODE_AFBC_1) {
    cfgUfbcReg_afbc(ufbc);
    if (vcmd_en == 1) {
#ifdef VCMD_BUILD_SUPPORT
      VcmdbufCollectWriteUFBCRegData(ufbc->ewl_inst, ufbc->vcmd, &ufbc->regParam.afbc.regs[2], 2,
                              UFBC_AFBC_REG_MAX - 2);
#endif
    } else {
      for (u32 i = 2; i < UFBC_AFBC_REG_MAX; i++)
        EWLWriteRegbyClientType(ufbc->ewl_inst, HSWREG(i), ufbc->regParam.afbc.regs[i],
                          EWL_CLIENT_TYPE_UFBC);
    }
  }
}

void EncUfbcAsicStop(const void *ewl, void *inst) {
  EWLWriteRegbyClientType(ewl, HSWREG(2), 0,
                        EWL_CLIENT_TYPE_UFBC);
}
#endif

void GetUfbcAlignment(u32 asic_format, u32 blockType, u32 *width_alignment, u32 *height_alignment) {
  u32 alignment = 0;
  if (blockType == 0) {
    alignment = 32;
    *height_alignment = 8;
  } else {
    alignment = 16;
    *height_alignment = 16;
  }
  if(asic_format == ASIC_INPUT_YUV420SEMIPLANAR){
    *width_alignment = alignment;
  } else if (asic_format == ASIC_INPUT_P010) {
    *width_alignment = alignment * 2;
  } else {
    *width_alignment = alignment * 4;
  }
}

void EncUfbcGetSize(u32 inputFormat, u32 blockType, u32 width, u32 height, u32 *asic_format, u32 *headerSize, u64 *frameSize) {
  u32 lumaStride, chromaStride, heightStride = 0;
  u32 ufbc_width_alignment, ufbc_height_alignment = 0;
  u64 lumaSize = 0, chromaSize = 0;
  *asic_format = EncPreGetHwFormat(inputFormat);
  GetUfbcAlignment(*asic_format, blockType, &ufbc_width_alignment, &ufbc_height_alignment);
  u32 pixelByte = EncGetAlignedByteStride(width, inputFormat, &lumaStride,
                                      &chromaStride, ufbc_width_alignment, 0);
  lumaStride = lumaStride / pixelByte;
  heightStride = STRIDE(height, ufbc_height_alignment);
  EncUfbcgetAlignedPicSizebyFormat(inputFormat, width,
                          heightStride, ufbc_width_alignment, &lumaSize,
                          &chromaSize, frameSize);
  *headerSize = STRIDE(lumaStride * heightStride / 256 * 16, 128);
  if (*asic_format == ASIC_INPUT_P010) {
    *frameSize = lumaStride * heightStride / 256 * 512;
  }
}


