#include "isp_mem.h"
#include <ameba_soc.h>

//#define DDR_START 0x6F000000
#define DDR_START 0x80000000

#define LDC_EN 0
#define MASK_EN 0

#define FIXP_FRAME_WIDTH 0x0800
#define FIXP_FRAME_HEIGHT 0x0804
#define FIXP_DUMMY_PIX 0x0808
#define FIXP_DUMMY_LINE 0x080c
#define FIXP_CTRL 0x0820
#define BLOCK_START_X 0x0828
#define BLOCK_START_Y 0x082c
#define BLOCK_WIDTH 0x0830
#define BLOCK_HEIGHT 0x0834
#define FIXP_VC_DELAY 0x0840

#define FIXP_START 0x100
#define FIXP_STOP 0x200
#define FIXP_VC_EN 0x80

#define POLL_MEM 0
#define BUF_RESET 1
#define CRC_DO 1
#define BY_CMD 1
#if 1
#define ispdbeg_printf(fmt, arg...)		DiagPrintf("[%s][%u][DEBUG]"fmt"\r",__func__,__LINE__, ##arg)
#define ispwarn_printf(fmt, arg...)		DiagPrintf("[%s][%u][WARNING]"fmt"\r",__func__,__LINE__, ##arg)
#define isperro_printf(fmt, arg...)		DiagPrintf("[%s][%u][ERROR]"fmt"\r",__func__,__LINE__, ##arg)
#define ispinfo_printf(fmt, arg...)		DiagPrintf("[%s][%u][INFO]"fmt"\r",__func__,__LINE__, ##arg)
#define isp_printf(fmt, arg...)		    DiagPrintf("[%s][%u]"fmt"\r",__func__,__LINE__, ##arg)
//#define ispdbeg_printf(fmt, arg...)		RTK_LOGS(NOTAG, RTK_LOG_DEBUG,  "[DEBUG]"fmt"\r", ##arg)
//#define ispwarn_printf(fmt, arg...)		RTK_LOGS(NOTAG, RTK_LOG_WARN ,  "[WARNING]"fmt"\r", ##arg)
//#define isperro_printf(fmt, arg...)		RTK_LOGS(NOTAG, RTK_LOG_ERROR,  "[ERROR]"fmt"\r", ##arg)
//#define ispinfo_printf(fmt, arg...)		RTK_LOGS(NOTAG, RTK_LOG_INFO,   "[INFO]"fmt"\r", ##arg)
//#define isp_printf(fmt, arg...)		    RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, ""fmt"\r", ##arg)
#else
#define ispdbeg_printf(fmt, arg...)
#define ispwarn_printf(fmt, arg...)
#define isperro_printf(fmt, arg...)
#define ispinfo_printf(fmt, arg...)
#define isp_printf(fmt, arg...)
#endif

enum isp_io_type {
	ISP_INPUT_CROP_L,
	ISP_INPUT_CROP_S,
	ISP_INPUT_TPNR_IN,
	ISP_INPUT_FUSION_IN,
	ISP_INPUT_FUSION_OUT,
	ISP_INPUT_RAW_BACKEND,
	ISP_INPUT_RAW_FINAL,
	ISP_INPUT_RGB,
	ISP_INPUT_YUV,
	ISP_INPUT_YUYV,
	ISP_INPUT_NV16,
	/* SYSCTRL_L1A_IMGO_L1_SEL */
	ISP_OUTPUT_CROP_OUT_L,     /*  2 */
	ISP_OUTPUT_CROP_OUT_S,     /*XXXX*/
	ISP_OUTPUT_TPNR_OUT,       /*  4 */
	ISP_OUTPUT_FUSION_IN_L,    /*  6 */
	ISP_OUTPUT_FUSION_IN_S,    /*XXXX*/
	ISP_OUTPUT_FUSION_IN_HDR,  /*XXXX*/
	ISP_OUTPUT_FUSION_OUT,     /*  8 */
	ISP_OUTPUT_NLSC_IN,        /*  9 */
	ISP_OUTPUT_AWB_GAIN_OUT,   /* 10 */
	ISP_OUTPUT_TM_IN,          /* 11 */
	ISP_OUTPUT_GTM_OUT,        /*XXXX*/
	ISP_OUTPUT_LTM_OUT,        /* 13 */
	ISP_OUTPUT_INTP_IN,        /* 14 */
	ISP_OUTPUT_RGB,            /* 15 */
	ISP_OUTPUT_YUV,            /* 16 */
	ISP_OUTPUT_YUYV,           /* 20 */
};

enum dbg_path {
	RTS_RAW_SRC_DISABLE,
	RTS_RAW_SRC_BLC_LONGEXP_B,
	RTS_RAW_SRC_BLC_LONGEXP_A,
	RTS_RAW_SRC_DNS_LONGEXP_A,
	RTS_RAW_SRC_DPC_LONGEXP_A,
	RTS_RAW_SRC_AE_B,
	RTS_RAW_SRC_R_CCM_B,
	RTS_RAW_SRC_G_CCM_B,
	RTS_RAW_SRC_B_CCM_B,
};

#define SLOT_N 4
#define STREAM_N 5
struct isp_io {
	void *base;

	size_t width;
	size_t height;
	int in_type;
	int out_type;

	size_t in_y_addr;
	size_t in_uv_addr;
	size_t out_y_addr[STREAM_N - 1][SLOT_N];
	size_t out_uv_addr[STREAM_N - 1][SLOT_N];
	size_t out_rgb_addr[3][SLOT_N];
	size_t in_y_size;
	size_t in_uv_size;
	size_t out_y_size[STREAM_N - 1];
	size_t out_uv_size[STREAM_N - 1];
	size_t out_rgb_size;

	struct rtsc_dma_buffer in_buf;
	struct rtsc_dma_buffer out_buf;
	struct rtsc_dma_buffer tnr_buf;

	uint32_t initialized : 1;
// add zoom 2024.0925
	uint8_t zoom_en;
// add zoom 2024.0925

	enum isp_io_type g_input_type;
	enum isp_io_type g_output_type;
	int save_output  ;
	int zoomout_nv16 ;
	int crop_w       ;
	int crop_h       ;
	int zoom_w[STREAM_N];
	int zoom_h[STREAM_N];
	int pattern_input;
	int dbg_path     ;
	int fixp_en      ;
	int modules_en   ;
	int vhdr_en      ;
	int mempool_start;
	int mempool_size ;
	int aligned_w[STREAM_N];
	int remap_fps    ;
	int low_latency_mode;
	int cmpr_fr_mode;
	int wait_fd_round;
	int wait_fd_delay;
	int mask_en;
	int ldc_en;
};
