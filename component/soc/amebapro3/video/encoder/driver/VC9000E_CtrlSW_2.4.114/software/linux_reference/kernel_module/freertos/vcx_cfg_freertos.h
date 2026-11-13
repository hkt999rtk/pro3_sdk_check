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

#ifndef __VCX__CFG_FREERTOS_H__
#define __VCX__CFG_FREERTOS_H__

/* subsystem definition:
 *   1: defined
 *   0: not defined
 * subsystem0 is defined in default.
 */
#define SUBSYSTEM0    1
#define SUBSYSTEM1    0

#define SUBSYS_MAX (4)

/*0:no resource sharing inter slices (vce subsystems) 1: existing resource sharing*/
#define RESOURCE_SHARED_INTER_SUBSYS  0

/* subsys core offset definition:
 *   0xFFFF: not support this subsys core
 *   others: support this subsys core
 */
#define SUBSYS0_VCMD_OFFSET             (0)
#define SUBSYS0_VCE_OFFSET              (0x1000)
#define SUBSYS0_MMU_OFFSET              (0xFFFF)
#define SUBSYS0_MMU1_OFFSET             (0xFFFF)
#define SUBSYS0_AXIFE_OFFSET            (0xFFFF)
#define SUBSYS0_AXIFE1_OFFSET           (0xFFFF)
#define SUBSYS0_DEC400_OFFSET           (0xFFFF)
#define SUBSYS0_UFBC_OFFSET             (0x1C00)
#define SUBSYS0_L2CACHE_OFFSET          (0xFFFF)

#define SUBSYS1_VCMD_OFFSET             (0)
#define SUBSYS1_CUTREE_OFFSET           (0x1000)
#define SUBSYS1_MMU_OFFSET              (0xFFFF)
#define SUBSYS1_MMU1_OFFSET             (0xFFFF)
#define SUBSYS1_AXIFE_OFFSET            (0xFFFF)
#define SUBSYS1_AXIFE1_OFFSET           (0xFFFF)
#define SUBSYS1_DEC400_OFFSET           (0xFFFF)
#define SUBSYS1_UFBC_OFFSET             (0xFFFF)
#define SUBSYS1_L2CACHE_OFFSET          (0xFFFF)

#include "device_platform_config.h" //AmebaPro3 device config
/******************************************************************
 *         irq cfg for vcmd driver
 ******************************************************************/
#define VCMD0_IRQ                       AMEBAPRO3_ENC_IRQ_NUM
#define VCMD1_IRQ                       -1

/* According to System Memory Map*/
 /*customer specify according to own platform*/
#define SUBSYS_0_IO_ADDR (AMEBAPRO3_ENC_IO_ADDR)    //(0x02300000 + 0x00000)
//#if 0 //for normal driver, VCMD driver don't care
#define SUBSYS_0_IO_SIZE (0x10000) /* bytes */
#define SUBSYS_1_IO_ADDR (0x02300000 + 0x40000)
#define SUBSYS_1_IO_SIZE (0x10000) /* bytes */
#define INT_PIN_SUBSYS_0_VCE 4 /* SUBSYS_0 IRQ */
#define INT_PIN_SUBSYS_0_DEC400 0
//#endif

#define SUBSYS_0_REG_OFFSET_VCE (0x1000)
#define SUBSYS_0_REG_OFFSET_DEC400 (0xFFFF)
/*SubSys 1*/
#define SUBSYS_1_REG_OFFSET_CUTREE (0x1000)
#define SUBSYS_1_REG_OFFSET_DEC400 (0xFFFF)
#define INT_PIN_SUBSYS_1_CUTREE 5
#define INT_PIN_SUBSYS_1_DEC400 1

#endif //__VCX__CFG_FREERTOS_H__
