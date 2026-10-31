#pragma once
#include "ue_shim.h"
// Single-player target: replication is accepted and ignored (the shipping game is also single-instance on the console).
enum ELifetimeCondition { COND_None };
enum ELifetimeRepNotifyCondition { REPNOTIFY_Always };
struct FLifetimeProperty {};
#define DOREPLIFETIME_CONDITION_NOTIFY(...) ((void)0)
#define DOREPLIFETIME(...) ((void)0)
#define DOREPLIFETIME_CONDITION(...) ((void)0)
