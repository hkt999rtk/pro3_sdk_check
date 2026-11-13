# Video AT command test tool

This tool configures the AmebaPro3 V1-V5 video pipelines through AT commands, checks data delivery, and optionally records V1 or V2 H264/HEVC video to an MP4 on SD. A PC can download the recording over HTTP for analysis. CH1/V2 also supports optional RTSP or UVC live preview selected by `AT+VIDOUT`.

Start with this guide. See [VIDEO_CMD_TEST_REGRESSION.md](VIDEO_CMD_TEST_REGRESSION.md) for detailed board regression procedures and [mmf2_video_cmd_test_tool.c](mmf2_video_cmd_test_tool.c) for the implementation. The procedures and expected results in these documents do not imply completed hardware validation.

## Six things to know first

1. Stop the test before changing parameters. Run `AT+VIDVALID` before `AT+VIDTEST=1`.
2. Normal command channel numbers start at **0**. `VIDREC` uses different source numbers: **0=recording off, 1=V1, 2=V2**.
3. Successful `VIDTEST=1` means pipeline setup completed. `VIDCHECK` PASS means valid buffers arrived during the observation window; it does not verify every encoder parameter.
4. Recording is disabled by default. Record either V1 or V2 per run, using **H264/HEVC only, without audio**.
5. After capture finishes, run `VIDTEST=0`, confirm `MP4 finalized`, then run `VIDHTTP=1` to enable downloads.
6. Each recording saves a new numbered MP4/parameter JSON pair. Download both, then run `VIDHTTP=0` before starting another test.

## V2 live preview: `AT+VIDOUT`

`AT+VIDOUT=<mode>` selects one optional live sink for **CH1/V2**:

| Mode | Output | Requirements |
|---|---|---|
| `0` | No preview (default) | Statistics and optional MP4 still run |
| `1` | RTSP | `CONFIG_LWIP_LAYER`; V2 H264/HEVC; network ready; port 554 available |
| `2` | UVC | `CONFIG_USBD_UVC`; exclusive USB device ownership |

`AT+VIDOUT` queries the selection. Configure V2 and enable CH1 with `VIDCHEN`
before selecting a mode. Selection alone does not open USB or RTSP: resources
open on `VIDTEST=1`. Changes while starting/running/stopping return state error 2.
Invalid/unsupported selections return parameter error 1 and preserve the previous
mode. `VIDVALID` and startup recheck channel, feature and preview requirements.
UVC does not compare V2 format, resolution or FPS against USB descriptors or host settings.
Encoded slice splitting must be disabled for preview.

RTSP and UVC are mutually exclusive. Either can coexist with V1/V2 MP4 recording:
the same buffer passes through statistics, optional MP4 and preview in one SISO
task. Slow USB/network consumers can affect recording and measured throughput.
`VIDCHECK` verifies incoming buffers, not successful display on the PC.

### RTSP example

Connect the board to the network and stop other RTSP applications first.
Starting from a stopped test:

```text
AT+VIDCHEN=0,1,0,0,0
AT+VIDXCLR=1,ALL
AT+VIDFMT=1,H264
AT+VIDRES=1,1280,720
AT+VIDFPS=1,30
AT+VIDREC=0
AT+VIDOUT=1
AT+VIDVALID
AT+VIDTEST=1
```

Open `rtsp://<device-ip>:554` in a player supporting the chosen codec, such as
VLC. Like the joint-test example, this tool does not restrict the URL path; the
previous `/v2` URL also remains usable. Connect Wi-Fi and obtain an IP before
starting; pipeline setup success does not confirm that the RTSP socket is listening.
RTSP PLAY requests an I frame. To record V2 simultaneously, use
`AT+VIDREC=2,10` before starting. `AT+VIDTEST=0` closes the server, connections,
module and video pipeline. The mode remains selected: start again and reconnect
the player for another run.

### UVC example

Stop the test and select UVC to forward the current V2 configuration:

```text
AT+VIDCHEN=0,1,0,0,0
AT+VIDREC=0
AT+VIDOUT=2
AT+VIDVALID
AT+VIDTEST=1
```

Connect the board's USB device port to the PC and open it in a camera application
supporting the video format. The tool forwards V2 without descriptor/host
format, resolution or FPS checks and does not drop frames because these settings
differ. Selecting UVC and host negotiation do not overwrite V2 parameters.
The USB descriptors and the PC application's supported modes still determine
which settings the PC can select; removing the checks does not change them.

`AT+VIDTEST=0` follows `mmf2_video_example_joint_test_init.c`: stop video,
stop/delete SISO, send `CMD_UVCD_STOP`, close the UVC module, then close video.
RTSP uses the same sequence with `CMD_RTSP2_SET_STREAMMING, OFF` before module
close. No shared RTSP/UVC implementation or header changes are required.

The current SDK UVC module's STOP and destroy handlers are empty. Calling this
sequence therefore does not guarantee USB controller shutdown or full resource
release. Repeated UVC start/stop needs board validation against the SDK module
being used. Do not run another USB device/composite application concurrently.

To return to statistics/recording only:

```text
AT+VIDTEST=0
AT+VIDOUT=0
AT+VIDTEST=1
```

### V2 preview board regression checklist

These procedures require hardware; they do not imply completed board validation.

- Query the default mode; reject mode 3, malformed arguments, and mode changes
  while running. Verify selecting a mode alone allocates no streaming resources.
- Reject preview with CH1 disabled, missing firmware feature, RTSP with JPEG/NV12,
  or encoded slice splitting enabled. Change parameters after selecting a valid
  mode and confirm `VIDVALID`/startup catch the new invalid configuration.
- Select UVC with V2 settings differing from the USB descriptors; verify that
  selection/startup is not rejected for this mismatch and V2 parameters remain
  unchanged. Host setting differences must not trigger preview frame filtering.
- Repeat start/stop at least 20 times per mode; switch modes while stopped.
  Verify heap/task/socket usage stabilizes, no cumulative resource loss, and no
  SISO stop-timeout messages.
- Stop with no viewer, during playback, after closing the viewer, and after
  unplugging USB/network. Verify shutdown and a successful subsequent restart.
- Record V2 while previewing; stop, download and analyze the MP4/JSON pair.
- Inject module/allocation failures, including a later channel failing after V2
  opens; verify rollback releases preview resources and permits the next run.

## V5 object detection: `AT+VIDNN`

`AT+VIDNN=1` enables fixed YOLOv4-tiny inference on **CH4/V5**;
`AT+VIDNN=0` disables it (default). `AT+VIDNN` queries the selection.
Change the selection only while stopped. It persists across stop/start cycles.
Configure and enable CH4 before selecting NN. The fixed model requires RGB
416x416 input and `vfs:/yolov4_tiny_asymu8.nb`, using the same model loading
mechanism as the joint-test example. There is no model-selection command.
The firmware must include VIPNN and the `yolov4_tiny` model adapter.

Starting from a stopped test, with HTTP downloads stopped:

```text
AT+VIDOUT=0
AT+VIDREC=0
AT+VIDCHEN=0,0,0,0,1
AT+VIDRES=4,416,416
AT+VIDNN=1
AT+VIDVALID
AT+VIDTEST=1
```

V5 retains its buffer statistics and forwards each frame synchronously to VIPNN
in the same SISO consumer. Detection prints all returned classes (up to 32 boxes)
in this format, with coordinates expressed in V5 input pixels:

```text
[VID_CMD_TEST-I] 0,c0:12 34 200 380
```

The fields are detection index, class ID, xmin, ymin, xmax, ymax. No OSD is drawn.
No detection lines are printed for a frame with no detected objects. `VIDCHECK`
measures buffer delivery; it does not validate detection accuracy. Inference and
serial logging can reduce V5 throughput. V2 RTSP/UVC may be enabled independently.

`AT+VIDTEST=0` stops video and the SISO consumer, then pauses/closes VIPNN before
closing video modules. Setup failures use the same rollback path. To disable NN
for the next run, issue `AT+VIDNN=0` while stopped.

Board checks (not yet hardware-validated): test NN off/on/off with repeated
stop/start; detect a known object; reject disabled CH4 or non-416x416 input;
reject changes while running; test a missing model and retry after restoring it.
`VIDVALID` checks channel settings; model loading is checked during `VIDTEST=1`.

## Prerequisites

- The firmware must include this AT tool. `ap_media_example/CMakeLists.txt` includes the source when `CONFIG_SUPPORT_ATCMD` is enabled.
- The sensor and video firmware must work correctly. Choose resolution/FPS combinations supported by the sensor and channel. Numeric limits accepted by the tool do not guarantee hardware support.
- Send commands sequentially through the existing AT interface. Wait for each command's OK/ERROR terminator before sending the next command.
- Do not run another example that owns the same video channels concurrently.
- MP4 recording requires an inserted, writable FAT-formatted SD card with sufficient free space and the corresponding firmware support. Do not run another SD/MP4 application concurrently.
- HTTP requires the SDK WLAN/httpd network configuration. The source checks `CONFIG_WLAN` together with `CONFIG_WHC_HOST`, `CONFIG_WHC_NONE`, or `CONFIG_WHC_DEV_TCPIP_KEEPALIVE`. Without this support, `VIDHTTP` reports that the feature is unavailable.
- The PC and board must be reachable on the local network. Do not start the audio HTTP server or another httpd user concurrently.

## Channels and initial settings

These are the initialization values. Stopping and restarting a test does not reset parameters.

| AT channel | Video stream | Supported formats | Initial format | Initial resolution | FPS / GOP | Bitrate |
|---|---|---|---|---|---|---|
| 0 | V1 | HEVC, H264, JPEG, NV12 | HEVC | 2688x1520 | 15 / 15 | 2000000 |
| 1 | V2 | HEVC, H264, JPEG, NV12 | H264 | 1280x720 | 30 / 30 | 500000 |
| 2 | V3 | HEVC, H264, JPEG, NV12 | JPEG | 1920x1080 | 1 / 1 | 1000000 |
| 3 | V4 | NV12 | NV12 | 640x480 | 10 / N/A | N/A |
| 4 | V5 | RGB | RGB | 416x416 | 10 / N/A | N/A |

Initially, all five channels and AINR are enabled, while recording is disabled. Start with a single channel selected through `VIDCHEN`. AINR is global across V1-V5. Every selected channel explicitly carries the same `VIDAINR` setting at startup, so V1 does not need to be enabled. Changing AINR requires stopping the test first.

## Quick start: check V1 output without recording

This example assumes the sensor supports 1920x1080 at 15 FPS. If this tool's HTTP server is already active, first run `AT+VIDHTTP=0`.

```text
AT+VIDTEST=0
AT+VIDREC=0
AT+VIDCHEN=1,0,0,0,0
AT+VIDXCLR=0,ALL
AT+VIDFMT=0,HEVC
AT+VIDRES=0,1920,1080
AT+VIDFPS=0,15
AT+VIDGOP=0,15
AT+VIDBPS=0,2000000
AT+VIDINFO=0
AT+VIDVALID
AT+VIDTEST=1
AT+VIDCHECK=3000
AT+VIDSTAT=0
AT+VIDTEST=0
```

Expect CH0 to report PASS during the three-second observation window. After stopping, run `VIDSTAT=0` again to confirm that cumulative buffer counts no longer increase. Counters reset when a new run starts.

`VIDXCLR=0,ALL` clears only advanced encoder settings. It does not reset resolution, FPS, bitrate, basic RC mode, QP, channel selection, or recording options. Automated tests should explicitly set the basic parameters needed by each case.

## Record MP4 and download it to a PC

### 1. Select the recording channel

Configure V1 as described above, then run the following while stopped:

```text
AT+VIDREC=1,10
AT+VIDVALID
AT+VIDTEST=1
```

This records V1 to `sd:/video_test_v1_000001.mp4` (example ID). Wait for `MP4 capture finished`, then run:

```text
AT+VIDTEST=0
```

Confirm that `MP4 finalized` appears. To stop early, send `VIDTEST=0` directly; at least two samples must have been collected to finalize a shorter MP4.

To record V2 instead, use the configuration below. Note that `VIDFMT` uses **CH1**, while `VIDREC` uses **source 2**:

```text
AT+VIDCHEN=0,1,0,0,0
AT+VIDXCLR=1,ALL
AT+VIDFMT=1,H264
AT+VIDRES=1,1280,720
AT+VIDFPS=1,30
AT+VIDGOP=1,30
AT+VIDBPS=1,2000000
AT+VIDREC=2,10
AT+VIDVALID
AT+VIDTEST=1
```

Again, run `VIDTEST=0` after capture finishes. The file is `sd:/video_test_v2_000001.mp4` (example ID).

### 2. Start the download server

For Wi-Fi, replace the SSID and password in this example:

```text
AT+WLCONN=ssid,<ssid>,pw,<password>
AT+VIDHTTP=1
```

On the PC, open the logged `http://<device-ip>/video` URL in a browser and click **Download MP4** and **Download parameters JSON**. Keep both original filenames in the same PC folder. You can also open `http://<device-ip>/video/record.mp4` directly.

The URL is a fixed download endpoint, not an SD path. It serves only this tool's latest successfully finalized recording. The browser determines where the file is saved on the PC. Downloads use plain HTTP on port 80, without authentication or Range/resume support.

### 3. Finish downloading and prepare the next run

```text
AT+VIDHTTP=0
AT+VIDREC=0
```

If the next run should also record, keep the recording selection and omit `VIDREC=0`. HTTP must stop first. If shutdown reports busy, wait and retry `VIDHTTP=0`. Keep the SD card inserted throughout recording and downloading.

### Recording limits and validation timing

- `VIDREC` checks only the source number, duration, and stopped state. Its OK response does not mean the current format is suitable for recording.
- `VIDVALID` and `VIDTEST=1` check that the selected channel is enabled, uses H264/HEVC, has a valid sample count, and has slice splitting disabled. Recording does not start if these checks fail.
- The duration is 1-60 nominal seconds, and `configured FPS * seconds` must be between 2 and 7200 samples. The muxer stops by sample count, not elapsed wall-clock time. Low output rates or frame skipping can extend capture time.
- The video pipeline continues after the recording target is reached, until `VIDTEST=0`.
- Clips with fewer than two samples, recording errors, or startup rollback are not offered for download. Partial files may remain on SD; their presence does not mean they are valid MP4 files.
- Each channel uses the first free six-digit number (000001¡V999999). Existing MP4 or parameter files reserve that number across reboots; deleting both permits reuse. IDs are local to this SD card and channel, not globally unique. Failed runs may leave reserved files. Only the latest finalized pair in the current session is exposed over HTTP.
- MP4 writes and buffer statistics share the same SISO path. SD write speed can affect output; performance tests should record separate results with recording enabled and disabled.

## Common commands

`<ch>` means 0-4 unless otherwise restricted below. Configuration commands are accepted only in the STOPPED state. `VIDIFR` is an operation performed while running.

| Command | Purpose / limits |
|---|---|
| `AT+VID?` | Print the complete help |
| `AT+VIDCHEN=<0 or 1>,<0 or 1>,<0 or 1>,<0 or 1>,<0 or 1>` | Select CH0-CH4 in order; startup requires at least one enabled channel |
| `AT+VIDFMT=<ch>,<HEVC/H264/JPEG/NV12/RGB>` | Set a format supported by the channel |
| `AT+VIDRES=<ch>,<width>,<height>` | Set dimensions; NV12 sources require even width/height; overflow checks apply |
| `AT+VIDFPS=<ch>,<fps>` | Set a nonzero FPS; actual support depends on hardware |
| `AT+VIDGOP=<ch0-2>,<gop>` | Set a nonzero GOP |
| `AT+VIDBPS=<ch0-2>,<bps>` | Bitrate, 1-2147483647 |
| `AT+VIDRCM=<ch0-2>,<mode>` | 0=CVBR, 1=CBR, 2=VBR, 3=ABR, 4=CRF, 5=CQP |
| `AT+VIDQP=<ch0-2>,<min>,<max>` | 0-51, with min no greater than max |
| `AT+VIDQLVL=<ch0-2>,<qlevel>` | JPEG quality, 0-10 |
| `AT+VIDAINR=<0 or 1>` | Global AINR for V1-V5; applied on startup even with V1 disabled |
| `AT+VIDINFO` or `AT+VIDINFO=<ch or ALL>` | Show configured parameters and pipeline state; not hardware readback |
| `AT+VIDVALID` | Validate local settings for enabled channels; does not query actual sensor capabilities |
| `AT+VIDTEST=<0 or 1>` | Stop/start; failed startup rolls back |
| `AT+VIDSTAT` or `AT+VIDSTAT=<ch>` | Cumulative statistics for all channels or one channel; the ALL string is not accepted |
| `AT+VIDCHECK=<ms>` | Observe a running test for 100-10000 ms; occupies the AT dispatcher during the window |
| `AT+VIDIFR=<ch0-2>` | Request an I-frame on an active H264/HEVC channel; REQUESTED does not verify the bitstream |
| `AT+VIDNN` or `AT+VIDNN=<0 or 1>` | Query/select fixed V5 YOLOv4-tiny inference; stopped only; RGB 416x416 |
| `AT+VIDOUT` or `AT+VIDOUT=<0 or 1 or 2>` | Query/select V2 preview: 0=none, 1=RTSP, 2=UVC; stopped only |
| `AT+VIDREC=0` or `AT+VIDREC=<1 or 2>,<seconds>` | Disable recording, record V1, or record V2 |
| `AT+VIDHTTP=<0 or 1>` | Stop/start the latest recording's download server; startup requires the test to be stopped |

## Advanced encoder commands

These configuration commands require CH0-CH2 to be configured as H264 or HEVC and the test to be stopped. The exception is `VIDXCLR`, which can clear residual settings on CH0-CH2 regardless of their current format. Accepted tool settings do not guarantee that every combination is supported by the hardware/driver.

| Command (prefix each with `AT+`) | Parameters and meaning |
|---|---|
| `VIDPROF=<ch>,<profile>,<level>,<tier>` | Profile -1=adaptive; HEVC 0-9, H264 9-12; level 65535=auto; HEVC tier 0-1, H264 tier 0 |
| `VIDQPHDR=<ch>,<qp>` | -1=auto, 0-51=initial QP |
| `VIDIQP=<ch>,<min>,<max>` | I-frame QP range, 0-51 |
| `VIDIQPD=<ch>,<delta>` | Intra QP delta, -51 to 51 |
| `VIDFIQP=<ch>,<qp>` | Fixed intra QP; 0=disabled, 1-51=specified value |
| `VIDPICRC=<ch>,<0 or 1>` | Picture-level RC |
| `VIDCTBRC=<ch>,<mode>` | 0=off, 1=subjective, 2=precise, 3=mixed |
| `VIDBLKRC=<ch>,<size>` | RC block size: 0=64, 1=32, 2=16 |
| `VIDPSKIP=<ch>,<0 or 1>` | Picture skipping |
| `VIDHRD=<ch>,<0 or 1>,<cpb_size>,<cpb_max_rate>` | CPB: 0=default, 1-2147483647, or 0xffffffff=bitrate; max rate: 0=auto or 1-2147483647 |
| `VIDBRWIN=<ch>,<frames>` | Bitrate window, 1-300 |
| `VIDMVBR=<ch>,<tolerance>,<frames>` | Tolerance 0-2000%; monitor 10-120 frames |
| `VIDFILL=<ch>,<0 or 1>` | Filler; requires explicit nonzero CPB and PICRC=1 |
| `VIDCRF=<ch>,<crf>` | -1=disabled, 0-51=enabled |
| `VIDVBR=<ch>,<0 or 1>` | VBR extra; cannot be enabled together with HRD |
| `VIDRCFPS=<ch>,<num>,<denom>` | Rational RC FPS; num 1-1048575, denom 1-2147483647; does not change ISP FPS |
| `VIDSLICE=<ch>,<rows>` | Slice block rows; 0=disabled; must be disabled for MP4 recording |
| `VIDSEI=<ch>,<driver_sei>,<user_sei>` | Each is 0/1; controls the two SEI sources |
| `VIDFULLR=<ch>,<0 or 1>` | VUI limited/full range |
| `VIDDBLK=<ch>,<disable>,<tc>,<beta>` | disable 0/1; each offset is -6 to 6 |
| `VIDSAO=<HEVC ch>,<0 or 1>` | HEVC SAO |
| `VIDCABAC=<H264 ch>,<0 or 1>` | H264 CABAC; cannot be enabled with the baseline profile |
| `VIDSAR=<ch>,<width>,<height>` | 0,0=unspecified; otherwise both values must be 1-65535 |
| `VIDCIR=<ch>,<start>,<interval>` | Each value is between 0 and the current encoder block count |
| `VIDROI=<ch>,<idx>,<en>,<top>,<left>,<bottom>,<right>,<delta_qp>,<qp>` | idx 0-7; HEVC 64x64 CTB / H264 16x16 MB coordinates; delta -51 to 51; qp=-1 uses delta, 0-51 uses absolute QP |
| `VIDXCLR=<ch0-2>,<PROFILE/RC/CODING/ALL>` | Clear advanced flags/settings in the selected category |

Accepted levels: HEVC uses 30,60,63,90,93,120,123,150,153,156,180,183,186; H264 uses 10,11,12,13,20,21,22,30,31,32,40,41,42,50,51,52,60,61,62,99. Both accept 65535 for automatic selection.

Changing codec or resolution does not automatically clear old ROI, slice, profile, or other settings. If `VIDVALID` reports a conflict, correct the settings or clear them with `VIDXCLR` before retrying. Checks also cover conflicts such as HRD/VBR, CRF/CTB RC, and baseline/CABAC.

## Interpreting PASS/FAIL

| Observation | What it establishes | What it does not establish by itself |
|---|---|---|
| `VIDINFO` settings and ACTIVE | Settings stored by the tool and pipeline resource state | Actual hardware settings or continuous frame output |
| `VIDCHECK` PASS | Every enabled channel received at least one buffer with nonzero address and size during the window, with no invalid buffers in that window | Correct FPS, complete frame count, image quality, decodability, or effective advanced settings |
| `VIDSTAT` | Buffers, KiB, invalid count, and last valid buffer age_ms since the last start | Exact dropped-frame count or decoded-frame count |
| `MP4 finalized` and successful download | The muxer reported successful finalization and the file can be transferred to the PC | Successful completion of all encoding/image-quality tests |

`buffers_per_s` is a buffer rate, not necessarily FPS, especially with slice output. `kbps` is calculated from the data volume during the observation window. For low rates such as 1 FPS, observe at least three seconds; a shorter window may fall entirely between two frames.

`VIDCHECK` examines only its observation window. Check `VIDSTAT` for earlier invalid buffers. A failed check does not automatically stop the pipeline. Buffer/invalid counters and displayed KiB wrap at 32 bits; age is subject to RTOS tick wraparound. Age is shown as 0 when no valid buffer has arrived, so also inspect the buffer count.

PC analysis should check the properties relevant to each case: codec, decoded dimensions, timestamps, frame types/GOP, bitrate, headers/SEI/VUI, and so on. ROI and AINR may also require encoder statistics, controlled scenes, and image comparisons. QP bounds are checked using the exact picture/block QP backend described below. Mark cases without the required evidence as unverified/SKIP instead of assigning PASS solely because data arrived.

## Error codes and troubleshooting

| Code | Meaning | Suggested checks |
|---|---|---|
| 1 | Invalid parameters or local configuration | Check channel, values, codec, advanced-setting compatibility, and the preceding log |
| 2 | Operation not allowed in the current state | Stop before configuring; stop the test and other servers before starting HTTP; retry busy HTTP shutdown |
| 3 | Startup failed | Check precheck results, SD, module/queue/SISO allocation, and lower-level logs; not necessarily a hardware failure |
| 4 | VIDCHECK saw no data or invalid buffers | Inspect per-channel results and sensor/VP logs; extend the observation window; stop and retry if needed |
| 5 | MP4/SD/HTTP I/O or completion-state error | Check SD availability/space, insufficient samples, recording logs, and HTTP startup results |

The same invalid configuration may produce code 1 from `VIDVALID` and code 3 from `VIDTEST=1`, because the latter classifies it as a startup failure. Even if `VIDTEST=0` reports code 5 for an incomplete MP4, teardown has run; use `VIDINFO` to confirm the state.

If the browser cannot connect, check the IP, local network connectivity, port 80, and whether `VIDHTTP=1` succeeded. Do not use `sd:/...` as a URL. HTTP 404 means opening the file failed; 500 indicates a file-size, seek, memory, or similar error. Restart a full download after an interrupted transfer.

## Notes for maintainers and AI agents

The data path is:

```text
video_module -> SISO -> video_test_sink_module
                         |- filesaver callback: update buffer statistics
                         |- selected V1/V2: mp4_module.handle -> SD MP4
                         `- V2: optional rtsp2_module / uvcd_module -> PC

Stop all SISO tasks -> MP4 finalize/close -> start HTTP -> read SD in chunks -> PC
```

- `at_video_cmd_test_items`: command registration table. When adding commands, update `fVIDHELP`, this README, and the regression cases.
- `video_validate_selected_params`: recording, dimension, and advanced-parameter precheck shared by startup and `VIDVALID`.
- `mmf2_video_cmd_test`, `video_test_start_channel`: initialization; create the consumer before applying video; roll back on failure.
- `video_preview_validate/open/close`: V2 preview validation/lifetime; stop video and delete SISO, then stop/close the preview module before closing video, as in the joint-test example.
- `video_test_sink_handle`: count, preview and mux the same queue item while preserving its timestamp. Do not add a second SISO that competes for the same ready queue.
- `video_record_prepare`: create the muxer, check mounting, and configure recording. Muxer GOP=1 reserves a keyframe-index entry per frame; it does not change the encoder GOP.
- `video_test_deinit`, `video_record_close`: stop producers/SISO tasks before finalizing the muxer to avoid concurrent access. Clips with fewer than two samples take the error-cleanup path to avoid timestamp-table out-of-bounds access.
- `video_stats_snapshot`: protect statistics updated by SISO tasks, including the 64-bit byte count, with a critical section.
- `video_http_download`: serve the fixed finalized file in 4 KiB chunks, send Content-Length and an attachment filename, and consistently release the file, RAM, and connection.
- `fVIDHTTP`: httpd is shared; do not stop another user's server. `httpd_stop()` is asynchronous. Wait for the server/download callback to finish before unmounting SD.
- State handling assumes sequential command execution by the AT dispatcher. Do not call command handlers directly from multiple tasks without synchronization.
- Repeated VFS registration does not provide independent reference counting. Do not interleave this tool's SD/HTTP lifecycle with other SD users.
- Follow the repository AGENTS.md when making changes. Do not build/configure/flash without an explicit request. Use diff review, C/C++ static analysis, and AStyle for modified C/H files as appropriate. Documented cases are procedures to execute, not evidence of passed tests.


## PC recording analysis

Use [Video Validation Reporter](video_validation_reporter/README.md) to analyze
downloaded MP4 files against their matching parameter snapshots and generate
HTML reports, including picture/block QP verification.

All PC scripts, setup instructions, tests and local recording files are grouped
under `video_validation_reporter/`. Enter that directory before following its
Windows or Linux setup and analysis commands.

### AINR with V1 disabled

The test tool keeps one global AINR setting (default ON) and copies it into every
selected channel before opening it. The video module sends `--ainr` for V1-V5
using the normal `CMD_VIDEO_APPLY, 0` call. Update the MMFV2 library as well when
using a prebuilt library.

The first channel opened determines AINR for the run. Opening additional channels
does not reset AINR while another channel is streaming. For combined startup via
`video_module_stream_start()`, the first non-NULL channel in V1-to-V5 order supplies
the global value; later channel defaults cannot override it. Other applications
starting without V1 should set `ainr_en` on their first channel explicitly.

On the board, enable only V2 (`AT+VIDCHEN=0,1,0,0,0`), select RTSP, then test
`VIDAINR=1`, `0`, and `1` across stopped runs. Repeat from a cold boot with OFF
first, and with each of V3-V5 selected alone. Verify VP's parsed `ai_denoise_en`
and AINR enable/disable logs; use controlled scene comparisons for image quality.
These procedures have not yet been validated on hardware.

## ISP SRAM comparison

See [ISP_SRAM_TEST.md](ISP_SRAM_TEST.md) for the UART-only H.264 frame/SRAM slice configurations, memory map, build and test procedure.
