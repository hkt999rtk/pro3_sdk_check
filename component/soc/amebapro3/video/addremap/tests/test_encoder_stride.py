"""Execute the production preprocessing stride branch without the hardware SDK."""
from pathlib import Path
import subprocess
import tempfile
import unittest


class EncoderStrideTest(unittest.TestCase):
    def test_uncompressed_rtt_stride_matches_isp(self):
        soc = Path(__file__).resolve().parents[3]
        source = (soc / "sw/media/mmfv2/module/module_encoder.c").read_text()
        start = source.index("\t\tif (enc_ctx->enc_parm.decompEnable", source.index("/* PreP setup */"))
        end = source.index('\t\tENCODER_DBG_DEBUG("Set origWidth', start)
        body = source[start:end]
        program = r'''
#include <assert.h>
#include <stdint.h>
#define ENC_ALIGN(x,a) (((x)+(a)-1)&~((a)-1))
static uint32_t stride(uint32_t width, int decomp, int lowlatency) {
    struct {
        struct { uint32_t width; int decompEnable, lowLatencyEnable; } enc_parm;
        struct { uint32_t origWidth; } hevc_preProcCfg;
    } context = {{width, decomp, lowlatency}, {0}}, *enc_ctx = &context;
''' + body + r'''
    return enc_ctx->hevc_preProcCfg.origWidth;
}
int main(void) {
    assert(stride(1920, 0, 1) == 1920);
    assert(stride(2160, 0, 1) == 2176);
    assert(stride(1920, 1, 1) == 2048);
    assert(stride(1920, 1, 0) == 2048);
    assert(stride(1920, 0, 0) == 1920);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as folder:
            src = Path(folder) / "stride.c"
            exe = Path(folder) / "stride"
            src.write_text(program)
            subprocess.run(["cc", "-Wall", "-Wextra", "-Werror", str(src), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


    def test_comparison_rejects_different_fps(self):
        soc = Path(__file__).resolve().parents[3]
        source = (soc / "sw/media_example/ap_media_example/video_example_src/video_test_tool/mmf2_video_cmd_test_tool.c").read_text()
        start = source.index("if (!video_channel_enabled[0]", source.index("static int video_validate_selected_params"))
        end = source.index(") {", start)
        condition = source[start + 4:end]
        program = r"""
#include <assert.h>
#define VIDEO_H264 1
static int video_channel_enabled[5] = {1,0,0,0,0};
static int video_ainr_enabled, video_nn_enabled, video_preview_mode, video_record_source;
static struct { int format, width, height, fps; struct { int sliceSize; } coding; }
    video_params[1] = {{VIDEO_H264, 1920, 1080, 15, {0}}};
static int video_enabled_channel_count(void) { return 1; }
static int valid(void) { return !(""" + condition + r"""); }
int main(void) {
    assert(valid());
    video_params[0].fps = 30;
    assert(!valid());
    video_params[0].fps = 15;
    video_params[0].coding.sliceSize = 1;
    assert(!valid());
    return 0;
}
"""
        with tempfile.TemporaryDirectory() as folder:
            src = Path(folder) / "validation.c"
            exe = Path(folder) / "validation"
            src.write_text(program)
            subprocess.run(["cc", "-Wall", "-Wextra", "-Werror", str(src), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    unittest.main()
