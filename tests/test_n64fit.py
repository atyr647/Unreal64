import json
import os
import struct
import sys
import tempfile
import unittest

import numpy as np

ROOT = os.path.join(os.path.dirname(__file__), "..")
sys.path.insert(0, os.path.join(ROOT, "tools"))
sys.path.insert(0, os.path.dirname(__file__))
import scene_gen  # noqa: E402
from n64fit import bake, gltfio, pipeline, textures  # noqa: E402
from PIL import Image  # noqa: E402

# Keys the Tiny3D importer reads without a default (tools/gltf_importer/src/parser/materialParser.cpp).
REQUIRED_RDP = ["g_cull_back", "g_cull_front", "g_fog", "g_mdsft_cycletype", "g_mdsft_text_filt", "g_tex_gen", "set_rendermode",
                "rendermode_preset_cycle_1", "rendermode_preset_cycle_2"]
REQUIRED_CC = ["A", "B", "C", "D", "A_alpha", "B_alpha", "C_alpha", "D_alpha"]


def read_glb(path):
    d = open(path, "rb").read()
    n = struct.unpack_from("<I", d, 12)[0]
    return json.loads(d[20:20 + n])


class Base(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp = tempfile.TemporaryDirectory()
        cls.src = os.path.join(cls.tmp.name, "level.gltf")
        scene_gen.build(cls.src)

    @classmethod
    def tearDownClass(cls):
        cls.tmp.cleanup()


class LoaderTests(Base):
    def test_loads_geometry_materials_lights(self):
        s = gltfio.load(self.src)
        self.assertEqual(len(s.prims), 7)
        self.assertEqual({L.kind for L in s.lights}, {"directional", "point"})
        self.assertIsNotNone(s.materials[0].texture)
        self.assertTrue(all(p.indices.max() < len(p.positions) for p in s.prims))
        lamp = next(L for L in s.lights if L.kind == "point")
        np.testing.assert_allclose(lamp.position, [-3, 2.5, -2], atol=1e-5)


class TextureTests(unittest.TestCase):
    def test_fits_tmem_for_every_candidate(self):
        rng = np.random.default_rng(1)
        img = Image.fromarray(rng.integers(0, 255, (300, 200, 3), dtype=np.uint8))
        t = textures.fit_texture(img, False)
        self.assertLessEqual(t["bytes"], 4096)
        self.assertEqual(2 ** t["mask_s"], t["width"])

    def test_flat_colour_picks_palette_and_is_tiny(self):
        t = textures.fit_texture(Image.new("RGBA", (256, 256), (200, 30, 30, 255)), False)
        self.assertLess(t["error"], 12)  # 5-bit-per-channel rounding of (200,30,30) alone costs ~9 MSE

    def test_cutout_keeps_transparency(self):
        a = np.zeros((64, 64, 4), np.uint8)
        a[..., :3] = 255
        a[:, :32, 3] = 255
        t = textures.fit_texture(Image.fromarray(a), True)
        out = np.asarray(t["image"].convert("RGBA"))
        self.assertEqual(int(out[10, 3, 3]), 255)
        self.assertEqual(int(out[10, -3, 3]), 0)


class BakeTests(Base):
    def _scene(self):
        s = gltfio.load(self.src)
        return s, [np.array([0.7, 0.7, 0.7], np.float32)] * len(s.prims)

    def test_shadow_behind_pillar_is_darker(self):
        s, alb = self._scene()
        s.lights = [L for L in s.lights if L.kind == "point"]
        s.lights[0].position = np.array([-4.0, 1.5, 0.0], np.float32)  # pillar at origin
        bake.bake(s, alb, samples=0, ambient=(0, 0, 0), exposure=1.0)
        floor = s.prims[0]
        pos, col = floor.positions, floor.colors[:, 0]
        lit = col[np.argmin(np.linalg.norm(pos - np.array([-2.0, 0, 0.0]), axis=1))]
        shadowed = col[np.argmin(np.linalg.norm(pos - np.array([2.0, 0, 0.0]), axis=1))]
        self.assertGreater(lit, shadowed * 3)

    def test_ao_darkens_corners_and_is_deterministic(self):
        s, alb = self._scene()
        s.lights = []
        bake.bake(s, alb, samples=24, bounces=0, exposure=1.0)
        floor = s.prims[0]
        c1 = floor.colors.copy()
        corner = np.argmin(np.linalg.norm(floor.positions - np.array([-5.9, 0, -5.9]), axis=1))
        open_ = np.argmin(np.linalg.norm(floor.positions - np.array([3.0, 0, -3.0]), axis=1))
        self.assertLess(c1[corner, 0], c1[open_, 0])
        s2, alb2 = self._scene()
        s2.lights = []
        bake.bake(s2, alb2, samples=24, bounces=0, exposure=1.0)
        np.testing.assert_array_equal(c1, s2.prims[0].colors)

    def test_bounce_adds_light(self):
        a, alb = self._scene()
        b, _ = self._scene()
        bake.bake(a, alb, samples=16, bounces=0, exposure=1.0)
        bake.bake(b, alb, samples=16, bounces=1, exposure=1.0)
        mean = lambda s: np.mean([p.colors[:, :3].mean() for p in s.prims])
        self.assertGreater(mean(b), mean(a))


class PipelineTests(Base):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls.out = os.path.join(cls.tmp.name, "out")
        cls.m = pipeline.convert(cls.src, cls.out, tris=700, cell=0.0, samples=8, log=lambda *_: None)

    def test_decimation_respects_budget(self):
        self.assertGreater(self.m["triangles_source"], self.m["triangles_out"])
        self.assertLessEqual(self.m["triangles_out"], 700)  # hard budget, including re-tessellation for lighting

    def test_positions_fit_int16_after_importer_scale(self):
        for c in self.m["cells"]:
            doc = read_glb(os.path.join(self.out, "assets", c["name"] + ".glb"))
            for a in doc["accessors"]:
                if a["type"] == "VEC3" and a.get("max") and a["componentType"] == 5126 and a["min"] != a["max"]:
                    self.assertLessEqual(max(abs(v) for v in a["max"] + a["min"]) * c["base_scale"], 32767)

    def test_material_extras_satisfy_importer(self):
        c = self.m["cells"][0]
        doc = read_glb(os.path.join(self.out, "assets", c["name"] + ".glb"))
        self.assertTrue(doc["materials"])
        for mat in doc["materials"]:
            self.assertTrue(mat["name"])
            f = mat["extras"]["f3d_mat"]
            for k in REQUIRED_RDP:
                self.assertIn(k, f["rdp_settings"])
            for cc in ("combiner1", "combiner2"):
                for k in REQUIRED_CC:
                    self.assertIn(k, f[cc])
        textured = [m for m in doc["materials"] if "tex0" in m["extras"]["f3d_mat"]]
        self.assertEqual(len(textured), 1)
        t = textured[0]["extras"]["f3d_mat"]["tex0"]
        self.assertTrue(os.path.exists(os.path.join(self.out, "assets", t["tex"]["name"])))
        self.assertEqual(2 ** t["S"]["mask"], t["S"]["high"] + 1)

    def test_cells_cover_all_triangles(self):
        out2 = os.path.join(self.tmp.name, "out_cells")
        m2 = pipeline.convert(self.src, out2, tris=700, cell=6.0, samples=0, log=lambda *_: None)
        self.assertGreater(len(m2["cells"]), 1)
        self.assertEqual(sum(c["tris"] for c in m2["cells"]), m2["triangles_out"])
        hdr = open(os.path.join(out2, "level_index.h")).read()
        self.assertIn(f"U64_CELL_COUNT {len(m2['cells'])}", hdr)

    def test_vertex_colours_present_and_nontrivial(self):
        c = self.m["cells"][0]
        doc = read_glb(os.path.join(self.out, "assets", c["name"] + ".glb"))
        self.assertTrue(all("COLOR_0" in mp["attributes"] for me in doc["meshes"] for mp in me["primitives"]))


class SubdivideTests(unittest.TestCase):
    def _quad(self):
        P = np.array([[0, 0, 0], [4, 0, 0], [4, 0, 4], [0, 0, 4]], np.float32)
        return gltfio.Prim("q", P, np.tile([0, 1, 0], (4, 1)).astype(np.float32), P[:, [0, 2]].copy(),
                           np.ones((4, 4), np.float32), np.array([[0, 1, 2], [0, 2, 3]], np.int32))

    def test_conforming_and_area_preserving(self):
        from n64fit import subdivide
        q = self._quad()
        subdivide.subdivide_prim(q, 1.1)
        a, b, c = (q.positions[q.indices[:, i]] for i in range(3))
        area = np.linalg.norm(np.cross(b - a, c - a), axis=1).sum() / 2
        self.assertAlmostEqual(float(area), 16.0, places=3)
        self.assertGreater(len(q.indices), 16)
        # Conforming: every undirected edge is shared by at most 2 triangles (a T-junction would show as extra splits).
        e = np.sort(np.concatenate([q.indices[:, [0, 1]], q.indices[:, [1, 2]], q.indices[:, [2, 0]]]), axis=1)
        _, counts = np.unique(e, axis=0, return_counts=True)
        self.assertLessEqual(counts.max(), 2)
        # Winding preserved: all triangles still face +Y.
        n = np.cross(b - a, c - a)
        self.assertTrue((np.sign(n[:, 1]) == np.sign(np.cross(*(q.positions[[1, 2]] - q.positions[0]))[1])).all())

    def test_budget_is_respected(self):
        from n64fit import subdivide
        q = self._quad()
        subdivide.subdivide_to_budget([q], 40, start_edge=0.2)
        self.assertLessEqual(len(q.indices), 40)


if __name__ == "__main__":
    unittest.main()
