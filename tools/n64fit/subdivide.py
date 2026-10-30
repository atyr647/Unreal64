"""Re-tessellate big triangles so per-vertex baked light can show shadows and falloff across flat surfaces.

Decimation collapses flat floors/walls to a few huge triangles; vertex lighting on those is a single flat colour.
Long edges are split at their midpoints and every triangle re-cut conformingly (no T-junctions).
"""
from __future__ import annotations

import numpy as np


def _split_once(p, max_edge):
    pos, idx = p.positions, p.indices
    edges = np.sort(np.concatenate([idx[:, [0, 1]], idx[:, [1, 2]], idx[:, [2, 0]]]), axis=1)
    uniq = np.unique(edges, axis=0)
    length = np.linalg.norm(pos[uniq[:, 0]] - pos[uniq[:, 1]], axis=1)
    longE = uniq[length > max_edge]
    if len(longE) == 0:
        return False
    base = len(pos)
    mid_of = {(int(a), int(b)): base + i for i, (a, b) in enumerate(longE)}
    a_, b_ = longE[:, 0], longE[:, 1]

    def lerp(arr):
        return (arr[a_] + arr[b_]) * 0.5

    new_n = lerp(p.normals)
    new_n /= np.maximum(np.linalg.norm(new_n, axis=1, keepdims=True), 1e-12)
    p.positions = np.concatenate([pos, lerp(pos)])
    p.normals = np.concatenate([p.normals, new_n])
    if p.uvs is not None:
        p.uvs = np.concatenate([p.uvs, lerp(p.uvs)])
    if p.colors is not None:
        p.colors = np.concatenate([p.colors, lerp(p.colors)])
    out = []
    for a, b, c in idx.tolist():
        v = (a, b, c)
        m = [mid_of.get((min(v[i], v[(i + 1) % 3]), max(v[i], v[(i + 1) % 3])), -1) for i in range(3)]
        n = sum(x >= 0 for x in m)
        if n == 0:
            out.append((a, b, c))
        elif n == 3:
            out += [(a, m[0], m[2]), (m[0], b, m[1]), (m[2], m[1], c), (m[0], m[1], m[2])]
        elif n == 1:
            i = next(k for k in range(3) if m[k] >= 0)
            vi, vj, vk = v[i], v[(i + 1) % 3], v[(i + 2) % 3]
            out += [(vi, m[i], vk), (m[i], vj, vk)]
        else:
            i = next(k for k in range(3) if m[k] >= 0 and m[(k - 1) % 3] >= 0)  # common vertex v[i]
            pv, q, r = v[i], v[(i + 1) % 3], v[(i + 2) % 3]
            mq, mr = m[i], m[(i - 1) % 3]
            out += [(pv, mq, mr), (mq, q, r), (mq, r, mr)]
    p.indices = np.array(out, np.int32)
    return True


def subdivide_prim(p, max_edge, max_passes=8):
    for _ in range(max_passes):
        if not _split_once(p, max_edge):
            break
    return p


def tris_after(prims, max_edge):
    """Triangle count if every prim were subdivided to `max_edge` (works on copies)."""
    import copy
    total = 0
    for p in prims:
        q = copy.copy(p)
        q.positions, q.normals, q.uvs, q.colors, q.indices = p.positions, p.normals, p.uvs, p.colors, p.indices
        subdivide_prim(q, max_edge)
        total += len(q.indices)
    return total


def subdivide_to_budget(prims, budget, start_edge=1.0, growth=1.25, max_iter=30):
    """Smallest max-edge (>= start_edge, growing) whose result fits `budget`; applies it. Returns the edge used."""
    edge = start_edge
    for _ in range(max_iter):
        if tris_after(prims, edge) <= budget:
            break
        edge *= growth
    for p in prims:
        subdivide_prim(p, edge)
    return edge
