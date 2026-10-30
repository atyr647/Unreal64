"""python -m unreal64 selftest|stream  (run from the host/ directory)"""
from __future__ import annotations

import argparse
import sys
import time

import numpy as np

from . import capture, frame, protocol as P


def synthetic_frames(w: int, h: int, n: int):
    """Moving gradient + sliding box: a rough stand-in for a game scene with a static HUD."""
    yy, xx = np.mgrid[0:h, 0:w]
    for i in range(n):
        img = np.zeros((h, w, 3), dtype=np.uint8)
        img[..., 0] = (xx * 255 // w + i * 3) % 256
        img[..., 1] = (yy * 255 // h)
        img[..., 2] = 96
        x0 = (i * 5) % (w - 48)
        img[h // 2:h // 2 + 48, x0:x0 + 48] = 255
        img[:16, :64] = (20, 20, 20)
        yield img


def run(frames_iter, w: int, h: int, fps: int, link_mbps: float, out_png: str | None) -> int:
    budget = int(link_mbps * 1e6 / fps)
    enc = frame.DeltaEncoder(w, h, budget)
    rx = frame.Receiver()
    rx.handle(P.hello(0, w, h, fps))
    sizes, last = [], None
    t0 = time.perf_counter()
    for rgb in frames_iter:
        px = frame.quantize_rgba5551(rgb)
        pkt = enc.encode(px)
        rx.handle(pkt)
        sizes.append(len(pkt))
        last = px
    dt = time.perf_counter() - t0
    wrong = int((rx.fb != last).sum())
    print(f"{len(sizes)} frames, {w}x{h}, link budget {budget} B/frame ({link_mbps} MB/s @ {fps} fps)")
    print(f"first(full)={sizes[0]} B, mean delta={int(np.mean(sizes[1:])) if len(sizes) > 1 else 0} B, "
          f"max={max(sizes)} B, host encode {len(sizes) / dt:.0f} fps")
    print(f"final picture differs from target in {wrong}/{w * h} pixels")
    if out_png:
        from PIL import Image
        Image.fromarray(rx.to_rgb()).save(out_png)
        print("wrote", out_png)
    return 0


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(prog="unreal64")
    sub = ap.add_subparsers(dest="cmd", required=True)
    for name in ("selftest", "stream"):
        p = sub.add_parser(name)
        p.add_argument("--width", type=int, default=320)
        p.add_argument("--height", type=int, default=240)
        p.add_argument("--fps", type=int, default=30)
        p.add_argument("--link-mbps", type=float, default=2.0, help="assumed usable link throughput, MB/s (UNMEASURED)")
        p.add_argument("--png", default=None)
        p.add_argument("--frames", type=int, default=120)
    sub.choices["stream"].add_argument("--window", default=None, help="window title to capture (Windows)")
    sub.choices["stream"].add_argument("--lavfi", default=None, help="ffmpeg lavfi source for testing")
    a = ap.parse_args(argv)
    if a.cmd == "selftest":
        return run(synthetic_frames(a.width, a.height, a.frames), a.width, a.height, a.fps, a.link_mbps, a.png)
    args = ["-f", "lavfi", "-i", a.lavfi] if a.lavfi else capture.ffmpeg_input_args(a.window)
    src = capture.frames(args, a.width, a.height, a.fps)
    it = (f for _, f in zip(range(a.frames), src))
    return run(it, a.width, a.height, a.fps, a.link_mbps, a.png)


if __name__ == "__main__":
    sys.exit(main())
