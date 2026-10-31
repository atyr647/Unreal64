"""AnimSequence raw tracks (editor-saved assets keep RawAnimationData next to the compressed data) and pose evaluation on a reference skeleton.

After the tagged properties of an AnimSequence export: strip flags (4 bytes), the 16 byte raw data GUID, 2 bytes, an int32 track count, then per track
three bulk arrays (int32 element size, int32 count, data): position keys (vec3), rotation keys (quat xyzw), scale keys (vec3). One key means a constant track.
"""
import math
import struct

import props
import skeleton
import uasset


def read_sequence(p, d, e):
    """-> dict(name, length, frames, tracks={bone: (pos[(x,y,z)], rot[(x,y,z,w)])}) for an AnimSequence export, or None if the raw data is not there."""
    o = props.read_object(p, d, e)
    names = [n[1] for n in o.get("AnimationTrackNames", ("array", []))[1]]
    r = props.R(d)
    r.p = e.offset
    props.read_stream(r, props.Ctx(p, d), e.offset + e.size)
    t = d[r.p:e.offset + e.size]
    pos = 26
    try:
        n = struct.unpack_from("<i", t, 22)[0]
        if n != len(names):
            return None

        def bulk(w):
            nonlocal pos
            es, c = struct.unpack_from("<ii", t, pos)
            pos += 8
            if es != 4 * w or c < 0 or pos + es * c > len(t):
                raise ValueError("bad bulk array")
            arr = [struct.unpack_from("<%df" % w, t, pos + i * es) for i in range(c)]
            pos += es * c
            return arr

        tracks = {}
        for i in range(n):
            pk, rk, sk = bulk(3), bulk(4), bulk(3)
            tracks[names[i]] = (pk, rk)
    except (ValueError, struct.error):
        return None
    return {"name": e.name, "length": float(o.get("SequenceLength", 0.0)), "frames": int(o.get("NumFrames", 1)), "tracks": tracks}


def _slerp(a, b, t):
    dot = sum(x * y for x, y in zip(a, b))
    if dot < 0:
        b, dot = tuple(-x for x in b), -dot
    if dot > 0.9995:
        q = tuple(x + (y - x) * t for x, y in zip(a, b))
    else:
        th = math.acos(dot)
        s = math.sin(th)
        wa, wb = math.sin((1 - t) * th) / s, math.sin(t * th) / s
        q = tuple(wa * x + wb * y for x, y in zip(a, b))
    l = math.sqrt(sum(x * x for x in q)) or 1.0
    return tuple(x / l for x in q)


def sample_track(keys, frames, length, time, lerp):
    """Key value at `time` (seconds); one key = constant; keys are evenly spaced over `frames` frames."""
    if len(keys) == 1:
        return keys[0]
    pos = 0.0 if length <= 0 else min(max(time / length, 0.0), 1.0) * (len(keys) - 1)
    i = min(int(pos), len(keys) - 2)
    return lerp(keys[i], keys[i + 1], pos - i)


def pose(bones, seq, time):
    """Local bone transforms of `bones` (skeleton.ref_skeleton rows) with the sequence's tracks applied at `time`:
    -> [(name, parent, translation, quat, scale)]. Bones without a track keep the reference pose."""
    out = []
    for name, par, t, q, s in bones:
        tr = seq["tracks"].get(name)
        if tr:
            lerp3 = lambda a, b, f: tuple(x + (y - x) * f for x, y in zip(a, b))
            if tr[0]:
                t = sample_track(tr[0], seq["frames"], seq["length"], time, lerp3)
            if tr[1]:
                q = sample_track(tr[1], seq["frames"], seq["length"], time, _slerp)
        out.append((name, par, t, q, s))
    return out


def socket_transform(bones_pose, socket_bone, rel_loc, rel_rot):
    """Mesh-space transform (translation, quat) of a socket on a bone for a posed skeleton."""
    cs = skeleton.component_space(bones_pose)
    index = {b[0]: i for i, b in enumerate(bones_pose)}
    bt, bq = cs[index[socket_bone]]
    t = skeleton.quat_rot(bq, rel_loc)
    return (bt[0] + t[0], bt[1] + t[1], bt[2] + t[2]), skeleton.quat_mul(bq, skeleton.euler_to_quat(*rel_rot))
