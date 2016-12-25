#ifndef FX_TIME_H
#define FX_TIME_H

#include "fx_types.h"
#include "fx_error.h"
#include "fx_string.h"

#if _WIN32
#include <sys/types.h>
#include <sys/timeb.h>
#include <Windows.h>
#define HAVE_GETSYSTEMTIMEASFILETIME 1
#else
#include <sys/time.h>
#endif
#include <time.h>


#if 0
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
#endif

/* Get current time in seconds
 * To get duration, use the following codes:
 * i_start = fx_gettime();
 * i_end = fx_gettime();
 * duration=(i_end-i_start)*1.0/1000 //microseconds
 * duration=(i_end-i_start)*1.0/1000000 //seconds
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
#elif _WIN32
    struct timeb tb;
    ftime( &tb );
    return ((int64_t)tb.time * 1000 + (int64_t)tb.millitm) * 1000;
#else
    return -1;
#endif
}

//Same with gmtime(), but stores the data in a user-supplied struct
static inline struct tm *fx_gmtime_r(const time_t* clock, struct tm *result)
{
    struct tm *ptr = gmtime(clock);
    if (!ptr)
        return NULL;
    *result = *ptr;
    return result;
}

//Same with localtime(), but stores the data in a user-supplied struct
static inline struct tm *fx_localtime_r(const time_t* clock, struct tm *result)
{
    struct tm *ptr = localtime(clock);
    if (!ptr)
        return NULL;
    *result = *ptr;
    return result;
}

/* get a positive number between n_min and n_max, for a maximum length
   of len_max. Return -1 if error. */
static int date_get_num(const char **pp,
                        int n_min, int n_max, int len_max)
{
    int i, val, c;
    const char *p;

    p = *pp;
    val = 0;
    for(i = 0; i < len_max; i++) {
        c = *p;
        if (!fx_isdigit(c))
            break;
        val = (val * 10) + c - '0';
        p++;
    }
    /* no number read ? */
    if (p == *pp)
        return -1;
    if (val < n_min || val > n_max)
        return -1;
    *pp = p;
    return val;
}
/**
 * Simplified version of strptime
 *
 * Parse the input string p according to the format string fmt and
 * store its results in the structure dt.
 * This implementation supports only a subset of the formats supported
 * by the standard strptime().
 *
 * The supported input field descriptors are listed below.
 * - %H: the hour as a decimal number, using a 24-hour clock, in the
 *   range '00' through '23'
 * - %J: hours as a decimal number, in the range '0' through INT_MAX
 * - %M: the minute as a decimal number, using a 24-hour clock, in the
 *   range '00' through '59'
 * - %S: the second as a decimal number, using a 24-hour clock, in the
 *   range '00' through '59'
 * - %Y: the year as a decimal number, using the Gregorian calendar
 * - %m: the month as a decimal number, in the range '1' through '12'
 * - %d: the day of the month as a decimal number, in the range '1'
 *   through '31'
 * - %T: alias for '%H:%M:%S'
 * - %%: a literal '%'
 *
 * @return a pointer to the first character not processed in this function
 *         call. In case the input string contains more characters than
 *         required by the format string the return value points right after
 *         the last consumed input character. In case the whole input string
 *         is consumed the return value points to the null byte at the end of
 *         the string. On failure NULL is returned.
 */

static char *fx_small_strptime(const char *p, const char *fmt, struct tm *dt)
{
    int c, val;

    while((c = *fmt++)) {
        if (c != '%') {
            if (fx_isspace(c))
                for (; *p && fx_isspace(*p); p++);
            else if (*p != c)
                return NULL;
            else p++;
            continue;
        }

        c = *fmt++;
        switch(c) {
        case 'H':
        case 'J':
            val = date_get_num(&p, 0, c == 'H' ? 23 : INT_MAX, 2);

            if (val == -1)
                return NULL;
            dt->tm_hour = val;
            break;
        case 'M':
            val = date_get_num(&p, 0, 59, 2);
            if (val == -1)
                return NULL;
            dt->tm_min = val;
            break;
        case 'S':
            val = date_get_num(&p, 0, 59, 2);
            if (val == -1)
                return NULL;
            dt->tm_sec = val;
            break;
        case 'Y':
            val = date_get_num(&p, 0, 9999, 4);
            if (val == -1)
                return NULL;
            dt->tm_year = val - 1900;
            break;
        case 'm':
            val = date_get_num(&p, 1, 12, 2);
            if (val == -1)
                return NULL;
            dt->tm_mon = val - 1;
            break;
        case 'd':
            val = date_get_num(&p, 1, 31, 2);
            if (val == -1)
                return NULL;
            dt->tm_mday = val;
            break;
        case 'T':
            p = fx_small_strptime(p, "%H:%M:%S", dt);
            if (!p)
                return NULL;
            break;
        case '%':
            if (*p++ != '%')
                return NULL;
            break;
        default:
            return NULL;
        }
    }

    return (char*)p;
}
/**
 * Convert the decomposed UTC time in tm to a time_t value.
 */

static time_t fx_timegm(struct tm *tm)
{
    time_t t;

    int y = tm->tm_year + 1900, m = tm->tm_mon + 1, d = tm->tm_mday;

    if (m < 3) {
        m += 12;
        y--;
    }

    t = 86400LL *
        (d + (153 * m - 457) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 719469);

    t += 3600 * tm->tm_hour + 60 * tm->tm_min + tm->tm_sec;

    return t;
}
/**
 * Parse timestr and return in *time a corresponding number of
 * microseconds.
 *
 * @param timeval puts here the number of microseconds corresponding
 * to the string in timestr. If the string represents a duration, it
 * is the number of microseconds contained in the time interval.  If
 * the string is a date, is the number of microseconds since 1st of
 * January, 1970 up to the time of the parsed date.  If timestr cannot
 * be successfully parsed, set *time to INT64_MIN.

 * @param timestr a string representing a date or a duration.
 * - If a date the syntax is:
 * @code
 * [{YYYY-MM-DD|YYYYMMDD}[T|t| ]]{{HH:MM:SS[.m...]]]}|{HHMMSS[.m...]]]}}[Z]
 * now
 * @endcode
 * If the value is "now" it takes the current time.
 * Time is local time unless Z is appended, in which case it is
 * interpreted as UTC.
 * If the year-month-day part is not specified it takes the current
 * year-month-day.
 * - If a duration the syntax is:
 * @code
 * [-][HH:]MM:SS[.m...]
 * [-]S+[.m...]
 * @endcode
 * @param duration flag which tells how to interpret timestr, if not
 * zero timestr is interpreted as a duration, otherwise as a date
 * @return >= 0 in case of success, a negative value corresponding to an
 * AVERROR code otherwise
 */
static int fx_parse_time(int64_t *timeval, const char *timestr, int duration)
{
    const char *p, *q;
    int64_t t, now64;
    time_t now;
    struct tm dt = { 0 }, tmbuf;
    int today = 0, negative = 0, microseconds = 0;
    int i;
    static const char * const date_fmt[] = {
        "%Y - %m - %d",
        "%Y%m%d",
    };
    static const char * const time_fmt[] = {
        "%H:%M:%S",
        "%H%M%S",
    };
    static const char * const tz_fmt[] = {
        "%H:%M",
        "%H%M",
        "%H",
    };

    p = timestr;
    q = NULL;
    *timeval = INT64_MIN;
    if (!duration) {
        now64 = fx_gettime();
        now = now64 / 1000000;

        if (!fx_strcasecmp(timestr, "now")) {
            *timeval = now64;
            return 0;
        }

        /* parse the year-month-day part */
        for (i = 0; i < ARRAY_ELEMS(date_fmt); i++) {
            q = fx_small_strptime(p, date_fmt[i], &dt);
            if (q)
                break;
        }

        /* if the year-month-day part is missing, then take the
         * current year-month-day time */
        if (!q) {
            today = 1;
            q = p;
        }
        p = q;

        if (*p == 'T' || *p == 't')
            p++;
        else
            while (fx_isspace(*p))
                p++;

        /* parse the hour-minute-second part */
        for (i = 0; i < ARRAY_ELEMS(time_fmt); i++) {
            q = fx_small_strptime(p, time_fmt[i], &dt);
            if (q)
                break;
        }
    } else {
        /* parse timestr as a duration */
        if (p[0] == '-') {
            negative = 1;
            ++p;
        }
        /* parse timestr as HH:MM:SS */
        q = fx_small_strptime(p, "%J:%M:%S", &dt);
        if (!q) {
            /* parse timestr as MM:SS */
            q = fx_small_strptime(p, "%M:%S", &dt);
            dt.tm_hour = 0;
        }
        if (!q) {
            char *o;
            /* parse timestr as S+ */
            dt.tm_sec = strtol(p, &o, 10);
            if (o == p) /* the parsing didn't succeed */
                return AVERROR(EINVAL);
            dt.tm_min = 0;
            dt.tm_hour = 0;
            q = o;
        }
    }

    /* Now we have all the fields that we can get */
    if (!q)
        return AVERROR(EINVAL);

    /* parse the .m... part */
    if (*q == '.') {
        int n;
        q++;
        for (n = 100000; n >= 1; n /= 10, q++) {
            if (!fx_isdigit(*q))
                break;
            microseconds += n * (*q - '0');
        }
        while (fx_isdigit(*q))
            q++;
    }

    if (duration) {
        t = dt.tm_hour * 3600 + dt.tm_min * 60 + dt.tm_sec;
    } else {
        int is_utc = *q == 'Z' || *q == 'z';
        int tzoffset = 0;
        q += is_utc;
        if (!today && !is_utc && (*q == '+' || *q == '-')) {
            struct tm tz = { 0 };
            int sign = (*q == '+' ? -1 : 1);
            q++;
            p = q;
            for (i = 0; i < ARRAY_ELEMS(tz_fmt); i++) {
                q = fx_small_strptime(p, tz_fmt[i], &tz);
                if (q)
                    break;
            }
            if (!q)
                return AVERROR(EINVAL);
            tzoffset = sign * (tz.tm_hour * 60 + tz.tm_min) * 60;
            is_utc = 1;
        }
        if (today) { /* fill in today's date */
            struct tm dt2 = is_utc ? *fx_gmtime_r(&now, &tmbuf) : *fx_localtime_r(&now, &tmbuf);
            dt2.tm_hour = dt.tm_hour;
            dt2.tm_min  = dt.tm_min;
            dt2.tm_sec  = dt.tm_sec;
            dt = dt2;
        }
        dt.tm_isdst = is_utc ? 0 : -1;
        t = is_utc ? fx_timegm(&dt) : mktime(&dt);
        t += tzoffset;
    }

    /* Check that we are at the end of the string */
    if (*q)
        return AVERROR(EINVAL);

    t *= 1000000;
    t += microseconds;
    *timeval = negative ? -t : t;
    return 0;
}

//check if the year/month/day is validate
static int validate_date(int year,int month,int day){
    if(year < 1900 || 2099 < year)
        return 0;
    switch(month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            if(day<1||day>31)
                return 0;
            break;
        case 4: case 6: case 9: case 11:
            if(day<1||day>30)
                return 0;
            break;
        case 2:
            {
                int leap = (0 == year % 4 && 0 != year % 100) || (0 == year % 400);
                if(day<1||leap&&day>29||!leap&&day>28)
                    return 0;
            }
            break;
        default:
            return 0;
    }
    return 1;
}

//valid formats: 2016-1-1,2016/1/1
int parse_date(const char* date,int *year,int *month,int *day){
    int ret;
    ret=sscanf(date,"%4d%*[-/]%2d%*[-/]%2d",year,month,day);
#if _DEBUG
    {
        fprintf(stdout,"year=%4d\n",*year);
        fprintf(stdout,"month=%02d\n",*month);
        fprintf(stdout,"day=%02d\n",*day);
    }
#endif
    return ret==3;
}

int check_date(const char* date, int *year, int *month, int *day){
    return parse_date(date,year,month,day)&&validate_date(*year,*month,*day);
}

//if ret<0, date1 is earlier than date2
//if ret>0, date1 is later than date2
int time_cmp(int y1,int m1,int d1,
        int y2,int m2,int d2){
    if(y1==y2){
        if(m1==m2){
            return d1-d2;
        }
        else
            return m1-m2;
    }
    else
        return y1-y2;
}

#endif