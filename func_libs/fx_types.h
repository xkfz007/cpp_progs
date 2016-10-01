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

typedef unsigned char    uint8_t;
typedef signed char      int8_t;
typedef unsigned short   uint16_t;
typedef signed short     int16_t;
typedef unsigned int     uint32_t;
typedef signed int       int32_t;

typedef unsigned char    byte;
typedef unsigned char    pixel;

#if defined(WIN32)
/** uint64_t is a 64 bit unsigned quantity that is 64 bit word aligned */   
typedef unsigned __int64  uint64_t;

/** int64_t is a 64 bit signed quantity that is 64 bit word aligned */
typedef signed   __int64  int64_t;

#else /* WIN32 */
/** uint64_t is a 64 bit unsigned quantity that is 64 bit word aligned */
typedef unsigned long long uint64_t;

/** int64_t is a 64 bit signed quantity that is 64 bit word aligned */
typedef signed long long int64_t;
#endif /* WIN32 */

#endif
