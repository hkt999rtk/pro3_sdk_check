/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef __ADDREMAP_SYS_API_H__
#define __ADDREMAP_SYS_API_H__

#include "semphr.h"



#define ISP_WID         	0x3
#define RMP_DDR_WID_0   	0x0
#define RMP_DDR_WID_1   	0x1
#define RMP_DDR_WID_2   	0x2
#define RMP_SRAM_WID_0  	0x4
#define RMP_SRAM_WID_1  	0x5
#define RMP_SRAM_WID_2		0x6
#define RMP_SRAM_HEADER		0x7
#define RMP_HDR_SRAM_W		0x31
#define RMP_HDR_SRAM_R		0x32
#define RMP_HDR_DRAM_RW		0x33

#define VIDEO_SRAM_SIZE 	196608  //192KB

#define HEADER_FLUSH_BUFFER 	96
#define HEADER_SLOT				2

#define CMPR_HEADER_ALIGN		32
#define CMPR_HDR_START_ALIGN	1024

typedef struct rmp_isp_ctx_s {
	rtos_sema_t xrmp_isp_Semaphore;
	int sema_timeout_ms;
	int IRQ_priority;
	unsigned int slice_th_cnt;
	unsigned int sliceEOF_cnt;
	unsigned int rmp_isp_err_cnt;
	unsigned int rmp_isp_err_value;
	unsigned int HDREOF_cnt;

} rmp_isp_ctx_t;

typedef struct rmp_enc_ctx_s {
	rtos_sema_t xrmp_enc_Semaphore;
	int sema_timeout_ms;
	int IRQ_priority;
	unsigned int rmp_enc_err_cnt;
	unsigned int rmp_enc_err_value;
} rmp_enc_ctx_t;




typedef struct remap_drv_ctx_s {

	rmp_isp_ctx_t rmp_isp_ctx;
	rmp_enc_ctx_t rmp_enc_ctx;
	remap_intf_ctx_t rmp_intf;
} remap_drv_ctx_t;



typedef enum {
	RMPDRV_CTRL_SET_TS_PARA		,
	RMPDRV_CTRL_SET_FCMPR_PARA,
	RMPDRV_CTRL_SET_SWRAP_PARA,
	RMPDRV_CTRL_SET_HDRWP_PARA,
	RMPDRV_CTRL_PRE_PROC		,
	RMPDRV_CTRL_RUN				,
	RMPDRV_CTRL_GET_ENC_CFG		,
	RMPDRV_CTRL_GET_ENC_TS_CFG	,
} remap_ctrl_cmd_t;


typedef enum {
	REMAP_BURST_64		,
	REMAP_BURST_128		,
	REMAP_BURST_256 	,
	REMAP_BURST_512		,
} remap_burst_size_e;


typedef struct remap_enc_data_s {

	unsigned int input_data_width;
	unsigned int input_data_height;
	unsigned int output_data_width;
	unsigned int output_data_height;

	unsigned int busLuma;
	unsigned int busChromaU;

	unsigned int exp_of_input_alignment;

	unsigned int dec400LumTableBase[2];
	unsigned int dec400CbTableBase[2];
	unsigned int ufbcCompAlign;
	unsigned char decEnable;

	unsigned char wrap_num_to_enc;
	unsigned char low_latency_set;
	unsigned char timeout_disable;


} remap_enc_data_t;

typedef struct ts_remap_para_s {

	unsigned int tile_wrap_num;

	unsigned int ts_wrap_vsram_addr;
	unsigned int ts_wrap_vsram_size;

	unsigned int ts_enc_in_fake_addr;
	unsigned int ts_out_w;
	unsigned int ts_out_h;

	unsigned char hevc_enc_type;

} ts_remap_para_t;


typedef struct fr_cmpr_para_s {

	unsigned int tile_wrap_num;

	unsigned int ts_wrap_vsram_addr;
	unsigned int ts_wrap_vsram_size;

	unsigned int ts_enc_in_fake_addr;
	unsigned int ts_out_w;
	unsigned int ts_out_h;

	unsigned char hevc_enc_type;

} fr_cmpr_para_t;


typedef struct slice_wrap_para_s {

	unsigned int slice_wrap_num;
	remap_burst_size_e slice_wrap_burst_size;
	unsigned int slice_wrap_num_th;

	unsigned int width;
	unsigned int height;

	unsigned int slice_wrap_vsram_addr;
	unsigned int slice_wrap_vsram_size;

	unsigned int isp_out_y_addr;
	unsigned int isp_out_uv_addr;
	unsigned int isp_out_y_size;
	unsigned int isp_out_uv_size;
	unsigned int isp_out_y_addr_slot2;
	unsigned int isp_out_uv_addr_slot2;

	unsigned char CompEnb;
	unsigned char SWrapEnb;
	unsigned char RTTEnb;
	unsigned char PaREnb;

	unsigned char hevc_enc_type;
	unsigned char enc_timeout_disable;



} slice_wrap_para_t;


typedef struct hdr_wrap_para_s {

	unsigned int tile_wrap_num;

	unsigned int ts_wrap_vsram_addr;
	unsigned int ts_wrap_vsram_size;

	unsigned int ts_enc_in_fake_addr;
	unsigned int ts_out_w;
	unsigned int ts_out_h;

	unsigned char hevc_enc_type;

} hdr_wrap_para_t;



typedef struct remap_intf_ctx_s {

	unsigned char HdrEnb;
	unsigned char CompEnb;
	unsigned char SWrapEnb;
	unsigned char ParSWrapEnb;
	unsigned char RTTEnb;
	unsigned char PaREnb;

	unsigned char hevc_enc_type;
	unsigned char enc_timeout_disable;

	unsigned int slice_wrap_num;
	remap_burst_size_e slice_wrap_burst_size;
	unsigned int slice_wrap_num_th;
	unsigned int slice_height;

	unsigned int width;
	unsigned int height;
	unsigned int slice_wrap_align_width;

	unsigned int slice_wrap_vsram_addr;
	unsigned int slice_wrap_vsram_size;

	unsigned int slice_out_addr_cmp0;
	unsigned int slice_out_addr_cmp1;

	remap_burst_size_e hdr_remap_burst_size;
	unsigned int hdr_width;
	unsigned int hdr_height;
	unsigned int hdr_isp_buff_size;
	unsigned int hdr_remap_buf_addr_base;
	unsigned int hdr_remap_buf_size;
	unsigned int hdr_remap_denominator;

	unsigned int mem_for_cmpr_header;

	unsigned int isp_out_y_addr;
	unsigned int isp_out_uv_addr;
	unsigned int isp_out_y_size;
	unsigned int isp_out_uv_size;
	unsigned int isp_out_y_addr_slot2;
	unsigned int isp_out_uv_addr_slot2;

	unsigned int cmpr_header_y_addr[2];
	unsigned int cmpr_header_uv_addr[2];

	unsigned char cmpr_fr_mode;
	unsigned char slice_wrap_at_vsram;

	unsigned int tile_wrap_num;
	unsigned int tile_w;
	unsigned int tile_h;

	unsigned int ts_wrap_vsram_addr;
	unsigned int ts_wrap_vsram_size;

	unsigned int ts_enc_in_fake_addr;
	unsigned int ts_out_w;
	unsigned int ts_out_h;
	unsigned int ts_out_align_w;

} remap_intf_ctx_t;


remap_drv_ctx_t *remap_open(void);
void remap_close(remap_drv_ctx_t *ctx);
int remap_ctrl(remap_drv_ctx_t *ctx, remap_ctrl_cmd_t ctrl_cmd, void *arg);


#endif /* __ADDREMAP_SYS_API_H__ */

