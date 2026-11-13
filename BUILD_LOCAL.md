# AmebaPro3 本機修改與 Build

更新日期：2026-11-13。主機為 macOS／Apple Silicon（Darwin arm64）。

## 工作目錄

- **後續修改與 build：** `/Users/kevinhuang/work/pro3_sdk_check`
- SDK 來源：`/Users/kevinhuang/Desktop/RL-7013 8735C Tasks/amebapro3_sdk_3afe21e`
- 舊版初始參考：`/Users/kevinhuang/Desktop/RL-7013 8735C Tasks/pro3_sdk`

此目錄是新版 SDK 的獨立完整副本，不是連到來源的 symlink 或共用檔案的 hard link。複製了 9,910 個檔案，共 353,169,693 bytes；複製後已用 rsync checksum 比對內容。後續請在此目錄修改，來源 SDK 留作參考。

## 指定工具鏈與本機路徑

專案 `component/soc/amebapro3/project/CMakeLists.txt` 指定 ASDK `12.3.1`；`cmake/toolchain/ameba-toolchain-asdk-12.3.1.cmake` 對 AmebaPro3 選用 build `4600`，macOS 版本使用 newlib。

| 項目 | 版本／位置 |
| --- | --- |
| GCC | Realtek ASDK-12.3.1 Build 4600；GCC 12.3.1 20260609 |
| 工具鏈根目錄 | `/Users/kevinhuang/rtk-toolchain/asdk-12.3.1-4600/darwin/newlib` |
| C compiler | `/Users/kevinhuang/rtk-toolchain/asdk-12.3.1-4600/darwin/newlib/bin/arm-none-eabi-gcc` |
| C++ compiler | 同一個 `bin/` 下的 `arm-none-eabi-g++` |
| objcopy／objdump／readelf／gdb | 同一個 `bin/` 下的 `arm-none-eabi-*` 工具 |
| 下載的工具鏈壓縮檔 | `/Users/kevinhuang/rtk-toolchain/asdk-12.3.1-darwin-newlib-build-4600-arm64.tar.bz2` |
| 下載來源與校驗紀錄 | `/Users/kevinhuang/rtk-toolchain/asdk-12.3.1-4600-install.json` |
| 本機 Python 3.13.2 | `/Library/Frameworks/Python.framework/Versions/3.13/bin/python3` |
| 本機 CMake 4.3.1 | `/opt/homebrew/bin/cmake` |
| 本機 Ninja 1.13.2 | `/opt/homebrew/bin/ninja` |

下載來源為 SDK 所列的 [Ameba-AIoT 官方 release](https://github.com/Ameba-AIoT/ameba-toolchain/releases/download/12.3.1_v4/asdk-12.3.1-darwin-newlib-build-4600-arm64.tar.bz2)。SHA-256 已與官方 release asset 公布的值比對一致：

```text
c4ce74684d7f9f11beb796d48caf09e7cfa9ac037ef19f5b312c7db6af4ce0f3
```

本機另有 GCC 9.3.1 與 Homebrew GCC 15.2.0；build 應沿用 SDK 選定的上述 Realtek 工具鏈。工具鏈位於預設搜尋路徑，不需要更改系統 GCC 或永久修改 PATH。

## 準備環境與 Build

在 Terminal 的 bash 或 zsh 中執行：

```bash
cd /Users/kevinhuang/work/pro3_sdk_check
export RTK_TOOLCHAIN_DIR=/Users/kevinhuang/rtk-toolchain
source ./env.sh
```

`env.sh` 會準備／載入 `~/rtk-toolchain/prebuilts-darwin-1.0.3/`，建立並啟用本副本的 `.venv/`，依 `tools/requirements.txt` 安裝 Python 套件。目前只安裝了上面的 ASDK；此副本尚未執行 `env.sh`，prebuilts 與 `.venv` 尚未建立。首次執行需要網路及額外下載。環境準備後使用的工具版本，以當次環境實際輸出為準。

選擇晶片並 build：

```bash
python ameba.py soc RTL8735C
python ameba.py build
```

需要調整專案組態時，在 build 前執行：

```bash
python ameba.py menuconfig
```

依 `tools/scripts/ameba_manager.py` 的路徑規則，RTL8735C 的 build 目錄為 `build_RTL8735C/build/`，最終映像輸出目錄為 `build_RTL8735C/`。具體產物仍依所選組態決定。

## 已驗證的範圍

- 官方壓縮檔 SHA-256 一致，已解壓安裝。
- `arm-none-eabi-gcc --version` 回報 Realtek ASDK-12.3.1 Build 4600。
- `arm-none-eabi-gcc -check-license` 回報 `License file is found.`。
- Cortex-M33 與 Cortex-A32 的最小 C 原始碼均可交叉編譯成 ARM ELF32 object。
- **尚未執行完整 SDK configure／compile／link／image packaging，也未上板測試。** 工具鏈可執行不等於完整 SDK 已 build 通過。
- 尚未實作前述 H.264／2160 寬／16-line slice 全 SRAM 修改；此副本保留原 SDK 行為。
