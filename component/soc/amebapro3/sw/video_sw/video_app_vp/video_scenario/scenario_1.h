#ifndef __VIDEO_SCENARIO_1_H__
#define __VIDEO_SCENARIO_1_H__

/********************************************************************************/
/*  Video User Configuration                                                    */
/********************************************************************************/

/*** V1 ***/
#define V1_RESOLUTION				VIDEO_2K
#define V1_FPS						30
#define V1_TYPE                     VIDEO_HEVC
#define V1_BITRATE					2*1024*1024
#define V1_CROP_SCALE_EN            0
#define V1_OSD_EN                   1
#define V1_MOSAIC_EN                0
#define V1_MOSAIC_SIZE              1   // 0:8x8, 1:16x16, 2:32x32, 3:64x64, 4:128x128

/*** V2 ***/
#define V2_RESOLUTION				VIDEO_2K
#define V2_FPS						30
#define V2_TYPE                     VIDEO_H264
#define V2_BITRATE					1*1024*1024
#define V2_CROP_SCALE_EN            0
#define V2_OSD_EN                   1
#define V2_MOSAIC_EN                0
#define V2_MOSAIC_SIZE              1

/*** V3 ***/
#define V3_RESOLUTION				VIDEO_2K
#define V3_FPS						1
#define V3_TYPE                     VIDEO_JPEG
#define V3_BITRATE					1*1024*1024
#define V3_CROP_SCALE_EN            0
#define V3_OSD_EN                   1

/*** V4 ***/
#define V4_RESOLUTION				VIDEO_WVGA
#define V4_FPS						15
#define V4_TYPE                     VIDEO_NV12
#define V4_CROP_SCALE_EN            0
#define V4_OSD_EN                   0

/*** V5 ***/
#define V5_WIDTH	                576
#define V5_HEIGHT                   320
#define V5_FPS						10
#define V5_TYPE                     VIDEO_RGB   //only RGB
#define V5_CROP_SCALE_EN            0
#define V5_OSD_EN                   0


/********************************************************************************/
/*  ISP Global User Setting                                                     */
/********************************************************************************/
#define ISP_AI_DENOISE_EN           0


/********************************************************************************/
/*  Encoder Rate Control User Setting                                           */
/********************************************************************************/
#define ENC_MIN_QP                  25
#define ENC_MAX_QP                  48
#define ENC_RC_MODE                 ENC_ABR
#define ENC_ROTATION                0
#define ENC_META_EN                 0
#define ENC_JPEG_QLEVEL             1

#endif /* __VIDEO_SCENARIO_1_H__ */
