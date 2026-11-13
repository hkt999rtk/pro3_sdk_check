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
#include "device_platform_config.h"
#include "osal.h"
#include "memalloc_freertos.h"

/* Allocate chunks from a malloc-backed shared pool. */
#define ENCODER_MEMALLOC_SHARED_POOL 1
/* Allocate and free each buffer directly with system malloc/free. */
#define ENCODER_MEMALLOC_SYSTEM_MALLOC 2

#ifndef CONFIG_ENCODER_MEMALLOC_STRATEGY
#define CONFIG_ENCODER_MEMALLOC_STRATEGY ENCODER_MEMALLOC_SYSTEM_MALLOC
#endif

#if (CONFIG_ENCODER_MEMALLOC_STRATEGY != ENCODER_MEMALLOC_SHARED_POOL) && \
    (CONFIG_ENCODER_MEMALLOC_STRATEGY != ENCODER_MEMALLOC_SYSTEM_MALLOC)
#error "Unsupported CONFIG_ENCODER_MEMALLOC_STRATEGY"
#endif

#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SHARED_POOL
/* Dev Memory Map for NETNT */
#define DEV_MEM_BANK_0 (0)
#define MEM_BANK0_SIZE (16 * 1024 * 1024 * 1024LL)
#define DEV_MEM_BANK_1 (1)
#define MEM_BANK1_SIZE (16 * 1024 * 1024 * 1024LL)

static void *shared_mem_pool;

#define SHARED_MEM_DEV_ADDRESS ((unsigned long long)(uintptr_t)shared_mem_pool)
#define SHARED_MEM_CPU_ADDRESS (shared_mem_pool)
#define SHARED_MEM_SIZE (22 * 1024 * 1024)

#define SHARED_MEM_BANK (DEV_MEM_BANK_0)

/*
#define MEM_0_DEV_ADDRESS (SHARED_MEM_DEV_ADDRESS + SHARED_MEM_SIZE)
#define MEM_0_CPU_ADDRESS (0)
#define MEM_0_SIZE (MEM_BANK0_SIZE + MEM_BANK1_SIZE - SHARED_MEM_SIZE)
#define MEM_0_BANK (DEV_MEM_BANK_0)

#define MEM_1_DEV_ADDRESS        (0xC00000000LL)
#define MEM_1_CPU_ADDRESS        (0)
#define MEM_1_SIZE               (16*1024*1024*1024LL)
#define MEM_1_BANK               (DEV_MEM_BANK_1)
*/

/* the size of chunk in MEMALLOC_DYNAMIC */
#define CHUNK_SIZE (16 * 1024)

/* DO NOT change below defines, they need to be matched with EWL_MEM_TYPE_xx */
#define _MEM_TYPE_CPU 0U /* CPU RW. non-secure CMA memory */
#define _MEM_TYPE_SLICE 1U /* VPU W, CPU RW. As output buffer for Encoder */
#define _MEM_TYPE_DPB 2U /* VPU W. As Input/Recon buffer for Encoder */
#define _MEM_TYPE_VPU_WORKING                                                  \
	3U /* VPU RW, CPU RW. non-secure memory. As working buffer for Encoder */
#define _MEM_TYPE_VPU_WORKING_SPECIAL 4U /* VPU R, CPU RW. */
#define _MEM_TYPE_VPU_ONLY 5U /* VPU RW only. */
#endif

/*
  memory type enum
*/
#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SHARED_POOL
typedef enum {
	MEM_TYPE_SHARED = 0,
	MEM_TYPE_DEVICE,
} _mem_type;
#endif

/*
  memory region id enum, also as index of array "_regions" and array "hlina_mgr"
*/
typedef enum {
	MEM_ID_SHARED = 0,
	/*  MEM_ID_0, */
	/*  MEM_ID_1, */
	MAX_MEM_REGION
} _mem_region_id;

#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SHARED_POOL
/*
  memory region properties struct
*/
typedef struct __mem_region {
	unsigned long long bus_addr; /* Physical address of dev memory region */
	char *virt_addr; /* virtual address mapping to CPU, 0 means invisible to CPU */
	unsigned long long size; /* size of dev memory region */
	unsigned short bank; /* DDR RAM bank id */
	_mem_type type;

	unsigned long
		translation_offset; /* user space SW will subtract HLINA_TRANSL_OFFSET from the bus address
					  * and decoder HW will use the result as the address translated base
					  * address. The SW needs the original host memory bus address for memory
					  * mapping to virtual address. */
} _mem_region_t;

/*
  descriptor for each chunk
*/
typedef struct hlinc {
	unsigned short
		chunks_occupied; /* indicate number of occupied chunks which start from current chunk */
	int filp; /* Client that allocated this chunk */
} hlina_chunk_t;

/*
  memory region management struct
*/
typedef struct __hlina_mgr {
	hlina_chunk_t *hlina_chunks; /* point to descriptor of each chunks */
	unsigned long chunks; /* total chunks in this mem region */

	unsigned long
		reserved_chunks; /* number of reserved chunks in this mem rtegion.
					 * The reserved chunks is located at the beginning of corresponding mem region, application can't use them */
	unsigned long
		used_chunks_max; /* high water mark of concurrently occupied chunks (excluding reserved chunks) */
	_mem_region_t *mem_prop;
} _hlina_mgr_t;

/*
  memory regions perporties, indexed by _mem_region_id.
*/
static _mem_region_t _regions[MAX_MEM_REGION] = {
#if 0   // --> move to memalloc_init()
	/*MEM_ID_SHARED*/ { SHARED_MEM_DEV_ADDRESS,
			    (char *)SHARED_MEM_CPU_ADDRESS, SHARED_MEM_SIZE,
			    SHARED_MEM_BANK, MEM_TYPE_SHARED, 0 },
	/*MEM_ID_0      { MEM_0_DEV_ADDRESS, (char *)MEM_0_CPU_ADDRESS,
			    MEM_0_SIZE, MEM_0_BANK, MEM_TYPE_DEVICE, 0 } */
	/*MEM_ID_1        { MEM_1_DEV_ADDRESS,      (char *)MEM_1_CPU_ADDRESS,      MEM_1_SIZE,      MEM_1_BANK,       MEM_TYPE_DEVICE,  0 } */
#endif
};

/*
  memory regions hlina manager, indexed by _mem_region_id.
*/
static _hlina_mgr_t hlina_mgr[MAX_MEM_REGION];
#endif

int memalloc_major = -1;
//static DEFINE_SPINLOCK(mem_lock);
spinlock_t mem_lock;
/*
  Functions declaration
*/
static int AllocMemory(unsigned long long *busaddr, unsigned int size,
		       unsigned int region, const int filp);
static int FreeMemory(unsigned long long busaddr, const int filp);
//static void * DirectMemoryMap(unsigned long long busaddr, unsigned long map_size);
#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SHARED_POOL
static void CalculateChunks(_mem_region_id id, unsigned long *chunks,
			    unsigned long *chunks_mgr_size);
#endif
static void ResetMems(void);
#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SHARED_POOL
static unsigned int GetFreeChunks(_mem_region_id id);
#endif

#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SHARED_POOL
/*
  To specified mem region, calculate total chunks number and aligned buffer size to store chunks' descriptors.
*/
static void CalculateChunks(_mem_region_id id, unsigned long *chunks,
			    unsigned long *chunks_mgr_size)
{
	*chunks = 0;
	*chunks_mgr_size = 0;

	if (id < MAX_MEM_REGION) {
		*chunks = _regions[id].size / CHUNK_SIZE;
		*chunks_mgr_size =
			(((unsigned int)*chunks) * sizeof(hlina_chunk_t) +
			 CHUNK_SIZE - 1) &
			(~(CHUNK_SIZE - 1));
	}
}
#endif

/*
  To specified bus address, convert it to virtual (CPU) address according to mem region properties.
*/
void *DirectMemoryMap(unsigned long long busaddr, unsigned long map_size)
{
#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SHARED_POOL
	int i;
	unsigned long long offset;
	void *virtaddr = NULL;
	u32 busAddrRight = 0;
#endif

	if (map_size == 0) {
		return NULL;
	}

#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SYSTEM_MALLOC
	return (void *)(uintptr_t)busaddr;
#else
	for (i = 0; i < MAX_MEM_REGION; i++) {
		if ((busaddr >= _regions[i].bus_addr) &&
		    (busaddr < _regions[i].bus_addr + _regions[i].size)) {
			busAddrRight = 1;
			if (!_regions[i].virt_addr) {
				continue;
			} else {
				offset = busaddr - _regions[i].bus_addr;
				if ((offset + map_size) <= _regions[i].size) {
					virtaddr =
						(void *)(_regions[i].virt_addr +
							 offset);
					break;
				}
			}
		}
	}
	if ((!busAddrRight) && (!virtaddr)) {
		return (void *)(unsigned long)busaddr;
	}

	return virtaddr;
#endif
}

/*
  ioctl
*/
long memalloc_ioctl(rtos_vpu_filp *filp, unsigned int cmd, void *arg)
{
	int ret = 0;
	MemallocParams memparams;
	ptr_t busaddr;
#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SHARED_POOL
	unsigned int region;
#endif

	PDEBUG("memalloc ioctl cmd 0x%08x\n", cmd);

	/*
   * extract the type and number bitfields, and don't decode
   * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
   */
	if (_IOC_TYPE(cmd) != MEMALLOC_IOC_MAGIC)
		return -ENOTTY;
	if (_IOC_NR(cmd) > MEMALLOC_IOC_MAXNR)
		return -ENOTTY;

	if (_IOC_DIR(cmd) & _IOC_READ)
		ret = !access_ok(VERIFY_WRITE, arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		ret = !access_ok(VERIFY_READ, arg, _IOC_SIZE(cmd));
	if (ret)
		return -EFAULT;

	spin_lock(&mem_lock);

	switch (cmd) {
	case MEMALLOC_IOCHARDRESET:
		PDEBUG("memalloc HARDRESET\n");
		ResetMems();
		break;
	case MEMALLOC_IOCXGETBUFFER:
		PDEBUG("memalloc GETBUFFER");

		ret = copy_from_user(&memparams, (MemallocParams *)arg,
				     sizeof(MemallocParams));
		if (ret)
			break;

#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SYSTEM_MALLOC
		ret = AllocMemory(&memparams.bus_address, memparams.size,
				  MEM_ID_SHARED, filp->fd);
		memparams.translation_offset = 0;
#else
		region = MEM_ID_SHARED;
		if (memparams.mem_type == _MEM_TYPE_DPB ||
		    memparams.mem_type == _MEM_TYPE_VPU_ONLY)
#ifndef MEM_ONLY_DEV_CHECK
			region =
				MEM_ID_SHARED; //For get output buffer when  no pcie
#else
			region = MEM_ID_0;
#endif
		ret = AllocMemory(&memparams.bus_address, memparams.size,
				  region, filp->fd);

		memparams.translation_offset =
			_regions[region].translation_offset;
#endif

		ret |= copy_to_user((MemallocParams *)arg, &memparams,
				    sizeof(MemallocParams));

		break;
	case MEMALLOC_IOCSFREEBUFFER:
		PDEBUG("memalloc FREEBUFFER\n");

		__get_user(busaddr, (ptr_t *)arg);
		ret = FreeMemory(busaddr, filp->fd);
		break;
	}

	spin_unlock(&mem_lock);

	return ret ? -EFAULT : 0;
}

int memalloc_open(void *inode, rtos_vpu_filp *filp)
{
	PDEBUG("memalloc dev opened\n");
	return 0;
}

int memalloc_release(void *inode, rtos_vpu_filp *filp)
{
#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SYSTEM_MALLOC
	(void)inode;
	(void)filp;
#else
	u32 i = 0, j = 0;
	hlina_chunk_t *p_hlina;

	for (i = 0; i < MAX_MEM_REGION; i++) {
		p_hlina = hlina_mgr[i].hlina_chunks;
		for (j = hlina_mgr[i].reserved_chunks; j < hlina_mgr[i].chunks;
		     j++) {
			spin_lock(&mem_lock);
			if (p_hlina[j].filp == filp->fd) {
				//printk(KERN_WARNING
				//       "memalloc: Found unfreed memory at release time!\n");

				p_hlina[j].filp = 0;
				p_hlina[j].chunks_occupied = 0;
			}
			spin_unlock(&mem_lock);
		}
	}
#endif
	PDEBUG("memalloc dev closed\n");
	return 0;
}

void __exit memalloc_cleanup(void)
{
	unregister_chrdev(memalloc_major, "memalloc");
#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SHARED_POOL
	free(shared_mem_pool);
	shared_mem_pool = NULL;
#endif
}

int __init memalloc_init(rtos_vpu_dev *__vdev)
{
	int result;
#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SHARED_POOL
	int i;
	unsigned long chunks, mgr_size, reserved_size;
	void *p;
#endif

	PDEBUG("memalloc module init\n");
	printk("memalloc: Linear Memory Allocator\n");

#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SYSTEM_MALLOC
	printk("memalloc: using system malloc\n");
#else
	shared_mem_pool = malloc(SHARED_MEM_SIZE);
	if (!shared_mem_pool) {
		printk(KERN_ERR "memalloc: unable to allocate shared memory pool\n");
		return -ENOMEM;
	}

	/* init the memory region */
	_regions[MEM_TYPE_SHARED].bus_addr = SHARED_MEM_DEV_ADDRESS;
	_regions[MEM_TYPE_SHARED].virt_addr = (char *)SHARED_MEM_CPU_ADDRESS;
	_regions[MEM_TYPE_SHARED].size = SHARED_MEM_SIZE;
	_regions[MEM_TYPE_SHARED].bank = SHARED_MEM_BANK;
	_regions[MEM_TYPE_SHARED].type = MEM_ID_SHARED;
	_regions[MEM_TYPE_SHARED].translation_offset = 0;

	/*
    Allocate memory for hlina_mgr
    In NETINT platform, there is no big heap memory to store hlina_mgr, so it is put to beginning of shared memory region directly
  */
	reserved_size = 0;
	for (i = 0; i < MAX_MEM_REGION; i++) {
		CalculateChunks(i, &chunks, &mgr_size);

		/* all hlina_chunks are put to SHARED memory region */
		p = DirectMemoryMap(_regions[MEM_ID_SHARED].bus_addr +
					    reserved_size,
				    mgr_size);
		hlina_mgr[i].hlina_chunks = (hlina_chunk_t *)p;
		hlina_mgr[i].chunks = chunks;
		hlina_mgr[i].reserved_chunks = 0;
		hlina_mgr[i].mem_prop = &_regions[i];

		reserved_size += mgr_size;
		printk("memalloc: Linear memory base = 0x%08llx\n",
		       _regions[i].bus_addr);
		printk(KERN_INFO
		       "memalloc: Region[%d] size %lu MB; %lu chunks of size %d\n",
		       i, (unsigned long)(_regions[i].size / (1024 * 1024)),
		       chunks, CHUNK_SIZE);
	}
	hlina_mgr[MEM_ID_SHARED].reserved_chunks = reserved_size / CHUNK_SIZE;
#endif

	result = register_chrdev(memalloc_major, "memalloc", &memalloc_fops);
	if (result < 0) {
		PDEBUG("memalloc: unable to get major %d\n", memalloc_major);
		goto err;
	} else if (result != 0) { /* this is for dynamic major */
		memalloc_major = result;
	}

	ResetMems();
	spin_lock_init(&mem_lock);

#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SYSTEM_MALLOC
	__vdev->ctx = NULL;
#else
	__vdev->ctx = (void *)&hlina_mgr;
#endif
	__vdev->_open = memalloc_open;
	__vdev->_release = memalloc_release;
	__vdev->_ioctl = memalloc_ioctl;
	__vdev->_cleanup = memalloc_cleanup;

	printf("memalloc init successful...\n");
	return 0;

err:
#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SHARED_POOL
	free(shared_mem_pool);
	shared_mem_pool = NULL;
#endif

	return result;
}

/*
  Cycle through the buffers in specified region, give the first free one.
  TBD: need to check if specifying _mem_type is better than region.
*/
static int AllocMemory(unsigned long long *busaddr, unsigned int size,
		       unsigned int region, const int filp)
{
#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SYSTEM_MALLOC
	void *address;

	(void)region;
	(void)filp;
	*busaddr = 0;

	address = malloc(size);
	if (!address) {
		printk("memalloc: Allocation FAILED: size = %u\n", size);
		return -EFAULT;
	}

	*busaddr = (unsigned long long)(uintptr_t)address;
	return 0;
#else
	u32 i = 0;
	u32 j = 0;
	unsigned int skip_chunks = 0;
	unsigned long total_chunks;
	hlina_chunk_t *p_hlina;

	/* calculate how many chunks we need; round up to chunk boundary */
	unsigned int alloc_chunks = (size + CHUNK_SIZE - 1) / CHUNK_SIZE;

	*busaddr = 0;

	if (region >= MAX_MEM_REGION) {
		printk("memalloc: Allocation FAILED: unknown region = %u\n",
		       region);
		return -EFAULT;
	}

	p_hlina = hlina_mgr[region].hlina_chunks;
	total_chunks = hlina_mgr[region].chunks;

	/* run through the chunk table */
	for (i = hlina_mgr[region].reserved_chunks; i < total_chunks;) {
		skip_chunks = 0;
		/* if this chunk is available */
		if (!p_hlina[i].chunks_occupied) {
			/* check that there is enough memory left */
			if (i + alloc_chunks > total_chunks)
				break;

			/* check that there is enough consecutive chunks available */
			for (j = i; j < i + alloc_chunks; j++) {
				if (p_hlina[j].chunks_occupied) {
					skip_chunks = 1;
					/* skip the used chunks */
					i = j + p_hlina[j].chunks_occupied;
					break;
				}
			}

			/* if enough free memory found */
			if (!skip_chunks) {
				*busaddr =
					hlina_mgr[region].mem_prop->bus_addr +
					i * CHUNK_SIZE;
				p_hlina[i].filp = filp;
				p_hlina[i].chunks_occupied = alloc_chunks;
				break;
			}
		} else {
			/* skip the used chunks */
			i += p_hlina[i].chunks_occupied;
		}
	}

	if (*busaddr == 0) {
		printk("memalloc: Allocation FAILED: size = %u\n", size);
		return -EFAULT;
	} else {
		unsigned long used_chunks = (total_chunks - hlina_mgr[region].reserved_chunks) -
									GetFreeChunks((_mem_region_id)region);

		if (used_chunks > hlina_mgr[region].used_chunks_max) {
			hlina_mgr[region].used_chunks_max = used_chunks;
		}

		PDEBUG("MEMALLOC OK: size: %u, reserved: %u\n", size,
		       alloc_chunks * CHUNK_SIZE);
	}

	return 0;
#endif
}

/* Free a buffer based on bus address */
static int FreeMemory(unsigned long long busaddr, const int filp)
{
#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SYSTEM_MALLOC
	(void)filp;
	free((void *)(uintptr_t)busaddr);
	return 0;
#else
	u32 i, id;
	unsigned long long addr;
	hlina_chunk_t *p_hlina;

	for (i = 0; i < MAX_MEM_REGION; i++) {
		addr = busaddr + _regions[i].translation_offset;
		if ((addr >= _regions[i].bus_addr) &&
		    (addr < _regions[i].bus_addr + _regions[i].size)) {
			id = (addr - _regions[i].bus_addr) / CHUNK_SIZE;
			p_hlina = &hlina_mgr[i].hlina_chunks[id];
			if ((p_hlina->chunks_occupied) &&
			    (p_hlina->filp == filp)) {
				p_hlina->filp = 0;
				p_hlina->chunks_occupied = 0;
				return 0;
			}
		}
	}

	printk(KERN_WARNING "memalloc: Owner mismatch while freeing memory!\n");
	return -1;
#endif
}

/* Reset "used" status */
static void ResetMems(void)
{
#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SYSTEM_MALLOC
	/* Direct allocations have no shared allocator state to reset. */
#else
	u32 i, j = 0;
	hlina_chunk_t *p_hlina;

	for (i = 0; i < MAX_MEM_REGION; i++) {
		p_hlina = hlina_mgr[i].hlina_chunks;
		for (j = hlina_mgr[i].reserved_chunks; j < hlina_mgr[i].chunks;
		     j++) {
			p_hlina[j].filp = 0;
			p_hlina[j].chunks_occupied = 0;
		}
	}
#endif
}

#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SHARED_POOL
/* Walk the chunk table the same way AllocMemory() does, and count chunks not covered by any occupied run */
static unsigned int GetFreeChunks(_mem_region_id id)
{
	u32 i;
	unsigned long total_chunks;
	unsigned int free_chunks = 0;
	hlina_chunk_t *p_hlina;

	if (id >= MAX_MEM_REGION) {
		return 0;
	}

	p_hlina = hlina_mgr[id].hlina_chunks;
	total_chunks = hlina_mgr[id].chunks;

	for (i = hlina_mgr[id].reserved_chunks; i < total_chunks;) {
		if (p_hlina[i].chunks_occupied) {
			i += p_hlina[i].chunks_occupied;
		} else {
			free_chunks++;
			i++;
		}
	}

	return free_chunks;
}
#endif

//memory type is shared
//return busAddr and virtrulAddr
ptr_t AllocMemForIODevice(unsigned int size)
{
	unsigned long long bus_addr = 0;

	AllocMemory(&bus_addr, size, MEM_ID_SHARED, MEM_FD);

	return (ptr_t)bus_addr;
}

void FreeMemForIODevice(ptr_t ba)
{
	unsigned long long bus_addr = (unsigned long long)ba;

	FreeMemory(bus_addr, MEM_FD);
}

/* Memory currently available to the encoder allocator */
static unsigned int GetFreeMem(void)
{
#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SYSTEM_MALLOC
	return (unsigned int)xPortGetFreeHeapSize();
#else
	return GetFreeChunks(MEM_ID_SHARED) * CHUNK_SIZE;
#endif
}

/* Total memory currently available to or allocated by the encoder allocator */
static unsigned int GetTotalMem(void)
{
#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SYSTEM_MALLOC
	extern u32 total_heap_size;

	return total_heap_size;
#else
	return (hlina_mgr[MEM_ID_SHARED].chunks -
			hlina_mgr[MEM_ID_SHARED].reserved_chunks) * CHUNK_SIZE;
#endif
}

/* Currently allocated size, in bytes */
static unsigned int GetUsedMem(void)
{
	return GetTotalMem() - GetFreeMem();
}

/* High water mark, in bytes, of allocated size ever reached */
static unsigned int GetUsedMemMax(void)
{
#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SYSTEM_MALLOC
	extern u32 total_heap_size;

	return total_heap_size - (unsigned int)xPortGetMinimumEverFreeHeapSize();
#else
	return hlina_mgr[MEM_ID_SHARED].used_chunks_max * CHUNK_SIZE;
#endif
}

/* Log encoder allocator total/free/used/used_max memory */
void EncLogMemInfo(void)
{
	printk("EncMemStats:\r\n");
#if CONFIG_ENCODER_MEMALLOC_STRATEGY == ENCODER_MEMALLOC_SYSTEM_MALLOC
	printk("Mode:         system malloc\r\n");
#else
	printk("Mode:         shared pool\r\n");
#endif
	printk("Total Mem:    %u\r\n", GetTotalMem());
	printk("Mem Free Now: %u\r\n", GetFreeMem());
	printk("Mem Used Now: %u\r\n", GetUsedMem());
	printk("Mem Used Max: %u\r\n", GetUsedMemMax());
}
