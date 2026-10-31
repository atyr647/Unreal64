"""Animation: track sampling and pose composition (tools/ue64/animation.py)."""
import math
import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "tools", "ue64"))
import animation  # noqa: E402


class SampleTests(unittest.TestCase):
    def test_constant_track(self):
        self.assertEqual(animation.sample_track([(1, 2, 3)], 8, 1.0, 0.7, lambda a, b, f: a), (1, 2, 3))

    def test_linear_keys(self):
        lerp = lambda a, b, f: tuple(x + (y - x) * f for x, y in zip(a, b))
        keys = [(0, 0, 0), (10, 0, 0), (10, 20, 0)]
        self.assertEqual(animation.sample_track(keys, 3, 1.0, 0.25, lerp), (5.0, 0.0, 0.0))     # keys span the length: 0.25 s is half way to key 1
        self.assertEqual(animation.sample_track(keys, 3, 1.0, 2.0, lerp), (10, 20, 0))          # clamped at the end

    def test_slerp_halfway(self):
        s = math.sqrt(0.5)
        q = animation._slerp((0, 0, 0, 1), (0, 0, s, s), 0.5)                                    # identity -> 90 degrees about z: 45 degrees
        self.assertAlmostEqual(q[2], math.sin(math.radians(22.5)), places=5)
        self.assertAlmostEqual(q[3], math.cos(math.radians(22.5)), places=5)

    def test_pose_applies_tracks_to_named_bones(self):
        bones = [("root", -1, (0, 0, 0), (0, 0, 0, 1), (1, 1, 1)), ("arm", 0, (0, 0, 10), (0, 0, 0, 1), (1, 1, 1))]
        seq = {"length": 1.0, "frames": 2, "tracks": {"arm": ([(0, 0, 10), (0, 0, 20)], [])}}
        posed = animation.pose(bones, seq, 0.5)
        self.assertEqual(posed[1][2], (0.0, 0.0, 15.0))
        self.assertEqual(posed[0][2], (0, 0, 0))                                                  # no track: the reference pose

    def test_socket_follows_posed_bone(self):
        bones = [("root", -1, (0, 0, 0), (0, 0, 0, 1), (1, 1, 1)), ("hand", 0, (0, 0, 100), (0, 0, 0, 1), (1, 1, 1))]
        t, q = animation.socket_transform(bones, "hand", (5, 0, 0), (0, 0, 0))
        self.assertEqual(t, (5.0, 0.0, 100.0))


if __name__ == "__main__":
    unittest.main()
