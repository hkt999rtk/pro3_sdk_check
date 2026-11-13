/******************************************************************************\
|* Copyright (c) 2017-2024 by Vivante Corporation.  All Rights Reserved.      *|
|*                                                                            *|
|* The material in this file is confidential and contains trade secrets of    *|
|* of Vivante Corporation.  This is proprietary information owned by Vivante  *|
|* Corporation.  No part of this work may be disclosed, reproduced, copied,   *|
|* transmitted, or used in any way for any purpose, without the express       *|
|* written permission of Vivante Corporation.                                 *|
|*                                                                            *|
\******************************************************************************/

#ifndef _HAL_AINR_H_
#define _HAL_AINR_H_

#include "ameba_soc.h"
#include "./inc/neurobrick_common.h"
#include "./nb_loader_impl.h"


/*
brief read a hardware register
param address the address read.
param data data read to address.
*/
u32 AINR_ReadReg(
	IN  u32 address,
	OUT u32 *data
);

/*
brief write a hardware register
param address the address write.
param data data write to address.
*/
u32 AINR_WriteReg(
	IN  u32 address
);

u32 AINR_WriteData(
	IN  u32 data,
	IN  u32 offset
);

u32 AINR_WriteDataDma(
	IN  unsigned int *data,
	IN  u32 length
);

nb_status_e AINR_ConfigHardware(
	nb_fmt_t *nb_fmt
);

nb_status_e AINR_ConfigHardwareDma(
	nb_fmt_t *nb_fmt
);

void AINR_Init(void);
void AINR_DeInit(void);
void AINR_SetWidthHeight(uint32_t w, uint32_t h);
void AINR_SetEnable(uint32_t enable);
void AINR_SetMode(uint32_t mode);
void AINR_SetPixfmt(uint8_t pixfmt);
void AINR_GetStatus(void);

#endif
