# Exact HEVC block-QP backend

Stock FFmpeg does not export HEVC coding-block QP maps. This small source adapter
exports each libde265 coding block's decoded luma QPY, its slice initial QPY,
coordinates, and size. It does not infer QP from pixels or colour overlays.

Use `python setup_analysis.py` (Windows) or `python3 setup_analysis.py` (Linux)
from the parent directory. See the parent README for prerequisites. The setup
installs PyAV 16.0.1 in a platform-specific virtual environment and runs the
native known-QP integration tests after building. The analyzer never downloads
packages automatically.

## Host decoder build

`build.py` dispatches to the Visual Studio build on Windows and CMake/Ninja on
Linux. Outputs are `build/windows/bin/libde265_qp.dll` and
`build/linux/bin/libde265_qp.so`, respectively. The analyzer chooses only its
current operating system's library. Use `--hevc-qp-library PATH` to override.
These are PC decoder builds, separate from all SDK firmware builds.

CMake downloads libde265 v1.0.16 with a pinned SHA-256 and adds `qp_export.cc`.
Do not substitute a stock libde265 library: the versioned `video_qp_abi` export
is required. Decoder calls run in a child process with the analyzer timeout.

libde265 is LGPL-3.0; retain its license/source and this adapter when distributing
compiled libraries. Setup downloads source and license under
`build/<platform>/_deps/libde265-src`. This repository distributes the adapter
and build recipe, not third-party binaries. Build outputs are ignored by Git.

## Interpretation

- QP is 8-bit **luma QPY**, not the mapped chroma QP or an average.
- Picture level means all slice initial QPY values in the picture. Multiple
  slices need not share one QP. A PPS initial QP alone is not the picture QP.
- All final coding-block QPY values (including block deltas) are checked against
  the same inclusive `min_qp` and `max_qp` bounds, including I and P pictures.
- The decoder PTS is matched to the independently probed MP4 frame PTS.
- PASS requires complete, non-overlapping coverage, all frames matched, clean
  decoding, both limits supplied, and no out-of-range QP.
- Any observed out-of-range value fails. Missing decoder/data/coverage is SKIP.
- PCM and transquant-bypass blocks do not have an ordinary quantizer; these
  prevent complete block verification rather than silently passing.
- Only progressive 8-bit 4:2:0 clips are supported by the initial PC workflow.
  Geometry mismatch/cropping prevents complete verification.
- The HTML lists per-frame extrema, pixel-area-weighted means, histograms,
  out-of-range counts and up to 64 offending block locations per frame. Counts
  include all violations; omitted-location counts are retained in full results.

References: [libde265](https://github.com/strukturag/libde265/tree/v1.0.16),
[FFmpeg public H.264 QP side-data semantics](https://github.com/FFmpeg/FFmpeg/blob/n8.0/libavutil/video_enc_params.h).
