#pragma once
// UMG widget blueprints at runtime: the widget table exported from the WidgetBlueprints (tools/ue64/widgets.py), instances (one object per widget, its state in properties
// the Blueprint's SetText / SetPercent / SetVisibility natives write), layout in a viewport, and the list of things to draw.
#include "u64_rt.h"
typedef struct {
  const char *bp, *name; int parent, type, visible;                 // type: 0 canvas panel, 1 text, 2 progress bar, 3 image, 4 widget switcher; visible: ESlateVisibility
  float anchors[4], offsets[4], align[2]; int autosize, z;           // the canvas slot
  const char *text; float font; int justify; float color[4];         // text block (justify: 0 left, 1 center, 2 right)
  float percent, fill[4], back[4]; float image[2], tint[4];          // progress bar / image
  int active, variable;
} u64_widget_def;
extern const u64_widget_def u64_widget_defs[];
extern const int u64_widget_count;

typedef struct { int kind; float x, y, w, h; float rgba[4]; const char *text; int justify; float font; const char *name; } u64_draw_cmd;     // kind: 0 rectangle, 1 text, 2 image placeholder
// The UI scale of a viewport (UUserInterfaceSettings, no project override): the "shortest side" rule with the default curve (480 -> 4/9, 720 -> 2/3, 1080 -> 1.0, 8640 -> 8.0, linear in between,
// constant below the first key). Widgets are laid out in viewport / scale units: a 16:9 viewport of 720 or 1080 lines gives the 1920 x 1080 layout space the HUD was designed in.
float u64_ui_scale(float vw, float vh);
void u64_ui_layout_size(float vw, float vh, float *lw, float *lh);
void u64_widget_instantiate(u64_obj *userwidget, const char *bp);                    // one object per widget, named like the widget, with the asset's defaults
int u64_widget_layout(const char *bp, float vw, float vh, float (*rects)[4], int max);   // rectangle (x, y, w, h) of every widget of a blueprint; returns the count
int u64_widget_draw_list(u64_obj *userwidget, const char *bp, float vw, float vh, u64_draw_cmd *out, int max);
