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


def quat_to_rotator(x, y, z, w):
    """FQuat::Rotator(): (pitch, yaw, roll) in degrees, including the gimbal-lock poles."""
    import math
    test = z * x - w * y
    yaw_y, yaw_x = 2 * (w * z + x * y), 1 - 2 * (y * y + z * z)
    norm = lambda d: d - 360.0 * math.floor((d + 180.0) / 360.0)
    if test < -0.4999995:
        yaw = math.degrees(math.atan2(yaw_y, yaw_x)); return (-90.0, yaw, norm(-yaw - 2 * math.degrees(math.atan2(x, w))))
    if test > 0.4999995:
        yaw = math.degrees(math.atan2(yaw_y, yaw_x)); return (90.0, yaw, norm(yaw - 2 * math.degrees(math.atan2(x, w))))
    return (math.degrees(math.asin(2 * test)), math.degrees(math.atan2(yaw_y, yaw_x)), math.degrees(math.atan2(-2 * (w * x + y * z), 1 - 2 * (x * x + y * y))))


CLASS_FNS = {}          # class -> names of the functions its Blueprint defines (filled by bpc.py before generating)


class Gen:
    cls = ""

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
            return self.lv(n.args[3], f"u64_asobj({obj})")
        if n.op == "StructMemberContext":
            return f"(*u64_member(&{self.lv(n.args[1], self_expr)}, {cstr(last(n.args[0]))}))"
        self.unsupported.append(n.op + " (as l-value)")
        return "L._unsupported"

    def ex(self, n, s="self", c=None):
        # s: object the expression runs against (changes under Context); c: the caller frame, where call arguments
        # are evaluated (Unreal evaluates arguments before switching to the callee object).
        c = c or s
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
        if o == "TransformConst":                                  # (rotation quat xyzw, translation, scale): keeps location + rotation
            return f"u64_xform_new(u64_vec({a[4]!r}f, {a[5]!r}f, {a[6]!r}f), u64_vec({', '.join(repr(v) + 'f' for v in quat_to_rotator(*a[:4]))}))"
        if o == "ArrayGetByRef": return f"u64_array_get({self.ex(a[0], s)}, u64_as_int({self.ex(a[1], s)}))"
        if o == "Nothing": return "u64_int(0)"
        if o == "PrimitiveCast": return self.ex(a[1], s)
        if o == "DynamicCast":                                           # fails (null) when the object is not of that class; classes the runtime does not know pass
            cname = last(a[0]); cname = cname[:-2] if cname.endswith("_C") else cname
            return f"u64_cast({self.ex(a[1], s)}, {cstr(cname)})"
        if o in ("MetaCast", "ObjToInterfaceCast", "CrossInterfaceCast", "InterfaceToObjCast"): return self.ex(a[1], s)
        if o in ("FinalFunction", "CallMath", "LocalFinalFunction"):
            return self.call(a[0], a[1], s, c, final=(o != "CallMath"))
        if o in ("VirtualFunction", "LocalVirtualFunction"):
            return self.call(a[0], a[1], s, c)
        if o in ("Context", "Context_FailSilent", "ClassContext"):
            obj = self.ex(a[0], c)
            return f"({{ u64_v _o = {obj}; {self.ex(a[3], 'u64_asobj(_o)', c)}; }})"
        if o == "TextConst":
            kind = a[0]
            if kind in (1, 2, 3) and len(a) > 1: return self.ex(a[1], s)      # localized text: its source string; invariant text; literal string
            return 'u64_str("")'
        if o == "SwitchValue":
            _, _, idx, cases, default = a
            sel = self.ex(idx, s)
            out = self.ex(default, s)
            for cv, _, res in reversed(cases):
                out = f"(u64_as_int(_sw) == u64_as_int({self.ex(cv, s)}) ? {self.ex(res, s)} : {out})"
            return f"({{ u64_v _sw = {sel}; {out}; }})"
        if o == "InterfaceContext": return self.ex(a[0], s)
        if o == "SkipOffsetConst": return f"u64_int({a[0]})"
        if o == "FieldPathConst": return self.ex(a[0], s)
        if o == "StructConst":
            sname, fields = a[0], a[2]
            if sname == "LatentActionInfo" and len(fields) >= 4:   # (linkage, uuid, function, target)
                return f"u64_latent({self.ex(fields[3], s)}, {self.ex(fields[2], s)}.u.s, {self.ex(fields[0], s)}.u.i)"
            if sname in ("GameplayTag", "CollisionProfileName", "GameplayAttribute") and fields:
                return self.ex(fields[0], s)    # these structs are identified by their name
            return "u64_int(0) /* struct const */"
        self.unsupported.append(o)
        return f'u64_int(0) /* UNSUPPORTED {o} */'

    def arg(self, p, s, name, i):
        # Out-parameters: LocalOutVariable always; Break* natives also write their trailing args.
        # The compiler names the temporaries that receive a call's outputs CallFunc_<Function>_<Parameter>; a caller passes such a
        # variable (as a plain local) for every out parameter, so it is passed by reference.
        if p.op in ("LocalVariable", "LocalOutVariable", "InstanceVariable") and p.args and last(p.args[0]).startswith(f"CallFunc_{name}_") \
                and not last(p.args[0]).endswith("_ReturnValue"):
            return f"u64_ref(&{self.lv(p, s)})"
        if p.op == "LocalOutVariable" or (name.startswith("Array_") and i == 0 and p.op in ("LocalVariable", "InstanceVariable", "DefaultVariable", "StructMemberContext")) \
                or (name.startswith("Break") and i > 0 and p.op in ("LocalVariable", "InstanceVariable")):
            return f"u64_ref(&{self.lv(p, s)})"
        return self.ex(p, s)

    def call(self, fn, params, s, c=None, final=False):
        c = c or s
        name = last(fn)
        self.natives.add(name)
        args = ", ".join(self.arg(p, c, name, i) for i, p in enumerate(params))
        owner = getattr(fn, "owner", None)
        owner = owner[:-2] if owner and owner.endswith("_C") else owner
        # "Call Parent Function": a non-virtual call of an inherited function that this class overrides. It must start the lookup
        # above this class, otherwise the override calls itself.
        if final and s == "self" and owner and owner != self.cls and name in CLASS_FNS.get(self.cls, ()):
            return f"U64_CALL_SUPER({cstr(self.cls)}, {cstr(name)}{', ' + args if args else ''})"
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
            if a[0].op == "Nothing": return "goto u64_ret;"
            return f"L.ReturnValue = {self.ex(a[0])}; goto u64_ret;"
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


def order_params(params, stmts):
    """Property export order is not declaration order. Event stubs copy their parameters into persistent-frame variables
    one statement per parameter, in pin order, so that order is the declaration order; any other parameters keep export order."""
    names = {n for n, _ in params}
    seen = []
    for st in stmts:
        if st.op == "LetValueOnPersistentFrame" and isinstance(st.args[1], Node) and st.args[1].op == "LocalVariable":
            v = last(st.args[1].args[0])
            if v in names and v not in seen:
                seen.append(v)
    by_name = dict(params)
    ordered = [(n, by_name[n]) for n in seen]
    return ordered + [(n, o) for n, o in params if n not in seen]


def gen_function(cls, fname, stmts, params=()):
    params = order_params(list(params), stmts) if params and not fname.startswith("ExecuteUbergraph") else params
    """params: [(name, is_out)] in declaration order. Out/reference parameters are written back to the caller on return."""
    uber = fname.startswith("ExecuteUbergraph")
    g = Gen(uber)
    g.cls = cls
    body = [(n, g.stmt(n)) for n in stmts]
    starts = [n.mem for n, _ in body]
    cases = " ".join(f"case {m}: goto L_{m};" for m in starts)
    lines = []
    for n, text in body:
        text = text.replace("DISPATCH_CASES", cases)
        lines.append(f"  L_{n.mem}:; {text}")
    pnames = [pn for pn, _ in params]
    names = sorted((g.locals | {"ReturnValue", "_unsupported"} | {ident(x) for x in pnames}) if not uber else {"ReturnValue", "_unsupported"})
    locs = "".join(f" u64_v {n};" for n in names)
    bind, back = [], []
    for i, (pn, is_out) in enumerate(params):
        tgt = f"(*u64_pframe(self, {cstr(pn)}))" if uber else f"L.{ident(pn)}"
        if is_out and not uber:
            bind.append(f"  if (argc > {i}) {tgt} = args[{i}].t == U64_REF ? *args[{i}].u.r : args[{i}];")
            back.append(f"  if (argc > {i}) u64_out(args[{i}], {tgt});")
        else:
            bind.append(f"  if (argc > {i}) {tgt} = args[{i}];")
    src = (f"void bp_{ident(cls)}__{ident(fname)}(u64_obj *self, const u64_v *args, int argc) {{\n"
           f"  (void)args; (void)argc;\n  struct {{{locs} }} L; memset(&L, 0, sizeof L); (void)L;\n"
           f"  U64_FLOW_DECL; (void)flow_n;\n" + "\n".join(bind + lines) +
           "\n  u64_ret:; u64_set_return(L.ReturnValue);\n" + "\n".join(back) + "\n}\n")
    return src, g
