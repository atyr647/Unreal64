// A small, honest subset of the Unreal Engine C++ API: just enough for a project's gameplay C++ to compile unmodified and run on
// the Unreal64 runtime (host g++ or mips64-elf-g++, -fno-exceptions -fno-rtti). Not the engine; each type documents what it keeps.
// Reflection (UCLASS/UPROPERTY/GENERATED_BODY) is produced by tools in this directory (uht_lite.py), not by macros alone.
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <type_traits>

typedef int8_t int8; typedef int16_t int16; typedef int32_t int32; typedef int64_t int64;
typedef uint8_t uint8; typedef uint16_t uint16; typedef uint32_t uint32; typedef uint64_t uint64;

#define TEXT(x) x
#define INDEX_NONE (-1)
#define FORCEINLINE inline __attribute__((always_inline))
#define GASSHOOTER_API
#define UCLASS(...)
#define USTRUCT(...)
#define UENUM(...)
#define UINTERFACE(...)
#define MinimalAPI
#define UPROPERTY(...)
#define UFUNCTION(...)
#define UMETA(...)
#define OUT
#define ensure(x) (!!(x))
#define check(x) ((void)0)
#define checkf(x, ...) ((void)0)
#define UE_LOG(...) ((void)0)
#define GET_MEMBER_NAME_CHECKED(Class, Member) (static_cast<void>(sizeof(Class::Member)), FName(#Member))   // fails to compile if the member does not exist
#define UE_CONCAT_(a, b) a##b
#define UE_CONCAT(a, b) UE_CONCAT_(a, b)
// UHT emits <File>.generated.h with one macro per GENERATED_BODY() line; uht_lite.py does the same.
#define GENERATED_USTRUCT_BODY() GENERATED_BODY()
struct FDelegateBase { template <class... A> void Broadcast(A...) const {} template <class... A> void AddDynamic(A...) {} template <class... A> void RemoveDynamic(A...) {} template <class... A> void AddUniqueDynamic(A...) {} template <class... A> void BindDynamic(A...) {} bool IsBound() const { return false; } template <class... A> void Execute(A...) const {} template <class... A> bool ExecuteIfBound(A...) const { return false; } };
#define DECLARE_DYNAMIC_MULTICAST_DELEGATE(Name) struct Name : FDelegateBase {}
#define DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(Name, T, N) struct Name : FDelegateBase { void (*hook)(void *, T) = nullptr; void *ctx = nullptr; void Broadcast(T v) const { if (hook) hook(ctx, v); } }   /* one receiver, bound by the bridge */
#define DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(Name, ...) struct Name : FDelegateBase {}
#define DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(Name, ...) struct Name : FDelegateBase {}
#define DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(Name, ...) struct Name : FDelegateBase {}
#define DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(Name, ...) struct Name : FDelegateBase {}
#define DECLARE_DYNAMIC_DELEGATE_OneParam(Name, ...) struct Name : FDelegateBase {}
#define DECLARE_DYNAMIC_DELEGATE(Name) struct Name : FDelegateBase {}
#define DECLARE_MULTICAST_DELEGATE_OneParam(Name, ...) struct Name : FDelegateBase {}
#define DECLARE_MULTICAST_DELEGATE_TwoParams(Name, ...) struct Name : FDelegateBase {}
#define DECLARE_DELEGATE_OneParam(Name, ...) struct Name : FDelegateBase {}
#define GENERATED_BODY() UE_CONCAT(UE_CONCAT(CURRENT_FILE_ID, _), UE_CONCAT(__LINE__, _GENERATED_BODY))

struct FName {                                               // interned: identical names compare by pointer
  const char *s;
  FName() : s("None") {}
  FName(const char *n) : s(intern(n)) {}
  static const char *intern(const char *n);                    // runtime-provided string table
  bool operator==(const FName &o) const { return s == o.s; }
  bool operator!=(const FName &o) const { return s != o.s; }
  const char *ToString() const { return s; }
};

struct UObject;
struct UClass { const char *Name; UObject *(*Make)() = nullptr; UObject *Cdo = nullptr; void *Rt = nullptr; UObject *GetDefaultObject() { if (!Cdo && Make) Cdo = Make(); return Cdo; } };   // Make: the class's default constructor (the class default object is built lazily)
template <class T> UObject *ShimMake();
// Members the gameplay code reaches that the runtime does not back (yet): they compile, answer a default, and are counted (ShimUnimplemented, reported once).
void ShimUnimplemented(const char *what);
struct ShimAny {
  template <class T> operator T() const { return T(); }
  ShimAny *operator->() { return this; } ShimAny &operator*() { return *this; }
  template <class... A> ShimAny operator()(A...) const { return ShimAny(); }
  template <class T> bool operator==(const T &) const { return false; } template <class T> bool operator!=(const T &) const { return true; }
  bool operator!() const { return true; }
};
#define SHIM_FN(name) template <class... A> ShimAny name(A...) const { ShimUnimplemented(#name); return ShimAny(); }
#define SHIM_VAR(name) ShimAny name;
struct FText { const char *s = ""; };
struct FString { const char *s = ""; FString() {} FString(const char *x) : s(x) {} template <class... A> static FString Printf(const char *, A...) { return FString(); }
  FString operator+(const char *o) const { static char ring[4][256]; static int k; char *b = ring[k++ & 3]; snprintf(b, 256, "%s%s", s, o); return FString(b); }
  FString operator+(const FString &o) const { return *this + o.s; } };
static int GPlayInEditorID = 0;
struct FProperty { FName Name; };
static const FName NAME_None;
#include "ue_stubs.h"
struct UObject {
  SHIM_OBJECT_STUBS
  virtual ~UObject() {}
  virtual bool InstanceOf(const UClass *) const { return false; }     // generated per class (Cast<> without RTTI)
  template <class T, class... A> T *CreateDefaultSubobject(A...) { return new T(); }
  void *rt = nullptr; struct UWorld *GetWorld() const;                                                     // the Unreal64 runtime object this one stands for
};
template <class T> UObject *ShimMake() { if constexpr (std::is_base_of_v<UObject, T> && std::is_default_constructible_v<T> && !std::is_abstract_v<T>) return new T(); else return nullptr; }
template <class T, class U> T *Cast(U *p) { return p && p->InstanceOf(T::StaticClass()) ? static_cast<T *>(p) : nullptr; }
template <class T> struct TWeakObjectPtr;
enum class ECastCheckedType { NullChecked, NullAllowed };
template <class T, class U> T *CastChecked(U *p, ECastCheckedType = ECastCheckedType::NullChecked) { return Cast<T>(p); }
template <class T> struct TEnumAsByte { uint8 v = 0; TEnumAsByte() {} TEnumAsByte(T x) : v((uint8)x) {} operator T() const { return (T)v; } };
template <class T> struct TSubclassOf { UClass *c = nullptr; TSubclassOf() {} TSubclassOf(decltype(nullptr)) {} TSubclassOf(UClass *x) : c(x) {} T *GetDefaultObject() const { return c ? static_cast<T *>(c->GetDefaultObject()) : nullptr; } bool operator!() const { return c == nullptr; } explicit operator bool() const { return c != nullptr; } UClass *Get() const { return c; } };
template <class T> struct TWeakObjectPtr { T *p = nullptr; bool operator!=(long v) const { return (long)p != v; } bool operator==(long v) const { return (long)p == v; } TWeakObjectPtr() {} TWeakObjectPtr(T *x) : p(x) {} bool operator==(const TWeakObjectPtr &o) const { return p == o.p; } bool IsValid() const { return p != nullptr; } T *Get() const { return p; } T *operator->() const { return p; } };
template <class T> struct TSharedPtr { T *p = nullptr; bool IsValid() const { return p != nullptr; } T *Get() const { return p; } T *operator->() const { return p; } T &operator*() const { return *p; } };
template <class T, class U> T *Cast(const TWeakObjectPtr<U> &w) { return Cast<T>(w.Get()); }
struct UPackage {};
FORCEINLINE UPackage *GetTransientPackage() { static UPackage p; return &p; }
template <class T> T *NewObject(UPackage *, FName = FName()) { return new T(); }
struct FObjectInitializer {};
struct FTimerHandle { int id = 0; }; struct FDelegateHandle {}; struct FDelegateRemoveShim { void Remove(FDelegateHandle) {} };
struct UAnimMontage; struct USoundCue; struct UWidgetComponent;
template <class T> T *FindFieldChecked(UClass *cls, FName name)      // one FProperty per name, stable for the program's lifetime
{
  (void)cls; static FProperty pool[256]; static int n;
  for (int i = 0; i < n; i++) if (pool[i].Name == name) return (T *)&pool[i];
  FProperty *p = &pool[n < 256 ? n++ : 255]; p->Name = name; return (T *)p;
}

namespace FMath {
template <class T> FORCEINLINE T Clamp(T v, T lo, T hi) { return v < lo ? lo : (v > hi ? hi : v); }
template <class T> FORCEINLINE T Max(T a, T b) { return a > b ? a : b; }
template <class T> FORCEINLINE T Min(T a, T b) { return a < b ? a : b; }
template <class T> FORCEINLINE T Lerp(T a, T b, float t) { return a + (b - a) * t; }
FORCEINLINE float Sqrt(float x) { return sqrtf(x); } FORCEINLINE float DegreesToRadians(float d) { return d * (3.1415926535897932f / 180.f); } FORCEINLINE int32 Rand() { static uint32 s = 1; s = s * 1103515245u + 12345u; return (int32)((s >> 16) & 0x7fff); }   // not libc rand(): the same sequence on the host and on the N64 template <class T> FORCEINLINE T Square(T x) { return x * x; }
FORCEINLINE bool IsNearlyEqual(float a, float b, float tol = 1.e-4f) { return fabsf(a - b) <= tol; }
}

template <class T> struct TArray {                             // the part of TArray the gameplay code uses; fixed capacity, no allocation
  T d[64]; int32 n = 0;
  int32 Add(const T &v) { if (n < 64) d[n++] = v; return n - 1; }
  void RemoveAt(int32 i) { if (i >= 0 && i < n) { for (int k = i; k + 1 < n; k++) d[k] = d[k + 1]; n--; } }
  int32 AddUnique(const T &v) { for (int i = 0; i < n; i++) if (d[i] == v) return i; return Add(v); }
  void Append(const TArray &o) { for (int i = 0; i < o.n; i++) Add(o.d[i]); }
  template <class U> void Append(const TArray<U> &o) { for (int i = 0; i < o.n; i++) Add(T(o.d[i])); }
  bool Contains(const T &v) const { for (int i = 0; i < n; i++) if (d[i] == v) return true; return false; }
  void Empty() { n = 0; } void Reset() { n = 0; } int32 Remove(const T &v) { int32 r = 0; for (int i = 0; i < n;) if (d[i] == v) { d[i] = d[--n]; r++; } else i++; return r; }
  bool IsValidIndex(int32 i) const { return i >= 0 && i < n; }
  T *begin() { return d; } T *end() { return d + n; } const T *begin() const { return d; } const T *end() const { return d + n; }
  int32 Num() const { return n; }
  void SetNum(int32 k) { n = k < 64 ? k : 64; }
  T &operator[](int32 i) { return d[i]; }
  const T &operator[](int32 i) const { return d[i]; }
};
struct FVector { float X = 0, Y = 0, Z = 0; FVector() {} FVector(float x, float y, float z) : X(x), Y(y), Z(z) {} static const FVector ZeroVector;
  FVector operator+(const FVector &o) const { return FVector(X + o.X, Y + o.Y, Z + o.Z); } FVector operator-(const FVector &o) const { return FVector(X - o.X, Y - o.Y, Z - o.Z); }
  FVector operator*(float f) const { return FVector(X * f, Y * f, Z * f); } FVector GetSafeNormal() const { float l = Size(); return l > 1e-8f ? FVector(X / l, Y / l, Z / l) : FVector(); } bool IsZero() const { return X == 0 && Y == 0 && Z == 0; } FVector GetLocation() const { return *this; }
  FVector RotateAngleAxis(float deg, const FVector &axis) const; struct FRotator Rotation() const;
  static float DistSquared(const FVector &a, const FVector &b) { return (a - b).SizeSquared(); } static float DotProduct(const FVector &a, const FVector &b) { return a.X * b.X + a.Y * b.Y + a.Z * b.Z; } float SizeSquared() const { return X * X + Y * Y + Z * Z; } float Size() const { return sqrtf(SizeSquared()); } };
typedef FVector FVector_NetQuantize;
template <class K, class V> struct TMap { K k[32]; V v[32]; int n = 0;                            // the lookups the gameplay code does; fixed capacity
  V *Find(const K &key) { for (int i = 0; i < n; i++) if (k[i] == key) return &v[i]; return nullptr; }
  const V *Find(const K &key) const { for (int i = 0; i < n; i++) if (k[i] == key) return &v[i]; return nullptr; }
  V &Add(const K &key, const V &val) { V *f = Find(key); if (f) { *f = val; return *f; } k[n] = key; v[n] = val; return v[n++]; }
  bool Contains(const K &key) const { return Find(key) != nullptr; } void Empty() { n = 0; } int32 Remove(const K &key) { for (int i = 0; i < n; i++) if (k[i] == key) { k[i] = k[n - 1]; v[i] = v[n - 1]; n--; return 1; } return 0; }
  V &operator[](const K &key) { V *f = Find(key); return f ? *f : Add(key, V()); } int32 Num() const { return n; } };
