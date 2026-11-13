/**
 ******************************************************************************
 * This file contains sensor configurations for AmebaPro platform
 ******************************************************************************
 */


#ifndef __SENSOR_H
#define __SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#define SENSOR_PARAM_COUNT    64
#define IQ_COUNT    8

struct sensor_info_t {
	unsigned int width;
	unsigned int height;
	unsigned int fps;
	unsigned int reserved[13];
};

typedef enum {
	SENSOR_FIXP,
	SENSOR_OS04A10,
	SENSOR_GC4023,
	SENSOR_SC485SL,
	SENSOR_SC485SLHDR,
	SENSOR_GC8613,
	SENSOR_MULTI,
} sensor_list_t;

struct sensor_params_t {
	unsigned int hdr;
	const struct rts_isp_sensor_ops *(*get_ops)(void);
	const void *iq_buf[IQ_COUNT];
};
extern const struct sensor_params_t sensor_params[SENSOR_PARAM_COUNT];
extern unsigned int SENSOR_USE;
extern unsigned int IQ_USE;

#ifdef __cplusplus
}
#endif


#endif /* __SENSOR_H */
