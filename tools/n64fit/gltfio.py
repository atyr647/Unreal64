"""Minimal glTF 2.0 / GLB reader (what Unreal's exporter emits) and a Tiny3D-flavoured GLB writer.

Reader flattens the node graph into world-space primitives. Supported: TRS/matrix nodes, uint8/16/32 indices,
normalised/float attributes, byteStride, external/embedded/bufferView images, KHR_lights_punctual.
Not supported (raises or is ignored): sparse accessors, skinning, morph targets, KHR_texture_transform.
"""
from __future__ import annotations

import base64
import io
import json
import os
import struct
from dataclasses import dataclass, field

import numpy as np
from PIL import Image

_COMP = {5120: np.int8, 5121: np.uint8, 5122: np.int16, 5123: np.uint16, 5125: np.uint32, 5126: np.float32}
_NCOMP = {"SCALAR": 1, "VEC2": 2, "VEC3": 3, "VEC4": 4, "MAT4": 16}
_NORM_DIV = {np.int8: 127.0, np.uint8: 255.0, np.int16: 32767.0, np.uint16: 65535.0}


@dataclass
class Material:
    name: str = "default"
    base_color: tuple = (1.0, 1.0, 1.0, 1.0)  # linear
    texture: Image.Image | None = None
    emissive: tuple = (0.0, 0.0, 0.0)
    alpha_mode: str = "OPAQUE"
    alpha_cutoff: float = 0.5
    double_sided: bool = False
    roughness: float = 1.0
    metallic: float = 0.0


@dataclass
class Prim:
    name: str
    positions: np.ndarray  # (N,3) f32 world space
    normals: np.ndarray  # (N,3) f32
    uvs: np.ndarray | None  # (N,2) f32
    colors: np.ndarray | None  # (N,4) f32 linear
    indices: np.ndarray  # (M,3) int32
    material: int = 0


@dataclass
class Light:
    kind: str  # directional | point | spot
    color: tuple
    intensity: float
    position: np.ndarray
    direction: np.ndarray  # direction the light points
    range: float = 0.0
    inner: float = 0.0
    outer: float = 0.7853982


@dataclass
class Scene:
    prims: list[Prim] = field(default_factory=list)
    materials: list[Material] = field(default_factory=list)
    lights: list[Light] = field(default_factory=list)


def _srgb_to_linear(c):
    c = np.asarray(c, dtype=np.float32)
    return np.where(c <= 0.04045, c / 12.92, ((c + 0.055) / 1.055) ** 2.4)


def _read_container(path):
    with open(path, "rb") as f:
        data = f.read()
    if data[:4] != b"glTF":
        return json.loads(data), None
    pos, doc, binchunk = 12, None, None
    while pos < len(data):
        length, ctype = struct.unpack_from("<II", data, pos)
        body = data[pos + 8:pos + 8 + length]
        if ctype == 0x4E4F534A:
            doc = json.loads(body)
        elif ctype == 0x004E4942 and binchunk is None:
            binchunk = body
        pos += 8 + length + (-length % 4)
    return doc, binchunk


def _load_uri(uri, base_dir):
    if uri.startswith("data:"):
        return base64.b64decode(uri.split(",", 1)[1])
    with open(os.path.join(base_dir, uri), "rb") as f:
        return f.read()


class _Doc:
    def __init__(self, path):
        self.doc, glb_bin = _read_container(path)
        self.dir = os.path.dirname(os.path.abspath(path))
        self.buffers = []
        for i, b in enumerate(self.doc.get("buffers", [])):
            self.buffers.append(glb_bin if "uri" not in b and i == 0 and glb_bin is not None
                                else _load_uri(b["uri"], self.dir))

    def view_bytes(self, vi):
        v = self.doc["bufferViews"][vi]
        off = v.get("byteOffset", 0)
        return self.buffers[v["buffer"]][off:off + v["byteLength"]]

    def accessor(self, ai):
        a = self.doc["accessors"][ai]
        if "sparse" in a:
            raise NotImplementedError("sparse accessors")
        dt, n = _COMP[a["componentType"]], _NCOMP[a["type"]]
        count = a["count"]
        if "bufferView" not in a:
            return np.zeros((count, n), dtype=np.float32)
        view = self.doc["bufferViews"][a["bufferView"]]
        raw = self.buffers[view["buffer"]]
        start = view.get("byteOffset", 0) + a.get("byteOffset", 0)
        itemsize = np.dtype(dt).itemsize
        stride = view.get("byteStride") or itemsize * n
        if stride == itemsize * n:
            arr = np.frombuffer(raw, dtype=dt, count=count * n, offset=start).reshape(count, n)
        else:
            rows = np.frombuffer(raw, dtype=np.uint8, count=stride * (count - 1) + itemsize * n, offset=start)
            arr = np.stack([np.frombuffer(rows[i * stride:i * stride + itemsize * n].tobytes(), dtype=dt)
                            for i in range(count)])
        if a.get("normalized") and dt in _NORM_DIV:
            arr = np.maximum(arr.astype(np.float32) / _NORM_DIV[dt], -1.0)
        return arr

    def image(self, ii):
        im = self.doc["images"][ii]
        blob = self.view_bytes(im["bufferView"]) if "bufferView" in im else _load_uri(im["uri"], self.dir)
        return Image.open(io.BytesIO(blob)).convert("RGBA")


def _node_matrix(node):
    if "matrix" in node:
        return np.array(node["matrix"], dtype=np.float64).reshape(4, 4).T
    t = np.array(node.get("translation", (0, 0, 0)), dtype=np.float64)
    x, y, z, w = node.get("rotation", (0, 0, 0, 1))
    s = np.array(node.get("scale", (1, 1, 1)), dtype=np.float64)
    r = np.array([[1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
                  [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
                  [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]])
    m = np.eye(4)
    m[:3, :3] = r * s
    m[:3, 3] = t
    return m


def load(path) -> Scene:
    d = _Doc(path)
    doc = d.doc
    out = Scene()
    images = {}
    for m in doc.get("materials", []):
        pbr = m.get("pbrMetallicRoughness", {})
        bc = pbr.get("baseColorFactor", [1, 1, 1, 1])
        tex = None
        ti = pbr.get("baseColorTexture", {}).get("index")
        if ti is not None:
            src = doc["textures"][ti].get("source")
            if src is not None:
                tex = images.setdefault(src, d.image(src))
        out.materials.append(Material(
            m.get("name", f"mat{len(out.materials)}"), tuple(float(v) for v in bc), tex,
            tuple(m.get("emissiveFactor", (0, 0, 0))), m.get("alphaMode", "OPAQUE"),
            m.get("alphaCutoff", 0.5), m.get("doubleSided", False),
            pbr.get("roughnessFactor", 1.0), pbr.get("metallicFactor", 1.0)))
    if not out.materials:
        out.materials.append(Material())
    punctual = doc.get("extensions", {}).get("KHR_lights_punctual", {}).get("lights", [])

    def visit(ni, parent):
        node = doc["nodes"][ni]
        world = parent @ _node_matrix(node)
        if "mesh" in node:
            for pi, p in enumerate(doc["meshes"][node["mesh"]]["primitives"]):
                if p.get("mode", 4) != 4:
                    continue
                attrs = p["attributes"]
                pos = d.accessor(attrs["POSITION"]).astype(np.float64)
                pos_w = (np.c_[pos, np.ones(len(pos))] @ world.T)[:, :3]
                if "NORMAL" in attrs:
                    nm = np.linalg.inv(world[:3, :3]).T
                    nrm = d.accessor(attrs["NORMAL"]).astype(np.float64) @ nm.T
                    nrm /= np.maximum(np.linalg.norm(nrm, axis=1, keepdims=True), 1e-12)
                else:
                    nrm = None
                idx = (d.accessor(p["indices"]).reshape(-1) if "indices" in p else np.arange(len(pos))).astype(np.int32)
                tris = idx.reshape(-1, 3)
                if np.linalg.det(world[:3, :3]) < 0:
                    tris = tris[:, ::-1]  # mirrored transform flips winding
                if nrm is None:
                    a, b, c = (pos_w[tris[:, i]] for i in range(3))
                    fn = np.cross(b - a, c - a)
                    fn /= np.maximum(np.linalg.norm(fn, axis=1, keepdims=True), 1e-12)
                    nrm = np.zeros_like(pos_w)
                    for i in range(3):
                        np.add.at(nrm, tris[:, i], fn)
                    nrm /= np.maximum(np.linalg.norm(nrm, axis=1, keepdims=True), 1e-12)
                uv = d.accessor(attrs["TEXCOORD_0"]).astype(np.float32) if "TEXCOORD_0" in attrs else None
                col = None
                if "COLOR_0" in attrs:
                    col = d.accessor(attrs["COLOR_0"]).astype(np.float32)
                    if col.shape[1] == 3:
                        col = np.c_[col, np.ones(len(col), dtype=np.float32)]
                out.prims.append(Prim(f"{node.get('name', f'node{ni}')}_{pi}", pos_w.astype(np.float32),
                                      nrm.astype(np.float32), uv, col, tris.astype(np.int32), p.get("material", 0)))
        li = node.get("extensions", {}).get("KHR_lights_punctual", {}).get("light")
        if li is not None:
            L = punctual[li]
            direction = world[:3, :3] @ np.array([0.0, 0.0, -1.0])
            direction /= max(np.linalg.norm(direction), 1e-12)
            spot = L.get("spot", {})
            out.lights.append(Light(L["type"], tuple(L.get("color", (1, 1, 1))), float(L.get("intensity", 1.0)),
                                    world[:3, 3].astype(np.float32), direction.astype(np.float32),
                                    float(L.get("range", 0.0)), spot.get("innerConeAngle", 0.0),
                                    spot.get("outerConeAngle", 0.7853982)))
        for c in node.get("children", []):
            visit(c, world)

    scene_i = doc.get("scene", 0)
    for root in doc["scenes"][scene_i]["nodes"]:
        visit(root, np.eye(4))
    return out


# ---------------------------------------------------------------- writer

def write_glb(path, prims, materials_extras, tex_names, positions_scale=1.0):
    """Write prims (already fitted) as GLB with Fast64-style `extras` on each material.

    `materials_extras[i]` is the material's JSON extras dict; `tex_names[i]` is the texture file name (or None).
    Positions are written in glTF float units; the t3d importer multiplies by --base-scale.
    """
    blob = bytearray()
    views, accessors, meshes_prims = [], [], []

    def add(arr, comp, ctype, target, normalized=False, minmax=False):
        blob.extend(b"\0" * (-len(blob) % 4))
        views.append({"buffer": 0, "byteOffset": len(blob), "byteLength": arr.nbytes, "target": target})
        blob.extend(arr.tobytes())
        acc = {"bufferView": len(views) - 1, "componentType": comp, "count": len(arr), "type": ctype}
        if normalized:
            acc["normalized"] = True
        if minmax:
            acc["min"], acc["max"] = arr.min(0).tolist(), arr.max(0).tolist()
        accessors.append(acc)
        return len(accessors) - 1

    for p in prims:
        a = {"POSITION": add((p.positions * positions_scale).astype("<f4"), 5126, "VEC3", 34962, minmax=True),
             "NORMAL": add(p.normals.astype("<f4"), 5126, "VEC3", 34962)}
        if p.uvs is not None:
            a["TEXCOORD_0"] = add(p.uvs.astype("<f4"), 5126, "VEC2", 34962)
        if p.colors is not None:
            a["COLOR_0"] = add(np.clip(p.colors * 255 + 0.5, 0, 255).astype("<u1"), 5121, "VEC4", 34962, True)
        meshes_prims.append({"attributes": a, "indices": add(p.indices.reshape(-1).astype("<u4"), 5125, "SCALAR", 34963),
                             "material": p.material})
    nodes = [{"name": p.name, "mesh": i} for i, p in enumerate(prims)]
    doc = {
        "asset": {"version": "2.0", "generator": "n64fit"},
        "scene": 0, "scenes": [{"nodes": list(range(len(nodes)))}], "nodes": nodes,
        "meshes": [{"name": p.name, "primitives": [mp]} for p, mp in zip(prims, meshes_prims)],
        "materials": [{"name": f"m{i}_{e.get('_name', 'mat')}", "extras": {k: v for k, v in e.items() if not k.startswith("_")}}
                      for i, e in enumerate(materials_extras)],
        "accessors": accessors, "bufferViews": views, "buffers": [{"byteLength": len(blob)}],
    }
    js = json.dumps(doc, separators=(",", ":")).encode()
    js += b" " * (-len(js) % 4)
    blob.extend(b"\0" * (-len(blob) % 4))
    total = 12 + 8 + len(js) + 8 + len(blob)
    with open(path, "wb") as f:
        f.write(struct.pack("<4sII", b"glTF", 2, total))
        f.write(struct.pack("<II", len(js), 0x4E4F534A) + js)
        f.write(struct.pack("<II", len(blob), 0x004E4942) + bytes(blob))
