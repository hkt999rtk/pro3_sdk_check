# ISP → H.264 frame / SRAM slice comparison

This reuses the UART video test tool. One firmware supports both modes and defaults to CH0 only,
H.264, 1920×1080 at 15 fps, GOP 15, 2 Mbit/s VBR. AINR, NN, preview and recording
are disabled. Encoder reference-frame and compressed-output allocations are
unchanged. No H.264 NAL slice splitting is enabled.

## Handoff memory

| UART selection | ISP → encoder handoff |
| --- | --- |
| `AT+VIDPIPE=0` | Existing complete NV12 frame buffers |
| `AT+VIDPIPE=1` | Four ring slots, each 16 Y lines + 8 UV lines, entirely in Video SRAM |

At 1920-byte stride the slice ring uses 184320 bytes (180 KiB):

- Y: `0x6F000000`–`0x6F01DFFF`, 122880 bytes.
- UV: `0x6F01E000`–`0x6F02CFFF`, 61440 bytes.
- Video SRAM ends at `0x6F030000` (exclusive); 12288 bytes remain unused.

Four is the ring capacity, **not the number of encoded slices per frame**.
The ISP and encoder use the same wrap depth. Partial SRAM, compression, and an
allocation that cannot fit both planes are rejected; there is no DRAM fallback.
The hardware legality of a mixed Y-in-SRAM/UV-in-DRAM configuration has not been
established; this test never selects that configuration.

The original driver still allocates full-frame DDR address ranges used for ISP
remap detection. These pre-remap addresses appear in logs/registers; the offsets
redirect writes into the SRAM ring, and the encoder receives SRAM addresses.
This is not a zero-DDR-allocation firmware. The test checks the physical Y/UV
mapping and shared mode/depth registers before starting the sensor. It does not
prove bus traffic stayed in SRAM: that requires board/bus verification.

## Build on this macOS workspace

From the repository root, after the setup in `BUILD_LOCAL.md`:

```bash
export RTK_TOOLCHAIN_DIR="$HOME/rtk-toolchain"
export AMEBA_SDK="$PWD"
source "$RTK_TOOLCHAIN_DIR/prebuilts-darwin-1.0.3/setenv.sh"
source .venv/bin/activate
export FIPTOOL="$PWD/build_RTL8735C/native-tools/fiptool"
video_test_dir=component/soc/amebapro3/sw/media_example/ap_media_example/video_example_src/video_test_tool
python ameba.py menuconfig --apply-file "$video_test_dir/configs/isp_uart.conf"
python ameba.py build -a video_ap
```

The `-a video_ap` selection is required to include the AP media example
and its UART command table. Kconfig alone does not select that application.

Applying a configuration resets prior menuconfig overrides to SDK defaults plus
the selected file. The addremap archive is now compiled locally from restored
vendor source; no external `libaddremap.a` is required. See
`video/addremap/SOURCE.md` under the SoC directory for provenance.

The SDK's NP power-management library requires Wi-Fi symbols. Wi-Fi libraries
remain linked, but this test configuration suppresses AP/NP Wi-Fi initialization
and FP Wi-Fi task/idle entry points. RTSP/HTTP preview is disabled, and no network
connection is needed. Do not invoke unrelated Wi-Fi/video example commands.

## UART procedure

Flash once. The same firmware supports both modes; no reflash is needed.
`AT+VIDPIPE` queries the selected mode. `AT+VIDPIPE=0` selects frame handoff;
`AT+VIDPIPE=1` selects the complete SRAM slice ring. The selection is held in RAM
and defaults to frame after reset; it is not saved to flash.

Select the mode **before the first start in each boot**. After any start attempt,
including a failure, reboot before another run. The firmware rejects subsequent
starts and mode changes because remap restart is not validated.

Frame run:

```text
AT+VIDPIPE=0
AT+VIDINFO
AT+VIDVALID
AT+VIDTEST=1
AT+VIDCHECK=3000
AT+VIDSTAT
AT+VIDTEST=0
```

Reset/reboot the board, then use the same firmware for the slice run:

```text
AT+VIDPIPE=1
AT+VIDVALID
AT+VIDTEST=1
AT+VIDCHECK=3000
AT+VIDSTAT
AT+VIDTEST=0
```

For slice mode, require `[ISP SRAM]` logs showing both expected address ranges
and `Y/UV mapping readback OK; partial SRAM disabled`. Treat any remap error,
start error, missing data or invalid buffer as a failure. Record the UART output
and compare frame/slice runs using the same sensor, scene, exposure and settings.

`VIDCHECK` measures buffer delivery and bitrate, not decoded picture correctness
or end-to-end latency. UART-only checks cannot validate the H.264 picture; do not
interpret a successful buffer check as proof that the video decodes correctly.

## Host regression checks

From the repository root:

```bash
remap_dir=component/soc/amebapro3/video/addremap
cc -Wall -Wextra -Werror -I"$remap_dir/driver" \
  "$remap_dir/tests/test_slice_sram.c" -o /tmp/test_slice_sram
/tmp/test_slice_sram
cc -Werror -Wno-int-to-pointer-cast -I"$remap_dir/tests/include" \
  -I"$remap_dir/hal" "$remap_dir/tests/test_remap_runtime.c" \
  "$remap_dir/driver/addremap_runtime.c" -o /tmp/test_remap_runtime
/tmp/test_remap_runtime
python3 "$remap_dir/tests/test_encoder_stride.py"
python3 "$remap_dir/tests/test_pipeline_command.py"
```

These check capacity/overflow boundaries, encoder stride, command validation and
mode locking, paired ISP/encoder flags, and vendor register writes using mocked
MMIO. They do not emulate silicon timing or the sensor/encoder handshake.

## Local verification result

The unified image built and linked successfully with `-a video_ap`. The final AP
image contains the UART command table and `fVIDPIPE`, `fVIDTEST`, `fVIDCHECK`;
the VP image contains the restored remap implementation. Both C checks and all
four Python-driven regression tests passed. Startup disassembly confirms no
AP/NP/FP Wi-Fi startup calls or FP Wi-Fi idle call in this configuration.

Artifacts and evidence are under `build_RTL8735C/isp_uart/` in this workspace.
Board operation, picture decoding and actual SRAM-only traffic remain untested.
