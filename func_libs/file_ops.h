#ifndef _FILE_OPS_H
#define _FILE_OPS_H

#if defined(__GNUC__)
#define fseeko  fseeko
#define ftello  ftello

#if defined(__MINGW32__)
#define fseeko fseeko64
#define ftello ftello64
#endif

#elif defined(_MSC_VER)
#define fseeko _fseeki64
#define ftello _ftelli64
#endif 

#endif