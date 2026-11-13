#ifndef _RTS_ISP_H_INC_
#define _RTS_ISP_H_INC_

#include <stdint.h>
#include <rtsavisp.h>
#include <rts_isp_ctrl.h>
#include <rts_isp_sensor.h>
#include <rts_isp_ae.h>
#include <rts_isp_awb.h>
#include <rts_isp_af.h>

#ifdef __cplusplus
extern "C" {
#endif

struct rts_isp_mask_entry {
	const int32_t type; /* enum rts_isp_geom_type */

	int enable;
	union {
		struct {
			const uint32_t max_cols;
			const uint32_t max_rows;
			rts_isp_grid_t grid;
			void *bitmap;
		};
		rts_isp_prect_t rect;
	};
};

struct rts_isp_mask {
	uint32_t color; /* rgb888 */
	const int number;
	struct rts_isp_mask_entry *entries;
};

int rts_isp_query_mask(uint32_t isp_id, struct rts_isp_mask **mask);
int rts_isp_get_mask(uint32_t isp_id, struct rts_isp_mask *mask);
int rts_isp_set_mask(uint32_t isp_id, struct rts_isp_mask *mask);
void rts_isp_release_mask(uint32_t isp_id, struct rts_isp_mask *mask);

enum rts_isp_ae_mode {
	RTS_ISP_AE_MANUAL_MODE,
	RTS_ISP_AE_AUTO_MODE,
};

struct rts_isp_ae_auto_setting {
	const uint32_t window_num;
	uint8_t win_weights[256];
	int target_delta;
	uint16_t gain_max;
	float min_fps;
	float exposure_max;
};

struct rts_isp_ae_manual_setting {
	uint32_t exposure_time;
	uint16_t total_gain;
	struct {
		uint16_t analog_gain;
		uint16_t digital_gain;
		uint16_t isp_gain;
	} gain;
};

struct rts_isp_ae_setting {
	int32_t mode; /* enum rts_isp_ae_mode */
	struct rts_isp_ae_auto_setting _auto;
	struct rts_isp_ae_manual_setting _manual;
};

int rts_isp_query_ae_setting(uint32_t isp_id,
							 struct rts_isp_ae_setting **ae_setting);
int rts_isp_get_ae_setting(uint32_t isp_id,
						   struct rts_isp_ae_setting *ae_setting);
int rts_isp_set_ae_setting(uint32_t isp_id,
						   struct rts_isp_ae_setting *ae_setting);
void rts_isp_release_ae_setting(uint32_t isp_id,
								struct rts_isp_ae_setting *ae_setting);

int rts_isp_get_ae_statis_info(uint32_t isp_id,
							   struct rts_isp_ae_stat_info *info);
int rts_isp_query_ae_statis(uint32_t isp_id,
							const struct rts_isp_ae_statis **ae_statis);
int rts_isp_get_ae_statis(uint32_t isp_id,
						  const struct rts_isp_ae_statis *ae_statis);
int rts_isp_put_ae_statis(uint32_t isp_id,
						  const struct rts_isp_ae_statis *ae_statis)
__attribute__((deprecated));
void rts_isp_release_ae_statis(uint32_t isp_id,
							   const struct rts_isp_ae_statis *ae_statis);

enum rts_isp_awb_mode {
	RTS_ISP_AWB_TEMPERATURE_MODE,
	RTS_ISP_AWB_AUTO_MODE,
	RTS_ISP_AWB_COMPONENT_MODE,
};

struct rts_isp_awb_setting {
	int32_t mode; /* enum rts_isp_awb_mode */

	struct {
		uint32_t temperature;
	} _temperature;

	struct {
		uint16_t r_gain_adj;
		uint16_t b_gain_adj;
	} _auto;

	struct {
		uint16_t r_gain;
		uint16_t g_gain;
		uint16_t b_gain;
	} _component;
};

int rts_isp_query_awb_setting(uint32_t isp_id,
							  struct rts_isp_awb_setting **awb_setting);
int rts_isp_get_awb_setting(uint32_t isp_id,
							struct rts_isp_awb_setting *awb_setting);
int rts_isp_set_awb_setting(uint32_t isp_id,
							struct rts_isp_awb_setting *awb_setting);
void rts_isp_release_awb_setting(uint32_t isp_id,
								 struct rts_isp_awb_setting *awb_setting);

int rts_isp_get_awb_statis_info(uint32_t isp_id,
								struct rts_isp_awb_stat_info *info);
int rts_isp_query_awb_statis(uint32_t isp_id,
							 const struct rts_isp_awb_statis **awb_statis);
int rts_isp_get_awb_statis(uint32_t isp_id,
						   const struct rts_isp_awb_statis *awb_statis);
int rts_isp_put_awb_statis(uint32_t isp_id,
						   const struct rts_isp_awb_statis *awb_statis)
__attribute__((deprecated));
void rts_isp_release_awb_statis(uint32_t isp_id,
								const struct rts_isp_awb_statis *awb_statis);

int rts_isp_get_af_statis_info(uint32_t isp_id,
							   struct rts_isp_af_stat_info *info);
int rts_isp_query_af_statis(uint32_t isp_id,
							const struct rts_isp_af_statis **af_statis);
int rts_isp_get_af_statis(uint32_t isp_id,
						  const struct rts_isp_af_statis *af_statis);
int rts_isp_put_af_statis(uint32_t isp_id,
						  const struct rts_isp_af_statis *af_statis)
__attribute__((deprecated));
void rts_isp_release_af_statis(uint32_t isp_id,
							   const struct rts_isp_af_statis *af_statis);

struct rts_isp_image_size {
	int32_t sensor_bit_depth; /* enum rts_isp_sensor_bit_depth */
	rts_isp_size_t sensor_size;
	rts_isp_size_t isp_middle_size;
	rts_isp_size_t isp_final_size;
};

int rts_isp_get_image_size(uint32_t isp_id, struct rts_isp_image_size *size);

struct rts_isp_bypass {
	union {
		uint32_t all;
		struct {
			uint32_t bypass_blc_0 : 1;
			uint32_t bypass_blc_1 : 1;
			uint32_t bypass_dpc_0 : 1;
			uint32_t bypass_dpc_1 : 1;
			uint32_t bypass_rnr_0 : 1;
			uint32_t bypass_rnr_1 : 1;
			uint32_t bypass_nlsc : 1;
			uint32_t bypass_mlsc : 1;
			uint32_t bypass_awb : 1;
			uint32_t bypass_ae_0 : 1;
			uint32_t bypass_ae_1 : 1;
			uint32_t bypass_ae : 1;
			uint32_t bypass_ccm : 1;
			uint32_t bypass_gamma : 1;
			uint32_t bypass_wdr : 1;
			uint32_t bypass_ygc : 1;
			uint32_t bypass_uvtune : 1;
			uint32_t bypass_dither : 1;
			uint32_t bypass_ldc : 1;
			uint32_t bypass_eeh : 1;
			uint32_t bypass_lce : 1;
			uint32_t bypass_spe : 1;
			uint32_t bypass_ygamma : 1;
			uint32_t bypass_hue : 1;
			uint32_t bypass_motion_info : 1;
			uint32_t bypass_gcac : 1;
		};
	};
};

int rts_isp_get_bypass(uint32_t isp_id, struct rts_isp_bypass *bypass);
int rts_isp_set_bypass(uint32_t isp_id, struct rts_isp_bypass *bypass);

enum rts_isp_blc_position {
	BLC_AFTER_RNR = 0,
	BLC_BEFORE_RNR = 1,
	BLC_AFTER_DECOMP = 2,
};

int rts_isp_get_blc_position(uint32_t isp_id);

const char *rts_isp_get_bound_sensor_name(uint32_t isp_id);

int rts_isp_set_time_tracker(uint32_t isp_id, int on);

int rts_isp_controller_i2c_read(uint32_t isp_id, uint8_t i2c_id, void *buf,
								int len);
int rts_isp_controller_i2c_write(uint32_t isp_id, uint8_t i2c_id, void *buf,
								 int len);
int rts_isp_controller_read_sensor_reg(uint32_t isp_id,
									   struct rts_isp_i2c_reg *reg);
int rts_isp_controller_write_sensor_reg(uint32_t isp_id,
										struct rts_isp_i2c_reg *reg);
int rts_isp_test_preview(uint32_t isp_id, int fps);

enum rts_isp_eeh_position {
	EEH_AFTER_LCE = 0,
	EEH_BEFORE_LCE = 1,
};

int rts_isp_get_eeh_position(uint32_t isp_id);

#ifdef __cplusplus
}
#endif

#endif /* _RTS_ISP_H_INC_ */
