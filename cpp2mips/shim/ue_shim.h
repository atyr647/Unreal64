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
#define ensure(x) (!!(x))
#define check(x) ((void)0)
#define checkf(x, ...) ((void)0)
#define UE_LOG(...) ((void)0)
#define GET_MEMBER_NAME_CHECKED(Class, Member) (static_cast<void>(sizeof(Class::Member)), FName(#Member))   // fails to compile if the member does not exist
#define UE_CONCAT_(a, b) a##b
#define UE_CONCAT(a, b) UE_CONCAT_(a, b)
// UHT emits <File>.generated.h with one macro per GENERATED_BODY() line; uht_lite.py does the same.
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
struct FProperty { FName Name; };
struct UObject { virtual ~UObject() {} };
template <class T> T *FindFieldChecked(UClass *cls, FName name) { (void)cls; static FProperty pool[256]; static int n; FProperty *p = &pool[n++ & 255]; p->Name = name; return (T *)p; }

namespace FMath {
template <class T> FORCEINLINE T Clamp(T v, T lo, T hi) { return v < lo ? lo : (v > hi ? hi : v); }
template <class T> FORCEINLINE T Max(T a, T b) { return a > b ? a : b; }
template <class T> FORCEINLINE T Min(T a, T b) { return a < b ? a : b; }
FORCEINLINE bool IsNearlyEqual(float a, float b, float tol = 1.e-4f) { return fabsf(a - b) <= tol; }
}

template <class T> struct TArray {                             // the part of TArray the gameplay code uses; fixed capacity, no allocation
  T d[64]; int32 n = 0;
  int32 Add(const T &v) { if (n < 64) d[n++] = v; return n - 1; }
  int32 Num() const { return n; }
  T &operator[](int32 i) { return d[i]; }
  const T &operator[](int32 i) const { return d[i]; }
};
