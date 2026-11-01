// Animation Blueprint evaluation on the bone chain a project needs (sockets, aim): compiled graph tables from tools/ue64/animgraph.py.
#pragma once
#include "u64_rt.h"
typedef struct { const char *name; int parent; float t[3], q[4]; } u64_an_bone;
typedef struct { int npos, nrot; const float *pos, *rot; } u64_an_track;                       // keys: vec3 / quat xyzw, evenly spaced; 1 key = constant, 0 = reference pose
typedef struct { const char *name; float length; int frames; const u64_an_track *tracks; } u64_an_seq;
typedef struct { const char *name; int kind, nlinks, links[8], seq; float rate; int loop, machine, emap[8]; float times[8]; const char *slot; int bs, nfilt, filt[4], depth[4]; } u64_an_node;
typedef struct { int seq; float x, y; } u64_an_bsample;
typedef struct { int idx[3]; float w[3]; } u64_an_bgrid;                                         // one grid point: up to three samples and their weights
typedef struct { const char *name; float min[2], max[2]; int div[2], nsamples; const u64_an_bsample *samples; const u64_an_bgrid *grid; } u64_an_bspace;   // 2D blend space
typedef struct { const char *name, *slot; float blend_in, blend_out, length; int seq; float start, end, rate; } u64_an_montage;     // one segment montages (the project's)
typedef struct { int index, node, desired; } u64_an_exit;
typedef struct { const char *name; int root, nexits; u64_an_exit exits[4]; } u64_an_state;
typedef struct { int prev, next; float time; } u64_an_trans;
typedef struct { const char *name; int initial, nstates; const u64_an_state *states; int ntrans; const u64_an_trans *trans; } u64_an_machine;
typedef struct { const char *var, *node, *field; } u64_an_copy;
typedef struct {
  const char *name, *mesh; int nnodes; const u64_an_node *nodes; int nmachines; const u64_an_machine *machines; int ncopies; const u64_an_copy *copies;
  int nfuncs; const char *const *funcs; int nbones; const u64_an_bone *bones; int nseqs; const u64_an_seq *seqs; int nmontages; const u64_an_montage *montages; int nbspaces; const u64_an_bspace *bspaces;
} u64_an_graph;
#define U64_AN_MAXBONES 32
typedef struct { float t[U64_AN_MAXBONES][3], q[U64_AN_MAXBONES][4]; } u64_an_pose;
typedef struct u64_an_inst u64_an_inst;
u64_an_inst *u64_anim_new(const u64_an_graph *g, u64_obj *instance);                            // instance: the AnimInstance object (variables, node structs)
int u64_anim_montage_play(u64_an_inst *a, const char *montage, float rate);                      // 0 if the graph does not know the montage
void u64_anim_free(u64_an_inst *a);
void u64_anim_update(u64_an_inst *a, float dt);                                                 // BlueprintUpdateAnimation, variable copies, rules, then evaluate
const u64_an_pose *u64_anim_pose(const u64_an_inst *a);                                         // local bone poses of the chain
int u64_anim_bone_cs(const u64_an_inst *a, const char *bone, float t[3], float q[4]);           // component-space transform of a chain bone
const u64_an_graph *u64_anim_find_graph(const char *abp);
typedef struct { const char *mesh, *socket; float t[3], q[4]; } u64_mesh_socket;            // socket transform in its skeletal mesh's space (reference pose)
typedef struct { const char *cls, *comp, *mesh; } u64_weapon_mesh;                             // Blueprint WeaponMesh components -> mesh asset
extern const u64_mesh_socket u64_mesh_sockets[]; extern const u64_weapon_mesh u64_weapon_meshes[];
extern const u64_an_graph *const u64_an_graphs[];                                               // generated: null-terminated
