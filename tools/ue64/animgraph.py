"""Compiled Animation Blueprint -> tables for runtime/u64_anim.c.

An AnimBlueprintGeneratedClass keeps its pose graph as struct properties of the CDO (AnimNode_* in class-property order: LinkID = position in that
order), the state machines as BakedStateMachines (node indices there count from the other end: index i = position N-1-i), and the variable -> node
copies as a PropertyAccessLibrary (source variable path, destination "Node.Field" path). Transition rules and other exposed inputs are ordinary compiled
functions (already translated by codegen) that write the nodes' fields.
"""
import glob
import os

import animation
import props
import skeleton
import uasset

NODE_KIND = {"Root": 0, "Slot": 1, "StateMachine": 2, "StateResult": 3, "SequencePlayer": 4, "BlendListByBool": 5, "BlendListByEnum": 6,
             "LayeredBoneBlend": 7, "TransitionResult": 8, "BlendSpacePlayer": 9, "UseCachedPose": 10, "SaveCachedPose": 11, "ConvertLocalToComponentSpace": 12,
             "ConvertComponentToLocalSpace": 12, "Fabrik": 12, "RotationOffsetBlendSpace": 12}


def _load(f):
    p = uasset.parse_tables(f)
    return p, open(f, "rb").read()


def read_abp(path):
    """-> dict(name, nodes, machines, copies, functions) of an AnimBlueprint package."""
    p, d = _load(path)
    cls_e = next(e for e in p.exports if uasset.class_name(p, e.cls) == "AnimBlueprintGeneratedClass")
    cdo_e = next(e for e in p.exports if e.name.startswith("Default__"))
    cls, cdo = props.read_object(p, d, cls_e), props.read_object(p, d, cdo_e)
    raw = [(k, v) for k, v in cdo.items() if isinstance(v, tuple) and v[0] == "struct" and v[2] and v[2].startswith("AnimNode_")]
    nodes = []
    for k, v in raw:                                   # position in the CDO = LinkID
        kind = v[2][len("AnimNode_"):]
        f = v[1]
        link = lambda s: s[1]["LinkID"]
        n = {"name": k, "kind": NODE_KIND.get(kind, -1), "links": [], "times": [], "seq": "", "rate": 1.0, "enum_map": [], "machine": -1, "loop": 1, "slot": "", "bs": "", "filters": [], "cache": ""}
        if kind in ("Root",):
            n["links"] = [link(f["Result"])]
        elif kind == "Slot":
            n["links"] = [link(f["Source"])]
            n["slot"] = f.get("SlotName", ("name", ""))[1]
        elif kind == "StateResult":
            n["links"] = [link(f["Result"])]
        elif kind == "SequencePlayer":
            sq = f.get("Sequence")
            n["seq"] = sq[1] if isinstance(sq, tuple) and sq[0] == "obj" else ""
            n["rate"] = float(f.get("PlayRate", 1.0)); n["loop"] = int(bool(f.get("bLoopAnimation", True)))
        elif kind in ("BlendListByBool", "BlendListByEnum"):
            n["links"] = [link(x) for x in f["BlendPose"][1]]
            n["times"] = [float(x) for x in f["BlendTime"][1]]
            if kind == "BlendListByEnum":
                n["enum_map"] = list(f["EnumToPoseIndex"][1])
        elif kind == "LayeredBoneBlend":
            n["links"] = [link(f["BasePose"])] + [link(x) for x in f["BlendPoses"][1]]
            n["times"] = [float(x) for x in f["BlendWeights"][1]]
            if f["LayerSetup"][1]:
                n["filters"] = [(b[1]["BoneName"][1], int(b[1].get("BlendDepth", 0))) for b in f["LayerSetup"][1][0][1]["BranchFilters"][1]]
        elif kind == "BlendSpacePlayer":
            bs = f.get("BlendSpace"); n["bs"] = bs[1] if isinstance(bs, tuple) and bs[0] == "obj" else ""
            n["rate"] = float(f.get("PlayRate", 1.0))
        elif kind == "UseCachedPose":
            n["links"] = [link(f["LinkToCachingNode"])]
        elif kind == "SaveCachedPose":
            n["links"] = [link(f["Pose"])]
        elif kind in ("ConvertLocalToComponentSpace", "ConvertComponentToLocalSpace", "Fabrik", "RotationOffsetBlendSpace"):
            key = {"ConvertLocalToComponentSpace": "LocalPose", "ConvertComponentToLocalSpace": "ComponentPose", "Fabrik": "ComponentPose", "RotationOffsetBlendSpace": "BasePose"}[kind]
            if key in f and isinstance(f[key], tuple) and f[key][0] == "struct" and "LinkID" in f[key][1]:
                n["links"] = [link(f[key])]
        elif kind == "StateMachine":
            n["machine"] = int(f.get("StateMachineIndexInClass", 0))
        nodes.append(n)
    N = len(nodes)
    machines = []
    for m in cls["BakedStateMachines"][1]:
        m = m[1]
        states = []
        for s in m["States"][1]:
            s = s[1]
            states.append({"name": s["StateName"][1], "root": N - 1 - s["StateRootNodeIndex"],
                           "transitions": [(t[1]["TransitionIndex"], N - 1 - t[1]["CanTakeDelegateIndex"], bool(t[1]["bDesiredTransitionReturnValue"])) for t in s["Transitions"][1]]})
        trans = [{"prev": t[1]["PreviousState"], "next": t[1]["NextState"], "time": float(t[1]["CrossfadeDuration"])} for t in m["Transitions"][1]]
        machines.append({"name": m["MachineName"][1], "initial": m["InitialState"], "states": states, "transitions": trans})
    lib = cls.get("PropertyAccessLibrary", ("struct", {"PathSegments": ("array", []), "SrcPaths": ("array", []), "DestPaths": ("array", [])}))[1]
    segs = [s[1] for s in lib["PathSegments"][1]]
    copies = []
    for sp, dp in zip(lib["SrcPaths"][1], lib["DestPaths"][1]):
        sp, dp = sp[1], dp[1]
        var = segs[sp["PathSegmentStartIndex"]]["Name"][1]
        names = [segs[dp["PathSegmentStartIndex"] + i]["Name"][1] for i in range(dp["PathSegmentCount"])]
        copies.append((var, names[0], names[1]))
    funcs = [x[1]["BoundFunction"][1] for x in cls["EvaluateGraphExposedInputs"][1] if x[1]["BoundFunction"][1] != "None"]
    return {"name": os.path.splitext(os.path.basename(path))[0], "nodes": nodes, "machines": machines, "copies": copies, "functions": funcs}


def find_sequence(content, name):
    for f in glob.glob(content + "/**/%s.uasset" % name, recursive=True):
        p, d = _load(f)
        for e in p.exports:
            if uasset.class_name(p, e.cls) == "AnimSequence":
                return animation.read_sequence(p, d, e)
    return None


def find_montages(content, skeleton_name):
    """[(name, slot, blend_in, blend_out, length, [(sequence, start, end, rate)])] of the montages made for a skeleton."""
    out = []
    for f in glob.glob(content + "/**/*.uasset", recursive=True):
        try:
            p, d = _load(f)
        except Exception:
            continue
        for e in p.exports:
            if uasset.class_name(p, e.cls) != "AnimMontage":
                continue
            o = props.read_object(p, d, e)
            sk = o.get("Skeleton")
            if not (isinstance(sk, tuple) and sk[0] == "obj" and sk[1] == skeleton_name):
                continue
            tr = o.get("SlotAnimTracks", ("array", []))[1]
            if not tr:
                continue
            segs = []
            for sg in tr[0][1]["AnimTrack"][1]["AnimSegments"][1]:
                sg = sg[1]
                ref = sg.get("AnimReference")
                if isinstance(ref, tuple) and ref[0] == "obj":
                    segs.append((ref[1], float(sg.get("AnimStartTime", 0.0)), float(sg.get("AnimEndTime", 0.0)), float(sg.get("AnimPlayRate", 1.0))))
            out.append((e.name, tr[0][1]["SlotName"][1], float(o.get("BlendIn", ("struct", {}))[1].get("BlendTime", 0.25)), float(o.get("BlendOut", ("struct", {}))[1].get("BlendTime", 0.25)), float(o.get("SequenceLength", 0.0)), segs))
    return out


def chain(bones, bone):
    idx = {b[0]: i for i, b in enumerate(bones)}
    out, i = [], idx[bone]
    while i >= 0:
        out.append(i)
        i = bones[i][1]
    return out[::-1]


def _f(v):
    return repr(float(v)) + "f"


def _cs(s):
    return '"' + s.replace("\\", "\\\\").replace('"', '\\"') + '"'


def emit_c(content, abp_name, mesh_name, target_bones):
    """C tables for one Animation Blueprint driving one skeletal mesh, restricted to the bone chain up to socket_bone (what sockets / aiming need)."""
    path = glob.glob(content + "/**/%s.uasset" % abp_name, recursive=True)[0]
    abp = read_abp(path)
    mp = glob.glob(content + "/**/%s.uasset" % mesh_name, recursive=True)[0]
    p, d = _load(mp)
    me = next(e for e in p.exports if uasset.class_name(p, e.cls) == "SkeletalMesh")
    bones = skeleton.ref_skeleton(p, d, me)
    if isinstance(target_bones, str):
        target_bones = [target_bones]
    keep = sorted({i for b in target_bones for i in chain(bones, b)})                  # bone chains to every target, parents first
    ch = keep
    names = [bones[i][0] for i in ch]
    sub = {bi: k for k, bi in enumerate(ch)}
    out = ['#include "u64_anim.h"']
    # reference pose of the chain
    rows = ", ".join("{%s, %d, {%s}, {%s}}" % (_cs(bones[i][0]), sub[bones[i][1]] if bones[i][1] >= 0 else -1, ", ".join(_f(x) for x in bones[i][2]), ", ".join(_f(x) for x in bones[i][3])) for i in ch)
    pfx = "an_%s_" % abp_name
    out.append("static const u64_an_bone %sbones[] = {%s};" % (pfx, rows))
    montages = find_montages(content, abp_skeleton(path))
    bspaces = [read_blendspace(content, n) for n in sorted({n["bs"] for n in abp["nodes"] if n["bs"]})]
    seqs = sorted({n["seq"] for n in abp["nodes"] if n["seq"]} | {sg[0] for m in montages for sg in m[5]} | {a for b in bspaces if b for a in b["samples_anim"]})
    srows = []
    for si, sname in enumerate(seqs):
        sq = find_sequence(content, sname)
        if not sq:
            srows.append("{%s, 0, 0, 0}" % _cs(sname)); continue
        trs = []
        for ti, bn in enumerate(names):
            pk, rk = sq["tracks"].get(bn, ([], []))
            out.append("static const float %ss%d_p%d[] = {%s};" % (pfx, si, ti, ", ".join(_f(x) for k in pk for x in k) or "0"))
            out.append("static const float %ss%d_r%d[] = {%s};" % (pfx, si, ti, ", ".join(_f(x) for k in rk for x in k) or "0"))
            trs.append("{%d, %d, %ss%d_p%d, %ss%d_r%d}" % (len(pk), len(rk), pfx, si, ti, pfx, si, ti))
        out.append("static const u64_an_track %ss%d_t[] = {%s};" % (pfx, si, ", ".join(trs)))
        srows.append("{%s, %s, %d, %ss%d_t}" % (_cs(sname), _f(sq["length"]), sq["frames"], pfx, si))
    out.append("static const u64_an_seq %sseqs[] = {%s};" % (pfx, ", ".join(srows)))
    seqidx = {s: i for i, s in enumerate(seqs)}
    nrows = []
    for n in abp["nodes"]:
        links = (n["links"] + [0] * 8)[:8]
        emap = (n["enum_map"] + [0] * 8)[:8]
        times = (n["times"] + [0.0] * 8)[:8]
        bsi = [b["name"] for b in bspaces if b].index(n["bs"]) if n["bs"] and n["bs"] in [b["name"] for b in bspaces if b] else -1
        fl = [(names.index(b), dep) for b, dep in n["filters"] if b in names][:4]
        fl_idx = ([str(a) for a, _ in fl] + ["0"] * 4)[:4]
        fl_dep = ([str(b) for _, b in fl] + ["0"] * 4)[:4]
        nrows.append("{%s, %d, %d, {%s}, %d, %s, %d, %d, {%s}, {%s}, %s, %d, %d, {%s}, {%s}}" % (
            _cs(n["name"]), n["kind"], len(n["links"]), ", ".join(map(str, links)), seqidx.get(n["seq"], -1), _f(n["rate"]), n["loop"], n["machine"],
            ", ".join(map(str, emap)), ", ".join(_f(x) for x in times), _cs(n["slot"]), bsi, len(fl), ", ".join(fl_idx), ", ".join(fl_dep)))
    brow = []
    for bi, b in enumerate([x for x in bspaces if x]):
        out.append("static const u64_an_bsample %sb%d_s[] = {%s};" % (pfx, bi, ", ".join("{%d, %s, %s}" % (seqidx[a], _f(x), _f(y)) for a, x, y in zip(b["samples_anim"], b["sx"], b["sy"]))))
        out.append("static const u64_an_bgrid %sb%d_g[] = {%s};" % (pfx, bi, ", ".join("{{%d, %d, %d}, {%s, %s, %s}}" % (g[0], g[1], g[2], _f(g[3]), _f(g[4]), _f(g[5])) for g in b["grid"])))
        brow.append("{%s, {%s, %s}, {%s, %s}, {%d, %d}, %d, %sb%d_s, %sb%d_g}" % (_cs(b["name"]), _f(b["min"][0]), _f(b["min"][1]), _f(b["max"][0]), _f(b["max"][1]), b["div"][0], b["div"][1], len(b["samples_anim"]), pfx, bi, pfx, bi))
    out.append("static const u64_an_bspace %sbspaces[] = {%s};" % (pfx, ", ".join(brow) or "{0}"))
    mrows2 = []
    for m in montages:
        sg = m[5][0] if m[5] else None
        mrows2.append("{%s, %s, %s, %s, %s, %d, %s, %s, %s}" % (_cs(m[0]), _cs(m[1]), _f(m[2]), _f(m[3]), _f(m[4]), seqidx[sg[0]] if sg else -1, _f(sg[1] if sg else 0), _f(sg[2] if sg else 0), _f(sg[3] if sg else 1)))
    out.append("static const u64_an_montage %smontages[] = {%s};" % (pfx, ", ".join(mrows2) or "{0}"))
    out.append("static const u64_an_node %snodes[] = {%s};" % (pfx, ",\n  ".join(nrows)))
    mrows = []
    for mi, m in enumerate(abp["machines"]):
        srr = []
        for si, s in enumerate(m["states"]):
            tr = ", ".join("{%d, %d, %d}" % (a, b, int(c)) for a, b, c in s["transitions"])
            srr.append("{%s, %d, %d, {%s}}" % (_cs(s["name"]), s["root"], len(s["transitions"]), tr))
        out.append("static const u64_an_state %sm%d_states[] = {%s};" % (pfx, mi, ", ".join(srr)))
        trr = ", ".join("{%d, %d, %s}" % (t["prev"], t["next"], _f(t["time"])) for t in m["transitions"])
        out.append("static const u64_an_trans %sm%d_trans[] = {%s};" % (pfx, mi, trr))
        mrows.append("{%s, %d, %d, %sm%d_states, %d, %sm%d_trans}" % (_cs(m["name"]), m["initial"], len(m["states"]), pfx, mi, len(m["transitions"]), pfx, mi))
    out.append("static const u64_an_machine %smachines[] = {%s};" % (pfx, ", ".join(mrows)))
    crow = ", ".join("{%s, %s, %s}" % (_cs(a), _cs(b), _cs(c)) for a, b, c in abp["copies"])
    frow = ", ".join(_cs(x) for x in abp["functions"])
    out.append("static const u64_an_copy %scopies[] = {%s};" % (pfx, crow))
    out.append("static const char *const %sfuncs[] = {%s};" % (pfx, frow))
    out.append("const u64_an_graph u64_an_%s = {%s, %s, %d, %snodes, %d, %smachines, %d, %scopies, %d, %sfuncs, %d, %sbones, %d, %sseqs, %d, %smontages, %d, %sbspaces};" % (
        abp_name, _cs(abp_name), _cs(mesh_name), len(abp["nodes"]), pfx, len(abp["machines"]), pfx, len(abp["copies"]), pfx, len(abp["functions"]), pfx, len(ch), pfx, len(seqs), pfx, len(montages), pfx, len([x for x in bspaces if x]), pfx))
    return "\n".join(out) + "\n", abp


def emit_sockets(content):
    """Sockets of every skeletal mesh (in the mesh's space, ref pose of the socket's bone chain) and the skeletal mesh component -> mesh asset of Blueprints."""
    rows, meshes = [], []
    for f in glob.glob(content + "/**/*.uasset", recursive=True):
        try:
            p, d = _load(f)
        except Exception:
            continue
        cn = [uasset.class_name(p, e.cls) for e in p.exports]
        if "SkeletalMeshSocket" in cn and "SkeletalMesh" in cn:
            me = p.exports[cn.index("SkeletalMesh")]
            try:
                bones = skeleton.ref_skeleton(p, d, me)
                cs = skeleton.component_space(bones)
                idx = {b[0]: i for i, b in enumerate(bones)}
                for sname, bone, loc, rot, sc in skeleton.sockets(p, d):
                    if bone in idx:
                        bt, bq = cs[idx[bone]]
                        t = skeleton.quat_rot(bq, loc)
                        q = skeleton.quat_mul(bq, skeleton.euler_to_quat(*rot))
                        rows.append("{%s, %s, {%s}, {%s}}" % (_cs(me.name), _cs(sname), ", ".join(_f(bt[i] + t[i]) for i in range(3)), ", ".join(_f(x) for x in q)))
            except Exception:
                pass
        if "BlueprintGeneratedClass" in cn and "SkeletalMeshComponent" in cn:
            owner = p.exports[cn.index("BlueprintGeneratedClass")].name.removesuffix("_C")
            for e in p.exports:
                if uasset.class_name(p, e.cls) == "SkeletalMeshComponent" and e.name.startswith("WeaponMesh"):
                    try:
                        m = props.read_object(p, d, e).get("SkeletalMesh")
                        if isinstance(m, tuple) and m[0] == "obj":
                            meshes.append("{%s, %s, %s}" % (_cs(owner), _cs(e.name), _cs(m[1])))
                    except Exception:
                        pass
    return ("const u64_mesh_socket u64_mesh_sockets[] = {%s%s{0}};\nconst u64_weapon_mesh u64_weapon_meshes[] = {%s%s{0}};\n"
            % (", ".join(rows), ", " if rows else "", ", ".join(meshes), ", " if meshes else ""))


def abp_skeleton(path):
    p, d = _load(path)
    cls = props.read_object(p, d, next(e for e in p.exports if uasset.class_name(p, e.cls) == "AnimBlueprintGeneratedClass"))
    t = cls.get("TargetSkeleton")
    return t[1] if isinstance(t, tuple) and t[0] == "obj" else ""


def read_blendspace(content, name):
    """2D blend space (BlendSpace / AimOffsetBlendSpace): sample animations and positions, axis ranges, precomputed grid (index = xi * (divisions_y + 1) + yi)."""
    for f in glob.glob(content + "/**/%s.uasset" % name, recursive=True):
        p, d = _load(f)
        for e in p.exports:
            if "BlendSpace" in uasset.class_name(p, e.cls):
                o = props.read_object(p, d, e)
                sd = o["SampleData"][1]
                anims, sx, sy = [], [], []
                for smp in sd:
                    smp = smp[1]
                    a = smp["Animation"]
                    anims.append(a[1] if isinstance(a, tuple) and a[0] == "obj" else "")
                    v = smp["SampleValue"][1]
                    sx.append(float(v[0])); sy.append(float(v[1]))
                bx, by = o["BlendParameters"][1], o["BlendParameters[1]"][1]
                grid = []
                for g in o["GridSamples"][1]:
                    g = g[1]
                    grid.append((g.get("Indices", 0), g.get("Indices[1]", 0), g.get("Indices[2]", 0), g.get("Weights", 0.0), g.get("Weights[1]", 0.0), g.get("Weights[2]", 0.0)))
                dx, dy = int(bx.get("GridNum", 4)), int(by.get("GridNum", 4))
                if len(grid) != (dx + 1) * (dy + 1):
                    n = int(round(len(grid) ** 0.5)) - 1
                    dx = dy = n
                return {"name": name, "samples_anim": anims, "sx": sx, "sy": sy, "min": (bx.get("Min", 0.0), by.get("Min", 0.0)), "max": (bx.get("Max", 100.0), by.get("Max", 100.0)), "div": (dx, dy), "grid": grid}
    return None
