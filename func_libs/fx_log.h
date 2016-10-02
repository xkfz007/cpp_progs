#ifndef FX_LOG_H
#define FX_LOG_H
/* Log level */
#define FX_LOG_NONE           0
#define FX_LOG_ERROR          1
#define FX_LOG_WARNING        2
#define FX_LOG_INFO           3
#define FX_LOG_DEBUG          4
#define FX_LOG_TRACE          5

#include <errno.h>
#include <stdarg.h>
#include <string.h>
static void fx_log(const char* caller, int level, const char* fmt, ...)
{
#define  bufferSize 4096
    char buffer[bufferSize];
    int p = 0;
    va_list arg;
	const char* log_level[]={
		"Unknown",
		"Error",
		"Warning",
		"Info",
		"Debug",
		"Full",
	};
    if (caller)
        p += sprintf(buffer, "[%-4s]", caller);
	if(level>FX_LOG_TRACE)
		level=FX_LOG_NONE;
	if(level==FX_LOG_ERROR)
		p += sprintf(buffer, "[%s]:[Errno=%d Err=%s]: ", log_level[level],errno,strerror(errno));
	else
		p += sprintf(buffer, "[%s]: ", log_level[level]);

    va_start(arg, fmt);
    vsnprintf(buffer + p, bufferSize - p, fmt, arg);
    va_end(arg);
    fputs(buffer, stderr);

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
    printf("%s",colors[c]);
    va_list arg;
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
    use_color = (con != INVALID_HANDLE_VALUE);// && !getenv("NO_COLOR") && !getenv("AV_LOG_FORCE_NOCOLOR");
    if (use_color) {
        GetConsoleScreenBufferInfo(con, &con_info);
        attr_orig  = con_info.wAttributes;
        background = attr_orig & 0xF0;
    }
#elif HAVE_ISATTY
    char *term = getenv("TERM");
    use_color = //!getenv("NO_COLOR") && !getenv("AV_LOG_FORCE_NOCOLOR") &&
                (getenv("TERM") && isatty(2) || getenv("AV_LOG_FORCE_COLOR"));
    if (   //getenv("AV_LOG_FORCE_256COLOR") || 
        (term && strstr(term, "256color")))
        use_color *= 256;
#else
    use_color = 1;//getenv("AV_LOG_FORCE_COLOR") && !getenv("NO_COLOR") && !getenv("AV_LOG_FORCE_NOCOLOR");
#endif
}

static void colored_fputs(int level, int tint, const char *str)
{
    int local_use_color;
    if (!*str)
        return;

    if (use_color < 0)
        check_color_terminal();

    if (level == AV_LOG_INFO/8) local_use_color = 0;
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