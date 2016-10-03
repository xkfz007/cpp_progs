#ifndef FX_LOG_H
#define FX_LOG_H
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "fx_types.h"

/* Log level */
/**
 * Print no output.
 */
#define FX_LOG_QUIET    -8

/**
 * Something went really wrong and we will crash now.
 */
#define FX_LOG_PANIC     0

/**
 * Something went wrong and recovery is not possible.
 * For example, no header was found for a format which depends
 * on headers or an illegal combination of parameters is used.
 */
#define FX_LOG_FATAL     8

/**
 * Something went wrong and cannot losslessly be recovered.
 * However, not all future data is affected.
 */
#define FX_LOG_ERROR    16

/**
 * Something somehow does not look correct. This may or may not
 * lead to problems. An example would be the use of '-vstrict -2'.
 */
#define FX_LOG_WARNING  24

/**
 * Standard information.
 */
#define FX_LOG_INFO     32

/**
 * Detailed information.
 */
#define FX_LOG_VERBOSE  40

/**
 * Stuff which is only useful for libav* developers.
 */
#define FX_LOG_DEBUG    48

/**
 * Extremely verbose debugging, useful for libav* development.
 */
#define FX_LOG_TRACE    56

#define FX_LOG_MAX_OFFSET (FX_LOG_TRACE - FX_LOG_QUIET)


static int fx_log_level = FX_LOG_INFO;

static int fx_log_get_level(void)
{
    return fx_log_level;
}

static void fx_log_set_level(int level)
{
    fx_log_level = level;
}

static const char *get_level_str(int level)
{
    switch (level) {
    case FX_LOG_QUIET:
        return "quiet";
    case FX_LOG_DEBUG:
        return "debug";
    case FX_LOG_VERBOSE:
        return "verbose";
    case FX_LOG_INFO:
        return "info";
    case FX_LOG_WARNING:
        return "warning";
    case FX_LOG_ERROR:
        return "error";
    case FX_LOG_FATAL:
        return "fatal";
    case FX_LOG_PANIC:
        return "panic";
    default:
        return "";
    }
}

static void fx_log(const char* caller, int level, const char* fmt, ...)
{
#define  bufferSize 4096
    char buffer[bufferSize];
    int p = 0;
    va_list arg;
	//const char* log_level[]={
	//	"Unknown",
	//	"Error",
	//	"Warning",
	//	"Info",
	//	"Debug",
	//	"Full",
	//};

    if (level > fx_log_level)
        return;

    if (caller)
        p += sprintf(buffer+p, "[%-4s]", caller);
	if(level>FX_LOG_TRACE)
		level=FX_LOG_TRACE;
	if(level==FX_LOG_ERROR)
		p += sprintf(buffer+p, "[%s]:[Errno=%d Err=%s]: ", get_level_str(level),errno,strerror(errno));
	else
		p += sprintf(buffer+p, "[%s]: ", get_level_str(level));

    va_start(arg, fmt);
    vsnprintf(buffer + p, bufferSize - p, fmt, arg);
    va_end(arg);
    fputs(buffer, stderr);
    fflush(stderr);

#undef bufferSize
}
//int x264_vfprintf( FILE *stream, const char *format, va_list arg )
//{
//    HANDLE console = NULL;
//    DWORD mode;
//
//    if( stream == stdout )
//        console = GetStdHandle( STD_OUTPUT_HANDLE );
//    else if( stream == stderr )
//        console = GetStdHandle( STD_ERROR_HANDLE );
//
//    /* Only attempt to convert to UTF-16 when writing to a non-redirected console screen buffer. */
//    if( GetConsoleMode( console, &mode ) )
//    {
//        char buf[4096];
//        wchar_t buf_utf16[4096];
//
//        int length = vsnprintf( buf, sizeof(buf), format, arg );
//        if( length > 0 && length < sizeof(buf) )
//        {
//            /* WriteConsoleW is the most reliable way to output Unicode to a console. */
//            int length_utf16 = MultiByteToWideChar( CP_UTF8, 0, buf, length, buf_utf16, sizeof(buf_utf16)/sizeof(wchar_t) );
//            DWORD written;
//            WriteConsoleW( console, buf_utf16, length_utf16, &written, NULL );
//            return length;
//        }
//    }
//    return vfprintf( stream, format, arg );
//}
const char *colors[]={ 
    "\033[0;40;31m",
    "\033[0;40;32m",
    "\033[0;40;33m",
    "\033[0;40;34m",
    "\033[0;40;35m",
    "\033[0;40;36m"
};
const char *const normal = "\033[0m";
static void color_print(int c,const char* fmt, ...){
    va_list arg;
    printf("%s",colors[c]);
    va_start(arg, fmt);
    vprintf(fmt, arg);
    va_end(arg);
    printf("%s",normal);
}

#if defined(_WIN32) && !defined(__MINGW32CE__) && HAVE_SETCONSOLETEXTATTRIBUTE
#include <windows.h>
static const uint8_t color[16] = {
    [FX_LOG_ERROR  /8] = 12,
    [FX_LOG_WARNING/8] = 14,
    [FX_LOG_INFO   /8] =  7,
    [FX_LOG_DEBUG  /8] = 10,
    [FX_LOG_TRACE  /8] = 8,
};

static int16_t background, attr_orig;
static HANDLE con;
#else

static const uint32_t color[16] = {
    [FX_LOG_ERROR  /8] = 196 <<  8 | 0x11,
    [FX_LOG_WARNING/8] = 226 <<  8 | 0x03,
    [FX_LOG_INFO   /8] = 253 <<  8 | 0x09,
    [FX_LOG_DEBUG  /8] =  34 <<  8 | 0x02,
    [FX_LOG_TRACE  /8] =  34 <<  8 | 0x07,
};

#endif
static int use_color = -1;

static void check_color_terminal(void)
{
#if defined(_WIN32) && !defined(__MINGW32CE__) && HAVE_SETCONSOLETEXTATTRIBUTE
    CONSOLE_SCREEN_BUFFER_INFO con_info;
    con = GetStdHandle(STD_ERROR_HANDLE);
    use_color = (con != INVALID_HANDLE_VALUE);// && !getenv("NO_COLOR") && !getenv("fx_LOG_FORCE_NOCOLOR");
    if (use_color) {
        GetConsoleScreenBufferInfo(con, &con_info);
        attr_orig  = con_info.wAttributes;
        background = attr_orig & 0xF0;
    }
#elif HAVE_ISATTY
    char *term = getenv("TERM");
    use_color = //!getenv("NO_COLOR") && !getenv("fx_LOG_FORCE_NOCOLOR") &&
                (getenv("TERM") && isatty(2) || getenv("fx_LOG_FORCE_COLOR"));
    if (   //getenv("fx_LOG_FORCE_256COLOR") || 
        (term && strstr(term, "256color")))
        use_color *= 256;
#else
    use_color = 1;//getenv("fx_LOG_FORCE_COLOR") && !getenv("NO_COLOR") && !getenv("fx_LOG_FORCE_NOCOLOR");
#endif
}

static void colored_fputs(int level, int tint, const char *str)
{
    int local_use_color;
    if (!*str)
        return;

    if (use_color < 0)
        check_color_terminal();

    if (level == FX_LOG_INFO/8) local_use_color = 0;
    else                        local_use_color = use_color;

#if defined(_WIN32) && !defined(__MINGW32CE__) && HAVE_SETCONSOLETEXTATTRIBUTE
    if (local_use_color)
        SetConsoleTextAttribute(con, background | color[level]);
    fputs(str, stderr);
    if (local_use_color)
        SetConsoleTextAttribute(con, attr_orig);
#else
    if (local_use_color == 1) {
        fprintf(stderr,
                "\033[%d;3%dm%s\033[0m",
                (color[level] >> 4) & 15,
                color[level] & 15,
                str);
    } else if (tint && use_color == 256) {
        fprintf(stderr,
                "\033[48;5;%dm\033[38;5;%dm%s\033[0m",
                (color[level] >> 16) & 0xff,
                tint,
                str);
    } else if (local_use_color == 256) {
        fprintf(stderr,
                "\033[48;5;%dm\033[38;5;%dm%s\033[0m",
                (color[level] >> 16) & 0xff,
                (color[level] >> 8) & 0xff,
                str);
    } else
        fputs(str, stderr);
#endif

}


#endif