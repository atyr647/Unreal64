// UMG layout and the HUD's draw list. The canvas rules are those of tools/ue64/widgets.py layout() (checked against it by tests/test_widgets.py).
#include "u64_widgets.h"
#include <string.h>
#include <math.h>

static int block(const char *bp, int *first)                          // the widgets of a blueprint are contiguous in the table
{
  int n = 0; *first = -1;
  for (int i = 0; i < u64_widget_count; i++) if (!strcmp(u64_widget_defs[i].bp, bp)) { if (*first < 0) *first = i; n++; }
  return n;
}
static void measure(const u64_widget_def *w, float *mw, float *mh)    // desired size of auto-sized slots: images by their brush, text by a fixed-pitch estimate (the engine measures the font)
{
  if (w->type == 3) { *mw = w->image[0]; *mh = w->image[1]; }
  else if (w->type == 1) { *mw = (float)strlen(w->text) * w->font * 0.5f; *mh = w->font * 1.2f; }
  else { *mw = 0; *mh = 0; }
}
int u64_widget_layout(const char *bp, float vw, float vh, float (*rects)[4], int max)
{
  int first, n = block(bp, &first); if (n > max) n = max;
  for (int i = 0; i < n; i++) {
    const u64_widget_def *w = &u64_widget_defs[first + i];
    if (w->parent < 0) { rects[i][0] = 0; rects[i][1] = 0; rects[i][2] = vw; rects[i][3] = vh; continue; }
    const float *pr = rects[w->parent]; const u64_widget_def *par = &u64_widget_defs[first + w->parent];
    if (par->type == 4) { memcpy(rects[i], pr, sizeof rects[i]); continue; }       // a switcher's child fills it
    float msz[2]; measure(w, &msz[0], &msz[1]); float start[2], size[2];
    for (int ax = 0; ax < 2; ax++) {
      float a0 = w->anchors[ax], a1 = w->anchors[ax + 2], psz = ax ? pr[3] : pr[2], so = w->offsets[ax], eo = w->offsets[ax + 2];
      if (a0 == a1) { size[ax] = w->autosize ? msz[ax] : eo; start[ax] = a0 * psz + so - w->align[ax] * size[ax]; }
      else { start[ax] = a0 * psz + so; size[ax] = a1 * psz - eo - start[ax]; }
    }
    rects[i][0] = pr[0] + start[0]; rects[i][1] = pr[1] + start[1]; rects[i][2] = size[0]; rects[i][3] = size[1];
  }
  return n;
}
void u64_widget_instantiate(u64_obj *uw, const char *bp)
{
  static const char *const cls[] = {"CanvasPanel", "TextBlock", "ProgressBar", "Image", "WidgetSwitcher"};
  int first, n = block(bp, &first);
  for (int i = 0; i < n; i++) {
    const u64_widget_def *w = &u64_widget_defs[first + i];
    u64_obj *o = u64_new(u64_intern(w->name), w->type >= 0 ? cls[w->type] : "Widget");
    *u64_ivar(o, "Visibility") = u64_int(w->visible);
    if (w->type == 1) *u64_ivar(o, "Text") = u64_str(w->text);
    if (w->type == 2) *u64_ivar(o, "Percent") = u64_float(w->percent);
    if (w->type == 4) *u64_ivar(o, "ActiveWidgetIndex") = u64_int(w->active);
    *u64_ivar(uw, u64_intern(w->name)) = u64_objv(o);
  }
}
static int shown(int vis) { return vis == 0 || vis == 3 || vis == 4; }
static float fget(u64_v v, float d) { return v.t == U64_FLOAT ? v.u.f : (v.t == U64_INT || v.t == U64_BOOL) ? (float)v.u.i : d; }
int u64_widget_draw_list(u64_obj *uw, const char *bp, float vw, float vh, u64_draw_cmd *out, int max)
{
  float rects[64][4]; int first, n = block(bp, &first); if (n > 64) n = 64;
  u64_widget_layout(bp, vw, vh, rects, 64);
  int vis[64], child_no[64], cnt[64]; memset(cnt, 0, sizeof cnt);
  u64_obj *ob[64];
  for (int i = 0; i < n; i++) {
    const u64_widget_def *w = &u64_widget_defs[first + i];
    u64_v *ov = u64_ivar(uw, u64_intern(w->name)); ob[i] = ov->t == U64_OBJ ? ov->u.o : 0;
    int v = ob[i] ? (int)fget(*u64_ivar(ob[i], "Visibility"), (float)w->visible) : w->visible;
    child_no[i] = w->parent >= 0 ? cnt[w->parent]++ : 0;
    int ok = shown(v);
    if (ok && w->parent >= 0) {
      const u64_widget_def *par = &u64_widget_defs[first + w->parent];
      ok = vis[w->parent];
      if (ok && par->type == 4) ok = child_no[i] == (int)fget(*u64_ivar(ob[w->parent], "ActiveWidgetIndex"), (float)par->active);
    }
    vis[i] = ok;
  }
  int k = 0;
  for (int pass = 0; pass < 2; pass++)                                  // slot z-order: z 0 widgets first (tree order), then the raised ones
    for (int i = 0; i < n && k < max; i++) {
      const u64_widget_def *w = &u64_widget_defs[first + i]; if (!vis[i] || (w->z != 0) != pass) continue;
      u64_draw_cmd *c = &out[k]; memset(c, 0, sizeof *c); c->x = rects[i][0]; c->y = rects[i][1]; c->w = rects[i][2]; c->h = rects[i][3]; c->name = w->name;
      if (w->type == 3) {
        memcpy(c->rgba, w->tint, sizeof c->rgba); c->kind = w->image[0] > 0 || w->tint[3] > 0 ? 0 : 2;
        if (w->parent >= 0 && w->tint[3] <= 0) continue;
        if (w->tint[0] == 1 && w->tint[1] == 1 && w->tint[2] == 1 && w->image[0] > 0) c->kind = 2;        // a texture brush (not available): a placeholder frame
        k++;
      } else if (w->type == 2) {
        float p = ob[i] ? fget(*u64_ivar(ob[i], "Percent"), w->percent) : w->percent; if (p < 0) p = 0; if (p > 1) p = 1;
        memcpy(c->rgba, w->back, sizeof c->rgba); k++;
        if (k < max) { u64_draw_cmd *f = &out[k++]; memset(f, 0, sizeof *f); f->x = c->x; f->y = c->y; f->w = c->w * p; f->h = c->h; memcpy(f->rgba, w->fill, sizeof f->rgba); f->name = w->name; }
      } else if (w->type == 1) {
        u64_v t = ob[i] ? *u64_ivar(ob[i], "Text") : u64_str(w->text);
        c->kind = 1; c->text = t.t == U64_STR || t.t == U64_NAME ? t.u.s : w->text; c->justify = w->justify; c->font = w->font; memcpy(c->rgba, w->color, sizeof c->rgba);
        if (!c->text || !*c->text) continue;
        k++;
      }
    }
  return k;
}
