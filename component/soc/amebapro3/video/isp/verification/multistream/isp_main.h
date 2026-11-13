#ifndef _ISP_MAIN_H_INC_
#define _ISP_MAIN_H_INC_

#include "isp_mem.h"
#include "isp_hal.h"
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

#define stream0 0
#define stream1 1
#define stream2 2
#define stream3 3
#define frame0 0
#define frame1 1

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

/* color */
#define NONE         "\033[m"
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"

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

	struct rtsc_dma_buffer in_buf;
	struct rtsc_dma_buffer out_buf;
	struct rtsc_dma_buffer tnr_buf;

	// add zoom 2024.0925
	uint8_t zoom_en;
	// add zoom 2024.0925

	int g_input_type;
	int g_output_type;
	int save_output  ;
	int zoomout_nv16 ;
	int crop_w		 ;
	int crop_h		 ;
	int zoom_w[4];
	int zoom_h[4];
	int pattern_input;
	int dbg_path	 ;
	int fixp_en 	 ;
	int modules_en	 ;
	int vhdr_en 	 ;
	int mempool_start;
	int mempool_size ;
	//int aligned_w[4];
	//int remap_fps	 ;
	//int low_latency_mode;
	int cmpr_fr_mode;
	int wait_fd_round;
	int wait_fd_delay;
	int mask_en;
	int ldc_en;
	size_t width;
	size_t height;
	int out_type;

	struct io_buffer io_buffer_ISP[stream_count][buffer_count];
	struct io_buffer *io_buffers[stream_count];
	int aligned_w[stream_count];
	int remap_fps;
	int low_latency_mode;
	int fixp_hts_ratio_2base;
	int cmpr_fr_stm_id;
	uint32_t initialized : 1;
};


typedef enum thread_status {
	thread_is_available,
	thread_is_unavailable
} thread_status_t;

#if !FIXP_STREAM
struct isp_pipe {
	QueueHandle_t producerQueue;
	QueueHandle_t consumerQueue;
};
#endif

struct stream_verify_mode {
	uint8_t mode;
	uint8_t crc_en;
	uint8_t vhist_en;
	uint8_t specify_fm_en;
	uint32_t specify_fm;
};


int isp_init(struct isp_io *io);
//void isp_release(struct isp_io *io);
void isp_stop(struct isp_io *io);
//int isp_alloc_buffer(struct isp_io *io, size_t width, size_t height,
//                     int out_type, struct zoom_config *zoom);
void isp_start(struct isp_io *io);

//void *isp_write_input_begin(struct isp_io *io, int index);
//void isp_write_input_finish(struct isp_io *io, int index);
//void isp_read_output_finish(struct isp_io *io, int stream_index,
//                            int frame_index);
//void *isp_read_output_begin(struct isp_io *io, int stream_index,
//                            int frame_index);
int isp_streaming(struct isp_io *io, size_t width, size_t height, int out_type,
				  struct zoom_config *zoom, void *priv);
//int isp_wait_completion_multi(struct isp_io *io, int stream_index);

//int producer_write_message(int *message, int stream_index);
//int producer_read_message(int *message, int stream_index);
//int send_buffer(struct isp_io *io, int stream_index, int frame_index);
//int release_buffer(struct isp_io *io, int stream_index, int frame_index);
void isp_buf_release(int stream_id, void *buf_addr, int buf_id);
struct stream_verify_mode *stream_verify_get_mode(void);

#endif /* _ISP_MAIN_H_INC_ */
