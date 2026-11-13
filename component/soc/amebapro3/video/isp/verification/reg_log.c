#include "isp_reg.h"
#include "ameba_soc.h"
#include "isp_regs.h"
#include "rts_errno.h"
#include "isp_mem.h"
#include "isp_io.h"
#include "reg_api.h"
extern struct isp_io g_io;

void ZOOM_DBG_LOG()
{
	int i;
	void *base = g_io.base;
	printf("[%s] 0x%X: 0x%X \n\r", __FUNCTION__, 0xd8c08, read_reg(base, 0xd8c08));
	printf("[%s] 0x%X: 0x%X \n\r", __FUNCTION__, 0xd8c10, read_reg(base, 0xd8c10));
	printf("[%s] 0x%X: 0x%X \n\r", __FUNCTION__, 0xd8c14, read_reg(base, 0xd8c14));
	printf("[%s] 0x%X: 0x%X \n\r", __FUNCTION__, 0xd8c18, read_reg(base, 0xd8c18));
	printf("[%s] 0x%X: 0x%X \n\r", __FUNCTION__, 0xd8c1c, read_reg(base, 0xd8c1c));
	printf("[%s] 0x%X: 0x%X \n\r", __FUNCTION__, 0xd8c20, read_reg(base, 0xd8c20));

	printf("[%s][R_SYS_ZOOM_CONTROL1         ] 0x%X: 0x%X \n\r", __FUNCTION__, R_SYS_ZOOM_CONTROL1, read_reg(base, R_SYS_ZOOM_CONTROL1));
	printf("[%s][R_SYS_ZOOM_SPEED_CTRL       ] 0x%X: 0x%X \n\r", __FUNCTION__, R_SYS_ZOOM_SPEED_CTRL, read_reg(base, R_SYS_ZOOM_SPEED_CTRL));
	printf("[%s][R_SYS_ZOOM_LAST_BREAK       ] 0x%X: 0x%X \n\r", __FUNCTION__, R_SYS_ZOOM_LAST_BREAK, read_reg(base, R_SYS_ZOOM_LAST_BREAK));
	printf("[%s][R_ISP_ZOOM_UP_STEP_H        ] 0x%X: 0x%X \n\r", __FUNCTION__, R_ISP_ZOOM_UP_STEP_H, read_reg(base, R_ISP_ZOOM_UP_STEP_H));
	printf("[%s][R_ISP_ZOOM_UP_STEP_V        ] 0x%X: 0x%X \n\r", __FUNCTION__, R_ISP_ZOOM_UP_STEP_V, read_reg(base, R_ISP_ZOOM_UP_STEP_V));
	printf("[%s][R_ISP_ZOOM_UP_CTRL          ] 0x%X: 0x%X \n\r", __FUNCTION__, R_ISP_ZOOM_UP_CTRL, read_reg(base, R_ISP_ZOOM_UP_CTRL));
	printf("[%s][R_ISP_ZOOM_UP_REG_LOAD      ] 0x%X: 0x%X \n\r", __FUNCTION__, R_ISP_ZOOM_UP_REG_LOAD, read_reg(base, R_ISP_ZOOM_UP_REG_LOAD));
//	printf("[%s][R_SYS_ZOOM_CONTROL1         ] 0x%X: 0x%X \n\r", __FUNCTION__, R_SYS_ZOOM_CONTROL1, read_reg(base, R_SYS_ZOOM_CONTROL1));
	printf("[%s][R_SYS_ZOOM_IMAGE_MODE_SEL   ] 0x%X: 0x%X \n\r", __FUNCTION__, R_SYS_ZOOM_IMAGE_MODE_SEL, read_reg(base, R_SYS_ZOOM_IMAGE_MODE_SEL));
	printf("[%s][VIDEO_YUV422_INTERLEAVED_SEL] 0x%X: 0x%X \n\r", __FUNCTION__, VIDEO_YUV422_INTERLEAVED_SEL, read_reg(base, VIDEO_YUV422_INTERLEAVED_SEL));
	printf("[%s][SYS_IMAGE_SEL1              ] 0x%X: 0x%X \n\r", __FUNCTION__, SYS_IMAGE_SEL1, read_reg(base, SYS_IMAGE_SEL1));
	printf("[%s][SYS_IMAGE_SEL0              ] 0x%X: 0x%X \n\r", __FUNCTION__, 0x0048, read_reg(base, 0x0048));

	for (i = 0; i < 24; i++) {
		printf("[%s][ZOOM_CH0] 0x%X: 0x%X \n\r", __FUNCTION__, 0xd8c00 + 4 * i, read_reg(base, 0xd8c00 + 4 * i));
	}

	for (i = 0; i < 14; i++) {
		printf("[%s][SYS_ZOOM] 0x%X: 0x%X \n\r", __FUNCTION__, 0xd8000 + 4 * i, read_reg(base, 0xd8000 + 4 * i));
	}
	return;
}

void SYS_LOG()
{
	void *base = g_io.base;
	printf("[%s][BASE] 0x%X \n\r", base);
	printf("[%s][VIDEO_Y0_FRAME_ADDR0 ] 0x%X: 0x%X \n\r", __FUNCTION__, VIDEO_Y0_FRAME_ADDR0, read_reg(base, VIDEO_Y0_FRAME_ADDR0));
	printf("[%s][VIDEO_UV0_FRAME_ADDR0] 0x%X: 0x%X \n\r", __FUNCTION__, VIDEO_UV0_FRAME_ADDR0, read_reg(base, VIDEO_UV0_FRAME_ADDR0));
	printf("[%s][VIDEO_FRAME_LEN_Y0   ] 0x%X: 0x%X \n\r", __FUNCTION__, VIDEO_FRAME_LEN_Y0, read_reg(base, VIDEO_FRAME_LEN_Y0));
	printf("[%s][VIDEO_FRAME_LEN_UV0  ] 0x%X: 0x%X \n\r \n\r", __FUNCTION__, VIDEO_FRAME_LEN_UV0, read_reg(base, VIDEO_FRAME_LEN_UV0));
	//write_reg(base, io->out_y_addr, VIDEO_Y0_FRAME_ADDR0);
	//write_reg(base, io->out_uv_addr, VIDEO_UV0_FRAME_ADDR0);
	//write_reg(base, io->out_y_size, VIDEO_FRAME_LEN_Y0);
	//write_reg(base, io->out_uv_size, VIDEO_FRAME_LEN_UV0);

	printf("[%s][VERIFY_Y_DDR_ADDR0 ] 0x%X: 0x%X \n\r", __FUNCTION__, VERIFY_Y_DDR_ADDR0, read_reg(base, VERIFY_Y_DDR_ADDR0));
	printf("[%s][VERIFY_UV_DDR_ADDR0] 0x%X: 0x%X \n\r", __FUNCTION__, VERIFY_UV_DDR_ADDR0, read_reg(base, VERIFY_UV_DDR_ADDR0));
	printf("[%s][VERIFY_Y_DDR_LEN   ] 0x%X: 0x%X \n\r", __FUNCTION__, VERIFY_Y_DDR_LEN, read_reg(base, VERIFY_Y_DDR_LEN));
	printf("[%s][VERIFY_UV_DDR_LEN  ] 0x%X: 0x%X \n\r \n\r", __FUNCTION__, VERIFY_UV_DDR_LEN, read_reg(base, VERIFY_UV_DDR_LEN));
	//write_reg(base, io->in_y_addr, VERIFY_Y_DDR_ADDR0);
	//write_reg(base, io->in_uv_addr, VERIFY_UV_DDR_ADDR0);
	//write_reg(base, io->in_y_size, VERIFY_Y_DDR_LEN);
	//write_reg(base, io->in_uv_size, VERIFY_UV_DDR_LEN);
	printf("[%s][MAX_BURST_LENGTH_SELECT] 0x%X: 0x%X \n\r", __FUNCTION__, 0x000F0190, read_reg(base, 0x000F0190));
	printf("[%s][0x000F0330] 0x%X: 0x%X \n\r", __FUNCTION__, 0x000F0330, read_reg(base, 0x000F0330));
	printf("[%s][0x000F0030] 0x%X: 0x%X \n\r", __FUNCTION__, 0x000F0030, read_reg(base, 0x000F0030));
	printf("[%s][0x000F9030] 0x%X: 0x%X \n\r", __FUNCTION__, 0x000F9030, read_reg(base, 0x000F9030));
	printf("[%s][0x000F9034] 0x%X: 0x%X \n\r", __FUNCTION__, 0x000F9034, read_reg(base, 0x000F9034));
	printf("[%s][0X000F01A0] 0x%X: 0x%X \n\r", __FUNCTION__, 0X000F01A0, read_reg(base, 0X000F01A0));
	return;
}
