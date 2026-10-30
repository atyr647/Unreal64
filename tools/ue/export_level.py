"""Export the open level of an UNMODIFIED Unreal project to glTF for n64fit, using Unreal's own glTF exporter.

Run headless against the project (nothing in the project is changed; the script lives outside it):
  UnrealEditor-Cmd <Project>.uproject -run=pythonscript -script="tools/ue/export_level.py <Map> <out.glb>"

STATUS: UNTESTED. There is no Unreal Engine in the dev container. The exporter class/option names below are from
memory of the UE 5.x Python API and are probed defensively; expect to adjust them on a real install.
"""
import sys

import unreal


def main(map_path, out_path):
    unreal.EditorLoadingAndSavingUtils.load_map(map_path)
    world = unreal.EditorLevelLibrary.get_editor_world()
    opts = unreal.GLTFExportOptions()
    # Lights are needed for the bake; n64fit shrinks textures itself.
    wanted = {"export_lights": True, "export_unlit_materials": True, "export_vertex_colors": True,
              "export_hidden_in_game": False}
    for name, value in wanted.items():
        try:
            opts.set_editor_property(name, value)
        except Exception as e:  # noqa: BLE001 - option names differ across engine versions
            unreal.log_warning(f"glTF option {name} not available: {e}")
    messages = unreal.GLTFExportMessages()
    ok = unreal.GLTFExporter.export_to_gltf(world, out_path, opts, set(), messages)
    unreal.log(f"glTF export {'succeeded' if ok else 'FAILED'}: {out_path}")
    if not ok:
        raise SystemExit(1)


if __name__ == "__main__":
    main(*sys.argv[1:3])
