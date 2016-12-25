#define  _TIME
#ifdef _TIME
#include "fx_time.h"
static void test_av_small_strptime(void)
{
    int i;
    struct tm tm = { 0 };
    struct fmt_timespec_entry {
        const char *fmt, *timespec;
    } fmt_timespec_entries[] = {
        { "%Y-%m-%d",                    "2012-12-21" },
        { "%Y - %m - %d",                "2012-12-21" },
        { "%Y-%m-%d %H:%M:%S",           "2012-12-21 20:12:21" },
        { "  %Y - %m - %d %H : %M : %S", "   2012 - 12 -  21   20 : 12 : 21" },
    };

    //av_log_set_level(AV_LOG_DEBUG);
    for (i = 0;  i < FF_ARRAY_ELEMS(fmt_timespec_entries); i++) {
        char *p;
        struct fmt_timespec_entry *e = &fmt_timespec_entries[i];
        printf("fmt:'%s' spec:'%s' -> ", e->fmt, e->timespec);
        p = fx_small_strptime(e->timespec, e->fmt, &tm);
        if (p) {
            printf("%04d-%02d-%2d %02d:%02d:%02d\n",
                   1900+tm.tm_year, tm.tm_mon+1, tm.tm_mday,
                   tm.tm_hour, tm.tm_min, tm.tm_sec);
        } else {
            printf("error\n");
        }
    }
}

static void test_av_parse_time(void)
{
    int i;
    int64_t tv;
    time_t tvi;
    struct tm *tm;
    static char tzstr[] = "TZ=CET-1";
    static const char * const time_string[] = {
        "now",
        "12:35:46",
        "2000-12-20 0:02:47.5z",
        "2012 - 02-22  17:44:07",
        "2000-12-20T010247.6",
        "2000-12-12 1:35:46+05:30",
        "2002-12-12 22:30:40-02",
    };
    static const char * const duration_string[] = {
        "2:34:56.79",
        "-1:23:45.67",
        "42.1729",
        "-1729.42",
        "12:34",
    };

    //av_log_set_level(AV_LOG_DEBUG);
    putenv(tzstr);
    printf("(now is 2012-03-17 09:14:13.2 +0100, local time is UTC+1)\n");
    for (i = 0;  i < FF_ARRAY_ELEMS(time_string); i++) {
        printf("%-24s -> ", time_string[i]);
        if (fx_parse_time(&tv, time_string[i], 0)) {
            printf("error\n");
        } else {
            tvi = tv / 1000000;
            tm = gmtime(&tvi);
            printf("%14"PRIi64".%06d = %04d-%02d-%02dT%02d:%02d:%02dZ\n",
                   tv / 1000000, (int)(tv % 1000000),
                   tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                   tm->tm_hour, tm->tm_min, tm->tm_sec);
        }
    }
    for (i = 0;  i < FF_ARRAY_ELEMS(duration_string); i++) {
        printf("%-24s -> ", duration_string[i]);
        if (fx_parse_time(&tv, duration_string[i], 1)) {
            printf("error\n");
        } else {
            printf("%+21"PRIi64"\n", tv);
        }
    }
}
int main(){
    test_av_small_strptime();
    test_av_parse_time();
}
#endif