/*
 * Copyright (c) 2020 Realtek Semiconductor Corp.	All rights reserved.
 *
 * Author: PSP Software Group
 */

#define UART_BASE_ADDRESS		0x40202000

#define TSGEN_BASE				0x60460000					/*?*/
#define TSGEN_CNTFID0			(TSGEN_BASE + 0x20)
#define TSGEN_FREQ				0x017d7840      /* 25 MHz */
//#define TSGEN_FREQ				12500000      /* 12.5 MHz */

#define TSGEN_CNTCR				(TSGEN_BASE + 0x0)
#define TSGEN_ENABLE			0x1

#ifndef FIQ_STACK_SIZE
#define FIQ_STACK_SIZE			1024
#endif
#ifndef IRQ_STACK_SIZE
#define IRQ_STACK_SIZE			1024
#endif
#ifndef SUPERVISOR_STACK_SIZE
#define SUPERVISOR_STACK_SIZE	2048
#endif
#ifndef ABORT_STACK_SIZE
#define ABORT_STACK_SIZE		1024
#endif
#ifndef UNDEF_STACK_SIZE
#define UNDEF_STACK_SIZE		1024
#endif
#ifndef SYS_STACK_SIZE
#define SYS_STACK_SIZE			1024
#endif

#define FIQ_STACK_SIZE_TOTAL	(FIQ_STACK_SIZE * CONFIG_CPUS_NUM)
#define IRQ_STACK_SIZE_TOTAL	(IRQ_STACK_SIZE * CONFIG_CPUS_NUM)
#define SUPERVISOR_STACK_SIZE_TOTAL	(SUPERVISOR_STACK_SIZE * CONFIG_CPUS_NUM)
#define ABORT_STACK_SIZE_TOTAL	(ABORT_STACK_SIZE * CONFIG_CPUS_NUM)
#define UNDEF_STACK_SIZE_TOTAL	(UNDEF_STACK_SIZE * CONFIG_CPUS_NUM)
#define SYS_STACK_SIZE_TOTAL	(SYS_STACK_SIZE * CONFIG_CPUS_NUM)

#ifndef HEAP_SIZE
#define HEAP_SIZE				0x1000
#endif

	.section .stack, "aw"
	.align 3
__fiq_stack:
	.space FIQ_STACK_SIZE_TOTAL
	.size __fiq_stack, . - __fiq_stack

__irq_stack:
	.space IRQ_STACK_SIZE_TOTAL
	.size __irq_stack, . - __irq_stack

__supervisor_stack:
	.space SUPERVISOR_STACK_SIZE_TOTAL
	.size __supervisor_stack, . - __supervisor_stack

__abort_stack:
	.space ABORT_STACK_SIZE_TOTAL
	.size __abort_stack, . - __abort_stack

__undef_stack:
	.space UNDEF_STACK_SIZE_TOTAL
	.size __undef_stack, . - __undef_stack

__sys_stack:
	.space SYS_STACK_SIZE_TOTAL
	.size __sys_stack, . - __sys_stack

	.section .heap,"aw"
	.align 3
__heap:
	.space HEAP_SIZE
	.size __heap, . - __heap

	.section .boot,"ax"
	.global	_boot
/* this initializes the various processor modes */
_prestart:
_boot:
	/* set VBAR to the _vector_table address in linker script */
	ldr	r0, =_vector_table
	mcr	p15, 0, r0, c12, c0, 0

	/* Read mpidr */
	mrc	p15, 0, r0, c0, c0, 5
	and	r8, r0, #0xff
	add	r8, r8, #0x1				@ for stack top calculation

	/* Disable MMU, if enabled */
	/* clear I/C to disable I/D Cache */
	/* clear M to disable MMU, clear Z to disable branch prediction */
	mrc	p15, 0, r0, c1, c0, 0		@ read CP15 register 1
	bic	r0, r0, #0x00002000			@ clear bits 13 (--V-)
	bic	r0, r0, #0x00000007			@ clear bits 2:0 (-CAM)
	orr	r0, r0, #0x00000002			@ set bit 1 (--A-) Align
	orr	r0, r0, #0x00000800			@ set bit 11 (Z---) BTB
	bic	r0, r0, #0x00001000			@ clear bit 12 (I) I-cache
	mcr	p15, 0, r0, c1, c0, 0		@ write value back

	/* Invalidate caches and TLBs */
	mov	r0, #0						@ r0 = 0
	mcr	p15, 0, r0, c8, c7, 0		@ invalidate TLBs
	mcr	p15, 0, r0, c7, c5, 0		@ invalidate icache
	mcr	p15, 0, r0, c7, c5, 6		@ invalidate branch predictor array
	dsb	sy
	isb	sy

	cmp	r8, #1						@ r8 = cpuid + 1
	blgt	invalidate_dcache_louis
	cmp	r8, #1
	bleq	invalidate_dcache_all

	/* set stack */
	/* cpsr[4:0] = mode */
	/* cpsr[5] = 0 , ARM state */
	/* cpsr[6] = 1 , Disable FIQ */
	/* cpsr[7] = 1 , Disable IRQ */
	mrs	r0, cpsr					@ get the current PSR
	mvn	r1, #0x1f					@ set up the FIQ stack pointer
	and	r2, r1, r0
	orr	r2, r2, #0x11				@ FIQ mode
	msr	cpsr, r2
	mov	r0, #FIQ_STACK_SIZE
	mul	r1, r0, r8
	ldr	r2, =__fiq_stack
	add	r13, r1, r2					@ FIQ stack pointer

	mrs	r0, cpsr					@ get the current PSR
	mvn	r1, #0x1f					@ set up the irq stack pointer
	and	r2, r1, r0
	orr	r2, r2, #0x12				@ IRQ mode
	msr	cpsr, r2
	mov	r0, #IRQ_STACK_SIZE
	mul	r1, r0, r8
	ldr	r2, =__irq_stack
	add	r13, r1, r2					@ IRQ stack pointer

	mrs	r0, cpsr					@ get the current PSR
	mvn	r1, #0x1f					@ set up the Abort stack pointer
	and	r2, r1, r0
	orr	r2, r2, #0x17				@ Abort mode
	msr	cpsr, r2
	mov	r0, #ABORT_STACK_SIZE
	mul	r1, r0, r8
	ldr	r2, =__abort_stack
	add	r13, r1, r2					@ Abort stack pointer

	mrs	r0, cpsr					@ get the current PSR
	mvn	r1, #0x1f					@ set up the Undefine stack pointer
	and	r2, r1, r0
	orr	r2, r2, #0x1b				@ Undefine mode
	msr	cpsr, r2
	mov	r0, #UNDEF_STACK_SIZE
	mul	r1, r0, r8
	ldr	r2, =__undef_stack
	add	r13, r1, r2					@ Undefine stack pointer

	mrs	r0, cpsr					@ get the current PSR
	mvn	r1, #0x1f					@ set up the system stack pointer
	and	r2, r1, r0
	orr	r2, r2, #0x1f				@ SYS mode
	msr	cpsr, r2
	mov	r0, #SYS_STACK_SIZE
	mul	r1, r0, r8
	ldr	r2, =__sys_stack
	add	r13, r1, r2					@ SYS stack pointer

	mrs	r0, cpsr					@ get the current PSR
	mvn	r1, #0x1f					@ set up the supervisor stack pointer
	and	r2, r1, r0
	orr	r2, r2, #0x13				@ Supervisor mode
	msr	cpsr, r2
	mov	r0, #SUPERVISOR_STACK_SIZE
	mul	r1, r0, r8
	ldr	r2, =__supervisor_stack
	add	r13, r1, r2					@ Supervisor stack pointer

	/* initialize TTBCR */
	/* TODO: use C function */
	mov	r0, #0						@ Use short descriptor
	mcr	p15, 0, r0, c2, c0, 2		@ Base address is 16KB aligned
									@ Perform translation table walk for TTBR0	
	/* enable MMU and cache */
	bl  init_mmu_table
	ldr	r0, =MMUTable				@ load MMU translation table base
	orr	r0, r0, #0x5b				@ Outer-cacheable, WB
	mcr	p15, 0, r0, c2, c0, 0		@ TTB0


	ldr	r0, =0x55555555				@ set all domains to client
	mcr	p15, 0, r0, c3, c0, 0

	/* write to CPUECTLR */
	mrrc	p15, 1, r0, r1, c15		@ Read CPUECTLR
	tst r0, #(0x01 << 6)			@ Test if SMPEN bit is already set
	bne done						@ if SMPEN is set, skip the write
	orr	r0, r0, #(0x01 << 6)		@ set SMPEN bit
	mcrr	p15, 1, r0, r1, c15		@ Write ACTLR
done:

	/* enable mmu, icahce and dcache */
	mrc	p15, 0, r0, c1, c0, 0		@ read SCTLR
	bic	r0, r0, #(0x1 << 13)
	bic	r0, r0, #(0x1 << 1)			@ disable alignment check
	orr	r0, r0, #(0x1 << 12)		@ eanble I-cache
	orr	r0, r0, #(0x1 << 2)			@ enable D-cache
	orr	r0, r0, #(0x1 << 0)			@ enable MMU
	dsb								@ dsb allow the MMU to start up
	mcr	p15, 0, r0, c1, c0, 0		@ enable cache and MMU
	isb								@ isb flush prefetch buffer

	/* initialize L2 cache */

	/* if floating point and simd, Set CPACR */
	mrc	p15, 0, r1, c1, c0, 2		/* read cp access control register (CPACR) into r1 */
	orr	r1, r1, #(0xf << 20)		/* enable full access for cp10 & cp11 */
	mcr	p15, 0, r1, c1, c0, 2		/* write back into CPACR */

	/* enable vfp */
	fmrx	r1, FPEXC				@ read the exception register
	orr		r1,r1, #(0x1 << 30)		@ set VFP enable bit, leave the others in orig state
	fmxr	FPEXC, r1				@ write back the exception register

#if (CONFIG_CPUS_NUM > 1)
	/* only Core 0 calls main() */
	cmp	r8, #1
	bgt	vPortSecondaryStart
#endif

	/* clear bss */
	mov	r0, #0
	ldr	r1, =__bss_start__
	ldr	r2, =__bss_end__
clear_bss:
	str	r0, [r1], #4
	cmp	r1, r2
	ble	clear_bss
	
	/* TODO: data init */

	/* initialize uart, move to c start */
	/* bl __UartInit */
	/*
	ldr	r0, =UART_BASE_ADDRESS
	mov	r1, #0x80					@ UART_LCR = LCR_DLAB
	str	r1, [r0, #0x0c]
	mov	r1, #0x00					@ UART_DLH = 0
	str	r1, [r0, #0x04]
	mov	r1, #0x1b					@ UART_DLL = 0x1b, baud = 57600
	str	r1, [r0, #0x00]
	mov	r1, #0x03					@ UART_LCR = LCR_DLS
	str	r1, [r0, #0x0c]
	*/
	
	/*
	 * Setup and initialize coresight timestamp
	 *
	 * ARMv7a generic timer, which is built in A7/A15, requires
	 * a reference clock counter source. In sheipa3 platform, we
	 * use Corsight timestamp module.
	 */
	
	ldr	r0, =TSGEN_CNTFID0
	ldr	r1, =TSGEN_FREQ
	str	r1, [r0]
	ldr	r0, =TSGEN_CNTCR
	ldr	r1, =TSGEN_ENABLE
	str	r1, [r0]
	
	bl  cstart
	//bl	main
	bl	exit

	/* should never get here */
	.global _exit
_exit:
	b 	_exit

