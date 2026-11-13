# Repository Guidelines

## Project Structure & Module Organization

This is a Realtek Ameba RTOS SDK copy focused on AmebaPro3 / RTL8735C.

- `component/`: firmware, drivers, FreeRTOS, networking, and bundled libraries.
- `component/soc/amebapro3/`: chip-specific HAL, boot code, video/audio support, and applications. `project/` holds CMake, Kconfig, linker scripts, and per-core configuration.
- `example/`: peripheral, networking, storage, USB, and SSL examples.
- `cmake/`: build helpers, toolchain definitions, and Kconfig tooling.
- `tools/`: Python CLI, image processing, flashing, and debug utilities.
- Tests live beside their components; video assets and sensor configuration belong under their existing media directories.

## Build, Test, and Development Commands

Follow `BUILD_LOCAL.md` for this macOS workspace; the root README describes an older Make workflow. Use the selected Realtek ASDK 12.3.1 build 4600 toolchain.

Run from the repository root:

```bash
export RTK_TOOLCHAIN_DIR="$HOME/rtk-toolchain"
source ./env.sh                 # Prepare tools and activate .venv; initial setup downloads dependencies
python ameba.py soc RTL8735C    # Select the target
python ameba.py menuconfig     # Configure firmware options
python ameba.py build          # Compile and package firmware
python ameba.py clean          # Remove selected target build products
```

Build intermediates go in `build_RTL8735C/build/`; final images go in `build_RTL8735C/`. Firmware runs on the target board. Use `python ameba.py monitor -h` for serial-monitor options.

## Coding Style & Naming Conventions

Match the surrounding module. Python uses four-space indentation and `snake_case` functions; C commonly uses tabs, uppercase macros, and existing SDK prefixes. Preserve copyright and SPDX headers. Apply the ISP subtree's `.clang-format` only within its scope; avoid formatting unrelated vendor code. No root-wide formatter or linter configuration is provided.

## Testing Guidelines

No root-wide test runner or coverage threshold is defined. Add focused regression checks beside the affected component, following existing `test_*.py` or `test_*.c` naming. Video analysis uses Python `unittest`:

```bash
cd component/soc/amebapro3/sw/media_example/ap_media_example/video_example_src/video_test_tool/video_validation_reporter
python -m unittest test_analyze_video
```

For firmware changes, build the affected configuration and verify behavior on hardware. Record board, configuration, commands, and serial output; distinguish compilation from board validation.

## Commit & Pull Request Guidelines

This copy contains no Git metadata, so commit conventions cannot be verified. Use concise imperative subjects, optionally prefixed with the component. PRs should describe the change, link applicable issues, list validation and hardware limitations, and include logs or screenshots when relevant. Exclude generated images, toolchains, virtual environments, and secrets.
