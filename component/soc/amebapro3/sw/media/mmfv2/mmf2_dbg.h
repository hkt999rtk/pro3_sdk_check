#ifndef __MMF_DBG_H_
#define __MMF_DBG_H_

#include <stdint.h>
#include "rom/log.h"

/* define debug group */
#define	_MMF_DBG_RTSP_              0x00000001
#define	_MMF_DBG_RTP_               0x00000002
#define	_MMF_DBG_ISP_               0x00000004
#define	_MMF_DBG_ENCODER_           0x00000008
#define	_MMF_DBG_VIDEO_             0x00000010
#define	_MMF_DBG_AUDIO_             0x00000020
#define	_MMF_DBG_NN_                0x00000040
#define	_MMF_DBG_MODULE_            0x00001000
#define	_MMF_DBG_LINKER_            0x00002000


extern uint32_t ConfigDebugMmfErr;
extern uint32_t ConfigDebugMmfWarn;
extern uint32_t ConfigDebugMmfInfo;
extern uint32_t ConfigDebugMmfDebug;


/* debug log ON/OFF */
#define DBG_MMF_ERR_MSG_ON(x)       (ConfigDebugMmfErr |= (x))
#define DBG_MMF_WARN_MSG_ON(x)      (ConfigDebugMmfWarn |= (x))
#define DBG_MMF_INFO_MSG_ON(x)      (ConfigDebugMmfInfo |= (x))
#define DBG_MMF_DEBUG_MSG_ON(x)     (ConfigDebugMmfDebug |= (x))

#define DBG_MMF_ERR_MSG_OFF(x)      (ConfigDebugMmfErr &= ~(x))
#define DBG_MMF_WARN_MSG_OFF(x)     (ConfigDebugMmfWarn &= ~(x))
#define DBG_MMF_INFO_MSG_OFF(x)     (ConfigDebugMmfInfo &= ~(x))
#define DBG_MMF_DEBUG_MSG_OFF(x)    (ConfigDebugMmfDebug &= ~(x))


/* log prefix */
#define RTSP_ERR_PREFIX         "[RTSP Err]"
#define RTSP_WARN_PREFIX        "[RTSP Wrn]"
#define RTSP_INFO_PREFIX        "[RTSP Inf]"
#define RTSP_DEBUG_PREFIX       "[RTSP Dbg]"

#define RTP_ERR_PREFIX          "[RTP Err]"
#define RTP_WARN_PREFIX         "[RTP Wrn]"
#define RTP_INFO_PREFIX         "[RTP Inf]"
#define RTP_DEBUG_PREFIX        "[RTP Dbg]"

#define ISP_ERR_PREFIX          "[ISP Err]"
#define ISP_WARN_PREFIX         "[ISP Wrn]"
#define ISP_INFO_PREFIX         "[ISP Inf]"
#define ISP_DEBUG_PREFIX        "[ISP Dbg]"

#define ENCODER_ERR_PREFIX      "[Enc Err]"
#define ENCODER_WARN_PREFIX     "[Enc Wrn]"
#define ENCODER_INFO_PREFIX     "[Enc Inf]"
#define ENCODER_DEBUG_PREFIX    "[Enc Dbg]"

#define VIDEO_ERR_PREFIX        "[VID Err]"
#define VIDEO_WARN_PREFIX       "[VID Wrn]"
#define VIDEO_INFO_PREFIX       "[VID Inf]"
#define VIDEO_DEBUG_PREFIX      "[VID Dbg]"

#define AUDIO_ERR_PREFIX        "[AUD Err]"
#define AUDIO_WARN_PREFIX       "[AUD Wrn]"
#define AUDIO_INFO_PREFIX       "[AUD Inf]"
#define AUDIO_DEBUG_PREFIX      "[AUD Dbg]"

#define NN_ERR_PREFIX           "[NN Err]"
#define NN_WARN_PREFIX          "[NN Wrn]"
#define NN_INFO_PREFIX          "[NN Inf]"
#define NN_DEBUG_PREFIX         "[NN Dbg]"

#define MODULE_ERR_PREFIX	    "[MODULE Err]"
#define MODULE_WARN_PREFIX	    "[MODULE Wrn]"
#define MODULE_INFO_PREFIX	    "[MODULE Inf]"
#define MODULE_DEBUG_PREFIX     "[MODULE Dbg]"

#define LINKER_ERR_PREFIX       "[LINKER Err]"
#define LINKER_WARN_PREFIX      "[LINKER Wrn]"
#define LINKER_INFO_PREFIX	    "[LINKER Inf]"
#define LINKER_DEBUG_PREFIX     "[LINKER Dbg]"


/* printf macro */
#define MMF_PRINTK(...)         RTK_LOGA("MMF", ##__VA_ARGS__)


/* debug ERROR */
#define RTSP_DBG_ERROR(...)													\
			do {															\
				if (ConfigDebugMmfErr & _MMF_DBG_RTSP_)						\
					MMF_PRINTK("\r\n" RTSP_ERR_PREFIX __VA_ARGS__);			\
			}while(0)
#define RTP_DBG_ERROR(...)													\
			do {															\
				if (ConfigDebugMmfErr & _MMF_DBG_RTP_)						\
					MMF_PRINTK("\r\n" RTP_ERR_PREFIX __VA_ARGS__);			\
			}while(0)
#define ISP_DBG_ERROR(...)													\
			do {															\
				if (ConfigDebugMmfErr & _MMF_DBG_ISP_)						\
					MMF_PRINTK("\r\n" ISP_ERR_PREFIX __VA_ARGS__);			\
			}while(0)
#define ENCODER_DBG_ERROR(...)												\
			do {															\
				if (ConfigDebugMmfErr & _MMF_DBG_ENCODER_)					\
					MMF_PRINTK("\r\n" ENCODER_ERR_PREFIX __VA_ARGS__);		\
			}while(0)

#define VIDEO_DBG_ERROR(...)												\
			do {															\
				if (ConfigDebugMmfErr & _MMF_DBG_VIDEO_)					\
					MMF_PRINTK("\r\n" VIDEO_ERR_PREFIX __VA_ARGS__);		\
			}while(0)
#define AUDIO_DBG_ERROR(...)												\
			do {															\
				if (ConfigDebugMmfErr & _MMF_DBG_AUDIO_)					\
					MMF_PRINTK("\r\n" AUDIO_ERR_PREFIX __VA_ARGS__);		\
			}while(0)
#define NN_DBG_ERROR(...)												    \
			do {															\
				if (ConfigDebugMmfErr & _MMF_DBG_NN_)					    \
					MMF_PRINTK("\r\n" NN_ERR_PREFIX __VA_ARGS__);		    \
			}while(0)
#define MODULE_DBG_ERROR(...)												\
			do {															\
				if (ConfigDebugMmfErr & _MMF_DBG_MODULE_)					\
					MMF_PRINTK("\r\n" MODULE_ERR_PREFIX __VA_ARGS__);		\
			}while(0)
#define LINKER_DBG_ERROR(...)												\
			do {															\
				if (ConfigDebugMmfErr & _MMF_DBG_LINKER_)					\
					MMF_PRINTK("\r\n" LINKER_ERR_PREFIX __VA_ARGS__);		\
			}while(0)


/* debug WARNING */
#define RTSP_DBG_WARNING(...)												\
			do {															\
				if (ConfigDebugMmfWarn & _MMF_DBG_RTSP_)				    \
					MMF_PRINTK("\r\n" RTSP_WARN_PREFIX __VA_ARGS__);		\
			}while(0)
#define RTP_DBG_WARNING(...)												\
			do {															\
				if (ConfigDebugMmfWarn & _MMF_DBG_RTP_)						\
					MMF_PRINTK("\r\n" RTP_WARN_PREFIX __VA_ARGS__);			\
			}while(0)
#define ISP_DBG_WARNING(...)												\
			do {															\
				if (ConfigDebugMmfWarn & _MMF_DBG_ISP_)						\
					MMF_PRINTK("\r\n" ISP_WARN_PREFIX __VA_ARGS__);			\
			}while(0)
#define ENCODER_DBG_WARNING(...)											\
			do {															\
				if (ConfigDebugMmfWarn & _MMF_DBG_ENCODER_)					\
					MMF_PRINTK("\r\n" ENCODER_WARN_PREFIX __VA_ARGS__);		\
			}while(0)
#define VIDEO_DBG_WARNING(...)											    \
			do {															\
				if (ConfigDebugMmfWarn & _MMF_DBG_VIDEO_)					\
					MMF_PRINTK("\r\n" VIDEO_WARN_PREFIX __VA_ARGS__);		\
			}while(0)
#define AUDIO_DBG_WARNING(...)											    \
			do {															\
				if (ConfigDebugMmfWarn & _MMF_DBG_AUDIO_)					\
					MMF_PRINTK("\r\n" AUDIO_WARN_PREFIX __VA_ARGS__);		\
			}while(0)
#define NN_DBG_WARNING(...)											        \
			do {															\
				if (ConfigDebugMmfWarn & _MMF_DBG_NN_)					    \
					MMF_PRINTK("\r\n" NN_WARN_PREFIX __VA_ARGS__);		    \
			}while(0)
#define MODULE_DBG_WARNING(...)												\
			do {															\
				if (ConfigDebugMmfWarn & _MMF_DBG_MODULE_)					\
					MMF_PRINTK("\r\n" MODULE_WARN_PREFIX __VA_ARGS__);		\
			}while(0)
#define LINKER_DBG_WARNING(...)												\
			do {															\
				if (ConfigDebugMmfWarn & _MMF_DBG_LINKER_)					\
					MMF_PRINTK("\r\n" LINKER_WARN_PREFIX __VA_ARGS__);		\
			}while(0)


/* debug INFO */
#define RTSP_DBG_INFO(...)													\
			do {															\
				if (ConfigDebugMmfInfo & _MMF_DBG_RTSP_)				    \
					MMF_PRINTK("\r\n" RTSP_INFO_PREFIX __VA_ARGS__);		\
			}while(0)
#define RTP_DBG_INFO(...)													\
			do {															\
				if (ConfigDebugMmfInfo & _MMF_DBG_RTP_)						\
					MMF_PRINTK("\r\n" RTP_INFO_PREFIX __VA_ARGS__);			\
			}while(0)
#define ISP_DBG_INFO(...)													\
			do {															\
				if (ConfigDebugMmfInfo & _MMF_DBG_ISP_)						\
					MMF_PRINTK("\r\n" ISP_INFO_PREFIX __VA_ARGS__);			\
			}while(0)
#define ENCODER_DBG_INFO(...)												\
			do {															\
				if (ConfigDebugMmfInfo & _MMF_DBG_ENCODER_)					\
					MMF_PRINTK("\r\n" ENCODER_INFO_PREFIX __VA_ARGS__);		\
			}while(0)
#define VIDEO_DBG_INFO(...)											        \
			do {															\
				if (ConfigDebugMmfInfo & _MMF_DBG_VIDEO_)					\
					MMF_PRINTK("\r\n" VIDEO_INFO_PREFIX __VA_ARGS__);		\
			}while(0)
#define AUDIO_DBG_INFO(...)											        \
			do {															\
				if (ConfigDebugMmfInfo & _MMF_DBG_AUDIO_)					\
					MMF_PRINTK("\r\n" AUDIO_INFO_PREFIX __VA_ARGS__);		\
			}while(0)
#define NN_DBG_INFO(...)											        \
			do {															\
				if (ConfigDebugMmfInfo & _MMF_DBG_NN_)					    \
					MMF_PRINTK("\r\n" NN_INFO_PREFIX __VA_ARGS__);		    \
			}while(0)
#define MODULE_DBG_INFO(...)												\
			do {															\
				if (ConfigDebugMmfInfo & _MMF_DBG_MODULE_)					\
					MMF_PRINTK("\r\n" MODULE_INFO_PREFIX __VA_ARGS__);		\
			}while(0)
#define LINKER_DBG_INFO(...)												\
			do {															\
				if (ConfigDebugMmfInfo & _MMF_DBG_LINKER_)					\
					MMF_PRINTK("\r\n" LINKER_INFO_PREFIX __VA_ARGS__);		\
			}while(0)


/* debug DEBUG */
#define RTSP_DBG_DEBUG(...)													\
			do {															\
				if (ConfigDebugMmfDebug & _MMF_DBG_RTSP_)				    \
					MMF_PRINTK("\r\n" RTSP_DEBUG_PREFIX __VA_ARGS__);		\
			}while(0)
#define RTP_DBG_DEBUG(...)													\
			do {															\
				if (ConfigDebugMmfDebug & _MMF_DBG_RTP_)				    \
					MMF_PRINTK("\r\n" RTP_DEBUG_PREFIX __VA_ARGS__);	    \
			}while(0)
#define ISP_DBG_DEBUG(...)													\
			do {															\
				if (ConfigDebugMmfDebug & _MMF_DBG_ISP_)				    \
					MMF_PRINTK("\r\n" ISP_DEBUG_PREFIX __VA_ARGS__);	    \
			}while(0)
#define ENCODER_DBG_DEBUG(...)												\
			do {															\
				if (ConfigDebugMmfDebug & _MMF_DBG_ENCODER_)			    \
					MMF_PRINTK("\r\n" ENCODER_DEBUG_PREFIX __VA_ARGS__);    \
			}while(0)
#define VIDEO_DBG_DEBUG(...)											    \
			do {															\
				if (ConfigDebugMmfDebug & _MMF_DBG_VIDEO_)					\
					MMF_PRINTK("\r\n" VIDEO_DEBUG_PREFIX __VA_ARGS__);		\
			}while(0)
#define AUDIO_DBG_DEBUG(...)											    \
			do {															\
				if (ConfigDebugMmfDebug & _MMF_DBG_AUDIO_)					\
					MMF_PRINTK("\r\n" AUDIO_DEBUG_PREFIX __VA_ARGS__);		\
			}while(0)
#define NN_DBG_DEBUG(...)											        \
			do {															\
				if (ConfigDebugMmfDebug & _MMF_DBG_NN_)					    \
					MMF_PRINTK("\r\n" NN_DEBUG_PREFIX __VA_ARGS__);		    \
			}while(0)
#define MODULE_DBG_DEBUG(...)												\
			do {															\
				if (ConfigDebugMmfDebug & _MMF_DBG_MODULE_)					\
					MMF_PRINTK("\r\n" MODULE_DEBUG_PREFIX __VA_ARGS__);		\
			}while(0)
#define LINKER_DBG_DEBUG(...)												\
			do {															\
				if (ConfigDebugMmfDebug & _MMF_DBG_LINKER_)					\
					MMF_PRINTK("\r\n" LINKER_DEBUG_PREFIX __VA_ARGS__);		\
			}while(0)


void mm_set_abort_func(void (*func)(void));
void mmf2_abort(void);

#endif
