#include "stdio.h"
#include "instance.h"

#include "user_data.h"

uint32_t enc_crc32(const unsigned char *message, uint32_t len, uint32_t preCRCValue)
{
	int i, j;
	uint32_t byte, crc, mask;

	i = 0;
	crc = 0xFFFFFFFF;
	if (preCRCValue != 0) {
		crc = ~preCRCValue;
	}
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

const void *InitEwlInst_simple(struct test_bench *tb)
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

//RTK-TBC
extern u32 output_data_addr;

int AllocateDMABuffer(struct test_bench *tb)
{
	u32 lumaSize = 0, chromaSize = 0;
	u32 pictureSize = 0;
	u32 outbufSize = 0;
	int ret = EWL_OK;
	/* picture buffer size YUV 420 */
	lumaSize = STRIDE(tb->srcwidth, tb->input_alignment);
	chromaSize = STRIDE(tb->srcwidth / 2, tb->input_alignment);
	tb->lumaSize = lumaSize * tb->srcheight;
	tb->chromaSize = chromaSize * tb->srcheight / 2 * 2;
	pictureSize = tb->lumaSize + tb->chromaSize;
	// printf("tb->lumaSize:%d\r\n", tb->lumaSize);
	// printf("tb->chromaSize:%d\r\n", tb->chromaSize);
	ret = EWLMallocLinear(tb->ewlInst, pictureSize, tb->input_alignment,
						  &tb->pictureMem);
	if (ret != EWL_OK) {
		tb->pictureMem.virtualAddress = NULL;
		return 1;
	}
	printf(">> tb->pictureMem.virtualAddress:%p\r\n", tb->pictureMem.virtualAddress);
	printf(">> tb->pictureMem.busAddress:%p\r\n", tb->pictureMem.busAddress);
	/* Output buffer size */
	outbufSize = 4 * pictureSize; //0.5 * pictureSize; //4 * pictureSize;
	outbufSize = CLIP3(OUTBUF_SIZE_MIN, OUTBUF_SIZE_MAX, outbufSize);
	ret = EWLMallocLinear(tb->ewlInst, outbufSize, tb->input_alignment,
						  &tb->outbufMem);
	if (ret != EWL_OK) {
		tb->outbufMem.virtualAddress = NULL;
		return 1;
	}
	printf(">> tb->outbufMem.virtualAddress:%p\r\n", tb->outbufMem.virtualAddress);
	printf(">> tb->outbufMem.busAddress:%p\r\n", tb->outbufMem.busAddress);
	return ret;
}

void FreeDMABuffer(struct test_bench *tb)
{
	const void *ewl_inst = tb->ewlInst;
	EWLFreeLinear(ewl_inst, &tb->pictureMem);
	EWLFreeLinear(ewl_inst, &tb->outbufMem);
	return;
}

i32 allocVariableMem(struct test_bench *tb)
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

void FreeVariableMem(struct test_bench *tb)
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
			printf("Read input picture error!nn");
		}
		return NOK;
	}
	return OK;
}

i32 ReadYuv420PlanarFrame(struct test_bench *tb, u32 src_img_size,
						  u32 src_width, u32 src_height)
{
	printf("Read YUV420 from file...\r\n");
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
	//alignment = input_alignment;
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
	printf("Read YUV420 from file done.\r\n");
	return 0;
}

i32 ReadYuv420SemiPlanarFrame(struct test_bench *tb, u32 src_img_size,
							  u32 src_width, u32 src_height)
{
	printf("Read YUV420SP from file...\r\n");
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
	seek = ((u32)tb->picture_encoded_cnt) * ((u32)src_img_size);
	lum = tb->lum;
	cb = tb->cb;
	cr = tb->cr;
	//alignment = tb->input_alignment;
	alignment = tb->input_alignment == 0 ? 1 : tb->input_alignment;
	//alignment = input_alignment;
	luma_stride = STRIDE(src_width, alignment);
	chroma_stride = STRIDE(src_width, alignment);
	/* Lum */
	for (i = 0; i < src_height; i++) {
		if (file_read(tb->inP, lum, seek, src_width)) {
			return NOK;
		}
		seek += src_width;
		lum += luma_stride;
	}

	/* Cb */
	for (i = 0; i < (src_height / 2); i++) {
		if (file_read(tb->inP, cb, seek, src_width)) {
			return NOK;
		}
		seek += src_width;
		cb += chroma_stride;
	}

	printf("Read YUV420SP from file done.\r\n");
	return 0;
}

u32 GetOsdmapReadHeight_simple(enc_user_data *cml) {
  u32 fillSize = cml->codecFormat == 1 ? 16 : 8;
  u32 filledHeight = ((cml->height  + fillSize - 1) / fillSize * fillSize);
  u32 filledWidth = ((cml->width  + fillSize - 1) / fillSize * fillSize);
  u32 read_height = (((cml->rotation == 1 || cml->rotation == 2) ? filledWidth : filledHeight) + 7) / 8;

  return read_height;
}

void SetupOverlayBuffer_simple(struct test_bench *tb, enc_user_data *cml,
							   VCEncIn *pEncIn)
{
	int i, j;
	u8 *lum, *cb, *cr;
	u32 src_width, size_luma, size_chroma;
	u64 seek;
	u32 overlay_size = 0;
	u32 dec400TotalTableSize = 0;
	for (i = 0; i < MAX_OVERLAY_NUM; i++) {

#ifdef HEIF_SUPPORT

		if ((tb->heif_enable) &&
			(tb->heif_inst->heif_config->grid_mode >= HEIF_GRID_INPUT_PIC)) {
			pEncIn->overlayEnable[i] = tb->heif_inst->heif_config->OSD_config.overlayArea[i].enable;
		} else {
			pEncIn->overlayEnable[i] = (cml->overlayEnables >> i) & 1;
		}
#else
		pEncIn->overlayEnable[i] = (cml->overlayEnables >> i) & 1;
#endif
		if ((cml->overlayEnables >> i) & 1) {
			u32 read_height = cml->olHeight[i];
			int num;
			u32 y_stride = cml->olYStride[i];
			u32 uv_stride = cml->olUVStride[i];
			pEncIn->overlayInputYAddr[i] = tb->overlayInputMem[i].busAddress;
			lum = (u8 *)tb->overlayInputMem[i].virtualAddress;
			overlay_size = 0;
#ifdef HEIF_SUPPORT
			static int grid_num[MAX_OVERLAY_NUM] = {0};
			if ((tb->heif_enable == false)
				|| (tb->heif_inst->heif_config->grid_mode < HEIF_GRID_INPUT_PIC)) {
				grid_num[i] = next_picture(tb, tb->encIn.picture_cnt % tb->olFrameNume[i]);
			} else if (tb->heif_inst->input_numbers == 0) { //Only support one picture for gird mode 2 & 3
				grid_num[i] = next_picture(tb, tb->encIn.picture_cnt % tb->olFrameNume[i]);
			}
			num = grid_num[i];
#else
			num = 0; //next_picture(tb, tb->encIn.picture_cnt % tb->olFrameNume[i]);
#endif
			switch (cml->olFormat[i]) {
			case 0:  //ARGB
				src_width = cml->olWidth[i] * 4;
				seek = num * (cml->olHeight[i] * cml->olWidth[i] * 4);
				if (cml->olSuperTile[i]) {
					src_width = cml->olYStride[i];
					read_height = (cml->olHeight[i] + 63) / 64;
					seek = num * src_width * read_height;
				}
				if (tb->osdDec400TableMem[i]) {
					seek = num * (cml->olHeight[i] * cml->olYStride[i]);
					if (file_read(tb->overlayFile[i], lum, seek, cml->olHeight[i] * cml->olYStride[i])) {
						printf("Error: fail to read overlay\n");
						pEncIn->overlayEnable[i] = 0;
						break;
					}
				} else {
					for (j = 0; j < read_height; j++) {
						if (file_read(tb->overlayFile[i], lum, seek, src_width)) {
							printf("Error: fail to read overlay\n");
							pEncIn->overlayEnable[i] = 0;
							break;
						} else {
							seek += src_width;
							lum += y_stride;
						}
					}
				}
				overlay_size = y_stride * read_height;
				break;
			case 1:  //NV12
				size_luma = cml->olYStride[i] * cml->olHeight[i];
				seek = num * ((cml->olWidth[i] * cml->olHeight[i]) +
							  (cml->olWidth[i]) * cml->olHeight[i] / 2);
				src_width = cml->olWidth[i];
				pEncIn->overlayInputUAddr[i] =
					pEncIn->overlayInputYAddr[i] + size_luma;  //Cb and Cr
				cb = lum + size_luma;
				if (tb->osdDec400TableMem[i]) {
					seek = num * (cml->olHeight[i] * cml->olYStride[i] + cml->olYStride[i] * cml->olHeight[i] / 2);
					if (file_read(tb->overlayFile[i], lum, seek, cml->olHeight[i] * cml->olYStride[i] +
								  cml->olYStride[i] * cml->olHeight[i] / 2)) {
						printf("Error: fail to read overlay\n");
						pEncIn->overlayEnable[i] = 0;
						break;
					}
				} else {
					//Luma
					for (j = 0; j < read_height; j++) {
						if (file_read(tb->overlayFile[i], lum, seek, src_width)) {
							printf("Error: fail to read overlay\n");
							pEncIn->overlayEnable[i] = 0;
							break;
						} else {
							seek += src_width;
							lum += y_stride;
						}
					}
					//Cb and Cr
					for (j = 0; j < (read_height / 2); j++) {
						if (file_read(tb->overlayFile[i], cb, seek, src_width)) {
							//This situation means corrupted input, so do not rolling back
							pEncIn->overlayEnable[i] = 0;
							break;
						}
						seek += src_width;
						cb += uv_stride;
					}
				}
				size_chroma = uv_stride * (read_height / 2);
				overlay_size = size_luma + size_chroma;
				break;
			case 2:  //Bitmap
				/* Foreground pixels in bitmap need to be 2x2 aligned, otherwise there will be avoidless chroma
				   artifact cased by YUV420 coding. This chroma artifact is affected by alpha value, background
				   chroma, foreground bitmap chroma. */
				src_width = cml->olWidth[i] / 8;
				seek = num * cml->olHeight[i] * cml->olWidth[i] / 8;
				for (j = 0; j < read_height; j++) {
					if (file_read(tb->overlayFile[i], lum, seek, src_width)) {
						printf("Error: fail to read overlay\n");
						pEncIn->overlayEnable[i] = 0;
						break;
					} else {
						seek += src_width;
						lum += y_stride;
					}
				}
				overlay_size = y_stride * read_height;
				break;
			default:
				break;
			}
			EWLSyncMemData(tb->overlayInputMem[i], 0, overlay_size, HOST_TO_DEVICE);
		}
	}
	if (cml->osdMapEnable) {
    	pEncIn->osdMapEnable = cml->osdMapEnable;
    	u32 read_height = GetOsdmapReadHeight_simple(cml);
    	int num = 0; //next_picture(tb, tb->encIn.picture_cnt % tb->osdMapFrameNum);
    	u32 osdMapSize = 0;

    	pEncIn->osdMapInputAddr = tb->osdMapMem.busAddress;
    	lum = (u8 *)tb->osdMapMem.virtualAddress;
    	src_width = cml->osdMapStride;//unit: byte; 4bits per 8x8 block.
    	seek = num * src_width * read_height;
    	for (j = 0; j < read_height; j++) {
      	if (file_read(tb->osdMapFile, lum, seek, src_width)) {
        	printf("Error: fail to read osdMap\n");
        	pEncIn->osdMapEnable = 0;
        	break;
      	} else {
        	seek += src_width;
        	lum += src_width;
      	}
   	}
    osdMapSize = src_width * read_height;
    EWLSyncMemData(tb->osdMapMem, 0, osdMapSize, HOST_TO_DEVICE);
  }

}

//#define SAVE_OUTPUT_TO_MEM  0

int SaveStream(u8 *buf, u32 size, FILE *fp, u32 *crc32value)
{
	int ret = 0;
//#if SAVE_OUTPUT_TO_MEM
	if (output_data_addr != NULL) {
		static int cur = 0;
		memcpy((output_data_addr + cur), buf, size);
		printf("Save Stream to mem addr:0x%x with cur:%d, size:%d\r\n", output_data_addr, cur, size);
		cur += size;
	}
//#else
	if (fp != NULL) {
		ret = fwrite(buf, 1, size, fp);
	}
//#endif
	*crc32value = enc_crc32(buf, size, *crc32value);
	return ret;
}

i32 read_dec400Data_Simple(struct test_bench *tb, u32 inputFormat, u32 src_width,
						   u32 src_height)
{
	i32 num;
	u64 seek;
	u8 *lum;
	u8 *cb;
	u8 *cr;
	u32 lumaSize;
	u32 chrSize;
	u32 src_img_size;

	//num = next_picture(tb, tb->encIn.picture_cnt) + 4;
	num = 4;

	/*dec400 compress table*/
	lumaSize = tb->dec400LumaTableSize;
	chrSize = tb->dec400ChrTableSize;
	src_img_size = tb->dec400FrameTableSize;
	if (tb->dec400TSHeaderEnable) {
		tb->dec400Table = tb->inP;
		src_img_size = tb->dec400FrameTableSize + tb->lumaSize + tb->chromaSize;
	}

	//if (num > tb->lastPic || tb->dec400Table == NULL) return NOK;
	seek = ((u64)num) * ((u64)src_img_size);

	lum = (u8 *)tb->Dec400CmpTableMem.virtualAddress;
	cb = lum + lumaSize;
	cr = cb + chrSize / 2;

	switch (inputFormat) {
	case VCENC_YUV420_PLANAR:
	case VCENC_YUV420_PLANAR_10BIT_I010:
		if (file_read(tb->dec400Table, lum, seek, lumaSize)) {
			return NOK;
		}
		seek += lumaSize;
		if (file_read(tb->dec400Table, cb, seek, chrSize / 2)) {
			return NOK;
		}
		seek += chrSize / 2;
		if (file_read(tb->dec400Table, cr, seek, chrSize / 2)) {
			return NOK;
		}
		break;
	case VCENC_YUV420_SEMIPLANAR:
	case VCENC_YUV420_SEMIPLANAR_VU:
	case VCENC_YUV420_PLANAR_10BIT_P010:
	case VCENC_YVU420_PLANAR_10BIT_P010:
	case VCENC_YUV420_SEMIPLANAR_8BIT_TILE_4_4:
	case VCENC_YUV420_SEMIPLANAR_VU_8BIT_TILE_4_4:
	case VCENC_YUV420_PLANAR_10BIT_P010_TILE_4_4:
	case VCENC_YUV420_8BIT_TILE_8_8:
	case VCENC_YUV420_10BIT_TILE_8_8:
		if (file_read(tb->dec400Table, lum, seek, lumaSize)) {
			return NOK;
		}
		seek += lumaSize;
		if (file_read(tb->dec400Table, cb, seek, chrSize)) {
			return NOK;
		}
		break;
	case VCENC_RGB888:
	case VCENC_BGR888:
	case VCENC_RGB101010:
	case VCENC_BGR101010:
	case VCENC_RGBX8888:
	case VCENC_BGRX8888:
	case VCENC_RGBX1010102:
	case VCENC_BGRX1010102:
	case VCENC_YUV422_INTERLEAVED_YUYV:
	case VCENC_Y8b_TILE_8_8:
	case VCENC_Y10bWH_TILE_8_8:
	case VCENC_RGB444:
	case VCENC_BGR444:
	case VCENC_RGB555:
	case VCENC_BGR555:
	case VCENC_RGB565:
	case VCENC_BGR565:
		if (file_read(tb->dec400Table, lum, seek, lumaSize)) {
			return NOK;
		}
		break;
	default:
		printf("DEC400 not support this format\n");
		return NOK;
	}

	/*dec400 compress data*/
	lumaSize = tb->lumaSize;
	chrSize = tb->chromaSize;
	src_img_size = lumaSize + chrSize;
	if (tb->dec400TSHeaderEnable) {
		seek += tb->dec400FrameTableSize;
	} else {
		seek = ((u64)num) * ((u64)src_img_size);
	}

	lum = tb->lum;
	cb = tb->cb;
	cr = tb->cr;

	switch (inputFormat) {
	case VCENC_YUV420_PLANAR:
	case VCENC_YUV420_PLANAR_10BIT_I010:
		if (file_read(tb->inP, lum, seek, lumaSize)) {
			return NOK;
		}
		seek += lumaSize;
		if (file_read(tb->inP, cb, seek, chrSize / 2)) {
			return NOK;
		}
		seek += chrSize / 2;
		if (file_read(tb->inP, cr, seek, chrSize / 2)) {
			return NOK;
		}
		break;
	case VCENC_YUV420_SEMIPLANAR:
	case VCENC_YUV420_SEMIPLANAR_VU:
	case VCENC_YUV420_PLANAR_10BIT_P010:
	case VCENC_YVU420_PLANAR_10BIT_P010:
	case VCENC_YUV420_SEMIPLANAR_8BIT_TILE_4_4:
	case VCENC_YUV420_SEMIPLANAR_VU_8BIT_TILE_4_4:
	case VCENC_YUV420_PLANAR_10BIT_P010_TILE_4_4:
	case VCENC_YUV420_8BIT_TILE_8_8:
	case VCENC_YUV420_10BIT_TILE_8_8:
		if (file_read(tb->inP, lum, seek, lumaSize)) {
			return NOK;
		}
		seek += lumaSize;
		if (file_read(tb->inP, cb, seek, chrSize)) {
			return NOK;
		}
		break;
	case VCENC_RGB888:
	case VCENC_BGR888:
	case VCENC_RGB101010:
	case VCENC_BGR101010:
	case VCENC_RGBX8888:
	case VCENC_BGRX8888:
	case VCENC_RGBX1010102:
	case VCENC_BGRX1010102:
	case VCENC_YUV422_INTERLEAVED_YUYV:
	case VCENC_Y8b_TILE_8_8:
	case VCENC_Y10bWH_TILE_8_8:
	case VCENC_RGB444:
	case VCENC_BGR444:
	case VCENC_RGB555:
	case VCENC_BGR555:
	case VCENC_RGB565:
	case VCENC_BGR565:
		if (file_read(tb->inP, lum, seek, lumaSize)) {
			return NOK;
		}
		break;
	default:
		printf("DEC400 not support this format\n");
		return NOK;
	}

	return OK;
}