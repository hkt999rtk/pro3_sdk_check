"""Pure-Python analysis regression tests; no SDK build or hardware required."""
import unittest
import tempfile
import json
from pathlib import Path
from analyze_video import load_expected, html_report

from analyze_video import TRACE_PATTERN, evaluate, summarize, validate_expected


def frames(count, fps):
    return [{"key_frame": int(i % 15 == 0), "pict_type": "I" if i % 15 == 0 else "P",
             "best_effort_timestamp_time": str(i / fps), "duration_time": str(1 / fps),
             "pkt_size": "1000"} for i in range(count)]


class ReportTests(unittest.TestCase):
    def test_snapshot_pairing(self):
        with tempfile.TemporaryDirectory() as directory:
            video = Path(directory) / "video_test_v2_000123.mp4"
            snapshot = video.with_name(video.stem + "_params.expected.json")
            expected = {"_video_file": video.name, "_record_id": "v2_000123", "fps": 30}
            snapshot.write_text(json.dumps(expected))
            self.assertEqual(load_expected(video)[0], expected)
            expected["_record_id"] = "v2_000124"
            snapshot.write_text(json.dumps(expected))
            with self.assertRaises(ValueError):
                load_expected(video)
            expected["_record_id"] = "v2_000123"
            expected["_video_file"] = "other.mp4"
            snapshot.write_text(json.dumps(expected))
            with self.assertRaises(ValueError):
                load_expected(video)

    def test_html_escapes_data_and_preserves_details(self):
        report = {"input": "clip.mp4", "result": "PARTIAL", "sha256": "abc",
                  "expected": {"_note": "<script>alert(1)</script>"}, "summary": {"fps": 15},
                  "checks": [{"check": "x", "status": "SKIP", "expected": 1, "actual": None, "detail": "unknown"}],
                  "headers": {"test": {1: 150}}, "diagnostics": {"decode": "success"}}
        result = html_report(report, frames(3, 15))
        self.assertNotIn("<script>", result)
        self.assertIn("&lt;script&gt;", result)
        for field in ("Per-frame data (3 frames)", "Packet bytes", "Bitstream headers", "diagnostics", "success"):
            self.assertIn(field, result)


class AnalysisTests(unittest.TestCase):
    def setUp(self):
        self.summary = summarize({"codec_name": "hevc", "width": 2688, "height": 1520,
                                  "r_frame_rate": "30000/1001"}, frames(150, 15))

    def checks(self, expected, headers=None, trace=True):
        return {c["check"]: c for c in evaluate(self.summary, headers or {}, expected, True, True, trace, .05, .2)}

    def test_nominal_fps_does_not_override_timestamps(self):
        self.assertAlmostEqual(self.summary["timestamp_fps"], 15)
        self.assertEqual(self.checks({"fps": 15})["fps"]["status"], "PASS")
        self.assertEqual(self.checks({"fps": 30})["fps"]["status"], "FAIL")

    def test_gop_and_recording_count(self):
        checks = self.checks({"gop": 15, "fps": 15, "record_seconds": 10})
        self.assertEqual(checks["gop"]["status"], "PASS")
        self.assertEqual(checks["record_sample_count"]["status"], "PASS")
        self.assertEqual(self.checks({"gop": 30})["gop"]["status"], "FAIL")

    def test_missing_or_failed_header_scan_cannot_pass_disabled_setting(self):
        self.assertEqual(self.checks({"sao": 0})["sao"]["status"], "SKIP")
        headers = {"sample_adaptive_offset_enabled_flag": {0: 1}}
        self.assertEqual(self.checks({"sao": 0}, headers, False)["sao"]["status"], "SKIP")

    def test_changing_parameter_sets_fail_exact_match(self):
        headers = {"sample_adaptive_offset_enabled_flag": {0: 1, 1: 5}}
        self.assertEqual(self.checks({"sao": 1}, headers)["sao"]["status"], "FAIL")

    def test_vbr_and_qp_are_not_falsely_verified(self):
        checks = self.checks({"bps": 2000000, "rc_mode": "VBR", "min_qp": 26, "ainr": 1})
        self.assertTrue(all(checks[k]["status"] == "SKIP" for k in ("bps", "rc_mode", "min_qp", "ainr")))
        self.assertEqual(self.checks({"bps": 2000000, "rc_mode": "CBR"})["bps"]["status"], "FAIL")

    def test_missing_and_duplicate_timestamps_fail(self):
        data = frames(3, 15)
        data[1].pop("best_effort_timestamp_time")
        data[2]["best_effort_timestamp_time"] = "0"
        self.summary = summarize({}, data)
        self.assertEqual(self.checks({})["timestamps"]["status"], "FAIL")

    def test_short_clip_has_no_gop_evidence(self):
        self.summary = summarize({}, frames(2, 15))
        self.assertEqual(self.checks({"gop": 15})["gop"]["status"], "SKIP")

    def test_trace_parses_negative_qp_delta(self):
        match = TRACE_PATTERN.search("[trace_headers @ 00001234] 24 slice_qp_delta 0001011 = -5")
        self.assertEqual(match.groups(), ("slice_qp_delta", "-5"))

    def test_sdk_profile_enum_is_not_profile_idc(self):
        check = self.checks({"profile": 0}, {"general_profile_idc": {1: 2}})["profile"]
        self.assertEqual(check["status"], "PASS")

    def test_invalid_expected_numeric_values(self):
        for value in (0, -1, "15", float("nan"), float("inf"), True):
            with self.subTest(value=value), self.assertRaises(ValueError):
                validate_expected({"fps": value})


if __name__ == "__main__":
    unittest.main()
