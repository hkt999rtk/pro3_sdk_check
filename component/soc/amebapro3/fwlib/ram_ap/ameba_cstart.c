#include <string.h>
#include <stdint.h>
#include <ameba_soc.h>
extern void main(void);
extern void prvSetupHardware(void);
extern void LOGUART_Init(void *);

//extern uint8_t _bss_start[];
//extern uint8_t _bss_end[];
extern uint8_t __data_load_addr__[];
extern uint8_t __data_start__[];
extern uint8_t __data_end__[];

__weak void _init(void)
{
	return;
}

int cstart(void)
{
	/* clear BSS */
	//memset(_bss_start, 0, _bss_end - _bss_start);

	/* init data */
	memcpy(__data_start__, __data_load_addr__, __data_end__ - __data_start__);

	prvSetupHardware();

	DiagPrintf("GT clock %dMhz\n", __get_CNTFRQ() / 1000000);

	/* to initial ROM code using global variable */
	if (SYSCFG_OTP_DisBootLog() == FALSE) {
		/* Set log level for diaplaying */
		rtk_log_level_set("*", RTK_LOG_INFO);
	} else {
		rtk_log_level_set("*", RTK_LOG_ERROR);
	}

	/* update delay api */
	DelayClkUpdate(CPU_ClkGet());
	DelayDivUpdate(4);

#if defined (__GNUC__)
	extern void __libc_init_array(void);
	/* Add This for C++ support */
	__libc_init_array();
#endif
	main();

	while (1);
	return 0;
}
