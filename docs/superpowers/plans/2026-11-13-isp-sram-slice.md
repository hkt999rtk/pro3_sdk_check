# ISP to H.264 SRAM slice comparison

## Approved scope

Reuse the UART video test example and restore the missing vendor addremap runtime.
Compare frame handoff with full Video SRAM slice handoff; both Y and UV must fit,
with no partial SRAM or DRAM fallback. Preserve encoder reference-frame storage.
The latest user instruction requires one firmware and command selection; reboot
between runs is allowed, reflashing between modes is not.

## Completed implementation

1. Restored the vendor runtime functions and unchanged HAL, with provenance in
   `video/addremap/SOURCE.md`. Addremap now builds from source into the build tree.
2. Added paired Y/UV SRAM layout validation, failure propagation and register
   readback before sensor start. Ring depth is four; both blocks use 1920 stride.
3. Added `AT+VIDPIPE=0/1`, query support, default frame mode and a first-run lock.
   After a start attempt, reboot and select the mode again. CH0 H.264 1920x1080@15,
   AINR/NN/preview/recording off. No encoded NAL slice splitting.
4. Built with `isp_uart.conf` and `python ameba.py build -a video_ap`.
   The AP example selection is required in addition to Kconfig. Only the UART
   comparison entry sources are selected, avoiding unrelated optional sinks.

## Verification

- SRAM capacity/overflow and mocked vendor MMIO C checks: passed.
- Four Python-driven C regression tests: passed (encoder stride, fixed FPS,
  UART mode/query/error/lock, paired ISP/encoder command flags).
- Fresh review found stride mismatch and unenforced FPS; reproduced then fixed.
  Focused follow-up review found no issue in command selection/copy/serialization.
- Unified firmware build/link/packaging: passed. Final AP symbols include
  fVIDPIPE, fVIDTEST, fVIDCHECK and the AT table. VP has the restored remap symbols.
- SRAM linker bounds are 0x6F000000–0x6F030000. Y+UV ring uses 184320 bytes.
- AP/NP/FP main disassembly has no Wi-Fi startup calls; FP idle has no Wi-Fi call.
- OTA payload equals the application image. Images, maps, symbols, configuration,
  SHA256SUMS and verification logs are in `build_RTL8735C/isp_uart/`.

## Limits

Wi-Fi libraries remain linked for NP PMC dependencies; initialization is disabled.
Original ISP full-frame DDR allocations remain as remap detection address ranges;
physical handoff targets are SRAM. This is not a zero-DDR-allocation firmware.
No board, decoded-picture or bus-traffic verification was performed. AGENTS.md
was not changed. Preexisting BUILD_LOCAL.md edits are retained separately.
