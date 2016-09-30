#ifndef _FX_STRING2_H
#define _FX_STRING2_H
#include "fx_memops.h"

/**
 * Unescape the given string until a non escaped terminating char,
 * and return the token corresponding to the unescaped string.
 *
 * The normal \ and ' escaping is supported. Leading and trailing
 * whitespaces are removed, unless they are escaped with '\' or are
 * enclosed between ''.
 *
 * @param buf the buffer to parse, buf will be updated to point to the
 * terminating char
 * @param term a 0-terminated list of terminating chars
 * @return the malloced unescaped string, which must be fx_freed by
 * the user, NULL in case of allocation failure
 */
#define WHITESPACES " \n\t\r"

char *fx_get_token(const char **buf, const char *term)
{
    char *out     = fx_malloc(strlen(*buf) + 1);
    char *ret     = out, *end = out;
    const char *p = *buf;
    if (!out)
        return NULL;
    p += strspn(p, WHITESPACES);

    while (*p && !strspn(p, term)) {
        char c = *p++;
        if (c == '\\' && *p) {
            *out++ = *p++;
            end    = out;
        } else if (c == '\'') {
            while (*p && *p != '\'')
                *out++ = *p++;
            if (*p) {
                p++;
                end = out;
            }
        } else {
            *out++ = c;
        }
    }

    do
        *out-- = 0;
    while (out >= end && strspn(out, WHITESPACES));

    *buf = p;

    return ret;
}

/**
 * Append path component to the existing path.
 * Path separator '/' is placed between when needed.
 * Resulting string have to be freed with fx_free().
 * @param path      base path
 * @param component component to be appended
 * @return new path or NULL on error.
 */
char *fx_append_path_component(const char *path, const char *component)
{
    size_t p_len, c_len;
    char *fullpath;

    if (!path)
        return fx_strdup(component);
    if (!component)
        return fx_strdup(path);

    p_len = strlen(path);
    c_len = strlen(component);
    if (p_len > SIZE_MAX - c_len || p_len + c_len > SIZE_MAX - 2)
        return NULL;
    fullpath = fx_malloc(p_len + c_len + 2);
    if (fullpath) {
        if (p_len) {
            fx_strlcpy(fullpath, path, p_len + 1);
            if (c_len) {
                if (fullpath[p_len - 1] != '/' && component[0] != '/')
                    fullpath[p_len++] = '/';
                else if (fullpath[p_len - 1] == '/' && component[0] == '/')
                    p_len--;
            }
        }
        fx_strlcpy(&fullpath[p_len], component, c_len + 1);
        fullpath[p_len + c_len] = 0;
    }
    return fullpath;
}

#endif