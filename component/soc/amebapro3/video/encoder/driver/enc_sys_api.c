/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "ewl.h"
#include "hal_enc.h"

/* Init ewl instance to malloc/free buffer */
void *enc_init_ewl_inst(void)
{
	/* Init EWL instance for test bench to malloc/free linear buffer*/
	EWLInitParam_t param;
	param.context = NULL;
	param.enc_dev = "/tmp/dev/vsi_vcx";
	param.mem_dev = "/tmp/dev/memalloc";
	param.clientType = EWL_CLIENT_TYPE_MEM; //buffer operation
	param.slice_idx = 0;
	param.useVcmd = 1;
	return EWLInit(&param);
}

void enc_deinit_ewl_inst(void *inst)
{
	if (inst) {
		EWLRelease(inst);
	}
}

/* ENC HW initialization */
void enc_rtk_hw_init(void)
{
	ENC_Init();
}

