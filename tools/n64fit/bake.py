"""Offline light bake into vertex colours: the N64 stand-in for Lumen / ray-traced shadows / path-traced GI.

Per vertex: direct light from every KHR punctual light with ray-traced shadows, plus a cosine-weighted gather that
gives sky occlusion (misses return the ambient colour) and one bounce of diffuse GI (hits return the hit
triangle's direct light times its albedo). Output is LINEAR; Tiny3D's importer gamma-encodes vertex colours.

Ray casting is brute-force vectorised Moller-Trumbore: fine for N64-budget scenes (a few thousand triangles),
not for the full source mesh. For production-quality bakes this is the module to swap for Embree/Cycles.
"""
from __future__ import annotations

import numpy as np


class RayContext:
    def __init__(self, positions, tris):
        a, b, c = (positions[tris[:, i]].astype(np.float32) for i in range(3))
        self.v0, self.e1, self.e2 = a, b - a, c - a
        self.n = len(tris)

    def _chunks(self, count):
        step = max(1, 300_000 // max(self.n, 1))
        for s in range(0, count, step):
            yield slice(s, min(count, s + step))

    def nearest(self, o, d, tmin=1e-4):
        """Returns (tri index or -1, t) for each ray."""
        out_i = np.full(len(o), -1, np.int64)
        out_t = np.full(len(o), np.inf, np.float32)
        v0, e1, e2 = self.v0, self.e1, self.e2
        for sl in self._chunks(len(o)):
            ox, oy, oz = (o[sl, k, None] for k in range(3))
            dx, dy, dz = (d[sl, k, None] for k in range(3))
            hx = dy * e2[None, :, 2] - dz * e2[None, :, 1]
            hy = dz * e2[None, :, 0] - dx * e2[None, :, 2]
            hz = dx * e2[None, :, 1] - dy * e2[None, :, 0]
            a = e1[None, :, 0] * hx + e1[None, :, 1] * hy + e1[None, :, 2] * hz
            ok = np.abs(a) > 1e-10
            f = np.where(ok, 1.0 / np.where(ok, a, 1.0), 0.0)
            sx, sy, sz = ox - v0[None, :, 0], oy - v0[None, :, 1], oz - v0[None, :, 2]
            u = f * (sx * hx + sy * hy + sz * hz)
            qx = sy * e1[None, :, 2] - sz * e1[None, :, 1]
            qy = sz * e1[None, :, 0] - sx * e1[None, :, 2]
            qz = sx * e1[None, :, 1] - sy * e1[None, :, 0]
            v = f * (dx * qx + dy * qy + dz * qz)
            t = f * (e2[None, :, 0] * qx + e2[None, :, 1] * qy + e2[None, :, 2] * qz)
            hit = ok & (u >= 0) & (v >= 0) & (u + v <= 1) & (t > tmin)
            t = np.where(hit, t, np.inf)
            j = t.argmin(axis=1)
            tj = t[np.arange(len(j)), j]
            out_i[sl] = np.where(np.isfinite(tj), j, -1)
            out_t[sl] = tj
        return out_i, out_t


def _cosine_dirs(normals, samples, rng):
    n = len(normals)
    u1, u2 = rng.random((n, samples)), rng.random((n, samples))
    r, phi = np.sqrt(u1), 2 * np.pi * u2
    x, y, z = r * np.cos(phi), r * np.sin(phi), np.sqrt(np.maximum(0, 1 - u1))
    up = np.where(np.abs(normals[:, 1:2]) < 0.99, np.array([[0, 1, 0]]), np.array([[1, 0, 0]]))
    t = np.cross(up, normals)
    t /= np.linalg.norm(t, axis=1, keepdims=True)
    b = np.cross(normals, t)
    return (x[..., None] * t[:, None] + y[..., None] * b[:, None] + z[..., None] * normals[:, None]).astype(np.float32)


def _direct(ctx, pos, nrm, lights, norms):
    """Direct light at points (N,3) with normals; shadows traced. Returns (N,3)."""
    total = np.zeros((len(pos), 3), np.float32)
    eps = 1e-3
    for L, scale in zip(lights, norms):
        col = np.array(L.color, np.float32) * L.intensity * scale
        if L.kind == "directional":
            to_l = np.tile(-L.direction.astype(np.float32), (len(pos), 1))
            att = np.ones(len(pos), np.float32)
            dist = np.full(len(pos), np.inf, np.float32)
        else:
            vec = L.position.astype(np.float32)[None] - pos
            dist = np.linalg.norm(vec, axis=1)
            to_l = vec / np.maximum(dist[:, None], 1e-6)
            att = 1.0 / np.maximum(dist ** 2, 1e-2)
            if L.range > 0:
                att *= np.clip(1 - (dist / L.range) ** 4, 0, 1) ** 2
            if L.kind == "spot":
                cos_ang = (-to_l) @ L.direction.astype(np.float32)
                lo, hi = np.cos(L.outer), np.cos(L.inner)
                s = np.clip((cos_ang - lo) / max(hi - lo, 1e-4), 0, 1)
                att *= s * s * (3 - 2 * s)
        ndl = np.maximum((nrm * to_l).sum(1), 0)
        live = (ndl > 0) & (att > 0)
        vis = np.zeros(len(pos), np.float32)
        if live.any():
            idx, t = ctx.nearest(pos[live] + nrm[live] * eps, to_l[live])
            vis[live] = ~(idx >= 0) | (t >= dist[live] - 1e-3)
        total += (att * ndl * vis)[:, None] * col[None]
    return total


def light_norms(lights, ref_dist=5.0):
    """Unreal exports physical units; normalise so the strongest light of each kind contributes 1.0
    (directional: at any point; point/spot: at ref_dist)."""
    dir_max = max([L.intensity for L in lights if L.kind == "directional"], default=1.0)
    pt_max = max([L.intensity for L in lights if L.kind != "directional"], default=1.0)
    return [1.0 / dir_max if L.kind == "directional" else ref_dist ** 2 / pt_max for L in lights]


def bake(scene, albedo_per_prim, ambient=(0.25, 0.28, 0.35), samples=24, bounces=1, seed=7, exposure="auto"):
    """Fills prim.colors with linear baked light. albedo_per_prim[i] is the mean linear albedo of prim i."""
    rng = np.random.default_rng(seed)
    pos = np.concatenate([p.positions for p in scene.prims])
    nrm = np.concatenate([p.normals for p in scene.prims])
    offs = np.cumsum([0] + [len(p.positions) for p in scene.prims])
    tris = np.concatenate([p.indices + offs[i] for i, p in enumerate(scene.prims)])
    tri_albedo = np.concatenate([np.tile(albedo_per_prim[i], (len(p.indices), 1)) for i, p in enumerate(scene.prims)])
    ctx = RayContext(pos, tris)
    norms = light_norms(scene.lights)
    amb = np.array(ambient, np.float32)

    cen = pos[tris].mean(1)
    fn = np.cross(pos[tris[:, 1]] - pos[tris[:, 0]], pos[tris[:, 2]] - pos[tris[:, 0]])
    fn /= np.maximum(np.linalg.norm(fn, axis=1, keepdims=True), 1e-12)
    # Face normals can point either way; light the side that faces each light.
    tri_rad = np.maximum(_direct(ctx, cen, fn, scene.lights, norms), _direct(ctx, cen, -fn, scene.lights, norms))

    light = _direct(ctx, pos, nrm, scene.lights, norms)
    if samples > 0:
        dirs = _cosine_dirs(nrm, samples, rng)
        o = np.repeat(pos + nrm * 1e-3, samples, axis=0)
        idx, _ = ctx.nearest(o, dirs.reshape(-1, 3))
        idx = idx.reshape(len(pos), samples)
        gather = np.tile(amb, (len(pos), samples, 1))
        hit = idx >= 0
        if bounces > 0:
            hv = np.where(hit[..., None], (tri_rad * tri_albedo)[np.maximum(idx, 0)], 0.0)
        else:
            hv = 0.0
        gather = np.where(hit[..., None], hv, gather)
        light = light + gather.mean(1)
    else:
        light = light + amb

    lum = light @ np.array([0.2126, 0.7152, 0.0722], np.float32)
    k = (0.85 / max(float(np.percentile(lum, 95)), 1e-6)) if exposure == "auto" else float(exposure)
    c = light * k
    c = c * (1 + c / 4.0) / (1 + c)  # extended Reinhard (white point 2.0)
    c = np.clip(c, 0, 1).astype(np.float32)
    for i, p in enumerate(scene.prims):
        p.colors = np.c_[c[offs[i]:offs[i + 1]], np.ones(len(p.positions), np.float32)]
    return {"exposure": k, "mean_luminance": float(lum.mean() * k)}
