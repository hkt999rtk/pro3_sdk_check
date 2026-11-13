#ifndef _ISP_MAIN_H_INC_
#define _ISP_MAIN_H_INC_

//#include "isp_mem.h"
//#include "isp_hal.h"
#include "rtsc.h"
#include "FreeRTOS.h"
#include "queue.h"

#if defined(REALSENSOR) && (REALSENSOR==1)
#define FIXP_STREAM 0
#else
#define FIXP_STREAM 1
#endif

#define CONSUMER_BYPASS 1
#define RTOS_SB_TIMEOUT 500

#define buffer_count 2
#define stream_count 5
#define STREAM_CNT_MAX 5
#define BUFFER_CNT_MAX 4

#define stream0 0
#define stream1 1
#define stream2 2
#define stream3 3
#define frame0 0
#define frame1 1

#define ISP_FORMAT_YUV420_SEMIPLANAR 0x01
#define ISP_FORMAT_YUV422_SEMIPLANAR 0x02
#define ISP_FORMAT_YUY2_INTERLEAVED  0x03       //YUYVYUYV
#define ISP_FORMAT_BAYER_PATTERN     0x04
#define ISP_FORMAT_RGB888_PLANAR     0x05

#define ISP_TUNING_MODE 0
#if ISP_TUNING_MODE
#define RAW_SIZE_FACTOR 3
#else
#define RAW_SIZE_FACTOR 1
#endif

#define ISP_LOG 0

#define isperro_printf(fmt, arg...)		DiagPrintf("[%s][%u][ERROR]"fmt"\r",__func__,__LINE__, ##arg)

#if ISP_LOG
#define ispdbeg_printf(fmt, arg...)		DiagPrintf("[%s][%u][DEBUG]"fmt"\r",__func__,__LINE__, ##arg)
#define ispwarn_printf(fmt, arg...)		DiagPrintf("[%s][%u][WARNING]"fmt"\r",__func__,__LINE__, ##arg)
#define ispinfo_printf(fmt, arg...)		DiagPrintf("[%s][%u][INFO]"fmt"\r",__func__,__LINE__, ##arg)
#define isp_printf(fmt, arg...)		    DiagPrintf("[%s][%u]"fmt"\r",__func__,__LINE__, ##arg)
//#define ispdbeg_printf(fmt, arg...)		RTK_LOGS(NOTAG, RTK_LOG_DEBUG,  "[DEBUG]"fmt"\r", ##arg)
//#define ispwarn_printf(fmt, arg...)		RTK_LOGS(NOTAG, RTK_LOG_WARN ,  "[WARNING]"fmt"\r", ##arg)
//#define ispinfo_printf(fmt, arg...)		RTK_LOGS(NOTAG, RTK_LOG_INFO,   "[INFO]"fmt"\r", ##arg)
//#define isp_printf(fmt, arg...)		    RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, ""fmt"\r", ##arg)
#else
#define ispdbeg_printf(fmt, arg...)
#define ispwarn_printf(fmt, arg...)
#define ispinfo_printf(fmt, arg...)
#define isp_printf(fmt, arg...)
#endif

// #define DEBUG_MODE
#ifdef DEBUG_MODE
#define RTS_DEBUG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define RTS_DEBUG(fmt, ...)
#endif

enum out_fmt_type {
	FMT_NV12,
	FMT_NV16,
	FMT_YUYV,
	FMT_RGB,
};


struct zoom_config {
	int enable;
	int out_fmt;
	uint32_t left;
	uint32_t top;
	uint32_t right;
	uint32_t bottom;
	uint32_t width;
	uint32_t height;
	uint32_t align_width;
};

struct io_buffer {
	size_t out_y_addr;
	size_t out_uv_addr;
	size_t out_y_size;
	size_t out_uv_size;

	size_t out_r_addr;
	size_t out_g_addr;
	size_t out_b_addr;
	size_t out_rgb_size;

	struct rtsc_dma_buffer out_buf;
};

struct isp_io {
	void *base;
	size_t in_y_addr;
	size_t in_uv_addr;
	size_t out_y_addr[4][4];
	size_t out_uv_addr[4][4];
	size_t out_rgb_addr[3][4];
	size_t in_y_size;
	size_t in_uv_size;
	size_t out_y_size[4];
	size_t out_uv_size[4];
	size_t out_rgb_size;

	int crop_w;
	int crop_h;
	int zoom_w[4];
	int zoom_h[4];
	int vhdr_en;
	int cmpr_fr_mode;
	int mask_en;
	int ldc_en;
	size_t width;
	size_t height;

	struct io_buffer io_buffer_ISP[STREAM_CNT_MAX][BUFFER_CNT_MAX];
	struct io_buffer *io_buffers[STREAM_CNT_MAX];
	int aligned_w[STREAM_CNT_MAX];
	int remap_fps;
	int low_latency_mode;
	int fixp_hts_ratio_2base;
	int cmpr_fr_stm_id;
	uint32_t initialized : 1;
};

void isp_buf_release(int stream_id, void *buf_addr, int buf_id);
int isp_initial(struct isp_io *io);

#endif /* _ISP_MAIN_H_INC_ */
