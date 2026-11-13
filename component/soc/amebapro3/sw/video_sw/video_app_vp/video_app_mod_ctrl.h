/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef __VIDEO_APP_MOD_CTRL_H__
#define __VIDEO_APP_MOD_CTRL_H__

#include <stdlib.h>
#include "module_encoder.h"
#include "module_isp.h"

int vp_isp_mod_ctrl(int ch, uint32_t cmd, uint32_t arg1, uint32_t arg2);
int vp_enc_mod_ctrl(int ch, uint32_t cmd, uint32_t arg1, uint32_t arg2);

#endif /* __VIDEO_APP_MOD_CTRL_H__ */
