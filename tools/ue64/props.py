"""Recursive tagged-property reader: structs, arrays (of structs/scalars), enums, nested tags.

Every value is checked against its declared size; anything that does not consume exactly that many bytes is kept as
('raw', bytes) rather than guessed. Layout: UE 4.2x - 5.3 property tags.
"""
from __future__ import annotations

import struct

import uasset
from uasset import R, fname, class_name

BINARY_STRUCTS = {  # structs serialised as plain bytes instead of a tag stream
    "Vector": None, "Vector3f": 12, "Vector3d": 24, "Rotator": None, "Quat": None, "Transform": None, "Vector2D": None,
    "LinearColor": 16, "Color": 4, "Guid": 16, "IntPoint": 8, "Box": None, "DateTime": 8, "Timespan": 8,
}


UNPARSED = {}   # struct name -> times it could not be read as a tag stream


class Ctx:
    def __init__(self, p, data):
        self.p, self.data = p, data


def _tag_header(r, ctx):
    name = fname(r, ctx.p.names)
    if name == "None":
        return None
    typ = fname(r, ctx.p.names)
    size, idx = r.i32(), r.i32()
    sub = inner = None
    if typ == "StructProperty":
        sub = fname(r, ctx.p.names)
        if ctx.p.ue4 >= 441:
            r.skip(16)
    elif typ == "BoolProperty":
        sub = r.b[r.p]; r.p += 1
    elif typ in ("ByteProperty", "EnumProperty"):
        sub = fname(r, ctx.p.names)
    elif typ in ("ArrayProperty", "SetProperty"):
        inner = fname(r, ctx.p.names)
    elif typ == "MapProperty":
        inner = (fname(r, ctx.p.names), fname(r, ctx.p.names))
    if ctx.p.ue4 >= 282:
        has_guid = r.b[r.p]; r.p += 1
        if has_guid:
            r.skip(16)
    return name, typ, size, idx, sub, inner


def read_stream(r, ctx, end):
    """Tagged properties until the 'None' terminator (or end) -> dict."""
    out = {}
    while r.p + 8 <= end:
        h = _tag_header(r, ctx)
        if h is None:
            break
        name, typ, size, idx, sub, inner = h
        start = r.p
        if size < 0 or start + size > end:
            raise ValueError("property overruns its container")
        try:
            v = _value(r, ctx, typ, size, sub, inner, start)
        except Exception:
            v = None
            r.p = start + size
            v = ("raw", bytes(r.b[start:start + size]))
        if r.p != start + size:        # value reader disagreed with the declared size: keep raw, trust the size
            v = ("raw", bytes(r.b[start:start + size]))
            r.p = start + size
        key = name if idx == 0 else f"{name}[{idx}]"
        out[key] = v
    return out


def _scalar(r, ctx, typ, size, enum=None):
    p = ctx.p
    if typ in ("IntProperty",): return r.i32()
    if typ == "UInt32Property": return r.u32()
    if typ == "Int64Property": v = struct.unpack_from("<q", r.b, r.p)[0]; r.p += 8; return v
    if typ == "FloatProperty": v = struct.unpack_from("<f", r.b, r.p)[0]; r.p += 4; return v
    if typ == "DoubleProperty": v = struct.unpack_from("<d", r.b, r.p)[0]; r.p += 8; return v
    if typ == "NameProperty": return ("name", fname(r, p.names))
    if typ == "StrProperty": return r.fstring()
    if typ in ("ObjectProperty", "ClassProperty", "SoftObjectProperty", "InterfaceProperty", "WeakObjectProperty"):
        ref = r.i32()
        return ("obj", class_name(p, ref), ref)
    if typ == "ByteProperty":
        if size == 1 or (enum in (None, "None") and size not in (8,)):
            v = r.b[r.p]; r.p += 1; return v
        return ("enum", fname(r, p.names))
    if typ == "EnumProperty":
        return ("enum", fname(r, p.names))
    if typ == "BoolProperty":
        v = r.b[r.p]; r.p += 1; return bool(v)
    raise ValueError(typ)


def _struct(r, ctx, name, size, start):
    if name in ("Vector", "Rotator") and size in (12, 24):
        v = struct.unpack_from("<3f" if size == 12 else "<3d", r.b, r.p); r.p += size
        return ("vec", v)
    if name in BINARY_STRUCTS and BINARY_STRUCTS[name] in (None, size):
        raw = bytes(r.b[r.p:r.p + size]); r.p += size
        return ("raw", raw)
    if name == "GameplayTag" and size == 8:
        return ("tag", fname(r, ctx.p.names))
    if name == "GameplayTagContainer" and size >= 4:      # stored as TArray<FGameplayTag>: count + names
        n = r.i32()
        if 0 <= n <= (size - 4) // 8:
            tags = [fname(r, ctx.p.names) for _ in range(n)]
            if r.p == start + size:
                return ("tags", tags)
        r.p = start
    save = r.p
    try:
        v = read_stream(r, ctx, start + size)
        if r.p <= start + size:
            return ("struct", v, name)
    except Exception:
        pass
    UNPARSED[name] = UNPARSED.get(name, 0) + 1
    r.p = save + size
    return ("raw", bytes(r.b[save:save + size]))


def _value(r, ctx, typ, size, sub, inner, start):
    if typ == "BoolProperty":
        return bool(sub)
    if typ == "StructProperty":
        return _struct(r, ctx, sub, size, start)
    if typ == "ArrayProperty":
        n = r.i32()
        if inner == "StructProperty":
            if n == 0:
                r.p = start + size            # an empty array may still carry its inner tag header
                return ("array", [])
            h = _tag_header(r, ctx)               # one shared header describes the elements
            _, _, _, _, sname, _ = h
            elems = []
            esize = (h[2]) // max(n, 1) if False else None
            end_all = r.p + h[2]
            for _ in range(n):
                if sname in BINARY_STRUCTS or sname == "GameplayTag":
                    k = BINARY_STRUCTS.get(sname) or 8
                    elems.append(_struct(r, ctx, sname, k, r.p))
                else:
                    save = r.p
                    try:
                        elems.append(("struct", read_stream(r, ctx, end_all), sname))
                    except Exception:
                        UNPARSED[sname] = UNPARSED.get(sname, 0) + 1
                        raise
            return ("array", elems)
        return ("array", [_scalar(r, ctx, inner, 8 if inner in ("NameProperty",) else 0) for _ in range(n)])
    if typ in ("MapProperty", "SetProperty"):
        raise ValueError(typ)
    return _scalar(r, ctx, typ, size, sub)


def read_object(p, data, e):
    """All tagged properties of an export -> nested dict."""
    r = R(data)
    r.p = e.offset
    return read_stream(r, Ctx(p, data), e.offset + e.size)
