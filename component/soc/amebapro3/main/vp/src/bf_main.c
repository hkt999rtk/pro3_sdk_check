#include <stdio.h>
#include <diag.h>
#include <log.h>

#include <FreeRTOS.h>
#include <task.h>

#include "ameba_soc.h"
#include "os_wrapper.h"

/* rodata */
char *test_str = "BF test string\n";
/* test global data */
int test_num = 20;
/* test bss, should be 0 */
int test_bss;

void bf_test_api(int idx)
{
	/* rodata */
	DiagPrintf("BF test %d\n", idx);

}

void bf_main(void)
{
	DiagPrintf(test_str);
	if (test_bss != 0) {
		DiagPrintf("BSS FAIL\n");
	}
	if (test_num != 20) {
		DiagPrintf("DATA FAIL\n");
	}

	bf_test_api(test_num + 1);
	bf_test_api(test_num + 2);

	bf_test_api(test_bss + 1);
	bf_test_api(test_bss + 2);

}
