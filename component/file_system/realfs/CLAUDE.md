# REALFS — UI-oriented embedded file system

Implementation of the design in `/home/liang_cai/REALFS_design_clean.md`. O(1)
hash-bucket file lookup, extent data mapping, head-in-inode small-file storage,
and A/B copy-on-write metadata commit for power-loss safety.

## Layout of this directory

| File | Role |
|------|------|
| `realfs.h` | Public API, on-disk field layout, compile-time config, error codes |
| `realfs_bdev.h` | Block-device abstraction (NOR/SD/eMMC — rewritable blocks) |
| `realfs_erasedev.h` | Erase-device abstraction (route-B — seg_read/seg_prog/seg_erase; used by raw NAND and NOR@512B map) |
| `realfs.c` | **Portable core** — the single source of truth; contains both the rewritable-block (NOR-direct/SD/eMMC) and erase-aware (route-B) modes. Erase-aware geometry (page/block/seg) is derived from `edev` at mount, so the same path serves NAND (page 2048→block 2032) and NOR@512B map (page 512→block 496) |
| `port/realfs_bdev_file.{c,h}` | File/image-backed bdev (PC tools & tests only) |
| `port/realfs_bdev_sd.{c,h}` | SD-card bdev backend (firmware; 4 KB block) |
| `port/realfs_bdev_flash.{c,h}` | NOR-direct bdev backend (firmware; 4 KB block = erase unit) |
| `port/realfs_bdev_ram.{c,h}` | VFS RAM-disk bdev backend (firmware; plain bdev, no GC; block size from Kconfig) |
| `port/realfs_bdev_levelx_nor.{c,h}` | NOR via LevelX bdev backend (firmware; 512 B block; **legacy — double COW**, prefer NOR-map) |
| `port/realfs_erasedev_lbm.{c,h}` | **NAND erase-device backend via LBM** (firmware; RTL8730E 32MB NAND) |
| `port/realfs_erasedev_nor.{c,h}` | **NOR@512B erase-device backend** (firmware; route-B map, seg = 4 KB sector, 8×512 B pages; single-layer COW, no LevelX log) |
| `port/realfs_erasedev_sim.{c,h}` | route-B simulator erasedev (PC tests only; enforces program-once; parameterizable page/seg for NAND & NOR geometries) |
| `test/realfs_test.c` | Functional tests (NOR/SD mode) |
| `test/realfs_powerloss_test.c` | Crash-at-every-write-point COW safety tests (NOR/SD mode) |
| `test/realfs_ea_test.c` | Functional tests (route-B erase-aware mode, uses simulator). Geometry is `-D`-overridable; `make nor_test` runs the same suite under NOR@512B geometry (page 512 / block 496 / 8-page seg / 128 segs) |
| `test/Makefile` | PC build: `make test` (NOR/SD) + `make nand_test` (NAND geom) + `make nor_test` (NOR@512B geom) |
| `CMakeLists.txt` | Firmware build |
| `REALFS_NAND_ROUTE_B_DESIGN.md` | NAND 路线 B 设计文档(含实现状态、已知 bug 修复、性能结果) |

The VFS adapter lives in the **vfs** component: `vfs/vfs_realfs.{c,h}` (registers
`realfs_drv`, `vfs_type = VFS_REALFS`). The vfs build also pulls in
`port/realfs_bdev_sd.c` when `CONFIG_VFS_REALFS_INCLUDED` is set.

## Build & test

PC (host) — the same `realfs.c` that ships in firmware:
```bash
cd test && make test
```
Firmware: gated off by default; see `Kconfig` (`CONFIG_VFS_REALFS_INCLUDED`).
`realfs.c` depends only on `<stdint.h>`/`<string.h>` + `realfs_bdev.h` — **keep all
board/RTOS includes out of it** (guard with `REALFS_PC_BUILD` if ever unavoidable)
so firmware and PC tools never diverge in on-disk format.

## Firmware usage (VFS over SD)

Enable `CONFIG_VFS_REALFS_INCLUDED` (and SD host `CONFIG_SUPPORT_SDIOH`) in
menuconfig, then:

```c
#include "vfs.h"
#include "vfs_realfs.h"

vfs_init();
/* one-time, only when (re)formatting — DESTROYS data: */
realfs_sd_format(64);                 /* 64 hash buckets (power of two) */

/* mount under a prefix; SD interface only for now */
vfs_user_register("realfs", VFS_REALFS, VFS_INF_SD, VFS_REGION_4, VFS_RW);

/* then use stdio-style VFS paths "<prefix>:/..." */
FILE *f = fopen("realfs:/ui/home/icon.png", "w");
fwrite(buf, 1, len, f);
fclose(f);                            /* flush => one realfs_write() */
```

Adapter semantics (see `vfs/vfs_realfs.c`):
- **Read opens** stream straight through `realfs_read()`.
- **Write/append/update opens** ("w"/"a"/"r+"/"w+"/"a+") buffer the whole file
  in RAM and flush with one `realfs_write()` on `fflush`/`fclose` — matches
  REALFS's whole-file write model and the KB~hundreds-KB asset range.
- **Directories are virtual**: `mkdir`/`rmdir` are no-op successes; `readdir`
  is unsupported until the core gets an enumeration API.
- `rename` = read-old → write-new → delete-old.
- Block size is 4 KB on SD / NOR-direct (`RFS_BLOCK_SIZE`); on the VFS RAM disk
  (`port/realfs_bdev_ram.c`, `CONFIG_REALFS_RAM`) it is
  `CONFIG_VFS_RAM_REALFS_BLOCK_SIZE` (512/1024/2048/4096; 512 caps the disk at
  ~1 MB because the superblock must hold the bucket table). Static buffers stay
  sized by `REALFS_MAX_BLOCK_SIZE`, so a smaller block does not save RAM.
  One mounted `struct realfs` (~17 KB BSS) + per-open handle (read: ~4 KB inode
  buf; write: a buffer the size of the file).

## On-disk format (v02)

- All fields little-endian; access via offset helpers (`ld32`/`st32`), **never**
  by casting structs — the inode is a variable-length packed record
  (header → filename → extents[] → head_data) so a plain `struct` cast is wrong.
- CRC-32 = standard zlib (reflected, poly `0xEDB88320`, init/final `0xFFFFFFFF`).
  Self-check: `crc32("123456789") == 0xCBF43926`.
- Path hash = FNV-1a 32-bit over the path bytes, **excluding** the NUL, case
  sensitive, separators preserved. `fnv1a("") == 0x811C9DC5`.
- Superblock at block 0 (group A) and block N-1 (group B); bitmaps adjacent to
  each (A: `[1, 1+M)`, B: `[N-1-M, N-1)`); data region `[data_start, data_end)`.
  `M = ceil(N/8/bs)`. The active group is the valid superblock with the larger
  `seq`; commits write the inactive group then flip — active is always read-only.

These constants are frozen for `version=2`; `realfs_crc32`/`realfs_fnv1a` are exposed
so a PC tool can print matching vectors before trusting an image (design §10.2).

## Implemented (verified by tests)

- `realfs_format`, `realfs_mount`, `realfs_unmount`
- `realfs_open` / `realfs_close` / `realfs_read` (head + extent traversal, arbitrary
  offset/length, multi-block reads)
- `realfs_write` — create **and** atomic replace (update); iterative
  extent_count/head_size convergence, multi-extent fallback for fragmented free
  space (`REALFS_MAX_EXTENTS`)
- `realfs_delete`, `realfs_stat`
- Hash bucket overflow chains: append / prepend-on-full / compact-on-delete,
  with chain-prefix COW when a non-head block changes
- A/B COW commit; power-loss tested at every write point (write/update) — the
  committed state is never corrupted, new writes are all-or-nothing.

### Allocator note
The free-space allocator (`find_runs`) scans the active bitmap on demand
(one bitmap block cached at a time) and tracks per-transaction allocations/frees
as run lists applied at commit. Commit rewrites the **entire** inactive bitmap
(M blocks) from the active one, because A/B alternation leaves the inactive copy
two generations stale — the design's "write only dirty bitmap blocks"
optimization (§4.8) is intentionally **not** taken here for correctness. M is
tiny (1 for ≤256 MB, 8 for 1 GB), so commit stays O(M), independent of file count.

## Not yet implemented (design sections, follow-ups)

- `realfs_append` / `realfs_truncate` (§4.11–4.12)
- Online defragmentation / forward-migration (§4.10) and `max_contiguous_free`
- Bucket active-merge on delete (§4.5 Step 4) — currently skipped (chains stay
  short in practice; correctness unaffected)
- Batch commit `realfs_begin_batch`/`realfs_end_batch` (§9.1)
- Hot-bucket LRU cache (§9.2), `readdir`/`dir_hash` (§9.3)
- 256 B windowed bitmap RAM optimization (§4.8/§9.4) — core currently uses a
  single cached bitmap block (the "conservative" config) for simplicity
- More storage bdev backends: eMMC. (SD done: `port/realfs_bdev_sd.c`;
  NOR-direct done: `port/realfs_bdev_flash.c`;
  **raw NAND via LBM done**: `port/realfs_erasedev_lbm.c` + NAND erase-aware
  mode in `realfs.c` — see `REALFS_NAND_ROUTE_B_DESIGN.md` for design and
  `REALFS_PROGRESS.md §16` for test results;
  **NOR@512B map done (host-verified)**: `port/realfs_erasedev_nor.c` reuses the
  erase-aware core with page 512→block 496 — see `REALFS_PROGRESS.md §17`;
  on-board benchmarks pending a NOR SoC build.)
- VFS adapter `readdir`/directory listing (needs a core enumeration API);
  currently mkdir/rmdir are no-ops and readdir is unsupported.

## Current RAM footprint (per `struct realfs`)

`bucket_map` (`REALFS_MAX_BUCKETS`×4) + three `REALFS_MAX_BLOCK_SIZE` scratch buffers
(sb/io/io2) + one cached bitmap block ≈ ~17 KB at 256 buckets / 4 KB blocks.
This is larger than the design's 2.6 KB target (which assumes 512 B I/O buffers
and the 256 B windowed bitmap); shrinking it is a known optimization, not a
correctness issue. Each open handle holds a full 4 KB inode buffer.
