#ifndef _FX_TYPES_H
#define _FX_TYPES_H

#include <stddef.h>

/*  7.18.1.1  Exact-width integer types */
typedef unsigned char    uint8_t;
typedef signed char      int8_t;
typedef unsigned short   uint16_t;
typedef signed short     int16_t;
typedef unsigned int     uint32_t;
typedef signed int       int32_t;

#if defined(WIN32)
typedef unsigned __int64  uint64_t;
typedef signed   __int64  int64_t;
#else 
typedef unsigned long long uint64_t;
//typedef   signed long long int64_t;
#endif 

/* Types for `void *' pointers.  */  
#if __WORDSIZE == 64  
# ifndef __intptr_t_defined  
typedef long int        intptr_t;  
#  define __intptr_t_defined  
# endif  
typedef unsigned long int   uintptr_t;  
#else  
# ifndef __intptr_t_defined  
typedef int         intptr_t;  
#  define __intptr_t_defined  
# endif  
typedef unsigned int        uintptr_t;  
#endif  

/* 7.18.2  Limits of specified-width integer types */
#if !defined ( __cplusplus) || defined (__STDC_LIMIT_MACROS)

/* 7.18.2.1  Limits of exact-width integer types */
#define INT8_MIN (-128)
#define INT16_MIN (-32768)
#define INT32_MIN (-2147483647 - 1)
#define INT64_MIN  (-9223372036854775807LL - 1)

#define INT8_MAX 127
#define INT16_MAX 32767
#define INT32_MAX 2147483647
#define INT64_MAX 9223372036854775807LL

#define UINT8_MAX 0xff /* 255U */
#define UINT16_MAX 0xffff /* 65535U */
#define UINT32_MAX 0xffffffff  /* 4294967295U */
#define UINT64_MAX 0xffffffffffffffffULL /* 18446744073709551615ULL */

/* 7.18.2.2  Limits of minimum-width integer types */
#define INT_LEAST8_MIN INT8_MIN
#define INT_LEAST16_MIN INT16_MIN
#define INT_LEAST32_MIN INT32_MIN
#define INT_LEAST64_MIN INT64_MIN

#define INT_LEAST8_MAX INT8_MAX
#define INT_LEAST16_MAX INT16_MAX
#define INT_LEAST32_MAX INT32_MAX
#define INT_LEAST64_MAX INT64_MAX

#define UINT_LEAST8_MAX UINT8_MAX
#define UINT_LEAST16_MAX UINT16_MAX
#define UINT_LEAST32_MAX UINT32_MAX
#define UINT_LEAST64_MAX UINT64_MAX

/* 7.18.2.3  Limits of fastest minimum-width integer types */
#define INT_FAST8_MIN INT8_MIN
#define INT_FAST16_MIN INT16_MIN
#define INT_FAST32_MIN INT32_MIN
#define INT_FAST64_MIN INT64_MIN

#define INT_FAST8_MAX INT8_MAX
#define INT_FAST16_MAX INT16_MAX
#define INT_FAST32_MAX INT32_MAX
#define INT_FAST64_MAX INT64_MAX

#define UINT_FAST8_MAX UINT8_MAX
#define UINT_FAST16_MAX UINT16_MAX
#define UINT_FAST32_MAX UINT32_MAX
#define UINT_FAST64_MAX UINT64_MAX

/* 7.18.2.4  Limits of integer types capable of holding
    object pointers */
#if defined(_WIN64) || defined(__LP64__)
#define INTPTR_MIN INT64_MIN
#define INTPTR_MAX INT64_MAX
#define UINTPTR_MAX UINT64_MAX
#else
#define INTPTR_MIN INT32_MIN
#define INTPTR_MAX INT32_MAX
#define UINTPTR_MAX UINT32_MAX
#endif

/* 7.18.2.5  Limits of greatest-width integer types */
#define INTMAX_MIN INT64_MIN
#define INTMAX_MAX INT64_MAX
#define UINTMAX_MAX UINT64_MAX

/* 7.18.3  Limits of other integer types */
#if defined(_WIN64) || defined(__LP64__)
#define PTRDIFF_MIN INT64_MIN
#define PTRDIFF_MAX INT64_MAX
#else
#define PTRDIFF_MIN INT32_MIN
#define PTRDIFF_MAX INT32_MAX
#endif

#define SIG_ATOMIC_MIN INT32_MIN
#define SIG_ATOMIC_MAX INT32_MAX

#ifndef SIZE_MAX
#if defined(_WIN64) || defined(__LP64__)
#define SIZE_MAX UINT64_MAX
#else
#define SIZE_MAX UINT32_MAX
#endif
#endif

#ifndef WCHAR_MIN  /* also in wchar.h */
#define WCHAR_MIN 0
#define WCHAR_MAX ((wchar_t)-1) /* UINT16_MAX */
#endif

/*
 * wint_t is unsigned short for compatibility with MS runtime
 */
#define WINT_MIN 0
#define WINT_MAX ((wint_t)-1) /* UINT16_MAX */

#endif /* !defined ( __cplusplus) || defined __STDC_LIMIT_MACROS */


/* 7.18.4  Macros for integer constants */
#if !defined ( __cplusplus) || defined (__STDC_CONSTANT_MACROS)

/* 7.18.4.1  Macros for minimum-width integer constants

    Accoding to Douglas Gwyn <gwyn@arl.mil>:
	"This spec was changed in ISO/IEC 9899:1999 TC1; in ISO/IEC
	9899:1999 as initially published, the expansion was required
	to be an integer constant of precisely matching type, which
	is impossible to accomplish for the shorter types on most
	platforms, because C99 provides no standard way to designate
	an integer constant with width less than that of type int.
	TC1 changed this to require just an integer constant
	*expression* with *promoted* type."

	The trick used here is from Clive D W Feather.
*/

#define INT8_C(val) (INT_LEAST8_MAX-INT_LEAST8_MAX+(val))
#define INT16_C(val) (INT_LEAST16_MAX-INT_LEAST16_MAX+(val))
#define INT32_C(val) (INT_LEAST32_MAX-INT_LEAST32_MAX+(val))
#define INT64_C(val) (INT_LEAST64_MAX-INT_LEAST64_MAX+(val))

#define UINT8_C(val) (UINT_LEAST8_MAX-UINT_LEAST8_MAX+(val))
#define UINT16_C(val) (UINT_LEAST16_MAX-UINT_LEAST16_MAX+(val))
#define UINT32_C(val) (UINT_LEAST32_MAX-UINT_LEAST32_MAX+(val))
#define UINT64_C(val) (UINT_LEAST64_MAX-UINT_LEAST64_MAX+(val))

/* 7.18.4.2  Macros for greatest-width integer constants */
#define INTMAX_C(val) (INTMAX_MAX-INTMAX_MAX+(val))
#define UINTMAX_C(val) (UINTMAX_MAX-UINTMAX_MAX+(val))

#endif  /* !defined ( __cplusplus) || defined __STDC_CONSTANT_MACROS */


typedef unsigned char    byte;
typedef unsigned char    pixel;


enum AVEscapeMode {
    FX_ESCAPE_MODE_AUTO,      ///< Use auto-selected escaping mode.
    FX_ESCAPE_MODE_BACKSLASH, ///< Use backslash escaping.
    FX_ESCAPE_MODE_QUOTE,     ///< Use single-quote escaping.
};

/**
 * Consider spaces special and escape them even in the middle of the
 * string.
 *
 * This is equivalent to adding the whitespace characters to the special
 * characters lists, except it is guaranteed to use the exact same list
 * of whitespace characters as the rest of libavutil.
 */
#define FX_ESCAPE_FLAG_WHITESPACE (1 << 0)

/**
 * Escape only specified special characters.
 * Without this flag, escape also any characters that may be considered
 * special by av_get_token(), such as the single quote.
 */
#define FX_ESCAPE_FLAG_STRICT (1 << 1)


/*
 * Macros for format specifiers.
 */

/* fprintf(3) macros for signed integers. */

#define	PRId8		"d"	/* int8_t */
#define	PRId16		"d"	/* int16_t */
#define	PRId32		"d"	/* int32_t */
#define	PRId64		"lld"	/* int64_t */
#define	PRIdLEAST8	"d"	/* int_least8_t */
#define	PRIdLEAST16	"d"	/* int_least16_t */
#define	PRIdLEAST32	"d"	/* int_least32_t */
#define	PRIdLEAST64	"lld"	/* int_least64_t */
#define	PRIdFAST8	"d"	/* int_fast8_t */
#define	PRIdFAST16	"d"	/* int_fast16_t */
#define	PRIdFAST32	"d"	/* int_fast32_t */
#define	PRIdFAST64	"lld"	/* int_fast64_t */
#define	PRIdMAX		"jd"	/* intmax_t */
#define	PRIdPTR		"d"	/* intptr_t */

#define	PRIi8		"i"	/* int8_t */
#define	PRIi16		"i"	/* int16_t */
#define	PRIi32		"i"	/* int32_t */
#define	PRIi64		"lli"	/* int64_t */
#define	PRIiLEAST8	"i"	/* int_least8_t  */
#define	PRIiLEAST16	"i"	/* int_least16_t */
#define	PRIiLEAST32	"i"	/* int_least32_t */
#define	PRIiLEAST64	"lli"	/* int_least64_t */
#define	PRIiFAST8	"i"	/* int_fast8_t */
#define	PRIiFAST16	"i"	/* int_fast16_t */
#define	PRIiFAST32	"i"	/* int_fast32_t */
#define	PRIiFAST64	"lli"	/* int_fast64_t */
#define	PRIiMAX		"ji"	/* intmax_t */
#define	PRIiPTR		"i"	/* intptr_t */

/* fprintf(3) macros for unsigned integers. */

#define	PRIo8		"o"	/* uint8_t */
#define	PRIo16		"o"	/* uint16_t */
#define	PRIo32		"o"	/* uint32_t */
#define	PRIo64		"llo"	/* uint64_t */
#define	PRIoLEAST8	"o"	/* uint_least8_t */
#define	PRIoLEAST16	"o"	/* uint_least16_t */
#define	PRIoLEAST32	"o"	/* uint_least32_t */
#define	PRIoLEAST64	"llo"	/* uint_least64_t */
#define	PRIoFAST8	"o"	/* uint_fast8_t */
#define	PRIoFAST16	"o"	/* uint_fast16_t */
#define	PRIoFAST32	"o"	/* uint_fast32_t */
#define	PRIoFAST64	"llo"	/* uint_fast64_t */
#define	PRIoMAX		"jo"	/* uintmax_t */
#define	PRIoPTR		"o"	/* uintptr_t */

#define	PRIu8		"u"	/* uint8_t */
#define	PRIu16		"u"	/* uint16_t */
#define	PRIu32		"u"	/* uint32_t */
#define	PRIu64		"llu"	/* uint64_t */
#define	PRIuLEAST8	"u"	/* uint_least8_t */
#define	PRIuLEAST16	"u"	/* uint_least16_t */
#define	PRIuLEAST32	"u"	/* uint_least32_t */
#define	PRIuLEAST64	"llu"	/* uint_least64_t */
#define	PRIuFAST8	"u"	/* uint_fast8_t */
#define	PRIuFAST16	"u"	/* uint_fast16_t */
#define	PRIuFAST32	"u"	/* uint_fast32_t */
#define	PRIuFAST64	"llu"	/* uint_fast64_t */
#define	PRIuMAX		"ju"	/* uintmax_t */
#define	PRIuPTR		"u"	/* uintptr_t */

#define	PRIx8		"x"	/* uint8_t */
#define	PRIx16		"x"	/* uint16_t */
#define	PRIx32		"x"	/* uint32_t */
#define	PRIx64		"llx"	/* uint64_t */
#define	PRIxLEAST8	"x"	/* uint_least8_t */
#define	PRIxLEAST16	"x"	/* uint_least16_t */
#define	PRIxLEAST32	"x"	/* uint_least32_t */
#define	PRIxLEAST64	"llx"	/* uint_least64_t */
#define	PRIxFAST8	"x"	/* uint_fast8_t */
#define	PRIxFAST16	"x"	/* uint_fast16_t */
#define	PRIxFAST32	"x"	/* uint_fast32_t */
#define	PRIxFAST64	"llx"	/* uint_fast64_t */
#define	PRIxMAX		"jx"	/* uintmax_t */
#define	PRIxPTR		"x"	/* uintptr_t */

#define	PRIX8		"X"	/* uint8_t */
#define	PRIX16		"X"	/* uint16_t */
#define	PRIX32		"X"	/* uint32_t */
#define	PRIX64		"llX"	/* uint64_t */
#define	PRIXLEAST8	"X"	/* uint_least8_t */
#define	PRIXLEAST16	"X"	/* uint_least16_t */
#define	PRIXLEAST32	"X"	/* uint_least32_t */
#define	PRIXLEAST64	"llX"	/* uint_least64_t */
#define	PRIXFAST8	"X"	/* uint_fast8_t */
#define	PRIXFAST16	"X"	/* uint_fast16_t */
#define	PRIXFAST32	"X"	/* uint_fast32_t */
#define	PRIXFAST64	"llX"	/* uint_fast64_t */
#define	PRIXMAX		"jX"	/* uintmax_t */
#define	PRIXPTR		"X"	/* uintptr_t */

/* fscanf(3) macros for signed integers. */

#define	SCNd8		"hhd"	/* int8_t */
#define	SCNd16		"hd"	/* int16_t */
#define	SCNd32		"d"	/* int32_t */
#define	SCNd64		"lld"	/* int64_t */
#define	SCNdLEAST8	"hhd"	/* int_least8_t */
#define	SCNdLEAST16	"hd"	/* int_least16_t */
#define	SCNdLEAST32	"d"	/* int_least32_t */
#define	SCNdLEAST64	"lld"	/* int_least64_t */
#define	SCNdFAST8	"d"	/* int_fast8_t */
#define	SCNdFAST16	"d"	/* int_fast16_t */
#define	SCNdFAST32	"d"	/* int_fast32_t */
#define	SCNdFAST64	"lld"	/* int_fast64_t */
#define	SCNdMAX		"jd"	/* intmax_t */
#define	SCNdPTR		"d"	/* intptr_t */

#define	SCNi8		"hhi"	/* int8_t */
#define	SCNi16		"hi"	/* int16_t */
#define	SCNi32		"i"	/* int32_t */
#define	SCNi64		"lli"	/* int64_t */
#define	SCNiLEAST8	"hhi"	/* int_least8_t */
#define	SCNiLEAST16	"hi"	/* int_least16_t */
#define	SCNiLEAST32	"i"	/* int_least32_t */
#define	SCNiLEAST64	"lli"	/* int_least64_t */
#define	SCNiFAST8	"i"	/* int_fast8_t */
#define	SCNiFAST16	"i"	/* int_fast16_t */
#define	SCNiFAST32	"i"	/* int_fast32_t */
#define	SCNiFAST64	"lli"	/* int_fast64_t */
#define	SCNiMAX		"ji"	/* intmax_t */
#define	SCNiPTR		"i"	/* intptr_t */

/* fscanf(3) macros for unsigned integers. */

#define	SCNo8		"hho"	/* uint8_t */
#define	SCNo16		"ho"	/* uint16_t */
#define	SCNo32		"o"	/* uint32_t */
#define	SCNo64		"llo"	/* uint64_t */
#define	SCNoLEAST8	"hho"	/* uint_least8_t */
#define	SCNoLEAST16	"ho"	/* uint_least16_t */
#define	SCNoLEAST32	"o"	/* uint_least32_t */
#define	SCNoLEAST64	"llo"	/* uint_least64_t */
#define	SCNoFAST8	"o"	/* uint_fast8_t */
#define	SCNoFAST16	"o"	/* uint_fast16_t */
#define	SCNoFAST32	"o"	/* uint_fast32_t */
#define	SCNoFAST64	"llo"	/* uint_fast64_t */
#define	SCNoMAX		"jo"	/* uintmax_t */
#define	SCNoPTR		"o"	/* uintptr_t */

#define	SCNu8		"hhu"	/* uint8_t */
#define	SCNu16		"hu"	/* uint16_t */
#define	SCNu32		"u"	/* uint32_t */
#define	SCNu64		"llu"	/* uint64_t */
#define	SCNuLEAST8	"hhu"	/* uint_least8_t */
#define	SCNuLEAST16	"hu"	/* uint_least16_t */
#define	SCNuLEAST32	"u"	/* uint_least32_t */
#define	SCNuLEAST64	"llu"	/* uint_least64_t */
#define	SCNuFAST8	"u"	/* uint_fast8_t */
#define	SCNuFAST16	"u"	/* uint_fast16_t */
#define	SCNuFAST32	"u"	/* uint_fast32_t */
#define	SCNuFAST64	"llu"	/* uint_fast64_t */
#define	SCNuMAX		"ju"	/* uintmax_t */
#define	SCNuPTR		"u"	/* uintptr_t */

#define	SCNx8		"hhx"	/* uint8_t */
#define	SCNx16		"hx"	/* uint16_t */
#define	SCNx32		"x"	/* uint32_t */
#define	SCNx64		"llx"	/* uint64_t */
#define	SCNxLEAST8	"hhx"	/* uint_least8_t */
#define	SCNxLEAST16	"hx"	/* uint_least16_t */
#define	SCNxLEAST32	"x"	/* uint_least32_t */
#define	SCNxLEAST64	"llx"	/* uint_least64_t */
#define	SCNxFAST8	"x"	/* uint_fast8_t */
#define	SCNxFAST16	"x"	/* uint_fast16_t */
#define	SCNxFAST32	"x"	/* uint_fast32_t */
#define	SCNxFAST64	"llx"	/* uint_fast64_t */
#define	SCNxMAX		"jx"	/* uintmax_t */
#define	SCNxPTR		"x"	/* uintptr_t */

#endif
