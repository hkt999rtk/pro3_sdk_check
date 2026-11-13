#ifndef _ISP_SENSOR_H_INC_
#define _ISP_SENSOR_H_INC_

#include <rts_isp_sensor.h>

#define sensor_magic(version) ((version) >> 16)
#define sensor_major_version(version) (((version)&0xff00) >> 8)
#define sensor_minor_version(version) ((version)&0xff)

struct isp_sensor_mode {
	int32_t hdr; /* enum rts_isp_hdr_mode */
	struct {
		rts_isp_size_t snr;
		rts_isp_size_t isp;
	} size;
	float max_fps;
	float min_fps;

	uint32_t hts;
	uint32_t min_vts;
	uint32_t max_vts;
	uint32_t pclk;
	float exposure_step;
};

int isp_sensor_check(uint32_t isp_id);
/* Return 1 once if the probe left the sensor streaming (early_start). */
int isp_sensor_take_early_started(uint32_t isp_id);
int isp_sensor_get_ops(uint32_t isp_id,
					   const struct rts_isp_sensor_ops **sensor_ops);

#endif /* _ISP_SENSOR_H_INC_ */
