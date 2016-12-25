/*
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * Public dictionary API.
 * @deprecated
 *  AVDictionary is provided for compatibility with libav. It is both in
 *  implementation as well as API inefficient. It does not scale and is
 *  extremely slow with large dictionaries.
 *  It is recommended that new code uses our tree container from tree.c/h
 *  where applicable, which uses AVL trees to achieve O(log n) performance.
 */

#ifndef AVUTIL_DICT_H
#define AVUTIL_DICT_H


#include "fx_types.h"
//#include "version.h"
#include "fx_memops.h"
#include "fx_bprint.h"
#include "fx_time.h"
#include "fx_string.h"
#include "fx_string2.h"

/**
 * @addtogroup lavu_dict AVDictionary
 * @ingroup lavu_data
 *
 * @brief Simple key:value store
 *
 * @{
 * Dictionaries are used for storing key:value pairs. To create
 * an AVDictionary, simply pass an address of a NULL pointer to
 * fx_dict_set(). NULL can be used as an empty dictionary wherever
 * a pointer to an AVDictionary is required.
 * Use fx_dict_get() to retrieve an entry or iterate over all
 * entries and finally fx_dict_free() to free the dictionary
 * and all its contents.
 *
 @code
   AVDictionary *d = NULL;           // "create" an empty dictionary
   AVDictionaryEntry *t = NULL;

   fx_dict_set(&d, "foo", "bar", 0); // add an entry

   char *k = fx_strdup("key");       // if your strings are already allocated,
   char *v = fx_strdup("value");     // you can avoid copying them like this
   fx_dict_set(&d, k, v, fx_DICT_DONT_STRDUP_KEY | fx_DICT_DONT_STRDUP_VAL);

   while (t = fx_dict_get(d, "", t, fx_DICT_IGNORE_SUFFIX)) {
       <....>                             // iterate over all entries in d
   }
   fx_dict_free(&d);
 @endcode
 */

#define FX_DICT_MATCH_CASE      1   /**< Only get an entry with exact-case key match. Only relevant in fx_dict_get(). */
#define FX_DICT_IGNORE_SUFFIX   2   /**< Return first entry in a dictionary whose first part corresponds to the search key,
                                         ignoring the suffix of the found key string. Only relevant in fx_dict_get(). */
#define FX_DICT_DONT_STRDUP_KEY 4   /**< Take ownership of a key that's been
                                         allocated with fx_malloc() or another memory allocation function. */
#define FX_DICT_DONT_STRDUP_VAL 8   /**< Take ownership of a value that's been
                                         allocated with fx_malloc() or another memory allocation function. */
#define FX_DICT_DONT_OVERWRITE 16   ///< Don't overwrite existing entries.
#define FX_DICT_APPEND         32   /**< If the entry already exists, append to it.  Note that no
                                      delimiter is added, the strings are simply concatenated. */
#define FX_DICT_MULTIKEY       64   /**< Allow to store several equal keys in the dictionary */

typedef struct AVDictionaryEntry {
    char *key;
    char *value;
} AVDictionaryEntry;

struct AVDictionary {
    int count;
    AVDictionaryEntry *elems;
};

typedef struct AVDictionary AVDictionary;

/**
 * Get a dictionary entry with matching key.
 *
 * The returned entry key or value must not be changed, or it will
 * cause undefined behavior.
 *
 * To iterate through all the dictionary entries, you can set the matching key
 * to the null string "" and set the fx_DICT_IGNORE_SUFFIX flag.
 *
 * @param prev Set to the previous matching element to find the next.
 *             If set to NULL the first matching element is returned.
 * @param key matching key
 * @param flags a collection of fx_DICT_* flags controlling how the entry is retrieved
 * @return found entry or NULL in case no matching entry was found in the dictionary
 */
AVDictionaryEntry *fx_dict_get(const AVDictionary *m, const char *key,
                               const AVDictionaryEntry *prev, int flags)
{
    unsigned int i, j;

    if (!m)
        return NULL;

    if (prev)
        i = prev - m->elems + 1;
    else
        i = 0;

    for (; i < m->count; i++) {
        const char *s = m->elems[i].key;
        if (flags & FX_DICT_MATCH_CASE)
            for (j = 0; s[j] == key[j] && key[j]; j++)
                ;
        else
            for (j = 0; fx_toupper(s[j]) == fx_toupper(key[j]) && key[j]; j++)
                ;
        if (key[j])
            continue;
        if (s[j] && !(flags & FX_DICT_IGNORE_SUFFIX))
            continue;
        return &m->elems[i];
    }
    return NULL;
}

/**
 * Get number of entries in dictionary.
 *
 * @param m dictionary
 * @return  number of entries in dictionary
 */
static int fx_dict_count(const AVDictionary *m)
{
    return m ? m->count : 0;
}

/**
 * Set the given entry in *pm, overwriting an existing entry.
 *
 * Note: If fx_DICT_DONT_STRDUP_KEY or fx_DICT_DONT_STRDUP_VAL is set,
 * these arguments will be freed on error.
 *
 * Warning: Adding a new entry to a dictionary invalidates all existing entries
 * previously returned with fx_dict_get.
 *
 * @param pm pointer to a pointer to a dictionary struct. If *pm is NULL
 * a dictionary struct is allocated and put in *pm.
 * @param key entry key to add to *pm (will either be fx_strduped or added as a new key depending on flags)
 * @param value entry value to add to *pm (will be fx_strduped or added as a new key depending on flags).
 *        Passing a NULL value will cause an existing entry to be deleted.
 * @return >= 0 on success otherwise an error code <0
 */
int fx_dict_set(AVDictionary **pm, const char *key, const char *value,
                int flags)
{
    AVDictionary *m = *pm;
    AVDictionaryEntry *tag = NULL;
    char *oldval = NULL, *copy_key = NULL, *copy_value = NULL;

    if (!(flags & FX_DICT_MULTIKEY)) {
        tag = fx_dict_get(m, key, NULL, flags);
    }
    if (flags & FX_DICT_DONT_STRDUP_KEY)
        copy_key = (void *)key;
    else
        copy_key = fx_strdup(key);
    if (flags & FX_DICT_DONT_STRDUP_VAL)
        copy_value = (void *)value;
    else if (copy_key)
        copy_value = fx_strdup(value);
    if (!m)
        m = *pm = fx_mallocz(sizeof(*m));
    if (!m || (key && !copy_key) || (value && !copy_value))
        goto err_out;

    if (tag) {
        if (flags & FX_DICT_DONT_OVERWRITE) {
            fx_free(copy_key);
            fx_free(copy_value);
            return 0;
        }
        if (flags & FX_DICT_APPEND)
            oldval = tag->value;
        else
            fx_free(tag->value);
        fx_free(tag->key);
        *tag = m->elems[--m->count];
    } else if (copy_value) {
        AVDictionaryEntry *tmp = fx_realloc(m->elems,
                                            (m->count + 1) * sizeof(*m->elems));
        if (!tmp)
            goto err_out;
        m->elems = tmp;
    }
    if (copy_value) {
        m->elems[m->count].key = copy_key;
        m->elems[m->count].value = copy_value;
        if (oldval && flags & FX_DICT_APPEND) {
            size_t len = strlen(oldval) + strlen(copy_value) + 1;
            char *newval = fx_mallocz(len);
            if (!newval)
                goto err_out;
            fx_strlcat(newval, oldval, len);
            fx_freep(&oldval);
            fx_strlcat(newval, copy_value, len);
            m->elems[m->count].value = newval;
            fx_freep(&copy_value);
        }
        m->count++;
    } else {
        fx_freep(&copy_key);
    }
    if (!m->count) {
        fx_freep(&m->elems);
        fx_freep(pm);
    }

    return 0;

err_out:
    if (m && !m->count) {
        fx_freep(&m->elems);
        fx_freep(pm);
    }
    fx_free(copy_key);
    fx_free(copy_value);
    return -1;
}

/**
 * Convenience wrapper for fx_dict_set that converts the value to a string
 * and stores it.
 *
 * Note: If fx_DICT_DONT_STRDUP_KEY is set, key will be freed on error.
 */
int fx_dict_set_int(AVDictionary **pm, const char *key, int64_t value,
                int flags)
{
    char valuestr[22];
    snprintf(valuestr, sizeof(valuestr), "%"PRId64, value);
    flags &= ~FX_DICT_DONT_STRDUP_VAL;
    return fx_dict_set(pm, key, valuestr, flags);
}

/**
 * Parse the key/value pairs list and add the parsed entries to a dictionary.
 *
 * In case of failure, all the successfully set entries are stored in
 * *pm. You may need to manually free the created dictionary.
 *
 * @param key_val_sep  a 0-terminated list of characters used to separate
 *                     key from value
 * @param pairs_sep    a 0-terminated list of characters used to separate
 *                     two pairs from each other
 * @param flags        flags to use when adding to dictionary.
 *                     fx_DICT_DONT_STRDUP_KEY and fx_DICT_DONT_STRDUP_VAL
 *                     are ignored since the key/value tokens will always
 *                     be duplicated.
 * @return             0 on success, negative AVERROR code on failure
 */
static int parse_key_value_pair(AVDictionary **pm, const char **buf,
                                const char *key_val_sep, const char *pairs_sep,
                                int flags)
{
    char *key = fx_get_token(buf, key_val_sep);
    char *val = NULL;
    int ret;

    if (key && *key && strspn(*buf, key_val_sep)) {
        (*buf)++;
        val = fx_get_token(buf, pairs_sep);
    }

    if (key && *key && val && *val)
        ret = fx_dict_set(pm, key, val, flags);
    else
        ret = -1;

    fx_freep(&key);
    fx_freep(&val);

    return ret;
}

/**
 * Copy entries from one AVDictionary struct into another.
 * @param dst pointer to a pointer to a AVDictionary struct. If *dst is NULL,
 *            this function will allocate a struct for you and put it in *dst
 * @param src pointer to source AVDictionary struct
 * @param flags flags to use when setting entries in *dst
 * @note metadata is read using the fx_DICT_IGNORE_SUFFIX flag
 * @return 0 on success, negative AVERROR code on failure. If dst was allocated
 *           by this function, callers should free the associated memory.
 */
int fx_dict_copy(AVDictionary **dst, const AVDictionary *src, int flags)
{
    AVDictionaryEntry *t = NULL;

    while ((t = fx_dict_get(src, "", t, FX_DICT_IGNORE_SUFFIX))) {
        int ret = fx_dict_set(dst, t->key, t->value, flags);
        if (ret < 0)
            return ret;
    }

    return 0;
}

/**
 * Free all the memory allocated for an AVDictionary struct
 * and all keys and values.
 */
void fx_dict_free(AVDictionary **pm)
{
    AVDictionary *m = *pm;

    if (m) {
        while (m->count--) {
            fx_freep(&m->elems[m->count].key);
            fx_freep(&m->elems[m->count].value);
        }
        fx_freep(&m->elems);
    }
    fx_freep(pm);
}

int fx_dict_parse_string(AVDictionary **pm, const char *str,
                         const char *key_val_sep, const char *pairs_sep,
                         int flags)
{
    int ret;

    if (!str)
        return 0;

    /* ignore STRDUP flags */
    flags &= ~(FX_DICT_DONT_STRDUP_KEY | FX_DICT_DONT_STRDUP_VAL);

    while (*str) {
        if ((ret = parse_key_value_pair(pm, &str, key_val_sep, pairs_sep, flags)) < 0)
            return ret;

        if (*str)
            str++;
    }

    return 0;
}

/**
 * Get dictionary entries as a string.
 *
 * Create a string containing dictionary's entries.
 * Such string may be passed back to fx_dict_parse_string().
 * @note String is escaped with backslashes ('\').
 *
 * @param[in]  m             dictionary
 * @param[out] buffer        Pointer to buffer that will be allocated with string containg entries.
 *                           Buffer must be freed by the caller when is no longer needed.
 * @param[in]  key_val_sep   character used to separate key from value
 * @param[in]  pairs_sep     character used to separate two pairs from each other
 * @return                   >= 0 on success, negative on error
 * @warning Separators cannot be neither '\\' nor '\0'. They also cannot be the same.
 */

int fx_dict_get_string(const AVDictionary *m, char **buffer,
                       const char key_val_sep, const char pairs_sep)
{
    AVDictionaryEntry *t = NULL;
    AVBPrint bprint;
    int cnt = 0;
    char special_chars[] = {pairs_sep, key_val_sep, '\0'};

    if (!buffer || pairs_sep == '\0' || key_val_sep == '\0' || pairs_sep == key_val_sep ||
        pairs_sep == '\\' || key_val_sep == '\\')
        return -1;

    if (!fx_dict_count(m)) {
        *buffer = fx_strdup("");
        return *buffer ? 0 : -1;
    }

    fx_bprint_init(&bprint, 64, FX_BPRINT_SIZE_UNLIMITED);
    while ((t = fx_dict_get(m, "", t, FX_DICT_IGNORE_SUFFIX))) {
        if (cnt++)
            fx_bprint_append_data(&bprint, &pairs_sep, 1);
        fx_bprint_escape(&bprint, t->key, special_chars, FX_ESCAPE_MODE_BACKSLASH, 0);
        fx_bprint_append_data(&bprint, &key_val_sep, 1);
        fx_bprint_escape(&bprint, t->value, special_chars, FX_ESCAPE_MODE_BACKSLASH, 0);
    }
    return fx_bprint_finalize(&bprint, buffer);
}


int avpriv_dict_set_timestamp(AVDictionary **dict, const char *key, int64_t timestamp)
{
    time_t seconds = timestamp / 1000000;
    struct tm *ptm, tmbuf;
    ptm = fx_gmtime_r(&seconds, &tmbuf);
    if (ptm) {
        char buf[32];
        if (!strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", ptm))
            return AVERROR_EXTERNAL;
        fx_strlcatf(buf, sizeof(buf), ".%06dZ", (int)(timestamp % 1000000));
        return fx_dict_set(dict, key, buf, 0);
    } else {
        return AVERROR_EXTERNAL;
    }
}

/**
 * @}
 */

#endif /* AVUTIL_DICT_H */
