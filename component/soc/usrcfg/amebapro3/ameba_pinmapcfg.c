/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "ameba_soc.h"

const PMAP_TypeDef pmap_func[] = {
//Pin Name		Func PU/PD					Slp PU/PD
	{_PA_0,			GPIO_PuPd_KEEP,			GPIO_PuPd_DOWN},			 //
	{_PA_1,			GPIO_PuPd_KEEP,			GPIO_PuPd_DOWN},			 //
	{_PA_2,			GPIO_PuPd_KEEP,			GPIO_PuPd_DOWN},			 //
	{_PA_3,			GPIO_PuPd_KEEP,			GPIO_PuPd_DOWN},			 //
	{_PA_4,			GPIO_PuPd_KEEP,			GPIO_PuPd_DOWN},			 //
	{_PA_5,			GPIO_PuPd_KEEP,			GPIO_PuPd_DOWN},			 //
	{_PC_0,			GPIO_PuPd_KEEP,			GPIO_PuPd_DOWN},			 //
	{_PC_1,			GPIO_PuPd_KEEP,			GPIO_PuPd_DOWN},			 //
	{_PC_2,			GPIO_PuPd_KEEP,			GPIO_PuPd_DOWN},			 //
	{_PC_3,			GPIO_PuPd_KEEP,			GPIO_PuPd_UP},			     //
	{_PC_4,			GPIO_PuPd_KEEP,			GPIO_PuPd_UP},			     //
	{_PC_5,			GPIO_PuPd_KEEP,			GPIO_PuPd_UP},			     //
	{_PC_6,			GPIO_PuPd_KEEP,			GPIO_PuPd_DOWN},			 //
	{_PC_7,			GPIO_PuPd_KEEP,			GPIO_PuPd_DOWN},			 //
	{_PC_8,			GPIO_PuPd_KEEP,			GPIO_PuPd_DOWN},			 //
	{_PC_9,			GPIO_PuPd_KEEP,			GPIO_PuPd_DOWN},			 //
	{_PC_10,		GPIO_PuPd_KEEP,			GPIO_PuPd_DOWN},			 //
	{_PE_0,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PE_1,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PE_2,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PE_3,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PE_4,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PE_5,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PE_6,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PE_7,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PE_8,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PE_9,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PE_10,		GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PE_11,		GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PE_12,		GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PE_13,		GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PE_14,		GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PE_15,		GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PF_0,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PF_1,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PF_2,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PF_3,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PF_4,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PF_5,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PF_6,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_UP},			     //
	{_PF_7,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_UP},			     //
	{_PF_8,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PF_9,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PF_10,		GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PF_11,		GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PF_12,		GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PF_13,		GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PF_14,		GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PF_15,		GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PS_0,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PS_1,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_UP},			     //
	{_PS_2,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_UP},			     //
	{_PS_3,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_UP},			     //
	{_PS_4,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_UP},			     //
	{_PS_5,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_UP},			     //
	{_PS_6,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_UP},			     //
	{_PP_0,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_UP},			     //
	{_PP_1,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_UP},			     //
	{_PP_2,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_UP},			     //
	{_PP_3,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_UP},			     //
	{_PP_4,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_UP},			     //
	{_PP_5,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_UP},			     //
	{_PP_6,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_UP},			     //
	{_PP_7,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_UP},			     //
	{_PP_8,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_UP},			     //
	{_PP_9,		    GPIO_PuPd_KEEP,		    GPIO_PuPd_DOWN},			 //
	{_PP_10,		GPIO_PuPd_KEEP,		    GPIO_PuPd_UP},			     //
	{_PP_11,		GPIO_PuPd_KEEP,		    GPIO_PuPd_UP},			     //
	{_PNC,			GPIO_PuPd_KEEP,			GPIO_PuPd_KEEP},			 //table end
};
