"""Real decoder tests using small, locally generated known-QP clips (no firmware)."""
import importlib.util
import json
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

from analyze_video import probe, trace_headers, summarize_qp
from video_qp import worker, default_hevc_library


@unittest.skipUnless(shutil.which('ffmpeg') and shutil.which('ffprobe') and importlib.util.find_spec('av'),
                     'Requires ffmpeg, ffprobe and the prebuilt PyAV package')
class DecoderIntegrationTests(unittest.TestCase):
    def check_codec(self, codec):
        library = default_hevc_library()
        if codec == 'hevc':
            self.assertIsNotNone(library, 'Run setup_analysis.py to build the HEVC QP helper')
        with tempfile.TemporaryDirectory(prefix='qp-test-') as directory:
            video = Path(directory)/'known.mp4'
            codec_options = ['-c:v','libx264','-qp','30','-x264-params','ipratio=1:pbratio=1:aq-mode=0:mbtree=0:slices=2'] if codec == 'h264' else [
                '-c:v','libx265','-x265-params','qp=30:aq-mode=0:cutree=0:ipratio=1:pbratio=1:bframes=2:log-level=error:pools=none']
            subprocess.run(['ffmpeg','-nostdin','-v','error','-f','lavfi','-i','testsrc2=size=128x96:rate=6',
                            '-frames:v','6',*codec_options,str(video)],check=True,stdin=subprocess.DEVNULL,capture_output=True,timeout=60)
            data, log = probe('ffprobe',video,['-select_streams','v:0','-show_frames'],60)
            fields, code, tail, slices = trace_headers('ffmpeg',video,60)
            raw = worker({'video':str(video),'min_qp':30,'max_qp':30,'library':str(library)})
            self.assertEqual(raw['error'],'')
            self.assertEqual(len(raw['frames']),6)
            report = summarize_qp(raw,data['frames'],codec,{'min_qp':30,'max_qp':30},slices,code==0,not log)
            self.assertTrue(report['aligned'])
            self.assertEqual([c['status'] for c in report['checks']],['PASS','PASS'])
            self.assertTrue(all(r['min']==r['max']==30 for r in raw['frames']))
            # Re-extract with a deliberately wrong bound: all block positions must fail.
            raw = worker({'video':str(video),'min_qp':31,'max_qp':40,'library':str(library)})
            report = summarize_qp(raw,data['frames'],codec,{'min_qp':31,'max_qp':40},slices,True,True)
            self.assertEqual([c['status'] for c in report['checks']],['FAIL','FAIL'])
            self.assertTrue(all(r['out_of_range_blocks']==r['blocks'] for r in raw['frames']))
            self.assertTrue(all(r['violations'] for r in raw['frames']))

    def test_h264_multislice_and_reordered_pictures(self):
        self.check_codec('h264')

    def test_hevc_known_qp_and_reordered_pictures(self):
        self.check_codec('hevc')


if __name__=='__main__': unittest.main()
