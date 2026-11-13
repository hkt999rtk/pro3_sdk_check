"""Library discovery must not mix Windows and Linux in a shared checkout."""
import unittest
from unittest.mock import patch
from video_qp import default_hevc_library


class PlatformTests(unittest.TestCase):
    def test_platform_library_when_both_exist(self):
        for platform, suffix in [('win32', 'windows/bin/libde265_qp.dll'),
                                 ('linux', 'linux/bin/libde265_qp.so')]:
            with self.subTest(platform=platform), patch('sys.platform', platform), \
                    patch('pathlib.Path.is_file', return_value=True):
                self.assertTrue(default_hevc_library().as_posix().endswith(suffix))

    def test_foreign_library_is_not_selected(self):
        for platform, foreign_suffix in [('linux', '.dll'), ('win32', '.so')]:
            with self.subTest(platform=platform), patch('sys.platform', platform), \
                    patch('pathlib.Path.is_file', lambda path: path.suffix == foreign_suffix):
                self.assertIsNone(default_hevc_library())
