/* --------------------------*/
/* ENC verification           */
/* --------------------------*/
#include "ameba_soc.h"
#include <diag.h>
#include <log.h>

#include "stdio.h"

#include <FreeRTOS.h>
#include <task.h>

/* ENC header */
#include "hevcencapi.h"
#include "ewl.h"

extern void dump_addr(u8 *buf, int size);

static uint32_t crc32(const unsigned char *message, uint32_t len)
{
	int i, j;
	uint32_t byte, crc, mask;

	i = 0;
	crc = 0xFFFFFFFF;
	while (i < len) {
		byte = message[i];            // Get next byte.
		crc = crc ^ byte;
		for (j = 7; j >= 0; j--) {    // Do eight times.
			mask = -(crc & 1);
			crc = (crc >> 1) ^ (0xEDB88320 & mask);
		}
		i = i + 1;
	}
	return ~crc;
}

static int pass_cnt = 0, fail_cnt = 0;

#define TEST(condition, message, ...)       \
    do {                                    \
        if (!(condition)) {                 \
            DiagPrintf("[FAIL] ");          \
            fail_cnt++;                     \
        } else {                            \
            DiagPrintf("[PASS] ");          \
            pass_cnt++;                     \
        }                                   \
        DiagPrintf(message, ##__VA_ARGS__); \
        DiagPrintf("\r\n\r\n");             \
    } while (0)


/*------------------------------------------------------------------------------
-- --
-- A Simple Demo of Encoding 300 Frames (HEVC) --
-- --
--------------------------------------------------------------------------------*/
#include "hevcencapi.h"
#include "ewl.h"

#define TEST_HEVC_YUV_FILE_NAME "D:\\FPGA_test_data\\jay_192x128_yuv420.yuv"
#define TEST_HEVC_OUTPUT_FILE_NAME "D:\\FPGA_test_data\\result.hevc"

#define TEST_HEVC_WIDTH     192
#define TEST_HEVC_HEIGHT    128
struct test_bench {
	char *inputFile;
	char *outputFile;
	FILE *inP;
	FILE *outP;
	i32 width;
	i32 height;
	i32 firstPic;
	i32 lastPic;
	i32 picture_encoded_cnt; /*count of frame finish encoding*/
	u8 *lum;
	u8 *cb;
	u8 *cr;
	u32 input_alignment;
	u32 lumaSize;
	u32 chromaSize;
	EWLLinearMem_t pictureMem;
	EWLLinearMem_t outbufMem;
	u32 gopSize;
	VCEncIn encIn;
	const void *ewlInst;
	i32 *tile_width;
	i32 *tile_height;
};
/*Init tb’s ewl instance to malloc/free buffer in test_bench*/
static const void *InitEwlInst(struct test_bench *tb)
{
	if (NULL == tb->ewlInst) {
		/* Init EWL instance for test bench to malloc/free linear buffer*/
		EWLInitParam_t param;
		param.context = NULL;
		param.enc_dev = ENC_MODULE_PATH;
		param.mem_dev = MEMALLOC_MODULE_PATH;
		param.clientType = EWL_CLIENT_TYPE_MEM; //buffer operation
		param.slice_idx = 0;
		param.useVcmd = 1; //-1;
		if ((tb->ewlInst = EWLInit(&param)) == NULL) {
			return NULL;
		}
	}
	return tb->ewlInst;
}
static int AllocateDMABuffer(struct test_bench *tb)
{
	u32 lumaSize = 0, chromaSize = 0;
	u32 pictureSize = 0;
	u32 outbufSize = 0;
	int ret = EWL_OK;
	/* picture buffer size YUV 420 */
	lumaSize = STRIDE(tb->width, tb->input_alignment);
	chromaSize = STRIDE(tb->width / 2, tb->input_alignment);
	tb->lumaSize = lumaSize * tb->height;
	tb->chromaSize = chromaSize * tb->height / 2 * 2;
	pictureSize = tb->lumaSize + tb->chromaSize;
	// DiagPrintf("tb->lumaSize:%d\r\n", tb->lumaSize);
	// DiagPrintf("tb->chromaSize:%d\r\n", tb->chromaSize);
	ret = EWLMallocLinear(tb->ewlInst, pictureSize, tb->input_alignment,
						  &tb->pictureMem);
	if (ret != EWL_OK) {
		DiagPrintf("EWLMallocLinear1 fail with ret:%d\r\n", ret);
		tb->pictureMem.virtualAddress = NULL;
		return 1;
	}
	//DiagPrintf(">> tb->pictureMem.virtualAddress:%p\r\n", tb->pictureMem.virtualAddress);
	//DiagPrintf(">> tb->pictureMem.busAddress:%p\r\n", tb->pictureMem.busAddress);
	/* Output buffer size */
	outbufSize = pictureSize; //0.5 * pictureSize; //4 * pictureSize;
	ret = EWLMallocLinear(tb->ewlInst, outbufSize, tb->input_alignment,
						  &tb->outbufMem);
	if (ret != EWL_OK) {
		DiagPrintf("EWLMallocLinear2 fail with ret:%d\r\n", ret);
		tb->outbufMem.virtualAddress = NULL;
		return 1;
	}
	//DiagPrintf(">> tb->outbufMem.virtualAddress:%p\r\n", tb->outbufMem.virtualAddress);
	//DiagPrintf(">> tb->outbufMem.busAddress:%p\r\n", tb->outbufMem.busAddress);
	return ret;
}
static void FreeDMABuffer(struct test_bench *tb)
{
	const void *ewl_inst = tb->ewlInst;
	EWLFreeLinear(ewl_inst, &tb->pictureMem);
	// EWLFreeLinear(ewl_inst, &tb->outbufMem);
	return;
}
static i32 allocVariableMem(struct test_bench *tb)
{
	tb->tile_width = (i32 *)calloc(1, sizeof(i32));
	if (tb->tile_width == NULL) {
		return 0;
	}
	tb->tile_height = (i32 *)calloc(1, sizeof(i32));
	if (tb->tile_height == NULL) {
		return 0;
	}
	return 1;
}
static void FreeVariableMem(struct test_bench *tb)
{
	free(tb->tile_width);
	free(tb->tile_height);
}
static i32 file_read(FILE *file, u8 *data, u32 seek, size_t size)
{
	if ((file == NULL) || (data == NULL)) {
		return NOK;
	}
	fseek(file, seek, SEEK_SET);
	if (fread(data, sizeof(u8), size, file) < size) {
		if (!feof(file)) {
			DiagPrintf("Read input picture error!nn");
		}
		return NOK;
	}
	return OK;
}
static i32 ReadYuv420PlanarFrame(struct test_bench *tb, u32 src_img_size,
								 u32 src_width, u32 src_height)
{
	DiagPrintf("Read YUV420 from file...\r\n");
	static i32 num = 0;
	u32 seek;
	i32 w;
	i32 h;
	u8 *lum;
	u8 *cb;
	u8 *cr;
	i32 i;
	u32 luma_stride;
	u32 chroma_stride;
	u32 alignment;
	seek = ((u32)num++) * ((u32)src_img_size);
	lum = tb->lum;
	cb = tb->cb;
	cr = tb->cr;
	alignment = tb->input_alignment;
	luma_stride = STRIDE(src_width, alignment);
	chroma_stride = STRIDE(src_width / 2, alignment);
	/* Lum */
	for (i = 0; i < src_height; i++) {
		if (file_read(tb->inP, lum, seek, src_width)) {
			return NOK;
		}
		seek += src_width;
		lum += luma_stride;
	}
	w = ((src_width + 1) >> 1);
	h = ((src_height + 1) >> 1);
	/* Cb */
	for (i = 0; i < h; i++) {
		if (file_read(tb->inP, cb, seek, w)) {
			return NOK;
		}
		seek += w;
		cb += chroma_stride;
	}
	/* Cr */
	for (i = 0; i < h; i++) {
		if (file_read(tb->inP, cr, seek, w)) {
			return NOK;
		}
		seek += w;
		cr += chroma_stride;
	}
	DiagPrintf("Read YUV420 from file done.\r\n");
	return 0;
}

uint8_t tmp_output_buf[TEST_HEVC_WIDTH * TEST_HEVC_HEIGHT];

#define SAVE_OUTPUT_TO_MEM  1

static int SaveStream(u8 *buf, u32 size, FILE *fp)
{
	int ret = 0;
#if SAVE_OUTPUT_TO_MEM
	static int cur = 0;
	memcpy(&tmp_output_buf[cur], buf, size);
	//DiagPrintf("Save Stream to mem addr:0x%x with cur:%d, size:%d\r\n", tmp_output_buf, cur, size);
	cur += size;
#else
	ret = fwrite(buf, 1, size, fp);
#endif
	return ret;
}

static int HevcInitGopConfigs(int gopSize, VCEncGopConfig *gopCfg,
							  u8 *gopCfgOffset)
{
	VCEncGopPicConfig *cfg = NULL;
	gopCfg->special_size = 0;
	gopCfg->ltrcnt = 0;
	/* gopCfgOffset: keep record of gopSize’s offset in pGopPicCfg */
	if (gopCfgOffset) {
		gopCfgOffset[gopSize] = gopCfg->size;
	}
	/* gop_config : #Frame1: P 1 0 0.578 0 1 -1 1 */
	cfg = &(gopCfg->pGopPicCfg[gopCfg->size++]);
	cfg->poc = 1;
	;
	cfg->QpOffset = 0;
	cfg->QpFactor = 0.760263; //sqrt(0.578)
	cfg->temporalId = 0;
	cfg->codingType = 1;
	cfg->nonReference = 0;
	cfg->numRefPics = 1;
	cfg->refPics->ref_pic = -1;
	cfg->refPics->used_by_cur = 1;
	return 0;
}

#include "jay_192x128_yuv420.c"
#define INPUT_RAW_DATA  jay_192x128_yuv420

#if 1
#define DEBUG_KEVIN     do { \
    DiagPrintf(">>>[DEBUG] %s:%d\r\n", __func__, __LINE__);\
    HAL_WRITE32(0x4020205c, 0, 'a');\
    HAL_WRITE32(0x28014000, 0, __LINE__);\
} while(0);
#else
#define DEBUG_KEVIN
#endif

#define DEBUG_UART(x)     do { \
    HAL_WRITE32(0x4020205c, 0, x);\
} while(0);

/* --------------------- */
/* -- HEVC demo entry -- */
/* --------------------- */
static int Demo_HEVC_Encoding(void)
{
	i32 ret = OK;
	int i;
	VCEncConfig cfg;
	DEBUG_KEVIN
	struct test_bench tb;
	VCEncInst hevc_encoder;
	VCEncGopPicConfig gopPicCfg[MAX_GOP_PIC_CONFIG_NUM];
	VCEncIn *pEncIn = &(tb.encIn);
	VCEncOut encOut;
	u32 streamSize = 0;
	u32 src_img_size;
	memset(&tb, 0, sizeof(struct test_bench));
	DEBUG_KEVIN
	const void *ewl_inst = InitEwlInst(&tb);
	DEBUG_KEVIN
	if (ewl_inst == NULL) {
		DiagPrintf("error: InitEwlInst return NULL\r\n");
		ret = NOK;
		goto error;
	}
	/* Step 1: Initialize an hevc encoder instance */
	DEBUG_KEVIN
	memset(&cfg, 0, sizeof(VCEncConfig));
	DEBUG_KEVIN
	/* Image Resolution */
	cfg.width = TEST_HEVC_WIDTH;
	cfg.height = TEST_HEVC_HEIGHT;
	/* 30 fps Frame Rate */
	cfg.frameRateDenom = 1;
	cfg.frameRateNum = 30;
	/* Stream Type */
	cfg.streamType = VCENC_BYTE_STREAM;
	cfg.level = VCENC_HEVC_LEVEL_6;
	cfg.tier = VCENC_HEVC_MAIN_TIER;
	cfg.profile = VCENC_HEVC_MAIN_PROFILE;
	/* Codec Format */
	cfg.codecFormat = VCENC_VIDEO_CODEC_HEVC;
	cfg.bitDepthLuma = 8;
	cfg.bitDepthChroma = 8;
	cfg.maxTLayers = 1;
	cfg.refFrameAmount = 1;
	cfg.enableSsim = 0; //1;
	cfg.enablePsnr = 0; //1;
	cfg.parallelCoreNum = 1;
	cfg.dumpCuInfo = 0;
	cfg.log2MaxPicOrderCntLsb = 16;
	cfg.log2MaxFrameNum = 12;
	cfg.interlacedFrame = 0;
	cfg.inLoopDSRatio = 1;
	cfg.cuInfoVersion = -1;
	cfg.extSramLumHeightBwd = 16;
	cfg.extSramChrHeightBwd = 8;
	cfg.extSramLumHeightFwd = 16;
	cfg.extSramChrHeightFwd = 8;
	cfg.gopSize = 1;
	cfg.codedChromaIdc = VCENC_CHROMA_IDC_420;
	cfg.writeReconToDDR = 1;
	cfg.refRingBufEnable = 1;
	cfg.av1InterFiltSwitch = 1;
	cfg.tune = VCENC_TUNE_PSNR;
	DEBUG_KEVIN
	allocVariableMem(&tb);
	DEBUG_KEVIN
	cfg.tile_width = tb.tile_width;
	cfg.tile_height = tb.tile_height;
	cfg.tiles_enabled_flag = 0;
	cfg.num_tile_columns = 1;
	cfg.num_tile_rows = 1;
	cfg.loop_filter_across_tiles_enabled_flag = 0;
	u32 log2_ctb_size = 6;
	i32 width = cfg.width;
	i32 height = cfg.height;
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
	if ((ret = VCEncInit(&cfg, &hevc_encoder, NULL)) != VCENC_OK) {
		DiagPrintf("VCEncInit fail with ret = %d\r\n", ret);
		return (int)ret;
	}
	DEBUG_KEVIN
	tb.inputFile = TEST_HEVC_YUV_FILE_NAME;
	tb.outputFile = TEST_HEVC_OUTPUT_FILE_NAME;
	tb.firstPic = 0;
	tb.lastPic = 1; //30;
	tb.width = TEST_HEVC_WIDTH;
	tb.height = TEST_HEVC_HEIGHT;
	tb.input_alignment = 64; //64;
	tb.picture_encoded_cnt = 0;
	DEBUG_KEVIN
	/* Step 2: Extra encoder configuration. See the next example code for how to change the
	* default encoder parameters
	*/
	/* GOP configuration */
	DEBUG_KEVIN
	memset(gopPicCfg, 0, sizeof(gopPicCfg));
	DEBUG_KEVIN
	tb.encIn.gopConfig.pGopPicCfg = gopPicCfg;
	DEBUG_KEVIN
	HevcInitGopConfigs(tb.gopSize, &(tb.encIn.gopConfig),
					   tb.encIn.gopConfig.gopCfgOffset);
	DEBUG_KEVIN
	tb.encIn.gopConfig.outputRateNumer = 30; /* Output frame rate numerator */
	tb.encIn.gopConfig.outputRateDenom = 1; /* Output frame rate denominator */
	tb.encIn.gopConfig.inputRateNumer = 30; /* Input frame rate numerator */
	tb.encIn.gopConfig.inputRateDenom = 1; /* Input frame rate denominator */
	/* Step 3: Allocate linear memory resources (input and output buffers) */
	DEBUG_KEVIN
	if ((ret = AllocateDMABuffer(&tb)) != VCENC_OK) {
		goto end;
	}
	DEBUG_KEVIN
	/* Step 4: Start the stream */
	// VCEncRateCtrl rcCfg = {0};
	// if ((ret = VCEncGetRateCtrl(hevc_encoder, &rcCfg)) != VCENC_OK) {
	// DiagPrintf("error: VCEncGetRateCtrl fail with ret:%d\r\n", ret);
	// ret = -1;
	// goto error;
	// }
	// DiagPrintf(
	// "Get rate control: qp %2d qpRange I[%2u, %2u] PB[%2u, %2u] %8u bps  "
	// "pic %u skip %u  hrd %u  cpbSize %u cpbMaxRate %u bitrateWindow %u "
	// "intraQpDelta %2d\n",
	// rcCfg.qpHdr, rcCfg.qpMinI, rcCfg.qpMaxI, rcCfg.qpMinPB, rcCfg.qpMaxPB,
	// rcCfg.bitPerSecond, rcCfg.pictureRc, rcCfg.pictureSkip, rcCfg.hrd,
	// rcCfg.hrdCpbSize, rcCfg.cpbMaxRate, rcCfg.bitrateWindow,
	// rcCfg.intraQpDelta);
	// rcCfg.bitPerSecond = 128 * 1024;
	// rcCfg.qpHdr = -1;
	// rcCfg.qpMinI = 45;
	// rcCfg.qpMaxI = 50;
	// rcCfg.intraQpDelta = -5;
	// DiagPrintf(
	// "Set rate control: qp %2d qpRange I[%2u, %2u] PB[%2u, %2u] %9u bps  "
	// "pic %u skip %u  hrd %u"
	// "  cpbSize %u cpbMaxRate %u bitrateWindow %u intraQpDelta %2d "
	// "fixedIntraQp %2u\n",
	// rcCfg.qpHdr, rcCfg.qpMinI, rcCfg.qpMaxI, rcCfg.qpMinPB, rcCfg.qpMaxPB,
	// rcCfg.bitPerSecond, rcCfg.pictureRc, rcCfg.pictureSkip, rcCfg.hrd,
	// rcCfg.hrdCpbSize, rcCfg.cpbMaxRate, rcCfg.bitrateWindow,
	// rcCfg.intraQpDelta, rcCfg.fixedIntraQp);
	// if ((ret = VCEncSetRateCtrl(hevc_encoder, &rcCfg)) != VCENC_OK) {
	// DiagPrintf("error: VCEncSetRateCtrl fail with ret:%d\r\n", ret);
	// ret = -1;
	// goto error;
	// }
	/* Output/input streams */
	// if (!(tb.inP = fopen(tb.inputFile, "rb"))) {
	// DiagPrintf("fail to open input file\r\n");
	// goto error;
	// }
	// if (!(tb.outP = fopen(tb.outputFile, "wb"))) {
	// DiagPrintf("fail to open output file\r\n");
	// goto error;
	// }
	pEncIn->busOutBuf[0] = tb.outbufMem.busAddress; //malloc(192 * 128); //tb.outbufMem.busAddress;
	pEncIn->outBufSize[0] = tb.outbufMem.size; //192 * 128; //tb.outbufMem.size;
	pEncIn->pOutBuf[0] = pEncIn->busOutBuf[0];
	pEncIn->cur_out_buffer[0] = &tb.outbufMem;
	pEncIn->dec400Enable = 0;
	/* Video, sequence and picture parameter sets */
	if (VCEncStrmStart(hevc_encoder, pEncIn, &encOut)) {
		/* Handle the error situation here*/
		DiagPrintf("error: VCEncStrmStart fail\r\n");
		goto error;
	}
	/* Save the stream header */
	DEBUG_KEVIN
	SaveStream((u8 *)pEncIn->pOutBuf[0], encOut.streamSize, tb.outP);
	DEBUG_KEVIN
	streamSize += encOut.streamSize;
	DEBUG_KEVIN
	// dump_addr(pEncIn->pOutBuf[0], encOut.streamSize);
	/* Step 5: Encode the video frames */
	while (HANTRO_TRUE) {
		DEBUG_KEVIN
		// if (tb.picture_encoded_cnt >= tb.lastPic) {
		// break;
		// }
		/* Setup encoder input, we assume one linear buffer for the input image in */
		/* planar YCbCr 4:2:0 format */
		pEncIn->busLuma = tb.pictureMem.busAddress;
		tb.lum = (u8 *)tb.pictureMem.virtualAddress;
		pEncIn->busChromaU = pEncIn->busLuma + tb.lumaSize;
		tb.cb = tb.lum + tb.lumaSize;
		pEncIn->busChromaV = pEncIn->busChromaU + tb.chromaSize / 2;
		tb.cr = tb.cb + tb.chromaSize / 2;
		src_img_size =
			tb.width * tb.height * 12 / 8; //bit_pre_pixel of yuv420 is 12
		/* Read YUV to input buffer */
		// int test_y_size = sizeof(INPUT_RAW_DATA) * 2 / 3;
		// int test_u_size = test_y_size / 2 / 2;
		// DiagPrintf(">>test_y_size:%d test_u_size:%d \r\n", test_y_size, test_u_size);
		// memcpy(pEncIn->busLuma, (uint8_t *)INPUT_RAW_DATA, test_y_size);
		// memcpy(pEncIn->busChromaU, (uint8_t *)INPUT_RAW_DATA + test_y_size, test_u_size);
		// memcpy(pEncIn->busChromaV, (uint8_t *)INPUT_RAW_DATA + test_y_size + test_u_size, test_u_size);
		// DiagPrintf(">>busLuma:0x%x busChromaU:0x%x busChromaV:0x%x\r\n", pEncIn->busLuma, pEncIn->busChromaU, pEncIn->busChromaV);
		//memcpy(pEncIn->busLuma, INPUT_RAW_DATA, 128 * 128 * 3 / 2);
		// ReadYuv420PlanarFrame(&tb, src_img_size, tb.width, tb.height);
		DEBUG_KEVIN
		//DiagPrintf("=== Encoding %i %s ...\n\n", tb.picture_encoded_cnt, tb.inputFile);
		ret = VCEncStrmEncode(hevc_encoder, pEncIn, &encOut, NULL, NULL);
		/* Save encoded stream */
		switch (ret) {
		case VCENC_FRAME_ENQUEUE:
			pEncIn->picture_cnt++;
			// break;
			continue;
		case VCENC_FRAME_READY:
			if (encOut.codingType != VCENC_NOTCODED_FRAME) {
				pEncIn->picture_cnt++;
			}
			if (encOut.streamSize != 0) {
				SaveStream((u8 *)pEncIn->pOutBuf[0], encOut.streamSize, tb.outP);
				streamSize += encOut.streamSize;
				tb.picture_encoded_cnt++;
				// DiagPrintf(">> VCENC_FRAME_READY:\r\n");
				// dump_addr(pEncIn->pOutBuf[0], encOut.streamSize);
				DiagPrintf("VCEncStrmEncode OK, frame ready [%d]\r\n", tb.picture_encoded_cnt);
				vTaskDelay(10);
			}
			// break;
			continue;
		case VCENC_SYSTEM_ERROR:
			/* Fatal system error, can’t continue. */
			continue;
		default:
			goto error;
		}
	} /* End of main encoding loop */
	/* Flush encoder internal buffered frames
	* In Multicore enabled or 2-pass enabled, encoder internal cached frames
	* must be flushed
	*/
	DEBUG_KEVIN
	ret = VCEncFlush(hevc_encoder, pEncIn, &encOut, NULL, NULL);
	/* Step 6: End stream */
	ret = VCEncStrmEnd(hevc_encoder, pEncIn, &encOut);
	if (ret == VCENC_OK) {
		SaveStream((u8 *)pEncIn->pOutBuf[0], encOut.streamSize, tb.outP);
		streamSize += encOut.streamSize;
	} else {
		/* Handle the error situation here*/
		goto error;
	}
	DiagPrintf(">> End stream:\r\n");
	// dump_addr(pEncIn->pOutBuf[0], encOut.streamSize);
	//dump_addr(pEncIn->pOutBuf[0], 128);

	DiagPrintf("Total of %d frames encoded, %lu bytes\n\n", tb.picture_encoded_cnt,
			   (long unsigned int)streamSize);
	DiagPrintf("stream CRC32: %x \r\n", crc32(tmp_output_buf, streamSize));
error:
	/* Free all resources */
	// if (tb.inP) {
	// fclose(tb.inP);
	// tb.inP = NULL;
	// }
	// if (tb.outP) {
	// fclose(tb.outP);
	// tb.outP = NULL;
	// }
	FreeDMABuffer(&tb);
	FreeVariableMem(&tb);
	// EWLRelease(ewl_inst);
end:
	/* Last Step: Release the encoder instance */
	VCEncRelease(hevc_encoder);
	return ret;
}


/*------------------------------------------------------------------------------
-- --
-- A Simple Demo of Encoding 300 Frames (H264) --
-- --
--------------------------------------------------------------------------------*/
#define TEST_H264_YUV_FILE_NAME "D:\\FPGA_test_data\\jay_192x128_yuv420.yuv"
#define TEST_H264_OUTPUT_FILE_NAME "D:\\FPGA_test_data\\result.h264"

#define TEST_H264_WIDTH 192
#define TEST_H264_HEIGHT 128
static int H264InitGopConfigs(int gopSize, VCEncGopConfig *gopCfg,
							  u8 *gopCfgOffset)
{
	VCEncGopPicConfig *cfg = NULL;
	gopCfg->special_size = 0;
	gopCfg->ltrcnt = 0;
	/* gopCfgOffset: keep record of gopSize’s offset in pGopPicCfg */
	if (gopCfgOffset) {
		gopCfgOffset[gopSize] = gopCfg->size;
	}
	/* gop_config : #Frame1: P 1 0 0.578 0 1 -1 1 */
	cfg = &(gopCfg->pGopPicCfg[gopCfg->size++]);
	cfg->poc = 1;
	;
	cfg->QpOffset = 0;
	cfg->QpFactor = 0.760263; //sqrt(0.578)
	cfg->temporalId = 0;
	cfg->codingType = 1;
	cfg->nonReference = 0;
	cfg->numRefPics = 1;
	cfg->refPics->ref_pic = -1;
	cfg->refPics->used_by_cur = 1;
	return 0;
}
/* --------------------- */
/* -- H264 demo entry -- */
/* --------------------- */
static int Demo_H264_Encoding(void)
{
	i32 ret = OK;
	int i;
	VCEncConfig cfg;
	struct test_bench tb;
	VCEncInst h264_encoder;
	VCEncGopPicConfig gopPicCfg[MAX_GOP_PIC_CONFIG_NUM];
	VCEncIn *pEncIn = &(tb.encIn);
	VCEncOut encOut;
	u32 streamSize = 0;
	u32 src_img_size;
	memset(&tb, 0, sizeof(struct test_bench));
	const void *ewl_inst = InitEwlInst(&tb);
	/* Step 1: Initialize an h264 encoder instance */
	memset(&cfg, 0, sizeof(VCEncConfig));
	/* Image Resolution */
	cfg.width = TEST_H264_WIDTH;
	cfg.height = TEST_H264_HEIGHT;
	/* 30 fps Frame Rate */
	cfg.frameRateDenom = 1;
	cfg.frameRateNum = 30;
	/* Stream Type */
	cfg.streamType = VCENC_BYTE_STREAM;
	cfg.level = VCENC_H264_LEVEL_5_1;
	cfg.tier = VCENC_HEVC_MAIN_TIER;
	cfg.profile = VCENC_H264_HIGH_PROFILE;
	/* Codec Format */
	cfg.codecFormat = VCENC_VIDEO_CODEC_H264;
	cfg.bitDepthLuma = 8;
	cfg.bitDepthChroma = 8;
	cfg.maxTLayers = 1;
	cfg.refFrameAmount = 1; //4;
	cfg.enableSsim = 0; //1;
	cfg.enablePsnr = 0; //1;
	cfg.parallelCoreNum = 1;
	cfg.dumpCuInfo = 0;
	cfg.log2MaxPicOrderCntLsb = 16;
	cfg.log2MaxFrameNum = 12;
	cfg.interlacedFrame = 0;
	cfg.inLoopDSRatio = 1;
	cfg.cuInfoVersion = -1;
	cfg.extSramLumHeightBwd = 16;
	cfg.extSramChrHeightBwd = 8;
	cfg.extSramLumHeightFwd = 16;
	cfg.extSramChrHeightFwd = 8;
	cfg.gopSize = 1;
	cfg.codedChromaIdc = VCENC_CHROMA_IDC_420;
	cfg.refRingBufEnable = 1;
	cfg.writeReconToDDR = 1;
	cfg.av1InterFiltSwitch = 1;
	cfg.tune = VCENC_TUNE_PSNR;
	allocVariableMem(&tb);
	cfg.tile_width = tb.tile_width;
	cfg.tile_height = tb.tile_height;
	cfg.tiles_enabled_flag = 0;
	cfg.num_tile_columns = 1;
	cfg.num_tile_rows = 1;
	cfg.loop_filter_across_tiles_enabled_flag = 0;
	u32 log2_ctb_size = 4;
	i32 width = cfg.width;
	i32 height = cfg.height;
	u32 ctbPerRow = ((width + (1 << log2_ctb_size) - 1) >> log2_ctb_size);
	u32 ctbPerColumn = ((height + (1 << log2_ctb_size) - 1) >> log2_ctb_size);
	u32 tileId;
	for (tileId = 0; tileId < cfg.num_tile_columns; tileId++) {
		cfg.tile_width[tileId] =
			(tileId + 1) * ctbPerRow / cfg.num_tile_columns - (tileId * ctbPerRow) / cfg.num_tile_columns;
	}
	for (tileId = 0; tileId < cfg.num_tile_rows; tileId++) {
		cfg.tile_height[tileId] =
			(tileId + 1) * ctbPerColumn / cfg.num_tile_rows -
			(tileId * ctbPerColumn) / cfg.num_tile_rows;
	}
	if ((ret = VCEncInit(&cfg, &h264_encoder, NULL)) != VCENC_OK) {
		return (int)ret;
	}
	tb.inputFile = TEST_H264_YUV_FILE_NAME;
	tb.outputFile = TEST_H264_OUTPUT_FILE_NAME;
	tb.firstPic = 0;
	tb.lastPic = 1; //30;
	tb.width = TEST_H264_WIDTH;
	tb.height = TEST_H264_HEIGHT;
	tb.input_alignment = 64;
	tb.picture_encoded_cnt = 0;
	/* Step 2: Extra encoder configuration. See the next example code for how to change the
	* default encoder parameters
	*/
	/* GOP configuration */
	memset(gopPicCfg, 0, sizeof(gopPicCfg));
	tb.encIn.gopConfig.pGopPicCfg = gopPicCfg;
	H264InitGopConfigs(tb.gopSize, &(tb.encIn.gopConfig),
					   tb.encIn.gopConfig.gopCfgOffset);
	tb.encIn.gopConfig.outputRateNumer = 30; /* Output frame rate numerator */
	tb.encIn.gopConfig.outputRateDenom = 1; /* Output frame rate denominator */
	tb.encIn.gopConfig.inputRateNumer = 30; /* Input frame rate numerator */
	tb.encIn.gopConfig.inputRateDenom = 1; /* Input frame rate denominator */
	/* Step 3: Allocate linear memory resources (input and output buffers) */
	if ((ret = AllocateDMABuffer(&tb)) != VCENC_OK) {
		goto end;
	}
	/* Step 4: Start the stream */
	/* Output/input streams */
	if (!(tb.inP = fopen(tb.inputFile, "rb"))) {
		goto error;
	}
	if (!(tb.outP = fopen(tb.outputFile, "wb"))) {
		goto error;
	}
	pEncIn->busOutBuf[0] = tb.outbufMem.busAddress;
	pEncIn->outBufSize[0] = tb.outbufMem.size;
	pEncIn->pOutBuf[0] = tb.outbufMem.virtualAddress;
	pEncIn->cur_out_buffer[0] = &tb.outbufMem;
	pEncIn->dec400Enable = 0;
	for (i = 0; i < VCENC_MAX_LT_REF_FRAMES; i++) {
		pEncIn->long_term_ref_pic[i] = INVALITED_POC;
	}
	/* Video, sequence and picture parameter sets */
	if (VCEncStrmStart(h264_encoder, pEncIn, &encOut)) {
		/* Handle the error situation here*/
		goto error;
	}
	/* Save the stream header */
	SaveStream((u8 *)pEncIn->pOutBuf[0], encOut.streamSize, tb.outP);
	streamSize += encOut.streamSize;
	DiagPrintf(">>Save the stream header > streamSize:%d \r\n", streamSize);
	/* Step 5: Encode the video frames */
	while (HANTRO_TRUE) {
		if (tb.picture_encoded_cnt >= tb.lastPic) {
			break;
		}
		/* Setup encoder input, we assume one linear buffer for the input image in */
		/* planar YCbCr 4:2:0 format */
		pEncIn->busLuma = tb.pictureMem.busAddress;
		tb.lum = (u8 *)tb.pictureMem.virtualAddress;
		pEncIn->busChromaU = pEncIn->busLuma + tb.lumaSize;
		tb.cb = tb.lum + tb.lumaSize;
		pEncIn->busChromaV = pEncIn->busChromaU + tb.chromaSize / 2;
		tb.cr = tb.cb + tb.chromaSize / 2;
		src_img_size =
			tb.width * tb.height * 12 / 8; //bit_pre_pixel of yuv420 is 12
		/* Read YUV to input buffer */
		//ReadYuv420PlanarFrame(&tb, src_img_size, tb.width, tb.height);
		DiagPrintf("=== Encoding %i %s ...\n\n", tb.picture_encoded_cnt, tb.inputFile);
		ret = VCEncStrmEncode(h264_encoder, pEncIn, &encOut, NULL, NULL);
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
				SaveStream((u8 *)pEncIn->pOutBuf[0], encOut.streamSize, tb.outP);
				streamSize += encOut.streamSize;
				tb.picture_encoded_cnt++;
				DiagPrintf("VCEncStrmEncode OK, frame ready\r\n");
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
	ret = VCEncFlush(h264_encoder, pEncIn, &encOut, NULL, NULL);
	/* Step 6: End stream */
	ret = VCEncStrmEnd(h264_encoder, pEncIn, &encOut);
	if (ret == VCENC_OK) {
		SaveStream((u8 *)pEncIn->pOutBuf[0], encOut.streamSize, tb.outP);
		streamSize += encOut.streamSize;
	} else {
		/* Handle the error situation here*/
		goto error;
	}

	DiagPrintf("Total of %d frames encoded, %lu bytes\n\n", tb.picture_encoded_cnt,
			   (long unsigned int)streamSize);
	DiagPrintf("stream CRC32: %x \r\n", crc32(tmp_output_buf, streamSize));
error:
	/* Free all resources */
	if (tb.inP) {
		fclose(tb.inP);
		tb.inP = NULL;
	}
	if (tb.outP) {
		fclose(tb.outP);
		tb.outP = NULL;
	}
	FreeDMABuffer(&tb);
	FreeVariableMem(&tb);
	// EWLRelease(ewl_inst);
end:
	/* Last Step: Release the encoder instance */
	ret = VCEncRelease(h264_encoder);
	return ret;
}


/*------------------------------------------------------------------------------
-- --
-- A Simple Demo of JPEG Encoding --
-- --
--------------------------------------------------------------------------------*/
#include "jpegencapi.h"

#define JPEG_TEST_YUV_FILE_NAME "D:\\FPGA_test_data\\jay_192x128_yuv420.yuv"
#define JPEG_TEST_OUTPUT_FILE_NAME "D:\\FPGA_test_data\\result.jpeg"

#define TEST_JPEG_WIDTH 192
#define TEST_JPEG_HEIGHT 128

static int Demo_JPEG_Encoding(void)
{
	int retStatus = 0;
	JpegEncRet ret;
	JpegEncInst encoder = {0};
	JpegEncCfg cfg = {0};
	JpegEncIn encIn = {0};
	JpegEncOut encOut = {0};
	/* Output buffer size: For most images 1 byte/pixel is enough
	for high quality JPEG */
	u32 outbuf_size = TEST_JPEG_WIDTH * TEST_JPEG_HEIGHT;
	u32 pict_bus_address = 0;
	u32 *outbuf_virt_address = NULL;
	u32 output_bus_address = 0;

	/* EWL init */
	EWLInitParam_t param;
	param.context = NULL;
	param.enc_dev = ENC_MODULE_PATH;
	param.mem_dev = MEMALLOC_MODULE_PATH;
	param.clientType = EWL_CLIENT_TYPE_MEM; //buffer operation
	param.slice_idx = 0;
	param.useVcmd = 1; //-1;
	const void *ewl_inst = EWLInit(&param);
	if (ewl_inst == NULL) {
		retStatus = -1;
		goto end;
	}

	/* Step 1: Initialize an encoder instance */
	cfg.qLevel = 5;
	cfg.frameType = JPEGENC_YUV420_PLANAR; //JPEGENC_YUV420_SEMIPLANAR
	cfg.markerType = JPEGENC_SINGLE_MARKER;
	cfg.unitsType = JPEGENC_DOTS_PER_INCH;
	cfg.xDensity = 72;
	cfg.yDensity = 72;
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
	cfg.xOffset = 0;
	cfg.yOffset = 0;
	cfg.rotation = JPEGENC_ROTATE_0;
	cfg.codingType = JPEGENC_WHOLE_FRAME;
	/* Restart interval 5 MCU rows (= 80 pixel rows), average quantization,
	encode whole frame at once */
	cfg.restartInterval = 0; //5;
	if ((ret = JpegEncSetPictureSize(encoder, &cfg)) != JPEGENC_OK) {
		/* Handle the error situation */
		retStatus = -1;
		goto close;
	}

	/* Step 3: Allocate linear output buffer */
	EWLLinearMem_t outbufMem;
	ret = EWLMallocLinear(ewl_inst, outbuf_size, 2, &outbufMem);
	if (ret != EWL_OK) {
		outbufMem.virtualAddress = NULL;
		retStatus = -1;
		goto close;
	}
	outbuf_virt_address = outbufMem.busAddress;
	output_bus_address = (u32)outbuf_virt_address;
	encIn.pOutBuf[0] = outbuf_virt_address;
	encIn.busOutBuf[0] = output_bus_address;
	encIn.outBufSize[0] = outbuf_size; /* bytes */
	DiagPrintf("encIn.pOutBuf[0] %p\n", encIn.pOutBuf[0]);
	/* Enable all frame headers */
	encIn.frameHeader = 1;

	/* Step 4: Get the picture and setup input */
	/* Allocate linear picture memory and read the picture */
	FILE *inP = fopen(JPEG_TEST_YUV_FILE_NAME, "rb");
	if (inP == NULL) {
		DiagPrintf("fail to open input file \r\n");
		retStatus = -1;
		goto close;
	}
	FILE *outP = fopen(JPEG_TEST_OUTPUT_FILE_NAME, "wb");
	if (outP == NULL) {
		DiagPrintf("fail to open output file \r\n");
		retStatus = -1;
		goto close;
	}
	EWLLinearMem_t pictureMem;
	ret = EWLMallocLinear(ewl_inst, (cfg.inputWidth * cfg.inputHeight * 3 / 2), 2, &pictureMem);
	if (ret != EWL_OK) {
		pictureMem.virtualAddress = NULL;
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

			SaveStream(encIn.pOutBuf[0], encOut.jfifSize, outP);
			// dump_addr(encIn.pOutBuf[0], encOut.jfifSize);
			frame_size += encOut.jfifSize;
			if (ret == JPEGENC_FRAME_READY) {
				DiagPrintf("JpegEncEncode OK, frame ready\r\n");
			}
			break;
		}
		default:
			/* All the others are error codes */
			DiagPrintf("error: JpegEncEncode fail\r\n");
			retStatus = -1;
			goto close;
		}
	} while (ret == JPEGENC_RESTART_INTERVAL);

	DiagPrintf("Frame encoded, %lu bytes\n\n", frame_size);
	DiagPrintf("stream CRC32: %x \r\n", crc32(tmp_output_buf, frame_size));

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


#define TEST_BENCH  0

#if TEST_BENCH
static int hevc_test_bench(char *arg)
{
	DiagPrintf("enc test func\n\r");
	extern int main_HEVC_H264_Test(char *arg) ;
	int ret = main_HEVC_H264_Test(arg);
	if (ret != 0) {
		return FAIL;
	}
	return SUCCESS;
}
static int jpeg_test_bench(char *arg)
{
	DiagPrintf("jpeg test func\n\r");
	extern int main_JPEG_Test(char *arg);
	main_JPEG_Test(arg);
	return SUCCESS;
}
#endif

extern void enc_rtk_hw_init(void);

static void enc_run_verification(void)
{

	DiagPrintf("ENC enc_rtk_hw_init start...\n\r");
	void enc_rtk_hw_init(void);
	DiagPrintf("ENC enc_rtk_hw_init done...\n\r");

	DiagPrintf("ENC verification start...\n\r");

	// DiagPrintf("set video sram to NS...\n\r");
	// set video sram to NS
	// RXI_CORE_BASE->RBARA1 = 0x6f000000;             //VIDEO SRAM
	// RXI_CORE_BASE->RLARA1 = 0x6f02ffff;
	// RXI_CORE_BASE->IDAU_CTRL |= RXI_CORE_IDAU_CTRL_ENABLE(2);


#if TEST_BENCH

	DiagPrintf("TEST BENCH start...\n\r");
#if CONFIG_BUILD_ENC_HEVC_TB
	/* HEVC */
	TEST(hevc_test_bench("./hevc_testenc_vce -i=D:\\FPGA_test_data\\jay_64x64_yuv420.yuv -o=D:\\FPGA_test_data\\stream.hevc --codecFormat=0 -f=30 -j=30 -R=30 -B=256000 -w=64 -h=64 -l=0 --gopSize=1 -b=0 --useVcmd=1")
		 == SUCCESS, "ENC HEVC TEST");
	/* H264 */
	TEST(hevc_test_bench("./h264_testenc_vce -i=D:\\FPGA_test_data\\jay_64x64_yuv420.yuv -o=D:\\FPGA_test_data\\stream.h264 --codecFormat=1 -f=30 -j=30 -R=30 -B=256000 -w=64 -h=64 -l=0 --gopSize=1 -b=0 --useVcmd=1")
		 == SUCCESS, "ENC H264 TEST");
#else
	/* JPEG */
	TEST(jpeg_test_bench("./jpeg_testenc_vce -i=D:\\FPGA_test_data\\jay_192x128_yuv420.yuv -o=D:\\FPGA_test_data\\stream.jpeg -w=192 -h=128 -g=0 -b=0 -q=8 --useVcmd=1 --qpHdr=-1")
		 == SUCCESS, "ENC JPEG TEST");
#endif

#else

	DiagPrintf("SIMPLE DEMO start...\n\r");

	/* init VCMD driver */
	extern int encoder_rtos_driver_init(void);
	encoder_rtos_driver_init();

	/* HEVC Demo */
	TEST(Demo_HEVC_Encoding() == OK, "Demo HEVC Encoding TEST");

	/* H264 Demo */
	//TEST(Demo_H264_Encoding() == OK, "Demo H264 Encoding TEST");

	/* JPEG Demo */
	//TEST(Demo_JPEG_Encoding() == OK, "Demo JPEG Encoding TEST");

#endif
}

#if 1
#include "user_freertos.h"
#include "io_tools.h"
#include "memalloc_freertos.h"
#include "vcx_driver_freertos.h"
#include "vcx_vcmd_priv_freertos.h"
extern rtos_vpu_dev g_vpu_dev[VPU_FD_NUM];

static void enc_reg_dump(void)
{
	DiagPrintf("\r\n\r\n");
	DiagPrintf("==================================================\r\n");
	DiagPrintf("VCMD / VCE Reg Dump:\r\n");

	uint32_t *vcmd_swreg_base = 0x60108000;
	uint32_t *vce_swreg_base = 0x60109000;

	int vcmd_swreg_num = 64;
	for (int i = 0; i < vcmd_swreg_num; i++) {
		DiagPrintf("vcmd_swreg[%d]: 0x%x\r\n", i, vcmd_swreg_base[i]);
	}
	DiagPrintf("\r\n\r\n");

	int vce_swreg_num = 500;
	for (int i = 0; i < vce_swreg_num; i++) {
		DiagPrintf("vce_swreg[%d]: 0x%x\r\n", i, vce_swreg_base[i]);
	}
	DiagPrintf("\r\n\r\n");

	DiagPrintf("==================================================\r\n");
	vcmd_mgr_t *vcmd_mgr = g_vpu_dev[ENC_FD].ctx;
	extern void dump_addr(u8 * buf, int size);
	DiagPrintf("VCMD BUF\r\n");
	dump_addr(vcmd_mgr->mem_vcmd.va, vcmd_mgr->mem_vcmd.size);
	DiagPrintf("\r\n");

	DiagPrintf("STATUS BUF\r\n");
	dump_addr(vcmd_mgr->mem_status.va, vcmd_mgr->mem_status.size);
	DiagPrintf("\r\n");

	DiagPrintf("==================================================\r\n");


}
#endif

static int run_flag = 0;
static void enc_verify_task(void *dummy)
{
	DiagPrintf("== enc_verify_task ==\n\r");

	enc_run_verification();

	DiagPrintf("enc_verify_task END\n\r");
	run_flag = 0;
	vTaskDelete(NULL);
}

// #if TEST_BENCH
// #define VERIFY_TASK_STACK_SIZE  (40 * 1024)
// #else
// #define VERIFY_TASK_STACK_SIZE  (10 * 1024)
// #endif

#define VERIFY_TASK_STACK_SIZE  (40 * 1024)

u32 EncCmdTest2(IN u16 argc, IN u8 *argv[])
{
	DiagPrintf("Encoder Cmd Test \n\r");

	/*
	 * Note:
	 * Encoder HW can only access "video sram" and "DDR" !
	 */

	/* security setting */
	*((uint32_t *)0x6010c540) = 0x1;
	*((uint32_t *)0x6010c500) = 0x6f000000;
	*((uint32_t *)0x6010c504) = 0x6f02ffff;

	// set video sram to NS
	// RXI_CORE_BASE->RBARA0 = 0x6f000000;             //VIDEO SRAM
	// RXI_CORE_BASE->RLARA0 = 0x6f0211ff;
	// DiagPrintf("RXI_CORE_BASE->RLARA0 %x \n\r", RXI_CORE_BASE->RLARA0);

	// RXI_CORE_BASE->IDAU_CTRL |= RXI_CORE_IDAU_CTRL_ENABLE(1);



	DiagPrintf("argc %d \n\r", argc);
	for (int i = 0; i < argc; i++) {
		DiagPrintf("argv[%d] = %s\n\r", i, argv[i]);
	}

	if (_strcmp((const char *)argv[0], "run") == 0) {
		run_flag = 1;
		if (xTaskCreate(enc_verify_task, "enc_verify", VERIFY_TASK_STACK_SIZE, NULL, 2, NULL) != pdPASS) {
			DiagPrintf("Fail to create encoder test task, memory not enough\r\n");
			return 0;
		}
		while (run_flag) {
			vTaskDelay(1000);
		}
		DiagPrintf("======================================\n\r");
		if (fail_cnt) {
			DiagPrintf("FAIL with %d PASS, %d FAIL\n\r", pass_cnt, fail_cnt);
		} else {
			DiagPrintf("TOTAL PASS!\n\r");
		}
		DiagPrintf("======================================\n\r");

	} else if (_strcmp((const char *)argv[0], "go") == 0) {

		DiagPrintf("ENC enc_rtk_hw_init start...\n\r");
		void enc_rtk_hw_init(void);
		DiagPrintf("ENC enc_rtk_hw_init done...\n\r");

		/* init VCMD driver */
		extern int encoder_rtos_driver_init(void);
		encoder_rtos_driver_init();

		/* HEVC Demo */
		DiagPrintf("Demo_HEVC_Encoding start...\n\r");
		Demo_HEVC_Encoding();
		DiagPrintf("Demo_HEVC_Encoding done...\n\r");
	}

	return 0;
}

// CMD_TABLE_DATA_SECTION
// const COMMAND_TABLE video_encoder2_test_cmd[] = {
    // /* command string is upper case only, and user input argv[0] in console will convert to upper case */
    // {
    // (const u8 *)"ENC2",  7, EncCmdTest2, (const u8 *)"\tENC2 \n"
    // "\t\t\t\t Encoder2 test command\n"
    // },
// };