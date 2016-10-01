#ifndef _FX_STRING_H
#define _FX_STRING_H
#include "fx_types.h"
#include "fx_mathops.h"

#ifdef _MSC_VER
#define inline __inline
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define snprintf _snprintf
#define strtok_r strtok_s
#define S_ISREG(x) (((x) & S_IFMT) == S_IFREG)
#endif

#ifdef _WIN32
#ifndef strtok_r
#define strtok_r(str,delim,save) strtok(str,delim)
#endif
#define HAVE_DOS_PATHS 1
#endif


/**
 * Return non-zero if pfx is a prefix of str. If it is, *ptr is set to
 * the address of the first character in str after the prefix.
 *
 * @param str input string
 * @param pfx prefix to test
 * @param ptr updated if the prefix is matched inside str
 * @return non-zero if the prefix matches, zero otherwise
 */
int fx_strstart(const char *str, const char *pfx, const char **ptr)
{
    while (*pfx && *pfx == *str) {
        pfx++;
        str++;
    }
    if (!*pfx && ptr)
        *ptr = str;
    return !*pfx;
}

int fx_stristart(const char *str, const char *pfx, const char **ptr)
{
    while (*pfx && fx_toupper((unsigned)*pfx) == fx_toupper((unsigned)*str)) {
        pfx++;
        str++;
    }
    if (!*pfx && ptr)
        *ptr = str;
    return !*pfx;
}


/* strend: return 1 if string sfix occurs at the end of str */
int fx_strend(const char *s, const char *t,const char **ptr)
{
    char *bs=s; /* remember beginning of strs */
    char *bt=t;
    for(;*s;s++)/* end of the string */
        ;
    for(;*t;t++)
        ;

    for(;*s==*t;s--,t--)
        if(t==bt||s==bs)
            break;/* at the beginning of a str */

    if(*s==*t&&t==bt&&*s!='\0') {
        if(ptr)
            *ptr=s;
        return 1;
    }
    else
        return 0;
}

/**
 * Locate the first case-independent occurrence in the string haystack
 * of the string needle.  A zero-length string needle is considered to
 * match at the start of haystack.
 *
 * This function is a case-insensitive version of the standard strstr().
 *
 * @param haystack string to search in
 * @param needle   string to search for
 * @return         pointer to the located match within haystack
 *                 or a null pointer if no match
 */
char *fx_stristr(const char *s1, const char *s2)
{
    if (!*s2)
        return (char*)(intptr_t)s1;

    do
        if (fx_stristart(s1, s2, NULL))
            return (char*)(intptr_t)s1;
    while (*s1++);

    return NULL;
}


/**
 * Locate the first occurrence of the string needle in the string haystack
 * where not more than hay_length characters are searched. A zero-length
 * string needle is considered to match at the start of haystack.
 *
 * This function is a length-limited version of the standard strstr().
 *
 * @param haystack   string to search in
 * @param needle     string to search for
 * @param hay_length length of string to search in
 * @return           pointer to the located match within haystack
 *                   or a null pointer if no match
 */
char *fx_strnstr(const char *haystack, const char *needle, size_t hay_length)
{
    size_t needle_len = strlen(needle);
    if (!needle_len)
        return (char*)haystack;
    while (hay_length >= needle_len) {
        hay_length--;
        if (!memcmp(haystack, needle, needle_len))
            return (char*)haystack;
        haystack++;
    }
    return NULL;
}


/**
 * Copy the string src to dst, but no more than size - 1 bytes, and
 * null-terminate dst.
 *
 * This function is the same as BSD strlcpy().
 *
 * @param dst destination buffer
 * @param src source string
 * @param size size of destination buffer
 * @return the length of src
 *
 * @warning since the return value is the length of src, src absolutely
 * _must_ be a properly 0-terminated string, otherwise this will read beyond
 * the end of the buffer and possibly crash.
 */
size_t fx_strlcpy(char *dst, const char *src, size_t size)
{
    size_t len = 0;
    while (++len < size && *src)
        *dst++ = *src++;
    if (len <= size)
        *dst = 0;
    return len + strlen(src) - 1;
}


/**
 * Append the string src to the string dst, but to a total length of
 * no more than size - 1 bytes, and null-terminate dst.
 *
 * This function is similar to BSD strlcat(), but differs when
 * size <= strlen(dst).
 *
 * @param dst destination buffer
 * @param src source string
 * @param size size of destination buffer
 * @return the total length of src and dst
 *
 * @warning since the return value use the length of src and dst, these
 * absolutely _must_ be a properly 0-terminated strings, otherwise this
 * will read beyond the end of the buffer and possibly crash.
 */
size_t fx_strlcat(char *dst, const char *src, size_t size)
{
    size_t len = strlen(dst);
    if (size <= len + 1)
        return len + strlen(src);
    return len + fx_strlcpy(dst + len, src, size - len);
}


/**
 * Append output to a string, according to a format. Never write out of
 * the destination buffer, and always put a terminating 0 within
 * the buffer.
 * @param dst destination buffer (string to which the output is
 *  appended)
 * @param size total size of the destination buffer
 * @param fmt printf-compatible format string, specifying how the
 *  following parameters are used
 * @return the length of the string that would have been generated
 *  if enough space had been available
 */
size_t fx_strlcatf(char *dst, size_t size, const char *fmt, ...)
{
    size_t len = strlen(dst);
    va_list vl;

    va_start(vl, fmt);
    len += vsnprintf(dst + len, size > len ? size - len : 0, fmt, vl);
    va_end(vl);

    return len;
}


/**
 * Get the count of continuous non zero chars starting from the beginning.
 *
 * @param len maximum number of characters to check in the string, that
 *            is the maximum value which is returned by the function
 */
static inline size_t fx_strnlen(const char *s, size_t len)
{
    size_t i;
    for (i = 0; i < len && s[i]; i++)
        ;
    return i;
}


/**
 * Split the string into several tokens which can be accessed by
 * successive calls to fx_strtok().
 *
 * A token is defined as a sequence of characters not belonging to the
 * set specified in delim.
 *
 * On the first call to fx_strtok(), s should point to the string to
 * parse, and the value of saveptr is ignored. In subsequent calls, s
 * should be NULL, and saveptr should be unchanged since the previous
 * call.
 *
 * This function is similar to strtok_r() defined in POSIX.1.
 *
 * @param s the string to parse, may be NULL
 * @param delim 0-terminated list of token delimiters, must be non-NULL
 * @param saveptr user-provided pointer which points to stored
 * information necessary for fx_strtok() to continue scanning the same
 * string. saveptr is updated to point to the next character after the
 * first delimiter found, or to NULL if the string was terminated
 * @return the found token, or NULL when no token is found
 */
char *fx_strtok(char *s, const char *delim, char **saveptr)
{
    char *tok;

    if (!s && !(s = *saveptr))
        return NULL;

    /* skip leading delimiters */
    s += strspn(s, delim);

    /* s now points to the first non delimiter char, or to the end of the string */
    if (!*s) {
        *saveptr = NULL;
        return NULL;
    }
    tok = s++;

    /* skip non delimiters */
    s += strcspn(s, delim);
    if (*s) {
        *s = 0;
        *saveptr = s+1;
    } else {
        *saveptr = NULL;
    }

    return tok;
}


/**
 * Locale-independent conversion of ASCII isdigit.
 */
static int fx_isdigit(int c)
{
    return c >= '0' && c <= '9';
}

/**
 * Locale-independent conversion of ASCII isgraph.
 */
static  int fx_isgraph(int c)
{
    return c > 32 && c < 127;
}

/**
 * Locale-independent conversion of ASCII isspace.
 */
static int fx_isspace(int c)
{
    return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' ||
           c == '\v';
}

/**
 * Locale-independent conversion of ASCII characters to uppercase.
 */
static int fx_toupper(int c)
{
    if (c >= 'a' && c <= 'z')
        c ^= 0x20;
    return c;
}

/**
 * Locale-independent conversion of ASCII characters to lowercase.
 */
static int fx_tolower(int c)
{
    if (c >= 'A' && c <= 'Z')
        c ^= 0x20;
    return c;
}

/**
 * Locale-independent conversion of ASCII isxdigit.
 */
static int fx_isxdigit(int c)
{
    c = fx_tolower(c);
    return fx_isdigit(c) || (c >= 'a' && c <= 'f');
}

/**
 * Locale-independent case-insensitive compare.
 * @note This means only ASCII-range characters are case-insensitive
 */
int fx_strcasecmp(const char *a, const char *b)
{
    uint8_t c1, c2;
    do {
        c1 = fx_tolower(*a++);
        c2 = fx_tolower(*b++);
    } while (c1 && c1 == c2);
    return c1 - c2;
}


/**
 * Locale-independent case-insensitive compare.
 * @note This means only ASCII-range characters are case-insensitive
 */
int fx_strncasecmp(const char *a, const char *b, size_t n)
{
    const char *end = a + n;
    uint8_t c1, c2;
    do {
        c1 = fx_tolower(*a++);
        c2 = fx_tolower(*b++);
    } while (a < end && c1 && c1 == c2);
    return c1 - c2;
}



/**
 * Thread safe basename.
 * @param path the path, on DOS both \ and / are considered separators.
 * @return pointer to the basename substring.
 */
const char *fx_basename(const char *path)
{
    char *p = strrchr(path, '/');

#if HAVE_DOS_PATHS
    char *q = strrchr(path, '\\');
    char *d = strchr(path, ':');

    p = MAX3(p, q, d);
#endif

    if (!p)
        return path;

    return p + 1;
}


/**
 * Thread safe dirname.
 * @param path the path, on DOS both \ and / are considered separators.
 * @return the path with the separator replaced by the string terminator or ".".
 * @note the function may change the input string.
 */
const char *fx_dirname(char *path)
{
    char *p = strrchr(path, '/');

#if HAVE_DOS_PATHS
    char *q = strrchr(path, '\\');
    char *d = strchr(path, ':');

    d = d ? d + 1 : d;

    p = MAX3(p, q, d);
#endif

    if (!p)
        return ".";

    *p = '\0';

    return path;
}


/**
 * Match instances of a name in a comma-separated list of names.
 * List entries are checked from the start to the end of the names list,
 * the first match ends further processing. If an entry prefixed with '-'
 * matches, then 0 is returned. The "ALL" list entry is considered to
 * match all names.
 *
 * @param name  Name to look for.
 * @param names List of names.
 * @return 1 on match, 0 otherwise.
 */
int fx_match_name_internal(const char *name, const char *names,char delim)
{
    const char *p;
    int len, namelen;

    if (!name || !names)
        return 0;

    namelen = strlen(name);
    while (*names) {
        int negate = '-' == *names;
        p = strchr(names, ',');
        if (!p)
            p = names + strlen(names);
        names += negate;
        len = MAX(p - names, namelen);
        if (!fx_strncasecmp(name, names, len) || !strncmp("ALL", names, MAX(3, p - names)))
            return !negate;
        names = p + (*p == ',');
    }
    return 0;
}
int fx_match_name(const char *name, const char *names)
{
	return fx_match_name_internal(name,names,',');
}


/**
 * Check if a name is in a list.
 * @returns 0 if not found, or the 1 based index where it has been found in the
 *            list.
 */
int fx_match_list(const char *name, const char *list, char separator)
{
    const char *p, *q;

    for (p = name; p && *p; ) {
        for (q = list; q && *q; ) {
            int k;
            for (k = 0; p[k] == q[k] || (p[k]*q[k] == 0 && p[k]+q[k] == separator); k++)
                if (k && (!p[k] || p[k] == separator))
                    return 1;
            q = strchr(q, separator);
            q += !!q;
        }
        p = strchr(p, separator);
        p += !!p;
    }

    return 0;
}



#endif
