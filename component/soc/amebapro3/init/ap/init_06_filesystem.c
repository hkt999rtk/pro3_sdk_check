#include "ameba_soc.h"

#ifdef CONFIG_VFS_ENABLED
#include "vfs.h"

extern uint32_t vfs_ftl_init(void);
extern int vfs_kv_init(void);
#if defined(CONFIG_SUPPORT_SD_CARD) && CONFIG_SUPPORT_SD_CARD
void sd_init_task(void *p)
{
	(void)p;
	int ret = 0;
	ret = vfs_target_register("sd", VFS_TARGET_SD, VFS_RW);
	if (ret == 0) {
		RTK_LOGI("AP_INIT", "VFS-SD-FAT Init Success \n");
	} else {
		RTK_LOGI("AP_INIT", "VFS-SD-FAT Init Fail \n");
	}
	rtos_task_delete(NULL);
}
#endif
void app_filesystem_init(void)
{
	vfs_init();
	vfs_auto_register();
	if (vfs_kv_init() == 0) {
		RTK_LOGI("AP_INIT", "File System Init Success \n");
	} else {
		RTK_LOGE("AP_INIT", "File System Init Fail \n");
	}

#if defined(CONFIG_FTL_ENABLED) && CONFIG_FTL_ENABLED
	vfs_ftl_init();
#endif

	/* SD file system */
#if defined(CONFIG_SUPPORT_SD_CARD) && CONFIG_SUPPORT_SD_CARD
	rtos_task_create(NULL, "sd init", sd_init_task, NULL, 2048, 5);
#endif
}
#endif

__attribute__((constructor))
void app_fs_init(void)
{
#ifdef CONFIG_VFS_ENABLED
	app_filesystem_init();
#endif
}
