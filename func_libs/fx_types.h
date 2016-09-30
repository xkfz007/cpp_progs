//#pragma once
//
//#ifndef _MSC_VER
//#error "Use this header only with Microsoft Visual C++ compilers!"
//#endif
//
//#include <crtdefs.h> // for intptr_t
//#if !defined(UINT64_MAX)
//#include <limits.h>
//#define UINT64_MAX _UI64_MAX
//#define INT64_MAX _I64_MAX
//#define INT16_MAX  _I16_MAX
//#endif
//
///* a minimal set of C99 types for use with MSVC (VC9) */
//
//typedef signed char int8_t;
//typedef short int int16_t;
//typedef int int32_t;
//typedef __int64 FX_S64;
//
//typedef unsigned char uint8_t;
//typedef unsigned short int uint16_t;
//typedef unsigned int uint32_t;
//typedef unsigned __int64 uFX_S64;

#ifndef _FX_TYPES_H
#define _FX_TYPES_H

typedef unsigned char    FX_U8;
typedef signed char      FX_S8;
typedef unsigned short   FX_U16;
typedef signed short     FX_S16;
typedef unsigned int     FX_U32;
typedef signed int       FX_S32;

typedef unsigned char byte;

#if defined(WIN32)
/** FX_U64 is a 64 bit unsigned quantity that is 64 bit word aligned */   
typedef unsigned __int64  FX_U64;

/** FX_S64 is a 64 bit signed quantity that is 64 bit word aligned */
typedef signed   __int64  FX_S64;

#else /* WIN32 */
/** FX_U64 is a 64 bit unsigned quantity that is 64 bit word aligned */
typedef unsigned long long FX_U64;

/** FX_S64 is a 64 bit signed quantity that is 64 bit word aligned */
typedef signed long long FX_S64;
#endif /* WIN32 */

#endif
