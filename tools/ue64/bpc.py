"""Blueprint -> C compiler driver. python tools/ue64/bpc.py <Content dir> <out dir>"""
import os
import sys

sys.path.insert(0, os.path.dirname(__file__))
import bpscan  # noqa: E402
import codegen  # noqa: E402
import leveldata  # noqa: E402
import uasset  # noqa: E402


def main(content, out):
    os.makedirs(out, exist_ok=True)
    natives, unsup, nfun, files, reg, skipped = set(), {}, 0, 0, [], []
    defaults, maps_found = {}, []
    for root, _, names in os.walk(content):
        for fn in sorted(names):
            if not fn.endswith((".uasset", ".umap")):
                continue
            f = os.path.join(root, fn)
            p = uasset.parse_tables(f)
            data = open(f, "rb").read()
            defaults.update(leveldata.class_defaults(p, data))
            if fn.endswith(".umap"):
                found = leveldata.level_actors(p, data)
                maps_found.append((len(found), fn, found))
            funcs = [e for e in p.exports if uasset.class_name(p, e.cls) == "Function"]
            if not funcs:
                continue
            cls = os.path.splitext(fn)[0]
            chunks = ['#include "u64_rt.h"\n']
            for e in funcs:
                try:
                    st, _ = bpscan.decode_function(p, data, e)
                except Exception as ex:
                    skipped.append((cls, e.name, str(ex)[:60]))
                    continue
                params = uasset.function_params(p, data, e) or (["EntryPoint"] if e.name.startswith("ExecuteUbergraph") else [])
                src, g = codegen.gen_function(cls, e.name, st, params)
                chunks.append(src)
                reg.append((cls, e.name))
                natives |= g.natives
                for u in g.unsupported:
                    unsup[u] = unsup.get(u, 0) + 1
                nfun += 1
            with open(os.path.join(out, f"bp_{codegen.ident(cls)}.c"), "w") as fh:
                fh.write("\n".join(chunks))
            files += 1
    # The level to bake in: the map that places the most Blueprint actors whose classes we compiled.
    known = {c for c, _ in reg}
    maps_found.sort(key=lambda m: -sum(1 for a in m[2] if a[1] in known))
    actors = maps_found[0][2] if maps_found else []
    with open(os.path.join(out, "u64_defaults.c"), "w") as fh:
        fh.write(leveldata.emit_defaults_c(defaults))
    with open(os.path.join(out, "u64_level.c"), "w") as fh:
        fh.write(leveldata.emit_level_c(actors))
    with open(os.path.join(out, "bp_registry.c"), "w") as fh:
        fh.write('#include "u64_rt.h"\n')
        for cls, fn in reg:
            fh.write(f"void bp_{codegen.ident(cls)}__{codegen.ident(fn)}(u64_obj *, const u64_v *, int);\n")
        fh.write("static const u64_bpfn table[] = {\n")
        for cls, fn in reg:
            fh.write(f'  {{"{cls}", "{fn}", bp_{codegen.ident(cls)}__{codegen.ident(fn)}}},\n')
        fh.write("};\nconst u64_bpfn *u64_bp_registry(void) { return table; }\nint u64_bp_registry_count(void) { return sizeof table / sizeof table[0]; }\n")
    with open(os.path.join(out, "compat_report.txt"), "w") as fh:
        fh.write(f"functions compiled: {nfun} in {files} files\nunsupported constructs: {unsup}\nskipped (undecodable): {skipped}\n")
        import re
        rt = open(os.path.join(os.path.dirname(__file__), "..", "..", "runtime", "u64_natives.c")).read()
        impl = set(re.findall(r'IS\("([A-Za-z_0-9]+)"\)', rt))
        bpfn = {fn for _, fn in reg}
        external = sorted(n for n in natives if n not in bpfn)  # calls into engine/plugin code, not other compiled BPs
        done = [n for n in external if n in impl]
        missing = [n for n in external if n not in impl]
        fh.write(f"native coverage: {len(done)}/{len(external)} engine functions implemented in runtime/u64_natives.c\n")
        fh.write("missing (the runtime work queue):\n  " + "\n  ".join(missing) + "\n")
    print(f"{nfun} functions in {files} files; {len(natives)} distinct callees; unsupported: {unsup}")


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])
