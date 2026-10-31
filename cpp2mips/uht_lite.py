#!/usr/bin/env python3
"""UHT-lite: the part of Unreal Header Tool the gameplay code needs. For each header it writes <Name>.generated.h with the
GENERATED_BODY() expansions (Super / ThisClass / StaticClass) and a name -> member lookup for every UPROPERTY FGameplayAttributeData.

    uht_lite.py <out dir> <header>...
"""
import os, re, sys

CLASS = re.compile(r"^\s*(?:class|struct)\s+(?:\w+_API\s+)?(\w+)\s*(?::\s*(?:public\s+)?(\w+))?")
PROP = re.compile(r"^\s*FGameplayAttributeData\s+(\w+)\s*;")


def generate(header, out):
    lines = open(header, encoding="utf-8", errors="ignore").read().splitlines()
    base = os.path.splitext(os.path.basename(header))[0]
    fid = re.sub(r"\W", "_", base)
    res = ["#pragma once", f"#undef CURRENT_FILE_ID", f"#define CURRENT_FILE_ID {fid}"]
    res += [f"{k} {n};" for n, k in re.findall(r"^\s*(class|struct)\s+(?:\w+_API\s+)?(\w+)", "\n".join(lines), re.M) for n, k in [(k, n)] if False]
    for kind, name in re.findall(r"^\s*(class|struct)\s+(?:\w+_API\s+)?(\w+)\s*(?::|\{|$)", "\n".join(lines), re.M):
        res.append(f"{kind} {name};")
    cur, bases, props, kinds = None, {}, {}, {}
    for i, ln in enumerate(lines, 1):
        m = CLASS.match(ln)
        if m and ";" not in ln.split("{")[0]:
            cur = m.group(1); bases[cur] = m.group(2); kinds[cur] = ln.split()[0] if ln.split()[0] in ('class','struct') else 'class'; props.setdefault(cur, [])
        p = PROP.match(ln)
        if p and cur:
            props[cur].append(p.group(1))
        if re.search(r"GENERATED_(USTRUCT_)?BODY\(\)", ln) and cur:
            sup = f"typedef {bases[cur]} Super; " if bases.get(cur) else ""
            finders = ""
            if bases.get(cur) == "UAttributeSet" or (bases.get(cur) or "").startswith("UGS") and False:
                pass
            sstruct = f" static UScriptStruct *StaticStruct() {{ static UScriptStruct s; return &s; }}" if kinds.get(cur) == "struct" else ""
            inst = f" bool InstanceOf(const UClass *c) const override {{ return c == StaticClass() || Super::InstanceOf(c); }}" if bases.get(cur) and bases[cur][0] in "UA" else ""
            body = f"public: {sup}typedef {cur} ThisClass; static UClass *StaticClass() {{ static UClass c = {{\"{cur}\"}}; return &c; }}{inst}{sstruct}"
            res.append(f"#define {fid}_{i}_GENERATED_BODY {body} @FIND@{cur}@")
    text = "\n".join(res) + "\n"
    for cls, plist in props.items():
        if not plist:
            text = re.sub(r"@FIND@%s@" % cls, "", text); continue
        chain = "Super::FindAttributeData(n)" if bases.get(cls) else "nullptr"
        code = "".join(f"if (n == FName(\"{p}\")) return &{p}; " for p in plist)
        names = ", ".join(f'"{p}"' for p in plist)
        text = text.replace(f"@FIND@{cls}@", f"FGameplayAttributeData *FindAttributeData(FName n) override {{ {code}return {chain}; }} static const char *const *AttributeNames() {{ static const char *const a[] = {{{names}, 0}}; return a; }}")
    text = re.sub(r"@FIND@\w+@", "", text)
    path = os.path.join(out, base + ".generated.h")
    open(path, "w").write(text)
    return path


if __name__ == "__main__":
    os.makedirs(sys.argv[1], exist_ok=True)
    for h in sys.argv[2:]:
        print(generate(h, sys.argv[1]))
