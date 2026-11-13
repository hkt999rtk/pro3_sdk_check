/*
 * Copyright (c) 2020 Realtek Semiconductor Corp.	All rights reserved.
 *
 * Author: PSP Software Group
 */

#ifndef ARM_GICV3_PRIV_H
#define ARM_GICV3_PRIV_H

#include "cpu.h"

/*
 * GIC Register Interface Base Addresses
 */

#define GIC_GET_RDIST(cpuid)	gic_rdists[cpuid]

/* SGI base is at 64K offset from Redistributor */
#define GICR_SGI_BASE_OFF		0x10000

/* GICR registers offset from RD_base(n) */
#define GICR_CTLR				0x0000
#define GICR_IIDR				0x0004
#define GICR_TYPER				0x0008
#define GICR_STATUSR			0x0010
#define GICR_WAKER				0x0014

/* Register bit definations */

/* GICD_CTLR Interrupt group definitions */
#define GICD_CTLR_ENABLE_G0		0
#define GICD_CTLR_ENABLE_G1NS	1
#define GICD_CTLR_ENABLE_G1S	2

/* GICD_CTLR Register write progress bit */
#define GICD_CTLR_RWP			31

/* GICR_CTLR */
#define GICR_CTLR_RWP			3

/* GICR_WAKER */
#define GICR_WAKER_PS			1
#define GICR_WAKER_CA			2

#endif /* ARM_GICV3_PRIV_H */
