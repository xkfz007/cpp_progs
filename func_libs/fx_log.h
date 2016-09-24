#ifndef FX_LOG_H
#define FX_LOG_H
/* Log level */
#define FX_LOG_NONE           0
#define FX_LOG_ERROR          1
#define FX_LOG_WARNING        2
#define FX_LOG_INFO           3
#define FX_LOG_DEBUG          4
#define FX_LOG_FULL           5

#include <errno.h>
#include <stdarg.h>
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
	if(level>FX_LOG_FULL)
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
#endif