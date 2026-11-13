/* --------------------------*/
/* ENC verification           */
/* --------------------------*/
#include "ameba_soc.h"
#include <diag.h>
#include <log.h>

#include "stdio.h"

#include <FreeRTOS.h>
#include <task.h>

#include "user_data.h"

#include "hevcencapi.h"
#include "ewl.h"
#include "instance.h"

#define OSD_EN

u32 output_data_addr = 0;

static u32 wrap_num_to_enc = 0;
static u32 low_latency_set = 0;
u32 user_timeout_disable = 0;

int Demo_Video_Read_File(struct test_bench *tb)
{
	i32 ret = OK;
	u32 src_img_size;

	tb->lum = (u8 *)tb->pictureMem.virtualAddress;
	tb->cb = tb->lum + tb->lumaSize;
	tb->cr = tb->cb + tb->chromaSize / 2;
	src_img_size =
		tb->width * tb->height * 12 / 8; //bit_pre_pixel of yuv420 is 12
	//printf("src_img_size %d, input_data_width %d input_data_height %d\r\n", src_img_size, tb->width, tb->height);
	switch (tb->inputFormat) {
	case VCENC_YUV420_PLANAR:
		ReadYuv420PlanarFrame(tb, src_img_size, tb->width, tb->height);
		break;
	case VCENC_YUV420_SEMIPLANAR:
		ReadYuv420SemiPlanarFrame(tb, src_img_size, tb->width, tb->height);
		break;
	default:
		ret = NOK;
	}

	return ret;
}

int Demo_Video_pre(enc_user_data *user_data)
{
	i32 ret = OK;
	u32 lumaSize = 0, chromaSize = 0;
	u32 luma_stride = 0, chroma_stride = 0;
	u32 pictureSize = 0;
	u32 outbufSize = 0;
	u32 block_size;
	u32 ufbcHeaderSize = 0;

	struct test_bench tb;
	memset(&tb, 0, sizeof(struct test_bench));

	tb.dec400TableFile = user_data->dec400CompTableinput;
	tb.width = user_data->width;
	tb.height = user_data->height;
	tb.firstPic = user_data->firstPic;
	tb.lastPic = user_data->lastPic;
	tb.picture_encoded_cnt = 0; //RTK_TBC
	tb.input_alignment = (user_data->exp_of_input_alignment == 0 ? 0
						  : (1 << user_data->exp_of_input_alignment));
	tb.idr_interval = user_data->intraPicRate;
	tb.ufbcEnable = user_data->ufbcEnable;
	tb.ufbcBlockType = user_data->ufbcBlockType;
	tb.dec400Enable = 0; //RTK_TBC
	tb.dec400TSHeaderEnable = user_data->dec400TSHeaderEnable;

#ifdef OSD_EN
	//for OSD
	/* Set y and uv stride for overlay region */
	int i;
	for (i = 0; i < MAX_OVERLAY_NUM; i++) {
		//Set default stride
		if (user_data->olYStride[i] == 0) {
			switch (user_data->olFormat[i]) {
			case 0:  //ARGB
				/* For super tile format, 64 rows aligned */
				if (user_data->olSuperTile[i] == 0) {
					user_data->olYStride[i] = user_data->olWidth[i] * 4;
				} else {
					user_data->olYStride[i] = STRIDE(user_data->olWidth[i], 64) * 64 * 4;
				}
				break;
			case 1:  //NV12
				user_data->olYStride[i] = user_data->olWidth[i];
				break;
			case 2:  //Bitmap
				user_data->olYStride[i] = user_data->olWidth[i] / 8;
				break;
			default:
				user_data->olYStride[i] = user_data->olWidth[i];
			}
		}
		if (user_data->olUVStride[i] == 0) {
			user_data->olUVStride[i] = user_data->olYStride[i];
		}

		//Set default cropping info
		if (user_data->olCropWidth[i] == 0) {
			user_data->olCropWidth[i] = user_data->olWidth[i];
		}
		if (user_data->olCropHeight[i] == 0) {
			user_data->olCropHeight[i] = user_data->olHeight[i];
		}
		if (user_data->olScaleWidth[i] == 0) {
			user_data->olScaleWidth[i] = user_data->olCropWidth[i];
		}
		if (user_data->olScaleHeight[i] == 0) {
			user_data->olScaleHeight[i] = user_data->olCropHeight[i];
		}

		tb.olEnable[i] = (user_data->overlayEnables >> i) & 1;
	}
#endif

	//extern
	tb.srcwidth = user_data->lumWidthSrc;
	tb.srcheight = user_data->lumHeightSrc;
	tb.inputFormat = user_data->inputFormat;
	tb.codecFormat = user_data->codecFormat;
	tb.profile = user_data->profile;
	tb.level = user_data->level;
	tb.log2_ctb_size = (tb.codecFormat == VCENC_VIDEO_CODEC_H264) ? 4 : 6; //RTK_TBC

	const void *ewl_inst = InitEwlInst_simple(&tb);
	if (ewl_inst == NULL) {
		printf("error: InitEwlInst return NULL\r\n");
		ret = NOK;
		goto error;
	}

	// step 1: alloc memory
#if 0
	if ((ret = AllocateDMABuffer(&tb)) != OK) {
		goto error;
	}
#else
	u32 alignment = tb.input_alignment == 0 ? 1 : tb.input_alignment;
	luma_stride = STRIDE(user_data->lumWidthSrc, alignment);
	chroma_stride = STRIDE(user_data->lumWidthSrc, alignment);
	lumaSize = luma_stride * user_data->lumHeightSrc;
	chromaSize = chroma_stride * user_data->lumHeightSrc / 2;
	pictureSize = lumaSize + chromaSize;
	tb.lumaSize = lumaSize;
	tb.chromaSize = chromaSize;
	if (user_data->ufbcEnable) {
		u32 asic_format = 0;
		EncUfbcGetSize(user_data->inputFormat, tb.ufbcBlockType, user_data->lumWidthSrc, user_data->lumHeightSrc,
					   &asic_format, &ufbcHeaderSize, &pictureSize);
	}
	tb.pictureMem.mem_type =
		EXT_WR | VPU_RD | EWL_MEM_TYPE_DPB;
	SET_MEM_USAGE(tb.pictureMem.mem_type,
				  EWL_MEM_USAGE_IN_SURFACE, user_data->secure_mode);
	ret = EWLMallocLinear(ewl_inst, pictureSize + ufbcHeaderSize, tb.input_alignment,
						  &tb.pictureMem);
	if (ret != OK) {
		tb.pictureMem.virtualAddress = NULL;
		goto error;
	}
	//calc UV address
	tb.busLuma = tb.pictureMem.busAddress;
	tb.busChromaU = tb.busLuma + tb.lumaSize;
	tb.busChromaV = tb.busChromaU + tb.chromaSize / 2;
#ifdef OSD_EN
	for (i = 0; i < MAX_OVERLAY_NUM; i++) {
		if ((user_data->overlayEnables >> i) & 1) {
			switch (user_data->olFormat[i]) {
			case 0:  //ARGB
				if (user_data->olSuperTile[i] == 0) {
					block_size = user_data->olYStride[i] * user_data->olHeight[i];
				} else {
					block_size = user_data->olYStride[i] * ((user_data->olHeight[i] + 63) / 64);
				}
				break;
			case 1:  //NV12
				block_size = user_data->olYStride[i] * user_data->olHeight[i] +
							 user_data->olUVStride[i] * user_data->olHeight[i] / 2;
				break;
			case 2:  //Bitmap
				block_size = user_data->olYStride[i] * user_data->olHeight[i];
				break;
			default:  //3
				block_size = 0;
			}

			tb.overlayInputMem[i].mem_type =
				EXT_WR | VPU_RD | EWL_MEM_TYPE_VPU_WORKING;
			SET_MEM_USAGE(tb.overlayInputMem[i].mem_type,
						  EWL_MEM_USAGE_IN_OVERLAY, user_data->secure_mode);
			if (EWLMallocLinear(ewl_inst, block_size, tb.input_alignment,
								&tb.overlayInputMem[i]) != OK) {
				tb.overlayInputMem[i].virtualAddress = NULL;
				return 1;
			}
			memset(tb.overlayInputMem[i].virtualAddress, 0,
				   block_size);
		}
	}

	if (user_data->osdMapEnable) {
		//u32 read_height = GetOsdmapReadHeight(user_data);
		u32 read_height = GetOsdmapReadHeight_simple(user_data);
		block_size = user_data->osdMapStride * read_height;
		tb.osdMapMem.mem_type =
			EXT_WR | VPU_RD | EWL_MEM_TYPE_VPU_WORKING;
		SET_MEM_USAGE(tb.osdMapMem.mem_type,
					  EWL_MEM_USAGE_IN_OVERLAY, user_data->secure_mode);
		if (EWLMallocLinear(ewl_inst, block_size, tb.input_alignment,
							&tb.osdMapMem) !=
			EWL_OK) {
			tb.osdMapMem.virtualAddress = NULL;
			return 1;
		}
		memset(tb.osdMapMem.virtualAddress, 0, block_size);
	}
#endif
	tb.dec400Enable = 1;
	if (tb.dec400TableFile != NULL) {
		tb.dec400Table = fopen(tb.dec400TableFile, "rb");
	}
	if (tb.dec400Table != NULL || tb.dec400TSHeaderEnable == 1) {
		u32 dec400Enable = 0;
		dec400Enable = user_data->dec400TileSize << 8 | user_data->dec400TileMode << 16 |
					   user_data->dec400DataAlignment << 29 | user_data->dec400RGBAX << 28 |
					   user_data->dec400RGBFormat << 25;
		tb.dec400Enable = 2 | dec400Enable;
		EncGetDec400TsBufferSize(
			user_data->inputFormat, user_data->lumWidthSrc, user_data->lumHeightSrc, tb.input_alignment,
			&tb.dec400LumaTableSize, &tb.dec400ChrTableSize,
			&tb.dec400FrameTableSize, user_data->scanType,
			tb.dec400Enable, user_data->dec400TSHeaderEnable);
		if (tb.dec400FrameTableSize == 0) {
			if (tb.dec400Table) {
				fclose(tb.dec400Table);
			}
			goto error;
		}
		tb.Dec400CmpTableMem.mem_type =
			EXT_WR | VPU_RD | EWL_MEM_TYPE_VPU_WORKING;
		SET_MEM_USAGE(tb.Dec400CmpTableMem.mem_type,
					  EWL_MEM_USAGE_IN_SURFACE_DECTS, user_data->secure_mode);
		ret = EWLMallocLinear(ewl_inst,
							  (tb.dec400LumaTableSize + tb.dec400ChrTableSize),
							  16, &tb.Dec400CmpTableMem);
		if (ret != OK) {
			tb.Dec400CmpTableMem.virtualAddress = NULL;
			if (tb.dec400Table) {
				fclose(tb.dec400Table);
			}
			goto error;
		}
		if (tb.dec400Table) {
			fclose(tb.dec400Table);
		}
	}

	//calc UV address
	tb.dec400LumTableBase = tb.Dec400CmpTableMem.busAddress;
	tb.dec400CbTableBase = tb.Dec400CmpTableMem.busAddress + tb.dec400LumaTableSize;
	tb.dec400CrTableBase = tb.dec400CbTableBase + tb.dec400ChrTableSize / 2;

	outbufSize = 4 * tb.pictureMem.size;
	outbufSize = CLIP3(OUTBUF_SIZE_MIN, OUTBUF_SIZE_MAX, outbufSize);
	tb.outbufMem.mem_type =
		VPU_WR | CPU_WR | CPU_RD | EWL_MEM_TYPE_SLICE;
	SET_MEM_USAGE(tb.outbufMem.mem_type,
				  EWL_MEM_USAGE_OUT_STRM, user_data->secure_mode);
	ret = EWLMallocLinear(ewl_inst, outbufSize, tb.input_alignment,
						  &tb.outbufMem);
	if (ret != OK) {
		tb.outbufMem.virtualAddress = NULL;
		goto error;
	}
#endif

	//step 2: read data
	if (!(tb.inP = fopen(user_data->input, "rb"))) {
		printf("fail to open input file\r\n");
		ret = NOK;
		goto error;
	}

	if (!(tb.outP = fopen(user_data->output, "wb"))) {
		printf("fail to open output file\r\n");
		ret = NOK;
		goto error;
	}

	if (tb.dec400TableFile != NULL) {
		tb.dec400Table = fopen(tb.dec400TableFile, "rb");
	}

	if (tb.dec400Table != NULL) {
		tb.lum = (u8 *)tb.pictureMem.virtualAddress;
		tb.cb = tb.lum + tb.lumaSize;
		tb.cr = tb.cb + tb.chromaSize / 2;
		if (read_dec400Data_Simple(&tb, tb.inputFormat, tb.srcwidth,
								   tb.srcheight) == NOK) {
			printf(
				"DEC400 data read over, please check the yuv real frames and "
				"your cmdline about lastPic!!\n");
			ret = NOK;
			goto error;
		}
	}

#ifdef OSD_EN

	for (i = 0; i < MAX_OVERLAY_NUM; i++) {
		if ((user_data->overlayEnables >> i) & 1) {
			u32 frameByte = 1;
			int j;
			for (j = 0; j < i; j++) {
				if (strcmp(user_data->olInput[i], user_data->olInput[j]) == 0) {
					tb.overlayFile[i] = tb.overlayFile[j];
				}
			}
			if (tb.overlayFile[i] == 0) {
				tb.overlayFile[i] = open_file(user_data->olInput[i], "rb");
			}
			//Get number of input overlay frames
			fseeko(tb.overlayFile[i], 0, SEEK_END);
			if (user_data->olFormat[i] == 0) {
				if (user_data->olSuperTile[i] == 0) {
					frameByte = (user_data->olWidth[i] * user_data->olHeight[i]) * 4;
				} else
					frameByte = (((user_data->olWidth[i] + 63) / 64) * 64) *
								(((user_data->olHeight[i] + 63) / 64) * 64) * 4;
			} else if (user_data->olFormat[i] == 1) {
				frameByte = (user_data->olWidth[i] * user_data->olHeight[i]) / 2 * 3;
			} else if (user_data->olFormat[i] == 2) {
				frameByte = (user_data->olWidth[i] / 8 * user_data->olHeight[i]);
			}
			tb.olFrameNume[i] = ftello(tb.overlayFile[i]) / frameByte;
			if (tb.olFrameNume[i] == 0) {
				printf(
					"Overlay file %d: Error, file %s size is not suitable for region "
					"size\n",
					i + 1, user_data->olInput[i]);
				return -1;
			}
		}
	}
	if (user_data->osdMapEnable) {
		tb.osdMapFile = open_file(user_data->osdMapInput, "rb");
		u32 frameByte = 1;
		u32 read_height = GetOsdmapReadHeight_simple(user_data);
		fseeko(tb.osdMapFile, 0, SEEK_END);
		frameByte = user_data->osdMapStride * read_height;
		tb.osdMapFrameNum = ftello(tb.osdMapFile) / frameByte;
		if (tb.osdMapFrameNum == 0) {
			printf(
				"OsdMap file: Error, file %s size is not suitable for region "
				"size\n", user_data->osdMapInput);
			return -1;
		}
	}
#endif
	//step 3: run
	if (Demo_Video_Encoding(&tb, user_data) != 0) {
		ret = NOK;
		goto error;
	}
error:
	/* Free all resources */
	FreeVariableMem(&tb); //RTK_TBC
	if (tb.inP) {
		fclose(tb.inP);
		tb.inP = NULL;
	}
	if (tb.outP) {
		fclose(tb.outP);
		tb.outP = NULL;
	}
	if (tb.dec400TSHeaderEnable == 0) {
		if (tb.dec400Table) {
			fclose(tb.dec400Table);
			tb.dec400Table = NULL;
		}
	}
#if 0
	FreeDMABuffer(&tb);
#else
	EWLFreeLinear(ewl_inst, &tb.pictureMem);
	EWLFreeLinear(ewl_inst, &tb.Dec400CmpTableMem);
	EWLFreeLinear(ewl_inst, &tb.outbufMem);
#endif
	if (ewl_inst) {
		EWLRelease(ewl_inst);
		ewl_inst = NULL;
	}
end:

	return ret;
}

/* --------------------- */
/* -- HEVC demo entry -- */
/* --------------------- */
int Demo_Video_Encoding(struct test_bench *tb, enc_user_data *user_data)
{
	i32 ret = OK;
	int i;
	u32 crc32value = 0;
	VCEncConfig cfg;
	VCEncRateCtrl rcCfg;
	//VCEncInst hevc_encoder;
	VCEncInst *hevc_encoder = tb->hevc_encoder;
	VCEncGopPicConfig gopPicCfg[MAX_GOP_PIC_CONFIG_NUM];
	VCEncPreProcessingCfg preProcCfg;
	VCEncIn *pEncIn = &(tb->encIn);
	VCEncOut encOut;
	u32 streamSize = 0;
	memset(&rcCfg, 0, sizeof(VCEncRateCtrl));
	memset(&preProcCfg, 0, sizeof(VCEncPreProcessingCfg));
	const void *ewl_inst = tb->ewlInst;
	if (ewl_inst == NULL) {
		printf("error: InitEwlInst return NULL\r\n");
		ret = NOK;
		goto error;
	}

	/* Step 1: Initialize an hevc encoder instance */
	memset(&cfg, 0, sizeof(VCEncConfig));
	init_video_config(&cfg);
	cfg.exp_of_input_alignment = tb->input_alignment; //8;
	cfg.width = tb->width;
	cfg.height = tb->height;
	cfg.level = tb->level;
	cfg.profile = tb->profile;
	cfg.codecFormat = tb->codecFormat;
	cfg.ufbcParam.mode = tb->ufbcEnable;
	cfg.ufbcParam.param.dec400.compAlign = user_data->ufbcCompAlign;

	cfg.enableTMVP = 0;
	cfg.rdoLevel = 0;

	allocVariableMem(tb);
	cfg.tile_width = tb->tile_width;
	cfg.tile_height = tb->tile_height;
	u32 log2_ctb_size = tb->log2_ctb_size;
	i32 width = tb->width;
	i32 height = tb->height;
	u32 ctbPerRow = ((width + (1 << log2_ctb_size) - 1) >> log2_ctb_size);
	u32 ctbPerColumn = ((height + (1 << log2_ctb_size) - 1) >> log2_ctb_size);
	u32 tileId;
	for (tileId = 0; tileId < cfg.num_tile_columns; tileId++) {
		cfg.tile_width[tileId] =
			(tileId + 1) * ctbPerRow / cfg.num_tile_columns -
			(tileId * ctbPerRow) / cfg.num_tile_columns;
	}
	for (tileId = 0; tileId < cfg.num_tile_rows; tileId++) {
		cfg.tile_height[tileId] =
			(tileId + 1) * ctbPerColumn / cfg.num_tile_rows -
			(tileId * ctbPerColumn) / cfg.num_tile_rows;
	}

	if ((ret = VCEncInit(&cfg, &(tb->hevc_encoder), NULL)) != VCENC_OK) {
		return (int)ret;
	}

	if ((ret = VCEncGetRateCtrl(tb->hevc_encoder, &rcCfg)) != VCENC_OK) {
		return (int)ret;
	}
	HevcInitRateConfigs(&rcCfg);

	if ((ret = VCEncSetRateCtrl(tb->hevc_encoder, &rcCfg)) != VCENC_OK) {
		return (int)ret;
	}

	/* Step 2: Extra encoder configuration. See the next example code for how to change the
	* default encoder parameters
	*/
	/* GOP configuration */
	memset(gopPicCfg, 0, sizeof(gopPicCfg));
	tb->encIn.gopConfig.pGopPicCfg = gopPicCfg;
	VideoInitGopConfigs(tb->gopSize, &(tb->encIn.gopConfig),
						tb->encIn.gopConfig.gopCfgOffset);
	tb->encIn.gopConfig.outputRateNumer = 30; /* Output frame rate numerator */
	tb->encIn.gopConfig.outputRateDenom = 1; /* Output frame rate denominator */
	tb->encIn.gopConfig.inputRateNumer = 30; /* Input frame rate numerator */
	tb->encIn.gopConfig.inputRateDenom = 1; /* Input frame rate denominator */
	tb->encIn.gopConfig.idr_interval = tb->idr_interval;
	/* Step 3: Allocate linear memory resources (input and output buffers) */

	//for input format +++
	/* PreP setup */
	if ((ret = VCEncGetPreProcessing(tb->hevc_encoder, &preProcCfg)) != VCENC_OK) {
		goto error;
	}
	HevcInitPreConfigs(&preProcCfg);
	preProcCfg.inputType = tb->inputFormat;

	preProcCfg.origWidth = tb->srcwidth;
	preProcCfg.origHeight = tb->srcheight;
	preProcCfg.alignmentWidth = tb->srcwidth;
	preProcCfg.alignmentHeight = tb->srcheight;
	preProcCfg.input_alignment = 1 << tb->input_alignment;

#ifdef OSD_EN
	for (i = 0; i < MAX_OVERLAY_NUM; i++) {
		preProcCfg.overlayArea[i].xoffset = user_data->olXoffset[i];
		preProcCfg.overlayArea[i].cropXoffset = user_data->olCropXoffset[i];
		preProcCfg.overlayArea[i].yoffset = user_data->olYoffset[i];
		preProcCfg.overlayArea[i].cropYoffset = user_data->olCropYoffset[i];
		preProcCfg.overlayArea[i].width = user_data->olWidth[i];
		preProcCfg.overlayArea[i].cropWidth = user_data->olCropWidth[i];
		preProcCfg.overlayArea[i].height = user_data->olHeight[i];
		preProcCfg.overlayArea[i].cropHeight = user_data->olCropHeight[i];
		preProcCfg.overlayArea[i].format = user_data->olFormat[i];
		preProcCfg.overlayArea[i].alpha = user_data->olAlpha[i];
		preProcCfg.overlayArea[i].enable = (user_data->overlayEnables >> i) & 1;
		preProcCfg.overlayArea[i].Ystride = user_data->olYStride[i];
		preProcCfg.overlayArea[i].UVstride = user_data->olUVStride[i];
		preProcCfg.overlayArea[i].bitmapY = user_data->olBitmapY[i];
		preProcCfg.overlayArea[i].bitmapU = user_data->olBitmapU[i];
		preProcCfg.overlayArea[i].bitmapV = user_data->olBitmapV[i];
		preProcCfg.overlayArea[i].superTile = user_data->olSuperTile[i];
		preProcCfg.overlayArea[i].scaleWidth = user_data->olScaleWidth[i];
		preProcCfg.overlayArea[i].scaleHeight = user_data->olScaleHeight[i];
	}

	/* Set mosaic region parameters */
	for (i = 0; i < MAX_MOSAIC_NUM; i++) {
		preProcCfg.mosEnable[i] = (user_data->mosaicEnables >> i) & 1;
		preProcCfg.mosXoffset[i] = user_data->mosXoffset[i];
		preProcCfg.mosYoffset[i] = user_data->mosYoffset[i];
		preProcCfg.mosWidth[i] = user_data->mosWidth[i];
		preProcCfg.mosHeight[i] = user_data->mosHeight[i];
	}
	preProcCfg.mosSizeIndex = user_data->mosSizeIndex;

	/* Set OSD map parameters */
	preProcCfg.osdMapEnable = user_data->osdMapEnable;
	preProcCfg.osdMapInput = user_data->osdMapInput;
	preProcCfg.osdMapStride = user_data->osdMapStride;
	for (i = 0; i < MAX_OSDMAP_COLOR_NUM; i++) {
		preProcCfg.osdMapAlpha[i] = user_data->osdMapAlpha[i];
		preProcCfg.osdMapY[i] = user_data->osdMapY[i];
		preProcCfg.osdMapU[i] = user_data->osdMapU[i];
		preProcCfg.osdMapV[i] = user_data->osdMapV[i];
	}
	preProcCfg.mosSizeIndex = user_data->mosSizeIndex;
#endif

	if ((ret = VCEncSetPreProcessing(tb->hevc_encoder, &preProcCfg)) != VCENC_OK) {
		goto error;
	}

	//for input format ---
	VCEncCodingCtrl codingCfg;
	memset(&codingCfg, 0, sizeof(VCEncCodingCtrl));
	if ((ret = VCEncGetCodingCtrl(tb->hevc_encoder, &codingCfg)) != VCENC_OK) {
		goto error;
	}
	if (low_latency_set) {
		/* low latency */
		codingCfg.inputLineBufEn = 1; //(cml->inputLineBufMode > 0) ? 1 : 0;
		codingCfg.inputLineBufLoopBackEn =
			1; //(cml->inputLineBufMode == 1 || cml->inputLineBufMode == 2) ? 1 : 0;
		codingCfg.inputLineBufDepth = 1; //cml->inputLineBufDepth;
		codingCfg.inputLineBufHwModeEn =
			1; //(cml->inputLineBufMode == 2 || cml->inputLineBufMode == 4) ? 1 : 0;
		codingCfg.amountPerLoopBack = wrap_num_to_enc; //cml->amountPerLoopBack;
	}
	if ((ret = VCEncSetCodingCtrl(tb->hevc_encoder, &codingCfg)) != VCENC_OK) {
		goto error;
	}

	pEncIn->busOutBuf[0] = tb->outbufMem.busAddress;
	pEncIn->outBufSize[0] = tb->outbufMem.size;
	pEncIn->pOutBuf[0] = tb->outbufMem.virtualAddress;
	pEncIn->cur_out_buffer[0] = &tb->outbufMem;
	pEncIn->dec400Enable = 0;

	pEncIn->bIsIDR = tb->idr_interval;

#ifdef OSD_EN
	SetupOverlayBuffer_simple(tb, user_data, pEncIn);
#endif

	for (i = 0; i < VCENC_MAX_LT_REF_FRAMES; i++) {
		pEncIn->long_term_ref_pic[i] = INVALITED_POC;
	}

	/* Video, sequence and picture parameter sets */
	if (VCEncStrmStart(tb->hevc_encoder, pEncIn, &encOut)) {
		/* Handle the error situation here*/
		printf("error: VCEncStrmStart fail\r\n");
		goto error;
	}
	/* Save the stream header */
	SaveStream((u8 *)pEncIn->pOutBuf[0], encOut.streamSize, tb->outP, &crc32value);
	streamSize += encOut.streamSize;
	/* Step 5: Encode the video frames */
	while (HANTRO_TRUE) {
		if (tb->picture_encoded_cnt >= tb->lastPic) {
			break;
		}
		/* Setup encoder input, we assume one linear buffer for the input image in */
		/* planar YCbCr 4:2:0 format */
#if 0
		pEncIn->busLuma = tb->pictureMem.busAddress;
		//tb.lum = (u8 *)pictureMem.virtualAddress;
		pEncIn->busChromaU = pEncIn->busLuma + tb->lumaSize;
		//tb.cb = tb.lum + lumaSize;
		pEncIn->busChromaV = pEncIn->busChromaU + tb->chromaSize / 2;
#else
		pEncIn->busLuma = tb->busLuma;
		pEncIn->busChromaU = tb->busChromaU;
		pEncIn->busChromaV = tb->busChromaV;
#endif
		//tb.cr = tb.cb + chromaSize / 2;
		//src_img_size =
		//	input_data_width * input_data_height * 12 / 8; //bit_pre_pixel of yuv420 is 12
		/* Read YUV to input buffer */

		//if (tb->dec400Table) {
		if (tb->dec400Enable > 1) {
#if 0
			pEncIn->dec400LumTableBase = tb->Dec400CmpTableMem.busAddress;
			pEncIn->dec400CbTableBase =
				tb->Dec400CmpTableMem.busAddress + tb->dec400LumaTableSize;
			pEncIn->dec400CrTableBase =
				pEncIn->dec400CbTableBase + tb->dec400ChrTableSize / 2;
#else
			pEncIn->dec400LumTableBase = tb->dec400LumTableBase;
			pEncIn->dec400CbTableBase = tb->dec400CbTableBase;
			pEncIn->dec400CrTableBase = tb->dec400CrTableBase;
#endif
		}

		if (tb->dec400Enable < 2 && tb->dec400Table == NULL && tb->dec400TSHeaderEnable == 0 && tb->inP != NULL) { //RTK-TBC
			if ((ret = Demo_Video_Read_File(tb)) != VCENC_OK) {
				goto error;
			}
		} else {
			pEncIn->dec400Enable = tb->dec400Enable;
			pEncIn->dec400TSHeaderEnable = tb->dec400TSHeaderEnable;
		}
		printf("=== Encoding %i ...\n\n", tb->picture_encoded_cnt);

		ret = VCEncStrmEncode(tb->hevc_encoder, pEncIn, &encOut, NULL, NULL);
		/* Save encoded stream */
		switch (ret) {
		case VCENC_FRAME_ENQUEUE:
			pEncIn->picture_cnt++;
			break;
		case VCENC_FRAME_READY:
			if (encOut.codingType != VCENC_NOTCODED_FRAME) {
				pEncIn->picture_cnt++;
			}
			if (encOut.streamSize != 0) {
				SaveStream((u8 *)pEncIn->pOutBuf[0], encOut.streamSize, tb->outP, &crc32value);
				streamSize += encOut.streamSize;
				tb->picture_encoded_cnt++;
				// printf(">> VCENC_FRAME_READY:\r\n");
				// dump_addr(pEncIn->pOutBuf[0], encOut.streamSize);
				printf("VCEncStrmEncode OK, frame ready\r\n");

			}
			break;
		case VCENC_SYSTEM_ERROR:
		/* Fatal system error, can’t continue. */
		default:
			goto error;
		}
	} /* End of main encoding loop */
	/* Flush encoder internal buffered frames
	* In Multicore enabled or 2-pass enabled, encoder internal cached frames
	* must be flushed
	*/
	ret = VCEncFlush(tb->hevc_encoder, pEncIn, &encOut, NULL, NULL);
	/* Step 6: End stream */
	ret = VCEncStrmEnd(tb->hevc_encoder, pEncIn, &encOut);
	if (ret == VCENC_OK) {
		SaveStream((u8 *)pEncIn->pOutBuf[0], encOut.streamSize, tb->outP, &crc32value);
		streamSize += encOut.streamSize;
	} else {
		/* Handle the error situation here*/
		goto error;
	}
	// printf(">> End stream:\r\n");

	printf("Total of %d frames encoded, %lu bytes\n\n", tb->picture_encoded_cnt,
		   (long unsigned int)streamSize);
	printf("stream CRC32: 0x%X \r\n", crc32value);
	tb->streamSize = streamSize;
error:

end:
	/* Last Step: Release the encoder instance */
	ret = VCEncRelease(tb->hevc_encoder);
	return ret;
}

/*------------------------------------------------------------------------------
-- --
-- A Simple Demo of Encoding 300 Frames (HEVC) --
-- --
--------------------------------------------------------------------------------*/
//input file path
#define TEST_HEVC_FILE_NAME "D:\\FPGA_test_data\\rushHour_1920x1080_5f.nv12"
//output file path
#define TEST_HEVC_OUTPUT_FILE_NAME "D:\\FPGA_test_data\\result.hevc"

#define TEST_HEVC_CRC32_VALUE 0x91298077 //0x645368DF

//input resolution
#define TEST_HEVC_INPUT_WIDTH 1920
#define TEST_HEVC_INPUT_HEIGHT 1080
//output resolution
#define TEST_HEVC_OUTPUT_WIDTH 1920
#define TEST_HEVC_OUTPUT_HEIGHT 1080
//input data format
#define TEST_HEVC_INPUT_FORMAT VCENC_YUV420_SEMIPLANAR

int Demo_HEVC_Encoding(void)
{
	i32 ret = OK;
	struct enc_user_data para;
	memset(&para, 0, sizeof(struct enc_user_data));

	para.input = TEST_HEVC_FILE_NAME;
	para.output = TEST_HEVC_OUTPUT_FILE_NAME;
	para.firstPic = 0;
	para.lastPic = 2;
	para.width = TEST_HEVC_OUTPUT_WIDTH;
	para.height = TEST_HEVC_OUTPUT_HEIGHT;
	para.lumWidthSrc = TEST_HEVC_INPUT_WIDTH;
	para.lumHeightSrc = TEST_HEVC_INPUT_HEIGHT;
	para.inputFormat = TEST_HEVC_INPUT_FORMAT;
	para.codecFormat = VCENC_VIDEO_CODEC_HEVC;
	para.profile = VCENC_ADAPTIVE_PROFILE;
	para.level = VCENC_AUTO_LEVEL;
	para.exp_of_input_alignment = 6;
	para.intraPicRate = 0;

	Demo_Video_pre(&para);

	return ret;
}

/*------------------------------------------------------------------------------
-- --
-- A Simple Demo of Encoding 300 Frames (H264) --
-- --
--------------------------------------------------------------------------------*/
//input file path
#define TEST_H264_FILE_NAME "D:\\FPGA_test_data\\foreman_176x144_3f.nv12"
//output file path
#define TEST_H264_OUTPUT_FILE_NAME "D:\\FPGA_test_data\\result.h264"

#define TEST_H264_CRC32_VALUE 0xE899B11A

//input resolution
#define TEST_H264_INPUT_WIDTH 176
#define TEST_H264_INPUT_HEIGHT 144
//output resolution
#define TEST_H264_OUTPUT_WIDTH 176
#define TEST_H264_OUTPUT_HEIGHT 144
//input data format
#define TEST_H264_INPUT_FORMAT VCENC_YUV420_SEMIPLANAR

int Demo_H264_Encoding(void)
{
	i32 ret = OK;
	struct enc_user_data para;
	memset(&para, 0, sizeof(struct enc_user_data));

	para.input = TEST_H264_FILE_NAME;
	para.output = TEST_H264_OUTPUT_FILE_NAME;
	para.firstPic = 0;
	para.lastPic = 2;
	para.width = TEST_H264_OUTPUT_WIDTH;
	para.height = TEST_H264_OUTPUT_HEIGHT;
	para.lumWidthSrc = TEST_H264_INPUT_WIDTH;
	para.lumHeightSrc = TEST_H264_INPUT_HEIGHT;
	para.inputFormat = TEST_H264_INPUT_FORMAT;
	para.codecFormat = VCENC_VIDEO_CODEC_H264;
	para.profile = VCENC_H264_HIGH_PROFILE;
	para.level = VCENC_H264_LEVEL_5_1;
	para.exp_of_input_alignment = 0;

	Demo_Video_pre(&para);

	return ret;
}


//input file path
#define TEST_OSD_FILE_NAME "D:\\FPGA_test_data\\foreman_176x144_3f.nv12"
#define TEST_OVERLAY1_INPUT_FILE_NAME "D:\\FPGA_test_data\\osd_RandomAlpha_4096_2048.rgb888"
#define TEST_OVERLAY2_INPUT_FILE_NAME "D:\\FPGA_test_data\\kuubaRandom_352x288.nv12"
#define TEST_OVERLAY3_INPUT_FILE_NAME "D:\\FPGA_test_data\\osd_80_80.bmp"
#define TEST_OSDMAP_INPUT_FILE_NAME "D:\\FPGA_test_data\\osd_80_80_1.bmp"
//output file path
#define TEST_OSD_OUTPUT_FILE_NAME "D:\\FPGA_test_data\\osd_result.h264"

#define TEST_OSD_CRC32_VALUE 0x1ABB8CBC

//input resolution
#define TEST_OSD_INPUT_WIDTH 176
#define TEST_OSD_INPUT_HEIGHT 144
//output resolution
#define TEST_OSD_OUTPUT_WIDTH 176
#define TEST_OSD_OUTPUT_HEIGHT 144
//input data format
#define TEST_OSD_INPUT_FORMAT VCENC_YUV420_SEMIPLANAR

int Demo_OSD_Encoding(void)
{
	i32 ret = OK;
	struct enc_user_data para;
	memset(&para, 0, sizeof(struct enc_user_data));
	para.input = TEST_OSD_FILE_NAME;
	para.output = TEST_OSD_OUTPUT_FILE_NAME;
	para.firstPic = 0;
	para.lastPic = 1;
	para.width = TEST_OSD_OUTPUT_WIDTH;
	para.height = TEST_OSD_OUTPUT_HEIGHT;
	para.lumWidthSrc = TEST_OSD_INPUT_WIDTH;
	para.lumHeightSrc = TEST_OSD_INPUT_HEIGHT;
	para.inputFormat = TEST_OSD_INPUT_FORMAT;
	para.codecFormat = VCENC_VIDEO_CODEC_H264;
	para.profile = VCENC_H264_HIGH_PROFILE;
	para.level = VCENC_H264_LEVEL_5_1;
	para.exp_of_input_alignment = 0;

	//OSD para
	para.overlayEnables = 2;
	para.osdMapEnable = 1;

	para.osdMapInput = TEST_OSDMAP_INPUT_FILE_NAME;
	para.osdMapStride = 32;

	para.osdMapAlpha[0] = 255;
	para.osdMapY[0] = 20;
	para.osdMapU[0] = 70;
	para.osdMapV[0] = 80;

	//para.osdMapAlpha[1] = 155;
	//para.osdMapY[1] = 120;
	//para.osdMapU[1] = 111;
	//para.osdMapV[1] = 200;

	para.osdMapAlpha[2] = 255;
	para.osdMapY[2] = 220;
	para.osdMapU[2] = 234;
	para.osdMapV[2] = 100;

	para.osdMapAlpha[3] = 55;
	para.osdMapY[3] = 144;
	para.osdMapU[3] = 200;
	para.osdMapV[3] = 80;

	para.osdMapAlpha[4] = 211;
	para.osdMapY[4] = 160;
	para.osdMapU[4] = 100;
	para.osdMapV[4] = 50;

	para.osdMapAlpha[5] = 111;
	para.osdMapY[5] = 120;
	para.osdMapU[5] = 255;
	para.osdMapV[5] = 55;

	//para.osdMapAlpha[6] = 99;
	//para.osdMapY[6] = 222;
	//para.osdMapU[6] = 156;
	//para.osdMapV[6] = 99;

	para.osdMapAlpha[7] = 100;
	para.osdMapY[7] = 156;
	para.osdMapU[7] = 180;
	para.osdMapV[7] = 122;

	para.osdMapAlpha[8] = 122;
	para.osdMapY[8] = 169;
	para.osdMapU[8] = 120;
	para.osdMapV[8] = 222;

	para.osdMapAlpha[9] = 250;
	para.osdMapY[9] = 244;
	para.osdMapU[9] = 150;
	para.osdMapV[9] = 145;

	para.osdMapAlpha[10] = 115;
	para.osdMapY[10] = 111;
	para.osdMapU[10] = 190;
	para.osdMapV[10] = 189;

	para.osdMapAlpha[11] = 222;
	para.osdMapY[11] = 190;
	para.osdMapU[11] = 110;
	para.osdMapV[11] = 40;

#if 0
	para.olInput[0] = TEST_OVERLAY1_INPUT_FILE_NAME;
	para.olFormat[0] = 0;
	para.olAlpha[0] = 0;
	para.olWidth[0] = 4096;
	para.olHeight[0] = 2048;
	para.olXoffset[0] = 6;
	para.olYoffset[0] = 6;
	para.olYStride[0] = 0;
	para.olUVStride[0] = 0;
	para.olCropXoffset[0] = 8;
	para.olCropYoffset[0] = 0;
	para.olCropWidth[0] = 56;
	para.olCropHeight[0] = 16;
	para.olBitmapY[0] = 200;
	para.olBitmapU[0] = 200;
	para.olBitmapV[0] = 200;
#else
	para.olInput[0] = TEST_OVERLAY3_INPUT_FILE_NAME;
	para.olFormat[0] = 2;
	para.olAlpha[0] = 120;
	para.olWidth[0] = 80;
	para.olHeight[0] = 80;
	para.olXoffset[0] = 0;
	para.olYoffset[0] = 0;
	para.olYStride[0] = 80;
	para.olUVStride[0] = 80;
	para.olCropXoffset[0] = 0;
	para.olCropYoffset[0] = 0;
	para.olCropWidth[0] = 16;
	para.olCropHeight[0] = 16;
	para.olBitmapY[0] = 200;
	para.olBitmapU[0] = 200;
	para.olBitmapV[0] = 200;
#endif

	para.olInput[1] = TEST_OVERLAY2_INPUT_FILE_NAME;
	para.olFormat[1] = 1;
	para.olAlpha[1] = 120;
	para.olWidth[1] = 352;
	para.olHeight[1] = 288;
	para.olXoffset[1] = 0;
	para.olYoffset[1] = 64;
	para.olYStride[1] = 352;
	para.olUVStride[1] = 352;
	para.olCropXoffset[1] = 8;
	para.olCropYoffset[1] = 4;
	para.olCropWidth[1] = 16;
	para.olCropHeight[1] = 16;
	para.olBitmapY[1] = 200;
	para.olBitmapU[1] = 200;
	para.olBitmapV[1] = 200;

	para.olInput[2] = TEST_OVERLAY3_INPUT_FILE_NAME;
	para.olFormat[2] = 2;
	para.olAlpha[2] = 140;
	para.olWidth[2] = 80;
	para.olHeight[2] = 80;
	para.olXoffset[2] = 0;
	para.olYoffset[2] = 128;
	para.olYStride[2] = 80;
	para.olUVStride[2] = 80;
	para.olCropXoffset[2] = 0;
	para.olCropYoffset[2] = 0;
	para.olCropWidth[2] = 16;
	para.olCropHeight[2] = 16;
	para.olBitmapY[2] = 200;
	para.olBitmapU[2] = 200;
	para.olBitmapV[2] = 200;

	para.olInput[3] = TEST_OVERLAY2_INPUT_FILE_NAME;
	para.olFormat[3] = 1;
	para.olAlpha[3] = 140;
	para.olWidth[3] = 352;
	para.olHeight[3] = 288;
	para.olXoffset[3] = 64;
	para.olYoffset[3] = 0;
	para.olYStride[3] = 352;
	para.olUVStride[3] = 352;
	para.olCropXoffset[3] = 8;
	para.olCropYoffset[3] = 4;
	para.olCropWidth[3] = 16;
	para.olCropHeight[3] = 16;
	para.olBitmapY[3] = 200;
	para.olBitmapU[3] = 200;
	para.olBitmapV[3] = 200;

	para.olInput[4] = TEST_OVERLAY3_INPUT_FILE_NAME;
	para.olFormat[4] = 2;
	para.olAlpha[4] = 160;
	para.olWidth[4] = 80;
	para.olHeight[4] = 80;
	para.olXoffset[4] = 64;
	para.olYoffset[4] = 64;
	para.olYStride[4] = 80;
	para.olUVStride[4] = 80;
	para.olCropXoffset[4] = 0;
	para.olCropYoffset[4] = 0;
	para.olCropWidth[4] = 16;
	para.olCropHeight[4] = 16;
	para.olBitmapY[4] = 200;
	para.olBitmapU[4] = 200;
	para.olBitmapV[4] = 200;

	para.olInput[5] = TEST_OVERLAY2_INPUT_FILE_NAME;
	para.olFormat[5] = 1;
	para.olAlpha[5] = 160;
	para.olWidth[5] = 352;
	para.olHeight[5] = 288;
	para.olXoffset[5] = 64;
	para.olYoffset[5] = 128;
	para.olYStride[5] = 352;
	para.olUVStride[5] = 352;
	para.olCropXoffset[5] = 8;
	para.olCropYoffset[5] = 4;
	para.olCropWidth[5] = 16;
	para.olCropHeight[5] = 16;
	para.olBitmapY[5] = 200;
	para.olBitmapU[5] = 200;
	para.olBitmapV[5] = 200;

	para.olInput[6] = TEST_OVERLAY3_INPUT_FILE_NAME;
	para.olFormat[6] = 2;
	para.olAlpha[6] = 180;
	para.olWidth[6] = 80;
	para.olHeight[6] = 80;
	para.olXoffset[6] = 128;
	para.olYoffset[6] = 0;
	para.olYStride[6] = 80;
	para.olUVStride[6] = 80;
	para.olCropXoffset[6] = 0;
	para.olCropYoffset[6] = 0;
	para.olCropWidth[6] = 16;
	para.olCropHeight[6] = 16;
	para.olBitmapY[6] = 200;
	para.olBitmapU[6] = 200;
	para.olBitmapV[6] = 200;

	para.olInput[7] = TEST_OVERLAY2_INPUT_FILE_NAME;
	para.olFormat[7] = 1;
	para.olAlpha[7] = 180;
	para.olWidth[7] = 352;
	para.olHeight[7] = 288;
	para.olXoffset[7] = 128;
	para.olYoffset[7] = 64;
	para.olYStride[7] = 352;
	para.olUVStride[7] = 352;
	para.olCropXoffset[7] = 8;
	para.olCropYoffset[7] = 4;
	para.olCropWidth[7] = 16;
	para.olCropHeight[7] = 16;
	para.olBitmapY[7] = 200;
	para.olBitmapU[7] = 200;
	para.olBitmapV[7] = 200;

	para.olInput[8] = TEST_OVERLAY3_INPUT_FILE_NAME;
	para.olFormat[8] = 2;
	para.olAlpha[8] = 200;
	para.olWidth[8] = 80;
	para.olHeight[8] = 80;
	para.olXoffset[8] = 128;
	para.olYoffset[8] = 128;
	para.olYStride[8] = 80;
	para.olUVStride[8] = 80;
	para.olCropXoffset[8] = 0;
	para.olCropYoffset[8] = 0;
	para.olCropWidth[8] = 16;
	para.olCropHeight[8] = 16;
	para.olBitmapY[8] = 200;
	para.olBitmapU[8] = 200;
	para.olBitmapV[8] = 200;

	para.olInput[9] = TEST_OVERLAY2_INPUT_FILE_NAME;
	para.olFormat[9] = 1;
	para.olAlpha[9] = 200;
	para.olWidth[9] = 352;
	para.olHeight[9] = 288;
	para.olXoffset[9] = 0;
	para.olYoffset[9] = 32;
	para.olYStride[9] = 352;
	para.olUVStride[9] = 352;
	para.olCropXoffset[9] = 8;
	para.olCropYoffset[9] = 4;
	para.olCropWidth[9] = 16;
	para.olCropHeight[9] = 16;
	para.olBitmapY[9] = 200;
	para.olBitmapU[9] = 200;
	para.olBitmapV[9] = 200;

	para.olInput[10] = TEST_OVERLAY3_INPUT_FILE_NAME;
	para.olFormat[10] = 2;
	para.olAlpha[10] = 220;
	para.olWidth[10] = 80;
	para.olHeight[10] = 80;
	para.olXoffset[10] = 64;
	para.olYoffset[10] = 32;
	para.olYStride[10] = 80;
	para.olUVStride[10] = 80;
	para.olCropXoffset[10] = 0;
	para.olCropYoffset[10] = 0;
	para.olCropWidth[10] = 16;
	para.olCropHeight[10] = 16;
	para.olBitmapY[10] = 200;
	para.olBitmapU[10] = 200;
	para.olBitmapV[10] = 200;

	para.olInput[11] = TEST_OVERLAY2_INPUT_FILE_NAME;
	para.olFormat[11] = 1;
	para.olAlpha[11] = 220;
	para.olWidth[11] = 352;
	para.olHeight[11] = 288;
	para.olXoffset[11] = 128;
	para.olYoffset[11] = 32;
	para.olYStride[11] = 352;
	para.olUVStride[11] = 352;
	para.olCropXoffset[11] = 8;
	para.olCropYoffset[11] = 4;
	para.olCropWidth[11] = 16;
	para.olCropHeight[11] = 16;
	para.olBitmapY[11] = 200;
	para.olBitmapU[11] = 200;
	para.olBitmapV[11] = 200;

	para.mosaicEnables = 1;
	para.mosXoffset[6] = 0;
	para.mosYoffset[6] = 0;
	para.mosWidth[6] = 128;
	para.mosHeight[6] = 128;
	para.mosSizeIndex = 0;
	//printf("%s %d \r\n", __func__, __LINE__);
	Demo_Video_pre(&para);

	return ret;
}

//input file path
#define TEST_DEC_FILE_NAME "D:\\FPGA_test_data\\dec400_a64.yuv"
//output file path
#define TEST_DEC_OUTPUT_HEVC_FILE_NAME "D:\\FPGA_test_data\\dec_a64_result.hevc"
#define TEST_DEC_OUTPUT_H264_FILE_NAME "D:\\FPGA_test_data\\dec_a64_result.h264"
//DEC table path
#define TEST_DEC_TABLE_FILE_NAME "D:\\FPGA_test_data\\dec400_table_a64.bin"

#define TEST_DEC_CRC32_VALUE 0xA5387099 //0x6E5C7B1C

//input resolution
#define TEST_DEC_INPUT_WIDTH 1280
#define TEST_DEC_INPUT_HEIGHT 720
//output resolution
#define TEST_DEC_OUTPUT_WIDTH 1280
#define TEST_DEC_OUTPUT_HEIGHT 720
//input data format
#define TEST_DEC_INPUT_FORMAT VCENC_YUV420_SEMIPLANAR

int Demo_DEC_Encoding(void)
{
	i32 ret = OK;
	struct enc_user_data para;
	memset(&para, 0, sizeof(struct enc_user_data));

	para.input = TEST_DEC_FILE_NAME;
	para.output = TEST_DEC_OUTPUT_HEVC_FILE_NAME;
	para.firstPic = 0;
	para.lastPic = 1;
	para.width = TEST_DEC_OUTPUT_WIDTH;
	para.height = TEST_DEC_OUTPUT_HEIGHT;
	para.lumWidthSrc = TEST_DEC_INPUT_WIDTH;
	para.lumHeightSrc = TEST_DEC_INPUT_HEIGHT;
	para.inputFormat = TEST_DEC_INPUT_FORMAT;
	para.codecFormat = VCENC_VIDEO_CODEC_HEVC;
	para.profile = VCENC_ADAPTIVE_PROFILE;
	para.level = VCENC_AUTO_LEVEL;
	para.exp_of_input_alignment = 6;

	//DEC para
	para.dec400CompTableinput = TEST_DEC_TABLE_FILE_NAME;
	para.ufbcEnable = 3;
	para.ufbcBlockType = 0;
	para.ufbcCompAlign = 1; //64 byte

	Demo_Video_pre(&para);

	return ret;
}

/*------------------------------------------------------------------------------
-- --
-- A Simple Demo of JPEG Encoding --
-- --
--------------------------------------------------------------------------------*/
#include "jpegencapi.h"

static u32 comLen = 38;
static u8 comment[39] = "This is Hantro's test COM data header.";

void init_config(JpegEncCfg *param)
{
	//
	// ref #include "hwcfg/ewl_hwcfg.c"
	//
	memset(param, 0, sizeof(JpegEncCfg));
	//param->inputWidth = 176;
	//param->inputHeight = 144;
	//param->codingWidth = 176;
	//param->codingHeight = 144;
	param->quality = -1;
	param->qLevel = 1;
	param->frameType = 1;
	param->xDensity = 1;
	param->yDensity = 1;
	param->comLength = comLen;
	param->pCom = comment;

	param->inputLineBufDepth = 1;
	//param->inputLineBufCbFunc = NULL;
	//param->inputLineBufCbData = NULL;
	param->constCb = 128;
	param->constCr = 128;
	param->frameRateNum = 1; //TBC
	param->frameRateDenom = 1;
	param->qpmax = 51;
	param->fixedQP = -1;
	param->rcMode = 1;
	param->picQpDeltaMin = -2;
	param->picQpDeltaMax = 3;
	param->streamMultiSegmentAmount = 4;
	param->streamMultiSegmentSize = 1024;
	//param->streamMultiSegCbFunc = NULL;
	//param->streamMultiSegCbData = NULL;
	param->irqTypeMask = 0x1f0;
	//param->osdDec400TableFile[0] = NULL;
	param->osdMapStride = 11; //TBC
	param->sbi_id_1 = 1;
	param->sbi_id_2 = 2;
	param->segmentUnitHeight = 16;
	param->sramPowerdownTimerDiv32 = 96;
	//param->enc_dev = NULL;
	//param->mem_dev = NULL;
	param->useVcmd = -1;
	param->lowlatGatingDisable = 1;
}

//input file path
#define JPEG_TEST_FILE_NAME "D:\\FPGA_test_data\\foreman_176x144_3f.nv12"
//output file path
#define JPEG_TEST_OUTPUT_FILE_NAME "D:\\FPGA_test_data\\result.jpeg"

#define JPEG_TEST_CRC32_VALUE 0x52D8514F

//input and output resolution
#define TEST_JPEG_WIDTH 176
#define TEST_JPEG_HEIGHT 144

int Demo_JPEG_Encoding(void)
{
	int retStatus = 0;
	u32 crc32value = 0;
	JpegEncRet ret;
	JpegEncInst encoder = {0};
	JpegEncCfg cfg = {0};
	JpegEncIn encIn = {0};
	JpegEncOut encOut = {0};
	/* Output buffer size: For most images 1 byte/pixel is enough
	for high quality JPEG */
	u32 outbuf_size = 32 * 32 + TEST_JPEG_WIDTH * TEST_JPEG_HEIGHT * 2;
	u32 pict_bus_address = 0;
	u32 *outbuf_virt_address = NULL;
	u32 output_bus_address = 0;

	/* EWL init */
	EWLInitParam_t param;
	memset(&param, 0, sizeof(EWLInitParam_t));
	param.context = NULL;
	param.clientType = EWL_CLIENT_TYPE_MEM; //buffer operation
	param.enc_dev = ENC_MODULE_PATH;
	param.mem_dev = MEMALLOC_MODULE_PATH;
	param.useVcmd = 1;
	const void *ewl_inst = EWLInit(&param);
	if (ewl_inst == NULL) {
		retStatus = -1;
		goto end;
	}
	init_config(&cfg);

	/* Step 1: Initialize an encoder instance */

	cfg.frameType = JPEGENC_YUV420_SEMIPLANAR;

	if ((ret = JpegEncInit(&cfg, &encoder, NULL)) != JPEGENC_OK) {
		/* Handle the error situation */
		retStatus = -1;
		goto end;
	}

	/* Step 2: Configuration of picture size */
	/* VGA resolution, no cropping */
	cfg.inputWidth = TEST_JPEG_WIDTH;
	cfg.codingWidth = TEST_JPEG_WIDTH;
	cfg.inputHeight = TEST_JPEG_HEIGHT;
	cfg.codingHeight = TEST_JPEG_HEIGHT;

	EWLLinearMem_t pictureMem;
	ret = EWLMallocLinear(ewl_inst, (cfg.inputWidth * cfg.inputHeight * 3 / 2), 2, &pictureMem);
	if (ret != EWL_OK) {
		pictureMem.virtualAddress = NULL;
		retStatus = -1;
		goto close;
	}

	EWLLinearMem_t outbufMem;
	ret = EWLMallocLinear(ewl_inst, outbuf_size, 2, &outbufMem);
	if (ret != EWL_OK) {
		outbufMem.virtualAddress = NULL;
		retStatus = -1;
		goto close;
	}

	if ((ret = JpegEncSetPictureSize(encoder, &cfg)) != JPEGENC_OK) {
		/* Handle the error situation */
		retStatus = -1;
		goto close;
	}

	/* Step 3: Allocate linear output buffer */
	outbuf_virt_address = outbufMem.busAddress;
	output_bus_address = (u32)outbuf_virt_address;
	encIn.pOutBuf[0] = outbuf_virt_address;
	encIn.busOutBuf[0] = output_bus_address;
	encIn.outBufSize[0] = outbuf_size; /* bytes */
	/* Enable all frame headers */
	encIn.frameHeader = 1;

	/* Step 4: Get the picture and setup input */
	/* Allocate linear picture memory and read the picture */
	FILE *inP = fopen(JPEG_TEST_FILE_NAME, "rb");
	if (inP == NULL) {
		printf("fail to open input file \r\n");
		retStatus = -1;
		goto close;
	}
	FILE *outP = fopen(JPEG_TEST_OUTPUT_FILE_NAME, "wb");
	if (outP == NULL) {
		printf("fail to open output file \r\n");
		retStatus = -1;
		goto close;
	}

	pict_bus_address = pictureMem.busAddress;
	fread(pict_bus_address, cfg.inputWidth * cfg.inputHeight * 3 / 2, 1, inP);
	/* We use one linear buffer for the input image: YYYY...UU...VV... */
	encIn.busLum = pict_bus_address;
	encIn.busCb = encIn.busLum + cfg.inputWidth * cfg.inputHeight;
	encIn.busCr = encIn.busCb + (cfg.inputWidth / 2) * (cfg.inputHeight / 2);
	/* Virtual addresses of input picture, used by software encoder */
	encIn.pLum = (u8 *)pictureMem.virtualAddress;
	encIn.pCb = encIn.pLum + cfg.inputWidth * cfg.inputHeight;
	encIn.pCr = encIn.pCb + (cfg.inputWidth / 2) * (cfg.inputHeight / 2);
	/* Step 5: Encode the picture */
	/* Loop until the frame is ready */
	u32 frame_size = 0;
	do {
		ret = JpegEncEncode(encoder, &encIn, &encOut);
		switch (ret) {
		case JPEGENC_RESTART_INTERVAL:
		case JPEGENC_FRAME_READY: {

			SaveStream(encIn.pOutBuf[0], encOut.jfifSize, outP, &crc32value);
			// dump_addr(encIn.pOutBuf[0], encOut.jfifSize);
			frame_size += encOut.jfifSize;
			if (ret == JPEGENC_FRAME_READY) {
				printf("JpegEncEncode OK, frame ready\r\n");
			}
			break;
		}
		default:
			/* All the others are error codes */
			printf("error: JpegEncEncode fail\r\n");
			retStatus = -1;
			goto close;
		}
	} while (ret == JPEGENC_RESTART_INTERVAL);

	printf("Frame encoded, %lu bytes\n\n", frame_size);
	//printf("stream CRC32: %x \r\n", enc_crc32((u8 *)encIn.pOutBuf[0], frame_size, 0));
	printf("stream CRC32: 0x%X \r\n", crc32value);

close:
	/* Last Step: Release the encoder instance */
	if (JpegEncRelease(encoder) != JPEGENC_OK) {
		/*
		There is not much to do for an error here,
		just a notification message. There has to be a fault
		somewhere else that caused a failure of the
		encoder release
		*/
		retStatus = -1;
	}
	/* release output buffer and picture buffer, if needed */
	/* Free all resources */
	if (inP) {
		fclose(inP);
		inP = NULL;
	}
	if (outP) {
		fclose(outP);
		outP = NULL;
	}
	EWLFreeLinear(ewl_inst, &pictureMem);
	EWLFreeLinear(ewl_inst, &outbufMem);
	// EWLRelease(ewl_inst);
end: /* nothing else to do */
	return retStatus;
}

void encode_frame(enc_user_data *user_data)
{
	i32 ret = OK;
	u32 pictureSize = 0;
	u32 outbufSize = 0;
	char *outputFile;
	struct test_bench tb;
	memset(&tb, 0, sizeof(struct test_bench));

	/* init VCMD driver */
	extern int encoder_rtos_driver_init(void);
	extern void encoder_rtos_driver_deinit(void);
	if (encoder_rtos_driver_init() != 0) {
		return NOK;
	}
	//printf("input %d %d\r\n", user_data->input_data_width, user_data->input_data_height);
	//printf("output %d %d\r\n", user_data->output_data_width, user_data->output_data_height);

	tb.busLuma = user_data->busLuma;
	tb.busChromaU = user_data->busChromaU;
	tb.busChromaV = user_data->busChromaV;


	output_data_addr = user_data->output_data_addr;
	tb.srcwidth = user_data->input_data_width;
	tb.srcheight = user_data->input_data_height;
	tb.width = user_data->output_data_width;
	tb.height = user_data->output_data_height;
	tb.inputFormat = VCENC_YUV420_SEMIPLANAR;
	tb.input_alignment = 0;
	tb.firstPic = 0;
	tb.lastPic = 1; //30;
	tb.picture_encoded_cnt = 0;
	if (user_data->codecFormat == VCENC_VIDEO_CODEC_HEVC) {
		outputFile = TEST_HEVC_OUTPUT_FILE_NAME;
		tb.codecFormat = VCENC_VIDEO_CODEC_HEVC;
		tb.level = VCENC_HEVC_LEVEL_6; //VCENC_AUTO_LEVEL;
		tb.profile = VCENC_HEVC_MAIN_PROFILE; //VCENC_ADAPTIVE_PROFILE;
		tb.log2_ctb_size = 6;
	} else {
		outputFile = TEST_H264_OUTPUT_FILE_NAME;
		tb.codecFormat = VCENC_VIDEO_CODEC_H264;
		tb.level = VCENC_H264_LEVEL_5_1;
		tb.profile = VCENC_H264_HIGH_PROFILE;
		tb.log2_ctb_size = 4;
	}

	wrap_num_to_enc = user_data->wrap_num_to_enc;
	low_latency_set = user_data->low_latency_set;
	user_timeout_disable = user_data->timeout_disable;

	const void *ewl_inst = InitEwlInst_simple(&tb);
	if (ewl_inst == NULL) {
		printf("error: InitEwlInst return NULL\r\n");
		return NOK;
	}
//#if 1
#if 0
	if ((ret = AllocateDMABuffer(&tb)) != VCENC_OK) {
		goto end;
	}

	if (!(tb.outP = fopen(outputFile, "wb"))) {
		printf("fail to open output file\r\n");
		return 1;
	}
	//tb.outP = outP;
#else
	u32 lumaSize = 0, chromaSize = 0;
	u32 alignment = tb.input_alignment == 0 ? 1 : tb.input_alignment;
	lumaSize = STRIDE(tb.width, alignment);
	chromaSize = STRIDE(tb.width / 2, alignment);
	tb.lumaSize = lumaSize * tb.height;
	tb.chromaSize = chromaSize * tb.height / 2 * 2;
	pictureSize = tb.lumaSize + tb.chromaSize;
	tb.pictureMem.mem_type = EXT_WR | VPU_RD | EWL_MEM_TYPE_DPB;
	SET_MEM_USAGE(tb.pictureMem.mem_type,
				  EWL_MEM_USAGE_IN_SURFACE, tb.secure_mode);
	tb.pictureMem.busAddress = user_data->input_data_addr;
	tb.pictureMem.virtualAddress = user_data->input_data_addr;
	tb.pictureMem.size = pictureSize;
	tb.pictureMem.allocVirtualAddr = user_data->input_data_addr;
	tb.pictureMem.allocBusAddr = user_data->input_data_addr;
	tb.pictureMem.total_size = pictureSize;

	outbufSize = 4 * pictureSize;
	outbufSize = CLIP3(OUTBUF_SIZE_MIN, OUTBUF_SIZE_MAX, outbufSize);
	tb.outbufMem.mem_type = EXT_WR | VPU_RD | EWL_MEM_TYPE_DPB;
	SET_MEM_USAGE(tb.outbufMem.mem_type,
				  EWL_MEM_USAGE_IN_SURFACE, tb.secure_mode);
	//printf("%s %d outbufSize = %d\r\n", __func__, __LINE__, outbufSize);
	//if (user_data->output_data_addr != NULL) {
	if (0) {
		tb.outbufMem.busAddress = user_data->output_data_addr;
		tb.outbufMem.virtualAddress = user_data->output_data_addr;
		tb.outbufMem.size = user_data->output_data_size;
		tb.outbufMem.allocVirtualAddr = user_data->output_data_addr;
		tb.outbufMem.allocBusAddr = user_data->output_data_addr;
		tb.outbufMem.total_size = user_data->output_data_size;
		//printf("%s %d outbufSize = %d\r\n", __func__, __LINE__, outbufSize);

	} else {
#if 1
		ret = EWLMallocLinear(ewl_inst, outbufSize, tb.input_alignment,
							  &tb.outbufMem);
		if (ret != EWL_OK) {
			tb.outbufMem.virtualAddress = NULL;
			return 1;
		}
#else
		tb.outbufMem.busAddress = 0x801B4000;
		tb.outbufMem.virtualAddress = 0x801B4000;
		tb.outbufMem.size = outbufSize;
		tb.outbufMem.allocVirtualAddr = 0x801B4000;
		tb.outbufMem.allocBusAddr = 0x801B4000;
		tb.outbufMem.total_size = outbufSize;
#endif
	}
#endif
//#endif
	if (user_data->output_data_addr == NULL) {
		if (!(tb.outP = fopen(outputFile, "wb"))) {
			printf("fail to open output file\r\n");
		}
	} else {
		printf("return result to caller\r\n");
	}

	if (Demo_Video_Encoding(&tb, user_data) != 0) {
		ret = NOK;
	}
	user_data->streamSize = tb.streamSize;

#if 1
	if (tb.outP) {
		fclose(tb.outP);
		tb.outP = NULL;
	}
#endif
#if 0
	FreeDMABuffer(&tb);
	FreeVariableMem(&tb);
#endif
end:
	//encoder_rtos_driver_deinit();
//#endif
	return ret;
}

void encode_frame_264(enc_user_data *user_data)
{
	i32 ret = OK;
	u32 outbufSize = 0;
	char *outputFile;
	struct test_bench tb;
	memset(&tb, 0, sizeof(struct test_bench));

	/* init VCMD driver */
	extern int encoder_rtos_driver_init(void);
	extern void encoder_rtos_driver_deinit(void);
	if (encoder_rtos_driver_init() != 0) {
		return 1;
	}
	outputFile = TEST_H264_OUTPUT_FILE_NAME;

	//intput_data_addr = user_data->input_data_addr;
	//output_data_width = user_data->output_data_width;
	//output_data_height = user_data->output_data_height;
	tb.srcwidth = user_data->input_data_width;
	tb.srcheight = user_data->input_data_height;
	tb.width = user_data->output_data_width;
	tb.height = user_data->output_data_height;
	tb.inputFormat = VCENC_YUV420_SEMIPLANAR;
	//input_alignment = 64;
	tb.input_alignment = 64; //input_alignment;
	tb.firstPic = 0;
	tb.lastPic = 1; //30;
	tb.picture_encoded_cnt = 0;
	tb.codecFormat = VCENC_VIDEO_CODEC_H264;
	tb.level = VCENC_H264_LEVEL_5_1;
	tb.profile = VCENC_H264_HIGH_PROFILE;
	tb.log2_ctb_size = 4;

	wrap_num_to_enc = user_data->wrap_num_to_enc;
	low_latency_set = user_data->low_latency_set;
	user_timeout_disable = user_data->timeout_disable;

	const void *ewl_inst = InitEwlInst_simple(&tb);
	if (ewl_inst == NULL) {
		printf("error: InitEwlInst return NULL\r\n");
		return 1;
	}

	if ((ret = AllocateDMABuffer(&tb)) != VCENC_OK) {
		goto end;
	}

	if (!(tb.outP = fopen(outputFile, "wb"))) {
		printf("fail to open output file\r\n");
		return 1;
	}
	//tb.outP = outP;

	if (Demo_Video_Encoding(&tb, user_data) != 0) {
		return 1;
	}

	if (tb.outP) {
		fclose(tb.outP);
		tb.outP = NULL;
	}

	FreeDMABuffer(&tb);
	FreeVariableMem(&tb);
end:
	encoder_rtos_driver_deinit();
	return ret;
}

void encode_frame_dec(enc_user_data *user_data)
{
	i32 ret = OK;
	u32 pictureSize = 0;
	u32 outbufSize = 0;
	char *outputFile;
	struct test_bench tb;
	memset(&tb, 0, sizeof(struct test_bench));

	/* init VCMD driver */
	extern int encoder_rtos_driver_init(void);
	extern void encoder_rtos_driver_deinit(void);
	if (encoder_rtos_driver_init() != 0) {
		return NOK;
	}

	tb.busLuma = user_data->busLuma;
	tb.busChromaU = user_data->busChromaU;
	tb.busChromaV = user_data->busChromaV;
	tb.dec400LumTableBase = user_data->dec400LumTableBase;
	tb.dec400CbTableBase = user_data->dec400CbTableBase;
	tb.dec400CrTableBase = user_data->dec400CrTableBase;
	tb.dec400Enable = 2;

	output_data_addr = user_data->output_data_addr;
	tb.srcwidth = user_data->input_data_width;
	tb.srcheight = user_data->input_data_height;
	tb.width = user_data->output_data_width;
	tb.height = user_data->output_data_height;
	tb.inputFormat = VCENC_YUV420_SEMIPLANAR;
	tb.input_alignment = 64;
	tb.firstPic = 0;
	tb.lastPic = 1; //30;
	tb.picture_encoded_cnt = 0;
	if (user_data->codecFormat == VCENC_VIDEO_CODEC_HEVC) {
		outputFile = TEST_DEC_OUTPUT_HEVC_FILE_NAME;
		tb.codecFormat = VCENC_VIDEO_CODEC_HEVC;
		tb.level = VCENC_AUTO_LEVEL;
		tb.profile = VCENC_ADAPTIVE_PROFILE;
		tb.log2_ctb_size = 6;
	} else {
		outputFile = TEST_DEC_OUTPUT_H264_FILE_NAME;
		tb.codecFormat = VCENC_VIDEO_CODEC_H264;
		tb.level = VCENC_H264_LEVEL_5_1;
		tb.profile = VCENC_H264_HIGH_PROFILE;
		tb.log2_ctb_size = 4;
	}

	wrap_num_to_enc = user_data->wrap_num_to_enc;
	low_latency_set = user_data->low_latency_set;
	user_timeout_disable = user_data->timeout_disable;
	if (user_data->busLuma == NULL) {
		printf("error: input data address is NULL\r\n");
		return NOK;
	}
	if (user_data->dec400LumTableBase == NULL) {
		printf("error: DEC table address is NULL\r\n");
		return NOK;
	}

	const void *ewl_inst = InitEwlInst_simple(&tb);
	if (ewl_inst == NULL) {
		printf("error: InitEwlInst return NULL\r\n");
		return NOK;
	}

	u32 ufbcHeaderSize = 0;
	tb.ufbcEnable = 3;
	tb.inputFormat = 1;
	tb.ufbcBlockType = 0;
	if (tb.ufbcEnable) {
		u32 asic_format = 0;
		EncUfbcGetSize(tb.inputFormat, tb.ufbcBlockType, tb.width, tb.height,
					   &asic_format, &ufbcHeaderSize, &pictureSize);
	}

	outbufSize = 4 * (pictureSize + ufbcHeaderSize);
	outbufSize = CLIP3(OUTBUF_SIZE_MIN, OUTBUF_SIZE_MAX, outbufSize);
	tb.outbufMem.mem_type = EXT_WR | VPU_RD | EWL_MEM_TYPE_DPB;
	SET_MEM_USAGE(tb.outbufMem.mem_type,
				  EWL_MEM_USAGE_IN_SURFACE, tb.secure_mode);
	//if (user_data->output_data_addr != NULL) {
	if (0) {
		tb.outbufMem.busAddress = user_data->output_data_addr;;
		tb.outbufMem.virtualAddress = user_data->output_data_addr;;
		tb.outbufMem.size = user_data->output_data_size;
		tb.outbufMem.allocVirtualAddr = user_data->output_data_addr;;
		tb.outbufMem.allocBusAddr = user_data->output_data_addr;;
		tb.outbufMem.total_size = user_data->output_data_size;

	} else {
#if 1
		ret = EWLMallocLinear(ewl_inst, outbufSize, tb.input_alignment,
							  &tb.outbufMem);
		if (ret != EWL_OK) {
			tb.outbufMem.virtualAddress = NULL;
			return 1;
		}
#else
		tb.outbufMem.busAddress = 0x801B4000;
		tb.outbufMem.virtualAddress = 0x801B4000;
		tb.outbufMem.size = outbufSize;
		tb.outbufMem.allocVirtualAddr = 0x801B4000;
		tb.outbufMem.allocBusAddr = 0x801B4000;
		tb.outbufMem.total_size = outbufSize;
#endif
	}

	if (user_data->output_data_addr == NULL) {
		if (!(tb.outP = fopen(outputFile, "wb"))) {
			printf("fail to open output file\r\n");
		}
	} else {
		printf("return result to caller\r\n");
	}

	if (Demo_Video_Encoding(&tb, user_data) != 0) {
		ret = NOK;
	}
	user_data->streamSize = tb.streamSize;
error:
	if (tb.outP) {
		fclose(tb.outP);
		tb.outP = NULL;
	}

	if (user_data->output_data_addr == NULL) {
		EWLFreeLinear(ewl_inst, &tb.outbufMem);
	}

	FreeVariableMem(&tb);
end:
	encoder_rtos_driver_deinit();
	return ret;
}

