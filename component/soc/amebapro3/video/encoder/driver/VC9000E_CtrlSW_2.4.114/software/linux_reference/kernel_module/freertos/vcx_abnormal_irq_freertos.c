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

#include "vcx_abnormal_irq_freertos.h"

extern void proc_add_done_job(vcmd_mgr_t *vcmd_mgr, struct cmdbuf_obj *obj);
/**
 * process vce multi-slice abnormal irq
 */
static void process_vce_slice_irq(vcmd_mgr_t *vcmd_mgr, volatile void *hwregs,
								  struct cmdbuf_obj *obj, u32 irq)
{
	u32 slice_rdy_num;
	u32 *status_va;

	if (obj->slice_run_done == 0) {
		/* status_main_addr is 0 */
		status_va = obj->status_va + 0;
		// clear VCE slice_rdy irq (bit 8 of reg1)
		irq &= 0xffff903;
		iowrite32(irq, (void __iomem *)(hwregs + 0x4));

		// read out & backup slice irq related info
		slice_rdy_num = (u32)ioread32((void __iomem *)(hwregs + 0x1C));

		*(status_va + VCMD_SLICE_RDY_INTERRUPT) = irq;
		*(status_va + VCMD_SLICE_RDY_NUM) = slice_rdy_num;
		obj->slice_run_done = 1;
		proc_add_done_job(vcmd_mgr, obj);
	}
	// TODO, more info need to be read for Xiaomi project.
}

void process_vce_abn_irq(vcmd_mgr_t *vcmd_mgr, struct hantrovcmd_dev *dev,
						struct cmdbuf_obj *obj)
{
	struct vcmd_subsys_info *subsys = dev->subsys_info;

	volatile void *hwregs;
	unsigned long flags;
	u32 irq;

	hwregs = subsys->hwregs[SUB_MOD_MAIN];

	isr_spin_lock_irqsave(&dev->abn_irq_lock, flags);
	// read VCE int_status
	irq = (u32)ioread32((void __iomem *)(hwregs + 0x04));
	if (irq & ASIC_STATUS_SLICE_READY) {
		/* slice rdy process*/
		process_vce_slice_irq(vcmd_mgr, hwregs, obj, irq);
	}
	isr_spin_unlock_irqrestore(&dev->abn_irq_lock, flags);

}
