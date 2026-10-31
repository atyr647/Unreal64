// Animation Blueprint evaluation on the bone chain a project needs (sockets, aim): compiled graph tables from tools/ue64/animgraph.py.
#pragma once
#include "u64_rt.h"
typedef struct { const char *name; int parent; float t[3], q[4]; } u64_an_bone;
typedef struct { int npos, nrot; const float *pos, *rot; } u64_an_track;                       // keys: vec3 / quat xyzw, evenly spaced; 1 key = constant, 0 = reference pose
typedef struct { const char *name; float length; int frames; const u64_an_track *tracks; } u64_an_seq;
typedef struct { const char *name; int kind, nlinks, links[8], seq; float rate; int loop, machine, emap[8]; float times[8]; } u64_an_node;
typedef struct { int index, node, desired; } u64_an_exit;
typedef struct { const char *name; int root, nexits; u64_an_exit exits[4]; } u64_an_state;
typedef struct { int prev, next; float time; } u64_an_trans;
typedef struct { const char *name; int initial, nstates; const u64_an_state *states; int ntrans; const u64_an_trans *trans; } u64_an_machine;
typedef struct { const char *var, *node, *field; } u64_an_copy;
typedef struct {
  const char *name, *mesh; int nnodes; const u64_an_node *nodes; int nmachines; const u64_an_machine *machines; int ncopies; const u64_an_copy *copies;
  int nfuncs; const char *const *funcs; int nbones; const u64_an_bone *bones; int nseqs; const u64_an_seq *seqs;
} u64_an_graph;
#define U64_AN_MAXBONES 16
typedef struct { float t[U64_AN_MAXBONES][3], q[U64_AN_MAXBONES][4]; } u64_an_pose;
typedef struct u64_an_inst u64_an_inst;
u64_an_inst *u64_anim_new(const u64_an_graph *g, u64_obj *instance);                            // instance: the AnimInstance object (variables, node structs)
void u64_anim_free(u64_an_inst *a);
void u64_anim_update(u64_an_inst *a, float dt);                                                 // BlueprintUpdateAnimation, variable copies, rules, then evaluate
const u64_an_pose *u64_anim_pose(const u64_an_inst *a);                                         // local bone poses of the chain
int u64_anim_bone_cs(const u64_an_inst *a, const char *bone, float t[3], float q[4]);           // component-space transform of a chain bone
const u64_an_graph *u64_anim_find_graph(const char *abp);
extern const u64_an_graph *const u64_an_graphs[];                                               // generated: null-terminated
