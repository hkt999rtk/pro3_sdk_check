/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "atcmd_service.h"


/****************************************************************
AT command process:
	AT+DEMO
	[+DEMO]: OK
****************************************************************/
void at_demo(void *arg)
{
	if (arg) {
		at_printf("\r\n arg len = %d \r\n", strlen((char *)arg));
		at_printf("\r\n arg = %s \r\n", (char *)arg);
	}

	UNUSED(arg);
	at_printf("\r\n%sOK\r\n", "+TEST:");
}


log_item_t at_demo_items[] = {
	{"+DEMO", at_demo, {NULL, NULL}}
};

void print_demo_at(void)
{
	int index;
	int cmd_len = 0;

	cmd_len = sizeof(at_demo_items) / sizeof(at_demo_items[0]);
	for (index = 0; index < cmd_len; index++) {
		at_printf("AT%s\r\n", at_demo_items[index].log_cmd);
	}
}

void at_demo_init(void)
{
	atcmd_service_add_table(at_demo_items, sizeof(at_demo_items) / sizeof(at_demo_items[0]));
}
