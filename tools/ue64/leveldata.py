"""Class defaults (CDOs) and placed actors from packages -> C tables for the runtime."""
from __future__ import annotations

import codegen
import props
import uasset


import itertools
_ids = itertools.count()


def cval(v, owner="o"):
    """C expression for a tagged-property value, or None if it can't be represented."""
    if isinstance(v, bool): return f"u64_bool({int(v)})"
    if isinstance(v, int): return f"u64_int({v})"
    if isinstance(v, float): return f"u64_float({v!r}f)"
    if isinstance(v, str): return f"u64_str({codegen.cstr(v)})"
    if isinstance(v, tuple):
        if v[0] == "name": return f"u64_name({codegen.cstr(v[1])})"
        if v[0] == "obj":
            if len(v) > 2 and v[2] > 0:        # a sub-object of this package (component): owned by the actor
                return f"u64_comp({owner}, {codegen.cstr(v[1])})"
            return f"u64_cls({codegen.cstr(v[1].removesuffix('_C'))})"
        if v[0] == "vec": return f"u64_vec({v[1][0]!r}f, {v[1][1]!r}f, {v[1][2]!r}f)"
        if v[0] == "enum": return f"u64_name({codegen.cstr(v[1])})"
        if v[0] == "struct" and len(v) > 2 and v[2] == "GameplayTag" and isinstance(v[1], dict) and v[1].get("TagName", (None,))[0] == "name":
            return f"u64_name({codegen.cstr(v[1]['TagName'][1])})"                 # 4.26 saves the tag as a struct with TagName
        if v[0] == "struct" and len(v) > 2 and v[2] == "AbilityTriggerData" and isinstance(v[1], dict):   # [trigger tag, source enum]
            tg, src = cval(v[1].get("TriggerTag", ("name", "None")), owner), cval(v[1].get("TriggerSource", ("enum", "EGameplayAbilityTriggerSource::GameplayEvent")), owner)
            if tg and src:
                return f"u64_array_const({next(_ids)}, 2, (u64_v[]){{u64_int(0), {tg}, {src}}} + 1)"
            return None
        if v[0] == "struct" and len(v) > 2 and v[2] == "GameplayTagContainer" and isinstance(v[1], dict) and isinstance(v[1].get("GameplayTags"), tuple):
            return cval(("array", v[1]["GameplayTags"][1]) if v[1]["GameplayTags"][0] == "array" else v[1]["GameplayTags"], owner)
        if v[0] == "map":                                          # flat array: key, value, key, value ... (struct keys/values: see struct_cval)
            parts = [cval(k, owner) for k, _ in v[1]] and [x for kv in v[1] for x in (cval(kv[0], owner), cval(kv[1], owner))]
            if all(parts):
                return f"u64_array_const({next(_ids)}, {len(parts)}, (u64_v[]){{u64_int(0){', ' + ', '.join(parts) if parts else ''}}} + 1)"
            return None
        if v[0] == "struct" and len(v) > 2 and v[2] is None and isinstance(v[1], dict):     # an anonymous (map key/value) struct
            d = v[1]
            if set(d) == {"TagName"}: return cval(d["TagName"], owner)                 # a GameplayTag key
            parts = [x for k, fv in d.items() for x in (f"u64_name({codegen.cstr(k)})", cval(fv, owner))]
            if all(parts):                                                              # flat array: field name, value, ...
                return f"u64_array_const({next(_ids)}, {len(parts)}, (u64_v[]){{u64_int(0), {', '.join(parts)}}} + 1)"
            return None
        if v[0] == "tag": return f"u64_name({codegen.cstr(v[1])})"
        if v[0] == "tags":
            items = ", ".join(f"u64_name({codegen.cstr(t)})" for t in v[1])
            return f"u64_array_const({next(_ids)}, {len(v[1])}, (u64_v[]){{u64_int(0){', ' + items if items else ''}}} + 1)"
        if v[0] == "array":
            parts = [cval(x, owner) for x in v[1]]
            if all(parts):
                items = ", ".join(parts)
                return f"u64_array_const({next(_ids)}, {len(parts)}, (u64_v[]){{u64_int(0){', ' + items if items else ''}}} + 1)"
    return None


def class_defaults(p, data):
    """{ClassName: {prop: value}} from `Default__X_C` exports."""
    out = {}
    for e in p.exports:
        if e.name.startswith("Default__") and e.name.endswith("_C"):
            cls = e.name[len("Default__"):-2]
            try:
                out[cls] = props.read_object(p, data, e)          # nested: arrays, tag containers, enums
                for k, v in list(out[cls].items()):               # properties of the actor's components (camera boom, gun, ...)
                    if isinstance(v, tuple) and v[0] == "obj" and len(v) > 2 and 0 < v[2] <= len(p.exports):
                        try:
                            out[cls]["__comp:" + v[1]] = ("compprops", props.read_object(p, data, p.exports[v[2] - 1]))
                        except Exception:
                            pass
            except Exception:
                try:
                    out[cls] = uasset.read_tags(p, data, e)
                except Exception:
                    pass  # older tag layouts (pre-UE4.20 assets) are not handled; defaults are skipped
    return out


def level_actors(p, data):
    """Blueprint-class actors placed in a map: [(label, class, props, (x,y,z))]."""
    actors = []
    for e in p.exports:
        cn = uasset.class_name(p, e.cls)
        if (not cn.endswith("_C") and cn != "PlayerStart" and e.name != "EnemyHeroSpawn") or e.name.startswith("Default__"):
            continue
        try:
            props = uasset.read_tags(p, data, e)
        except Exception:
            continue
        loc, rot, scale = (0.0, 0.0, 0.0), (0.0, 0.0, 0.0), (1.0, 1.0, 1.0)
        root = props.get("RootComponent")
        if isinstance(root, tuple) and root[0] == "obj" and 0 < root[2] <= len(p.exports):
            try:
                rt = uasset.read_tags(p, data, p.exports[root[2] - 1])
            except Exception:
                rt = {}
            if isinstance(rt.get("RelativeLocation"), tuple) and rt["RelativeLocation"][0] == "vec":
                loc = rt["RelativeLocation"][1]
            if isinstance(rt.get("RelativeRotation"), tuple) and rt["RelativeRotation"][0] == "vec":
                rot = rt["RelativeRotation"][1]
            if isinstance(rt.get("RelativeScale3D"), tuple) and rt["RelativeScale3D"][0] == "vec":
                scale = rt["RelativeScale3D"][1]
        props = dict(props); props["__rot"] = ("vec", rot); props["__scale"] = ("vec", scale)
        actors.append((e.name, cn.removesuffix("_C"), props, loc))
    return actors


def emit_defaults_c(defaults):
    lines = ['#include "u64_gas_core.h"', "void u64_apply_defaults(u64_obj *o, const char *cls) {"]
    for cls in sorted(defaults):
        sets = [(k, cval(v, "o")) for k, v in defaults[cls].items() if k not in ("UberGraphFrame", "ActorLabel") and not k.startswith("__comp:")]
        sets = [(k, c) for k, c in sets if c]
        comps = []
        for k, v in defaults[cls].items():
            if k.startswith("__comp:") and v[0] == "compprops":
                cs = [(pk, cval(pv, "o")) for pk, pv in v[1].items() if pk not in ("UberGraphFrame",)]
                cs = [(pk, c) for pk, c in cs if c and not c.startswith("u64_comp(")]
                if cs:
                    comps.append((k[len("__comp:"):], cs))
        if not sets and not comps:
            continue
        lines.append(f'  if (!strcmp(cls, {codegen.cstr(cls)})) {{')
        lines += [f'    *u64_ivar(o, {codegen.cstr(codegen.ident(k))}) = {c};' for k, c in sets]
        for cname, cs in comps:
            lines.append(f'    {{ u64_obj *c = u64_asobj(u64_comp(o, {codegen.cstr(cname)}));')
            lines += [f'      *u64_ivar(c, {codegen.cstr(codegen.ident(pk))}) = {c};' for pk, c in cs]
            lines.append("    }")
        lines.append("  }")
    lines.append("}")
    return "\n".join(lines) + "\n"


def emit_level_c(actors):
    lines = ['#include "u64_gas_core.h"', "void u64_level_load(void) {", "  u64_obj *a;"]
    for label, cls, props, loc in actors:
        lines.append(f"  a = u64_new({codegen.cstr(label)}, {codegen.cstr(cls)});")
        lines.append(f"  *u64_ivar(a, \"Location\") = u64_vec({loc[0]!r}f, {loc[1]!r}f, {loc[2]!r}f);")
        for k, v in props.items():
            c = cval(v, "a")
            if c and k not in ("UberGraphFrame", "ActorLabel", "RootComponent"):
                name = {"__rot": "Rotation", "__scale": "Scale"}.get(k, codegen.ident(k))
                lines.append(f"  *u64_ivar(a, {codegen.cstr(name)}) = {c};")
    lines.append("  u64_world_begin_play();")
    lines.append("}")
    return "\n".join(lines) + "\n"
