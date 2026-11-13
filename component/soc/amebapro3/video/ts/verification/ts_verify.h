#include "ameba_soc.h"
#include "ts_hal.h"

#ifndef TS_VERIFY_H
#define TS_VERIFY_H

#define MAX_TS_TEST_CASE 200

#define DDR_DATA_INPUT 1

void ts_convert_if2of(struct ts_info_t ts_info);
void ts_dump_input_param(void);
void ts_check_end(void);
void ts_clear_operation_status(void);

#endif
