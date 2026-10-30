"""Unreal material -> Tiny3D (Fast64-style) material extras. The numeric codes are the RDP combiner input codes;
checked against a real Fast64 export in the Tiny3D repo: A/B zero=8, C zero=16, D zero=7 / one=6, alpha zero=7."""
from __future__ import annotations

TEX0, SHADE = 1, 4
PRESET_OPAQUE, PRESET_CUTOUT, PRESET_TRANSPARENT = 1, 4, 5


def _cc(a, b, c, d, aa, ba, ca, da):
    return {"name": "", "A": a, "B": b, "C": c, "D": d, "A_alpha": aa, "B_alpha": ba, "C_alpha": ca, "D_alpha": da}


def build_extras(name, tex, alpha_mode, double_sided):
    """tex: dict from textures.fit_texture plus 'file' (png name) or None."""
    if tex is not None:
        # (TEX0 - 0) * SHADE + 0 ; alpha = texture alpha for cutout, else 1
        cc = _cc(TEX0, 8, SHADE, 7, 7, 7, 7, 1 if alpha_mode == "MASK" else 6)
    else:
        cc = _cc(8, 8, 16, SHADE, 7, 7, 7, 6)  # SHADE only
    preset = {"MASK": PRESET_CUTOUT, "BLEND": PRESET_TRANSPARENT}.get(alpha_mode, PRESET_OPAQUE)
    f3d = {
        "name": "", "combiner1": cc, "combiner2": dict(cc),
        "set_prim": 0, "prim_color": [1, 1, 1, 1], "set_env": 0, "env_color": [1, 1, 1, 1],
        "set_blend": 0, "blend_color": [0, 0, 0, 1],
        "rdp_settings": {
            "g_cull_front": 0, "g_cull_back": 0 if double_sided else 1, "g_fog": 0, "g_lighting": 0, "g_tex_gen": 0,
            "g_mdsft_cycletype": 0, "g_mdsft_text_filt": 2, "set_rendermode": 1,
            "rendermode_preset_cycle_1": preset, "rendermode_preset_cycle_2": preset,
        },
        "draw_layer": {"oot": 0, "sm64": 1},
    }
    if tex is not None:
        s = {"low": 0.0, "high": float(tex["width"] - 1), "mask": tex["mask_s"], "shift": 0}
        t = {"low": 0.0, "high": float(tex["height"] - 1), "mask": tex["mask_t"], "shift": 0}
        f3d["tex0"] = {"tex_set": 1, "tex": {"name": tex["file"], "type": "Image"}, "S": s, "T": t}
    return {"f3d_mat": f3d, "_name": name}
