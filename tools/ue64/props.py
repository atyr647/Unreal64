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
    "RichCurveKey": 27,
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
    if ctx.p.ue4 >= 503:                                    # VER_UE4_PROPERTY_GUID_IN_PROPERTY_TAG
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
    if name == "RichCurveKey" and size == 27:       # InterpMode, TangentMode, TangentWeightMode (u8), then Time, Value, ArriveTangent, ArriveWeight, LeaveTangent, LeaveWeight
        im, tm, tw = struct.unpack_from("<BBB", r.b, r.p)
        t, v, at, aw, lt, lw = struct.unpack_from("<6f", r.b, r.p + 3); r.p += 27
        return ("key", {"interp": im, "time": t, "value": v, "arrive": at, "leave": lt})
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
            body = r.p
            try:
                return _struct_array(r, ctx, n, True)
            except Exception:
                r.p = body                         # some (older) saves have no shared inner header: the elements are plain tag streams
                return _struct_array(r, ctx, n, False, start + size)
            return ("array", elems)
        return ("array", [_scalar(r, ctx, inner, 8 if inner in ("NameProperty",) else 0) for _ in range(n)])
    if typ == "MapProperty":
        ktype, vtype = inner
        r.i32()                                   # entries to remove (delta serialisation)
        n = r.i32()
        items = []
        for _ in range(n):
            # struct keys and values are tagged property sets (a GameplayTag key is {TagName: name}); scalars read as usual
            key = ("struct", read_stream(r, ctx, start + size), None) if ktype == "StructProperty" else _scalar(r, ctx, ktype, 8 if ktype == "NameProperty" else 0)
            val = ("struct", read_stream(r, ctx, start + size), None) if vtype == "StructProperty" else _scalar(r, ctx, vtype, 8 if vtype == "NameProperty" else 0)
            items.append((key, val))
        return ("map", items)
    if typ == "SetProperty":
        raise ValueError(typ)
    return _scalar(r, ctx, typ, size, sub)


def _struct_array(r, ctx, n, shared, end_all=None):
    sname = None
    if shared:
        h = _tag_header(r, ctx)               # one shared header describes the elements
        sname = h[4]
        end_all = r.p + h[2]
    elems = []
    for _ in range(n):
        if sname in BINARY_STRUCTS or sname == "GameplayTag":
            k = BINARY_STRUCTS.get(sname) or (12 if sname in ("Vector", "Rotator") else 8)     # (RichCurveKey: 27; Vector/Rotator: 3 floats pre-LWC)
            elems.append(_struct(r, ctx, sname, k, r.p))
        else:
            try:
                elems.append(("struct", read_stream(r, ctx, end_all), sname))
            except Exception:
                UNPARSED[sname] = UNPARSED.get(sname, 0) + 1
                raise
    return ("array", elems)


def read_object(p, data, e):
    """All tagged properties of an export -> nested dict."""
    r = R(data)
    r.p = e.offset
    return read_stream(r, Ctx(p, data), e.offset + e.size)
