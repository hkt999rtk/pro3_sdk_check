# Restored vendor addremap runtime

Source: local reference SDK `pro3_sdk/source/component/soc/amebapro3/video/addremap`
from `Desktop/RL-7013 8735C Tasks` (not the amebapro3v variant).
All seven shipped addremap headers match the reference SDK byte for byte.

- `hal/hal_addremap.c`: copied unchanged.
- `driver/addremap_runtime.c`: ISP/encoder IRQ handlers, `isp_remap_preprocess`,
  `set_isp_remap`, `update_cmpr_hdr_addr`, and `set_enc_remap` extracted from
  `verification/addremap_verify.c`, with their required constants and globals.
  Testbench dependencies and redundant blank lines removed; two `write_reg`
  calls use equivalent `HAL_WRITE32`; one pointer cast and missing printf
  argument corrected. Hardware register values/order retained.
- `slice_sram.h`: local capacity validation, not vendor hardware initialization.

Source SHA-256:

- `hal/hal_addremap.c`: `4217912247f7ecf5abf16d258794bbe56b9d2db2989a27d7235fe74e0ccd9a97`
- `verification/addremap_verify.c`: `30bad06cf1e16023c26a0da6c899a0671c899557f01a2a7f35d16e14804ddeba`
