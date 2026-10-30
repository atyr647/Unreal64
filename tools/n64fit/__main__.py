"""python -m n64fit <level.glb> <outdir> [--tris N] [--cell METRES] [--samples N] [--bounces 0|1]"""
import argparse
import sys

from .pipeline import convert


def main(argv=None):
    ap = argparse.ArgumentParser(prog="n64fit", description=__doc__)
    ap.add_argument("src")
    ap.add_argument("outdir")
    ap.add_argument("--tris", type=int, default=4000, help="total triangle budget after decimation")
    ap.add_argument("--cell", type=float, default=0.0, help="split into square cells of this size in metres (0 = one cell)")
    ap.add_argument("--samples", type=int, default=24, help="GI/AO rays per vertex (0 = flat ambient)")
    ap.add_argument("--bounces", type=int, default=1, choices=(0, 1))
    ap.add_argument("--ambient", default="0.25,0.28,0.35", help="sky colour in normalised light units")
    ap.add_argument("--exposure", default="auto")
    a = ap.parse_args(argv)
    m = convert(a.src, a.outdir, a.tris, a.cell, a.samples, a.bounces, tuple(float(v) for v in a.ambient.split(",")), a.exposure)
    print(f"{len(m['cells'])} cell(s), {m['triangles_out']} triangles, {m['textures']} texture(s) -> {a.outdir}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
