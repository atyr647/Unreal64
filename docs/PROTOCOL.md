# Wire protocol

Big-endian throughout. Each message is carried as exactly **one** carrier-level USB message
(libdragon `usb_write`/`usb_read` with `DATATYPE_RAWBINARY`), so the N64 never re-frames a stream.
The host-side carrier framing for the flashcart (UNFLoader-style) is **not implemented yet** and must be
confirmed on hardware; the payload protocol below is carrier-independent.

Header (12 bytes): `"U64P"` | type u8 | flags u8 | seq u16 | length u32 (payload bytes)

| Type | Dir | Payload |
|---|---|---|
| 0x01 HELLO | host→N64 | width u16, height u16, fmt u8 (0 = RGBA5551), tile u8 (16), fps u8 |
| 0x02 FRAME_FULL | host→N64 | frame_id u16, then width*height*2 bytes RGBA5551 |
| 0x03 FRAME_DELTA | host→N64 | frame_id u16, tile_count u16, then tile_count × (index u16, 16×16×2 bytes) |
| 0x10 INPUT | N64→host | buttons u16, stick_x i8, stick_y i8, last_frame_id u16 |
| 0x11 STATS | N64→host | reserved |

Tile index is row-major over a `(width/16) × (height/16)` grid. Width and height must be multiples of 16.
Button bits (LSB first): a b z start d_up d_down d_left d_right y x l r c_up c_down c_left c_right.
`last_frame_id` lets the host measure round-trip latency and throttle.

Reference implementations: `host/unreal64/protocol.py` (+ `frame.py` `Receiver`), `n64/src/main.c`.
