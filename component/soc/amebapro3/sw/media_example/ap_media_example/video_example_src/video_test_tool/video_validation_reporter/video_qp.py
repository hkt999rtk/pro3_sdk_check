"""Exact luma-QP extraction. Runs native decoders in a timeout-bounded child process."""
import ctypes as C
import json
import re
import sys
from collections import Counter
from pathlib import Path

def default_hevc_library():
    """Select only a library for this OS; Windows and WSL may share a checkout."""
    base = Path(__file__).resolve().parent / 'hevc_qp_backend/build'
    if sys.platform == 'win32':
        candidates = ['windows/bin/libde265_qp.dll', 'bin/libde265_qp.dll',
                      'bin/Release/libde265_qp.dll']
    elif sys.platform == 'linux':
        candidates = ['linux/bin/libde265_qp.so', 'bin/libde265_qp.so']
    else:
        candidates = []
    return next((base / name for name in candidates if (base / name).is_file()), None)


class EncParams(C.Structure):
    # Public FFmpeg AVVideoEncParams ABI; block_offset/size govern trailing data.
    _fields_ = [('nb_blocks', C.c_uint), ('blocks_offset', C.c_size_t),
                ('block_size', C.c_size_t), ('type', C.c_int), ('qp', C.c_int32),
                ('delta_qp', (C.c_int32 * 2) * 4)]


class QPBlock(C.Structure):
    _fields_ = [(k, C.c_int32) for k in ('x', 'y', 'w', 'h', 'qp', 'slice_qp', 'flags')]


def h264_blocks(data):
    if len(data) < C.sizeof(EncParams):
        raise ValueError('Truncated encoding parameter header')
    header = EncParams.from_buffer_copy(data)
    if header.type != 1 or not header.nb_blocks:
        raise ValueError('No H.264 macroblock QP data')
    if (header.blocks_offset < C.sizeof(EncParams) or header.block_size < 20 or
            header.blocks_offset + header.nb_blocks * header.block_size > len(data)):
        raise ValueError('Invalid encoding parameter block layout')
    import struct
    blocks = []
    for i in range(header.nb_blocks):
        x, y, w, h, delta = struct.unpack_from('=iiiii', data, header.blocks_offset + i * header.block_size)
        blocks.append((x, y, w, h, header.qp + delta, None, 0))
    return blocks, header.qp


def frame_stats(blocks, width, height, low, high):
    """Check every block and non-overlapping full-frame coverage on a 4-pixel grid."""
    if not (0 < width <= 16384 and 0 < height <= 16384):
        raise ValueError('Unsupported QP map dimensions')
    cols, rows = (width + 3) // 4, (height + 3) // 4
    coverage = bytearray(cols * rows)
    histogram, pixels = Counter(), Counter()
    pictures = set()
    violations = []
    bad_count = excluded = count = 0
    complete = True
    for x, y, w, h, qp, picture_qp, flags in blocks:
        if x < 0 or y < 0 or w <= 0 or h <= 0 or x >= width or y >= height:
            raise ValueError('Invalid QP block coordinates')
        w, h = min(w, width - x), min(h, height - y)
        if x % 4 or y % 4 or (w % 4 and x+w != width) or (h % 4 and y+h != height):
            raise ValueError('QP block is not aligned to the coverage grid')
        for gy in range(y // 4, (y + h + 3) // 4):
            start, end = gy * cols + x // 4, gy * cols + (x + w + 3) // 4
            if any(coverage[start:end]):
                complete = False
            coverage[start:end] = b'\1' * (end - start)
        if picture_qp is not None:
            pictures.add(picture_qp)
        count += 1
        if flags:
            excluded += 1
            continue  # PCM/bypass has no ordinary quantization; never claim full PASS.
        if not -48 <= qp <= 63:
            raise ValueError('Decoder returned an invalid luma QP')
        histogram[qp] += 1
        pixels[qp] += w*h
        if (low is not None and qp < low) or (high is not None and qp > high):
            bad_count += 1
            if len(violations) < 64:
                violations.append({'x': x, 'y': y, 'w': w, 'h': h, 'qp': qp})
    return {'complete': bool(count) and complete and all(coverage) and not excluded,
            'blocks': count, 'excluded_blocks': excluded,
            'min': min(histogram) if histogram else None, 'max': max(histogram) if histogram else None,
            'mean': sum(q*n for q,n in pixels.items()) / sum(pixels.values()) if pixels else None,
            'histogram': dict(sorted(histogram.items())), 'picture_qps': sorted(pictures),
            'out_of_range_blocks': bad_count, 'violations': violations,
            'omitted_violation_locations': bad_count - len(violations)}


class SliceTrace:
    """Associate H264 slice initial QPs with packet PTS, respecting PPS replacements."""
    def __init__(self):
        self.pps = {}
        self.section = None
        self.fields = {}
        self.pts = None
        self.values = {}
        self.invalid = set()

    def finish(self):
        f = self.fields
        if self.section == 'Picture Parameter Set':
            if 'pic_parameter_set_id' in f and 'pic_init_qp_minus26' in f:
                self.pps[f['pic_parameter_set_id']] = 26 + f['pic_init_qp_minus26']
        if self.section == 'Slice Header' and self.pts is not None:
            pps = self.pps.get(f.get('pic_parameter_set_id'))
            if pps is None or 'slice_qp_delta' not in f:
                self.invalid.add(self.pts)
            else:
                self.values.setdefault(self.pts, []).append(pps + f['slice_qp_delta'])
        self.fields = {}
        self.section = None

    def feed(self, line):
        if '[trace_headers' not in line:
            return
        text = line.split('] ', 1)[-1].strip()
        if text.startswith('Packet:'):
            self.finish()
            m = re.search(r'\bpts (-?\d+)', text)
            self.pts = int(m[1]) if m else None
        elif re.fullmatch(r'[A-Za-z][A-Za-z /-]+', text):
            self.finish()
            self.section = text
        else:
            m = re.match(r'\d+\s+(\w+)\s+[01]+\s+=\s+(-?\d+)\s*$', text)
            if m:
                self.fields[m[1]] = int(m[2])


def extract_h264(av, container, stream, low, high):
    stream.codec_context.options = {'export_side_data': '+venc_params', 'err_detect': 'explode'}
    result, slices = [], SliceTrace()
    def capture(logs):
        for level, context, message in logs:
            if context == 'trace_headers':
                for line in message.splitlines():
                    slices.feed('[trace_headers] ' + line)
    def record(frame):
        if frame.format.name != 'yuv420p' or frame.interlaced_frame or frame.is_corrupt:
            raise ValueError('QP backend currently requires clean progressive 8-bit 4:2:0 pictures')
        side = next((s for s in frame.side_data if s.type.name == 'VIDEO_ENC_PARAMS'), None)
        if side is None:
            raise ValueError('PyAV/FFmpeg decoder did not export VIDEO_ENC_PARAMS')
        blocks, pps_qp = h264_blocks(bytes(side))
        row = frame_stats(blocks, frame.width, frame.height, low, high)
        row.update(pts=frame.pts, width=frame.width, height=frame.height, pps_initial_qp=pps_qp)
        result.append(row)
    previous_level = av.logging.get_level()
    av.logging.set_level(av.logging.INFO)
    try:
        # Capture complete log records synchronously; CLI stderr can interleave
        # trace fragments from the muxer and progress/output threads.
        with av.logging.Capture() as logs:
            trace = av.BitStreamFilterContext('trace_headers', stream)
        capture(logs)
        for packet in container.demux(stream):
            if not packet.size:
                continue
            with av.logging.Capture() as logs:
                packets = trace.filter(packet)
            capture(logs)
            for filtered in packets:
                for frame in stream.codec_context.decode(filtered):
                    record(frame)
        with av.logging.Capture() as logs:
            packets = trace.filter(None)
        capture(logs)
        for packet in packets:
            for frame in stream.codec_context.decode(packet):
                record(frame)
        for frame in stream.codec_context.decode(None):
            record(frame)
        slices.finish()
        for row in result:
            row['picture_qps'] = sorted(set(slices.values.get(row['pts'], [])))
            row['picture_complete'] = bool(row['picture_qps']) and row['pts'] not in slices.invalid
        return result, 'PyAV ' + av.__version__ + ' / FFmpeg H.264 VIDEO_ENC_PARAMS + synchronous slice trace'
    finally:
        av.logging.set_level(previous_level)


def hvcc_nals(extra):
    if len(extra) < 23 or extra[0] != 1:
        raise ValueError('HEVC backend requires MP4 hvcC extradata')
    length_size, n, pos = (extra[21] & 3) + 1, extra[22], 23
    nals = []
    for _ in range(n):
        if pos + 3 > len(extra): raise ValueError('Truncated hvcC array')
        count = int.from_bytes(extra[pos+1:pos+3], 'big'); pos += 3
        for _ in range(count):
            if pos + 2 > len(extra): raise ValueError('Truncated hvcC NAL length')
            size = int.from_bytes(extra[pos:pos+2], 'big'); pos += 2
            if size < 2 or pos + size > len(extra): raise ValueError('Truncated hvcC NAL')
            nals.append(extra[pos:pos+size]); pos += size
    return length_size, nals


def packet_nals(data, length_size):
    pos = 0
    while pos < len(data):
        if pos + length_size > len(data): raise ValueError('Truncated NAL length')
        size = int.from_bytes(data[pos:pos+length_size], 'big'); pos += length_size
        if size < 2 or pos+size > len(data): raise ValueError('Invalid NAL size')
        yield data[pos:pos+size]
        pos += size


def extract_hevc(container, stream, low, high, library):
    if not library:
        raise ValueError('Exact HEVC block QP requires libde265_qp; see hevc_qp_backend/README.md')
    lib = C.CDLL(str(Path(library).resolve()))
    def bind(name, ret, *args):
        fn = getattr(lib, name); fn.restype = ret; fn.argtypes = list(args); return fn
    ptr, integer = C.c_void_p, C.c_int
    abi = bind('video_qp_abi', integer)
    if abi() != 1: raise ValueError('Unsupported QP helper ABI')
    copy = bind('video_qp_copy', integer, ptr, C.POINTER(QPBlock), integer, C.POINTER(integer), C.POINTER(integer))
    version = bind('de265_get_version', C.c_char_p)
    new = bind('de265_new_decoder', ptr)
    free = bind('de265_free_decoder', integer, ptr)
    push = bind('de265_push_NAL', integer, ptr, ptr, integer, C.c_int64, ptr)
    decode = bind('de265_decode', integer, ptr, C.POINTER(integer))
    peek = bind('de265_peek_next_picture', ptr, ptr)
    release = bind('de265_release_next_picture', None, ptr)
    pts = bind('de265_get_image_PTS', C.c_int64, ptr)
    bits = bind('de265_get_bits_per_pixel', integer, ptr, integer)
    warn = bind('de265_get_warning', integer, ptr)
    flush = bind('de265_flush_data', integer, ptr)
    end_frame = bind('de265_push_end_of_frame', None, ptr)
    ctx = new()
    if not ctx: raise ValueError('Cannot allocate HEVC decoder')
    result = []
    def drain():
        while True:
            img = peek(ctx)
            if not img: break
            if bits(img, 0) != 8: raise ValueError('HEVC QP backend currently supports 8-bit luma')
            width, height = integer(), integer()
            count = copy(img, None, 0, C.byref(width), C.byref(height))
            if not 0 < count <= 4194304: raise ValueError('Invalid HEVC coding-block count')
            blocks = (QPBlock * count)()
            if copy(img, blocks, count, C.byref(width), C.byref(height)) != count:
                raise ValueError('Incomplete HEVC QP map')
            row = frame_stats(((b.x,b.y,b.w,b.h,b.qp,b.slice_qp,b.flags) for b in blocks),
                              width.value, height.value, low, high)
            row.update(pts=pts(img), width=width.value, height=height.value)
            result.append(row)
            release(ctx)
    def decode_pending():
        for _ in range(100000):
            more = integer()
            err = decode(ctx, C.byref(more))
            drain()
            warning = warn(ctx)
            if warning: raise ValueError('HEVC decoder warning ' + str(warning))
            if err == 13: return  # waiting for more input
            if err not in (0, 9): raise ValueError('HEVC decoder error ' + str(err))
            if not more.value: return
        raise ValueError('HEVC decoder made no progress')
    try:
        length_size, nals = hvcc_nals(stream.codec_context.extradata)
        for nal in nals:
            if push(ctx, nal, len(nal), 0, None): raise ValueError('HEVC extradata rejected')
        for packet in container.demux(stream):
            if not packet.size: continue
            if packet.pts is None: raise ValueError('HEVC packet has no PTS')
            for nal in packet_nals(bytes(packet), length_size):
                if push(ctx, nal, len(nal), packet.pts, None): raise ValueError('HEVC NAL rejected')
            end_frame(ctx)
            decode_pending()
        if flush(ctx): raise ValueError('HEVC flush failed')
        decode_pending()
        drain()
        return result, 'libde265 ' + version().decode() + ' / exact coding-block QPY export ABI 1'
    finally:
        free(ctx)


def worker(request):
    try:
        import av
        with av.open(request['video']) as container:
            stream = container.streams.video[0]
            if stream.codec_context.format and stream.codec_context.format.name != 'yuv420p':
                raise ValueError('QP workflow currently requires 8-bit 4:2:0 video')
            if stream.codec_context.name == 'h264':
                rows, backend = extract_h264(av, container, stream, request['min_qp'], request['max_qp'])
            elif stream.codec_context.name == 'hevc':
                rows, backend = extract_hevc(container, stream, request['min_qp'], request['max_qp'], request['library'])
            else: raise ValueError('QP extraction supports H264/HEVC only')
        return {'frames': rows, 'backend': backend, 'error': ''}
    except Exception as exc:
        return {'frames': [], 'backend': '', 'error': f'{type(exc).__name__}: {exc}'}


if __name__ == '__main__':
    request = json.loads(Path(sys.argv[1]).read_text(encoding='utf-8'))
    Path(sys.argv[2]).write_text(json.dumps(worker(request)), encoding='utf-8')
