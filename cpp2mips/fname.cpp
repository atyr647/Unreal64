// The string table behind FName (equal names share one pointer).
#include "ue_shim.h"
#include <stdlib.h>
const char *FName::intern(const char *n) {                         // a tiny string table: equal names share one pointer
  static const char *tab[256]; static int cnt;
  for (int i = 0; i < cnt; i++) if (!strcmp(tab[i], n)) return tab[i];
  return tab[cnt++] = strdup(n);
}

