/*
 * SPDX_license-Identifier: GPL-2.0  OR BSD-3-Clause
 * Copyright (c) 2015, Verisilicon Inc. - All Rights Reserved
 *
 ********************************************************************************
 *
 * GPL-2.0
 *
 ********************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; version 2.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51 Franklin
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 ********************************************************************************
 *
 * Alternatively, This software may be distributed under the terms of
 * BSD-3-Clause, in which case the following provisions apply instead of the ones
 * mentioned above :
 *
 ********************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ********************************************************************************
 */

#include <linux/kernel.h>
#include <linux/module.h>
/* needed for __init,__exit directives */
#include <linux/init.h>
/* needed for remap_page_range
 * SetPageReserved
 * ClearPageReserved
 */
#include <linux/mm.h>
/* obviously, for kmalloc */
#include <linux/slab.h>
/* for struct file_operations, register_chrdev() */
#include <linux/fs.h>
/* standard error codes */
#include <linux/errno.h>

#include <linux/moduleparam.h>
/* request_irq(), free_irq() */
#include <linux/interrupt.h>
#include <linux/sched.h>

#include <linux/semaphore.h>
#include <linux/spinlock.h>
/* needed for virt_to_phys() */
#include <asm/io.h>
#include <linux/pci.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>

#include <asm/irq.h>

#include <linux/version.h>
#include <linux/vmalloc.h>
#include <linux/timer.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>

#ifdef DTB_SUPPORT
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#endif

/* our own stuff */
#include "vcx_driver.h"
#include "vcx_normal_cfg.h"

/****************************************************************
 * Macro definitions
 ***************************************************************/
//#define MULTI_THR_TEST

/**
 * Add function get_of_property() to get data from DTB,
 * it's added to match Qemu implementition
 * (node & irq string are hard-coded in Qemu model).
 */
#ifdef DTB_SUPPORT
#define VCE_DTB_NODE_NAME "verisilicon,vcx"
#define VCE_DTB_IRQ_NAME  "vcx_irq"
#endif

/****************************************************************
 * external/global variables declarations
 ***************************************************************/
#ifdef HANTROMMU_SUPPORT
extern unsigned int mmu_enable;
#endif
extern struct platform_device *platformdev;
#ifdef PCIE_EN
unsigned long gBaseDDRHw;/* PCI base register address (memalloc) */
#endif

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
	// ufbc IRQ status of each core
	u32 ufbc_irq_status[CORE_MAX];
	u32 job_id[CORE_MAX];

	volatile u8 *hwregs;

	//watchdog timer to check if there is no response from hardware.
	struct timer_list watchdog_timer;
	u8 watchdog_active; // 1: watchdog is start
	u8 watchdog_triggered; // 1: watchdog is triggered
#ifdef CONFIG_ENC_PM
	u32 reg_buf[ASIC_SWREG_AMOUNT];
	struct semaphore core_suspend_sem;
	u32 reg_corrupt[CORE_MAX];
#endif
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

	struct semaphore enc_core_sem;

	unsigned long sram_base;
	unsigned int sram_size;
	int linebuffer_sram_used;
	/* and this is our MAJOR; use 0 for dynamic allocation (recommended)*/
	int hantroenc_major;
#ifdef PCIE_EN
	struct pci_dev *gDev;    /* PCI device structure. */
#endif

	/* kernel thread related */
	struct task_struct *kthread;
	u8 stop_kthread;
	wait_queue_head_t kthread_waitq;
} subsys_mgr;



/****************************************************************
 * local functions declarations
 ***************************************************************/
static int ReserveIO(void);
static void ReleaseIO(void);
//static void ResetAsic(hantroenc_t * dev);

#ifdef hantroenc_DEBUG
static void dump_regs(unsigned long data);
#endif

/* IRQ handler */
#if (KERNEL_VERSION(2, 6, 18) > LINUX_VERSION_CODE)
static irqreturn_t hantroenc_isr(int irq, void *dev_id, struct pt_regs *regs);
#else
static irqreturn_t hantroenc_isr(int irq, void *dev_id);
#endif

static int hantroenc_mmap(struct file *filp, struct vm_area_struct *vma);

#ifdef IRQ_SIMULATION
struct timer_list timer0;
struct timer_list timer1;
#endif

#ifdef MULTI_THR_TEST

#define WAIT_NODE_NUM 32
struct wait_list_node {
	u32 node_id; //index of the node
	u32 used_flag; //1:the node is insert to the wait queue list.
	u32 sem_used; //1:the source is released and the semphone is uped.
	struct semaphore wait_sem; //the unique semphone for per reserve_encoder thread.
	u32 wait_cond; //the condition for wait. Equal to the "core_info".
	struct list_head wait_list; //list node.
};

static struct list_head reserve_header;
static struct wait_list_node res_wait_node[WAIT_NODE_NUM];

static void wait_delay(unsigned int delay)
{
	if (delay > 0) {
#if KERNEL_VERSION(2, 6, 28) <= LINUX_VERSION_CODE
		ktime_t dl = ktime_set((delay / MSEC_PER_SEC),
					   (delay % MSEC_PER_SEC) * NSEC_PER_MSEC);
		__set_current_state(TASK_UNINTERRUPTIBLE);
		schedule_hrtimeout(&dl, HRTIMER_MODE_REL);
#else
		msleep(delay);
#endif
	}
}

static u32 request_wait_node(struct wait_list_node **node, u32 start_id)
{
	u32 i;
	struct wait_list_node *temp_node;

	while (1) {
		for (i = start_id; i < WAIT_NODE_NUM; i++) {
			temp_node = &res_wait_node[i];
			if (temp_node->used_flag == 0) {
				temp_node->used_flag = 1;
				*node = temp_node;
				return i;
			}
		}
		wait_delay(10);
	}
}

static void request_wait_sema(struct wait_list_node **node)
{
	u32 i;
	struct wait_list_node *temp_node;

	while (1) {
		for (i = 0; i < WAIT_NODE_NUM; i++) {
			temp_node = &res_wait_node[i];
			if (temp_node->used_flag == 0 &&
				temp_node->sem_used == 0) {
				temp_node->sem_used = 1;
				*node = temp_node;
				return;
			}
		}
		wait_delay(10);
	}
}

static void init_wait_node(struct wait_list_node *node, u32 cond, u32 sem_flag)
{
	node->used_flag = 0;
	node->wait_cond = cond;
	sema_init(&node->wait_sem, sem_flag);
	INIT_LIST_HEAD(&node->wait_list);
	if (sem_flag > 0)
		node->sem_used = 1;
}

static void init_reserve_wait(u32 dev_num)
{
	u32 i;
	u32 cond = 0x80000001;
	u32 sem_flag = 0;
	struct wait_list_node *node;

	//    printk("%s,%d, dev_num %d\n",__FUNCTION__,__LINE__,dev_num);

	INIT_LIST_HEAD(&reserve_header);

	for (i = 0; i < WAIT_NODE_NUM; i++) {
		if (i < dev_num)
			sem_flag = 1;
		else
			sem_flag = 0;
		node = &res_wait_node[i];
		node->node_id = i;
		init_wait_node(node, cond, sem_flag);
	}
}

void release_reserve_wait(u32 dev_num)
{
}

#endif

/********************************************************************
 * watchdog and kthread functions
 ********************************************************************/
#ifdef SUPPORT_WATCHDOG
/**
 * @brief hook function for system-driver to do further process
 *  for tiggered watchdog.
 */
static void hook_watchdog(void *_dev, int succeed)
{
	if (succeed)
		pr_err("hantroenc: axife flush succeed!!");
	else
		pr_err("hantroenc: axife flush failed, need to re-power sub-system!\n");
}

/**
 * @brief stop vce when watchdog triggered
 */
int watchdog_stop_vce(volatile u8 *reg_base)
{
	u32 loop_count = 20;
	u32 irq;

	if ((u32)ioread32((void __iomem *)(reg_base + 0x14)) & 0x1) {
		//Stop VCE by setting reg5 bit0 to 0.
		iowrite32(0, (void __iomem *)(reg_base + 0x14));
		// wait vce core disable.
		do {
			if (((u32)ioread32((void __iomem *)(reg_base + 0x14)) & 0x1) == 0) {
				/* clear reg1 by writing 1 */
				irq = (u32)ioread32((void __iomem *)(reg_base + 0x4));
				iowrite32(irq, (void __iomem *)(reg_base + 0x4));
				return 0;
			}
			mdelay(10); // wait 10ms
		} while (loop_count--);
		pr_err("hantroenc: too long before vce core disable\n");
		return -1;
	}
	return 0;
}

/**
 * @brief process when watchdog triggered.
 */
static void watchdog_process(hantroenc_t *dev)
{
	int succeed = 1;
	u32 core_id = dev->subsys_id;
	unsigned long flags;
#ifdef HANTROAXIFE_SUPPORT
	volatile u8 *axife_hwregs;
#endif

	if (core_id >= subsys_mgr.total_subsys_num)
		return;
	spin_lock_irqsave(&subsys_mgr.owner_lock, flags);
	watchdog_stop_vce(dev->hwregs);
#ifdef HANTROAXIFE_SUPPORT
	axife_hwregs = dev->hwregs +
					dev->subsys_data.core_info.offset[CORE_AXIFE];
	if (succeed)
		succeed = (AXIFEFlush(axife_hwregs) != -1);
	axife_hwregs = dev->hwregs +
					dev->subsys_data.core_info.offset[CORE_AXIFE_1];
	if (succeed)
		succeed = (AXIFEFlush(axife_hwregs) != -1);
#else
	succeed = 0;
#endif
	hook_watchdog(dev, succeed);
	spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);
}
#endif //SUPPORT_WATCHDOG

/**
 * @brief To check subsys/dev's actions which need kthread to process
 * @return int: 0: no actions; 1: have actions
 */
static int _kthread_actions(hantroenc_t **dev)
{
	int ret = 0, i;
	hantroenc_t *hantroenc_data = subsys_mgr.hantroenc_data;

	if (subsys_mgr.stop_kthread == 1)
		return 1;

	for (i = 0; i < subsys_mgr.total_subsys_num; i++) {
		*dev = &hantroenc_data[i];
		if ((*dev)->watchdog_triggered == 1) {
			ret = 1;
			break;
		} else {
			ret = 0;
			/*TODO*/
		}
	}

	return ret;
}

/**
 * @brief hantroenc kernel thread main function
 */
static int _kthread_fn(void *data)
{
	hantroenc_t *dev = NULL;

	while (!kthread_should_stop()) {
		if (wait_event_interruptible(subsys_mgr.kthread_waitq,
				_kthread_actions(&dev))) {
			pr_err("hantroenc: %s: signaled!!!\n", __func__);
			return -ERESTARTSYS;
		}

		if (dev == NULL)
			continue;

#ifdef SUPPORT_WATCHDOG
		if (dev->watchdog_triggered == 1) {
			dev->watchdog_triggered = 0;
			watchdog_process(dev);
			continue;
		}
#endif
	}

	return 0;
}

/**
 * @brief wake up hantroenc kernel thread
 */
static void hantroenc_kthread_wakeup(void)
{

	if (IS_ERR(subsys_mgr.kthread))
		return;

	wake_up_interruptible_all(&subsys_mgr.kthread_waitq);
}

/**
 * @brief create kernel thread for hantroenc driver
 */
static void hantroenc_kthread_create(void)
{
	subsys_mgr.stop_kthread = 0;
	init_waitqueue_head(&subsys_mgr.kthread_waitq);
	subsys_mgr.kthread = kthread_run(_kthread_fn, NULL, "hantroenc_kthread");
	if (IS_ERR(subsys_mgr.kthread)) {
		pr_err("hantroenc: create hantroenc kthread failed\n");
		return;
	}
}

/**
 * @brief stop kernel thread
 */
static void hantroenc_kthread_stop(void)
{
	if (!IS_ERR(subsys_mgr.kthread)) {
		subsys_mgr.stop_kthread = 1;
		kthread_stop(subsys_mgr.kthread);
		subsys_mgr.kthread = NULL;
	}
}

#ifdef SUPPORT_WATCHDOG
/**
 * @brief timer callback function of watchdog
 */
#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
static void _watchdog_cb(unsigned long arg)
#else
static void _watchdog_cb(struct timer_list *timer)
#endif
{
#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
	struct timer_list *timer = (struct timer_list *)arg;
#endif
	hantroenc_t *dev;

	dev = container_of(timer, hantroenc_t, watchdog_timer);

	dev->watchdog_triggered = 1;
	hantroenc_kthread_wakeup();
}

/**
 * @brief start watchdog
 */
static void _watchdog_start(hantroenc_t *dev)
{
	struct timer_list *timer = &dev->watchdog_timer;

#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
	init_timer(timer);
	timer->function = _watchdog_cb;
	timer->data = (unsigned long)timer;
#else
	timer_setup(timer, _watchdog_cb, 0);
#endif
	dev->watchdog_active = 1;
}

/**
 * stop watchdog
 */
static void _watchdog_stop(hantroenc_t *dev)
{
	if (dev->watchdog_active) {
		del_timer(&dev->watchdog_timer);
		dev->watchdog_active = 0;
	}
}

/**
 * feed and start watchdog
 */
static void _watchdog_feed(hantroenc_t *dev)
{
	if (dev->watchdog_active == 0)
		_watchdog_start(dev);
	mod_timer(&dev->watchdog_timer,
		(jiffies + HZ * ONE_JOB_WAIT_TIME / 1000));
}
#endif // SUPPORT_WATCHDOG

#ifdef CONFIG_ENC_PM
extern void hantroenc_pm_runtime_get(struct device *dev);
extern void hantroenc_pm_runtime_put(struct device *dev);

static long EncRestoreRegs(hantroenc_t *dev)
{
	long i;
	/* write all regs to hardware */
	u32 *reg_buf = dev->reg_buf;

	for (i = 0; i < ASIC_SWREG_AMOUNT * 4; i += 4)
		iowrite32(reg_buf[i/4], (void __iomem *)(dev->hwregs + i));

	return 0;
}

static long EncStoreRegs(hantroenc_t *dev)
{
	long i;
	/* read all registers from hardware */
	u32 *reg_buf = dev->reg_buf;

	for (i = 0; i < ASIC_SWREG_AMOUNT * 4; i += 4)
		reg_buf[i/4] = ioread32((void __iomem *)(dev->hwregs + i));

	return 0;
}

int enc_pm_suspend(void *handler)
{
	int i;
	hantroenc_t *dev = (hantroenc_t *)handler;

	pr_info("%s start..\n", __func__);

	for (i = 0; i < subsys_mgr.total_subsys_num; i++) {
		/*if HW is active, need to wait until frame ready interrupt*/
		if ((dev[i].is_reserved[0] == 0) || (down_interruptible(&dev[i].core_suspend_sem)))
			continue;

		dev[i].reg_corrupt[0] = 1;
		if (dev[i].irq_status[0] & 0x04)
			EncStoreRegs(&dev[i]);

		up(&dev[i].core_suspend_sem);
	}

	pr_info("%s succeed!\n", __func__);
	return 0;
}

int enc_pm_resume(void *handler)
{
	int i;
	u32 *reg_buf;
	hantroenc_t *dev = (hantroenc_t *)handler;

	pr_info("%s start..\n", __func__);

	for (i = 0; i < subsys_mgr.total_subsys_num; i++) {
		if (dev[i].is_reserved[0] == 0)
			continue;

		reg_buf = dev[i].reg_buf;

		if (dev[i].irq_status[0] & 0x04) {
			EncRestoreRegs(&dev[i]);
			dev[i].reg_corrupt[0] = 0;
		}
	}

	pr_info("%s succeed!\n", __func__);
	return 0;
}
#endif

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
	if ((dev[subsys_idx].ufbc_irq_status[core_type] &
				ASIC_STATUS_UFBC_DEC_ERR) ||
				(dev[subsys_idx].ufbc_irq_status[core_type] &
				ASIC_STATUS_UFBC_CFG_ERR) && job_id) {
		rdy = 1;
		*core_info = subsys_idx;
		*irq_status |= dev[subsys_idx].ufbc_irq_status[core_type];
		*job_id = dev[subsys_idx].job_id[core_type];
	}
#endif

	spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);

	return rdy;
}

static int WaitEncReady(hantroenc_t *dev, u32 *core_info,
				 u32 *irq_status)
{
	PDEBUG("%s\n", __func__);
#ifndef USE_SW_TIMEOUT
	if (wait_event_interruptible(subsys_mgr.enc_wait_queue,
					 CheckEncIrq(dev, core_info, irq_status, NULL))) {
		PDEBUG("ENC wait_event_interruptible interrupted\n");
		return -ERESTARTSYS;
	}
#else
	if (wait_event_interruptible_timeout(subsys_mgr.enc_wait_queue,
						CheckEncIrq(dev, core_info, irq_status, NULL), msecs_to_jiffies(2000)) == 0) {
		PDEBUG("%s: wait interrupt timeout !\n", __func__);
		return -1;
	}
#endif
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
#ifdef CONFIG_ENC_PM
		//if frame_rdy IRQ is received, then HW will not be used any more.
		if (*irq_status & ASIC_STATUS_FRAME_READY)
			up(&dev->core_suspend_sem);
#endif
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
#ifdef CONFIG_ENC_PM
		//if frame_rdy IRQ is received, then HW will not be used any more.
		if (*irq_status & ASIC_STATUS_FRAME_READY)
			up(&dev->core_suspend_sem);
#endif
			goto end_1;
		}

#ifdef LOW_LATENCY_SLICEINFO_SUPPORT
		u32 poll_sliceinfo_irq_status = 0;
		u32 hw_reset_irq = 0;

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
		u32 ufbc_irq_status = 0;
		u32 ufbc_irq_status_tmp = 0;

		ufbc_irq_status =
			 (u32)ioread32((void __iomem *)(dev[subsys_idx].hwregs + reg_offset + 0xc20));
		ufbc_irq_status_tmp = ufbc_irq_status << 14;
		if ((ufbc_irq_status_tmp & ASIC_STATUS_UFBC_DEC_ERR) || (ufbc_irq_status_tmp & ASIC_STATUS_UFBC_CFG_ERR)) {
			wClr = ufbc_irq_status;
			iowrite32(wClr, (void __iomem *)(dev[subsys_idx].hwregs + reg_offset + 0xc20));
			dev[subsys_idx].ufbc_irq_status[core_type] =
				ufbc_irq_status_tmp;
			rdy = 1;
			*core_info = subsys_idx;
			*irq_status = irq;
			*irq_status |=
			     dev[subsys_idx].ufbc_irq_status[core_type];
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
#ifdef SUPPORT_WATCHDOG
			/* if there is an interrupt(exclusive slice ready) from VCE,
			 * should stop the watchdog.
			 */
			if (irq_status & ASIC_STATUS_ALL & ~ASIC_STATUS_SLICE_READY)
				_watchdog_stop(&dev[i]);
#endif
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
#ifdef SUPPORT_WATCHDOG
			/* if there is an interrupt(exclusive slice ready) from VCE,
			 * should stop the watchdog.
			 */
			if (irq_status & ASIC_STATUS_ALL & ~ASIC_STATUS_SLICE_READY)
				_watchdog_stop(&dev[i]);
#endif
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
#ifndef MULTI_THR_TEST
		dev->pid[core_type] = current->pid;
#endif
		ret = 1;
		PDEBUG("%s pid=%d\n", __func__, dev->pid[core_type]);
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

	PDEBUG("%s: core_info=%x\n", __func__, *core_info);

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
#ifdef SUPPORT_WATCHDOG
				/* start watchdog due to usr has start hw
				 * but not notisfy kernel driver
				 */
				_watchdog_feed(&dev[i]);
#endif
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
#ifdef MULTI_THR_TEST
	struct wait_list_node *wait_node;
	u32 start_id = 0;
#endif

	/*If HW resources are shared inter cores, just make sure only one is
	 *using the HW
	 */
	if (dev[0].subsys_data.cfg.resource_shared) {
		if (down_interruptible(&subsys_mgr.enc_core_sem))
			return -ERESTARTSYS;
	}

#ifdef MULTI_THR_TEST
	while (1) {
		start_id = request_wait_node(&wait_node, start_id);
		if (wait_node->sem_used == 1) {
			if (GetWorkableCore(dev, core_info, &core_info_tmp)) {
				down_interruptible(&wait_node->wait_sem);
				wait_node->sem_used = 0;
				wait_node->used_flag = 0;
				break;
			} else {
				start_id++;
			}
		} else {
			wait_node->wait_cond = *core_info;
			list_add_tail(&wait_node->wait_list, &reserve_header);
			down_interruptible(&wait_node->wait_sem);
			*core_info = wait_node->wait_cond;
			list_del(&wait_node->wait_list);
			wait_node->sem_used = 0;
			wait_node->used_flag = 0;
			break;
		}
	}
#else

	/* lock a core that has specified core id*/
	if (wait_event_interruptible(subsys_mgr.hw_queue,
					 GetWorkableCore(dev, core_info, &core_info_tmp) != 0))
		return -ERESTARTSYS;
#endif

	return 0;
}

static void ReleaseEncoder(hantroenc_t *dev, u32 *core_info)
{
	unsigned long flags;
	u8 core_type = 0, subsys_idx = 0, unCheckPid = 0;

	unCheckPid = (u8)((*core_info) >> 31);
#ifdef MULTI_THR_TEST
	u32 release_ok = 0;
	struct list_head *node;
	struct wait_list_node *wait_node;
	u32 core_info_tmp = 0;
#endif
	subsys_idx = (u8)((*core_info & 0xF0) >> 4);
	core_type = (u8)(*core_info & 0x0F);

	PDEBUG("%s:subsys_idx=%d,core_type=%x\n", __func__, subsys_idx,
		   core_type);
	/* release specified subsys and core type */

	if (dev[subsys_idx].subsys_data.core_info.type_info &
		(1 << core_type)) {
		core_type =
			(core_type == CORE_VCEJ ? CORE_VCE : core_type);
		spin_lock_irqsave(&subsys_mgr.owner_lock, flags);
		PDEBUG("subsys[%d].pid[%d]=%d,current->pid=%d\n", subsys_idx,
			   core_type, dev[subsys_idx].pid[core_type], current->pid);
#ifdef MULTI_THR_TEST
		if (dev[subsys_idx].is_reserved[core_type])
#else
		if (dev[subsys_idx].is_reserved[core_type] &&
			(dev[subsys_idx].pid[core_type] == current->pid ||
			 unCheckPid == 1))
#endif
		{
			dev[subsys_idx].pid[core_type] = -1;
			dev[subsys_idx].is_reserved[core_type] = 0;
			dev[subsys_idx].irq_received[core_type] = 0;
			dev[subsys_idx].irq_status[core_type] = 0;
			dev[subsys_idx].job_id[core_type] = 0;
#ifdef CONFIG_ENC_PM
			dev[subsys_idx].reg_corrupt[core_type] = 0;
#endif
#ifdef LOW_LATENCY_SLICEINFO_SUPPORT
			dev[subsys_idx].poll_sliceinfo_timeout_irq_status[core_type] = 0;
#endif
#ifdef SUPPORT_UFBC
			dev[subsys_idx].ufbc_irq_status[core_type] = 0;
#endif
			spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);
#ifdef MULTI_THR_TEST
			release_ok = 0;
			if (list_empty(&reserve_header)) {
				request_wait_sema(&wait_node);
				up(&wait_node->wait_sem);
			} else {
				list_for_each(node, &reserve_header) {
					wait_node = container_of(node, struct wait_list_node, wait_list);
					if ((GetWorkableCore(dev, &wait_node->wait_cond, &core_info_tmp)) &&
						wait_node->sem_used == 0) {
						release_ok = 1;
						wait_node->sem_used = 1;
						up(&wait_node->wait_sem);
						break;
					}
				}
				if (release_ok == 0) {
					request_wait_sema(&wait_node);
					up(&wait_node->wait_sem);
				}
			}
#endif

		} else {
			if (dev[subsys_idx].pid[core_type] != current->pid &&
				unCheckPid == 0)
				pr_info("WARNING:pid(%d) is trying to release core reserved by pid(%d)\n",
					current->pid, dev[subsys_idx].pid[core_type]);
			spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);
		}
	}
#ifndef MULTI_THR_TEST
	wake_up_interruptible_all(&subsys_mgr.hw_queue);
#endif
	if (dev->subsys_data.cfg.resource_shared)
		up(&subsys_mgr.enc_core_sem);
}

#ifdef IRQ_SIMULATION
static void get_random_bytes(void *buf, int nbytes);

static void hantroenc_trigger_irq_0(unsigned long value)
{
	hantroenc_t *hantroenc_data = subsys_mgr.hantroenc_data;

	PDEBUG("trigger core 0 irq\n");
	del_timer(&timer0);
	hantroenc_isr(0, (void *)&hantroenc_data[0]);
}

static void hantroenc_trigger_irq_1(unsigned long value)
{
	hantroenc_t *hantroenc_data = subsys_mgr.hantroenc_data;

	PDEBUG("trigger core 1 irq\n");
	del_timer(&timer1);
	hantroenc_isr(0, (void *)&hantroenc_data[1]);
}

#endif

static long hantroenc_ioctl(struct file *filp, unsigned int cmd,
				unsigned long arg)
{
	int err = 0;
	unsigned int tmp;
	hantroenc_t *hantroenc_data = subsys_mgr.hantroenc_data;

	PDEBUG("ioctl cmd %08ux\n", cmd);
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
#if KERNEL_VERSION(5, 0, 0) <= LINUX_VERSION_CODE
		err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
#else
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
#endif
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
#if KERNEL_VERSION(5, 0, 0) <= LINUX_VERSION_CODE
		err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
#else
		err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
#endif
	if (err)
		return -EFAULT;

	switch (cmd) {
	case HANTRO_IOCH_GET_VCMD_ENABLE: {
		__put_user(0, (unsigned long __user  *)arg);
		break;
	}

	case HANTRO_IOCH_GET_MMU_ENABLE: {
		__put_user(subsys_mgr.mmu_enable, (unsigned int __user *)arg);
		break;
	}

	case HANTRO_IOCH_GET_PM_SUPPORT: {
		u32 pm_support = 0;

#ifdef CONFIG_ENC_PM
		pm_support = 1;
#endif
		__put_user(pm_support, (unsigned int __user *)arg);
		break;
	}

	case HANTRO_IOCG_HWOFFSET: {
		u32 id;

		__get_user(id, (u32 __user *)arg);

		if (id >= subsys_mgr.total_subsys_num)
			return -EFAULT;
		__put_user(hantroenc_data[id].subsys_data.cfg.base_addr,
			   (unsigned long __user *)arg);
		break;
	}

	case HANTRO_IOCG_HWIOSIZE: {
		u32 id;
		u32 io_size;

		__get_user(id, (u32 __user *)arg);

		if (id >= subsys_mgr.total_subsys_num)
			return -EFAULT;
		io_size = hantroenc_data[id].subsys_data.cfg.iosize;
		__put_user(io_size, (u32 __user *)arg);

		return 0;
	}
	case HANTRO_IOCG_SRAMOFFSET:
		__put_user(subsys_mgr.sram_base, (unsigned long __user *)arg);
		subsys_mgr.linebuffer_sram_used = 1;
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
			PDEBUG("copy_from_user failed, returned %u\n", tmp);
			return -EFAULT;
		}
		idx = in_data.type_info;
		if (idx > subsys_mgr.total_subsys_num - 1)
			return -1;

		tmp = copy_to_user((void __user *)arg,
				 &hantroenc_data[idx].subsys_data.core_info,
				 sizeof(SUBSYS_CORE_INFO));
		if (tmp) {
			PDEBUG("copy_to_user failed, returned %u\n", tmp);
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
	case HANTRO_IOCG_ENABLE_CORE:
	{
#ifdef CONFIG_ENC_PM
		u8 core_id, core_type;
		u32 reg_value, core_info;

		__get_user(core_info, (u32 *)arg);
		PDEBUG("Enable ENC Core\n");

		core_type = (u8)(core_info & 0x0F);
		core_id = (u8)(core_info >> 4);
		if (hantroenc_data[core_id].is_reserved[core_type] == 0)
			return -EPERM;

		if (hantroenc_data[core_id].reg_corrupt[core_type])   {
			/*need to re-config HW if exception happen between reserve and enable*/
			hantroenc_data[core_id].reg_corrupt[core_type] = 0;
			return -EAGAIN;
		}

		if (down_interruptible(&hantroenc_data[core_id].core_suspend_sem))
			return -ERESTARTSYS;

		reg_value = (u32)ioread32((void *)(hantroenc_data[core_id].hwregs +
			hantroenc_data[core_id].subsys_data.core_info.offset[CORE_VCE]
			+ 0x14));
		reg_value |= 0x01;
		iowrite32(reg_value, (void *)(hantroenc_data[core_id].hwregs +
			hantroenc_data[core_id].subsys_data.core_info.offset[CORE_VCE] +
			0x14));
#endif
		break;
	}

	case HANTRO_IOCG_CORE_WAIT: {
		u32 core_info;
		u32 irq_status;

		__get_user(core_info, (u32 __user *)arg);
#ifdef IRQ_SIMULATION
		u32 random_num;

		get_random_bytes(&random_num, sizeof(u32));
		random_num = random_num % 10 + 80;
		PDEBUG("random_num=%d\n", random_num);

		/*init a timer to trigger irq*/
		if (core_info == 1) {
			init_timer(&timer0);
			timer0.function = &hantroenc_trigger_irq_0;
			//the expires time is 1s
			timer0.expires = jiffies + random_num * HZ / 10;
			add_timer(&timer0);
		}

		if (core_info == 2) {
			init_timer(&timer1);
			timer1.function = &hantroenc_trigger_irq_1;
			//the expires time is 1s
			timer1.expires = jiffies + random_num * HZ / 10;
			add_timer(&timer1);
		}
#endif
#ifdef CONFIG_ENC_PM
		u8 core_type = (u8)(core_info & 0x0F);
		u8 core_id = (u8)(core_info >> 4);

		if (hantroenc_data[core_id].is_reserved[core_type] == 0)
			return -EPERM;
#endif
		err = WaitEncReady(hantroenc_data, &core_info, &irq_status);
		if (err == 0) {
			__put_user(irq_status, (unsigned int __user *)arg);
			return core_info; //return core_id
		} else {
			return -1;
		}

		break;
	}
	case HANTRO_IOCG_ANYCORE_WAIT_POLLING: {
		CORE_WAIT_OUT out;

		memset(&out, 0, sizeof(CORE_WAIT_OUT));
#ifdef IRQ_SIMULATION
		u32 random_num;

		get_random_bytes(&random_num, sizeof(u32));
		random_num = random_num % 10 + 80;
		PDEBUG("random_num=%d\n", random_num);

		/*init a timer to trigger irq*/
		if (core_info == 1) {
			init_timer(&timer0);
			timer0.function = &hantroenc_trigger_irq_0;
			//the expires time is 1s
			timer0.expires = jiffies + random_num * HZ / 10;
			add_timer(&timer0);
		}

		if (core_info == 2) {
			init_timer(&timer1);
			timer1.function = &hantroenc_trigger_irq_1;
			//the expires time is 1s
			timer1.expires = jiffies + random_num * HZ / 10;
			add_timer(&timer1);
		}
#endif

		tmp = WaitEncAnyReadyByPolling(hantroenc_data, &out);
		if (tmp == 0) {
			tmp = copy_to_user((void __user *)arg, &out, sizeof(CORE_WAIT_OUT));
			if (tmp) {
				PDEBUG("copy_to_user failed, returned %u\n", tmp);
				return -EFAULT;
			}
			return 0;
		} else
			return -1;

		break;
	}
	case HANTRO_IOCG_ANYCORE_WAIT: {
		CORE_WAIT_OUT out;

		memset(&out, 0, sizeof(CORE_WAIT_OUT));

		tmp = WaitEncAnyReady(hantroenc_data, &out);
		if (tmp == 0) {
			tmp = copy_to_user((void __user *)arg, &out, sizeof(CORE_WAIT_OUT));
			if (tmp) {
				PDEBUG("copy_to_user failed, returned %u\n", tmp);
				return -EFAULT;
			}
			return 0;
		} else
			return -1;

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
#endif
	}
	}
	return 0;
}

static int hantroenc_open(struct inode *inode, struct file *filp)
{
	int result = 0;
	hantroenc_t *dev = subsys_mgr.hantroenc_data;

	filp->private_data = (void *)dev;

#if defined(HANTROMMU_SUPPORT) && defined(SUPPORT_48PA_MMU)
	if (mmu_enable) {
		int ret = MMUCreatePageTable(filp);

		if (ret < 0) {
			pr_err("MMU Create page table failed!\n");
			return -EINVAL;
		}
	}
#endif

#ifdef CONFIG_ENC_PM
	hantroenc_pm_runtime_get(&platformdev->dev);
#endif
	PDEBUG("dev opened\n");
	return result;
}

static int hantroenc_release(struct inode *inode, struct file *filp)
{
	hantroenc_t *dev = (hantroenc_t *)filp->private_data;
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
			if (dev[i].is_reserved[core_id] == 1 &&
				dev[i].pid[core_id] == current->pid) {
				dev[i].pid[core_id] = -1;
				dev[i].is_reserved[core_id] = 0;
				dev[i].irq_received[core_id] = 0;
				dev[i].irq_status[core_id] = 0;
#ifdef LOW_LATENCY_SLICEINFO_SUPPORT
				dev[i].poll_sliceinfo_timeout_irq_status[core_id] = 0;
#endif
#ifdef SUPPORT_UFBC
				dev[i].ufbc_irq_status[core_id] = 0;
#endif
				PDEBUG("release reserved core\n");
			}
			spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);
		}

#ifdef HANTROMMU_SUPPORT
		if ((dev[i].hwregs + dev[i].subsys_data.core_info.offset[CORE_MMU]) &&
			(dev[i].subsys_data.core_info.type_info & (1 << CORE_MMU)))
			has_mmu = 1;
#endif
	}

#ifdef HANTROMMU_SUPPORT
		if (has_mmu)
			MMURelease(filp);
#endif

	wake_up_interruptible_all(&subsys_mgr.hw_queue);

	if (dev->subsys_data.cfg.resource_shared)
		up(&subsys_mgr.enc_core_sem);
#ifdef CONFIG_ENC_PM
	hantroenc_pm_runtime_put(&platformdev->dev);
#endif

	return 0;
}

/* VFS methods */
static const struct file_operations hantroenc_fops = {
	.owner = THIS_MODULE,
	.open = hantroenc_open,
	.mmap = hantroenc_mmap,
	.release = hantroenc_release,
	.unlocked_ioctl = hantroenc_ioctl,
	.fasync = NULL,
};

static const struct vm_operations_struct hantroenc_vm_ops = {
#ifdef CONFIG_HAVE_IOREMAP_PROT
	.access = generic_access_phys
#endif
};

#ifdef PCIE_EN
/*------------------------------------------------------------------------------
 *Function name   : PcieInit
 *Description     : Initialize PCI Hw access
 *
 *Return type     : int
 *------------------------------------------------------------------------------
 */
static int PcieInit(void)
{
	int i = 0;
	struct pci_dev *gDev = NULL;    /* PCI device structure. */
	unsigned long gBaseHdwr; /* PCI base register address (Hardware address) */
	u32 gBaseLen;            /* Base register address Length */

	gDev = pci_get_device(PCI_VENDOR_ID_HANTRO, PCI_DEVICE_ID_HANTRO, gDev);
	if (!gDev) {
		pr_err("Init: Hardware not found.\n");
		goto out;
	}
	subsys_mgr.gDev = gDev;

	if (pci_enable_device(gDev) < 0) {
		pr_err("Init: Device not enabled.\n");
		goto out;
	}

	gBaseHdwr = pci_resource_start(gDev, PCI_H2_BAR);
	if (gBaseHdwr < 0) {
		pr_info("Init: Base Address not set.\n");
		goto out_pci_disable_device;
	}
	pr_info("Base hw val 0x%llx\n", (unsigned long long)gBaseHdwr);

	gBaseLen = pci_resource_len(gDev, PCI_H2_BAR);
	pr_info("Base hw len 0x%08x\n", (unsigned int)gBaseLen);

	for (i = 0; i < subsys_mgr.total_subsys_num; i++) {
		//the offset is based on which bus interface is chosen
		subsys_array[i].base_addr =
			gBaseHdwr + subsys_array[i].base_addr;
	}

	subsys_mgr.sram_base = gBaseHdwr + 0x000000; //axi0 interface
	subsys_mgr.sram_size = 0x80000;

	gBaseDDRHw = pci_resource_start(gDev, PCI_DDR_BAR);
	if (gBaseDDRHw == 0) {
		pr_info("%s: Base Address not set.\n", __func__);
		goto out_pci_disable_device;
	}
	pr_info("Base memory val 0x%llx\n", (unsigned long long)gBaseDDRHw);

	gBaseLen = pci_resource_len(gDev, PCI_DDR_BAR);
	pr_info("Base memory len 0x%08x\n", (unsigned int)gBaseLen);

	return 0;

out_pci_disable_device:
	pci_disable_device(gDev);
out:
	return -1;
}
#endif

#ifdef DTB_SUPPORT
/* get base address and irq number from DTB */
static int get_of_property(void)
{
	struct resource res;
	int irq_num;
	struct device_node *np;
	int ret;

	np = of_find_compatible_node(NULL, NULL, VCE_DTB_NODE_NAME);

	ret = of_address_to_resource(np, 0, &res);
	if (ret) {
		pr_err("hantroenc: can't get base address from DTB. ret=%d,\n", ret);
		return -EINVAL;
	}
	pr_info("hantroenc: get base address 0x%llx\n", res.start);
	subsys_array[0].base_addr = res.start;

	irq_num = of_irq_get_byname(np, VCE_DTB_IRQ_NAME);
	if (irq_num > 0) {
		pr_info("hantroenc: get irq number %d\n", irq_num);
		core_array[0].irq = irq_num;
	}

	return 0;
}
#endif

int hantroenc_normal_init(void **_hantroenc_data)
{
	int result = 0;
	int i, j, array_sz;
	hantroenc_t *hantroenc_data;

#ifdef DTB_SUPPORT
	result = get_of_property();
	if (result)
		goto err1;
#endif

#ifdef HANTROMMU_SUPPORT
	enum MMUStatus mmu_status = MMU_STATUS_FALSE;
#endif

	subsys_mgr.total_subsys_num = sizeof(subsys_array) / sizeof(SUBSYS_CONFIG);

#ifdef PCIE_EN
	result = PcieInit();
	if (result)
		goto err1;
#endif

	for (i = 0; i < subsys_mgr.total_subsys_num; i++) {
		pr_info("hantroenc: module init - subsys[%d] addr =%p\n", i,
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
#ifdef CONFIG_ENC_PM
		sema_init(&hantroenc_data[i].core_suspend_sem, 1);
#endif
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

	result = register_chrdev(subsys_mgr.hantroenc_major, enc_dev_n, &hantroenc_fops);
	if (result < 0) {
		pr_info("hantroenc: unable to get major <%d>\n",
			subsys_mgr.hantroenc_major);
		goto err1;
	} else if (result != 0) {
		/* this is for dynamic major */
		subsys_mgr.hantroenc_major = result;
	}

	result = ReserveIO();
	if (result < 0)
		goto err;

	//ResetAsic(hantroenc_data);  /* reset hardware */

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
			pr_info("MMU does not exist!\n");
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
	PDEBUG("mmu_enable is %d\n", mmu_enable);
	if (mmu_status != MMU_STATUS_OK) {
		pr_info("MMUEnable: MMU enable failed\n");
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
			if (hantroenc_data[i].subsys_data.core_info.irq[j] !=
				-1) {
				result = request_irq(
					hantroenc_data[i].subsys_data.core_info.irq[j],
					hantroenc_isr,
#if (KERNEL_VERSION(2, 6, 18) > LINUX_VERSION_CODE)
					SA_INTERRUPT | SA_SHIRQ,
#else
					IRQF_SHARED,
#endif
					enc_dev_n, (void *)&hantroenc_data[i]);
				if (result == -EINVAL) {
					pr_err("hantroenc: Bad irq number or handler\n");
					ReleaseIO();
					goto err;
				} else if (result == -EBUSY) {
					pr_err("hantroenc: IRQ <%d> busy, change your config\n",
						   hantroenc_data[i].subsys_data.core_info.irq[j]);
					ReleaseIO();
					goto err;
				}
			} else {
				pr_info("hantroenc: IRQ not in use!\n");
			}
		}
	}
#ifdef MULTI_THR_TEST
	init_reserve_wait(subsys_mgr.total_subsys_num);
#endif

	*_hantroenc_data = (void *)hantroenc_data;
	/* create kthread for normal driver, but it in sleep status, should wake up it when needed */
	hantroenc_kthread_create();
	pr_info("hantroenc: module inserted. Major <%d>\n", subsys_mgr.hantroenc_major);

	return 0;

err:
	unregister_chrdev(subsys_mgr.hantroenc_major, enc_dev_n);
err1:
#ifdef PCIE_EN
	if (subsys_mgr.gDev)
		pci_disable_device(subsys_mgr.gDev);
#endif
	if (hantroenc_data)
		vfree(hantroenc_data);
	pr_info("hantroenc: module not inserted\n");
	return result;
}

void hantroenc_normal_cleanup(void)
{
	int i = 0, j = 0;
	hantroenc_t *hantroenc_data = subsys_mgr.hantroenc_data;

	hantroenc_kthread_stop();

	for (i = 0; i < subsys_mgr.total_subsys_num; i++) {
		if (hantroenc_data[i].is_valid == 0)
			continue;
		//writel(0, hantroenc_data[i].hwregs + 0x14); /* disable HW */
		//writel(0, hantroenc_data[i].hwregs + 0x04); /* clear enc IRQ */

		/* free the core IRQ */
		for (j = 0; j < CORE_MAX; j++) {
			if (hantroenc_data[i].subsys_data.core_info.irq[j] != -1) {
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

#ifdef PCIE_EN
	if (subsys_mgr.gDev)
		pci_disable_device(subsys_mgr.gDev);
#endif

	unregister_chrdev(subsys_mgr.hantroenc_major, enc_dev_n);

	pr_info("hantroenc: module removed\n");
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
				enc_dev_n)) {
			pr_info("hantroenc: failed to reserve HW regs\n");
			continue;
		}
#if (KERNEL_VERSION(4, 17, 0) > LINUX_VERSION_CODE)
		hantroenc_data[i].hwregs = (volatile u8 __force *)ioremap_nocache(
			hantroenc_data[i].subsys_data.cfg.base_addr,
			hantroenc_data[i].subsys_data.cfg.iosize);
#else
		hantroenc_data[i].hwregs = (volatile u8 *)ioremap(
			hantroenc_data[i].subsys_data.cfg.base_addr,
			hantroenc_data[i].subsys_data.cfg.iosize);
#endif
		if (!hantroenc_data[i].hwregs) {
			pr_info("hantroenc: failed to ioremap HW regs\n");
			ReleaseIO();
			continue;
		}

		/*read hwid and check validness and store it*/
		VCE_core_idx = GET_ENCODER_IDX(
			hantroenc_data[0].subsys_data.core_info.type_info);
		if (!(hantroenc_data[i].subsys_data.core_info.type_info &
			  (1 << CORE_VCE)))
			VCE_core_idx = CORE_CUTREE;
		hwid = (u32)ioread32(
			(void __iomem *)hantroenc_data[i].hwregs +
			hantroenc_data[i].subsys_data.core_info.offset[VCE_core_idx]);
		pr_info("hwid=0x%08x\n", hwid);

		/* check for encoder HW ID */
		if (((((hwid >> 16) & 0xFFFF) !=
			  ((ENC_HW_ID1 >> 16) & 0xFFFF))) &&
			  ((((hwid >> 16) & 0xFFFF) !=
			  ((ENC_HW_ID2 >> 16) & 0xFFFF))) &&
			  (hwid != hantroenc_data[i].subsys_data.cfg.asic_id)) {
			pr_info("hantroenc: HW not found at %p\n",
				(void *)hantroenc_data[i]
					.subsys_data.cfg.base_addr);
#ifdef hantroenc_DEBUG
			dump_regs((unsigned long)&hantroenc_data);
#endif
			ReleaseIO();
			hantroenc_data[i].is_valid = 0;
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

		pr_info("hantroenc: HW at base <%p> with ID <0x%08x>\n",
			(void *)hantroenc_data[i].subsys_data.cfg.base_addr, hwid);
	}

	if (found_hw == 0) {
		pr_err("hantroenc: NO ANY HW found!!\n");
		return -1;
	}

	return 0;
}

static void ReleaseIO(void)
{
	u32 i;
	hantroenc_t *hantroenc_data = subsys_mgr.hantroenc_data;

	for (i = 0; i < subsys_mgr.total_subsys_num; i++) {
		if (hantroenc_data[i].is_valid == 0)
			continue;
		if (hantroenc_data[i].hwregs)
			iounmap((void __iomem *)hantroenc_data[i].hwregs);
		release_mem_region(hantroenc_data[i].subsys_data.cfg.base_addr,
				   hantroenc_data[i].subsys_data.cfg.iosize);
	}
}

#if (KERNEL_VERSION(2, 6, 18) > LINUX_VERSION_CODE)
static irqreturn_t hantroenc_isr(int irq, void *dev_id, struct pt_regs *regs)
#else
static irqreturn_t hantroenc_isr(int irq, void *dev_id)
#endif
{
	unsigned int handled = 0;
	hantroenc_t *dev = (hantroenc_t *)dev_id;
	u32 irq_status;
	unsigned long flags;
	u32 core_type = 0, i = 0;
	unsigned long reg_offset = 0;
	u32 hwId, majorId, wClr, minorId, productId;

	/*get core id by irq from subsys config*/
	for (i = 0; i < CORE_MAX; i++) {
		if (dev->subsys_data.core_info.irq[i] == irq) {
			core_type = i;
			reg_offset = dev->subsys_data.core_info.offset[i];
			break;
		}
	}

	/*If core is not reserved by any user, but irq is received, just clean it*/
	spin_lock_irqsave(&subsys_mgr.owner_lock, flags);
	if (!dev->is_reserved[core_type]) {
		pr_debug(
			"hantroenc_isr:received IRQ but core is not reserved!\n");
		irq_status = (u32)ioread32(
			(void __iomem *)(dev->hwregs + reg_offset + 0x04));
		if (irq_status & 0x01) {
#ifdef SUPPORT_WATCHDOG
			/* if there is an interrupt(exclusive slice ready) from VCE,
			 * should stop the watchdog.
			 */
			if (irq_status & ASIC_STATUS_ALL & ~ASIC_STATUS_SLICE_READY)
				_watchdog_stop(dev);
#endif
			/*  Disable HW when buffer over-flow happen
			 *  HW behavior changed in over-flow
			 *    in-pass, HW cleanup HWIF_ENC_E auto
			 *    new version:  ask SW cleanup HWIF_ENC_E when buffer over-flow
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
		spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);
		return IRQ_HANDLED;
	}
	spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);

	pr_debug("hantroenc_isr:received IRQ!\n");
	irq_status = (u32)ioread32((void __iomem *)(dev->hwregs + reg_offset + 0x04));
	pr_debug("irq_status of subsys %d core %d is:%x\n", dev->subsys_id,
		 core_type, irq_status);
	if (irq_status & 0x01) {
#ifdef SUPPORT_WATCHDOG
		/* if there is an interrupt(exclusive slice ready) from VCE,
		 * should stop the watchdog.
		 */
		if (irq_status & ASIC_STATUS_ALL & ~ASIC_STATUS_SLICE_READY)
			_watchdog_stop(dev);
#endif
		/*  Disable HW when buffer over-flow happen
		 *  HW behavior changed in over-flow
		 *    in-pass, HW cleanup HWIF_ENC_E auto
		 *    new version:  ask SW cleanup HWIF_ENC_E when buffer over-flow
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

		spin_lock_irqsave(&subsys_mgr.owner_lock, flags);
		dev->irq_received[core_type] = 1;
		dev->irq_status[core_type] = irq_status & (~0x01);
		spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);

#ifdef LOW_LATENCY_SLICEINFO_SUPPORT
		u32 poll_sliceinfo_irq_status = 0;
		u32 hw_reset_irq = 0;

		pr_debug("hantroenc_isr:received polling sliceinfo timeout IRQ!\n");
		/* swreg118 bit 0 for polling sliceinfo timeout IRQ status */
		poll_sliceinfo_irq_status =
			 (u32)ioread32((void __iomem *)(dev->hwregs + reg_offset + 0x1d8));
		pr_debug("poll_sliceinfo_irq_status of subsys %d core %d is:%x\n",
				dev->subsys_id, core_type, poll_sliceinfo_irq_status);
		if (poll_sliceinfo_irq_status & 0x1) {
			/* clear irq bit by writing 1 */
			wClr = poll_sliceinfo_irq_status;
			iowrite32(wClr, (void __iomem *)(dev->hwregs + reg_offset + 0x1d8));

			spin_lock_irqsave(&subsys_mgr.owner_lock, flags);
			dev->irq_received[core_type] = 1;
			dev->poll_sliceinfo_timeout_irq_status[core_type] =
				(poll_sliceinfo_irq_status & 0x01) << 13;
			spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);
			handled++;
		}
#endif

#ifdef SUPPORT_UFBC
		u32 ufbc_irq_status = 0;

		pr_debug("hantroenc_isr:received ufbc decode error IRQ!\n");
		/* swreg118 bit 0 for polling sliceinfo timeout IRQ status */
		ufbc_irq_status =
			 (u32)ioread32((void __iomem *)(dev->hwregs + reg_offset + 0xc20));
		pr_debug("poll_sliceinfo_irq_status of subsys %d core %d is:%x\n",
			    dev->subsys_id, core_type, ufbc_irq_status);
		if (ufbc_irq_status & 0x3) {
			/* clear irq bit by writing 1 */
			wClr = ufbc_irq_status;
			iowrite32(wClr, (void __iomem *)(dev->hwregs + reg_offset + 0xc20));

			spin_lock_irqsave(&subsys_mgr.owner_lock, flags);
			dev->ufbc_irq_status[core_type] =
				(ufbc_irq_status & 0x03) << 14;
			spin_unlock_irqrestore(&subsys_mgr.owner_lock, flags);
			handled++;
		}
#endif
#ifdef CONFIG_ENC_PM
		//if frame_rdy IRQ is received, then HW will not be used any more.
		if (irq_status & ASIC_STATUS_FRAME_READY)
			up(&dev->core_suspend_sem);
#endif
		wake_up_interruptible_all(&subsys_mgr.enc_wait_queue);
		handled++;
	}
	if (!handled)
		PDEBUG("IRQ received, but not hantro's!\n");
	return IRQ_HANDLED;
}

#ifdef hantroenc_DEBUG
static void ResetAsic(hantroenc_t *dev)
{
	int i, n;

	for (n = 0; n < subsys_mgr.total_subsys_num; n++) {
		if (dev[n].is_valid == 0)
			continue;
		iowrite32(0, (void *)(dev[n].hwregs + 0x14));
		for (i = 4; i < dev[n].subsys_data.cfg.iosize; i += 4)
			iowrite32(0, (void *)(dev[n].hwregs + i));
	}
}

static void dump_regs(unsigned long data)
{
	hantroenc_t *dev = (hantroenc_t *)data;
	int i;

	PDEBUG("Reg Dump Start\n");
	for (i = 0; i < dev->iosize; i += 4)
		PDEBUG("\toffset %02X = %08X\n", i, ioread32(dev->hwregs + i));
	PDEBUG("Reg Dump End\n");
}
#endif

static int hantroenc_mmap(struct file *filp, struct vm_area_struct *vma)
{
	size_t size = vma->vm_end - vma->vm_start;
	int i, ret = 0;
	unsigned long start = 0, end = 0;
	unsigned long base_start = 0, base_end = 0;

	PDEBUG("%s %08lx-%08lx -> %08lx, %s\n", __func__,
		   (long)(vma->vm_pgoff << PAGE_SHIFT),
		   (long)(vma->vm_pgoff << PAGE_SHIFT) + (int)(size),
		   (long)vma->vm_start,
		   (filp->f_flags & O_SYNC) ? "uncached" : "cached");

	start = vma->vm_pgoff << PAGE_SHIFT;
	end = start + size;
	if (subsys_mgr.linebuffer_sram_used) {
		base_start = subsys_mgr.sram_base;
		base_end   = ((subsys_mgr.sram_base + subsys_mgr.sram_size) / PAGE_SIZE + 1) * PAGE_SIZE;
		if (start < base_start || base_end < end)
			pr_info("It is not Linebuffer address!\n");
		else
			goto mem_remap;
	}

	for (i = 0; i < subsys_mgr.total_subsys_num; i++) {
		base_start = subsys_array[i].base_addr;
		base_end   = ((subsys_array[i].base_addr + subsys_array[i].iosize) /
						PAGE_SIZE + 1) * PAGE_SIZE;
		if (start < base_start || base_end < end) {
			pr_info("It is not subsys %d reg base address !\n", i);
			if ((i + 1) == subsys_mgr.total_subsys_num)
				goto err_addr;
		} else {
			goto mem_remap;
		}
	}

mem_remap:
	// support only uncached mode
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

	vma->vm_ops = &hantroenc_vm_ops;

	ret = remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff, size,
		  vma->vm_page_prot);
	if (ret != 0) {
		pr_err("remap_pfn_range() failed.\n");
		goto err_out;
	}

	return 0;

err_addr:
	pr_err("Invalid adress %08lx-%08lx\n", start, end);
	return -EINVAL;

err_out:
	return ret;
}
