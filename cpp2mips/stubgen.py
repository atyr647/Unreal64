#!/usr/bin/env python3
"""stubgen.py <Class> <header>... : empty definitions for every virtual / override member a header declares (the class's vtable needs them to link).
Used for engine-facing project classes the build does not compile (AGSHeroCharacter, AGSWeapon): the runtime owns their behaviour; the proxy only has to exist."""
import re, sys
cls = sys.argv[1]
text = "\n".join(open(h, encoding="utf-8", errors="ignore").read() for h in sys.argv[2:])
text = re.sub(r"/\*.*?\*/", "", text, flags=re.S); text = re.sub(r"//[^\n]*", "", text)
m = re.search(r"class\s+\w*_?API\s+%s\b[^{]*\{(.*?)\n\};" % cls, text, re.S) or re.search(r"class\s+%s\b[^{]*\{(.*?)\n\};" % cls, text, re.S)
body = m.group(1)
out = []
for decl, term in re.findall(r"((?:virtual|[\w:<>\*& ]+?)\s+\w+\s*\([^;{]*\)\s*(?:const)?\s*(?:override)?\s*)(;|\{)", body):
    if term == "{": continue                       # defined inline in the header
    if "virtual" not in decl and "override" not in decl: continue
    if "~" in decl or "UFUNCTION" in decl: continue
    d = decl.replace("virtual", "").replace("override", "").replace("static", "").strip()
    mm = re.match(r"(.*?)(\w+)\s*\((.*)\)\s*(const)?$", d, re.S)
    if not mm: continue
    ret, name, args, const = mm.group(1).strip(), mm.group(2), mm.group(3), mm.group(4) or ""
    args = re.sub(r"\s*=\s*[^,)]+", "", args)
    ret = ret or "void"
    val = "" if ret == "void" else (" return %s();" % ret if "*" not in ret else " return nullptr;")
    out.append(f"{ret} {cls}::{name}({args}) {const} {{{val} }}")
print("\n".join(sorted(set(out))))
