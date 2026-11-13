/*
 * Copyright (c) 2020 Realtek Semiconductor Corp.	All rights reserved.
 *
 * Author: PSP Software Group
 */

#include <stdio.h>
#include <stdint.h>

uint32_t UndefinedExceptionAddr;
uint32_t DataAbortAddr;
uint32_t PrefetchAbortAddr;


__attribute__((weak)) void IRQHandler(void)
{
	for (;;);
}

__attribute__((weak)) void FIQHandler(void)
{
	for (;;);
}

#if 0
__attribute__((weak)) void UndefinedExceptionHandler(void)
{
	printf("Address of the undefined instruction 0x%08lx\n", UndefinedExceptionAddr);

	for (;;);
}

__attribute__((weak)) void DataAbortHandler(void)
{
	uint32_t FaultStatus;

	asm volatile("mrc	p15, 0, %0, c5, c0, 0" : "=r"(FaultStatus));

	printf("Data abort with Data Fault Status Register  0x%08lx\n", FaultStatus);
	printf("Address of Instruction causing Data abort 0x%08lx\n", DataAbortAddr);

	for (;;);
}

__attribute__((weak)) void PrefetchAbortHandler(void)
{
	uint32_t FaultStatus;

	asm volatile("mrc	p15, 0, %0, c5, c0, 1" : "=r"(FaultStatus));

	printf("Prefetch abort with Instruction Fault Status Register 0x%08lx\n", FaultStatus);
	printf("Address of Instruction causing Data abort 0x%08lx\n", PrefetchAbortAddr);

	for (;;);
}
#endif
