// The string table behind FName (equal names share one pointer).
#include "ue_shim.h"
#include <stdlib.h>
const char *FName::intern(const char *n) {                         // a tiny string table: equal names share one pointer
  static const char *tab[256]; static int cnt;
  for (int i = 0; i < cnt; i++) if (!strcmp(tab[i], n)) return tab[i];
  return tab[cnt++] = strdup(n);
}


#include <stdio.h>
void ShimUnimplemented(const char *what)                      // each distinct name once, to stderr, when UE_SHIM_REPORT is set
{
  static const char *seen[512]; static int n; static const char *on = getenv("UE_SHIM_REPORT");
  for (int i = 0; i < n; i++) if (seen[i] == what || !strcmp(seen[i], what)) return;
  if (n < 512) seen[n++] = what;
  if (on) fprintf(stderr, "[shim] unimplemented: %s\n", what);
}
