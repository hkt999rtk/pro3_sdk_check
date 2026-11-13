# Adaptive encoder OSD and polygon privacy masks

Public APIs: `video_osd_stream.h`, `video_osd.h`, `video_osd_privacy.h`. The component belongs beside the shared
`video_osd.h` helpers, not inside an example or NN postprocessor. It is AP/CA32
code; the frame-boundary transaction is implemented by the VP encoder module.
AP and VP must be updated together, including any prebuilt multimedia libraries.

## Contract

- One zero-initialized, caller-owned context per H.264/HEVC stream. Initialize
  `video_osd_stream_init()` before creating any logo, timestamp, NN or privacy
  clients on that stream. Privacy auto-creates a manager if none exists, for
  compatibility; explicit stream ownership is recommended. The
  target must match the running encoder dimensions and codec. Low-latency/slice,
  look-ahead, multi-pass and combined H.264/HEVC + JPEG encoding are rejected;
  this implementation uses the SDK's single-core, single-pass synchronous path.
- Up to 8 polygons, each with 3 to 16 vertices. Convex/concave shapes and either
  winding are supported. Follow the perimeter, without self-intersections or
  holes. A duplicate final closing vertex is accepted and counts toward 16.
  Duplicate IDs, zero-area/degenerate shapes, out-of-range coordinates and
  shapes that collapse after scaling are rejected, even if disabled.
- Coordinates are image edges: origin at top-left, x right, y down. Choose
  normalized integers `[0,10000]` or reference pixels `[0,reference_width]` and
  `[0,reference_height]`. Reference dimensions must be in `[1,1000000]`.
  The right/bottom edge maps to the target width/height. APP letterboxing, crop,
  mirror, rotation and PTZ/world-coordinate transforms are the caller's job.
- One opaque `0x00RRGGBB` fill for the scene, black by default. Overlapping masks
  form a union; no transparent edges, blur, mosaic, outline-only mode or holes.
  Rasterization conservatively covers boundaries by approximately one pixel.
- `apply()` replaces and copies the entire scene; the caller may reuse its
  input immediately. Remove a polygon by omitting its ID, disable one with
  `enabled=0`, or remove all with `polygon_count=0`. These are explicit unmasking
  operations, not error fallbacks. `scene.enabled=0` disables all masks while
  retaining their definitions. `get()` returns the last accepted scene.
- APIs are task-context only. Serialize per-stream client lifetimes, including
  init/deinit versus other calls. Do not call them from the encoder/IPC callback.

## Coexistence and cost

The manager rasterizes only aligned bounding regions. Disjoint regions remain
separate; regions sharing a 64x16 H.264 or 64x64 HEVC block are merged before VP
submission. Merging repeats until no output regions share an encoder block.
Opaque bitmap regions of the same YUV color remain bitmap; mixed colors,
translucent overlap or an ARGB source require a localized ARGB8888 region.
There is no unconditional full-frame ARGB allocation. A large connected overlap
group can still require a large, even full-frame, ARGB region.

Composition order is privacy, NN, logo, timestamp. Logo and timestamp may overlap
each other or privacy. A translucent logo over privacy blends with the opaque
mask, not camera pixels. If any NN layer shares a block with a protected
logo/timestamp region or a privacy polygon's conservative bounding rectangle,
the entire NN group is omitted. The planner also omits the entire group if final
alignment or merging would put any of its layers into a protected output region,
including merges forced by index scarcity. It then replans without that group;
the source layers remain available for subsequent non-conflicting updates.
This includes a concave polygon's empty notch and gaps bridged by merged bounds;
arbitration is not pixel-exact. The public NN drawing APIs are unchanged.

### All feature combinations

L = logo, T = timestamp, N = NN group, P = privacy polygons.
The table describes block-disjoint output; the merge/arbitration rules above
apply to every row. Each feature can contain more than one region.

| Enabled | Initial output choice |
|---|---|
| None | No pixel buffer or hardware index |
| L | Cropped ARGB |
| T | Cropped bitmap |
| N | Cropped bitmap per NN color layer |
| P | Cropped bitmap polygons |
| L + T | ARGB L + bitmap T |
| L + N | ARGB L + bitmap N |
| L + P | ARGB L + bitmap P |
| T + N | Bitmap T + bitmap N |
| T + P | Bitmap T + bitmap P |
| N + P | Bitmap N + bitmap P |
| L + T + N | ARGB L + bitmap T/N |
| L + T + P | ARGB L + bitmap T/P |
| L + N + P | ARGB L + bitmap N/P |
| T + N + P | Bitmap T/N/P |
| L + T + N + P | ARGB L + bitmap T/N/P |

The planner also merges when doing so reduces pixel payload, or when physical
indices are insufficient, choosing the smallest payload increase and repeating
block-conflict closure. This is a bounded greedy planner, not globally optimal
packing or per-block tiling. NV12 is not selected by this manager.

### Memory and ownership

A bitmap region costs `aligned_width * height / 8` payload bytes; ARGB costs
`width * height * 4`. Output regions are immutable while the encoder reads them.
Unchanged regions reuse the same buffer. Changed regions retain old and new
buffers only until frame-boundary completion, then retire the old allocation.
Static regions therefore do not retain a second output canvas.

Input storage is additional: logo retains its owned ARGB copy; timestamp retains
two small bitmap sources; NN retains its original full-frame 1-bit drawing canvas.
The managed NN path skips the intermediate cropped buffer and copies a strided
view into a tight manager-owned shadow. This preserves the public drawing API;
it does not eliminate all source/output duplication.

`video_osd_stream_get_status()` reports:
- `allocated_bytes`: manager, descriptor and live/staged output allocations.
- `source_bytes`: sampled registered client storage, including NN working canvases.
- `peak_bytes`: largest observed sum of those two values.
- `bitmap_regions`, `argb_regions`: currently applied output region counts.
- `index_count`: reserved physical indices, including a pending update.
- Revisions, pending state, applied frame timestamp and encoder error.

`memory_budget=0` disables the limit. Otherwise init/commit reject manager
allocations that would exceed the budget including sampled sources and old/new
outputs. It is not a reservation or a hard cap on all process heap usage: client
source allocations can occur before registration/commit. RTOS tasks, semaphores,
allocator metadata, unrelated buffers and unobserved transient allocations are
not included. The demo's 8 MiB setting is a limit, not an 8 MiB allocation.
`privacy_get_caps().canvas_bytes` remains a compatibility worst-case estimate
of two full ARGB canvases; it is NOT the amount allocated by privacy init.

The manager alone reserves physical indices from the shared 12-index pool.
Managed helpers use logical handles, not additional physical reservations.
If consolidation cannot fit the pool or budget, the update fails and the prior
accepted scene remains. One transaction may be pending; retry EBUSY with a delay
and coalesce APP drag updates.

The existing logo/timestamp/NN APIs remain unchanged. Without explicit stream
init (or privacy auto-init), their legacy direct-overlay behavior is retained.
Do not introduce direct `video_set_osd*()` clients on a managed stream; VP rejects
legacy OSD writes while scene ownership is active. Mosaic remains a separate
feature sharing the hardware index pool, not a polygon backend.

## Shared timestamp / NN font

`video_osd_font.c` now uses **Ameba OSD Sans**, a monochrome ASCII derivative
of Liberation Sans Bold 2.1.5 (Arial-compatible source metrics, SIL OFL 1.1).
Scale 1/2/3/5 selects separately rasterized glyphs in 8/16/24/40-pixel cells instead of
enlarging the old 8x8 pixel-art font. Text uses proportional advances with
equal-width digits and 1/1/2/3-pixel extra spacing for scale 1/2/3/5 (capped at the
cell width); use `video_osd_font_string_width()` for actual layout width.
Timestamp protection dimensions remain fixed at `max_chars * 8 * scale`, with
the measured text aligned inside. NN text APIs and glyph heights are unchanged.
This remains 1-bit text, not grayscale
antialiasing; it does not force an ARGB output or add runtime font allocations.
Timestamp defaults to scale 3; set `config.font_scale = 5` for large text, or
set `V1_TIMESTAMP_FONT_SCALE` to 5 in the V1 demo. For `max_chars=19`, scale 5
reserves 760x40 pixels; this reservation plus margins must fit the frame.
Constant font payload is 29640 bitmap bytes plus 380 advance bytes. See `fonts/README.md` for
provenance, regeneration and licensing; keep `fonts/OFL.txt` with distributions.
`tests/video_osd_font_test.c` covers native glyph copying, larger NN scales and
clipping, and is opt-in like the other C tests below.

## Synchronous convenience APIs (recommended for managed streams)

Existing APIs and their signatures remain available. New task-context helpers
hide polling/retry loops; they do not introduce a session object, worker task,
queue, canvas or new AP/VP protocol.

| API | Success means |
|---|---|
| `video_osd_stream_wait_applied(ctx, revision, timeout_ms, status)` | The applied revision reached the fixed target, with no reported encoder error |
| `video_osd_logo_show_sync(ctx, config, timeout_ms)` | Initial logo update was accepted and encoder-applied |
| `video_osd_timestamp_start_sync(ctx, config, timeout_ms)` | Clock started and its initial update was encoder-applied |
| `video_osd_privacy_apply_sync(ctx, scene, revision, timeout_ms)` | A complete replacement was accepted and encoder-applied |
| `video_osd_timestamp_stop_sync(ctx, timeout_ms)` | Clock task stopped, removal confirmed and context released |
| `video_osd_logo_hide_sync(ctx, timeout_ms)` | Removal confirmed and context released |
| `video_osd_privacy_deinit_sync(ctx, timeout_ms)` | Privacy deinitialized, including an auto-owned manager if present |
| `video_osd_stream_deinit_sync(ctx, timeout_ms)` | Manager released after all clients were removed |

For wait, revision 0 snapshots the current requested revision on the first valid
status read. Later clock/NN updates do not move the target: success need not mean
`pending == 0` if a newer transaction is in flight. A future revision is invalid.
The optional status output contains the latest successful status sample, including
`applied_timestamp`; it is not initialized if no status read succeeded.

Logo show_sync and timestamp start_sync require a matching manager (stream ID,
codec and frame size) and an empty context. They retry EBUSY before acceptance,
then wait for the initial revision captured under the manager lock. There is no
extra per-feature wait in the example. Keep config/format/logo pixels stable
until return; source pixels and format storage may be released afterwards.
The logo rows and timestamp format are copied internally.

On any start/show error, retain the context and use the corresponding sync
removal before a fresh attempt. A post-accept timeout/error can leave the OSD
active; an accepted clock keeps refreshing. Do not clear priv, blindly repeat
start/show, or treat timeout as cancellation. Unlike sync removal, start/show
on a nonempty context returns EINVAL. A generic stream wait succeeding after a
start timeout does not prove that this particular client was accepted: the
timeout may have occurred while waiting to submit. The simple recovery path is
sync removal (safe for empty contexts), followed by a fresh start/show.

Privacy sync retries only pre-accept EBUSY, then waits without resubmitting.
Its optional revision output is cleared on entry and filled at acceptance, even
if completion later times out. Keep the input scene stable throughout the call.

All helpers return 0 or a negative error. `VIDEO_OSD_ETIMEDOUT = -6` is new; existing
error values are unchanged and are now declared in `video_osd.h`. `timeout_ms`
is one polling/retry budget for that call, including elapsed operation time.
An in-flight IPC/mutex call can exceed it; this is not a hard real-time deadline.
Zero performs one attempt; UINT32_MAX is a finite duration, not wait forever.
Only EBUSY causes retry; other operation errors are returned. Timeout does not
cancel, roll back or free a submitted update.

Retain contexts after errors/timeouts. Retry the same sync removal until success,
never switch a partially removed client to legacy void stop/hide or restart it.
The clock stops refreshing once managed stop begins, even if removal times out.
Empty, zero-initialized contexts are already removed and return 0; NULL is invalid.
Live legacy/direct-overlay logo/clock clients cannot use these sync removals
(EINVAL); they do not have the required encoder completion protocol.

Serialize application updates and lifecycles per stream. Background clock/NN
refreshes may continue, but NN producers must be stopped/unregistered before
manager deinit. Do not call synchronous helpers from ISR or encoder/IPC callbacks,
or while holding an NN group lock needed by a background callback.

## Safe startup, update and shutdown

1. Open `video_module`, set `CMD_VIDEO_SET_OUTPUT_GATE` with `blocked=1`, set
   parameters and queues, then `CMD_VIDEO_APPLY`. Keep downstream delivery closed.
2. Call `video_osd_stream_init()` with matching target and a memory budget.
   Add only needed clients. For masks, initialize privacy and call
   `video_osd_privacy_apply_sync()`. Use `video_osd_logo_show_sync()` and
   `video_osd_timestamp_start_sync()` for logo/clock. Each returns 0 only after
   encoder application; no separate per-feature wait is needed.
3. Call `video_osd_stream_wait_applied()` with a status output to obtain the applied
   timestamp for the output gate (and any intervening refresh), not as a required
   second wait for each sync start. Check its return value before opening delivery.
4. Start the sink/linker; open the gate with `filter_timestamp` enabled only when
   `applied_revision != 0` and `minimum_timestamp=status.applied_timestamp`.
   This drops stale pre-mask IPC frames. Request an I-frame.
5. For updates use `video_osd_privacy_apply_sync()` with a complete replacement.
   On ETIMEDOUT with a nonzero returned revision, continue waiting for that revision
   using the stream wait API. Do not assume the update was rejected or cancelled.
   Already encoded/queued/network-buffered frames remain from the prior revision;
   neither acceptance nor an encoder-applied ACK is a player/display ACK.
6. Before shutdown block delivery and stop the downstream linker/sink. Call
   timestamp stop_sync, logo hide_sync, privacy deinit_sync, stream deinit_sync
   in order, checking each return value. Remove NN groups before manager deinit
   if used. Stop the encoder ONLY when all removals succeed. On failure keep
   delivery blocked and encoder running, and retry the stop entry.
   Each API has its own timeout budget; the example's four removal calls may
   consume four budgets. The manager does not implicitly destroy other clients.

Advanced applications may retain the asynchronous APIs with their own polling.
Legacy `apply()` returning 0 means accepted, not applied; the old void stop/hide
contracts have not been changed. New applications should not inspect `ctx.priv`
to determine completion.

The completion ACK means preprocessing has selected the entire region list for
an input frame and the previous buffers are retired; it is not a packet/display ACK. Buffers are
not reused or freed while completion is pending, including an ambiguous IPC
timeout. Keep the context and retry/poll; never free it on a timeout. If VP stops
responding, keep output blocked and recover/restart the pipeline under application
control rather than assuming that unmasking or buffer retirement succeeded.

Validation/allocation/busy errors leave the previous accepted scene intact. A
runtime preprocessing rejection is reported in status and stops encoded output
while scene ownership is active instead of disabling privacy. Submit a valid
scene to recover; do not gate a stream open while status reports an error.
Do not stop/reconfigure the encoder externally before deinitializing its clients.

This protects only the configured **encoded stream**. JPEG snapshots, RGB/NN
inputs, other streams and pre-encoder recordings are not automatically masked.
The APP transport (HTTP/MQTT/P2P), authorization, persistence and transformations
belong above this API. A production product must enforce masking on every output
it exposes, including startup, snapshot and recovery paths.

## Demo and verification

`mmf2_video_example_v1_osd_init.c` exports these VIDEO table entries:

- `mmf2_video_example_v1_osd_init`: 720p H.264, UVC (requires `CONFIG_USBD_UVC`).
- `mmf2_video_example_v1_osd_rtsp_init`: same scene, RTSP (requires networking).
- `mmf2_video_example_v1_osd_update`: move the concave polygon left/back at runtime.
- `mmf2_video_example_v1_osd_stop`: block output and release resources.

The default scene contains a seven-sided convex polygon and a ten-vertex concave
star, with mostly diagonal edges and intentional overlap with the top-left RGB
logo and bottom-right timestamp. Edit the `v1_privacy_polygons[]` table near the
top of the example to replace vertices; `V1_PRIVACY_SHIFT_X` controls the update
entry's leftward movement. Replace the RGB-bar asset in `v1_logo_start()` with
customer ARGB pixels. The sectioned `mmf2_video_example_v1_osd_start()` shows the
V1 -> stream manager -> optional privacy/logo/timestamp -> UVC/RTSP -> SISO
setup in order. Feature switches also support running with all three disabled.
Applied status logs show allocation, source, peak and region/index counts.
Time synchronization remains the system's responsibility; the existing timestamp
helper counts uptime from the epoch before synchronization.

`tests/video_osd_privacy_test.c` provides an opt-in
`video_osd_privacy_geometry_test()` returning a failure count. It exercises
coordinate mapping, winding/concavity, invalid geometry, opaque composition and
H.264/HEVC block conflicts. Its static fixtures make the test non-reentrant.
`tests/video_osd_plan_test.c` additionally covers all 16 feature combinations,
slot pressure, same/different-color merges, conflict closure, H.264/HEVC and
cropped polygon coverage. `tests/video_osd_sync_test.c` adds revision-target,
zero-timeout/error propagation, empty-context removal and invalid/reused-start checks. These tests are
not included in production CMake; add them to an authorized test build to run them. Static checking does not replace these execution/board checks:

- H.264 UVC/RTSP and HEVC: RGB/alpha correctness, clipped boundaries, overlap and NN
  suppression/restoration, repeated polygon updates and timestamp refresh.
- Queue saturation, OSD index exhaustion, allocation failure and preprocessing
  rejection: no unmasked fallback, no early buffer reuse, recovery/retry works.
- Slow frame rates, stop/start cycles, pending IPC timeout and output-gate timestamp
  rollover: no stale startup frames or use-after-free.
- Measure AP update latency/heap and encoder bandwidth at each product resolution.

No firmware build or board verification is implied by this implementation.

Static review: the new component/test sources and V1 example were checked with
Cppcheck (C99, 32-bit model), AStyle and `git diff --check`. Broader integration
checking also reports pre-existing `%ld`/integer-type format warnings in
`module_video.c`; these unrelated logging statements were not changed. Cppcheck
without the full target build configuration does not establish compile/link or
runtime correctness. The opt-in C tests have not been compiled or executed.
