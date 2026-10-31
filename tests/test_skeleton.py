import os, struct, sys, unittest
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "tools", "ue64"))
import skeleton


def blob(bones, poses, stride_fmt="<10f"):
    b = struct.pack("<i", len(bones))
    for nm, par in bones:
        s = nm.encode() + b"\0"
        b += struct.pack("<iiii", nm_idx[nm], 0, par, len(s)) + s
    b += struct.pack("<i", len(poses))
    for t, q, s in poses:
        b += struct.pack(stride_fmt, *q, *t, *s)
    return b


nm_idx = {"root": 0, "arm": 1}
NAMES = ["root", "arm"]


class RefSkeletonTests(unittest.TestCase):
    def test_reads_bones_and_poses(self):
        h = 0.7071067811865476
        d = b"\xaa" * 3 + blob([("root", -1), ("arm", 0)], [((0, 0, 0), (0, 0, h, h), (1, 1, 1)), ((10, 0, 5), (0, 0, 0, 1), (1, 1, 1))])
        bones, poses, _ = skeleton._bones_at(d, NAMES, 3, len(d))
        self.assertEqual(bones, [("root", -1), ("arm", 0)])
        cs = skeleton.component_space([(b[0], b[1]) + p for b, p in zip(bones, poses)])
        self.assertAlmostEqual(cs[1][0][0], 0, places=5)      # (10,0,5) rotated by yaw 90 -> (0,10,5)
        self.assertAlmostEqual(cs[1][0][1], 10, places=5)
        self.assertAlmostEqual(cs[1][0][2], 5, places=5)

    def test_rejects_non_unit_quaternion(self):
        d = blob([("root", -1)], [((0, 0, 0), (0, 0, 0, 3), (1, 1, 1))])
        self.assertIsNone(skeleton._bones_at(d, NAMES, 0, len(d)))

    def test_rejects_bad_parent(self):
        d = blob([("root", -1), ("arm", 1)], [((0, 0, 0), (0, 0, 0, 1), (1, 1, 1))] * 2)
        self.assertIsNone(skeleton._bones_at(d, NAMES, 0, len(d)))

    def test_euler_yaw90(self):
        q = skeleton.euler_to_quat(0, 90, 0)
        v = skeleton.quat_rot(q, (1, 0, 0))
        self.assertAlmostEqual(v[1], 1, places=6)


if __name__ == "__main__":
    unittest.main()
