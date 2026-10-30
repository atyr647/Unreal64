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
    p.export_count, _ = r.i32(), r.i32()
    p.import_count, _ = r.i32(), r.i32()
    r.p = name_off
    for _ in range(name_count):
        p.names.append(r.fstring())
        if p.ue4 >= 504:
            r.skip(4)  # case-preserving + non-case-preserving hashes
    return p
