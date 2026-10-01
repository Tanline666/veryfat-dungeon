#ifndef TYPES_H
#define TYPES_H

#ifdef __cplusplus
#include <MSL/cstdarg>
#include <MSL/cstddef>
#include <MSL/new>

#else
#include <MSL/stdarg.h>
#include <MSL/stddef.h>
#endif

#include <decomp/compat.h>
#include <decomp/decomp.h>
#include <decomp/lang.h>

#include <macros.h>

typedef unsigned long long u64;
typedef signed long long s64;

typedef unsigned long u32;
typedef signed long s32;

typedef unsigned short u16;
typedef signed short s16;

typedef unsigned char u8;
typedef signed char s8;

typedef float f32;
typedef double f64;

typedef int UNKWORD;
typedef void UNKTYPE;

enum { FALSE, TRUE };
typedef int BOOL;

typedef void (*funcptr_t)(void);

#endif
