#ifndef FX_TIME_H
#define FX_TIME_H

#include "fx_types.h"

#if _WIN32
#include <sys/types.h>
#include <sys/timeb.h>
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

#endif
