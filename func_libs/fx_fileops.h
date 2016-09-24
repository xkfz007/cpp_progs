#ifndef _FX_FILEOPS_H
#define _FX_FILEOPS_H

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

#ifdef _WIN32
#define utf8_to_utf16( utf8, utf16 )\
    MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, utf8, -1, utf16, sizeof(utf16)/sizeof(wchar_t) )
FILE *fx_fopen( const char *filename, const char *mode );
int fx_rename( const char *oldname, const char *newname );
#define fx_struct_stat struct _stati64
#define fx_fstat _fstati64
int fx_stat( const char *path, fx_struct_stat *buf );
int fx_vfprintf( FILE *stream, const char *format, va_list arg );
int fx_is_pipe( const char *path );
#else
#define fx_fopen       fopen
#define fx_rename      rename
#define fx_struct_stat struct stat
#define fx_fstat       fstat
#define fx_stat        stat
#define fx_vfprintf    vfprintf
#define fx_is_pipe(x)  0
#endif

#endif