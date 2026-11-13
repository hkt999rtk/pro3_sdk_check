#ifndef _IRQ_H_
#define _IRQ_H_

#include "platform_autoconf.h"
#include "basic_types.h"
#include "section_config.h"
#include "ameba_vector.h"

enum isp_irq_type {
	ISP_IRQ_FRAME_DONE,
	ISP_IRQ_ISP_EVENT,
	ISP_IRQ_MD,
	ISP_IRQ_OSD0,
	ISP_IRQ_OSD1,
	ISP_IRQ_OSD2,
	ISP_IRQ_MAX,
};

int isp_irq_register(enum isp_irq_type type, void (*isr)(void), u32 priority);
int isp_irq_enable(enum isp_irq_type type);
int isp_irq_disable(enum isp_irq_type type);
int isp_irq_unregister(enum isp_irq_type type);

#endif /* _IRQ_H_ */
