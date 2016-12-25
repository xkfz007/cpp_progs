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
 * misc parsing utilities
 */

//#define _PARSEUTILS
#ifdef _PARSEUTILS
#include <time.h>

#include "fx_macros.h"
#include "fx_parseutils.h"

#define fx_get_random_seed fx_get_random_seed_deterministic
static uint32_t fx_get_random_seed_deterministic(void);

#define fx_gettime() 1331972053200000

static uint32_t randomv = MKTAG('L','A','V','U');

static uint32_t fx_get_random_seed_deterministic(void)
{
    return randomv = randomv * 1664525 + 1013904223;
}

static void test_fx_parse_video_rate(void)
{
    int i;
    static const char *const rates[] = {
        "-inf",
        "inf",
        "nan",
        "123/0",
        "-123 / 0",
        "",
        "/",
        " 123  /  321",
        "foo/foo",
        "foo/1",
        "1/foo",
        "0/0",
        "/0",
        "1/",
        "1",
        "0",
        "-123/123",
        "-foo",
        "123.23",
        ".23",
        "-.23",
        "-0.234",
        "-0.0000001",
        "  21332.2324   ",
        " -21332.2324   ",
    };

    for (i = 0; i < ARRAY_ELEMS(rates); i++) {
        int ret;
        AVRational q = { 0, 0 };
        ret = fx_parse_video_rate(&q, rates[i]);
        printf("'%s' -> %d/%d %s\n",
               rates[i], q.num, q.den, ret ? "ERROR" : "OK");
    }
}

static void test_fx_parse_color(void)
{
    int i;
    uint8_t rgba[4];
    static const char *const color_names[] = {
        "bikeshed",
        "RaNdOm",
        "foo",
        "red",
        "Red ",
        "RED",
        "Violet",
        "Yellow",
        "Red",
        "0x000000",
        "0x0000000",
        "0xff000000",
        "0x3e34ff",
        "0x3e34ffaa",
        "0xffXXee",
        "0xfoobar",
        "0xffffeeeeeeee",
        "#ff0000",
        "#ffXX00",
        "ff0000",
        "ffXX00",
        "red@foo",
        "random@10",
        "0xff0000@1.0",
        "red@",
        "red@0xfff",
        "red@0xf",
        "red@2",
        "red@0.1",
        "red@-1",
        "red@0.5",
        "red@1.0",
        "red@256",
        "red@10foo",
        "red@-1.0",
        "red@-0.0",
    };

    fx_log_set_level(FX_LOG_DEBUG);

    for (i = 0;  i < ARRAY_ELEMS(color_names); i++) {
        if (fx_parse_color(rgba, color_names[i], -1, NULL) >= 0)
            printf("%s -> R(%d) G(%d) B(%d) A(%d)\n",
                   color_names[i], rgba[0], rgba[1], rgba[2], rgba[3]);
        else
            printf("%s -> error\n", color_names[i]);
    }
}



static void test_fx_get_known_color_name(void)
{
    int i;
    const uint8_t *rgba;
    const char *color;

    for (i = 0; i < ARRAY_ELEMS(color_table); ++i) {
        color = fx_get_known_color_name(i, &rgba);
        if (color)
            printf("%s -> R(%d) G(%d) B(%d) A(%d)\n",
                    color, rgba[0], rgba[1], rgba[2], rgba[3]);
        else
            printf("Color ID: %d not found\n", i);
    }
}

static void test_fx_find_info_tag(void)
{
    static const char args[] = "?tag1=val1&tag2=val2&tag3=val3&tag41=value 41&tag42=random1";
    static const char *tags[] = {"tag1", "tag2", "tag3", "tag4", "tag41", "41", "random1"};
    char buff[16];
    int i;

    for (i = 0; i < ARRAY_ELEMS(tags); ++i) {
        if (fx_find_info_tag(buff, sizeof(buff), tags[i], args))
            printf("%d. %s found: %s\n", i, tags[i], buff);
        else
            printf("%d. %s not found\n", i, tags[i]);
    }
}

int main(void)
{
    printf("Testing fx_parse_video_rate()\n");
    test_fx_parse_video_rate();

    printf("\nTesting fx_parse_color()\n");
    test_fx_parse_color();

    printf("\nTesting fx_get_known_color_name()\n");
    test_fx_get_known_color_name();

    printf("\nTesting fx_find_info_tag()\n");
    test_fx_find_info_tag();
    return 0;
}









#endif