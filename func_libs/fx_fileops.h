/*
 * fx_fileops.h
 *
 *  Created on: 2016-05-21 6:05:55 pm
 *  Author: Felix
 */

#ifndef _FX_FILEOPS_H
#define _FX_FILEOPS_H
#include <stdio.h>
#include <wchar.h>
#include "fx_log.h"

//#define MAX_PATH  1024

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
#include <windows.h>
#define utf8_to_utf16( utf8, utf16 )\
    MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, utf8, -1, utf16, sizeof(utf16)/sizeof(wchar_t) )
FILE *fx_fopen( const char *filename, const char *mode );
int fx_rename( const char *oldname, const char *newname );
#define fx_struct_stat struct _stati64
#define fx_fstat _fstati64
int fx_stat( const char *path, fx_struct_stat *buf );
int fx_vfprintf( FILE *stream, const char *format, va_list arg );
int fx_is_pipe( const char *path );
//#define S_ISREG(x) (((x) & S_IFMT) == S_IFREG)
#else
#define fx_fopen       fopen
#define fx_rename      rename
#define fx_struct_stat struct stat
#define fx_fstat       fstat
#define fx_stat        stat
#define fx_vfprintf    vfprintf
#define fx_is_pipe(x)  0
#endif
#include <string.h>

/*
 * get file name extension
 */
static char *fx_get_filename_extension( const char *filename )
{
	char *ext;
	if(!filename)
		return NULL;
    ext = filename + strlen( filename );
    while( *ext != '.' && ext > filename )
        ext--;
    ext += *ext == '.';
    return ext;
}
static char *fx_get_filename_extension2(const char *filename){
	char *ext;
	if(!filename)
		return NULL;
	ext=strrchr(filename,'.');
	return ext;
}


#ifdef _WIN32
/* Functions for dealing with Unicode on Windows. */
static FILE *fx_fopen( const char *filename, const char *mode )
{
    wchar_t filename_utf16[MAX_PATH];
    wchar_t mode_utf16[16];
    if( utf8_to_utf16( filename, filename_utf16 ) && utf8_to_utf16( mode, mode_utf16 ) )
        return _wfopen( filename_utf16, mode_utf16 );
    return NULL;
}

static int fx_rename( const char *oldname, const char *newname )
{
    wchar_t oldname_utf16[MAX_PATH];
    wchar_t newname_utf16[MAX_PATH];
    if( utf8_to_utf16( oldname, oldname_utf16 ) && utf8_to_utf16( newname, newname_utf16 ) )
    {
        /* POSIX says that rename() removes the destination, but Win32 doesn't. */
        _wunlink( newname_utf16 );
        return _wrename( oldname_utf16, newname_utf16 );
    }
    return -1;
}

static int fx_stat( const char *path, fx_struct_stat *buf )
{
    wchar_t path_utf16[MAX_PATH];
    if( utf8_to_utf16( path, path_utf16 ) )
        return _wstati64( path_utf16, buf );
    return -1;
}

static int fx_vfprintf( FILE *stream, const char *format, va_list arg )
{
    HANDLE console = NULL;
    DWORD mode;

    if( stream == stdout )
        console = GetStdHandle( STD_OUTPUT_HANDLE );
    else if( stream == stderr )
        console = GetStdHandle( STD_ERROR_HANDLE );

    /* Only attempt to convert to UTF-16 when writing to a non-redirected console screen buffer. */
    if( GetConsoleMode( console, &mode ) )
    {
        char buf[4096];
        wchar_t buf_utf16[4096];

        int length = vsnprintf( buf, sizeof(buf), format, arg );
        if( length > 0 && length < sizeof(buf) )
        {
            /* WriteConsoleW is the most reliable way to output Unicode to a console. */
            int length_utf16 = MultiByteToWideChar( CP_UTF8, 0, buf, length, buf_utf16, sizeof(buf_utf16)/sizeof(wchar_t) );
            DWORD written;
            WriteConsoleW( console, buf_utf16, length_utf16, &written, NULL );
            return length;
        }
    }
    return vfprintf( stream, format, arg );
}

static int fx_is_pipe( const char *path )
{
    wchar_t path_utf16[MAX_PATH];
    if( utf8_to_utf16( path, path_utf16 ) )
        return WaitNamedPipeW( path_utf16, 0 );
    return 0;
}
#endif

//static int fx_is_regular_file( FILE *filehandle )
//{
//    fx_struct_stat file_stat;
//    if( fx_fstat( fileno( filehandle ), &file_stat ) )
//        return 1;
//    return S_ISREG( file_stat.st_mode );
//}
//
//static int fx_is_regular_file_path( const char *filename )
//{
//    fx_struct_stat file_stat;
//    if( fx_stat( filename, &file_stat ) )
//        return !fx_is_pipe( filename );
//    return S_ISREG( file_stat.st_mode );
//}
/*
 * get filesize
*/
int64_t fx_get_filesize( FILE *fp )
{
    int64_t save_pos;
    int64_t size_of_file;
	int ret;

	if((save_pos = ftello( fp ))<0){
		fx_log(__FUNCTION__,FX_LOG_ERROR,"Can't tell file\n");
		return -1;
	}
	if((ret=fseeko( fp, 0L, SEEK_END ))){
		fx_log(__FUNCTION__,FX_LOG_ERROR,"Can't seek file\n");
		return -1;
	}
	if(size_of_file = ftell( fp )){
		fx_log(__FUNCTION__,FX_LOG_ERROR,"Can't tell file\n");
		return -1;
	}
	if(ret=fseeko( fp, save_pos, SEEK_SET )){
		fx_log(__FUNCTION__,FX_LOG_ERROR,"Can't seek file\n");
		return -1;
	}

    return( size_of_file );
}
//#include <sys/stat.h>
//
//int fx_get_filesize( void )
//{
//    struct stat buf;
//
//    if( stat( "file", &buf ) != -1 ) {
//      printf( "File size = %d\n", buf.st_size );
//    }
//    return EXIT_SUCCESS;
//}

//int main( void )
//{
//    FILE *fp;
//    fp = fopen( "aa.txt", "r" );
//    if( fp != NULL ) {
//        printf( "File size=%ld\n", filesize( fp ) );
//        fclose( fp );
//
//        return EXIT_SUCCESS;
//    }
//     return EXIT_FAILURE;
//}
#endif