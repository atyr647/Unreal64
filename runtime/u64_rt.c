// Minimal Unreal64 Blueprint runtime: named instance variables, persistent frames, function dispatch.
// Static storage only (no malloc) so it runs on the N64 as is. Natives are provided through u64_native_hook.
#include "u64_rt.h"
#include <stdio.h>
u64_v u64_native(u64_obj *self, const char *fn, int argc, const u64_v *argv);

#define MAX_OBJS 1024
#define ARENA_SLOTS 32768          // shared variable storage, one slot per variable

typedef struct slot { const char *k; u64_v v; struct slot *next; } slot_t;
typedef struct { slot_t *head, *tail; int n; } vars_t;       // singly linked: a slot's address never changes (delegate bindings key on it)
struct u64_obj { const char *name; const char *cls; vars_t iv, pf; int alive; };

static struct u64_obj objs[MAX_OBJS];
static int nobjs;
static struct u64_obj null_obj = {"None", "None"};
static slot_t arena[ARENA_SLOTS];
static int arena_used;
int u64_rt_overflows;              // non-zero means storage ran out: a test and the runtime report treat it as a failure

const u64_bpfn *u64_bp_table;   // set by the generated registry
int u64_bp_count;
u64_native_fn u64_native_hook;
u64_log_fn u64_log;

static void overflow(const char *what)
{
  if (!u64_rt_overflows++ && u64_log) u64_log("  [error] %s exhausted: variables would alias", what);
}

u64_obj *u64_new(const char *name, const char *cls)
{
  if (nobjs >= MAX_OBJS) { overflow("object pool"); return &null_obj; }
  struct u64_obj *o = &objs[nobjs++];
  o->name = name; o->cls = cls; o->alive = 1;
  // Construction order: the root C++ class first, each class's constructor then its Blueprint defaults, down to the leaf.
  const char *chain[10]; int n = 0;
  for (const char *c = cls; c && n < 10; c = u64_parent_of(c)) chain[n++] = c;
  for (int i = n - 1; i >= 0; i--) { u64_cpp_construct(o, chain[i]); u64_apply_defaults(o, chain[i]); }
  return o;
}

static u64_v *vars_find(vars_t *t, const char *k) { for (slot_t *s = t->head; s; s = s->next) if (s->k == k || !strcmp(s->k, k)) return &s->v; return 0; }
static u64_v *vars_get(vars_t *t, const char *k)
{
  u64_v *f = vars_find(t, k);
  if (f) return f;
  if (arena_used >= ARENA_SLOTS) { overflow("variable arena"); static slot_t dead; return &dead.v; }
  slot_t *s = &arena[arena_used++];
  s->k = k; memset(&s->v, 0, sizeof(u64_v)); s->next = 0;
  if (t->tail) t->tail->next = s; else t->head = s;
  t->tail = s; t->n++;
  return &s->v;
}

u64_v *u64_ivar(u64_obj *o, const char *name)   { if (!o) o = &null_obj; return vars_get(&o->iv, name); }
u64_v *u64_pframe(u64_obj *o, const char *name) { if (!o) o = &null_obj; return vars_get(&o->pf, name); }
u64_v *u64_member(u64_v *s, const char *name)   { static u64_v scratch[8]; static int i; if (s && s->t == U64_OBJ && s->u.o) return u64_ivar(s->u.o, name); return &scratch[i++ & 7]; }

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
void u64_obj_set_alive(u64_obj *o, int v) { if (o) o->alive = v; }
int u64_obj_is_actor(u64_obj *o)      { u64_v *l = o && o->alive ? vars_find(&o->iv, "Location") : 0; return l && l->t == U64_VEC; }   // actors have a location; components and tasks do not
void u64_obj_destroy(u64_obj *o)      { if (o) { o->alive = 0; if (u64_log) u64_log("  [destroy] %s", o->name); } }
u64_v u64_array_of(int n, const u64_v *items);
void u64_array_set(u64_v *a, int n, const u64_v *e) { *a = u64_array_of(n, e); }

static u64_v last_return;
void u64_set_return(u64_v v) { last_return = v; }

int u64_trace_calls;   // diagnostic: log every call (class::function and the result type)
u64_v u64_call(u64_obj *self, const char *fn, int argc, const u64_v *argv)
{
  if (!self) self = &null_obj;
  if (u64_trace_calls && u64_log) u64_log("    call %s::%s/%d", self->cls, fn, argc);
  for (const char *c = self->cls; c; c = u64_parent_of(c))              // Blueprint functions, leaf class first, then parents
  for (int i = 0; i < u64_bp_count; i++) {
    if (!strcmp(u64_bp_table[i].fn, fn) && !strcmp(u64_bp_table[i].cls, c)) {
      if (u64_log) u64_log("[bp] %s::%s", u64_bp_table[i].cls, fn);
      last_return = (u64_v){U64_NONE};
      u64_bp_table[i].impl(self, argv, argc);
      u64_v r = last_return; last_return = (u64_v){U64_NONE};
      return r;
    }
  }
  { u64_v cr; if (u64_cpp_dispatch(self, fn, argc, argv, &cr)) return cr; }          // ported C++ members
  u64_v r = u64_native(self, fn, argc, argv);
  if (r.t == U64_NONE && u64_native_hook) return u64_native_hook(self, fn, argc, argv);
  return r;
}

u64_v u64_comp(u64_obj *owner, const char *name)
{
  if (!owner) return (u64_v){U64_NONE};
  const char *k = u64_intern(name);
  char key[96]; snprintf(key, sizeof key, "comp:%s", k);
  u64_v *slot = u64_ivar(owner, u64_intern(key));
  if (slot->t == U64_OBJ) return *slot;
  char nm[64]; snprintf(nm, sizeof nm, "%s.%s", owner->name, name);
  u64_obj *c = u64_new(u64_intern(nm), "Component");
  *u64_ivar(c, "Owner") = u64_objv(owner);
  *slot = u64_objv(c);
  return *slot;
}
