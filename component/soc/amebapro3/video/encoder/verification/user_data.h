#ifndef __USER_DATA_H__
#define __USER_DATA_H__

#include "hevcencapi.h"

#define OUTBUF_SIZE_MAX 12 *1024 *1024
#define OUTBUF_SIZE_MIN 1024 * 11

//mapping to commandLine_s  ~ Line 50 ~ 756
typedef struct enc_user_data {
	char *input;
	char *output;
	char *dec400CompTableinput;

	i32 firstPic;
	i32 lastPic;

	i32 width;
	i32 height;
	i32 lumWidthSrc;
	i32 lumHeightSrc;

	i32 inputFormat;

	//Line 88
	VCEncVideoCodecFormat codecFormat; /* Video Codec Format: HEVC/H264/AV1 */

	//Line 276
	i32 intraPicRate; /* IDR interval */

	//Line 337
	i32 profile; /*main profile or main still picture profile*/
	i32 tier;    /*main tier or high tier*/
	i32 level;   /*main profile level*/
	
	//Line 353
	i32 rotation;

	//Line 361
	i32 scanType;

	//Line 515
	/*stride*/
	u32 exp_of_input_alignment;

	//Line 595
	/*Overlay*/
	u32 overlayEnables;
	char *olInput[MAX_OVERLAY_NUM];
	u32 olFormat[MAX_OVERLAY_NUM];
	u32 olAlpha[MAX_OVERLAY_NUM];
	u32 olWidth[MAX_OVERLAY_NUM];
	u32 olCropWidth[MAX_OVERLAY_NUM];
	u32 olScaleWidth[MAX_OVERLAY_NUM];
	u32 olHeight[MAX_OVERLAY_NUM];
	u32 olCropHeight[MAX_OVERLAY_NUM];
	u32 olScaleHeight[MAX_OVERLAY_NUM];
	u32 olXoffset[MAX_OVERLAY_NUM];
	u32 olCropXoffset[MAX_OVERLAY_NUM];
	u32 olYoffset[MAX_OVERLAY_NUM];
	u32 olCropYoffset[MAX_OVERLAY_NUM];
	u32 olYStride[MAX_OVERLAY_NUM];
	u32 olUVStride[MAX_OVERLAY_NUM];
	u32 olBitmapY[MAX_OVERLAY_NUM];
	u32 olBitmapU[MAX_OVERLAY_NUM];
	u32 olBitmapV[MAX_OVERLAY_NUM];
	u32 olSuperTile[MAX_OVERLAY_NUM];

	/* Mosaic parameters */
	u32 mosaicEnables;
	u32 mosXoffset[MAX_MOSAIC_NUM];
	u32 mosYoffset[MAX_MOSAIC_NUM];
	u32 mosWidth[MAX_MOSAIC_NUM];
	u32 mosHeight[MAX_MOSAIC_NUM];
	u32 mosSizeIndex;

	/* OSD_MAP */
	u32 osdMapEnable;
	char *osdMapInput;
	u32 osdMapStride;
	u32 osdMapAlpha[MAX_OSDMAP_COLOR_NUM];
	u32 osdMapY[MAX_OSDMAP_COLOR_NUM];
	u32 osdMapU[MAX_OSDMAP_COLOR_NUM];
	u32 osdMapV[MAX_OSDMAP_COLOR_NUM];

	//Line 713
	i32 ufbcEnable;
	i32 ufbcYuvTrans;
	u32 ufbcBlockType;
	u32 ufbcBlockSplit;
	u32 ufbcCompAlign;
	u32 secure_mode;

	//Line 750
	u32 dec400TileSize;
	u32 dec400TileMode;
	u32 dec400DataAlignment;
	u32 dec400RGBFormat;
	u32 dec400RGBAX;
	u32 dec400TSHeaderEnable;

	u32 input_data_addr;
	u32 busLuma;
	u32 busChromaU;
	u32 busChromaV;
	u32 output_data_addr;
	u32 dec_table_data_addr;
	u32 dec400LumTableBase;
	u32 dec400CbTableBase;
	u32 dec400CrTableBase;
	u32 output_data_size;
	u32 input_data_width;
	u32 input_data_height;
	u32 output_data_width;
	u32 output_data_height;
	u32 streamSize;

	u32 inputLineBufEn;
	u32 inputLineBufLoopBackEn;
	u32 inputLineBufDepth;
	u32 inputLineBufHwModeEn;
	u32 amountPerLoopBack;

	u32 wrap_num_to_enc;
	u32 low_latency_set;
	u32 timeout_disable;

	u32 decEnable;
	u32 crc32value;
} enc_user_data;

//Line 807 ~ 989
struct test_bench {
	char *dec400TableFile;

	FILE *dec400Table;

	i32 width;
	i32 height;

	//Line 826
	i32 firstPic;
	i32 lastPic;
	i32 picture_encoded_cnt; /*count of frame finish encoding*/

	i32 idr_interval;
	
	//Line 835
	u8 *lum;
	u8 *cb;
	u8 *cr;

	//Line 843
	u32 input_alignment;

	u32 lumaSize;
	u32 chromaSize;

	//Line 855
	u32 dec400LumaTableSize;
	u32 dec400ChrTableSize;
	u32 dec400FrameTableSize;

	//Line 885
	EWLLinearMem_t pictureMem; //RTK-TBC
	//EWLLinearMem_t *pictureDSMem;
	EWLLinearMem_t outbufMem;	//RTK-TBC
	EWLLinearMem_t overlayInputMem[MAX_OVERLAY_NUM];
	EWLLinearMem_t *osdDec400TableMem[MAX_OVERLAY_NUM];
	EWLLinearMem_t Dec400CmpTableMem; //RTK-TBC
	EWLLinearMem_t osdMapMem;

	EWLLinearMem_t Dec400CmpTableMemFactory[64];
	//EWLLinearMem_t overlayInputMemFactory[MAX_OVERLAY_NUM];

	//Line 919
	i32 *tile_width;
	i32 *tile_height;

	//Line 935
	u32 gopSize;

	VCEncIn encIn;

	//Line 955
	FILE *overlayFile[MAX_OVERLAY_NUM];
	FILE *osdDec400TableFile[MAX_OVERLAY_NUM];
	u32 olFrameNume[MAX_OVERLAY_NUM];
	u8 olEnable[MAX_OVERLAY_NUM];
	u32 osdDec400TableSize[MAX_OVERLAY_NUM][3];
	FILE *osdMapFile;
	u32 osdMapFrameNum;

	//Line 978
	const void *ewlInst;

	u32 ufbcEnable;
	u32 ufbcBlockType;

	u32 dec400Enable;
	u32 dec400TSHeaderEnable;

	char *inputFile;
	char *outputFile;
	FILE *inP;
	FILE *outP;
	i32 srcwidth;
	i32 srcheight;

	//VCEncPictureType inputType; //(VCEncPictureType)cml->inputFormat
	/** \brief The color format of the input picture. */
	/** 0 - YUV420P8b_Raster_A16N. */
	// VCENC_YUV420_PLANAR = ENC_PIXFMT_YUV420_PLANAR,
	/** 1 - YUV420SP8b_Raster_A16N. */
	// VCENC_YUV420_SEMIPLANAR = ENC_PIXFMT_YUV420_SEMIPLANAR,
	i32 inputFormat;
	u32 streamSize;
	VCEncInst hevc_encoder;
	VCEncVideoCodecFormat codecFormat;
	VCEncLevel level;
	VCEncProfile profile;
	u32 log2_ctb_size;
	//inputLineBufferCfg inputCtbLineBuf;
	//i32 inputFormat;

	u32 scanType;
	u32 secure_mode;
	ptr_t busLuma;
	ptr_t busChromaU;
	ptr_t busChromaV;
	ptr_t dec400LumTableBase;
	ptr_t dec400CbTableBase;
	ptr_t dec400CrTableBase;
};

#endif /* __USER_DATA_H__ */

