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
/* our own stuff */
#include "vcx_driver_freertos.h"
#include "io_tools.h"
#include "frameworks.h"
#include "vcx_normal_cfg_freertos.h"
#include "user_freertos.h"

/****************************************************************
 * external/global variables declarations
 ***************************************************************/
#ifdef HANTROMMU_SUPPORT
extern unsigned int mmu_enable;
#endif
unsigned int pcie; /* used in vcx_mmu_freertos.c*/

extern volatile u32 g_spin_lock_cnt;

/* here's all the must remember stuff */
typedef struct {
	SUBSYS_DATA subsys_data; //config of each core,such as base addr, iosize,etc
	u32 hw_id; //VCE/VCEJ hw id to indicate project
	u32 subsys_id; //subsys id for driver and sw internal use
	u32 is_valid; //indicate this subsys is hantro's core or not
	int pid[CORE_MAX]; //indicate which process is occupying the subsys
	u32 is_reserved[CORE_MAX]; //indicate this subsys is occupied by user or not
	u32 irq_received[CORE_MAX]; //indicate which core receives irq
	u32 irq_status[CORE_MAX]; //IRQ status of each core
	// poll sliceinfo timeout IRQ status of each core
	u32 poll_sliceinfo_timeout_irq_status[CORE_MAX];
	// ufbc decode error IRQ status of each core
	u32 ufbc_dec_err_irq_status[CORE_MAX];
	u32 job_id[CORE_MAX];

	volatile u8 *hwregs;

} hantroenc_t;

static struct subsys_manager {
	int total_subsys_num;
	unsigned int mmu_enable;
	volatile u8 *mmu_hwregs[HXDEC_MAX_CORES][2];

	/* dynamic allocation*/
	hantroenc_t *hantroenc_data;

	wait_queue_head_t hw_queue;
	spinlock_t owner_lock;
	wait_queue_head_t enc_wait_queue;

	SemaphoreHandle_t enc_core_sem;

	unsigned long sram_base;
	unsigned int sram_size;
	int linebuffer_sram_used;
	/* and this is our MAJOR; use 0 for dynamic allocation (recommended)*/
	int hantroenc_major;

} subsys_mgr;

static int ReserveIO(void);
static void ReleaseIO(void);

#ifdef hantroenc_DEBUG
static void dump_regs(unsigned long data);
#endif

/* IRQ handler */
static irqreturn_t hantroenc_isr(/*int irq, */ void *dev_id);

/**
 * @brief set interrupt gate, and not gate the interrupt from IPs to cpu
 */
static void enc_interrupt_gate_set(hantroenc_t *dev)
{
	int i;
	volatile u8 *hwregs;

	for (i = 0; i < subsys_mgr.total_subsys_num; i++) {
		hwregs = dev[i].hwregs + dev[i].subsys_data.core_info.offset[CORE_VCMD];
		if (hwregs)
			iowrite32(0x0000, (void __iomem *)(hwregs + 25 * 4));
	}
}

static int CheckEncIrq(hantroenc_t *dev, u32 *core_info, u32 *irq_status,
			   u32 *job_id)
{
	unsigned long flags;
	int rdy = 0;
	u8 core_type = 0;
	u8 subsys_idx = 0;

	core_type = (u8)(*core_info & 0x0F);
	subsys_idx = (u8)(*core_info >> 4);

	if (subsys_idx > subsys_mgr.total_subsys_num - 1) {
		*core_info = -1;
		*irq_status = 0;
		return 1;
	}

	spin_lock_irqsave(&subsys_mgr.owner_lock, flags);

	if (dev[subsys_idx].irq_received[core_type]) {
		/* reset the wait condition(s) */
		PDEBUG("check subsys[%d][%d] irq ready\n", subsys_idx, core_type);
		//dev[subsys_idx].irq_received[core_type] = 0;
		rdy = 1;
		*core_info = subsys_idx;
		*irq_status = dev[subsys_idx].irq_status[core_type];
		if (job_id)
			*job_id = dev[subsys_idx].job_id[core_type];
	}

#ifdef LOW_LATENCY_SLICEINFO_SUPPORT
	if (dev[subsys_idx].poll_sliceinfo_timeout_irq_status[core_type] &
			ASIC_STATUS_POLL_SLICEINFO_TIMEOUT && job_id) {
		rdy = 1;
		*core_info = subsys_idx;
		*irq_status |= dev[subsys_idx].poll_sliceinfo_timeout_irq_status[core_type];
		*job_id = dev[subsys_idx].job_id[core_type];
	}
#endif

#ifdef SUPPORT_UFBC
	if (dev[subsys_idx].ufbc_dec_err_irq_status[core_type] &
			ASIC_STATUS_UFBC_DEC_ERR && job_id) {
		rdy = 1;
		*core_info = subsys_idx;
		*irq_status |= dev[subsys_idx].ufbc_dec_err_irq_status[core_type];
		*job_id = dev[subsys_idx].job_id[core_type];
	}
#endif

	spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);

	return rdy;
}

static int WaitEncReady(hantroenc_t *dev, u32 *core_info,
				 u32 *irq_status)
{
	PDEBUG("WaitEncReady\n");
	if (wait_event_interruptible(subsys_mgr.enc_wait_queue,
					 CheckEncIrq(dev, core_info, irq_status, NULL))) {
		PDEBUG("ENC wait_event_interruptible interrupted\n");
		return -ERESTARTSYS;
	}
	return 0;
}

static int CheckEncIrqbyPolling(hantroenc_t *dev, u32 *core_info,
				u32 *irq_status, u32 *job_id)
{
	unsigned long flags;
	int rdy = 0;
	u8 core_type = 0;
	u8 subsys_idx = 0;
	u32 irq, hwId, majorId, wClr, minorId, productId;
	unsigned long reg_offset = 0;
	u32 loop = 300;
	u32 interval = 10;

	core_type = (u8)(*core_info & 0x0F);
	subsys_idx = (u8)(*core_info >> 4);

	if (subsys_idx > subsys_mgr.total_subsys_num - 1) {
		*core_info = -1;
		*irq_status = 0;
		return 1;
	}

	do {
		spin_lock_irqsave(&subsys_mgr.owner_lock, flags);
		if (dev[subsys_idx].is_reserved[core_type] == 0) {
			/*printk(KERN_DEBUG"subsys[%d][%d]  is not reserved\n",
			 *subsys_idx, core_type);
			 */
			goto end_1;
		} else if (dev[subsys_idx].irq_received[core_type] &&
			   (dev[subsys_idx].irq_status[core_type] &
				(ASIC_STATUS_FUSE_ERROR | ASIC_STATUS_HW_TIMEOUT |
				 ASIC_STATUS_BUFF_FULL | ASIC_STATUS_HW_RESET |
				 ASIC_STATUS_ERROR | ASIC_STATUS_FRAME_READY))) {
			rdy = 1;
			*core_info = subsys_idx;
			*irq_status = dev[subsys_idx].irq_status[core_type];
			*job_id = dev[subsys_idx].job_id[core_type];
			goto end_1;
		}

		reg_offset =
			dev[subsys_idx].subsys_data.core_info.offset[core_type];
		irq = (u32)ioread32(
			(void __iomem *)(dev[subsys_idx].hwregs + reg_offset + 0x04));

		if (irq & ASIC_STATUS_ALL) {
			PDEBUG("check subsys[%d][%d] irq ready\n", subsys_idx,
				   core_type);
			if (irq & 0x20)
				iowrite32(0, (void __iomem *)(dev[subsys_idx].hwregs +
							  reg_offset + 0x14));

			/* clear all IRQ bits. (hwId >= 0x80006100) means IRQ is cleared
			 * by writing 1
			 */
			hwId = ioread32((void __iomem *)dev[subsys_idx].hwregs +
					reg_offset);
			productId = (hwId & 0xFFFF0000) >> 16;
			majorId = (hwId & 0x0000FF00) >> 8;
			minorId = (hwId & 0x000000FF);
			wClr = ((majorId >= 0x61) || (productId == 0x9000) || (productId == 0x9010) ||
			((productId == 0x8000) && (minorId >= 1))) ? irq : (irq & (~0x1FD));
			iowrite32(wClr, (void __iomem *)(dev[subsys_idx].hwregs +
						 reg_offset + 0x04));

			rdy = 1;
			*core_info = subsys_idx;
			*irq_status = irq;
			dev[subsys_idx].irq_received[core_type] = 1;
			dev[subsys_idx].irq_status[core_type] = irq;
			*job_id = dev[subsys_idx].job_id[core_type];
			goto end_1;
		}

#ifdef LOW_LATENCY_SLICEINFO_SUPPORT
		u32 poll_sliceinfo_irq_status = 0;

		poll_sliceinfo_irq_status =
			 (u32)ioread32((void __iomem *)(dev[subsys_idx].hwregs + reg_offset + 0x1d8));
		poll_sliceinfo_irq_status = (poll_sliceinfo_irq_status & 0x01) << 13;
		if (poll_sliceinfo_irq_status & ASIC_STATUS_POLL_SLICEINFO_TIMEOUT) {
			wClr = poll_sliceinfo_irq_status >> 13;
			iowrite32(wClr, (void __iomem *)(dev[subsys_idx].hwregs + reg_offset + 0x1d8));
			dev[subsys_idx].poll_sliceinfo_timeout_irq_status[core_type] =
				poll_sliceinfo_irq_status;
			rdy = 1;
			*core_info = subsys_idx;
			dev[subsys_idx].irq_received[core_type] = 1;
			*irq_status =
				 dev[subsys_idx].poll_sliceinfo_timeout_irq_status[core_type];
			*job_id = dev[subsys_idx].job_id[core_type];
			goto end_1;
		}
#endif

#ifdef SUPPORT_UFBC
		u32 ufbc_dec_irq_status = 0;

		ufbc_dec_irq_status =
			 (u32)ioread32((void __iomem *)(dev[subsys_idx].hwregs + reg_offset + 0xc20));
		ufbc_dec_irq_status = (ufbc_dec_irq_status & 0x01) << 14;
		if (ufbc_dec_irq_status & ASIC_STATUS_UFBC_DEC_ERR) {
			wClr = ufbc_dec_irq_status >> 14;
			iowrite32(wClr, (void __iomem *)(dev[subsys_idx].hwregs + reg_offset + 0xc20));
			dev[subsys_idx].ufbc_dec_err_irq_status[core_type] =
				ufbc_dec_irq_status;
			rdy = 1;
			*core_info = subsys_idx;
			*irq_status = irq;
			*irq_status |=
				 dev[subsys_idx].ufbc_dec_err_irq_status[core_type];
			*job_id = dev[subsys_idx].job_id[core_type];
			goto end_1;
		}
#endif

		spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);
		mdelay(interval);
	} while (loop--);
	goto end_2;

end_1:
	spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);
end_2:
	return rdy;
}

static int CheckEncAnyIrqByPolling(hantroenc_t *dev, CORE_WAIT_OUT *out)
{
	u32 i;
	int rdy = 0;
	u32 core_info, irq_status, job_id;
	u32 core_type = CORE_VCE;

	for (i = 0; i < subsys_mgr.total_subsys_num; i++) {
		if (!(dev[i].subsys_data.core_info.type_info & (1 << core_type)))
			continue;

		core_info = ((i << 4) | core_type);
		if ((CheckEncIrqbyPolling(dev, &core_info, &irq_status, &job_id) == 1) &&
			(core_info == i)) {
			out->job_id[out->irq_num] = job_id;
			out->irq_status[out->irq_num] = irq_status;
			//printk(KERN_DEBUG "irq_status of subsys %d job_id %d is:%x\n",i,job_id,irq_status);
			out->irq_num++;
			rdy = 1;
		}
	}

	return rdy;
}

static unsigned int WaitEncAnyReadyByPolling(hantroenc_t *dev, CORE_WAIT_OUT *out)
{
	CheckEncAnyIrqByPolling(dev, out);
	return 0;
}

static int CheckEncAnyIrq(hantroenc_t *dev, CORE_WAIT_OUT *out)
{
	u32 i;
	int rdy = 0;
	u32 core_info, irq_status, job_id;
	u32 core_type = CORE_VCE;

	for (i = 0; i < subsys_mgr.total_subsys_num; i++) {
		if (!(dev[i].subsys_data.core_info.type_info &
			  (1 << core_type)))
			continue;

		core_info = ((i << 4) | core_type);
		if ((CheckEncIrq(dev, &core_info, &irq_status,
					  &job_id) == 1) && core_info == i) {
			out->job_id[out->irq_num] = job_id;
			out->irq_status[out->irq_num] = irq_status;
			/*printk(KERN_DEBUG "irq_status of subsys %d job_id %d is:%x\n",
			 *i,job_id,irq_status);
			 */
			out->irq_num++;
			rdy = 1;
		}
	}

	return rdy;
}

static unsigned int WaitEncAnyReady(hantroenc_t *dev, CORE_WAIT_OUT *out)
{
	if (wait_event_interruptible(subsys_mgr.enc_wait_queue,
					 CheckEncAnyIrq(dev, out))) {
		PDEBUG("ENC wait_event_interruptible interrupted\n");
		return -ERESTARTSYS;
	}

	return 0;
}

static int CheckCoreOccupation(hantroenc_t *dev, u8 core_type)
{
	int ret = 0;
	unsigned long flags;

	core_type = (core_type == CORE_VCEJ ? CORE_VCE : core_type);

	spin_lock_irqsave(&subsys_mgr.owner_lock, flags);
	if (!dev->is_reserved[core_type]) {
		dev->is_reserved[core_type] = 1;
		int current_pthread_id = getpid();

		dev->pid[core_type] = current_pthread_id;
		ret = 1;
		PDEBUG("CheckCoreOccupation pid=%d\n", dev->pid[core_type]);
	}

	spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);

	return ret;
}

static int GetWorkableCore(hantroenc_t *dev, u32 *core_info, u32 *core_info_tmp)
{
	int ret = 0;
	u32 i = 0;
	u32 cores;
	u8 core_type = 0;
	static u32 reserved_job_id;
	unsigned long flags;
	/*input core_info[32 bit]: mode[1bit](1:all 0:specified)+
	 *reserved feature[3bit](reserved)+reserved+core_type[8bit]

	 *output core_info[32 bit]: the reserved core info to user space and
	 *defined as below.
	 *mode[1bit](1:all 0:specified)+amount[3bit](reserved total core num)+
	 *reserved+subsys_mapping[8bit]
	 */
	cores = *core_info;
	core_type = (u8)(cores & 0xFF);

	PDEBUG("GetWorkableCore: core_info=%x\n", *core_info);

	/* a valid free Core with specified core type */
	for (i = 0; i < subsys_mgr.total_subsys_num; i++) {
		if (dev[i].subsys_data.core_info.type_info & (1 << core_type)) {
			core_type = (core_type == CORE_VCEJ ? CORE_VCE : core_type);
			if (dev[i].is_valid && CheckCoreOccupation(&dev[i], core_type)) {
				*core_info_tmp |= (1 << i);
				spin_lock_irqsave(&subsys_mgr.owner_lock, flags);
				*core_info = (reserved_job_id << 16) | (*core_info_tmp & 0xFF);
				dev[i].job_id[core_type] = reserved_job_id;
				/*maintain job_id in 16 bits for core_info can
				 *only save job_id in high 16 bits
				 */
				reserved_job_id = (reserved_job_id + 1) & 0xFFFF;
				spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);
				core_info_tmp = 0;
				ret = 1;
				break;
			}
		}
	}

	PDEBUG("*core_info = %x\n", *core_info);
	return ret;
}

static long ReserveEncoder(hantroenc_t *dev, u32 *core_info)
{
	u32 core_info_tmp = 0;

	/*If HW resources are shared inter cores, just make sure only one is
	 *using the HW
	 */
	if (dev[0].subsys_data.cfg.resource_shared) {
		if (down_interruptible(&subsys_mgr.enc_core_sem))
			return -ERESTARTSYS;
	}

	/* lock a core that has specified core id*/
	if (wait_event_interruptible(subsys_mgr.hw_queue,
					 GetWorkableCore(dev, core_info, &core_info_tmp) != 0))
		return -ERESTARTSYS;
	return 0;
}

static void ReleaseEncoder(hantroenc_t *dev, u32 *core_info)
{
	unsigned long flags;
	u8 core_type = 0, subsys_idx = 0, unCheckPid = 0;

	unCheckPid = (u8)((*core_info) >> 31);
	subsys_idx = (u8)((*core_info & 0xF0) >> 4);
	core_type = (u8)(*core_info & 0x0F);

	PDEBUG("ReleaseEncoder:subsys_idx=%d,core_type=%x\n", subsys_idx,
		   core_type);
	/* release specified subsys and core type */
	int current_pthread_id = getpid();

	if (dev[subsys_idx].subsys_data.core_info.type_info &
		(1 << core_type)) {
		core_type =
			(core_type == CORE_VCEJ ? CORE_VCE : core_type);
		spin_lock_irqsave(&subsys_mgr.owner_lock, flags);
		PDEBUG("subsys[%u].pid[%u]=%d,current_pthread_id=%d\n",
				subsys_idx, core_type, dev[subsys_idx].pid[core_type],
				current_pthread_id);
		if (dev[subsys_idx].is_reserved[core_type] &&
			(dev[subsys_idx].pid[core_type] == current_pthread_id ||
			 unCheckPid == 1)) {
			dev[subsys_idx].pid[core_type] = -1;
			dev[subsys_idx].is_reserved[core_type] = 0;
			dev[subsys_idx].irq_received[core_type] = 0;
			dev[subsys_idx].irq_status[core_type] = 0;
			dev[subsys_idx].job_id[core_type] = 0;
#ifdef LOW_LATENCY_SLICEINFO_SUPPORT
			dev[subsys_idx].poll_sliceinfo_timeout_irq_status[core_type] = 0;
#endif
#ifdef SUPPORT_UFBC
			dev[subsys_idx].ufbc_dec_err_irq_status[core_type] = 0;
#endif
			spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);
		} else {
			if (dev[subsys_idx].pid[core_type] != current_pthread_id &&
				unCheckPid == 0) {
				printk(KERN_ERR
					"WARNING:pid(%d) is trying to release core reserved by pid(%d)\n",
					current_pthread_id, dev[subsys_idx].pid[core_type]);
			}
			spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);
		}
	}
	wake_up_interruptible_all(&subsys_mgr.hw_queue);
	if (dev->subsys_data.cfg.resource_shared)
		up(&subsys_mgr.enc_core_sem);

	return;
}

long hantroenc_ioctl(rtos_vpu_filp *filp, unsigned int cmd, void *arg)
{
	int err = 0;
	unsigned int tmp;
	hantroenc_t *hantroenc_data = subsys_mgr.hantroenc_data;

	printk(KERN_INFO "ioctl cmd %08ux\n", cmd);
	/* extract the type and number bitfields, and don't encode
	 * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
	 */
	if (_IOC_TYPE(cmd) != HANTRO_IOC_MAGIC
#ifdef HANTROMMU_SUPPORT
		&& _IOC_TYPE(cmd) != HANTRO_IOC_MMU
#endif
	)
		return -ENOTTY;
	if ((_IOC_TYPE(cmd) == HANTRO_IOC_MAGIC &&
		 _IOC_NR(cmd) > HANTRO_IOC_MAXNR)
#ifdef HANTROMMU_SUPPORT
		|| (_IOC_TYPE(cmd) == HANTRO_IOC_MMU &&
		_IOC_NR(cmd) > HANTRO_IOC_MMU_MAXNR)
#endif
	)
		return -ENOTTY;

	/* the direction is a bitmask, and VERIFY_WRITE catches R/W
	 * transfers. `Type' is user-oriented, while
	 * access_ok is kernel-oriented, so the concept of "read" and
	 * "write" is reversed
	 */
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
	if (err)
		return -EFAULT;

	switch (cmd) {
	case HANTRO_IOCH_GET_VCMD_ENABLE: {
		__put_user(0, (unsigned long *)arg);
		break;
	}
    case HANTRO_IOCH_GET_MMU_ENABLE: {
#ifdef HANTROMMU_SUPPORT
		__put_user(subsys_mgr.mmu_enable, (unsigned int *) arg);
		break;
#endif
    }


	case HANTRO_IOCG_HWOFFSET: {
		u32 id;

		__get_user(id, (u32 __user *)arg);

		if (id >= subsys_mgr.total_subsys_num) {
			return -EFAULT;
		}
		__put_user(hantroenc_data[id].subsys_data.cfg.base_addr,
			   (unsigned long __user *)arg);
		break;
	}

	case HANTRO_IOCG_HWIOSIZE: {
		u32 id;
		u32 io_size;

		__get_user(id, (u32 __user *)arg);

		if (id >= subsys_mgr.total_subsys_num) {
			return -EFAULT;
		}
		io_size = hantroenc_data[id].subsys_data.cfg.iosize;
		__put_user(io_size, (u32 __user *)arg);

		return 0;
	}
	case HANTRO_IOCG_SRAMOFFSET:
		__put_user(subsys_mgr.sram_base, (unsigned long __user *)arg);
		break;
	case HANTRO_IOCG_SRAMEIOSIZE:
		__put_user(subsys_mgr.sram_size, (unsigned int __user *)arg);
		break;
	case HANTRO_IOCG_CORE_NUM:
		__put_user(subsys_mgr.total_subsys_num, (unsigned int __user *)arg);
		break;
	case HANTRO_IOCG_CORE_INFO: {
		u32 idx;
		SUBSYS_CORE_INFO in_data;

		tmp = copy_from_user(&in_data, (void __user *)arg, sizeof(SUBSYS_CORE_INFO));
		if (tmp) {
			printk(KERN_INFO "copy_from_user failed, returned %u\n", tmp);
			return -EFAULT;
		}
		idx = in_data.type_info;
		if (idx > subsys_mgr.total_subsys_num - 1)
			return -1;

		tmp = copy_to_user((void __user *)arg,
				 &hantroenc_data[idx].subsys_data.core_info,
				 sizeof(SUBSYS_CORE_INFO));
		if (tmp) {
			printk(KERN_INFO "copy_to_user failed, returned %u\n", tmp);
			return -EFAULT;
		}
		break;
	}
	case HANTRO_IOCH_ENC_RESERVE: {
		u32 core_info;
		int ret;

		PDEBUG("Reserve ENC Cores\n");
		__get_user(core_info, (u32 __user *)arg);
		ret = ReserveEncoder(hantroenc_data, &core_info);
		if (ret == 0)
			__put_user(core_info, (u32 __user *)arg);
		return ret;
	}
	case HANTRO_IOCH_ENC_RELEASE: {
		u32 core_info;

		__get_user(core_info, (u32 __user *)arg);

		PDEBUG("Release ENC Core\n");

		ReleaseEncoder(hantroenc_data, &core_info);

		break;
	}

	case HANTRO_IOCG_CORE_WAIT: {
		u32 core_info;
		u32 irq_status;

		__get_user(core_info, (u32 __user *)arg);

		tmp = WaitEncReady(hantroenc_data, &core_info, &irq_status);
		if (tmp == 0) {
			__put_user(irq_status, (unsigned int __user *)arg);
			return core_info; //return core_id
		} else {
			__put_user(0, (unsigned int *)arg);
			return -1;
		}
		break;
	}

	case HANTRO_IOCG_ANYCORE_WAIT_POLLING: {
		CORE_WAIT_OUT out;

		memset(&out, 0, sizeof(CORE_WAIT_OUT));
		tmp = WaitEncAnyReadyByPolling(hantroenc_data, &out);
		if (tmp == 0) {
			tmp = copy_to_user((void __user *)arg, &out, sizeof(CORE_WAIT_OUT));
			if (tmp) {
				printk(KERN_INFO "copy_to_user failed, returned %u\n", tmp);
				return -EFAULT;
			}
			return 0;
		} else {
			return -1;
		}

		break;
	}

	case HANTRO_IOCG_ANYCORE_WAIT: {
		CORE_WAIT_OUT out;

		memset(&out, 0, sizeof(CORE_WAIT_OUT));
		tmp = WaitEncAnyReady(hantroenc_data, &out);
		if (tmp == 0) {
			tmp = copy_to_user((void __user *)arg, &out, sizeof(CORE_WAIT_OUT));
			if (tmp) {
				printk(KERN_INFO "copy_to_user failed, returned %u\n", tmp);
				return -EFAULT;
			}
			return 0;
		} else {
			return -1;
		}

		break;
	}
	default: {
#ifdef HANTROMMU_SUPPORT
		if (_IOC_TYPE(cmd) == HANTRO_IOC_MMU) {
			if (mmu_enable)
				return MMUIoctl(cmd, filp, arg, subsys_mgr.mmu_hwregs);
			else
				return -1;
		}
#else
		return -ENOTTY;
#endif
	}
	}
	return 0;
}

int hantroenc_open(void *inode, rtos_vpu_filp *filp)
{
	int result = 0;

	printk(KERN_INFO "dev opened\n");
	return result;
}

int hantroenc_release(void *inode, rtos_vpu_filp *filp)
{
	hantroenc_t *dev = subsys_mgr.hantroenc_data;
	u32 core_id = 0, i = 0;
#ifdef HANTROMMU_SUPPORT
	u32 has_mmu = 0;
#endif

#ifdef hantroenc_DEBUG
	dump_regs((unsigned long)dev); /* dump the regs */
#endif
	unsigned long flags;

	PDEBUG("dev closed\n");

	for (i = 0; i < subsys_mgr.total_subsys_num; i++) {
		for (core_id = 0; core_id < CORE_MAX; core_id++) {
			spin_lock_irqsave(&subsys_mgr.owner_lock, flags);
			int current_pthread_id = getpid();

			if (dev[i].is_reserved[core_id] == 1 &&
			    dev[i].pid[core_id] == current_pthread_id) {
				dev[i].pid[core_id] = -1;
				dev[i].is_reserved[core_id] = 0;
				dev[i].irq_received[core_id] = 0;
				dev[i].irq_status[core_id] = 0;
#ifdef LOW_LATENCY_SLICEINFO_SUPPORT
				dev[i].poll_sliceinfo_timeout_irq_status[core_id] = 0;
#endif
#ifdef SUPPORT_UFBC
				dev[i].ufbc_dec_err_irq_status[core_id] = 0;
#endif
				PDEBUG("release reserved core\n");
			}
			spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);
		}
	}

#ifdef HANTROMMU_SUPPORT
		if (has_mmu)
			MMURelease(filp);
#endif

	wake_up_interruptible_all(&subsys_mgr.hw_queue);

	if (dev->subsys_data.cfg.resource_shared)
		up(&subsys_mgr.enc_core_sem);

	return 0;
}

void __exit hantroenc_normal_cleanup(void)
{
	int i = 0, j = 0;
	hantroenc_t *hantroenc_data = subsys_mgr.hantroenc_data;

	for (i = 0; i < subsys_mgr.total_subsys_num; i++) {
		if (hantroenc_data[i].is_valid == 0)
			continue;

		/* free the core IRQ */
		for (j = 0; j < CORE_MAX; j++) {
			if (hantroenc_data[i].subsys_data.core_info.irq[j] !=-1) {
				free_irq(hantroenc_data[i].subsys_data.core_info.irq[j],
					 (void *)&hantroenc_data[i]);
			}
		}
	}

#ifdef HANTROMMU_SUPPORT
	MMUCleanup();
#endif

	ReleaseIO();
	vfree(subsys_mgr.hantroenc_data);

	unregister_chrdev(subsys_mgr.hantroenc_major, "vsi_vcx");

	printk(KERN_INFO "hantroenc: module removed\n");
	return;
}

int __init hantroenc_normal_init(rtos_vpu_dev *_vdev)
{
	int result = 0;
	int i, j, array_sz;
	int irq_enabled = 0;
	hantroenc_t *hantroenc_data;

#ifdef HANTROMMU_SUPPORT
	enum MMUStatus mmu_status = MMU_STATUS_FALSE;
#endif

	subsys_mgr.total_subsys_num = sizeof(subsys_array) / sizeof(SUBSYS_CONFIG);
	for (i = 0; i < subsys_mgr.total_subsys_num; i++) {
		printk(KERN_INFO
				"hantroenc: module init - subsys[%d] addr =%p\n", i,
				(void *)subsys_array[i].base_addr);
	}

	hantroenc_data = vmalloc(sizeof(hantroenc_t) * subsys_mgr.total_subsys_num);
	if (!hantroenc_data)
		goto err1;
	memset(hantroenc_data, 0, sizeof(hantroenc_t) * subsys_mgr.total_subsys_num);
	subsys_mgr.hantroenc_data = hantroenc_data;

	for (i = 0; i < subsys_mgr.total_subsys_num; i++) {
		hantroenc_data[i].subsys_data.cfg = subsys_array[i];
		hantroenc_data[i].hwregs = NULL;
		hantroenc_data[i].subsys_id = i;
		for (j = 0; j < CORE_MAX; j++)
			hantroenc_data[i].subsys_data.core_info.irq[j] = -1;
	}
	init_waitqueue_head(&subsys_mgr.hw_queue);
	spin_lock_init(&subsys_mgr.owner_lock);
	init_waitqueue_head(&subsys_mgr.enc_wait_queue);

	array_sz = sizeof(core_array) / sizeof(CORE_CONFIG);
	for (i = 0; i < array_sz; i++) {
		if (core_array[i].offset == 0xFFFF)
			continue;
		hantroenc_data[core_array[i].subsys_idx].subsys_data.core_info.type_info |= (1 << (core_array[i].core_type));
		hantroenc_data[core_array[i].subsys_idx].subsys_data.core_info.offset[core_array[i].core_type] = core_array[i].offset;
		hantroenc_data[core_array[i].subsys_idx].subsys_data.core_info.regSize[core_array[i].core_type] = core_array[i].reg_size;
		hantroenc_data[core_array[i].subsys_idx].subsys_data.core_info.irq[core_array[i].core_type] = core_array[i].irq;
	}

	result = register_chrdev(subsys_mgr.hantroenc_major, "vsi_vcx", &hantroenc_fops);
	if (result < 0) {
		printk(KERN_INFO "hantroenc: unable to get major <%d>\n",
		       subsys_mgr.hantroenc_major);
		goto err1;
	} else if (result != 0) { /* this is for dynamic major */
		subsys_mgr.hantroenc_major = result;
	}

	result = ReserveIO();
	if (result < 0) {
		goto err;
	}
	/* set interrupt gate */
	enc_interrupt_gate_set(subsys_mgr.hantroenc_data);
	sema_init(&subsys_mgr.enc_core_sem, 1);

#ifdef HANTROMMU_SUPPORT
	/* MMU only initial once No matter how many MMU we have */
	for (i = 0; i < subsys_mgr.total_subsys_num; i++) {
		if (!(hantroenc_data[i].subsys_data.core_info.type_info & (1 << CORE_MMU)))
			continue;

		result = MMUInit(
			hantroenc_data[i].hwregs +
			hantroenc_data[i].subsys_data.core_info.offset[CORE_MMU]);
		if (result == MMU_STATUS_NOT_FOUND)
			printk(KERN_INFO "MMU does not exist!\n");
		else if (result != MMU_STATUS_OK) {
			ReleaseIO();
			goto err;
		}

		break;
	}

	memset(subsys_mgr.mmu_hwregs, 0, MAX_SUBSYS_NUM * 2);
	for (i = 0; i < subsys_mgr.total_subsys_num; i++) {
		if (hantroenc_data[i].subsys_data.core_info.type_info &
			(1 << CORE_MMU))
			subsys_mgr.mmu_hwregs[i][0] =
				hantroenc_data[i].hwregs +
				hantroenc_data[i].subsys_data.core_info.offset[CORE_MMU];

		if (hantroenc_data[i].subsys_data.core_info.type_info &
			(1 << CORE_MMU_1))
			subsys_mgr.mmu_hwregs[i][1] = hantroenc_data[i].hwregs +
					   hantroenc_data[i].subsys_data.core_info.offset[CORE_MMU_1];
	}

	mmu_status = MMUEnable(subsys_mgr.mmu_hwregs);
	subsys_mgr.mmu_enable = mmu_enable;
	printk(KERN_INFO "mmu_enable is %d\n", mmu_enable);
	if (mmu_status != MMU_STATUS_OK) {
		printk(KERN_INFO "MMUEnable: MMU enable failed\n");
		return -3;
	}
#else
	subsys_mgr.mmu_enable = 0;
#endif

	/* get the IRQ line */
	for (i = 0; i < subsys_mgr.total_subsys_num; i++) {
		if (hantroenc_data[i].is_valid == 0)
			continue;

		for (j = 0; j < CORE_MAX; j++) {
			if (hantroenc_data[i].subsys_data.core_info.irq[j] != -1) {
				enable_irq(hantroenc_data[i].subsys_data.core_info.irq[j]);
				irq_enabled = 1;
			} else {
				printk(KERN_INFO
				       "hantroenc: IRQ not in use!\n");
			}
		}
	}

	if (irq_enabled) {
		result = request_irq(CPU_INT_IRQ, hantroenc_isr, IRQF_SHARED,
				     "vsi_vcx", (void *)&hantroenc_data[0]);
		if (result == -EINVAL) {
			printk(KERN_ERR
			       "hantroenc: Bad irq number or handler\n");
			ReleaseIO();
			goto err;
		} else if (result == -EBUSY) {
			printk(KERN_ERR
			       "hantroenc: IRQ <%d> busy, change your config\n",
			       hantroenc_data[i].subsys_data.core_info.irq[j]);
			ReleaseIO();
			goto err;
		}
	}

	_vdev->ctx = (void *)&subsys_mgr;
	_vdev->_open = hantroenc_open;
	_vdev->_release = hantroenc_release;
	_vdev->_ioctl = hantroenc_ioctl;
	_vdev->_cleanup = hantroenc_normal_cleanup;

	printk(KERN_INFO "hantroenc: module inserted. Major <%d>\n",
			subsys_mgr.hantroenc_major);
	printk(KERN_INFO "hantroenc: init successfully.\n");

	return 0;

err:
	unregister_chrdev(subsys_mgr.hantroenc_major, "vsi_vcx");
err1:
	if (hantroenc_data)
		vfree(hantroenc_data);
	printk(KERN_INFO "hantroenc: module not inserted\n");
	return result;
}

static int ReserveIO(void)
{
	u32 hwid;
	int i;
	u32 found_hw = 0, hw_cfg;
	u32 VCE_core_idx;
	hantroenc_t *hantroenc_data = subsys_mgr.hantroenc_data;

	for (i = 0; i < subsys_mgr.total_subsys_num; i++) {
		if (!request_mem_region(
				hantroenc_data[i].subsys_data.cfg.base_addr,
				hantroenc_data[i].subsys_data.cfg.iosize,
				"vsi_vcx")) {
			printk(KERN_INFO "hantroenc: failed to reserve HW regs\n");
			continue;
		}

		hantroenc_data[i].hwregs = (volatile u8 *)ioremap_nocache(
			hantroenc_data[i].subsys_data.cfg.base_addr,
			hantroenc_data[i].subsys_data.cfg.iosize);

		if (!hantroenc_data[i].hwregs) {
			printk(KERN_INFO
			       "hantroenc: failed to ioremap HW regs\n");
			ReleaseIO();
			continue;
		}

		/*read hwid and check validness and store it*/
		VCE_core_idx = GET_ENCODER_IDX(
			hantroenc_data[0].subsys_data.core_info.type_info);
		hwid = (u32)ioread32(
			(void __iomem *)hantroenc_data[i].hwregs +
			hantroenc_data[i].subsys_data.core_info.offset[VCE_core_idx]);
		printk(KERN_INFO "hwid=0x%08x\n", hwid);

		/* check for encoder HW ID */
		if (((((hwid >> 16) & 0xFFFF) !=
			((ENC_HW_ID1 >> 16) & 0xFFFF))) &&
			((((hwid >> 16) & 0xFFFF) !=
			((ENC_HW_ID2 >> 16) & 0xFFFF)))) {
			printk(KERN_INFO "hantroenc: HW not found at %p\n",
				(void *)hantroenc_data[i].subsys_data.cfg.base_addr);
#ifdef hantroenc_DEBUG
			dump_regs((unsigned long)&hantroenc_data);
#endif
			hantroenc_data[i].is_valid = 0;
			ReleaseIO();
			continue;
		}
		hantroenc_data[i].hw_id = hwid;
		hantroenc_data[i].is_valid = 1;
		found_hw = 1;

		hw_cfg = (u32)ioread32(
			(void __iomem *)hantroenc_data[i].hwregs +
			hantroenc_data[i].subsys_data.core_info.offset[VCE_core_idx] + 320);
		hantroenc_data[i].subsys_data.core_info.type_info &= 0xFFFFFFFC;
		if (hw_cfg & 0x88000000)
			hantroenc_data[i].subsys_data.core_info.type_info |= (1 << CORE_VCE);
		if (hw_cfg & 0x00008000)
			hantroenc_data[i].subsys_data.core_info.type_info |= (1 << CORE_VCEJ);

		printk(KERN_INFO
		       "hantroenc: HW at base <%p> with ID <0x%08x>\n",
		       (void *)hantroenc_data[i].subsys_data.cfg.base_addr,
		       hwid);
	}

	if (found_hw == 0) {
		printk(KERN_ERR "hantroenc: NO ANY HW found!!\n");
		return -1;
	}

	return 0;
}

static void ReleaseIO(void)
{
	u32 i;
	hantroenc_t *hantroenc_data = subsys_mgr.hantroenc_data;

	for (i = 0; i < subsys_mgr.total_subsys_num; i++) {
		if (hantroenc_data[i].is_valid == 0) {
			continue;
		}
		if (hantroenc_data[i].hwregs) {
			iounmap((void __iomem *)hantroenc_data[i].hwregs);
		}
		release_mem_region(hantroenc_data[i].subsys_data.cfg.base_addr,
				   hantroenc_data[i].subsys_data.cfg.iosize);
	}
}

irqreturn_t hantroenc_isr(/*int irq, */ void *dev_id)
{
	unsigned int handled = 0;
	hantroenc_t *dev; /* = (hantroenc_t *) dev_id;*/
	u32 irq_status;
	unsigned long flags;
	u32 core_type = 0, i = 0;
	unsigned long reg_offset = 0;
	u32 hwId, majorId, wClr, minorId, productId;

	u32 intr_val, subsys_idx, array_sz;
	int irq;

	intr_val = (u32)ioread32((void *)/*SYS_REG_INT_VAL*/ SYS_REG_INT_STAT);
	PDEBUG("int 0x%x\n", intr_val);
	array_sz = sizeof(core_array) / sizeof(CORE_CONFIG);
	irq = 0;
	while (intr_val) {
		if (intr_val & 1) {
			for (i = 0; i < array_sz; i++) {
				if (core_array[i].irq != irq) {
					continue;
				}

				/* get slice_idx & core_id */
				core_type = core_array[i].core_type;
				subsys_idx = core_array[i].subsys_idx;
				reg_offset = core_array[i].offset;
				dev = &subsys_mgr.hantroenc_data[subsys_idx];

				/* interrupt processing*/
				/*If core is not reserved by any user, but irq is received, just clean it*/
				isr_spin_lock_irqsave(&subsys_mgr.owner_lock, flags);
				clear_irq(irq);
				if (!dev->is_reserved[core_type]) {
					printk(KERN_DEBUG
					       "hantroenc_isr:received IRQ but core is not reserved!\n");
					irq_status = (u32)ioread32(
						(void __iomem *)(dev->hwregs + reg_offset + 0x04));
					if (irq_status & 0x01) {
					/*  Disable HW when buffer over-flow happen
		    		*   HW behavior changed in over-flow
		    		*   in-pass, HW cleanup HWIF_ENC_E auto
		    		*   new version:  ask SW cleanup HWIF_ENC_E when buffer over-flow
		    		*/
						if (irq_status & 0x20)
							iowrite32(0, (void __iomem *)(dev->hwregs + reg_offset + 0x14));

						/* clear all IRQ bits. (hwId >= 0x80006100) means IRQ is cleared by writing 1 */
						hwId = ioread32((void __iomem *)dev->hwregs + reg_offset);
						productId = (hwId & 0xFFFF0000) >> 16;
						majorId = (hwId & 0x0000FF00) >> 8;
						minorId = (hwId & 0x000000FF);
						wClr = ((majorId >= 0x61) || (productId == 0x9000) || (productId == 0x9010) ||
						((productId == 0x8000) && (minorId >= 1))) ? irq_status : (irq_status & (~0x1FD));
						iowrite32(wClr, (void __iomem *)(dev->hwregs + reg_offset + 0x04));
					}
					isr_spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);
					return IRQ_HANDLED;
				}
				isr_spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);

				printk(KERN_DEBUG "hantroenc_isr:received IRQ!\n");
				irq_status = (u32)ioread32( (void __iomem *)(dev->hwregs + reg_offset + 0x04));

				printk(KERN_DEBUG "irq_status of subsys %u core %u is:%x\n",
									dev->subsys_id, core_type, irq_status);
				if (irq_status & 0x01) {
				/*  Disable HW when buffer over-flow happen
		  		*   HW behavior changed in over-flow
		  		*   in-pass, HW cleanup HWIF_ENC_E auto
		  		*   new version:  ask SW cleanup HWIF_ENC_E when buffer over-flow
		  		*/
					if (irq_status & 0x20)
						iowrite32(0, (void __iomem *)(dev->hwregs + reg_offset + 0x14));

					/* clear all IRQ bits. (hwId >= 0x80006100) means IRQ is cleared by writing 1 */
					hwId = ioread32((void __iomem *)dev->hwregs + reg_offset);
					productId = (hwId & 0xFFFF0000) >> 16;
					majorId = (hwId & 0x0000FF00) >> 8;
					minorId = (hwId & 0x000000FF);
					wClr = ((majorId >= 0x61) || (productId == 0x9000) || (productId == 0x9010) ||
					((productId == 0x8000) && (minorId >= 1))) ? irq_status : (irq_status & (~0x1FD));
					iowrite32(wClr, (void __iomem *)(dev->hwregs + reg_offset + 0x04));

					isr_spin_lock_irqsave(&subsys_mgr.owner_lock, flags);

					dev->irq_received[core_type] = 1;
					dev->irq_status[core_type] = irq_status & (~0x01);
					isr_spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);

#ifdef LOW_LATENCY_SLICEINFO_SUPPORT
					u32 poll_sliceinfo_irq_status = 0;

					printk(KERN_DEBUG "hantroenc_isr:received polling sliceinfo timeout IRQ!\n");
					/* swreg118 bit 0 for polling sliceinfo timeout IRQ status */
					poll_sliceinfo_irq_status =
						(u32)ioread32((void __iomem *)(dev->hwregs + reg_offset + 0x1d8));
					printk(KERN_DEBUG "poll_sliceinfo_irq_status of subsys %u core %u is:%x\n",
							dev->subsys_id, core_type, poll_sliceinfo_irq_status);
					if (poll_sliceinfo_irq_status & 0x1) {
						/* clear irq bit by writing 1 */
						wClr = poll_sliceinfo_irq_status;
						iowrite32(wClr, (void __iomem *)(dev->hwregs + reg_offset + 0x1d8));

						isr_spin_lock_irqsave(&subsys_mgr.owner_lock, flags);
						dev->irq_received[core_type] = 1;
						dev->poll_sliceinfo_timeout_irq_status[core_type] =
							(poll_sliceinfo_irq_status & 0x01) << 13;
						isr_spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);
						handled++;
					}
#endif

#ifdef SUPPORT_UFBC
					u32 ufbc_dec_irq_status = 0;

					printk(KERN_DEBUG "hantroenc_isr:received ufbc decode error IRQ!\n");
					/* swreg118 bit 0 for polling sliceinfo timeout IRQ status */
					ufbc_dec_irq_status =
						(u32)ioread32((void __iomem *)(dev->hwregs + reg_offset + 0xc20));
					printk(KERN_DEBUG "poll_sliceinfo_irq_status of subsys %u core %u is:%x\n",
							dev->subsys_id, core_type, ufbc_dec_irq_status);
					if (ufbc_dec_irq_status & 0x1) {
						/* clear irq bit by writing 1 */
						wClr = ufbc_dec_irq_status;
						iowrite32(wClr, (void __iomem *)(dev->hwregs + reg_offset + 0xc20));

						isr_spin_lock_irqsave(&subsys_mgr.owner_lock, flags);
						dev->ufbc_dec_err_irq_status[core_type] =
							(ufbc_dec_irq_status & 0x01) << 14;
						isr_spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);
						handled++;
					}
#endif
					wake_up_interruptible_all(&subsys_mgr.enc_wait_queue);
					handled++;
				}
				break; // found the irq source, can break directly
			}
		}
		/* next irq */
		intr_val >>= 1;
		irq++;
	}
	if (!handled) {
		PDEBUG("IRQ received, but not hantro's!\n");
	}
	return IRQ_HANDLED;
}

#ifdef hantroenc_DEBUG
static void ResetAsic(hantroenc_t *dev)
{
	int i, n;

	for (n = 0; n < subsys_mgr.total_subsys_num; n++) {
		if (dev[n].is_valid == 0)
			continue;
		iowrite32(0, (void __iomem *)(dev[n].hwregs + 0x14));
		for (i = 4; i < dev[n].subsys_data.cfg.iosize; i += 4) {
			iowrite32(0, (void __iomem *)(dev[n].hwregs + i));
		}
	}
}

static void dump_regs(unsigned long data)
{
	hantroenc_t *dev = (hantroenc_t *)data;
	int i;

	PDEBUG("Reg Dump Start\n");
	for (i = 0; i < dev->iosize; i += 4) {
		PDEBUG("\toffset %02X = %08X\n", i, ioread32(dev->hwregs + i));
	}
	PDEBUG("Reg Dump End\n");
}
#endif
