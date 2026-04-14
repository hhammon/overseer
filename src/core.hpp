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

#define no_mangle extern "C"

#define enum_flags(Name, Type) typedef Type Name; enum

/**
 * Utility Macros
 */

#define arrlen(arr) (sizeof(arr) / sizeof((arr)[0]))

#define unused_var(var) ((void)(var))

#endif // __CORE_HPP__
