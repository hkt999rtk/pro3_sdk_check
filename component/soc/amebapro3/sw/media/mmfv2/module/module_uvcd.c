/******************************************************************************
*
* Copyright(c) 2007 - 2018 Realtek Corporation. All rights reserved.
*
******************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include "mmf2_module.h"
#include "module_uvcd.h"
#include "usbd.h"
#include "usbd_uvc.h"
#include "avcodec.h"

#define USBD_UVC_ISO_IN_EP    0x83U

#define num                   1

/* NAL unit types, Ref H.264 Table 7-1 and H.265 Table 7-1 */
#define UVCD_H264_NAL_TYPE(b)       ((b) & 0x1FU)
#define UVCD_H264_NAL_SLICE         1U
#define UVCD_H264_NAL_IDR           5U
#define UVCD_HEVC_NAL_TYPE(b)       (((b) >> 1) & 0x3FU)
#define UVCD_HEVC_NAL_VCL_MAX       31U
#define UVCD_HEVC_NAL_IRAP_MIN      16U   /* BLA_W_LP */
#define UVCD_HEVC_NAL_IRAP_MAX      21U   /* CRA_NUT */

/* Upper bound on frames dropped while waiting for a detectable I frame (~5s at 30fps).
   Open-GOP/intra-refresh encoding or recovery-point-SEI-only streams never emit an IDR/IRAP
   slice, so without this limit uvcd_wait_iframe would never clear and the host would be stuck
   on a black screen forever. */
#define UVCD_WAIT_IFRAME_MAX_DROP   150U

static const char *const TAG = "UVCD_MODULE";

static usbd_uvc_buffer_t uvc_payload[num];
static usbd_uvc_format_t *uvc_format_ptr = NULL;
/* The host decoder cannot start from a P frame, so drop H.264/HEVC frames until an I frame */
static volatile u8 uvcd_wait_iframe = 1U;
static u16 uvcd_wait_iframe_drop_cnt = 0U;

/* Return 1 if the first slice of the H.264/HEVC access unit is an I frame (H.264 IDR, HEVC IRAP) */
static int uvcd_is_iframe(const u8 *buf, u32 size, u32 codec)
{
	u32 i;
	u8 type;

	for (i = 0U; (i + 3U) < size; i++) {
		if ((buf[i] != 0U) || (buf[i + 1U] != 0U) || (buf[i + 2U] != 1U)) {
			continue;
		}
		/* SPS/PPS/VPS/SEI come before the first slice, only that slice tells the frame type */
		if (codec == AV_CODEC_ID_H264) {
			type = UVCD_H264_NAL_TYPE(buf[i + 3U]);
			if ((type >= UVCD_H264_NAL_SLICE) && (type <= UVCD_H264_NAL_IDR)) {
				return (type == UVCD_H264_NAL_IDR) ? 1 : 0;
			}
		} else {
			type = UVCD_HEVC_NAL_TYPE(buf[i + 3U]);
			if (type <= UVCD_HEVC_NAL_VCL_MAX) {
				return ((type >= UVCD_HEVC_NAL_IRAP_MIN) && (type <= UVCD_HEVC_NAL_IRAP_MAX)) ? 1 : 0;
			}
		}
		i += 2U;
	}
	return 0;
}

static const usbd_uvc_ep_cfg_t uvc_ep = {
	.iso_in_addr = USBD_UVC_ISO_IN_EP,
};

static const usbd_config_t uvcd_cfg = {
	.info = {
		.prod_str = "Realtek UVC",
	},
	.isr_priority = INT_PRI_MIDDLE,
	/* Enable SOF interrupt: drives ISOC IN arming/recovery and the SCR SOF counter. */
	.ext_intr_enable = USBD_SOF_INTR,
#ifdef CONFIG_SUPPORT_USB_SHARED_DFIFO
	.intr_use_ptx_fifo  = 0U,
#endif
#if defined (CONFIG_AMEBAPRO3)
	//DFIFO total 2232 DWORD, resv 8 DWORD for DMA addr and EP0 fixed 256 DWORD
	//ptx_fifo_depth[i] configures dedicated TX FIFO #(i+1), not endpoint (i+1)
	//directly: usbd_hal_get_tx_fifo_num() remaps EP3 -> FIFO#2 and EP5 -> FIFO#3
	//on this core (EP1/EP3/EP5 are the only IN-capable endpoints; EP2/EP4 are
	//OUT-only by hardware design and can't be used for ISOC IN at all). So
	//index1 (FIFO#2, hardware-fixed at 768 DWORD = 3072B) is what actually
	//backs USBD_UVC_ISO_IN_EP=EP3 (see usbd_uvc.h), not index2. EP1 (interrupt
	//status, 64B MPS) and FIFO#3 (unused - only EP5 maps there) only need a
	//minimal FIFO.
	.rx_fifo_depth = 1168U,
	.ptx_fifo_depth = {16U, 768U, 16U, },
#endif
};

void uvcd_change_format_resolution(void *ctx)
{
	(void)ctx;
	RTK_LOGS(TAG, RTK_LOG_INFO, "uvcd_change_format_resolution\r\n");
	uvcd_wait_iframe = 1U;
	uvcd_wait_iframe_drop_cnt = 0U;
	usb_os_sema_give(uvc_format_ptr->uvcd_change_sema);
}

void *uvcd_create(void *parent)
{
	(void)parent;
	int i = 0;

	int ret = HAL_OK;

	ret = usbd_init(&uvcd_cfg);

	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "usbd uvc init error\r\n");
		goto exit_usbd_init_fail;
	}

	uvcd_ctx_t *uvc_ctx = malloc(sizeof(uvcd_ctx_t));

	if (usbd_uvc_init(&uvc_ep) < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "usbd uvc init error\r\n");
		goto exit_usbd_uvc_init_fail;
	}
	uvc_format_ptr = usbd_uvc_get_format();

	for (i = 0; i < num; i++) {
		INIT_LIST_HEAD(&uvc_payload[i].buffer_list);
		usbd_uvc_video_put_out_stream_queue(&uvc_payload[i]);
	}

	usbd_uvc_set_change_parm_cb((int)uvcd_change_format_resolution);

	return (void *)uvc_ctx;
exit_usbd_uvc_init_fail:
	usbd_deinit();
	usbd_uvc_deinit();
	if (uvc_ctx) {
		free(uvc_ctx);
	}
exit_usbd_init_fail:
	return NULL;
}

int uvcd_handle(void *p, void *input, void *output)
{
	(void)p;
	(void)output;
	int ret = 0;

	mm_queue_item_t *input_item = (mm_queue_item_t *)input;

	usbd_uvc_buffer_t *payload = NULL;

	extern int usbd_uvc_get_status(void);
	if (usbd_uvc_get_status() == 0) {
		uvcd_wait_iframe = 1U;
		uvcd_wait_iframe_drop_cnt = 0U;
		goto end;
	}

	if ((uvcd_wait_iframe != 0U) && ((input_item->type == AV_CODEC_ID_H264) || (input_item->type == AV_CODEC_ID_H265))) {
		if (uvcd_is_iframe((const u8 *)input_item->data_addr, input_item->size, input_item->type) == 0) {
			uvcd_wait_iframe_drop_cnt++;
			/* No detectable IDR/IRAP within the budget (open-GOP/intra-refresh encoding, or
			   recovery-point-SEI-only streams never emit one): give up gating and let frames
			   through, else the host would be stuck on a black screen forever. */
			if (uvcd_wait_iframe_drop_cnt < UVCD_WAIT_IFRAME_MAX_DROP) {
				goto end;
			}
			RTK_LOGS(TAG, RTK_LOG_WARN, "No I frame after %d drops, forcing passthrough\n", uvcd_wait_iframe_drop_cnt);
		}
		uvcd_wait_iframe = 0U;
		uvcd_wait_iframe_drop_cnt = 0U;
		RTK_LOGS(TAG, RTK_LOG_INFO, "First I frame %s\n", (input_item->type == AV_CODEC_ID_H264) ? "H264" : "HEVC");
	}

	do {
		payload = usbd_uvc_video_out_stream_queue();
		if (payload == NULL) {
			/* Streaming stopped (host SET_INTERFACE alt=0) or usbd_uvc_deinit() ran: deinit
			   clears cdev->running before wiping output_queue, so nothing will ever be put
			   back into it and this would otherwise spin forever on a buffer that never
			   reappears, then go on to use input_lock after deinit has deleted it. */
			if (usbd_uvc_get_status() == 0) {
				uvcd_wait_iframe = 1U;
				uvcd_wait_iframe_drop_cnt = 0U;
				goto end;
			}
			//RTK_LOGS(TAG, RTK_LOG_ERROR,"NULL\r\n");
			vTaskDelay(1);
		}
	} while (payload == NULL);

	payload->mem = (unsigned char *) input_item->data_addr;
	payload->bytesused = input_item->size;
	usbd_uvc_video_put_in_stream_queue(payload);
	extern void usbd_uvc_wait_frame_down(void);
	usbd_uvc_wait_frame_down();

end:
	return ret;
}

int uvcd_control(void *p, int cmd, int arg)
{
	(void)arg;
	usbd_uvc_dev_t *uvc_ctx = (usbd_uvc_dev_t *)p;
	(void)uvc_ctx;
	switch (cmd) {
	case CMD_UVCD_CALLBACK_SET://Prot A and B
		break;
	case CMD_UVCD_CALLBACK_GET:
		break;
	case CMD_UVCD_STOP:
		break;
	}
	return 0;
}

void *uvcd_destroy(void *p)
{
	/* Release the USB device so the host sees it detach and a later open can init it again */
	usbd_uvc_deinit();
	usbd_deinit();
	uvc_format_ptr = NULL;
	free(p);
	return NULL;
}

mm_module_t uvcd_module = {
	.create = uvcd_create,
	.destroy = uvcd_destroy,
	.control = uvcd_control,
	.handle = uvcd_handle,

	.new_item = NULL,
	.del_item = NULL,

	.output_type = MM_TYPE_NONE,     // no output
	.module_type = MM_TYPE_VSINK,    // module type is video algorithm
	.name = "UVCD"
};
