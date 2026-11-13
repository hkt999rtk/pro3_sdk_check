/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef _ISP_REGS_H_
#define _ISP_REGS_H_

/* System control registers */
#define SYS_CONTROL0                 0x0000
#define SYS_DATA_DELAY               0x009c

/* Video stream control */
#define VIDEO_STREAM0_ENABLE         0xF0000
#define VIDEO_YUV422_INTERLEAVED_SEL 0xF0074
#define VIDEO_STREAM0_BUFFER_STATUS  0xF0080

/* Frame buffer address registers (Y channel, stream 0-3) */
#define VIDEO_Y0_FRAME_ADDR0         0xF00C0
#define VIDEO_Y0_FRAME_ADDR1         0xF00C4

/* Frame length registers */
#define VIDEO_FRAME_LEN_Y0           0xF02A0
#define VIDEO_FRAME_LEN_UV0          0xF02A4
#define VIDEO_FRAME_LEN_Y1           0xF02A8
#define VIDEO_FRAME_LEN_UV1          0xF02AC

#endif /* _ISP_REGS_H_ */
