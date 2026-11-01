"""FRandomStream / VRandCone / RotateAngleAxis in the UE shim (cpp2mips/shim/ue_math.h) against an independent Python implementation of the engine's definitions:
Seed = Seed * 196314165 + 907633515 (uint32 wrap); GetFraction = frac(float with the exponent of 1.0 and the seed's low 23 bits as mantissa); the elliptical cone sampling.
The reference is written from the same definitions, so it checks the C++ port (integer wrap, bit trick, rotation formulas), not the engine itself."""
import math, os, struct, subprocess, tempfile, unittest

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def frac_stream(seed):
    seed &= 0xFFFFFFFF
    def nxt():
        nonlocal seed
        seed = (seed * 196314165 + 907633515) & 0xFFFFFFFF
        bits = (0x3F800000 & 0xFF800000) | (seed & 0x007FFFFF)
        f = struct.unpack("<f", struct.pack("<I", bits))[0]
        return f - math.trunc(f)
    return nxt


def rot_axis(v, deg, a):
    r = math.radians(deg); S, C = math.sin(r), math.cos(r); x, y, z = a; OMC = 1 - C
    return ((OMC*x*x + C)*v[0] + (OMC*x*y - z*S)*v[1] + (OMC*z*x + y*S)*v[2],
            (OMC*x*y + z*S)*v[0] + (OMC*y*y + C)*v[1] + (OMC*y*z - x*S)*v[2],
            (OMC*z*x - y*S)*v[0] + (OMC*y*z + x*S)*v[1] + (OMC*z*z + C)*v[2])


def norm(v):
    l = math.sqrt(sum(c*c for c in v)); return tuple(c/l for c in v) if l > 1e-8 else (0, 0, 0)


def rotation(v):
    return (math.degrees(math.atan2(v[2], math.hypot(v[0], v[1]))), math.degrees(math.atan2(v[1], v[0])), 0.0)


def vector(r):
    p, y = math.radians(r[0]), math.radians(r[1]); return (math.cos(p)*math.cos(y), math.cos(p)*math.sin(y), math.sin(p))


def cone(d, h, seed):
    if h <= 0: return norm(d)
    nxt = frac_stream(seed); u, v = nxt(), nxt(); th = 2*math.pi*u; ph = math.acos(2*v - 1)
    half = math.sqrt(1.0 / ((math.cos(th)/h)**2 + (math.sin(th)/h)**2)); ph = math.fmod(ph, half)
    rt = rotation(d); dz = vector(rt); p, y, r = map(math.radians, rt); dy = (-math.sin(y), math.cos(y), 0.0)    # roll is 0
    res = rot_axis(d, math.degrees(ph), dy); res = rot_axis(res, math.degrees(th), dz)
    return norm(res)


class RandomStream(unittest.TestCase):
    def test_against_reference(self):
        with tempfile.TemporaryDirectory() as t:
            exe = os.path.join(t, "m")
            subprocess.check_call(["g++", "-std=c++20", "-w", "-I" + ROOT + "/cpp2mips/shim", "-o", exe, ROOT + "/cpp2mips/test_math.cpp", ROOT + "/cpp2mips/fname.cpp"])
            out = subprocess.check_output([exe], text=True).splitlines()
        got = {}
        for ln in out:
            k, *rest = ln.split(); got.setdefault(k, []).append([float(x) for x in rest])
        for row in got["frac"]:
            n = frac_stream(int(row[0])); exp = [n(), n(), n()]
            for g, e in zip(row[1:], exp): self.assertAlmostEqual(g, e, places=6)
        cases = [((1, 0, 0), 0.1, 12345), ((0.6, 0.3, 0.7), 0.2, 99), ((0, 0, 1), 0.05, -3), ((-0.2, 0.9, -0.4), 0.35, 2024), ((1, 0, 0), 0.0, 5)]
        for row, (d, h, s) in zip(got["cone"], cases):
            self.assertEqual(int(row[0]), s)
            for g, e in zip(row[1:], cone(d, h, s)): self.assertAlmostEqual(g, e, places=4)
        for g, e in zip(got["rot"][0], rot_axis((1, 2, 3), 40, (0, 0, 1))): self.assertAlmostEqual(g, e, places=5)
        for g, e in zip(got["rotation"][0], rotation((0.5, -0.3, 0.8))): self.assertAlmostEqual(g, e, places=3)

    def test_cone_stays_inside_the_half_angle(self):
        # property of the engine's sampling: Phi = fmod(acos(...), ellipse radius) < half angle
        for s in range(200):
            d = norm((0.3, -0.5, 0.8)); r = cone(d, 0.25, s)
            ang = math.acos(max(-1, min(1, sum(a*b for a, b in zip(d, r)))))
            self.assertLessEqual(ang, 0.25 + 1e-4)


if __name__ == "__main__":
    unittest.main()
