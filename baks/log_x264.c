#define _LOG_X264
#ifdef _LOG_X264

/****************************************************************************
 * x264_log:
 ****************************************************************************/
void x264_log( x264_t *h, int i_level, const char *psz_fmt, ... )
{
    if( !h || i_level <= h->param.i_log_level )
    {
        va_list arg;
        va_start( arg, psz_fmt );
        if( !h )
            x264_log_default( NULL, i_level, psz_fmt, arg );
        else
            h->param.pf_log( h->param.p_log_private, i_level, psz_fmt, arg );
        va_end( arg );
    }
}

static void x264_log_default( void *p_unused, int i_level, const char *psz_fmt, va_list arg )
{
    char *psz_prefix;
    switch( i_level )
    {
        case X264_LOG_ERROR:
            psz_prefix = "error";
            break;
        case X264_LOG_WARNING:
            psz_prefix = "warning";
            break;
        case X264_LOG_INFO:
            psz_prefix = "info";
            break;
        case X264_LOG_DEBUG:
            psz_prefix = "debug";
            break;
        default:
            psz_prefix = "unknown";
            break;
    }
    fprintf( stderr, "x264 [%s]: ", psz_prefix );
    x264_vfprintf( stderr, psz_fmt, arg );
    fflush(stderr);
}
int x264_vfprintf( FILE *stream, const char *format, va_list arg )
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

#endif