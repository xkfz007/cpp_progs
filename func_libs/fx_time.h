#ifndef FX_TIME_H
#define FX_TIME_H

#include "fx_types.h"

#if _WIN32
#include <sys/types.h>
#include <sys/timeb.h>
#include <Windows.h>
#define HAVE_GETSYSTEMTIMEASFILETIME 1
#else
#include <sys/time.h>
#endif
#include <time.h>


static int64_t fx_mdate( void )
{
#if _WIN32
    struct timeb tb;
    ftime( &tb );
    return ((int64_t)tb.time * 1000 + (int64_t)tb.millitm) * 1000;
#else
    struct timeval tv_date;
    gettimeofday( &tv_date, NULL );
    return (int64_t)tv_date.tv_sec * 1000000 + (int64_t)tv_date.tv_usec;
#endif
}
/*get duration
i_start = fx_mdate();
i_end = fx_mdate();
duration=(i_end-i_start)*1.0/1000 //microseconds
duration=(i_end-i_start)*1.0/1000000 //seconds
*/

static int64_t fx_gettime(void)
{
#if HAVE_GETTIMEOFDAY
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
#elif HAVE_GETSYSTEMTIMEASFILETIME
    FILETIME ft;
    int64_t t;
    GetSystemTimeAsFileTime(&ft);
    t = (int64_t)ft.dwHighDateTime << 32 | ft.dwLowDateTime;
    return t / 10 - 11644473600000000; /* Jan 1, 1601 */
#else
    return -1;
#endif
}


#define HAVE_GMTIME_R 0
#define HAVE_LOCALTIME_R 0

#if !HAVE_GMTIME_R && !defined(gmtime_r)
static inline struct tm *gmtime_r(const time_t* clock, struct tm *result)
{
    struct tm *ptr = gmtime(clock);
    if (!ptr)
        return NULL;
    *result = *ptr;
    return result;
}
#endif

#if !HAVE_LOCALTIME_R && !defined(localtime_r)
static inline struct tm *localtime_r(const time_t* clock, struct tm *result)
{
    struct tm *ptr = localtime(clock);
    if (!ptr)
        return NULL;
    *result = *ptr;
    return result;
}
#endif

#endif
