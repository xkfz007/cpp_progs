#ifndef FX_LOG_H
#define FX_LOG_H
/* Log level */
#define FX_LOG_NONE           0
#define FX_LOG_ERROR          1
#define FX_LOG_WARNING        2
#define FX_LOG_INFO           3
#define FX_LOG_DEBUG          4
#define FX_LOG_FULL           5

void fx_log(const char* caller, int level, const char* fmt, ...);
#endif