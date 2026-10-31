"""Decode every Function export in a project; report coverage. python tools/ue64/bpscan.py <Content dir>"""
import collections
import glob
import os
import sys

sys.path.insert(0, os.path.dirname(__file__))
import kismet  # noqa: E402
import uasset  # noqa: E402


def decode_function(p, data, e):
    """Returns (stmts, decoder) for the first candidate that decodes and validates, else raises."""
    def resolve(i):
        if i < 0 and -i - 1 < len(p.imports):
            return p.imports[-i - 1][3]
        if i > 0 and i - 1 < len(p.exports):
            return p.exports[i - 1].name
        return "None"
    last = None
    for buf, ser, script in uasset.find_scripts(data, e):
        # dialects are tried in turn and confirmed by the in-memory size + jump targets: pointer encoding (object index vs
        # field path, with or without the trailing owner index) and the pre-4.1x Let without a property operand
        modes = [(fp, ol, ow) for fp, ow in (((False, False), (True, False), (True, True)) if p.ue4 >= 518 or p.ue5 else ((False, False),))
                 for ol in ((False, True) if p.ue4 < 500 else (False,))]
        for field_path, old_let, owner in modes:
            try:
                d = kismet.Decoder(script, p.names, resolve, p.ue4, p.ue5, field_path, old_let, owner)
                st = d.run()
                if d.mem != buf:
                    raise kismet.KismetError(f"mem {d.mem} != {buf}")
                bad = d.check_jumps(st)
                if bad:
                    raise kismet.KismetError(f"bad jumps {bad[:2]}")
                return st, d
            except Exception as ex:  # try the next candidate / pointer encoding
                last = ex
    raise kismet.KismetError(str(last) if last else "no script")


def main(content):
    ok, fails, ops = 0, [], collections.Counter()
    for f in sorted(glob.glob(content + "/**/*.uasset", recursive=True) + glob.glob(content + "/**/*.umap", recursive=True)):
        p = uasset.parse_tables(f)
        data = open(f, "rb").read()
        for e in p.exports:
            if uasset.class_name(p, e.cls) != "Function":
                continue
            try:
                st, _ = decode_function(p, data, e)
                ok += 1
                stack = list(st)
                while stack:
                    n = stack.pop()
                    if isinstance(n, kismet.Node):
                        ops[n.op] += 1
                        stack += n.args
                    elif isinstance(n, list):
                        stack += n
            except Exception as ex:
                fails.append((os.path.relpath(f, content), e.name, str(ex)[:70]))
    print(f"functions decoded+validated: {ok}, failed: {len(fails)}")
    for x in fails[:10]:
        print("  ", x)
    print(ops.most_common(12))
    return 0 if not fails else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1]))
