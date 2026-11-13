/*------------------------------------------------------------------------------
--        Copyright (c) 2015, VeriSilicon Inc. All rights reserved            --

--                                                                            --
-- This software is confidential and proprietary and may be used only as      --
--   expressly authorized by VeriSilicon in a written licensing agreement.    --
--                                                                            --
--         This entire notice must be reproduced on all copies                --
--                       and may not be removed.                              --
--                                                                            --
--------------------------------------------------------------------------------
-- Redistribution and use in source and binary forms, with or without         --
-- modification, are permitted provided that the following conditions are met:--
-- * Redistributions of source code must retain the above copyright notice,   --
--       this list of conditions and the following disclaimer.                --
-- * Redistributions in binary form must reproduce the above copyright        --
--       notice, this list of conditions and the following disclaimer in the  --
--       documentation and/or other materials provided with the distribution. --
-- * Neither the names of VeriSilicon nor the names of its contributors may be--
--       used to endorse or promote products derived from this software       --
--       without specific prior written permission.                           --
--------------------------------------------------------------------------------
-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"--
-- AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE  --
-- IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE --
-- ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE  --
-- LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR        --
-- CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF       --
-- SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS   --
-- INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN    --
-- CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)    --
-- ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE --
-- POSSIBILITY OF SUCH DAMAGE.                                                --
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
*/

/*------------------------------------------------------------------------------
 *
 *   Table of contents
 *
 *   1. Include headers
 *   2. External compiler flags
 *   3. Module defines
 *
 *------------------------------------------------------------------------------
 */
/*------------------------------------------------------------------------------
 *   1. Include headers
 *------------------------------------------------------------------------------
 */
#include "dev_common_freertos.h"
#include "vcmdswhwregisters.h"

/* NOTE: Don't use ',' in descriptions, because it is used as separator in csv
 * parsing.
 */
const regVcmdField_s asicVcmdRegisterDesc[] = {
#include "vcmdregistertable.h"
};

/*------------------------------------------------------------------------------
 *   2. External compiler flags
 *------------------------------------------------------------------------------
 *
 *------------------------------------------------------------------------------
 *   3. Module defines
 *------------------------------------------------------------------------------
 */

/* Define this to print debug info for every register write.
 *#define DEBUG_PRINT_REGS
 */

/*******************************************************************************
 * Function name   : vcmd_read_reg
 * Description     : Retrieve the content of a hadware register
 *		    Note: The status register will be read after every MB
 *		    so it may be needed to buffer it's content if reading
 *		    the HW register is slow.
 * Return type     : u32
 * Argument        : u32 offset
 *******************************************************************************
 */
u32 vcmd_read_reg(const void *hwregs, u32 offset)
{
	u32 val;

	val = (u32)ioread32((void __iomem *)hwregs + offset);

	PDEBUG("%s 0x%02x --> %08x\n", __func__, offset, val);

	return val;
}

/*******************************************************************************
 * Function name   : vcmd_write_reg
 * Description     : Set the content of a hadware register
 * Return type     : void
 * Argument        : u32 offset
 * Argument        : u32 val
 *******************************************************************************
 */
void vcmd_write_reg(const void *hwregs, u32 offset, u32 val)
{
	iowrite32(val, (void __iomem *)hwregs + offset);

	PDEBUG("%s 0x%02x with value %08x\n", __func__, offset, val);
}

/*------------------------------------------------------------------------------
 *
 *   vcmd_write_register_value
 *
 *   Write a value into a defined register field (write will happens actually).
 *
 *------------------------------------------------------------------------------
 */
void vcmd_write_register_value(const void *hwregs, u32 *reg_mirror,
			       regVcmdName name, u32 value)
{
	int base = asicVcmdRegisterDesc[name].base;

	vcmd_set_reg_mirror(reg_mirror, name, value);
	/* write it into HW registers */
	vcmd_write_reg(hwregs, base, reg_mirror[base / 4]);
}

/*------------------------------------------------------------------------------
 *
 *   vcmd_get_register_value
 *
 *    Get an unsigned value from the ASIC registers
 *
 *------------------------------------------------------------------------------
 */
u32 vcmd_get_register_value(const void *hwregs, u32 *reg_mirror,
			    regVcmdName name)
{
	const regVcmdField_s *field;
	u32 value;

	field = &asicVcmdRegisterDesc[name];

	PDEBUG("field->base < ASIC_VCMD_SWREG_AMOUNT * 4=%d\n",
	       field->base < ASIC_VCMD_SWREG_AMOUNT * 4);

	value = reg_mirror[field->base / 4] =
		vcmd_read_reg(hwregs, field->base);
	value = (value & field->mask) >> field->lsb;

	return value;
}
