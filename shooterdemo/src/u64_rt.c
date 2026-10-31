// Minimal Unreal64 Blueprint runtime: named instance variables, persistent frames, function dispatch.
// Static storage only (no malloc) so it runs on the N64 as is. Natives are provided through u64_native_hook.
#include "u64_rt.h"
#include <stdio.h>
#include <stddef.h>
void u64_array_gc_begin(void); void u64_array_gc_sweep(void); void u64_array_gc_mark(u64_v a);
u64_v u64_native(u64_obj *self, const char *fn, int argc, const u64_v *argv);

#define MAX_OBJS 2048
#define ARENA_SLOTS 32768          // shared variable storage, one slot per variable

typedef struct slot { const char *k; u64_v v; struct slot *next; struct u64_obj *owner; } slot_t;
typedef struct { slot_t *head, *tail; int n; struct u64_obj *owner; } vars_t;       // singly linked: a slot's address never changes (delegate bindings key on it)
struct u64_obj { const char *name; const char *cls; vars_t iv, pf; int alive; char namebuf[40]; };

static struct u64_obj objs[MAX_OBJS];
static int nobjs;
static struct u64_obj null_obj = {"None", "None"};
static slot_t arena[ARENA_SLOTS];
static int arena_used;
static slot_t *free_slots;                    // slots of collected objects
static int free_objs[MAX_OBJS], n_free_objs;
int u64_rt_overflows;              // non-zero means storage ran out: a test and the runtime report treat it as a failure

const u64_bpfn *u64_bp_table;   // set by the generated registry
int u64_bp_count;
u64_native_fn u64_native_hook;
u64_log_fn u64_log;

static void overflow(const char *what)
{
  if (!u64_rt_overflows++ && u64_log) u64_log("  [error] %s exhausted: variables would alias", what);
}

static struct u64_obj *obj_take(void)
{
  struct u64_obj *o;
  if (n_free_objs) o = &objs[free_objs[--n_free_objs]];
  else if (nobjs >= MAX_OBJS) { overflow("object pool"); return 0; }
  else o = &objs[nobjs++];
  memset(o, 0, sizeof *o);
  o->iv.owner = o->pf.owner = o;
  return o;
}
static u64_obj *obj_init(struct u64_obj *o, const char *name, const char *cls)
{
  o->name = name; o->cls = cls; o->alive = 1;
  // Construction order: the root C++ class first, each class's constructor then its Blueprint defaults, down to the leaf.
  const char *chain[10]; int n = 0;
  for (const char *c = cls; c && n < 10; c = u64_parent_of(c)) chain[n++] = c;
  for (int i = n - 1; i >= 0; i--) { u64_cpp_construct(o, chain[i]); u64_apply_defaults(o, chain[i]); }
  return o;
}
u64_obj *u64_new(const char *name, const char *cls)       // name must outlive the object (a literal or interned string)
{
  struct u64_obj *o = obj_take();
  return o ? obj_init(o, name, cls) : &null_obj;
}
u64_obj *u64_new_copy(const char *name, const char *cls)    // spawned objects: the name is copied into the object, nothing is interned
{
  struct u64_obj *o = obj_take();
  if (!o) return &null_obj;
  snprintf(o->namebuf, sizeof o->namebuf, "%s", name);
  return obj_init(o, o->namebuf, cls);
}

static u64_v *vars_find(vars_t *t, const char *k) { for (slot_t *s = t->head; s; s = s->next) if (s->k == k || !strcmp(s->k, k)) return &s->v; return 0; }
static u64_v *vars_get(vars_t *t, const char *k)
{
  u64_v *f = vars_find(t, k);
  if (f) return f;
  slot_t *s;
  if (free_slots) { s = free_slots; free_slots = s->next; }
  else if (arena_used >= ARENA_SLOTS) { overflow("variable arena"); static slot_t dead; return &dead.v; }
  else s = &arena[arena_used++];
  s->k = k; memset(&s->v, 0, sizeof(u64_v)); s->next = 0; s->owner = t->owner;
  if (t->tail) t->tail->next = s; else t->head = s;
  t->tail = s; t->n++;
  return &s->v;
}

u64_v *u64_ivar(u64_obj *o, const char *name)   { if (!o) o = &null_obj; return vars_get(&o->iv, name); }
u64_v *u64_pframe(u64_obj *o, const char *name) { if (!o) o = &null_obj; return vars_get(&o->pf, name); }
// Struct members: a struct variable is an object; an unset (default-constructed) struct variable becomes an empty one on first access.
u64_v *u64_member(u64_v *s, const char *name)
{
  if (s && s->t == U64_NONE) { u64_obj *o = u64_new("Struct", "Struct"); *s = u64_objv(o); }
  if (s && s->t == U64_OBJ && s->u.o) return u64_ivar(s->u.o, name);
  static u64_v scratch[8]; static int i; return &scratch[i++ & 7];
}
u64_obj *u64_obj_clone(u64_obj *src)                         // struct / spec copy: same class, every variable copied (arrays and objects shared)
{
  if (!src) return 0;
  u64_obj *o = u64_new_copy(src->name, src->cls);
  for (slot_t *sl = src->iv.head; sl; sl = sl->next) *u64_ivar(o, sl->k) = sl->v;
  return o;
}

int u64_class_known(const char *cls)                      // a Blueprint class or a registered C++ class: its ancestry is modelled, so a cast to it can fail
{
  if (u64_parent_of(cls)) return 1;
  for (int i = 0; i < u64_bp_count; i++) if (!strcmp(u64_bp_table[i].cls, cls)) return 1;
  return u64_cpp_class_registered(cls);
}
u64_v u64_cast(u64_v v, const char *cls)
{
  if (v.t != U64_OBJ || !v.u.o) return v.t == U64_NONE ? v : u64_objv(0);
  if (!strcmp(v.u.o->cls, "Component") || !u64_class_known(cls) || u64_class_is(v.u.o->cls, cls)) return v;     // components are not typed here
  return u64_objv(0);
}
int u64_has_bp(u64_obj *self, const char *fn)          // does a compiled Blueprint function of this name exist on the class chain?
{
  for (const char *c = self ? self->cls : 0; c; c = u64_parent_of(c))
    for (int i = 0; i < u64_bp_count; i++) if (!strcmp(u64_bp_table[i].fn, fn) && !strcmp(u64_bp_table[i].cls, c)) return 1;
  return 0;
}

u64_obj *u64_find(const char *name)
{
  for (int i = 0; i < nobjs; i++) if (!strcmp(objs[i].name, name)) return &objs[i];
  return u64_new(name, name);
}

u64_v u64_delegate(u64_v obj, const char *fn)  { u64_v r = {U64_DELEGATE}; r.u.s = fn; r.dobj = obj.u.o; return r; }

// Multicast delegates: bindings keyed by the address of the (stable) variable slot.
#define MAX_BINDS 384
#define MAX_LISTENERS 48                                            // bindings of one delegate that a single broadcast calls
static struct { u64_v *m; u64_v d; } binds[MAX_BINDS];
static int nbinds;
void u64_mc_add(u64_v *m, u64_v d)
{
  if (nbinds < MAX_BINDS) { binds[nbinds].m = m; binds[nbinds++].d = d; return; }
  u64_rt_overflows++;                                                  // never drop a binding silently
}
void u64_mc_remove(u64_v *m, u64_v d) { for (int i = 0; i < nbinds; i++) if (binds[i].m == m && binds[i].d.dobj == d.dobj && !strcmp(binds[i].d.u.s, d.u.s)) { binds[i] = binds[--nbinds]; return; } }
void u64_mc_clear(u64_v *m)           { for (int i = 0; i < nbinds; ) { if (binds[i].m == m) binds[i] = binds[--nbinds]; else i++; } }
void u64_mc_broadcast(u64_v *m, int argc, const u64_v *argv)
{
  u64_v snap[MAX_LISTENERS]; int n = 0;
  for (int i = 0; i < nbinds; i++) if (binds[i].m == m) { if (n < MAX_LISTENERS) snap[n++] = binds[i].d; else u64_rt_overflows++; }
  for (int i = 0; i < n; i++) u64_call(snap[i].dobj, snap[i].u.s, argc, argv);
}
const char *u64_obj_name(u64_obj *o)  { return o->name; }
const char *u64_obj_class(u64_obj *o) { return o->cls; }
int u64_obj_count(void)               { return nobjs; }
u64_obj *u64_obj_at(int i)            { return &objs[i]; }
int u64_obj_alive(u64_obj *o)         { return o && o->alive; }
void u64_obj_set_alive(u64_obj *o, int v) { if (o) o->alive = v; }
// actors have a location; components, tasks and plain value objects (a HitResult also carries a Location) do not
int u64_obj_is_actor(u64_obj *o)      { if (o && o->cls[0] == 'H' && !strcmp(o->cls, "HitResult")) return 0; u64_v *l = o && o->alive ? vars_find(&o->iv, "Location") : 0; return l && l->t == U64_VEC; }
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

// Parent call: the lookup starts at the parent of `current_class`, then the C++ ports above it, then the natives.
u64_v u64_call_super(u64_obj *self, const char *cur, const char *fn, int argc, const u64_v *argv)
{
  if (!self) self = &null_obj;
  for (const char *c = u64_parent_of(cur); c; c = u64_parent_of(c))
    for (int i = 0; i < u64_bp_count; i++)
      if (!strcmp(u64_bp_table[i].fn, fn) && !strcmp(u64_bp_table[i].cls, c)) {
        last_return = (u64_v){U64_NONE};
        u64_bp_table[i].impl(self, argv, argc);
        u64_v r = last_return; last_return = (u64_v){U64_NONE};
        return r;
      }
  { u64_v cr; if (u64_cpp_dispatch_from(u64_parent_of(cur), self, fn, argc, argv, &cr)) return cr; }
  return u64_native(self, fn, argc, argv);
}

u64_v u64_comp(u64_obj *owner, const char *name)
{
  if (!owner) return (u64_v){U64_NONE};
  const char *k = u64_intern(name);
  char key[96]; snprintf(key, sizeof key, "comp:%s", k);
  u64_v *slot = u64_ivar(owner, u64_intern(key));
  if (slot->t == U64_OBJ) return *slot;
  char nm[64]; snprintf(nm, sizeof nm, "%s.%s", owner->name, name);
  u64_obj *c = u64_new_copy(nm, "Component");
  *u64_ivar(c, "Owner") = u64_objv(owner);
  *slot = u64_objv(c);
  return *slot;
}

// ---------------------------------------------------------------------------------------------------- garbage collection
// Mark and sweep over objects, their variable slots and (via the module hooks) arrays. Runs only between world ticks, when no
// Blueprint code is on the C stack, so every live value is reachable from the roots below. Roots: alive actors, delegate targets
// of reachable multicasts, and what the modules register (timers, tasks, ability system, game state).
static unsigned char gc_mark[MAX_OBJS];
static int gc_stack[MAX_OBJS], gc_sp;
static int obj_index(u64_obj *o) { return o >= objs && o < objs + MAX_OBJS ? (int)(o - objs) : -1; }
void u64_gc_mark_obj(u64_obj *o)
{
  int i = obj_index(o);
  if (i < 0 || gc_mark[i]) return;
  gc_mark[i] = 1; gc_stack[gc_sp++] = i;
}
void u64_gc_mark_v(u64_v v)
{
  if (v.t == U64_OBJ) u64_gc_mark_obj(v.u.o);
  else if (v.t == U64_DELEGATE) u64_gc_mark_obj(v.dobj);
  else if (v.t == U64_ARRAY) u64_array_gc_mark(v);
}
static void gc_drain(void)
{
  while (gc_sp) {
    struct u64_obj *o = &objs[gc_stack[--gc_sp]];
    for (slot_t *s = o->iv.head; s; s = s->next) u64_gc_mark_v(s->v);
    for (slot_t *s = o->pf.head; s; s = s->next) u64_gc_mark_v(s->v);
  }
}
__attribute__((weak)) void u64_gc_roots_natives(void) {}
__attribute__((weak)) void u64_gc_roots_gas(void) {}
__attribute__((weak)) void u64_gc_roots_core(void) {}
__attribute__((weak)) void u64_gc_roots_cpp(void) {}
__attribute__((weak)) void u64_gc_roots_project(void) {}
int u64_gc_stress;                         // tests: collect after every tick to flush out dangling references
int u64_gc_live(void) { return nobjs - n_free_objs; }
int u64_gc_pressure(void) { return nobjs - n_free_objs > MAX_OBJS * 5 / 8 || arena_used > ARENA_SLOTS * 5 / 8 ? 1 : 0; }
void u64_gc_collect(void)
{
  memset(gc_mark, 0, sizeof gc_mark); gc_sp = 0;
  u64_array_gc_begin();
  for (int i = 0; i < nobjs; i++) if (objs[i].alive && u64_obj_is_actor(&objs[i])) u64_gc_mark_obj(&objs[i]);
  u64_gc_roots_natives(); u64_gc_roots_gas(); u64_gc_roots_core(); u64_gc_roots_cpp(); u64_gc_roots_project();
  gc_drain();
  for (int changed = 1; changed; ) {                                   // delegate targets stay alive while their multicast's owner does
    changed = 0;
    for (int i = 0; i < nbinds; i++) {
      slot_t *sl = (slot_t *)((char *)binds[i].m - offsetof(slot_t, v));
      int oi = sl >= arena && sl < arena + ARENA_SLOTS ? obj_index(sl->owner) : -1;
      if (oi < 0 || gc_mark[oi]) {
        int before = gc_sp; u64_gc_mark_v(binds[i].d);
        if (gc_sp != before) { changed = 1; gc_drain(); }
      }
    }
  }
  for (int i = 0; i < nbinds; ) {                                      // drop bindings whose owner is gone
    slot_t *sl = (slot_t *)((char *)binds[i].m - offsetof(slot_t, v));
    int oi = sl >= arena && sl < arena + ARENA_SLOTS ? obj_index(sl->owner) : -1;
    if (oi >= 0 && !gc_mark[oi]) binds[i] = binds[--nbinds]; else i++;
  }
  for (int i = 0; i < nobjs; i++) {
    struct u64_obj *o = &objs[i];
    if (gc_mark[i] || !o->cls || !strcmp(o->cls, "__free")) continue;
    for (vars_t *vt = &o->iv; vt; vt = vt == &o->iv ? &o->pf : 0)
      for (slot_t *s = vt->head, *nx; s; s = nx) { nx = s->next; s->next = free_slots; s->k = "__free"; free_slots = s; }
    o->iv.head = o->iv.tail = o->pf.head = o->pf.tail = 0; o->iv.n = o->pf.n = 0;
    o->name = "__free"; o->cls = "__free"; o->alive = 0;
    free_objs[n_free_objs++] = i;
  }
  u64_array_gc_sweep();
}
