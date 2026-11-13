/*
 * Hardware address configuration for RTS3921 video-in driver.
 *
 * TODO: verify all addresses against RTS3921 hardware specification.
 * Fill in CONFIG_RTS_SOC_IRQ and CONFIG_RTS_MD_IRQ before enabling videoin.
 */

#ifndef _RTS_CAMERA_HW_CONFIG_H
#define _RTS_CAMERA_HW_CONFIG_H

/* Video-in SOC registers (ISP capture engine) */
#define CONFIG_RTS_SOC_CAM_BASE   0x600f0000
#define CONFIG_RTS_SOC_CAM_SIZE   0x400

/* IRQ numbers — per ameba_irqn.h NP-core enum */
/* ISP0_IRQ: frame-done interrupt */
#define CONFIG_RTS_SOC_IRQ ISP0_IRQ
#define CONFIG_RTS_SOC_IRQ_PRIO 1

/* ISP1_IRQ: ISP pipeline event interrupt */
#define CONFIG_RTS_ISP_EVENT_IRQ ISP1_IRQ

/* ISP2_IRQ: ISP MD interrupt */
#define CONFIG_RTS_ISP_MD_IRQ ISP2_IRQ


/* Zoom / scaler registers */
#define CONFIG_RTS_ZOOM_SYS_BASE 0x600d8000 /* ZOOM_SYS_REG_BASE (absolute) */
#define CONFIG_RTS_ZOOM_CH_BASE \
	0x600d8c00 /* Channel register base (absolute) */

/* Zoom stream configuration (replaces DT stream nodes).
 * FMT values are RTSCAM_FORMAT_TYPE_* bitmasks from rts_camera_config.h:
 *   0x01 = YUV420 semiplanar (NV12)
 *   0x02 = YUV422 semiplanar (NV16)
 *   0x04 = RGB
 *   0x08 = Y-only
 *   0x10 = YUYV interleaved  (NOT NV12!)
 * Combine with | for streams that support multiple formats.
 */
#define CONFIG_RTS_ZOOM_S0_FMT \
	0x53 /* RTSCAM_FORMAT_TYPE_YUV420_SEMIPLANAR (NV12) | RTSCAM_FORMAT_TYPE_128BIT_3PIXEL (raw dump, see dbg_raw) */
/* S0 zoom-in uses these maxima directly, without clamping to the pipeline.
 * Select the larger capture/portrait limits only for the 4K runtime profile.
 */
#define CONFIG_RTS_ZOOM_S0_WIDTH 2708
#define CONFIG_RTS_ZOOM_S0_HEIGHT 2880
#define CONFIG_RTS_ZOOM_S0_WIDTH_4K 3848
#define CONFIG_RTS_ZOOM_S0_HEIGHT_4K 4328

#define CONFIG_RTS_ZOOM_S1_FMT 0x03
#define CONFIG_RTS_ZOOM_S1_WIDTH 2688
#define CONFIG_RTS_ZOOM_S1_HEIGHT 2880
#define CONFIG_RTS_ZOOM_S1_HEIGHT_4K 4328

/*
 * Stream 2 is 1280x720 (was 640x480).  The OSDI six-block verify
 * (test_osdi.c Phase 5) overlays a 3-col x 2-row grid of 320x240 blocks
 * onto this stream's OSDI instance.  The HAL overlap check (CMP in
 * hal_osdi.h) treats edge-touching blocks as overlapping, so the grid
 * needs frame_w > 3*320 (>960) and frame_h > 2*240 (>480); 640x480 could
 * not host it.  Consumers that request <=640x480 (e.g. test_vin ch2)
 * remain valid — the format check only rejects requests larger than this.
 */
#define CONFIG_RTS_ZOOM_S2_FMT 0x03
#define CONFIG_RTS_ZOOM_S2_WIDTH 2688
#define CONFIG_RTS_ZOOM_S2_HEIGHT 2880
#define CONFIG_RTS_ZOOM_S2_HEIGHT_4K 4328

#define CONFIG_RTS_ZOOM_S3_FMT 0x03
#define CONFIG_RTS_ZOOM_S3_WIDTH 2688
#define CONFIG_RTS_ZOOM_S3_HEIGHT 2880
#define CONFIG_RTS_ZOOM_S3_HEIGHT_4K 4328

#define CONFIG_RTS_ZOOM_S4_FMT 0x0c /* RTSCAM_FORMAT_TYPE_RGB | Y_ONLY — AI channel supports both */
#define CONFIG_RTS_ZOOM_S4_WIDTH 2688
#define CONFIG_RTS_ZOOM_S4_HEIGHT 1520

/* YUV frame buffer DMA addresses (AXI buffer config).
 * Each entry is: stream N, plane 0=Y 1=UV, field = base address and size.
 * Provide physical addresses of the pre-allocated frame buffers.
 * 4 isp stream shared SRAM axi buffer (icfgs) */
#define CONFIG_RTS_YUV_S0_Y_BASE 0x0
#define CONFIG_RTS_YUV_S0_Y_SIZE 0x200
#define CONFIG_RTS_YUV_S0_UV_BASE 0x200
#define CONFIG_RTS_YUV_S0_UV_SIZE 0x100

#define CONFIG_RTS_YUV_S1_Y_BASE 0x300
#define CONFIG_RTS_YUV_S1_Y_SIZE 0x100
#define CONFIG_RTS_YUV_S1_UV_BASE 0x400
#define CONFIG_RTS_YUV_S1_UV_SIZE 0x80

#define CONFIG_RTS_YUV_S2_Y_BASE 0x480
#define CONFIG_RTS_YUV_S2_Y_SIZE 0x80
#define CONFIG_RTS_YUV_S2_UV_BASE 0x500
#define CONFIG_RTS_YUV_S2_UV_SIZE 0x40

#define CONFIG_RTS_YUV_S3_Y_BASE 0x540
#define CONFIG_RTS_YUV_S3_Y_SIZE 0x80
#define CONFIG_RTS_YUV_S3_UV_BASE 0x5c0
#define CONFIG_RTS_YUV_S3_UV_SIZE 0x40

/* Other stream shared SRAM (fcfg) */
#define CONFIG_RTS_STATIS_BUF_BASE 0x0
#define CONFIG_RTS_STATIS_BUF_SIZE 0x300
// MD History
#define CONFIG_RTS_MD0_BUF_BASE 0x300
#define CONFIG_RTS_MD0_BUF_SIZE 0x800
// MD result
#define CONFIG_RTS_MD1_BUF_BASE 0xb00
#define CONFIG_RTS_MD1_BUF_SIZE 0x300
#define CONFIG_RTS_RGB_R_BASE 0xe00
#define CONFIG_RTS_RGB_R_SIZE 0x600
#define CONFIG_RTS_RGB_G_BASE 0x1400
#define CONFIG_RTS_RGB_G_SIZE 0x600
#define CONFIG_RTS_RGB_B_BASE 0x1a00
#define CONFIG_RTS_RGB_B_SIZE 0x600

/* Motion detection registers — TODO: fill in correct address */
#define CONFIG_RTS_MD_BASE 0x600df000 /* TODO: MD register base */
#define CONFIG_RTS_MD_SIZE 0x200
#define CONFIG_RTS_MD_IRQ 0 /* TODO: MD interrupt number */
#define CONFIG_RTS_MD_IRQ_PRIO 2

#define CONFIG_RTS_OSDI_BASE        0x600F4000
#define CONFIG_RTS_OSDI_IRQ0        ISP5_IRQ
#define CONFIG_RTS_OSDI_IRQ1        ISP6_IRQ
#define CONFIG_RTS_OSDI_IRQ2        ISP7_IRQ
#define CONFIG_RTS_OSDI_IRQ_PRIO    2

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

/* Frame buffer memory pool (used by rts_camera_mem.c) */

/* 4K pipeline peak (4 KiB pages): RAW16 capture 4050 + rotated RAW16
 * input 4050 + NV12 output 3038 + 3DNR 2025 + REGCFG 5 = 13168 pages
 * (51.44 MiB), plus one page for base alignment. The optional 1440x2560
 * NV12 sub stream adds 1 x 1350 pages (5.27 MiB) and the 576x320 planar
 * RGB tap 1 x 135 pages (0.53 MiB). 58 MiB leaves ~0.76 MiB (194 pages)
 * headroom; ISP OSD/MD or extra ISP streams also draw from this pool.
 */
/* rtscam_video_init() selects the pool by ISP_Get4KMode(). Normal mode keeps
 * the existing 32 MiB budget for non-4K multi-stream users; 4K mode uses the
 * 58 MiB budget above.
 */
#define CONFIG_RTS_MEM_SIZE (32U * 1024U * 1024U)
#define CONFIG_RTS_MEM_SIZE_4K (58U * 1024U * 1024U)

#endif /* _RTS_CAMERA_HW_CONFIG_H */
