// Prints the layout of a widget blueprint from the generated widget table (checked against tools/ue64/widgets.py by tests/test_widgets.py).
#include "u64_widgets.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv)
{
  float r[64][4]; int n = u64_widget_layout(argv[1], (float)atof(argv[2]), (float)atof(argv[3]), r, 64);
  for (int i = 0; i < n; i++) printf("%.4f %.4f %.4f %.4f\n", r[i][0], r[i][1], r[i][2], r[i][3]);
  return 0;
}
