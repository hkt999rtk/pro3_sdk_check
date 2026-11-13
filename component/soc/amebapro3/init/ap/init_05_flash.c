#include "ameba_soc.h"
#include "flash_api.h"
void flash_load_struct(FLASH_InitTypeDef *para)
{
	FLASH_InitTypeDef *boot_flash_para = (FLASH_InitTypeDef *)BOOT_MAILBOX_Read(MB_IDX_FLASH_PARA);
	memcpy(para, boot_flash_para, sizeof(FLASH_InitTypeDef));
}
__attribute__((constructor))
void flash_init(void)
{
	/* flash 4IO mode */
	extern FLASH_InitTypeDef flash_init_para;
	//FLASH_StructInit(&flash_init_para);
	flash_load_struct(&flash_init_para);
	FLASH_Init(flash_init_para.FLASH_cur_bitmode);
	RTK_LOGI("AP_INIT", "Flash Init (cur_bitmode) done\n");
}
