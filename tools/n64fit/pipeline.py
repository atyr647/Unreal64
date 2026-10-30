"""Whole conversion: glTF (from Unreal) -> assets/*.glb + *.png + formats.mk + level_index.h for the Tiny3D runtime."""
from __future__ import annotations

import json
import os

import numpy as np
from PIL import Image

from . import bake as bake_mod, decimate, gltfio, materials, subdivide, textures
from .gltfio import Prim

INT16_LIMIT = 30000  # headroom under 32767 for the importer's fixed-point positions


def _srgb_mean_linear(img):
    a = np.asarray(img.convert("RGB"), dtype=np.float32).reshape(-1, 3) / 255.0
    return gltfio._srgb_to_linear(a).mean(0)


def split_cells(prims, size):
    """Group triangles into (cell, material) primitives. size<=0 -> one cell."""
    groups: dict = {}
    for p in prims:
        cen = p.positions[p.indices].mean(1)
        cell = np.zeros((len(cen), 2), np.int64) if size <= 0 else np.floor(cen[:, [0, 2]] / size).astype(np.int64)
        for key in np.unique(cell, axis=0):
            sel = np.where((cell == key).all(1))[0]
            groups.setdefault((int(key[0]), int(key[1])), []).append((p, p.indices[sel]))
    cells = {}
    for key, parts in groups.items():
        by_mat: dict = {}
        for p, tris in parts:
            used, inv = np.unique(tris, return_inverse=True)
            sub = Prim(p.name, p.positions[used], p.normals[used],
                       None if p.uvs is None else p.uvs[used], p.colors[used], inv.reshape(-1, 3).astype(np.int32), p.material)
            by_mat.setdefault(p.material, []).append(sub)
        merged = []
        for m, subs in by_mat.items():
            off = np.cumsum([0] + [len(s.positions) for s in subs])
            merged.append(Prim(f"cell_m{m}", np.concatenate([s.positions for s in subs]), np.concatenate([s.normals for s in subs]),
                               None if subs[0].uvs is None else np.concatenate([s.uvs for s in subs]),
                               np.concatenate([s.colors for s in subs]),
                               np.concatenate([s.indices + off[i] for i, s in enumerate(subs)]), m))
        cells[key] = merged
    return cells


def _name(v):
    return f"m{-v}" if v < 0 else str(v)


def convert(src, outdir, tris=4000, cell=0.0, samples=24, bounces=1, ambient=(0.25, 0.28, 0.35), exposure="auto",
            lighting_share=0.5, log=print):
    assets = os.path.join(outdir, "assets")
    os.makedirs(assets, exist_ok=True)
    scene = gltfio.load(src)
    # Part of the budget is kept back to re-tessellate flat surfaces so baked light has vertices to live on.
    orig_tris, new_tris = decimate.budget_scene(scene, int(tris * (1 - lighting_share)))
    log(f"decimate: {orig_tris} -> {new_tris} triangles")
    edge = subdivide.subdivide_to_budget(scene.prims, tris)
    new_tris = sum(len(p.indices) for p in scene.prims)
    log(f"subdivide for lighting (max edge {edge:.2f} m): {new_tris} triangles (budget {tris})")

    # Materials -> fitted textures
    tex_info, fmts = {}, {}
    for i, m in enumerate(scene.materials):
        if m.texture is None:
            continue
        cutout = m.alpha_mode != "OPAQUE"
        key = textures.texture_key(m.texture, cutout)
        if key not in fmts:
            t = textures.fit_texture(m.texture, cutout)
            t["file"] = f"tex_{key}.png"
            t["image"].save(os.path.join(assets, t["file"]), transparency=t["image"].info.get("transparency")) \
                if t["format"] != "RGBA16" else t["image"].save(os.path.join(assets, t["file"]))
            fmts[key] = t
            log(f"texture {m.name}: {t['format']} {t['width']}x{t['height']} ({t['bytes']} B, err {t['error']:.1f})")
        tex_info[i] = fmts[key]

    albedo = []
    for p in scene.prims:
        m = scene.materials[min(p.material, len(scene.materials) - 1)]
        tm = _srgb_mean_linear(m.texture) if m.texture is not None else np.ones(3, np.float32)
        albedo.append(np.array(m.base_color[:3], np.float32) * tm)
    if not scene.lights:
        log("warning: no KHR_lights_punctual lights in export; only ambient/GI from sky will be baked")
    stats = bake_mod.bake(scene, albedo, ambient, samples, bounces, exposure=exposure)
    log(f"bake: exposure x{stats['exposure']:.3g}, mean luminance {stats['mean_luminance']:.2f}")

    for p in scene.prims:
        m = scene.materials[min(p.material, len(scene.materials) - 1)]
        rgb = p.colors[:, :3] * np.array(m.base_color[:3], np.float32) + np.array(m.emissive, np.float32)
        a = m.base_color[3] if m.alpha_mode == "BLEND" else 1.0
        p.colors = np.c_[np.clip(rgb, 0, 1), np.full(len(rgb), a, np.float32)]

    allpos = np.concatenate([p.positions for p in scene.prims])
    lo, hi = allpos.min(0), allpos.max(0)
    cells = split_cells(scene.prims, cell)
    out_cells = []
    for (ix, iz), prims in sorted(cells.items()):
        pos = np.concatenate([p.positions for p in prims])
        cmin, cmax = pos.min(0), pos.max(0)
        origin = (cmin + cmax) / 2
        extent = float(np.abs(pos - origin).max())
        base_scale = max(1, int(INT16_LIMIT // max(extent, 1e-6)))
        used = sorted({p.material for p in prims})
        remap = {m: k for k, m in enumerate(used)}
        local = []
        for p in prims:
            q = Prim(p.name, p.positions - origin, p.normals, p.uvs, p.colors, p.indices, remap[p.material])
            local.append(q)
        extras = []
        for m in used:
            mat = scene.materials[min(m, len(scene.materials) - 1)]
            extras.append(materials.build_extras(mat.name, tex_info.get(m), mat.alpha_mode, mat.double_sided))
        fname = f"cell_{_name(ix)}_{_name(iz)}"
        gltfio.write_glb(os.path.join(assets, fname + ".glb"), local, extras, None)
        out_cells.append(dict(name=fname, origin=[float(v) for v in origin], base_scale=base_scale,
                              aabb_min=[float(v) for v in cmin], aabb_max=[float(v) for v in cmax],
                              tris=int(sum(len(p.indices) for p in prims)), verts=int(sum(len(p.positions) for p in prims))))

    with open(os.path.join(assets, "formats.mk"), "w") as f:
        for t in fmts.values():
            f.write(f"SPRITE_FMT_{t['file'][:-4]} = {t['format']}\n")
        for c in out_cells:
            f.write(f"BASE_SCALE_{c['name']} = {c['base_scale']}\n")
    start = [float((lo[0] + hi[0]) / 2), float(lo[1] + 1.7), float((lo[2] + hi[2]) / 2)]
    manifest = dict(source=os.path.basename(src), triangles_source=orig_tris, triangles_out=new_tris, textures=len(fmts),
                    cells=out_cells, aabb_min=[float(v) for v in lo], aabb_max=[float(v) for v in hi], start=start, bake=stats)
    with open(os.path.join(outdir, "level.json"), "w") as f:
        json.dump(manifest, f, indent=1)
    with open(os.path.join(outdir, "level_index.h"), "w") as f:
        f.write("// Generated by n64fit. Do not edit.\n#pragma once\n"
                "typedef struct { const char *file; float origin[3]; float scale; float aabb_min[3]; float aabb_max[3]; } u64_cell_t;\n"
                f"#define U64_CELL_COUNT {len(out_cells)}\n"
                f"static const float U64_START[3] = {{{start[0]:.4f}f, {start[1]:.4f}f, {start[2]:.4f}f}};\n"
                "static const u64_cell_t U64_CELLS[U64_CELL_COUNT] = {\n")
        for c in out_cells:
            o, a, b = c["origin"], c["aabb_min"], c["aabb_max"]
            f.write(f'  {{"rom:/{c["name"]}.t3dm", {{{o[0]:.4f}f,{o[1]:.4f}f,{o[2]:.4f}f}}, {1.0 / c["base_scale"]:.8f}f, '
                    f'{{{a[0]:.4f}f,{a[1]:.4f}f,{a[2]:.4f}f}}, {{{b[0]:.4f}f,{b[1]:.4f}f,{b[2]:.4f}f}}}},\n')
        f.write("};\n")
    return manifest
