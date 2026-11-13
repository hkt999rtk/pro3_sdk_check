/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the License); you may
* not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an AS IS BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************************/
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "avcodec.h"
#include "ameba_soc.h"
#include "mmf2_module.h"
#include "mmf2_dbg.h"
#include "mmf2_mediatime_pro3.h"
#include "module_encoder.h"
#include "memory_encoder.h"
#include "enc_sys_api.h"
#include "video_common_type.h"
#ifdef CONFIG_VMODULE_TS
#include "ts_verify.h"
#endif
#ifdef CONFIG_VMODULE_ADDREMAP
#include "addremap_hal.h"
#endif

#define ENC_ADDR_REMAP_ENA  1

#define ENC_ALIGN(value, alignment) \
  (((value) + (alignment) - 1) & (~((alignment) - 1)))

typedef struct enc_global_s {
	uint8_t *enc_virtual_addr;
	int module_cnt;
	int driver_opened;
} enc_global_t;

static enc_global_t g_enc = {
	.enc_virtual_addr = (uint8_t *)0xc0000000,  //can be an invalid memory address
};

#define ENC_SEI_UUID_SIZE               16
#define ENC_SEI_USER_DATA_PAYLOAD_TYPE  5
#define ENC_SEI_HEVC_PREFIX_NAL_TYPE    39

static uint8_t g_enc_sei_uuid[ENC_SEI_UUID_SIZE] = {
	'R', 'T', 'K', 'P', '3', 'S', 'E', 'I',
	'U', 'U', 'I', 'D', '0', '0', '0', '1'
};

#if defined(CONFIG_VMODULE_ADDREMAP) && defined(CONFIG_VMODULE_TS) && ENC_ADDR_REMAP_ENA
static void enc_pre_setup(void *p);
static void enc_post_setup(void *p);
#else
__weak u32 user_timeout_disable = 0;
#endif

/* enc_encode() returns 0 or a mask of these failures */
#define ENC_ERR_ENCODE      (1 << 0)  /* live H.264/H.265 or JPEG-only encode failed, drop the frame */
#define ENC_ERR_SNAPSHOT    (1 << 1)  /* JPEG snapshot failed, already reported by snapshot_cb */

static int enc_encode(enc_context_t *);
static void enc_rc_ext_to_driver(VCEncRateCtrl *rcCfg, enc_rc_ext_t *p);
static void enc_coding_to_driver(VCEncCodingCtrl *codingCfg, enc_coding_t *p);
static void enc_setup_user_data_sei(enc_context_t *enc_ctx, VCEncIn *pEncIn);

int enc_handle(void *p, void *input, void *output)
{
	enc_ctx_t *ctx = (enc_ctx_t *)p;
	enc_context_t *enc_ctx = ctx->enc_cont;
	int ch = enc_ctx->enc_parm.stream_id;
	int ret = 0;

	if (enc_ctx->pause) {
		return 0;
	}

	mm_queue_item_t *input_item = (mm_queue_item_t *)input;
	mm_queue_item_t *output_item = (mm_queue_item_t *)output;

	enc_ctx->isp_phy_y_addr = input_item->isp.y_addr;
	enc_ctx->isp_phy_uv_addr = input_item->isp.uv_addr;
	enc_ctx->enc_y_addr = enc_ctx->isp_phy_y_addr;
	enc_ctx->enc_uv_addr = enc_ctx->isp_phy_uv_addr;
	enc_ctx->jpeg_y_addr = enc_ctx->isp_phy_y_addr;
	enc_ctx->jpeg_uv_addr = enc_ctx->isp_phy_uv_addr;

	/* preprocess the isp output image */
	if (enc_ctx->preprocess_cb && enc_ctx->proc_img_buf_64aligned) {
		enc_ctx->preprocess_cb((void *)enc_ctx->isp_phy_y_addr, (void *)enc_ctx->isp_phy_uv_addr, enc_ctx->enc_parm.width, enc_ctx->enc_parm.height,
							   enc_ctx->proc_img_buf_64aligned);
		enc_ctx->enc_y_addr = enc_ctx->jpeg_y_addr = (uint32_t)enc_ctx->proc_img_buf_64aligned;
		uint32_t uv_offset = enc_ctx->enc_parm.width * enc_ctx->enc_parm.height;
		enc_ctx->enc_uv_addr = enc_ctx->jpeg_uv_addr = enc_ctx->enc_y_addr + uv_offset;
	}

#if defined(CONFIG_VMODULE_ADDREMAP) && defined(CONFIG_VMODULE_TS) && ENC_ADDR_REMAP_ENA
	/* pre setup for encoder */
	enc_pre_setup(enc_ctx);
#endif

	ENCODER_DBG_DEBUG("[ch:%d] isp y 0x%x, uv 0x%x, size %d\n\r", ch, input_item->isp.y_addr, input_item->isp.uv_addr, input_item->size);
	ENCODER_DBG_DEBUG("[ch:%d] enc y 0x%x, uv 0x%x, dst 0x%x\n\r", ch, enc_ctx->enc_y_addr, enc_ctx->enc_uv_addr, output_item->data_addr);

	enc_ctx->dest_addr = (uint32_t)output_item->data_addr;
	enc_ctx->dest_buf_len = enc_ctx->enc_parm.mem_info.mem_frame_size;
	enc_ctx->isp_timestamp = input_item->timestamp;
	ENCODER_DBG_DEBUG("[ch:%d]dest_addr= 0x%x len = %d\r\n", ch, enc_ctx->dest_addr, enc_ctx->dest_buf_len);

	/* encode the frame */
	ret = enc_encode(enc_ctx);
	if (ret & ENC_ERR_SNAPSHOT) {
		ENCODER_DBG_WARNING("[ch:%d]snapshot fail\r\n", ch);
	}
	if (ret & ENC_ERR_ENCODE) {
		ENCODER_DBG_ERROR("error: [ch:%d]encode fail\r\n", enc_ctx->enc_parm.stream_id);
		goto cleanUp;
	}

#if defined(CONFIG_VMODULE_ADDREMAP) && defined(CONFIG_VMODULE_TS) && ENC_ADDR_REMAP_ENA
	/* post setup for encoder */
	enc_post_setup(enc_ctx);
#endif

	/* output item */
	uint32_t codec_fmt = enc_ctx->enc_parm.codec_fmt;
	if (codec_fmt & VIDEO_HEVC) {
		output_item->type = AV_CODEC_ID_H265;
	} else if (codec_fmt & VIDEO_H264) {
		output_item->type = AV_CODEC_ID_H264;
	} else if (codec_fmt == VIDEO_JPEG) {
		output_item->type = AV_CODEC_ID_JPEG;
	}
	if (codec_fmt & (VIDEO_HEVC | VIDEO_H264)) {
		enc_ctx->dest_actual_data_len = enc_ctx->hevc_frame_size;
	} else if (codec_fmt == VIDEO_JPEG) {
		enc_ctx->dest_actual_data_len = enc_ctx->jpeg_frame_size;
	}
	output_item->size = enc_ctx->dest_actual_data_len;
	output_item->timestamp = input_item->timestamp;
	output_item->hw_timestamp = input_item->hw_timestamp;

cleanUp:
	if (ret & ENC_ERR_ENCODE) {
		output_item->size = 0;
	}

	return output_item->size;
}

#if defined(CONFIG_VMODULE_ADDREMAP) && defined(CONFIG_VMODULE_TS) && ENC_ADDR_REMAP_ENA
extern void ts_convert_if2of(struct ts_info_t ts_info);
extern void ts_check_end(void);
extern void ts_clear_operation_status(void);
extern u32 set_enc_remap_ts(struct enc_remap_config *, u16, u16, u32);
extern uint8_t __ts_vsram_start__[];
extern uint8_t __ts_vsram_end__[];
#define TS_SRAM_SIZE  ((uint32_t)__ts_vsram_end__ - (uint32_t)__ts_vsram_start__)

static void enc_pre_setup(void *p)
{
	enc_context_t *enc_ctx = (enc_context_t *)p;

	if (enc_ctx->enc_parm.cs.cropScaleEnable) {
		struct ts_info_t ts_info = {0};
		struct enc_remap_config ts_remap_cfg = {0};
		int remap_en = 1;
		uint32_t tile_w = 64;
		uint32_t tile_h = 64;
		if (enc_ctx->enc_parm.codec_fmt & VIDEO_H264) {
			tile_h = 16;
		}

		uint32_t wrap_num = TS_SRAM_SIZE / (tile_w * tile_h * 3 / 2);
		ENCODER_DBG_INFO("TS_SRAM_SIZE:%lu set wrap_num=%lu\r\n", TS_SRAM_SIZE, wrap_num);

		uint8_t *ts_out_buf = __ts_vsram_start__;  //video sram

		/* TS setup */
		ts_info.in_f = TS_FORMAT_S420;
		ts_info.out_f = TS_FORMAT_S420;
		ts_info.buf = (u8 *)enc_ctx->enc_y_addr;
		ts_info.out_buf = ts_out_buf;
		ENCODER_DBG_DEBUG("ts_info.out_buf:0x%x\r\n", ts_info.out_buf);
		ENCODER_DBG_DEBUG("wrap_num=%ld\r\n", wrap_num);
		ts_info.in_ROI_left = enc_ctx->enc_parm.cs.x_offset;
		ts_info.in_ROI_top = enc_ctx->enc_parm.cs.y_offset;
		ts_info.in_w = enc_ctx->enc_parm.cs.width;
		ts_info.in_h = enc_ctx->enc_parm.cs.height;
		ENCODER_DBG_DEBUG("[TS ROI]x:%d y:%d w:%d h:%d\r\n", ts_info.in_ROI_left, ts_info.in_ROI_top, ts_info.in_w, ts_info.in_h);
		ts_info.in_frame_w = enc_ctx->enc_parm.width;
		ts_info.in_frame_h = enc_ctx->enc_parm.height;
		ts_info.out_w = enc_ctx->enc_parm.width;
		ts_info.out_h = enc_ctx->enc_parm.height;
		ts_info.out_frame_w = enc_ctx->enc_parm.width;
		ts_info.out_frame_h = enc_ctx->enc_parm.height;
		ts_info.lpf_en = 1;
		ts_info.tile_en = 1;
		ts_info.start_0 = 0;
		ts_info.scale_order = (1 << 8);
		ts_info.wrap_en = 1;
		if (wrap_num) {
			ts_info.wrap_num = wrap_num - 1;
		}
		ts_info.tile_w = tile_w;
		ts_info.tile_h = tile_h;
		if (remap_en) {
			ENCODER_DBG_INFO("TS tile enable wrap enable\r\n");
			ts_info.tile_en = 1;
			ts_info.wrap_en = 1;
		}

		/* remap setup */
		ts_remap_cfg.tile_width = tile_w;
		ts_remap_cfg.SliceHeight = tile_h;
		ts_remap_cfg.ParSliceWrapNum = wrap_num;
		ts_remap_cfg.enc_remap_component[0].dram_saddr = (uint32_t)g_enc.enc_virtual_addr;
		if (remap_en) {
			ENCODER_DBG_DEBUG("set_enc_remap_ts\r\n");
			set_enc_remap_ts(&ts_remap_cfg, ts_info.out_w, ts_info.out_h, (u32)ts_info.out_buf);
		}

		/* trigger TS */
		ENCODER_DBG_DEBUG("ts_convert_if2of start\r\n");
		ts_convert_if2of(ts_info);
		ENCODER_DBG_DEBUG("ts_convert_if2of end\r\n");

		/* set encoder source address to remapped virtual address */
		enc_ctx->enc_y_addr = (uint32_t)g_enc.enc_virtual_addr;
		uint32_t uv_offset = enc_ctx->enc_parm.width * enc_ctx->enc_parm.height;
		enc_ctx->enc_uv_addr = enc_ctx->enc_y_addr + uv_offset;
	}

	if (enc_ctx->enc_parm.decompEnable) {
		if (enc_ctx->enc_parm.lowLatencyEnable) {
			int slice_ch = 0;  //slice mode can only run on ch0
			enc_ctx->enc_parm.ispDecompLumTable = (void *)enc_ctx->remap_param->encDec400LumTableBase[slice_ch];
			enc_ctx->enc_parm.ispDecompChrTable = (void *)enc_ctx->remap_param->encDec400ChrTableBase[slice_ch];
			ENCODER_DBG_DEBUG("[ENC DEC ch%d] LumTable:0x%x ChrTable:0x%x\n", slice_ch, enc_ctx->enc_parm.ispDecompLumTable,
							  enc_ctx->enc_parm.ispDecompChrTable);
		} else {
			extern int isp_get_buf_idx(int stream_id, void *buf_addr);
			int stream_id = enc_ctx->enc_parm.stream_id;
			int buf_idx = isp_get_buf_idx(stream_id, (void *)enc_ctx->isp_phy_y_addr);
			enc_ctx->enc_parm.ispDecompLumTable = (void *)enc_ctx->remap_param->encDec400LumTableBase[buf_idx];
			enc_ctx->enc_parm.ispDecompChrTable = (void *)enc_ctx->remap_param->encDec400ChrTableBase[buf_idx];
			int ch = enc_ctx->enc_parm.stream_id;
			ENCODER_DBG_DEBUG("[ENC DEC ch%d] buf_id:%d, LumTable:0x%x ChrTable:0x%x\n", ch, buf_idx, enc_ctx->enc_parm.ispDecompLumTable,
							  enc_ctx->enc_parm.ispDecompChrTable);
		}
	}

}

static void enc_post_setup(void *p)
{
	enc_context_t *enc_ctx = (enc_context_t *)p;

	if (enc_ctx->enc_parm.cs.cropScaleEnable) {
		/* TS check end */
		ENCODER_DBG_DEBUG("ts_check_end start\r\n");
		ts_check_end();
		ENCODER_DBG_DEBUG("ts_check_end end\r\n");
		/* TS clean up */
		ts_clear_operation_status();
	}
}
#endif

static int enc_hevc_init_gop_configs(int gopSize, VCEncGopConfig *gopCfg,
									 uint8_t *gopCfgOffset)
{
	VCEncGopPicConfig *hevc_cfg = NULL;
	gopCfg->special_size = 0;
	gopCfg->ltrcnt = 0;
	/* gopCfgOffset: keep record of gopSize’s offset in pGopPicCfg */
	if (gopCfgOffset) {
		gopCfgOffset[gopSize] = gopCfg->size;
	}
	/* gop_config : #Frame1: P 1 0 0.578 0 1 -1 1 */
	hevc_cfg = &(gopCfg->pGopPicCfg[gopCfg->size++]);
	hevc_cfg->poc = 1;
	;
	hevc_cfg->QpOffset = 0;
	hevc_cfg->QpFactor = 0.760263; //sqrt(0.578)
	hevc_cfg->temporalId = 0;
	hevc_cfg->codingType = 1;
	hevc_cfg->nonReference = 0;
	hevc_cfg->numRefPics = 1;
	hevc_cfg->refPics->ref_pic = -1;
	hevc_cfg->refPics->used_by_cur = 1;
	return 0;
}

static int enc_h264_init_gop_configs(int gopSize, VCEncGopConfig *gopCfg,
									 uint8_t *gopCfgOffset)
{
	VCEncGopPicConfig *hevc_cfg = NULL;
	gopCfg->special_size = 0;
	gopCfg->ltrcnt = 0;
	/* gopCfgOffset: keep record of gopSize’s offset in pGopPicCfg */
	if (gopCfgOffset) {
		gopCfgOffset[gopSize] = gopCfg->size;
	}
	/* gop_config : #Frame1: P 1 0 0.578 0 1 -1 1 */
	hevc_cfg = &(gopCfg->pGopPicCfg[gopCfg->size++]);
	hevc_cfg->poc = 1;
	hevc_cfg->QpOffset = 0;
	hevc_cfg->QpFactor = 0.760263; //sqrt(0.578)
	hevc_cfg->temporalId = 0;
	hevc_cfg->codingType = 1;
	hevc_cfg->nonReference = 0;
	hevc_cfg->numRefPics = 1;
	hevc_cfg->refPics->ref_pic = -1;
	hevc_cfg->refPics->used_by_cur = 1;
	return 0;
}

static void enc_osd_scene_complete(video_osd_scene_t *scene, uint32_t result)
{
	*scene->completion = result;
	DCache_Clean((uint32_t)scene->completion, CACHE_LINE_SIZE);
}

static int enc_close(enc_context_t *enc_ctx)
{
	int ret = 0;

	if (enc_ctx->enc_parm.codec_fmt & (VIDEO_HEVC | VIDEO_H264)) {

		if (enc_ctx->hevc_encoder) {
			int VCEnc_ret = VCENC_OK;

			/* The MMF close path does not own an output buffer or forward drained
			 * frames/EOS downstream. Keep the drain sequence disabled until a
			 * proper stop/drain path provides and consumes its own output buffer. */
#if 0
			VCEncIn *pEncIn = &(enc_ctx->hevc_encIn);

			/* Flush encoder internal buffered frames
			 * In Multicore enabled or 2-pass enabled, encoder internal cached frames
			 * must be flushed
			 */
			VCEnc_ret = VCEncFlush(enc_ctx->hevc_encoder, pEncIn, &enc_ctx->hevc_encOut, NULL, NULL);
			if (VCEnc_ret != VCENC_OK) {
				ENCODER_DBG_ERROR("error: VCEncFlush fail ret=%d\r\n", VCEnc_ret);
				ret = -1;
			}

			/* End stream — attempt even if flush failed */
			VCEnc_ret = VCEncStrmEnd(enc_ctx->hevc_encoder, pEncIn, &enc_ctx->hevc_encOut);
			if (VCEnc_ret != VCENC_OK) {
				ENCODER_DBG_ERROR("error: VCEncStrmEnd fail ret=%d\r\n", VCEnc_ret);
				ret = -1;
			} else {
				enc_ctx->hevc_streamSize += enc_ctx->hevc_encOut.streamSize;
				ENCODER_DBG_INFO("Total of %ld frames encoded, %lu bytes\n\n",
								 enc_ctx->hevc_picture_encoded_cnt, (uint32_t)enc_ctx->hevc_streamSize);
			}
#endif

			/* Release encoder instance — always attempt to avoid resource leak */
			VCEnc_ret = VCEncRelease(enc_ctx->hevc_encoder);
			if (VCEnc_ret != VCENC_OK) {
				ENCODER_DBG_ERROR("error: VCEncRelease fail ret=%d\r\n", VCEnc_ret);
				ret = -1;
			}
			enc_ctx->hevc_encoder = NULL;
		}

		if (enc_ctx->hevc_osd_update_q) {
			rtos_queue_delete(enc_ctx->hevc_osd_update_q);
			enc_ctx->hevc_osd_update_q = NULL;
		}
		if (enc_ctx->hevc_osd_scene_q) {
			video_osd_scene_t scene;
			while (rtos_queue_receive(enc_ctx->hevc_osd_scene_q, &scene, 0) == SUCCESS) {
				enc_osd_scene_complete(&scene, VIDEO_OSD_SCENE_CLOSED);
			}
			rtos_queue_delete(enc_ctx->hevc_osd_scene_q);
			enc_ctx->hevc_osd_scene_q = NULL;
		}
		enc_ctx->osd_scene_owned = 0;
		enc_ctx->osd_scene_fault = 0;
		if (enc_ctx->hevc_mosaic_update_q) {
			rtos_queue_delete(enc_ctx->hevc_mosaic_update_q);
			enc_ctx->hevc_mosaic_update_q = NULL;
		}
	}

	if (enc_ctx->enc_parm.codec_fmt & VIDEO_JPEG) {

		if (enc_ctx->jpeg_encoder) {
			int JpegEnc_ret = JPEGENC_OK;

			JpegEnc_ret = JpegEncRelease(enc_ctx->jpeg_encoder);
			if (JpegEnc_ret != JPEGENC_OK) {
				ENCODER_DBG_ERROR("error: JpegEncRelease fail with ret:%d\r\n", JpegEnc_ret);
				ret = -1;
			}
			enc_ctx->jpeg_encoder = NULL;
		}

		/* release output buffer — always attempt */
		if (enc_ctx->ewlInst && enc_ctx->jpeg_outbufMem.size) {
			EWLFreeLinear(enc_ctx->ewlInst, &enc_ctx->jpeg_outbufMem);
		}
	}

	/* deinit EWL — always attempt regardless of above errors */
	if (enc_ctx->ewlInst) {
		enc_deinit_ewl_inst(enc_ctx->ewlInst);
		enc_ctx->ewlInst = NULL;
	}

	if (enc_ctx->hevc_header_buf) {
		free(enc_ctx->hevc_header_buf);
		enc_ctx->hevc_header_buf = NULL;
		enc_ctx->hevc_header_buf_size = 0;
	}

	if (ret != 0) {
		ENCODER_DBG_ERROR("error: enc_close completed with errors\r\n");
	}
	return ret;
}

static int enc_osd_config(VCEncOverlayArea *overlay_area, osd_overlay_t *osdOverlay)
{
	overlay_area->xoffset = osdOverlay->osdOverlayXoffset;
	overlay_area->cropXoffset = 0;
	overlay_area->yoffset = osdOverlay->osdOverlayYoffset;
	overlay_area->cropYoffset = 0;
	overlay_area->width = osdOverlay->osdOverlayWidth;
	overlay_area->cropWidth = overlay_area->width;
	overlay_area->height = osdOverlay->osdOverlayHeight;
	overlay_area->cropHeight = overlay_area->height;
	overlay_area->format = osdOverlay->osdOverlayFormat;
	overlay_area->alpha = osdOverlay->osdOverlayAlpha;
	overlay_area->enable = osdOverlay->osdOverlayEnable;
	switch (overlay_area->format) {
	case 0:  //ARGB
		overlay_area->Ystride = osdOverlay->osdOverlayWidth * 4;
		break;
	case 1:  //NV12
		overlay_area->Ystride = osdOverlay->osdOverlayWidth;
		break;
	case 2:  //Bitmap
		overlay_area->Ystride = osdOverlay->osdOverlayWidth / 8;
		break;
	default:
		overlay_area->Ystride = osdOverlay->osdOverlayWidth;
	}
	overlay_area->UVstride = overlay_area->Ystride;
	overlay_area->bitmapY = osdOverlay->osdOverlayBitmapY;
	overlay_area->bitmapU = osdOverlay->osdOverlayBitmapU;
	overlay_area->bitmapV = osdOverlay->osdOverlayBitmapV;
	overlay_area->superTile = 0;
	overlay_area->scaleWidth = overlay_area->cropWidth;
	overlay_area->scaleHeight = overlay_area->cropHeight;
	return 0;
}

/* Called only between synchronous encodes. One transaction replaces the whole
 * OSD configuration; the old picture is no longer being read at completion. */
static int enc_apply_osd_scene(enc_context_t *ctx)
{
	video_osd_scene_t scene;
	VCEncPreProcessingCfg candidate;
	if (!ctx->hevc_osd_scene_q || rtos_queue_receive(ctx->hevc_osd_scene_q, &scene, 0) != SUCCESS) {
		return ctx->osd_scene_fault ? -1 : 0;
	}
	if (VCEncGetPreProcessing(ctx->hevc_encoder, &candidate) != VCENC_OK) {
		goto rejected;
	}
	for (int i = 0; i < MAX_OVERLAY_NUM; i++) {
		candidate.overlayArea[i].enable = 0;
	}
	for (uint32_t i = 0; i < scene.count; i++) {
		enc_osd_config(&candidate.overlayArea[scene.overlays[i].osdOverlayIdx], &scene.overlays[i]);
	}
	if (VCEncSetPreProcessing(ctx->hevc_encoder, &candidate) != VCENC_OK) {
		goto rejected;
	}
	ctx->hevc_preProcCfg = candidate;
	int had_overlay = 0;
	for (int i = 0; i < MAX_OVERLAY_NUM; i++) {
		had_overlay |= ctx->enc_parm.osd_overlay[i].osdOverlayEnable;
	}
	if (!had_overlay && scene.count) {
		ctx->force_idr = 1;
	}
	memset(ctx->enc_parm.osd_overlay, 0, sizeof(ctx->enc_parm.osd_overlay));
	for (uint32_t i = 0; i < scene.count; i++) {
		ctx->enc_parm.osd_overlay[scene.overlays[i].osdOverlayIdx] = scene.overlays[i];
	}
	ctx->osd_scene_owned = scene.release ? 0 : 1;
	ctx->osd_scene_fault = 0;
	scene.completion[1] = ctx->isp_timestamp;
	enc_osd_scene_complete(&scene, VIDEO_OSD_SCENE_APPLIED);
	return 0;
rejected:
	/* Never fall back to emitting an unmasked frame after a privacy failure.
	 * A later valid transaction recovers the stream. */
	ctx->osd_scene_fault = 1;
	enc_osd_scene_complete(&scene, VIDEO_OSD_SCENE_FAULT);
	return -1;
}

int enc_open(enc_context_t *enc_ctx)
{
	int ret = 0;

	/* init encoder wrapper layer */
	enc_ctx->ewlInst = enc_init_ewl_inst();
	if (enc_ctx->ewlInst == NULL) {
		ENCODER_DBG_ERROR("error: enc_init_ewl_inst return NULL\r\n");
		ret = -1;
		goto error;
	}

	if (enc_ctx->enc_parm.codec_fmt & (VIDEO_HEVC | VIDEO_H264)) {

		int VCEnc_ret = VCENC_OK;
		VCEncIn *pEncIn = &(enc_ctx->hevc_encIn);
		memset(&enc_ctx->hevc_preProcCfg, 0, sizeof(VCEncPreProcessingCfg));
		/* Step 1: Initialize an h264 encoder instance */
		memset(&enc_ctx->hevc_cfg, 0, sizeof(VCEncConfig));
		/* CTB RC buffers must be allocated by VCEncInit, before setting RC. */
		uint32_t rc_mode = (enc_ctx->enc_parm.initRc.flags & VPAP_ENC_RC_EXT_RCMODE) ?
						   enc_ctx->enc_parm.initRc.rcMode : enc_ctx->enc_parm.rcMode;
		enc_ctx->hevc_cfg.ctbRcMode = (rc_mode == ENC_CQP) ? 0 : 3;
		if (rc_mode == ENC_CRF || ((enc_ctx->enc_parm.initRc.flags & VPAP_ENC_RC_EXT_CRF) && enc_ctx->enc_parm.initRc.crf >= 0)) {
			/* CRF cannot use the precise bitrate-control bit. */
			enc_ctx->hevc_cfg.ctbRcMode = (rc_mode == ENC_CQP) ? 0 : 1;
		}
		if (enc_ctx->enc_parm.initRc.flags & VPAP_ENC_RC_EXT_CTB_RC) {
			enc_ctx->hevc_cfg.ctbRcMode = enc_ctx->enc_parm.initRc.ctbRc;
		}
		/* Image Resolution */
		enc_ctx->hevc_cfg.width = enc_ctx->enc_parm.width;
		enc_ctx->hevc_cfg.height = enc_ctx->enc_parm.height;
		/* 30 fps Frame Rate */
		enc_ctx->hevc_cfg.frameRateDenom = 1;
		enc_ctx->hevc_cfg.frameRateNum = enc_ctx->enc_parm.fps;
		/* Stream Type */
		enc_ctx->hevc_cfg.streamType = VCENC_BYTE_STREAM;
		enc_ctx->hevc_cfg.tier = VCENC_HEVC_MAIN_TIER;
		/* Codec Format */
		if (enc_ctx->enc_parm.codec_fmt & VIDEO_H264) {
			enc_ctx->hevc_cfg.codecFormat = VCENC_VIDEO_CODEC_H264;
			enc_ctx->hevc_cfg.level = VCENC_H264_LEVEL_5_1;
			enc_ctx->hevc_cfg.profile = VCENC_H264_HIGH_PROFILE;
		} else if (enc_ctx->enc_parm.codec_fmt & VIDEO_HEVC) {
			enc_ctx->hevc_cfg.codecFormat = VCENC_VIDEO_CODEC_HEVC;
			enc_ctx->hevc_cfg.level = VCENC_HEVC_LEVEL_6;
			enc_ctx->hevc_cfg.profile = VCENC_HEVC_MAIN_PROFILE;
		}
		if (enc_ctx->enc_parm.profileSet) {
			enc_ctx->hevc_cfg.profile = (VCEncProfile)enc_ctx->enc_parm.profile;
		}
		if (enc_ctx->enc_parm.levelSet) {
			enc_ctx->hevc_cfg.level = (VCEncLevel)enc_ctx->enc_parm.level;
		}
		if (enc_ctx->enc_parm.tierSet) {
			enc_ctx->hevc_cfg.tier = (VCEncTier)enc_ctx->enc_parm.tier;
		}
		/* decompressor setting */
		if (enc_ctx->enc_parm.decompEnable) {
			enc_ctx->hevc_cfg.ufbcParam.mode = UFBC_MODE_DEC400;
			enc_ctx->hevc_cfg.ufbcParam.param.dec400.compAlign = 1; //0: aligmnent 32 byte, 1: aligmnent 64 byte

			enc_ctx->dec400Enable = 2;
			enc_ctx->dec400TSHeaderEnable = 0;
			enc_ctx->dec400LumaTableSize = 0;
			enc_ctx->dec400ChrTableSize = 0;
			enc_ctx->dec400FrameTableSize = 0;
			uint32_t scanType = 0;
			extern void EncGetDec400TsBufferSize(u32 type, u32 width, u32 height, u32 alignment, u32 * luma_Size, u32 * chroma_Size,
												 u32 * picture_Size, u32 scanType, u32 dec400Enable, u32 dec400TSHeaderEnable);
			EncGetDec400TsBufferSize(
				ENC_PIXFMT_YUV420_SEMIPLANAR, enc_ctx->enc_parm.width, enc_ctx->enc_parm.height, enc_ctx->hevc_input_alignment,
				&enc_ctx->dec400LumaTableSize, &enc_ctx->dec400ChrTableSize,
				&enc_ctx->dec400FrameTableSize, scanType,
				enc_ctx->dec400Enable, enc_ctx->dec400TSHeaderEnable);
			ENCODER_DBG_DEBUG("[ENC]LumaTableSize:%d ChrTableSize:%d FrameTableSize:%d\r\n", enc_ctx->dec400LumaTableSize, enc_ctx->dec400ChrTableSize,
							  enc_ctx->dec400FrameTableSize);
		}
		enc_ctx->hevc_cfg.bitDepthLuma = 8;
		enc_ctx->hevc_cfg.bitDepthChroma = 8;
		enc_ctx->hevc_cfg.maxTLayers = 1;
		enc_ctx->hevc_cfg.refFrameAmount = 1; //4;
		enc_ctx->hevc_cfg.enableSsim = 0; //1;
		enc_ctx->hevc_cfg.enablePsnr = 0; //1;
		enc_ctx->hevc_cfg.parallelCoreNum = 1;
		enc_ctx->hevc_cfg.dumpCuInfo = 0;
		enc_ctx->hevc_cfg.log2MaxPicOrderCntLsb = 16;
		enc_ctx->hevc_cfg.log2MaxFrameNum = 12;
		enc_ctx->hevc_cfg.interlacedFrame = 0;
		enc_ctx->hevc_cfg.inLoopDSRatio = 1;
		enc_ctx->hevc_cfg.cuInfoVersion = -1;
		enc_ctx->hevc_cfg.extSramLumHeightBwd = 16;
		enc_ctx->hevc_cfg.extSramChrHeightBwd = 8;
		enc_ctx->hevc_cfg.extSramLumHeightFwd = 16;
		enc_ctx->hevc_cfg.extSramChrHeightFwd = 8;
		enc_ctx->hevc_cfg.gopSize = 1;
		enc_ctx->hevc_cfg.codedChromaIdc = VCENC_CHROMA_IDC_420;
		enc_ctx->hevc_cfg.refRingBufEnable = 0;
		enc_ctx->hevc_cfg.writeReconToDDR = 1;
		enc_ctx->hevc_cfg.av1InterFiltSwitch = 1;
		enc_ctx->hevc_cfg.tune = VCENC_TUNE_PSNR;
		enc_ctx->hevc_cfg.tile_width = &enc_ctx->hevc_tile_width;
		enc_ctx->hevc_cfg.tile_height = &enc_ctx->hevc_tile_height;
		enc_ctx->hevc_cfg.tiles_enabled_flag = 0;
		enc_ctx->hevc_cfg.irqTypeMask = 0x01f0; /* irq type mask, 0:normal interrupt, 1:abnormal interrupt */
		enc_ctx->hevc_cfg.irqTypeCutreeMask = 0x01f0;
		enc_ctx->hevc_cfg.num_tile_columns = 1;
		enc_ctx->hevc_cfg.num_tile_rows = 1;
		enc_ctx->hevc_cfg.loop_filter_across_tiles_enabled_flag = 0;
		enc_ctx->hevc_cfg.useVcmd = 1;
		enc_ctx->hevc_cfg.compressor = 3;
		uint32_t log2_ctb_size = 4;
		if (enc_ctx->enc_parm.codec_fmt & VIDEO_HEVC) {
			log2_ctb_size = 6;
		}
		int width = enc_ctx->hevc_cfg.width;
		int height = enc_ctx->hevc_cfg.height;
		uint32_t ctbPerRow = ((width + (1 << log2_ctb_size) - 1) >> log2_ctb_size);
		uint32_t ctbPerColumn = ((height + (1 << log2_ctb_size) - 1) >> log2_ctb_size);
		int tileId;
		for (tileId = 0; tileId < enc_ctx->hevc_cfg.num_tile_columns; tileId++) {
			enc_ctx->hevc_cfg.tile_width[tileId] =
				(tileId + 1) * ctbPerRow / enc_ctx->hevc_cfg.num_tile_columns - (tileId * ctbPerRow) / enc_ctx->hevc_cfg.num_tile_columns;
		}
		for (tileId = 0; tileId < enc_ctx->hevc_cfg.num_tile_rows; tileId++) {
			enc_ctx->hevc_cfg.tile_height[tileId] =
				(tileId + 1) * ctbPerColumn / enc_ctx->hevc_cfg.num_tile_rows -
				(tileId * ctbPerColumn) / enc_ctx->hevc_cfg.num_tile_rows;
		}
		if ((VCEnc_ret = VCEncInit(&enc_ctx->hevc_cfg, &enc_ctx->hevc_encoder, NULL)) != VCENC_OK) {
			ENCODER_DBG_ERROR("error: VCEncInit fail with ret:%d\r\n", VCEnc_ret);
			ret = -1;
			goto error;
		}
		enc_ctx->hevc_input_alignment = 64;
		const EWLHwConfig_t *hw_cfg = EncGetAsicConfig(enc_ctx->hevc_cfg.codecFormat, enc_ctx->ewlInst);
		if (!hw_cfg) {
			ENCODER_DBG_ERROR("error: cannot get encoder hardware configuration\r\n");
			ret = -1;
			goto error;
		}
		enc_ctx->hevc_picture_encoded_cnt = 0;
		/* Step 2: Extra encoder configuration. See the next example code for how to change the
		* default encoder parameters
		*/
		/* GOP configuration */
		memset(enc_ctx->hevc_gopPicCfg, 0, sizeof(enc_ctx->hevc_gopPicCfg));
		enc_ctx->hevc_encIn.gopConfig.pGopPicCfg = enc_ctx->hevc_gopPicCfg;
		switch (enc_ctx->hevc_cfg.codecFormat) {
		case VCENC_VIDEO_CODEC_H264:
			ret = enc_h264_init_gop_configs(enc_ctx->hevc_gopSize, &(enc_ctx->hevc_encIn.gopConfig),
											enc_ctx->hevc_encIn.gopConfig.gopCfgOffset);
			break;
		case VCENC_VIDEO_CODEC_HEVC:
			ret = enc_hevc_init_gop_configs(enc_ctx->hevc_gopSize, &(enc_ctx->hevc_encIn.gopConfig),
											enc_ctx->hevc_encIn.gopConfig.gopCfgOffset);
			break;
		default:
			ENCODER_DBG_ERROR("error: not supported codec format\r\n");
			ret = -1;
			break;
		}
		if (ret < 0) {
			ENCODER_DBG_ERROR("error: init gop configs fail\r\n");
			goto error;
		}
		enc_ctx->hevc_encIn.gopConfig.outputRateNumer = enc_ctx->enc_parm.fps; /* Output frame rate numerator */
		enc_ctx->hevc_encIn.gopConfig.outputRateDenom = 1; /* Output frame rate denominator */
		enc_ctx->hevc_encIn.gopConfig.inputRateNumer = enc_ctx->enc_parm.fps; /* Input frame rate numerator */
		enc_ctx->hevc_encIn.gopConfig.inputRateDenom = 1; /* Input frame rate denominator */
		enc_ctx->hevc_encIn.gopConfig.idr_interval = enc_ctx->enc_parm.gop;
		/* Write VUI timing_info in every SPS so players (e.g. VLC) read the correct framerate */
		enc_ctx->hevc_encIn.vui_timing_info_enable = 1;
		/* Step 3: Allocate linear memory resources (input and output buffers) */
		/* picture buffer size YUV 420 */
		uint32_t lumaSize, chromaSize;
		lumaSize = STRIDE(enc_ctx->enc_parm.width, enc_ctx->hevc_input_alignment);
		chromaSize = STRIDE(enc_ctx->enc_parm.width / 2, enc_ctx->hevc_input_alignment);
		enc_ctx->hevc_lumaSize = lumaSize * enc_ctx->enc_parm.height;
		enc_ctx->hevc_chromaSize = chromaSize * enc_ctx->enc_parm.height / 2 * 2;
		/* Step 4: Start the stream */
		/* Encoder setup: rate control */
		VCEncRateCtrl rcCfg = {0};
		if ((VCEnc_ret = VCEncGetRateCtrl(enc_ctx->hevc_encoder, &rcCfg)) != VCENC_OK) {
			ENCODER_DBG_ERROR("error: VCEncGetRateCtrl fail with ret:%d\r\n", VCEnc_ret);
			ret = -1;
			goto error;
		}
		ENCODER_DBG_DEBUG(
			"Get rate control: qp %2d qpRange I[%2u, %2u] PB[%2u, %2u] %8u bps  "
			"pic %u skip %u  hrd %u  cpbSize %u cpbMaxRate %u bitrateWindow %u "
			"intraQpDelta %2d\n",
			rcCfg.qpHdr, rcCfg.qpMinI, rcCfg.qpMaxI, rcCfg.qpMinPB, rcCfg.qpMaxPB,
			rcCfg.bitPerSecond, rcCfg.pictureRc, rcCfg.pictureSkip, rcCfg.hrd,
			rcCfg.hrdCpbSize, rcCfg.cpbMaxRate, rcCfg.bitrateWindow,
			rcCfg.intraQpDelta);
		rcCfg.bitPerSecond = enc_ctx->enc_parm.bps;
		rcCfg.qpMinI = rcCfg.qpMinPB = enc_ctx->enc_parm.minQp;
		rcCfg.qpMaxI = rcCfg.qpMaxPB = enc_ctx->enc_parm.maxQp;
		rcCfg.intraQpDelta = -5;
		rcCfg.rcMode = enc_ctx->enc_parm.rcMode;
		/* Selecting an RC mode does not enable picture RC in the driver. */
		rcCfg.pictureRc = (rcCfg.rcMode != ENC_CQP);
		rcCfg.ctbRc = enc_ctx->hevc_cfg.ctbRcMode;
		/* Match the reference API setup: disable the optional intra QP reset.
		 * The driver default (-1) would pass a negative bit budget to InitialQp. */
		rcCfg.changePos = 90;
		/* Match the vendor test bench RC setup; explicit initRc flags win below. */
		rcCfg.blockRCSize = (hw_cfg->ctbRcVersion == 2) ? 2 : 0;
		if (enc_ctx->enc_parm.gop) {
			rcCfg.bitrateWindow = MIN(enc_ctx->enc_parm.gop, 300);
		}
		rcCfg.u32StaticSceneIbitPercent = 80;
		if (enc_ctx->enc_parm.initRc.flags) {
			enc_rc_ext_to_driver(&rcCfg, &enc_ctx->enc_parm.initRc);
		}
		if (!(enc_ctx->enc_parm.initRc.flags & VPAP_ENC_RC_EXT_QP_HDR)) {
			/* Estimate the initial QP for RC; preserve the fixed-QP default. */
			rcCfg.qpHdr = rcCfg.pictureRc ? -1 : (int32_t)enc_ctx->enc_parm.minQp;
		}
		ENCODER_DBG_DEBUG(
			"Set rate control: qp %2d qpRange I[%2u, %2u] PB[%2u, %2u] %9u bps  "
			"pic %u ctb %u skip %u  hrd %u"
			"  cpbSize %u cpbMaxRate %u bitrateWindow %u intraQpDelta %2d "
			"fixedIntraQp %2u\n",
			rcCfg.qpHdr, rcCfg.qpMinI, rcCfg.qpMaxI, rcCfg.qpMinPB, rcCfg.qpMaxPB,
			rcCfg.bitPerSecond, rcCfg.pictureRc, rcCfg.ctbRc, rcCfg.pictureSkip, rcCfg.hrd,
			rcCfg.hrdCpbSize, rcCfg.cpbMaxRate, rcCfg.bitrateWindow,
			rcCfg.intraQpDelta, rcCfg.fixedIntraQp);
		if ((VCEnc_ret = VCEncSetRateCtrl(enc_ctx->hevc_encoder, &rcCfg)) != VCENC_OK) {
			ENCODER_DBG_ERROR("error: VCEncSetRateCtrl fail with ret:%d\r\n", VCEnc_ret);
			ret = -1;
			goto error;
		}
		/* PreP setup */
		if ((VCEnc_ret = VCEncGetPreProcessing(enc_ctx->hevc_encoder, &enc_ctx->hevc_preProcCfg)) != VCENC_OK) {
			ENCODER_DBG_ERROR("error: VCEncGetPreProcessing fail\r\n");
			ret = -1;
			goto error;
		}
		enc_ctx->hevc_preProcCfg.input_alignment = 8;
		enc_ctx->hevc_preProcCfg.inputType = VCENC_YUV420_SEMIPLANAR;
		enc_ctx->hevc_preProcCfg.rotation = 0;
		if (enc_ctx->enc_parm.decompEnable) {
			enc_ctx->hevc_preProcCfg.origWidth = ENC_ALIGN(enc_ctx->enc_parm.width, 256);  //compression UFBCBlock:256x1
		} else if (enc_ctx->enc_parm.lowLatencyEnable) {
			enc_ctx->hevc_preProcCfg.origWidth = ENC_ALIGN(enc_ctx->enc_parm.width, 64);  //match ISP uncompressed RTT stride
		} else {
			enc_ctx->hevc_preProcCfg.origWidth = enc_ctx->enc_parm.width;
		}
		ENCODER_DBG_DEBUG("Set origWidth %lu\r\n", enc_ctx->hevc_preProcCfg.origWidth);
		enc_ctx->hevc_preProcCfg.origHeight = enc_ctx->enc_parm.height;
		/* OSD overlay area */
		for (int i = 0; i < MAX_OVERLAY_NUM; i++) {
			osd_overlay_t *osdOverlay = &enc_ctx->enc_parm.osd_overlay[i];
			if (osdOverlay->osdOverlayEnable) {
				enc_osd_config(&enc_ctx->hevc_preProcCfg.overlayArea[i], osdOverlay);
			}
		}
		/* Set mosaic region parameters.
		 * mosSizeIndex is channel-level; take it from the first enabled region only.
		 * All enabled regions must carry the same mosOverlaySizeIdx (see mosaic_overlay_t). */
		int mos_size_set = 0;
		for (int i = 0; i < MAX_MOSAIC_NUM; i++) {
			mosaic_overlay_t *mos = &enc_ctx->enc_parm.mosaic[i];
			if (mos->mosOverlayEnable) {
				enc_ctx->hevc_preProcCfg.mosEnable[i] = mos->mosOverlayEnable;
				enc_ctx->hevc_preProcCfg.mosXoffset[i] = mos->mosOverlayXoffset;
				enc_ctx->hevc_preProcCfg.mosYoffset[i] = mos->mosOverlayYoffset;
				enc_ctx->hevc_preProcCfg.mosWidth[i] = mos->mosOverlayWidth;
				enc_ctx->hevc_preProcCfg.mosHeight[i] = mos->mosOverlayHeight;
				if (!mos_size_set) {
					enc_ctx->hevc_preProcCfg.mosSizeIndex = mos->mosOverlaySizeIdx;
					mos_size_set = 1;
				}
			}
		}
		if ((VCEnc_ret = VCEncSetPreProcessing(enc_ctx->hevc_encoder, &enc_ctx->hevc_preProcCfg)) != VCENC_OK) {
			ENCODER_DBG_ERROR("error: VCEncSetPreProcessing fail\r\n");
			ret = -1;
			goto error;
		}
		/* Always apply vendor coding defaults, even without ROI/low-latency extras. */
		{
			/* Coding control */
			VCEncCodingCtrl codingCfg = {0};
			if ((VCEnc_ret = VCEncGetCodingCtrl(enc_ctx->hevc_encoder, &codingCfg)) != VCENC_OK) {
				ENCODER_DBG_ERROR("error: VCEncGetCodingCtrl fail\r\n");
				ret = -1;
				goto error;
			}
			/* CTB RC v2 subjective defaults from vendor get_option.c/SetCodingCtrl.
			 * GetCodingCtrl returns zero direction/thresholds on a new instance. */
			if (hw_cfg->ctbRcVersion == 2) {
				static const uint32_t ctb_rc_threshold[16] = {0, 0, 0, 0, 3, 3, 5, 5, 6, 6, 6, 11, 11, 13, 17, 17};
				codingCfg.ctbRcDirection = 8;
				memcpy(codingCfg.ctbRcThresholdI, ctb_rc_threshold, sizeof(ctb_rc_threshold));
				memcpy(codingCfg.ctbRcThresholdP, ctb_rc_threshold, sizeof(ctb_rc_threshold));
				memcpy(codingCfg.ctbRcThresholdB, ctb_rc_threshold, sizeof(ctb_rc_threshold));
				codingCfg.ctbRcSkinQPDelta = 2;
				codingCfg.ctbRcSkinMinQPDelta = 3;
				codingCfg.ctbRcSkinLumMax = 255;
				codingCfg.ctbRcTrailDeltaQp = -4;
			}
			codingCfg.visualBitRateTolerance = 50;
			if (enc_ctx->enc_parm.initCoding.flags) {
				enc_coding_to_driver(&codingCfg, &enc_ctx->enc_parm.initCoding);
			}
			if (enc_ctx->enc_parm.lowLatencyEnable) {
				codingCfg.inputLineBufEn = 1;
				codingCfg.inputLineBufLoopBackEn = 1;
				codingCfg.inputLineBufDepth = 1;
				codingCfg.inputLineBufHwModeEn = 1;
				codingCfg.amountPerLoopBack = enc_ctx->enc_remap_param->slice_wrap_num;
				codingCfg.lowlatGatingDisable = 0;
				codingCfg.lowlatGatingType = 0;
				codingCfg.lowlatGatingCyc = IS_H264(enc_ctx->hevc_cfg.codecFormat) ? 7 : 31;
				ENCODER_DBG_DEBUG("set enc low latency slice_wrap_num:%lu\r\n", codingCfg.amountPerLoopBack);
			}
			if ((VCEnc_ret = VCEncSetCodingCtrl(enc_ctx->hevc_encoder, &codingCfg)) != VCENC_OK) {
				ENCODER_DBG_ERROR("error: VCEncSetCodingCtrl fail\r\n");
				ret = -1;
				goto error;
			}
			ENCODER_DBG_INFO("RC init[ch%lu]: bps=%lu pic=%lu ctb=%lu hw_ctbrc=%lu block=%lu window=%lu static_i=%lu direction=%lu\r\n",
							 (unsigned long)enc_ctx->enc_parm.stream_id, (unsigned long)rcCfg.bitPerSecond,
							 (unsigned long)rcCfg.pictureRc, (unsigned long)rcCfg.ctbRc, (unsigned long)hw_cfg->ctbRcVersion,
							 (unsigned long)rcCfg.blockRCSize, (unsigned long)rcCfg.bitrateWindow,
							 (unsigned long)rcCfg.u32StaticSceneIbitPercent, (unsigned long)codingCfg.ctbRcDirection);
		}

		/* VCEncStrmStart requires at least 11 KiB, including 10 KiB of scratch space. */
		enc_ctx->hevc_header_buf_size = VCENC_STREAM_MIN_BUF0_SIZE;
		enc_ctx->hevc_header_buf = malloc(enc_ctx->hevc_header_buf_size);
		if (!enc_ctx->hevc_header_buf) {
			ENCODER_DBG_ERROR("error: fail to allocate hevc_header_buf\r\n");
			ret = -1;
			goto error;
		}
		pEncIn->busOutBuf[0] = (uint32_t)enc_ctx->hevc_header_buf; //enc_ctx->hevc_outbufMem.busAddress;
		pEncIn->outBufSize[0] = enc_ctx->hevc_header_buf_size; //enc_ctx->hevc_outbufMem.size;
		pEncIn->pOutBuf[0] = (u32 *)enc_ctx->hevc_header_buf;
		pEncIn->cur_out_buffer[0] = enc_ctx->hevc_header_buf;
		pEncIn->dec400Enable = 0;
		for (int i = 0; i < VCENC_MAX_LT_REF_FRAMES; i++) {
			pEncIn->long_term_ref_pic[i] = INVALITED_POC;
		}
		/* Video, sequence and picture parameter sets */
		if ((VCEnc_ret = VCEncStrmStart(enc_ctx->hevc_encoder, pEncIn, &enc_ctx->hevc_encOut)) != VCENC_OK) {
			ENCODER_DBG_ERROR("error: VCEncStrmStart fail\r\n");
			ret = -1;
			goto error;
		}
		/* stream header */
		//SaveStream(enc_ctx->hevc_header_buf, enc_ctx->hevc_encOut.streamSize);
		enc_ctx->hevc_streamSize += enc_ctx->hevc_encOut.streamSize;
		ENCODER_DBG_INFO("stream header: streamSize:%ld \r\n", enc_ctx->hevc_streamSize);
		free(enc_ctx->hevc_header_buf);
		enc_ctx->hevc_header_buf = NULL;
		enc_ctx->hevc_header_buf_size = 0;
		pEncIn->busOutBuf[0] = 0;
		pEncIn->outBufSize[0] = 0;
		pEncIn->pOutBuf[0] = NULL;
		pEncIn->cur_out_buffer[0] = NULL;

		/* OSD update notify queue */
		if (rtos_queue_create(&enc_ctx->hevc_osd_update_q, MAX_OVERLAY_NUM, sizeof(osd_overlay_t)) != SUCCESS) {
			ENCODER_DBG_ERROR("error: fail to create isp out queue, memory not enough\r\n");
			ret = -1;
			goto error;
		}
		xQueueReset(enc_ctx->hevc_osd_update_q);
		if (rtos_queue_create(&enc_ctx->hevc_mosaic_update_q, MAX_MOSAIC_NUM, sizeof(mosaic_overlay_t)) != SUCCESS) {
			ENCODER_DBG_ERROR("error: fail to create mosaic update queue, memory not enough\r\n");
			ret = -1;
			goto error;
		}
		xQueueReset(enc_ctx->hevc_mosaic_update_q);
	}

	if (enc_ctx->enc_parm.codec_fmt & VIDEO_JPEG) {

		int JpegEnc_ret = JPEGENC_OK;
		/* Step 1: Initialize an encoder instance */
		enc_ctx->jpeg_cfg.qLevel = enc_ctx->enc_parm.qlevel;
		enc_ctx->jpeg_cfg.quality = -1;  /* use quantization table defined by qLevel */
		enc_ctx->jpeg_cfg.frameType = JPEGENC_YUV420_SEMIPLANAR;
		enc_ctx->jpeg_cfg.markerType = JPEGENC_SINGLE_MARKER;
		enc_ctx->jpeg_cfg.unitsType = JPEGENC_DOTS_PER_INCH;
		enc_ctx->jpeg_cfg.xDensity = 72;
		enc_ctx->jpeg_cfg.yDensity = 72;
		enc_ctx->jpeg_cfg.irqTypeMask = 0x01f0;
		enc_ctx->jpeg_cfg.useVcmd = 1;
		/* decompressor setting */
		if (enc_ctx->enc_parm.decompEnable) {
			enc_ctx->jpeg_cfg.ufbcParam.mode = JPEG_UFBC_MODE_DEC400;
			enc_ctx->jpeg_cfg.ufbcParam.param.dec400.compAlign = 1; //0: aligmnent 32 byte, 1: aligmnent 64 byte
		}
		if ((JpegEnc_ret = JpegEncInit(&enc_ctx->jpeg_cfg, &enc_ctx->jpeg_encoder, NULL)) != JPEGENC_OK) {
			/* Handle the error situation */
			ENCODER_DBG_ERROR("error: JpegEncInit fail with ret:%d\r\n", JpegEnc_ret);
			ret = -1;
			goto error;
		}

		/* Step 2: Configuration of picture size */
		/* VGA resolution, no cropping */
		enc_ctx->jpeg_cfg.inputWidth = enc_ctx->enc_parm.width;
		enc_ctx->jpeg_cfg.codingWidth = enc_ctx->enc_parm.width;
		enc_ctx->jpeg_cfg.inputHeight = enc_ctx->enc_parm.height;
		enc_ctx->jpeg_cfg.codingHeight = enc_ctx->enc_parm.height;
		enc_ctx->jpeg_cfg.xOffset = 0;
		enc_ctx->jpeg_cfg.yOffset = 0;
		enc_ctx->jpeg_cfg.rotation = enc_ctx->enc_parm.rotation;
		enc_ctx->jpeg_cfg.codingType = JPEGENC_WHOLE_FRAME;
		/* Restart interval 5 MCU rows (= 80 pixel rows), average quantization,
		encode whole frame at once */
		enc_ctx->jpeg_cfg.restartInterval = 0; //5;
		if ((JpegEnc_ret = JpegEncSetPictureSize(enc_ctx->jpeg_encoder, &enc_ctx->jpeg_cfg)) != JPEGENC_OK) {
			/* Handle the error situation */
			ENCODER_DBG_ERROR("error: JpegEncSetPictureSize fail with ret:%d\r\n", JpegEnc_ret);
			ret = -1;
			goto error;
		}

		/* Step 3: Allocate linear output buffer for snapshot only; JPEG-only
		 * channels encode directly into the MMF output pool item. */
		if (enc_ctx->enc_parm.codec_fmt != VIDEO_JPEG) {
			uint32_t outbufSize = (uint32_t)((uint64_t)enc_ctx->enc_parm.width * enc_ctx->enc_parm.height * 3 / 2);
			int EWL_ret = EWL_OK;
			if (outbufSize < JPEGENC_STREAM_MIN_BUF0_SIZE) {
				outbufSize = JPEGENC_STREAM_MIN_BUF0_SIZE;
			}
			/* AP invalidates whole lines when reading the hardware output directly. */
			outbufSize = ENC_ALIGN(outbufSize, VIDEO_SNAPSHOT_BUFFER_ALIGNMENT);
			if ((EWL_ret = EWLMallocLinear(enc_ctx->ewlInst, outbufSize, VIDEO_SNAPSHOT_BUFFER_ALIGNMENT, &enc_ctx->jpeg_outbufMem)) != EWL_OK) {
				ENCODER_DBG_ERROR("error: EWLMallocLinear fail with ret:%d\r\n", EWL_ret);
				ret = -1;
				goto error;
			}
		}

		/* Enable all frame headers */
		enc_ctx->jpeg_encIn.frameHeader = 1;
	}

error:
	if (ret != 0) {
		enc_close(enc_ctx);
	}

	return ret;
}

#define DBG_ENC_AXI_REG_DUMP    0

void enc_dump_axi_regs(void)
{
#if !DBG_ENC_AXI_REG_DUMP
	return;
#endif
	if ((ConfigDebugMmfDebug & _MMF_DBG_ENCODER_) == 0) {
		return;
	}
	uint32_t *vce_base = (uint32_t *)(ENCODER_REG_BASE + 0x1000);
	ENCODER_DBG_INFO("[ENC AXI] Accumulated ARLEN+1: 0x%lx\n", *(vce_base + 215));
	ENCODER_DBG_INFO("[ENC AXI] RVALID & RREADY: 0x%lx\n", *(vce_base + 216));
	ENCODER_DBG_INFO("[ENC AXI] ARVALID & ARREADY: 0x%lx\n", *(vce_base + 217));
	ENCODER_DBG_INFO("[ENC AXI] RVALID & RREADY & RLAST: 0x%lx\n", *(vce_base + 218));
	ENCODER_DBG_INFO("[ENC AXI] Accumulated AWLEN+1: 0x%lx\n", *(vce_base + 219));
	ENCODER_DBG_INFO("[ENC AXI] WVALID & WREADY: 0x%lx\n", *(vce_base + 220));
	ENCODER_DBG_INFO("[ENC AXI] AWVALID & AWREADY: 0x%lx\n", *(vce_base + 221));
	ENCODER_DBG_INFO("[ENC AXI] WVALID & WREADY & WLAST: 0x%lx\n", *(vce_base + 222));
	ENCODER_DBG_INFO("[ENC AXI] BVALID & BREADY: 0x%lx\n\n", *(vce_base + 223));
}

static void enc_setup_user_data_sei(enc_context_t *enc_ctx, VCEncIn *pEncIn)
{
	enc_coding_t *coding = &enc_ctx->enc_parm.initCoding;
	uint32_t slot = pEncIn->picture_cnt % ENC_SEI_USER_DATA_RING_NUM;

	pEncIn->externalSEICount = 0;
	pEncIn->pExternalSEI = NULL;

	if (!(coding->flags & VPAP_ENC_CODING_SEI_USER_DATA) || coding->seiUserData == 0) {
		return;
	}

	enc_ctx->sei_user_data[slot].nalType = ENC_SEI_HEVC_PREFIX_NAL_TYPE;
	enc_ctx->sei_user_data[slot].payloadType = ENC_SEI_USER_DATA_PAYLOAD_TYPE;
	enc_ctx->sei_user_data[slot].payloadDataSize = sizeof(g_enc_sei_uuid);
	enc_ctx->sei_user_data[slot].pPayloadData = g_enc_sei_uuid;
	enc_ctx->sei_user_data[slot].frame_id = pEncIn->picture_cnt;
	pEncIn->externalSEICount = 1;
	pEncIn->pExternalSEI = &enc_ctx->sei_user_data[slot];
}

static int enc_hevc_encode(enc_context_t *enc_ctx)
{
	int ret = 0;
	int ch = enc_ctx->enc_parm.stream_id;
	uint32_t ts = 0, te = 0;
	int VCEnc_ret = VCENC_OK;
	uint32_t cmpr_header_y;
	uint32_t cmpr_header_uv;

	VCEncIn *pEncIn = &(enc_ctx->hevc_encIn);

	/* Step 5: Encode the video frames */
	/* Setup encoder input, we assume one linear buffer for the input image in */
	pEncIn->busLuma = enc_ctx->enc_y_addr;  //enc_ctx->hevc_pictureMem.busAddress;
	pEncIn->busChromaU = enc_ctx->enc_uv_addr;
	pEncIn->busChromaV = pEncIn->busChromaU + enc_ctx->hevc_chromaSize / 2;

	pEncIn->busOutBuf[0] = (unsigned int)enc_ctx->dest_addr;
	pEncIn->outBufSize[0] = enc_ctx->dest_buf_len;
	pEncIn->pOutBuf[0] = (u32 *)enc_ctx->dest_addr;
	pEncIn->cur_out_buffer[0] = (void *)enc_ctx->dest_addr;

	ENCODER_DBG_DEBUG("[ENC BEFORE]busLuma:0x%x busChromaU:0x%x\r\n", pEncIn->busLuma, pEncIn->busChromaU);

	/* decompressor */
	if (enc_ctx->enc_parm.decompEnable) {
		cmpr_header_y = (unsigned int)enc_ctx->enc_parm.ispDecompLumTable;
		cmpr_header_uv = (unsigned int)enc_ctx->enc_parm.ispDecompChrTable;
		pEncIn->dec400LumTableBase = cmpr_header_y/*(unsigned int)enc_ctx->enc_parm.ispDecompLumTable*/;
		pEncIn->dec400CbTableBase =	cmpr_header_uv/*(unsigned int)enc_ctx->enc_parm.ispDecompChrTable*/;
		pEncIn->dec400CrTableBase =	cmpr_header_uv/* + enc_ctx->dec400ChrTableSize / 2*/;
		pEncIn->dec400Enable = enc_ctx->dec400Enable;
		pEncIn->dec400TSHeaderEnable = enc_ctx->dec400TSHeaderEnable;
		ENCODER_DBG_DEBUG("[ENC BEFORE]dec400Enable:%d dec400TSHeaderEnable:%d\n", pEncIn->dec400Enable, pEncIn->dec400TSHeaderEnable);
		ENCODER_DBG_DEBUG("[ENC BEFORE]dec400LumTableBase:0x%x dec400CbTableBase:0x%x\r\n", pEncIn->dec400LumTableBase, pEncIn->dec400CbTableBase);
	}

	if (enc_apply_osd_scene(enc_ctx) != 0) {
		return -1;
	}

	/* OSD and mosaic update: drain both queues under a single Get/Set pair so that
	 * changes from both sources are visible to the encoder in the same frame. */
	osd_overlay_t osdOverlayUpdate;
	mosaic_overlay_t mosaicUpdate;
	int has_osd = rtos_queue_receive(enc_ctx->hevc_osd_update_q, &osdOverlayUpdate, 0) == SUCCESS;
	int has_mosaic = rtos_queue_receive(enc_ctx->hevc_mosaic_update_q, &mosaicUpdate, 0) == SUCCESS;
	if (has_osd || has_mosaic) {
		if ((VCEnc_ret = VCEncGetPreProcessing(enc_ctx->hevc_encoder, &enc_ctx->hevc_preProcCfg)) != VCENC_OK) {
			ENCODER_DBG_ERROR("error: VCEncGetPreProcessing fail\r\n");
			ret = -1;
			goto error;
		}

		if (has_osd) {
			ENCODER_DBG_DEBUG("Get OSD from queue and set\r\n");
			do {
				/* A legacy sender may have raced with scene ownership. It must
				 * never replace or disable the privacy compositor's overlay. */
				if (enc_ctx->osd_scene_owned) {
					continue;
				}
				ENCODER_DBG_DEBUG("OSD param: %d %d %lu [%lu %lu %lu %lu] [%lu %lu %lu %lu] 0x%08x\r\n",
								  enc_ctx->enc_parm.stream_id,
								  osdOverlayUpdate.osdOverlayIdx,
								  osdOverlayUpdate.osdOverlayFormat,
								  osdOverlayUpdate.osdOverlayXoffset,
								  osdOverlayUpdate.osdOverlayYoffset,
								  osdOverlayUpdate.osdOverlayWidth,
								  osdOverlayUpdate.osdOverlayHeight,
								  osdOverlayUpdate.osdOverlayAlpha,
								  osdOverlayUpdate.osdOverlayBitmapY,
								  osdOverlayUpdate.osdOverlayBitmapU,
								  osdOverlayUpdate.osdOverlayBitmapV,
								  osdOverlayUpdate.osdOverlayPicBuf);
				int idx = osdOverlayUpdate.osdOverlayIdx;
				if (idx < 0 || idx >= MAX_OVERLAY_NUM) {
					ENCODER_DBG_ERROR("error: invalid OSD idx %d\r\n", idx);
					continue;
				}
				memcpy(&enc_ctx->enc_parm.osd_overlay[idx], &osdOverlayUpdate, sizeof(osd_overlay_t));
				enc_osd_config(&enc_ctx->hevc_preProcCfg.overlayArea[idx], &enc_ctx->enc_parm.osd_overlay[idx]);
			} while (rtos_queue_receive(enc_ctx->hevc_osd_update_q, &osdOverlayUpdate, 0) == SUCCESS);
		}

		if (has_mosaic) {
			ENCODER_DBG_DEBUG("Get mosaic from queue and set\r\n");
			do {
				int idx = mosaicUpdate.mosOverlayIdx;
				if (idx < 0 || idx >= MAX_MOSAIC_NUM) {
					ENCODER_DBG_ERROR("error: invalid mosaic idx %d\r\n", idx);
					continue;
				}
				mosaic_overlay_t *mos = &enc_ctx->enc_parm.mosaic[idx];
				mos->mosOverlayEnable = mosaicUpdate.mosOverlayEnable ? 1 : 0;
				mos->mosOverlayXoffset = mosaicUpdate.mosOverlayXoffset;
				mos->mosOverlayYoffset = mosaicUpdate.mosOverlayYoffset;
				mos->mosOverlayWidth = mosaicUpdate.mosOverlayWidth;
				mos->mosOverlayHeight = mosaicUpdate.mosOverlayHeight;
				mos->mosOverlaySizeIdx = mosaicUpdate.mosOverlaySizeIdx;
				enc_ctx->hevc_preProcCfg.mosEnable[idx] = mos->mosOverlayEnable;
				enc_ctx->hevc_preProcCfg.mosXoffset[idx] = mos->mosOverlayXoffset;
				enc_ctx->hevc_preProcCfg.mosYoffset[idx] = mos->mosOverlayYoffset;
				enc_ctx->hevc_preProcCfg.mosWidth[idx] = mos->mosOverlayWidth;
				enc_ctx->hevc_preProcCfg.mosHeight[idx] = mos->mosOverlayHeight;
				if (mos->mosOverlayEnable) {
					enc_ctx->hevc_preProcCfg.mosSizeIndex = mosaicUpdate.mosOverlaySizeIdx;
				}
			} while (rtos_queue_receive(enc_ctx->hevc_mosaic_update_q, &mosaicUpdate, 0) == SUCCESS);
		}

		/* Always call SetPreProcessing after draining the queues. Skipping it based
		 * on a per-item update count is fragile (e.g. continue on invalid idx skips
		 * the count increment), and a no-op Set when nothing changed is harmless. */
		if ((VCEnc_ret = VCEncSetPreProcessing(enc_ctx->hevc_encoder, &enc_ctx->hevc_preProcCfg)) != VCENC_OK) {
			if (enc_ctx->osd_scene_owned) {
				enc_ctx->osd_scene_fault = 1;
				return -1;
			}
			/* A rejected preprocessing update (typically two overlays overlapping /
			 * sharing a CTB) must not stall the pipeline: warn, skip drawing OSD this
			 * frame, and keep encoding. */
			ENCODER_DBG_WARNING("warning: VCEncSetPreProcessing rejected osd update (ret %d), skip osd this frame\r\n", VCEnc_ret);
			/* Dump every enabled overlay so we can see which pair overlaps/shares a CTB.
			 * A 64x64 (H.265) / 64x16 (H.264) CTB is shared when the ranges below touch. */
			ENCODER_DBG_DEBUG("dump enabled overlays:\r\n");
			for (int di = 0; di < MAX_OVERLAY_NUM; di++) {
				VCEncOverlayArea *oa = &enc_ctx->hevc_preProcCfg.overlayArea[di];
				if (!oa->enable) {
					continue;
				}
				ENCODER_DBG_DEBUG("  overlay %d: x[%d..%d] y[%d..%d] w %d h %d\r\n",
								  di, oa->xoffset, oa->xoffset + oa->width - 1,
								  oa->yoffset, oa->yoffset + oa->height - 1,
								  oa->width, oa->height);
			}
			/* Do not draw OSD this frame: disable the overlays so pEncIn does not
			 * reference the rejected config. The next valid update re-enables them. */
			for (int di = 0; di < MAX_OVERLAY_NUM; di++) {
				enc_ctx->enc_parm.osd_overlay[di].osdOverlayEnable = 0;
			}
		}
	}
	/* OSD overlay area set */
	for (int i = 0; i < MAX_OVERLAY_NUM; i++) {
		osd_overlay_t *osdOverlay = &enc_ctx->enc_parm.osd_overlay[i];
		pEncIn->overlayEnable[i] = osdOverlay->osdOverlayEnable;
		if (osdOverlay->osdOverlayEnable) {
			pEncIn->overlayInputYAddr[i] = (unsigned int)osdOverlay->osdOverlayPicBuf;
			uint32_t size_luma = osdOverlay->osdOverlayWidth * osdOverlay->osdOverlayHeight;
			pEncIn->overlayInputUAddr[i] = pEncIn->overlayInputYAddr[i] + size_luma;
			pEncIn->osdDec400Enable[i] = 1;  // bypass
		}
	}

	/* Set resend Parameter Set flag for IDR frame. A pending enc_force_iframe()
	 * request takes priority so it is not silently dropped by the periodic check. */
	int idr_interval = enc_ctx->hevc_encIn.gopConfig.idr_interval;
	uint8_t is_idr = enc_ctx->force_idr || (idr_interval && (pEncIn->picture_cnt % idr_interval == 0));
	pEncIn->resendPPS = pEncIn->resendSPS = pEncIn->resendVPS = is_idr;
	pEncIn->bIsIDR = is_idr;
	enc_ctx->force_idr = 0;
	/* Set SEI user data */
	enc_setup_user_data_sei(enc_ctx, pEncIn);

	/* Advance the RC clock for every picture after the first, including IDRs.
	 * Leaving timeIncrement at zero makes RC accumulate bitrate overshoot. */
	VCEncRateCtrl rcCfg = {0};
	if ((VCEnc_ret = VCEncGetRateCtrl(enc_ctx->hevc_encoder, &rcCfg)) != VCENC_OK) {
		ENCODER_DBG_ERROR("error: VCEncGetRateCtrl for frame timing fail with ret:%d\r\n", VCEnc_ret);
		ret = -1;
		goto error;
	}
	pEncIn->timeIncrement = pEncIn->picture_cnt ? rcCfg.frameRateDenom : 0;

	ENCODER_DBG_DEBUG("[ch%d]start encoding ISP data ...\n\n", ch);
	enc_ctx->hevc_frame_size = 0;
	ts = mm_read_mediatime_ms();
	VCEnc_ret = VCEncStrmEncode(enc_ctx->hevc_encoder, pEncIn, &enc_ctx->hevc_encOut, NULL, NULL);
	/* Save encoded stream */
	switch (VCEnc_ret) {
	case VCENC_FRAME_ENQUEUE:
		/* A frame is inside the encoder's internal queue but not encoded. It will be encoded and output in a subsequent encoder API call. */
		pEncIn->picture_cnt++;
		break;
	case VCENC_FRAME_READY:
		/* Encoding of a frame is finished. */
		if (enc_ctx->hevc_encOut.codingType != VCENC_NOTCODED_FRAME) {
			pEncIn->picture_cnt++;
		}
		if (enc_ctx->hevc_encOut.streamSize != 0) {
			enc_ctx->hevc_streamSize += enc_ctx->hevc_encOut.streamSize;
			enc_ctx->hevc_frame_size += enc_ctx->hevc_encOut.streamSize;
			enc_ctx->hevc_picture_encoded_cnt++;
			ENCODER_DBG_DEBUG("VCEncStrmEncode[ch:%d] OK\r\n", ch);
		}
		break;
	case VCENC_OUTPUT_BUFFER_OVERFLOW:
		pEncIn->picture_cnt++;
		ENCODER_DBG_ERROR("error: VCEncStrmEncode[ch:%d] buffer overflow, frame lost!\r\n", ch);
		ret = -1;
		goto error;
	case VCENC_SYSTEM_ERROR:
		/* Fatal system error, can’t continue. */
		ENCODER_DBG_ERROR("error: VCEncStrmEncode[ch:%d] with return VCENC_SYSTEM_ERROR. Fatal system error, can’t continue.\r\n", ch);
		ret = -1;
		goto error;
	default:
		ENCODER_DBG_ERROR("error: VCEncStrmEncode[ch:%d] unexpected error: %d.\r\n", ch, VCEnc_ret);
		ret = -1;
		goto error;
	}
	te = mm_read_mediatime_ms();
	ENCODER_DBG_DEBUG("%s encoded[ch:%d][size:%lu][t:%lu][hw cyc:%lu]\n\n",
					  enc_ctx->enc_parm.codec_fmt & VIDEO_HEVC ? "HEVC" : "H264",
					  ch,
					  enc_ctx->hevc_frame_size,
					  te - ts,
					  VCEncGetPerformance(enc_ctx->hevc_encoder));

error:
	enc_dump_axi_regs();

	return ret;
}

static int enc_jpeg_encode(enc_context_t *enc_ctx)
{
	int ret = 0;
	int ch = enc_ctx->enc_parm.stream_id;
	uint32_t ts = 0, te = 0;
	int JpegEnc_ret = JPEGENC_OK;
	JpegEncIn *pEncIn = &enc_ctx->jpeg_encIn;
	uint32_t cmpr_header_y;
	uint32_t cmpr_header_uv;
	/* reset frame size */
	enc_ctx->jpeg_frame_size = 0;
	/* Step 4: Get the picture and setup input */
	/* We use one linear buffer for the input image: YYYY...UU...VV... */
	pEncIn->busLum = enc_ctx->jpeg_y_addr;
	pEncIn->busCb = enc_ctx->jpeg_uv_addr;
	pEncIn->busCr = pEncIn->busCb + (enc_ctx->jpeg_cfg.inputWidth / 2) * (enc_ctx->jpeg_cfg.inputHeight / 2);
	/* Virtual addresses of input picture, used by software encoder */
	pEncIn->pLum = (uint8_t *)pEncIn->busLum;
	pEncIn->pCb = (uint8_t *)pEncIn->busCb;
	pEncIn->pCr = (uint8_t *)pEncIn->busCr;
	/* Setup Output */
	if (enc_ctx->enc_parm.codec_fmt == VIDEO_JPEG) {
		/* jpeg stream */
		pEncIn->pOutBuf[0] = (uint8_t *)enc_ctx->dest_addr;
		pEncIn->busOutBuf[0] = enc_ctx->dest_addr;
		pEncIn->outBufSize[0] = enc_ctx->dest_buf_len; /* bytes */
	} else {
		/* snapshot: consume the request under the same lock CMD_ENC_JPEG_DO_SNAPSHOT arms it with */
		int fire = 0;
		taskENTER_CRITICAL();
		if (enc_ctx->snapshot_trigger && ++enc_ctx->snapshot_counter == enc_ctx->snapshot_idx) {
			enc_ctx->snapshot_counter = 0;
			enc_ctx->snapshot_trigger = 0;
			fire = 1;
		}
		taskEXIT_CRITICAL();
		if (!fire) {
			return ret;  //not triggered or skip some frame
		}
		pEncIn->pOutBuf[0] = (uint8_t *)enc_ctx->jpeg_outbufMem.virtualAddress;
		pEncIn->busOutBuf[0] = enc_ctx->jpeg_outbufMem.busAddress;
		pEncIn->outBufSize[0] = enc_ctx->jpeg_outbufMem.size; /* bytes */
		ENCODER_DBG_DEBUG("pEncIn->pOutBuf[0] %p\r\n", pEncIn->pOutBuf[0]);
	}

	ENCODER_DBG_DEBUG("[JPEG ENC BEFORE]busLum:0x%x busCb:0x%x\r\n", pEncIn->busLum, pEncIn->busCb);

	/* decompressor */
	if (enc_ctx->enc_parm.decompEnable) {
		cmpr_header_y = (unsigned int)enc_ctx->enc_parm.ispDecompLumTable;
		cmpr_header_uv = (unsigned int)enc_ctx->enc_parm.ispDecompChrTable;
		pEncIn->dec400TableBusLum = cmpr_header_y/*(unsigned int)enc_ctx->enc_parm.ispDecompLumTable*/;
		pEncIn->dec400TableBusCb =	cmpr_header_uv/*(unsigned int)enc_ctx->enc_parm.ispDecompChrTable*/;
		pEncIn->dec400TableBusCr =	cmpr_header_uv/* + enc_ctx->dec400ChrTableSize / 2*/;
		pEncIn->dec400TablepLum = (const u8 *)pEncIn->dec400TableBusLum;
		pEncIn->dec400TablepCb = (const u8 *)pEncIn->dec400TableBusCb;
		pEncIn->dec400TablepCr = (const u8 *)pEncIn->dec400TableBusCr;
		pEncIn->dec400Enable = enc_ctx->dec400Enable;
		pEncIn->dec400TSHeaderEnable = enc_ctx->dec400TSHeaderEnable;
		ENCODER_DBG_DEBUG("[JPEG ENC BEFORE]dec400Enable:%d dec400TSHeaderEnable:%d\n", pEncIn->dec400Enable, pEncIn->dec400TSHeaderEnable);
		ENCODER_DBG_DEBUG("[JPEG ENC BEFORE]dec400LumTableBase:0x%x dec400CbTableBase:0x%x\r\n", pEncIn->dec400TableBusLum, pEncIn->dec400TableBusCb);
	}

	/* Step 5: Encode the picture */
	/* Loop until the frame is ready */
	ts = mm_read_mediatime_ms();
	do {
		JpegEnc_ret = JpegEncEncode(enc_ctx->jpeg_encoder, &enc_ctx->jpeg_encIn, &enc_ctx->jpeg_encOut);
		switch (JpegEnc_ret) {
		case JPEGENC_RESTART_INTERVAL:
		case JPEGENC_FRAME_READY: {
			//SaveStream(pEncIn->pOutBuf[0], enc_ctx->jpeg_encOut.jfifSize, outP);
			// dump_addr(pEncIn->pOutBuf[0], enc_ctx->jpeg_encOut.jfifSize);
			enc_ctx->jpeg_frame_size += enc_ctx->jpeg_encOut.jfifSize;
			if (JpegEnc_ret == JPEGENC_FRAME_READY) {
				ENCODER_DBG_DEBUG("JpegEncEncode[ch:%d] OK\r\n", ch);
			}
			break;
		}
		case JPEGENC_OUTPUT_BUFFER_OVERFLOW:
			ENCODER_DBG_ERROR("error: JpegEncEncode[ch:%d] buffer overflow, frame lost!\r\n", ch);
			ret = -1;
			goto error;
		default:
			/* All the others are error codes */
			ENCODER_DBG_ERROR("error: JpegEncEncode[ch:%d] fail:%d\r\n", ch, JpegEnc_ret);
			ret = -1;
			goto error;
		}
	} while (JpegEnc_ret == JPEGENC_RESTART_INTERVAL);
	te = mm_read_mediatime_ms();

	ENCODER_DBG_DEBUG("Jpeg encoded[ch:%d][size:%lu][t:%lu][hw cyc:%lu]\n\n",
					  ch,
					  enc_ctx->jpeg_frame_size,
					  te - ts,
					  JpegEncGetPerformance(enc_ctx->jpeg_encoder));

error:
	if (enc_ctx->enc_parm.codec_fmt != VIDEO_JPEG) {
		if (ret < 0) {
			enc_ctx->jpeg_frame_size = 0;
		}
		/* Report every fired snapshot here, independent of the live encode result. */
		if (enc_ctx->snapshot_cb) {
			enc_ctx->snapshot_cb(enc_ctx->jpeg_frame_size ? (void *)pEncIn->pOutBuf[0] : NULL, enc_ctx->jpeg_frame_size);
		}
	}
	enc_dump_axi_regs();

	return ret;
}

static int enc_encode(enc_context_t *enc_ctx)
{
	uint32_t format = enc_ctx->enc_parm.codec_fmt;
	int err = 0;
	/* Live and snapshot encodes are independent, so both failures can be reported together. */
	if ((format & (VIDEO_HEVC | VIDEO_H264)) && enc_hevc_encode(enc_ctx) < 0) {
		err |= ENC_ERR_ENCODE;
	}
	if ((format & VIDEO_JPEG) && enc_jpeg_encode(enc_ctx) < 0) {
		err |= (format == VIDEO_JPEG) ? ENC_ERR_ENCODE : ENC_ERR_SNAPSHOT;
	}
	return err;
}

static int enc_get_rate_control(enc_context_t *enc_ctx, rc_param_t *p)
{
	int ret = 0;
	int VCEnc_ret = VCENC_OK;
	VCEncRateCtrl rcCfg = {0};
	if ((VCEnc_ret = VCEncGetRateCtrl(enc_ctx->hevc_encoder, &rcCfg)) != VCENC_OK) {
		ENCODER_DBG_ERROR("error: VCEncGetRateCtrl fail with ret:%d\r\n", VCEnc_ret);
		ret = -1;
		goto error;
	}
	ENCODER_DBG_DEBUG(
		"Get rate control: qp %2d qpRange I[%2u, %2u] PB[%2u, %2u] %8u bps  "
		"pic %u skip %u  hrd %u  cpbSize %u cpbMaxRate %u bitrateWindow %u "
		"intraQpDelta %2d\n",
		rcCfg.qpHdr, rcCfg.qpMinI, rcCfg.qpMaxI, rcCfg.qpMinPB, rcCfg.qpMaxPB,
		rcCfg.bitPerSecond, rcCfg.pictureRc, rcCfg.pictureSkip, rcCfg.hrd,
		rcCfg.hrdCpbSize, rcCfg.cpbMaxRate, rcCfg.bitrateWindow,
		rcCfg.intraQpDelta);
	p->rcMode = rcCfg.rcMode;
	p->bps = rcCfg.bitPerSecond;
	p->minQp = rcCfg.qpMinPB;
	p->maxQp = rcCfg.qpMaxPB;
	p->intraQpDelta = rcCfg.intraQpDelta;
	DCache_Clean((u32)p, sizeof(rc_param_t));

error:
	return ret;
}

static int enc_set_rate_control(enc_context_t *enc_ctx, rc_param_t *p)
{
	int ret = 0;
	int VCEnc_ret = VCENC_OK;
	VCEncRateCtrl rcCfg = {0};
	if ((VCEnc_ret = VCEncGetRateCtrl(enc_ctx->hevc_encoder, &rcCfg)) != VCENC_OK) {
		ENCODER_DBG_ERROR("error: VCEncGetRateCtrl fail with ret:%d\r\n", VCEnc_ret);
		ret = -1;
		goto error;
	}
	DCache_Invalidate((u32)p, sizeof(rc_param_t));
	ENCODER_DBG_DEBUG("[enc_set_rc] old: rcMode=%lu bps=%lu qpMin=%lu qpMax=%lu intraQpDelta=%ld\r\n",
					  (unsigned long)rcCfg.rcMode, (unsigned long)rcCfg.bitPerSecond,
					  (unsigned long)rcCfg.qpMinPB, (unsigned long)rcCfg.qpMaxPB,
					  (long)rcCfg.intraQpDelta);
	ENCODER_DBG_DEBUG("[enc_set_rc] new: rcMode=%lu bps=%lu qpMin=%lu qpMax=%lu intraQpDelta=%ld\r\n",
					  (unsigned long)p->rcMode, (unsigned long)p->bps,
					  (unsigned long)p->minQp, (unsigned long)p->maxQp,
					  (long)p->intraQpDelta);
	/* Bitrate/QP-only updates must preserve an explicit picture RC override. */
	if (rcCfg.rcMode != p->rcMode) {
		rcCfg.pictureRc = (p->rcMode != ENC_CQP);
		rcCfg.ctbRc = (p->rcMode == ENC_CQP) ? 0 : enc_ctx->hevc_cfg.ctbRcMode;
		if (p->rcMode == ENC_CRF || rcCfg.crf >= 0) {
			rcCfg.ctbRc &= 1;
		}
	}
	rcCfg.rcMode = p->rcMode;
	rcCfg.bitPerSecond = p->bps;
	rcCfg.qpMinI = rcCfg.qpMinPB = p->minQp;
	rcCfg.qpMaxI = rcCfg.qpMaxPB = p->maxQp;
	rcCfg.intraQpDelta = p->intraQpDelta;
	if ((VCEnc_ret = VCEncSetRateCtrl(enc_ctx->hevc_encoder, &rcCfg)) != VCENC_OK) {
		ENCODER_DBG_ERROR("error: VCEncSetRateCtrl fail with ret:%d\r\n", VCEnc_ret);
		ret = -1;
		goto error;
	}

error:
	return ret;
}

static void enc_rc_ext_from_driver(enc_rc_ext_t *p, VCEncRateCtrl *rcCfg)
{
	p->flags = VPAP_ENC_RC_EXT_ALL;
	p->rcMode = rcCfg->rcMode;
	p->bps = rcCfg->bitPerSecond;
	p->qpHdr = rcCfg->qpHdr;
	p->minQp = rcCfg->qpMinPB;
	p->maxQp = rcCfg->qpMaxPB;
	p->minIQp = rcCfg->qpMinI;
	p->maxIQp = rcCfg->qpMaxI;
	p->intraQpDelta = rcCfg->intraQpDelta;
	p->fixedIntraQp = rcCfg->fixedIntraQp;
	p->pictureRc = rcCfg->pictureRc;
	p->ctbRc = rcCfg->ctbRc;
	p->blockRCSize = rcCfg->blockRCSize;
	p->pictureSkip = rcCfg->pictureSkip;
	p->hrd = rcCfg->hrd;
	p->hrdCpbSize = rcCfg->hrdCpbSize;
	p->cpbMaxRate = rcCfg->cpbMaxRate;
	p->bitrateWindow = rcCfg->bitrateWindow;
	p->tolMovingBitRate = rcCfg->tolMovingBitRate;
	p->monitorFrames = rcCfg->monitorFrames;
	p->fillerData = rcCfg->fillerData;
	p->crf = rcCfg->crf;
	p->vbr = rcCfg->vbr;
	p->frameRateNum = rcCfg->frameRateNum;
	p->frameRateDenom = rcCfg->frameRateDenom;
}

static void enc_rc_ext_to_driver(VCEncRateCtrl *rcCfg, enc_rc_ext_t *p)
{
	if (p->flags & VPAP_ENC_RC_EXT_RCMODE) {
		rcCfg->rcMode = p->rcMode;
		/* A subsequent PIC_RC flag can explicitly override this mode default. */
		rcCfg->pictureRc = (p->rcMode != ENC_CQP);
	}
	if (p->flags & VPAP_ENC_RC_EXT_BPS) {
		rcCfg->bitPerSecond = p->bps;
	}
	if (p->flags & VPAP_ENC_RC_EXT_QP_HDR) {
		rcCfg->qpHdr = p->qpHdr;
	}
	if (p->flags & VPAP_ENC_RC_EXT_QP_RANGE) {
		rcCfg->qpMinI = rcCfg->qpMinPB = p->minQp;
		rcCfg->qpMaxI = rcCfg->qpMaxPB = p->maxQp;
	}
	if (p->flags & VPAP_ENC_RC_EXT_I_QP_RANGE) {
		rcCfg->qpMinI = p->minIQp;
		rcCfg->qpMaxI = p->maxIQp;
	}
	if (p->flags & VPAP_ENC_RC_EXT_INTRA_QP_DELTA) {
		rcCfg->intraQpDelta = p->intraQpDelta;
	}
	if (p->flags & VPAP_ENC_RC_EXT_FIXED_INTRA_QP) {
		rcCfg->fixedIntraQp = p->fixedIntraQp;
	}
	if (p->flags & VPAP_ENC_RC_EXT_PIC_RC) {
		rcCfg->pictureRc = p->pictureRc;
	}
	if (p->flags & VPAP_ENC_RC_EXT_CTB_RC) {
		rcCfg->ctbRc = p->ctbRc;
	}
	if (p->flags & VPAP_ENC_RC_EXT_BLOCK_RC_SIZE) {
		rcCfg->blockRCSize = p->blockRCSize;
	}
	if (p->flags & VPAP_ENC_RC_EXT_PIC_SKIP) {
		rcCfg->pictureSkip = p->pictureSkip;
	}
	if (p->flags & VPAP_ENC_RC_EXT_HRD) {
		rcCfg->hrd = p->hrd;
	}
	if (p->flags & VPAP_ENC_RC_EXT_CPB) {
		rcCfg->hrdCpbSize = p->hrdCpbSize;
		rcCfg->cpbMaxRate = p->cpbMaxRate;
	}
	if (p->flags & VPAP_ENC_RC_EXT_BITRATE_WINDOW) {
		rcCfg->bitrateWindow = p->bitrateWindow;
	}
	if (p->flags & VPAP_ENC_RC_EXT_MOVING_BITRATE) {
		rcCfg->tolMovingBitRate = p->tolMovingBitRate;
		rcCfg->monitorFrames = p->monitorFrames;
	}
	if (p->flags & VPAP_ENC_RC_EXT_FILLER_DATA) {
		rcCfg->fillerData = p->fillerData;
	}
	if (p->flags & VPAP_ENC_RC_EXT_CRF) {
		rcCfg->crf = p->crf;
	}
	if (p->flags & VPAP_ENC_RC_EXT_VBR) {
		rcCfg->vbr = p->vbr;
	}
	if (p->flags & VPAP_ENC_RC_EXT_FRAME_RATE) {
		rcCfg->frameRateNum = p->frameRateNum;
		rcCfg->frameRateDenom = p->frameRateDenom;
	}
}

static int enc_get_rate_control_ext(enc_context_t *enc_ctx, enc_rc_ext_t *p)
{
	int ret = 0;
	int VCEnc_ret = VCENC_OK;
	VCEncRateCtrl rcCfg = {0};

	if ((VCEnc_ret = VCEncGetRateCtrl(enc_ctx->hevc_encoder, &rcCfg)) != VCENC_OK) {
		ENCODER_DBG_ERROR("error: VCEncGetRateCtrl fail with ret:%d\r\n", VCEnc_ret);
		ret = -1;
		goto error;
	}
	enc_rc_ext_from_driver(p, &rcCfg);
	DCache_Clean((u32)p, sizeof(enc_rc_ext_t));

error:
	return ret;
}

static int enc_set_rate_control_ext(enc_context_t *enc_ctx, enc_rc_ext_t *p)
{
	int ret = 0;
	int VCEnc_ret = VCENC_OK;
	VCEncRateCtrl rcCfg = {0};

	if ((VCEnc_ret = VCEncGetRateCtrl(enc_ctx->hevc_encoder, &rcCfg)) != VCENC_OK) {
		ENCODER_DBG_ERROR("error: VCEncGetRateCtrl fail with ret:%d\r\n", VCEnc_ret);
		ret = -1;
		goto error;
	}
	DCache_Invalidate((u32)p, sizeof(enc_rc_ext_t));
	if (p->flags & VPAP_ENC_RC_EXT_RCMODE) {
		rcCfg.ctbRc = (p->rcMode == ENC_CQP) ? 0 : enc_ctx->hevc_cfg.ctbRcMode;
	}
	enc_rc_ext_to_driver(&rcCfg, p);
	if (!(p->flags & VPAP_ENC_RC_EXT_CTB_RC) && (rcCfg.rcMode == ENC_CRF || rcCfg.crf >= 0)) {
		rcCfg.ctbRc &= 1;
	}
	if ((rcCfg.ctbRc & 2) && !(enc_ctx->hevc_cfg.ctbRcMode & 2)) {
		ENCODER_DBG_ERROR("error: precise CTB RC requires buffers allocated at encoder open; reopen with CTB RC enabled\r\n");
		ret = -1;
		goto error;
	}
	if ((VCEnc_ret = VCEncSetRateCtrl(enc_ctx->hevc_encoder, &rcCfg)) != VCENC_OK) {
		ENCODER_DBG_ERROR("error: VCEncSetRateCtrl ext fail with ret:%d\r\n", VCEnc_ret);
		ret = -1;
		goto error;
	}

error:
	return ret;
}

static void enc_roi_from_driver(vpap_shared_enc_roi_t *dst, VCEncPictureArea *area, int delta_qp, int qp)
{
	dst->enable = area->enable;
	dst->top = area->top;
	dst->left = area->left;
	dst->bottom = area->bottom;
	dst->right = area->right;
	dst->deltaQp = delta_qp;
	dst->qp = qp;
}

/* sets only the area bounds; deltaQp and qp are applied by the caller in enc_coding_to_driver */
static void enc_roi_to_driver(VCEncPictureArea *area, vpap_shared_enc_roi_t *src)
{
	area->enable = src->enable;
	area->top = src->top;
	area->left = src->left;
	area->bottom = src->bottom;
	area->right = src->right;
}

static void enc_coding_from_driver(enc_coding_t *p, VCEncCodingCtrl *codingCfg)
{
	p->flags = VPAP_ENC_CODING_ALL;
	p->sliceSize = codingCfg->sliceSize;
	p->seiMessages = codingCfg->seiMessages;
	p->vuiVideoFullRange = codingCfg->vuiVideoFullRange;
	p->disableDeblockingFilter = codingCfg->disableDeblockingFilter;
	p->tcOffset = codingCfg->tc_Offset;
	p->betaOffset = codingCfg->beta_Offset;
	p->enableSao = codingCfg->enableSao;
	p->enableCabac = codingCfg->enableCabac;
	p->sampleAspectRatioWidth = codingCfg->sampleAspectRatioWidth;
	p->sampleAspectRatioHeight = codingCfg->sampleAspectRatioHeight;
	p->cirStart = codingCfg->cirStart;
	p->cirInterval = codingCfg->cirInterval;
	p->seiUserData = 0;
	enc_roi_from_driver(&p->roi[0], &codingCfg->roi1Area, codingCfg->roi1DeltaQp, codingCfg->roi1Qp);
	enc_roi_from_driver(&p->roi[1], &codingCfg->roi2Area, codingCfg->roi2DeltaQp, codingCfg->roi2Qp);
	enc_roi_from_driver(&p->roi[2], &codingCfg->roi3Area, codingCfg->roi3DeltaQp, codingCfg->roi3Qp);
	enc_roi_from_driver(&p->roi[3], &codingCfg->roi4Area, codingCfg->roi4DeltaQp, codingCfg->roi4Qp);
	enc_roi_from_driver(&p->roi[4], &codingCfg->roi5Area, codingCfg->roi5DeltaQp, codingCfg->roi5Qp);
	enc_roi_from_driver(&p->roi[5], &codingCfg->roi6Area, codingCfg->roi6DeltaQp, codingCfg->roi6Qp);
	enc_roi_from_driver(&p->roi[6], &codingCfg->roi7Area, codingCfg->roi7DeltaQp, codingCfg->roi7Qp);
	enc_roi_from_driver(&p->roi[7], &codingCfg->roi8Area, codingCfg->roi8DeltaQp, codingCfg->roi8Qp);
}

static void enc_coding_to_driver(VCEncCodingCtrl *codingCfg, enc_coding_t *p)
{
	if (p->flags & VPAP_ENC_CODING_SLICE_SIZE) {
		codingCfg->sliceSize = p->sliceSize;
	}
	if (p->flags & VPAP_ENC_CODING_SEI) {
		codingCfg->seiMessages = p->seiMessages;
	}
	if (p->flags & VPAP_ENC_CODING_FULL_RANGE) {
		codingCfg->vuiVideoFullRange = p->vuiVideoFullRange;
	}
	if (p->flags & VPAP_ENC_CODING_DEBLOCK) {
		codingCfg->disableDeblockingFilter = p->disableDeblockingFilter;
		codingCfg->tc_Offset = p->tcOffset;
		codingCfg->beta_Offset = p->betaOffset;
	}
	if (p->flags & VPAP_ENC_CODING_SAO) {
		codingCfg->enableSao = p->enableSao;
	}
	if (p->flags & VPAP_ENC_CODING_CABAC) {
		codingCfg->enableCabac = p->enableCabac;
	}
	if (p->flags & VPAP_ENC_CODING_SAR) {
		codingCfg->sampleAspectRatioWidth = p->sampleAspectRatioWidth;
		codingCfg->sampleAspectRatioHeight = p->sampleAspectRatioHeight;
	}
	if (p->flags & VPAP_ENC_CODING_CIR) {
		codingCfg->cirStart = p->cirStart;
		codingCfg->cirInterval = p->cirInterval;
	}
	if (p->flags & VPAP_ENC_CODING_ROI) {
		enc_roi_to_driver(&codingCfg->roi1Area, &p->roi[0]);
		enc_roi_to_driver(&codingCfg->roi2Area, &p->roi[1]);
		enc_roi_to_driver(&codingCfg->roi3Area, &p->roi[2]);
		enc_roi_to_driver(&codingCfg->roi4Area, &p->roi[3]);
		enc_roi_to_driver(&codingCfg->roi5Area, &p->roi[4]);
		enc_roi_to_driver(&codingCfg->roi6Area, &p->roi[5]);
		enc_roi_to_driver(&codingCfg->roi7Area, &p->roi[6]);
		enc_roi_to_driver(&codingCfg->roi8Area, &p->roi[7]);
		codingCfg->roi1DeltaQp = p->roi[0].deltaQp;
		codingCfg->roi2DeltaQp = p->roi[1].deltaQp;
		codingCfg->roi3DeltaQp = p->roi[2].deltaQp;
		codingCfg->roi4DeltaQp = p->roi[3].deltaQp;
		codingCfg->roi5DeltaQp = p->roi[4].deltaQp;
		codingCfg->roi6DeltaQp = p->roi[5].deltaQp;
		codingCfg->roi7DeltaQp = p->roi[6].deltaQp;
		codingCfg->roi8DeltaQp = p->roi[7].deltaQp;
		codingCfg->roi1Qp = p->roi[0].qp;
		codingCfg->roi2Qp = p->roi[1].qp;
		codingCfg->roi3Qp = p->roi[2].qp;
		codingCfg->roi4Qp = p->roi[3].qp;
		codingCfg->roi5Qp = p->roi[4].qp;
		codingCfg->roi6Qp = p->roi[5].qp;
		codingCfg->roi7Qp = p->roi[6].qp;
		codingCfg->roi8Qp = p->roi[7].qp;
	}
}

static int enc_get_coding_control(enc_context_t *enc_ctx, enc_coding_t *p)
{
	int ret = 0;
	int VCEnc_ret = VCENC_OK;
	VCEncCodingCtrl codingCfg = {0};

	if ((VCEnc_ret = VCEncGetCodingCtrl(enc_ctx->hevc_encoder, &codingCfg)) != VCENC_OK) {
		ENCODER_DBG_ERROR("error: VCEncGetCodingCtrl fail with ret:%d\r\n", VCEnc_ret);
		ret = -1;
		goto error;
	}
	enc_coding_from_driver(p, &codingCfg);
	p->seiUserData = enc_ctx->enc_parm.initCoding.seiUserData;
	DCache_Clean((u32)p, sizeof(enc_coding_t));

error:
	return ret;
}

static int enc_set_coding_control(enc_context_t *enc_ctx, enc_coding_t *p)
{
	int ret = 0;
	int VCEnc_ret = VCENC_OK;
	VCEncCodingCtrl codingCfg = {0};

	if ((VCEnc_ret = VCEncGetCodingCtrl(enc_ctx->hevc_encoder, &codingCfg)) != VCENC_OK) {
		ENCODER_DBG_ERROR("error: VCEncGetCodingCtrl fail with ret:%d\r\n", VCEnc_ret);
		ret = -1;
		goto error;
	}
	DCache_Invalidate((u32)p, sizeof(enc_coding_t));
	if (p->flags & VPAP_ENC_CODING_SEI_USER_DATA) {
		enc_ctx->enc_parm.initCoding.flags |= VPAP_ENC_CODING_SEI_USER_DATA;
		enc_ctx->enc_parm.initCoding.seiUserData = p->seiUserData;
	}
	enc_coding_to_driver(&codingCfg, p);
	if ((VCEnc_ret = VCEncSetCodingCtrl(enc_ctx->hevc_encoder, &codingCfg)) != VCENC_OK) {
		ENCODER_DBG_ERROR("error: VCEncSetCodingCtrl fail with ret:%d\r\n", VCEnc_ret);
		ret = -1;
		goto error;
	}

error:
	return ret;
}

void enc_set_gop(enc_context_t *enc_ctx, int arg)
{
	int idr_interval = arg;
	VCEncIn *pEncIn = &(enc_ctx->hevc_encIn);
	pEncIn->gopConfig.idr_interval = idr_interval;
}

void enc_force_iframe(enc_context_t *enc_ctx)
{
	enc_ctx->force_idr = 1;
}

int enc_set_osd(enc_context_t *enc_ctx, osd_overlay_t *osdOverlay)
{
	if (!enc_ctx->hevc_osd_update_q || enc_ctx->osd_scene_owned) {
		return -1;
	}
	if (rtos_queue_send(enc_ctx->hevc_osd_update_q, osdOverlay, 100) != SUCCESS) {
		ENCODER_DBG_WARNING("warning: hevc_osd_update_q full, osd dropped\r\n");
		return -1;
	}
	return 0;
}

void enc_set_mosaic(enc_context_t *enc_ctx, mosaic_overlay_t *mosaic)
{
	if (!enc_ctx->hevc_mosaic_update_q) {
		return;
	}
	if (rtos_queue_send(enc_ctx->hevc_mosaic_update_q, mosaic, 0) != SUCCESS) {
		ENCODER_DBG_WARNING("warning: hevc_mosaic_update_q full, mosaic dropped\r\n");
	}
}

enc_context_t *enc_ctx_init(void)
{
	enc_context_t *enc_ctx = (enc_context_t *)malloc(sizeof(enc_context_t));
	if (!enc_ctx) {
		ENCODER_DBG_ERROR("error: fail alloc for enc_ctx\r\n");
		return NULL;
	}
	memset(enc_ctx, 0, sizeof(enc_context_t));
	return enc_ctx;
}

int enc_ctx_deinit(enc_context_t *enc_ctx)
{
	if (enc_ctx) {
		if (enc_ctx->proc_img_buf) {
			free(enc_ctx->proc_img_buf);
		}
		free(enc_ctx);
	}
	return 0;
}

void enc_hw_init(void)
{
#if !defined(CONFIG_FPGA) || (CONFIG_FPGA == 0)
	/* encoder hw init on rtk system */
	enc_rtk_hw_init();
#endif
}

static int enc_driver_init(void)
{
	if (g_enc.driver_opened) {
		ENCODER_DBG_INFO("ENC driver opened already\r\n");
		return 0;
	}
	/* init sw encoder driver */
	extern int encoder_rtos_driver_init(void);
	if (encoder_rtos_driver_init() != 0) {
		ENCODER_DBG_ERROR("error: encoder driver init failed\r\n");
		return -1;
	}
	g_enc.driver_opened = 1;
	return 0;
}

UNUSED_WARN_DIS
static void enc_driver_deinit(void)
{
	if (!g_enc.driver_opened) {
		return;
	}
	/* deinit sw encoder driver */
	extern void encoder_rtos_driver_deinit(void);
	encoder_rtos_driver_deinit();
	g_enc.driver_opened = 0;
}

int enc_control(void *p, int cmd, int arg)
{
	enc_ctx_t *ctx = (enc_ctx_t *)p;
	enc_context_t *enc_ctx = ctx->enc_cont;

	switch (cmd) {
	case CMD_ENC_SET_PARAMS: {
		enc_parameter_t *params = (enc_parameter_t *)arg;
		memcpy(&enc_ctx->enc_parm, params, sizeof(enc_parameter_t));
	}
	break;
	case CMD_ENC_GET_PARAMS: {
		enc_parameter_t *params = (enc_parameter_t *)arg;
		memcpy(params, &enc_ctx->enc_parm, sizeof(enc_parameter_t));
	}
	break;
	case CMD_ENC_SET_HEIGHT:
		enc_ctx->enc_parm.height = arg;
		break;
	case CMD_ENC_SET_WIDTH:
		enc_ctx->enc_parm.width = arg;
		break;
	case CMD_ENC_SET_FORCE_DROP_FRAME:
		enc_ctx->enc_parm.force_drop_frame = arg;
		break;
	case CMD_ENC_SET_GOPLEN:
		enc_set_gop(enc_ctx, arg);
		break;
	case CMD_ENC_SET_RCPARAM:
		enc_set_rate_control(enc_ctx, (rc_param_t *)arg);
		break;
	case CMD_ENC_GET_RCPARAM:
		enc_get_rate_control(enc_ctx, (rc_param_t *)arg);
		break;
	case CMD_ENC_SET_RC_EXT:
		enc_set_rate_control_ext(enc_ctx, (enc_rc_ext_t *)arg);
		break;
	case CMD_ENC_GET_RC_EXT:
		enc_get_rate_control_ext(enc_ctx, (enc_rc_ext_t *)arg);
		break;
	case CMD_ENC_SET_CODING_CTRL:
		enc_set_coding_control(enc_ctx, (enc_coding_t *)arg);
		break;
	case CMD_ENC_GET_CODING_CTRL:
		enc_get_coding_control(enc_ctx, (enc_coding_t *)arg);
		break;
	case CMD_ENC_SET_DEC_LUM_TABLE:
		enc_ctx->enc_parm.ispDecompLumTable = (void *)arg;
		break;
	case CMD_ENC_SET_DEC_CHR_TABLE:
		enc_ctx->enc_parm.ispDecompChrTable = (void *)arg;
		break;
	case CMD_ENC_SET_ISP_REMAP:
		enc_ctx->remap_param = (isp_remap_param_t *)arg;
		break;
	case CMD_ENC_SET_ENC_REMAP:
		enc_ctx->enc_remap_param = (enc_remap_param_t *)arg;
		break;
	case CMD_ENC_FORCE_IFRAME:
		enc_force_iframe(enc_ctx);
		break;
	case CMD_ENC_SET_OSD:
		return enc_set_osd(enc_ctx, (osd_overlay_t *)arg);
	case CMD_ENC_GET_OSD_SCENE_STATE:
		return enc_ctx->hevc_encoder && !enc_ctx->osd_scene_fault ? enc_ctx->osd_scene_owned : -1;
	case CMD_ENC_SET_OSD_SCENE: {
		video_osd_scene_t *scene = (video_osd_scene_t *)arg;
		if (!scene || !scene->completion || !enc_ctx->hevc_encoder ||
			enc_ctx->enc_parm.lowLatencyEnable ||
			enc_ctx->hevc_cfg.parallelCoreNum != 1 || enc_ctx->hevc_cfg.gopSize != 1 ||
			enc_ctx->hevc_cfg.pass || enc_ctx->hevc_cfg.lookaheadDepth ||
			(scene->codec != VIDEO_H264 && scene->codec != VIDEO_HEVC) ||
			scene->codec != enc_ctx->enc_parm.codec_fmt ||
			scene->count > VIDEO_OSD_SCENE_MAX_REGIONS || (scene->release && scene->count)) {
			return -1;
		}
		uint32_t indices = 0;
		for (uint32_t i = 0; i < scene->count; i++) {
			osd_overlay_t *o = &scene->overlays[i];
			if (o->osdOverlayIdx < 0 || o->osdOverlayIdx >= MAX_OVERLAY_NUM ||
				(indices & (1U << o->osdOverlayIdx)) || !o->osdOverlayEnable || !o->osdOverlayPicBuf ||
				(o->osdOverlayFormat != VIDEO_OSD_FORMAT_ARGB8888 && o->osdOverlayFormat != VIDEO_OSD_FORMAT_BITMAP) ||
				!o->osdOverlayWidth || !o->osdOverlayHeight ||
				o->osdOverlayWidth > enc_ctx->enc_parm.width || o->osdOverlayHeight > enc_ctx->enc_parm.height ||
				o->osdOverlayXoffset > enc_ctx->enc_parm.width - o->osdOverlayWidth ||
				o->osdOverlayYoffset > enc_ctx->enc_parm.height - o->osdOverlayHeight ||
				(o->osdOverlayXoffset & 1U) || (o->osdOverlayYoffset & 1U) ||
				(o->osdOverlayFormat == VIDEO_OSD_FORMAT_BITMAP && (o->osdOverlayWidth & 7U))) {
				return -1;
			}
			indices |= 1U << o->osdOverlayIdx;
		}
		if (!enc_ctx->hevc_osd_scene_q &&
			rtos_queue_create(&enc_ctx->hevc_osd_scene_q, 1, sizeof(*scene)) != SUCCESS) {
			return -1;
		}
		if (rtos_queue_message_waiting(enc_ctx->hevc_osd_update_q)) {
			return -1;
		}
		int owned = enc_ctx->osd_scene_owned;
		enc_ctx->osd_scene_owned = 1;
		if (rtos_queue_send(enc_ctx->hevc_osd_scene_q, scene, 0) != SUCCESS) {
			enc_ctx->osd_scene_owned = owned;
			return -1;
		}
		return 0;
	}
	case CMD_ENC_SET_MOSAIC:
		enc_set_mosaic(enc_ctx, (mosaic_overlay_t *)arg);
		break;
	case CMD_ENC_INIT_MEM_POOL:
		if (ctx->mem_pool) {
			break;
		}
		ctx->mem_pool = memory_init(enc_ctx->enc_parm.mem_info.mem_total_size, enc_ctx->enc_parm.mem_info.mem_block_size);//(4*1024*1024,512);
		if (ctx->mem_pool == NULL) {
			ENCODER_DBG_ERROR("error: fail to allocate encoder buffer\r\n");
			return -1;
		}
		break;
	case CMD_ENC_APPLY:
		if (!ctx->mem_pool) {
			ENCODER_DBG_ERROR("error: need CMD_ENC_INIT_MEM_POOL before apply encoder\n\r");
			return -1;
		}
		ENCODER_DBG_INFO("CMD_ENC_APPLY and do encoder open\n\r");
		if (enc_open(enc_ctx) < 0) {
			ENCODER_DBG_ERROR("error: enc_open fail\n\r");
			return -1;
		}
		break;
	case CMD_ENC_PAUSE:
		enc_ctx->pause = arg;
		break;
	case CMD_ENC_STOP:
		enc_ctx->stop = arg;
		break;
	case CMD_ENC_JPEG_SNAPSHOT_CB:
		enc_ctx->snapshot_cb = (enc_snapshot_t)arg;
		break;
	case CMD_ENC_JPEG_DO_SNAPSHOT:
		taskENTER_CRITICAL();
		if (!enc_ctx->snapshot_trigger) {
			enc_ctx->snapshot_idx = arg;
			enc_ctx->snapshot_trigger = 1;
		}
		taskEXIT_CRITICAL();
		break;
	case CMD_ENC_SET_PREPROCESS_CB:
		enc_ctx->preprocess_cb = (enc_preprocess_t)arg;
		if (enc_ctx->proc_img_buf) {
			free(enc_ctx->proc_img_buf);
			enc_ctx->proc_img_buf = NULL;
			enc_ctx->proc_img_buf_64aligned = NULL;
		}
		enc_ctx->proc_img_buf = (uint8_t *)malloc(enc_ctx->enc_parm.width * enc_ctx->enc_parm.height * 3 / 2 + 64);
		if (!enc_ctx->proc_img_buf) {
			ENCODER_DBG_ERROR("error: fail to allocate memory for processed image\n\r");
			return -1;
		}
		enc_ctx->proc_img_buf_64aligned = (uint8_t *)(((uint32_t)enc_ctx->proc_img_buf + 63) & ~((uint32_t)63));
		break;
	}
	return 0;
}

void *enc_destroy(void *p)
{
	enc_ctx_t *ctx = (enc_ctx_t *)p;

	if (ctx) {
		if (ctx->enc_cont) {
			enc_close(ctx->enc_cont);
			enc_ctx_deinit(ctx->enc_cont);
		}
		if (ctx->mem_pool) {
			memory_deinit(ctx->mem_pool);
		}
		free(ctx);

		if (g_enc.module_cnt > 0) {
			g_enc.module_cnt--;
		}
	}

	return NULL;
}

void *enc_create(void *parent)
{
	enc_ctx_t *ctx = malloc(sizeof(enc_ctx_t));
	if (!ctx) {
		return NULL;
	}
	memset(ctx, 0, sizeof(enc_ctx_t));
	ctx->parent = parent;

	ENCODER_DBG_INFO("Encoder ctx init\r\n");
	ctx->enc_cont = enc_ctx_init();
	if (!ctx->enc_cont) {
		goto enc_create_fail;
	}

	if (!g_enc.driver_opened) {
		enc_hw_init();
		if (enc_driver_init() < 0) {
			goto enc_create_fail;
		}
	}
	g_enc.module_cnt++;

	return ctx;

enc_create_fail:
	if (ctx) {
		if (ctx->enc_cont) {
			enc_ctx_deinit(ctx->enc_cont);
		}
		free(ctx);
	}
	return NULL;
}

void *enc_new_item(void *p)
{
	void *ptr;
	enc_ctx_t *ctx = (enc_ctx_t *)p;
	enc_context_t *enc_cont = ctx->enc_cont;

	ptr = memory_alloc(ctx->mem_pool, enc_cont->enc_parm.mem_info.mem_frame_size);
	if (!ptr) {
		ENCODER_DBG_ERROR("error: enc new item fail\r\n");
		if ((ConfigDebugMmfDebug & _MMF_DBG_ENCODER_)) {
			dump_info(ctx->mem_pool);
		}
	}

	return ptr;
}

void *enc_del_item(void *p, void *d)
{
	enc_ctx_t *ctx = (enc_ctx_t *)p;

	if (d) {
		memory_free(ctx->mem_pool, d);
	}
	return NULL;
}

void *enc_rsz_item(void *p, void *d, int len)
{
	enc_ctx_t *ctx = (enc_ctx_t *)p;

	ENCODER_DBG_DEBUG("enc resize item %d\n\r", len);
	return memory_realloc(ctx->mem_pool, d, len);
}

mm_module_t enc_module = {
	.create = enc_create,
	.destroy = enc_destroy,
	.control = enc_control,
	.handle = enc_handle,

	.new_item = enc_new_item,
	.del_item = enc_del_item,
	.rsz_item = enc_rsz_item,

	.output_type = MM_TYPE_VSINK,    // output for video sink
	.module_type = MM_TYPE_VDSP,     // module type is video algorithm
	.name = "ENC"
};
