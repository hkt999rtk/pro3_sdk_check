#include "irq.h"
#include "diag.h"
#include "rts_camera_hw_config.h"

static void (*isr_table[ISP_IRQ_MAX])(void);

static u32 isp_irq_generic_wrapper(void *data)
{
	u32 idx = (u32)data;

	if (idx >= ISP_IRQ_MAX || !isr_table[idx]) {
		return 0;
	}

	isr_table[idx]();
	return 0;
}

static IRQn_Type isp_irq_to_irqn(enum isp_irq_type type)
{
	switch (type) {
	case ISP_IRQ_FRAME_DONE:
		return (IRQn_Type)CONFIG_RTS_SOC_IRQ;
	case ISP_IRQ_ISP_EVENT:
		return (IRQn_Type)CONFIG_RTS_ISP_EVENT_IRQ;
	case ISP_IRQ_MD:
		return (IRQn_Type)CONFIG_RTS_ISP_MD_IRQ;
	case ISP_IRQ_OSD0:
		return (IRQn_Type)CONFIG_RTS_OSDI_IRQ0;
	case ISP_IRQ_OSD1:
		return (IRQn_Type)CONFIG_RTS_OSDI_IRQ1;
	case ISP_IRQ_OSD2:
		return (IRQn_Type)CONFIG_RTS_OSDI_IRQ2;
	default:
		return (IRQn_Type) - 1;
	}
}

int isp_irq_register(enum isp_irq_type type, void (*isr)(void), u32 priority)
{
	IRQn_Type irqn;

	if (type >= ISP_IRQ_MAX || !isr) {
		return -1;
	}

	irqn = isp_irq_to_irqn(type);
	if (irqn == (IRQn_Type) - 1) {
		return -1;
	}

	isr_table[type] = isr;
	InterruptRegister(isp_irq_generic_wrapper, irqn, (u32)type, priority);
	return 0;
}

int isp_irq_enable(enum isp_irq_type type)
{
	IRQn_Type irqn = isp_irq_to_irqn(type);

	if (irqn == (IRQn_Type) - 1) {
		return -1;
	}

	if (!isr_table[type]) {
		return -1;
	}

	InterruptEn(irqn, 0);
	return 0;
}

int isp_irq_disable(enum isp_irq_type type)
{
	IRQn_Type irqn = isp_irq_to_irqn(type);

	if (irqn == (IRQn_Type) - 1) {
		return -1;
	}

	InterruptDis(irqn);
	return 0;
}

int isp_irq_unregister(enum isp_irq_type type)
{
	IRQn_Type irqn = isp_irq_to_irqn(type);

	if (irqn == (IRQn_Type) - 1) {
		return -1;
	}

	isr_table[type] = 0;
	InterruptUnRegister(irqn);
	return 0;
}
