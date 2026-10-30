"""Synthetic stand-in for an Unreal glTF export: textured room, pillar (shadow caster), high-poly sphere, sun + point light."""
import base64
import io
import json

import numpy as np
from PIL import Image


def _box(c, s):
    cx, cy, cz = c
    sx, sy, sz = s
    P, N, T = [], [], []
    for axis in range(3):
        for sign in (-1, 1):
            n = np.zeros(3)
            n[axis] = sign
            u, v = [(1, 2), (2, 0), (0, 1)][axis]
            for du, dv in ((-1, -1), (1, -1), (1, 1), (-1, 1)):
                p = np.array([cx, cy, cz], float)
                p[axis] += sign * (sx, sy, sz)[axis]
                p[u] += du * (sx, sy, sz)[u]
                p[v] += dv * (sx, sy, sz)[v]
                P.append(p), N.append(n), T.append(((du + 1) / 2, (dv + 1) / 2))
    idx = []
    for f in range(6):
        b = f * 4
        sign = -1 if f % 2 == 0 else 1
        quad = [0, 1, 2, 0, 2, 3]
        if (f // 2 + (1 if sign < 0 else 0)) % 2 == 1:
            quad = [0, 2, 1, 0, 3, 2]
        idx += [b + q for q in quad]
    return np.array(P, np.float32), np.array(N, np.float32), np.array(T, np.float32), np.array(idx, np.uint32)


def _floor(size, div):
    xs = np.linspace(-size, size, div + 1)
    P = np.array([[x, 0, z] for z in xs for x in xs], np.float32)
    T = np.array([[(x / size + 1) * 2, (z / size + 1) * 2] for z in xs for x in xs], np.float32)
    idx = []
    for j in range(div):
        for i in range(div):
            a = j * (div + 1) + i
            idx += [a, a + div + 1, a + 1, a + 1, a + div + 1, a + div + 2]
    return P, np.tile([0, 1, 0], (len(P), 1)).astype(np.float32), T, np.array(idx, np.uint32)


def _sphere(center, r, sub):
    v = [(-1, 1.618, 0), (1, 1.618, 0), (-1, -1.618, 0), (1, -1.618, 0), (0, -1, 1.618), (0, 1, 1.618), (0, -1, -1.618),
         (0, 1, -1.618), (1.618, 0, -1), (1.618, 0, 1), (-1.618, 0, -1), (-1.618, 0, 1)]
    f = [(0, 11, 5), (0, 5, 1), (0, 1, 7), (0, 7, 10), (0, 10, 11), (1, 5, 9), (5, 11, 4), (11, 10, 2), (10, 7, 6), (7, 1, 8),
         (3, 9, 4), (3, 4, 2), (3, 2, 6), (3, 6, 8), (3, 8, 9), (4, 9, 5), (2, 4, 11), (6, 2, 10), (8, 6, 7), (9, 8, 1)]
    verts = [np.array(x, float) / np.linalg.norm(x) for x in v]
    for _ in range(sub):
        cache, nf = {}, []

        def mid(a, b):
            k = (min(a, b), max(a, b))
            if k not in cache:
                verts.append((verts[a] + verts[b]) / np.linalg.norm(verts[a] + verts[b]))
                cache[k] = len(verts) - 1
            return cache[k]
        for a, b, c in f:
            ab, bc, ca = mid(a, b), mid(b, c), mid(c, a)
            nf += [(a, ab, ca), (b, bc, ab), (c, ca, bc), (ab, bc, ca)]
        f = nf
    V = np.array(verts, np.float32)
    uv = np.c_[np.arctan2(V[:, 2], V[:, 0]) / (2 * np.pi) + 0.5, np.arccos(np.clip(V[:, 1], -1, 1)) / np.pi].astype(np.float32)
    return V * r + np.array(center, np.float32), V.copy(), uv, np.array(f, np.uint32).reshape(-1)


def _checker():
    a = np.zeros((128, 128, 3), np.uint8)
    for y in range(128):
        for x in range(128):
            a[y, x] = (200, 160, 110) if ((x // 16) + (y // 16)) % 2 else (90, 60, 40)
    b = io.BytesIO()
    Image.fromarray(a).save(b, "PNG")
    return "data:image/png;base64," + base64.b64encode(b.getvalue()).decode()


def build(path):
    """Writes a .gltf (embedded buffers) and returns metadata for assertions."""
    parts = {
        "floor": (_floor(6, 8), 0), "pillar": (_box((0, 1.5, 0), (0.5, 1.5, 0.5)), 1),
        "wall_n": (_box((0, 1.5, -6.2), (6.2, 1.5, 0.2)), 1), "wall_s": (_box((0, 1.5, 6.2), (6.2, 1.5, 0.2)), 1),
        "wall_e": (_box((6.2, 1.5, 0), (0.2, 1.5, 6.2)), 1), "wall_w": (_box((-6.2, 1.5, 0), (0.2, 1.5, 6.2)), 1),
        "ball": (_sphere((3, 0.8, 3), 0.8, 3), 1),
    }
    buf, views, accs, meshes, nodes = bytearray(), [], [], [], []

    def add(arr, comp, typ, target):
        buf.extend(b"\0" * (-len(buf) % 4))
        views.append({"buffer": 0, "byteOffset": len(buf), "byteLength": arr.nbytes, "target": target})
        buf.extend(arr.tobytes())
        a = {"bufferView": len(views) - 1, "componentType": comp, "count": len(arr), "type": typ}
        if typ == "VEC3" and comp == 5126:
            a["min"], a["max"] = arr.min(0).tolist(), arr.max(0).tolist()
        accs.append(a)
        return len(accs) - 1
    for name, ((P, N, T, I), mat) in parts.items():
        meshes.append({"primitives": [{"attributes": {"POSITION": add(P, 5126, "VEC3", 34962), "NORMAL": add(N, 5126, "VEC3", 34962),
                                                      "TEXCOORD_0": add(T, 5126, "VEC2", 34962)},
                                       "indices": add(I, 5125, "SCALAR", 34963), "material": mat}]})
        nodes.append({"name": name, "mesh": len(meshes) - 1})
    nodes.append({"name": "sun", "rotation": [-0.5, 0.2, 0.1, 0.8], "extensions": {"KHR_lights_punctual": {"light": 0}}})
    nodes.append({"name": "lamp", "translation": [-3, 2.5, -2], "extensions": {"KHR_lights_punctual": {"light": 1}}})
    doc = {
        "asset": {"version": "2.0"}, "scene": 0, "scenes": [{"nodes": list(range(len(nodes)))}], "nodes": nodes, "meshes": meshes,
        "materials": [{"name": "Floor", "pbrMetallicRoughness": {"baseColorTexture": {"index": 0}}},
                      {"name": "Plaster", "pbrMetallicRoughness": {"baseColorFactor": [0.8, 0.75, 0.7, 1]}}],
        "textures": [{"source": 0}], "images": [{"uri": _checker()},],
        "accessors": accs, "bufferViews": views,
        "buffers": [{"byteLength": len(buf), "uri": "data:application/octet-stream;base64," + base64.b64encode(bytes(buf)).decode()}],
        "extensionsUsed": ["KHR_lights_punctual"],
        "extensions": {"KHR_lights_punctual": {"lights": [
            {"type": "directional", "color": [1, 0.95, 0.85], "intensity": 3.0},
            {"type": "point", "color": [1, 0.7, 0.4], "intensity": 800.0, "range": 0}]}},
    }
    with open(path, "w") as f:
        json.dump(doc, f)
    return {"source_tris": sum(len(I) // 3 for (_, (_, _, _, I), _) in [(k, v[0], v[1]) for k, v in parts.items()])}


if __name__ == "__main__":
    import sys
    out = sys.argv[1] if len(sys.argv) > 1 else "level.gltf"
    build(out)
    print("wrote", out)
