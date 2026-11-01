"""UMG layout: the canvas rules (tools/ue64/widgets.py layout) on hand-made widgets with exact values, and Python vs the C layout (runtime/u64_widgets.c) on the project's real HUD
when GASSHOOTER_CONTENT points at its Content directory."""
import os, subprocess, sys, tempfile, unittest

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, ROOT + "/tools/ue64")
import widgets  # noqa: E402


def W(parent, anchors, offsets, align=(0.0, 0.0), auto=False, typ=0, **kw):
    w = {"name": "w", "cls": "", "parent": parent, "type": typ, "visible": 0, "slot": {"anchors": anchors, "offsets": offsets, "align": align, "autosize": auto, "z": 0},
         "text": "", "font": 24.0, "image": (0.0, 0.0)}
    w.update(kw); return w


class CanvasRules(unittest.TestCase):
    def test_point_anchor_offsets_and_alignment(self):
        # anchored at the bottom-centre of a 1280x720 viewport, 300x75, centred horizontally, hanging 100 above the bottom: pos = anchor + offset - align * size
        ws = [W(-1, (0, 0, 0, 0), (0, 0, 0, 0)), W(0, (0.5, 1, 0.5, 1), (0, -100, 300, 75), align=(0.5, 0.0))]
        r = widgets.layout(ws, 1280, 720)
        self.assertEqual(r[1], (640 - 150, 720 - 100, 300, 75))

    def test_stretch_anchors_use_margins(self):
        # stretched horizontally with 10 left / 30 right margins, top-anchored 40 tall
        ws = [W(-1, (0, 0, 0, 0), (0, 0, 0, 0)), W(0, (0, 0, 1, 0), (10, 5, 30, 40))]
        self.assertEqual(widgets.layout(ws, 1000, 500)[1], (10, 5, 1000 - 30 - 10, 40))

    def test_autosize_uses_the_measured_content(self):
        ws = [W(-1, (0, 0, 0, 0), (0, 0, 0, 0)), W(0, (1, 1, 1, 1), (-20, -10, 0, 0), align=(1.0, 1.0), auto=True, typ=3, image=(64.0, 32.0))]
        r = widgets.layout(ws, 800, 600, measure=lambda w: w["image"])
        self.assertEqual(r[1], (800 - 20 - 64, 600 - 10 - 32, 64, 32))

    def test_nested_panels_offset_by_the_parent(self):
        ws = [W(-1, (0, 0, 0, 0), (0, 0, 0, 0)), W(0, (0, 0, 0, 0), (100, 50, 400, 300)), W(1, (0.5, 0.5, 0.5, 0.5), (0, 0, 40, 20), align=(0.5, 0.5))]
        r = widgets.layout(ws, 1280, 720)
        self.assertEqual(r[2], (100 + 200 - 20, 50 + 150 - 10, 40, 20))

    def test_switcher_children_fill_it(self):
        ws = [W(-1, (0, 0, 0, 0), (0, 0, 0, 0)), W(0, (0, 0, 0, 0), (10, 10, 200, 100), typ=4), W(1, (0, 0, 0, 0), (0, 0, 5, 5))]
        r = widgets.layout(ws, 800, 600); self.assertEqual(r[2], r[1])


def ui_scale(w, h):
    side = min(w, h); keys = [(480, 4 / 9), (720, 2 / 3), (1080, 1.0), (8640, 8.0)]
    if side <= keys[0][0]: return keys[0][1]
    for (a, b), (c, d) in zip(keys, keys[1:]):
        if side <= c: return b + (d - b) * (side - a) / (c - a)
    return keys[-1][1]


class RealHud(unittest.TestCase):
    @unittest.skipUnless(os.environ.get("GASSHOOTER_CONTENT"), "set GASSHOOTER_CONTENT to the project's Content directory")
    def test_python_and_c_layout_agree(self):
        content = os.environ["GASSHOOTER_CONTENT"]
        with tempfile.TemporaryDirectory() as t:
            subprocess.check_call([sys.executable, ROOT + "/tools/ue64/bpc.py", content, t], stdout=subprocess.DEVNULL)
            exe = t + "/w"
            # the C layout only needs the table: build without the whole runtime
            stub = t + "/stub.c"
            with open(stub, "w") as fh: fh.write('#include "u64_rt.h"\nu64_obj *u64_new(const char *a, const char *b) { return 0; } u64_v *u64_ivar(u64_obj *o, const char *n) { static u64_v v; return &v; } const char *u64_intern(const char *s) { return s; }\n')
            subprocess.check_call(["gcc", "-w", "-I" + ROOT + "/runtime", "-o", exe, ROOT + "/tests/widgets_test.c", ROOT + "/runtime/u64_widgets.c", t + "/u64_widget_data.c", stub, "-lm"])
            for vw, vh in ((1280, 720), (1920, 1080), (640, 480), (3840, 2160), (800, 450)):          # the UI scale rule: layout space = viewport / scale (16:9 at 720 and 1080 lines -> 1920 x 1080)
                sc, lw, lh = [float(x) for x in subprocess.check_output([exe, "scale", str(vw), str(vh)], text=True).split()]
                self.assertAlmostEqual(sc, ui_scale(vw, vh), places=3); self.assertAlmostEqual(lw, vw / ui_scale(vw, vh), places=1)
            self.assertAlmostEqual(ui_scale(1280, 720) * 1920, 1280, places=6); self.assertAlmostEqual(ui_scale(1920, 1080), 1.0)
            tree = widgets.read_widget_tree(content + "/GASShooter/UI/UI_HUD.uasset")
            ws = tree["widgets"]
            def meas(w):
                if w["type"] == 3: return w["image"]
                if w["type"] == 1: return (len(w["text"]) * w["font"] * 0.5, w["font"] * 1.2)
                return (0.0, 0.0)
            for vw, vh in ((1280, 720), (1920, 1080)):
                py = widgets.layout(ws, vw, vh, measure=meas)
                out = subprocess.check_output([exe, "UI_HUD", str(vw), str(vh)], text=True).split("\n")
                rows = [[float(x) for x in l.split()] for l in out if l]
                self.assertEqual(len(rows), len(py))
                for a, b in zip(rows, py):
                    for x, y in zip(a, b): self.assertAlmostEqual(x, y, places=2)


if __name__ == "__main__":
    unittest.main()
