# Video Validation Reporter

Video Validation Reporter compares recorded video with the board parameter snapshot
and produces a standalone HTML validation report.

Use [analyze_video.py](analyze_video.py) to decode and inspect the downloaded MP4,
compare observable VID settings, and generate one standalone HTML report with per-frame details.
### First-time setup after cloning (Windows / Linux)

Supported baseline: x86-64 Windows 10/11 or Ubuntu 24.04, 64-bit Python
3.10-3.13 (Windows Python 3.11 and Ubuntu Python 3.12 tested).
Other Linux distributions need a compatible PyAV wheel (glibc >= 2.28) and
are not individually certified. Internet access to PyPI and GitHub is required
for setup. Analysis itself works offline. No SDK firmware build is involved.

Run the following commands **inside this `video_validation_reporter` directory**.

**Windows prerequisites:** install 64-bit Python, FFmpeg with `ffmpeg.exe` and
`ffprobe.exe` on PATH, and Visual Studio 2019 or newer / Build Tools with
"Desktop development with C++" and "C++ CMake tools for Windows".
The FFmpeg build must include libx264, libx265 and the trace_headers bitstream
filter; setup verifies them with real generated clips.

```powershell
python setup_analysis.py
.\.venv-windows\Scripts\python.exe analyze_video.py video_test_v1_000001.mp4
```

**Ubuntu / WSL prerequisites:**

```bash
sudo apt-get update
sudo apt-get install -y python3 python3-venv build-essential ffmpeg ca-certificates
python3 setup_analysis.py
./.venv-linux/bin/python analyze_video.py video_test_v1_000001.mp4
```

Setup creates a platform-specific virtual environment and installs pinned PyAV.
Linux also installs pinned CMake/Ninja; Windows uses the Visual Studio bundled
CMake/Ninja tools. It builds the HEVC QP adapter from hash-verified upstream
source and runs H.264/HEVC known-QP tests. Run setup again after changing Python
or dependencies. A failed setup is not a ready installation: fix the reported
prerequisite/download/build/test error first. Use the virtual-environment Python
shown above for every analysis; activation is optional.

Windows and WSL can share a checkout: their virtual environments and decoder
builds are separate. Do not copy Windows DLLs or Python packages to Linux.
Use `--hevc-qp-library PATH` only for a custom, matching-platform helper.

The board saves `video_test_v1_000001_params.expected.json` before capture. The analyzer automatically loads the matching snapshot and produces only `video_test_v1_000001.analysis.html`, including all checks, headers, metadata and per-frame data. Missing expectations or unavailable measurements are SKIP; any failed check makes the report FAIL, otherwise skipped checks make it PARTIAL. Use `--require-all` to return a nonzero exit code for PARTIAL as well as FAIL.

## Picture and block QP range verification

The HTML report now includes separate `picture_qp_range` and `block_qp_range`
checks using the captured `min_qp` / `max_qp`. Picture QP means every slice's
initial luma QP, not PPS base QP or a frame average. Block QP includes local QP
deltas. Bounds are inclusive and apply to all I/P pictures and their blocks.

H.264 uses PyAV/FFmpeg macroblock side data. HEVC needs the exact-QP decoder
adapter described in [hevc_qp_backend/README.md](hevc_qp_backend/README.md).
Missing dependencies or incomplete coverage are visibly SKIP; they cannot pass.
The report includes per-frame QP ranges, block counts, distribution and failing
block coordinates. The default workflow still produces only one HTML file.

```powershell
python analyze_video.py video_test_v1_000001.mp4
```

Run regression tests (including generated H.264/HEVC decoder tests when dependencies are available) with:

```powershell
python -B -m unittest discover -p "test_*.py"
```

### Files to commit and local outputs

Commit README, `.gitignore`, Python sources and tests,
`requirements-analysis.txt`, and the backend source/build/documentation
files. Do not commit virtual environments, downloaded third-party dependencies,
`hevc_qp_backend/build/`, Python caches, recordings or generated reports.
A fresh clone recreates all host dependencies with `setup_analysis.py`.

Store downloaded MP4 and matching `*_params.expected.json` together in
`recordings/` (ignored by Git). For example, pass
`recordings/video_test_v1_000001.mp4` to the analyzer; the HTML is written beside
it. These recordings are evidence, not installation dependencies.

Run all regression tests using the platform virtual-environment Python:

```powershell
.\.venv-windows\Scripts\python.exe -B -m unittest discover -p "test_*.py" -v
```

```bash
./.venv-linux/bin/python -B -m unittest discover -p 'test_*.py' -v
```

The board commands and recording/download workflow are documented in the
[parent test tool README](../README.md). Virtual environments are local, generated
files; after relocating a checkout, recreate them with `setup_analysis.py`.
