#!/usr/bin/env python3
"""Generate a standalone AmebaPro3 AP/VP/NP memory usage report."""

from __future__ import annotations

import argparse
import html
import re
import sys
from collections import defaultdict
from dataclasses import dataclass, field
from datetime import datetime
from pathlib import Path
from typing import Iterable


CPU_ORDER = ("AP", "VP", "NP")

CPU_CONFIG = {
    "AP": {
        "display": "AP | CA32",
        "map": "build_RTL8735C/build/project_ap/image/text_image2.map",
        "linker": "component/soc/amebapro3/project/project_ap/ld/ameba_img2.ld",
        "regions": (
            "AP_DDR_RO",
            "AP_DDR",
            "AP_DDR_HEAP_EXT",
            "AP_DDR_STK",
            "AP_SSRAM_RET",
            "AP_PSRAM_RET",
        ),
        "accent": "#7c3aed",
    },
    "VP": {
        "display": "VP / MP | KM4",
        "map": "build_RTL8735C/build/project_vp/image/text_image2_ns.map",
        "linker": "component/soc/amebapro3/project/project_vp/ld/ameba_img2.ld",
        "regions": ("VP_PSRAM", "VP_DDR", "VP_DDR_HEAP_EXT", "VP_DTCM"),
        "accent": "#2563eb",
    },
    "NP": {
        "display": "NP | KM4",
        "map": "build_RTL8735C/build/project_np/image/text_image2_ns.map",
        "linker": "component/soc/amebapro3/project/project_np/ld/ameba_img2_all.ld",
        "regions": (
            "NP_SSRAM",
            "NP_SSRAM_NC",
            "NP_PSRAM",
            "NP_PSRAM_HEAP_EXT",
            "NP_SSRAM_MSP",
            "NP_SSRAM_MSP_S",
            "NP_DTCM_BSS",
            "NP_DTCM_NOINIT",
            "NP_DTCM_RTT",
        ),
        "accent": "#0f9f8f",
    },
}

LAYOUT_PATH = "build_RTL8735C/build/project_np/make/image2/layout.ld"

CATEGORY_ORDER = (
    "text",
    "rodata",
    "data",
    "bss",
    "data_copy",
    "metadata",
    "retention",
    "stack",
)

CATEGORY_INFO = {
    "text": (".text / vectors", "Executable code", "#2563eb"),
    "rodata": (".rodata / const", "Read-only constants, unwind data, and constructors", "#4f46e5"),
    "data": (".data", "Initialized writable runtime data", "#0891b2"),
    "bss": (".bss", "Zero-initialized runtime data", "#0d9488"),
    "data_copy": ("Data copy", "Load-image or VP PSRAM backup copy", "#8b5cf6"),
    "metadata": ("Metadata / padding", "Vector headers, MMU tables, entries, and alignment", "#64748b"),
    "retention": ("Retention", "Linker-allocated retention sections", "#d97706"),
    "stack": ("Stack", "Linker or platform-reserved stack", "#f59e0b"),
}

REGION_DESCRIPTIONS = {
    "AP_DDR_RO": "AP application code, read-only data, and the .data load image",
    "AP_DDR": "AP runtime .data/.bss followed by the FreeRTOS heap",
    "AP_DDR_HEAP_EXT": "AP optional heap extension",
    "AP_DDR_STK": "AP dedicated DDR stack region",
    "AP_SSRAM_RET": "AP SRAM retention",
    "AP_PSRAM_RET": "AP PSRAM retention with an FCS-reserved tail",
    "VP_PSRAM": "VP application code, read-only data, and data backup images",
    "VP_DDR": "VP runtime .data/.bss followed by the FreeRTOS heap",
    "VP_DDR_HEAP_EXT": "VP optional heap extension",
    "VP_DTCM": "VP DTCM with its upper half reserved for the MSP stack",
    "NP_SSRAM": "NP SRAM-only code and data",
    "NP_SSRAM_NC": "NP non-cacheable SRAM data",
    "NP_PSRAM": "NP primary application code/data followed by the FreeRTOS heap",
    "NP_PSRAM_HEAP_EXT": "NP optional heap extension",
    "NP_SSRAM_MSP": "NP non-secure MSP platform reservation",
    "NP_SSRAM_MSP_S": "NP secure ROM MSP platform reservation",
    "NP_DTCM_BSS": "NP DTCM ROM BSS partition",
    "NP_DTCM_NOINIT": "NP DTCM no-init partition",
    "NP_DTCM_RTT": "NP DTCM RTT partition",
}

APP_GUIDANCE = {
    "AP": {
        "primary_region": "AP_DDR_RO",
        "runtime_region": "AP_DDR",
        "primary_sections": "Normal customer .text and .rodata",
        "watch": "Watch the AP_DDR_RO row and its Largest Contiguous value.",
        "impact": (
            "New .text/.rodata consumes AP_DDR_RO headroom. New .data/.bss "
            "consumes AP_DDR and reduces the AP heap; .data also requires an "
            "load-image copy in AP_DDR_RO."
        ),
        "capacity_mode": "headroom",
    },
    "VP": {
        "primary_region": "VP_PSRAM",
        "runtime_region": "VP_DDR",
        "primary_sections": "Normal customer .text and .rodata",
        "watch": "Watch the VP_PSRAM row and its Largest Contiguous value.",
        "impact": (
            "New .text/.rodata consumes VP_PSRAM headroom. New .data/.bss "
            "consumes VP_DDR and reduces the VP heap; data backup copies also "
            "consume VP_PSRAM."
        ),
        "capacity_mode": "headroom",
    },
    "NP": {
        "primary_region": "NP_PSRAM",
        "runtime_region": "NP_PSRAM",
        "primary_sections": "Normal customer .text, .rodata, .data, and .bss",
        "watch": "Watch NP_PSRAM together with the NP FreeRTOS heap.",
        "impact": (
            "NP_PSRAM is shared by the normal application image and heap. "
            "Application growth moves __heap_start__ upward and reduces raw "
            "heap approximately byte-for-byte. Only explicit .sram.only.* "
            "sections go to NP_SSRAM."
        ),
        "capacity_mode": "heap_shared",
    },
}

IMAGE_CATEGORIES = {
    "text",
    "rodata",
    "data",
    "bss",
    "data_copy",
    "metadata",
    "retention",
}


class ReportError(RuntimeError):
    pass


@dataclass(frozen=True)
class MemoryRegion:
    name: str
    origin: int
    length: int
    attributes: str

    @property
    def end(self) -> int:
        return self.origin + self.length


@dataclass
class OutputSection:
    name: str
    address: int
    size: int
    load_address: int | None = None
    inputs: dict[str, int] = field(default_factory=lambda: defaultdict(int))

    @property
    def end(self) -> int:
        return self.address + self.size


@dataclass
class ParsedMap:
    path: Path
    regions: dict[str, MemoryRegion]
    sections: list[OutputSection]
    symbols: dict[str, int]
    warnings: list[str]


@dataclass
class Allocation:
    name: str
    address: int
    size: int
    region: str
    breakdown: dict[str, int]
    kind: str = "runtime"

    @property
    def end(self) -> int:
        return self.address + self.size


@dataclass
class ReservedInterval:
    start: int
    end: int
    label: str
    role: str

    @property
    def size(self) -> int:
        return self.end - self.start


@dataclass
class RegionStats:
    region: MemoryRegion
    allocations: list[Allocation]
    heap: list[ReservedInterval]
    fixed: list[ReservedInterval]
    image_bytes: int
    heap_bytes: int
    fixed_bytes: int
    unallocated_bytes: int
    largest_free_block: int


@dataclass
class CpuReport:
    cpu: str
    parsed: ParsedMap
    allocations: list[Allocation]
    region_stats: list[RegionStats]
    section_totals: dict[str, int]
    heap_start: int
    heap_end: int
    heap_size: int
    heap_regions: list[tuple[str, int]]
    warnings: list[str]


MEMORY_RE = re.compile(
    r"^(\S+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s*(\S*)$"
)
OUTPUT_INLINE_RE = re.compile(
    r"^(\.[^\s]+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)"
    r"(?:\s+load address\s+(0x[0-9a-fA-F]+))?(?:\s.*)?$"
)
OUTPUT_NAME_RE = re.compile(r"^(\.[^\s]+)\s*$")
OUTPUT_CONT_RE = re.compile(
    r"^\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)"
    r"(?:\s+load address\s+(0x[0-9a-fA-F]+))?\s*$"
)
INPUT_INLINE_RE = re.compile(
    r"^ (?P<name>\.[^\s]+|COMMON|\*fill\*)\s+"
    r"(?P<address>0x[0-9a-fA-F]+)\s+(?P<size>0x[0-9a-fA-F]+)(?:\s+.*)?$"
)
INPUT_NAME_RE = re.compile(r"^ (?P<name>\.[^\s]+|COMMON)\s*$")
INPUT_CONT_RE = re.compile(
    r"^\s+(?P<address>0x[0-9a-fA-F]+)\s+"
    r"(?P<size>0x[0-9a-fA-F]+)\s+.+$"
)
SYMBOL_RE = re.compile(
    r"^\s*(0x[0-9a-fA-F]+)\s+([A-Za-z_][A-Za-z0-9_]*)\s*="
)


def parse_int(value: str) -> int:
    return int(value, 16)


def parse_map(path: Path) -> ParsedMap:
    regions: dict[str, MemoryRegion] = {}
    sections: list[OutputSection] = []
    symbols: dict[str, int] = {}
    warnings: list[str] = []
    in_memory = False
    in_linker_map = False
    current: OutputSection | None = None
    pending_output: str | None = None
    pending_input: str | None = None

    with path.open("r", encoding="utf-8", errors="replace") as source:
        for raw_line in source:
            line = raw_line.rstrip("\r\n")
            stripped = line.strip()

            if stripped == "Memory Configuration":
                in_memory = True
                continue
            if stripped == "Linker script and memory map":
                in_memory = False
                in_linker_map = True
                continue

            if in_memory:
                match = MEMORY_RE.match(line)
                if match and match.group(1) != "Name" and match.group(1) != "*default*":
                    name, origin, length, attributes = match.groups()
                    regions[name] = MemoryRegion(
                        name, parse_int(origin), parse_int(length), attributes
                    )
                continue

            if not in_linker_map:
                continue
            if stripped == "Cross Reference Table":
                break

            symbol_match = SYMBOL_RE.match(line)
            if symbol_match:
                symbols[symbol_match.group(2)] = parse_int(symbol_match.group(1))

            if pending_output:
                continuation = OUTPUT_CONT_RE.match(line)
                if continuation:
                    address, size, load_address = continuation.groups()
                    current = OutputSection(
                        pending_output,
                        parse_int(address),
                        parse_int(size),
                        parse_int(load_address) if load_address else None,
                    )
                    sections.append(current)
                    pending_output = None
                    pending_input = None
                    continue
                if line and not line[0].isspace():
                    # GNU ld prints an empty output section as only its name and
                    # wildcard body, without an address/size line.
                    pending_output = None

            if pending_input:
                continuation = INPUT_CONT_RE.match(line)
                if continuation and current:
                    current.inputs[pending_input] += parse_int(
                        continuation.group("size")
                    )
                    pending_input = None
                    continue
                if stripped:
                    pending_input = None

            if line and not line[0].isspace():
                inline_output = OUTPUT_INLINE_RE.match(line)
                if inline_output:
                    name, address, size, load_address = inline_output.groups()
                    current = OutputSection(
                        name,
                        parse_int(address),
                        parse_int(size),
                        parse_int(load_address) if load_address else None,
                    )
                    sections.append(current)
                    pending_input = None
                    continue

                output_name = OUTPUT_NAME_RE.match(line)
                if output_name:
                    pending_output = output_name.group(1)
                    current = None
                    pending_input = None
                    continue

            if current:
                inline_input = INPUT_INLINE_RE.match(line)
                if inline_input:
                    current.inputs[inline_input.group("name")] += parse_int(
                        inline_input.group("size")
                    )
                    continue

                input_name = INPUT_NAME_RE.match(line)
                if input_name:
                    pending_input = input_name.group("name")

    if not regions:
        raise ReportError(f"Memory Configuration not found: {path}")
    if not sections:
        raise ReportError(f"Linker output sections not found: {path}")
    return ParsedMap(path, regions, sections, symbols, warnings)


def output_category(section_name: str, cpu: str) -> str | None:
    name = section_name.lower()
    if cpu == "AP" and name == ".text":
        return None
    if "heap" in name:
        return "heap"
    if "stack" in name:
        return "stack"
    if name.startswith(".ret"):
        return "retention"
    if "bkup" in name and "data" in name:
        return "data_copy"
    if "bss" in name:
        return "bss"
    if (
        "rodata" in name
        or name.startswith(".arm.extab")
        or name.startswith(".arm.exidx")
        or name in {
            ".ctors",
            ".dtors",
            ".preinit_array",
            ".init_array",
            ".fini_array",
        }
    ):
        return "rodata"
    if "data" in name:
        return "data"
    if "text" in name or "vector" in name or name in {".boot", ".init", ".fini"}:
        return "text"
    return "metadata"


def input_category(input_name: str, output_name: str) -> str:
    name = input_name.lower()
    output = output_name.lower()
    if name == "common" or "bss" in name:
        return "bss"
    if (
        "rodata" in name
        or name.startswith(".arm.extab")
        or name.startswith(".arm.exidx")
        or name in {
            ".eh_frame",
            ".ctors",
            ".dtors",
            ".preinit_array",
            ".init_array",
            ".fini_array",
            ".cmd.table.data",
        }
    ):
        return "rodata"
    if "text" in name or "vector" in name or name in {
        ".boot",
        ".init",
        ".fini",
        ".glue_7",
        ".glue_7t",
        ".vfp11_veneer",
    }:
        return "text"
    if "data" in name:
        return "rodata" if output == ".text" else "data"
    return "metadata"


def section_breakdown(
    section: OutputSection,
    cpu: str,
    warnings: list[str],
    symbols: dict[str, int],
) -> dict[str, int]:
    category = output_category(section.name, cpu)
    if category is not None:
        return {category: section.size} if section.size else {}

    text_end = symbols.get("__text_end__")
    if text_end is not None and section.address <= text_end <= section.end:
        # AP deliberately merges executable input sections, command tables,
        # .rodata, glue and init/fini into one .text output section. The linker
        # symbol is the exact boundary after all .text* inputs. Count vectors,
        # boot code and alignment before that boundary as executable footprint;
        # the remaining read-only tail is reported as rodata/const footprint.
        text_size = text_end - section.address
        return {
            "text": text_size,
            "rodata": section.size - text_size,
        }

    totals: dict[str, int] = defaultdict(int)
    for input_name, size in section.inputs.items():
        totals[input_category(input_name, section.name)] += size

    accounted = sum(totals.values())
    if accounted < section.size:
        totals["metadata"] += section.size - accounted
    elif accounted > section.size:
        warnings.append(
            f"{section.name} input sections total {accounted} B, which exceeds "
            f"the output size {section.size} B; category totals were truncated"
        )
        overflow = accounted - section.size
        for key in ("metadata", "rodata", "text", "data", "bss"):
            reduction = min(totals.get(key, 0), overflow)
            totals[key] -= reduction
            overflow -= reduction
            if not overflow:
                break
    return {key: value for key, value in totals.items() if value}


def memory_class(region_name: str) -> str:
    if "DTCM" in region_name or "ITCM" in region_name:
        return "TCM"
    if "SSRAM" in region_name:
        return "SRAM"
    if "VSRAM" in region_name:
        return "VSRAM"
    if "PSRAM" in region_name:
        return "PSRAM"
    if "DDR" in region_name:
        return "DDR"
    if "XIP" in region_name:
        return "XIP"
    return "Memory"


def containing_region(
    address: int, size: int, regions: Iterable[MemoryRegion]
) -> MemoryRegion | None:
    for region in regions:
        if region.length == 0:
            continue
        if address >= region.origin and address + size <= region.end:
            return region
    return None


def merge_intervals(intervals: Iterable[tuple[int, int]]) -> list[tuple[int, int]]:
    ordered = sorted((start, end) for start, end in intervals if end > start)
    merged: list[list[int]] = []
    for start, end in ordered:
        if not merged or start > merged[-1][1]:
            merged.append([start, end])
        else:
            merged[-1][1] = max(merged[-1][1], end)
    return [(start, end) for start, end in merged]


def interval_total(intervals: Iterable[tuple[int, int]]) -> int:
    return sum(end - start for start, end in merge_intervals(intervals))


def free_intervals(
    region: MemoryRegion, occupied: Iterable[tuple[int, int]]
) -> list[tuple[int, int]]:
    clipped = []
    for start, end in occupied:
        start = max(start, region.origin)
        end = min(end, region.end)
        if end > start:
            clipped.append((start, end))
    result = []
    cursor = region.origin
    for start, end in merge_intervals(clipped):
        if start > cursor:
            result.append((cursor, start))
        cursor = max(cursor, end)
    if cursor < region.end:
        result.append((cursor, region.end))
    return result


def fixed_reservations(cpu: str, parsed: ParsedMap) -> list[ReservedInterval]:
    fixed: list[ReservedInterval] = []
    if cpu == "AP":
        fcs = parsed.regions.get("AP_PSRAM_FCS_RET")
        if fcs and fcs.length:
            fixed.append(
                ReservedInterval(fcs.origin, fcs.end, "AP FCS retention", "fixed")
            )
    elif cpu == "VP":
        dtcm = parsed.regions.get("VP_DTCM")
        stack_size = parsed.symbols.get("__vp_stack_size__", 0)
        stack_start = parsed.symbols.get("__vp_msp_limit__")
        if dtcm and stack_size and stack_start is not None:
            fixed.append(
                ReservedInterval(
                    stack_start,
                    min(stack_start + stack_size, dtcm.end),
                    "VP MSP stack",
                    "stack",
                )
            )
    elif cpu == "NP":
        labels = {
            "NP_SSRAM_MSP": "NP non-secure MSP",
            "NP_SSRAM_MSP_S": "NP secure ROM MSP",
            "NP_DTCM_BSS": "NP ROM DTCM BSS",
            "NP_DTCM_NOINIT": "NP DTCM no-init",
            "NP_DTCM_RTT": "NP DTCM RTT",
        }
        for region_name, label in labels.items():
            region = parsed.regions.get(region_name)
            if region and region.length:
                fixed.append(
                    ReservedInterval(region.origin, region.end, label, "fixed")
                )
    return fixed


def analyze_cpu(cpu: str, parsed: ParsedMap) -> CpuReport:
    config = CPU_CONFIG[cpu]
    warnings = list(parsed.warnings)
    selected_regions = [
        parsed.regions[name]
        for name in config["regions"]
        if name in parsed.regions and parsed.regions[name].length > 0
    ]
    missing_regions = [name for name in config["regions"] if name not in parsed.regions]
    if missing_regions:
        raise ReportError(
            f"{cpu} map is missing memory regions: {', '.join(missing_regions)}"
        )

    allocations: list[Allocation] = []
    for section in parsed.sections:
        if section.size == 0:
            continue
        region = containing_region(section.address, section.size, selected_regions)
        if region:
            breakdown = section_breakdown(section, cpu, warnings, parsed.symbols)
            if "heap" not in breakdown:
                allocations.append(
                    Allocation(
                        section.name,
                        section.address,
                        section.size,
                        region.name,
                        breakdown,
                    )
                )

            runtime_categories = set(breakdown)
            if (
                section.load_address is not None
                and section.load_address != section.address
                and not runtime_categories.intersection(
                    {"bss", "heap", "stack", "retention"}
                )
            ):
                load_region = containing_region(
                    section.load_address, section.size, selected_regions
                )
                if load_region:
                    allocations.append(
                        Allocation(
                            f"LOAD_COPY({section.name})",
                            section.load_address,
                            section.size,
                            load_region.name,
                            {"data_copy": section.size},
                            "load copy",
                        )
                    )

    try:
        heap_start = parsed.symbols["__heap_start__"]
        heap_end = parsed.symbols["__heap_end__"]
    except KeyError as error:
        raise ReportError(
            f"{cpu} map is missing heap symbol: {error.args[0]}"
        ) from error
    if heap_end < heap_start:
        raise ReportError(f"{cpu} heap end is below heap start")
    heap_size = heap_end - heap_start

    heap_intervals: list[ReservedInterval] = []
    heap_regions: list[tuple[str, int]] = []
    for region in selected_regions:
        start = max(heap_start, region.origin)
        end = min(heap_end, region.end)
        if end > start:
            heap_intervals.append(
                ReservedInterval(start, end, "FreeRTOS heap_5", "heap")
            )
            heap_regions.append((region.name, end - start))
    covered_heap = sum(size for _, size in heap_regions)
    if covered_heap != heap_size:
        warnings.append(
            f"{heap_size - covered_heap} B of the heap range is outside the "
            "selected report regions"
        )

    fixed = fixed_reservations(cpu, parsed)
    region_stats: list[RegionStats] = []
    for region in selected_regions:
        region_allocations = [item for item in allocations if item.region == region.name]
        region_heap = [
            item
            for item in heap_intervals
            if item.start >= region.origin and item.end <= region.end
        ]
        region_fixed = [
            item
            for item in fixed
            if item.start >= region.origin and item.end <= region.end
        ]

        image_intervals = [
            (item.address, item.end)
            for item in region_allocations
            if set(item.breakdown).intersection(IMAGE_CATEGORIES)
        ]
        stack_intervals = [
            (item.address, item.end)
            for item in region_allocations
            if "stack" in item.breakdown
        ]
        heap_ranges = [(item.start, item.end) for item in region_heap]
        fixed_ranges = [(item.start, item.end) for item in region_fixed] + stack_intervals
        occupied = image_intervals + heap_ranges + fixed_ranges

        image_bytes = interval_total(image_intervals)
        heap_bytes = interval_total(heap_ranges)
        fixed_bytes = interval_total(fixed_ranges)
        all_used = interval_total(occupied)
        if image_bytes + heap_bytes + fixed_bytes != all_used:
            warnings.append(
                f"{region.name} contains overlapping image, heap, or fixed allocations"
            )
        free = free_intervals(region, occupied)
        region_stats.append(
            RegionStats(
                region,
                region_allocations,
                region_heap,
                region_fixed,
                image_bytes,
                heap_bytes,
                fixed_bytes,
                max(0, region.length - all_used),
                max((end - start for start, end in free), default=0),
            )
        )

    section_totals: dict[str, int] = defaultdict(int)
    for allocation in allocations:
        for category, size in allocation.breakdown.items():
            section_totals[category] += size

    return CpuReport(
        cpu,
        parsed,
        allocations,
        region_stats,
        dict(section_totals),
        heap_start,
        heap_end,
        heap_size,
        heap_regions,
        warnings,
    )


def validate_memory_mode(reports: list[CpuReport]) -> None:
    for report in reports:
        ddr = report.parsed.regions.get("DDR")
        psram = report.parsed.regions.get("PSRAM")
        if not ddr or ddr.origin < 0x80000000:
            raise ReportError(
                f"{report.cpu} map does not use the external-DDR-enabled layout; "
                "this skill requires DDR to be enabled"
            )
        if not psram or psram.length == 0:
            raise ReportError(
                f"{report.cpu} map has no usable PSRAM; this skill requires "
                "PSRAM to be enabled"
            )

    by_cpu = {report.cpu: report for report in reports}
    np_report = by_cpu["NP"]
    np_psram = np_report.parsed.regions["NP_PSRAM"]
    if not (np_psram.origin <= np_report.heap_start < np_psram.end):
        raise ReportError(
            "NP heap is outside NP_PSRAM and conflicts with the PSRAM-enabled assumption"
        )
    vp_report = by_cpu.get("VP")
    if vp_report and not any(
        item.region == "VP_PSRAM" for item in vp_report.allocations
    ):
        raise ReportError(
            "VP image has no VP_PSRAM allocation and conflicts with the "
            "PSRAM-enabled assumption"
        )


def consistency_warnings(reports: list[CpuReport]) -> list[str]:
    warnings: list[str] = []
    critical = (
        "PSRAM",
        "DDR",
        "NP_PSRAM",
        "VP_PSRAM",
        "AP_DDR_RO",
        "AP_DDR",
        "VP_DDR",
    )
    baseline = reports[0]
    for name in critical:
        expected = baseline.parsed.regions.get(name)
        if not expected:
            continue
        for report in reports[1:]:
            actual = report.parsed.regions.get(name)
            if not actual or (actual.origin, actual.length) != (
                expected.origin,
                expected.length,
            ):
                warnings.append(
                    f"{name} differs across the three maps; verify that all maps "
                    "come from the same configuration and build"
                )
                break
    return warnings


def format_bytes(value: int) -> str:
    if value >= 1024 * 1024:
        return f"{value / (1024 * 1024):.2f} MiB"
    if value >= 1024:
        return f"{value / 1024:.2f} KiB"
    return f"{value} B"


def format_size_html(value: int) -> str:
    return (
        f"<span class=\"size-main\">{html.escape(format_bytes(value))}</span>"
        f"<span class=\"size-exact\">{value:,} B</span>"
    )


def format_address(value: int) -> str:
    width = 16 if value > 0xFFFFFFFF else 8
    return f"0x{value:0{width}X}"


def relpath(path: Path, root: Path) -> str:
    try:
        return path.relative_to(root).as_posix()
    except ValueError:
        return str(path)


def category_breakdown_text(breakdown: dict[str, int]) -> str:
    parts = []
    for category in CATEGORY_ORDER:
        size = breakdown.get(category, 0)
        if size:
            parts.append(f"{CATEGORY_INFO[category][0]} {format_bytes(size)}")
    return " | ".join(parts) or "None"


def render_bar(stats: RegionStats) -> str:
    length = stats.region.length
    if not length:
        return ""
    segments = (
        ("image", stats.image_bytes, "Image / static"),
        ("heap", stats.heap_bytes, "FreeRTOS heap"),
        ("fixed", stats.fixed_bytes, "Stack / platform reserved"),
        ("free", stats.unallocated_bytes, "Unallocated"),
    )
    return "".join(
        f'<span class="bar-{kind}" style="width:{value * 100 / length:.5f}%" '
        f'title="{label}: {html.escape(format_bytes(value))}"></span>'
        for kind, value, label in segments
        if value
    )


def primary_region_stats(report: CpuReport) -> RegionStats:
    primary_region = APP_GUIDANCE[report.cpu]["primary_region"]
    return next(
        stats for stats in report.region_stats if stats.region.name == primary_region
    )


def application_capacity(report: CpuReport) -> tuple[str, int, str]:
    guidance = APP_GUIDANCE[report.cpu]
    stats = primary_region_stats(report)
    if guidance["capacity_mode"] == "heap_shared":
        return (
            "Raw heap shared with application growth",
            report.heap_size,
            "Application growth reduces this heap",
        )
    return (
        "Largest contiguous application headroom",
        stats.largest_free_block,
        f"Unallocated in {stats.region.name}",
    )


def render_application_focus(report: CpuReport) -> str:
    guidance = APP_GUIDANCE[report.cpu]
    capacity_label, capacity_value, capacity_hint = application_capacity(report)
    return f"""
    <div class="app-focus">
      <div class="app-focus-primary">
        <span class="eyebrow">Primary customer application code region</span>
        <strong>{html.escape(guidance['primary_region'])}</strong>
        <p>{html.escape(guidance['primary_sections'])}</p>
      </div>
      <div class="app-focus-stat">
        <span>{html.escape(capacity_label)}</span>
        <strong>{html.escape(format_bytes(capacity_value))}</strong>
        <small>{html.escape(capacity_hint)}</small>
      </div>
      <div class="app-focus-guidance">
        <b>{html.escape(guidance['watch'])}</b>
        <p>{html.escape(guidance['impact'])}</p>
        <span>Runtime writable region: <code>{html.escape(guidance['runtime_region'])}</code></span>
      </div>
    </div>
    """


def render_cpu_summary(report: CpuReport) -> str:
    config = CPU_CONFIG[report.cpu]
    guidance = APP_GUIDANCE[report.cpu]
    capacity_label, capacity_value, capacity_hint = application_capacity(report)
    return f"""
    <article class="cpu-summary" style="--accent:{config['accent']}">
      <div class="cpu-title"><span>{html.escape(config['display'])}</span><b>{report.cpu}</b></div>
      <div class="app-region-callout">
        <span>If you inspect only one region</span>
        <strong>{html.escape(guidance['primary_region'])}</strong>
        <small>{html.escape(guidance['primary_sections'])}</small>
      </div>
      <div class="summary-stat"><span>{html.escape(capacity_label)}</span><strong>{format_bytes(capacity_value)}</strong></div>
      <div class="summary-stat heap-highlight"><span>FreeRTOS raw heap</span><strong>{format_bytes(report.heap_size)}</strong></div>
      <div class="summary-foot">{html.escape(capacity_hint)}. Runtime writable data: <b>{html.escape(guidance['runtime_region'])}</b>.</div>
    </article>
    """


def render_metric(label: str, value: int, hint: str) -> str:
    return f"""
    <div class="metric">
      <span>{html.escape(label)}</span>
      <strong>{html.escape(format_bytes(value))}</strong>
      <small>{html.escape(hint)}</small>
    </div>
    """


def render_section_table(report: CpuReport) -> str:
    rows = []
    for category in CATEGORY_ORDER:
        total = report.section_totals.get(category, 0)
        if total == 0 and category not in {"text", "rodata", "data", "bss"}:
            continue
        placements: dict[str, int] = defaultdict(int)
        section_names: list[str] = []
        for allocation in report.allocations:
            size = allocation.breakdown.get(category, 0)
            if size:
                placements[allocation.region] += size
                section_names.append(allocation.name)
        placement_html = "<br>".join(
            f"<b>{html.escape(name)}</b> <span class=\"muted\">({memory_class(name)})</span> | {html.escape(format_bytes(size))}"
            for name, size in placements.items()
        ) or "None"
        unique_sections = list(dict.fromkeys(section_names))
        section_hint = ", ".join(unique_sections[:4])
        if len(unique_sections) > 4:
            section_hint += f", plus {len(unique_sections) - 4} more"
        label, description, color = CATEGORY_INFO[category]
        rows.append(
            f"""
            <tr>
              <td><span class="section-dot" style="--dot:{color}"></span><b>{html.escape(label)}</b><small>{html.escape(description)}</small></td>
              <td>{placement_html}<small>{html.escape(section_hint)}</small></td>
              <td class="num">{format_size_html(total)}</td>
            </tr>
            """
        )
    return "".join(rows)


def render_region_table(report: CpuReport) -> str:
    rows = []
    for stats in report.region_stats:
        region = stats.region
        used = region.length - stats.unallocated_bytes
        percentage = used * 100 / region.length if region.length else 0
        rows.append(
            f"""
            <tr>
              <td>
                <b>{html.escape(region.name)}</b>
                <small>{html.escape(REGION_DESCRIPTIONS.get(region.name, ''))}</small>
              </td>
              <td><span class="memory-pill">{memory_class(region.name)}</span></td>
              <td class="address"><code>{format_address(region.origin)}</code><small>to {format_address(region.end)}</small></td>
              <td class="num">{format_size_html(region.length)}</td>
              <td class="num">{format_size_html(stats.image_bytes)}</td>
              <td class="num">{format_size_html(stats.heap_bytes)}</td>
              <td class="num">{format_size_html(stats.fixed_bytes)}</td>
              <td class="num">{format_size_html(stats.unallocated_bytes)}<small>Largest contiguous: {html.escape(format_bytes(stats.largest_free_block))}</small></td>
              <td class="usage-cell">
                <div class="usage-bar">{render_bar(stats)}</div>
                <small>{percentage:.1f}% committed</small>
              </td>
            </tr>
            """
        )
    return "".join(rows)


def render_output_details(report: CpuReport) -> str:
    rows = []
    for allocation in sorted(report.allocations, key=lambda item: (item.region, item.address)):
        rows.append(
            f"""
            <tr>
              <td><code>{html.escape(allocation.name)}</code></td>
              <td><b>{html.escape(allocation.region)}</b><small>{html.escape(memory_class(allocation.region))} | {html.escape(allocation.kind.upper())}</small></td>
              <td class="address"><code>{format_address(allocation.address)}</code></td>
              <td class="num">{format_size_html(allocation.size)}</td>
              <td>{html.escape(category_breakdown_text(allocation.breakdown))}</td>
            </tr>
            """
        )
    return "".join(rows)


def render_heap(report: CpuReport, root: Path) -> str:
    config = CPU_CONFIG[report.cpu]
    backing = " + ".join(
        f"{name} {format_bytes(size)}" for name, size in report.heap_regions
    ) or "Outside the selected regions"
    base_region = next(
        (
            stats.region
            for stats in report.region_stats
            if stats.region.origin <= report.heap_start <= stats.region.end
        ),
        None,
    )
    ext_names = [name for name in config["regions"] if name.endswith("HEAP_EXT")]
    ext_text = ", ".join(
        f"{name}={format_bytes(report.parsed.regions[name].length)}"
        for name in ext_names
        if name in report.parsed.regions
    )
    return f"""
    <div class="heap-box">
      <div>
        <span class="eyebrow">FreeRTOS heap_5 raw region</span>
        <div class="formula"><code>{format_address(report.heap_end)} - {format_address(report.heap_start)}</code><strong>= {format_bytes(report.heap_size)}</strong></div>
        <p><code>__heap_start__</code> = {format_address(report.heap_start)} | <code>__heap_end__</code> = {format_address(report.heap_end)}</p>
      </div>
      <dl>
        <div><dt>Backing region</dt><dd>{html.escape(backing)}</dd></div>
        <div><dt>Heap base</dt><dd>{html.escape(base_region.name if base_region else 'unknown')}</dd></div>
        <div><dt>Extension</dt><dd>{html.escape(ext_text or 'none')}</dd></div>
      </dl>
    </div>
    """


def render_cpu_section(report: CpuReport, root: Path) -> str:
    config = CPU_CONFIG[report.cpu]
    code_ro = report.section_totals.get("text", 0) + report.section_totals.get(
        "rodata", 0
    )
    runtime_rw = report.section_totals.get("data", 0) + report.section_totals.get(
        "bss", 0
    )
    copies = report.section_totals.get("data_copy", 0)
    largest = max(report.region_stats, key=lambda item: item.largest_free_block)
    map_mtime = datetime.fromtimestamp(report.parsed.path.stat().st_mtime).astimezone()
    warning_html = ""
    if report.warnings:
        warning_html = "<div class=\"warning\"><b>Parser / layout warnings</b><ul>" + "".join(
            f"<li>{html.escape(item)}</li>" for item in dict.fromkeys(report.warnings)
        ) + "</ul></div>"

    return f"""
    <section class="cpu-section" id="{report.cpu.lower()}" style="--accent:{config['accent']}">
      <header class="section-header">
        <div><span class="eyebrow">CPU memory analysis</span><h2>{html.escape(config['display'])}</h2></div>
        <div class="build-meta"><code>{html.escape(relpath(report.parsed.path, root))}</code><span>Map | {map_mtime.strftime('%Y-%m-%d %H:%M:%S UTC%z')}</span></div>
      </header>

      {render_application_focus(report)}

      <div class="metrics">
        {render_metric('Code + read-only', code_ro, '.text + .rodata physical footprint')}
        {render_metric('Runtime RW', runtime_rw, '.data + .bss runtime physical footprint')}
        {render_metric('Data copies', copies, 'Load-image / VP backup physical copies')}
        {render_metric('FreeRTOS raw heap', report.heap_size, '__heap_end__ - __heap_start__')}
        {render_metric('Largest unallocated block', largest.largest_free_block, largest.region.name)}
      </div>

      <h3>Core section placement</h3>
      <div class="table-wrap"><table class="section-table">
        <thead><tr><th>Section type</th><th>Physical placement</th><th class="num">Size</th></tr></thead>
        <tbody>{render_section_table(report)}</tbody>
      </table></div>

      <h3>Memory region usage and customer headroom</h3>
      <div class="legend"><span class="image">Image / static</span><span class="heap">FreeRTOS heap</span><span class="fixed">Stack / platform reserved</span><span class="free">Unallocated</span></div>
      <div class="table-wrap"><table class="region-table">
        <thead><tr><th>Region</th><th>Memory</th><th>Address range</th><th class="num">Capacity</th><th class="num">Image / static</th><th class="num">Heap</th><th class="num">Fixed reserve</th><th class="num">Unallocated</th><th>Distribution</th></tr></thead>
        <tbody>{render_region_table(report)}</tbody>
      </table></div>

      {render_heap(report, root)}
      {warning_html}

      <details>
        <summary>View linker output section and load-copy details</summary>
        <div class="table-wrap"><table class="detail-table">
          <thead><tr><th>Output section</th><th>Region</th><th>Address</th><th class="num">Size</th><th>Category</th></tr></thead>
          <tbody>{render_output_details(report)}</tbody>
        </table></div>
      </details>
    </section>
    """


STYLE = r"""
:root {
  color-scheme: light;
  --ink: #102238;
  --muted: #5c6f84;
  --line: #dce4ec;
  --paper: #ffffff;
  --canvas: #eef3f8;
  --image: #2563eb;
  --heap: #10b981;
  --fixed: #f59e0b;
  --free: #dbe4ed;
}
* { box-sizing: border-box; }
html { scroll-behavior: smooth; }
body {
  margin: 0;
  color: var(--ink);
  background: var(--canvas);
  font-family: Inter, "Segoe UI", "Noto Sans TC", Arial, sans-serif;
  font-size: 14px;
  line-height: 1.55;
}
.hero {
  position: relative;
  overflow: hidden;
  padding: 54px max(28px, calc((100vw - 1280px) / 2));
  color: #fff;
  background:
    radial-gradient(circle at 86% 12%, rgba(43, 205, 190, .26), transparent 30%),
    linear-gradient(128deg, #071a2e 0%, #10395a 58%, #0f5d67 100%);
}
.hero::after {
  content: "";
  position: absolute;
  inset: 0;
  opacity: .16;
  background-image: linear-gradient(rgba(255,255,255,.16) 1px, transparent 1px), linear-gradient(90deg, rgba(255,255,255,.16) 1px, transparent 1px);
  background-size: 34px 34px;
  mask-image: linear-gradient(to left, #000, transparent 76%);
}
.hero > * { position: relative; z-index: 1; }
.hero .eyebrow { color: #69e5d5; }
.hero h1 { margin: 7px 0 12px; max-width: 780px; font-size: clamp(32px, 5vw, 52px); line-height: 1.08; letter-spacing: -.035em; }
.hero p { max-width: 830px; margin: 0; color: #cfe2ef; font-size: 16px; }
.hero-badges { display: flex; flex-wrap: wrap; gap: 9px; margin-top: 26px; }
.hero-badges span { padding: 7px 11px; border: 1px solid rgba(255,255,255,.2); border-radius: 999px; background: rgba(255,255,255,.08); backdrop-filter: blur(5px); }
.page { width: min(1280px, calc(100% - 40px)); margin: -23px auto 60px; position: relative; z-index: 2; }
.quick-guide {
  padding: 26px;
  background: var(--paper);
  border: 1px solid rgba(16,34,56,.08);
  border-radius: 18px;
  box-shadow: 0 16px 36px rgba(17, 42, 69, .08);
}
.quick-guide > h2 { margin: 4px 0 5px; font-size: 26px; }
.quick-guide > p { max-width: 950px; margin: 0; color: var(--muted); }
.executive { display: grid; grid-template-columns: repeat(3, 1fr); gap: 16px; }
.cpu-summary, .cpu-section, .methodology {
  background: var(--paper);
  border: 1px solid rgba(16,34,56,.08);
  box-shadow: 0 16px 36px rgba(17, 42, 69, .08);
}
.quick-guide .executive { margin-top: 20px; }
.quick-guide .cpu-summary { box-shadow: none; background: #f8fafc; }
.cpu-summary { border-radius: 15px; padding: 20px; border-top: 4px solid var(--accent); }
.cpu-title { display: flex; justify-content: space-between; align-items: center; margin-bottom: 16px; }
.cpu-title span { font-size: 17px; font-weight: 750; }
.cpu-title b { color: var(--accent); font-size: 12px; border: 1px solid color-mix(in srgb, var(--accent), white 65%); border-radius: 999px; padding: 4px 8px; }
.app-region-callout { margin-bottom: 13px; padding: 14px; border-radius: 10px; color: #fff; background: linear-gradient(125deg, color-mix(in srgb, var(--accent), #102238 32%), var(--accent)); }
.app-region-callout span, .app-region-callout small { display: block; color: rgba(255,255,255,.78); }
.app-region-callout span { font-size: 10px; font-weight: 800; letter-spacing: .08em; text-transform: uppercase; }
.app-region-callout strong { display: block; margin: 4px 0 1px; font-family: "Cascadia Mono", Consolas, monospace; font-size: 23px; }
.app-region-callout small { font-size: 11px; }
.summary-stat { display: flex; justify-content: space-between; gap: 16px; padding: 7px 0; color: var(--muted); border-bottom: 1px solid #edf1f5; }
.summary-stat strong { color: var(--ink); }
.heap-highlight strong { color: #07835e; }
.summary-foot { margin-top: 12px; color: var(--muted); font-size: 12px; }
.cpu-section, .methodology { margin-top: 22px; border-radius: 18px; padding: 28px; }
.cpu-section { border-top: 5px solid var(--accent); }
.section-header { display: flex; align-items: flex-end; justify-content: space-between; gap: 24px; padding-bottom: 18px; border-bottom: 1px solid var(--line); }
.eyebrow { display: block; color: var(--accent, #0f766e); font-size: 11px; font-weight: 800; letter-spacing: .14em; text-transform: uppercase; }
h2 { margin: 3px 0 0; font-size: 29px; letter-spacing: -.02em; }
h3 { margin: 30px 0 11px; font-size: 16px; letter-spacing: -.01em; }
.build-meta { max-width: 560px; text-align: right; color: var(--muted); }
.build-meta code { display: block; color: var(--ink); overflow-wrap: anywhere; }
.build-meta span { font-size: 12px; }
.app-focus { display: grid; grid-template-columns: 1fr .75fr 1.55fr; gap: 0; margin-top: 20px; overflow: hidden; border: 1px solid color-mix(in srgb, var(--accent), white 70%); border-radius: 13px; background: #f8fafc; }
.app-focus > div { padding: 18px; }
.app-focus > div + div { border-left: 1px solid #dce4ec; }
.app-focus-primary { color: #fff; background: linear-gradient(135deg, color-mix(in srgb, var(--accent), #102238 28%), var(--accent)); }
.app-focus-primary .eyebrow { color: rgba(255,255,255,.75); }
.app-focus-primary strong { display: block; margin: 5px 0 2px; font-family: "Cascadia Mono", Consolas, monospace; font-size: 27px; }
.app-focus-primary p, .app-focus-guidance p { margin: 0; }
.app-focus-primary p { color: rgba(255,255,255,.82); }
.app-focus-stat span, .app-focus-stat small, .app-focus-guidance span { display: block; color: var(--muted); }
.app-focus-stat span { font-size: 11px; font-weight: 750; text-transform: uppercase; letter-spacing: .04em; }
.app-focus-stat strong { display: block; margin: 8px 0 3px; color: var(--accent); font-size: 24px; }
.app-focus-stat small, .app-focus-guidance span { font-size: 11px; }
.app-focus-guidance b { display: block; margin-bottom: 5px; color: var(--accent); }
.app-focus-guidance p { margin-bottom: 8px; color: #40566c; }
.metrics { display: grid; grid-template-columns: repeat(5, minmax(0, 1fr)); gap: 11px; margin-top: 20px; }
.metric { min-height: 105px; padding: 15px; border-radius: 12px; background: #f6f9fc; border: 1px solid #e8eef4; }
.metric span, .metric small { display: block; color: var(--muted); }
.metric strong { display: block; margin: 8px 0 3px; color: var(--accent); font-size: 20px; line-height: 1.15; }
.metric small { font-size: 11px; }
.table-wrap { overflow-x: auto; border: 1px solid var(--line); border-radius: 11px; }
table { width: 100%; border-collapse: collapse; background: #fff; }
th { padding: 10px 12px; color: #53677b; background: #f4f7fa; font-size: 11px; text-align: left; text-transform: uppercase; letter-spacing: .055em; white-space: nowrap; }
td { padding: 11px 12px; border-top: 1px solid #e7edf3; vertical-align: middle; }
tbody tr:hover { background: #fbfdff; }
td small { display: block; margin-top: 2px; color: var(--muted); font-size: 11px; }
.num { text-align: right; white-space: nowrap; font-variant-numeric: tabular-nums; }
.address { white-space: nowrap; }
code { font-family: "Cascadia Mono", Consolas, monospace; font-size: .9em; }
.size-main, .size-exact { display: block; }
.size-exact { color: var(--muted); font-size: 10px; }
.section-dot { display: inline-block; width: 8px; height: 8px; margin-right: 8px; border-radius: 2px; background: var(--dot); }
.muted { color: var(--muted); }
.memory-pill { display: inline-block; min-width: 55px; padding: 3px 7px; color: #24435f; background: #e8f0f7; border-radius: 999px; font-size: 10px; font-weight: 750; text-align: center; }
.usage-cell { min-width: 160px; }
.usage-bar { display: flex; width: 100%; height: 11px; overflow: hidden; background: var(--free); border-radius: 999px; box-shadow: inset 0 0 0 1px rgba(16,34,56,.06); }
.bar-image { background: var(--image); }
.bar-heap { background: var(--heap); }
.bar-fixed { background: var(--fixed); }
.bar-free { background: var(--free); }
.legend { display: flex; flex-wrap: wrap; gap: 14px; margin: 0 0 9px; color: var(--muted); font-size: 11px; }
.legend span::before { content: ""; display: inline-block; width: 9px; height: 9px; margin-right: 5px; border-radius: 2px; vertical-align: -1px; background: var(--free); }
.legend .image::before { background: var(--image); }
.legend .heap::before { background: var(--heap); }
.legend .fixed::before { background: var(--fixed); }
.heap-box { display: grid; grid-template-columns: minmax(0, 1.15fr) minmax(340px, .85fr); gap: 24px; margin-top: 22px; padding: 22px; color: #eaf7f4; background: linear-gradient(130deg, #0d2d3f, #0d625f); border-radius: 13px; }
.heap-box .eyebrow { color: #74e6d2; }
.formula { display: flex; flex-wrap: wrap; align-items: baseline; gap: 12px; margin: 7px 0 5px; }
.formula code { color: #c4f1e9; font-size: 15px; }
.formula strong { font-size: 25px; }
.heap-box p { margin: 0; color: #b8d6db; }
.heap-box dl { margin: 0; display: grid; grid-template-columns: 1fr 1fr; gap: 9px; }
.heap-box dl div { padding: 10px; border: 1px solid rgba(255,255,255,.13); border-radius: 8px; background: rgba(255,255,255,.055); }
.heap-box dt { color: #9fc7cb; font-size: 10px; text-transform: uppercase; letter-spacing: .06em; }
.heap-box dd { margin: 3px 0 0; overflow-wrap: anywhere; }
.warning { margin-top: 18px; padding: 14px 17px; color: #7a4610; background: #fff8e8; border: 1px solid #f7d99b; border-radius: 10px; }
.warning ul { margin: 7px 0 0; padding-left: 20px; }
details { margin-top: 18px; border: 1px solid var(--line); border-radius: 10px; }
summary { padding: 12px 15px; cursor: pointer; color: #294a68; font-weight: 700; }
details .table-wrap { border: 0; border-top: 1px solid var(--line); border-radius: 0 0 10px 10px; }
.methodology h2 { font-size: 24px; }
.method-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 24px; }
.methodology ul { margin: 8px 0 0; padding-left: 20px; }
.source-list code { overflow-wrap: anywhere; }
.global-warning { margin: 22px 0 0; }
.report-footer { margin: 24px 0; text-align: center; color: var(--muted); font-size: 11px; }
@media (max-width: 1050px) {
  .executive { grid-template-columns: 1fr; }
  .metrics { grid-template-columns: repeat(2, 1fr); }
  .app-focus, .heap-box, .method-grid { grid-template-columns: 1fr; }
  .app-focus > div + div { border-left: 0; border-top: 1px solid #dce4ec; }
}
@media (max-width: 650px) {
  .page { width: min(100% - 20px, 1280px); }
  .hero { padding: 38px 20px 54px; }
  .quick-guide { padding: 19px; }
  .cpu-section, .methodology { padding: 19px; }
  .section-header { align-items: flex-start; flex-direction: column; }
  .build-meta { text-align: left; }
  .metrics { grid-template-columns: 1fr; }
  .heap-box dl { grid-template-columns: 1fr; }
}
@media print {
  body { background: #fff; font-size: 10px; }
  .hero { padding: 24px; print-color-adjust: exact; -webkit-print-color-adjust: exact; }
  .page { width: 100%; margin: 12px 0; }
  .cpu-summary, .cpu-section, .methodology { box-shadow: none; break-inside: avoid; }
  .cpu-section { break-before: page; }
  details { display: none; }
}
"""


def render_report(
    reports: list[CpuReport],
    root: Path,
    global_warnings: list[str],
    source_paths: list[Path],
) -> str:
    generated = datetime.now().astimezone()
    cpu_label = " / ".join(report.cpu for report in reports)

    global_warning_html = ""
    if global_warnings:
        global_warning_html = "<div class=\"warning global-warning\"><b>Cross-CPU configuration warnings</b><ul>" + "".join(
            f"<li>{html.escape(item)}</li>" for item in dict.fromkeys(global_warnings)
        ) + "</ul></div>"

    sources_html = "".join(
        f"<li><code>{html.escape(relpath(path, root))}</code></li>" for path in source_paths
    )
    summaries = "".join(render_cpu_summary(report) for report in reports)
    sections = "".join(render_cpu_section(report, root) for report in reports)

    return f"""<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>AmebaPro3 {cpu_label} Memory Report</title>
  <style>{STYLE}</style>
</head>
<body>
  <header class="hero">
    <span class="eyebrow">AmebaPro3 | RTL8735C</span>
    <h1>{cpu_label} Code Size &amp; FreeRTOS Heap Report</h1>
    <p>Map-derived physical memory analysis of code, data, reserved space, FreeRTOS heap, and customer headroom across SRAM, PSRAM, and DDR.</p>
    <div class="hero-badges"><span>PSRAM enabled</span><span>External DDR enabled</span><span>FP excluded</span><span>Map-derived</span><span>{generated.strftime('%Y-%m-%d %H:%M:%S UTC%z')}</span></div>
  </header>

  <main class="page">
    <section class="quick-guide">
      <span class="eyebrow">Customer application placement | quick answer</span>
      <h2>Where Customer Application Code Goes</h2>
      <p>If you inspect only one memory row per CPU, use the highlighted primary region below. The detailed CPU sections explain where writable data goes and whether application growth consumes unallocated space or FreeRTOS heap.</p>
      <div class="executive">{summaries}</div>
    </section>
    {global_warning_html}
    {sections}

    <section class="methodology">
      <span class="eyebrow">Method &amp; scope</span>
      <h2>Calculation Method and Sources</h2>
      <div class="method-grid">
        <div>
          <h3>Calculation rules</h3>
          <ul>
            <li>Memory region origin and length come from each CPU map's <code>Memory Configuration</code>.</li>
            <li>Runtime sections are classified by their linked runtime physical address. A distinct load address is counted as a separate physical load-image copy. AP <code>.data</code> therefore occupies AP_DDR at runtime and AP_DDR_RO in the load image.</li>
            <li>FreeRTOS heap is calculated exactly as <code>__heap_end__ - __heap_start__</code>, matching the raw range passed to <code>vPortDefineHeapRegions()</code> by <code>init_01_heap.c</code>.</li>
            <li>Unallocated bytes are not covered by the image, heap, stack, or known platform reservations. The report also shows the largest contiguous block and never combines different memory types into one interchangeable free-space number.</li>
          </ul>
        </div>
        <div>
          <h3>Scope limits</h3>
          <ul>
            <li>FP is excluded from the customer-development report.</li>
            <li>Dynamic media and NN pools such as <code>EN_DDR</code>, <code>TG_DDR</code>, <code>NN_DDR</code>, <code>VP_PSRAM_NN</code>, and VSRAM are outside these image2 linker and heap allocations, so they are excluded from code and heap headroom.</li>
            <li>Heap values are linker-provided raw bytes. FreeRTOS alignment and allocator metadata make runtime allocatable space slightly smaller.</li>
            <li>The report analyzes existing build artifacts only. It does not build, configure, clean, or flash the SDK.</li>
          </ul>
        </div>
      </div>
      <h3>Authoritative files</h3>
      <ul class="source-list">{sources_html}</ul>
    </section>
    <footer class="report-footer">Generated by <code>generate_memory_report.py</code> | standalone HTML | no external assets</footer>
  </main>
</body>
</html>
"""


def find_sdk_root(script: Path) -> Path:
    for candidate in (Path.cwd(), *script.parents):
        if (candidate / "component/soc/amebapro3").is_dir():
            return candidate.resolve()
    raise ReportError(
        "AmebaPro3 SDK root not found; run from the source root or specify --sdk-root"
    )


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Generate an AmebaPro3 AP/VP/NP code-size and FreeRTOS heap HTML report."
    )
    parser.add_argument(
        "--sdk-root",
        type=Path,
        help="AmebaPro3 SDK source root (defaults to auto-detection).",
    )
    parser.add_argument(
        "--output",
        type=Path,
        help="Output HTML path (default: build_RTL8735C/amebapro3_memory_report.html).",
    )
    parser.add_argument(
        "--np-map",
        type=Path,
        help=f"NP linker map path (default: {CPU_CONFIG['NP']['map']}).",
    )
    parser.add_argument(
        "--np-linker",
        type=Path,
        help=f"NP linker script path (default: {CPU_CONFIG['NP']['linker']}).",
    )
    for cpu in ("AP", "VP"):
        parser.add_argument(
            f"--{cpu.lower()}-map",
            type=Path,
            help=(
                f"{cpu} linker map path. {cpu} is included in the report only "
                f"if --{cpu.lower()}-map or --{cpu.lower()}-linker is given."
            ),
        )
        parser.add_argument(
            f"--{cpu.lower()}-linker",
            type=Path,
            help=(
                f"{cpu} linker script path. {cpu} is included in the report only "
                f"if --{cpu.lower()}-map or --{cpu.lower()}-linker is given."
            ),
        )
    parser.add_argument(
        "--layout",
        type=Path,
        help=(
            "Preprocessed memory layout linker script, combining "
            f"ameba_layout.ld and ameba_layout.h (default: {LAYOUT_PATH})."
        ),
    )
    return parser.parse_args()


def main() -> int:
    arguments = parse_arguments()
    script = Path(__file__).resolve()
    root = (
        arguments.sdk_root.resolve()
        if arguments.sdk_root
        else find_sdk_root(script)
    )
    output = (
        arguments.output.resolve()
        if arguments.output
        else root / "build_RTL8735C/amebapro3_memory_report.html"
    )

    layout_path = (
        arguments.layout.resolve() if arguments.layout else root / LAYOUT_PATH
    )
    cpu_enabled = {
        "NP": True,
        "AP": arguments.ap_map is not None or arguments.ap_linker is not None,
        "VP": arguments.vp_map is not None or arguments.vp_linker is not None,
    }
    active_cpus = [cpu for cpu in CPU_ORDER if cpu_enabled[cpu]]

    map_paths = {}
    linker_paths = {}
    for cpu in active_cpus:
        cpu_map = getattr(arguments, f"{cpu.lower()}_map")
        cpu_linker = getattr(arguments, f"{cpu.lower()}_linker")
        map_paths[cpu] = cpu_map.resolve() if cpu_map else root / CPU_CONFIG[cpu]["map"]
        linker_paths[cpu] = (
            cpu_linker.resolve() if cpu_linker else root / CPU_CONFIG[cpu]["linker"]
        )

    required = [layout_path]
    for cpu in active_cpus:
        required.extend([map_paths[cpu], linker_paths[cpu]])
    missing = [path for path in required if not path.is_file()]
    if missing:
        formatted = "\n".join(f"  - {path}" for path in missing)
        raise ReportError(f"Required input files are missing:\n{formatted}")

    reports = []
    for cpu in active_cpus:
        parsed = parse_map(map_paths[cpu])
        reports.append(analyze_cpu(cpu, parsed))
    validate_memory_mode(reports)
    global_warnings = consistency_warnings(reports)

    source_paths = [layout_path] + [linker_paths[cpu] for cpu in active_cpus]
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(
        render_report(reports, root, global_warnings, source_paths), encoding="utf-8"
    )

    print(f"AmebaPro3 memory report: {output}")
    for report in reports:
        print(
            f"{report.cpu}: heap {format_bytes(report.heap_size)} "
            f"({format_address(report.heap_start)} - {format_address(report.heap_end)}), "
            f"text {format_bytes(report.section_totals.get('text', 0))}, "
            f"rodata {format_bytes(report.section_totals.get('rodata', 0))}, "
            f"data {format_bytes(report.section_totals.get('data', 0))}, "
            f"bss {format_bytes(report.section_totals.get('bss', 0))}"
        )
        for warning in dict.fromkeys(report.warnings):
            print(f"  warning: {warning}")
    for warning in global_warnings:
        print(f"warning: {warning}")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except ReportError as error:
        print(f"error: {error}", file=sys.stderr)
        raise SystemExit(2)
