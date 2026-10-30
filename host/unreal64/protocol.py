"""Unreal64 wire protocol. All integers are big-endian (the N64 is big-endian).

Every message is a 12-byte header followed by `length` payload bytes. Each message maps
to exactly one carrier-level USB message, so the N64 never has to re-frame a byte stream.
See docs/PROTOCOL.md.
"""
from __future__ import annotations

import struct
from dataclasses import dataclass

MAGIC = b"U64P"
HEADER = struct.Struct(">4sBBHI")  # magic, type, flags, seq, length
HEADER_SIZE = HEADER.size  # 12

T_HELLO = 0x01
T_FRAME_FULL = 0x02
T_FRAME_DELTA = 0x03
T_INPUT = 0x10
T_STATS = 0x11

TILE = 16
FMT_RGBA5551 = 0

_HELLO = struct.Struct(">HHBBB")  # width, height, fmt, tile, target_fps
_FRAME_FULL = struct.Struct(">H")  # frame_id, then width*height*2 bytes
_FRAME_DELTA = struct.Struct(">HH")  # frame_id, tile_count; then (u16 index + tile bytes)*
_INPUT = struct.Struct(">HbbH")  # buttons, stick_x, stick_y, last_frame_id

# Button bit order matches libdragon's joypad_buttons_t (LSB first).
BUTTONS = ("a", "b", "z", "start", "d_up", "d_down", "d_left", "d_right",
           "y", "x", "l", "r", "c_up", "c_down", "c_left", "c_right")


class ProtocolError(ValueError):
    pass


@dataclass
class Message:
    type: int
    seq: int
    payload: bytes
    flags: int = 0


def pack(msg_type: int, seq: int, payload: bytes = b"", flags: int = 0) -> bytes:
    return HEADER.pack(MAGIC, msg_type, flags, seq & 0xFFFF, len(payload)) + payload


def unpack(data: bytes) -> Message:
    if len(data) < HEADER_SIZE:
        raise ProtocolError("short header")
    magic, mtype, flags, seq, length = HEADER.unpack_from(data)
    if magic != MAGIC:
        raise ProtocolError("bad magic")
    if len(data) != HEADER_SIZE + length:
        raise ProtocolError(f"length mismatch: header says {length}, have {len(data) - HEADER_SIZE}")
    return Message(mtype, seq, bytes(data[HEADER_SIZE:]), flags)


def hello(seq: int, width: int, height: int, fps: int) -> bytes:
    return pack(T_HELLO, seq, _HELLO.pack(width, height, FMT_RGBA5551, TILE, fps))


def parse_hello(payload: bytes) -> tuple[int, int, int, int, int]:
    return _HELLO.unpack(payload)


def frame_full(seq: int, frame_id: int, pixels_be: bytes) -> bytes:
    return pack(T_FRAME_FULL, seq, _FRAME_FULL.pack(frame_id & 0xFFFF) + pixels_be)


def frame_delta(seq: int, frame_id: int, tiles: list[tuple[int, bytes]]) -> bytes:
    body = bytearray(_FRAME_DELTA.pack(frame_id & 0xFFFF, len(tiles)))
    for index, data in tiles:
        if len(data) != TILE * TILE * 2:
            raise ProtocolError("bad tile size")
        body += struct.pack(">H", index) + data
    return pack(T_FRAME_DELTA, seq, bytes(body))


def input_msg(seq: int, buttons: int, stick_x: int, stick_y: int, last_frame: int) -> bytes:
    return pack(T_INPUT, seq, _INPUT.pack(buttons, stick_x, stick_y, last_frame & 0xFFFF))


def parse_input(payload: bytes) -> tuple[int, int, int, int]:
    return _INPUT.unpack(payload)


def tile_record_size() -> int:
    return 2 + TILE * TILE * 2
