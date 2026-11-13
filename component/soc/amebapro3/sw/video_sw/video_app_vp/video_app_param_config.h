#ifndef __VIDEO_APP_PARAM_CONFIG_H__
#define __VIDEO_APP_PARAM_CONFIG_H__

#include "video_common_type.h"
#include "module_encoder.h"
#include "module_isp.h"

/********************************************************************************/
/*  Video Params Parsing Func                                                   */
/********************************************************************************/

/**
 * @brief  parse user app video config to mmf module config
 * @param  v_ch_en: isp/enc channel enable.
 * @param  user_cmd: user video command line.
 * @param  isp_param: isp mmf module config.
 * @param  enc_param: encoder mmf module config.
 */
int video_app_params_parsing(isp_enc_ch_en_t *v_ch_en, char *user_cmd, isp_params_t *isp_param, enc_parameter_t *enc_param);

/**
 * @brief  fill encoder memory-pool sizes for all VIDEO_ENC_VALID_CH entries.
 * @param  enc_params: encoder mmf module config array (VIDEO_ENC_VALID_CH entries).
 */
void video_enc_buf_pool_setup(enc_parameter_t *enc_params);

#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
#include "video_fcs_params.h"
/**
 * @brief  fill isp/enc params directly from PSRAM FCS struct, bypassing string parsing.
 * @param  p:          pointer to validated fcs_persistent_params_t in PSRAM.
 * @param  v_ch_en:    isp/enc channel enable output.
 * @param  isp_param:  isp mmf module config output (VIDEO_ISP_VALID_CH entries).
 * @param  enc_param:  encoder mmf module config output (VIDEO_ENC_VALID_CH entries).
 * @return 0 on success, -1 if the resulting params fail the conflict check.
 */
int video_app_fcs_params_load(const fcs_persistent_params_t *p,
							  isp_enc_ch_en_t *v_ch_en,
							  isp_params_t *isp_param,
							  enc_parameter_t *enc_param);
#endif


#endif /* __VIDEO_APP_PARAM_CONFIG_H__ */
