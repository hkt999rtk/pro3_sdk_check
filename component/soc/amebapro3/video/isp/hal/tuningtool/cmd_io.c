/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"
#include "usbd_uvc.h"
#include "usbd_video.h"
#include "usb_ch9.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <ctype.h>
#include "tuning-server.h"
#include <ameba_soc.h>

#if USBD_TUNING_MODE

extern usbd_uvc_process_unit_t p_data;
extern usbd_uvc_process_unit_t x_data;
//usbd_uvc_process_unit_t f_data;
static int uvc_format = 0;

//Tuning tool
#define USBD_UVC_VendorCommand   	0X0A
#define USBD_UVC_VendorData      	0X0B
#define USBD_UVC_VendorLength    	0X40
#define USBD_UVC_UserDefineCommand  	0X0C
#define USBD_UVC_UserDefineData     	0X0D
#define USBD_UVC_UserDefineLength   	0X40

typedef struct _isp_usbd_cmd_data {
	u16 cmdcode;
	u8 index;
	u8 length;
	u16 param;
	u16 addr;
	u8 buf[0x40];//The maximum buffer size
	u32 offset;//to recode the index for read and write
	//u32 error_type;
} isp_usbd_cmd_data;

struct ExtUnitCmd {
	u8 byCmd;
	u8 bySubCmd;
	u16 byarAddr;
	u16 byarLen;
	union {
		u16 byReserved;
		u16 byarAddrH;
	};
	u32 offset;//to recode the index for read and write
};

static isp_usbd_cmd_data isp_data;
static struct ExtUnitCmd isp_data_pro2;

#define ITT_CAMERA      0x01
#define PROCESS_UNIT    0X02
#define EXTENSION_UNIT  0X03


static const char *const TAG = "TUNING_IO";
/**
  * @brief  Initialize UVC control parameters
  *         Initialize default/min/max/current values for
  *         Processing Unit and Extension Unit controls.
  * @retval 0 on success
  */


typedef unsigned char BYTE;
static BYTE subcmd_cur = 0;
static unsigned char get_test_64[USBD_UVC_UserDefineLength];
unsigned char *reg_buf_ab = NULL;
unsigned char *reg_buf_cd = NULL;
static int g_bIsQualify = 0;
static int g_bIsGetDataFinished = 0;
#define DBG_USB_CAMERA_ID               "AMEBAPRO2"

#define dbg_DiagPrintf //DiagPrintf
#define dbg_printf //printf

int show_log = 1;
static int dbg_log_flag = 0x0;//0xFFFFFFFF;

extern void isp_ctrl_enable_log(int enable);

unsigned char g_uvcd_ext_errcode[USBD_UVC_UserDefineLength] = {0};
static void print_isp_data(char *func, struct ExtUnitCmd *isp_cmd)
{
	struct ExtUnitCmd *cmd = isp_cmd;
	dbg_DiagPrintf("[%s] byCmd    : 0x%X\r\n", func, cmd->byCmd);
	dbg_DiagPrintf("[%s] bySubCmd : 0x%X\r\n", func, cmd->bySubCmd);
	dbg_DiagPrintf("[%s] byarAddr : 0x%X\r\n", func, cmd->byarAddr);
	//dbg_DiagPrintf("[%s] byarLen  : 0x%X\r\n", func, cmd->byarLen);
	//dbg_DiagPrintf("[%s] byarAddrH: 0x%X\r\n", func, cmd->byarAddrH);
	//dbg_DiagPrintf("[%s] offset   : 0x%X\r\n", func, cmd->offset);
	dbg_DiagPrintf("[%s] [tick:%lu] \r\n", __func__, (unsigned long)xTaskGetTickCountFromISR());

	return;
}

void usbd_set_errorcode(void *buf)
{
	int *err = (int *)g_uvcd_ext_errcode;
	*err = *((int *)buf);
	dbg_printf("[%s] error code = %d.\r\n", __FUNCTION__, *err);
}
void usbd_set_log_level(int level)
{
	dbg_log_flag = level;
}

void usbd_ext_init(void)
{
	//isp_ctrl_enable_log(1);
	DiagPrintf("[%s][L:%d] initialization of extension unit handle\r\n", __func__, __LINE__);

	if (reg_buf_ab != NULL) {
		free(reg_buf_ab);
	}
	reg_buf_ab = malloc(TUNING_IQ_MAX_SIZE);

	if (reg_buf_cd != NULL) {
		free(reg_buf_cd);
	}
	reg_buf_cd = malloc(TUNING_IQ_MAX_SIZE);

	tuning_init();
}

void usbd_ext_deinit()
{
	if (reg_buf_ab != NULL) {
		free(reg_buf_ab);
	}
	reg_buf_ab = NULL;

	if (reg_buf_cd != NULL) {
		free(reg_buf_cd);
	}
	reg_buf_cd = NULL;

}

/* void get_command_process_unit(struct uvc_dev *dev, struct usb_ctrlrequest *ctrl, struct uvc_request_data *resp)
{
} */

void get_command_camera(struct uvc_dev *dev, struct usb_ctrlrequest *ctrl, struct uvc_request_data *resp)
{
}




/* void set_command_process_unit(struct uvc_dev *dev, struct uvc_request_data *data)
{
} */

void set_command_camera(struct uvc_dev *dev, struct uvc_request_data *data)
{
}

#include "FreeRTOS.h"
#include "task.h"
extern struct tuning_cmd_uvc *g_cmd_uvc;
extern unsigned char *g_reg_buf;
extern int g_isTuningDataReady;
void update_ptr(struct tuning_cmd_uvc *cmd_uvc, unsigned char *reg_buf)
{
	g_cmd_uvc = cmd_uvc;
	g_reg_buf = reg_buf;
}
void do_tune(void *param)
{
	do_tuning_uvc(NULL, NULL);
	vTaskDelete(NULL);
	return;
}
void tuning_task(int cmd)
{
	if (0) { //cmd == 0xC2) {
		do_tuning_uvc(NULL, NULL);
	} else {
		if (xTaskCreate(do_tune, ((const char *)"mmf2_video"), 4096, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
			printf("\r\n video_example_main: Create Task Error\n");
		}
	}
}


#if 1

void get_command_extension_unit(usbd_uvc_dev_t *dev, usb_setup_req_t *ctrl, usbd_uvc_request_data_t *resp)
{
	int i;
	//for (i = 0; i < 64; i++) {
	//	get_test_64[i] = i;
	//}
	if (dev->control == USBD_UVC_VendorCommand || dev->control == USBD_UVC_VendorData) {
		switch (ctrl->bRequest) {
		case USBD_UVC_SET_CUR:
			break;
		case USBD_UVC_GET_CUR: {
			if (reg_buf_ab != NULL) { //show_log!=0)
				i = isp_data_pro2.bySubCmd;
				if ((i & 0xC0) == 0xC0) {
					if (!g_bIsQualify) {
						if (g_bIsGetDataFinished) { //---Once! Do the event before getting data!
							//update_ptr((struct tuning_cmd_uvc *)&isp_data_pro2, reg_buf_ab);
							//tuning_task();
							g_bIsGetDataFinished = 0;
						}
						if (i == UVC_ERRCODE_READ) {
							int *err = (int *)g_uvcd_ext_errcode;
							memcpy(resp->data, g_uvcd_ext_errcode, ctrl->wLength);
						} else {
							memcpy(resp->data, reg_buf_ab + isp_data_pro2.offset, ctrl->wLength);
							if (g_isTuningDataReady == 0) {
								dbg_DiagPrintf("[%s][L:%d][uvc_dbg][tick:%d]  (AB)data is not_ready \r\n", __func__, __LINE__, (unsigned int)xTaskGetTickCountFromISR());
							}
							if (isp_data_pro2.offset % 800 == 0) {
								//dbg_DiagPrintf("[%s][L:%d][uvc_dbg][tick:%d]  (AB)isp_data_pro2.offset:0x%X(%d)  ctrl->wLength:%d  reg_buf_ab:0x%X\r\n", __func__, __LINE__, (unsigned int)xTaskGetTickCountFromISR(), isp_data_pro2.offset, isp_data_pro2.offset, ctrl->wLength, reg_buf_ab);
							}
							isp_data_pro2.offset += ctrl->wLength;
							int offset = ((isp_data_pro2.offset >> 6) << 6);
							uint32_t len = (isp_data_pro2.byarLen + (isp_data_pro2.byReserved << 16));
							if (i == UVC_CUSTOMIZED_GET_CMD || i == UVC_CUSTOMIZED_GET_SIZE_CMD) {
								if (isp_data_pro2.offset >= len) {
									memset(&isp_data_pro2, 0, sizeof(struct ExtUnitCmd));
								}
							}
						}
					} else {
						memcpy(resp->data, DBG_USB_CAMERA_ID, ctrl->wLength);
					}
				}
			} else {
				//if (dev->uvcd_ext_get_cb) {
				//	(dev->uvcd_ext_get_cb)(resp->data, ctrl->wLength);
				//} else {
				//	memcpy(resp->data, (unsigned char *)&x_data.cur, sizeof(int));
				//}
			}
			break;
		}
		case USBD_UVC_GET_MIN:
			memcpy(resp->data, (unsigned char *)&x_data.min, sizeof(int));
			break;
		case USBD_UVC_GET_MAX:
			memcpy(resp->data, (unsigned char *)&x_data.max, sizeof(int));
			break;
		case USBD_UVC_GET_DEF:
			memcpy(resp->data, (unsigned char *)&x_data.def, sizeof(int));
			break;
		case USBD_UVC_GET_RES:
			memcpy(resp->data, (unsigned char *)&x_data.res, sizeof(int));
			break;
		case USBD_UVC_GET_LEN:
			memcpy(resp->data, (unsigned char *)&x_data.len, sizeof(int));
			break;
		case USBD_UVC_GET_INFO:
			memcpy(resp->data, (unsigned char *)&x_data.info, sizeof(int));
			break;
		}
	} else if (dev->control == USBD_UVC_UserDefineCommand || dev->control == USBD_UVC_UserDefineData) {
		switch (ctrl->bRequest) {
		case USBD_UVC_SET_CUR:
			break;
		case USBD_UVC_GET_CUR: {
			if (reg_buf_cd != NULL) { //show_log!=0)
				i = isp_data_pro2.bySubCmd;
				if ((i & 0xC0) == 0xC0) {
					if (!g_bIsQualify) {
						if (g_bIsGetDataFinished) { //---Once! Do the event before getting data!
							//update_ptr((struct tuning_cmd_uvc *)&isp_data_pro2, reg_buf_cd);
							//tuning_task();
							g_bIsGetDataFinished = 0;
						}
						if (i == UVC_ERRCODE_READ) {
							int *err = (int *)g_uvcd_ext_errcode;
							memcpy(resp->data, g_uvcd_ext_errcode, ctrl->wLength);
						} else {
							memcpy(resp->data, reg_buf_cd + isp_data_pro2.offset, ctrl->wLength);
							if (g_isTuningDataReady == 0) {
								dbg_DiagPrintf("[%s][L:%d][uvc_dbg][tick:%d]  (CD)data is not_ready \r\n", __func__, __LINE__, (unsigned int)xTaskGetTickCountFromISR());
							}
							if (isp_data_pro2.offset % 800 == 0) {
								dbg_DiagPrintf("[%s][L:%d][uvc_dbg][tick:%d]  (CD)isp_data_pro2.offset:0x%X(%d)  ctrl->wLength:%d\r\n", __func__, __LINE__,
											   (unsigned int)xTaskGetTickCountFromISR(), isp_data_pro2.offset, isp_data_pro2.offset, ctrl->wLength);
							}
							isp_data_pro2.offset += ctrl->wLength;
							int offset = ((isp_data_pro2.offset >> 6) << 6);
							uint32_t len = (isp_data_pro2.byarLen + (isp_data_pro2.byReserved << 16));
							if (i == UVC_CUSTOMIZED_GET_CMD || i == UVC_CUSTOMIZED_GET_SIZE_CMD) {
								if (isp_data_pro2.offset >= len) {
									memset(&isp_data_pro2, 0, sizeof(struct ExtUnitCmd));
								}
							}
						}

					} else {
						memcpy(resp->data, DBG_USB_CAMERA_ID, ctrl->wLength);
					}
				}
			}
			break;
		}
		case USBD_UVC_GET_MIN:
			memcpy(resp->data, (unsigned char *)&x_data.min, sizeof(int));
			break;
		case USBD_UVC_GET_MAX:
			memcpy(resp->data, (unsigned char *)&x_data.max, sizeof(int));
			break;
		case USBD_UVC_GET_DEF:
			memcpy(resp->data, (unsigned char *)&x_data.def, sizeof(int));
			break;
		case USBD_UVC_GET_RES:
			memcpy(resp->data, (unsigned char *)&x_data.res, sizeof(int));
			break;
		case USBD_UVC_GET_LEN:
			memcpy(resp->data, (unsigned char *)&x_data.len, sizeof(int));
			break;
		case USBD_UVC_GET_INFO:
			memcpy(resp->data, (unsigned char *)&x_data.info, sizeof(int));
			break;
		}
	}
}

void set_command_extension_unit(usbd_uvc_dev_t *dev, usbd_uvc_request_data_t *data)
{
	int i;
	dbg_printf("[UVCD_SET]%X %d.\r\n", dev->control, data->length);
	print_isp_data(__func__, (struct ExtUnitCmd *)data->data);
	if (reg_buf_ab == NULL) {
		dbg_printf("[UVCD_SET] --------------reg_buf_ab=NULL----------------.\r\n", dev->control, data->length);
		//if (dev->control == USBD_UVC_VendorCommand || dev->control == USBD_UVC_VendorData) {
		//	if (dev->uvcd_ext_set_cb) {
		//		(dev->uvcd_ext_set_cb)(data->data, data->length);
		//	}
		//}
		return;
	}

	if (dev->control == USBD_UVC_VendorCommand) {
		dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X\r\n", __func__, __LINE__, dev->control);
		if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
			dbg_printf("set ext command.\r\n");
		}
		struct ExtUnitCmd *tcmd = (struct ExtUnitCmd *)data->data;
		subcmd_cur = tcmd->bySubCmd;
		short waddr = tcmd->byarAddr;
		int ext_stru_size = sizeof(struct ExtUnitCmd);
		//if(subcmd_cur!=0xF0 && subcmd_cur!=0xF1 && subcmd_cur!=0xF2 && subcmd_cur!=0xF4 && subcmd_cur!=0xF5 && subcmd_cur!=0xF6 && subcmd_cur!=0xF7)
		//if(subcmd_cur!=0xC2 && subcmd_cur!=0x82 && subcmd_cur!=0xCA && subcmd_cur!=0x8A && subcmd_cur!=0xB5 && subcmd_cur!=0xB6 && subcmd_cur!=0xFF)
		//if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS)
		//	dbg_printf("[EXT64_S]bySubCmd:0x%02X byarAddr:0x%04X byarLen:%d  set_len=%d,cs=0x%02X.\r\n",
		//	   tcmd->bySubCmd, tcmd->byarAddr, tcmd->byarLen, data->length, dev->control);

		memcpy(&isp_data_pro2, tcmd, 8);
		//print_isp_data(__func__, &isp_data_pro2);
		isp_data_pro2.offset = 0;//To record the sequency data count
		i = isp_data_pro2.bySubCmd;
		//if(i==0xC2 || i==0xC3 || i==0xF5 || i==0xF6 || i==0xF7 || i==0xF8 || i==0xF0 || i==0xF1 || i==0xF2 || i==0xF3 || i==0xF9)
		if ((i & 0xC0) == 0xC0) {
			g_bIsGetDataFinished = 1;
			memset(reg_buf_ab, 0, 64);
			update_ptr((struct tuning_cmd_uvc *)&isp_data_pro2, reg_buf_ab);
			tuning_task(i);
		}
	} else if (dev->control == USBD_UVC_VendorData) {
		dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X\r\n", __func__, __LINE__, dev->control);
		//if(subcmd_cur!=0xF0 && subcmd_cur!=0xF1 && subcmd_cur!=0xF2 && subcmd_cur!=0xF4 && subcmd_cur!=0xF5 && subcmd_cur!=0xF6 && subcmd_cur!=0xF7)
		//if(subcmd_cur!=0xC2 && subcmd_cur!=0x82 && subcmd_cur!=0xCA && subcmd_cur!=0x8A && subcmd_cur!=0xB5 && subcmd_cur!=0xB6 && subcmd_cur!=0xFF)
		if (1) {//dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
			for (int i = 0; i < (data->length + 7) / 8; i++)
				dbg_printf("[EXT64_S]data: 0x%02X  0x%02X  0x%02X  0x%02X  0x%02X  0x%02X  0x%02X  0x%02X.\r\n",
						   data->data[i * 8 + 0], data->data[i * 8 + 1], data->data[i * 8 + 2], data->data[i * 8 + 3], data->data[i * 8 + 4], data->data[i * 8 + 5], data->data[i * 8 + 6],
						   data->data[i * 8 + 7]);
		}
		if (1) {//(isp_data_pro2.bySubCmd & 0x80) == 0x80) {
			memcpy(reg_buf_ab + isp_data_pro2.offset, data->data, data->length);
			isp_data_pro2.offset += data->length;
			if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
				dbg_printf("set ext data: offset %d/%d.\r\n", isp_data_pro2.offset, isp_data_pro2.byarLen);
			}

			uint32_t len;
			if (isp_data_pro2.bySubCmd == UVC_ISP_IQ_FW_WRITE || isp_data_pro2.bySubCmd == UVC_ISP_IQ_FW_READ) {
				len = (isp_data_pro2.byarLen + (isp_data_pro2.byReserved << 16));
				if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
					struct ExtUnitCmd *pro2_cmd = &isp_data_pro2;
					dbg_printf("[%s] SubCmd:0x%X  cmd_uvc->byarLen: %d cmd_uvc->byReserved: %d\r\n", __FUNCTION__, pro2_cmd->bySubCmd, pro2_cmd->byarLen, pro2_cmd->byReserved);
				}
			} else {
				len = (isp_data_pro2.byarLen);
			}
			if (isp_data_pro2.offset >= len && len > 0) {
				i = isp_data_pro2.bySubCmd;
				//if(i==0x82 || i==0x83 || i==0xB5 || i==0xB6 || i==0xB8)
				if ((i & 0xC0) == 0x80) {
					//memset(reg_buf_ab, 0, 64);
					update_ptr((struct tuning_cmd_uvc *)&isp_data_pro2, reg_buf_ab);
					tuning_task(i);
					//memset(&isp_data_pro2, 0, sizeof(struct ExtUnitCmd));
				}
			}
		}
	} else if (dev->control == USBD_UVC_UserDefineCommand) {
		dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X\r\n", __func__, __LINE__, dev->control);
		if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
			dbg_printf("set ext command.\r\n");
		}
		struct ExtUnitCmd *tcmd = (struct ExtUnitCmd *)data->data;
		subcmd_cur = tcmd->bySubCmd;
		short waddr = tcmd->byarAddr;
		int ext_stru_size = sizeof(struct ExtUnitCmd);
		//if(subcmd_cur!=0xF0 && subcmd_cur!=0xF1 && subcmd_cur!=0xF2 && subcmd_cur!=0xF4 && subcmd_cur!=0xF5 && subcmd_cur!=0xF6 && subcmd_cur!=0xF7)
		//if(subcmd_cur!=0xC2 && subcmd_cur!=0x82 && subcmd_cur!=0xCA && subcmd_cur!=0x8A && subcmd_cur!=0xB5 && subcmd_cur!=0xB6 && subcmd_cur!=0xFF)
		//if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS)
		//	dbg_printf("[EXT64_S]bySubCmd:0x%02X byarAddr:0x%04X byarLen:%d  set_len=%d,cs=0x%02X.\r\n",
		//		   tcmd->bySubCmd, tcmd->byarAddr, tcmd->byarLen, data->length, dev->control);

		memcpy(&isp_data_pro2, tcmd, 8);
		//print_isp_data(__func__, &isp_data_pro2);

		isp_data_pro2.offset = 0;//To record the sequency data count
		i = isp_data_pro2.bySubCmd;
		//if(i==0xC2 || i==0xC3 || i==0xF5 || i==0xF6 || i==0xF7 || i==0xF8 || i==0xF0 || i==0xF1 || i==0xF2 || i==0xF3 || i==0xF9)
		if ((i & 0xC0) == 0xC0) {
			memset(reg_buf_cd, 0, 64);
			update_ptr((struct tuning_cmd_uvc *)&isp_data_pro2, reg_buf_cd);
			tuning_task(i);
			g_bIsGetDataFinished = 1;
		}
	} else if (dev->control == USBD_UVC_UserDefineData) {
		dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X\r\n", __func__, __LINE__, dev->control);
		//if(subcmd_cur!=0xF0 && subcmd_cur!=0xF1 && subcmd_cur!=0xF2 && subcmd_cur!=0xF4 && subcmd_cur!=0xF5 && subcmd_cur!=0xF6 && subcmd_cur!=0xF7)
		//if(subcmd_cur!=0xC2 && subcmd_cur!=0x82 && subcmd_cur!=0xCA && subcmd_cur!=0x8A && subcmd_cur!=0xB5 && subcmd_cur!=0xB6 && subcmd_cur!=0xFF)
		//if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
		for (int i = 0; i < (data->length + 7) / 8; i++)
			dbg_printf("[EXT64_S]data: 0x%02X  0x%02X  0x%02X  0x%02X  0x%02X  0x%02X  0x%02X  0x%02X.\r\n",
					   data->data[i * 8 + 0], data->data[i * 8 + 1], data->data[i * 8 + 2], data->data[i * 8 + 3], data->data[i * 8 + 4], data->data[i * 8 + 5], data->data[i * 8 + 6],
					   data->data[i * 8 + 7]);
		//}
		if (1) {//(isp_data_pro2.bySubCmd & 0x80) == 0x80) {
			memcpy(reg_buf_cd + isp_data_pro2.offset, data->data, data->length);
			isp_data_pro2.offset += data->length;
			if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
				dbg_printf("set ext data: offset %d/%d.\r\n", isp_data_pro2.offset, isp_data_pro2.byarLen);
			}

			uint32_t len;
			if (isp_data_pro2.bySubCmd == UVC_ISP_IQ_FW_WRITE || isp_data_pro2.bySubCmd == UVC_ISP_IQ_FW_READ) {
				len = (isp_data_pro2.byarLen + (isp_data_pro2.byReserved << 16));
				if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
					struct ExtUnitCmd *pro2_cmd = &isp_data_pro2;
					dbg_printf("[%s] SubCmd:0x%X  cmd_uvc->byarLen: %d cmd_uvc->byReserved: %d\r\n", __FUNCTION__, pro2_cmd->bySubCmd, pro2_cmd->byarLen, pro2_cmd->byReserved);
				}
			} else {
				len = (isp_data_pro2.byarLen);
			}
			dbg_printf("[%s]isp_data_pro2.offset = %d  len = %d.\r\n", __FUNCTION__, isp_data_pro2.offset, len);
			if (isp_data_pro2.offset >= len && len > 0) {
				i = isp_data_pro2.bySubCmd;
				//if(i==0x82 || i==0x83 || i==0xB5 || i==0xB6 || i==0xB8)
				if ((i & 0xC0) == 0x80) {
					//memset(reg_buf_cd, 0, 64);
					update_ptr((struct tuning_cmd_uvc *)&isp_data_pro2, reg_buf_cd);
					tuning_task(i);
					//memset(&isp_data_pro2, 0, sizeof(struct ExtUnitCmd));
				}
			}
		}
	}
}

#else //Add tuning-server

void get_command_extension_unit(usbd_uvc_dev_t *dev, usb_setup_req_t *ctrl, usbd_uvc_request_data_t *resp)
{
	int i;
	//}else if(ctrl->wIndex == EXTENSION_UNIT){
#if 1 //PRO3 log turn-off
	dbg_printf("[UVCD_GET]%X %d. req %d\r\n", dev->control, ctrl->wLength, ctrl->bRequest);
#endif
	//for (i = 0; i < 64; i++) {
	//	get_test_64[i] = i;
	//}
	if (dev->control == USBD_UVC_VendorCommand || dev->control == USBD_UVC_VendorData) {
		switch (ctrl->bRequest) {
		case USBD_UVC_SET_CUR:
//			dbg_printf("[UVCD_GET]%X %d. req %d\r\n", dev->control, ctrl->wLength,ctrl->bRequest);
			print_isp_data(__func__, &isp_data_pro2);
			break;
		case USBD_UVC_GET_CUR: {
//			dbg_printf("[UVCD_GET]%X %d. req %d\r\n", dev->control, ctrl->wLength,ctrl->bRequest);
//			print_isp_data(__func__, &isp_data_pro2);
			dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X  len:%d  req:%d\r\n", __func__, __LINE__, dev->control, ctrl->wLength, ctrl->bRequest);
			if (reg_buf_ab != NULL) { //show_log!=0)
				dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X  len:%d  req:%d\r\n", __func__, __LINE__, dev->control, ctrl->wLength, ctrl->bRequest);
				//if(subcmd_cur!=0xF0 && subcmd_cur!=0xF1 && subcmd_cur!=0xF2 && subcmd_cur!=0xF4 && subcmd_cur!=0xF5 && subcmd_cur!=0xF6 && subcmd_cur!=0xF7)
				//	if(subcmd_cur!=0xC2 && subcmd_cur!=0x82 && subcmd_cur!=0xCA && subcmd_cur!=0x8A && subcmd_cur!=0xB5 && subcmd_cur!=0xB6 && subcmd_cur!=0xFF)
				if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
					dbg_printf("[EXT64_G] uvc_extension_unit, get_length =%d, cs = 0x%02X\r\n", ctrl->wLength, dev->control);
				}
				i = isp_data_pro2.bySubCmd;
				if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
					dbg_printf("[PRO2_DBG] isp_data_pro2.bySubCmd=0x%02X.\r\n", i);
				}
				//if(i==0xC2 || i==0xC3 || i==0xF5 || i==0xF6 || i==0xF7 || i==0xF8 || i==0xF0 || i==0xF1 || i==0xF2 || i==0xF3 || i==0xF9)
				dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X  len:%d  req:%d\r\n", __func__, __LINE__, dev->control, ctrl->wLength, ctrl->bRequest);
				if ((i & 0xC0) == 0xC0) {
					if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
						dbg_printf("[PRO2_DBG] g_bIsQualify=%d.\r\n", g_bIsQualify);
					}
					if (!g_bIsQualify) {
						if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
							dbg_printf("[PRO2_DBG] g_bIsGetDataFinished=%d.\r\n", g_bIsGetDataFinished);
						}
						dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X  len:%d  req:%d\r\n", __func__, __LINE__, dev->control, ctrl->wLength, ctrl->bRequest);
						if (g_bIsGetDataFinished) { //---Once! Do the event before getting data!
							dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X  len:%d  req:%d\r\n", __func__, __LINE__, dev->control, ctrl->wLength, ctrl->bRequest);
							memset(reg_buf_ab, 0, 64);
							update_ptr((struct tuning_cmd_uvc *)&isp_data_pro2, reg_buf_ab);
							tuning_task(i);
							dbg_printf("reg_buf_ab 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X.\r\n", reg_buf_ab[0], reg_buf_ab[1], reg_buf_ab[2], reg_buf_ab[3], reg_buf_ab[4], reg_buf_ab[5],
									   reg_buf_ab[6], reg_buf_ab[7]);

							if (i == UVC_ISP_IQ_TABLE_SIZE || i == UVC_ISP_ALGO_PARAM_SIZE || i == UVC_ISP_STATIS_SIZE || i == UVC_ISP_IQ_FW_SIZE) {
								int *dtemp = (int *)reg_buf_ab;
								if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
									dbg_printf("table size:%d.\r\n", *dtemp);
								}
							}
							g_bIsGetDataFinished = 0;
						}
						if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
							dbg_printf("[PRO2_DBG] Get data.\r\n");
						}
						if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_DATADUMP) {
							//if (i == UVC_ISP_IQ_TABLE_READ || i == UVC_ISP_ALGO_PARAM_READ || i == UVC_ISP_STATIS || i == UVC_ISP_IQ_FW_READ) {
							//	for (i = 0; i < ctrl->wLength; i++) {
							//		if (i > 0 && i % 16 == 0) {
							//			dbg_printf("\r\n");
							//		}
							//		dbg_printf("0x%02X,", reg_buf_ab[isp_data_pro2.offset + i]);
							//	}
							//	dbg_printf("\r\n");
							//}
						}
						if (i == UVC_ERRCODE_READ) {
							int *err = (int *)g_uvcd_ext_errcode;
							dbg_printf("[%s] error code = %d.\r\n", __FUNCTION__, *err);
							memcpy(resp->data, g_uvcd_ext_errcode, ctrl->wLength);
							dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X  len:%d  req:%d\r\n", __func__, __LINE__, dev->control, ctrl->wLength, ctrl->bRequest);
						} else {
							memcpy(resp->data, reg_buf_ab + isp_data_pro2.offset, ctrl->wLength);
							isp_data_pro2.offset += ctrl->wLength;
							//if (i == UVC_ISP_IQ_FW_READ) {
							//	if (isp_data_pro2.offset >= *((int *)reg_buf_ab)) {
							//		dbg_printf("\r\n");
							//	}
							//}
							int offset = ((isp_data_pro2.offset >> 6) << 6);
							//if ((offset % 1024) == 0) {
							//	dbg_printf("0x%02X ", resp->data[0]);
							//	if (((offset >> 10) % 8) == 0) {
							//		dbg_printf("\r\n");
							//	}
							//}
							uint32_t len = (isp_data_pro2.byarLen + (isp_data_pro2.byReserved << 16));
							if (i == UVC_CUSTOMIZED_GET_CMD || i == UVC_CUSTOMIZED_GET_SIZE_CMD) {
								if (isp_data_pro2.offset >= len) {
									memset(&isp_data_pro2, 0, sizeof(struct ExtUnitCmd));
								}
							}
							dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X  len:%d  req:%d\r\n", __func__, __LINE__, dev->control, ctrl->wLength, ctrl->bRequest);
						}
						dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X  len:%d  req:%d\r\n", __func__, __LINE__, dev->control, ctrl->wLength, ctrl->bRequest);

					} else {
						dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X  len:%d  req:%d\r\n", __func__, __LINE__, dev->control, ctrl->wLength, ctrl->bRequest);
						memcpy(resp->data, DBG_USB_CAMERA_ID, ctrl->wLength);
					}
				}
				dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X  len:%d  req:%d\r\n", __func__, __LINE__, dev->control, ctrl->wLength, ctrl->bRequest);
			} else {
				dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X  len:%d  req:%d\r\n", __func__, __LINE__, dev->control, ctrl->wLength, ctrl->bRequest);
				//if (dev->uvcd_ext_get_cb) {
				//	(dev->uvcd_ext_get_cb)(resp->data, ctrl->wLength);
				//} else {
				//	memcpy(resp->data, (unsigned char *)&x_data.cur, sizeof(int));
				//}
			}
			break;
		}
		case USBD_UVC_GET_MIN:
			memcpy(resp->data, (unsigned char *)&x_data.min, sizeof(int));
			break;
		case USBD_UVC_GET_MAX:
			memcpy(resp->data, (unsigned char *)&x_data.max, sizeof(int));
			break;
		case USBD_UVC_GET_DEF:
			memcpy(resp->data, (unsigned char *)&x_data.def, sizeof(int));
			break;
		case USBD_UVC_GET_RES:
			memcpy(resp->data, (unsigned char *)&x_data.res, sizeof(int));
			break;
		case USBD_UVC_GET_LEN:
			memcpy(resp->data, (unsigned char *)&x_data.len, sizeof(int));
			break;
		case USBD_UVC_GET_INFO:
			memcpy(resp->data, (unsigned char *)&x_data.info, sizeof(int));
			break;
		}
	} else if (dev->control == USBD_UVC_UserDefineCommand || dev->control == USBD_UVC_UserDefineData) {
		switch (ctrl->bRequest) {
		case USBD_UVC_SET_CUR:
			break;
		case USBD_UVC_GET_CUR: {
			dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X  len:%d  req:%d\r\n", __func__, __LINE__, dev->control, ctrl->wLength, ctrl->bRequest);
			if (reg_buf_cd != NULL) { //show_log!=0)
				//if(subcmd_cur!=0xF0 && subcmd_cur!=0xF1 && subcmd_cur!=0xF2 && subcmd_cur!=0xF4 && subcmd_cur!=0xF5 && subcmd_cur!=0xF6 && subcmd_cur!=0xF7)
				//	if(subcmd_cur!=0xC2 && subcmd_cur!=0x82 && subcmd_cur!=0xCA && subcmd_cur!=0x8A && subcmd_cur!=0xB5 && subcmd_cur!=0xB6 && subcmd_cur!=0xFF)
				if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
					dbg_printf("[EXT64_G] uvc_extension_unit, get_length =%d, cs = 0x%02X\r\n", ctrl->wLength, dev->control);
				}
				i = isp_data_pro2.bySubCmd;
				if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
					dbg_printf("[PRO2_DBG] isp_data_pro2.bySubCmd=0x%02X.\r\n", i);
				}
				//if(i==0xC2 || i==0xC3 || i==0xF5 || i==0xF6 || i==0xF7 || i==0xF8 || i==0xF0 || i==0xF1 || i==0xF2 || i==0xF3 || i==0xF9)
				dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X  len:%d  req:%d\r\n", __func__, __LINE__, dev->control, ctrl->wLength, ctrl->bRequest);
				if ((i & 0xC0) == 0xC0) {
					dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X  len:%d  req:%d\r\n", __func__, __LINE__, dev->control, ctrl->wLength, ctrl->bRequest);
					if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
						dbg_printf("[PRO2_DBG] g_bIsQualify=%d.\r\n", g_bIsQualify);
					}
					if (!g_bIsQualify) {
						if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
							dbg_printf("[PRO2_DBG] g_bIsGetDataFinished=%d.\r\n", g_bIsGetDataFinished);
						}
						dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X  len:%d  req:%d\r\n", __func__, __LINE__, dev->control, ctrl->wLength, ctrl->bRequest);
						if (g_bIsGetDataFinished) { //---Once! Do the event before getting data!
							dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X  len:%d  req:%d\r\n", __func__, __LINE__, dev->control, ctrl->wLength, ctrl->bRequest);
							memset(reg_buf_cd, 0, 64);
							update_ptr((struct tuning_cmd_uvc *)&isp_data_pro2, reg_buf_cd);
							tuning_task(i);
							if (i == UVC_ISP_IQ_TABLE_SIZE || i == UVC_ISP_ALGO_PARAM_SIZE || i == UVC_ISP_STATIS_SIZE || i == UVC_ISP_IQ_FW_SIZE) {
								int *dtemp = (int *)reg_buf_cd;
								if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
									dbg_printf("table size:%d.\r\n", *dtemp);
								}
							}
							g_bIsGetDataFinished = 0;
						}
						if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
							dbg_printf("[PRO2_DBG] Get data.\r\n");
						}
						if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_DATADUMP) {
							//if (i == UVC_ISP_IQ_TABLE_READ || i == UVC_ISP_ALGO_PARAM_READ || i == UVC_ISP_STATIS || i == UVC_ISP_IQ_FW_READ) {
							//	for (i = 0; i < ctrl->wLength; i++) {
							//		if (i > 0 && i % 16 == 0) {
							//			dbg_printf("\r\n");
							//		}
							//		dbg_printf("0x%02X,", reg_buf_cd[isp_data_pro2.offset + i]);
							//	}
							//	dbg_printf("\r\n");
							//}
						}
						dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X  len:%d  req:%d\r\n", __func__, __LINE__, dev->control, ctrl->wLength, ctrl->bRequest);
						if (i == UVC_ERRCODE_READ) {
							dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X  len:%d  req:%d\r\n", __func__, __LINE__, dev->control, ctrl->wLength, ctrl->bRequest);
							int *err = (int *)g_uvcd_ext_errcode;
							dbg_printf("[%s] error code = %d.\r\n", __FUNCTION__, *err);
							memcpy(resp->data, g_uvcd_ext_errcode, ctrl->wLength);
						} else {
							memcpy(resp->data, reg_buf_cd + isp_data_pro2.offset, ctrl->wLength);
							isp_data_pro2.offset += ctrl->wLength;
							//if (i == UVC_ISP_IQ_FW_READ) {
							//	if (isp_data_pro2.offset >= *((int *)reg_buf_cd)) {
							//		dbg_printf("\r\n");
							//	}
							//}
							int offset = ((isp_data_pro2.offset >> 6) << 6);
							//if ((offset % 1024) == 0) {
							//	dbg_printf("0x%02X ", resp->data[0]);
							//	if (((offset >> 10) % 8) == 0) {
							//		dbg_printf("\r\n");
							//	}
							//}
							uint32_t len = (isp_data_pro2.byarLen + (isp_data_pro2.byReserved << 16));
							if (i == UVC_CUSTOMIZED_GET_CMD || i == UVC_CUSTOMIZED_GET_SIZE_CMD) {
								if (isp_data_pro2.offset >= len) {
									memset(&isp_data_pro2, 0, sizeof(struct ExtUnitCmd));
								}
							}
							dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X  len:%d  req:%d\r\n", __func__, __LINE__, dev->control, ctrl->wLength, ctrl->bRequest);
						}

					} else {
						dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X  len:%d  req:%d\r\n", __func__, __LINE__, dev->control, ctrl->wLength, ctrl->bRequest);
						memcpy(resp->data, DBG_USB_CAMERA_ID, ctrl->wLength);
					}
				}
			}
			break;
		}
		case USBD_UVC_GET_MIN:
			memcpy(resp->data, (unsigned char *)&x_data.min, sizeof(int));
			break;
		case USBD_UVC_GET_MAX:
			memcpy(resp->data, (unsigned char *)&x_data.max, sizeof(int));
			break;
		case USBD_UVC_GET_DEF:
			memcpy(resp->data, (unsigned char *)&x_data.def, sizeof(int));
			break;
		case USBD_UVC_GET_RES:
			memcpy(resp->data, (unsigned char *)&x_data.res, sizeof(int));
			break;
		case USBD_UVC_GET_LEN:
			memcpy(resp->data, (unsigned char *)&x_data.len, sizeof(int));
			break;
		case USBD_UVC_GET_INFO:
			memcpy(resp->data, (unsigned char *)&x_data.info, sizeof(int));
			break;
		}
	}
}
void set_command_extension_unit(usbd_uvc_dev_t *dev, usbd_uvc_request_data_t *data)
{
	int i;
	dbg_printf("[UVCD_SET]%X %d.\r\n", dev->control, data->length);
	print_isp_data(__func__, (struct ExtUnitCmd *)data->data);
	if (reg_buf_ab == NULL) {
		dbg_printf("[UVCD_SET] --------------reg_buf_ab=NULL----------------.\r\n", dev->control, data->length);
		//if (dev->control == USBD_UVC_VendorCommand || dev->control == USBD_UVC_VendorData) {
		//	if (dev->uvcd_ext_set_cb) {
		//		(dev->uvcd_ext_set_cb)(data->data, data->length);
		//	}
		//}
		return;
	}

	if (dev->control == USBD_UVC_VendorCommand) {
		dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X\r\n", __func__, __LINE__, dev->control);
		if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
			dbg_printf("set ext command.\r\n");
		}
		struct ExtUnitCmd *tcmd = (struct ExtUnitCmd *)data->data;
		subcmd_cur = tcmd->bySubCmd;
		short waddr = tcmd->byarAddr;
		int ext_stru_size = sizeof(struct ExtUnitCmd);
		//if(subcmd_cur!=0xF0 && subcmd_cur!=0xF1 && subcmd_cur!=0xF2 && subcmd_cur!=0xF4 && subcmd_cur!=0xF5 && subcmd_cur!=0xF6 && subcmd_cur!=0xF7)
		//if(subcmd_cur!=0xC2 && subcmd_cur!=0x82 && subcmd_cur!=0xCA && subcmd_cur!=0x8A && subcmd_cur!=0xB5 && subcmd_cur!=0xB6 && subcmd_cur!=0xFF)
		//if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS)
		//	dbg_printf("[EXT64_S]bySubCmd:0x%02X byarAddr:0x%04X byarLen:%d  set_len=%d,cs=0x%02X.\r\n",
		//		   tcmd->bySubCmd, tcmd->byarAddr, tcmd->byarLen, data->length, dev->control);

		memcpy(&isp_data_pro2, tcmd, 8);
		//print_isp_data(__func__, &isp_data_pro2);
		isp_data_pro2.offset = 0;//To record the sequency data count
		i = isp_data_pro2.bySubCmd;
		//if(i==0xC2 || i==0xC3 || i==0xF5 || i==0xF6 || i==0xF7 || i==0xF8 || i==0xF0 || i==0xF1 || i==0xF2 || i==0xF3 || i==0xF9)
		if ((i & 0xC0) == 0xC0) {
			g_bIsGetDataFinished = 1;
		}
	} else if (dev->control == USBD_UVC_VendorData) {
		dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X\r\n", __func__, __LINE__, dev->control);
		//if(subcmd_cur!=0xF0 && subcmd_cur!=0xF1 && subcmd_cur!=0xF2 && subcmd_cur!=0xF4 && subcmd_cur!=0xF5 && subcmd_cur!=0xF6 && subcmd_cur!=0xF7)
		//if(subcmd_cur!=0xC2 && subcmd_cur!=0x82 && subcmd_cur!=0xCA && subcmd_cur!=0x8A && subcmd_cur!=0xB5 && subcmd_cur!=0xB6 && subcmd_cur!=0xFF)
		//if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
		//	for (int i = 0; i < (data->length + 7) / 8; i++)
		//		dbg_printf("[EXT64_S]data: 0x%02X  0x%02X  0x%02X  0x%02X  0x%02X  0x%02X  0x%02X  0x%02X.\r\n",
		//			   data->data[i * 8 + 0], data->data[i * 8 + 1], data->data[i * 8 + 2], data->data[i * 8 + 3], data->data[i * 8 + 4], data->data[i * 8 + 5], data->data[i * 8 + 6],
		//			   data->data[i * 8 + 7]);
		//}
		if (1) {//(isp_data_pro2.bySubCmd & 0x80) == 0x80) {
			memcpy(reg_buf_ab + isp_data_pro2.offset, data->data, data->length);
			isp_data_pro2.offset += data->length;
			if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
				dbg_printf("set ext data: offset %d/%d.\r\n", isp_data_pro2.offset, isp_data_pro2.byarLen);
			}

			uint32_t len;
			if (isp_data_pro2.bySubCmd == UVC_ISP_IQ_FW_WRITE || isp_data_pro2.bySubCmd == UVC_ISP_IQ_FW_READ) {
				len = (isp_data_pro2.byarLen + (isp_data_pro2.byReserved << 16));
				//if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
				//	struct ExtUnitCmd *pro2_cmd = &isp_data_pro2;
				//	dbg_printf("[%s] SubCmd:0x%X  cmd_uvc->byarLen: %d cmd_uvc->byReserved: %d\r\n", __FUNCTION__, pro2_cmd->bySubCmd, pro2_cmd->byarLen, pro2_cmd->byReserved);
				//}
			} else {
				len = (isp_data_pro2.byarLen);
			}
			if (isp_data_pro2.offset >= len && len > 0) {
				i = isp_data_pro2.bySubCmd;
				//if(i==0x82 || i==0x83 || i==0xB5 || i==0xB6 || i==0xB8)
				if ((i & 0xC0) == 0x80) {
					memset(reg_buf_ab, 0, 64);
					update_ptr((struct tuning_cmd_uvc *)&isp_data_pro2, reg_buf_ab);
					tuning_task(i);
					memset(&isp_data_pro2, 0, sizeof(struct ExtUnitCmd));
				}
			}
		}
	} else if (dev->control == USBD_UVC_UserDefineCommand) {
		dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X\r\n", __func__, __LINE__, dev->control);
		if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
			dbg_printf("set ext command.\r\n");
		}
		struct ExtUnitCmd *tcmd = (struct ExtUnitCmd *)data->data;
		subcmd_cur = tcmd->bySubCmd;
		short waddr = tcmd->byarAddr;
		int ext_stru_size = sizeof(struct ExtUnitCmd);
		//if(subcmd_cur!=0xF0 && subcmd_cur!=0xF1 && subcmd_cur!=0xF2 && subcmd_cur!=0xF4 && subcmd_cur!=0xF5 && subcmd_cur!=0xF6 && subcmd_cur!=0xF7)
		//if(subcmd_cur!=0xC2 && subcmd_cur!=0x82 && subcmd_cur!=0xCA && subcmd_cur!=0x8A && subcmd_cur!=0xB5 && subcmd_cur!=0xB6 && subcmd_cur!=0xFF)
		//if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS)
		//	dbg_printf("[EXT64_S]bySubCmd:0x%02X byarAddr:0x%04X byarLen:%d  set_len=%d,cs=0x%02X.\r\n",
		//		   tcmd->bySubCmd, tcmd->byarAddr, tcmd->byarLen, data->length, dev->control);

		memcpy(&isp_data_pro2, tcmd, 8);
		//print_isp_data(__func__, &isp_data_pro2);

		isp_data_pro2.offset = 0;//To record the sequency data count
		i = isp_data_pro2.bySubCmd;
		//if(i==0xC2 || i==0xC3 || i==0xF5 || i==0xF6 || i==0xF7 || i==0xF8 || i==0xF0 || i==0xF1 || i==0xF2 || i==0xF3 || i==0xF9)
		if ((i & 0xC0) == 0xC0) {
			g_bIsGetDataFinished = 1;
		}
	} else if (dev->control == USBD_UVC_UserDefineData) {
		dbg_DiagPrintf("[%s][L:%d] ext-dbg  ctrl:%X\r\n", __func__, __LINE__, dev->control);
		//if(subcmd_cur!=0xF0 && subcmd_cur!=0xF1 && subcmd_cur!=0xF2 && subcmd_cur!=0xF4 && subcmd_cur!=0xF5 && subcmd_cur!=0xF6 && subcmd_cur!=0xF7)
		//if(subcmd_cur!=0xC2 && subcmd_cur!=0x82 && subcmd_cur!=0xCA && subcmd_cur!=0x8A && subcmd_cur!=0xB5 && subcmd_cur!=0xB6 && subcmd_cur!=0xFF)
		//if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
		//	for (int i = 0; i < (data->length + 7) / 8; i++)
		//		dbg_printf("[EXT64_S]data: 0x%02X  0x%02X  0x%02X  0x%02X  0x%02X  0x%02X  0x%02X  0x%02X.\r\n",
		//			   data->data[i * 8 + 0], data->data[i * 8 + 1], data->data[i * 8 + 2], data->data[i * 8 + 3], data->data[i * 8 + 4], data->data[i * 8 + 5], data->data[i * 8 + 6],
		//			   data->data[i * 8 + 7]);
		//}
		if (1) {//(isp_data_pro2.bySubCmd & 0x80) == 0x80) {
			memcpy(reg_buf_cd + isp_data_pro2.offset, data->data, data->length);
			isp_data_pro2.offset += data->length;
			if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
				dbg_printf("set ext data: offset %d/%d.\r\n", isp_data_pro2.offset, isp_data_pro2.byarLen);
			}

			uint32_t len;
			if (isp_data_pro2.bySubCmd == UVC_ISP_IQ_FW_WRITE || isp_data_pro2.bySubCmd == UVC_ISP_IQ_FW_READ) {
				len = (isp_data_pro2.byarLen + (isp_data_pro2.byReserved << 16));
				if (dbg_log_flag & RTS_USBDEXT_LOG_MASK_IDX_STATUS) {
					struct ExtUnitCmd *pro2_cmd = &isp_data_pro2;
					dbg_printf("[%s] SubCmd:0x%X  cmd_uvc->byarLen: %d cmd_uvc->byReserved: %d\r\n", __FUNCTION__, pro2_cmd->bySubCmd, pro2_cmd->byarLen, pro2_cmd->byReserved);
				}
			} else {
				len = (isp_data_pro2.byarLen);
			}
			dbg_printf("[%s]isp_data_pro2.offset = %d  len = %d.\r\n", __FUNCTION__, isp_data_pro2.offset, len);
			if (isp_data_pro2.offset >= len && len > 0) {
				i = isp_data_pro2.bySubCmd;
				//if(i==0x82 || i==0x83 || i==0xB5 || i==0xB6 || i==0xB8)
				if ((i & 0xC0) == 0x80) {
					memset(reg_buf_cd, 0, 64);
					update_ptr((struct tuning_cmd_uvc *)&isp_data_pro2, reg_buf_cd);
					tuning_task(i);
					memset(&isp_data_pro2, 0, sizeof(struct ExtUnitCmd));
				}
			}
		}
	}

}

#endif

#endif //USBD_TUNING_MODE
