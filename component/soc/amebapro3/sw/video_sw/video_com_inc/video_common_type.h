
#ifndef __VIDEO_COMMON_TYPE_H__
#define __VIDEO_COMMON_TYPE_H__

/********************************************************************************/
/*  Common Definition                                                           */
/********************************************************************************/
/* resolution */
#define VIDEO_QCIF              0
#define VIDEO_CIF               1
#define VIDEO_WVGA              2
#define VIDEO_VGA               3
#define VIDEO_D1                4
#define VIDEO_HD                5
#define VIDEO_FHD               6
#define VIDEO_3M                7
#define VIDEO_5M                8
#define VIDEO_2K                9
#define VIDEO_NON_STANDARD      10
/* type */
#define VIDEO_HEVC              (1 << 0)                    // 1
#define VIDEO_H264              (1 << 1)                    // 2
#define VIDEO_JPEG              (1 << 2)                    // 4
#define VIDEO_NV12              (1 << 3)                    // 8
#define VIDEO_RGB               (1 << 4)                    // 16
#define VIDEO_NV16              (1 << 5)                    // 32
#define VIDEO_YUY2              (1 << 6)                    // 64
#define VIDEO_BAYER_RAW         (1 << 7)                    // 128
#define VIDEO_HEVC_JPEG         (VIDEO_HEVC | VIDEO_JPEG)   // 5
#define VIDEO_H264_JPEG         (VIDEO_H264 | VIDEO_JPEG)   // 6

typedef enum {
	VIDEO_OSD_FORMAT_ARGB8888 = 0,
	VIDEO_OSD_FORMAT_NV12 = 1,
	VIDEO_OSD_FORMAT_BITMAP = 2,
} video_osd_format_t;

/* Task-context snapshot request. On NOSPACE jpeg_len is the required size
 * for this frame. All other failures leave jpeg_len zero and dst untouched. */
typedef struct {
	uint8_t *dst;
	uint32_t capacity;
	uint32_t jpeg_len;
} video_snapshot_t;

/* Kept clear of AP2VP_SendCmd() IPC failures (-1/-2) so VP can return them directly. */
enum {
	VIDEO_SNAPSHOT_ERROR = -101,
	VIDEO_SNAPSHOT_BUSY = -102,
	VIDEO_SNAPSHOT_NOSPACE = -103,
	VIDEO_SNAPSHOT_TIMEOUT = -104,
	VIDEO_SNAPSHOT_INACTIVE = -105,
};

/* rc mode */
#define ENC_CVBR                0   /* Constrained VBR: VBR with a peak-bitrate cap */
#define ENC_CBR                 1   /* Constant Bitrate: fixed bps, strict HRD compliance */
#define ENC_VBR                 2   /* Variable Bitrate: quality-driven, bps may vary freely */
#define ENC_ABR                 3   /* Average Bitrate: targets average bps over a window */
#define ENC_CRF                 4   /* Constant Rate Factor: fixed perceptual quality */
#define ENC_CQP                 5   /* Constant QP: fixed quantisation parameter per frame */
/* status */
#define VIDEO_STOP              0
#define VIDEO_PAUSE             1
#define VIDEO_START             2
/* channel id */
#define V1_STREAM_ID            0
#define V2_STREAM_ID            1
#define V3_STREAM_ID            2
#define V4_STREAM_ID            3
#define V5_STREAM_ID            4
#define VIDEO_ISP_VALID_CH      5   // isp support 5 channels
#define VIDEO_ENC_VALID_CH      3   // encoder support in first 3 channels, but each channel can support multiple encoder instances

typedef struct isp_enc_ch_en_s {
	uint8_t isp[VIDEO_ISP_VALID_CH];
	uint8_t enc[VIDEO_ENC_VALID_CH];
} isp_enc_ch_en_t;


/********************************************************************************/
/*  AP/VP Shared Structure                                                     **/
/********************************************************************************/
/* Shared JPEG buffers and the two writers of the result need isolated cache lines. */
#define VIDEO_SNAPSHOT_BUFFER_ALIGNMENT 64U
#if defined(CACHE_LINE_SIZE) && (VIDEO_SNAPSHOT_BUFFER_ALIGNMENT % CACHE_LINE_SIZE != 0)
#error "Snapshot buffer alignment must cover whole cache lines on both cores"
#endif

/* VP publishes zero while pending, a JPEG length on completion, or an error.
 * AP sets active_id before START and clears it after its final buffer access. */
typedef struct vp_snapshot_result {
	uint32_t id;
	void *jpeg;
	int32_t size;
	uint32_t active_id __attribute__((aligned(VIDEO_SNAPSHOT_BUFFER_ALIGNMENT)));
} vp_snapshot_result_t;

typedef struct vpap_shared_rc_s {
	uint32_t rcMode;
	uint32_t bps;
	uint32_t minQp;
	uint32_t maxQp;
	uint32_t intraQpDelta;
} vpap_shared_rc_t;

#define VPAP_ENC_RC_EXT_RCMODE             (1U << 0)
#define VPAP_ENC_RC_EXT_BPS                (1U << 1)
#define VPAP_ENC_RC_EXT_QP_HDR             (1U << 2)
#define VPAP_ENC_RC_EXT_QP_RANGE           (1U << 3)
#define VPAP_ENC_RC_EXT_I_QP_RANGE         (1U << 4)
#define VPAP_ENC_RC_EXT_INTRA_QP_DELTA     (1U << 5)
#define VPAP_ENC_RC_EXT_FIXED_INTRA_QP     (1U << 6)
#define VPAP_ENC_RC_EXT_PIC_RC             (1U << 7)
#define VPAP_ENC_RC_EXT_CTB_RC             (1U << 8)
#define VPAP_ENC_RC_EXT_BLOCK_RC_SIZE      (1U << 9)
#define VPAP_ENC_RC_EXT_PIC_SKIP           (1U << 10)
#define VPAP_ENC_RC_EXT_HRD                (1U << 11)
#define VPAP_ENC_RC_EXT_CPB                (1U << 12)
#define VPAP_ENC_RC_EXT_BITRATE_WINDOW     (1U << 13)
#define VPAP_ENC_RC_EXT_MOVING_BITRATE     (1U << 14)
#define VPAP_ENC_RC_EXT_FILLER_DATA        (1U << 15)
#define VPAP_ENC_RC_EXT_CRF                (1U << 16)
#define VPAP_ENC_RC_EXT_VBR                (1U << 17)
#define VPAP_ENC_RC_EXT_FRAME_RATE         (1U << 18)
#define VPAP_ENC_RC_EXT_ALL                ((1U << 19) - 1U)

typedef struct vpap_shared_enc_rc_ext_s {
	uint32_t flags;
	uint32_t rcMode;
	uint32_t bps;
	int32_t qpHdr;
	uint32_t minQp;
	uint32_t maxQp;
	uint32_t minIQp;
	uint32_t maxIQp;
	int32_t intraQpDelta;
	uint32_t fixedIntraQp;
	uint32_t pictureRc;
	uint32_t ctbRc;
	uint32_t blockRCSize;
	uint32_t pictureSkip;
	uint32_t hrd;
	uint32_t hrdCpbSize;
	uint32_t cpbMaxRate;
	uint32_t bitrateWindow;
	int32_t tolMovingBitRate;
	int32_t monitorFrames;
	uint32_t fillerData;
	int32_t crf;
	uint32_t vbr;
	uint32_t frameRateNum;
	uint32_t frameRateDenom;
} vpap_shared_enc_rc_ext_t;

#define VPAP_ENC_CODING_SLICE_SIZE         (1U << 0)
#define VPAP_ENC_CODING_SEI                (1U << 1)
#define VPAP_ENC_CODING_FULL_RANGE         (1U << 2)
#define VPAP_ENC_CODING_DEBLOCK            (1U << 3)
#define VPAP_ENC_CODING_SAO                (1U << 4)
#define VPAP_ENC_CODING_CABAC              (1U << 5)
#define VPAP_ENC_CODING_SAR                (1U << 6)
#define VPAP_ENC_CODING_CIR                (1U << 7)
#define VPAP_ENC_CODING_ROI                (1U << 8)
#define VPAP_ENC_CODING_SEI_USER_DATA      (1U << 9)
#define VPAP_ENC_CODING_ALL                ((1U << 10) - 1U)

#define VPAP_ENC_MAX_ROI                   8

typedef struct vpap_shared_enc_roi_s {
	uint32_t enable;
	uint32_t top;
	uint32_t left;
	uint32_t bottom;
	uint32_t right;
	int32_t deltaQp;
	int32_t qp;
} vpap_shared_enc_roi_t;

typedef struct vpap_shared_enc_coding_s {
	uint32_t flags;
	uint32_t sliceSize;
	uint32_t seiMessages;
	uint32_t vuiVideoFullRange;
	uint32_t disableDeblockingFilter;
	int32_t tcOffset;
	int32_t betaOffset;
	uint32_t enableSao;
	uint32_t enableCabac;
	uint32_t sampleAspectRatioWidth;
	uint32_t sampleAspectRatioHeight;
	uint32_t cirStart;
	uint32_t cirInterval;
	uint32_t seiUserData;
	vpap_shared_enc_roi_t roi[VPAP_ENC_MAX_ROI];
} vpap_shared_enc_coding_t;

typedef struct vpap_shared_osd_s {
	int osdOverlayIdx;
	int osdOverlayEnable;
	uint32_t osdOverlayFormat;   /* video_osd_format_t */
	uint32_t osdOverlayXoffset;
	uint32_t osdOverlayYoffset;
	uint32_t osdOverlayWidth;
	uint32_t osdOverlayHeight;
	uint32_t osdOverlayAlpha;
	uint32_t osdOverlayBitmapY;
	uint32_t osdOverlayBitmapU;
	uint32_t osdOverlayBitmapV;
	uint8_t *osdOverlayPicBuf;
} vpap_shared_osd_t;

typedef struct vpap_shared_mosaic_s {
	int mosOverlayIdx;           /* mosaic region index (0~11) */
	int mosOverlayEnable;
	uint32_t mosOverlayXoffset;
	uint32_t mosOverlayYoffset;
	uint32_t mosOverlayWidth;
	uint32_t mosOverlayHeight;
	uint32_t mosOverlaySizeIdx;  /* block size: 0:8x8 1:16x16 2:32x32 3:64x64 4:128x128; hardware uses one size per channel — all enabled regions must share the same value */
} vpap_shared_mosaic_t;

typedef vpap_shared_osd_t osd_overlay_t;
typedef vpap_shared_mosaic_t mosaic_overlay_t;
typedef vpap_shared_rc_t rc_param_t;
typedef vpap_shared_enc_rc_ext_t enc_rc_ext_t;
typedef vpap_shared_enc_coding_t enc_coding_t;

/* The following types were present in earlier versions of this header and have
 * been intentionally removed as dead code.  A SDK-wide grep confirmed zero
 * references before removal:
 *   video_app_params_t, video_crop_params_t, video_osd_params_t,
 *   video_mosaic_params_t, video_isp_params_t, video_isp_init_t,
 *   video_enc_params_t
 * Do not re-add them without a corresponding user in the codebase.
 */


/********************************************************************************/
/*  Address Remap Param                                                         */
/********************************************************************************/
#define ISP_REMAP_ENC_H264      0
#define ISP_REMAP_ENC_HEVC      1
#define REMAP_HDR               BIT(0)
#define REMAP_SLICE             BIT(1)
#define REMAP_PARSLICE          BIT(2)
#define REMAP_RTT               BIT(3)
#define REMAP_PARSRAM           BIT(4)
#define REMAP_COMPRESS          BIT(5)

typedef struct isp_remap_param_s {
	uint32_t feature_bit;
	uint32_t enc_type;
	uint32_t wrap_num;
	uint32_t burst_id;
	uint32_t fixp_hts_ratio_2;
	uint32_t encDec400LumTableBase[2];
	uint32_t encDec400ChrTableBase[2];
} isp_remap_param_t;

typedef struct enc_remap_param_s {
	uint32_t slice_wrap_num;
} enc_remap_param_t;

#endif /* __VIDEO_COMMON_TYPE_H__ */
