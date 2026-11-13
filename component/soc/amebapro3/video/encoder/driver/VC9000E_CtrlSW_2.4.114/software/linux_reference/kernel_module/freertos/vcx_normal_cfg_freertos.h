/*------------------------------------------------------------------------------
--        Copyright (c) 2015, VeriSilicon Inc. All rights reserved            --

--                                                                            --
-- This software is confidential and proprietary and may be used only as      --
--   expressly authorized by VeriSilicon in a written licensing agreement.    --
--                                                                            --
--         This entire notice must be reproduced on all copies                --
--                       and may not be removed.                              --
--                                                                            --
--------------------------------------------------------------------------------
-- Redistribution and use in source and binary forms, with or without         --
-- modification, are permitted provided that the following conditions are met:--
-- * Redistributions of source code must retain the above copyright notice,   --
--       this list of conditions and the following disclaimer.                --
-- * Redistributions in binary form must reproduce the above copyright        --
--       notice, this list of conditions and the following disclaimer in the  --
--       documentation and/or other materials provided with the distribution. --
-- * Neither the names of VeriSilicon nor the names of its contributors may be--
--       used to endorse or promote products derived from this software       --
--       without specific prior written permission.                           --
--------------------------------------------------------------------------------
-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"--
-- AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE  --
-- IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE --
-- ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE  --
-- LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR        --
-- CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF       --
-- SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS   --
-- INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN    --
-- CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)    --
-- ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE --
-- POSSIBILITY OF SUCH DAMAGE.                                                --
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
*/
#ifndef __VCX_NORMAL_CFG_FREERTOS_H__
#define __VCX_NORMAL_CFG_FREERTOS_H__

#include "vcx_cfg_freertos.h"

/*for all subsystem, the subsys info should be listed here for subsequent use*/
/*base_addr, iosize, resource_shared*/
SUBSYS_CONFIG subsys_array[] = {
	{ SUBSYS_0_IO_ADDR, SUBSYS_0_IO_SIZE, RESOURCE_SHARED_INTER_SUBSYS, 0x90006000 },//subsys_0
#if SUBSYSTEM1
	{SUBSYS_1_IO_ADDR, SUBSYS_1_IO_SIZE, RESOURCE_SHARED_INTER_SUBSYS, 0x90006000}, //subsys_1
#endif
};

/*here config every core in all subsystem*/
/*slice_idx, core_type, offset, reg_size, irq*/
CORE_CONFIG core_array[] = {
	{ 0, CORE_VCE, SUBSYS_0_REG_OFFSET_VCE, 500 * 4, INT_PIN_SUBSYS_0_VCE },
	{ 0, CORE_DEC400, SUBSYS_0_REG_OFFSET_DEC400, 1600 * 4, INT_PIN_SUBSYS_0_DEC400 },
#if SUBSYSTEM1
	{ 1, CORE_CUTREE, SUBSYS_1_REG_OFFSET_CUTREE, 500 * 4, INT_PIN_SUBSYS_1_CUTREE },
	{ 1, CORE_DEC400, SUBSYS_1_REG_OFFSET_DEC400, 1600 * 4, INT_PIN_SUBSYS_1_DEC400},

#endif

};

#endif //__VCX_NORMAL_CFG_FREERTOS_H__
