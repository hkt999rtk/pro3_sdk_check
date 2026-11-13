/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2024 Realtek Semiconductor Corp. All rights reserved.
 *
 * This software is a confidential and proprietary property of Realtek
 * Semiconductor Corp. Disclosure, reproduction, redistribution, in
 * whole or in part, of this work and its derivatives without express
 * permission is prohibited.
 *
 * Realtek Semiconductor Corp. reserves the right to update, modify, or
 * discontinue this software at any time without notice. This software is
 * provided "as is" and any express or implied warranties, including, but
 * not limited to, the implied warranties of merchantability and fitness for
 * a particular purpose are disclaimed. In no event shall Realtek
 * Semiconductor Corp. be liable for any direct, indirect, incidental,
 * special, exemplary, or consequential damages (including, but not limited
 * to, procurement of substitute goods or services; loss of use, data, or
 * profits; or business interruption) however caused and on any theory of
 * liability, whether in contract, strict liability, or tort (including
 * negligence or otherwise) arising in any way out of the use of this software,
 * even if advised of the possibility of such damage.
 */


#ifndef __DRIVERS_RTS_ZOOM_REGS_H__
#define __DRIVERS_RTS_ZOOM_REGS_H__


#define ZOOM_CTRL				0x1400
#define ZOOM_IMG_W_SIZE			0x1404
#define ZOOM_IMG_H_SIZE			0x1408
#define ZOOM_OUTPUT_W_SIZE		0x140C
#define ZOOM_OUTPUT_H_SIZE		0x1410
#define ZOOM_CROP_IMG_W_SIZE	0x1414
#define ZOOM_CROP_IMG_H_SIZE 	0x1418
#define ZOOM_CROP_START_X 		0x141C
#define ZOOM_CROP_START_Y 		0x1420
#define ZOOM_SRC_IMG_FMT 		0x1424
#define ZOOM_SRC_IMG_CTRL		0x1428
#define ZOOM_DST_IMG_FMT 		0x142C
#define ZOOM_DST_IMG_CTRL 		0x1430
#define ZOOM_STEP_V 			0x1434
#define ZOOM_STEP_H				0x1438
#define ZOOM_FULL_LIMIT 		0x143C
#define ZOOM_TRANS_OFFSET0 		0x1440
#define ZOOM_TRANS_OFFSET1 		0x1444
#define ZOOM_R2Y_COEF0			0x1448
#define ZOOM_R2Y_COEF1			0x144C
#define ZOOM_R2Y_COEF2 			0x1450
#define ZOOM_R2Y_COEF3			0x1454
#define ZOOM_R2Y_COEF4          0x1458
#define ZOOM_R2Y_COEF5          0x145C
#define ZOOM_R2Y_COEF6          0x1460
#define ZOOM_R2Y_COEF7          0x1464
#define ZOOM_R2Y_COEF8          0x1468
#define ZOOM_Y2R_COEF0          0x146C
#define ZOOM_Y2R_COEF1          0x1470
#define ZOOM_Y2R_COEF2          0x1474
#define ZOOM_Y2R_COEF3          0x1478
#define ZOOM_Y2R_COEF4          0x147C
#define ZOOM_Y2R_COEF5          0x1480
#define ZOOM_Y2R_COEF6          0x1484
#define ZOOM_Y2R_COEF7          0x1488
#define ZOOM_Y2R_COEF8          0x148C
#define ZOOM_CH0_COEF0          0x1490
#define ZOOM_CH0_COEF1          0x1494
#define ZOOM_CH0_COEF2          0x1498
#define ZOOM_CH0_COEF3          0x149C
#define ZOOM_CH0_COEF4          0x14A0
#define ZOOM_CH0_COEF5          0x14A4
#define ZOOM_CH0_COEF6          0x14A8
#define ZOOM_CH0_COEF7          0x14AC
#define ZOOM_CH0_COEF8          0x14B0
#define ZOOM_CH0_COEF9          0x14B4
#define ZOOM_CH0_COEF10         0x14B8
#define ZOOM_CH0_COEF11         0x14BC
#define ZOOM_CH0_COEF12         0x14C0
#define ZOOM_CH0_COEF13         0x14C4
#define ZOOM_CH0_COEF14         0x14C8
#define ZOOM_CH0_COEF15         0x14CC
#define ZOOM_CH0_COEF16         0x14D0
#define ZOOM_CH0_COEF17         0x14D4
#define ZOOM_CH0_COEF18         0x14D8
#define ZOOM_CH0_COEF19         0x14DC
#define ZOOM_CH0_LINE_SPACE     0x14E0
#define ZOOM_BUFF_FULL_INT      0x14E4
#define ZOOM_BUFF_INT_CLR       0x14E8
#define ZOOM_BUFF_INT_EN        0x14EC
#define ZOOM_FRAME_WR_INT_EN    0x14F0
#define ZOOM_FRAME_WR_INT_CLR   0x14F4
#define ZOOM_FRAME_WR_INT       0x14F8
#define ZOOM_SRC_IMG_ADDR0      0x14FC
#define ZOOM_DST_IMG_ADDR0      0x1500
#define ZOOM_SRC_IMG_ADDR1      0x1504
#define ZOOM_DST_IMG_ADDR1      0x1508
#define ZOOM_SRC_IMG_ADDR2      0x150C
#define ZOOM_DST_IMG_ADDR2      0x1510
#define ZOOM_CFG_RD_LENGTH      0x1514
#define ZOOM_CFG_WR_LENGTH      0x1518
#define ZOOM_HS_W_SIZE          0x151C
#define ZOOM_HS_H_SIZE          0x1520
#define ZOOM_DUM_CLK_ON         0x1524
#define ZOOM_DST_IMG_SIZE_W     0x1528
#define ZOOM_DST_IMG_SIZE_H     0x152C
#define ZOOM_DST_INDEX_X        0x1530
#define ZOOM_DST_INDEX_Y        0x1534
#define ZOOM_RD_DBG_SIG0        0x1600
#define ZOOM_RD_DBG_SIG1        0x1604
#define ZOOM_RD_DBG_SIG2        0x1608
#define ZOOM_WR_DBG_SIG0        0x160C
#define ZOOM_WR_DBG_SIG1        0x1610
#define ZOOM_WR_DBG_SIG2        0x1614
#define ZOOM_FUNC_DBG_SIG       0x1618

#define ZOOM_CLK_EN             0x1808
#define ZOOM_CLK_EN_BITS        1


#endif
