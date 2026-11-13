/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/

#include <stdio.h>
#include "ameba_soc.h"
#include "hal_nn.h"
#include "nn_file_op.h"
#include "nn_sys_api.h"

/**
*  @brief Description of nn_init
*         nn_init is used to do NPU initialization
*
*  @param[in]  void
*
*  @returns void
*/
void nn_init(void)
{
	NN_Init();
}

/**
*  @brief Disable the NPU clocks and functions and clear the initialization state.
*  @note Stop NPU activity and release driver resources before calling.
*        Callers must serialize access and coordinate NPU ownership across cores.
*        The shared fast PCLK remains enabled for AINR and the encoder.
*
*  @returns void
*/
void nn_deinit(void)
{
	NN_DeInit();
}

/**
*  @brief Query the NPU initialization state recorded by this core's HAL.
*  @note Does not check hardware or driver/inference readiness, and does not
*        track changes made by another core or by power management.
*        Callers must serialize access with initialization and deinitialization.
*
*  @returns 1 after nn_init() completes, or 0 initially and after nn_deinit().
*/
int nn_is_inited(void)
{
	return NN_IsInited();
}

#if defined(CONFIG_ARM_CORE_CA32) && (CONFIG_ARM_CORE_CA32 == 1)
/**
*  @brief Store an NN model into the standby-retained PSRAM region.
*         Reads model_path from the file system and writes an
*         nn_retention_model_t header (magic + size) followed by the model
*         binary into __vp_psram_nn_start__. Call from the AP application
*         before entering standby.
*
*  @param[in]  model_path  file path of the model, e.g. "vfs:/xxx.nb"
*
*  @returns 0 on success, -1 on failure (open/read error or not enough room
*           before __vp_psram_nn_end__)
*/
int nn_retention_model_save(const char *model_path)
{
	extern uint8_t __vp_psram_nn_start__[];
	extern uint8_t __vp_psram_nn_end__[];

	void *fd = NULL;
	int ret = -1;

	uint32_t region_size = (uint32_t)__vp_psram_nn_end__ - (uint32_t)__vp_psram_nn_start__;

	fd = nn_f_open((char *)model_path, 0);
	if (fd == NULL) {
		printf("nn_retention_model_save: open %s fail\r\n", model_path);
		return -1;
	}

	nn_f_seek(fd, 0, SEEK_END);
	int model_size = nn_f_tell(fd);
	if (model_size <= 0) {
		printf("nn_retention_model_save: bad model size %d\r\n", model_size);
		goto exit;
	}
	nn_f_seek(fd, 0, SEEK_SET);

	/* header + model must not run past __vp_psram_nn_end__ */
	uint32_t need = sizeof(nn_retention_model_t) + (uint32_t)model_size;
	if (need > region_size) {
		printf("nn_retention_model_save: model too large, need %lu > region %lu\r\n",
			   (unsigned long)need, (unsigned long)region_size);
		goto exit;
	}

	nn_retention_model_t *hdr = (nn_retention_model_t *)__vp_psram_nn_start__;
	/* nn_f_read wraps fread(.., size, 1, ..), so 1 element == full read */
	if (nn_f_read(fd, hdr->model, model_size) <= 0) {
		printf("nn_retention_model_save: read model fail\r\n");
		goto exit;
	}
	hdr->magic = NN_RETENTION_MODEL_MAGIC;
	hdr->model_size = (uint32_t)model_size;

	/* flush to physical PSRAM so the data survives standby retention */
	DCache_CleanInvalidate((uint32_t)__vp_psram_nn_start__, need);

	ret = 0;
exit:
	nn_f_close(fd);
	return ret;
}

/**
*  @brief AT command handler: save an NN model into the standby-retained PSRAM.
*         Usage: NNSAVE <model_path>
*         Example: NNSAVE vfs:/yolov4_tiny_asymu8.nb
*
*  @param[in]  argc  argument count
*  @param[in]  argv  argv[0] is the model file path
*
*  @returns _TRUE on success, _FALSE on failure
*/
u32 CmdNpuRetentionModelSave(IN u16 argc, IN u8 *argv[])
{
	if (argc < 1 || argv[0] == NULL) {
		printf("Usage: NNSAVE <model_path>\r\n");
		printf("  e.g. NNSAVE vfs:/yolov4_tiny_asymu8.nb\r\n");
		return _FALSE;
	}

	const char *model_path = (const char *)argv[0];
	printf("nn_retention_model_save: saving %s\r\n", model_path);
	if (nn_retention_model_save(model_path) != 0) {
		printf("nn_retention_model_save: failed\r\n");
		return _FALSE;
	}
	printf("nn_retention_model_save: done\r\n");
	return _TRUE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE nn_retention_cmd[] = {
	{
		"NNSAVE", CmdNpuRetentionModelSave
	},
};
#endif
