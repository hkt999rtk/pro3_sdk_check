#include <errno.h>
#include <string.h>
#include <isp_rtos_iomem.h>
#include "pinctrl.h"

enum rtscam_rtos_pinctrl_group { DEFAULT, MIPI, DVP };
struct rtscam_rtos_pinctrl_state {
	enum rtscam_rtos_pinctrl_group id;
	uint32_t reg;
};

struct rtscam_rtos_pinctrl_state pinctrl[3];

int pinctrl_state_init(void)
{
	// TODO: need implementation
	pinctrl[0].id = DEFAULT;
	// pinctrl[0].reg = 0xb880011C;

	pinctrl[1].id = MIPI;
	// pinctrl[1].reg = 0xb880011C;

	pinctrl[2].id = DVP;
	// pinctrl[2].reg = 0xb880011C;

	return 0;
}

rtscam_rtos_pinctrl_t rtscam_rtos_pinctrl_lookup_state(const char *id)
{
	if (!strcmp("default", id)) {
		return &pinctrl[0];
	}
	if (!strcmp("mipi", id)) {
		return &pinctrl[1];
	}
	if (!strcmp("dvp", id)) {
		return &pinctrl[2];
	}
	return NULL;
}

int rtscam_rtos_pinctrl_select_state(rtscam_rtos_pinctrl_t state)
{
	struct rtscam_rtos_pinctrl_state *pinctrl = state;

	if (!pinctrl) {
		return -EINVAL;
	}
	switch (pinctrl->id) {
	case DEFAULT:
		// TODO: need implementation
		break;
	case MIPI:
		// TODO: need implementation
		break;
	case DVP:
		// TODO: need implementation
		break;
	default:
		break;
	}
	return 0;
}
