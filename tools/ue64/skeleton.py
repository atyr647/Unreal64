"""Skeletal data from cooked/imported assets: reference pose (bone hierarchy + local transforms) and sockets.

Sockets are tagged USkeletalMeshSocket exports (SocketName/BoneName/RelativeLocation/Rotation/Scale), so props.py reads them.
The reference skeleton lives in the native (non-tagged) part of the USkeletalMesh export, after the tag stream and a few
version-dependent header fields. Rather than hard-code every engine version's header, `ref_skeleton` scans for the one
structure that is unambiguous and validates it hard: TArray<FMeshBoneInfo> (FName, ParentIndex, ExportName FString)
followed by TArray<FTransform> with the same count, unit quaternions and positive scales. A misparse is rejected, not guessed.
"""
import math
import struct

import props
import uasset


def _bones_at(d, names, q, end):
    """Try to read bone infos + pose array at offset q. Returns (bones, poses, next) or None."""
    try:
        n = struct.unpack_from("<i", d, q)[0]
        if not 1 <= n <= 1024:
            return None
        pos = q + 4
        bones = []
        for i in range(n):
            ni, num, par, ln = struct.unpack_from("<iiii", d, pos)
            if not (0 <= ni < len(names)) or not (0 <= num < 1000) or not (-1 <= par < i) or not (i == 0) == (par == -1):
                return None
            if not 1 <= ln <= 256 or pos + 16 + ln > end:
                return None
            raw = d[pos + 16:pos + 16 + ln]
            if raw[-1] != 0 or not all(32 <= c < 127 for c in raw[:-1]):
                return None
            bones.append((names[ni], par))
            pos += 16 + ln
        cnt = struct.unpack_from("<i", d, pos)[0]
        pos += 4
        if cnt != n:
            return None
        for stride, fmt in ((40, "<10f"), (80, "<10d")):
            if pos + stride * n > end:
                continue
            poses, ok = [], True
            for i in range(n):
                v = struct.unpack_from(fmt, d, pos + i * stride)
                if abs(math.sqrt(sum(x * x for x in v[:4])) - 1.0) > 1e-2 or min(v[7:10]) <= 0 or any(abs(x) > 1e7 or x != x for x in v):
                    ok = False
                    break
                poses.append((tuple(v[4:7]), tuple(v[:4]), tuple(v[7:10])))     # (translation, quat xyzw, scale)
            if ok:
                return bones, poses, pos + stride * n
    except struct.error:
        return None
    return None


def ref_skeleton(p, d, e, window=1 << 17):
    """-> [(name, parent, (tx,ty,tz), (qx,qy,qz,qw), (sx,sy,sz))] for a SkeletalMesh export, or None."""
    r = props.R(d)
    r.p = e.offset
    props.read_stream(r, props.Ctx(p, d), e.offset + e.size)
    end = min(e.offset + e.size, r.p + window)
    for q in range(r.p, end - 8):
        if d[q + 3] != 0 or d[q + 2] != 0 or d[q + 1] != 0:                  # count < 256 for the quick reject (bones <= 255)
            continue
        got = _bones_at(d, p.names, q, e.offset + e.size)
        if got:
            bones, poses, _ = got
            return [(b[0], b[1]) + poses[i] for i, b in enumerate(bones)]
    return None


def sockets(p, d):
    """All USkeletalMeshSocket exports of a package -> [(name, bone, loc, rot(pitch,yaw,roll), scale)]."""
    out = []
    for e in p.exports:
        if uasset.class_name(p, e.cls) != "SkeletalMeshSocket":
            continue
        o = props.read_object(p, d, e)
        g = lambda k, dflt: o[k][1] if k in o and o[k][0] == "vec" else dflt
        out.append((o["SocketName"][1], o["BoneName"][1], g("RelativeLocation", (0, 0, 0)), g("RelativeRotation", (0, 0, 0)), g("RelativeScale", (1, 1, 1))))
    return out


def quat_mul(a, b):
    ax, ay, az, aw = a
    bx, by, bz, bw = b
    return (aw * bx + ax * bw + ay * bz - az * by, aw * by - ax * bz + ay * bw + az * bx,
            aw * bz + ax * by - ay * bx + az * bw, aw * bw - ax * bx - ay * by - az * bz)


def quat_rot(q, v):
    x, y, z, w = q
    t = (2 * (y * v[2] - z * v[1]), 2 * (z * v[0] - x * v[2]), 2 * (x * v[1] - y * v[0]))
    return (v[0] + w * t[0] + (y * t[2] - z * t[1]), v[1] + w * t[1] + (z * t[0] - x * t[2]), v[2] + w * t[2] + (x * t[1] - y * t[0]))


def euler_to_quat(pitch, yaw, roll):
    """UE FRotator (degrees) -> quaternion xyzw."""
    r = [math.radians(a) / 2 for a in (pitch, yaw, roll)]
    sp, sy, sr = (math.sin(a) for a in r)
    cp, cy, cr = (math.cos(a) for a in r)
    return (cr * sp * sy - sr * cp * cy, -cr * sp * cy - sr * cp * sy, cr * cp * sy - sr * sp * cy, cr * cp * cy + sr * sp * sy)


def component_space(bones):
    """Compose local reference transforms down the hierarchy -> [(translation, quat)] in component space."""
    out = []
    for name, par, t, q, s in bones:
        if par < 0:
            out.append((t, q))
        else:
            pt, pq = out[par]
            rt = quat_rot(pq, (t[0] * 1, t[1], t[2]))
            out.append(((pt[0] + rt[0], pt[1] + rt[1], pt[2] + rt[2]), quat_mul(pq, q)))
    return out
