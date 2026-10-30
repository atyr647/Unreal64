// Minimal Unreal64 Blueprint runtime: named instance variables, persistent frames, function dispatch.
// Static storage only (no malloc) so it runs on the N64 as is. Natives are provided through u64_native_hook.
#include "u64_rt.h"
#include <stdio.h>
u64_v u64_native(u64_obj *self, const char *fn, int argc, const u64_v *argv);

#define MAX_OBJS 64
#define MAX_VARS 48

typedef struct { const char *k; u64_v v; } slot_t;
struct u64_obj { const char *name; const char *cls; slot_t iv[MAX_VARS]; int niv; slot_t pf[MAX_VARS]; int npf; int alive; };

static struct u64_obj objs[MAX_OBJS];
static int nobjs;
static struct u64_obj null_obj = {"None", "None"};

const u64_bpfn *u64_bp_table;   // set by the generated registry
int u64_bp_count;
u64_native_fn u64_native_hook;
u64_log_fn u64_log;

u64_obj *u64_new(const char *name, const char *cls)
{
  if (nobjs >= MAX_OBJS) return &null_obj;
  struct u64_obj *o = &objs[nobjs++];
  o->name = name; o->cls = cls; o->alive = 1;
  u64_apply_defaults(o, cls);
  return o;
}

static u64_v *slot(slot_t *a, int *n, const char *k)
{
  for (int i = 0; i < *n; i++) if (!strcmp(a[i].k, k)) return &a[i].v;
  if (*n >= MAX_VARS) return &a[MAX_VARS - 1].v;
  a[*n].k = k; memset(&a[*n].v, 0, sizeof(u64_v));
  return &a[(*n)++].v;
}

u64_v *u64_ivar(u64_obj *o, const char *name)   { if (!o) o = &null_obj; return slot(o->iv, &o->niv, name); }
u64_v *u64_pframe(u64_obj *o, const char *name) { if (!o) o = &null_obj; return slot(o->pf, &o->npf, name); }
u64_v *u64_member(u64_v *s, const char *name)   { static u64_v scratch[8]; static int i; (void)s; (void)name; return &scratch[i++ & 7]; }

u64_obj *u64_find(const char *name)
{
  for (int i = 0; i < nobjs; i++) if (!strcmp(objs[i].name, name)) return &objs[i];
  return u64_new(name, name);
}

u64_v u64_delegate(u64_v obj, const char *fn)  { u64_v r = {U64_DELEGATE}; r.u.s = fn; r.dobj = obj.u.o; return r; }

// Multicast delegates: bindings keyed by the address of the (stable) variable slot.
#define MAX_BINDS 64
static struct { u64_v *m; u64_v d; } binds[MAX_BINDS];
static int nbinds;
void u64_mc_add(u64_v *m, u64_v d)    { if (nbinds < MAX_BINDS) { binds[nbinds].m = m; binds[nbinds++].d = d; } }
void u64_mc_remove(u64_v *m, u64_v d) { for (int i = 0; i < nbinds; i++) if (binds[i].m == m && binds[i].d.dobj == d.dobj && !strcmp(binds[i].d.u.s, d.u.s)) { binds[i] = binds[--nbinds]; return; } }
void u64_mc_clear(u64_v *m)           { for (int i = 0; i < nbinds; ) { if (binds[i].m == m) binds[i] = binds[--nbinds]; else i++; } }
void u64_mc_broadcast(u64_v *m, int argc, const u64_v *argv)
{
  u64_v snap[MAX_BINDS]; int n = 0;
  for (int i = 0; i < nbinds; i++) if (binds[i].m == m) snap[n++] = binds[i].d;
  for (int i = 0; i < n; i++) u64_call(snap[i].dobj, snap[i].u.s, argc, argv);
}
const char *u64_obj_name(u64_obj *o)  { return o->name; }
const char *u64_obj_class(u64_obj *o) { return o->cls; }
int u64_obj_count(void)               { return nobjs; }
u64_obj *u64_obj_at(int i)            { return &objs[i]; }
int u64_obj_alive(u64_obj *o)         { return o && o->alive; }
void u64_obj_destroy(u64_obj *o)      { if (o) o->alive = 0; }
void u64_array_set(u64_v *a, int n, const u64_v *e) { (void)a; (void)n; (void)e; }

u64_v u64_call(u64_obj *self, const char *fn, int argc, const u64_v *argv)
{
  if (!self) self = &null_obj;
  for (int i = 0; i < u64_bp_count; i++) {
    if (!strcmp(u64_bp_table[i].fn, fn) && (!strcmp(u64_bp_table[i].cls, self->cls) || !strcmp(self->cls, "*"))) {
      if (u64_log) u64_log("[bp] %s::%s", u64_bp_table[i].cls, fn);
      u64_bp_table[i].impl(self, argv, argc);
      return *u64_ivar(self, "ReturnValue");
    }
  }
  u64_v r = u64_native(self, fn, argc, argv);
  if (r.t == U64_NONE && u64_native_hook) return u64_native_hook(self, fn, argc, argv);
  return r;
}
