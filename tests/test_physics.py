"""Collision data extraction: convex element transforms, physics-asset bodies in the reference pose, hull planes."""
import math
import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "tools", "ue64"))
import gasdata  # noqa: E402

# The Transform of SM_Forge_Chest_Bottom's convex element as GASShooter stores it: a tagged-property blob left raw by the property reader
# (Rotation = Quat (0, 0, -0.7071, 0.7071), Translation = 0, Scale3D = 1).
BLOB = bytes.fromhex(
    "78000000000000008a000000000000001000000000000000740000000000000000000000000000000000000000000000000000000000000000"
    "f30435bff304353f8f000000000000008a000000000000000c00000000000000920000000000000000000000000000000000000000000000000000000000000000"
    "000000007a000000000000008a000000000000000c00000000000000920000000000000000000000000000000000000000000000000000803f0000803f0000803f"
    "6600000000000000")


class FakePackage:
    names = ["None"] * 200


FakePackage.names[0x78], FakePackage.names[0x8F], FakePackage.names[0x7A], FakePackage.names[0x66] = "Rotation", "Translation", "Scale3D", "None"


class ElemTransformTests(unittest.TestCase):
    def test_raw_blob(self):
        q, t, sc = gasdata._elem_transform(FakePackage, ("raw", BLOB))
        self.assertAlmostEqual(q[2], -math.sqrt(0.5), places=5)
        self.assertAlmostEqual(q[3], math.sqrt(0.5), places=5)
        self.assertEqual(t, (0.0, 0.0, 0.0))
        self.assertEqual(sc, (1.0, 1.0, 1.0))

    def test_yaw_minus_90_swaps_axes(self):
        q, t, sc = gasdata._elem_transform(FakePackage, ("raw", BLOB))
        (p,) = gasdata._xform_points([(64.0, -39.5, 0.5)], q, t, sc)
        self.assertAlmostEqual(p[0], -39.5, places=3)        # the chest is 79 deep (x) and 128 wide (y) once the element's rotation is applied
        self.assertAlmostEqual(p[1], -64.0, places=3)
        self.assertAlmostEqual(p[2], 0.5, places=3)

    def test_missing_transform_is_identity(self):
        self.assertEqual(gasdata._elem_transform(FakePackage, None), ((0.0, 0.0, 0.0, 1.0), (0.0, 0.0, 0.0), (1.0, 1.0, 1.0)))


class HullTests(unittest.TestCase):
    def test_cube_has_six_planes(self):
        pts = [(x, y, z) for x in (-1, 1) for y in (-2, 2) for z in (-3, 3)]
        planes = gasdata.hull_planes(pts)
        self.assertEqual(len(planes), 6)
        self.assertEqual(sorted(round(d, 3) for *_, d in planes), [1.0, 1.0, 2.0, 2.0, 3.0, 3.0])


class SkeletalBodyTests(unittest.TestCase):
    def test_body_follows_the_bone(self):
        # root at the origin, child bone 100 up and rotated 90 degrees about z; a sphyl element offset 10 along the bone's x
        s = math.sqrt(0.5)
        bones = [("root", -1, (0, 0, 0), (0, 0, 0, 1), (1, 1, 1)), ("arm", 0, (0, 0, 100), (0, 0, s, s), (1, 1, 1))]
        (b,) = gasdata._skeletal_bodies(bones, [("arm", 2, (10, 0, 0), (0, 0, 0), (7.0, 30.0, 0.0))])
        bone, kind, t, r, rad, half, _ = b
        self.assertEqual((bone, kind), ("arm", 2))
        self.assertAlmostEqual(rad, 7.0); self.assertAlmostEqual(half, 15.0)      # radius, half the cylinder length
        # the element offset (10, 0, 0) in the bone's frame is (0, 10, 0) in mesh space
        self.assertAlmostEqual(t[0], 0.0, places=5); self.assertAlmostEqual(t[1], 10.0, places=5); self.assertAlmostEqual(t[2], 100.0, places=5)
        self.assertAlmostEqual(r[0], 0.0, places=5); self.assertAlmostEqual(r[3], 1.0, places=5)    # local x maps to mesh y


if __name__ == "__main__":
    unittest.main()
