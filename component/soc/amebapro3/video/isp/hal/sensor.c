#include "sensor.h"
#include "prebuilt_iq.h"
#include "ameba_soc.h"

/* Forward declarations of sensor getter functions */
const struct rts_isp_sensor_ops *rts_isp_get_fixp_ops(void);
const struct rts_isp_sensor_ops *rts_isp_get_os04a10_ops(void);
const struct rts_isp_sensor_ops *rts_isp_get_gc4023_ops(void);
const struct rts_isp_sensor_ops *rts_isp_get_sc485sl_ops(void);
const struct rts_isp_sensor_ops *rts_isp_get_gc8613_ops(void);

const struct sensor_params_t sensor_params[SENSOR_PARAM_COUNT] = {
	[SENSOR_FIXP]         = {0, rts_isp_get_fixp_ops, {__isp_iq_default_table__}},
	[SENSOR_OS04A10]      = {0, rts_isp_get_os04a10_ops, {__isp_iq_default_table__}},
	[SENSOR_SC485SL]      = {0, rts_isp_get_sc485sl_ops, {__isp_iq_default_table__}},
	[SENSOR_SC485SLHDR]   = {1, rts_isp_get_sc485sl_ops, {__isp_iq_default_table__}},
	[SENSOR_GC4023]       = {0, rts_isp_get_gc4023_ops, {__isp_iq_default_table__}},
	[SENSOR_GC8613]       = {0, rts_isp_get_gc8613_ops, {__isp_iq_default_table__}},
	[SENSOR_MULTI]        = {0, rts_isp_get_os04a10_ops, {__isp_iq_default_table__}},
};

unsigned int SENSOR_USE = SENSOR_OS04A10;
unsigned int IQ_USE     = 0;


//#define SENSOR_USE SENSOR_SC485SLHDR
