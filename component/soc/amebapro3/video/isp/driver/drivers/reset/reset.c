#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <isp_rtos_iomem.h>
#include "reset.h"

enum rtscam_rtos_reset_id { ISP_RESET, MIPI_RESET, ISP_SYSMEM, ZOOM_RESET };
struct rtscam_rtos_reset_control {
	enum rtscam_rtos_reset_id id;
	uint8_t bit;
	uint32_t reg;
};

static struct rtscam_rtos_reset_control reset[4];

int reset_init(void)
{
	// TODO: need implementation
	reset[0].id = ISP_RESET;
	reset[0].bit = 0;
	reset[0].reg = 0;

	reset[1].id = MIPI_RESET;
	reset[1].bit = 0;
	reset[1].reg = 0;

	reset[2].id = ISP_SYSMEM;
	reset[2].bit = 0;
	reset[2].reg = 0;

	reset[3].id = ZOOM_RESET;
	reset[3].bit = 0;
	reset[3].reg = 0;

	return 0;
}

rtscam_rtos_reset_t rtscam_rtos_reset_control_get(const char *id)
{
	if (!strcmp("isp_reset", id)) {
		return &reset[0];
	}
	if (!strcmp("mipi_reset", id)) {
		return &reset[1];
	}
	if (!strcmp("isp-sysmem-up", id)) {
		return &reset[2];
	}
	if (!strcmp("zoom_reset", id)) {
		return &reset[3];
	}
	return NULL;
}

void rtscam_rtos_reset_control_put(rtscam_rtos_reset_t reset)
{
}

int rtscam_rtos_reset_control_reset(rtscam_rtos_reset_t reset)
{
	struct rtscam_rtos_reset_control *reset_ctl = reset;

	if (!reset_ctl) {
		return -EINVAL;
	}
	switch (reset_ctl->id) {
	case ISP_RESET:
	case MIPI_RESET:
	case ZOOM_RESET:
		// TODO: need implementation
		break;
	default:
		break;
	}
	return 0;
}

int rtscam_rtos_reset_control_assert(rtscam_rtos_reset_t reset)
{
	struct rtscam_rtos_reset_control *reset_ctl = reset;

	if (!reset_ctl) {
		return -EINVAL;
	}
	switch (reset_ctl->id) {
	case ISP_SYSMEM:
		// TODO: need implementation
		break;
	default:
		break;
	}
	return 0;
}

int rtscam_rtos_reset_control_deassert(rtscam_rtos_reset_t reset)
{
	struct rtscam_rtos_reset_control *reset_ctl = reset;

	if (!reset_ctl) {
		return -EINVAL;
	}
	switch (reset_ctl->id) {
	case ISP_SYSMEM:
		// TODO: need implementation
		break;
	default:
		break;
	}
	return 0;
}
