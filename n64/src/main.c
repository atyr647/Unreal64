// Unreal64 N64 client: receives frames from the host over the flashcart's USB link, shows them,
// and sends controller state back. Protocol: docs/PROTOCOL.md (mirrors host/unreal64/protocol.py).
//
// STATUS: written against libdragon's headers but NOT yet compiled or run on hardware.
#include <libdragon.h>
#include <usb.h>
#include <stdint.h>
#include <string.h>

#define MAGIC 0x55363450u  // "U64P"
#define HEADER_SIZE 12
#define T_HELLO 0x01
#define T_FRAME_FULL 0x02
#define T_FRAME_DELTA 0x03
#define T_INPUT 0x10
#define TILE 16

#define MAX_W 320
#define MAX_H 240
#define MAX_MSG (HEADER_SIZE + 4 + MAX_W * MAX_H * 2)

static uint8_t rx[MAX_MSG] __attribute__((aligned(16)));
static surface_t shadow;  // receiver state; deltas accumulate here, then it is copied to the display
static int width = MAX_W, height = MAX_H;
static uint16_t last_frame_id;
static bool have_frame;

static inline uint16_t be16(const uint8_t *p) { return (uint16_t)(p[0] << 8 | p[1]); }
static inline uint32_t be32(const uint8_t *p) { return (uint32_t)p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3]; }

static void apply(const uint8_t *m, uint32_t n)
{
    if (n < HEADER_SIZE || be32(m) != MAGIC) return;
    uint8_t type = m[4];
    uint32_t len = be32(m + 8);
    const uint8_t *p = m + HEADER_SIZE;
    if (HEADER_SIZE + len != n) return;

    if (type == T_HELLO && len >= 7) {
        int w = be16(p), h = be16(p + 2);
        if (w > 0 && w <= MAX_W && h > 0 && h <= MAX_H && w % TILE == 0 && h % TILE == 0) {
            width = w; height = h;
            memset(shadow.buffer, 0, shadow.stride * shadow.height);
        }
    } else if (type == T_FRAME_FULL && len == 2 + (uint32_t)width * height * 2) {
        last_frame_id = be16(p);
        for (int y = 0; y < height; y++)
            memcpy((uint8_t *)shadow.buffer + y * shadow.stride, p + 2 + y * width * 2, width * 2);
        have_frame = true;
    } else if (type == T_FRAME_DELTA && len >= 4) {
        last_frame_id = be16(p);
        uint32_t count = be16(p + 2), off = 4;
        uint32_t tiles_wide = width / TILE, rec = 2 + TILE * TILE * 2;
        for (uint32_t i = 0; i < count && off + rec <= len; i++, off += rec) {
            uint32_t idx = be16(p + off);
            uint32_t tx = (idx % tiles_wide) * TILE, ty = (idx / tiles_wide) * TILE;
            if (ty + TILE > (uint32_t)height) continue;  // reject out-of-range tile index
            for (int row = 0; row < TILE; row++)
                memcpy((uint8_t *)shadow.buffer + (ty + row) * shadow.stride + tx * 2,
                       p + off + 2 + row * TILE * 2, TILE * 2);
        }
        have_frame = true;
    }
}

// Builds the button word from fields: bit order is the one in host/unreal64/protocol.py BUTTONS.
// (Do not use .raw: bitfield order on big-endian MIPS is MSB-first.)
static uint16_t pack_buttons(joypad_buttons_t b)
{
    uint16_t v = 0;
    bool f[16] = { b.a, b.b, b.z, b.start, b.d_up, b.d_down, b.d_left, b.d_right,
                   b.y, b.x, b.l, b.r, b.c_up, b.c_down, b.c_left, b.c_right };
    for (int i = 0; i < 16; i++) v |= (uint16_t)f[i] << i;
    return v;
}

static void send_input(uint16_t seq)
{
    joypad_inputs_t in = joypad_get_inputs(JOYPAD_PORT_1);
    uint8_t pkt[HEADER_SIZE + 6];
    uint16_t btn = pack_buttons(in.btn);
    pkt[0] = 'U'; pkt[1] = '6'; pkt[2] = '4'; pkt[3] = 'P';
    pkt[4] = T_INPUT; pkt[5] = 0; pkt[6] = seq >> 8; pkt[7] = seq;
    pkt[8] = 0; pkt[9] = 0; pkt[10] = 0; pkt[11] = 6;
    pkt[12] = btn >> 8; pkt[13] = btn; pkt[14] = (uint8_t)in.stick_x; pkt[15] = (uint8_t)in.stick_y;
    pkt[16] = last_frame_id >> 8; pkt[17] = last_frame_id;
    usb_write(DATATYPE_RAWBINARY, pkt, sizeof pkt);
}

int main(void)
{
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_RESAMPLE);
    joypad_init();
    shadow = surface_alloc(FMT_RGBA16, MAX_W, MAX_H);
    memset(shadow.buffer, 0, shadow.stride * shadow.height);
    bool usb_ok = usb_initialize();
    uint16_t out_seq = 0;

    while (1) {
        // Drain every pending host message, applying each; only the newest picture is shown.
        uint32_t hdr;
        while (usb_ok && (hdr = usb_poll()) != 0) {
            uint32_t size = USBHEADER_GETSIZE(hdr);
            if (USBHEADER_GETTYPE(hdr) == DATATYPE_RAWBINARY && size >= HEADER_SIZE && size <= MAX_MSG) {
                usb_read(rx, size);
                apply(rx, size);
            } else {
                usb_skip(size);
            }
        }

        surface_t *disp = display_get();
        if (have_frame) {
            for (int y = 0; y < height; y++)
                memcpy((uint8_t *)disp->buffer + y * disp->stride,
                       (uint8_t *)shadow.buffer + y * shadow.stride, width * 2);
            data_cache_hit_writeback_invalidate(disp->buffer, disp->stride * disp->height);
        } else {
            memset(disp->buffer, 0x08, disp->stride * disp->height);  // "waiting for host" placeholder
            data_cache_hit_writeback_invalidate(disp->buffer, disp->stride * disp->height);
        }
        display_show(disp);

        joypad_poll();
        if (usb_ok) send_input(out_seq++);
    }
}
