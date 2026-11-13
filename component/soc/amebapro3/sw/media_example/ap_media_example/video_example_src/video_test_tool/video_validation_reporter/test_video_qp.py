"""QP bounds and completeness tests; no firmware/native compilation."""
import ctypes as C
import struct
import unittest
from video_qp import EncParams, frame_stats, h264_blocks, SliceTrace, hvcc_nals, packet_nals
from analyze_video import summarize_qp, validate_expected


class QPTests(unittest.TestCase):
    def test_exact_limits_and_local_delta(self):
        r = frame_stats([(0,0,8,8,26,30,0),(8,0,8,8,48,30,0)],16,8,26,48)
        self.assertTrue(r['complete'])
        self.assertEqual((r['min'],r['max'],r['out_of_range_blocks']),(26,48,0))
        r = frame_stats([(0,0,8,8,25,30,0),(8,0,8,8,49,30,0)],16,8,26,48)
        self.assertEqual(r['out_of_range_blocks'],2)
        self.assertEqual(r['violations'][1],{'x':8,'y':0,'w':8,'h':8,'qp':49})

    def test_missing_duplicate_and_bypass_blocks_prevent_pass(self):
        block=(0,0,8,8,30,30,0)
        self.assertFalse(frame_stats([block],16,8,26,48)['complete'])
        self.assertFalse(frame_stats([block,block],8,8,26,48)['complete'])
        self.assertFalse(frame_stats([(0,0,8,8,30,30,2)],8,8,26,48)['complete'])

    def test_edge_clipping_and_weighted_average(self):
        r=frame_stats([(0,0,8,8,30,30,0),(8,0,8,8,40,30,0)],10,8,26,48)
        self.assertTrue(r['complete'])
        self.assertEqual(r['mean'],32)

    def test_limit_validation(self):
        for expected in ({'min_qp':27,'max_qp':26},{'min_qp':-1},{'max_qp':52},{'min_qp':True},{'max_qp':2.5}):
            with self.assertRaises(ValueError): validate_expected(expected)

    def test_h264_side_data_reads_final_qp_not_pps_base(self):
        h=EncParams();h.nb_blocks=2;h.type=1;h.qp=37;h.blocks_offset=C.sizeof(h);h.block_size=24
        data=bytes(h)+struct.pack('=iiiiii',0,0,16,16,-11,0)+struct.pack('=iiiiii',16,0,16,16,12,0)
        blocks,base=h264_blocks(data)
        self.assertEqual(base,37)
        self.assertEqual([b[4] for b in blocks],[26,49])
        with self.assertRaises(ValueError): h264_blocks(data[:-1])

    def test_slice_trace_respects_pps_and_reordered_pts(self):
        t=SliceTrace()
        for line in ['Picture Parameter Set','0 pic_parameter_set_id 1 = 0','1 pic_init_qp_minus26 1 = 11',
                     'Packet: 4 bytes, pts 90, dts 0, duration 30.', 'Slice Header',
                     '0 pic_parameter_set_id 1 = 0','1 slice_qp_delta 1 = -11',
                     'Picture Parameter Set','0 pic_parameter_set_id 1 = 1','1 pic_init_qp_minus26 1 = 0',
                     'Packet: 4 bytes, pts 30, dts 30, duration 30.', 'Slice Header',
                     '0 pic_parameter_set_id 1 = 1','1 slice_qp_delta 1 = 4']:
            t.feed('[trace_headers @ abc] '+line)
        t.finish()
        self.assertEqual(t.values,{90:[26],30:[30]})

    def test_missing_pps_never_infers_picture_qp(self):
        t=SliceTrace()
        for line in ['Packet: 4 bytes, pts 0, dts 0, duration 30.','Slice Header','0 pic_parameter_set_id 1 = 4','1 slice_qp_delta 1 = 0']:
            t.feed('[trace_headers @ abc] '+line)
        t.finish();self.assertIn(0,t.invalid);self.assertFalse(t.values)

    def test_frame_alignment_and_fail_overrides_incomplete(self):
        frame={'pts':0,'width':16,'height':8,'pict_type':'P','best_effort_timestamp_time':'0'}
        row=frame_stats([(0,0,8,8,49,30,0)],16,8,26,48)
        row.update(pts=0,width=16,height=8)
        raw={'frames':[row],'backend':'test','error':''}
        r=summarize_qp(raw,[frame],'hevc',{'min_qp':26,'max_qp':48},SliceTrace(),True,True)
        self.assertEqual(r['checks'][1]['status'],'FAIL')
        row['pts']=1
        r=summarize_qp(raw,[frame],'hevc',{'min_qp':26,'max_qp':48},SliceTrace(),True,True)
        self.assertEqual(r['checks'][1]['status'],'SKIP')

    def test_truncated_nal_rejected(self):
        with self.assertRaises(ValueError): list(packet_nals(b'\x00\x00\x00\x10abc',4))
        with self.assertRaises(ValueError): hvcc_nals(b'bad')

    def test_violation_location_limit_does_not_change_count(self):
        blocks=[(i*8,0,8,8,49,30,0) for i in range(70)]
        r=frame_stats(blocks,560,8,26,48)
        self.assertEqual(r['out_of_range_blocks'],70)
        self.assertEqual(len(r['violations']),64)
        self.assertEqual(r['omitted_violation_locations'],6)


if __name__=='__main__': unittest.main()
