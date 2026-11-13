#ifndef _MODULE_VP2AP_H
#define _MODULE_VP2AP_H

#include <FreeRTOS.h>
#include <task.h>
#include <stdint.h>

#define CMD_VP2AP_SET_PARAMS            MM_MODULE_CMD(0x00)  // set parameter
#define CMD_VP2AP_GET_PARAMS            MM_MODULE_CMD(0x01)  // get parameter

#define CMD_VP2AP_APPLY                 MM_MODULE_CMD(0x10)

typedef void (*frm_process_t)(int ch, int type, void *buf, uint32_t size);

typedef struct vp2ap_param_s {
	uint32_t stream_id;
	uint32_t send_ap_off;
} vp2ap_params_t;

typedef struct vp2ap_ctx_s {
	void *parent;

	vp2ap_params_t params;

	frm_process_t frm_process_cb;

} vp2ap_ctx_t;

extern mm_module_t vp2ap_module;

#endif
