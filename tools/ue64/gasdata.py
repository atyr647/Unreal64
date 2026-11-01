"""Extract Gameplay Ability System data from assets: GameplayEffect definitions, montage lengths, collision shapes.

Unreal only saves properties that differ from their defaults, so every field below is read with the engine's default
(e.g. ModifierOp = Additive, magnitude calculation = ScalableFloat, attribute capture source = Source).
"""
from __future__ import annotations

import os
import props
import uasset


def enum_tail(v, default=None):
    if isinstance(v, tuple) and v[0] == "enum":
        return v[1].split("::")[-1]
    return default


def sf(v, default=0.0):
    """FScalableFloat -> value (the curve table is not used by this project; reported if it is)."""
    if isinstance(v, tuple) and v[0] == "struct":
        d = v[1]
        curve = d.get("Curve")
        if curve and isinstance(curve, tuple) and curve[0] == "struct" and curve[1].get("CurveTable", ("obj", "None"))[1] != "None":
            raise ValueError("scalable float uses a curve table")
        return float(d.get("Value", default))
    return default


def attr_name(v):
    if isinstance(v, tuple) and v[0] == "struct":
        return v[1].get("AttributeName") or (v[1].get("Attribute") or ("", ""))[1]
    return ""


def tags_of(v):
    return list(v[1]) if isinstance(v, tuple) and v[0] == "tags" else []


def tag_req(v):
    if isinstance(v, tuple) and v[0] == "struct":
        d = v[1]
        return tags_of(d.get("RequireTags")), tags_of(d.get("IgnoreTags"))
    return [], []


def parse_mod(m):
    d = m[1]
    mm = d.get("ModifierMagnitude", ("struct", {}))[1] if d.get("ModifierMagnitude") else {}
    calc = enum_tail(mm.get("MagnitudeCalculationType"), "ScalableFloat")
    out = dict(attr=attr_name(d.get("Attribute")), op=enum_tail(d.get("ModifierOp"), "Additive"), calc=calc, value=0.0,
               backing="", backing_src="Source", coef=1.0, pre=0.0, post=0.0, sbc_tag="")
    if calc == "ScalableFloat":
        out["value"] = sf(mm.get("ScalableFloatMagnitude"))
    elif calc == "AttributeBased":
        ab = mm.get("AttributeBasedMagnitude", ("struct", {}))[1]
        cap = ab.get("BackingAttribute", ("struct", {}))[1] if ab.get("BackingAttribute") else {}
        out["backing"] = attr_name(cap.get("AttributeToCapture"))
        out["backing_src"] = enum_tail(cap.get("AttributeSource"), "Source")
        out["coef"] = sf(ab.get("Coefficient"), 1.0) if "Coefficient" in ab else 1.0
        out["pre"] = sf(ab.get("PreMultiplyAdditiveValue"))
        out["post"] = sf(ab.get("PostMultiplyAdditiveValue"))
    elif calc == "SetByCaller":
        sbc = mm.get("SetByCallerMagnitude", ("struct", {}))[1] if mm.get("SetByCallerMagnitude") else {}
        t = sbc.get("DataTag")
        out["sbc_tag"] = t[1] if isinstance(t, tuple) and t[0] == "tag" else (t[1].get("TagName", ("name", ""))[1] if isinstance(t, tuple) and t[0] == "struct" else "")
    else:
        out["unsupported"] = calc          # CustomCalculationClass: reported, not guessed
    return out


def ge_definition(p, data, export):
    o = props.read_object(p, data, export)
    ge = dict(name=export.name[len("Default__"):-2], duration=enum_tail(o.get("DurationPolicy"), "Instant"), duration_value=0.0,
              period=0.0, mods=[], executions=[], granted_tags=[], asset_tags=[], remove_tags=[], app_req=([], []),
              ongoing_req=([], []), stack_type="None", stack_limit=0, cues=[], unsupported=[], conditional=[])
    dm = o.get("DurationMagnitude")
    if dm and dm[0] == "struct":
        calc = enum_tail(dm[1].get("MagnitudeCalculationType"), "ScalableFloat")
        if calc == "ScalableFloat":
            ge["duration_value"] = sf(dm[1].get("ScalableFloatMagnitude"))
        else:
            ge["unsupported"].append(f"duration {calc}")
    if "Period" in o:
        ge["period"] = sf(o["Period"])
    for m in (o.get("Modifiers") or ("array", []))[1]:
        pm = parse_mod(m)
        if "unsupported" in pm:
            ge["unsupported"].append(pm["unsupported"])
        ge["mods"].append(pm)
    for ex in (o.get("Executions") or ("array", []))[1]:
        cc = ex[1].get("CalculationClass")
        ge["executions"].append(cc[1].removesuffix("_C") if isinstance(cc, tuple) and cc[0] == "obj" else "")
        cm = ex[1].get("CalculationModifiers")
        if cm and not (cm[0] == "array" and not cm[1]):
            ge["unsupported"].append("execution calculation modifiers")
    own = o.get("InheritableOwnedTagsContainer")
    if own and own[0] == "struct":
        ge["granted_tags"] = tags_of(own[1].get("CombinedTags")) or tags_of(own[1].get("Added"))
    at = o.get("InheritableGameplayEffectTags")
    if at and at[0] == "struct":
        ge["asset_tags"] = tags_of(at[1].get("CombinedTags")) or tags_of(at[1].get("Added"))
    rt = o.get("RemoveGameplayEffectsWithTags")
    if rt and rt[0] == "struct":
        ge["remove_tags"] = tags_of(rt[1].get("CombinedTags")) or tags_of(rt[1].get("Added"))
    ge["app_req"] = tag_req(o.get("ApplicationTagRequirements"))
    ge["ongoing_req"] = tag_req(o.get("OngoingTagRequirements"))
    ge["stack_type"] = enum_tail(o.get("StackingType"), "None")
    ge["stack_limit"] = int(o.get("StackLimitCount", 0)) if isinstance(o.get("StackLimitCount", 0), int) else 0
    for ce in (o.get("ConditionalGameplayEffects") or ("array", []))[1]:       # FConditionalGameplayEffect
        d = ce[1]
        cls = d.get("EffectClass")
        if isinstance(cls, tuple) and cls[0] == "obj":
            ge["conditional"].append((cls[1].removesuffix("_C"), tags_of(d.get("RequiredSourceTags"))))
    for k in o:
        if k in ("OverflowEffects", "Components"):
            ge["unsupported"].append(k)
    return ge


COMPONENTS = []      # static-mesh components of Blueprints: dicts (class, comp, mesh, parent, transform, collision responses)
CHANNELS = {}        # project collision channel -> default response (Config/DefaultEngine.ini)
PROFILES = {}        # project collision profile -> (object type, [(channel, response)])
PHYS = {}            # physics asset name -> [(bone, kind, center, rotation(pitch,yaw,roll), dims)]; kind 0 box (full extents), 1 sphere (radius), 2 sphyl (radius, length)
SKELMESHES = {}      # skeletal mesh name -> bodies in mesh space [(bone, kind, t(3), r(3x3 row-major), a, b, c)]: box half extents, sphere radius, sphyl radius + half cylinder length
WORLDMESHES = []     # static meshes placed in the map (StaticMeshActor root components): (mesh, loc, rot, scale)
SKELCOMPS = []       # skeletal mesh components (the character Mesh) of Blueprints: dicts (class, comp, mesh, transform, collision responses)
MESHES = {}          # static mesh name -> [hull planes]; hull = [(nx, ny, nz, d)] with n.x <= d inside

TIMELINES = []      # [(class, template dict, [(property name, [keys])])] filled by collect(), emitted by emit_gas_data_c


def hull_planes(points, eps=1e-3):
    """Planes (nx, ny, nz, d) of the convex hull of a point set, n.x <= d inside: every plane through three points that has all
    points on one side. Chunked over the triples; point sets are deduplicated first (hulls here have tens to ~200 points)."""
    import itertools
    import numpy as np
    P = np.unique(np.round(np.array(points, dtype=np.float64), 4), axis=0)
    n = len(P)
    if n < 4:
        return []
    planes, seen = [], set()
    tri = itertools.combinations(range(n), 3)
    while True:
        chunk = np.array(list(itertools.islice(tri, 20000)))
        if len(chunk) == 0:
            break
        A, B, C = P[chunk[:, 0]], P[chunk[:, 1]], P[chunk[:, 2]]
        N = np.cross(B - A, C - A)
        L = np.linalg.norm(N, axis=1)
        ok = L > 1e-9
        N, A, L = N[ok] / L[ok, None], A[ok], L[ok]
        D = np.einsum("ij,ij->i", N, A)
        side = P @ N.T - D[None, :]
        pos, neg = (side > eps).any(axis=0), (side < -eps).any(axis=0)
        for i in np.where(~(pos & neg))[0]:
            n_, d_ = (-N[i], -D[i]) if pos[i] else (N[i], D[i])
            key = tuple(np.round(np.append(n_, d_), 2))
            if key not in seen:
                seen.add(key)
                planes.append((float(n_[0]), float(n_[1]), float(n_[2]), float(d_)))
    return planes


def _elem_transform(p, tr):
    """FTransform of a convex element as (quat xyzw, translation, scale). Older saves keep it as a tagged-property blob that the
    property reader leaves raw: Rotation (Quat), Translation, Scale3D, each 8-byte FName header + 49 bytes of tag + payload."""
    import struct as _st
    q, t, sc = (0.0, 0.0, 0.0, 1.0), (0.0, 0.0, 0.0), (1.0, 1.0, 1.0)
    if tr is None:
        return q, t, sc
    if tr[0] == "struct":
        d = tr[1]
        g = lambda k, dflt: tuple(d[k][1]) if k in d and isinstance(d[k], tuple) and d[k][0] in ("vec", "quat") else dflt
        return g("Rotation", q), g("Translation", t), g("Scale3D", sc)
    if tr[0] != "raw":
        return q, t, sc
    b, pos = tr[1], 0
    while pos + 8 <= len(b):
        ni = _st.unpack_from("<i", b, pos)[0]
        name = p.names[ni] if 0 <= ni < len(p.names) else "None"
        name = name if isinstance(name, str) else str(name[0] if isinstance(name, (tuple, list)) else getattr(name, "name", name))
        if name == "None" or pos + 49 > len(b):
            break
        size = _st.unpack_from("<i", b, pos + 16)[0]
        pay = pos + 49
        if name == "Rotation" and size == 16:
            q = _st.unpack_from("<4f", b, pay)
        elif name == "Translation" and size == 12:
            t = _st.unpack_from("<3f", b, pay)
        elif name == "Scale3D" and size == 12:
            sc = _st.unpack_from("<3f", b, pay)
        pos = pay + size
    return q, t, sc


def _xform_points(pts, q, t, sc):
    x, y, z, w = q
    r = [[1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
         [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
         [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]]
    out = []
    for px, py, pz in pts:
        v = (px * sc[0], py * sc[1], pz * sc[2])
        out.append(tuple(r[i][0] * v[0] + r[i][1] * v[1] + r[i][2] * v[2] + t[i] for i in range(3)))
    return out


def _body_hulls(p, data, e):
    """[(planes)] for a BodySetup export (convex elements), plus box / sphere / sphyl elements as planes where they are boxes."""
    o = props.read_object(p, data, e)
    ag = o.get("AggGeom", ("struct", {}, None))[1]
    hulls = []
    for el in ag.get("ConvexElems", ("array", []))[1]:
        vd = el[1].get("VertexData", ("array", []))
        if vd[0] == "raw":                                   # older saves: count + packed float triples
            import struct as _st
            n = _st.unpack_from("<i", vd[1], 0)[0]
            pts = [_st.unpack_from("<3f", vd[1], 4 + 12 * i) for i in range(n)] if 4 + 12 * n <= len(vd[1]) else []
        else:
            pts = [v[1] for v in vd[1] if v[0] == "vec"]
        q, t, sc = _elem_transform(p, el[1].get("Transform"))      # the element's own transform places the hull in mesh space
        pts = _xform_points(pts, q, t, sc)
        pl = hull_planes(pts)
        if pl:
            hulls.append(pl)
    for el in ag.get("BoxElems", ("array", []))[1]:
        d = el[1]
        c = d.get("Center", ("vec", (0, 0, 0)))[1]
        hx, hy, hz = float(d.get("X", 0)) / 2, float(d.get("Y", 0)) / 2, float(d.get("Z", 0)) / 2
        hulls.append([(1, 0, 0, c[0] + hx), (-1, 0, 0, -(c[0] - hx)), (0, 1, 0, c[1] + hy), (0, -1, 0, -(c[1] - hy)), (0, 0, 1, c[2] + hz), (0, 0, -1, -(c[2] - hz))])
    return hulls


def _physics_bodies(p, data, e):
    """[(bone, kind, center, rotation, dims)] of a PhysicsAsset export: its SkeletalBodySetups' simple shapes."""
    o = props.read_object(p, data, e)
    out = []
    for ref in o.get("SkeletalBodySetups", ("array", []))[1]:
        if ref[0] != "obj" or ref[2] <= 0:
            continue
        b = props.read_object(p, data, p.exports[ref[2] - 1])
        bone = b.get("BoneName", ("name", ""))[1]
        ag = b.get("AggGeom", ("struct", {}, None))[1]
        g = lambda d, k, dflt: tuple(d[k][1]) if k in d and d[k][0] == "vec" else dflt
        for el in ag.get("BoxElems", ("array", []))[1]:
            d = el[1]
            out.append((bone, 0, g(d, "Center", (0, 0, 0)), g(d, "Rotation", (0, 0, 0)), (float(d.get("X", 0)), float(d.get("Y", 0)), float(d.get("Z", 0)))))
        for el in ag.get("SphereElems", ("array", []))[1]:
            d = el[1]
            out.append((bone, 1, g(d, "Center", (0, 0, 0)), (0, 0, 0), (float(d.get("Radius", 0)), 0.0, 0.0)))
        for el in ag.get("SphylElems", ("array", []))[1]:
            d = el[1]
            out.append((bone, 2, g(d, "Center", (0, 0, 0)), g(d, "Rotation", (0, 0, 0)), (float(d.get("Radius", 0)), float(d.get("Length", 0)), 0.0)))
    return out


def _quat_matrix(q):
    x, y, z, w = q
    return [1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w),
            2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w),
            2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]


def _skeletal_bodies(bones, bodies):
    """Physics bodies placed in mesh space by the reference pose: bone transform * element transform."""
    import skeleton
    cs = skeleton.component_space(bones)
    index = {b[0]: i for i, b in enumerate(bones)}
    out = []
    for bone, kind, c, rot, dims in bodies:
        if bone not in index:
            continue
        bt, bq = cs[index[bone]]
        eq = skeleton.euler_to_quat(*rot)
        q = skeleton.quat_mul(bq, eq)
        t = skeleton.quat_rot(bq, c)
        t = (bt[0] + t[0], bt[1] + t[1], bt[2] + t[2])
        if kind == 0:
            a = (dims[0] / 2, dims[1] / 2, dims[2] / 2)
        elif kind == 1:
            a = (dims[0], 0.0, 0.0)
        else:
            a = (dims[0], dims[1] / 2, 0.0)
        out.append((bone, kind, t, _quat_matrix(q), a[0], a[1], a[2], tuple(c), _quat_matrix(eq)))
    return out


def _curve_keys(p, data, idx):
    """Keys of the CurveFloat export at package index idx (1-based): [(time, value, interp, arrive, leave)]."""
    if idx <= 0 or idx > len(p.exports):
        return []
    try:
        o = props.read_object(p, data, p.exports[idx - 1])
        keys = o["FloatCurve"][1]["Keys"][1]
        return [(k[1]["time"], k[1]["value"], k[1]["interp"], k[1]["arrive"], k[1]["leave"]) for k in keys]
    except Exception:
        return []


def collect(content_root):
    """Walk a project; return ({ge name: def}, {montage: length}, {class: {box: (x,y,z), capsule: (r, hh)}})."""
    import glob
    ges, montages, shapes, moves, projs, parents, bound = {}, {}, {}, {}, {}, {}, []
    TIMELINES.clear(); COMPONENTS.clear(); PHYS.clear(); WORLDMESHES.clear(); SKELMESHES.clear(); SKELCOMPS.clear(); MESHES.clear(); CHANNELS.clear(); PROFILES.clear()
    ini = os.path.join(content_root, "..", "Config", "DefaultEngine.ini")
    if os.path.exists(ini):
        import re as _re
        for m in _re.finditer(r'DefaultChannelResponses=\(Channel=\w+,DefaultResponse=ECR_(\w+),[^)]*Name="(\w+)"', open(ini, errors="ignore").read()):
            CHANNELS[m.group(2)] = {"Ignore": 0, "Overlap": 1, "Block": 2}[m.group(1)]
        for m in _re.finditer(r'\+Profiles=\(Name="(\w+)",[^\n]*?ObjectTypeName="(\w+)",CustomResponses=\((.*?)\),HelpMessage', open(ini, errors="ignore").read()):
            rs = [(c.group(1), {"Ignore": 0, "Overlap": 1, "Block": 2}[(c.group(2) or "Block")]) for c in _re.finditer(r'\(Channel="(\w+)"(?:,Response=ECR_(\w+))?\)', m.group(3))]
            PROFILES[m.group(1)] = (m.group(2), rs)
    import math as _m
    MESHES["Cylinder"] = [[(_m.cos(2 * _m.pi * i / 32), _m.sin(2 * _m.pi * i / 32), 0, 50 * _m.cos(_m.pi / 32)) for i in range(32)] + [(0, 0, 1, 50), (0, 0, -1, 50)]]   # /Engine/BasicShapes/Cylinder (r 50, h 100)
    MESHES["Cube"] = [[(1, 0, 0, 50), (-1, 0, 0, 50), (0, 1, 0, 50), (0, -1, 0, 50), (0, 0, 1, 50), (0, 0, -1, 50)]]      # /Engine/BasicShapes/Cube (100 uu)
    import skeleton as skeleton_mod
    pending_skel = []
    for f in glob.glob(content_root + "/**/*.uasset", recursive=True):
        try:
            p = uasset.parse_tables(f)
            data = open(f, "rb").read()
        except Exception:
            continue
        if any(uasset.class_name(p, x.cls) == "SCS_Node" for x in p.exports):
            _collect_components(p, data, os.path.splitext(os.path.basename(f))[0])
        for e in p.exports:
            cn = uasset.class_name(p, e.cls)
            if cn == "PhysicsAsset":
                try:
                    PHYS[e.name] = _physics_bodies(p, data, e)
                except Exception:
                    pass
            elif cn == "SkeletalMesh":
                try:
                    pa = props.read_object(p, data, e).get("PhysicsAsset")
                    bones = skeleton_mod.ref_skeleton(p, data, e)
                    if pa and pa[0] == "obj" and bones:
                        pending_skel.append((e.name, pa[1], bones))
                except Exception:
                    pass
            elif cn == "CapsuleComponent" and e.name == "CollisionCylinder":
                owner = next((x.name for x in p.exports if uasset.class_name(p, x.cls) == "BlueprintGeneratedClass"), None)
                try:
                    c = props.read_object(p, data, e)
                    responses, enabled = [], 2
                    bi = c.get("BodyInstance")
                    if owner and isinstance(bi, tuple) and bi[0] == "struct":
                        cr = bi[1].get("CollisionResponses")
                        if isinstance(cr, tuple) and cr[0] == "struct":
                            for r in cr[1].get("ResponseArray", ("array", []))[1]:
                                responses.append((r[1]["Channel"][1], {"ECR_Ignore": 0, "ECR_Overlap": 1, "ECR_Block": 2}.get(str(r[1]["Response"][1]), 2)))
                        SKELCOMPS.append({"cls": owner.removesuffix("_C"), "comp": "CapsuleComponent", "mesh": "", "loc": (0, 0, 0), "rot": (0, 0, 0), "scale": (1, 1, 1), "enabled": enabled, "responses": responses})
                except Exception:
                    pass
            elif cn == "SkeletalMeshComponent" and e.name == "CharacterMesh0" and not e.name.endswith("_GEN_VARIABLE"):
                owner = next((x.name for x in p.exports if uasset.class_name(p, x.cls) == "BlueprintGeneratedClass"), None)
                try:
                    c = props.read_object(p, data, e)
                    mesh = c.get("SkeletalMesh")
                    if owner and isinstance(mesh, tuple) and mesh[0] == "obj":
                        responses, enabled = [], 2
                        bi = c.get("BodyInstance")
                        if isinstance(bi, tuple) and bi[0] == "struct":
                            ce = bi[1].get("CollisionEnabled")
                            if isinstance(ce, tuple):
                                enabled = {"NoCollision": 0, "QueryOnly": 1, "PhysicsOnly": 3, "QueryAndPhysics": 2}.get(str(ce[1]).split("::")[-1], 2)
                            cr = bi[1].get("CollisionResponses")
                            if isinstance(cr, tuple) and cr[0] == "struct":
                                for r in cr[1].get("ResponseArray", ("array", []))[1]:
                                    responses.append((r[1]["Channel"][1], {"ECR_Ignore": 0, "ECR_Overlap": 1, "ECR_Block": 2}.get(str(r[1]["Response"][1]), 2)))
                        vec = lambda k, d: tuple(c[k][1]) if k in c and c[k][0] == "vec" else d
                        SKELCOMPS.append({"cls": owner.removesuffix("_C"), "comp": "Mesh", "mesh": mesh[1], "loc": vec("RelativeLocation", (0, 0, 0)),
                                          "rot": vec("RelativeRotation", (0, 0, 0)), "scale": vec("RelativeScale3D", (1, 1, 1)), "enabled": enabled, "responses": responses})
                except Exception:
                    pass
            if cn == "StaticMesh":                                  # the mesh's simple collision (a BodySetup export of the same package)
                try:
                    for x in p.exports:
                        if uasset.class_name(p, x.cls) == "BodySetup":
                            hs = _body_hulls(p, data, x)
                            if hs:
                                MESHES[e.name] = hs
                            break
                except Exception:
                    pass
            if cn == "TimelineTemplate":                      # a Timeline node's template: length, curve tracks, update / finished functions
                try:
                    t = props.read_object(p, data, e)
                    tracks = []
                    for tr in (t.get("FloatTracks", ("array", []))[1]):
                        d = tr[1]
                        tracks.append((d["PropertyName"][1], _curve_keys(p, data, d["CurveFloat"][2] if d["CurveFloat"][0] == "obj" else 0)))
                    g = lambda k, dflt="": t[k][1] if k in t and isinstance(t[k], tuple) else dflt
                    TIMELINES.append((os.path.splitext(os.path.basename(f))[0], {
                        "var": g("VariableName"), "length": float(t.get("TimelineLength", 5.0)), "loop": bool(t.get("bLoop", False)),
                        "autoplay": bool(t.get("bAutoPlay", False)), "update": g("UpdateFunctionName"), "finished": g("FinishedFunctionName"),
                        "direction": g("DirectionPropertyName")}, tracks))
                except Exception:
                    pass
            if cn == "BlueprintGeneratedClass" and e.super != 0:
                sup = uasset.class_name(p, e.super)
                if sup and sup != "None":
                    parents[e.name.removesuffix("_C")] = sup.removesuffix("_C")
            if cn == "AnimMontage":
                try:
                    o = props.read_object(p, data, e)
                    if "SequenceLength" in o:
                        bo = o.get("BlendOut")
                        blend = float(bo[1].get("BlendTime", 0.25)) if bo and bo[0] == "struct" else 0.25   # engine default
                        length = float(o["SequenceLength"])
                        notifies = []
                        for nt in (o.get("Notifies") or ("array", []))[1]:
                            d = nt[1]
                            name = d.get("NotifyName", ("name", ""))[1] if isinstance(d.get("NotifyName"), tuple) else ""
                            if not name or name == "None":
                                continue
                            method = enum_tail(d.get("LinkMethod"), "Absolute")
                            lv, seg0, seglen = float(d.get("LinkValue", 0.0)), float(d.get("SegmentBeginTime", 0.0)), float(d.get("SegmentLength", length))
                            t = seg0 + (lv if method == "Absolute" else lv * (seglen if method == "Relative" else length))
                            t += float(d.get("TriggerTimeOffset", 0.0))
                            tag = ""; dur = 0.0; state = 0
                            ref = d.get("Notify")
                            if isinstance(ref, tuple) and ref[0] == "obj" and ref[2] > 0:
                                try:
                                    no = props.read_object(p, data, p.exports[ref[2] - 1])
                                    et = no.get("EventTag")
                                    if isinstance(et, tuple) and et[0] == "struct":
                                        tag = et[1].get("TagName", ("name", ""))[1]
                                except Exception:
                                    pass
                            sref = d.get("NotifyStateClass")                    # a notify state: NotifyBegin at the time, NotifyEnd after Duration
                            if isinstance(sref, tuple) and sref[0] == "obj" and sref[2] > 0:
                                state = 1; dur = float(d.get("Duration", 0.0))
                                try:
                                    so = props.read_object(p, data, p.exports[sref[2] - 1])
                                    gt = so.get("GameplayTags")
                                    if isinstance(gt, tuple) and gt[0] == "tags":
                                        tag = "|".join(gt[1])
                                    name = uasset.class_name(p, p.exports[sref[2] - 1].cls)
                                except Exception:
                                    pass
                            notifies.append((max(t, 0.0), name.removesuffix("_C"), tag, dur, state))
                        montages[e.name] = (length, blend, sorted(notifies))
                except Exception:
                    pass
            elif cn == "AnimSequence":                         # GetPlayLength of a plain animation (no montage sections / notifies kept)
                try:
                    o = props.read_object(p, data, e)
                    if "SequenceLength" in o and e.name not in montages:
                        montages[e.name] = (float(o["SequenceLength"]), 0.0, [])
                except Exception:
                    pass
            elif cn == "ProjectileMovementComponent":
                owner = next((x.name for x in p.exports if uasset.class_name(p, x.cls) == "BlueprintGeneratedClass"), None)
                if owner:
                    try:
                        o = props.read_object(p, data, e)
                    except Exception:
                        o = {}
                    projs[owner.removesuffix("_C")] = (float(o.get("InitialSpeed", 0.0)), float(o.get("MaxSpeed", 0.0)), float(o.get("ProjectileGravityScale", 1.0)))
            elif cn.endswith("CharacterMovementComponent"):
                owner = next((x.name for x in p.exports if uasset.class_name(p, x.cls) == "BlueprintGeneratedClass"), None)
                if owner:
                    try:
                        o = props.read_object(p, data, e)
                    except Exception:
                        o = {}
                    rr = o.get("RotationRate")
                    moves[owner.removesuffix("_C")] = (
                        float(o.get("MaxAcceleration", 2048.0)), float(o.get("BrakingDecelerationWalking", 2048.0)),
                        float(o.get("GroundFriction", 8.0)), float(o.get("JumpZVelocity", 420.0)),
                        float(o.get("GravityScale", 1.0)), float(o.get("AirControl", 0.05)),
                        1.0 if o.get("bOrientRotationToMovement") else 0.0,
                        float(rr[1][1]) if isinstance(rr, tuple) and rr[0] == "vec" else 360.0)   # engine defaults
            elif cn == "K2Node_ComponentBoundEvent":
                owner = next((x.name for x in p.exports if uasset.class_name(p, x.cls) == "BlueprintGeneratedClass"), None)
                try:
                    o = props.read_object(p, data, e)
                    fnn = o["CustomFunctionName"][1]; comp = o["ComponentPropertyName"][1]; dlg = o["DelegatePropertyName"][1]
                    if owner and "Overlap" in dlg:
                        bound.append((owner.removesuffix("_C"), codegen_ident(fnn), comp, 1 if "End" in dlg else 0))
                except Exception:
                    pass
            elif cn in ("BoxComponent", "CapsuleComponent", "SphereComponent"):
                owner = next((x.name for x in p.exports if uasset.class_name(p, x.cls) == "BlueprintGeneratedClass"), None)
                if owner:
                    try:
                        o = props.read_object(p, data, e)
                    except Exception:
                        o = {}
                    vec = lambda k, d: tuple(o[k][1]) if isinstance(o.get(k), tuple) and o[k][0] == "vec" else d
                    sc = vec("RelativeScale3D", (1.0, 1.0, 1.0))
                    comp = e.name.removesuffix("_GEN_VARIABLE")
                    key = owner.removesuffix("_C")
                    if cn == "BoxComponent":      # engine default box extent is 32 per axis, scaled by the component scale
                        be = vec("BoxExtent", (32.0, 32.0, 32.0))
                        rl, rr = vec("RelativeLocation", (0.0, 0.0, 0.0)), vec("RelativeRotation", (0.0, 0.0, 0.0))
                        shapes.setdefault(key, []).append((comp, 1, be[0] * sc[0], be[1] * sc[1], be[2] * sc[2], rl[0], rl[1], rl[2], rr[1]))
                    elif cn == "SphereComponent":  # engine default sphere radius is 32
                        rl = vec("RelativeLocation", (0.0, 0.0, 0.0))
                        shapes.setdefault(key, []).append((comp, 3, float(o.get("SphereRadius", 32.0)) * max(sc), 0.0, 0.0, rl[0], rl[1], rl[2], 0.0))
                    elif cn == "CapsuleComponent":  # ACharacter default capsule: radius 34, half height 88
                        rl = vec("RelativeLocation", (0.0, 0.0, 0.0))
                        shapes.setdefault(key, []).append((comp, 2, float(o.get("CapsuleRadius", 34.0)), float(o.get("CapsuleHalfHeight", 88.0)), 0.0, rl[0], rl[1], rl[2], 0.0))
            elif e.name.startswith("Default__") and e.name.endswith("_C") and (e.name.startswith("Default__GE_") or "GameplayEffect" in cn):
                try:
                    ges[e.name[len("Default__"):-2]] = ge_definition(p, data, e)
                except Exception as ex:
                    ges[e.name[len("Default__"):-2]] = dict(name=e.name[len("Default__"):-2], error=str(ex))
    for f in glob.glob(content_root + "/**/*.umap", recursive=True):             # static mesh actors of the maps
        try:
            p = uasset.parse_tables(f); data = open(f, "rb").read()
            roots = set()
            for e in p.exports:
                if uasset.class_name(p, e.cls) == "StaticMeshActor":
                    rc = props.read_object(p, data, e).get("RootComponent")
                    if isinstance(rc, tuple) and rc[0] == "obj" and rc[2] > 0:
                        roots.add(rc[2] - 1)
            for i in sorted(roots):
                c = props.read_object(p, data, p.exports[i])
                m = c.get("StaticMesh")
                if not (isinstance(m, tuple) and m[0] == "obj"):
                    continue
                v = lambda k, d: tuple(c[k][1]) if k in c and c[k][0] == "vec" else d
                WORLDMESHES.append((m[1], v("RelativeLocation", (0, 0, 0)), v("RelativeRotation", (0, 0, 0)), v("RelativeScale3D", (1, 1, 1))))
        except Exception:
            continue
    for mname, pname, bones in pending_skel:                        # physics bodies of skeletal meshes, placed by the reference pose
        if pname in PHYS and PHYS[pname]:
            SKELMESHES[mname] = _skeletal_bodies(bones, PHYS[pname])
    return ges, montages, shapes, moves, projs, parents, bound


def _collect_components(p, data, cls):
    """Static-mesh components of a Blueprint from its SimpleConstructionScript: hierarchy, mesh, transform, collision responses."""
    nodes = {}
    for e in p.exports:
        if uasset.class_name(p, e.cls) == "SCS_Node":
            try:
                nodes[e.index + 1] = props.read_object(p, data, e)
            except Exception:
                pass
    parent_of = {}
    for idx, o in nodes.items():
        for ch in (o.get("ChildNodes") or ("array", []))[1]:
            if ch[0] == "obj":
                parent_of[ch[2]] = idx
    for idx, o in nodes.items():
        ct = o.get("ComponentTemplate")
        if not (isinstance(ct, tuple) and ct[0] == "obj" and ct[2] > 0):
            continue
        t = p.exports[ct[2] - 1]
        if uasset.class_name(p, t.cls) != "StaticMeshComponent":
            continue
        try:
            c = props.read_object(p, data, t)
        except Exception:
            continue
        mesh = c.get("StaticMesh")
        if not (isinstance(mesh, tuple) and mesh[0] == "obj"):
            continue
        par = nodes.get(parent_of.get(idx))
        pname = par["InternalVariableName"][1] if par and "InternalVariableName" in par else ""
        bi = c.get("BodyInstance")
        responses, profile, enabled = [], "BlockAll", 2
        if isinstance(bi, tuple) and bi[0] == "struct":
            profile = bi[1].get("CollisionProfileName", ("name", "BlockAll"))[1]
            ce = bi[1].get("CollisionEnabled")
            if isinstance(ce, tuple):
                enabled = {"NoCollision": 0, "QueryOnly": 1, "PhysicsOnly": 3, "QueryAndPhysics": 2}.get(str(ce[1]).split("::")[-1], 2)
            cr = bi[1].get("CollisionResponses")
            if isinstance(cr, tuple) and cr[0] == "struct":
                for r in cr[1].get("ResponseArray", ("array", []))[1]:
                    responses.append((r[1]["Channel"][1], {"ECR_Ignore": 0, "ECR_Overlap": 1, "ECR_Block": 2}.get(str(r[1]["Response"][1]), 2)))
        vec = lambda k, d: tuple(c[k][1]) if k in c and c[k][0] == "vec" else d
        COMPONENTS.append({"cls": cls, "comp": o["InternalVariableName"][1], "mesh": mesh[1], "parent": pname,
                           "loc": vec("RelativeLocation", (0, 0, 0)), "rot": vec("RelativeRotation", (0, 0, 0)), "scale": vec("RelativeScale3D", (1, 1, 1)),
                           "profile": profile, "enabled": enabled, "responses": responses})


# ---- C emission ----------------------------------------------------------------------------------------------------
def _f(v):
    return repr(float(v)) + "f"


def _cs(s):
    return '"' + str(s).replace("\\", "\\\\").replace('"', '\\"') + '"'


def _arr(name, items):
    return f"static const char *const {name}[] = {{{', '.join(_cs(i) for i in items)}{', ' if items else ''}0}};"


OPS = {"Additive": 0, "Multiplicative": 1, "Division": 2, "Override": 3}
CALC = {"ScalableFloat": 0, "AttributeBased": 1, "SetByCaller": 2}
DUR = {"Instant": 0, "Infinite": 1, "HasDuration": 2}
STACK = {"None": 0, "AggregateByTarget": 1, "AggregateBySource": 2}


def emit_gas_data_c(ges, montages, shapes, moves=None, projs=None, parents=None, bound=None):
    out = ['#include "u64_gas_core.h"']
    tl = []
    for i, (cls, t, tracks) in enumerate(TIMELINES):                                  # timelines: tracks of curve keys evaluated by u64_gas_extra.c
        for j, (prop, keys) in enumerate(tracks):
            ks = ", ".join("{%rf, %rf, %rf, %rf, %d}" % (k[0], k[1], k[3], k[4], k[2]) for k in keys)
            out.append(f"static const u64_curve_key tlk_{i}_{j}[] = {{{ks}{', ' if ks else ''}{{0}}}};")
        tr = ", ".join("{%s, %d, tlk_%d_%d}" % (_cs(prop), len(keys), i, j) for j, (prop, keys) in enumerate(tracks))
        out.append(f"static const u64_tl_track tlt_{i}[] = {{{tr}{', ' if tr else ''}{{0}}}};")
        tl.append("{%s, %s, %rf, %d, %d, %s, %s, %s, %d, tlt_%d}" % (_cs(cls), _cs(t["var"]), t["length"], int(t["loop"]), int(t["autoplay"]), _cs(t["update"]), _cs(t["finished"]), _cs(t["direction"]), len(tracks), i))
    # static-mesh collision: hulls per mesh, components per Blueprint class
    meshes = []
    for mi, (mname, hulls) in enumerate(sorted(MESHES.items())):
        hs = []
        for hi, pl in enumerate(hulls):
            out.append("static const float mh_%d_%d[] = {%s};" % (mi, hi, ", ".join(_f(v) for q in pl for v in q)))
            hs.append("{%d, mh_%d_%d}" % (len(pl), mi, hi))
        out.append("static const u64_hull mhs_%d[] = {%s%s{0}};" % (mi, ", ".join(hs), ", " if hs else ""))
        meshes.append("{%s, %d, mhs_%d}" % (_cs(mname), len(hulls), mi))
    out.append("const u64_mesh_def u64_meshes[] = {%s%s{0}};" % (", ".join(meshes), ", " if meshes else ""))
    comps = []
    for ci, c in enumerate(COMPONENTS):
        out.append("static const u64_resp cr_%d[] = {%s%s{0}};" % (ci, ", ".join("{%s, %d}" % (_cs(k), v) for k, v in c["responses"]), ", " if c["responses"] else ""))
        comps.append("{%s, %s, %s, %s, {%s}, {%s}, {%s}, %d, %d, cr_%d}" % (
            _cs(c["cls"]), _cs(c["comp"]), _cs(c["mesh"]), _cs(c["parent"]), ", ".join(_f(v) for v in c["loc"]), ", ".join(_f(v) for v in c["rot"]),
            ", ".join(_f(v) for v in c["scale"]), c["enabled"], len(c["responses"]), ci))
    pms = []                                                                            # physics bodies of skeletal meshes and the character Mesh components
    for mi, (mname, bodies) in enumerate(sorted(SKELMESHES.items())):
        rows = ", ".join("{%s, %d, {%s}, {%s}, %s, %s, %s, {%s}, {%s}}" % (_cs(b[0]), b[1], ", ".join(_f(v) for v in b[2]), ", ".join(_f(v) for v in b[3]), _f(b[4]), _f(b[5]), _f(b[6]),
                                                                      ", ".join(_f(v) for v in b[7]), ", ".join(_f(v) for v in b[8])) for b in bodies)
        out.append("static const u64_phys_body pb_%d[] = {%s%s{0}};" % (mi, rows, ", " if rows else ""))
        pms.append("{%s, %d, pb_%d}" % (_cs(mname), len(bodies), mi))
    out.append("const u64_phys_mesh u64_phys_meshes[] = {%s%s{0}};" % (", ".join(pms), ", " if pms else ""))
    scs = []
    for ci, c in enumerate(SKELCOMPS):
        out.append("static const u64_resp sr_%d[] = {%s%s{0}};" % (ci, ", ".join("{%s, %d}" % (_cs(k), v) for k, v in c["responses"]), ", " if c["responses"] else ""))
        scs.append("{%s, %s, %s, {%s}, {%s}, {%s}, %d, %d, sr_%d}" % (_cs(c["cls"]), _cs(c["comp"]), _cs(c["mesh"]), ", ".join(_f(v) for v in c["loc"]), ", ".join(_f(v) for v in c["rot"]),
                                                                    ", ".join(_f(v) for v in c["scale"]), c["enabled"], len(c["responses"]), ci))
    out.append("const u64_skel_comp_def u64_skel_comps[] = {%s%s{0}};" % (", ".join(scs), ", " if scs else ""))
    pdefs = []
    for pi, (k, (ot, rs)) in enumerate(sorted(PROFILES.items())):
        out.append("static const u64_resp pr_%d[] = {%s%s{0}};" % (pi, ", ".join("{%s, %d}" % (_cs(c), r) for c, r in rs), ", " if rs else ""))
        pdefs.append("{%s, %s, %d, pr_%d}" % (_cs(k), _cs(ot), len(rs), pi))
    wm = ", ".join("{%s, {%s}, {%s}, {%s}}" % (_cs(m), ", ".join(_f(x) for x in l), ", ".join(_f(x) for x in r), ", ".join(_f(x) for x in sc)) for m, l, r, sc in WORLDMESHES)
    out.append("const u64_world_mesh u64_world_meshes[] = {%s%s{0}};" % (wm, ", " if wm else ""))
    out.append("const u64_profile_def u64_profiles[] = {%s%s{0}};" % (", ".join(pdefs), ", " if pdefs else ""))
    out.append("const u64_resp u64_channel_defaults[] = {%s%s{0}};" % (", ".join("{%s, %d}" % (_cs(k), v) for k, v in sorted(CHANNELS.items())), ", " if CHANNELS else ""))
    out.append("const u64_comp_def u64_comp_defs[] = {%s%s{0}};" % (", ".join(comps), ", " if comps else ""))
    out.append("const u64_timeline_def u64_timelines[] = {%s%s{0}};" % (", ".join(tl), ", " if tl else ""))
    defs = []
    for n in sorted(ges):
        g = ges[n]
        if "error" in g:
            continue
        v = codegen_ident(n)
        mods = ", ".join("{%s, %d, %d, %rf, %s, %d, %rf, %rf, %rf, %s}" % (
            _cs(m["attr"]), OPS.get(m["op"], 0), CALC.get(m["calc"], 0), m["value"], _cs(m["backing"]),
            1 if m["backing_src"] == "Target" else 0, m["coef"], m["pre"], m["post"], _cs(m["sbc_tag"])) for m in g["mods"])
        out.append(f"static const u64_modinfo mods_{v}[] = {{{mods}{', ' if mods else ''}{{0}}}};")
        for key in ("execs", "granted", "asset", "remove", "app_req", "app_ign", "on_req", "on_ign"):
            src = {"execs": g["executions"], "granted": g["granted_tags"], "asset": g["asset_tags"], "remove": g["remove_tags"],
                   "app_req": g["app_req"][0], "app_ign": g["app_req"][1], "on_req": g["ongoing_req"][0], "on_ign": g["ongoing_req"][1]}[key]
            out.append(_arr(f"{key}_{v}", src))
        cond = g.get("conditional", [])
        out.append(_arr(f"cond_eff_{v}", [c for c, _ in cond]))
        out.append(_arr(f"cond_req_{v}", ["|".join(t) for _, t in cond]))
        defs.append("  {%s, %d, %rf, %rf, %d, mods_%s, execs_%s, granted_%s, asset_%s, remove_%s, app_req_%s, app_ign_%s, on_req_%s, on_ign_%s, %d, %d, cond_eff_%s, cond_req_%s}" % (
            _cs(n), DUR.get(g["duration"], 0), g["duration_value"], g["period"], len(g["mods"]), v, v, v, v, v, v, v, v, v,
            STACK.get(g["stack_type"], 0), g["stack_limit"], v, v))
    out.append("const u64_ge_def u64_ge_defs[] = {\n" + ",\n".join(defs) + "\n};")
    out.append(f"const int u64_ge_count = {len(defs)};")
    rows = []
    for k, v in sorted(montages.items()):
        nv = codegen_ident(k)
        nl = ", ".join("{%rf, %s, %s, %rf, %d}" % (t, _cs(c), _cs(tag), dur, st) for t, c, tag, dur, st in v[2])
        out.append(f"static const u64_notify_def notifies_{nv}[] = {{{nl}{', ' if nl else ''}{{0, 0, 0, 0, 0}}}};")
        rows.append("{%s, %rf, %rf, %d, notifies_%s}" % (_cs(k), v[0], v[1], len(v[2]), nv))
    ms = ", ".join(rows)
    out.append(f"const u64_montage_def u64_montages[] = {{{ms}{', ' if ms else ''}{{0}}}};")
    ss = ", ".join("{%s, %s, %d, %rf, %rf, %rf, %rf, %rf, %rf, %rf}" % ((_cs(k), _cs(e[0])) + tuple(e[1:])) for k, lst in sorted(shapes.items()) for e in lst)
    out.append(f"const u64_shape_def u64_shapes[] = {{{ss}{', ' if ss else ''}{{0}}}};")
    mv = ", ".join("{%s, %rf, %rf, %rf, %rf, %rf, %rf, %rf, %rf}" % ((_cs(k),) + tuple(v)) for k, v in sorted((moves or {}).items()))
    out.append(f"const u64_movement_def u64_movements[] = {{{mv}{', ' if mv else ''}{{0}}}};")
    pj = ", ".join("{%s, %rf, %rf, %rf}" % ((_cs(k),) + tuple(v)) for k, v in sorted((projs or {}).items()))
    out.append(f"const u64_projectile_def u64_projectiles[] = {{{pj}{', ' if pj else ''}{{0}}}};")
    be = ", ".join("{%s, %s, %s, %d}" % tuple((_cs(x) if isinstance(x, str) else x) for x in b) for b in sorted(set(bound or [])))
    out.append(f"const u64_bound_event_def u64_bound_events[] = {{{be}{', ' if be else ''}{{0}}}};")
    pr = ", ".join("{%s, %s}" % (_cs(k), _cs(v)) for k, v in sorted((parents or {}).items()))
    out.append(f"const u64_class_parent_def u64_class_parents[] = {{{pr}{', ' if pr else ''}{{0}}}};")
    return "\n".join(out) + "\n"


def codegen_ident(s):
    import re
    return re.sub(r"[^A-Za-z0-9_]", "_", s)
