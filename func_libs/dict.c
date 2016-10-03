/*
 * copyright (c) 2009 Michael Niedermayer
 *
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
//#define _DICT
#ifdef _DICT
#include <string.h>
#include "fx_dict.h"
static void print_dict(const AVDictionary *m)
{
    AVDictionaryEntry *t = NULL;
    while ((t = fx_dict_get(m, "", t, FX_DICT_IGNORE_SUFFIX)))
        printf("%s %s   ", t->key, t->value);
    printf("\n");
}

static void test_separators(const AVDictionary *m, const char pair, const char val)
{
    AVDictionary *dict = NULL;
    char pairs[] = {pair , '\0'};
    char vals[]  = {val, '\0'};

    char *buffer = NULL;
    fx_dict_copy(&dict, m, 0);
    print_dict(dict);
    fx_dict_get_string(dict, &buffer, val, pair);
    printf("%s\n", buffer);
    fx_dict_free(&dict);
    fx_dict_parse_string(&dict, buffer, vals, pairs, 0);
    fx_freep(&buffer);
    print_dict(dict);
    fx_dict_free(&dict);
}

int main(void)
{
    AVDictionary *dict = NULL;
    AVDictionaryEntry *e;
    char *buffer = NULL;

    printf("Testing fx_dict_get_string() and fx_dict_parse_string()\n");
    fx_dict_get_string(dict, &buffer, '=', ',');
    printf("%s\n", buffer);
    fx_freep(&buffer);
    fx_dict_set(&dict, "aaa", "aaa", 0);
    fx_dict_set(&dict, "b,b", "bbb", 0);
    fx_dict_set(&dict, "c=c", "ccc", 0);
    fx_dict_set(&dict, "ddd", "d,d", 0);
    fx_dict_set(&dict, "eee", "e=e", 0);
    fx_dict_set(&dict, "f,f", "f=f", 0);
    fx_dict_set(&dict, "g=g", "g,g", 0);
    test_separators(dict, ',', '=');
    fx_dict_free(&dict);
    fx_dict_set(&dict, "aaa", "aaa", 0);
    fx_dict_set(&dict, "bbb", "bbb", 0);
    fx_dict_set(&dict, "ccc", "ccc", 0);
    fx_dict_set(&dict, "\\,=\'\"", "\\,=\'\"", 0);
    test_separators(dict, '"',  '=');
    test_separators(dict, '\'', '=');
    test_separators(dict, ',', '"');
    test_separators(dict, ',', '\'');
    test_separators(dict, '\'', '"');
    test_separators(dict, '"', '\'');
    fx_dict_free(&dict);

    printf("\nTesting fx_dict_set()\n");
    fx_dict_set(&dict, "a", "a", 0);
    fx_dict_set(&dict, "b", fx_strdup("b"), FX_DICT_DONT_STRDUP_VAL);
    fx_dict_set(&dict, fx_strdup("c"), "c", FX_DICT_DONT_STRDUP_KEY);
    fx_dict_set(&dict, fx_strdup("d"), fx_strdup("d"), FX_DICT_DONT_STRDUP_KEY | FX_DICT_DONT_STRDUP_VAL);
    fx_dict_set(&dict, "e", "e", FX_DICT_DONT_OVERWRITE);
    fx_dict_set(&dict, "e", "f", FX_DICT_DONT_OVERWRITE);
    fx_dict_set(&dict, "f", "f", 0);
    fx_dict_set(&dict, "f", NULL, 0);
    fx_dict_set(&dict, "ff", "f", 0);
    fx_dict_set(&dict, "ff", "f", FX_DICT_APPEND);
    e = NULL;
    while ((e = fx_dict_get(dict, "", e, FX_DICT_IGNORE_SUFFIX)))
        printf("%s %s\n", e->key, e->value);
    fx_dict_free(&dict);

    fx_dict_set(&dict, NULL, "a", 0);
    fx_dict_set(&dict, NULL, "b", 0);
    fx_dict_get(dict, NULL, NULL, 0);
    e = NULL;
    while ((e = fx_dict_get(dict, "", e, FX_DICT_IGNORE_SUFFIX)))
        printf("'%s' '%s'\n", e->key, e->value);
    fx_dict_free(&dict);


    //valgrind sensible test
    printf("\nTesting fx_dict_set_int()\n");
    fx_dict_set_int(&dict, "1", 1, FX_DICT_DONT_STRDUP_VAL);
    fx_dict_set_int(&dict, fx_strdup("2"), 2, FX_DICT_DONT_STRDUP_KEY);
    fx_dict_set_int(&dict, fx_strdup("3"), 3, FX_DICT_DONT_STRDUP_KEY | FX_DICT_DONT_STRDUP_VAL);
    fx_dict_set_int(&dict, "4", 4, 0);
    fx_dict_set_int(&dict, "5", 5, FX_DICT_DONT_OVERWRITE);
    fx_dict_set_int(&dict, "5", 6, FX_DICT_DONT_OVERWRITE);
    fx_dict_set_int(&dict, "12", 1, 0);
    fx_dict_set_int(&dict, "12", 2, FX_DICT_APPEND);
    e = NULL;
    while ((e = fx_dict_get(dict, "", e, FX_DICT_IGNORE_SUFFIX)))
        printf("%s %s\n", e->key, e->value);
    fx_dict_free(&dict);

    //valgrind sensible test
    printf("\nTesting fx_dict_set() with existing AVDictionaryEntry.key as key\n");
    fx_dict_set(&dict, "key", "old", 0);
    e = fx_dict_get(dict, "key", NULL, 0);
    fx_dict_set(&dict, e->key, "new val OK", 0);
    e = fx_dict_get(dict, "key", NULL, 0);
    printf("%s\n", e->value);
    fx_dict_set(&dict, e->key, e->value, 0);
    e = fx_dict_get(dict, "key", NULL, 0);
    printf("%s\n", e->value);
    fx_dict_free(&dict);

    return 0;
}

















#endif