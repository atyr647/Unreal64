// Prints the layout of a widget blueprint from the generated widget table (checked against tools/ue64/widgets.py by tests/test_widgets.py).
#include "u64_widgets.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv)
{
  if (argc > 1 && !strcmp(argv[1], "scale")) { float lw, lh; u64_ui_layout_size((float)atof(argv[2]), (float)atof(argv[3]), &lw, &lh); printf("%.4f %.4f %.4f\n", u64_ui_scale((float)atof(argv[2]), (float)atof(argv[3])), lw, lh); return 0; }
  float r[64][4]; int n = u64_widget_layout(argv[1], (float)atof(argv[2]), (float)atof(argv[3]), r, 64);
  for (int i = 0; i < n; i++) printf("%.4f %.4f %.4f %.4f\n", r[i][0], r[i][1], r[i][2], r[i][3]);
  return 0;
}
