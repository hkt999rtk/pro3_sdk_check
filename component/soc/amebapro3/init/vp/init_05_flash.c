#include "ameba_soc.h"
#include "flash_api.h"
extern FLASH_InitTypeDef flash_init_para;
static int flash_load_struct(FLASH_InitTypeDef *para)
{
	FLASH_InitTypeDef *boot_flash_para = (FLASH_InitTypeDef *)BOOT_MAILBOX_Read(MB_IDX_FLASH_PARA);
	/* Validate pointers to avoid NULL dereference (HardFault) */
	if ((para == NULL) || (boot_flash_para == NULL)) {
		RTK_LOGE("VP_INIT", "Invalid flash para (para=%p, boot=%p)\n", (void *)para, (void *)boot_flash_para);
		return -1;
	}
	memcpy(para, boot_flash_para, sizeof(FLASH_InitTypeDef));
	return 0;
}
__attribute__((constructor))
void flash_init(void)
{
	/* Load parameters before touching the flash controller */
	if (flash_load_struct(&flash_init_para) != 0) {
		RTK_LOGE("VP_INIT", "Flash para load failed\n");
		return;
	}
	/* Initialize flash in 4-IO mode */
	FLASH_Init(flash_init_para.FLASH_cur_bitmode);
	RTK_LOGI("VP_INIT", "Flash Init (cur_bitmode) done\n");
}
