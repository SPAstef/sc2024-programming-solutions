#pragma once

#include <inttypes.h>
#include <limits.h>

#if defined(__INTEL_COMPILER) || defined(__INTEL_LLVM_COMPILER)
    #define INTEL_COMPILER
#endif


#ifdef __x86_64__
    #if defined(_WIN32)
        #include <intrin.h>
    #else
        #include <x86intrin.h>
        #define _rotr64 _lrotr
        #define _rotl64 _lrotl
    #endif
#endif

#ifndef CHAR_WIDTH
    #define CHAR_WIDTH CHAR_BIT
    #define UCHAR_WIDTH CHAR_WIDTH
#endif
#ifndef SHRT_WIDTH
    #if USHRT_MAX == 0xffULL
        #define SHRT_WIDTH 8
    #elif USHRT_MAX == 0xffffULL
        #define SHRT_WIDTH 16
    #elif USHRT_MAX == 0xffffffffULL
        #define SHRT_WIDTH 32
    #elif USHRT_MAX == 0xffffffffffffffffULL
        #define SHRT_WIDTH 64
    #endif
    #define USHRT_WIDTH SHRT_WIDTH
#endif
#ifndef INT_WIDTH
    #if UINT_MAX == 0xffULL
        #define INT_WIDTH 8
    #elif UINT_MAX == 0xffffULL
        #define INT_WIDTH 16
    #elif UINT_MAX == 0xffffffffULL
        #define INT_WIDTH 32
    #elif UINT_MAX == 0xffffffffffffffffULL
        #define INT_WIDTH 64
    #endif
    #define UINT_WIDTH INT_WIDTH
#endif
#ifndef LONG_WIDTH
    #if ULONG_MAX == 0xffULL
        #define LONG_WIDTH 8
    #elif ULONG_MAX == 0xffffULL
        #define LONG_WIDTH 16
    #elif ULONG_MAX == 0xffffffffULL
        #define LONG_WIDTH 32
    #elif ULONG_MAX == 0xffffffffffffffffULL
        #define LONG_WIDTH 64
    #endif
    #define ULONG_WIDTH LONG_WIDTH
#endif
#ifndef LLONG_WIDTH
    #if ULLONG_MAX == 0xffULL
        #define LLONG_WIDTH 8
    #elif ULLONG_MAX == 0xffffULL
        #define LLONG_WIDTH 16
    #elif ULLONG_MAX == 0xffffffffULL
        #define LLONG_WIDTH 32
    #elif ULLONG_MAX == 0xffffffffffffffffULL
        #define LLONG_WIDTH 64
    #endif
    #define ULLONG_WIDTH LLONG_WIDTH
#endif

// Godbolt says ARM-v7e-m supports _BitInt(128) on Clang 16/17...
#if __STDC_VERSION__ >= 202000L
    #define NATIVE_UINT128 1
typedef unsigned _BitInt(128) uint128_t;
#elif defined(__SIZEOF_INT128__)
    #define NATIVE_UINT128 1
typedef unsigned __int128 uint128_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__x86_64__) && (!defined(_MSC_VER) || defined(INTEL_COMPILER))
inline uint64_t _udiv128(uint64_t hi, uint64_t lo, uint64_t div, uint64_t *rem)
{
    // High bits go in RDX, low bits in RAX, quotient is in RAX, remainder is in RDX
    __asm__ inline( //
        "divq %4"
        : "=d"(hi), "=a"(lo)
        : "d"(hi), "a"(lo), "rm"(div));

    *rem = hi;

    return lo;
}
#endif

// If it does not have addc, I doubt it has any of the others. For now we don't use inline assembly
#if !defined(__has_builtin) || !__has_builtin(__builtin_addc)
static inline unsigned int __builtin_addc(unsigned int a, unsigned int b, unsigned int c,
                                          unsigned int *d)
{
    a += b;
    *d = a < b;
    a += c;
    *d |= a < c;

    return a;
}

static inline unsigned long __builtin_addcl(unsigned long a, unsigned long b, unsigned long c,
                                            unsigned long *d)
{
    a += b;
    *d = a < b;
    a += c;
    *d |= a < c;

    return a;
}

static inline unsigned long long __builtin_addcll(unsigned long long a, unsigned long long b,
                                                  unsigned long long c, unsigned long long *d)
{
    a += b;
    *d = a < b;
    a += c;
    *d |= a < c;

    return a;
}

static inline unsigned int __builtin_subc(unsigned int a, unsigned int b, unsigned int c,
                                          unsigned int *d)
{
    unsigned int e = a;

    a -= b;
    *d = a > e;
    e = a;
    a -= c;
    *d |= a > e;

    return a;
}

static inline unsigned long __builtin_subcl(unsigned long a, unsigned long b, unsigned long c,
                                            unsigned long *d)
{
    unsigned long e = a;

    a -= b;
    *d = a > e;
    e = a;
    a -= c;
    *d |= a > e;

    return a;
}

static inline unsigned long long __builtin_subcll(unsigned long long a, unsigned long long b,
                                                  unsigned long long c, unsigned long long *d)
{
    unsigned long long e = a;

    a -= b;
    *d = a > e;
    e = a;
    a -= c;
    *d |= a > e;

    return a;
}
#endif

static inline uint8_t _addc32(uint8_t c, uint32_t x, uint32_t y, uint32_t *z)
{
#ifdef __x86_64__
    return _addcarry_u32(c, x, y, z);
#elif UINT_WIDTH == 32
    *z = __builtin_addc(x, y, c, (unsigned int *)&x);
    return (uint8_t)x;
#elif ULONG_WIDTH == 32
    *z = __builtin_addcl(x, y, c, (unsigned long *)&x);
    return (uint8_t)x;
#elif ULLONG_WIDTH == 32
    *z = __builtin_addcll(x, y, c, (unsigned long long *)&x);
    return (uint8_t)x;
#endif
}

static inline uint8_t _addc64(uint8_t c, uint64_t x, uint64_t y, uint64_t *z)
{
#ifdef __x86_64__
    return _addcarry_u64(c, x, y, (unsigned long long *)z);
#elif UINT_WIDTH == 64
    *z = __builtin_addc(x, y, c, (unsigned int *)&x);
    return (uint8_t)x;
#elif ULONG_WIDTH == 64
    *z = __builtin_addcl(x, y, c, (unsigned long *)&x);
    return (uint8_t)x;
#elif ULLONG_WIDTH == 64
    *z = __builtin_addcll(x, y, c, (unsigned long long *)&x);
    return (uint8_t)x;
#endif
}

static inline uint8_t _subb32(uint8_t c, uint32_t x, uint32_t y, uint32_t *z)
{
#ifdef __x86_64__
    return _subborrow_u32(c, x, y, z);
#elif UINT_WIDTH == 32
    *z = __builtin_subc(x, y, c, (unsigned int *)&x);
    return (uint8_t)x;
#elif ULONG_WIDTH == 32
    *z = __builtin_subcl(x, y, c, (unsigned long *)&x);
    return (uint8_t)x;
#elif ULLONG_WIDTH == 32
    *z = __builtin_subcll(x, y, c, (unsigned long long *)&x);
    return (uint8_t)x;
#endif
}

static inline uint8_t _subb64(uint8_t c, uint64_t x, uint64_t y, uint64_t *z)
{
#ifdef __x86_64__
    return _subborrow_u64(c, x, y, (unsigned long long *)z);
#elif UINT_WIDTH == 64
    *z = __builtin_subc(x, y, c, (unsigned int *)&x);
    return (uint8_t)x;
#elif ULONG_WIDTH == 64
    *z = __builtin_subcl(x, y, c, (unsigned long *)&x);
    return (uint8_t)x;
#elif ULLONG_WIDTH == 64
    *z = __builtin_subcll(x, y, c, (unsigned long long *)&x);
    return (uint8_t)x;
#endif
}

static inline uint32_t _mulx32(uint32_t x, uint32_t y, uint32_t *h)
{
    uint64_t z = (uint64_t)x * y;

    *h = (uint32_t)(z >> 32);

    return (uint32_t)z;
}

static inline uint64_t _mulx64(uint64_t x, uint64_t y, uint64_t *h)
{
#if defined(__x86_64__)
    return _mulx_u64(x, y, (unsigned long long *)h);
#elif defined(NATIVE_UINT128)
    uint128_t z = (uint128_t)x * y;

    *h = (uint64_t)(z >> 64);

    return (uint64_t)z;
#else
    uint64_t xl = x & 0xFFFFFFFF;
    uint64_t xh = x >> 32;
    uint64_t yl = y & 0xFFFFFFFF;
    uint64_t yh = y >> 32;

    x = xl * yl;
    y = xh * yh;
    xl = xl * yh + xh * yl;
    y += xl >> 32;
    x += xl << 32;
    y += x < xl << 32;
    *h = y;

    return x;
#endif
}

static inline uint32_t _divx32(uint32_t h, uint32_t l, uint32_t d, uint32_t *r)
{
    uint64_t x = ((uint64_t)h << 32) | l;
    uint32_t q = (uint32_t)(x / d);

    *r = (uint32_t)(x % d);

    return q;
}

static inline uint64_t _divx64(uint64_t h, uint64_t l, uint64_t d, uint64_t *r)
{
#if defined(__x86_64__)
    return _udiv128(h, l, d, r);
#elif defined(NATIVE_UINT128)
    uint128_t x = ((uint128_t)h << 64) | l;
    uint64_t q = (uint64_t)(x / d);

    *r = (uint64_t)(x % d);

    return q;
#else
    return 0;
#endif
}
#ifdef __cplusplus
}
#endif

#ifdef INTEL_COMPILER
    #undef INTEL_COMPILER
#endif
