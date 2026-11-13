# Ameba OSD Sans

The generated `../video_osd_font_data.h` is a renamed, monochrome ASCII derivative
of **Liberation Sans Bold 2.1.5**, a modern sans-serif source font with metrics
compatible with Arial. This is not Microsoft's Arial font. The derivative is
named **Ameba OSD Sans** to respect the source font's reserved font names.

Source and license:

- Upstream: https://github.com/liberationfonts/liberation-fonts
- Official archive: https://github.com/liberationfonts/liberation-fonts/files/7261482/liberation-fonts-ttf-2.1.5.tar.gz
- Input: `LiberationSans-Bold.ttf`
- Input SHA-256: `788abee4c806d660e8aee46689dd8540cd4bb98da03dcc9d171ce3efd99a9173`
- Font data license: SIL Open Font License 1.1, with the original copyright and
  reserved-name notices in `OFL.txt`. Retain that file when distributing the font
  data with the SDK/product. The font-data license does not relicense the SDK.

## Rendering and compatibility

- 95 printable ASCII characters, independently rasterized from the Bold outlines
  (not bitmap dilation). To fit every glyph without clipping, 8/16/24/40-pixel
  cells use native 7/15/24/40 ppem respectively
- FreeType monochrome hinted rendering; no grayscale, dithering or runtime TTF
- 8x8, 16x16, 24x24 and 40x40 storage cells, with a shared baseline within each size
- Proportional advances based on the native hinted font metrics, expanded only
  when necessary to contain the ink without overhang, then padded by 1/1/2/3 pixels
  for scale 1/2/3/5. Advances are capped at the storage cell width to preserve the
  existing worst-case reservation. Digits have equal advances within each size
  (5/9/15/25 pixels); punctuation and spaces use narrower advances
- Drawing and `video_osd_font_string_width()` use the same advances, without
  kerning. Glyph height and APIs are unchanged, but text occupies less width
- Timestamp still reserves `max_chars * 8 * scale` pixels and aligns measured
  text within it. Its protection bounds do not change with the displayed time
- At scale 3, `2026-09-22 18:35:07` advances 259 pixels (unspaced Regular used 219;
  the original fixed-cell layout used 456). Existing
  NN callers doing manual horizontal layout must use the width helper instead
  of `strlen(text) * NN_OSD_FONT_W * scale`
- Timestamp accepts scale 1/2/3/5, default 3. Set `config.font_scale = 5` for
  large text: the sample timestamp advances 428 pixels, in a fixed 760x40
  protection area for `max_chars=19`. That entire area plus placement margins
  must fit the frame or start returns an error. Two source bitmaps use 7600
  pixel bytes, excluding cache alignment and manager output buffers
- Scale 1/2/3/5 selects the corresponding native table. Other positive NN scales
  (including 4) retain nearest-neighbor resampling of the 24-pixel table
- Bitmaps occupy 760 + 3040 + 6840 + 19000 = 29640 bytes, plus 380 advance bytes
  (30020 total). Adding scale 5 costs 19095 constant bytes; no scale-4 font table
  is stored. These are font payload sizes, not measured linked-image deltas
- No new dynamic font memory, task or target-side library dependency

## Reproduction (host asset tooling only)

Use Python with Pillow 12.2.0 and its FreeType 2.14.3 runtime. Extract the input
TTF from the official archive above; it is not needed in the firmware or SDK
build. `../tools/generate_font.py` verifies its SHA-256 before rasterization.
Its `LETTER_SPACING` table defines the additional per-size spacing.

Run the script with the absolute TTF path to emit the C header on stdout. Use
`--check <absolute-path-to-video_osd_font_data.h>` to verify checked-in data, and
optionally `--preview <absolute-output.png>` to view the exact generated pixels.
The preview's enlarged rows use nearest-neighbor scaling, not smoothing.

Each generated size is checked for bounds, visible non-space ASCII characters
and a blank space. Firmware rendering/clipping tests are available in
`../tests/video_osd_font_test.c`; those require a separately authorized test build.
