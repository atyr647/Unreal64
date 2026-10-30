"""Lower decoded Kismet bytecode to C (GCC computed goto) against runtime/u64_rt.h."""
from __future__ import annotations

import re

from kismet import Node


def ident(s):
    s = re.sub(r"[^A-Za-z0-9_]", "_", str(s))
    return ("_" + s) if s[:1].isdigit() else (s or "_")


def cstr(s):
    return '"' + str(s).replace("\\", "\\\\").replace('"', '\\"').replace("\n", "\\n").replace("\r", "\\r") + '"'


def last(path):
    return ident(str(path).split(".")[-1])


class Gen:
    def __init__(self, ubergraph=False):
        self.locals, self.natives, self.unsupported = set(), set(), []
        self.targets = set()
        self.uber = ubergraph  # event-graph locals live in the object's persistent frame

    # ---- l-values (assignment targets) and r-values
    def lv(self, n, self_expr="self"):
        if n.op in ("LocalVariable", "LocalOutVariable"):
            self.locals.add(last(n.args[0]))
            if self.uber:
                return f"(*u64_pframe(self, {cstr(last(n.args[0]))}))"
            return f"L.{last(n.args[0])}"
        if n.op in ("InstanceVariable", "DefaultVariable"):
            return f"(*u64_ivar({self_expr}, {cstr(last(n.args[0]))}))"
        if n.op == "Context":
            obj = self.ex(n.args[0], self_expr)
            return self.lv(n.args[3], f"({obj}).u.o")
        if n.op == "StructMemberContext":
            return f"(*u64_member(&{self.lv(n.args[1], self_expr)}, {cstr(last(n.args[0]))}))"
        self.unsupported.append(n.op + " (as l-value)")
        return "L._unsupported"

    def ex(self, n, s="self"):
        o, a = n.op, n.args
        if o in ("LocalVariable", "LocalOutVariable", "InstanceVariable", "DefaultVariable", "StructMemberContext"):
            return self.lv(n, s)
        if o == "IntConst": return f"u64_int({a[0]})"
        if o == "FloatConst": return f"u64_float({a[0]!r}f)"
        if o in ("DoubleConst",): return f"u64_float({a[0]!r}f)"
        if o in ("IntZero",): return "u64_int(0)"
        if o in ("IntOne",): return "u64_int(1)"
        if o in ("IntConstByte", "ByteConst"): return f"u64_int({a[0]})"
        if o in ("Int64Const", "UInt64Const"): return f"u64_int({a[0] & 0x7FFFFFFF})"
        if o == "True": return "u64_bool(1)"
        if o == "False": return "u64_bool(0)"
        if o in ("StringConst", "UnicodeStringConst"): return f"u64_str({cstr(a[0])})"
        if o == "NameConst": return f"u64_name({cstr(a[0])})"
        if o in ("NoObject", "NoInterface"): return "u64_objv(0)"
        if o == "Self": return f"u64_objv({s})"
        if o == "ObjectConst": return f"u64_objv(u64_find({cstr(a[0])}))"
        if o in ("VectorConst", "Vector3fConst", "RotationConst"): return f"u64_vec({a[0]!r}f, {a[1]!r}f, {a[2]!r}f)"
        if o == "Nothing": return "u64_int(0)"
        if o == "PrimitiveCast": return self.ex(a[1], s)
        if o in ("MetaCast", "DynamicCast", "ObjToInterfaceCast", "CrossInterfaceCast", "InterfaceToObjCast"): return self.ex(a[1], s)
        if o in ("FinalFunction", "CallMath", "LocalFinalFunction"):
            return self.call(a[0], a[1], s)
        if o in ("VirtualFunction", "LocalVirtualFunction"):
            return self.call(a[0], a[1], s)
        if o in ("Context", "Context_FailSilent", "ClassContext"):
            obj = self.ex(a[0], s)
            return f"({{ u64_v _o = {obj}; {self.ex(a[3], '_o.u.o')}; }})"
        if o == "TextConst":
            kind = a[0]
            if kind in (2, 3) and len(a) > 1: return self.ex(a[1], s)
            return 'u64_str("")'
        if o == "SwitchValue":
            _, _, idx, cases, default = a
            sel = self.ex(idx, s)
            out = self.ex(default, s)
            for cv, _, res in reversed(cases):
                out = f"(u64_as_int(_sw) == u64_as_int({self.ex(cv, s)}) ? {self.ex(res, s)} : {out})"
            return f"({{ u64_v _sw = {sel}; {out}; }})"
        if o == "InterfaceContext": return self.ex(a[0], s)
        if o == "StructConst":
            return "u64_int(0) /* struct const */"
        self.unsupported.append(o)
        return f'u64_int(0) /* UNSUPPORTED {o} */'

    def call(self, fn, params, s):
        name = last(fn)
        self.natives.add(name)
        args = ", ".join(self.ex(p, s) for p in params)
        return f"U64_CALL({s}, {cstr(name)}{', ' + args if args else ''})"

    # ---- statements
    def stmt(self, n):
        o, a = n.op, n.args
        if o in ("Tracepoint", "WireTracepoint", "Breakpoint", "InstrumentationEvent", "EndOfScript", "Nothing"):
            return ""
        if o in ("Let", "LetObj", "LetWeakObjPtr", "LetBool", "LetDelegate", "LetMulticastDelegate"):
            lhs, rhs = (a[1], a[2]) if o == "Let" else (a[0], a[1])
            return f"{self.lv(lhs)} = {self.ex(rhs)};"
        if o == "LetValueOnPersistentFrame":
            return f"*u64_pframe(self, {cstr(last(a[0]))}) = {self.ex(a[1])};"
        if o == "Return":
            if a[0].op == "Nothing": return "return;"
            return f"L.ReturnValue = {self.ex(a[0])}; return;"
        if o == "Jump":
            self.targets.add(a[0]); return f"goto L_{a[0]};"
        if o == "JumpIfNot":
            self.targets.add(a[0]); return f"if (!u64_true({self.ex(a[1])})) goto L_{a[0]};"
        if o == "PushExecutionFlow":
            self.targets.add(a[0]); return f"U64_PUSH(&&L_{a[0]});"
        if o == "PopExecutionFlow": return "goto *U64_POP();"
        if o == "PopExecutionFlowIfNot": return f"if (!u64_true({self.ex(a[0])})) goto *U64_POP();"
        if o == "ComputedJump": return f"u64_dispatch: switch (u64_as_int({self.ex(a[0])})) {{ DISPATCH_CASES }}"
        if o == "SkipOffsetConst": return ""
        if o == "BindDelegate":
            return f"{self.lv(a[1])} = u64_delegate({self.ex(a[2])}, {cstr(a[0])});"
        if o == "AddMulticastDelegate": return f"u64_mc_add(&{self.lv(a[0])}, {self.ex(a[1])});"
        if o == "RemoveMulticastDelegate": return f"u64_mc_remove(&{self.lv(a[0])}, {self.ex(a[1])});"
        if o == "ClearMulticastDelegate": return f"u64_mc_clear(&{self.lv(a[0])});"
        if o == "SetArray":
            elems = ", ".join(self.ex(x) for x in a[1])
            return f"u64_array_set(&{self.lv(a[0])}, {len(a[1])}, (u64_v[]){{u64_int(0){', ' + elems if elems else ''}}} + 1);"
        return self.ex(n) + ";"


def gen_function(cls, fname, stmts, params=()):
    uber = fname.startswith("ExecuteUbergraph")
    g = Gen(uber)
    body = [(n, g.stmt(n)) for n in stmts]
    starts = [n.mem for n, _ in body]
    cases = " ".join(f"case {m}: goto L_{m};" for m in starts)
    lines = []
    for n, text in body:
        text = text.replace("DISPATCH_CASES", cases)
        lines.append(f"  L_{n.mem}:; {text}")
    names = sorted((g.locals | {"ReturnValue", "_unsupported"} | set(params)) if not uber else {"ReturnValue", "_unsupported"})
    locs = "".join(f" u64_v {n};" for n in names)
    bind = []
    for i, pn in enumerate(params):
        tgt = f"(*u64_pframe(self, {cstr(pn)}))" if uber else f"L.{ident(pn)}"
        bind.append(f"  if (argc > {i}) {tgt} = args[{i}];")
    src = (f"void bp_{ident(cls)}__{ident(fname)}(u64_obj *self, const u64_v *args, int argc) {{\n"
           f"  (void)args; (void)argc;\n  struct {{{locs} }} L; memset(&L, 0, sizeof L); (void)L;\n"
           f"  U64_FLOW_DECL; (void)flow_n;\n" + "\n".join(bind + lines) + "\n}\n")
    return src, g
