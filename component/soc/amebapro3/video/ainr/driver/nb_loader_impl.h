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

#ifndef _NB_LOADER_IMPL_H_
#define _NB_LOADER_IMPL_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include "./inc/neurobrick_common.h"

/* The neurobrick model binary data (file) is composed as following layout:
1. Fixed section.
    Where fixed information is stored, such as version, layer count.

2. Dynamic section.
    Where the real data is stored, or indexed.
*/

typedef struct _nb_fmt_header {
	nb_char_t      magic[4];    /* the magic of neurobrick model file */
	nb_uint32_t    version;     /* the version of neurobrick model file format */
	nb_uint32_t    layer_count; /* the layer count */
	nb_char_t      reserve[244]; /* reserved for future */
} nb_fmt_header_t;

typedef struct _nb_fmt_layer_cfg_table {
	nb_uint32_t    cfg_count;
	nb_uint32_t    cfg_start_index;
} nb_fmt_layer_cfg_table_t;

typedef struct _nb_fmt_layer_cfg {
	nb_uint32_t    size;
	nb_uint32_t    offset;
} nb_fmt_layer_cfg_t;

typedef struct _nb_fmt {
	nb_fmt_header_t          header;
	nb_fmt_layer_cfg_table_t *layer_cfg_table;
	nb_fmt_layer_cfg_t       *layer_cfg;
	nb_uint32_t              *data;
} nb_fmt_t;

nb_status_e nb_read_model(
	const void *data,
	nb_uint32_t size,
	nb_fmt_t *nb_fmt
);

nb_status_e nb_fmt_dump_impl(
	nb_fmt_t *nb_fmt
);

nb_status_e nb_print(
	IN const nb_char_t *message,
	...
);

nb_status_e nb_config_hardware(
	nb_fmt_t *nb_fmt
);

nb_status_e nb_config_hardware_dma(
	nb_fmt_t *nb_fmt
);

void ainr_set_width_height(uint32_t w, uint32_t h);
void ainr_set_enable(uint32_t enable);
void ainr_set_mode(uint32_t mode);
void ainr_set_pixfmt(uint8_t pixfmt);
void ainr_get_status(void);

#define AINR_MIXUP_WEIGHT1_POS (0x64C)
#define AINR_MIXUP_WEIGHT2_POS (0x9A38)
nb_status_e ainr_mixup_to_weight(double mixup_ratio, uint32_t *mixup_weight1, uint32_t *mixup_weight2);
nb_status_e ainr_weight_to_mixup(uint32_t mixup_weight1, uint32_t mixup_weight2, float *mixup_ratio);

#endif
