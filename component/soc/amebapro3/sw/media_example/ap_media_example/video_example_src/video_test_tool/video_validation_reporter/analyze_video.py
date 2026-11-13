#!/usr/bin/env python3
"""Inspect a recorded video and compare observable properties with VID settings.

Requires Python 3.9+ and ffprobe/ffmpeg on PATH. Exact QP analysis also uses PyAV;
HEVC needs the libde265_qp helper (see hevc_qp_backend/README.md).
Exit codes: 0=no failed checks (inspect SKIP/PARTIAL), 1=failed checks, 2=tool/input error.
"""

import argparse
import html
import hashlib
import json
import math
import re
import statistics
import subprocess
import sys
import tempfile
from collections import Counter
from pathlib import Path
from video_qp import SliceTrace, default_hevc_library


HEADER_FIELDS = {
    "general_profile_idc", "general_level_idc", "general_tier_flag",
    "profile_idc", "level_idc", "constraint_set3_flag",
    "video_signal_type_present_flag", "video_full_range_flag",
    "aspect_ratio_info_present_flag", "aspect_ratio_idc", "sar_width", "sar_height",
    "colour_primaries", "transfer_characteristics", "matrix_coefficients",
    "vui_timing_info_present_flag", "timing_info_present_flag",
    "vui_num_units_in_tick", "vui_time_scale", "num_units_in_tick", "time_scale",
    "fixed_frame_rate_flag", "vui_hrd_parameters_present_flag",
    "nal_hrd_parameters_present_flag", "vcl_hrd_parameters_present_flag",
    "sample_adaptive_offset_enabled_flag", "slice_sao_luma_flag", "slice_sao_chroma_flag",
    "entropy_coding_mode_flag", "cabac_init_present_flag", "cabac_init_flag",
    "deblocking_filter_control_present_flag", "deblocking_filter_override_enabled_flag",
    "pps_deblocking_filter_disabled_flag", "slice_deblocking_filter_disabled_flag",
    "disable_deblocking_filter_idc", "pps_beta_offset_div2", "pps_tc_offset_div2",
    "slice_beta_offset_div2", "slice_tc_offset_div2", "slice_alpha_c0_offset_div2",
    "slice_beta_offset_div2", "init_qp_minus26", "pic_init_qp_minus26", "slice_qp_delta",
    "slice_type", "nal_unit_type", "payload_type", "last_payload_type_byte",
}
TRACE_PATTERN = re.compile(r"\[trace_headers[^]]*\]\s+\d+\s+(\w+)\s+[01]+\s+=\s+(-?\d+)\s*$")


def number(value):
    try:
        result = float(value)
        return result if math.isfinite(result) else None
    except (TypeError, ValueError):
        return None


def run(command, timeout):
    return subprocess.run(command, stdin=subprocess.DEVNULL, capture_output=True, text=True, encoding="utf-8",
                          errors="replace", timeout=timeout, check=False)


def probe(ffprobe, video, options, timeout):
    result = run([ffprobe, "-v", "error", *options, "-of", "json", str(video)], timeout)
    if result.returncode:
        raise ValueError("ffprobe failed: " + result.stderr[-4000:])
    return json.loads(result.stdout), result.stderr.strip()


def trace_headers(ffmpeg, video, timeout):
    fields = {}
    slices = SliceTrace()
    # Keep verbose trace output on disk rather than holding an entire clip's trace in RAM.
    with tempfile.TemporaryFile(mode="w+", encoding="utf-8", errors="replace") as log:
        result = subprocess.run(
            [ffmpeg, "-nostdin", "-nostats", "-hide_banner", "-loglevel", "info", "-copyts", "-i", str(video),
             "-map", "0:v:0", "-c:v", "copy", "-bsf:v", "trace_headers", "-f", "null", "-"],
            stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=log, timeout=timeout, check=False)
        log.seek(0)
        tail = []
        for line in log:
            slices.feed(line)
            tail.append(line.rstrip())
            tail = tail[-20:]
            match = TRACE_PATTERN.search(line)
            if match and match[1] in HEADER_FIELDS:
                fields.setdefault(match[1], Counter())[int(match[2])] += 1
    slices.finish()
    return {key: dict(sorted(values.items())) for key, values in sorted(fields.items())}, result.returncode, tail, slices


def summarize(stream, frames):
    timestamps = [number(f.get("best_effort_timestamp_time")) for f in frames]
    valid = [t for t in timestamps if t is not None]
    deltas = [b - a for a, b in zip(valid, valid[1:])]
    keys = [i for i, f in enumerate(frames) if f.get("key_frame") == 1]
    intra = [i for i, f in enumerate(frames) if f.get("pict_type") == "I"]
    span = valid[-1] - valid[0] if len(valid) > 1 else 0
    last_duration = number(frames[-1].get("duration_time", frames[-1].get("pkt_duration_time"))) if frames else None
    duration = span + (last_duration if last_duration and last_duration > 0 else statistics.median(deltas) if deltas else 0)
    sizes = [number(f.get("pkt_size")) for f in frames]
    total = sum(sizes) if sizes and all(s is not None for s in sizes) else None
    return {
        "decoded_frames": len(frames), "frame_types": dict(Counter(f.get("pict_type", "unknown") for f in frames)),
        "codec": stream.get("codec_name"), "profile": stream.get("profile"), "level": stream.get("level"),
        "width": stream.get("width"), "height": stream.get("height"), "pixel_format": stream.get("pix_fmt"),
        "color_range": stream.get("color_range"), "sample_aspect_ratio": stream.get("sample_aspect_ratio"),
        "metadata_avg_frame_rate": stream.get("avg_frame_rate"), "metadata_r_frame_rate": stream.get("r_frame_rate"),
        "timestamp_fps": (len(valid) - 1) / span if span > 0 and len(valid) == len(frames) else None,
        "timeline_duration_seconds": duration, "stream_duration_seconds": number(stream.get("duration")),
        "video_bitrate_bps": total * 8 / duration if duration > 0 and total is not None else None,
        "metadata_video_bitrate_bps": number(stream.get("bit_rate")),
        "missing_timestamps": len(frames) - len(valid),
        "non_increasing_timestamps": sum(d <= 0 for d in deltas),
        "frame_interval_ms": {"min": min(deltas) * 1000, "median": statistics.median(deltas) * 1000,
                              "max": max(deltas) * 1000} if deltas else {},
        "keyframe_indices": keys, "keyframe_gaps": [b - a for a, b in zip(keys, keys[1:])],
        "i_frame_indices": intra, "i_frame_gaps": [b - a for a, b in zip(intra, intra[1:])],
    }


def evaluate(summary, headers, expected, decode_ok, probe_clean, trace_ok, fps_tolerance, bitrate_tolerance):
    checks = []

    def add(name, actual, wanted, passed, detail=""):
        checks.append({"check": name, "status": "SKIP" if passed is None else "PASS" if passed else "FAIL",
                       "expected": wanted, "actual": actual, "detail": detail})

    def equal(name, actual, wanted, detail=""):
        add(name, actual, wanted, None if actual is None else actual == wanted, detail)

    def header(name):
        values = headers.get(name, {}) if trace_ok else {}
        return sorted(values) if values else None

    add("decode", summary["decoded_frames"], "all frames decode without error",
        decode_ok and summary["decoded_frames"] > 0)
    add("probe", probe_clean, True, probe_clean)
    add("header_scan", len(headers), "parsed header fields", True if trace_ok and headers else None,
        "Unavailable/empty header trace prevents header verification.")
    add("timestamps", [summary["missing_timestamps"], summary["non_increasing_timestamps"]], [0, 0],
        summary["decoded_frames"] > 1 and summary["missing_timestamps"] == summary["non_increasing_timestamps"] == 0)
    if not expected:
        add("parameter_comparison", None, None, None, "No expected settings supplied; observations only.")
    codec = summary["codec"]
    for key, wanted in expected.items():
        if key.startswith("_"):
            continue
        if key == "format":
            equal(key, codec, {"HEVC": "hevc", "H265": "hevc", "H264": "h264", "JPEG": "mjpeg"}.get(str(wanted).upper(), str(wanted).lower()))
        elif key in ("width", "height", "level"):
            if key == "level" and wanted in (65535, 99):
                add(key, summary[key], wanted, None, "Auto or SDK level-1b sentinel requires codec-specific interpretation.")
            else:
                equal(key, summary[key], wanted, "Level is the raw codec level_idc, not a decimal level label." if key == "level" else "")
        elif key == "profile":
            # SDK enum values differ from bitstream profile_idc; map only unambiguous cases.
            mapping = {0: 1, 1: 3, 2: 2} if codec == "hevc" else {9: 66, 10: 77, 11: 100, 12: 110} if codec == "h264" else {}
            if isinstance(wanted, int):
                equal(key, header("general_profile_idc" if codec == "hevc" else "profile_idc") if wanted in mapping else None,
                      [mapping[wanted]] if wanted in mapping else wanted, "Mapped SDK enum to profile_idc; auto/extended profiles are not inferred.")
            else:
                equal(key, summary["profile"], wanted, "String comparison uses ffprobe's exact profile name.")
        elif key == "fps":
            actual = summary["timestamp_fps"]
            add(key, actual, wanted, None if actual is None else abs(actual - wanted) <= wanted * fps_tolerance,
                f"Whole-clip timestamp rate, relative tolerance {fps_tolerance:.1%}; not a per-window FPS guarantee.")
        elif key == "gop":
            gaps = summary["i_frame_gaps"]
            add(key, gaps, wanted, all(g == wanted for g in gaps) if gaps else None,
                "Complete I-to-I intervals only; forced I-frames can intentionally shorten a GOP. I-frame is not always IDR.")
        elif key == "record_seconds":
            fps = expected.get("fps")
            add("record_sample_count", summary["decoded_frames"], fps * wanted if fps else None,
                summary["decoded_frames"] == fps * wanted if fps else None,
                "Checks the muxer's fps*seconds sample target, not a wall-clock timeout. Early stop will differ.")
        elif key == "bps":
            actual = summary["video_bitrate_bps"]
            mode = expected.get("rc_mode")
            cbr = str(mode).upper() in ("CBR", "1")
            add(key, actual, wanted, abs(actual - wanted) <= wanted * bitrate_tolerance if cbr and actual is not None else None,
                f"CBR whole-clip tolerance {bitrate_tolerance:.1%}; VBR/other modes have no fixed target-equality assertion."
                " Measured bitrate cannot identify the RC algorithm.")
        elif key == "full_range":
            equal(key, {"tv": 0, "pc": 1}.get(summary["color_range"]), int(wanted),
                  "Decoder-reported effective range; may be inferred when VUI signaling is absent.")
        elif key == "sar":
            actual = summary["sample_aspect_ratio"]
            try:
                a, b = map(int, actual.split(":"))
                c, d = map(int, wanted.split(":"))
                passed = a * d == b * c if b and d else None
            except (AttributeError, ValueError):
                passed = None
            add(key, actual, wanted, passed, "Missing/unspecified SAR is not asserted to be 1:1.")
        elif key in ("tier", "sao", "cabac"):
            field, supported = {"tier": ("general_tier_flag", codec == "hevc"),
                                "sao": ("sample_adaptive_offset_enabled_flag", codec == "hevc"),
                                "cabac": ("entropy_coding_mode_flag", codec == "h264")}[key]
            equal(key, header(field) if supported else None, [int(wanted)],
                  "Checks signaled header capability; does not prove every block uses the feature.")
        elif key == "headers":
            for field, value in wanted.items():
                equal("header." + field, header(field), [value],
                      "All observed occurrences must match. Missing fields are SKIP, not zero. Counts include repeated parameter sets.")
        else:
            add(key, None, wanted, None,
                "Not directly verifiable from this MP4 by this script. Requires encoder telemetry or controlled image/bitstream analysis.")
    return checks


def validate_expected(expected):
    if not isinstance(expected, dict):
        raise ValueError("Expected settings must be a JSON object")
    for key in ("fps", "bps", "width", "height", "gop", "record_seconds"):
        if key in expected and (type(expected[key]) not in (int, float) or not math.isfinite(expected[key]) or expected[key] <= 0):
            raise ValueError(f"{key} must be a positive finite number")
    for key in ("min_qp", "max_qp"):
        if key in expected and (type(expected[key]) is not int or not 0 <= expected[key] <= 51):
            raise ValueError(f"{key} must be an integer from 0 to 51 (8-bit luma QP)")
    if expected.get("min_qp", 0) > expected.get("max_qp", 51):
        raise ValueError("min_qp must not exceed max_qp")
    if "headers" in expected and (not isinstance(expected["headers"], dict) or
                                  any(type(v) is not int for v in expected["headers"].values())):
        raise ValueError("headers must map trace field names to integer values")
    for key in ("tier", "sao", "cabac", "full_range"):
        if key in expected and expected[key] not in (0, 1):
            raise ValueError(f"{key} must be 0 or 1")


def load_expected(video, explicit=None):
    path = explicit or video.with_name(video.stem + "_params.expected.json")
    expected = json.loads(path.read_text(encoding="utf-8-sig")) if explicit or path.exists() else {}
    validate_expected(expected)
    if "_video_file" in expected and expected["_video_file"] != video.name:
        raise ValueError("Parameter snapshot belongs to a different MP4: " + str(expected["_video_file"]))
    if "_record_id" in expected and video.stem != "video_test_" + str(expected["_record_id"]):
        raise ValueError("Recording ID does not match the MP4 filename")
    return expected, str(path.resolve()) if explicit or path.exists() else None


def qp_analysis(video, frames, codec, expected, slices, trace_ok, decode_ok, timeout, library):
    if library is None:
        library = default_hevc_library()
    with tempfile.TemporaryDirectory(prefix='video-qp-') as directory:
        request, output = Path(directory)/'request.json', Path(directory)/'result.json'
        request.write_text(json.dumps({'video': str(video), 'min_qp': expected.get('min_qp'),
                                      'max_qp': expected.get('max_qp'), 'library': str(library) if library else None}), encoding='utf-8')
        try:
            proc = run([sys.executable, '-B', str(Path(__file__).with_name('video_qp.py')), str(request), str(output)], timeout)
            raw = json.loads(output.read_text(encoding='utf-8')) if proc.returncode == 0 and output.exists() else {
                'frames': [], 'backend': '', 'error': f'QP worker failed ({proc.returncode}): {proc.stderr[-2000:]}'}
        except subprocess.TimeoutExpired:
            raw = {'frames': [], 'backend': '', 'error': 'QP decoder timed out; no complete coverage'}
    return summarize_qp(raw, frames, codec, expected, slices, trace_ok, decode_ok)


def summarize_qp(raw, frames, codec, expected, slices, trace_ok, decode_ok):
    low, high = expected.get('min_qp'), expected.get('max_qp')
    by_pts = {}
    aligned = len(raw['frames']) == len(frames) and bool(frames)
    for row in raw['frames']:
        if row.get('pts') is None or row['pts'] in by_pts:
            aligned = False
        by_pts[row.get('pts')] = row
    if len({f.get('pts') for f in frames}) != len(frames) or any(f.get('pts') is None for f in frames):
        aligned = False
    rows = []
    for i, frame in enumerate(frames):
        row = dict(by_pts.get(frame.get('pts'), {}))
        geometry_ok = (row.get('width') == frame.get('width') and row.get('height') == frame.get('height') and
                       not frame.get('interlaced_frame', 0))
        if not geometry_ok: aligned = False
        qps = row.get('picture_qps', [])
        picture_complete = bool(qps) and row.get('complete', False)
        if codec == 'h264' and 'picture_complete' in row:
            picture_complete = row['picture_complete'] and geometry_ok
        elif codec == 'h264':
            qps = slices.values.get(frame.get('pts'), []) if trace_ok else []
            picture_complete = bool(qps) and frame.get('pts') not in slices.invalid and geometry_ok
        bad_picture = [q for q in qps if (low is not None and q < low) or (high is not None and q > high)]
        row.update(frame=i, timestamp=frame.get('best_effort_timestamp_time'), type=frame.get('pict_type'),
                   picture_qps=sorted(set(qps)), picture_complete=picture_complete, bad_picture_qps=bad_picture)
        row['block_complete'] = bool(row.get('complete')) and geometry_ok
        rows.append(row)
    picture_values = [q for r in rows for q in r['picture_qps']]
    block_values = [v for r in rows for v in (r.get('min'),r.get('max')) if v is not None]
    picture_bad = sum(bool(r['bad_picture_qps']) for r in rows)
    block_bad = sum(r.get('out_of_range_blocks',0) for r in rows)
    complete_picture = aligned and decode_ok and not raw['error'] and all(r['picture_complete'] for r in rows)
    complete_block = aligned and decode_ok and not raw['error'] and all(r['block_complete'] for r in rows)
    limits_present = low is not None and high is not None
    def check(name, values, bad, complete):
        status = 'FAIL' if bad else 'PASS' if complete and limits_present else 'SKIP'
        return {'check': name, 'status': status, 'expected': {'min_qp': low, 'max_qp': high},
                'actual': {'min': min(values) if values else None, 'max': max(values) if values else None,
                           'violations': bad, 'complete': complete},
                'detail': 'Inclusive 8-bit luma QP limits. Any observed violation fails; PASS requires complete frame/region coverage and both bounds.'}
    checks = [check('picture_qp_range', picture_values, picture_bad, complete_picture),
              check('block_qp_range', block_values, block_bad, complete_block)]
    histogram = Counter()
    for row in rows:
        histogram.update({int(q):n for q,n in row.get('histogram',{}).items()})
    return {'backend': raw['backend'], 'error': raw['error'], 'bounds': [low,high],
            'aligned': aligned, 'picture_complete': complete_picture, 'block_complete': complete_block,
            'frames': rows, 'histogram': dict(sorted(histogram.items())), 'checks': checks,
            'note': 'Picture QP means slice initial QPY (all slices are checked), not a frame average or PPS base. '
                    'Block QP is final luma QPY including local deltas. Chroma mapped QP is outside these luma limits. '
                    'PCM/transquant-bypass blocks are reported as incomplete. Up to 64 violating locations per frame are shown; counts include all violations.'}


def qp_chart(qp):
    """One vertical line per frame spans its observed block QP range."""
    rows = [r for r in qp['frames'] if r.get('min') is not None]
    if not rows:
        return ''
    def y(q): return 205 - q * 180 / 51
    parts = ['<h3>QP range by frame</h3><p>Blue: block minimum to maximum. Orange: picture/slice QP. '
             'Green band: expected inclusive range. X axis: frame index; Y axis: luma QP.</p>',
             '<svg viewBox="0 0 1000 240" style="height:260px" role="img" aria-label="Per-frame block and picture QP ranges">']
    low, high = qp['bounds']
    if low is not None and high is not None:
        parts.append(f'<rect x="50" y="{y(high):.2f}" width="920" height="{(high-low)*180/51:.2f}" fill="#d9f2e4"/>')
    for q in [0,10,20,30,40,51]:
        parts.append(f'<line x1="50" y1="{y(q):.2f}" x2="970" y2="{y(q):.2f}" stroke="#c9d5e2"/>')
        parts.append(f'<text x="15" y="{y(q)+4:.2f}" font-size="12">{q}</text>')
    last = max(1,len(qp['frames'])-1)
    for r in rows:
        x = 50 + r['frame'] * 920 / last
        color = '#b32424' if r.get('out_of_range_blocks') else '#2876bd'
        parts.append(f'<line x1="{x:.2f}" x2="{x:.2f}" y1="{y(r["min"]):.2f}" y2="{y(r["max"]):.2f}" stroke="{color}" stroke-width="2"/>')
        for q in r['picture_qps']:
            parts.append(f'<circle cx="{x:.2f}" cy="{y(q):.2f}" r="2" fill="#b46512"/>')
    for i in sorted({0,last//2,last}):
        parts.append(f'<text x="{50+i*920/last:.2f}" y="228" text-anchor="middle" font-size="12">{i}</text>')
    parts.append('</svg>')
    return ''.join(parts)


def html_report(report, frames):
    def esc(value):
        return html.escape(str(value), quote=True)

    def value(item):
        return esc(json.dumps(item, ensure_ascii=False, allow_nan=False))

    def table(columns, rows):
        return '<div class="scroll"><table><thead><tr>' + ''.join('<th>' + esc(c) + '</th>' for c in columns) + '</tr></thead><tbody>' + ''.join(
            '<tr>' + ''.join('<td>' + value(c) + '</td>' for c in row) + '</tr>' for row in rows) + '</tbody></table></div>'

    counts = Counter(c['status'] for c in report['checks'])
    record_id = report['expected'].get('_record_id', 'Legacy recording / no captured ID')
    parts = ['<!doctype html><html lang="en"><meta charset="utf-8">',
             '<meta name="viewport" content="width=device-width, initial-scale=1">',
             '<title>Video analysis - ' + esc(record_id) + '</title>',
             """<style>
:root{font-family:Segoe UI,Arial,sans-serif;color:#18304a;background:#eef3f8}body{max-width:1280px;margin:32px auto;padding:0 24px}
h1{font-size:32px;margin:8px 0}h2{font-size:21px;margin-top:30px}header,section{background:white;padding:24px;border-radius:12px;margin:18px 0;box-shadow:0 3px 14px #18304a0a}
.label{font-size:12px;letter-spacing:2px;color:#52718e}.badge{display:inline-block;padding:9px 18px;border-radius:6px;font-weight:700;background:#e7edf5}.PASS{background:#dcf5e7;color:#17623b}.FAIL{background:#ffe0e0;color:#a71f1f}.PARTIAL{background:#fff0ca;color:#875900}
.cards{display:flex;gap:18px;flex-wrap:wrap;margin-top:20px}.card{padding:14px 20px;background:#f3f6fa;border-radius:8px}.card b{display:block;font-size:24px}table{border-collapse:collapse;width:100%;font-size:13px}th,td{text-align:left;border-bottom:1px solid #e0e7ef;padding:10px;vertical-align:top;overflow-wrap:anywhere}th{background:#eaf0f7;position:sticky;top:0}.scroll{overflow:auto;max-height:650px}pre{white-space:pre-wrap;overflow-wrap:anywhere;font-size:12px}summary{cursor:pointer;font-weight:600}p{line-height:1.6;overflow-wrap:anywhere}svg{width:100%;height:160px;background:#f5f8fc} @media print{body{margin:0;background:white}.scroll{max-height:none;overflow:visible}section{box-shadow:none}th{position:static}}
</style><body>""",
             '<header><div class="label">AMEBAPRO3 / VIDEO REGRESSION</div><h1>Recording analysis</h1>',
             '<p>Scenario: <b>' + esc(record_id) + '</b><br>MP4: ' + esc(Path(report['input']).name) +
             '<br>Parameters: ' + esc(report.get('expected_path') or 'Not supplied') + '</p>',
             '<span class="badge ' + report['result'] + '">' + report['result'] + '</span>',
             '<p>PASS applies only to the listed checks. SKIP means unverified; PARTIAL means at least one check remains unverified.</p>',
             '<div class="cards">' + ''.join('<div class="card"><b>' + str(counts[k]) + '</b>' + k + '</div>' for k in ('PASS', 'FAIL', 'SKIP')) + '</div></header>',
             '<section><h2>Measurement summary</h2>' + table(['Property', 'Measured value'], report['summary'].items()) + '</section>',
             '<section><h2>Parameter verification</h2>' + table(['Check', 'Result', 'Expected', 'Actual', 'Interpretation'],
                 ([c[k] for k in ('check', 'status', 'expected', 'actual', 'detail')] for c in report['checks'])) + '</section>']
    sizes = [number(f.get('pkt_size')) or 0 for f in frames]
    if sizes:
        peak = max(sizes) or 1
        points = ' '.join(f'{i * 1000 / max(1, len(sizes)-1):.2f},{150-size*140/peak:.2f}' for i, size in enumerate(sizes))
        parts += ['<section><h2>Encoded frame size</h2><p>Frame index from left to right; peak ' + str(int(peak)) + ' bytes. Spikes alone do not indicate an error.</p><svg viewBox="0 0 1000 160" role="img" aria-label="Encoded frame sizes"><polyline fill="none" stroke="#2676ba" stroke-width="1.5" points="' + points + '"/></svg></section>']
    qp = report.get('qp')
    if qp:
        parts += ['<section><h2>Picture and block QP verification</h2><p>' + esc(qp['note']) + '</p>',
                  '<p>Backend: ' + esc(qp['backend'] or 'Unavailable') + '<br>' + esc(qp['error']) + '</p>',
                  table(['Check', 'Status', 'Expected range', 'Observed range / coverage'],
                        ([c['check'],c['status'],c['expected'],c['actual']] for c in qp['checks'])),
                  '<p>Fully covered frames: ' + str(sum(r['block_complete'] for r in qp['frames'])) + '/' + str(len(qp['frames'])) +
                  '; coding blocks checked: ' + str(sum(r.get('blocks',0)-r.get('excluded_blocks',0) for r in qp['frames'])) + '</p>' + qp_chart(qp),
                  '<h3>Per-frame QP</h3>' + table(
                      ['Frame', 'Time (s)', 'Type', 'Picture / slice QPs', 'Block min', 'Block max',
                       'Area-weighted mean', 'Blocks', 'Out-of-range blocks', 'Full block coverage'],
                      ([r['frame'],r['timestamp'],r['type'],r['picture_qps'],r.get('min'),r.get('max'),
                        round(r['mean'],3) if r.get('mean') is not None else None,r.get('blocks',0),
                        r.get('out_of_range_blocks',0),r['block_complete']] for r in qp['frames'])),
                  '<details><summary>QP distribution (coding block counts)</summary>' +
                  table(['Luma QP', 'Blocks'], qp['histogram'].items()) + '</details>',
                  '<details><summary>Out-of-range block locations (up to 64 per frame)</summary>' +
                  table(['Frame', 'Time (s)', 'Type', 'X', 'Y', 'Width', 'Height', 'QP'],
                        ([r['frame'],r['timestamp'],r['type'],b['x'],b['y'],b['w'],b['h'],b['qp']]
                         for r in qp['frames'] for b in r.get('violations',[]))) + '</details></section>']
    parts += ['<section><h2>Captured settings</h2><p>' + esc(report['expected'].get('_note', 'Device state cannot be recovered from MP4 alone.')) + '</p><pre>' + value_pre(report['expected']) + '</pre></section>',
              '<section><h2>Bitstream headers</h2><p>Values map to occurrence counts, including repeated parameter sets. Missing fields are unknown.</p>' + table(['Field', 'Value: count'], report['headers'].items()) + '</section>',
              '<section><h2>Interpretation limits</h2><p>RC algorithm, ROI effectiveness and AINR require encoder telemetry or controlled image analysis. QP range checks use the dedicated decoder analysis above; header-only QP observations do not establish block coverage. SEI payloads do not uniquely identify driver/user switches. Header signaling does not prove image quality or per-block use. FPS comparisons use timestamps; nominal container FPS may differ. Successful decoding is not complete encoder certification.</p></section>',
              '<section><details><summary>Per-frame data (' + str(len(frames)) + ' frames)</summary>' + table(
                  ['Frame', 'Keyframe', 'Type', 'Timestamp (s)', 'Duration (s)', 'Legacy duration (s)', 'Packet bytes'],
                  ([i] + [f.get(k) for k in ('key_frame', 'pict_type', 'best_effort_timestamp_time', 'duration_time', 'pkt_duration_time', 'pkt_size')] for i, f in enumerate(frames))) + '</details></section>',
              '<section><details><summary>Complete results, source metadata and diagnostics</summary><pre>' + value_pre({**report, 'frames': frames}) + '</pre></details></section>',
              '<p>Input SHA-256: ' + esc(report['sha256']) + '</p></body></html>']
    return ''.join(parts)


def value_pre(data):
    return html.escape(json.dumps(data, indent=2, ensure_ascii=False, allow_nan=False))


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("video", type=Path)
    parser.add_argument("--expect", type=Path, help="Expected settings JSON (default: matching <video stem>_params.expected.json if present)")
    parser.add_argument("--output", type=Path, help="Output basename (default: <video stem>.analysis)")
    parser.add_argument("--hevc-qp-library", type=Path, help="Exact-QP libde265 helper DLL/shared library")
    parser.add_argument("--ffprobe", default="ffprobe")
    parser.add_argument("--ffmpeg", default="ffmpeg")
    parser.add_argument("--timeout", type=int, default=300, help="Timeout in seconds for each external tool invocation")
    parser.add_argument("--fps-tolerance", type=float, default=0.05)
    parser.add_argument("--bitrate-tolerance", type=float, default=0.20)
    parser.add_argument("--require-all", action="store_true", help="Return exit code 1 for PARTIAL as well as FAIL")
    args = parser.parse_args(argv)
    try:
        if args.timeout <= 0 or not all(math.isfinite(v) and 0 <= v <= 1 for v in (args.fps_tolerance, args.bitrate_tolerance)):
            raise ValueError("Timeout must be positive; relative tolerances must be between 0 and 1")
        video = args.video.resolve(strict=True)
        expected, expected_path = load_expected(video, args.expect)
        info, info_log = probe(args.ffprobe, video, ["-show_format", "-show_streams"], args.timeout)
        streams = [s for s in info["streams"] if s.get("codec_type") == "video"]
        if not streams:
            raise ValueError("No video stream found")
        data, frame_log = probe(args.ffprobe, video,
            ["-select_streams", "v:0", "-show_frames", "-show_entries",
             "frame=pts,key_frame,pict_type,best_effort_timestamp_time,duration_time,pkt_duration_time,pkt_size,width,height,interlaced_frame"], args.timeout)
        frames = data.get("frames", [])
        summary = summarize(streams[0], frames)
        decode = run([args.ffmpeg, "-hide_banner", "-v", "error", "-xerror", "-err_detect", "explode",
                      "-i", str(video), "-map", "0:v:0", "-an", "-f", "null", "-"], args.timeout)
        headers, trace_code, trace_tail, slices = trace_headers(args.ffmpeg, video, args.timeout)
        # A single shared PPS initial-QP value permits a slice-header QP summary.
        # Multiple distinct bases require PPS-to-slice tracking, which is not inferred.
        base_qps = headers.get("init_qp_minus26" if summary["codec"] == "hevc" else "pic_init_qp_minus26", {})
        deltas = headers.get("slice_qp_delta", {})
        if trace_code == 0 and len(base_qps) == 1 and deltas:
            qp_base = 26 + next(iter(base_qps))
            summary["slice_header_qp"] = {"min": qp_base + min(deltas), "max": qp_base + max(deltas),
                                          "count": sum(deltas.values()),
                                          "note": "Slice initial QP only; excludes coding-unit/macroblock QP deltas and ROI effects."}
        checks = evaluate(summary, headers, expected, decode.returncode == 0 and not decode.stderr.strip(),
                          not (info_log or frame_log), trace_code == 0,
                          args.fps_tolerance, args.bitrate_tolerance)
        qp = qp_analysis(video, frames, summary["codec"], expected, slices, trace_code == 0,
                         decode.returncode == 0 and not decode.stderr.strip(), args.timeout, args.hevc_qp_library)
        checks = [c for c in checks if c["check"] not in ("min_qp", "max_qp")] + qp["checks"]
        status = "FAIL" if any(c["status"] == "FAIL" for c in checks) else "PARTIAL" if any(c["status"] == "SKIP" for c in checks) else "PASS"
        digest = hashlib.sha256()
        with video.open("rb") as file:
            for chunk in iter(lambda: file.read(1024 * 1024), b""):
                digest.update(chunk)
        report = {"input": str(video), "sha256": digest.hexdigest(), "result": status,
                  "expected": expected, "expected_path": expected_path, "summary": summary, "checks": checks, "headers": headers,
                  "qp": qp, "container": info, "diagnostics": {"probe": (info_log + frame_log)[-8000:],
                  "decode_returncode": decode.returncode, "decode": decode.stderr[-8000:],
                  "trace_returncode": trace_code, "trace_tail": trace_tail if trace_code or not headers else [],
                  "selected_video_stream": streams[0]["index"], "video_stream_count": len(streams),
                  "ffmpeg_version": run([args.ffmpeg, "-version"], args.timeout).stdout.splitlines()[0]}}
        base = args.output or video.with_name(video.stem + ".analysis")
        path = Path(str(base) + ".html")
        if path.resolve() == video or (expected_path and path.resolve() == Path(expected_path)):
            raise ValueError("Output must not overwrite the video or expectation file")
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(html_report(report, frames), encoding="utf-8")
        print(f"{status}: {summary['codec']} {summary['width']}x{summary['height']}, {len(frames)} frames")
        for c in checks:
            print(f"  {c['status']:4} {c['check']}: actual={c['actual']} expected={c['expected']}")
        print("Report: " + str(path))
        return 1 if status == "FAIL" or (args.require_all and status == "PARTIAL") else 0
    except (OSError, ValueError, subprocess.TimeoutExpired, KeyError) as exc:
        print(f"Analysis error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
