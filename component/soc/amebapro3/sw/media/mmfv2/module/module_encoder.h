#ifndef _MODULE_ENCODER_H
#define _MODULE_ENCODER_H

#include "mmf2_module.h"
#include "video_common_type.h"
#include "video_osd_scene.h"
#include "os_wrapper.h"
#include "hevcencapi.h"
#include "jpegencapi.h"

#define ENC_SEI_USER_DATA_RING_NUM      8

#define CMD_ENC_SET_PARAMS              MM_MODULE_CMD(0x00)
#define CMD_ENC_GET_PARAMS              MM_MODULE_CMD(0x01)
#define CMD_ENC_SET_HEIGHT              MM_MODULE_CMD(0x02)
#define CMD_ENC_SET_WIDTH               MM_MODULE_CMD(0x03)
#define CMD_ENC_SET_FORCE_DROP_FRAME    MM_MODULE_CMD(0x04)
#define CMD_ENC_SET_GOPLEN              MM_MODULE_CMD(0x06)
#define CMD_ENC_SET_FPS                 MM_MODULE_CMD(0x07)
#define CMD_ENC_SET_RCPARAM             MM_MODULE_CMD(0x08)
#define CMD_ENC_GET_RCPARAM             MM_MODULE_CMD(0x09)
#define CMD_ENC_RCMODE                  MM_MODULE_CMD(0x0a)
#define CMD_ENC_FORCE_IFRAME            MM_MODULE_CMD(0x0b)
#define CMD_ENC_INIT_MEM_POOL           MM_MODULE_CMD(0x0c)
#define CMD_ENC_SET_DEC_LUM_TABLE       MM_MODULE_CMD(0x0d)
#define CMD_ENC_SET_DEC_CHR_TABLE       MM_MODULE_CMD(0x0e)
#define CMD_ENC_SET_PREPROCESS_CB       MM_MODULE_CMD(0x0f)
#define CMD_ENC_SET_ISP_REMAP           MM_MODULE_CMD(0x10)  // set isp remap param
#define CMD_ENC_SET_ENC_REMAP           MM_MODULE_CMD(0x11)  // set enc remap param
#define CMD_ENC_SET_OSD                 MM_MODULE_CMD(0x12)  // set enc OSD
#define CMD_ENC_GET_RC_EXT              MM_MODULE_CMD(0x13)
#define CMD_ENC_SET_RC_EXT              MM_MODULE_CMD(0x14)
#define CMD_ENC_GET_CODING_CTRL         MM_MODULE_CMD(0x15)
#define CMD_ENC_SET_CODING_CTRL         MM_MODULE_CMD(0x16)
#define CMD_ENC_SET_MOSAIC              MM_MODULE_CMD(0x17)  // set enc mosaic
#define CMD_ENC_SET_OSD_SCENE           MM_MODULE_CMD(0x18)
#define CMD_ENC_GET_OSD_SCENE_STATE     MM_MODULE_CMD(0x19)
#define CMD_ENC_APPLY                   MM_MODULE_CMD(0x20)
#define CMD_ENC_PAUSE                   MM_MODULE_CMD(0x21)
#define CMD_ENC_STOP                    MM_MODULE_CMD(0x22)

#define CMD_ENC_JPEG_SNAPSHOT_CB        MM_MODULE_CMD(0x30)
#define CMD_ENC_JPEG_DO_SNAPSHOT        MM_MODULE_CMD(0x31)  // do a snapshot at next [n] frame

typedef struct mem_info {
	uint32_t	mem_total_size;
	uint32_t	mem_block_size;
	uint32_t	mem_frame_size;
} mem_info_t;

typedef struct crop_scale_s {
	int cropScaleEnable;
	uint32_t x_offset;
	uint32_t y_offset;
	uint32_t width;
	uint32_t height;
} crop_scale_t;

typedef struct enc_parameter_s {
	uint32_t stream_id;
	uint32_t codec_fmt;
	uint32_t width;
	uint32_t height;
	uint32_t fps;
	uint32_t gop;
	uint32_t bps;		// for CBR/ABR
	uint32_t maxBps;	// for VBR
	uint32_t minBps;	// for VBR
	uint32_t rcMode;
	uint32_t minQp;		// for CBR/VBR/ABR
	uint32_t maxQp;		// for CBR/VBR/ABR
	int intraQpDelta;
	int profile;
	uint32_t level;
	uint32_t tier;
	uint8_t profileSet;
	uint8_t levelSet;
	uint8_t tierSet;
	enc_rc_ext_t initRc;
	enc_coding_t initCoding;
	uint32_t rotation;
	int qlevel;         // for JPEG

	int decompEnable;      // enable encoder decompressor
	void *ispDecompLumTable;  // isp remap compressor Y table
	void *ispDecompChrTable;  // isp remap compressor UV table

	int lowLatencyEnable;   // enable encoder low latency mode

	osd_overlay_t osd_overlay[MAX_OVERLAY_NUM];
	mosaic_overlay_t mosaic[MAX_MOSAIC_NUM];

	crop_scale_t cs;    // crop and scale enable and params

	uint32_t force_drop_frame;//The frame rate is follow by the h264 setting
	mem_info_t mem_info;
} enc_parameter_t;

/* A triggered JPEG encode reports NULL and length zero on failure. */
typedef void (*enc_snapshot_t)(void *, uint32_t);
typedef void (*enc_preprocess_t)(void *y_buf, void *uv_buf, uint32_t w, uint32_t h, void *out_yuv_buf);

typedef struct enc_context {

	void *ewlInst;

	/* HEVC/H264 */
	VCEncInst hevc_encoder;
	VCEncConfig hevc_cfg;
	VCEncGopPicConfig hevc_gopPicCfg[MAX_GOP_PIC_CONFIG_NUM];
	VCEncPreProcessingCfg hevc_preProcCfg;
	VCEncIn hevc_encIn;
	VCEncOut hevc_encOut;
	uint8_t *hevc_header_buf;
	uint32_t hevc_header_buf_size;
	ExternalSEI sei_user_data[ENC_SEI_USER_DATA_RING_NUM];
	uint32_t hevc_streamSize;
	uint32_t hevc_frame_size;
	uint32_t hevc_picture_encoded_cnt; /*count of frame finish encoding*/
	uint32_t hevc_input_alignment;
	uint32_t hevc_lumaSize;
	uint32_t hevc_chromaSize;
	uint32_t hevc_gopSize;
	long int hevc_tile_width;
	long int hevc_tile_height;
	rtos_queue_t hevc_osd_update_q;
	rtos_queue_t hevc_osd_scene_q;
	volatile int osd_scene_owned;
	volatile int osd_scene_fault;
	rtos_queue_t hevc_mosaic_update_q;
	/* JPEG */
	JpegEncInst jpeg_encoder;
	JpegEncCfg jpeg_cfg;
	JpegEncIn jpeg_encIn;
	JpegEncOut jpeg_encOut;
	EWLLinearMem_t jpeg_outbufMem;
	uint32_t jpeg_frame_size;

	/* decompressor */
	uint32_t dec400Enable;
	uint32_t dec400TSHeaderEnable;
	uint32_t dec400LumaTableSize;
	uint32_t dec400ChrTableSize;
	uint32_t dec400FrameTableSize;
	isp_remap_param_t *remap_param;
	enc_remap_param_t *enc_remap_param;

	enc_parameter_t enc_parm;

	int isp_timestamp;
	uint32_t isp_phy_y_addr;
	uint32_t isp_phy_uv_addr;
	uint32_t enc_y_addr;
	uint32_t enc_uv_addr;
	uint32_t jpeg_y_addr;
	uint32_t jpeg_uv_addr;
	uint32_t dest_addr;
	uint32_t dest_buf_len;
	uint32_t dest_actual_data_len;

	enc_snapshot_t snapshot_cb;
	int snapshot_trigger;
	int snapshot_idx;
	int snapshot_counter;
	enc_preprocess_t preprocess_cb;
	uint8_t *proc_img_buf;
	uint8_t *proc_img_buf_64aligned;

	int pause;
	int stop;
	int force_idr;
} enc_context_t;

typedef struct enc_ctx_s {
	void *parent;

	enc_context_t *enc_cont;

	void *mem_pool;
} enc_ctx_t;

extern mm_module_t enc_module;


#endif
