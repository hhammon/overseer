#ifndef __CORE_HPP__
#define __CORE_HPP__

#include <stdint.h>

/**
 * Primitives
 */

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef s8       b8;
typedef s16      b16;
typedef s32      b32;
typedef s64      b64;

typedef float    f32;
typedef double   f64;

typedef u64      uptr;
typedef s64      sptr;
typedef u64      uprocptr;
typedef s64      sprocptr;

/**
 * Codebase Keywords
 */

#define api_method    static
#define internal      static
#define global        static
#define local_persist static
#define thread_local  static __thread

#define no_mangle extern "C"

#define enum_flags(Name, Type) typedef Type Name; enum

/**
 * Constants
 */

#define THOUSAND ((u64)1000)
#define MILLION  ((u64)1000 * THOUSAND)
#define BILLION  ((u64)1000 * MILLION)
#define TRILLION ((u64)1000 * BILLION)

#define KILOBYTE ((u64)1024)
#define MEGABYTE ((u64)1024 * KILOBYTE)
#define GIGABYTE ((u64)1024 * MEGABYTE)
#define TERABYTE ((u64)1024 * GIGABYTE)

#define S8_MIN ((s8)0x80)
#define S8_MAX ((s8)0x7f)

#define S16_MIN ((s16)0x8000)
#define S16_MAX ((s16)0x7fff)

#define S32_MIN ((s32)0x80000000)
#define S32_MAX ((s32)0x7fffffff)

#define S64_MIN ((s64)0x8000000000000000)
#define S64_MAX ((s64)0x7fffffffffffffff)

#define U8_MIN ((u8)0)
#define U8_MAX ((u8)0xff)

#define U16_MIN ((u16)0)
#define U16_MAX ((u16)0xffff)

#define U32_MIN ((u32)0)
#define U32_MAX ((u32)0xffffffff)

#define U64_MIN ((u64)0)
#define U64_MAX ((u64)0xffffffffffffffff)

/**
 * Basic Types
 */

typedef u8* CString;

template<typename T>
struct View {
	T*  ptr;
	u64 len;

	T& operator[](u64 index);
};

typedef View<u8> String;
typedef String   StringZ; // Guaranteed to be null-terminated

#define S(literal) ((StringZ) { \
	.ptr = (u8*)(literal),      \
	.len = sizeof(literal) - 1  \
})

typedef View<u8> Blob;

/**
 * Utility Macros
 */

#define arrlen(arr) (sizeof(arr) / sizeof((arr)[0]))
#define sizeof_view(view) ((view).len * sizeof((view).ptr[0]))

#define unused_var(var) ((void)(var))

#define align_down(num, alignment) ((num) & ~((u64)(alignment) - 1))
#define align_up(  num, alignment) (align_down((num) + ((u64)(alignment) - 1), (u64)(alignment)))

#define is_power_of_two(num) ((((num) & ((num) - 1)) == 0) && num > 0)

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define clamp(l, val, h) (max(l, min(val, h)))

#if BUILD_DEBUG

#define assert(cond) do {      \
	if (!(cond)) {             \
		asm volatile ("int3"); \
	}                          \
} while (0)

#define debug_only if (1)

#else

#define assert(cond) ((void)(cond))
#define debug_only if (0)

#endif

#endif // __CORE_HPP__
