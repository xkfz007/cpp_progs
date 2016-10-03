/*
 * Copyright (c) 2012 Nicolas George
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

#ifndef AVUTIL_BPRINT_H
#define AVUTIL_BPRINT_H

#include <stdarg.h>
#include <time.h>
//
//#include "attributes.h"
//#include "avstring.h"
#include "fx_types.h"
#include "fx_mathops.h"
#include "fx_error.h"
#include "fx_memops.h"

#if !defined(va_copy) && defined(_MSC_VER)
#define va_copy(dst, src) ((dst) = (src))
#endif
#if !defined(va_copy) && defined(__GNUC__) && __GNUC__ < 3
#define va_copy(dst, src) __va_copy(dst, src)
#endif
/**
 * Define a structure with extra padding to a fixed size
 * This helps ensuring binary compatibility with future versions.
 */

#define FF_PAD_STRUCTURE(name, size, ...) \
struct ff_pad_helper_##name { __VA_ARGS__ }; \
typedef struct name { \
    __VA_ARGS__ \
    char reserved_padding[size - sizeof(struct ff_pad_helper_##name)]; \
} name;

/**
 * Buffer to print data progressively
 *
 * The string buffer grows as necessary and is always 0-terminated.
 * The content of the string is never accessed, and thus is
 * encoding-agnostic and can even hold binary data.
 *
 * Small buffers are kept in the structure itself, and thus require no
 * memory allocation at all (unless the contents of the buffer is needed
 * after the structure goes out of scope). This is almost as lightweight as
 * declaring a local "char buf[512]".
 *
 * The length of the string can go beyond the allocated size: the buffer is
 * then truncated, but the functions still keep account of the actual total
 * length.
 *
 * In other words, buf->len can be greater than buf->size and records the
 * total length of what would have been to the buffer if there had been
 * enough memory.
 *
 * Append operations do not need to be tested for failure: if a memory
 * allocation fails, data stop being appended to the buffer, but the length
 * is still updated. This situation can be tested with
 * fx_bprint_is_complete().
 *
 * The size_max field determines several possible behaviours:
 *
 * size_max = -1 (= UINT_MAX) or any large value will let the buffer be
 * reallocated as necessary, with an amortized linear cost.
 *
 * size_max = 0 prevents writing anything to the buffer: only the total
 * length is computed. The write operations can then possibly be repeated in
 * a buffer with exactly the necessary size
 * (using size_init = size_max = len + 1).
 *
 * size_max = 1 is automatically replaced by the exact size available in the
 * structure itself, thus ensuring no dynamic memory allocation. The
 * internal buffer is large enough to hold a reasonable paragraph of text,
 * such as the current paragraph.
 */

FF_PAD_STRUCTURE(AVBPrint, 1024,
    char *str;         /**< string so far */
    unsigned len;      /**< length so far */
    unsigned size;     /**< allocated memory */
    unsigned size_max; /**< maximum allocated memory */
    char reserved_internal_buffer[1];
)

/**
 * Convenience macros for special values for fx_bprint_init() size_max
 * parameter.
 */
#define FX_BPRINT_SIZE_UNLIMITED  ((unsigned)-1)
#define FX_BPRINT_SIZE_AUTOMATIC  1
#define FX_BPRINT_SIZE_COUNT_ONLY 0

#define fx_bprint_room(buf) ((buf)->size - MIN((buf)->len, (buf)->size))
#define fx_bprint_is_allocated(buf) ((buf)->str != (buf)->reserved_internal_buffer)



/**
 * Test if the print buffer is complete (not truncated).
 *
 * It may have been truncated due to a memory allocation failure
 * or the size_max limit (compare size and size_max if necessary).
 */
static inline int fx_bprint_is_complete(const AVBPrint *buf)
{
    return buf->len < buf->size;

}

static int fx_bprint_alloc(AVBPrint *buf, unsigned room)
{
    char *old_str, *new_str;
    unsigned min_size, new_size;

    if (buf->size == buf->size_max)
        return AVERROR(EIO);
    if (!fx_bprint_is_complete(buf))
        return AVERROR_INVALIDDATA; /* it is already truncated anyway */
    min_size = buf->len + 1 + MIN(UINT_MAX - buf->len - 1, room);
    new_size = buf->size > buf->size_max / 2 ? buf->size_max : buf->size * 2;
    if (new_size < min_size)
        new_size = MIN(buf->size_max, min_size);
    old_str = fx_bprint_is_allocated(buf) ? buf->str : NULL;
    new_str = fx_realloc(old_str, new_size);
    if (!new_str)
        return AVERROR(ENOMEM);
    if (!old_str)
        memcpy(new_str, buf->str, buf->len + 1);
    buf->str  = new_str;
    buf->size = new_size;
    return 0;
}

static void fx_bprint_grow(AVBPrint *buf, unsigned extra_len)
{
    /* arbitrary margin to avoid small overflows */
    extra_len = MIN(extra_len, UINT_MAX - 5 - buf->len);
    buf->len += extra_len;
    if (buf->size)
        buf->str[MIN(buf->len, buf->size - 1)] = 0;
}
/**
 * Init a print buffer.
 *
 * @param buf        buffer to init
 * @param size_init  initial size (including the final 0)
 * @param size_max   maximum size;
 *                   0 means do not write anything, just count the length;
 *                   1 is replaced by the maximum value for automatic storage;
 *                   any large value means that the internal buffer will be
 *                   reallocated as needed up to that limit; -1 is converted to
 *                   UINT_MAX, the largest limit possible.
 *                   Check also fx_BPRINT_SIZE_* macros.
 */
void fx_bprint_init(AVBPrint *buf, unsigned size_init, unsigned size_max)
{
    unsigned size_auto = (char *)buf + sizeof(*buf) -
                         buf->reserved_internal_buffer;

    if (size_max == 1)
        size_max = size_auto;
    buf->str      = buf->reserved_internal_buffer;
    buf->len      = 0;
    buf->size     = MIN(size_auto, size_max);
    buf->size_max = size_max;
    *buf->str = 0;
    if (size_init > buf->size)
        fx_bprint_alloc(buf, size_init - 1);
}

/**
 * Init a print buffer using a pre-existing buffer.
 *
 * The buffer will not be reallocated.
 *
 * @param buf     buffer structure to init
 * @param buffer  byte buffer to use for the string data
 * @param size    size of buffer
 */
void fx_bprint_init_for_buffer(AVBPrint *buf, char *buffer, unsigned size)
{
    buf->str      = buffer;
    buf->len      = 0;
    buf->size     = size;
    buf->size_max = size;
    *buf->str = 0;
}

/**
 * Append a formatted string to a print buffer.
 */
void fx_bprintf(AVBPrint *buf, const char *fmt, ...)
{
    unsigned room;
    char *dst;
    va_list vl;
    int extra_len;

    while (1) {
        room = fx_bprint_room(buf);
        dst = room ? buf->str + buf->len : NULL;
        va_start(vl, fmt);
        extra_len = vsnprintf(dst, room, fmt, vl);
        va_end(vl);
        if (extra_len <= 0)
            return;
        if (extra_len < room)
            break;
        if (fx_bprint_alloc(buf, extra_len))
            break;
    }
    fx_bprint_grow(buf, extra_len);
}

/**
 * Append a formatted string to a print buffer.
 */
void fx_vbprintf(AVBPrint *buf, const char *fmt, va_list vl_arg)
{
    unsigned room;
    char *dst;
    int extra_len;
    va_list vl;

    while (1) {
        room = fx_bprint_room(buf);
        dst = room ? buf->str + buf->len : NULL;
        va_copy(vl, vl_arg);
        extra_len = vsnprintf(dst, room, fmt, vl);
        va_end(vl);
        if (extra_len <= 0)
            return;
        if (extra_len < room)
            break;
        if (fx_bprint_alloc(buf, extra_len))
            break;
    }
    fx_bprint_grow(buf, extra_len);
}

/**
 * Append char c n times to a print buffer.
 */
void fx_bprint_chars(AVBPrint *buf, char c, unsigned n)
{
    unsigned room, real_n;

    while (1) {
        room = fx_bprint_room(buf);
        if (n < room)
            break;
        if (fx_bprint_alloc(buf, n))
            break;
    }
    if (room) {
        real_n = MIN(n, room - 1);
        memset(buf->str + buf->len, c, real_n);
    }
    fx_bprint_grow(buf, n);
}

/**
 * Append data to a print buffer.
 *
 * param buf  bprint buffer to use
 * param data pointer to data
 * param size size of data
 */
void fx_bprint_append_data(AVBPrint *buf, const char *data, unsigned size)
{
    unsigned room, real_n;

    while (1) {
        room = fx_bprint_room(buf);
        if (size < room)
            break;
        if (fx_bprint_alloc(buf, size))
            break;
    }
    if (room) {
        real_n = MIN(size, room - 1);
        memcpy(buf->str + buf->len, data, real_n);
    }
    fx_bprint_grow(buf, size);
}

struct tm;
/**
 * Append a formatted date and time to a print buffer.
 *
 * param buf  bprint buffer to use
 * param fmt  date and time format string, see strftime()
 * param tm   broken-down time structure to translate
 *
 * @note due to poor design of the standard strftime function, it may
 * produce poor results if the format string expands to a very long text and
 * the bprint buffer is near the limit stated by the size_max option.
 */

void fx_bprint_strftime(AVBPrint *buf, const char *fmt, const struct tm *tm)
{
    unsigned room;
    size_t l;

    if (!*fmt)
        return;
    while (1) {
        room = fx_bprint_room(buf);
        if (room && (l = strftime(buf->str + buf->len, room, fmt, tm)))
            break;
        /* strftime does not tell us how much room it would need: let us
           retry with twice as much until the buffer is large enough */
        room = !room ? strlen(fmt) + 1 :
               room <= INT_MAX / 2 ? room * 2 : INT_MAX;
        if (fx_bprint_alloc(buf, room)) {
            /* impossible to grow, try to manage something useful anyway */
            room = fx_bprint_room(buf);
            if (room < 1024) {
                /* if strftime fails because the buffer has (almost) reached
                   its maximum size, let us try in a local buffer; 1k should
                   be enough to format any real date+time string */
                char buf2[1024];
                if ((l = strftime(buf2, sizeof(buf2), fmt, tm))) {
                    fx_bprintf(buf, "%s", buf2);
                    return;
                }
            }
            if (room) {
                /* if anything else failed and the buffer is not already
                   truncated, let us add a stock string and force truncation */
                static const char txt[] = "[truncated strftime output]";
                memset(buf->str + buf->len, '!', room);
                memcpy(buf->str + buf->len, txt, MIN(sizeof(txt) - 1, room));
                fx_bprint_grow(buf, room); /* force truncation */
            }
            return;
        }
    }
    fx_bprint_grow(buf, l);
}

/**
 * Allocate bytes in the buffer for external use.
 *
 * @param[in]  buf          buffer structure
 * @param[in]  size         required size
 * @param[out] mem          pointer to the memory area
 * @param[out] actual_size  size of the memory area after allocation;
 *                          can be larger or smaller than size
 */
void fx_bprint_get_buffer(AVBPrint *buf, unsigned size,
                          unsigned char **mem, unsigned *actual_size)
{
    if (size > fx_bprint_room(buf))
        fx_bprint_alloc(buf, size);
    *actual_size = fx_bprint_room(buf);
    *mem = *actual_size ? buf->str + buf->len : NULL;
}

/**
 * Reset the string to "" but keep internal allocated data.
 */
void fx_bprint_clear(AVBPrint *buf)
{
    if (buf->len) {
        *buf->str = 0;
        buf->len  = 0;
    }
}

/**
 * Finalize a print buffer.
 *
 * The print buffer can no longer be used afterwards,
 * but the len and size fields are still valid.
 *
 * @arg[out] ret_str  if not NULL, used to return a permanent copy of the
 *                    buffer contents, or NULL if memory allocation fails;
 *                    if NULL, the buffer is discarded and freed
 * @return  0 for success or error code (probably AVERROR(ENOMEM))
 */

int fx_bprint_finalize(AVBPrint *buf, char **ret_str)
{
    unsigned real_size = MIN(buf->len + 1, buf->size);
    char *str;
    int ret = 0;

    if (ret_str) {
        if (fx_bprint_is_allocated(buf)) {
            str = fx_realloc(buf->str, real_size);
            if (!str)
                str = buf->str;
            buf->str = NULL;
        } else {
            str = fx_malloc(real_size);
            if (str)
                memcpy(str, buf->str, real_size);
            else
                ret = AVERROR(ENOMEM);
        }
        *ret_str = str;
    } else {
        if (fx_bprint_is_allocated(buf))
            fx_freep(&buf->str);
    }
    buf->size = real_size;
    return ret;
}

/**
 * Escape the content in src and append it to dstbuf.
 *
 * @param dstbuf        already inited destination bprint buffer
 * @param src           string containing the text to escape
 * @param special_chars string containing the special characters which
 *                      need to be escaped, can be NULL
 * @param mode          escape mode to employ, see fx_ESCAPE_MODE_* macros.
 *                      Any unknown value for mode will be considered equivalent to
 *                      fx_ESCAPE_MODE_BACKSLASH, but this behaviour can change without
 *                      notice.
 * @param flags         flags which control how to escape, see fx_ESCAPE_FLAG_* macros
 */

#define WHITESPACES " \n\t\r"

void fx_bprint_escape(AVBPrint *dstbuf, const char *src, const char *special_chars,
                      enum AVEscapeMode mode, int flags)
{
    const char *src0 = src;

    if (mode == FX_ESCAPE_MODE_AUTO)
        mode = FX_ESCAPE_MODE_BACKSLASH; /* TODO: implement a heuristic */

    switch (mode) {
    case FX_ESCAPE_MODE_QUOTE:
        /* enclose the string between '' */
        fx_bprint_chars(dstbuf, '\'', 1);
        for (; *src; src++) {
            if (*src == '\'')
                fx_bprintf(dstbuf, "'\\''");
            else
                fx_bprint_chars(dstbuf, *src, 1);
        }
        fx_bprint_chars(dstbuf, '\'', 1);
        break;

    /* case fx_ESCAPE_MODE_BACKSLASH or unknown mode */
    default:
        /* \-escape characters */
        for (; *src; src++) {
            int is_first_last       = src == src0 || !*(src+1);
            int is_ws               = !!strchr(WHITESPACES, *src);
            int is_strictly_special = special_chars && strchr(special_chars, *src);
            int is_special          =
                is_strictly_special || strchr("'\\", *src) ||
                (is_ws && (flags & FX_ESCAPE_FLAG_WHITESPACE));

            if (is_strictly_special ||
                (!(flags & FX_ESCAPE_FLAG_STRICT) &&
                 (is_special || (is_ws && is_first_last))))
                fx_bprint_chars(dstbuf, '\\', 1);
            fx_bprint_chars(dstbuf, *src, 1);
        }
        break;
    }
}

#endif /* AVUTIL_BPRINT_H */
