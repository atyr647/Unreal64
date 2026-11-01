"""UMG widget blueprints -> a flat widget table (layout, text, colours, defaults) for the HUD.

Reads the WidgetTree of a WidgetBlueprint: panels (CanvasPanel, WidgetSwitcher), TextBlock, ProgressBar, Image. A canvas slot's layout is its anchors, offsets, alignment,
auto-size flag and z-order; UMG's rules for turning them into a rectangle are in `layout()` (also in runtime/u64_widgets.c, checked against each other by tests/test_widgets.py).
Engine defaults used where the asset leaves a property out: font size 24, text / bar fill white, switcher active index 0, anchors (0,0)-(0,0), canvas slot offsets (0, 0, 100, 30).
"""
import struct
import uasset, props

DEFAULT_FONT = 24.0
TYPES = {"CanvasPanel": 0, "TextBlock": 1, "ProgressBar": 2, "Image": 3, "WidgetSwitcher": 4}
VIS = {"Visible": 0, "Collapsed": 1, "Hidden": 2, "HitTestInvisible": 3, "SelfHitTestInvisible": 4}
JUST = {"Left": 0, "Center": 1, "Right": 2}


def _floats(v, n):
    if isinstance(v, tuple) and v[0] == "raw" and len(v[1]) >= 4 * n:
        return struct.unpack("<%df" % n, v[1][:4 * n])
    return None


def _color(v, default=(1.0, 1.0, 1.0, 1.0)):
    """FLinearColor raw, or a FSlateColor struct holding SpecifiedColor."""
    if isinstance(v, tuple) and v[0] == "struct":
        v = v[1].get("SpecifiedColor", v[1].get("TintColor"))
        if isinstance(v, tuple) and v[0] == "struct":
            v = v[1].get("SpecifiedColor")
    f = _floats(v, 4)
    return tuple(f) if f else default


def parse_ftext(raw):
    """FText as stored in the asset: flags, history type, namespace / key / source strings (the source string is the displayed text for a literal)."""
    if not (isinstance(raw, tuple) and raw[0] == "raw"):
        return ""
    b = raw[1]; pos = 5; strs = []
    for _ in range(3):
        if pos + 4 > len(b):
            break
        n = struct.unpack_from("<i", b, pos)[0]; pos += 4
        if n <= 0 or pos + n > len(b):
            break
        strs.append(b[pos:pos + n - 1].decode("utf-8", "replace")); pos += n
    return strs[2] if len(strs) == 3 else ""


CANVAS_SLOT_OFFSETS = (0.0, 0.0, 100.0, 30.0)         # UCanvasPanelSlot's default FAnchorData offsets (a new widget is 100 x 30); the asset stores only the fields that differ from them


def _margin(m):
    d = m[1] if isinstance(m, tuple) and m[0] == "struct" else {}
    return tuple(float(d.get(k, dflt)) for k, dflt in zip(("Left", "Top", "Right", "Bottom"), CANVAS_SLOT_OFFSETS))


def read_widget_tree(path):
    p = uasset.parse_tables(path); data = open(path, "rb").read()
    tree = next((e for e in p.exports if uasset.class_name(p, e.cls) == "WidgetTree"), None)
    if tree is None:
        return None
    root = props.read_object(p, data, tree).get("RootWidget")
    if not (isinstance(root, tuple) and root[0] == "obj"):
        return None
    widgets = []; index = {}

    def obj(ref):
        return p.exports[ref[2] - 1] if isinstance(ref, tuple) and ref[0] == "obj" and 0 < ref[2] <= len(p.exports) else None

    def walk(exp, parent, slot):
        cn = uasset.class_name(p, exp.cls); o = props.read_object(p, data, exp)
        w = {"name": exp.name, "cls": cn, "parent": parent, "type": TYPES.get(cn, -1), "visible": VIS.get(str(o.get("Visibility", ("enum", "Visible"))[1]).split("::")[-1], 0) if isinstance(o.get("Visibility"), tuple) else 0,
             "slot": slot, "text": "", "font": DEFAULT_FONT, "justify": 0, "color": (1.0, 1.0, 1.0, 1.0), "percent": 0.0, "fill": (1.0, 1.0, 1.0, 1.0), "back": (0.0, 0.0, 0.0, 0.5), "image": (0.0, 0.0),
             "tint": (1.0, 1.0, 1.0, 1.0), "resource": "", "variable": bool(o.get("bIsVariable", True))}
        if cn == "TextBlock":
            w["text"] = parse_ftext(o.get("Text"))
            f = o.get("Font")
            if isinstance(f, tuple) and f[0] == "struct":
                w["font"] = float(f[1].get("Size", DEFAULT_FONT))
            j = o.get("Justification")
            if isinstance(j, tuple):
                w["justify"] = JUST.get(str(j[1]).split("::")[-1], 0)
            w["color"] = _color(o.get("ColorAndOpacity"))
        elif cn == "ProgressBar":
            w["percent"] = float(o.get("Percent", 0.0)); w["fill"] = _color(o.get("FillColorAndOpacity"))
            st = o.get("WidgetStyle")
            if isinstance(st, tuple) and st[0] == "struct":
                bg = st[1].get("BackgroundImage"); fi = st[1].get("FillImage")
                if isinstance(bg, tuple) and bg[0] == "struct":
                    w["back"] = _color(bg[1].get("TintColor"), w["back"])
                if isinstance(fi, tuple) and fi[0] == "struct":
                    fc = _color(fi[1].get("TintColor"), (1.0, 1.0, 1.0, 1.0)); w["fill"] = tuple(a * b for a, b in zip(w["fill"], fc))
        elif cn == "Image":
            br = o.get("Brush")
            if isinstance(br, tuple) and br[0] == "struct":
                sz = _floats(br[1].get("ImageSize"), 2); w["image"] = tuple(sz) if sz else (0.0, 0.0)
                w["tint"] = _color(br[1].get("TintColor"))
                r = br[1].get("ResourceObject")
                if isinstance(r, tuple) and r[0] == "obj":
                    w["resource"] = r[1]
        elif cn == "WidgetSwitcher":
            w["active"] = int(o.get("ActiveWidgetIndex", 0))
        idx = len(widgets); widgets.append(w); index[exp.name] = idx
        if cn in ("CanvasPanel", "WidgetSwitcher"):
            for sref in (o.get("Slots") or ("array", []))[1]:
                se = obj(sref)
                if se is None:
                    continue
                so = props.read_object(p, data, se); ce = obj(so.get("Content"))
                if ce is None:
                    continue
                walk(ce, idx, _slot(so))
        return idx

    walk(obj(root), -1, _slot({}))
    return {"blueprint": p.exports[0].name if False else path.rsplit("/", 1)[-1].split(".")[0], "widgets": widgets}


def _slot(so):
    ld = so.get("LayoutData")
    d = ld[1] if isinstance(ld, tuple) and ld[0] == "struct" else {}
    an = d.get("Anchors"); an = an[1] if isinstance(an, tuple) and an[0] == "struct" else {}
    mn = _floats(an.get("Minimum"), 2) or (0.0, 0.0); mx = _floats(an.get("Maximum"), 2) or (0.0, 0.0)
    al = _floats(d.get("Alignment"), 2) or (0.0, 0.0)
    off = _margin(d.get("Offsets"))
    return {"anchors": (mn[0], mn[1], mx[0], mx[1]), "offsets": off, "align": al, "autosize": bool(so.get("bAutoSize", False)), "z": int(so.get("ZOrder", 0))}


def layout(widgets, vw, vh, measure=lambda w: (0.0, 0.0)):
    """Rectangle (x, y, w, h) of every widget in a viewport vw x vh, from UMG's canvas rules:
    per axis, anchors a0 == a1: position = a0 * parent + offset_start - alignment * size, size = offset_end (the 'Right' / 'Bottom' field is the size);
    anchors a0 != a1 (stretch): start = a0 * parent + offset_start, end = a1 * parent - offset_end.
    Auto-size slots take the content's desired size (measure); a widget switcher gives each child the switcher's whole rectangle; the root fills the viewport."""
    rects = [None] * len(widgets)
    for i, w in enumerate(widgets):
        if w["parent"] < 0:
            rects[i] = (0.0, 0.0, float(vw), float(vh)); continue
        px, py, pw, ph = rects[w["parent"]]
        par = widgets[w["parent"]]
        if par["type"] == 4:                                  # switcher child: the parent's rectangle
            rects[i] = (px, py, pw, ph); continue
        s = w["slot"]; a = s["anchors"]; o = s["offsets"]; al = s["align"]; out = []
        for axis in (0, 1):
            a0, a1 = a[axis], a[axis + 2]; psz = pw if axis == 0 else ph; start_off, end_off = o[axis], o[axis + 2]
            if a0 == a1:
                size = measure(w)[axis] if s["autosize"] else end_off
                start = a0 * psz + start_off - al[axis] * size
            else:
                start = a0 * psz + start_off; size = a1 * psz - end_off - start
            out.append((start, size))
        rects[i] = (px + out[0][0], py + out[1][0], out[0][1], out[1][1])
    return rects


def emit_c(trees):
    """C table: u64_widget_def (blueprint, name, parent, type, visible, anchors, offsets, align, autosize, z, text, font, justify, color, percent, fill, back, image, tint, active)."""
    def cs(s):
        return '"' + str(s).replace("\\", "\\\\").replace('"', '\\"') + '"'
    rows = []
    for t in trees:
        for w in t["widgets"]:
            s = w["slot"]
            f = lambda xs: ", ".join("%rf" % float(x) for x in xs)
            rows.append("  {%s, %s, %d, %d, %d, {%s}, {%s}, {%s}, %d, %d, %s, %rf, %d, {%s}, %rf, {%s}, {%s}, {%s}, {%s}, %d, %d}" % (
                cs(t["blueprint"]), cs(w["name"]), w["parent"], w["type"], w["visible"], f(s["anchors"]), f(s["offsets"]), f(s["align"]), int(s["autosize"]), s["z"], cs(w["text"]),
                w["font"], w["justify"], f(w["color"]), w["percent"], f(w["fill"]), f(w["back"]), f(w["image"]), f(w["tint"]), w.get("active", 0), int(w["variable"])))
    return "#include \"u64_widgets.h\"\nconst u64_widget_def u64_widget_defs[] = {\n" + ",\n".join(rows) + "\n};\nconst int u64_widget_count = %d;\n" % len(rows)
