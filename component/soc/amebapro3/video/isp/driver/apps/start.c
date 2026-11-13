#include <freertos/cache.h>

#define __ISR_VECTOR __attribute__((used, section(".isr_vector")))
#define __RESET_FUNC __attribute__((naked, section(".after_vectors")))
#define __DEF_EXC_FUNC __attribute__((weak, section(".after_vectors")))

void ResetISR(void) __RESET_FUNC;
void NMI_Handler(void) __DEF_EXC_FUNC;
void HardFault_Handler(void) __DEF_EXC_FUNC;
void MemManage_Handler(void) __DEF_EXC_FUNC;
void BusFault_Handler(void) __DEF_EXC_FUNC;
void UsageFault_Handler(void) __DEF_EXC_FUNC;
void SecureFault_Handler(void) __DEF_EXC_FUNC;
void SVC_Handler(void) __DEF_EXC_FUNC;
void DebugMon_Handler(void) __DEF_EXC_FUNC;
void PendSV_Handler(void) __DEF_EXC_FUNC;
void SysTick_Handler(void) __DEF_EXC_FUNC;
void interrupt0_handler(void) __DEF_EXC_FUNC;
void interrupt1_handler(void) __DEF_EXC_FUNC;
void interrupt9_handler(void) __DEF_EXC_FUNC;
/*
 * TODO: Add video-in SOC and motion-detection interrupt entries once RTS3921
 * IRQ numbers are confirmed. Steps:
 *  1. Determine IRQ numbers for the capture engine and MD block from the
 *     RTS3921 interrupt controller spec.
 *  2. Update CONFIG_RTS_SOC_IRQ and CONFIG_RTS_MD_IRQ in
 *     drivers/videoin/rts_camera_hw_config.h.
 *  3. Rename rtscam_soc_isr() to the matching interruptN_handler() in
 *     drivers/videoin/rts_camera_soc.c and add its weak declaration here.
 *  4. Rename rts_md_isr() to the matching interruptM_handler() in
 *     drivers/videoin/rts_camera_md.c and add its weak declaration here.
 *  5. Add the new interruptN_handler / interruptM_handler entries to
 *     g_pfnVectors[] at the correct index (16 + IRQ_number).
 */

extern void __StackTop(void);
extern void __StackLimit(void);

void (*const g_pfnVectors[])(void) __ISR_VECTOR = {
	__StackTop,
	ResetISR,
	NMI_Handler,
	HardFault_Handler,
	MemManage_Handler,
	BusFault_Handler,
	UsageFault_Handler,
	SecureFault_Handler,
	0, // Reserved
	0, // Reserved
	0, // Reserved
	SVC_Handler,
	DebugMon_Handler,
	0, // Reserved
	PendSV_Handler,
	SysTick_Handler,
	interrupt0_handler,
	interrupt1_handler,
	0, // Reserved
	0, // Reserved
	0, // Reserved
	0, // Reserved
	0, // Reserved
	0, // Reserved
	0, // Reserved
	interrupt9_handler,
};

static void bss_init(unsigned int *start, unsigned int *end)
{
	while (start < end) {
		*start++ = 0;
	}
}

void ResetISR(void) /* __RESET_FUNC */
{
	extern unsigned int __bss_start__[];
	extern unsigned int __bss_end__[];

	extern void serial_init(void);
	extern int main(void);

	/* need manual setup sp when gdb debugging */
	asm volatile("mov sp, %0" : : "r"(__StackTop));
	asm volatile("msr msplim, %0" : : "r"(__StackLimit));

	icache_enable();
	dcache_enable();
	bss_init(__bss_start__, __bss_end__);
	serial_init();

	main();

	while (1)
		;
}

void NMI_Handler(void) /* __DEF_EXC_FUNC */
{
	while (1) {
	}
}

void HardFault_Handler(void) /* __DEF_EXC_FUNC */
{
	while (1) {
	}
}

void MemManage_Handler(void) /* __DEF_EXC_FUNC */
{
	while (1) {
	}
}

void BusFault_Handler(void) /* __DEF_EXC_FUNC */
{
	while (1) {
	}
}

void UsageFault_Handler(void) /* __DEF_EXC_FUNC */
{
	while (1) {
	}
}

void SecureFault_Handler(void) /* __DEF_EXC_FUNC */
{
	while (1) {
	}
}

void SVC_Handler(void) /* __DEF_EXC_FUNC */
{
	while (1) {
	}
}

void DebugMon_Handler(void) /* __DEF_EXC_FUNC */
{
	while (1) {
	}
}

void PendSV_Handler(void) /* __DEF_EXC_FUNC */
{
	while (1) {
	}
}

void SysTick_Handler(void) /* __DEF_EXC_FUNC */
{
	while (1) {
	}
}

void interrupt0_handler(void) /* __DEF_EXC_FUNC */
{
	while (1) {
	}
}

void interrupt1_handler(void) /* __DEF_EXC_FUNC */
{
	while (1) {
	}
}

void interrupt9_handler(void) /* __DEF_EXC_FUNC */
{
	while (1) {
	}
}
