#include "isp_reg.h"
#include "ameba_soc.h"
#include "reg_api.h"

uint32_t read_reg(void *base, int offset)
{
	//return isp_read_reg(offset);
	return HAL_READ32(0x60000000, offset);
}

uint32_t read_reg_dbg(void *base, int offset)
{
	//return isp_read_reg(offset);
	uint32_t ret = HAL_READ32(0x60000000, offset);
#if SHOW_LOG
	printf("[%s] 0x%X: 0x%X (%d) \r\n", __FUNCTION__, offset, ret, ret);
#endif
	return ret;
}

void write_reg(void *base, int value, int offset)
{
#if 0
	//printf("[%s] 0x%X: 0x%X (original value) \r\n", __FUNCTION__, offset, read_reg(base, offset));
	//isp_write_reg(value, offset);
	if (1) { //dbg_cnt<dbg_stop) {
		HAL_WRITE32(0x60000000, offset, value);
		//printf("--dbg_cnt--(%d)\r\n", dbg_cnt++);
		//printf("[%s] 0x%X: 0x%X (after setting value 0x%X) \r\n", __FUNCTION__, offset, read_reg(base, offset), value);
	}
#else
	int read_value = read_reg(base, offset);
#if SHOW_LOG
	printf("[%s] 0x%X: 0x%X (%d) (original value) \r\n", __FUNCTION__, offset, read_value, read_value);
#endif
	//isp_write_reg(value, offset);
	if (1) { //dbg_cnt<dbg_stop) {
		HAL_WRITE32(0x60000000, offset, value);
		read_value = read_reg(base, offset);
#if SHOW_LOG
		printf("[%s] 0x%X: 0x%X (%d) (after setting value 0x%X) \r\n", __FUNCTION__, offset, read_value, read_value, value);
#endif
		//printf("--dbg_cnt--(%d)\r\n", dbg_cnt++);
	}
#endif
}
void write_reg_dbg(void *base, int value, int offset)
{
	int read_value = read_reg(base, offset);
#if SHOW_LOG
	printf("[%s] 0x%X: 0x%X (%d) (original value) \r\n", __FUNCTION__, offset, read_value, read_value);
#endif
	//isp_write_reg(value, offset);
	if (1) { //dbg_cnt<dbg_stop) {
		HAL_WRITE32(0x60000000, offset, value);
		read_value = read_reg(base, offset);
#if SHOW_LOG
		printf("[%s] 0x%X: 0x%X (%d) (after setting value 0x%X) \r\n", __FUNCTION__, offset, read_value, read_value, value);
#endif
		//printf("--dbg_cnt--(%d)\r\n", dbg_cnt++);
	}
}

void write_reg_mask(uint32_t base, uint32_t value, uint32_t offset, uint32_t mask)
{
	if (1) { //dbg_cnt<dbg_stop) {
		isp_write_reg_mask(value, offset, mask);
		//printf("--dbg_cnt--(%d)\r\n", dbg_cnt++);
	}
}

void write_reg_mask_dbg(uint32_t base, uint32_t value, uint32_t offset, uint32_t mask)
{
	int read_value = read_reg(base, offset);
#if SHOW_LOG
	printf("[%s] 0x%X: 0x%X (%d) (original value) \r\n", __FUNCTION__, offset, read_value, read_value);
#endif
	if (1) { //dbg_cnt<dbg_stop) {
		read_value = read_reg(base, offset);
		isp_write_reg_mask(value, offset, mask);
#if SHOW_LOG
		printf("[%s] 0x%X: 0x%X (%d) (after setting value 0x%X) \r\n", __FUNCTION__, offset, read_value, read_value, value);
#endif
		//printf("--dbg_cnt--(%d)\r\n", dbg_cnt++);
	}
}
