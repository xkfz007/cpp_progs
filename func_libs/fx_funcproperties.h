#ifndef _FX_FUNCPROPERTIES_H
#define _FX_FUNCPROPERTIES_H

#if defined(__GNUC__) && (__GNUC__ > 3 || __GNUC__ == 3 && __GNUC_MINOR__ > 0)
#define UNUSED __attribute__((unused))
#define ALWAYS_INLINE __attribute__((always_inline)) inline
#define NOINLINE __attribute__((noinline))
#define MAY_ALIAS __attribute__((may_alias))
#define fx_constant_p(x) __builtin_constant_p(x)
#define fx_nonconstant_p(x) (!__builtin_constant_p(x))
#else
#ifdef _MSC_VER
#define ALWAYS_INLINE __forceinline
#define NOINLINE __declspec(noinline)
#else
#define ALWAYS_INLINE inline
#define NOINLINE
#endif
#define UNUSED
#define MAY_ALIAS
#define fx_constant_p(x) 0
#define fx_nonconstant_p(x) 0
#endif

#endif