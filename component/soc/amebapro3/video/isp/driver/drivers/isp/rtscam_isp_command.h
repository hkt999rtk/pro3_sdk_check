#ifndef _RTSCAM_ISP_COMMAND_H_
#define _RTSCAM_ISP_COMMAND_H_

/* MUST be same as struct isp_msg_hdr */
struct rtscam_isp_msg_hdr {
	uint32_t flags;
	uint32_t msg_len;
	uint32_t capacity;
	uint32_t isp_id;
	uint32_t mod_id;
	uint32_t action;
	int32_t ret_val;
	uint32_t reserved;
};

struct rtscam_isp_msg_buf {
	int refcnt;
	struct rtscam_isp_msg_hdr hdr;
	void *data;
	unsigned short sequence;
};

struct rtscam_isp_preview_info {
	struct v4l2_fract fps;
};

struct rtscam_isp_count_msg {
	uint32_t frame_count;
	uint32_t statis_count;
};

enum rtscam_isp_input_fmt {
	ISP_INPUT_SBGGR16 = 0,
	ISP_INPUT_SGBRG16,
	ISP_INPUT_SGRBG16,
	ISP_INPUT_SRGGB16,
	ISP_INPUT_SGRBG32 = 6,
	ISP_INPUT_RGB = 8,
	ISP_INPUT_YUV = 12,
	ISP_INPUT_YUYV = 16,
	ISP_INPUT_NV16,
};

struct rtscam_isp_input_behavior {
	uint32_t ddr_mode;

	uint32_t fmt;
	uint32_t width;
	uint32_t height;
	struct v4l2_fract fival;
};

struct rtscam_isp_input_buffer_msg {
	uint32_t phy_addr;
};

#define ISP_AID_NONE 0
#define ISP_AID_READ 1
#define ISP_AID_WRITE 2

#define ISP_AID_NR_SHIFT 0
#define ISP_AID_TYPE_SHIFT 8
#define ISP_AID_SIZE_SHIFT 16
#define ISP_AID_DIR_SHIFT 30

#define ISP_ACTC(dir, type, nr, size)                                          \
	(uint32_t)((dir) << ISP_AID_DIR_SHIFT | (size) << ISP_AID_SIZE_SHIFT | \
		   (type) << ISP_AID_TYPE_SHIFT | (nr) << ISP_AID_NR_SHIFT)
#define ISP_ACT(type, nr) ISP_ACTC(ISP_AID_NONE, type, nr, 0)
#define ISP_ACTR(type, nr, size) ISP_ACTC(ISP_AID_READ, type, nr, sizeof(size))
#define ISP_ACTW(type, nr, size) ISP_ACTC(ISP_AID_WRITE, type, nr, sizeof(size))
#define ISP_ACTWR(type, nr, size) \
	ISP_ACTC(ISP_AID_READ | ISP_AID_WRITE, type, nr, sizeof(size))

#define isp_act_has_read(cmd) (((cmd) >> ISP_AID_DIR_SHIFT) & ISP_AID_READ)
#define isp_act_has_write(cmd) (((cmd) >> ISP_AID_DIR_SHIFT) & ISP_AID_WRITE)

#define ISP_EXEC_TYPE 'E'

/* mod ids and actions must be the same as isplib */
#define v4l2_fourcc(a, b, c, d)                                         \
	((uint32_t)(a) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | \
	 ((uint32_t)(d) << 24))
enum rtscam_isp_mod_id {
	RTSCAM_ISP_MOD_CONTROL = v4l2_fourcc('c', 't', 'r', 'l'),
	RTSCAM_ISP_MOD_INPUT = v4l2_fourcc('i', 'p', 'u', 't'),
};

enum rtscam_isp_driver_action {
	/* mod control */
	RTSCAM_ISP_SET_FPS =
		ISP_ACTW(ISP_EXEC_TYPE, 0, struct rtscam_isp_preview_info),
	RTSCAM_ISP_GET_COUNT =
		ISP_ACTWR(ISP_EXEC_TYPE, 3, struct rtscam_isp_count_msg),

	/* mod input */
	RTSCAM_ISP_SET_BEHAVIOR =
		ISP_ACTW(ISP_EXEC_TYPE, 0, struct rtscam_isp_input_behavior),
	RTSCAM_ISP_SET_BUFFER =
		ISP_ACTW(ISP_EXEC_TYPE, 1, struct rtscam_isp_input_buffer_msg),
};

#endif /* _RTSCAM_ISP_COMMAND_H_ */
