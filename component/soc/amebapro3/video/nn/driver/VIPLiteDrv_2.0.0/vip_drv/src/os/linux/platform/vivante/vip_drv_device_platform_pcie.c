/****************************************************************************
*
*    The MIT License (MIT)
*
*    Copyright (c) 2017 - 2024 Vivante Corporation
*
*    Permission is hereby granted, free of charge, to any person obtaining a
*    copy of this software and associated documentation files (the "Software"),
*    to deal in the Software without restriction, including without limitation
*    the rights to use, copy, modify, merge, publish, distribute, sublicense,
*    and/or sell copies of the Software, and to permit persons to whom the
*    Software is furnished to do so, subject to the following conditions:
*
*    The above copyright notice and this permission notice shall be included in
*    all copies or substantial portions of the Software.
*
*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
*    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
*    DEALINGS IN THE SOFTWARE.
*
*****************************************************************************
*
*    The GPL License (GPL)
*
*    Copyright (C) 2017 - 2024 Vivante Corporation
*
*    This program is free software; you can redistribute it and/or
*    modify it under the terms of the GNU General Public License
*    as published by the Free Software Foundation; either version 2
*    of the License, or (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*****************************************************************************
*
*    Note: This software is released under dual MIT and GPL licenses. A
*    recipient may use this file under the terms of either the MIT license or
*    GPL License. If you wish to use only one license not the other, you can
*    indicate your decision by deleting one of the above license notices in your
*    version of this file.
*
*****************************************************************************/

#include "vip_drv_device_driver.h"
#include "vip_drv_device_platform_config_pcie.h"

#define MAX_PCIE_BAR    6
vip_uint32_t axi_sram_size = 0;

#if defined (USE_LINUX_PCIE_DEVICE)
vip_physical_t bar_addr[MAX_PCIE_BAR] = {0};
vip_uint64_t bar_size[MAX_PCIE_BAR] = {0};
#endif

#if defined (USE_LINUX_PCIE_DEVICE)
static const struct pci_device_id vivpcie_ids[] = {
  {
    .class       = 0x000000,
    .class_mask  = 0x000000,
    .vendor      = 0x10ee,
    .device      = 0x7012,
    .subvendor   = PCI_ANY_ID,
    .subdevice   = PCI_ANY_ID,
    .driver_data = 0
  },
  {
    .class       = 0x000000,
    .class_mask  = 0x000000,
    .vendor      = 0x10ee,
    .device      = 0x7014,
    .subvendor   = PCI_ANY_ID,
    .subdevice   = PCI_ANY_ID,
    .driver_data = 0
  },
  { /* End: all zeroes */ }
};

MODULE_DEVICE_TABLE(pci, vivpcie_ids);

int query_pcie_bar_nfo(
    struct pci_dev *pdev,
    vip_uint64_t *bar_addr,
    vip_uint64_t *bar_size,
    vip_uint32_t bar_num
    )
{
    vip_uint32_t addr = 0;
    vip_uint32_t size = 0;
    vip_uint32_t is_64_bit = 0;
    vip_uint64_t addr64, size64;

    /* Read the bar address */
    if (pci_read_config_dword(pdev, PCI_BASE_ADDRESS_0 + bar_num * 0x4, &addr) < 0)
        return -1;

    /* Read the bar size */
    if (pci_write_config_dword(pdev, PCI_BASE_ADDRESS_0 + bar_num * 0x4, 0xffffffff) < 0)
        return -1;

    if (pci_read_config_dword(pdev, PCI_BASE_ADDRESS_0 + bar_num * 0x4, &size) < 0)
        return -1;

    /* Write back the bar address */
    if (pci_write_config_dword(pdev, PCI_BASE_ADDRESS_0 + bar_num * 0x4, addr) < 0)
        return -1;

    /* The bar is not working properly */
    if (size == 0xffffffff)
        return -1;

    if ((size & PCI_BASE_ADDRESS_MEM_TYPE_MASK) == PCI_BASE_ADDRESS_MEM_TYPE_64)
        is_64_bit = 1;

    addr64 = addr;
    size &= 0xfffffff0;
    size  = ~size;
    size += 1;
    size64 = size;

    if (is_64_bit) {
        /* Read the bar address */
        if (pci_read_config_dword(pdev, PCI_BASE_ADDRESS_0 + (bar_num + 1) * 0x4, &addr) < 0)
            return -1;

        /* Read the bar size */
        if (pci_write_config_dword(pdev, PCI_BASE_ADDRESS_0 + (bar_num + 1) * 0x4, 0xffffffff) < 0)
            return -1;

        if (pci_read_config_dword(pdev, PCI_BASE_ADDRESS_0 + (bar_num + 1) * 0x4, &size) < 0)
            return -1;

        /* Write back the bar address */
        if (pci_write_config_dword(pdev, PCI_BASE_ADDRESS_0 + (bar_num + 1) * 0x4, addr) < 0)
            return -1;
        if (0xffffffff == size) {
            size = 0;
        }

        addr64 |= ((vip_uint64_t)addr << 32);

        size &= 0xfffffff0;
        size  = ~size;
        size += 1;
        size64 |= ((vip_uint64_t)size << 32);
    }

    addr64 &= ~0xfULL;

    PRINTK("vipcore, BAR[%d] addr=0x%"PRIx64", size=0x%"PRIx64", is_64_bit=%d\n",
            bar_num, addr64, size64, is_64_bit);

    *bar_addr = addr64;
    *bar_size = size64;

    return 0;
}
#endif

/*
@brief convert CPU physical to VIP physical.
@param cpu_physical. the physical address of CPU domain.
*/
vip_physical_t vipdrv_drv_get_vipphysical(
    vip_physical_t cpu_physical
    )
{
    vip_physical_t vip_physical = 0;
#if defined (USE_LINUX_PCIE_DEVICE)
    vip_physical_t cpu_axi_address = bar_addr[1] + 0x800000;
#else
    vip_physical_t cpu_axi_address = AXI_SRAM_BASE_ADDRESS;
#endif
    if ((cpu_physical >= cpu_axi_address) && (cpu_physical < (cpu_axi_address + axi_sram_size))) {
        /* AXI address need covert */
        vip_physical = cpu_physical - cpu_axi_address + AXI_SRAM_BASE_ADDRESS;
        PRINTK("address is AXI range cpu_physical=0x%llx, vip_physical=0x%llx.\n", cpu_physical, vip_physical);
    }
    else {
        vip_physical = cpu_physical;
    }
    if (vip_physical != cpu_physical) {
        PRINTK("get vip_physical=0x%llx, cpu_physical=0x%llx\n", vip_physical, cpu_physical);
    }

    return vip_physical;
}

/*
@brief convert VIP physical to CPU physical.
@param vip_physical. the physical address of VIP domain.
*/
vip_physical_t vipdrv_drv_get_cpuphysical(
    vip_physical_t vip_physical
    )
{
    vip_physical_t cpu_physical = 0;
#if defined (USE_LINUX_PCIE_DEVICE)
    vip_physical_t cpu_axi_address = bar_addr[1] + 0x800000;
#else
    vip_physical_t cpu_axi_address = AXI_SRAM_BASE_ADDRESS;
#endif

    if ((vip_physical >= AXI_SRAM_BASE_ADDRESS) && (vip_physical < (AXI_SRAM_BASE_ADDRESS + axi_sram_size))) {
        cpu_physical = vip_physical - AXI_SRAM_BASE_ADDRESS + cpu_axi_address;
        PRINTK("address is AXI range cpu_physical=0x%llx, vip_physical=0x%llx.\n", cpu_physical, vip_physical);
    }
    else {
        cpu_physical = vip_physical;
    }

    if (vip_physical != cpu_physical) {
        PRINTK("vipcore: get cpu_physical=0x%llx, vip_physical=0x%llx\n", cpu_physical, vip_physical);
    }

    return cpu_physical;
}

/*
@brief 1. platfrom(vendor) initialize. prepare environmnet for running VIP hardware.
       2. initialzie linux platform device.
       3. this function should be called before probe device/driver.
          so many variables not initialize in kdriver. such as kdriver->pdev.
@param pdrv vip drvier device object.
*/
vip_int32_t vipdrv_drv_platform_init(
    vipdrv_driver_t *kdriver
    )
{
#if defined (USE_LINUX_PCIE_DEVICE)
    kdriver->pdrv->id_table = vivpcie_ids;
#endif
    return 0;
}

/*
@brief 1. platfrom(vendor) un-initialize.
       2. uninitialzie linux platform device.
       have removed device/driver, so kdriver->pdev can't be used in this function.
@param pdrv vip drvier device object.
*/
vip_int32_t vipdrv_drv_platform_uninit(
    vipdrv_driver_t *kdriver
    )
{
    return 0;
}

/*
@brief adjust parameters of vip devices. such as irq, SRAM, video memory heap.
        You can overwrite insmod command line in vipdrv_drv_adjust_param() function.
        Verisilicon don't overwrite insmod command line parameters.
        this function called after probe device/driver, so kdriver->pdev can be used.
@param kdriver, vip device object.
*/
vip_int32_t vipdrv_drv_adjust_param(
    vipdrv_driver_t *kdriver
    )
{
    vip_int32_t ret = 0;
    vip_uint32_t core = 0;
#if defined (USE_LINUX_PCIE_DEVICE)
    vip_uint32_t i = 0;
    vip_uint32_t read = 0;
    struct pci_dev *pdev = kdriver->pdev;

    pci_read_config_dword(pdev, 0, &read);
    PRINTK("vipcore, device id 0x%08x, ==> vendor id=0x%04x, device id=0x%04x\n",
            read, read & 0xFFFF,  (read >> 16) & 0xFFFF);
    PRINTK("vipcore, PCI irq num=%d\n", pdev->irq);
    pci_read_config_dword(pdev, 0x3C, &read);
    PRINTK("vipcore, PCI info 0x%08x\n", read);
    for (i = 0; i < MAX_PCIE_BAR; i++) {
        query_pcie_bar_nfo(pdev, &bar_addr[i], &bar_size[i], i);
        kdriver->pci_bar_base[i] = bar_addr[i];
        kdriver->pci_bar_size[i] = bar_size[i];
    }
#endif
#if vpmdENABLE_VIDEO_MEMORY_HEAP
    /* video memory heap base physical address */
    if (0 == kdriver->cpu_physical[0]) {
        kdriver->cpu_physical[0] = VIDEO_MEMORY_HEAP_BASE_ADDRESS;
    }

    /* video memory heap size */
    if (0 == kdriver->heap_infos[0].vip_memsize) {
        kdriver->heap_infos[0].vip_memsize = VIDEO_MEMORY_HEAP_SIZE;
    }

    if (0 == vipdrv_os_strcmp("", kdriver->heap_infos[0].name)) {
        /* the reserved memory is comes from DTS at boot time */
        kdriver->heap_infos[0].name = ALLOCATOR_NAME_HEAP_EXCLUSIVE;
    }
#endif
    /* AXI SRAM base physical address */
    if (0 == kdriver->axi_sram_base) {
        kdriver->axi_sram_base = AXI_SRAM_BASE_ADDRESS;
    }

    axi_sram_size = 0;

    /* VIP SRAM base physical address */
    if (0 == kdriver->vip_sram_base) {
        kdriver->vip_sram_base = VIP_SRAM_BASE_ADDRESS;
    }

    for(core = 0; core < vpmdCORE_COUNT; core++) {
        /* the size of AXI-SRAM */
        if (0 == kdriver->axi_sram_size[core]) {
            kdriver->axi_sram_size[core] = AXI_SRAM_SIZE[core];
        }
        axi_sram_size += kdriver->axi_sram_size[core];

        /* IRQ line */
        if (0 == kdriver->irq_line[core]) {
            kdriver->irq_line[core] = IRQ_LINE_NUMBER[core];
        }

        /* VIP AHB register memory base physical address */
        if (0 == kdriver->vip_reg_phy[core]) {
            kdriver->vip_reg_phy[core] = AHB_REGISTER_BASE_ADDRESS[core];
        }

        /* VIP AHB register memory size */
        if (0 == kdriver->vip_reg_size[core]) {
            kdriver->vip_reg_size[core] = AHB_REGISTER_SIZE[core];
        }

        #if defined (USE_LINUX_PCIE_DEVICE)
        /* the index of pci bars */
        if (0 == kdriver->pci_bars[core]) {
            kdriver->pci_bars[core] = PCI_BARS[core];
        }
        kdriver->vip_reg_phy[core] = kdriver->pci_bar_base[kdriver->pci_bars[core]] + kdriver->reg_offset[core];
        #endif
    }
    kdriver->max_core_count = vpmdCORE_COUNT;

    /* core number for vip */
    if (0 == kdriver->core_count) {
        kdriver->core_count = vpmdCORE_COUNT;
    }
    /* caculate logical device */
    {
        vip_uint32_t  iter = 0, core_num = 0;
        for (iter = 0; iter < vpmdCORE_COUNT; iter++) {
            kdriver->device_core_number[iter] = LOGIC_DEVICES_WITH_CORE[iter];
            core_num += LOGIC_DEVICES_WITH_CORE[iter];
        }
        if (core_num < vpmdCORE_COUNT) {
            kdriver->core_count = core_num;
        }
    }

    kdriver->sys_heap_size = SYSTEM_MEMORY_HEAP_SIZE;

    return ret;
}

/*
@brief release resources created in vipdrv_drv_adjust_param if neeed.
        this function called before remove device/driver, so kdriver->pdev can be used.
@param kdriver, vip device object.
*/
vip_int32_t vipdrv_drv_unadjust_param(
    vipdrv_driver_t *kdriver
    )
{
    return 0;
}

/*
@brief customer SOC initialize. power on and rise up clock for VIP hardware
@param kdriver, vip device object.
*/
vip_int32_t vipdrv_drv_device_init(
    vipdrv_driver_t *kdriver,
    vip_uint32_t core
    )
{
    PRINTK_I("vipdrv_drv_device_init..power on core%d.\n", core);
    /* power on VIP */

    /* make sure power stable before this function return */
    vipdrv_os_delay(2);

    return 0;
}

/*
@brief customer SOC un-initialize. power off and cut down clock.
@param kdriver, vip device object.
*/
vip_int32_t vipdrv_drv_device_uninit(
    vipdrv_driver_t *kdriver,
    vip_uint32_t core
    )
{
     PRINTK_I("vipdrv_drv_device_uninit.. power off core%d\n", core);
     /* power off all cores */
     return 0;
}
