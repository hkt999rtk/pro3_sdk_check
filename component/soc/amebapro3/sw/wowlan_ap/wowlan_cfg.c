#include "wowlan_ipccfg.h"

static const WOWLANPATTERN wowlan_patterns[] = {
	{"wakeup1", 7, 3},
	{"wakeup2", 7, 3},
	{"wakeup3", 7, 3},
	{"wakeup4", 7, 3},
	{"wakeup5", 7, 3},
	{"wakeup6", 7, 3},
	{"wakeup7", 7, 3},
	{"wakeup8", 7, 3},
};

void wowlan_set_default_parameter(WOWLANCFG *param)
{
	param->dtim = 10;
	param->type = TCP_PROTOCOL_KA;
	param->interval = 240;
	param->retry_interval = 5;
	param->retry_cnt = 12;
	param->pattern_num = sizeof(wowlan_patterns) / sizeof(wowlan_patterns[0]);

	for (int i = 0; i < param->pattern_num; i++) {
		if (param->pattern_num > MAX_WAKEUP_PATTERNS_NUM) {
			break;
		}
		memcpy(param->wakeup_pattern[i], wowlan_patterns[i].pattern, wowlan_patterns[i].len);
		param->pattern_len[i]    = wowlan_patterns[i].len;
		param->pattern_offset[i] = wowlan_patterns[i].offset;
	}
}
