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

#ifndef _VCX_VCMD_PRIV_FREERTOS_H_
#define _VCX_VCMD_PRIV_FREERTOS_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "base_type.h"
#include "osal.h"
#include "dev_common_freertos.h"

#include "bidirect_list_freertos.h"
#include "vcmdswhwregisters.h"
#include "vcx_driver_freertos.h"

#define MAX_VCMD_NUM			(MAX_VCMD_CORE_NUM)

/* VCMD memory pool layout as:
 *	0: CMDBUF memory, size is SLOT_NUM_CMDBUF * SLOT_SIZE_CMDBUF
 *	1: STATUSBUF memory, size is SLOT_NUM_CMDBUF * SLOT_SIZE_STATUSBUF
 *	2: REGBUF memory, size is MAX_VCMD_NUM * SLOT_SIZE_REGBUF
 */
#define VCMD_POOL_TOTAL_SIZE	(5 * 1024 * 1024)	/* match with vcmd_size which is module param of memalloc */

#if 1
#define SLOT_SIZE_CMDBUF		(512 * 4 * 4)
#define SLOT_SIZE_STATUSBUF		(512 * 4 * 4)
#define SLOT_SIZE_REGBUF		(64 * 1024)
#define SLOT_NUM_CMDBUF			(16)		/*TBD: need to match with MAX_VCMD_ENTRIES in user space*/
#else
#define SLOT_SIZE_CMDBUF		(576 * 4)   //(512 * 4 * 4)
#define SLOT_SIZE_STATUSBUF		(576 * 4)   //(512 * 4 * 4)
#define SLOT_SIZE_REGBUF		(8 * 1024)   //(64 * 1024)
#define SLOT_NUM_CMDBUF			(1)   //(256)		/*TBD: need to match with MAX_VCMD_ENTRIES in user space*/
#endif

#define REG_ID_CMDBUF_EXE_CNT	(3)
#define REG_ID_CMDBUF_EXE_ID	(26)
/* read vcmd swreg26 (CMDBUF_EXE_ID) to reg-buf[REG_ID2_CMDBUF_EXE_ID] at
 * beginning of each cmdbuf to indicate which cmdbuf is executing in reg-buf
 */
#define REG_ID2_CMDBUF_EXE_ID	(REG_ID_CMDBUF_EXE_ID + 1)

#define VCMD_REG_NUM_READ			(27)
#define ANY_CMDBUF_ID				0xFFFF

#define _ALIGN(X, Factor) ((((X) + ((Factor)-1)) / (Factor)) * (Factor))
#define ALIGN_4K(X) _ALIGN(X, 4096)
#define ALIGN_64(X) _ALIGN(X, 64)

#define VCMD_STATE_POWER_ON		0
#define VCMD_STATE_IDLE			1
#define VCMD_STATE_WORKING		2

#define HW_WORK_STATE_IDLE		0
#define HW_WORK_STATE_WORK		1
#define HW_WORK_STATE_STALL		2
#define HW_WORK_STATE_PEND		3
#define HW_WORK_STATE_ABORT		4

#define CMDBUF_EXE_STATUS_OK 0
#define CMDBUF_EXE_STATUS_CMDERR 1
#define CMDBUF_EXE_STATUS_BUSERR 2
#define CMDBUF_EXE_STATUS_ABORTED 3

#define _GET_PO(filp)	((struct proc_obj *)((struct vcmd_priv_ctx *)filp->private_data)->po)

#define CMDBUF_OFF(id)		((id) * SLOT_SIZE_CMDBUF)
#define STATUSBUF_OFF(id)	((id) * SLOT_SIZE_STATUSBUF)
#define CMDBUF_OFF_32(id)		(((id) * SLOT_SIZE_CMDBUF) / sizeof(u32))
#define STATUSBUF_OFF_32(id)	(((id) * SLOT_SIZE_STATUSBUF) / sizeof(u32))

/*these size need to be modified according to hw config.*/
#define VCMD_ENCODER_REGISTER_SIZE              (ENCODER_REGISTER_SIZE * 4)
#define VCMD_DECODER_REGISTER_SIZE              (DECODER_REGISTER_SIZE * 4)
#define VCMD_IM_REGISTER_SIZE                   (IM_REGISTER_SIZE * 4)
#define VCMD_JPEG_ENCODER_REGISTER_SIZE         (JPEG_ENCODER_REGISTER_SIZE * 4)
#define VCMD_JPEG_DECODER_REGISTER_SIZE         (JPEG_DECODER_REGISTER_SIZE * 4)

/* VCE: 0; VCD: 8 */
#define ENC_INTR_SRC_BIT                         0
#define DEC_INTR_SRC_BIT                         8
#define ABNORMAL_IRQ_BIT_MASK(A)                 ((A)+16)
#define ENC_ABNORMAL_IRQ_MASK                                      \
	(1 << ABNORMAL_IRQ_BIT_MASK(ENC_INTR_SRC_BIT))

enum {
	LOGLVL_VERBOSE = 0,		// log all
	LOGLVL_FLOW,			// log all debug msg
	LOGLVL_CONFIG,			// log ctrl/config info, mostly at beginning
	LOGLVL_BRIEF,			// log critical point
	LOGLVL_WARNING,			// log warning msg
	LOGLVL_ERROR,			// log error
};
/* declarations */
#define vcmd_klog(lvl, fmt, ...) {\
	if (lvl == LOGLVL_ERROR)	\
		printf("hantrovcmd: " fmt, ##__VA_ARGS__);	\
	else if (lvl >= vsi_kloglvl)		\
		printf("hantrovcmd: " fmt, ##__VA_ARGS__);	\
}

/* ------------------------------------------------------
 *  single linked list
 *-------------------------------------------------------
 */
typedef struct si_linked_node {
	struct si_linked_node *next;
	void *data;
} si_linked_node;

struct si_linked_list {
	spinlock_t spinlock;
	struct si_linked_node *head;
	struct si_linked_node *tail;
	struct si_linked_node nodes[SLOT_NUM_CMDBUF];
};

struct proc_obj {
	volatile u32 total_workload;
	spinlock_t spinlock;
	wait_queue_head_t resource_waitq;
	wait_queue_head_t job_waitq;
	u32 module_type;	/* correspond to filp, indicates subsys type */

	spinlock_t job_lock;
	struct rtos_vpu_filp *filp;
	struct bi_list job_done_list;
	u32 in_wait;		/* user is waiting for a specified cmdbuf run done */
};

struct noncache_mem {
	u32 *va;
	u32 size;
	ptr_t pa;		/* buffer physical address */
	ptr_t mmu_ba;	/* buffer bus address in MMU*/
};

struct cmdbuf_obj {
	/* current CMDBUF type: input vce=0,
	 * IM=1,vcd=2, pege=3, jpegd=4
	 */
	u32 module_type;
	/* current CMDBUFpriority: normal=0, high=1 */
	u32 priority;
	/* workload = encoded_image_size*(rdoLevel+1)*(rdoq+1);*/
	u32 workload;
	/* current CMDBUF size, change to payload after linked  */
	u32 interrupt_ctrl;
	u32 cmdbuf_size;
	/* current CMDBUF virtual address.*/
	u32 *cmd_va;
	/*current status CMDBUF virtual address.*/
	u32 *status_va;
	/*current status CMDBUF size*/
	u32 status_size;
	/*current CMDBUF executing status.*/
	u32 executing_status;
	/* which vcmd core is assigned to.*/
	u16 core_id;
	/* used to manage CMDBUF in driver.It is a handle
	 * to identify cmdbuf.also is an interrupt vector.
	 * position in pool,same as status position.
	 */
	u16 cmdbuf_id;
	/* cmdbuf node is in dev->work_list (1) or not (0) */
	u8 cmdbuf_linked;
	/* if 0,waiting for CMDBUF finish; if 1,
	 * op code in CMDBUF has finished one by one.
	 * HANTRO_VCMD_IOCH_WAIT_CMDBUF will check this variable.
	 */
	volatile u8 cmdbuf_run_done;
	/* if 0, wait slice ready
	 * if 1, slice has ready
	 */
	volatile u8 slice_run_done;
	/* if 0, not need to remove CMDBUF;
	 * if 1, CMDBUF can be removed if it is not used by vcmd hw;
	 */
	u8 cmdbuf_need_remove;
	/* The last command is JMP (1) or END (0). */
	u8 has_jmp_cmd;
	/* JMP command will trigger JMP interrupt (1) or not. */
	u8 jmp_ie;
	/* current CMDBUF physical address. */
	ptr_t cmd_pa;
	/*current status CMDBUF physical address.*/
	ptr_t status_pa;

	/* which cores can be uesd.*/
	u32 core_mask;
	/*file pointer occupying this obj.*/
	struct file *filp;
	/* pointer of process object occupying this obj*/
	struct proc_obj *po;
	//current status CMDBUF mmu mapping address.
	ptr_t mmu_status_ba;
	//current CMDBUF mmu mapping address.
	ptr_t mmu_cmd_ba;
};

enum subsys_module_id {
	SUB_MOD_VCMD = 0,
	SUB_MOD_MAIN,
	SUB_MOD_L2CACHE,
	SUB_MOD_MMU0,
	SUB_MOD_MMU1,
	SUB_MOD_DEC400,
	SUB_MOD_AXIFE0,
	SUB_MOD_AXIFE1,
	SUB_MOD_UFBC,

	SUB_MOD_MAX
};

enum vcmd_sw_init_mode {
	VCMD_INIT_NORMAL = 0,		//Not run init commands
	VCMD_INIT_TRIGGER = 1,		//Run init commands only when it is triggered
	VCMD_INIT_EXE_CMDBUF = 2,	//Run init commands before execution of each CMDBUF
};

#define DEFAULT_VCMD_INIT_MODE VCMD_INIT_TRIGGER

struct vcmd_subsys_info {
	ptr_t reg_base;		/* physical reg base addr of subsys */
	int irq;				/* irq of vcmd */

	/*input vce=0,IM=1,vcd=2,jpege=3, jpegd=4*/
	u32 sub_module_type;	/* type of main module */

	u16 reg_off[SUB_MOD_MAX];	/* reg offset of each module */
	u16 io_size[SUB_MOD_MAX]; /* reg size of each module, in bytes*/
	u32 vcmd_priority;

	u16 rreg_id[SUB_MOD_MAX];  /* start reg-id to read out when init driver */
	u16 rreg_num[SUB_MOD_MAX]; /* num of regs to read out when init driver */

	volatile u8 *hwregs[SUB_MOD_MAX];
};

struct vcmd_hw_features {
	u8 vcarb_ver; //hw support arbiter (vcarb version) or not (0)
	u8 has_init_mode;	//hw support init-mode (1) or not (0)
};

struct hantrovcmd_dev {
	//config of each core,such as base addr, irq,etc
	struct vcmd_subsys_info *subsys_info;
	u32 hw_version_id;
	//vcmd core id for driver and sw internal use
	u32 core_id;
	u32 id_in_type;
	struct vcmd_hw_features hw_feature;

	volatile u8 *hwregs; /* registers IO mem base */
	u32 reg_mirror[ASIC_VCMD_SWREG_AMOUNT];

	spinlock_t owner_lock_vcmd;
	spinlock_t *spinlock;			//point to owner_lock_vcmd
	bi_list work_list;

	u32 sw_cmdbuf_rdy_num;

	//number of cmdbufs without interrupt.
	u32 duration;
	volatile u32 state;
	u32 total_workload;

	//vcmd registers memory
	u32 *reg_mem_va;
	u32 reg_mem_sz;
	ptr_t reg_mem_ba;
	ptr_t mmu_reg_mem_ba;

	//translation offset from bus addr (ba) to physical addr (pa).
	ptr_t pa_trans_offset;
	wait_queue_head_t abort_queue_vcmd;

	wait_queue_head_t *abort_waitq;	//point to abort_queue_vcmd

	u32 abort_mode;		//0: stop when JMP/END; 1: stop curr-cmd (imediately);
	enum vcmd_sw_init_mode init_mode;

	u32 init_cmd_idx;	//for setting init cmd

	u32 aborted_cmdbuf_id;

	u8 mmu_enable;
	volatile u8 b_in_isr;

	void *handler;		// point to vcmd_mgr
	u32 abn_irq_mask; // abnormal irq mask
	u32 intr_gate_mask;
	spinlock_t abn_irq_lock;

	// timeout interrupt post process related
	TimerHandle_t timeout_timer;	//timeout timer handle
	u8 timeout_timer_active;
	u8 timeout_timer_expired;

	//watchdog for axife flush
	TimerHandle_t watchdog_timer;	//watchdog timer handle
	u8 watchdog_active;
	u8 watchdog_triggered;

	// a flag to indicate arbiter reset irq received
	u32 arb_reset_irq;
	// a flag to indicate arbiter err irq received
	u32 arb_err_irq;

#ifdef SUPPORT_DBGFS
	void *dbgfs_info;
#endif
};

struct vcmd_module_mgr {
	u32 num;
	struct hantrovcmd_dev *dev[MAX_VCMD_NUM];
	SemaphoreHandle_t sem; //for reserve cmdbuf
};

typedef struct {
	/* input configuration to vcmd manager */
	int subsys_num;
	struct vcmd_subsys_info core_array[MAX_VCMD_NUM];
	/* and this is our MAJOR; use 0 for dynamic allocation (recommended)*/
	int hantrovcmd_major;

	/* three linear buffers: vcmd buffer / status / registers */
	struct noncache_mem mem_vcmd;
	struct noncache_mem mem_status;
	struct noncache_mem mem_regs;

	//translation offset from bus addr (ba) to physical addr (pa).
	ptr_t pa_trans_offset;
	ptr_t reg_base_offset;

	struct vcmd_module_mgr module_mgr[MAX_VCMD_TYPE];

	/* cmdbuf/obj/node management*/
	struct cmdbuf_obj objs[SLOT_NUM_CMDBUF];

	struct bi_list_node nodes[SLOT_NUM_CMDBUF];		//nodes for dev work-list
	struct bi_list_node po_jobs[SLOT_NUM_CMDBUF];	//node for po job-list

	SemaphoreHandle_t free_obj_sema;
	struct si_linked_list free_obj_list;

	struct hantrovcmd_dev *dev_ctx;

	u8 mmu_enable;
	u8 vcmd_irq_enabled;

	//PCIE_EN defined
	struct noncache_mem pcie_pool;	//pool for mem_vcmd, mem_status and mem_regs
	struct pci_dev *pcie_dev;

	struct proc_obj *init_po;

	SemaphoreHandle_t isr_polling_sema;

	volatile u8 *mmu_hwregs[MAX_VCMD_NUM][2];

	//Kernel Thread
	pthread_t _kthread;
	pthread_t *kthread;
	volatile u8 stop_kthread;
	SemaphoreHandle_t kt_sema;

#ifdef SUPPORT_DBGFS
	void *dbgfs_ctx;
#endif
} vcmd_mgr_t;


struct vcmd_priv_ctx {
	void *vcmd_mgr;
	/* process object */
	void *po;
};

#define RREG_ADDR_INC			(0)
#define RREG_ADDR_FIX			(1)
#define RREG_MODE(A)			(((A) & 1) << 26)
#define RREG_G_MODE(A)			(((A) >> 26) & 1)
#define RREG_LEN(A)				(((A) & 0x3FF) << 16)
#define RREG_G_LEN(A)			((((((A) >> 16) & 0x3FF) - 1) & 0x3FF) + 1)
#define RREG_START_ADDR(A)		((A) & 0xFFFF)
struct cmd_rreg_t {
	u32 opcode;					//refer to register spec.
	u32 lsb;					//reg value buf addr [31..0]
	u32 msb;					//reg value buf addr [63..32]
	u32 padding;
};

#define JMP_RDY(A)			(((A) & 1) << 26)
#define JMP_G_RDY(A)		(((A) >> 26) & 1)
#define JMP_IE(A)			(((A) & 1) << 25)
#define JMP_G_IE(A)			(((A) >> 25) & 1)
#define JMP_NEXT_LEN(A)		((A) & 0xFFFF)
struct cmd_jmp_t {
	u32 opcode;					//refer to register spec.
	u32 lsb;					//next cmd-buf addr [31..0]
	u32 msb;					//next cmd-buf addr [63..32]
	u32 id;						//next cmd-buf id
};

#define WREG_ADDR_INC			(0)
#define WREG_ADDR_FIX			(1)
#define WREG_MODE(A)			(((A) & 1) << 26)
#define WREG_G_MODE(A)			(((A) >> 26) & 1)
#define WREG_LEN(A)				(((A) & 0x3FF) << 16)
#define WREG_G_LEN(A)			((((((A) >> 16) & 0x3FF) - 1) & 0x3FF) + 1)
#define WREG_START_ADDR(A)		((A) & 0xFFFF)
struct cmd_wreg_t {
	u32 opcode;					//refer to register spec.
	u32 data[0];
};

struct cmd_end_t {
	u32 opcode;					//refer to register spec.
	u32 padding;
};

#define CMD_SET_ADDR(cmd, ba) do { \
	(cmd)->lsb = (u32)(ba); \
	(cmd)->msb = (sizeof(ptr_t) == 4) ? 0 : ((u32)((u64)(ba) >> 32)); \
} while (0)

#ifdef __cplusplus
}
#endif
#endif
