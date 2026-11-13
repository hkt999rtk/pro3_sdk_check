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

#ifndef _NB_LOADER_H_
#define _NB_LOADER_H_

#include "./neurobrick_common.h"

typedef struct _dma_params {
	nb_uint32_t    LayerID;
	nb_uint32_t    TensorXsizeMinus1;
	nb_uint32_t    TensorYsizeMinus1;
	nb_uint32_t    TensorBPP;
	nb_uint32_t    TensorZsize;
	nb_uint32_t    BaseAddr;
	nb_uint32_t    DMAStride;
} dma_params_t;

typedef enum _nb_hw_query_type {
	NB_HW_QUERY_FRAME_DONE  = 0,
	NB_HW_QUERY_LAYER_BUSY  = 1,
	NB_HW_QUERY_MAX
} nb_hw_query_type_e;

typedef enum _nb_hw_status {
	NB_HW_FRAME_RUN  = 0,
	NB_HW_FRAME_DONE  = 1,
	NB_HW_LAYER_BUSY  = 2,
	NB_HW_LAYER_IDLE  = 3,
	NB_HW_STATUS_MAX
} nb_hw_status_e;

typedef struct _nb_hw_query_params {
	nb_uint32_t  layer;
	nb_hw_query_type_e  type;
} nb_hw_query_params_t;

nb_status_e neurobrick_reset_hardware(
	void);

nb_status_e neurobrick_set_hardware(
	const void *data,
	nb_uint32_t size
);

nb_status_e neurobrick_set_dma_params(
	dma_params_t *params
);

nb_status_e neurobrick_set_dma_prepare(
	nb_uint32_t layer_id
);

/* if query type is hw frame done, layer is not needed */
nb_status_e neurobrick_query_hardware(
	nb_hw_query_params_t *query_params,
	nb_hw_status_e *hw_status
);
#endif
