"""Frame preparation: scale, quantise to the N64's RGBA5551 and delta-encode into tiles."""
from __future__ import annotations

import numpy as np

from . import protocol as P

# 4x4 Bayer matrix, centred so the dither has zero mean.
_BAYER4 = (np.array([[0, 8, 2, 10], [12, 4, 14, 6], [3, 11, 1, 9], [15, 7, 13, 5]], dtype=np.float32)
           + 0.5) / 16.0 - 0.5


def quantize_rgba5551(rgb: np.ndarray) -> np.ndarray:
    """(H, W, 3) uint8 -> (H, W) uint16 RGBA5551 with ordered dithering; alpha bit always set."""
    h, w, _ = rgb.shape
    threshold = np.tile(_BAYER4, (h // 4 + 1, w // 4 + 1))[:h, :w, None] * 8.0  # one 5-bit step = 8
    q = np.clip(rgb.astype(np.float32) + threshold, 0, 255).astype(np.uint16) >> 3
    return (q[..., 0] << 11) | (q[..., 1] << 6) | (q[..., 2] << 1) | 1


def to_be_bytes(px: np.ndarray) -> bytes:
    return px.astype(">u2").tobytes()


def tiles_wide(width: int) -> int:
    return width // P.TILE


def tile_view(px: np.ndarray) -> np.ndarray:
    """(H, W) -> (n_tiles, TILE, TILE) in row-major tile order. H and W must be multiples of TILE."""
    h, w = px.shape
    t = P.TILE
    return px.reshape(h // t, t, w // t, t).swapaxes(1, 2).reshape(-1, t, t)


class DeltaEncoder:
    """Tracks what the N64 is currently displaying and sends only what differs.

    `shadow` is the receiver's state, not the last captured frame, so tiles skipped because of
    the byte budget are retried on later frames and the picture converges.
    """

    def __init__(self, width: int, height: int, budget_bytes: int):
        if width % P.TILE or height % P.TILE:
            raise ValueError("resolution must be a multiple of the tile size")
        self.width, self.height, self.budget = width, height, budget_bytes
        self.shadow = None
        self.seq = 0
        self.frame_id = 0

    def encode(self, px: np.ndarray) -> bytes:
        self.seq += 1
        self.frame_id += 1
        if self.shadow is None:
            self.shadow = px.copy()
            return P.frame_full(self.seq, self.frame_id, to_be_bytes(px))
        new, old = tile_view(px), tile_view(self.shadow)
        changed = (new != old).reshape(len(new), -1).sum(axis=1)
        order = np.argsort(-changed, kind="stable")
        max_tiles = max(1, (self.budget - P.HEADER_SIZE - 4) // P.tile_record_size())
        chosen = sorted(int(i) for i in order[:max_tiles] if changed[i] > 0)
        tiles = [(i, to_be_bytes(new[i])) for i in chosen]
        sh = tile_view(self.shadow)
        for i in chosen:
            sh[i] = new[i]
        self._write_back(sh)
        return P.frame_delta(self.seq, self.frame_id, tiles)

    def _write_back(self, tiles: np.ndarray) -> None:
        t, h, w = P.TILE, self.height, self.width
        self.shadow = tiles.reshape(h // t, w // t, t, t).swapaxes(1, 2).reshape(h, w)


class Receiver:
    """Reference implementation of the N64's frame reconstruction (used by the simulator and tests)."""

    def __init__(self):
        self.width = self.height = 0
        self.fb = None
        self.frame_id = 0

    def handle(self, raw: bytes) -> None:
        msg = P.unpack(raw)
        if msg.type == P.T_HELLO:
            self.width, self.height, *_ = P.parse_hello(msg.payload)
            self.fb = np.zeros((self.height, self.width), dtype=np.uint16)
        elif msg.type == P.T_FRAME_FULL:
            self.frame_id = int.from_bytes(msg.payload[:2], "big")
            self.fb = np.frombuffer(msg.payload[2:], dtype=">u2").astype(np.uint16).reshape(self.height, self.width)
        elif msg.type == P.T_FRAME_DELTA:
            self.frame_id = int.from_bytes(msg.payload[:2], "big")
            count = int.from_bytes(msg.payload[2:4], "big")
            off, t = 4, P.TILE
            view = tile_view(self.fb).copy()
            for _ in range(count):
                idx = int.from_bytes(msg.payload[off:off + 2], "big")
                view[idx] = np.frombuffer(msg.payload[off + 2:off + 2 + t * t * 2], dtype=">u2").reshape(t, t)
                off += P.tile_record_size()
            self.fb = view.reshape(self.height // t, self.width // t, t, t).swapaxes(1, 2).reshape(self.height, self.width)

    def to_rgb(self) -> np.ndarray:
        p = self.fb.astype(np.uint16)
        out = np.stack([(p >> 11) & 31, (p >> 6) & 31, (p >> 1) & 31], axis=-1).astype(np.uint16)
        return ((out << 3) | (out >> 2)).astype(np.uint8)
