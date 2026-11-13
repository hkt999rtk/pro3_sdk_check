#ifndef _RTS_ISP_ISP_DEFINE_H_INC_
#define _RTS_ISP_ISP_DEFINE_H_INC_

#include <rts_isp_geom.h>

#ifdef __cplusplus
extern "C" {
#endif

/* IMPORTANT: DO NOT define hardware-dependent symbol here */

/**
 * @defgroup isp ISP of Realtek IPCam
 * @brief Structure and API for setting of Realtek IPCam ISP.
 * ISP must be configured before ISP video stream.
 * @ingroup total
 * @{
 */
#define RTS_ISP_NAME_LEN 32
#define RTS_ISP_PATH_LEN 256

/**
 * @brief ISP ID
 */
enum isp_id {
	ISP0, /**< the first ISP.Only one ISP is supported temporarily. */
	_ISP_ID_NUM,
};

#define RTS_ISP_HDR_CHAN_MAX 4

/**
 * @brief hdr mode of ISP
 */
enum rts_isp_hdr_mode {
	RTS_ISP_HDR_NONE = 0, /**< linear mode */
	RTS_ISP_HDR_LINE_2TO1 = 0x10, /**< stagger/dol hdr mode */
	RTS_ISP_HDR_DCG_COMBINE = 0x20, /**< dcg hdr combine */
	RTS_ISP_HDR_DCG_COMBINE_TM = 0x21, /**< dcg hdr combine & compress*/
	RTS_ISP_HDR_DCG_2TO1 = 0x23, /**< dcg hdr non-combine */
};

/**
 * @brief hdr transport variant of dol hdr mode
 */
enum rts_isp_hdr_variant {
	RTS_ISP_HDR_VAR_INVALID, /* The hdr variant is only valid when the rts_isp_hdr_mode is RTS_ISP_HDR_LINE_2TO1 */
	RTS_ISP_HDR_VAR_VC, /* virtual channel hdr */
	RTS_ISP_HDR_VAR_DUMMY, /* multi-exposure stagger with dummy */
	RTS_ISP_HDR_VAR_NODUMMY, /* multi-exposure stagger without dummy */
	RTS_ISP_HDR_VAR_FID, /* sony fid based hdr */
};

static inline int hdr_chan_num(int32_t hdr)
{
	if (hdr == RTS_ISP_HDR_LINE_2TO1) {
		return 2;
	}
	return 1;
}

#define RTS_ISP_ROI_AREA_MAX 8

/**
 * @brief mode of sensor
 */
struct rts_isp_sensor_mode {
	uint8_t hdr; /* enum rts_isp_hdr_mode */ /**< sensor hdr mode */
	uint8_t hdr_variant;
	rts_isp_size_t size; /**< sensor size after isp crop */
	float fps; /**< sensor base fps */
};

/**
 * @brief modes list of sensor
 */
struct rts_isp_sensor_modes {
	uint32_t num; /**< modes num */
	struct rts_isp_sensor_mode mode[8]; /**< modes list array */
};

/**
 * @brief ISP plugin type
 */
enum rts_isp_plugin_type {
	RTS_ISP_PLUG_TYPE_MIN,
	RTS_ISP_PLUG_TYPE_ALGO_AE = RTS_ISP_PLUG_TYPE_MIN,
	RTS_ISP_PLUG_TYPE_ALGO_AWB,
	RTS_ISP_PLUG_TYPE_ALGO_AF,
	RTS_ISP_PLUG_TYPE_ALGO_OTHER,
	RTS_ISP_PLUG_TYPE_SENSOR,
	RTS_ISP_PLUG_TYPE_IQ,
	RTS_ISP_PLUG_TYPE_MAX = RTS_ISP_PLUG_TYPE_IQ,
};

/**
 * @brief ISP plugin ID
 */
enum rts_isp_plugin_id {
	RTS_ISP_PLUG_ID_MIN,

	RTS_ISP_PLUG_ALGO_ID_MIN = RTS_ISP_PLUG_ID_MIN,
	RTS_ISP_PLUG_ALGO_AE_ID_MIN = RTS_ISP_PLUG_ALGO_ID_MIN,
	RTS_ISP_PLUG_ALGO_AE_ID0 = RTS_ISP_PLUG_ALGO_AE_ID_MIN,
	RTS_ISP_PLUG_ALGO_AE_ID1,
	RTS_ISP_PLUG_ALGO_AE_ID2,
	RTS_ISP_PLUG_ALGO_AE_ID3,
	RTS_ISP_PLUG_ALGO_AE_ID_MAX = RTS_ISP_PLUG_ALGO_AE_ID3,

	RTS_ISP_PLUG_ALGO_AWB_ID_MIN,
	RTS_ISP_PLUG_ALGO_AWB_ID0 = RTS_ISP_PLUG_ALGO_AWB_ID_MIN,
	RTS_ISP_PLUG_ALGO_AWB_ID1,
	RTS_ISP_PLUG_ALGO_AWB_ID2,
	RTS_ISP_PLUG_ALGO_AWB_ID3,
	RTS_ISP_PLUG_ALGO_AWB_ID_MAX = RTS_ISP_PLUG_ALGO_AWB_ID3,

	RTS_ISP_PLUG_ALGO_AF_ID_MIN,
	RTS_ISP_PLUG_ALGO_AF_ID0 = RTS_ISP_PLUG_ALGO_AF_ID_MIN,
	RTS_ISP_PLUG_ALGO_AF_ID1,
	RTS_ISP_PLUG_ALGO_AF_ID2,
	RTS_ISP_PLUG_ALGO_AF_ID3,
	RTS_ISP_PLUG_ALGO_AF_ID_MAX = RTS_ISP_PLUG_ALGO_AF_ID3,

	RTS_ISP_PLUG_ALGO_OTHER_ID_MIN,
	RTS_ISP_PLUG_ALGO_OTHER_ID0 = RTS_ISP_PLUG_ALGO_OTHER_ID_MIN,
	RTS_ISP_PLUG_ALGO_OTHER_ID1,
	RTS_ISP_PLUG_ALGO_OTHER_ID2,
	RTS_ISP_PLUG_ALGO_OTHER_ID3,
	RTS_ISP_PLUG_ALGO_OTHER_ID_MAX = RTS_ISP_PLUG_ALGO_OTHER_ID3,
	RTS_ISP_PLUG_ALGO_ID_MAX = RTS_ISP_PLUG_ALGO_OTHER_ID_MAX,

	RTS_ISP_PLUG_SENSOR_ID_MIN,
	RTS_ISP_PLUG_SENSOR_ID0 = RTS_ISP_PLUG_SENSOR_ID_MIN,
	RTS_ISP_PLUG_SENSOR_ID1,
	RTS_ISP_PLUG_SENSOR_ID2,
	RTS_ISP_PLUG_SENSOR_ID3,
	RTS_ISP_PLUG_SENSOR_ID4,
	RTS_ISP_PLUG_SENSOR_ID5,
	RTS_ISP_PLUG_SENSOR_ID6,
	RTS_ISP_PLUG_SENSOR_ID7,
	RTS_ISP_PLUG_SENSOR_ID_MAX = RTS_ISP_PLUG_SENSOR_ID7,

	RTS_ISP_PLUG_EXEC_ID_MAX = RTS_ISP_PLUG_SENSOR_ID_MAX,

	RTS_ISP_PLUG_IQ_ID_MIN,
	RTS_ISP_PLUG_IQ_ID0 = RTS_ISP_PLUG_IQ_ID_MIN,
	RTS_ISP_PLUG_IQ_ID1,
	RTS_ISP_PLUG_IQ_ID2,
	RTS_ISP_PLUG_IQ_ID3,
	RTS_ISP_PLUG_IQ_ID4,
	RTS_ISP_PLUG_IQ_ID5,
	RTS_ISP_PLUG_IQ_ID6,
	RTS_ISP_PLUG_IQ_ID7,
	RTS_ISP_PLUG_IQ_ID_MAX = RTS_ISP_PLUG_IQ_ID7,

	RTS_ISP_PLUG_ID_MAX = RTS_ISP_PLUG_IQ_ID_MAX,

	_RTS_ISP_PLUG_ID_NUM,
};

static inline void rts_isp_plugin_id_range(int plug_type, int *id_min,
		int *id_max)
{
	if (plug_type < RTS_ISP_PLUG_TYPE_SENSOR) {
		*id_min = plug_type * 4;
		*id_max = *id_min + 3;
	} else {
		*id_min = RTS_ISP_PLUG_SENSOR_ID_MIN +
				  (plug_type - RTS_ISP_PLUG_TYPE_SENSOR) * 8;
		*id_max = *id_min + 7;
	}
}

static inline int rts_isp_plugin_is_asset(int plug_id)
{
	return plug_id >= RTS_ISP_PLUG_IQ_ID_MIN &&
		   plug_id <= RTS_ISP_PLUG_IQ_ID_MAX;
}

static inline int rts_isp_plugin_id_to_type(int id)
{
	if (id <= RTS_ISP_PLUG_ALGO_AE_ID_MAX) {
		return RTS_ISP_PLUG_TYPE_ALGO_AE;
	} else if (id <= RTS_ISP_PLUG_ALGO_AWB_ID_MAX) {
		return RTS_ISP_PLUG_TYPE_ALGO_AWB;
	} else if (id <= RTS_ISP_PLUG_ALGO_AF_ID_MAX) {
		return RTS_ISP_PLUG_TYPE_ALGO_AF;
	} else if (id <= RTS_ISP_PLUG_ALGO_OTHER_ID_MAX) {
		return RTS_ISP_PLUG_TYPE_ALGO_OTHER;
	} else if (id <= RTS_ISP_PLUG_SENSOR_ID_MAX) {
		return RTS_ISP_PLUG_TYPE_SENSOR;
	}
	return RTS_ISP_PLUG_TYPE_IQ;
}

struct rts_isp_fract {
	uint32_t numerator;
	uint32_t denominator;
};

#ifdef __cplusplus
}
#endif

#endif /* _RTS_ISP_ISP_DEFINE_H_INC_ */
