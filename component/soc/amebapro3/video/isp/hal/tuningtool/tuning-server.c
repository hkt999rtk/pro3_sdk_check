//#include <stdint.h>
//#include <rts_errno.h>
//#include "platform_opts.h"
#include "rts_isp_sensor.h"
#include "rts_isp_tuning.h"
static const char *TAG = "TUNING_SERVER";

#define FN_FULL_IQ "full_iq.bin"
struct isp_tuning_cmd {
	uint32_t		err;
	uint16_t		opcode;
	uint16_t		status;
	uint32_t		addr;
	uint32_t		len;		/* only data */
	union {
		uint8_t	data[0];
		void	*ptr[0];
	};
} __attribute__((packed));

#define CONFIG_HW_VER_RTS3915

#ifdef CONFIG_HW_VER_RTS3915
#define ISP_MCU_MODE 0
#else
#define ISP_MCU_MODE 1
#endif

#ifndef TUNING_SRV_VERSION_MAJOR
#define TUNING_SRV_VERSION_MAJOR	0x0001
#endif
#ifndef TUNING_SRV_VERSION_MINOR
#define TUNING_SRV_VERSION_MINOR	0x0003
#endif
#ifndef TUNING_SRV_VERSION_BUILD
#define TUNING_SRV_VERSION_BUILD	0x0000
#endif

#define dbgprintf(fmt, arg...)				DiagPrintf("[%s]"fmt"\r\n",__func__, ##arg)

#define RTS_RAW_HEADER_LENGTH		128
#include "tuning-server.h"
#include "usb_hal.h"
//#include "hal_video.h"
//#include "flash_api.h"

#define TUNING_ISP_FUNC_GET		0
#define TUNING_ISP_FUNC_ENABLE		1
#define TUNING_ISP_FUNC_DISABLE		2
#define TUNING_IQ_TABLE_GET		0
#define TUNING_IQ_TABLE_SET		1
#define CMD_DATA_SIZE (64*1024)

static int dbg_log_flag = 0x00000000;//0xFFFFFFFF;
#define XMEM_MAX_NUM           (1000)
static unsigned int cus_iqaddr = 0x600000;
static int is_tuning_delay = 0;
static struct isp_tuning_cmd *iq_ptr = NULL;
uint32_t *reg_offset = NULL;
static int g_err_code = 0;
static void (*ext_customized_set_cmd_cb)(void *cmd, unsigned char *reg_buf) = (void (*)(void *, unsigned char *))NULL;
static void (*ext_customized_get_cmd_cb)(void *cmd, unsigned char *reg_buf) = (void (*)(void *, unsigned char *))NULL;
static void (*ext_customized_get_size_cmd_cb)(void *cmd, unsigned char *reg_buf) = (void (*)(void *, unsigned char *))NULL;
extern uint8_t __isp_iq_table_start__[];
int get_full_iq_size(void);

struct tuning_cmd_uvc *g_cmd_uvc = NULL;
unsigned char *g_reg_buf = NULL;
extern unsigned char *reg_buf_ab ;
extern unsigned char *reg_buf_cd ;

//static int send_buffer(struct netconn *conn, char *buf, int len);

static int signal_int_count;
//static int signal_pipe_count;

//iq parameters:
int test_tuning_param(void)
{
	int ret = RTS_ISP_OK;
	struct rts_isp_tuning_param *param;
	//struct rts_isp_tuning_param {
	//	int32_t type; /* enum rts_isp_tuning_param_type */
	//	void *const buf;
	//	uint32_t len;
	//	const uint32_t max_len;
	//};

	ret = rts_isp_tuning_query_param(ISP0, &param);
	if (ret) {
		DiagPrintf("[Error] query algo tuning param fail.\r\n");
		return ret;
	}

	param->type = RTS_ISP_TUNING_PARAM_ALL;
	DiagPrintf("[%s][L:%d] param->type   :%d \r\n",   __func__, __LINE__, param->type);
	DiagPrintf("[%s][L:%d] param->buf    :0x%X \r\n", __func__, __LINE__, param->buf);
	DiagPrintf("[%s][L:%d] param->len    :%d  \r\n",  __func__, __LINE__, param->len);
	DiagPrintf("[%s][L:%d] param->max_len:%d \r\n",   __func__, __LINE__, param->max_len);
	ret = rts_isp_tuning_get_param(ISP0, param);
	DiagPrintf("[%s][L:%d] param->type   :%d \r\n",   __func__, __LINE__, param->type);
	DiagPrintf("[%s][L:%d] param->buf    :0x%X \r\n", __func__, __LINE__, param->buf);
	DiagPrintf("[%s][L:%d] param->len    :%d  \r\n",  __func__, __LINE__, param->len);
	DiagPrintf("[%s][L:%d] param->max_len:%d \r\n",   __func__, __LINE__, param->max_len);
	if (ret) {
		DiagPrintf("[Error] get tuning algo param fail");
		goto out;
	}
out:
	rts_isp_tuning_release_param(ISP0, param);
	return ret;
}

//iq table:
//#include "Complete_set_1000d.h"
int test_tuning_iq_table(void)
{
	int ret;
	struct rts_isp_tuning_iq *tuning_iq;
	//struct rts_isp_tuning_iq {
	//	int32_t type; /* enum rts_isp_tuning_iq_type */
	//	void *const buf;
	//	uint32_t len;
	//	const uint32_t max_len;
	//};

	ret = rts_isp_tuning_query_iq(ISP0, &tuning_iq);
	if (ret) {
		DiagPrintf("[Error] test query tuning iq fail.\r\n");
		return ret;
	}

	tuning_iq->type = RTS_ISP_TUNING_IQ_TABLE_ALL;
	DiagPrintf("[%s][L:%d] tuning_iq->type   :%d \r\n",   __func__, __LINE__, tuning_iq->type);
	DiagPrintf("[%s][L:%d] tuning_iq->buf    :0x%X \r\n", __func__, __LINE__, tuning_iq->buf);
	DiagPrintf("[%s][L:%d] tuning_iq->len    :%d  \r\n",  __func__, __LINE__, tuning_iq->len);
	DiagPrintf("[%s][L:%d] tuning_iq->max_len:%d \r\n",   __func__, __LINE__, tuning_iq->max_len);
	ret = rts_isp_tuning_get_iq(ISP0, tuning_iq);
	DiagPrintf("[%s][L:%d] tuning_iq->type   :%d \r\n",   __func__, __LINE__, tuning_iq->type);
	DiagPrintf("[%s][L:%d] tuning_iq->buf    :0x%X \r\n", __func__, __LINE__, tuning_iq->buf);
	DiagPrintf("[%s][L:%d] tuning_iq->len    :%d  \r\n",  __func__, __LINE__, tuning_iq->len);
	DiagPrintf("[%s][L:%d] tuning_iq->max_len:%d \r\n",   __func__, __LINE__, tuning_iq->max_len);
	if (ret) {
		DiagPrintf("[Error] test get tuning iq fail.\r\n");
		goto out;
	}

#if 0
	DiagPrintf("[%s][L:%d] Complete_set_1000d len= %d\r\n", __func__, __LINE__, tuning_iq->len);
	tuning_iq->type = RTS_ISP_TUNING_IQ_TABLE_ALL;
	DiagPrintf("[%s][L:%d] Complete_set_1000d len= %d\r\n", __func__, __LINE__, tuning_iq->len);
	tuning_iq->len = sizeof(Complete_set_1000d);
	DiagPrintf("[%s][L:%d] Complete_set_1000d len= %d\r\n", __func__, __LINE__, tuning_iq->len);
	memcpy(tuning_iq->buf, Complete_set_1000d, iq_ptr->len);
	DiagPrintf("[%s][L:%d] Complete_set_1000d len= %d\r\n", __func__, __LINE__, tuning_iq->len);
#endif

	ret = rts_isp_tuning_set_iq(ISP0, tuning_iq);
	if (ret) {
		DiagPrintf("[Error] test set tuning iq fail.\r\n");
		goto out;
	}

out:
	rts_isp_tuning_release_iq(ISP0, tuning_iq);
	return ret;
}

//read statis:
int read_statis(void)
{
	int ret;
	int fd;
	int i;
	struct rts_isp_tuning_statis *statis;
	//struct rts_isp_tuning_statis {
	//	int32_t type; /* enum rts_isp_tuning_statis_type */
	//	const void *const buf;
	//	const uint32_t len;
	//};

	ret = rts_isp_tuning_query_statis(ISP0, &statis);
	if (ret) {
		DiagPrintf("[Error] query statis fail.\r\n");
		return ret;
	}
	for (i = RTS_ISP_TUNING_STATIS_ALL; i <= RTS_ISP_TUNING_STATIS_Y; i++) {

		statis->type = i;
		DiagPrintf("[%s][L:%d] statis->type   :%d \r\n",   __func__, __LINE__, statis->type);
		DiagPrintf("[%s][L:%d] statis->buf    :0x%X \r\n", __func__, __LINE__, statis->buf);
		DiagPrintf("[%s][L:%d] statis->len    :%d  \r\n",  __func__, __LINE__, statis->len);
		ret = rts_isp_tuning_get_statis(ISP0, statis);
		if (ret) {
			DiagPrintf("[Error] get statis fail.\r\n");
			rts_isp_tuning_release_statis(ISP0, statis);
			return ret;
		}
		DiagPrintf("[%s][L:%d] statis->type   :%d \r\n",   __func__, __LINE__, statis->type);
		DiagPrintf("[%s][L:%d] statis->buf    :0x%X \r\n", __func__, __LINE__, statis->buf);
		DiagPrintf("[%s][L:%d] statis->len    :%d  \r\n",  __func__, __LINE__, statis->len);
	}
	rts_isp_tuning_release_statis(ISP0, statis);

	return 0;
}

struct rts_tuning_server {
	struct rts_raw_t *praw;
	struct {
		int32_t size;
		uint8_t *pdata;
	} fw_backup;
};

static struct rts_tuning_server gserver;
//struct rtsc_hw_id rts_hw_id;
static int max_w = 2688;
static int max_h = 1520;
static int *g_iq_heap = NULL;

void tuning_set_custom_iq_addr(unsigned int iq_addr)
{
	if (iq_addr == 0) {
		if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_STATUS) {
			dbgprintf("[tuning error] iq address can't be 0.\r\n");
			dbgprintf("[tuning error] iq address keep (0x%08X).\r\n", cus_iqaddr);
		}
		return;
	}
	cus_iqaddr = iq_addr;
	if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_STATUS) {
		dbgprintf("iq fw addr(0x%08X).\r\n", cus_iqaddr);
	}
}

void tuning_set_log_level(int level)
{
	dbg_log_flag = level;
}

void tuning_command_delay(int enable)
{
	is_tuning_delay = enable;
}

void tuning_set_iq_heap(void *iq_heap)
{
	g_iq_heap = iq_heap;
}

#include "vfs.h"
static int is_vfs_init = 0;
int tuning_vfs_init(void)
{
	char *prefix = vfs_get_prefix(VFS_TARGET_FLASH_VFS1);
	int ret = prefix == NULL ? -1 : 0;
	if (ret == 0) {
		RTK_LOGI(TAG, "Tuning VFS-FAT Init Success \n");
		is_vfs_init = 1;
	} else {
		RTK_LOGI(TAG, "Tuning VFS-FAT Init Fail \n");
	}
	return ret;
}
void tuning_vfs_deinit(void)
{
	is_vfs_init = 0;
	return;
}
int tuning_write_file(char *fn, void *buf, int size)
{
	FILE *finfo;
	int ret;
	char path[64];
	char *prefix;
	if (!is_vfs_init) {
		tuning_vfs_init();
	}
	if (!is_vfs_init) {
		return -1;
	}

	prefix = vfs_get_prefix(VFS_TARGET_FLASH_VFS1);
	if (prefix == NULL) {
		is_vfs_init = 0;
		return -1;
	}
	if (snprintf(path, sizeof(path), "%s:%s", prefix, fn) >= (int)sizeof(path)) {
		return -1;
	}
	finfo = fopen(path, "w");
	if (finfo == NULL) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[%s] fopen() failed\r\n", __FUNCTION__);
		return -1;
	}

	ret = fwrite(buf, strlen(buf), 1, finfo);
	if (ret != (int)strlen(buf)) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[%s] fwrite() failed, err is %d\r\n", __FUNCTION__, ret);
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "[%s] fwrite() succeeded, write %d characters:%s\r\n", __FUNCTION__, strlen(buf), buf);
	}

	fclose(finfo);
}

int tuning_read_file(char *fn, void *buf, int size)
{
	FILE *finfo;
	int ret;
	char path[64];
	char *prefix;
	if (!is_vfs_init) {
		tuning_vfs_init();
	}
	if (!is_vfs_init) {
		return -1;
	}

	prefix = vfs_get_prefix(VFS_TARGET_FLASH_VFS1);
	if (prefix == NULL) {
		is_vfs_init = 0;
		return -1;
	}
	if (snprintf(path, sizeof(path), "%s:%s", prefix, fn) >= (int)sizeof(path)) {
		return -1;
	}
	finfo = fopen(path, "rb");
	if (finfo == NULL) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[%s] fopen() failed\r\n", __FUNCTION__);
		return -1;
	}

	int file_len = size;
	ret = fread(buf, file_len, 1, finfo);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "[%s] buf:%s ret=%d\r\n", __FUNCTION__, buf, ret);
	//if (ret != (int)strlen(content)) {
	//	RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[%s] fwrite() failed, err is %d\r\n", __FUNCTION__, ret);
	//} else {
	//}

	fclose(finfo);
}

void tuning_init(void)
{
	tuning_vfs_init();
	if (iq_ptr != NULL) {
		dbgprintf("iq_ptr is not null.");
		return;
	}
	if (reg_offset != NULL) {
		dbgprintf("reg_offset is not null.");
		return;
	}

	iq_ptr = malloc(CMD_DATA_SIZE);
	if (iq_ptr == NULL) {
		dbgprintf("isp_tuning_cmd malloc fail.");
		return;
	}

	reg_offset = (uint32_t *)malloc(XMEM_MAX_NUM * sizeof(uint32_t));
	if (reg_offset == NULL) {
		free(iq_ptr);
		iq_ptr = NULL;
		dbgprintf("reg_offset malloc fail.");
		return;
	}
	return;
}

void tuning_deinit(void)
{
	tuning_vfs_deinit();

	if (iq_ptr != NULL) {
		free(iq_ptr);
	}
	iq_ptr = NULL;

	if (reg_offset != NULL) {
		free(reg_offset);
	}
	reg_offset = NULL;
}

void tuning_set_max_resolution(int width, int height)
{
	max_w = width;
	max_h = height;
}

void tuning_set_custom_cmd_cb(int set_addr_cb, int get_addr_cb, int get_size_addr_cb)
{
	ext_customized_set_cmd_cb      = (void (*)(void *, unsigned char *))set_addr_cb;
	ext_customized_get_cmd_cb      = (void (*)(void *, unsigned char *))get_addr_cb;
	ext_customized_get_size_cmd_cb = (void (*)(void *, unsigned char *))get_size_addr_cb;
}

#if (ISP_MCU_MODE == 1)
static int stop_videostream(void)
{
	dbgprintf("%s: not implement\n", __func__);
	return RTS_OK;
}

static int start_videostream(void)
{
	dbgprintf("%s: not implement\n", __func__);
	return RTS_OK;
}
#endif

static void tuning_cmd_dump(struct tuning_cmd *cmd)
{
	int i;

	if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_STATUS)
		dbgprintf("opcode: %d, status: %d, addr: 0x%x, len: %d\n",
				  cmd->opcode, cmd->status, cmd->addr, cmd->len);
	//for (i = 0; i < (int)cmd->len && i < 128; i++)
	//	dbgprintf("data[%d] %x\n", i, cmd->data[i]);
}
#define ISP_MEM_BASE 0x60000000

static int tuning_xmem(struct tuning_cmd *cmd, unsigned char *reg_buf)
{
	int err = 0;

	uint16_t opcode = cmd->opcode;
	uint32_t len = cmd->len;
	uint32_t offset = cmd->addr;
	int num;
	int i;

	/* isp tuning */
	//test_tuning_param();
	//test_tuning_iq_table();
	//read_statis();

	num = len / 4;
	if (num == 0) {
		num = 1;
	}

	for (i = 0; i < num; i++) {
		reg_offset[i] = offset + i * 4;
	}

	/* iq_ptr->opcode = cmd->opcode;
	iq_ptr->status  = cmd->status;
	iq_ptr->addr    = cmd->addr;
	iq_ptr->len      = cmd->len; */

	struct rts_isp_tuning_reg *regs = malloc(num * sizeof(struct rts_isp_tuning_reg));
	if (opcode == MEM_READ) {

		int *ireg_buf = (int *)reg_buf;
		if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_XMEM) {
			//DiagPrintf("[EXT64_READ] tuning_xmem: \r\n");
		}
#if 0
		for (int i = 0; i < num; i++) {
			*ireg_buf = HAL_READ32(ISP_MEM_BASE, reg_offset[i]);
			if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_XMEM) {
				DiagPrintf("addr:0x%04X  value:0x%08X", reg_offset[i], *ireg_buf);
			}
			ireg_buf++;
		}
#else
		for (int i = 0; i < num; i++) {
			regs[i].offset = reg_offset[i];
			regs[i].mask = 0;
		}
		rts_isp_tuning_read_regs(0, regs, num);
		for (int i = 0; i < num; i++) {
			ireg_buf[i] = regs[i].value;
			DiagPrintf("[%s][L:%d] addr:0x%04X  value:0x%08X(ireg_buf:0x%X) \r\n", __func__, __LINE__, reg_offset[i], ireg_buf[i], ireg_buf);
		}
		if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_XMEM) {
			DiagPrintf("\r\n");
		}
#endif

	} else if (opcode == MEM_WRITE) {

		int *ireg_buf = (int *)reg_buf;
		if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_XMEM) {
			dbgprintf("[EXT64_WRITE] tuning_xmem: ");
		}
#if 0
		for (int i = 0; i < num; i++) {
			*ireg_buf = HAL_READ32(ISP_MEM_BASE, reg_offset[i]);
			if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_XMEM) {
				DiagPrintf("addr:0x%04X  value:0x%08X", reg_offset[i], *ireg_buf);
			}
			DiagPrintf("[%s][L:%d] addr:0x%04X  value:0x%08X \r\n", __func__, __LINE__, reg_offset[i], *ireg_buf);
			ireg_buf++;
		}
		for (int i = 0; i < num; i++) {
			if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_XMEM) {
				DiagPrintf("addr:0x%04X  value:0x%08X ", reg_offset[i], *ireg_buf);
			}

			HAL_WRITE32(ISP_MEM_BASE, reg_offset[i], *ireg_buf);
			ireg_buf++;
		}
#else
		for (int i = 0; i < num; i++) {
			regs[i].offset = reg_offset[i];
			regs[i].value = ireg_buf[i];
			regs[i].mask = 0;
			if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_XMEM) {
				DiagPrintf("addr:0x%04X  value:0x%08X", reg_offset[i], *ireg_buf);
			}
			DiagPrintf("[%s][L:%d] addr:0x%04X  value:0x%08X \r\n", __func__, __LINE__, reg_offset[i], *ireg_buf);
			//ireg_buf++;
		}
		rts_isp_tuning_write_regs(0, regs, num);
#endif


		cmd->len = 0;
	} else {
		err = RTS_RETURN(RTS_E_INVALID_TYPE);
	}
	if (regs) {
		free(regs);
	}
out:
	if (err) {
		dbgprintf("xmem err: %d\n", err);
	}

	return err;
}

#include "rts_isp_ctrl.h"
#include "errno.h"
#define isp_error(fmt, arg...)		printf("[%s][%u]Err"fmt"\r",__func__,__LINE__, ##arg)
int hal_isp_set_ctrl(uint32_t id, int *value)
{
	int ret;
	struct isp_v4l2_control c;
	int final_id;
	DiagPrintf("[%s][L:%d][TUNE_DBG] value=0x%X \r\n", __func__, __LINE__, *value);
	if (!value) {
		return -EINVAL;
	}
	final_id = ISP_V4L2_CID_BASE + id;
	//printf("isp_ctrl 0x%08x id %ld \r\n", final_id, id);
	c.id = final_id;
	c.value = *value;
	ret = rts_isp_v4l2_s_ctrl(0, &c);
	if (ret) {
		isp_error("failed to set ctrl 0x%08lx ret 0x%08x", id, ret);
		return ret;
	}
	*value = c.value;
	DiagPrintf("[%s][L:%d] ret=%d \r\n", __func__, __LINE__, ret);

	return 0;
}
int hal_isp_get_ctrl(uint32_t id, int *value)
{
	int ret;
	struct isp_v4l2_control c;
	int final_id;
	if (!value) {
		return -EINVAL;
	}
	final_id = ISP_V4L2_CID_BASE + id;
	//printf("isp_ctrl 0x%08x id %ld \r\n", final_id, id);
	c.id = final_id;
	c.value = -1;
	ret = rts_isp_v4l2_g_ctrl(0, &c);
	//if (ret) {
	//	isp_error(" failed to get ctrl 0x%08lx ret 0x%08x(%d)\r\n", id, ret, ret);
	//	return ret;
	//}
	*value = c.value;

	return 0;
}

i32 voe_isp_ctrl(uint32_t id, int *value, int set_flag)
{
	int ret;
	//if ( !check_isp_running() ) {
	//	printf("isp is not running \r\n");
	//	return -1;
	//}
	if (set_flag) {
		DiagPrintf("[%s][L:%d][TUNE_DBG] value=0x%X \r\n", __func__, __LINE__, *value);
		ret = hal_isp_set_ctrl(id, value);
		if (ret != 0) {
			printf("voe_isp_ctrl set err 0x%08x \r\n", ret);
			return ret;
		}
		int check_value = -1;
		ret = hal_isp_get_ctrl(id, &check_value);
		DiagPrintf("[%s][L:%d][TUNE_DBG] check_value=0x%X \r\n", __func__, __LINE__, check_value);
	} else {

		ret = hal_isp_get_ctrl(id, value);
		//DiagPrintf("[%s][L:%d][TUNE_DBG] value=0x%X \r\n", __func__, __LINE__, *value);
		if (ret != 0) {
			printf("voe_isp_ctrl get err 0x%08x \r\n", ret);
			return ret;
		}
	}

	return 0;
}

extern int get_control(int id, int *pval);
extern int set_control(int id, int val);

static int tuning_isp_ctrl(struct tuning_cmd *cmd, unsigned char *reg_buf)
{
	//DiagPrintf("[%s][L:%d][TUNE_DBG] reg_buf=0x%X \r\n", __func__, __LINE__, reg_buf);

#if (ISP_MCU_MODE == 0)
	int err = 0;
	uint16_t opcode = cmd->opcode;
	uint32_t len = cmd->len;
	uint32_t offset = cmd->addr - 0x900;

	int *ireg_buf = (int *)reg_buf;
	//DiagPrintf("[%s][L:%d][TUNE_DBG] ireg_buf=0x%X  value=\r\n", __func__, __LINE__, ireg_buf, *ireg_buf);
	if (opcode == ISP_CTRL_READ) {

		int ret = -1;
		int try_cnt = 0;
		*ireg_buf = -1;
		//vTaskDelay(100);
		ret = voe_isp_ctrl(offset, ireg_buf, 0);
		//DiagPrintf("[%s][L:%d][TUNE_DBG] get control value=0x%X \r\n", __func__, __LINE__, *ireg_buf);
		//vTaskDelay(100);
		if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_STATUS) {
			dbgprintf("[EXT64_G] tuning_isp_ctrl: addr:0x%04X  value:0x%08X  try_cnt:%d \r\n", offset, *ireg_buf, try_cnt);
		}
		err = ret;

	} else if (opcode == ISP_CTRL_WRITE) {
		if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_STATUS) {
			dbgprintf("[EXT64_S] tuning_isp_ctrl: addr:0x%04X  value:0x%08X \r\n", offset, *ireg_buf);
		}

		int ret = -1;
		//vTaskDelay(50);
		ret = voe_isp_ctrl(offset, ireg_buf, 1);
		//vTaskDelay(50);

		if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_STATUS) {
			dbgprintf("[EXT64_G] tuning_isp_ctrl: addr:0x%04X  value:0x%08X \r\n", offset, *ireg_buf);
		}
		cmd->len = 0;
		err = ret;
	} else {
		err = RTS_RETURN(RTS_E_INVALID_TYPE);
	}

out:
	if (err) {
		dbgprintf("isp ctrl err: %d\n", err);
	}
#endif

	return err;
}
static int tuning_sensor_i2c_rw(struct tuning_cmd *cmd, unsigned char *reg_buf)
{
	DiagPrintf("[%s][L:%d] opcode : %d\r\n", __func__, __LINE__, cmd->opcode);
	DiagPrintf("[%s][L:%d] status : %d\r\n", __func__, __LINE__, cmd->status);
	DiagPrintf("[%s][L:%d] addr   : %d\r\n", __func__, __LINE__, cmd->addr);
	DiagPrintf("[%s][L:%d] len    : %d\r\n", __func__, __LINE__, cmd->len);
	DiagPrintf("[%s][L:%d] data[0]: %d\r\n", __func__, __LINE__, cmd->data[0]);
	int err = 0;
	uint16_t opcode = cmd->opcode;
	uint16_t addr = cmd->addr & 0xFFFF;
	uint32_t len = cmd->len;

#if (ISP_MCU_MODE == 0)
	struct rts_isp_i2c_reg reg;
	int *ireg_buf = (int *)reg_buf;
	struct rts_isp_i2c_info info;
	info.i2c_id   = 0x36;
	info.addr_len = 2;
	info.data_len = 1;

	reg.addr = addr;
	if (opcode == SENSOR_I2C_READ) {
		reg.data = 0;
		int ret = rts_isp_read_i2c_reg(&info, &reg);
		//err = hal_video_i2c_read(&reg);

		dbgprintf("read addr:0x%04X, data:0x%04X.\r\n", reg.addr, reg.data);
		reg_buf[0] = reg.data & 0xFF;
		reg_buf[1] = reg.data >> 8;

		cmd->len = len;
	} else if (opcode == SENSOR_I2C_WRITE) {
		reg.data = reg_buf[0];
		if (len >= 2) {
			reg.data += (reg_buf[1] << 8);
		}

		dbgprintf("write addr:0x%04X, data:0x%04X.\r\n", reg.addr, reg.data);
		int ret = rts_isp_write_i2c_reg(&info, &reg);
		//err = hal_video_i2c_write(&reg);
		cmd->len = 0;
	} else {
		err = RTS_RETURN(RTS_E_INVALID_TYPE);
	}
#endif

	dbgprintf("err: %d\n", err);
	return err;

}

static int tuning_exec_isp_cmd(struct tuning_cmd_uvc *cmd, unsigned char *data)
{
	return 0;
}

static int __mcu_get_raw_header(struct tuning_cmd *cmd, unsigned char *reg_buf)
{
	int err = RTS_OK;

	uint32_t *ireg_buf = (int *)reg_buf;
	memset(ireg_buf, 0, RTS_RAW_HEADER_LENGTH);
	ireg_buf[1] = max_w;
	ireg_buf[2] = max_h;
	cmd->len = RTS_RAW_HEADER_LENGTH;

	return err;
}




static int tuning_server_version(struct tuning_cmd *cmd, unsigned char *reg_buf)
{
	int i = 0;

	if (SRV_VERSION != cmd->opcode) {
		return RTS_RETURN(RTS_E_INVALID_TYPE);
	}

	unsigned short *puwreg_buf = reg_buf;
	puwreg_buf[i++] = (TUNING_SRV_VERSION_MAJOR);
	puwreg_buf[i++] = (TUNING_SRV_VERSION_MINOR);
	puwreg_buf[i++] = (TUNING_SRV_VERSION_BUILD);

	cmd->len = i * 2;
	return 0;
}



static int tuning_mcu_fw_version(struct tuning_cmd *cmd)
{
	const char *info;
	int ret = RTS_OK;

	info = "not_available.";  //rts_av_get_isp_fw_info();
	cmd->len = sprintf((char *)cmd->data, "%s", info);
	return 0;
}

static int tuning_mcu_ic_name(struct tuning_cmd *cmd, unsigned char *reg_buf)
{
	//test_tuning_iq_table();

	char ic_name[] = "0x3921";
	memcpy(reg_buf, ic_name, strlen(ic_name));
	//if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_STATUS) {
	if (1) {
		dbgprintf("HW ID: %s \r\n", ic_name);
	}
	cmd->len = strlen(ic_name);
	//iq_ptr->len = cmd->len;
	return RTS_OK;
}

//static void set_raw_task(void *pvParameters)
//{
//	int ret = 0;
//	int *params = (int*)pvParameters;
//
//	printf("[%s][L:%d] streaming %d  bayer_type: %d.\r\n", __func__, __LINE__, params[0], params[1]);
//	rts_init_raw(params[1], &gserver);
//	ret = rts_hal_set_raw(params[0], params[1]);
//	vTaskDelete(NULL);
//}
//#include <FreeRTOS.h>
//
//int set_raw(uint8_t streamid, uint32_t fmt)
//{
//	int ret = 0;
//	int params[2] = {streamid, fmt};
//	xTaskCreate(set_raw_task, "set_raw_task", configMINIMAL_STACK_SIZE * 16, params,
//				(portPRIVILEGE_BIT | (configMAX_PRIORITIES / 2)), NULL);
//	return ret;
//}

#define v4l2_fourcc(a, b, c, d)\
	((__u32)(a) | ((__u32)(b) << 8) | ((__u32)(c) << 16) | ((__u32)(d) << 24))
#define V4L2_PIX_FMT_128BIT_3PIXEL	v4l2_fourcc('R', '1', '2', '8')
#define V4L2_PIX_FMT_NV16    v4l2_fourcc('N', 'V', '1', '6') /* 16  Y/CbCr 4:2:2  */
#include "rts_raw.h"
void ISP_SwitchRAWData(int ch, int raw_index, int pix_format);
static int tuning_bayer_mode(struct tuning_cmd *cmd, unsigned char *reg_buf)
{
	int ret = 0;
	unsigned char bayer_index = *reg_buf;
	dbgprintf("streaming %d  bayer_type: %d.\r\n", cmd->addr, bayer_index);
	if (cmd->addr > 3) {
		return RTS_FAIL;
	}
	//if (bayer_index > 0x10) {
	//	return RTS_FAIL;
	//}

	//if (bayer_index!=2 && bayer_index!=4 && bayer_index!=0) {
	//	dbgprintf("bayer_index should be 0, 2 or 4 .\r\n");
	//	return RTS_FAIL;
	//}
	if (bayer_index >= RTS_RAW_SRC_RESERVED) {
		dbgprintf("bayer_index is illegal .\r\n");
		return RTS_FAIL;
	}

	printf("[%s][L:%d] streaming %d  bayer_type: %d.\r\n", __func__, __LINE__, cmd->addr, bayer_index);

	int format = V4L2_PIX_FMT_NV16;
	if (bayer_index > RTS_RAW_SRC_DISABLE) {
		format = V4L2_PIX_FMT_128BIT_3PIXEL;
	}
	//int arg[64] = {bayer_index, format};
	extern void uvcd_pause(void);
	extern void uvcd_resume(void);
	uvcd_pause();
	ISP_SwitchRAWData(0, bayer_index, format);
	//streamoff_by_channel(0);
	//vTaskDelay(200);
	//streamon_by_channel(0, bayer_index, format, 2688, 1520, 1, 30, 1, 1000);
	uvcd_resume();


	return ret;
}


#if (ISP_MCU_MODE == 0)
void print_buf(u8 *buf, int len)
{
	int dLen16 = len / 16;
	unsigned char *pucTemp = (unsigned char *)buf;
	for (int i = 0; i < dLen16; i++) {
		char temp[1024] = {0};
		for (int j = 0; j < 16; j++) {
			DiagPrintf("%s0x%02X,", temp, pucTemp[16 * i + j]);
		}
		DiagPrintf("%s\r\n", temp);
		//vTaskDelay(2);
	}
	//vTaskDelay(10);
	for (int j = 0; j < len % 16; j++) {
		DiagPrintf("0x%02X,", pucTemp[len - len % 16 + j]);
	}
	DiagPrintf("\r\n");
}
static int tuning_set_table(int type, struct isp_tuning_cmd *iq_ptr)
{
	int ret = RTS_ISP_OK;
	if (is_tuning_delay) {
		//vTaskDelay(50);
	}

	if (iq_ptr->opcode == ISP_IQ_TABLE) {
		DiagPrintf("[%s][L:%d] ISP_IQ_TABLE\r\n", __func__, __LINE__);
		struct rts_isp_tuning_iq *tuning_iq;
		ret = rts_isp_tuning_query_iq(ISP0, &tuning_iq);
		if (ret) {
			DiagPrintf("[Error] test query tuning iq fail.\r\n");
			return ret;
		}

		tuning_iq->type = type;
		tuning_iq->len = iq_ptr->len;
		memcpy(tuning_iq->buf, &iq_ptr->data[0], iq_ptr->len);
		DiagPrintf("[%s][L:%d] tuning_iq->type   :%d \r\n",   __func__, __LINE__, tuning_iq->type);
		DiagPrintf("[%s][L:%d] tuning_iq->buf    :0x%X \r\n", __func__, __LINE__, tuning_iq->buf);
		DiagPrintf("[%s][L:%d] tuning_iq->len    :%d  \r\n",  __func__, __LINE__, tuning_iq->len);
		DiagPrintf("[%s][L:%d] tuning_iq->max_len:%d \r\n",   __func__, __LINE__, tuning_iq->max_len);

		print_buf(tuning_iq->buf, tuning_iq->len);

		ret = rts_isp_tuning_set_iq(ISP0, tuning_iq);
		if (ret) {
			DiagPrintf("[Error] Set tuning iq fail ret = 0x%X(%d).\r\n", ret, ret);
			rts_isp_tuning_release_iq(ISP0, tuning_iq);
			return ret;
		}

		rts_isp_tuning_release_iq(ISP0, tuning_iq);

	} else if (iq_ptr->opcode == ISP_ALGO_PARAM) {
		DiagPrintf("[%s][L:%d] ISP_ALGO_PARAM\r\n", __func__, __LINE__);
		struct rts_isp_tuning_param *param;

		ret = rts_isp_tuning_query_param(ISP0, &param);
		if (ret) {
			DiagPrintf("[Error] query algo tuning param fail.\r\n");
			return ret;
		}

		param->type = type;
		param->len = iq_ptr->len;
		memcpy(param->buf, &iq_ptr->data[0], iq_ptr->len);

		ret = rts_isp_tuning_set_param(ISP0, param);

		if (ret) {
			DiagPrintf("[Error] get tuning algo param fail.\r\n");
			rts_isp_tuning_release_param(ISP0, param);
			return ret;
		}
		rts_isp_tuning_release_param(ISP0, param);

	} else if (iq_ptr->opcode == ISP_STATIS) {
		DiagPrintf("[Error] Statis (Set-data is not available).\r\n");
	}

	if (is_tuning_delay) {
		//vTaskDelay(100);
	}
	return ret;
}

static int tuning_get_table(int type, struct isp_tuning_cmd *iq_ptr)
{
	int ret = RTS_ISP_OK;
	if (is_tuning_delay) {
		//vTaskDelay(50);
	}
	//DiagPrintf("[%s][L:%d] type: %d iq_ptr->opcode:%d\r\n", __func__, __LINE__, type, iq_ptr->opcode);
	//case UVC_ISP_IQ_TABLE_SIZE:
	//	cmd.opcode = ISP_IQ_TABLE_SIZE;
	//	break;
	//case UVC_ISP_ALGO_PARAM_SIZE:
	//	cmd.opcode = ISP_ALGO_PARAM_SIZE;
	//	break;
	//case UVC_ISP_STATIS_SIZE:
	//	cmd.opcode = ISP_STATIS_SIZE;
	//	break;

	//DiagPrintf("[%s][L:%d] Start \r\n", __func__, __LINE__);
	if (iq_ptr->opcode == ISP_IQ_TABLE || iq_ptr->opcode == ISP_IQ_TABLE_SIZE) {
		DiagPrintf("[%s][L:%d] ISP_IQ_TABLE\r\n", __func__, __LINE__);
		struct rts_isp_tuning_iq *tuning_iq;
		ret = rts_isp_tuning_query_iq(ISP0, &tuning_iq);
		if (ret) {
			DiagPrintf("[Error] test query tuning iq fail.\r\n");
			return ret;
		}

		tuning_iq->type = type;
		ret = rts_isp_tuning_get_iq(ISP0, tuning_iq);
		if (ret) {
			DiagPrintf("[Error] test get tuning iq fail.\r\n");
			rts_isp_tuning_release_iq(ISP0, tuning_iq);
			return ret;
		}
		DiagPrintf("[%s][L:%d] ISP_IQ_TABLE(Done) ret=%d\r\n", __func__, __LINE__, ret);

		//DiagPrintf("[%s][L:%d] tuning_iq->type   :%d \r\n",   __func__, __LINE__, tuning_iq->type);
		//DiagPrintf("[%s][L:%d] tuning_iq->buf    :0x%X \r\n", __func__, __LINE__, tuning_iq->buf);
		//DiagPrintf("[%s][L:%d] tuning_iq->len    :%d  \r\n",  __func__, __LINE__, tuning_iq->len);
		//DiagPrintf("[%s][L:%d] tuning_iq->max_len:%d \r\n",   __func__, __LINE__, tuning_iq->max_len);

		//Update to tuning-cmd
		iq_ptr->len = tuning_iq->len;
		if (iq_ptr->opcode != ISP_IQ_TABLE_SIZE) {
			memcpy(&iq_ptr->data[0], tuning_iq->buf, tuning_iq->len);
			DiagPrintf("[%s][L:%d] ISP_IQ_TABLE(Copy) len=%d\r\n", __func__, __LINE__, tuning_iq->len);
			//int dFileLen = tuning_iq->len;
			//int dFileLen16 = dFileLen/16;
			//unsigned char *pucTemp = (unsigned char *)tuning_iq->buf;
			//for(int i=0; i<dFileLen16; i++)
			//{
			//	char temp[1024]={0};
			//	for(int j=0; j<16; j++)
			//	{
			//		DiagPrintf("%s0x%02X,", temp, pucTemp[16*i+j]);
			//	}
			//	DiagPrintf("%s\r\n", temp);
			//	vTaskDelay(2);
			//}
			//vTaskDelay(10);
			//for(int j=0; j<dFileLen%16; j++)
			//{
			//	DiagPrintf("0x%02X,", pucTemp[dFileLen-dFileLen%16+j]);
			//}
			//DiagPrintf("\r\n");

		}

		rts_isp_tuning_release_iq(ISP0, tuning_iq);

	} else if (iq_ptr->opcode == ISP_ALGO_PARAM || iq_ptr->opcode == ISP_ALGO_PARAM_SIZE) {
//		DiagPrintf("[%s][L:%d] ISP_ALGO_PARAM\r\n", __func__, __LINE__);
		struct rts_isp_tuning_param *param;
		ret = rts_isp_tuning_query_param(ISP0, &param);
		if (ret) {
			DiagPrintf("[Error] query algo tuning param fail.\r\n");
			return ret;
		}

		param->type = type;
		ret = rts_isp_tuning_get_param(ISP0, param);
		if (ret) {
			DiagPrintf("[Error] get tuning algo param fail.\r\n");
			rts_isp_tuning_release_param(ISP0, param);
			return ret;
		}

		//DiagPrintf("[%s][L:%d] param->type   :%d \r\n",   __func__, __LINE__, param->type);
		//DiagPrintf("[%s][L:%d] param->buf    :0x%X \r\n", __func__, __LINE__, param->buf);
		//DiagPrintf("[%s][L:%d] param->len    :%d  \r\n",  __func__, __LINE__, param->len);
		//DiagPrintf("[%s][L:%d] param->max_len:%d \r\n",   __func__, __LINE__, param->max_len);
		//Update to tuning-cmd
		iq_ptr->len = param->len;
		if (iq_ptr->opcode != ISP_ALGO_PARAM_SIZE) {
			memcpy(&iq_ptr->data[0], param->buf, param->len);
		}

		rts_isp_tuning_release_param(ISP0, param);
	} else if (iq_ptr->opcode == ISP_STATIS || iq_ptr->opcode == ISP_STATIS_SIZE) {
		//DiagPrintf("[%s][L:%d] ISP_STATIS\r\n", __func__, __LINE__);
		struct rts_isp_tuning_statis *statis;
		ret = rts_isp_tuning_query_statis(ISP0, &statis);
		if (ret) {
			DiagPrintf("[Error] query statis fail.\r\n");
			return ret;
		}
		if (type == 0) {
			type = 1;
		}
		statis->type = type;
		ret = rts_isp_tuning_get_statis(ISP0, statis);
		if (ret) {
			DiagPrintf("[Error] get tuning statis fail.\r\n");
			rts_isp_tuning_release_statis(ISP0, statis);
			return ret;
		}

		//DiagPrintf("[%s][L:%d] statis->type   :%d \r\n",   __func__, __LINE__, statis->type);
		//DiagPrintf("[%s][L:%d] statis->buf    :0x%X \r\n", __func__, __LINE__, statis->buf);
		//DiagPrintf("[%s][L:%d] statis->len    :%d  \r\n",  __func__, __LINE__, statis->len);
		//Update to tuning-cmd
		iq_ptr->len = statis->len;
		if (iq_ptr->opcode != ISP_STATIS_SIZE) {
			memcpy(&iq_ptr->data[0], statis->buf, statis->len);
		}

		rts_isp_tuning_release_statis(ISP0, statis);
	}

	if (is_tuning_delay) {
		//vTaskDelay(100);
	}
	//DiagPrintf("[%s][L:%d] Stop \r\n", __func__, __LINE__);

	return ret;

}

static int tuning_get_statis_size(struct tuning_cmd *cmd, unsigned char *reg_buf)
{
	int err = RTS_OK;
	int iq_type = cmd->addr & 0xFFFF;

	iq_ptr->opcode = cmd->opcode;
	iq_ptr->status  = cmd->status;
	iq_ptr->addr    = cmd->addr;
	iq_ptr->len      = cmd->len;

	uint32_t *ireg_buf = (int *)reg_buf;
	if (cmd->len == 0) { /* GET */
		err = tuning_get_table(iq_type, iq_ptr);
		cmd->len = iq_ptr->len;
		*ireg_buf = iq_ptr->len;
		//DiagPrintf("[%s][L:%d] ireg_buf:0x%X, g_reg_buf:0x%X, reg_buf_ab:0x%X \r\n", __func__, __LINE__, ireg_buf, g_reg_buf, reg_buf_ab);
	} else {
		err = RTS_FAIL;
	}

	return err;
}

static int tuning_get_statis(struct tuning_cmd *cmd, unsigned char *reg_buf)
{
	int i;
	int err = RTS_OK;
	int iq_type = cmd->addr & 0xFFFF;

	iq_ptr->opcode = cmd->opcode;
	iq_ptr->status  = cmd->status;
	iq_ptr->addr    = cmd->addr;
	iq_ptr->len      = cmd->len;

	if (cmd->len == 0) { /* GET */
		err = tuning_get_table(iq_type, iq_ptr);
		memcpy(reg_buf, iq_ptr->data, iq_ptr->len);
		if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_TABLE_STA) {
			for (i = 0; i < iq_ptr->len; i++) {
				if (i > 0 && i % 16 == 0) {
					DiagPrintf("\r\n");
				}
				DiagPrintf("0x%02X,", iq_ptr->data[i]);
			}
		}
		if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_TABLE_STA) {
			dbgprintf("\r\tuning_get_statis Len=%d.\r\n", iq_ptr->len);
		}
		cmd->len = iq_ptr->len;
	} else {
		err = RTS_FAIL;
	}

	return err;
}

static int tuning_algo_param_size(struct tuning_cmd *cmd, unsigned char *reg_buf)
{
	int err = RTS_OK;
	int iq_type = cmd->addr & 0xFFFF;

	iq_ptr->opcode = cmd->opcode;
	iq_ptr->status  = cmd->status;
	iq_ptr->addr    = cmd->addr;
	iq_ptr->len      = cmd->len;

	uint32_t *ireg_buf = (int *)reg_buf;
	if (cmd->len == 0) { /* GET */
		err = tuning_get_table(iq_type, iq_ptr);
		cmd->len = iq_ptr->len;
		*ireg_buf = iq_ptr->len;
		DiagPrintf("[%s][L:%d] ireg_buf:0x%X, g_reg_buf:0x%X, reg_buf_ab:0x%X \r\n", __func__, __LINE__, ireg_buf, g_reg_buf, reg_buf_ab);
	} else {
		err = RTS_FAIL;
	}

	return err;
}

static int tuning_algo_param(struct tuning_cmd *cmd, unsigned char *reg_buf)
{
	int i;
	int err = RTS_OK;
	int iq_type = cmd->addr & 0xFFFF;

	iq_ptr->opcode = cmd->opcode;
	iq_ptr->status  = cmd->status;
	iq_ptr->addr    = cmd->addr;
	iq_ptr->len      = cmd->len;

	if (cmd->len == 0) { /* GET */
		err = tuning_get_table(iq_type, iq_ptr);
		memcpy(reg_buf, iq_ptr->data, iq_ptr->len);
		if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_TABLE_PARA) {
			for (i = 0; i < iq_ptr->len; i++) {
				if (i > 0 && i % 16 == 0) {
					DiagPrintf("\r\n");
				}
				DiagPrintf("0x%02X,", iq_ptr->data[i]);
			}
		}
		if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_TABLE_PARA) {
			dbgprintf("\r\nget tuning_algo_param Len=%d.\r\n", iq_ptr->len);
		}
		cmd->len = iq_ptr->len;
	} else { /* SET */


		memcpy(iq_ptr->data, reg_buf, cmd->len);
		iq_ptr->len = cmd->len;
		if (is_tuning_delay) {
			//vTaskDelay(100);
		}
		err = tuning_set_table(iq_type, iq_ptr);
		err = iq_ptr->err;
		if (is_tuning_delay) {
			//vTaskDelay(100);
		}

		if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_TABLE_PARA) {
			dbgprintf("set tuning_algo_param: type %d, len %d %d\n", (int)iq_ptr->addr, (int)iq_ptr->len, err);
		}

		cmd->len = 0;
	}

	return err;
}



static int tuning_iq_table_size(struct tuning_cmd *cmd, unsigned char *reg_buf)
{
	int err = RTS_OK;
	int iq_type = cmd->addr & 0xFFFF;

	iq_ptr->opcode = cmd->opcode;
	iq_ptr->status  = cmd->status;
	iq_ptr->addr    = cmd->addr;
	iq_ptr->len      = cmd->len;

	uint32_t *ireg_buf = (int *)reg_buf;
	if (cmd->len == 0) { /* GET */
		//dbgprintf("iq_ptr->addr=%d   %d.\r\n", iq_ptr->addr, ISP_TUNING_IQ_TABLE_ALL);
		//iq_ptr->addr = ISP_TUNING_IQ_TABLE_ALL;
		//iq_ptr->addr = (enum isp_tuning_iq_type)iq_ptr->addr;
		err = tuning_get_table(iq_type, iq_ptr);
		cmd->len = iq_ptr->len;
		DiagPrintf("[%s][L:%d][TUNE_DBG] err=0x%X  iq_ptr->len=0x%X \r\n", __func__, __LINE__, err, iq_ptr->len);
		*ireg_buf = iq_ptr->len;
		DiagPrintf("[%s][L:%d] ireg_buf:0x%X, g_reg_buf:0x%X, reg_buf_ab:0x%X \r\n", __func__, __LINE__, ireg_buf, g_reg_buf, reg_buf_ab);
		if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_TABLE_IQ) {
			dbgprintf("IQ table size (delay200):%d  %d.\r\n", *ireg_buf, err);
		}
	} else {
		err = RTS_FAIL;
	}

	return err;
}



static int tuning_iq_table(struct tuning_cmd *cmd, unsigned char *reg_buf)
{
	int i;
	int err = RTS_OK;
	int iq_type = cmd->addr & 0xFFFF;

	iq_ptr->opcode = cmd->opcode;
	iq_ptr->status  = cmd->status;
	iq_ptr->addr    = cmd->addr;
	iq_ptr->len      = cmd->len;

	if (cmd->len == 0) { /* GET */
		err = tuning_get_table(iq_type, iq_ptr);
		memcpy(reg_buf, iq_ptr->data, iq_ptr->len);
		if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_TABLE_IQ) {
			for (i = 0; i < iq_ptr->len; i++) {
				if (i > 0 && i % 16 == 0) {
					DiagPrintf("\r\n");
				}
				DiagPrintf("0x%02X,", iq_ptr->data[i]);
			}
		}
		if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_TABLE_IQ) {
			dbgprintf("\r\nget tuning_iq_table Len=%d.\r\n", iq_ptr->len);
		}
		cmd->len = iq_ptr->len;
	} else { /* SET */


		memcpy(iq_ptr->data, reg_buf, cmd->len);
		iq_ptr->len = cmd->len;

		if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_TABLE_IQ) {
			for (i = 0; i < iq_ptr->len; i++) {
				if (i > 0 && i % 16 == 0) {
					DiagPrintf("\r\n");
				}
				DiagPrintf("0x%02X,", iq_ptr->data[i]);
			}
		}
		if (is_tuning_delay) {
			//vTaskDelay(100);
		}
		err = tuning_set_table(iq_type, iq_ptr);
		//err = hal_video_isp_tuning(VOE_ISP_TUNING_SET_IQ, iq_ptr);
		//vTaskDelay(100);
		err = iq_ptr->err;
		dbgprintf("%d\r\n", err);
		dbgprintf("%d\r\n", err);
		dbgprintf("%d\r\n", err);
		if (is_tuning_delay) {
			//vTaskDelay(100);
		}

		if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_TABLE_IQ) {
			dbgprintf("set tuning_iq_table: type %d, len %d %d\n", (int)iq_ptr->addr, (int)iq_ptr->len, err);
		}

		cmd->len = 0;
	}

	return err;
}
#endif

static int mcu_fw_cycle_rootfs(struct tuning_cmd *cmd)
{
	int err = 0, err2 = 0;
	return err;
}


static int tuning_iq_fw_size(struct tuning_cmd *cmd, unsigned char *reg_buf)
{
	int err = RTS_OK;
	int *iregbuf = (int *)reg_buf;
#if 1
	*iregbuf = get_full_iq_size() + 4;
#else
	if (g_iq_heap == NULL || reg_buf == NULL) {
		dbgprintf("g_iq_heap:%p  reg_buf:%p.\r\n", g_iq_heap, reg_buf);
		return RTS_FAIL;
	}
	dbgprintf("size:%d 0x%X.\r\n", *g_iq_heap, *g_iq_heap);

	memcpy(reg_buf, g_iq_heap, sizeof(int));
	*iregbuf += 4;
#endif
	return err;
}

static int tuning_burn_firmware(struct tuning_cmd *cmd, unsigned char *reg_buf)
{
#if (ISP_MCU_MODE == 0)
	if (reg_buf == NULL) {
		return RTS_FAIL;
	}
	dbgprintf(".\r\n");
	if (cmd->len >= TUNING_IQ_MIN_SIZE) {
		int fw_size = 0;
		printf("[tuning_burn_firmware] IQ timestamp: 0x%08X.\r\n", *((int *)(reg_buf + 12)));
		reg_buf[3] = 0xFE;
		tuning_write_file(FN_FULL_IQ, reg_buf, cmd->len + 4);

		tuning_read_file(FN_FULL_IQ, (u8 *) &fw_size, sizeof(int));
		fw_size &= 0x00FFFFFF;
		dbgprintf("IQ FW size(0x%06X): 0x%04X 0x%04X.\r\n", cus_iqaddr, fw_size, *(int *)reg_buf);
	} else {
		dbgprintf("firmware size is small (< %d).\r\n", TUNING_IQ_MIN_SIZE);
	}
#endif

	return RTS_OK;
}
static void tuning_update_errorcode(struct tuning_cmd *cmd, unsigned char *reg_buf)
{
	return;
	extern void usbd_set_errorcode(void *buf);
	usbd_set_errorcode(&g_err_code);
}

extern u8 g_pTuningIQ[256 * 1024];
static int tuning_dump_firmware(struct tuning_cmd *cmd, unsigned char *reg_buf)
{
#if (ISP_MCU_MODE == 0)

	int dsize;
#if 1
	dsize = get_full_iq_size();
	memcpy(reg_buf, &dsize, 4);
	memcpy(reg_buf + 4, get_full_iq(), dsize);
#else
	if (g_iq_heap == NULL || reg_buf == NULL) {
		dbgprintf("[Err:Null Addr]: iq_heap:%p, reg_buf:%p.\r\n", g_iq_heap, reg_buf);
		return RTS_FAIL;
	}
	memcpy(&dsize, g_iq_heap, sizeof(int));
	if (dsize > TUNING_IQ_MAX_SIZE || dsize < TUNING_IQ_MIN_SIZE) {
		dbgprintf("[Err:Wrong Size]: size:0x%X.\r\n", dsize);
		return RTS_FAIL;
	}

	memcpy(reg_buf, ((char *)g_iq_heap), dsize + 4);
#endif
	unsigned int checksum = 0;
	for (int i = 0; i < dsize; i++) {
		checksum += reg_buf[i + 4];
	}
	dbgprintf("checksum: 0x%X.\r\n", checksum);

#endif

	return RTS_OK;
}

int do_tuning(struct tuning_cmd *cmd, unsigned char *reg_buf)
{
	int err = RTS_OK;
	int debug = 1;
	if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_STATUS) {
		debug = 1;
	} else {
		//debug = 0;
	}
	//DiagPrintf("[%s][L:%d][TUNE_DBG] reg_buf=0x%X \r\n", __func__, __LINE__, reg_buf);

	tuning_cmd_dump(cmd);
	//return err;
	//DiagPrintf("%s %d %d\r\n", __FUNCTION__, cmd->opcode, CUSTOMIZED_SET_CMD);
	//DiagPrintf("[%s][%d] cmd->opcode : 0x%X(%d) \r\n",__func__,__LINE__, cmd->opcode , cmd->opcode );
	//DiagPrintf("[%s][%d] cmd->status : 0x%X(%d) \r\n",__func__,__LINE__, cmd->status , cmd->status );
	//DiagPrintf("[%s][%d] cmd->addr   : 0x%X(%d) \r\n",__func__,__LINE__, cmd->addr   , cmd->addr   );
	//DiagPrintf("[%s][%d] cmd->len    : 0x%X(%d) \r\n",__func__,__LINE__, cmd->len    , cmd->len    );
	//DiagPrintf("[%s][%d] cmd->data[0]: 0x%X(%d) \r\n",__func__,__LINE__, cmd->data[0], cmd->data[0]);
	//DiagPrintf("[%s][%d] cmd->data[1]: 0x%X(%d) \r\n",__func__,__LINE__, cmd->data[1], cmd->data[1]);

	switch (cmd->opcode) {
	case MEM_READ:
		if (debug >= 1) {
			DiagPrintf("MEM_READ\r\n");
		}
		err = tuning_xmem(cmd, reg_buf);//--ERR DONE--//
		break;
	case MEM_WRITE:
		if (debug >= 1) {
			DiagPrintf("MEM_WRITE\r\n");
		}
		err = tuning_xmem(cmd, reg_buf);//--ERR DONE--//
		break;
	case ISP_CTRL_READ:
		if (debug >= 1) {
			//dbgprintf("ISP_CTRL_READ\n");
		}
		err = tuning_isp_ctrl(cmd, reg_buf);//--ERR DONE--//
		break;
	case ISP_CTRL_WRITE:
		if (debug >= 1) {
			dbgprintf("ISP_CTRL_WRITE\n");
		}
		err = tuning_isp_ctrl(cmd, reg_buf);//--ERR DONE--//
		break;
	case MCU_FW_BURN:
		if (debug >= 1) {
			dbgprintf("MCU_FW_BURN\n");
		}
		err = tuning_burn_firmware(cmd, reg_buf);//--ERR DONE--//
		break;
	case MCU_FW_RESET:
		if (debug >= 1) {
			dbgprintf("MCU_FW_RESET\n");
		}
		//err = mcu_fw_cycle_rootfs(cmd);//--ERR DONE--//
		break;
	case MCU_GET_RAW_HEADER:
		if (debug >= 1) {
			dbgprintf("MCU_GET_RAW_HEADER\n");
		}
		//err = __mcu_get_raw_header(cmd, reg_buf);//--ERR DONE--//
		break;
	case FW_DUMP:
		if (debug >= 1) {
			dbgprintf("FW_DUMP\n");
		}
		err = tuning_dump_firmware(cmd, reg_buf);//--ERR DONE--//
		break;
	case SRV_VERSION:
		if (debug >= 1) {
			dbgprintf("SRV_VERSION\n");
		}
		//err = tuning_server_version(cmd, reg_buf);//--ERR DONE--//
		break;
	case MCU_FW_VERSION:
		if (debug >= 1) {
			dbgprintf("MCU_FW_VERSION\n");
		}
		err = tuning_mcu_fw_version(cmd);//--ERR DONE--//
		break;
	case MCU_IC_NAME:
		if (debug >= 1) {
			DiagPrintf("MCU_IC_NAME\n");
		}
		err = tuning_mcu_ic_name(cmd, reg_buf);//--ERR DONE--//
		break;
	case BAYER_MODE_CMD:
		if (debug >= 1) {
			dbgprintf("BAYER_MODE_CMD\n");
		}
		err = tuning_bayer_mode(cmd, reg_buf);//--ERR DONE--//
		break;

	case SENSOR_I2C_READ:
	case SENSOR_I2C_WRITE:
		if (debug >= 1) {
			dbgprintf("SENSOR_I2C_READ/SENSOR_I2C_WRITE\n");
		}
		err = tuning_sensor_i2c_rw(cmd, reg_buf);//--ERR DONE--//
		break;

#if (ISP_MCU_MODE == 0)
	case ISP_STATIS_SIZE:
		if (debug >= 1) {
			//dbgprintf("ISP_STATIS_SIZE\n");
		}
		err = tuning_get_statis_size(cmd, reg_buf);//--ERR DONE--//
		break;

	case ISP_ALGO_PARAM_SIZE:
		if (debug >= 1) {
			//dbgprintf("ISP_ALGO_PARAM_SIZE\n");
		}
		err = tuning_algo_param_size(cmd, reg_buf);//--ERR DONE--//
		break;

	case ISP_IQ_TABLE_SIZE:
		if (debug >= 1) {
			dbgprintf("ISP_IQ_TABLE_SIZE\n");
		}
		err = tuning_iq_table_size(cmd, reg_buf);//--ERR DONE--//
		break;

	case ISP_IQ_FW_SIZE:
		if (debug >= 1) {
			dbgprintf("ISP_IQ_FW_SIZE\n");
		}
		err = tuning_iq_fw_size(cmd, reg_buf);//--ERR DONE--//
		break;

	case ISP_STATIS:
		if (debug >= 1) {
			//dbgprintf("ISP_STATIS\n");
		}
		err = tuning_get_statis(cmd, reg_buf);//--ERR DONE--//
		break;

	case ISP_ALGO_PARAM:
		if (debug >= 1) {
			//dbgprintf("ISP_ALGO_PARAM\n");
		}
		err = tuning_algo_param(cmd, reg_buf);//--ERR DONE--//
		break;

	case ISP_IQ_TABLE:
		if (debug >= 1) {
			dbgprintf("ISP_IQ_TABLE\n");
		}
		err = tuning_iq_table(cmd, reg_buf);//--ERR DONE--//
		break;

	case ERRCODE_READ:
		if (debug >= 1) {
			dbgprintf("ERRCODE_READ\n");
		}
		tuning_update_errorcode(cmd, reg_buf);
		break;

	case CUSTOMIZED_SET_CMD:
		if (debug >= 1) {
			dbgprintf("CUSTOMIZED_SET_CMD\n");
		}
		//DiagPrintf("%s %d %d\r\n", __FUNCTION__, cmd->opcode, CUSTOMIZED_SET_CMD);
		if (ext_customized_set_cmd_cb) {
			ext_customized_set_cmd_cb((void *)cmd, reg_buf);
		} else {
			dbgprintf("set_cmd_cb NULL.\r\n");
		}
		break;

	case CUSTOMIZED_GET_CMD:
		if (debug >= 1) {
			dbgprintf("CUSTOMIZED_GET_CMD\n");
		}
		if (ext_customized_get_cmd_cb) {
			ext_customized_get_cmd_cb((void *)cmd, reg_buf);
		} else {
			dbgprintf("get_cmd_cb NULL.\r\n");
		}
		break;
	case CUSTOMIZED_GET_SIZE_CMD:
		if (debug >= 1) {
			dbgprintf("CUSTOMIZED_GET_SIZE_CMD\n");
		}
		if (ext_customized_get_size_cmd_cb) {
			ext_customized_get_size_cmd_cb((void *)cmd, reg_buf);
		} else {
			dbgprintf("get_size_cmd_cb NULL.\r\n");
		}
		break;

#endif

	default:
		if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_STATUS) {
			dbgprintf("unknown cmd\n");
		}
		err = RTS_RETURN(RTS_E_UNKNOWN);
	}

	if (err < 0) {
		cmd->status = (uint16_t)(RTS_ERRNO(err));
		cmd->len = 0;
	} else {
		cmd->status = TUNING_FINISH;
	}
	g_err_code = err;
	if (g_err_code != 0) {
		if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_STATUS) {
			dbgprintf("err code: %d  cmd->opcode:%d.\r\n", g_err_code, cmd->opcode);
		}
	}

	tuning_cmd_dump(cmd);

	return err;
}

int g_isTuningDataReady = 0;
int do_tuning_uvc(struct tuning_cmd_uvc *cmd_uvc, unsigned char *reg_buf)
{
	struct tuning_cmd cmd;
	cmd_uvc = g_cmd_uvc;
	reg_buf = g_reg_buf;
	cmd.opcode = 0xFFFF;
	//DiagPrintf("[%s][L:%d][TUNE_DBG] reg_buf=0x%X \r\n", __func__, __LINE__, reg_buf);

	//if (rts_av_isp_get_status() < RTS_ISP_RUNNING)
	//{
	//	dbgprintf("ISP not inited. Run \"tdrv ispd\" first.\n");
	//	return -1;
	//}

	if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_STATUS) {
		dbgprintf("bySubCmd=0x%02X.\r\n", cmd_uvc->bySubCmd);
	}
//struct tuning_cmd_uvc {
//	BYTE byCmd;
//	BYTE bySubCmd;
//	BYTE byarAddr[2];
//	BYTE byarLen[2];
//	union {
//		BYTE byReserved[2];
//		BYTE byarAddrH[2];
//	};
//} __attribute__((packed));
	//if(cmd_uvc->bySubCmd!=UVC_ISP_ALGO_PARAM_READ && cmd_uvc->bySubCmd!=UVC_ISP_ALGO_PARAM_SIZE
	//&& cmd_uvc->bySubCmd!=UVC_ISP_STATIS          && cmd_uvc->bySubCmd!=UVC_ISP_STATIS_SIZE
	//&& cmd_uvc->bySubCmd!=UVC_ISP_CTRL_READ)
	//{
	//	DiagPrintf("[%s][%d] cmd->byCmd     : 0x%X(%d) \r\n",__func__,__LINE__, cmd_uvc->byCmd    , cmd_uvc->byCmd );
	//	DiagPrintf("[%s][%d] cmd->bySubCmd  : 0x%X(%d) \r\n",__func__,__LINE__, cmd_uvc->bySubCmd , cmd_uvc->bySubCmd );
	//	DiagPrintf("[%s][%d] cmd->byarAddr  : 0x%X(%d) \r\n",__func__,__LINE__, cmd_uvc->byarAddr[1]*256 +cmd_uvc->byarAddr[0]  , cmd_uvc->byarAddr[1]*256 +cmd_uvc->byarAddr[0]);
	//	DiagPrintf("[%s][%d] cmd->byarLen   : 0x%X(%d) \r\n",__func__,__LINE__, cmd_uvc->byarLen[1]*256  +cmd_uvc->byarLen[0]   , cmd_uvc->byarLen[1]*256  +cmd_uvc->byarLen[0]);
	//	DiagPrintf("[%s][%d] cmd->byarAddrH : 0x%X(%d) \r\n",__func__,__LINE__, cmd_uvc->byarAddrH[1]*256+cmd_uvc->byarAddrH[0] , cmd_uvc->byarAddrH[1]*256+cmd_uvc->byarAddrH[0]);
	//}

	switch (cmd_uvc->bySubCmd) {
	case UVC_MEM_READ:
		cmd.opcode = MEM_READ;
		break;
	case UVC_MEM_WRITE:
		cmd.opcode = MEM_WRITE;
		break;
	case UVC_ISP_CTRL_READ:
		cmd.opcode = ISP_CTRL_READ;
		break;
	case UVC_ISP_CTRL_WRITE:
		cmd.opcode = ISP_CTRL_WRITE;
		break;
	case UVC_MCU_ISP_CMD:
		cmd.opcode = MCU_ISP_CMD;
		break;
	case UVC_MCU_GET_RAW_HEADER:
		cmd.opcode = MCU_GET_RAW_HEADER;
		break;
	case UVC_SRV_VERSION:
		cmd.opcode = SRV_VERSION;
		break;
	case UVC_MCU_IC_NAME:
		cmd.opcode = MCU_IC_NAME;
		break;
	case UVC_ISP_BAYER_FORMAT_SET:
		cmd.opcode = BAYER_MODE_CMD;
		break;
	case UVC_SENSOR_I2C_READ:
		cmd.opcode = SENSOR_I2C_READ;
		break;
	case UVC_SENSOR_I2C_WRITE:
		cmd.opcode = SENSOR_I2C_WRITE;
		break;
	case UVC_ISP_IQ_TABLE_READ:
		cmd.len = 0;
		cmd.opcode = ISP_IQ_TABLE;
		break;
	case UVC_ISP_IQ_TABLE_WRITE:
		cmd.opcode = ISP_IQ_TABLE;
		break;
	case UVC_ISP_IQ_FW_READ:
		cmd.len = 0;
		cmd.opcode = FW_DUMP;
		break;
	case UVC_ISP_IQ_FW_WRITE:
		cmd.opcode = MCU_FW_BURN;
		break;
	case UVC_ISP_ALGO_PARAM_READ:
		cmd.len = 0;
		cmd.opcode = ISP_ALGO_PARAM;
		break;
	case UVC_ISP_ALGO_PARAM_WRITE:
		cmd.opcode = ISP_ALGO_PARAM;
		break;
	case UVC_ISP_STATIS:
		cmd.opcode = ISP_STATIS;
		break;
	case UVC_ISP_IQ_TABLE_SIZE:
		cmd.opcode = ISP_IQ_TABLE_SIZE;
		break;
	case UVC_ISP_ALGO_PARAM_SIZE:
		cmd.opcode = ISP_ALGO_PARAM_SIZE;
		break;
	case UVC_ISP_STATIS_SIZE:
		cmd.opcode = ISP_STATIS_SIZE;
		break;
	case UVC_ISP_IQ_FW_SIZE:
		cmd.opcode = ISP_IQ_FW_SIZE;
		break;
	case UVC_ERRCODE_READ:
		cmd.opcode = ERRCODE_READ;
		break;
	case UVC_CUSTOMIZED_SET_CMD:
		cmd.opcode = CUSTOMIZED_SET_CMD;
		break;
	case UVC_CUSTOMIZED_GET_CMD:
		cmd.opcode = CUSTOMIZED_GET_CMD;
		break;
	case UVC_CUSTOMIZED_GET_SIZE_CMD:
		cmd.opcode = CUSTOMIZED_GET_SIZE_CMD;
		break;
	default:
		break;
	}

	if (dbg_log_flag & RTS_TUNING_LOG_MASK_IDX_BUFFER) {
		//unsigned char *ptr = (unsigned char *)cmd_uvc;
		//dbgprintf("[0x%X.0x%X.0x%X]buf 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X.\r\n", TUNING_SRV_VERSION_MAJOR, TUNING_SRV_VERSION_MINOR,
		//		  TUNING_SRV_VERSION_BUILD, ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5], ptr[6], ptr[7]);
		if (cmd.opcode == 0xFFFF) {
			return -1;
		}
	}

	if (cmd.opcode != MCU_ISP_CMD) {
		if (cmd.opcode == MCU_FW_BURN || cmd.opcode == FW_DUMP) {
			uint32_t len1 = (cmd_uvc->byarLen[0] + (cmd_uvc->byarLen[1] << 8));
			uint32_t len2 = (cmd_uvc->byReserved[0] + (cmd_uvc->byReserved[1] << 8));
			cmd.len = (len1 + (len2 << 16));
			//dbgprintf("upload fw-size(4-bytes): %d ", cmd.len);
			//dbgprintf("SubCmd:0x%X  cmd_uvc->byarLen[0,1]: %d %d cmd_uvc->byReserved[0,1]: %d %d ", cmd_uvc->bySubCmd, cmd_uvc->byarLen[0], cmd_uvc->byarLen[1], cmd_uvc->byReserved[0], cmd_uvc->byReserved[1]);
			//dbgprintf("SubCmd:0x%X  cmd_uvc->byarLen[0,1]: %d %d cmd_uvc->byReserved[0,1]: %d %d ", cmd_uvc->bySubCmd, cmd_uvc->byarLen[0], cmd_uvc->byarLen[1], cmd_uvc->byReserved[0], cmd_uvc->byReserved[1]);
			//dbgprintf("SubCmd:0x%X  cmd_uvc->byarLen[0,1]: %d %d cmd_uvc->byReserved[0,1]: %d %d ", cmd_uvc->bySubCmd, cmd_uvc->byarLen[0], cmd_uvc->byarLen[1], cmd_uvc->byReserved[0], cmd_uvc->byReserved[1]);
		} else {
			cmd.len = (cmd_uvc->byarLen[0] + (cmd_uvc->byarLen[1] << 8));
		}
		//cmd.len  = (cmd_uvc->byarLen[0] + (cmd_uvc->byarLen[1] << 8));

		uint32_t addr1 = (cmd_uvc->byarAddr[0] + (cmd_uvc->byarAddr[1] << 8));
		uint32_t addr2 = (cmd_uvc->byarAddrH[0] + (cmd_uvc->byarAddrH[1] << 8));
		cmd.addr = (addr1 + (addr2 << 16)); //byarAddrH
		//cmd.ptr[0] = reg_buf;
		g_isTuningDataReady = 0;
		//DiagPrintf("[%s][%d] do_tuning Start (g_isTuningDataReady:%d). \r\n",__func__,__LINE__, g_isTuningDataReady);
		int ret = do_tuning(&cmd, reg_buf);
		g_isTuningDataReady = 1;
		//DiagPrintf("[%s][%d] do_tuning End   (g_isTuningDataReady:%d). \r\n",__func__,__LINE__, g_isTuningDataReady);
		return ret;
	} else {
		tuning_exec_isp_cmd(cmd_uvc, reg_buf);
		return 0;
	}
}

