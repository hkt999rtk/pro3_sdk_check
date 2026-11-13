#include "ameba_soc.h"

#ifdef CONFIG_VFS_ENABLED
#include "vfs.h"

extern uint32_t vfs_ftl_init(void);
extern int vfs_kv_init(void);
void app_filesystem_init(void)
{
	vfs_init();
	vfs_auto_register();
	if (vfs_kv_init() == 0) {
		RTK_LOGI("INIT", "File System Init Success \n");
	} else {
		RTK_LOGE("INIT", "File System Init Fail \n");
	}

#if defined(CONFIG_FTL_ENABLED) && CONFIG_FTL_ENABLED
	vfs_ftl_init();
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
