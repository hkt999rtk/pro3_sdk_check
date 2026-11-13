#ifndef _RTS_ISP_CTRL_H_INC_
#define _RTS_ISP_CTRL_H_INC_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ISP_V4L2_CTRL_ID_MASK (0x0fffffff)
#define ISP_V4L2_CTRL_ID2CLASS(id) ((id)&0x0fff0000UL)
#define ISP_V4L2_CTRL_ID2WHICH(id) ((id)&0x0fff0000UL)
#define ISP_V4L2_CTRL_DRIVER_PRIV(id) (((id)&0xffff) >= 0x1000)
#define ISP_V4L2_CTRL_MAX_DIMS (4)
#define ISP_V4L2_CTRL_WHICH_DEF_VAL 0x0f000000

enum isp_v4l2_ctrl_type {
	ISP_V4L2_CTRL_TYPE_INTEGER = 1,
	ISP_V4L2_CTRL_TYPE_BOOLEAN = 2,
	ISP_V4L2_CTRL_TYPE_MENU = 3,
	ISP_V4L2_CTRL_TYPE_BUTTON = 4,
	ISP_V4L2_CTRL_TYPE_INTEGER64 = 5,
	ISP_V4L2_CTRL_TYPE_CTRL_CLASS = 6,
	ISP_V4L2_CTRL_TYPE_STRING = 7,
	ISP_V4L2_CTRL_TYPE_BITMASK = 8,
	ISP_V4L2_CTRL_TYPE_INTEGER_MENU = 9,

	/* Compound types are >= 0x0100 */
	ISP_V4L2_CTRL_COMPOUND_TYPES = 0x0100,
	ISP_V4L2_CTRL_TYPE_U8 = 0x0100,
	ISP_V4L2_CTRL_TYPE_U16 = 0x0101,
	ISP_V4L2_CTRL_TYPE_U32 = 0x0102,
};

/*  Used in the VIDIOC_QUERYCTRL ioctl for querying controls */
struct isp_v4l2_queryctrl {
	uint32_t id;
	uint32_t type; /* enum isp_v4l2_ctrl_type */
	uint8_t name[32]; /* Whatever */
	int32_t minimum; /* Note signedness */
	int32_t maximum;
	int32_t step;
	int32_t default_value;
	uint32_t flags;
	uint32_t reserved[2];
};

/*  Used in the VIDIOC_QUERY_EXT_CTRL ioctl for querying extended controls */
struct isp_v4l2_query_ext_ctrl {
	uint32_t id;
	uint32_t type;
	char name[32];
	int64_t minimum;
	int64_t maximum;
	uint64_t step;
	int64_t default_value;
	uint32_t flags;
	uint32_t elem_size;
	uint32_t elems;
	uint32_t nr_of_dims;
	uint32_t dims[ISP_V4L2_CTRL_MAX_DIMS];
	uint32_t reserved[32];
};

/*  Used in the VIDIOC_QUERYMENU ioctl for querying menu items */
struct isp_v4l2_querymenu {
	uint32_t id;
	uint32_t index;
	union {
		uint8_t name[32]; /* Whatever */
		int64_t value;
	};
	uint32_t reserved;
} __attribute__((packed));

/*  Control flags  */
#define ISP_V4L2_CTRL_FLAG_DISABLED 0x0001
#define ISP_V4L2_CTRL_FLAG_GRABBED 0x0002
#define ISP_V4L2_CTRL_FLAG_READ_ONLY 0x0004
#define ISP_V4L2_CTRL_FLAG_UPDATE 0x0008
#define ISP_V4L2_CTRL_FLAG_INACTIVE 0x0010
#define ISP_V4L2_CTRL_FLAG_SLIDER 0x0020
#define ISP_V4L2_CTRL_FLAG_WRITE_ONLY 0x0040
#define ISP_V4L2_CTRL_FLAG_VOLATILE 0x0080
#define ISP_V4L2_CTRL_FLAG_HAS_PAYLOAD 0x0100
#define ISP_V4L2_CTRL_FLAG_EXECUTE_ON_WRITE 0x0200
#define ISP_V4L2_CTRL_FLAG_MODIFY_LAYOUT 0x0400

/*  Query flags, to be ORed with the control ID */
#define ISP_ISP_V4L2_CTRL_FLAG_NEXT_CTRL 0x80000000
#define ISP_ISP_V4L2_CTRL_FLAG_NEXT_COMPOUND 0x40000000

struct isp_v4l2_control {
	uint32_t id;
	int32_t value;
};

struct isp_v4l2_ext_control {
	uint32_t id;
	uint32_t size;
	uint32_t reserved2[1];
	union {
		int32_t value;
		int64_t value64;
		char *string;
		uint8_t *p_u8;
		uint16_t *p_u16;
		uint32_t *p_u32;
		void *ptr;
	};
} __attribute__((packed));

struct isp_v4l2_ext_controls {
	union {
		uint32_t ctrl_class;
		uint32_t which;
	};
	uint32_t count;
	uint32_t error_idx;
	int32_t request_fd;
	uint32_t reserved[1];
	struct isp_v4l2_ext_control *controls;
};

int rts_isp_v4l2_query_ctrl(uint32_t isp_id, struct isp_v4l2_queryctrl *qc);
int rts_isp_v4l2_query_menu(uint32_t isp_id, struct isp_v4l2_querymenu *qm);
int rts_isp_v4l2_g_ctrl(uint32_t isp_id, struct isp_v4l2_control *control);
int rts_isp_v4l2_s_ctrl(uint32_t isp_id, struct isp_v4l2_control *control);
int rts_isp_v4l2_query_ext_ctrl(uint32_t isp_id,
								struct isp_v4l2_query_ext_ctrl *qec);
int rts_isp_v4l2_g_ext_ctrls(uint32_t isp_id, struct isp_v4l2_ext_controls *ec);
int rts_isp_v4l2_s_ext_ctrls(uint32_t isp_id, struct isp_v4l2_ext_controls *ec);
int rts_isp_v4l2_try_ext_ctrls(uint32_t isp_id,
							   struct isp_v4l2_ext_controls *ec);

/*  User-class control IDs defined by V4L2 */
#define ISP_V4L2_CID_MAX_CTRLS 1024
/*  IDs reserved for driver specific controls */
#define ISP_V4L2_CID_PRIVATE_BASE 0x08000000

/* Control classes */
#define ISP_V4L2_CTRL_CLASS_USER 0x00980000 /* Old-style 'user' controls */
#define ISP_V4L2_CTRL_CLASS_CAMERA 0x009a0000 /* Camera class controls */

/* User-class control IDs */

#define ISP_V4L2_CID_BASE (ISP_V4L2_CTRL_CLASS_USER | 0x900)
#define ISP_V4L2_CID_USER_BASE ISP_V4L2_CID_BASE
#define ISP_V4L2_CID_USER_CLASS (ISP_V4L2_CTRL_CLASS_USER | 1)
#define ISP_V4L2_CID_BRIGHTNESS (ISP_V4L2_CID_BASE + 0)
#define ISP_V4L2_CID_CONTRAST (ISP_V4L2_CID_BASE + 1)
#define ISP_V4L2_CID_SATURATION (ISP_V4L2_CID_BASE + 2)
#define ISP_V4L2_CID_HUE (ISP_V4L2_CID_BASE + 3)
#define ISP_V4L2_CID_AUTO_WHITE_BALANCE (ISP_V4L2_CID_BASE + 12)
#define ISP_V4L2_CID_DO_WHITE_BALANCE (ISP_V4L2_CID_BASE + 13)
#define ISP_V4L2_CID_RED_BALANCE (ISP_V4L2_CID_BASE + 14)
#define ISP_V4L2_CID_BLUE_BALANCE (ISP_V4L2_CID_BASE + 15)
#define ISP_V4L2_CID_GAMMA (ISP_V4L2_CID_BASE + 16)
#define ISP_V4L2_CID_EXPOSURE (ISP_V4L2_CID_BASE + 17)
#define ISP_V4L2_CID_AUTOGAIN (ISP_V4L2_CID_BASE + 18)
#define ISP_V4L2_CID_GAIN (ISP_V4L2_CID_BASE + 19)
#define ISP_V4L2_CID_HFLIP (ISP_V4L2_CID_BASE + 20)
#define ISP_V4L2_CID_VFLIP (ISP_V4L2_CID_BASE + 21)

#define ISP_V4L2_CID_POWER_LINE_FREQUENCY (ISP_V4L2_CID_BASE + 24)
enum isp_v4l2_power_line_frequency {
	ISP_V4L2_CID_POWER_LINE_FREQUENCY_DISABLED = 0,
	ISP_V4L2_CID_POWER_LINE_FREQUENCY_50HZ = 1,
	ISP_V4L2_CID_POWER_LINE_FREQUENCY_60HZ = 2,
	ISP_V4L2_CID_POWER_LINE_FREQUENCY_AUTO = 3,
};
#define ISP_V4L2_CID_HUE_AUTO (ISP_V4L2_CID_BASE + 25)
#define ISP_V4L2_CID_WHITE_BALANCE_TEMPERATURE (ISP_V4L2_CID_BASE + 26)
#define ISP_V4L2_CID_SHARPNESS (ISP_V4L2_CID_BASE + 27)

/*  RTS user-class control IDs */

#define ISP_RTS_V4L2_CID_BASE (ISP_V4L2_CID_USER_BASE | 0xf000)
#define ISP_V4L2_CID_GREEN_BALANCE (ISP_RTS_V4L2_CID_BASE + 0)
#define ISP_V4L2_CID_IQ_TABLE (ISP_RTS_V4L2_CID_BASE + 1)
#define ISP_V4L2_CID_DAY_NIGHT (ISP_RTS_V4L2_CID_BASE + 2)
enum rts_isp_day_night {
	RTS_ISP_DAY,
	RTS_ISP_NIGHT,
};
#define ISP_V4L2_CID_DYNAMIC_FPS (ISP_RTS_V4L2_CID_BASE + 3)
#define ISP_V4L2_CID_DYNAMIC_IQ (ISP_RTS_V4L2_CID_BASE + 4)
#define ISP_V4L2_CID_TEMPORAL_DENOISE (ISP_RTS_V4L2_CID_BASE + 5)
#define ISP_V4L2_CID_DEHAZE (ISP_RTS_V4L2_CID_BASE + 6)
#define ISP_V4L2_CID_DEHAZE_LEVEL (ISP_RTS_V4L2_CID_BASE + 7)
#define ISP_V4L2_CID_LDC (ISP_RTS_V4L2_CID_BASE + 8)
#define ISP_V4L2_CID_GRAY (ISP_RTS_V4L2_CID_BASE + 9)
#define ISP_V4L2_CID_AUTO_CCM (ISP_RTS_V4L2_CID_BASE + 10)
#define ISP_V4L2_CID_CCM_MATRIX (ISP_RTS_V4L2_CID_BASE + 11)
#define ISP_V4L2_CID_WDR_MODE (ISP_RTS_V4L2_CID_BASE + 12)
enum rts_isp_wdr_mode {
	ISP_WDR_INNER_DISABLE,
	ISP_WDR_INNER_MANUAL,
	ISP_WDR_INNER_AUTO,
};
#define ISP_V4L2_CID_WDR_LEVEL (ISP_RTS_V4L2_CID_BASE + 13)
#define ISP_V4L2_CID_AUTO_GAMMA (ISP_RTS_V4L2_CID_BASE + 14)
#define ISP_V4L2_CID_GAMMA_CURVE (ISP_RTS_V4L2_CID_BASE + 15)
#define ISP_V4L2_CID_AUTO_YGAMMA (ISP_RTS_V4L2_CID_BASE + 16)
#define ISP_V4L2_CID_YGAMMA_CURVE (ISP_RTS_V4L2_CID_BASE + 17)
#define ISP_V4L2_CID_OUTPUT_POSITION (ISP_RTS_V4L2_CID_BASE + 18)
#define ISP_V4L2_CID_COLOR_RANGE (ISP_RTS_V4L2_CID_BASE + 19)
enum rts_isp_color_range {
	ISP_COLOR_RANGE_BT601_STUDIO_SWING,
	ISP_COLOR_RANGE_BT601_FULL_SWING,
	ISP_COLOR_RANGE_BT709_STUDIO_SWING,
	ISP_COLOR_RANGE_BT709_FULL_SWING,
};
#define ISP_V4L2_CID_NR_LEVEL (ISP_RTS_V4L2_CID_BASE + 20)
#define ISP_V4L2_CID_DE_LEVEL (ISP_RTS_V4L2_CID_BASE + 21)
#define ISP_V4L2_CID_SNR_CROP_X (ISP_RTS_V4L2_CID_BASE + 22)
#define ISP_V4L2_CID_SNR_CROP_Y (ISP_RTS_V4L2_CID_BASE + 23)
#define ISP_V4L2_CID_Y_GAIN (ISP_RTS_V4L2_CID_BASE + 24)
#define ISP_V4L2_CID_SMART_IR_MODE (ISP_RTS_V4L2_CID_BASE + 25)
#define ISP_V4L2_CID_SMART_IR_MANUAL_LEVEL (ISP_RTS_V4L2_CID_BASE + 26)
#define ISP_V4L2_CID_DAYNIGHT_DETECTION (ISP_RTS_V4L2_CID_BASE + 27)
#define ISP_V4L2_CID_AUTO_HDR_RATIO (ISP_RTS_V4L2_CID_BASE + 30)
#define ISP_V4L2_CID_HDR_RATIO (ISP_RTS_V4L2_CID_BASE + 31)
#define ISP_V4L2_CID_MIRROR_FLIP (ISP_RTS_V4L2_CID_BASE + 32)

/*  Camera class control IDs */

#define ISP_V4L2_CID_CAMERA_CLASS_BASE (ISP_V4L2_CTRL_CLASS_CAMERA | 0x900)
#define ISP_V4L2_CID_CAMERA_CLASS (ISP_V4L2_CTRL_CLASS_CAMERA | 1)

#define ISP_V4L2_CID_EXPOSURE_AUTO (ISP_V4L2_CID_CAMERA_CLASS_BASE + 1)
enum isp_v4l2_exposure_auto_type {
	ISP_V4L2_EXPOSURE_AUTO = 0,
	ISP_V4L2_EXPOSURE_MANUAL = 1,
	ISP_V4L2_EXPOSURE_SHUTTER_PRIORITY = 2,
	ISP_V4L2_EXPOSURE_APERTURE_PRIORITY = 3
};
#define ISP_V4L2_CID_EXPOSURE_ABSOLUTE (ISP_V4L2_CID_CAMERA_CLASS_BASE + 2)
#define ISP_V4L2_CID_EXPOSURE_AUTO_PRIORITY (ISP_V4L2_CID_CAMERA_CLASS_BASE + 3)

#define ISP_V4L2_CID_FOCUS_ABSOLUTE (ISP_V4L2_CID_CAMERA_CLASS_BASE + 10)
#define ISP_V4L2_CID_FOCUS_RELATIVE (ISP_V4L2_CID_CAMERA_CLASS_BASE + 11)
#define ISP_V4L2_CID_FOCUS_AUTO (ISP_V4L2_CID_CAMERA_CLASS_BASE + 12)

#define ISP_V4L2_CID_WIDE_DYNAMIC_RANGE (ISP_V4L2_CID_CAMERA_CLASS_BASE + 21)

#ifdef __cplusplus
}
#endif

#endif /* _RTS_ISP_CTRL_H_INC_ */
