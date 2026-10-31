// A small, honest subset of the Unreal Engine C++ API: just enough for a project's gameplay C++ to compile unmodified and run on
// the Unreal64 runtime (host g++ or mips64-elf-g++, -fno-exceptions -fno-rtti). Not the engine; each type documents what it keeps.
// Reflection (UCLASS/UPROPERTY/GENERATED_BODY) is produced by tools in this directory (uht_lite.py), not by macros alone.
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

typedef int8_t int8; typedef int16_t int16; typedef int32_t int32; typedef int64_t int64;
typedef uint8_t uint8; typedef uint16_t uint16; typedef uint32_t uint32; typedef uint64_t uint64;

#define TEXT(x) x
#define FORCEINLINE inline __attribute__((always_inline))
#define GASSHOOTER_API
#define UCLASS(...)
#define USTRUCT(...)
#define UENUM(...)
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
#define DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(Name, T1, N1) struct Name { void Broadcast(T1) {} }
#define DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(Name, T1, N1, T2, N2) struct Name { void Broadcast(T1, T2) {} }
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

struct UClass { const char *Name; };
struct FText { const char *s = ""; };
struct FString { const char *s = ""; };
struct FProperty { FName Name; };
static const FName NAME_None;
struct UObject {
  virtual ~UObject() {}
  virtual bool InstanceOf(const UClass *) const { return false; }     // generated per class (Cast<> without RTTI)
  void *rt = nullptr;                                                     // the Unreal64 runtime object this one stands for
};
template <class T, class U> T *Cast(U *p) { return p && p->InstanceOf(T::StaticClass()) ? static_cast<T *>(p) : nullptr; }
template <class T> struct TSubclassOf { UClass *c = nullptr; };
template <class T> struct TWeakObjectPtr { T *p = nullptr; TWeakObjectPtr() {} TWeakObjectPtr(T *x) : p(x) {} bool operator==(const TWeakObjectPtr &o) const { return p == o.p; } bool IsValid() const { return p != nullptr; } T *Get() const { return p; } T *operator->() const { return p; } };
template <class T> struct TSharedPtr { T *p = nullptr; bool IsValid() const { return p != nullptr; } T *Get() const { return p; } T *operator->() const { return p; } T &operator*() const { return *p; } };
struct UPackage {};
FORCEINLINE UPackage *GetTransientPackage() { static UPackage p; return &p; }
template <class T> T *NewObject(UPackage *, FName = FName()) { return new T(); }
struct FObjectInitializer {};
struct FTimerHandle {};
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
FORCEINLINE bool IsNearlyEqual(float a, float b, float tol = 1.e-4f) { return fabsf(a - b) <= tol; }
}

template <class T> struct TArray {                             // the part of TArray the gameplay code uses; fixed capacity, no allocation
  T d[64]; int32 n = 0;
  int32 Add(const T &v) { if (n < 64) d[n++] = v; return n - 1; }
  int32 AddUnique(const T &v) { for (int i = 0; i < n; i++) if (d[i] == v) return i; return Add(v); }
  void Append(const TArray &o) { for (int i = 0; i < o.n; i++) Add(o.d[i]); }
  bool Contains(const T &v) const { for (int i = 0; i < n; i++) if (d[i] == v) return true; return false; }
  void Empty() { n = 0; } void Reset() { n = 0; }
  bool IsValidIndex(int32 i) const { return i >= 0 && i < n; }
  T *begin() { return d; } T *end() { return d + n; } const T *begin() const { return d; } const T *end() const { return d + n; }
  int32 Num() const { return n; }
  void SetNum(int32 k) { n = k < 64 ? k : 64; }
  T &operator[](int32 i) { return d[i]; }
  const T &operator[](int32 i) const { return d[i]; }
};
struct FVector { float X = 0, Y = 0, Z = 0; FVector() {} FVector(float x, float y, float z) : X(x), Y(y), Z(z) {} static const FVector ZeroVector;
  FVector operator+(const FVector &o) const { return FVector(X + o.X, Y + o.Y, Z + o.Z); } FVector operator-(const FVector &o) const { return FVector(X - o.X, Y - o.Y, Z - o.Z); }
  FVector operator*(float f) const { return FVector(X * f, Y * f, Z * f); } float SizeSquared() const { return X * X + Y * Y + Z * Z; } float Size() const { return sqrtf(SizeSquared()); } };
typedef FVector FVector_NetQuantize;
template <class K, class V> struct TMap { K k[32]; V v[32]; int n = 0;                            // the lookups the gameplay code does; fixed capacity
  V *Find(const K &key) { for (int i = 0; i < n; i++) if (k[i] == key) return &v[i]; return nullptr; }
  const V *Find(const K &key) const { for (int i = 0; i < n; i++) if (k[i] == key) return &v[i]; return nullptr; }
  V &Add(const K &key, const V &val) { V *f = Find(key); if (f) { *f = val; return *f; } k[n] = key; v[n] = val; return v[n++]; }
  V &operator[](const K &key) { V *f = Find(key); return f ? *f : Add(key, V()); } int32 Num() const { return n; } };
