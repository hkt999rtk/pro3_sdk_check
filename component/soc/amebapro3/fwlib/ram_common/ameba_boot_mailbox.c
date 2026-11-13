#include <ameba.h>

#if defined(CONFIG_BOOTLOADER)
/* init and mailbox instance are in bootloader */
SECTION(".mailbox")
static u32 boot_mailbox[MB_SIZE];

void BOOT_MAILBOX_Init(void)
{
	BKUP_Write(BKUP_MAILBOX, (u32)boot_mailbox);
	boot_mailbox[0] = MB_SIZE;
}

void BOOT_MAILBOX_Write(u32 index, u32 value)
{
	if (index < MB_SIZE) {
		boot_mailbox[index] = value;
	}
	DCache_CleanInvalidate((u32)&boot_mailbox[0], sizeof(boot_mailbox));
}
#endif

/* only read by application */
u32 BOOT_MAILBOX_Read(u32 index)
{
	u32 *mailbox = (u32 *)BKUP_Read(BKUP_MAILBOX);
	u32 mb_size = mailbox[0];
	if (index < mb_size) {
		return mailbox[index];
	}
	return 0;
}
