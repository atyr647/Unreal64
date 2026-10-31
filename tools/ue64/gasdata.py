"""Extract Gameplay Ability System data from assets: GameplayEffect definitions, montage lengths, collision shapes.

Unreal only saves properties that differ from their defaults, so every field below is read with the engine's default
(e.g. ModifierOp = Additive, magnitude calculation = ScalableFloat, attribute capture source = Source).
"""
from __future__ import annotations

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


def collect(content_root):
    """Walk a project; return ({ge name: def}, {montage: length}, {class: {box: (x,y,z), capsule: (r, hh)}})."""
    import glob
    ges, montages, shapes, moves, projs, parents, bound = {}, {}, {}, {}, {}, {}, []
    for f in glob.glob(content_root + "/**/*.uasset", recursive=True):
        try:
            p = uasset.parse_tables(f)
            data = open(f, "rb").read()
        except Exception:
            continue
        for e in p.exports:
            cn = uasset.class_name(p, e.cls)
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
                            tag = ""
                            ref = d.get("Notify")
                            if isinstance(ref, tuple) and ref[0] == "obj" and ref[2] > 0:
                                try:
                                    no = props.read_object(p, data, p.exports[ref[2] - 1])
                                    et = no.get("EventTag")
                                    if isinstance(et, tuple) and et[0] == "struct":
                                        tag = et[1].get("TagName", ("name", ""))[1]
                                except Exception:
                                    pass
                            notifies.append((max(t, 0.0), name.removesuffix("_C"), tag))
                        montages[e.name] = (length, blend, sorted(notifies))
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
    return ges, montages, shapes, moves, projs, parents, bound


# ---- C emission ----------------------------------------------------------------------------------------------------
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
        nl = ", ".join("{%rf, %s, %s}" % (t, _cs(c), _cs(tag)) for t, c, tag in v[2])
        out.append(f"static const u64_notify_def notifies_{nv}[] = {{{nl}{', ' if nl else ''}{{0, 0, 0}}}};")
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
