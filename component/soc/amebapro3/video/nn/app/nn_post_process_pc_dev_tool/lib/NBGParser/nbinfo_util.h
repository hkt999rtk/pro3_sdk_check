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

#ifndef _GC_NBINFO_UTIL_H
#define _GC_NBINFO_UTIL_H

#include <nbg_parser.h>

#define vipdMEMORY_ALIGN_SIZE         64

vip_uint32_t nbg_get_sw_operation_count(
	nbg_fmt_descriptor_t *nbg
);

vip_uint32_t calc_command_size(
	nbg_fmt_descriptor_t *nbg,
	vip_uint32_t operation_type,
	vip_uint32_t operation_index
);
#endif
