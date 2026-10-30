"""Triangle-budget decimation (Nanite -> N64): meshoptimizer quadric simplification, attributes preserved because
simplification only drops indices and the original vertices are reused."""
from __future__ import annotations

import numpy as np

import meshoptimizer


def simplify_prim(prim, target_tris: int, max_error: float = 0.2):
    tris = prim.indices.shape[0]
    if tris <= target_tris:
        return prim
    idx = np.ascontiguousarray(prim.indices.reshape(-1), dtype=np.uint32)
    pos = np.ascontiguousarray(prim.positions, dtype=np.float32)
    dest = np.zeros_like(idx)
    n = meshoptimizer.simplify(dest, idx, pos, target_index_count=int(target_tris) * 3, target_error=max_error)
    new = dest[:n].reshape(-1, 3).astype(np.int32)
    used, inv = np.unique(new, return_inverse=True)
    prim.indices = inv.reshape(-1, 3).astype(np.int32)
    prim.positions = prim.positions[used]
    prim.normals = prim.normals[used]
    prim.uvs = None if prim.uvs is None else prim.uvs[used]
    prim.colors = None if prim.colors is None else prim.colors[used]
    return prim


def budget_scene(scene, total_tris: int, min_per_prim: int = 12):
    """Spread the budget over prims proportionally to their triangle count (tiny props keep a floor)."""
    total = sum(p.indices.shape[0] for p in scene.prims)
    ratio = min(1.0, total_tris / max(total, 1))
    for p in scene.prims:
        simplify_prim(p, max(min_per_prim, int(p.indices.shape[0] * ratio)))
    return total, sum(p.indices.shape[0] for p in scene.prims)
