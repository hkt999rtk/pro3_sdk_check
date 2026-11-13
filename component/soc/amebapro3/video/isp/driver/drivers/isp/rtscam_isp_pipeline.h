#ifndef _RTSCAM_ISP_PIPELINE_H_
#define _RTSCAM_ISP_PIPELINE_H_

#include <stdbool.h>
#include <rts_camera_isp.h>

struct rtscam_isp_pipeline;
enum rtscam_ispc_inner_req_id {
	RTSCAM_ISPC_PIPELINE_BASE = 0,
	RTSCAM_ISPC_PIPELINE_0 = RTSCAM_ISPC_PIPELINE_BASE,
	RTSCAM_ISPC_PIPELINE_NUM,
};

int rtscam_isp_probe(void);
int rtscam_isp_remove(void);
struct rtscam_isp_pipeline *rtscam_isp_get_pipeline(int id);
int rtscam_isp_get_frame_info(struct rtscam_isp_pipeline *pipeline,
							  struct rtscam_isp_frame_info *info);
int rtscam_isp_request(int id, int cmd, void *data, uint32_t size,
					   bool realtime);
int rtscam_isp_need_resume(uint32_t isp_id);

/* pipeline API for zoom layer */
int rtscam_isp_zoom_set_stream_on(struct rtscam_isp_pipeline *pipeline);
int rtscam_isp_zoom_set_stream_off(struct rtscam_isp_pipeline *pipeline);
int rtscam_isp_zoom_get_aov_mode(void);

/* pipeline API for soc layer */
int rtscam_isp_soc_set_fps(struct rtscam_isp_pipeline *pipeline,
						   struct v4l2_fract fps);
int rtscam_isp_soc_get_fps(struct rtscam_isp_pipeline *pipeline,
						   struct v4l2_fract *fps_min,
						   struct v4l2_fract *fps_max);
void rtscam_isp_resume(struct rtscam_isp_pipeline *pipeline);
void rtscam_isp_suspend(struct rtscam_isp_pipeline *pipeline);

#endif /* _RTSCAM_ISP_PIPELINE_H_ */
