"""Fit textures to the N64's 4 KB TMEM. Picks the format/size with the least error against a 64x64 reference.

TMEM facts used (verify on hardware): 4096 bytes total. RGBA16 is 2 B/texel -> 2048 texels. Palette formats keep their
palette in the upper half of TMEM, leaving 2048 B for texels: CI8 -> 2048 texels, CI4 -> 4096 texels.
"""
from __future__ import annotations

import hashlib

import numpy as np
from PIL import Image

MAX_TEXELS = {"RGBA16": 2048, "CI8": 2048, "CI4": 4096}
POW2 = (8, 16, 32, 64, 128)
_REF = 64


def _dims(aspect: float, max_texels: int):
    best, best_key = None, None
    for w in POW2:
        for h in POW2:
            if w * h > max_texels:
                continue
            key = (abs(np.log2(w / h) - np.log2(aspect)), -w * h)
            if best_key is None or key < best_key:
                best, best_key = (w, h), key
    return best


def _q5(a):
    q = (a.astype(np.int32) * 31 + 127) // 255
    return ((q * 255 + 15) // 31).astype(np.uint8)


def _rgba16(img: Image.Image, cutout: bool):
    a = np.asarray(img, dtype=np.uint8).copy()
    a[..., :3] = _q5(a[..., :3])
    a[..., 3] = np.where(a[..., 3] >= 128, 255, 0) if cutout else 255
    return Image.fromarray(a, "RGBA")


def _ci(img: Image.Image, colors: int, cutout: bool):
    rgba = np.asarray(img, dtype=np.uint8)
    transparent = (rgba[..., 3] < 128) if cutout else np.zeros(rgba.shape[:2], bool)
    n = colors - 1 if transparent.any() else colors
    q = img.convert("RGB").quantize(colors=n, method=Image.Quantize.MEDIANCUT, dither=Image.Dither.FLOYDSTEINBERG)
    pal = np.array(q.getpalette()[:n * 3], dtype=np.uint8).reshape(-1, 3)
    pal = _q5(pal)
    idx = np.asarray(q, dtype=np.uint8)
    if transparent.any():
        idx = np.where(transparent, 0, idx + 1).astype(np.uint8)
        pal = np.vstack([np.zeros((1, 3), np.uint8), pal])
    out = Image.fromarray(idx, "P")
    flat = np.zeros(256 * 3, np.uint8)
    flat[:pal.size] = pal.reshape(-1)
    out.putpalette(flat.tolist())
    if transparent.any():
        out.info["transparency"] = bytes([0]) + bytes([255]) * (len(pal) - 1)
    recon = Image.fromarray(np.where(transparent[..., None], 0, pal[np.minimum(idx, len(pal) - 1)]).astype(np.uint8), "RGB")
    return out, recon.convert("RGBA")


def _err(recon: Image.Image, ref: np.ndarray):
    r = np.asarray(recon.convert("RGB").resize((_REF, _REF), Image.BILINEAR), dtype=np.float32)
    return float(((r - ref) ** 2).mean())


def fit_texture(img: Image.Image, cutout: bool):
    """Returns dict(format, image, width, height, bytes, error, mask_s, mask_t)."""
    img = img.convert("RGBA")
    ref = np.asarray(img.convert("RGB").resize((_REF, _REF), Image.LANCZOS), dtype=np.float32)
    aspect = img.width / img.height
    best = None
    for fmt, maxt in MAX_TEXELS.items():
        w, h = _dims(aspect, maxt)
        small = img.resize((w, h), Image.LANCZOS)
        if fmt == "RGBA16":
            out = recon = _rgba16(small, cutout)
        else:
            out, recon = _ci(small, 256 if fmt == "CI8" else 16, cutout)
        e = _err(recon, ref)
        bytes_ = w * h * (2 if fmt == "RGBA16" else 1 if fmt == "CI8" else 0.5)
        cand = dict(format=fmt, image=out, width=w, height=h, bytes=int(bytes_), error=e,
                    mask_s=int(np.log2(w)), mask_t=int(np.log2(h)))
        # Prefer the lower error; near-ties go to the smaller texture (more TMEM left, less ROM).
        if best is None or e < best["error"] * 0.97 or (e <= best["error"] * 1.03 and cand["bytes"] < best["bytes"]):
            best = cand
    return best


def texture_key(img: Image.Image, cutout: bool) -> str:
    return hashlib.sha1(img.tobytes() + bytes([cutout])).hexdigest()[:10]
