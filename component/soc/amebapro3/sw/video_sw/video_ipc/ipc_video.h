#ifndef IPC_VIDEO_H
#define IPC_VIDEO_H

// size = IPC_MSG_STRUCT, 16byte

/* vp_cmd_t > cmd */
/************************************************************************
 31      30      29       28.......24  23.............16  15............0
|resv | resv  | cmd_prio |   resv     | command channel  |  execute  cmd |

cmd_prio : 0-low 1-high
*************************************************************************/
typedef struct ap_ch_info_s {
	uint32_t ch;
	uint32_t width;
	uint32_t height;
	uint32_t fps;
	uint32_t codec_type;
} ap_ch_info_t;

typedef struct vp_cmd_s {
	uint32_t cmd;
	uint32_t arg1;  //ap_ch_info_t
	uint32_t arg2;
	void *notify;
} vp_cmd_t;

/* vp_resp_t > resp */
/************************************************************************
 31        30             29.......24  23.............16  15............0
|cmd_resp| frame_resp    |     resv   | complete channel |  executed cmd |
*************************************************************************/
typedef struct vp_frame_info_s {
	uint32_t ch;
	uint32_t width;
	uint32_t height;
	uint32_t codec;
	uint32_t time_stamp;
	uint32_t cmd_status;
	uint32_t enc_addr;
	uint32_t enc_len;
} vp_frame_info_t;

typedef struct vp_resp_s {
	uint32_t resp;
	uint32_t cmd_res;
	union {
		void *notify;
		void *frame_info;  //vp_frame_info_t
	};
	uint32_t resv;
} vp_resp_t;

#define CMD_RESP_MASK           (1 << 31)
#define FRM_RESP_MASK           (1 << 30)

#define CMD_VALUE_OFFSET        0
#define CHN_VALUE_OFFSET        16
#define CMD_VALUE_MASK          (0xffff << CMD_VALUE_OFFSET)
#define CHN_VALUE_MASK          (0xff << CHN_VALUE_OFFSET)

#define GET_CHANNEL(x)          (((x) & CHN_VALUE_MASK) >> CHN_VALUE_OFFSET)
#define GET_COMMAND(x)          (((x) & CMD_VALUE_MASK) >> CMD_VALUE_OFFSET)

#define SET_CHANNEL(x, ch)      (((x) & (~CHN_VALUE_MASK)) | ((ch) << CHN_VALUE_OFFSET))
#define SET_COMMAND(x, cmd)     (((x) & (~CMD_VALUE_MASK)) | ((cmd) << CMD_VALUE_OFFSET))

#define SET_FRAME_RESP(x)       ((x) | FRM_RESP_MASK)
#define SET_COMMAND_RESP(x)     ((x) | CMD_RESP_MASK)

#define IS_CMD_RESP(resp)       (resp & CMD_RESP_MASK) ? 1 : 0
#define IS_FRM_RESP(resp)       (resp & FRM_RESP_MASK) ? 1 : 0

#define CMD_PRIO_MASK           (1 << 29)

#define SET_CMD_HIPRIO(cmd)     ((cmd) | CMD_PRIO_MASK)
#define IS_CMD_HIPRIO(cmd)      ((cmd) & CMD_PRIO_MASK) ? 1 : 0

/**********************************/
/**        AP side API           **/
/**********************************/
// AP send VP command
int AP2VP_SendCmd(uint32_t cmd, uint32_t arg1, uint32_t arg2, uint32_t timeout);

/**********************************/
/**        VP side API           **/
/**********************************/
// NOTE : seems no need response queue
void vp_ipc_sw_service_init(void);
void vp_ipc_send_frame(int ch, vp_frame_info_t *frm_info, uint32_t timeout);
void vp_ipc_notify_frame_recycled(int ch);
void vp_ipc_wait_frame_recycled(int ch, uint32_t timeout_ms);

/* VIDEO IPC CMD */
/***********************************
 15.....14  13...............0
|cmd_header|    video cmd     |
***********************************/
#define VIDEO_CMD_HEADER_OFFSET     14
#define VIDEO_COM_CMD_HEADER        0
#define VIDEO_ISP_CMD_HEADER        1
#define VIDEO_ENC_CMD_HEADER        2

#define VIDEO_COM_CMD(x)			((x) | (VIDEO_COM_CMD_HEADER << VIDEO_CMD_HEADER_OFFSET))
#define VIDEO_ISP_CMD(x)			((x) | (VIDEO_ISP_CMD_HEADER << VIDEO_CMD_HEADER_OFFSET))
#define VIDEO_ENC_CMD(x)			((x) | (VIDEO_ENC_CMD_HEADER << VIDEO_CMD_HEADER_OFFSET))

#define IS_VIDEO_COM_CMD(x)         (((x) >> VIDEO_CMD_HEADER_OFFSET) == VIDEO_COM_CMD_HEADER) ? 1 : 0
#define IS_VIDEO_ISP_CMD(x)         (((x) >> VIDEO_CMD_HEADER_OFFSET) == VIDEO_ISP_CMD_HEADER) ? 1 : 0
#define IS_VIDEO_ENC_CMD(x)         (((x) >> VIDEO_CMD_HEADER_OFFSET) == VIDEO_ENC_CMD_HEADER) ? 1 : 0

/* video common command */
#define VIDEO_CMD_OPEN              VIDEO_COM_CMD(0x01)
#define VIDEO_CMD_CLOSE             VIDEO_COM_CMD(0x02)
#define VIDEO_CMD_BUF_RELEASE       VIDEO_COM_CMD(0x03)
#define VIDEO_CMD_FCS_ATTACH        VIDEO_COM_CMD(0x04)
#define VIDEO_CMD_FCS_BOOT_STATE    VIDEO_COM_CMD(0x05)

/* VIDEO_CMD_FCS_BOOT_STATE return values; boot-wide, not per-channel state. */
#define VIDEO_FCS_BOOT_ERROR        (-1) /* IPC query or VP FCS startup failed. */
#define VIDEO_FCS_BOOT_NORMAL       0    /* VP selected the normal boot path without FCS. */
#define VIDEO_FCS_BOOT_ACTIVE       1    /* VP selected FCS; configured channels are starting or active. */

/* video isp command */
#define VIDEO_CMD_ENA_AINR          VIDEO_ISP_CMD(0x01)
#define VIDEO_CMD_SET_FPS           VIDEO_ISP_CMD(0x02)
#define VIDEO_CMD_ISP_SWITCH_RAW    VIDEO_ISP_CMD(0x03)
#define VIDEO_CMD_ISP_SET_SNR_MODE  VIDEO_ISP_CMD(0x04)
#define VIDEO_CMD_ISP_SET_CONTROL   VIDEO_ISP_CMD(0x05)
#define VIDEO_CMD_ISP_GET_CONTROL   VIDEO_ISP_CMD(0x06)
#define VIDEO_CMD_ISP_SET_I2C       VIDEO_ISP_CMD(0x07)
#define VIDEO_CMD_ISP_GET_I2C       VIDEO_ISP_CMD(0x08)
#define VIDEO_CMD_ISP_GET_SNR_INFO  VIDEO_ISP_CMD(0x09)
#define VIDEO_CMD_ISP_SET_SNR_IQ    VIDEO_ISP_CMD(0x0a)
/* video enc command */
#define VIDEO_CMD_GET_RC            VIDEO_ENC_CMD(0x01)
#define VIDEO_CMD_SET_RC            VIDEO_ENC_CMD(0x02)
#define VIDEO_CMD_SET_GOP           VIDEO_ENC_CMD(0x03)
#define VIDEO_CMD_FORCE_I           VIDEO_ENC_CMD(0x04)
#define VIDEO_CMD_SET_OSD           VIDEO_ENC_CMD(0x05)
#define VIDEO_CMD_GET_RC_EXT        VIDEO_ENC_CMD(0x06)
#define VIDEO_CMD_SET_RC_EXT        VIDEO_ENC_CMD(0x07)
#define VIDEO_CMD_GET_CODING        VIDEO_ENC_CMD(0x08)
#define VIDEO_CMD_SET_CODING        VIDEO_ENC_CMD(0x09)
#define VIDEO_CMD_SET_MOSAIC        VIDEO_ENC_CMD(0x0a)
#define VIDEO_CMD_SET_OSD_SCENE     VIDEO_ENC_CMD(0x0b)
#define VIDEO_CMD_GET_OSD_SCENE_STATE VIDEO_ENC_CMD(0x0c)
#define VIDEO_CMD_SNAPSHOT_START    VIDEO_ENC_CMD(0x0d)


#endif
