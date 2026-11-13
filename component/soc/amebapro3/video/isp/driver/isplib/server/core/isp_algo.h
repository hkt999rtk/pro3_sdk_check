#ifndef _ISP_ALGO_H_INC_
#define _ISP_ALGO_H_INC_

#include <rtsavisp.h>
#include <rts_isp_awb.h>
#include <rts_isp_ae.h>
#include <rts_isp_af.h>
#include <rts_isp_other.h>

#define ISP_MAX_ALGO_NUM \
	(RTS_ISP_PLUG_ALGO_AE_ID_MAX + 1 - RTS_ISP_PLUG_ALGO_AE_ID_MIN)

#define algo_magic(verion) ((version) >> 16)
#define algo_major_version(verion) (((version)&0xff00) >> 8)
#define algo_minor_version(verion) (((version)&0xff))

int isp_algo_get_ae(uint32_t isp_id, const struct rts_isp_ae_algo **ae_algo);
int isp_algo_get_awb(uint32_t isp_id, const struct rts_isp_awb_algo **awb_algo);
int isp_algo_get_af(uint32_t isp_id, const struct rts_isp_af_algo **af_algo);
int isp_algo_get_flick(uint32_t isp_id,
					   const struct rts_isp_flick_algo **flick_algo);
int isp_algo_get_wdr(uint32_t isp_id, const struct rts_isp_wdr_algo **wdr_algo);

#endif /* _ISP_ALGO_H_INC_ */
