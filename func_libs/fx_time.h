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


static FX_S64 fx_mdate( void )
{
#if _WIN32
    struct timeb tb;
    ftime( &tb );
    return ((FX_S64)tb.time * 1000 + (FX_S64)tb.millitm) * 1000;
#else
    struct timeval tv_date;
    gettimeofday( &tv_date, NULL );
    return (FX_S64)tv_date.tv_sec * 1000000 + (FX_S64)tv_date.tv_usec;
#endif
}
/*get duration
i_start = fx_mdate();
i_end = fx_mdate();
duration=(i_end-i_start)*1.0/1000 //microseconds
duration=(i_end-i_start)*1.0/1000000 //seconds
*/

#endif
