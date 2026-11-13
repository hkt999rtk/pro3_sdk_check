/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef _RTS_RAW_H_
#define _RTS_RAW_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief source of raw data (tap point in the ISP pipeline)
 */
enum rts_raw_src {
	RTS_RAW_SRC_DISABLE = 0, /**< disable raw */
	RTS_RAW_SRC_S_CROP_A = 0x1, /**< raw data after crop for short exp */
	RTS_RAW_SRC_L_CROP_A = 0x2, /**< raw data after crop for long exp */
	RTS_RAW_SRC_S_BLC_B = 0x3, /**< raw data before BLC for short exp */
	RTS_RAW_SRC_L_BLC_B = 0x4, /**< raw data before BLC for long exp */
	RTS_RAW_SRC_HDR_A = 0x5, /**< raw data after HDR fusion */
	RTS_RAW_SRC_GTM_A = 0x6, /**< raw data after global tone mapping */
	RTS_RAW_SRC_LTM_A = 0x7, /**< raw data after local tone mapping */
	RTS_RAW_SRC_CCM_B = 0x8, /**< raw data before CCM */
	RTS_RAW_SRC_UVT_B = 0x9, /**< raw data before UV tune */
	RTS_RAW_SRC_L_FUSION_B = 0xA, /**< raw data before fusion for long exp */
	RTS_RAW_SRC_S_FUSION_B = 0xB, /**< raw data before fusion for short exp */
	RTS_RAW_SRC_FUSION_B = 0xC, /**< raw data before fusion */
	RTS_RAW_SRC_AEGAIN_A = 0xD, /**< raw data after ae gain */
	RTS_RAW_SRC_WDR_A = 0xE, /**< raw data after WDR */
	RTS_RAW_SRC_YUV444TO422_B = 0xF, /**< raw data before yuv444to422 */
	RTS_RAW_SRC_EEH_A = 0x10, /**< raw data after eeh */
	RTS_RAW_SRC_MASK_A = 0x11, /**< raw data after mask */
	RTS_RAW_SRC_AWBGAIN_B = 0x12, /**< raw data before awb gain */
	RTS_RAW_SRC_3DNR_B = 0x13, /**< raw data before 3DNR */
	RTS_RAW_SRC_LCE_A = 0x14, /**< raw data after LCE */
	RTS_RAW_SRC_RESERVED
};

/**
 * @brief Query the sensor/ISP-middle-stage image size and bit depth
 * @param[out] w width
 * @param[out] h height
 * @param[out] fmt sensor bit depth, see \ref rts_isp_sensor_bit_depth
 * @return 0 : success, minus : fail
 */
int rts_hal_get_snr_fmt(uint32_t *w, uint32_t *h, uint32_t *fmt);
/**
 * @brief Select which pipeline stage a stream's raw output taps from
 * @param[in] streamid stream id (currently unused: one shared ISP0 tap point)
 * @param[in] fmt source of raw data, see \ref rts_raw_src
 * @return 0 : success, minus : fail
 */
int rts_hal_set_raw(uint8_t streamid, uint32_t fmt);
/**
 * @brief Bypass the ISP stages downstream of the selected raw tap point
 * @param[in] streamid stream id (currently unused: one shared ISP0 tap point)
 * @param[in] fmt source of raw data, see \ref rts_raw_src
 * @return 0 : success, minus : fail
 */
int rts_hal_set_bypass(uint8_t streamid, uint32_t fmt);
/**
 * @brief Hook run before stream-on; no-op on this platform
 */
int rts_hal_pre_streamon(void);
/**
 * @brief Hook run after stream-off; no-op on this platform
 */
int rts_hal_post_streamoff(void);

/**
 * @brief Get the raw source last applied via rts_hal_set_raw
 * @return current \ref rts_raw_src value, RTS_RAW_SRC_DISABLE if never set
 */
int rts_raw_get_current_src(void);

/*
 * --------------------------------------------------------------------------
 * Raw capture format descriptor (pro3 port of Linux rtsraw's set_raw_fmt()/
 * make_raw_header()). Scope is intentionally limited to ISP driver
 * integration (rts_hal_get_snr_fmt/rts_hal_set_raw/rts_hal_set_bypass): it
 * only knows the per-tap-point byte layout and how to switch the ISP tap
 * point. It owns no video-channel/buffer/streaming state or 2A (AE/AWB)
 * control - callers (e.g. mmf2_video_example_dbg_raw.c) open the channel
 * with praw->fmt/width/height and allocate buffers sized praw->length via
 * rtscam_video_*, on their own. Only channel 0 supports raw tap-off.
 * --------------------------------------------------------------------------
 */

#define RTS_RAW_HEADER_LENGTH 128

/**
 * @brief format of sensor raw data
 */
enum rts_raw_fmt {
	RTS_RAW_FMT_RAW16 = 16, /**< raw16 */
	RTS_RAW_FMT_RAW32 = 32, /**< raw32 */
	RTS_RAW_FMT_RAW128 = 128, /**< raw128 */
	RTS_RAW_FMT_RESERVED
};
/**
 * @brief format of image file
 */
enum rts_file_fmt {
	RTS_FILE_UNKNOWN = 0x0,
	RTS_FILE_BIN = 0x1,
};
/**
 * @brief data pattern
 */
enum rts_data_pattern {
	RTS_DATA_PATTERN_UNKNOWN = 0x0,
	RTS_DATA_PATTERN_RAW_GRBG = 0x1,
	RTS_DATA_PATTERN_HDR = 0x21,
	RTS_DATA_PATTERN_RGB = 0x41,
	RTS_DATA_PATTERN_YUV444 = 0x81,
	RTS_DATA_PATTERN_YUV422 = 0x82,
};
/**
 * @brief data type
 */
enum rts_data_type {
	RTS_DATA_TYPE_UNKNOWN = 0x0,
	RTS_DATA_TYPE_HDR_INTERLEAVED = 0x21,
	RTS_DATA_TYPE_RGB = 0x41,
	RTS_DATA_TYPE_YUV_INTERLEAVED = 0x81,
};

/**
 * @brief raw handler; describes one tap point's byte layout, holds no
 * buffer/streaming/2A state of its own
 */
struct rts_raw_t {
	uint8_t header[RTS_RAW_HEADER_LENGTH]; /**< see Linux rts_raw.h rts_raw_t.header for the byte layout */
	uint32_t length; /**< expected raw data length for width/height/raw_fmt */
	uint32_t width; /**< width of raw image, from rts_hal_get_snr_fmt() */
	uint32_t height; /**< height of raw image, from rts_hal_get_snr_fmt() */
	uint32_t fmt; /**< fourcc the caller must configure its video channel with (always V4L2_PIX_FMT_128BIT_3PIXEL) */
	unsigned int snr_fmt; /**< see \ref rts_raw_fmt */
	unsigned int eff_bits; /**< effective bits */
	unsigned int data_pattern; /**< see \ref rts_data_pattern */
	unsigned int data_type; /**< see \ref rts_data_type */
	unsigned int file_fmt; /**< see \ref rts_file_fmt */
	unsigned int raw_fmt; /**< raw source, see \ref rts_raw_src */
	unsigned int streamid; /**< ISP output channel raw is tapped onto (only channel 0 is supported) */
	int initialized; /**< flag of initialize raw handler */
};

/**
 * @brief Initialize raw handler: query sensor/ISP-middle size and fill in
 * the byte-layout fields (length/eff_bits/data_pattern/data_type/header)
 * for raw_fmt.
 * @param[in] raw_fmt source of raw data, see \ref rts_raw_src
 * @param[in] streamid ISP output channel raw will be tapped onto; only 0 is supported
 * @param[out] ppraw address of raw handler pointer
 * @return 0 : success, minus : fail
 */
int rts_init_raw(unsigned int raw_fmt, unsigned int streamid,
				 struct rts_raw_t **ppraw);
/**
 * @brief Release raw handler
 * @param[in] praw raw handler obtained by \ref rts_init_raw
 */
void rts_release_raw(struct rts_raw_t *praw);
/**
 * @brief Retap the ISP for praw->raw_fmt (rts_hal_set_raw + rts_hal_set_bypass)
 * @param[in] praw raw handler obtained by \ref rts_init_raw
 * @return 0 : success, minus : fail
 */
int rts_config_raw(struct rts_raw_t *praw);
/**
 * @brief Restore the ISP to normal (non-raw) output on praw->streamid
 * @param[in] praw raw handler obtained by \ref rts_init_raw
 * @return 0 : success, minus : fail
 */
int rts_disable_raw(struct rts_raw_t *praw);
/**
 * @brief Rebuild praw->header from the handler's current fields
 * @param[in] praw raw handler obtained by \ref rts_init_raw
 * @return 0 : success, minus : fail
 */
int rts_get_raw_header(struct rts_raw_t *praw);
/**
 * @brief Update raw format's byte-layout fields (does not retap the ISP;
 * call \ref rts_config_raw again to apply the new format)
 * @param[in] raw_fmt format of raw data, see \ref rts_raw_src
 * @param[in] praw raw handler obtained by \ref rts_init_raw
 * @return 0 : success, minus : fail
 */
int rts_update_raw_fmt(unsigned int raw_fmt, struct rts_raw_t *praw);

#ifdef __cplusplus
}
#endif

#endif /* _RTS_RAW_H_ */
