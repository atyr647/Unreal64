"""Kismet bytecode (EExprToken) decoder for UE 4.2x-5.x packages.

Disk vs memory: the script is stored with 4-byte object references and 8-byte names, but jump offsets index the
*in-memory* script, where object pointers are 8 bytes and names 12. We track both so jump targets can be resolved.
"""
from __future__ import annotations

import struct
from dataclasses import dataclass, field

# token values (Script.h EExprToken)
T = dict(LocalVariable=0x00, InstanceVariable=0x01, DefaultVariable=0x02, Return=0x04, Jump=0x06, JumpIfNot=0x07,
         Assert=0x09, Nothing=0x0B, Let=0x0F, ClassContext=0x12, MetaCast=0x13, LetBool=0x14, EndParmValue=0x15,
         EndFunctionParms=0x16, Self=0x17, Skip=0x18, Context=0x19, Context_FailSilent=0x1A, VirtualFunction=0x1B,
         FinalFunction=0x1C, IntConst=0x1D, FloatConst=0x1E, StringConst=0x1F, ObjectConst=0x20, NameConst=0x21,
         RotationConst=0x22, VectorConst=0x23, ByteConst=0x24, IntZero=0x25, IntOne=0x26, True_=0x27, False_=0x28,
         TextConst=0x29, NoObject=0x2A, TransformConst=0x2B, IntConstByte=0x2C, NoInterface=0x2D, DynamicCast=0x2E,
         StructConst=0x2F, EndStructConst=0x30, SetArray=0x31, EndArray=0x32, PropertyConst=0x33,
         UnicodeStringConst=0x34, Int64Const=0x35, UInt64Const=0x36, DoubleConst=0x37, PrimitiveCast=0x38,
         SetSet=0x39, EndSet=0x3A, SetMap=0x3B, EndMap=0x3C, SetConst=0x3D, EndSetConst=0x3E, MapConst=0x3F,
         EndMapConst=0x40, Vector3fConst=0x41, StructMemberContext=0x42, LetMulticastDelegate=0x43,
         LetDelegate=0x44, LocalVirtualFunction=0x45, LocalFinalFunction=0x46, LocalOutVariable=0x48,
         InstanceDelegate=0x4B, PushExecutionFlow=0x4C, PopExecutionFlow=0x4D, ComputedJump=0x4E,
         PopExecutionFlowIfNot=0x4F, Breakpoint=0x50, InterfaceContext=0x51, ObjToInterfaceCast=0x52,
         EndOfScript=0x53, CrossInterfaceCast=0x54, InterfaceToObjCast=0x55, WireTracepoint=0x5A,
         SkipOffsetConst=0x5B, AddMulticastDelegate=0x5C, ClearMulticastDelegate=0x5D, Tracepoint=0x5E,
         LetObj=0x5F, LetWeakObjPtr=0x60, BindDelegate=0x61, RemoveMulticastDelegate=0x62,
         CallMulticastDelegate=0x63, LetValueOnPersistentFrame=0x64, ArrayConst=0x65, EndArrayConst=0x66,
         SoftObjectConst=0x67, CallMath=0x68, SwitchValue=0x69, InstrumentationEvent=0x6A, ArrayGetByRef=0x6B,
         ClassSparseDataVariable=0x6C, FieldPathConst=0x6D)
NAME = {v: k.rstrip("_") for k, v in T.items()}


class KismetError(ValueError):
    pass


@dataclass
class Node:
    off: int       # disk offset
    mem: int       # in-memory offset (jump target space)
    op: str
    args: list = field(default_factory=list)


class Decoder:
    def __init__(self, script: bytes, names, resolve, ue4=517, ue5=0, field_path=None):
        self.b, self.p, self.mem = script, 0, 0
        self.names, self.resolve = names, resolve  # resolve(pkg_index) -> str
        self.field_path = (ue4 >= 518 or ue5 > 0) if field_path is None else field_path  # FFieldPath operands (4.25+)
        self.ue5 = ue5
        self.lwc = ue5 >= 1004                       # doubles in vector/rotator/transform constants
        self.starts: dict[int, Node] = {}

    # --- primitives (advance disk pos and in-memory pos separately)
    def _take(self, fmt, mem=None):
        sz = struct.calcsize(fmt)
        if self.p + sz > len(self.b):
            raise KismetError("read past end")
        v = struct.unpack_from(fmt, self.b, self.p)[0]
        self.p += sz
        self.mem += sz if mem is None else mem
        return v

    def u8(self): return self._take("<B")
    def u16(self): return self._take("<H")
    def u32(self): return self._take("<I")
    def i32(self): return self._take("<i")
    def f32(self): return self._take("<f")
    def f64(self): return self._take("<d")

    def pkg(self):
        return self.resolve(self._take("<i", 8))

    def fname(self):
        i, n = struct.unpack_from("<ii", self.b, self.p)
        if self.p + 8 > len(self.b):
            raise KismetError("read past end")
        self.p += 8
        self.mem += 12
        s = self.names[i] if 0 <= i < len(self.names) else f"<bad{i}>"
        return s if n == 0 else f"{s}_{n - 1}"

    def prop(self):
        if not self.field_path:
            return self.pkg()
        mem0 = self.mem
        n = self.i32()
        path = [self.fname() for _ in range(n)]  # names of the path
        if self.ue5:
            self.i32()                           # UE5 also stores the owning struct's package index
        self.mem = mem0 + 8                      # in memory the whole path is one 8-byte pointer
        return path[0] if path else ""            # innermost first: the property name, then its owners

    def cstr(self):
        e = self.b.index(b"\0", self.p)
        s = self.b[self.p:e].decode("latin-1")
        self.mem += e + 1 - self.p
        self.p = e + 1
        return s

    def ustr(self):
        e = self.p
        while self.b[e:e + 2] != b"\0\0":
            e += 2
            if e > len(self.b):
                raise KismetError("unterminated unicode string")
        s = self.b[self.p:e].decode("utf-16-le")
        self.mem += e + 2 - self.p
        self.p = e + 2
        return s

    # --- expressions
    def until(self, end_tok):
        out = []
        while True:
            if self.p >= len(self.b):
                raise KismetError("unterminated list")
            if self.b[self.p] == end_tok:
                self.u8()
                return out
            out.append(self.expr())

    def expr(self) -> Node:
        off, mem = self.p, self.mem
        tok = self.u8()
        if tok not in NAME:
            raise KismetError(f"unknown token {tok:#x} at {off}")
        n = Node(off, mem, NAME[tok])
        self.starts[mem] = n
        a, e = n.args, self.expr
        t = tok
        if t in (0x00, 0x01, 0x02, 0x48, 0x6C): a.append(self.prop())
        elif t == 0x04: a.append(e())
        elif t == 0x06: a.append(self.u32())
        elif t == 0x07: a += [self.u32(), e()]
        elif t == 0x09: a += [self.u16(), self.u8(), e()]
        elif t == 0x0F: a += [self.prop(), e(), e()]
        elif t in (0x12, 0x19, 0x1A): a += [e(), self.u32(), self.prop(), e()]
        elif t in (0x13, 0x2E, 0x52, 0x54, 0x55): a += [self.pkg(), e()]
        elif t in (0x14, 0x43, 0x44, 0x5C, 0x5F, 0x60, 0x62, 0x6B): a += [e(), e()]
        elif t == 0x5D: a.append(e())
        elif t == 0x18: a += [self.u32(), e()]
        elif t in (0x1B, 0x45): a += [self.fname(), self.until(0x16)]
        elif t in (0x1C, 0x46, 0x68): a += [self.pkg(), self.until(0x16)]
        elif t == 0x1D: a.append(self.i32())
        elif t == 0x1E: a.append(self.f32())
        elif t == 0x1F: a.append(self.cstr())
        elif t == 0x34: a.append(self.ustr())
        elif t == 0x20: a.append(self.pkg())
        elif t == 0x21: a.append(self.fname())
        elif t in (0x22, 0x23): a += [self.f64() if self.lwc else self.f32() for _ in range(3)]
        elif t == 0x41: a += [self.f32() for _ in range(3)]
        elif t == 0x2B: a += [self.f64() if self.lwc else self.f32() for _ in range(10)]
        elif t in (0x24, 0x2C): a.append(self.u8())
        elif t in (0x35, 0x36): a.append(self._take("<q" if t == 0x35 else "<Q"))
        elif t == 0x37: a.append(self.f64())
        elif t == 0x29: self._text(a)
        elif t == 0x2F: a += [self.pkg(), self.i32(), self.until(0x30)]
        elif t == 0x31: a += [e(), self.until(0x32)]
        elif t == 0x33: a.append(self.prop())
        elif t == 0x38: a += [self.u8(), e()]
        elif t == 0x39: a += [e(), self.i32(), self.until(0x3A)]
        elif t == 0x3B: a += [e(), self.i32(), self.until(0x3C)]
        elif t == 0x3D: a += [self.prop(), self.i32(), self.until(0x3E)]
        elif t == 0x3F: a += [self.prop(), self.prop(), self.i32(), self.until(0x40)]
        elif t == 0x42: a += [self.prop(), e()]
        elif t == 0x4B: a.append(self.fname())
        elif t == 0x4C: a.append(self.u32())
        elif t in (0x4E, 0x4F, 0x51, 0x67, 0x6D): a.append(e())
        elif t == 0x5B: a.append(self.u32())
        elif t == 0x61: a += [self.fname(), e(), e()]
        elif t == 0x63: a += [self.pkg(), e(), self.until(0x16)]
        elif t == 0x64: a += [self.prop(), e()]
        elif t == 0x65: a += [self.prop(), self.i32(), self.until(0x66)]
        elif t == 0x69: self._switch(a)
        elif t == 0x6A:
            kind = self.u8()
            a.append(kind)
            if kind == 4:
                a.append(self.fname())
        # remaining tokens carry no operands
        return n

    def _text(self, a):
        kind = self.u8()
        a.append(kind)
        if kind == 1:
            a += [self.expr(), self.expr(), self.expr()]
        elif kind in (2, 3):
            a.append(self.expr())
        elif kind == 4:
            a += [self.pkg(), self.expr(), self.expr()]
        elif kind not in (0,):
            raise KismetError(f"bad text const kind {kind}")

    def _switch(self, a):
        count, end = self.u16(), self.u32()
        a += [count, end, self.expr()]
        cases = []
        for _ in range(count):
            cases.append((self.expr(), self.u32(), self.expr()))
        a += [cases, self.expr()]

    def run(self):
        out = []
        while self.p < len(self.b):
            out.append(self.expr())
        if not out or out[-1].op != "EndOfScript":
            raise KismetError("script does not end with EndOfScript")
        return out

    def check_jumps(self, stmts):
        """Every jump target must be the start of a token (memory space)."""
        bad = []

        def walk(n):
            if not isinstance(n, Node):
                if isinstance(n, (list, tuple)):
                    for x in n: walk(x)
                return
            if n.op in ("Jump", "PushExecutionFlow") or n.op == "JumpIfNot":
                if n.args[0] not in self.starts:
                    bad.append((n.off, n.op, n.args[0]))
            for x in n.args: walk(x)
        for s in stmts: walk(s)
        return bad
