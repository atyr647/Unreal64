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
             "LayeredBoneBlend": 7, "TransitionResult": 8}


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
        n = {"name": k, "kind": NODE_KIND.get(kind, -1), "links": [], "times": [], "seq": "", "rate": 1.0, "enum_map": [], "machine": -1, "loop": 1}
        if kind in ("Root",):
            n["links"] = [link(f["Result"])]
        elif kind == "Slot":
            n["links"] = [link(f["Source"])]
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
    lib = cls["PropertyAccessLibrary"][1]
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


def emit_c(content, abp_name, mesh_name, socket_bone):
    """C tables for one Animation Blueprint driving one skeletal mesh, restricted to the bone chain up to socket_bone (what sockets / aiming need)."""
    path = glob.glob(content + "/**/%s.uasset" % abp_name, recursive=True)[0]
    abp = read_abp(path)
    mp = glob.glob(content + "/**/%s.uasset" % mesh_name, recursive=True)[0]
    p, d = _load(mp)
    me = next(e for e in p.exports if uasset.class_name(p, e.cls) == "SkeletalMesh")
    bones = skeleton.ref_skeleton(p, d, me)
    ch = chain(bones, socket_bone)
    names = [bones[i][0] for i in ch]
    out = ['#include "u64_anim.h"']
    # reference pose of the chain
    rows = ", ".join("{%s, %d, {%s}, {%s}}" % (_cs(bones[i][0]), k - 1 if k else -1, ", ".join(_f(x) for x in bones[i][2]), ", ".join(_f(x) for x in bones[i][3])) for k, i in enumerate(ch))
    pfx = "an_%s_" % abp_name
    out.append("static const u64_an_bone %sbones[] = {%s};" % (pfx, rows))
    seqs = sorted({n["seq"] for n in abp["nodes"] if n["seq"]})
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
        nrows.append("{%s, %d, %d, {%s}, %d, %s, %d, %d, {%s}, {%s}}" % (
            _cs(n["name"]), n["kind"], len(n["links"]), ", ".join(map(str, links)), seqidx.get(n["seq"], -1), _f(n["rate"]), n["loop"], n["machine"],
            ", ".join(map(str, emap)), ", ".join(_f(x) for x in times)))
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
    out.append("const u64_an_graph u64_an_%s = {%s, %s, %d, %snodes, %d, %smachines, %d, %scopies, %d, %sfuncs, %d, %sbones, %d, %sseqs};" % (
        abp_name, _cs(abp_name), _cs(mesh_name), len(abp["nodes"]), pfx, len(abp["machines"]), pfx, len(abp["copies"]), pfx, len(abp["functions"]), pfx, len(ch), pfx, len(seqs), pfx))
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
