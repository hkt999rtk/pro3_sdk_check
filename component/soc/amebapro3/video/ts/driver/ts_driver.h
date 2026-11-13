#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "os_wrapper_specific.h"
#include "ameba_soc.h"
#include "ts_hal.h"

#ifndef TS_DRIVER_H
#define TS_DRIVER_H

typedef struct ts_drv_ctx_s {
	void *data;
	void *tmp;
} ts_drv_ctx_t;

typedef enum {
	TSDRV_CTRL_CONVERT_IF2OF = 0,
	TSDRV_CTRL_WAIT_END     = 1,
} ts_ctrl_cmd_t;

typedef struct ts_drv_s {
	rtos_sema_t xTSSemaphore;
} ts_drv_t;

ts_drv_ctx_t *ts_open(void);
int ts_close(ts_drv_ctx_t *ctx);
int ts_ctrl(ts_drv_ctx_t *ctx, ts_ctrl_cmd_t ctrl_cmd, void *arg);

#endif

