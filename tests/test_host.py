import os
import sys
import unittest

import numpy as np

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "host"))
from unreal64 import frame, inputmap, protocol as P  # noqa: E402


def gradient(w=64, h=48):
    yy, xx = np.mgrid[0:h, 0:w]
    return np.stack([xx * 255 // w, yy * 255 // h, (xx + yy) % 256], -1).astype(np.uint8)


class ProtocolTests(unittest.TestCase):
    def test_roundtrip(self):
        m = P.unpack(P.hello(7, 320, 240, 30))
        self.assertEqual((m.type, m.seq), (P.T_HELLO, 7))
        self.assertEqual(P.parse_hello(m.payload), (320, 240, P.FMT_RGBA5551, P.TILE, 30))

    def test_rejects_garbage(self):
        with self.assertRaises(P.ProtocolError):
            P.unpack(b"XXXX" + bytes(8))
        with self.assertRaises(P.ProtocolError):
            P.unpack(P.hello(1, 1, 1, 1)[:-1])

    def test_input(self):
        st = inputmap.decode(P.unpack(P.input_msg(1, 0b101, -80, 40, 9)).payload)
        self.assertTrue(st.buttons["a"] and st.buttons["z"] and not st.buttons["b"])
        self.assertEqual((st.stick_x, st.stick_y), (-1.0, 0.5))


class FrameTests(unittest.TestCase):
    def test_quantize_range_and_alpha(self):
        q = frame.quantize_rgba5551(np.full((16, 16, 3), 255, np.uint8))
        self.assertTrue((q == 0xFFFF).all())
        self.assertTrue((frame.quantize_rgba5551(np.zeros((16, 16, 3), np.uint8)) == 1).all())

    def test_dither_preserves_mean_better_than_truncation(self):
        flat = np.full((64, 64, 3), 100, np.uint8)  # 100 is not a multiple of 8
        out = frame.Receiver()
        out.width = out.height = 64
        out.fb = frame.quantize_rgba5551(flat)
        err_dither = abs(out.to_rgb().mean() - 100)
        err_trunc = abs(((100 >> 3) << 3 | (100 >> 6)) - 100)
        self.assertLess(err_dither, err_trunc)

    def _run(self, budget, frames):
        w, h = 64, 48
        enc, rx = frame.DeltaEncoder(w, h, budget), frame.Receiver()
        rx.handle(P.hello(0, w, h, 30))
        for px in frames:
            rx.handle(enc.encode(px))
        return rx

    def test_lossless_with_big_budget(self):
        a = frame.quantize_rgba5551(gradient())
        b = a.copy()
        b[10:30, 5:40] ^= 0x0842
        rx = self._run(10**6, [a, b])
        self.assertTrue((rx.fb == b).all())

    def test_converges_under_tight_budget(self):
        a = frame.quantize_rgba5551(gradient())
        b = (a ^ 0x0842).astype(np.uint16)
        tight = P.HEADER_SIZE + 4 + 3 * P.tile_record_size()  # 3 tiles per frame, 12 tiles total
        rx = self._run(tight, [a, b, b, b, b, b])
        self.assertTrue((rx.fb == b).all())

    def test_static_scene_costs_almost_nothing(self):
        a = frame.quantize_rgba5551(gradient())
        enc = frame.DeltaEncoder(64, 48, 10**6)
        enc.encode(a)
        self.assertLessEqual(len(enc.encode(a)), P.HEADER_SIZE + 4)


if __name__ == "__main__":
    unittest.main()
