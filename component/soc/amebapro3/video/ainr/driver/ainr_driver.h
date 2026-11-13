#ifndef _AINR_DRIVER_H_
#define _AINR_DRIVER_H_

#include <stdint.h>

#define AINR_MAGIC "AINR"
#pragma pack(push, 1)
typedef struct {
	char     magic[4];      // Magic Word: "AINR"
	uint16_t width;
	uint16_t height;
	uint8_t  bayer_format;
	uint8_t  sensor_id;
	char     timestamp[22]; // "2026-04-25 02:44:06"
} ainr_header_t;
#pragma pack(pop)

typedef enum {
	AINR_CTRL_CMD_LOAD_MODEL,
	AINR_CTRL_CMD_SET_PARAM,
	AINR_CTRL_CMD_GET_PARAM,
	AINR_CTRL_CMD_GET_STATUS,
	AINR_CTRL_CMD_ENABLE,
	AINR_CTRL_CMD_DISABLE
} ainr_ctrl_cmd_t;

typedef enum {
	AINR_STATUS_UNINITIALIZED = 0,
	AINR_STATUS_MODEL_LOADED = 1,
	AINR_STATUS_PARAM_SET = 2,
	AINR_STATUS_ENABLED = 4,
	AINR_STATUS_FAILURE = 1 << 31
} ainr_status_t;

typedef enum {
	AINR_SUCCESS = 0,
	AINR_ERROR_INVALID_ARGUMENTS = -1,
	AINR_ERROR_FAILURE = -2,
	AINR_ERROR_MODEL_NOT_LOADED = -3,
	AINR_ERROR_PARAM_NOT_SET = -4
} ainr_error_t;

typedef struct {
	uint32_t width;
	uint32_t height;
	uint8_t pixfmt;
	uint32_t mode;
} ainr_param_t;

typedef struct {
	ainr_status_t status;
	ainr_param_t params;
} ainr_ctx;

typedef struct {
	const void *model_data;
	uint32_t model_size;
	uint32_t use_dma;
	double update_mixup_ratio;
} ainr_model_t;

ainr_ctx *ainr_driver_open(void);
int ainr_driver_close(ainr_ctx *ctx);
int ainr_driver_read(ainr_ctx *ctx, int *buf, int len);
int ainr_driver_write(ainr_ctx *ctx, int *buf, int len);
int ainr_driver_ctrl(ainr_ctx *ctx,  ainr_ctrl_cmd_t ctrl_cmd, void *arg);



#endif  // _AINR_DRIVER_H_
