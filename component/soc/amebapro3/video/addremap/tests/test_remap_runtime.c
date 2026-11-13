#include <assert.h>
#include <string.h>
#include "ameba_soc.h"
#include "addremap_hal.h"

static u32 isp[64], enc[64], burst, align_mode;
u32 test_read32(u32 base, u32 offset)
{
    if (base == ISP_REG_BASE) {
        assert(offset == 0xF0190 || offset == 0xD8034);
        return offset == 0xF0190 ? burst : align_mode;
    }
    assert(offset < sizeof(isp) && offset % 4 == 0);
    assert(base == REMAP_ISP_REG_BASE || base == REMAP_ENC_REG_BASE);
    return (base == REMAP_ISP_REG_BASE ? isp : enc)[offset / 4];
}
void test_write32(u32 base, u32 offset, u32 value)
{
    if (base == ISP_REG_BASE) {
        assert(offset == 0xF0190 || offset == 0xD8034);
        if (offset == 0xF0190) burst = value;
        else align_mode = value;
        return;
    }
    assert(offset < sizeof(isp) && offset % 4 == 0);
    assert(base == REMAP_ISP_REG_BASE || base == REMAP_ENC_REG_BASE);
    (base == REMAP_ISP_REG_BASE ? isp : enc)[offset / 4] = value;
}
extern u32 isp_remap_preprocess(struct isp_remap_config *, u32, u32, u32, u32, u32);
extern int set_isp_remap(struct isp_remap_config *, u32, u32);
extern void set_enc_remap(struct enc_remap_config *, u32, u32);

int main(void)
{
    struct isp_remap_config i = {0};
    struct enc_remap_config e = {0};
    const u32 y = VIDEO_MEM_BASE, uv = VIDEO_MEM_BASE + 122880;
    i.SWrapEnb = i.RTTEnb = 1;
    /* Preserve the silicon ECO control when setting offsets for Y and UV. */
    isp[0xE8 / 4] = 1;
    assert(isp_remap_preprocess(&i, 1920, 1080, 0, 4, 0) == 1920);
    assert(i.SliceHeight == 16 && i.SliceNumWrap == 4 && i.Slice_num == 67);
    i.slice_wrap_component[0].dram_saddr = 0x80000000;
    i.slice_wrap_component[0].dram_eaddr = 0x801FA3FF;
    i.slice_wrap_component[1].dram_saddr = 0x80200000;
    i.slice_wrap_component[1].dram_eaddr = 0x802FD1FF;
    i.slice_wrap_component[0].out_saddr_offset = 0x80000000 - y;
    i.slice_wrap_component[1].out_saddr_offset = 0x80200000 - uv;
    assert(set_isp_remap(&i, 1920, 1080) == 0);
    assert(isp[0x0C / 4] == 0x10100); /* Slice+RTT; no partial SRAM/HDR. */
    assert(isp[0x34 / 4] - isp[0xE0 / 4] == y);
    assert(isp[0x40 / 4] - isp[0xE4 / 4] == uv);
    assert(isp[0xE8 / 4] == 1);
    assert(isp[0x10 / 4] == 0); /* Compression disabled. */
    e.SWrapEnb = e.RTTEnb = 1;
    e.SliceHeight = i.SliceHeight;
    e.SliceNumWrap = i.SliceNumWrap;
    e.SliceBurstID = i.SliceBurstID;
    e.Slice_Stride = i.Slice_Stride;
    e.Slice_num = i.Slice_num;
    e.enc_remap_component[0].dram_saddr = y;
    e.enc_remap_component[0].dram_eaddr = uv - 1;
    e.enc_remap_component[1].dram_saddr = uv;
    e.enc_remap_component[1].dram_eaddr = uv + 61440 - 1;
    set_enc_remap(&e, 1920, 1080);
    assert(enc[0x0C / 4] == 0x10100);
    assert(enc[0x30 / 4] == isp[0x30 / 4]);
    assert(enc[0x34 / 4] == y && enc[0x38 / 4] == uv - 1);
    assert(enc[0x40 / 4] == uv && enc[0x44 / 4] == 0x6F02CFFF);
    assert(enc[0x08 / 4] == 1); /* Hardware handshake enabled. */
    return 0;
}
