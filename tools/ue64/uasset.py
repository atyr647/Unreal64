"""Read-only parser for the header of UE4/UE5 .uasset/.umap packages (summary + name table).

Enough to inventory a project (which classes, functions and assets each package mentions) without the editor.
Layout follows UE 5.0-5.4 (legacy -8, FileVersionUE5 < 1016, i.e. before the saved-hash header change).
"""
from __future__ import annotations

import struct
from dataclasses import dataclass, field

TAG = 0x9E2A83C1
PKG_FILTER_EDITOR_ONLY = 0x80000000


class R:
    def __init__(self, b): self.b, self.p = b, 0
    def i32(self): v = struct.unpack_from("<i", self.b, self.p)[0]; self.p += 4; return v
    def u32(self): v = struct.unpack_from("<I", self.b, self.p)[0]; self.p += 4; return v
    def u16(self): v = struct.unpack_from("<H", self.b, self.p)[0]; self.p += 2; return v
    def skip(self, n): self.p += n
    def fstring(self):
        n = self.i32()
        if n == 0: return ""
        if n > 0:
            s = self.b[self.p:self.p + n - 1].decode("latin-1"); self.p += n; return s
        s = self.b[self.p:self.p - 2 * n - 2].decode("utf-16-le"); self.p += -2 * n; return s


@dataclass
class Package:
    path: str
    legacy: int = 0
    ue4: int = 0
    ue5: int = 0
    flags: int = 0
    names: list = field(default_factory=list)
    export_count: int = 0
    import_count: int = 0
    header_size: int = 0
    export_offset: int = 0
    import_offset: int = 0
    imports: list = field(default_factory=list)
    exports: list = field(default_factory=list)


def parse(path) -> Package:
    with open(path, "rb") as f:
        data = f.read(4 << 20)  # headers sit at the front; names table well inside 4 MB
    r = R(data)
    if r.u32() != TAG:
        raise ValueError("not a UE package")
    p = Package(path)
    p.legacy = r.i32()
    if p.legacy != -4:
        r.i32()  # legacy UE3 version
    p.ue4 = r.i32()
    if p.legacy <= -8:
        p.ue5 = r.i32()
    r.i32()  # licensee version
    for _ in range(r.i32()):  # custom versions: guid + version (+ friendly name in the pre -6 'Guids' format)
        if p.legacy == -2:
            r.skip(8)  # 'Enums' format: tag + version
            continue
        r.skip(16 + 4)
        if p.legacy > -6:
            r.fstring()
    p.header_size = r.i32()
    r.fstring()  # folder name
    p.flags = r.u32()
    name_count, name_off = r.i32(), r.i32()
    if p.ue5 >= 1008:
        r.skip(8)  # soft object paths count/offset
    if p.ue4 >= 516 and not p.flags & PKG_FILTER_EDITOR_ONLY:
        r.fstring()  # localization id
    if p.ue4 >= 459:
        r.skip(8)  # gatherable text data
    p.export_count, p.export_offset = r.i32(), r.i32()
    p.import_count, p.import_offset = r.i32(), r.i32()
    r.p = name_off
    for _ in range(name_count):
        p.names.append(r.fstring())
        if p.ue4 >= 504:
            r.skip(4)  # case-preserving + non-case-preserving hashes
    return p


@dataclass
class Export:
    index: int
    cls: int          # package index (neg = import, pos = export, 0 = none)
    super: int
    outer: int
    name: str
    flags: int
    size: int
    offset: int


def fname(r, names):
    i, n = r.i32(), r.i32()
    s = names[i] if 0 <= i < len(names) else f"<bad{i}>"
    return s if n == 0 else f"{s}_{n - 1}"


def parse_tables(path) -> Package:
    """parse() plus import/export tables (UE 5.0-5.3 layout)."""
    p = parse(path)
    with open(path, "rb") as f:
        data = f.read()
    r = R(data)
    r.p = p.import_offset
    for _ in range(p.import_count):
        pkg, cls = fname(r, p.names), fname(r, p.names)
        outer = r.i32()
        name = fname(r, p.names)
        if p.ue4 >= 520 and not p.flags & PKG_FILTER_EDITOR_ONLY:
            r.skip(8)  # PackageName
        if p.ue5 >= 1003:
            r.skip(4)  # bImportOptional
        p.imports.append((pkg, cls, outer, name))
    r.p = p.export_offset
    for i in range(p.export_count):
        cls, sup = r.i32(), r.i32()
        if p.ue4 >= 508:
            r.i32()  # template index
        outer = r.i32()
        name = fname(r, p.names)
        flags = r.u32()
        if p.ue4 >= 511:  # 64-bit serial sizes
            size, off = struct.unpack_from("<qq", data, r.p); r.skip(16)
        else:
            size, off = r.i32(), r.i32()
        r.skip(12)  # forced export, not for client, not for server
        if p.ue5 < 1005:
            r.skip(16)  # package guid
        if p.ue5 >= 1006:
            r.skip(4)  # inherited instance
        if p.ue4 >= 225:
            r.skip(4)  # package flags
        if p.ue4 >= 365:
            r.skip(4)  # not-always-loaded-for-editor-game
        if p.ue4 >= 485:
            r.skip(4)  # is asset
        if p.ue5 >= 1003:
            r.skip(4)  # generate public hash
        if p.ue4 >= 507:
            r.skip(20)  # preload dependency ints
        if not (0 <= off < len(data) and 0 <= size <= len(data) - off and off >= p.header_size):
            raise ValueError(f"{path}: export table not understood (export {i} at {off}+{size}); package version {p.ue4}")
        p.exports.append(Export(i, cls, sup, outer, name, flags, size, off))   # sup: super class/struct package index
    return p


def class_name(p, idx):
    if idx < 0 and -idx - 1 < len(p.imports):
        return p.imports[-idx - 1][3]
    if idx > 0 and idx - 1 < len(p.exports):
        return p.exports[idx - 1].name
    return "None"


def find_scripts(data: bytes, e: Export):
    """Candidate (BytecodeBufferSize, SerializedScriptSize, script) triples inside a Function export, last first.

    Disk size can exceed in-memory size (field paths), so only require the script to end with EndOfScript; callers
    confirm a candidate by decoding it and comparing the in-memory size with BytecodeBufferSize.
    """
    blob = data[e.offset:e.offset + e.size]
    hits = []
    for k in range(0, len(blob) - 8):
        buf, ser = struct.unpack_from("<ii", blob, k)
        if 0 < buf <= 8 * ser and 0 < ser and k + 8 + ser <= len(blob) and blob[k + 8 + ser - 1] == 0x53:
            hits.append((buf, ser, blob[k + 8:k + 8 + ser]))
    return hits[::-1]


CPF_PARM, CPF_OUT, CPF_RETURN, CPF_REF = 0x80, 0x100, 0x400, 0x8000


def function_params(p, data, fn_export):
    """[(name, is_out)] parameters of a function in declaration order, excluding the return value.

    Pre-FField packages store each parameter as a property export owned by the function:
    tagged-properties terminator FName (8) + 4 bytes + ArrayDim (4) + PropertyFlags (8, at +16).
    """
    owner = fn_export.index + 1
    out = []
    for x in p.exports:
        if x.outer == owner and class_name(p, x.cls).endswith("Property") and x.size >= 24:
            flags = struct.unpack_from("<Q", data, x.offset + 16)[0]
            if flags & CPF_PARM and not flags & CPF_RETURN:
                out.append((x.name, bool(flags & (CPF_OUT | CPF_REF))))
    return out


# ---- tagged property reader (class default objects) -------------------------------------------------------------

def read_tags(p, data, e):
    """Tagged properties at the start of an export's data -> {name: value}. Unknown types are skipped by size.

    Values: int/float/bool/str; ('name', s); ('obj', pkg-index resolved name); ('vec', (x, y, z)); ('raw', bytes).
    Handles the UE 4.2x/5.0 tag layout (no 5.4 tag extensions).
    """
    r = R(data)
    r.p = e.offset
    end = e.offset + e.size
    out = {}
    while r.p + 8 <= end:
        name = fname(r, p.names)
        if name == "None":
            break
        typ = fname(r, p.names)
        size, idx = r.i32(), r.i32()
        sub = None
        if typ == "StructProperty":
            sub = fname(r, p.names)
            if p.ue4 >= 441:
                r.skip(16)
        elif typ == "BoolProperty":
            sub = r.b[r.p]; r.p += 1
        elif typ in ("ByteProperty", "EnumProperty"):
            sub = fname(r, p.names)
        elif typ in ("ArrayProperty", "SetProperty"):
            sub = fname(r, p.names)
        elif typ == "MapProperty":
            sub = (fname(r, p.names), fname(r, p.names))
        if p.ue4 >= 282:
            has_guid = r.b[r.p]; r.p += 1
            if has_guid:
                r.skip(16)
        start = r.p
        if r.p + size > end:
            break
        v = None
        if typ == "IntProperty": v = r.i32()
        elif typ == "UInt32Property": v = r.u32()
        elif typ == "Int64Property": v = struct.unpack_from("<q", r.b, r.p)[0]
        elif typ == "FloatProperty": v = struct.unpack_from("<f", r.b, r.p)[0]
        elif typ == "DoubleProperty": v = struct.unpack_from("<d", r.b, r.p)[0]
        elif typ == "BoolProperty": v = bool(sub)
        elif typ == "NameProperty": v = ("name", fname(r, p.names))
        elif typ == "StrProperty": v = r.fstring()
        elif typ in ("ObjectProperty", "ClassProperty", "SoftObjectProperty", "InterfaceProperty"):
            ref = r.i32()
            v = ("obj", class_name(p, ref), ref)
        elif typ == "ByteProperty": v = r.b[r.p] if size == 1 else ("name", fname(r, p.names))
        elif typ == "StructProperty" and sub in ("Vector", "Rotator") and size in (12, 24):
            fmt = "<3f" if size == 12 else "<3d"
            v = ("vec", struct.unpack_from(fmt, r.b, r.p))
        else:
            v = ("raw", bytes(r.b[r.p:r.p + size]))
        key = name if idx == 0 else f"{name}[{idx}]"
        out[key] = v
        r.p = start + size
    return out
