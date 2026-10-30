"""python tools/ue64/inventory.py <ProjectDir>  — what an Unreal project contains, from package headers alone."""
import collections
import glob
import os
import sys

sys.path.insert(0, os.path.dirname(__file__))
import uasset  # noqa: E402

KINDS = ("StaticMesh", "SkeletalMesh", "Material", "MaterialInstanceConstant", "Texture2D", "Blueprint", "WidgetBlueprint",
         "AnimBlueprint", "AnimSequence", "AnimMontage", "SoundWave", "ParticleSystem", "NiagaraSystem", "Landscape", "DataTable")


def main(project):
    root = os.path.join(project, "Content")
    kinds, maps, bps, bad = collections.Counter(), [], [], 0
    files = glob.glob(root + "/**/*.uasset", recursive=True) + glob.glob(root + "/**/*.umap", recursive=True)
    for f in files:
        try:
            p = uasset.parse(f)
        except Exception:
            bad += 1
            continue
        rel = os.path.relpath(f, root)
        names = set(p.names)
        kinds.update(k for k in KINDS if k in names)
        if f.endswith(".umap"):
            maps.append((rel, p.export_count))
        if "BlueprintGeneratedClass" in names:
            bps.append(rel)
    print(f"{len(files)} packages ({bad} unreadable)")
    print({k: v for k, v in kinds.items()})
    print("maps:", maps)
    print(f"{len(bps)} Blueprint classes")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1]))
