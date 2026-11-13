/*
 * Copyright (c) 2020 Realtek Semiconductor Corp.	All rights reserved.
 *
 * Author: PSP Software Group
 */

#include "arm_gic.h"
#include "arm_gic_common_priv.h"
#include "arm_gicv3_priv.h"

#include "sys_io.h"
#include "sheipa.h"

#define BIT(n)  (1UL << (n))
#define ICC_SRE_ELx_SRE		BIT(0)
#define ICC_SRE_ELx_DFB		BIT(1)
#define ICC_SRE_ELx_DIB		BIT(2)
#define ICC_SRE_EL3_EN		BIT(3)

/* Redistributor base addresses for each core */
mem_addr_t gic_rdists[GIC_NUM_CPU_IF];

/*
 * Wait for register write pending
 * TODO: add timed wait
 */
static int gic_wait_rwp(u32_t intid)
{
	u32_t rwp_mask;
	mem_addr_t base;

	if (intid < GIC_SPI_INT_BASE) {
		base = (GIC_GET_RDIST(GET_CPUID) + GICR_CTLR);
		rwp_mask = BIT(GICR_CTLR_RWP);
	} else {
		base = GICD_CTLR;
		rwp_mask = BIT(GICD_CTLR_RWP);
	}

	while (sys_read32(base) & rwp_mask)
		;

	return 0;
}

void arm_gic_irq_set_priority(unsigned int intid,
							  unsigned int prio, unsigned int flags)
{
	u32_t mask = BIT(intid & (GIC_NUM_INTR_PER_REG - 1));
	u32_t idx = intid / GIC_NUM_INTR_PER_REG;
	u32_t shift;
	u32_t val;
	mem_addr_t base = GET_DIST_BASE(intid);

	/* Disable the interrupt */
	sys_write32(mask, ICENABLER(base, idx));
	gic_wait_rwp(intid);

	/* PRIORITYR registers provide byte access */
	sys_write8(prio & GIC_PRI_MASK, IPRIORITYR(base, intid));

	/* Interrupt type config */
	idx = intid / GIC_NUM_CFG_PER_REG;
	shift = (intid & (GIC_NUM_CFG_PER_REG - 1)) * 2;

	val = sys_read32(ICFGR(base, idx));
	val &= ~(GICD_ICFGR_MASK << shift);
	if (flags & IRQ_TYPE_EDGE) {
		val |= (GICD_ICFGR_TYPE << shift);
	}
	sys_write32(val, ICFGR(base, idx));
}

void arm_gic_irq_enable(unsigned int intid)
{
	u32_t mask = BIT(intid & (GIC_NUM_INTR_PER_REG - 1));
	u32_t idx = intid / GIC_NUM_INTR_PER_REG;

	sys_write32(mask, ISENABLER(GET_DIST_BASE(intid), idx));
}

void arm_gic_irq_disable(unsigned int intid)
{
	u32_t mask = BIT(intid & (GIC_NUM_INTR_PER_REG - 1));
	u32_t idx = intid / GIC_NUM_INTR_PER_REG;

	sys_write32(mask, ICENABLER(GET_DIST_BASE(intid), idx));
	/* poll to ensure write is complete */
	gic_wait_rwp(intid);
}

int arm_gic_irq_is_enabled(unsigned int intid)
{
	u32_t mask = BIT(intid & (GIC_NUM_INTR_PER_REG - 1));
	u32_t idx = intid / GIC_NUM_INTR_PER_REG;
	u32_t val;

	val = sys_read32(ISENABLER(GET_DIST_BASE(intid), idx));

	return (val & mask) != 0;
}

unsigned int arm_gic_get_active(void)
{
	int intid;

	/* (Pending -> Active / AP) or (AP -> AP) */
	intid = read_sysreg(ICC_IAR1);

	return intid;
}

void arm_gic_eoi(unsigned int intid)
{
	/* (AP -> Pending) Or (Active -> Inactive) or (AP to AP) nested case */
	write_sysreg(intid, ICC_EOIR1);
}

void arm_gic_raise_softirq(unsigned int cpu, unsigned int irq)
{
	u64_t val;

	if (irq > 15 || cpu > 7) {
		return;
	}

	/* Support target cpu is within cluster 0 now
	 * TODO: convert cpu id to its affinity levels
	 * */
	val = irq << 24 | cpu << 16 | 1 << 0;

	write_sysreg(val, ICC_SGI1R);
}

/*
 * Wake up GIC redistributor.
 * clear ProcessorSleep and wait till ChildAsleep is cleared.
 * ProcessSleep to be cleared only when ChildAsleep is set
 * Check if redistributor is not powered already.
 */
static void gicv3_rdist_enable(unsigned int cpuid)
{
	mem_addr_t rdist = GIC_GET_RDIST(cpuid);

	if (!(sys_read32(rdist + GICR_WAKER) & BIT(GICR_WAKER_CA))) {
		return;
	}

	sys_clear_bit(rdist + GICR_WAKER, GICR_WAKER_PS);
	while (sys_read32(rdist + GICR_WAKER) & BIT(GICR_WAKER_CA))
		;
}

/*
 * Initialize the cpu interface. This should be called by each core.
 */
static void gicv3_cpuif_init(unsigned int cpuid)
{
	u32_t icc_sre;
	u32_t intid;

	mem_addr_t base = GIC_GET_RDIST(cpuid) + GICR_SGI_BASE_OFF;

	/* Disable all sgi ppi */
	sys_write32(0xffffffff, ICENABLER(base, 0));
	/* Any sgi/ppi intid ie. 0-31 will select GICR_CTRL */
	gic_wait_rwp(0);

	/* Clear pending */
	sys_write32(0xffffffff, ICPENDR(base, 0));

	/* Configure all SGIs/PPIs as G1NS.
	 * TODO: G1S or G1NS dependending on FreeRTOS is run in EL1S
	 * or EL1NS respectively.
	 * All interrupts will be delivered as irq
	 */
	sys_write32(0xffffffff, IGROUPR(base, 0));
	sys_write32(0x0, IGROUPMODR(base, 0));

	/*
	 * Configure default priorities for SGI 0:15 and PPI 0:15.
	 */
	for (intid = 0; intid < GIC_SPI_INT_BASE;
		 intid += GIC_NUM_PRI_PER_REG) {
		sys_write32(GIC_INT_DEF_PRI_X4, IPRIORITYR(base, intid));
	}

	/* Configure PPIs as level triggered */
	sys_write32(0, ICFGR(base, 1));

	/*
	 * Check if system interface can be enabled.
	 * 'icc_sre_el3' needs to be configured at 'EL3'
	 * to allow access to 'icc_sre_el1' at 'EL1'
	 * eg: z_arch_el3_plat_init can be used by platform.
	 */
	icc_sre = read_sysreg(ICC_SRE);

	if (!(icc_sre & ICC_SRE_ELx_SRE)) {
		icc_sre = (icc_sre | ICC_SRE_ELx_SRE |
				   ICC_SRE_ELx_DIB | ICC_SRE_ELx_DFB);
		write_sysreg(icc_sre, ICC_SRE);
		icc_sre = read_sysreg(ICC_SRE);

		configASSERT(icc_sre & ICC_SRE_ELx_SRE);
	}

	write_sysreg(GIC_IDLE_PRIO, ICC_PMR);

	/* Allow group1 interrupts */
	write_sysreg(1, ICC_IGRPEN1);
}

/*
 * Consider FreeRTOS in EL1NS.
 */
static void gicv3_dist_init(void)
{
	unsigned int num_ints;
	unsigned int intid;
	unsigned int idx;
	mem_addr_t base = GIC_DIST_BASE;

	num_ints = sys_read32(GICD_TYPER);
	num_ints &= GICD_TYPER_ITLINESNUM_MASK;
	num_ints = (num_ints + 1) << 5;

	/*
	 * Default configuration of all SPIs
	 */
	for (intid = GIC_SPI_INT_BASE; intid < num_ints;
		 intid += GIC_NUM_INTR_PER_REG) {
		idx = intid / GIC_NUM_INTR_PER_REG;
		/* Disable interrupt */
		sys_write32(0xffffffff, ICENABLER(base, idx));
		/* Clear pending */
		sys_write32(0xffffffff, ICPENDR(base, idx));
		/* All SPIs are G1NS and owned by FreeRTOS */
		sys_write32(0xffffffff, IGROUPR(base, idx));
		sys_write32(0, IGROUPMODR(base, idx));

	}
	/* wait for rwp on GICD */
	gic_wait_rwp(GIC_SPI_INT_BASE);

	/* Configure default priorities for all SPIs. */
	for (intid = GIC_SPI_INT_BASE; intid < num_ints;
		 intid += GIC_NUM_PRI_PER_REG) {
		sys_write32(GIC_INT_DEF_PRI_X4, IPRIORITYR(base, intid));
	}

	/* Configure all SPIs as active low, level triggered by default */
	for (intid = GIC_SPI_INT_BASE; intid < num_ints;
		 intid += GIC_NUM_CFG_PER_REG) {
		idx = intid / GIC_NUM_CFG_PER_REG;
		sys_write32(0, ICFGR(base, idx));
	}

	/* enable Group 1 non-secure interrupts */
	sys_set_bit(GICD_CTLR, GICD_CTLR_ENABLE_G1NS);
}

int arm_gic_init_secondary(void)
{
	unsigned int cpuid = GET_CPUID;

	/* Skip RD_base + SGI_base: 128K */
	gic_rdists[cpuid] = GIC_RDIST_BASE + (cpuid << 17);

	gicv3_rdist_enable(cpuid);

	gicv3_cpuif_init(cpuid);

	return 0;
}

int arm_gic_init(void)
{
	gicv3_dist_init();

	gic_rdists[0] = GIC_RDIST_BASE;

	gicv3_rdist_enable(0);

	gicv3_cpuif_init(0);

	return 0;
}
