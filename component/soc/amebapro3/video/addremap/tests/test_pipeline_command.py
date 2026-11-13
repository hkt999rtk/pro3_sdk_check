"""Run the actual UART handler and parser with host stubs for logging/state."""
from pathlib import Path
import subprocess
import tempfile
import unittest


class PipelineCommandTest(unittest.TestCase):
    def test_select_query_and_reboot_lock(self):
        soc = Path(__file__).resolve().parents[3]
        source = (soc / "sw/media_example/ap_media_example/video_example_src/video_test_tool/mmf2_video_cmd_test_tool.c").read_text()
        parser = source[source.index("static int video_parse_u32("):source.index("static int video_parse_i32(")]
        handler = source[source.index("void fVIDPIPE("):source.index("void fVIDTEST(")]
        handler = handler.removesuffix("#endif\n\n")
        program = r'''
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
typedef uint16_t u16;
#define RTK_LOGI(...) ((void)0)
#define VIDEO_AT_OK 0
#define VIDEO_AT_ERR_PARAM 1
#define VIDEO_AT_ERR_STATE 2
static int video_comparison_started, running, result;
static struct { uint32_t isp_slice_en; } video_params[1];
static int video_require_stopped(void) { return running ? -1 : 0; }
static void video_at_finish(int error) { result = error; }
''' + parser + handler + r'''
int main(void) {
    char *one[] = {"+VIDPIPE", "1"};
    char *zero[] = {"+VIDPIPE", "0"};
    char *bad[] = {"+VIDPIPE", "2"};
    assert(video_params[0].isp_slice_en == 0);
    fVIDPIPE(2, one);
    assert(result == VIDEO_AT_OK && video_params[0].isp_slice_en == 1);
    fVIDPIPE(2, bad);
    assert(result == VIDEO_AT_ERR_PARAM && video_params[0].isp_slice_en == 1);
    bad[1] = "-1";
    fVIDPIPE(2, bad);
    assert(result == VIDEO_AT_ERR_PARAM && video_params[0].isp_slice_en == 1);
    bad[1] = "1junk";
    fVIDPIPE(2, bad);
    assert(result == VIDEO_AT_ERR_PARAM && video_params[0].isp_slice_en == 1);
    running = 1;
    fVIDPIPE(2, zero);
    assert(result == VIDEO_AT_ERR_STATE && video_params[0].isp_slice_en == 1);
    running = 0; video_comparison_started = 1;
    fVIDPIPE(2, zero);
    assert(result == VIDEO_AT_ERR_STATE && video_params[0].isp_slice_en == 1);
    fVIDPIPE(1, one);
    assert(result == VIDEO_AT_OK && video_params[0].isp_slice_en == 1);
    /* A reboot clears BSS; then the same firmware accepts the other mode. */
    video_comparison_started = 0;
    fVIDPIPE(2, zero);
    assert(result == VIDEO_AT_OK && video_params[0].isp_slice_en == 0);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as folder:
            src = Path(folder) / "command.c"
            exe = Path(folder) / "command"
            src.write_text(program)
            subprocess.run(["cc", "-Wall", "-Wextra", "-Werror", str(src), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


    def test_command_pairs_isp_slice_with_encoder_lowlatency(self):
        soc = Path(__file__).resolve().parents[3]
        source = (soc / "sw/media/mmfv2/module/module_video.c").read_text()
        start = source.index("static int video_cmd_gen(")
        end = source.index("#define APPEND_ENC_PARAM", start)
        # Compile the production command prefix; later coding extras are unrelated.
        prefix = source[start:end] + "}\nreturn 0;\nerror: return -1;\n}\n"
        program = r"""
#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef unsigned long uint32_t;
#define CONFIG_VMODULE_ADDREMAP 1
#define VIDEO_DBG_ERROR(...) ((void)0)
#define VIDEO_H264 1
#define VIDEO_HEVC 2
#define VIDEO_JPEG 4
#define VIDEO_NV12 8
#define VIDEO_NV16 16
#define VIDEO_YUY2 32
#define VIDEO_BAYER_RAW 64
#define VIDEO_RGB 128
typedef struct {
    uint32_t isp_slice_en, stream_id, format, ainr_en, width, height, fps,
             gop, bps, rc_mode, min_qp, max_qp, qlevel;
} video_params_t;
""" + prefix + r"""
int main(void) {
    char cmd[512];
    video_params_t params = {.format=VIDEO_H264, .width=1920, .height=1080, .fps=15};
    assert(video_cmd_gen(cmd, sizeof(cmd), &params) == 0);
    assert(strstr(cmd, ",slice=0 ") && strstr(cmd, ",lowlatency=0"));
    params.isp_slice_en = 1;
    assert(video_cmd_gen(cmd, sizeof(cmd), &params) == 0);
    assert(strstr(cmd, ",slice=1 ") && strstr(cmd, ",lowlatency=1"));
    assert(video_cmd_gen(cmd, 8, &params) < 0);
    params.stream_id = 1;
    assert(video_cmd_gen(cmd, sizeof(cmd), &params) < 0);
    params.stream_id = 0; params.ainr_en = 1;
    assert(video_cmd_gen(cmd, sizeof(cmd), &params) < 0);
    params.ainr_en = 0; params.isp_slice_en = 2;
    assert(video_cmd_gen(cmd, sizeof(cmd), &params) < 0);
    return 0;
}
"""
        with tempfile.TemporaryDirectory() as folder:
            src = Path(folder) / "prefix.c"
            exe = Path(folder) / "prefix"
            src.write_text(program)
            subprocess.run(["cc", "-Wall", "-Wextra", "-Werror", str(src), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    unittest.main()
