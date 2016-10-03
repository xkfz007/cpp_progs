/*
 * AVOptions
 * copyright (c) 2005 Michael Niedermayer <michaelni@gmx.at>
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

#ifndef AVUTIL_OPT_H
#define AVUTIL_OPT_H

#include "fx_memops.h"
#include "fx_audio.h"
#include "fx_image.h"
#include "fx_mathops.h"

/**
 * @file
 * AVOptions
 */
/**
 * @defgroup avoptions AVOptions
 * @ingroup lavu_data
 * @{
 * AVOptions provide a generic system to declare options on arbitrary structs
 * ("objects"). An option can have a help text, a type and a range of possible
 * values. Options may then be enumerated, read and written to.
 *
 * @section avoptions_implement Implementing AVOptions
 * This section describes how to add AVOptions capabilities to a struct.
 *
 * All AVOptions-related information is stored in an AVClass. Therefore
 * the first member of the struct should be a pointer to an AVClass describing it.
 * The option field of the AVClass must be set to a NULL-terminated static array
 * of AVOptions. Each AVOption must have a non-empty name, a type, a default
 * value and for number-type AVOptions also a range of allowed values. It must
 * also declare an offset in bytes from the start of the struct, where the field
 * associated with this AVOption is located. Other fields in the AVOption struct
 * should also be set when applicable, but are not required.
 *
 * The following example illustrates an AVOptions-enabled struct:
 * @code
 * typedef struct test_struct {
 *     const AVClass *class;
 *     int      int_opt;
 *     char    *str_opt;
 *     uint8_t *bin_opt;
 *     int      bin_len;
 * } test_struct;
 *
 * static const AVOption test_options[] = {
 *   { "test_int", "This is a test option of int type.", offsetof(test_struct, int_opt),
 *     FX_OPT_TYPE_INT, { .i64 = -1 }, INT_MIN, INT_MAX },
 *   { "test_str", "This is a test option of string type.", offsetof(test_struct, str_opt),
 *     FX_OPT_TYPE_STRING },
 *   { "test_bin", "This is a test option of binary type.", offsetof(test_struct, bin_opt),
 *     FX_OPT_TYPE_BINARY },
 *   { NULL },
 * };
 *
 * static const AVClass test_class = {
 *     .class_name = "test class",
 *     .item_name  = fx_default_item_name,
 *     .option     = test_options,
 *     .version    = LIBAVUTIL_VERSION_INT,
 * };
 * @endcode
 *
 * Next, when allocating your struct, you must ensure that the AVClass pointer
 * is set to the correct value. Then, fx_opt_set_defaults() can be called to
 * initialize defaults. After that the struct is ready to be used with the
 * AVOptions API.
 *
 * When cleaning up, you may use the fx_opt_free() function to automatically
 * free all the allocated string and binary options.
 *
 * Continuing with the above example:
 *
 * @code
 * test_struct *alloc_test_struct(void)
 * {
 *     test_struct *ret = fx_mallocz(sizeof(*ret));
 *     ret->class = &test_class;
 *     fx_opt_set_defaults(ret);
 *     return ret;
 * }
 * void free_test_struct(test_struct **foo)
 * {
 *     fx_opt_free(*foo);
 *     fx_freep(foo);
 * }
 * @endcode
 *
 * @subsection avoptions_implement_nesting Nesting
 *      It may happen that an AVOptions-enabled struct contains another
 *      AVOptions-enabled struct as a member (e.g. AVCodecContext in
 *      libavcodec exports generic options, while its priv_data field exports
 *      codec-specific options). In such a case, it is possible to set up the
 *      parent struct to export a child's options. To do that, simply
 *      implement AVClass.child_next() and AVClass.child_class_next() in the
 *      parent struct's AVClass.
 *      Assuming that the test_struct from above now also contains a
 *      child_struct field:
 *
 *      @code
 *      typedef struct child_struct {
 *          AVClass *class;
 *          int flags_opt;
 *      } child_struct;
 *      static const AVOption child_opts[] = {
 *          { "test_flags", "This is a test option of flags type.",
 *            offsetof(child_struct, flags_opt), FX_OPT_TYPE_FLAGS, { .i64 = 0 }, INT_MIN, INT_MAX },
 *          { NULL },
 *      };
 *      static const AVClass child_class = {
 *          .class_name = "child class",
 *          .item_name  = fx_default_item_name,
 *          .option     = child_opts,
 *          .version    = LIBAVUTIL_VERSION_INT,
 *      };
 *
 *      void *child_next(void *obj, void *prev)
 *      {
 *          test_struct *t = obj;
 *          if (!prev && t->child_struct)
 *              return t->child_struct;
 *          return NULL
 *      }
 *      const AVClass child_class_next(const AVClass *prev)
 *      {
 *          return prev ? NULL : &child_class;
 *      }
 *      @endcode
 *      Putting child_next() and child_class_next() as defined above into
 *      test_class will now make child_struct's options accessible through
 *      test_struct (again, proper setup as described above needs to be done on
 *      child_struct right after it is created).
 *
 *      From the above example it might not be clear why both child_next()
 *      and child_class_next() are needed. The distinction is that child_next()
 *      iterates over actually existing objects, while child_class_next()
 *      iterates over all possible child classes. E.g. if an AVCodecContext
 *      was initialized to use a codec which has private options, then its
 *      child_next() will return AVCodecContext.priv_data and finish
 *      iterating. OTOH child_class_next() on AVCodecContext.fx_class will
 *      iterate over all available codecs with private options.
 *
 * @subsection avoptions_implement_named_constants Named constants
 *      It is possible to create named constants for options. Simply set the unit
 *      field of the option the constants should apply to a string and
 *      create the constants themselves as options of type FX_OPT_TYPE_CONST
 *      with their unit field set to the same string.
 *      Their default_val field should contain the value of the named
 *      constant.
 *      For example, to add some named constants for the test_flags option
 *      above, put the following into the child_opts array:
 *      @code
 *      { "test_flags", "This is a test option of flags type.",
 *        offsetof(child_struct, flags_opt), FX_OPT_TYPE_FLAGS, { .i64 = 0 }, INT_MIN, INT_MAX, "test_unit" },
 *      { "flag1", "This is a flag with value 16", 0, FX_OPT_TYPE_CONST, { .i64 = 16 }, 0, 0, "test_unit" },
 *      @endcode
 *
 * @section avoptions_use Using AVOptions
 * This section deals with accessing options in an AVOptions-enabled struct.
 * Such structs in FFmpeg are e.g. AVCodecContext in libavcodec or
 * AVFormatContext in libavformat.
 *
 * @subsection avoptions_use_examine Examining AVOptions
 * The basic functions for examining options are fx_opt_next(), which iterates
 * over all options defined for one object, and fx_opt_find(), which searches
 * for an option with the given name.
 *
 * The situation is more complicated with nesting. An AVOptions-enabled struct
 * may have AVOptions-enabled children. Passing the FX_OPT_SEARCH_CHILDREN flag
 * to fx_opt_find() will make the function search children recursively.
 *
 * For enumerating there are basically two cases. The first is when you want to
 * get all options that may potentially exist on the struct and its children
 * (e.g.  when constructing documentation). In that case you should call
 * fx_opt_child_class_next() recursively on the parent struct's AVClass.  The
 * second case is when you have an already initialized struct with all its
 * children and you want to get all options that can be actually written or read
 * from it. In that case you should call fx_opt_child_next() recursively (and
 * fx_opt_next() on each result).
 *
 * @subsection avoptions_use_get_set Reading and writing AVOptions
 * When setting options, you often have a string read directly from the
 * user. In such a case, simply passing it to fx_opt_set() is enough. For
 * non-string type options, fx_opt_set() will parse the string according to the
 * option type.
 *
 * Similarly fx_opt_get() will read any option type and convert it to a string
 * which will be returned. Do not forget that the string is allocated, so you
 * have to free it with fx_free().
 *
 * In some cases it may be more convenient to put all options into an
 * AVDictionary and call fx_opt_set_dict() on it. A specific case of this
 * are the format/codec open functions in lavf/lavc which take a dictionary
 * filled with option as a parameter. This makes it possible to set some options
 * that cannot be set otherwise, since e.g. the input file format is not known
 * before the file is actually opened.
 */

enum AVOptionType{
    FX_OPT_TYPE_FLAGS,
    FX_OPT_TYPE_INT,
    FX_OPT_TYPE_INT64,
    FX_OPT_TYPE_DOUBLE,
    FX_OPT_TYPE_FLOAT,
    FX_OPT_TYPE_STRING,
    FX_OPT_TYPE_RATIONAL,
    FX_OPT_TYPE_BINARY,  ///< offset must point to a pointer immediately followed by an int for the length
    FX_OPT_TYPE_DICT,
    FX_OPT_TYPE_CONST = 128,
    FX_OPT_TYPE_IMAGE_SIZE = MKBETAG('S','I','Z','E'), ///< offset must point to two consecutive integers
    FX_OPT_TYPE_PIXEL_FMT  = MKBETAG('P','F','M','T'),
    FX_OPT_TYPE_SAMPLE_FMT = MKBETAG('S','F','M','T'),
    FX_OPT_TYPE_VIDEO_RATE = MKBETAG('V','R','A','T'), ///< offset must point to AVRational
    FX_OPT_TYPE_DURATION   = MKBETAG('D','U','R',' '),
    FX_OPT_TYPE_COLOR      = MKBETAG('C','O','L','R'),
    FX_OPT_TYPE_CHANNEL_LAYOUT = MKBETAG('C','H','L','A'),
    FX_OPT_TYPE_BOOL           = MKBETAG('B','O','O','L'),
};

/**
 * AVOption
 */
typedef struct AVOption {
    const char *name;

    /**
     * short English help text
     * @todo What about other languages?
     */
    const char *help;

    /**
     * The offset relative to the context structure where the option
     * value is stored. It should be 0 for named constants.
     */
    int offset;
    enum AVOptionType type;

    /**
     * the default value for scalar options
     */
    union {
        int64_t i64;
        double dbl;
        const char *str;
        /* TODO those are unused now */
        AVRational q;
    } default_val;
    double min;                 ///< minimum valid value for the option
    double max;                 ///< maximum valid value for the option

    int flags;
#define FX_OPT_FLAG_ENCODING_PARAM  1   ///< a generic parameter which can be set by the user for muxing or encoding
#define FX_OPT_FLAG_DECODING_PARAM  2   ///< a generic parameter which can be set by the user for demuxing or decoding
#if FF_API_OPT_TYPE_METADATA
#define FX_OPT_FLAG_METADATA        4   ///< some data extracted or inserted into the file like title, comment, ...
#endif
#define FX_OPT_FLAG_AUDIO_PARAM     8
#define FX_OPT_FLAG_VIDEO_PARAM     16
#define FX_OPT_FLAG_SUBTITLE_PARAM  32
/**
 * The option is intended for exporting values to the caller.
 */
#define FX_OPT_FLAG_EXPORT          64
/**
 * The option may not be set through the AVOptions API, only read.
 * This flag only makes sense when FX_OPT_FLAG_EXPORT is also set.
 */
#define FX_OPT_FLAG_READONLY        128
#define FX_OPT_FLAG_FILTERING_PARAM (1<<16) ///< a generic parameter which can be set by the user for filtering
//FIXME think about enc-audio, ... style flags

    /**
     * The logical unit to which the option belongs. Non-constant
     * options and corresponding named constants share the same
     * unit. May be NULL.
     */
    const char *unit;
} AVOption;

/**
 * A single allowed range of values, or a single allowed value.
 */
typedef struct AVOptionRange {
    const char *str;
    /**
     * Value range.
     * For string ranges this represents the min/max length.
     * For dimensions this represents the min/max pixel count or width/height in multi-component case.
     */
    double value_min, value_max;
    /**
     * Value's component range.
     * For string this represents the unicode range for chars, 0-127 limits to ASCII.
     */
    double component_min, component_max;
    /**
     * Range flag.
     * If set to 1 the struct encodes a range, if set to 0 a single value.
     */
    int is_range;
} AVOptionRange;

/**
 * List of AVOptionRange structs.
 */
typedef struct AVOptionRanges {
    /**
     * Array of option ranges.
     *
     * Most of option types use just one component.
     * Following describes multi-component option types:
     *
     * FX_OPT_TYPE_IMAGE_SIZE:
     * component index 0: range of pixel count (width * height).
     * component index 1: range of width.
     * component index 2: range of height.
     *
     * @note To obtain multi-component version of this structure, user must
     *       provide FX_OPT_MULTI_COMPONENT_RANGE to fx_opt_query_ranges or
     *       fx_opt_query_ranges_default function.
     *
     * Multi-component range can be read as in following example:
     *
     * @code
     * int range_index, component_index;
     * AVOptionRanges *ranges;
     * AVOptionRange *range[3]; //may require more than 3 in the future.
     * fx_opt_query_ranges(&ranges, obj, key, FX_OPT_MULTI_COMPONENT_RANGE);
     * for (range_index = 0; range_index < ranges->nb_ranges; range_index++) {
     *     for (component_index = 0; component_index < ranges->nb_components; component_index++)
     *         range[component_index] = ranges->range[ranges->nb_ranges * component_index + range_index];
     *     //do something with range here.
     * }
     * fx_opt_freep_ranges(&ranges);
     * @endcode
     */
    AVOptionRange **range;
    /**
     * Number of ranges per component.
     */
    int nb_ranges;
    /**
     * Number of componentes.
     */
    int nb_components;
} AVOptionRanges;

enum {

    /**
     * Accept to parse a value without a key; the key will then be returned
     * as NULL.
     */
    FX_OPT_FLAG_IMPLICIT_KEY = 1,
};

/**
 * @defgroup opt_eval_funcs Evaluating option strings
 * @{
 * This group of functions can be used to evaluate option strings
 * and get numbers out of them. They do the same thing as fx_opt_set(),
 * except the result is written into the caller-supplied pointer.
 *
 * @param obj a struct whose first element is a pointer to AVClass.
 * @param o an option for which the string is to be evaluated.
 * @param val string to be evaluated.
 * @param *_out value of the string will be written here.
 *
 * @return 0 on success, a negative number on failure.
 */
/**
 * @}
 */

#define FX_OPT_SEARCH_CHILDREN   (1 << 0) /**< Search in possible children of the
                                               given object first. */
/**
 *  The obj passed to fx_opt_find() is fake -- only a double pointer to AVClass
 *  instead of a required pointer to a struct containing AVClass. This is
 *  useful for searching for options without needing to allocate the corresponding
 *  object.
 */
#define FX_OPT_SEARCH_FAKE_OBJ   (1 << 1)

/**
 *  In fx_opt_get, return NULL if the option has a pointer type and is set to NULL,
 *  rather than returning an empty string.
 */
#define FX_OPT_ALLOW_NULL (1 << 2)

/**
 *  Allows fx_opt_query_ranges and fx_opt_query_ranges_default to return more than
 *  one component for certain option types.
 *  @see AVOptionRanges for details.
 */
#define FX_OPT_MULTI_COMPONENT_RANGE (1 << 12)

/**
 * Iterate over all AVOptions belonging to obj.
 *
 * @param obj an AVOptions-enabled struct or a double pointer to an
 *            AVClass describing it.
 * @param prev result of the previous call to fx_opt_next() on this object
 *             or NULL
 * @return next AVOption or NULL
 */
const AVOption *fx_opt_next(const void *obj, const AVOption *last)
{
    const AVClass *class;
    if (!obj)
        return NULL;
    class = *(const AVClass**)obj;
    if (!last && class && class->option && class->option[0].name)
        return class->option;
    if (last && last[1].name)
        return ++last;
    return NULL;
}

static int read_number(const AVOption *o, const void *dst, double *num, int *den, int64_t *intnum)
{
    switch (o->type) {
    case FX_OPT_TYPE_FLAGS:
        *intnum = *(unsigned int*)dst;
        return 0;
    case FX_OPT_TYPE_PIXEL_FMT:
        *intnum = *(enum AVPixelFormat *)dst;
        return 0;
    case FX_OPT_TYPE_SAMPLE_FMT:
        *intnum = *(enum AVSampleFormat *)dst;
        return 0;
    case FX_OPT_TYPE_BOOL:
    case FX_OPT_TYPE_INT:
        *intnum = *(int *)dst;
        return 0;
    case FX_OPT_TYPE_CHANNEL_LAYOUT:
    case FX_OPT_TYPE_DURATION:
    case FX_OPT_TYPE_INT64:
        *intnum = *(int64_t *)dst;
        return 0;
    case FX_OPT_TYPE_FLOAT:
        *num = *(float *)dst;
        return 0;
    case FX_OPT_TYPE_DOUBLE:
        *num = *(double *)dst;
        return 0;
    case FX_OPT_TYPE_RATIONAL:
        *intnum = ((AVRational *)dst)->num;
        *den    = ((AVRational *)dst)->den;
        return 0;
    case FX_OPT_TYPE_CONST:
        *num = o->default_val.dbl;
        return 0;
    }
    return AVERROR(EINVAL);
}

static int write_number(void *obj, const AVOption *o, void *dst, double num, int den, int64_t intnum)
{
    if (o->type != FX_OPT_TYPE_FLAGS &&
        (o->max * den < num * intnum || o->min * den > num * intnum)) {
        num = den ? num * intnum / den : (num * intnum ? INFINITY : NAN);
        fx_log(obj, FX_LOG_ERROR, "Value %f for parameter '%s' out of range [%g - %g]\n",
               num, o->name, o->min, o->max);
        return AVERROR(ERANGE);
    }
    if (o->type == FX_OPT_TYPE_FLAGS) {
        double d = num*intnum/den;
        if (d < -1.5 || d > 0xFFFFFFFF+0.5 || (llrint(d*256) & 255)) {
            fx_log(obj, FX_LOG_ERROR,
                   "Value %f for parameter '%s' is not a valid set of 32bit integer flags\n",
                   num*intnum/den, o->name);
            return AVERROR(ERANGE);
        }
    }

    switch (o->type) {
    case FX_OPT_TYPE_PIXEL_FMT:
        *(enum AVPixelFormat *)dst = llrint(num / den) * intnum;
        break;
    case FX_OPT_TYPE_SAMPLE_FMT:
        *(enum AVSampleFormat *)dst = llrint(num / den) * intnum;
        break;
    case FX_OPT_TYPE_BOOL:
    case FX_OPT_TYPE_FLAGS:
    case FX_OPT_TYPE_INT:
        *(int *)dst = llrint(num / den) * intnum;
        break;
    case FX_OPT_TYPE_DURATION:
    case FX_OPT_TYPE_CHANNEL_LAYOUT:
    case FX_OPT_TYPE_INT64:
        *(int64_t *)dst = llrint(num / den) * intnum;
        break;
    case FX_OPT_TYPE_FLOAT:
        *(float *)dst = num * intnum / den;
        break;
    case FX_OPT_TYPE_DOUBLE:
        *(double    *)dst = num * intnum / den;
        break;
    case FX_OPT_TYPE_RATIONAL:
    case FX_OPT_TYPE_VIDEO_RATE:
        if ((int) num == num)
            *(AVRational *)dst = (AVRational) { num *intnum, den };
        else
            *(AVRational *)dst = fx_d2q(num * intnum / den, 1 << 24);
        break;
    default:
        return AVERROR(EINVAL);
    }
    return 0;
}

static int hexchar2int(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

static int set_string_binary(void *obj, const AVOption *o, const char *val, uint8_t **dst)
{
    int *lendst = (int *)(dst + 1);
    uint8_t *bin, *ptr;
    int len;

    fx_freep(dst);
    *lendst = 0;

    if (!val || !(len = strlen(val)))
        return 0;

    if (len & 1)
        return AVERROR(EINVAL);
    len /= 2;

    ptr = bin = fx_malloc(len);
    if (!ptr)
        return AVERROR(ENOMEM);
    while (*val) {
        int a = hexchar2int(*val++);
        int b = hexchar2int(*val++);
        if (a < 0 || b < 0) {
            fx_free(bin);
            return AVERROR(EINVAL);
        }
        *ptr++ = (a << 4) | b;
    }
    *dst    = bin;
    *lendst = len;

    return 0;
}

static int set_string(void *obj, const AVOption *o, const char *val, uint8_t **dst)
{
    fx_freep(dst);
    *dst = fx_strdup(val);
    return *dst ? 0 : AVERROR(ENOMEM);
}

#define DEFAULT_NUMVAL(opt) ((opt->type == FX_OPT_TYPE_INT64 || \
                              opt->type == FX_OPT_TYPE_CONST || \
                              opt->type == FX_OPT_TYPE_FLAGS || \
                              opt->type == FX_OPT_TYPE_INT)     \
                             ? opt->default_val.i64             \
                             : opt->default_val.dbl)

static int set_string_number(void *obj, void *target_obj, const AVOption *o, const char *val, void *dst)
{
    int ret = 0;
    int num, den;
    char c;

    if (sscanf(val, "%d%*1[:/]%d%c", &num, &den, &c) == 2) {
        if ((ret = write_number(obj, o, dst, 1, den, num)) >= 0)
            return ret;
        ret = 0;
    }

    for (;;) {
        int i = 0;
        char buf[256];
        int cmd = 0;
        double d;
        int64_t intnum = 1;

        if (o->type == FX_OPT_TYPE_FLAGS) {
            if (*val == '+' || *val == '-')
                cmd = *(val++);
            for (; i < sizeof(buf) - 1 && val[i] && val[i] != '+' && val[i] != '-'; i++)
                buf[i] = val[i];
            buf[i] = 0;
        }

        {
            const AVOption *o_named = fx_opt_find(target_obj, i ? buf : val, o->unit, 0, 0);
            int res;
            int ci = 0;
            double const_values[64];
            const char * const_names[64];
            if (o_named && o_named->type == FX_OPT_TYPE_CONST)
                d = DEFAULT_NUMVAL(o_named);
            else {
                if (o->unit) {
                    for (o_named = NULL; o_named = fx_opt_next(target_obj, o_named); ) {
                        if (o_named->type == FX_OPT_TYPE_CONST &&
                            o_named->unit &&
                            !strcmp(o_named->unit, o->unit)) {
                            if (ci + 6 >= ARRAY_ELEMS(const_values)) {
                                fx_log(obj, FX_LOG_ERROR, "const_values array too small for %s\n", o->unit);
                                return AVERROR_PATCHWELCOME;
                            }
                            const_names [ci  ] = o_named->name;
                            const_values[ci++] = DEFAULT_NUMVAL(o_named);
                        }
                    }
                }
                const_names [ci  ] = "default";
                const_values[ci++] = DEFAULT_NUMVAL(o);
                const_names [ci  ] = "max";
                const_values[ci++] = o->max;
                const_names [ci  ] = "min";
                const_values[ci++] = o->min;
                const_names [ci  ] = "none";
                const_values[ci++] = 0;
                const_names [ci  ] = "all";
                const_values[ci++] = ~0;
                const_names [ci] = NULL;
                const_values[ci] = 0;

                res = fx_expr_parse_and_eval(&d, i ? buf : val, const_names,
                                            const_values, NULL, NULL, NULL, NULL, NULL, 0, obj);
                if (res < 0) {
                    fx_log(obj, FX_LOG_ERROR, "Unable to parse option value \"%s\"\n", val);
                    return res;
                }
            }
        }
        if (o->type == FX_OPT_TYPE_FLAGS) {
            read_number(o, dst, NULL, NULL, &intnum);
            if (cmd == '+')
                d = intnum | (int64_t)d;
            else if (cmd == '-')
                d = intnum &~(int64_t)d;
        }

        if ((ret = write_number(obj, o, dst, d, 1, 1)) < 0)
            return ret;
        val += i;
        if (!i || !*val)
            return 0;
    }

    return 0;
}

static int set_string_image_size(void *obj, const AVOption *o, const char *val, int *dst)
{
    int ret;

    if (!val || !strcmp(val, "none")) {
        dst[0] =
        dst[1] = 0;
        return 0;
    }
    ret = fx_parse_video_size(dst, dst + 1, val);
    if (ret < 0)
        fx_log(obj, FX_LOG_ERROR, "Unable to parse option value \"%s\" as image size\n", val);
    return ret;
}

static int set_string_video_rate(void *obj, const AVOption *o, const char *val, AVRational *dst)
{
    int ret;
    if (!val) {
        ret = AVERROR(EINVAL);
    } else {
        ret = fx_parse_video_rate(dst, val);
    }
    if (ret < 0)
        fx_log(obj, FX_LOG_ERROR, "Unable to parse option value \"%s\" as video rate\n", val);
    return ret;
}

static int set_string_color(void *obj, const AVOption *o, const char *val, uint8_t *dst)
{
    int ret;

    if (!val) {
        return 0;
    } else {
        ret = fx_parse_color(dst, val, -1, obj);
        if (ret < 0)
            fx_log(obj, FX_LOG_ERROR, "Unable to parse option value \"%s\" as color\n", val);
        return ret;
    }
    return 0;
}

static const char *get_bool_name(int val)
{
    if (val < 0)
        return "auto";
    return val ? "true" : "false";
}

static int set_string_bool(void *obj, const AVOption *o, const char *val, int *dst)
{
    int n;

    if (!val)
        return 0;

    if (!strcmp(val, "auto")) {
        n = -1;
    } else if (fx_match_name(val, "true,y,yes,enable,enabled,on")) {
        n = 1;
    } else if (fx_match_name(val, "false,n,no,disable,disabled,off")) {
        n = 0;
    } else {
        char *end = NULL;
        n = strtol(val, &end, 10);
        if (val + strlen(val) != end)
            goto fail;
    }

    if (n < o->min || n > o->max)
        goto fail;

    *dst = n;
    return 0;

fail:
    fx_log(obj, FX_LOG_ERROR, "Unable to parse option value \"%s\" as boolean\n", val);
    return AVERROR(EINVAL);
}

static int set_string_fmt(void *obj, const AVOption *o, const char *val, uint8_t *dst,
                          int fmt_nb, int ((*get_fmt)(const char *)), const char *desc)
{
    int fmt, min, max;

    if (!val || !strcmp(val, "none")) {
        fmt = -1;
    } else {
        fmt = get_fmt(val);
        if (fmt == -1) {
            char *tail;
            fmt = strtol(val, &tail, 0);
            if (*tail || (unsigned)fmt >= fmt_nb) {
                fx_log(obj, FX_LOG_ERROR,
                       "Unable to parse option value \"%s\" as %s\n", val, desc);
                return AVERROR(EINVAL);
            }
        }
    }

    min = FFMAX(o->min, -1);
    max = FFMIN(o->max, fmt_nb-1);

    // hack for compatibility with old ffmpeg
    if(min == 0 && max == 0) {
        min = -1;
        max = fmt_nb-1;
    }

    if (fmt < min || fmt > max) {
        fx_log(obj, FX_LOG_ERROR,
               "Value %d for parameter '%s' out of %s format range [%d - %d]\n",
               fmt, o->name, desc, min, max);
        return AVERROR(ERANGE);
    }

    *(int *)dst = fmt;
    return 0;
}

static int set_string_pixel_fmt(void *obj, const AVOption *o, const char *val, uint8_t *dst)
{
    return set_string_fmt(obj, o, val, dst,
                          FX_PIX_FMT_NB, fx_get_pix_fmt, "pixel format");
}

static int set_string_sample_fmt(void *obj, const AVOption *o, const char *val, uint8_t *dst)
{
    return set_string_fmt(obj, o, val, dst,
                          FX_SAMPLE_FMT_NB, fx_get_sample_fmt, "sample format");
}
/**
 * @defgroup opt_set_funcs Option setting functions
 * @{
 * Those functions set the field of obj with the given name to value.
 *
 * @param[in] obj A struct whose first element is a pointer to an AVClass.
 * @param[in] name the name of the field to set
 * @param[in] val The value to set. In case of fx_opt_set() if the field is not
 * of a string type, then the given string is parsed.
 * SI postfixes and some named scalars are supported.
 * If the field is of a numeric type, it has to be a numeric or named
 * scalar. Behavior with more than one scalar and +- infix operators
 * is undefined.
 * If the field is of a flags type, it has to be a sequence of numeric
 * scalars or named flags separated by '+' or '-'. Prefixing a flag
 * with '+' causes it to be set without affecting the other flags;
 * similarly, '-' unsets a flag.
 * @param search_flags flags passed to fx_opt_find2. I.e. if FX_OPT_SEARCH_CHILDREN
 * is passed here, then the option may be set on a child of obj.
 *
 * @return 0 if the value has been set, or an AVERROR code in case of
 * error:
 * AVERROR_OPTION_NOT_FOUND if no matching option exists
 * AVERROR(ERANGE) if the value is out of range
 * AVERROR(EINVAL) if the value is not valid
 */

int fx_opt_set(void *obj, const char *name, const char *val, int search_flags)
{
    int ret = 0;
    void *dst, *target_obj;
    const AVOption *o = fx_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;
    if (!val && (o->type != FX_OPT_TYPE_STRING &&
                 o->type != FX_OPT_TYPE_PIXEL_FMT && o->type != FX_OPT_TYPE_SAMPLE_FMT &&
                 o->type != FX_OPT_TYPE_IMAGE_SIZE && o->type != FX_OPT_TYPE_VIDEO_RATE &&
                 o->type != FX_OPT_TYPE_DURATION && o->type != FX_OPT_TYPE_COLOR &&
                 o->type != FX_OPT_TYPE_CHANNEL_LAYOUT && o->type != FX_OPT_TYPE_BOOL))
        return AVERROR(EINVAL);

    if (o->flags & FX_OPT_FLAG_READONLY)
        return AVERROR(EINVAL);

    dst = ((uint8_t *)target_obj) + o->offset;
    switch (o->type) {
    case FX_OPT_TYPE_BOOL:
        return set_string_bool(obj, o, val, dst);
    case FX_OPT_TYPE_STRING:
        return set_string(obj, o, val, dst);
    case FX_OPT_TYPE_BINARY:
        return set_string_binary(obj, o, val, dst);
    case FX_OPT_TYPE_FLAGS:
    case FX_OPT_TYPE_INT:
    case FX_OPT_TYPE_INT64:
    case FX_OPT_TYPE_FLOAT:
    case FX_OPT_TYPE_DOUBLE:
    case FX_OPT_TYPE_RATIONAL:
        return set_string_number(obj, target_obj, o, val, dst);
    case FX_OPT_TYPE_IMAGE_SIZE:
        return set_string_image_size(obj, o, val, dst);
    case FX_OPT_TYPE_VIDEO_RATE: {
        AVRational tmp;
        ret = set_string_video_rate(obj, o, val, &tmp);
        if (ret < 0)
            return ret;
        return write_number(obj, o, dst, 1, tmp.den, tmp.num);
    }
    case FX_OPT_TYPE_PIXEL_FMT:
        return set_string_pixel_fmt(obj, o, val, dst);
    case FX_OPT_TYPE_SAMPLE_FMT:
        return set_string_sample_fmt(obj, o, val, dst);
    case FX_OPT_TYPE_DURATION:
        if (!val) {
            *(int64_t *)dst = 0;
            return 0;
        } else {
            if ((ret = fx_parse_time(dst, val, 1)) < 0)
                fx_log(obj, FX_LOG_ERROR, "Unable to parse option value \"%s\" as duration\n", val);
            return ret;
        }
        break;
    case FX_OPT_TYPE_COLOR:
        return set_string_color(obj, o, val, dst);
    case FX_OPT_TYPE_CHANNEL_LAYOUT:
        if (!val || !strcmp(val, "none")) {
            *(int64_t *)dst = 0;
        } else {
            int64_t cl = fx_get_channel_layout(val);
            if (!cl) {
                fx_log(obj, FX_LOG_ERROR, "Unable to parse option value \"%s\" as channel layout\n", val);
                ret = AVERROR(EINVAL);
            }
            *(int64_t *)dst = cl;
            return ret;
        }
        break;
    }

    fx_log(obj, FX_LOG_ERROR, "Invalid option type.\n");
    return AVERROR(EINVAL);
}

#define OPT_EVAL_NUMBER(name, opttype, vartype)                         \
int fx_opt_eval_ ## name(void *obj, const AVOption *o,                  \
                         const char *val, vartype *name ## _out)        \
{                                                                       \
    if (!o || o->type != opttype || o->flags & FX_OPT_FLAG_READONLY)    \
        return AVERROR(EINVAL);                                         \
    return set_string_number(obj, obj, o, val, name ## _out);           \
}

OPT_EVAL_NUMBER(flags,  FX_OPT_TYPE_FLAGS,    int)
OPT_EVAL_NUMBER(int,    FX_OPT_TYPE_INT,      int)
OPT_EVAL_NUMBER(int64,  FX_OPT_TYPE_INT64,    int64_t)
OPT_EVAL_NUMBER(float,  FX_OPT_TYPE_FLOAT,    float)
OPT_EVAL_NUMBER(double, FX_OPT_TYPE_DOUBLE,   double)
OPT_EVAL_NUMBER(q,      FX_OPT_TYPE_RATIONAL, AVRational)

static int set_number(void *obj, const char *name, double num, int den, int64_t intnum,
                      int search_flags)
{
    void *dst, *target_obj;
    const AVOption *o = fx_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);

    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;

    if (o->flags & FX_OPT_FLAG_READONLY)
        return AVERROR(EINVAL);

    dst = ((uint8_t *)target_obj) + o->offset;
    return write_number(obj, o, dst, num, den, intnum);
}

int fx_opt_set_int(void *obj, const char *name, int64_t val, int search_flags)
{
    return set_number(obj, name, 1, 1, val, search_flags);
}

int fx_opt_set_double(void *obj, const char *name, double val, int search_flags)
{
    return set_number(obj, name, val, 1, 1, search_flags);
}

int fx_opt_set_q(void *obj, const char *name, AVRational val, int search_flags)
{
    return set_number(obj, name, val.num, val.den, 1, search_flags);
}

int fx_opt_set_bin(void *obj, const char *name, const uint8_t *val, int len, int search_flags)
{
    void *target_obj;
    const AVOption *o = fx_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    uint8_t *ptr;
    uint8_t **dst;
    int *lendst;

    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;

    if (o->type != FX_OPT_TYPE_BINARY || o->flags & FX_OPT_FLAG_READONLY)
        return AVERROR(EINVAL);

    ptr = len ? fx_malloc(len) : NULL;
    if (len && !ptr)
        return AVERROR(ENOMEM);

    dst    = (uint8_t **)(((uint8_t *)target_obj) + o->offset);
    lendst = (int *)(dst + 1);

    fx_free(*dst);
    *dst    = ptr;
    *lendst = len;
    if (len)
        memcpy(ptr, val, len);

    return 0;
}

int fx_opt_set_image_size(void *obj, const char *name, int w, int h, int search_flags)
{
    void *target_obj;
    const AVOption *o = fx_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);

    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;
    if (o->type != FX_OPT_TYPE_IMAGE_SIZE) {
        fx_log(obj, FX_LOG_ERROR,
               "The value set by option '%s' is not an image size.\n", o->name);
        return AVERROR(EINVAL);
    }
    if (w<0 || h<0) {
        fx_log(obj, FX_LOG_ERROR,
               "Invalid negative size value %dx%d for size '%s'\n", w, h, o->name);
        return AVERROR(EINVAL);
    }
    *(int *)(((uint8_t *)target_obj)             + o->offset) = w;
    *(int *)(((uint8_t *)target_obj+sizeof(int)) + o->offset) = h;
    return 0;
}

int fx_opt_set_video_rate(void *obj, const char *name, AVRational val, int search_flags)
{
    void *target_obj;
    const AVOption *o = fx_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);

    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;
    if (o->type != FX_OPT_TYPE_VIDEO_RATE) {
        fx_log(obj, FX_LOG_ERROR,
               "The value set by option '%s' is not a video rate.\n", o->name);
        return AVERROR(EINVAL);
    }
    if (val.num <= 0 || val.den <= 0)
        return AVERROR(EINVAL);
    return set_number(obj, name, val.num, val.den, 1, search_flags);
}

static int set_format(void *obj, const char *name, int fmt, int search_flags,
                      enum AVOptionType type, const char *desc, int nb_fmts)
{
    void *target_obj;
    const AVOption *o = fx_opt_find2(obj, name, NULL, 0,
                                     search_flags, &target_obj);
    int min, max;

    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;
    if (o->type != type) {
        fx_log(obj, FX_LOG_ERROR,
               "The value set by option '%s' is not a %s format", name, desc);
        return AVERROR(EINVAL);
    }

    min = FFMAX(o->min, -1);
    max = FFMIN(o->max, nb_fmts-1);

    if (fmt < min || fmt > max) {
        fx_log(obj, FX_LOG_ERROR,
               "Value %d for parameter '%s' out of %s format range [%d - %d]\n",
               fmt, name, desc, min, max);
        return AVERROR(ERANGE);
    }
    *(int *)(((uint8_t *)target_obj) + o->offset) = fmt;
    return 0;
}

int fx_opt_set_pixel_fmt(void *obj, const char *name, enum AVPixelFormat fmt, int search_flags)
{
    return set_format(obj, name, fmt, search_flags, FX_OPT_TYPE_PIXEL_FMT, "pixel", FX_PIX_FMT_NB);
}

int fx_opt_set_sample_fmt(void *obj, const char *name, enum AVSampleFormat fmt, int search_flags)
{
    return set_format(obj, name, fmt, search_flags, FX_OPT_TYPE_SAMPLE_FMT, "sample", FX_SAMPLE_FMT_NB);
}

int fx_opt_set_channel_layout(void *obj, const char *name, int64_t cl, int search_flags)
{
    void *target_obj;
    const AVOption *o = fx_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);

    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;
    if (o->type != FX_OPT_TYPE_CHANNEL_LAYOUT) {
        fx_log(obj, FX_LOG_ERROR,
               "The value set by option '%s' is not a channel layout.\n", o->name);
        return AVERROR(EINVAL);
    }
    *(int64_t *)(((uint8_t *)target_obj) + o->offset) = cl;
    return 0;
}
/**
 * @note Any old dictionary present is discarded and replaced with a copy of the new one. The
 * caller still owns val is and responsible for freeing it.
 */

int fx_opt_set_dict_val(void *obj, const char *name, const AVDictionary *val,
                        int search_flags)
{
    void *target_obj;
    AVDictionary **dst;
    const AVOption *o = fx_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);

    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;
    if (o->flags & FX_OPT_FLAG_READONLY)
        return AVERROR(EINVAL);

    dst = (AVDictionary **)(((uint8_t *)target_obj) + o->offset);
    fx_dict_free(dst);
    fx_dict_copy(dst, val, 0);

    return 0;
}

/**
 * Set a binary option to an integer list.
 *
 * @param obj    AVClass object to set options on
 * @param name   name of the binary option
 * @param val    pointer to an integer list (must have the correct type with
 *               regard to the contents of the list)
 * @param term   list terminator (usually 0 or -1)
 * @param flags  search flags
 */
#define fx_opt_set_int_list(obj, name, val, term, flags) \
    (fx_int_list_length(val, term) > INT_MAX / sizeof(*(val)) ? \
     AVERROR(EINVAL) : \
     fx_opt_set_bin(obj, name, (const uint8_t *)(val), \
                    fx_int_list_length(val, term) * sizeof(*(val)), flags))

static void format_duration(char *buf, size_t size, int64_t d)
{
    char *e;

    fx_assert0(size >= 25);
    if (d < 0 && d != INT64_MIN) {
        *(buf++) = '-';
        size--;
        d = -d;
    }
    if (d == INT64_MAX)
        snprintf(buf, size, "INT64_MAX");
    else if (d == INT64_MIN)
        snprintf(buf, size, "INT64_MIN");
    else if (d > (int64_t)3600*1000000)
        snprintf(buf, size, "%"PRId64":%02d:%02d.%06d", d / 3600000000,
                 (int)((d / 60000000) % 60),
                 (int)((d / 1000000) % 60),
                 (int)(d % 1000000));
    else if (d > 60*1000000)
        snprintf(buf, size, "%d:%02d.%06d",
                 (int)(d / 60000000),
                 (int)((d / 1000000) % 60),
                 (int)(d % 1000000));
    else
        snprintf(buf, size, "%d.%06d",
                 (int)(d / 1000000),
                 (int)(d % 1000000));
    e = buf + strlen(buf);
    while (e > buf && e[-1] == '0')
        *(--e) = 0;
    if (e > buf && e[-1] == '.')
        *(--e) = 0;
}

/**
 * @defgroup opt_get_funcs Option getting functions
 * @{
 * Those functions get a value of the option with the given name from an object.
 *
 * @param[in] obj a struct whose first element is a pointer to an AVClass.
 * @param[in] name name of the option to get.
 * @param[in] search_flags flags passed to fx_opt_find2. I.e. if FX_OPT_SEARCH_CHILDREN
 * is passed here, then the option may be found in a child of obj.
 * @param[out] out_val value of the option will be written here
 * @return >=0 on success, a negative error code otherwise
 */
/**
 * @note the returned string will be fx_malloc()ed and must be fx_free()ed by the caller
 *
 * @note if FX_OPT_ALLOW_NULL is set in search_flags in fx_opt_get, and the option has
 * FX_OPT_TYPE_STRING or FX_OPT_TYPE_BINARY and is set to NULL, *out_val will be set
 * to NULL instead of an allocated empty string.
 */

int fx_opt_get(void *obj, const char *name, int search_flags, uint8_t **out_val)
{
    void *dst, *target_obj;
    const AVOption *o = fx_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    uint8_t *bin, buf[128];
    int len, i, ret;
    int64_t i64;

    if (!o || !target_obj || (o->offset<=0 && o->type != FX_OPT_TYPE_CONST))
        return AVERROR_OPTION_NOT_FOUND;

    dst = (uint8_t *)target_obj + o->offset;

    buf[0] = 0;
    switch (o->type) {
    case FX_OPT_TYPE_BOOL:
        ret = snprintf(buf, sizeof(buf), "%s", (char *)fx_x_if_null(get_bool_name(*(int *)dst), "invalid"));
        break;
    case FX_OPT_TYPE_FLAGS:
        ret = snprintf(buf, sizeof(buf), "0x%08X", *(int *)dst);
        break;
    case FX_OPT_TYPE_INT:
        ret = snprintf(buf, sizeof(buf), "%d", *(int *)dst);
        break;
    case FX_OPT_TYPE_INT64:
        ret = snprintf(buf, sizeof(buf), "%"PRId64, *(int64_t *)dst);
        break;
    case FX_OPT_TYPE_FLOAT:
        ret = snprintf(buf, sizeof(buf), "%f", *(float *)dst);
        break;
    case FX_OPT_TYPE_DOUBLE:
        ret = snprintf(buf, sizeof(buf), "%f", *(double *)dst);
        break;
    case FX_OPT_TYPE_VIDEO_RATE:
    case FX_OPT_TYPE_RATIONAL:
        ret = snprintf(buf, sizeof(buf), "%d/%d", ((AVRational *)dst)->num, ((AVRational *)dst)->den);
        break;
    case FX_OPT_TYPE_CONST:
        ret = snprintf(buf, sizeof(buf), "%f", o->default_val.dbl);
        break;
    case FX_OPT_TYPE_STRING:
        if (*(uint8_t **)dst) {
            *out_val = fx_strdup(*(uint8_t **)dst);
        } else if (search_flags & FX_OPT_ALLOW_NULL) {
            *out_val = NULL;
            return 0;
        } else {
            *out_val = fx_strdup("");
        }
        return *out_val ? 0 : AVERROR(ENOMEM);
    case FX_OPT_TYPE_BINARY:
        if (!*(uint8_t **)dst && (search_flags & FX_OPT_ALLOW_NULL)) {
            *out_val = NULL;
            return 0;
        }
        len = *(int *)(((uint8_t *)dst) + sizeof(uint8_t *));
        if ((uint64_t)len * 2 + 1 > INT_MAX)
            return AVERROR(EINVAL);
        if (!(*out_val = fx_malloc(len * 2 + 1)))
            return AVERROR(ENOMEM);
        if (!len) {
            *out_val[0] = '\0';
            return 0;
        }
        bin = *(uint8_t **)dst;
        for (i = 0; i < len; i++)
            snprintf(*out_val + i * 2, 3, "%02X", bin[i]);
        return 0;
    case FX_OPT_TYPE_IMAGE_SIZE:
        ret = snprintf(buf, sizeof(buf), "%dx%d", ((int *)dst)[0], ((int *)dst)[1]);
        break;
    case FX_OPT_TYPE_PIXEL_FMT:
        ret = snprintf(buf, sizeof(buf), "%s", (char *)fx_x_if_null(fx_get_pix_fmt_name(*(enum AVPixelFormat *)dst), "none"));
        break;
    case FX_OPT_TYPE_SAMPLE_FMT:
        ret = snprintf(buf, sizeof(buf), "%s", (char *)fx_x_if_null(fx_get_sample_fmt_name(*(enum AVSampleFormat *)dst), "none"));
        break;
    case FX_OPT_TYPE_DURATION:
        i64 = *(int64_t *)dst;
        format_duration(buf, sizeof(buf), i64);
        ret = strlen(buf); // no overflow possible, checked by an assert
        break;
    case FX_OPT_TYPE_COLOR:
        ret = snprintf(buf, sizeof(buf), "0x%02x%02x%02x%02x",
                       (int)((uint8_t *)dst)[0], (int)((uint8_t *)dst)[1],
                       (int)((uint8_t *)dst)[2], (int)((uint8_t *)dst)[3]);
        break;
    case FX_OPT_TYPE_CHANNEL_LAYOUT:
        i64 = *(int64_t *)dst;
        ret = snprintf(buf, sizeof(buf), "0x%"PRIx64, i64);
        break;
    default:
        return AVERROR(EINVAL);
    }

    if (ret >= sizeof(buf))
        return AVERROR(EINVAL);
    *out_val = fx_strdup(buf);
    return *out_val ? 0 : AVERROR(ENOMEM);
}

static int get_number(void *obj, const char *name, const AVOption **o_out, double *num, int *den, int64_t *intnum,
                      int search_flags)
{
    void *dst, *target_obj;
    const AVOption *o = fx_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    if (!o || !target_obj)
        goto error;

    dst = ((uint8_t *)target_obj) + o->offset;

    if (o_out) *o_out= o;

    return read_number(o, dst, num, den, intnum);

error:
    *den    =
    *intnum = 0;
    return -1;
}

int fx_opt_get_int(void *obj, const char *name, int search_flags, int64_t *out_val)
{
    int64_t intnum = 1;
    double num = 1;
    int ret, den = 1;

    if ((ret = get_number(obj, name, NULL, &num, &den, &intnum, search_flags)) < 0)
        return ret;
    *out_val = num * intnum / den;
    return 0;
}

int fx_opt_get_double(void *obj, const char *name, int search_flags, double *out_val)
{
    int64_t intnum = 1;
    double num = 1;
    int ret, den = 1;

    if ((ret = get_number(obj, name, NULL, &num, &den, &intnum, search_flags)) < 0)
        return ret;
    *out_val = num * intnum / den;
    return 0;
}

int fx_opt_get_q(void *obj, const char *name, int search_flags, AVRational *out_val)
{
    int64_t intnum = 1;
    double num = 1;
    int ret, den = 1;

    if ((ret = get_number(obj, name, NULL, &num, &den, &intnum, search_flags)) < 0)
        return ret;

    if (num == 1.0 && (int)intnum == intnum)
        *out_val = (AVRational){intnum, den};
    else
        *out_val = fx_d2q(num*intnum/den, 1<<24);
    return 0;
}

int fx_opt_get_image_size(void *obj, const char *name, int search_flags, int *w_out, int *h_out)
{
    void *dst, *target_obj;
    const AVOption *o = fx_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;
    if (o->type != FX_OPT_TYPE_IMAGE_SIZE) {
        fx_log(obj, FX_LOG_ERROR,
               "The value for option '%s' is not an image size.\n", name);
        return AVERROR(EINVAL);
    }

    dst = ((uint8_t*)target_obj) + o->offset;
    if (w_out) *w_out = *(int *)dst;
    if (h_out) *h_out = *((int *)dst+1);
    return 0;
}

int fx_opt_get_video_rate(void *obj, const char *name, int search_flags, AVRational *out_val)
{
    int64_t intnum = 1;
    double     num = 1;
    int   ret, den = 1;

    if ((ret = get_number(obj, name, NULL, &num, &den, &intnum, search_flags)) < 0)
        return ret;

    if (num == 1.0 && (int)intnum == intnum)
        *out_val = (AVRational) { intnum, den };
    else
        *out_val = fx_d2q(num * intnum / den, 1 << 24);
    return 0;
}

static int get_format(void *obj, const char *name, int search_flags, int *out_fmt,
                      enum AVOptionType type, const char *desc)
{
    void *dst, *target_obj;
    const AVOption *o = fx_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;
    if (o->type != type) {
        fx_log(obj, FX_LOG_ERROR,
               "The value for option '%s' is not a %s format.\n", desc, name);
        return AVERROR(EINVAL);
    }

    dst = ((uint8_t*)target_obj) + o->offset;
    *out_fmt = *(int *)dst;
    return 0;
}

int fx_opt_get_pixel_fmt(void *obj, const char *name, int search_flags, enum AVPixelFormat *out_fmt)
{
    return get_format(obj, name, search_flags, out_fmt, FX_OPT_TYPE_PIXEL_FMT, "pixel");
}

int fx_opt_get_sample_fmt(void *obj, const char *name, int search_flags, enum AVSampleFormat *out_fmt)
{
    return get_format(obj, name, search_flags, out_fmt, FX_OPT_TYPE_SAMPLE_FMT, "sample");
}

int fx_opt_get_channel_layout(void *obj, const char *name, int search_flags, int64_t *cl)
{
    void *dst, *target_obj;
    const AVOption *o = fx_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;
    if (o->type != FX_OPT_TYPE_CHANNEL_LAYOUT) {
        fx_log(obj, FX_LOG_ERROR,
               "The value for option '%s' is not a channel layout.\n", name);
        return AVERROR(EINVAL);
    }

    dst = ((uint8_t*)target_obj) + o->offset;
    *cl = *(int64_t *)dst;
    return 0;
}
/**
 * @param[out] out_val The returned dictionary is a copy of the actual value and must
 * be freed with fx_dict_free() by the caller
 */
int fx_opt_get_dict_val(void *obj, const char *name, int search_flags, AVDictionary **out_val)
{
    void *target_obj;
    AVDictionary *src;
    const AVOption *o = fx_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);

    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;
    if (o->type != FX_OPT_TYPE_DICT)
        return AVERROR(EINVAL);

    src = *(AVDictionary **)(((uint8_t *)target_obj) + o->offset);
    fx_dict_copy(out_val, src, 0);

    return 0;
}
/**
 * Check whether a particular flag is set in a flags field.
 *
 * @param field_name the name of the flag field option
 * @param flag_name the name of the flag to check
 * @return non-zero if the flag is set, zero if the flag isn't set,
 *         isn't of the right type, or the flags field doesn't exist.
 */
int fx_opt_flag_is_set(void *obj, const char *field_name, const char *flag_name)
{
    const AVOption *field = fx_opt_find(obj, field_name, NULL, 0, 0);
    const AVOption *flag  = fx_opt_find(obj, flag_name,
                                        field ? field->unit : NULL, 0, 0);
    int64_t res;

    if (!field || !flag || flag->type != FX_OPT_TYPE_CONST ||
        fx_opt_get_int(obj, field_name, 0, &res) < 0)
        return 0;
    return res & flag->default_val.i64;
}

static void log_value(void *fx_log_obj, int level, double d)
{
    if      (d == INT_MAX) {
        fx_log(fx_log_obj, level, "INT_MAX");
    } else if (d == INT_MIN) {
        fx_log(fx_log_obj, level, "INT_MIN");
    } else if (d == UINT32_MAX) {
        fx_log(fx_log_obj, level, "UINT32_MAX");
    } else if (d == (double)INT64_MAX) {
        fx_log(fx_log_obj, level, "I64_MAX");
    } else if (d == INT64_MIN) {
        fx_log(fx_log_obj, level, "I64_MIN");
    } else if (d == FLT_MAX) {
        fx_log(fx_log_obj, level, "FLT_MAX");
    } else if (d == FLT_MIN) {
        fx_log(fx_log_obj, level, "FLT_MIN");
    } else if (d == -FLT_MAX) {
        fx_log(fx_log_obj, level, "-FLT_MAX");
    } else if (d == -FLT_MIN) {
        fx_log(fx_log_obj, level, "-FLT_MIN");
    } else if (d == DBL_MAX) {
        fx_log(fx_log_obj, level, "DBL_MAX");
    } else if (d == DBL_MIN) {
        fx_log(fx_log_obj, level, "DBL_MIN");
    } else if (d == -DBL_MAX) {
        fx_log(fx_log_obj, level, "-DBL_MAX");
    } else if (d == -DBL_MIN) {
        fx_log(fx_log_obj, level, "-DBL_MIN");
    } else {
        fx_log(fx_log_obj, level, "%g", d);
    }
}

static const char *get_opt_const_name(void *obj, const char *unit, int64_t value)
{
    const AVOption *opt = NULL;

    if (!unit)
        return NULL;
    while ((opt = fx_opt_next(obj, opt)))
        if (opt->type == FX_OPT_TYPE_CONST && !strcmp(opt->unit, unit) &&
            opt->default_val.i64 == value)
            return opt->name;
    return NULL;
}

static char *get_opt_flags_string(void *obj, const char *unit, int64_t value)
{
    const AVOption *opt = NULL;
    char flags[512];

    flags[0] = 0;
    if (!unit)
        return NULL;
    while ((opt = fx_opt_next(obj, opt))) {
        if (opt->type == FX_OPT_TYPE_CONST && !strcmp(opt->unit, unit) &&
            opt->default_val.i64 & value) {
            if (flags[0])
                fx_strlcatf(flags, sizeof(flags), "+");
            fx_strlcatf(flags, sizeof(flags), "%s", opt->name);
        }
    }
    if (flags[0])
        return fx_strdup(flags);
    return NULL;
}

static void opt_list(void *obj, void *fx_log_obj, const char *unit,
                     int req_flags, int rej_flags)
{
    const AVOption *opt = NULL;
    AVOptionRanges *r;
    int i;

    while ((opt = fx_opt_next(obj, opt))) {
        if (!(opt->flags & req_flags) || (opt->flags & rej_flags))
            continue;

        /* Don't print CONST's on level one.
         * Don't print anything but CONST's on level two.
         * Only print items from the requested unit.
         */
        if (!unit && opt->type == FX_OPT_TYPE_CONST)
            continue;
        else if (unit && opt->type != FX_OPT_TYPE_CONST)
            continue;
        else if (unit && opt->type == FX_OPT_TYPE_CONST && strcmp(unit, opt->unit))
            continue;
        else if (unit && opt->type == FX_OPT_TYPE_CONST)
            fx_log(fx_log_obj, FX_LOG_INFO, "     %-15s ", opt->name);
        else
            fx_log(fx_log_obj, FX_LOG_INFO, "  %s%-17s ",
                   (opt->flags & FX_OPT_FLAG_FILTERING_PARAM) ? "" : "-",
                   opt->name);

        switch (opt->type) {
            case FX_OPT_TYPE_FLAGS:
                fx_log(fx_log_obj, FX_LOG_INFO, "%-12s ", "<flags>");
                break;
            case FX_OPT_TYPE_INT:
                fx_log(fx_log_obj, FX_LOG_INFO, "%-12s ", "<int>");
                break;
            case FX_OPT_TYPE_INT64:
                fx_log(fx_log_obj, FX_LOG_INFO, "%-12s ", "<int64>");
                break;
            case FX_OPT_TYPE_DOUBLE:
                fx_log(fx_log_obj, FX_LOG_INFO, "%-12s ", "<double>");
                break;
            case FX_OPT_TYPE_FLOAT:
                fx_log(fx_log_obj, FX_LOG_INFO, "%-12s ", "<float>");
                break;
            case FX_OPT_TYPE_STRING:
                fx_log(fx_log_obj, FX_LOG_INFO, "%-12s ", "<string>");
                break;
            case FX_OPT_TYPE_RATIONAL:
                fx_log(fx_log_obj, FX_LOG_INFO, "%-12s ", "<rational>");
                break;
            case FX_OPT_TYPE_BINARY:
                fx_log(fx_log_obj, FX_LOG_INFO, "%-12s ", "<binary>");
                break;
            case FX_OPT_TYPE_IMAGE_SIZE:
                fx_log(fx_log_obj, FX_LOG_INFO, "%-12s ", "<image_size>");
                break;
            case FX_OPT_TYPE_VIDEO_RATE:
                fx_log(fx_log_obj, FX_LOG_INFO, "%-12s ", "<video_rate>");
                break;
            case FX_OPT_TYPE_PIXEL_FMT:
                fx_log(fx_log_obj, FX_LOG_INFO, "%-12s ", "<pix_fmt>");
                break;
            case FX_OPT_TYPE_SAMPLE_FMT:
                fx_log(fx_log_obj, FX_LOG_INFO, "%-12s ", "<sample_fmt>");
                break;
            case FX_OPT_TYPE_DURATION:
                fx_log(fx_log_obj, FX_LOG_INFO, "%-12s ", "<duration>");
                break;
            case FX_OPT_TYPE_COLOR:
                fx_log(fx_log_obj, FX_LOG_INFO, "%-12s ", "<color>");
                break;
            case FX_OPT_TYPE_CHANNEL_LAYOUT:
                fx_log(fx_log_obj, FX_LOG_INFO, "%-12s ", "<channel_layout>");
                break;
            case FX_OPT_TYPE_BOOL:
                fx_log(fx_log_obj, FX_LOG_INFO, "%-12s ", "<boolean>");
                break;
            case FX_OPT_TYPE_CONST:
            default:
                fx_log(fx_log_obj, FX_LOG_INFO, "%-12s ", "");
                break;
        }
        fx_log(fx_log_obj, FX_LOG_INFO, "%c", (opt->flags & FX_OPT_FLAG_ENCODING_PARAM) ? 'E' : '.');
        fx_log(fx_log_obj, FX_LOG_INFO, "%c", (opt->flags & FX_OPT_FLAG_DECODING_PARAM) ? 'D' : '.');
        fx_log(fx_log_obj, FX_LOG_INFO, "%c", (opt->flags & FX_OPT_FLAG_FILTERING_PARAM)? 'F' : '.');
        fx_log(fx_log_obj, FX_LOG_INFO, "%c", (opt->flags & FX_OPT_FLAG_VIDEO_PARAM   ) ? 'V' : '.');
        fx_log(fx_log_obj, FX_LOG_INFO, "%c", (opt->flags & FX_OPT_FLAG_AUDIO_PARAM   ) ? 'A' : '.');
        fx_log(fx_log_obj, FX_LOG_INFO, "%c", (opt->flags & FX_OPT_FLAG_SUBTITLE_PARAM) ? 'S' : '.');
        fx_log(fx_log_obj, FX_LOG_INFO, "%c", (opt->flags & FX_OPT_FLAG_EXPORT)         ? 'X' : '.');
        fx_log(fx_log_obj, FX_LOG_INFO, "%c", (opt->flags & FX_OPT_FLAG_READONLY)       ? 'R' : '.');

        if (opt->help)
            fx_log(fx_log_obj, FX_LOG_INFO, " %s", opt->help);

        if (fx_opt_query_ranges(&r, obj, opt->name, FX_OPT_SEARCH_FAKE_OBJ) >= 0) {
            switch (opt->type) {
            case FX_OPT_TYPE_INT:
            case FX_OPT_TYPE_INT64:
            case FX_OPT_TYPE_DOUBLE:
            case FX_OPT_TYPE_FLOAT:
            case FX_OPT_TYPE_RATIONAL:
                for (i = 0; i < r->nb_ranges; i++) {
                    fx_log(fx_log_obj, FX_LOG_INFO, " (from ");
                    log_value(fx_log_obj, FX_LOG_INFO, r->range[i]->value_min);
                    fx_log(fx_log_obj, FX_LOG_INFO, " to ");
                    log_value(fx_log_obj, FX_LOG_INFO, r->range[i]->value_max);
                    fx_log(fx_log_obj, FX_LOG_INFO, ")");
                }
                break;
            }
            fx_opt_freep_ranges(&r);
        }

        if (opt->type != FX_OPT_TYPE_CONST  &&
            opt->type != FX_OPT_TYPE_BINARY &&
                !((opt->type == FX_OPT_TYPE_COLOR      ||
                   opt->type == FX_OPT_TYPE_IMAGE_SIZE ||
                   opt->type == FX_OPT_TYPE_STRING     ||
                   opt->type == FX_OPT_TYPE_VIDEO_RATE) &&
                  !opt->default_val.str)) {
            fx_log(fx_log_obj, FX_LOG_INFO, " (default ");
            switch (opt->type) {
            case FX_OPT_TYPE_BOOL:
                fx_log(fx_log_obj, FX_LOG_INFO, "%s", (char *)fx_x_if_null(get_bool_name(opt->default_val.i64), "invalid"));
                break;
            case FX_OPT_TYPE_FLAGS: {
                char *def_flags = get_opt_flags_string(obj, opt->unit, opt->default_val.i64);
                if (def_flags) {
                    fx_log(fx_log_obj, FX_LOG_INFO, "%s", def_flags);
                    fx_freep(&def_flags);
                } else {
                    fx_log(fx_log_obj, FX_LOG_INFO, "%"PRIX64, opt->default_val.i64);
                }
                break;
            }
            case FX_OPT_TYPE_DURATION: {
                char buf[25];
                format_duration(buf, sizeof(buf), opt->default_val.i64);
                fx_log(fx_log_obj, FX_LOG_INFO, "%s", buf);
                break;
            }
            case FX_OPT_TYPE_INT:
            case FX_OPT_TYPE_INT64: {
                const char *def_const = get_opt_const_name(obj, opt->unit, opt->default_val.i64);
                if (def_const)
                    fx_log(fx_log_obj, FX_LOG_INFO, "%s", def_const);
                else
                    log_value(fx_log_obj, FX_LOG_INFO, opt->default_val.i64);
                break;
            }
            case FX_OPT_TYPE_DOUBLE:
            case FX_OPT_TYPE_FLOAT:
                log_value(fx_log_obj, FX_LOG_INFO, opt->default_val.dbl);
                break;
            case FX_OPT_TYPE_RATIONAL: {
                AVRational q = fx_d2q(opt->default_val.dbl, INT_MAX);
                fx_log(fx_log_obj, FX_LOG_INFO, "%d/%d", q.num, q.den); }
                break;
            case FX_OPT_TYPE_PIXEL_FMT:
                fx_log(fx_log_obj, FX_LOG_INFO, "%s", (char *)fx_x_if_null(fx_get_pix_fmt_name(opt->default_val.i64), "none"));
                break;
            case FX_OPT_TYPE_SAMPLE_FMT:
                fx_log(fx_log_obj, FX_LOG_INFO, "%s", (char *)fx_x_if_null(fx_get_sample_fmt_name(opt->default_val.i64), "none"));
                break;
            case FX_OPT_TYPE_COLOR:
            case FX_OPT_TYPE_IMAGE_SIZE:
            case FX_OPT_TYPE_STRING:
            case FX_OPT_TYPE_VIDEO_RATE:
                fx_log(fx_log_obj, FX_LOG_INFO, "\"%s\"", opt->default_val.str);
                break;
            case FX_OPT_TYPE_CHANNEL_LAYOUT:
                fx_log(fx_log_obj, FX_LOG_INFO, "0x%"PRIx64, opt->default_val.i64);
                break;
            }
            fx_log(fx_log_obj, FX_LOG_INFO, ")");
        }

        fx_log(fx_log_obj, FX_LOG_INFO, "\n");
        if (opt->unit && opt->type != FX_OPT_TYPE_CONST)
            opt_list(obj, fx_log_obj, opt->unit, req_flags, rej_flags);
    }
}

/**
 * Show the obj options.
 *
 * @param req_flags requested flags for the options to show. Show only the
 * options for which it is opt->flags & req_flags.
 * @param rej_flags rejected flags for the options to show. Show only the
 * options for which it is !(opt->flags & req_flags).
 * @param fx_log_obj log context to use for showing the options
 */
int fx_opt_show2(void *obj, void *fx_log_obj, int req_flags, int rej_flags)
{
    if (!obj)
        return -1;

    fx_log(fx_log_obj, FX_LOG_INFO, "%s AVOptions:\n", (*(AVClass **)obj)->class_name);

    opt_list(obj, fx_log_obj, NULL, req_flags, rej_flags);

    return 0;
}

/**
 * Set the values of all AVOption fields to their default values. Only these
 * AVOption fields for which (opt->flags & mask) == flags will have their
 * default applied to s.
 *
 * @param s an AVOption-enabled struct (its first member must be a pointer to AVClass)
 * @param mask combination of FX_OPT_FLAG_*
 * @param flags combination of FX_OPT_FLAG_*
 */
void fx_opt_set_defaults2(void *s, int mask, int flags)
{
    const AVOption *opt = NULL;
    while ((opt = fx_opt_next(s, opt))) {
        void *dst = ((uint8_t*)s) + opt->offset;

        if ((opt->flags & mask) != flags)
            continue;

        if (opt->flags & FX_OPT_FLAG_READONLY)
            continue;

        switch (opt->type) {
            case FX_OPT_TYPE_CONST:
                /* Nothing to be done here */
                break;
            case FX_OPT_TYPE_BOOL:
            case FX_OPT_TYPE_FLAGS:
            case FX_OPT_TYPE_INT:
            case FX_OPT_TYPE_INT64:
            case FX_OPT_TYPE_DURATION:
            case FX_OPT_TYPE_CHANNEL_LAYOUT:
            case FX_OPT_TYPE_PIXEL_FMT:
            case FX_OPT_TYPE_SAMPLE_FMT:
                write_number(s, opt, dst, 1, 1, opt->default_val.i64);
                break;
            case FX_OPT_TYPE_DOUBLE:
            case FX_OPT_TYPE_FLOAT: {
                double val;
                val = opt->default_val.dbl;
                write_number(s, opt, dst, val, 1, 1);
            }
            break;
            case FX_OPT_TYPE_RATIONAL: {
                AVRational val;
                val = fx_d2q(opt->default_val.dbl, INT_MAX);
                write_number(s, opt, dst, 1, val.den, val.num);
            }
            break;
            case FX_OPT_TYPE_COLOR:
                set_string_color(s, opt, opt->default_val.str, dst);
                break;
            case FX_OPT_TYPE_STRING:
                set_string(s, opt, opt->default_val.str, dst);
                break;
            case FX_OPT_TYPE_IMAGE_SIZE:
                set_string_image_size(s, opt, opt->default_val.str, dst);
                break;
            case FX_OPT_TYPE_VIDEO_RATE:
                set_string_video_rate(s, opt, opt->default_val.str, dst);
                break;
            case FX_OPT_TYPE_BINARY:
                set_string_binary(s, opt, opt->default_val.str, dst);
                break;
            case FX_OPT_TYPE_DICT:
                /* Cannot set defaults for these types */
            break;
        default:
            fx_log(s, FX_LOG_DEBUG, "AVOption type %d of option %s not implemented yet\n",
                   opt->type, opt->name);
        }
    }
}
/**
 * Set the values of all AVOption fields to their default values.
 *
 * @param s an AVOption-enabled struct (its first member must be a pointer to AVClass)
 */

void fx_opt_set_defaults(void *s)
{
    fx_opt_set_defaults2(s, 0, 0);
}
/**
 * Store the value in the field in ctx that is named like key.
 * ctx must be an AVClass context, storing is done using AVOptions.
 *
 * @param buf the string to parse, buf will be updated to point at the
 * separator just after the parsed key/value pair
 * @param key_val_sep a 0-terminated list of characters used to
 * separate key from value
 * @param pairs_sep a 0-terminated list of characters used to separate
 * two pairs from each other
 * @return 0 if the key/value pair has been successfully parsed and
 * set, or a negative value corresponding to an AVERROR code in case
 * of error:
 * AVERROR(EINVAL) if the key/value pair cannot be parsed,
 * the error code issued by fx_opt_set() if the key/value pair
 * cannot be set
 */
static int parse_key_value_pair(void *ctx, const char **buf,
                                const char *key_val_sep, const char *pairs_sep)
{
    char *key = fx_get_token(buf, key_val_sep);
    char *val;
    int ret;

    if (!key)
        return AVERROR(ENOMEM);

    if (*key && strspn(*buf, key_val_sep)) {
        (*buf)++;
        val = fx_get_token(buf, pairs_sep);
        if (!val) {
            fx_freep(&key);
            return AVERROR(ENOMEM);
        }
    } else {
        fx_log(ctx, FX_LOG_ERROR, "Missing key or no key/value separator found after key '%s'\n", key);
        fx_free(key);
        return AVERROR(EINVAL);
    }

    fx_log(ctx, FX_LOG_DEBUG, "Setting entry with key '%s' to value '%s'\n", key, val);

    ret = fx_opt_set(ctx, key, val, FX_OPT_SEARCH_CHILDREN);
    if (ret == AVERROR_OPTION_NOT_FOUND)
        fx_log(ctx, FX_LOG_ERROR, "Key '%s' not found.\n", key);

    fx_free(key);
    fx_free(val);
    return ret;
}
/**
 * Parse the key/value pairs list in opts. For each key/value pair
 * found, stores the value in the field in ctx that is named like the
 * key. ctx must be an AVClass context, storing is done using
 * AVOptions.
 *
 * @param opts options string to parse, may be NULL
 * @param key_val_sep a 0-terminated list of characters used to
 * separate key from value
 * @param pairs_sep a 0-terminated list of characters used to separate
 * two pairs from each other
 * @return the number of successfully set key/value pairs, or a negative
 * value corresponding to an AVERROR code in case of error:
 * AVERROR(EINVAL) if opts cannot be parsed,
 * the error code issued by fx_opt_set() if a key/value pair
 * cannot be set
 */
int fx_set_options_string(void *ctx, const char *opts,
                          const char *key_val_sep, const char *pairs_sep)
{
    int ret, count = 0;

    if (!opts)
        return 0;

    while (*opts) {
        if ((ret = parse_key_value_pair(ctx, &opts, key_val_sep, pairs_sep)) < 0)
            return ret;
        count++;

        if (*opts)
            opts++;
    }

    return count;
}

#define WHITESPACES " \n\t\r"

static int is_key_char(char c)
{
    return (unsigned)((c | 32) - 'a') < 26 ||
           (unsigned)(c - '0') < 10 ||
           c == '-' || c == '_' || c == '/' || c == '.';
}

/**
 * Read a key from a string.
 *
 * The key consists of is_key_char characters and must be terminated by a
 * character from the delim string; spaces are ignored.
 *
 * @return  0 for success (even with ellipsis), <0 for failure
 */
static int get_key(const char **ropts, const char *delim, char **rkey)
{
    const char *opts = *ropts;
    const char *key_start, *key_end;

    key_start = opts += strspn(opts, WHITESPACES);
    while (is_key_char(*opts))
        opts++;
    key_end = opts;
    opts += strspn(opts, WHITESPACES);
    if (!*opts || !strchr(delim, *opts))
        return AVERROR(EINVAL);
    opts++;
    if (!(*rkey = fx_malloc(key_end - key_start + 1)))
        return AVERROR(ENOMEM);
    memcpy(*rkey, key_start, key_end - key_start);
    (*rkey)[key_end - key_start] = 0;
    *ropts = opts;
    return 0;
}

/**
 * Extract a key-value pair from the beginning of a string.
 *
 * @param ropts        pointer to the options string, will be updated to
 *                     point to the rest of the string (one of the pairs_sep
 *                     or the final NUL)
 * @param key_val_sep  a 0-terminated list of characters used to separate
 *                     key from value, for example '='
 * @param pairs_sep    a 0-terminated list of characters used to separate
 *                     two pairs from each other, for example ':' or ','
 * @param flags        flags; see the FX_OPT_FLAG_* values below
 * @param rkey         parsed key; must be freed using fx_free()
 * @param rval         parsed value; must be freed using fx_free()
 *
 * @return  >=0 for success, or a negative value corresponding to an
 *          AVERROR code in case of error; in particular:
 *          AVERROR(EINVAL) if no key is present
 *
 */

int fx_opt_get_key_value(const char **ropts,
                         const char *key_val_sep, const char *pairs_sep,
                         unsigned flags,
                         char **rkey, char **rval)
{
    int ret;
    char *key = NULL, *val;
    const char *opts = *ropts;

    if ((ret = get_key(&opts, key_val_sep, &key)) < 0 &&
        !(flags & FX_OPT_FLAG_IMPLICIT_KEY))
        return AVERROR(EINVAL);
    if (!(val = fx_get_token(&opts, pairs_sep))) {
        fx_free(key);
        return AVERROR(ENOMEM);
    }
    *ropts = opts;
    *rkey  = key;
    *rval  = val;
    return 0;
}
/**
 * Parse the key-value pairs list in opts. For each key=value pair found,
 * set the value of the corresponding option in ctx.
 *
 * @param ctx          the AVClass object to set options on
 * @param opts         the options string, key-value pairs separated by a
 *                     delimiter
 * @param shorthand    a NULL-terminated array of options names for shorthand
 *                     notation: if the first field in opts has no key part,
 *                     the key is taken from the first element of shorthand;
 *                     then again for the second, etc., until either opts is
 *                     finished, shorthand is finished or a named option is
 *                     found; after that, all options must be named
 * @param key_val_sep  a 0-terminated list of characters used to separate
 *                     key from value, for example '='
 * @param pairs_sep    a 0-terminated list of characters used to separate
 *                     two pairs from each other, for example ':' or ','
 * @return  the number of successfully set key=value pairs, or a negative
 *          value corresponding to an AVERROR code in case of error:
 *          AVERROR(EINVAL) if opts cannot be parsed,
 *          the error code issued by fx_set_string3() if a key/value pair
 *          cannot be set
 *
 * Options names must use only the following characters: a-z A-Z 0-9 - . / _
 * Separators must use characters distinct from option names and from each
 * other.
 */
int fx_opt_set_from_string(void *ctx, const char *opts,
                           const char *const *shorthand,
                           const char *key_val_sep, const char *pairs_sep)
{
    int ret, count = 0;
    const char *dummy_shorthand = NULL;
    char *fx_uninit(parsed_key), *fx_uninit(value);
    const char *key;

    if (!opts)
        return 0;
    if (!shorthand)
        shorthand = &dummy_shorthand;

    while (*opts) {
        ret = fx_opt_get_key_value(&opts, key_val_sep, pairs_sep,
                                   *shorthand ? FX_OPT_FLAG_IMPLICIT_KEY : 0,
                                   &parsed_key, &value);
        if (ret < 0) {
            if (ret == AVERROR(EINVAL))
                fx_log(ctx, FX_LOG_ERROR, "No option name near '%s'\n", opts);
            else
                fx_log(ctx, FX_LOG_ERROR, "Unable to parse '%s': %s\n", opts,
                       fx_err2str(ret));
            return ret;
        }
        if (*opts)
            opts++;
        if (parsed_key) {
            key = parsed_key;
            while (*shorthand) /* discard all remaining shorthand */
                shorthand++;
        } else {
            key = *(shorthand++);
        }

        fx_log(ctx, FX_LOG_DEBUG, "Setting '%s' to value '%s'\n", key, value);
        if ((ret = fx_opt_set(ctx, key, value, 0)) < 0) {
            if (ret == AVERROR_OPTION_NOT_FOUND)
                fx_log(ctx, FX_LOG_ERROR, "Option '%s' not found\n", key);
            fx_free(value);
            fx_free(parsed_key);
            return ret;
        }

        fx_free(value);
        fx_free(parsed_key);
        count++;
    }
    return count;
}

/**
 * Free all allocated objects in obj.
 */
void fx_opt_free(void *obj)
{
    const AVOption *o = NULL;
    while ((o = fx_opt_next(obj, o))) {
        switch (o->type) {
        case FX_OPT_TYPE_STRING:
        case FX_OPT_TYPE_BINARY:
            fx_freep((uint8_t *)obj + o->offset);
            break;

        case FX_OPT_TYPE_DICT:
            fx_dict_free((AVDictionary **)(((uint8_t *)obj) + o->offset));
            break;

        default:
            break;
        }
    }
}
/**
 * Set all the options from a given dictionary on an object.
 *
 * @param obj a struct whose first element is a pointer to AVClass
 * @param options options to process. This dictionary will be freed and replaced
 *                by a new one containing all options not found in obj.
 *                Of course this new dictionary needs to be freed by caller
 *                with fx_dict_free().
 * @param search_flags A combination of FX_OPT_SEARCH_*.
 *
 * @return 0 on success, a negative AVERROR if some option was found in obj,
 *         but could not be set.
 *
 * @see fx_dict_copy()
 */
int fx_opt_set_dict2(void *obj, AVDictionary **options, int search_flags)
{
    AVDictionaryEntry *t = NULL;
    AVDictionary    *tmp = NULL;
    int ret = 0;

    if (!options)
        return 0;

    while ((t = fx_dict_get(*options, "", t, FX_DICT_IGNORE_SUFFIX))) {
        ret = fx_opt_set(obj, t->key, t->value, search_flags);
        if (ret == AVERROR_OPTION_NOT_FOUND)
            ret = fx_dict_set(&tmp, t->key, t->value, 0);
        if (ret < 0) {
            fx_log(obj, FX_LOG_ERROR, "Error setting option %s to value %s.\n", t->key, t->value);
            fx_dict_free(&tmp);
            return ret;
        }
        ret = 0;
    }
    fx_dict_free(options);
    *options = tmp;
    return ret;
}
/**
 * Set all the options from a given dictionary on an object.
 *
 * @param obj a struct whose first element is a pointer to AVClass
 * @param options options to process. This dictionary will be freed and replaced
 *                by a new one containing all options not found in obj.
 *                Of course this new dictionary needs to be freed by caller
 *                with fx_dict_free().
 *
 * @return 0 on success, a negative AVERROR if some option was found in obj,
 *         but could not be set.
 *
 * @see fx_dict_copy()
 */

int fx_opt_set_dict(void *obj, AVDictionary **options)
{
    return fx_opt_set_dict2(obj, options, 0);
}
/**
 * Look for an option in an object. Consider only options which
 * have all the specified flags set.
 *
 * @param[in] obj A pointer to a struct whose first element is a
 *                pointer to an AVClass.
 *                Alternatively a double pointer to an AVClass, if
 *                FX_OPT_SEARCH_FAKE_OBJ search flag is set.
 * @param[in] name The name of the option to look for.
 * @param[in] unit When searching for named constants, name of the unit
 *                 it belongs to.
 * @param opt_flags Find only options with all the specified flags set (FX_OPT_FLAG).
 * @param search_flags A combination of FX_OPT_SEARCH_*.
 *
 * @return A pointer to the option found, or NULL if no option
 *         was found.
 *
 * @note Options found with FX_OPT_SEARCH_CHILDREN flag may not be settable
 * directly with fx_opt_set(). Use special calls which take an options
 * AVDictionary (e.g. avformat_open_input()) to set options found with this
 * flag.
 */

const AVOption *fx_opt_find(void *obj, const char *name, const char *unit,
                            int opt_flags, int search_flags)
{
    return fx_opt_find2(obj, name, unit, opt_flags, search_flags, NULL);
}
/**
 * Look for an option in an object. Consider only options which
 * have all the specified flags set.
 *
 * @param[in] obj A pointer to a struct whose first element is a
 *                pointer to an AVClass.
 *                Alternatively a double pointer to an AVClass, if
 *                FX_OPT_SEARCH_FAKE_OBJ search flag is set.
 * @param[in] name The name of the option to look for.
 * @param[in] unit When searching for named constants, name of the unit
 *                 it belongs to.
 * @param opt_flags Find only options with all the specified flags set (FX_OPT_FLAG).
 * @param search_flags A combination of FX_OPT_SEARCH_*.
 * @param[out] target_obj if non-NULL, an object to which the option belongs will be
 * written here. It may be different from obj if FX_OPT_SEARCH_CHILDREN is present
 * in search_flags. This parameter is ignored if search_flags contain
 * FX_OPT_SEARCH_FAKE_OBJ.
 *
 * @return A pointer to the option found, or NULL if no option
 *         was found.
 */
const AVOption *fx_opt_find2(void *obj, const char *name, const char *unit,
                             int opt_flags, int search_flags, void **target_obj)
{
    const AVClass  *c;
    const AVOption *o = NULL;

    if(!obj)
        return NULL;

    c= *(AVClass**)obj;

    if (!c)
        return NULL;

    if (search_flags & FX_OPT_SEARCH_CHILDREN) {
        if (search_flags & FX_OPT_SEARCH_FAKE_OBJ) {
            const AVClass *child = NULL;
            while (child = fx_opt_child_class_next(c, child))
                if (o = fx_opt_find2(&child, name, unit, opt_flags, search_flags, NULL))
                    return o;
        } else {
            void *child = NULL;
            while (child = fx_opt_child_next(obj, child))
                if (o = fx_opt_find2(child, name, unit, opt_flags, search_flags, target_obj))
                    return o;
        }
    }

    while (o = fx_opt_next(obj, o)) {
        if (!strcmp(o->name, name) && (o->flags & opt_flags) == opt_flags &&
            ((!unit && o->type != FX_OPT_TYPE_CONST) ||
             (unit  && o->type == FX_OPT_TYPE_CONST && o->unit && !strcmp(o->unit, unit)))) {
            if (target_obj) {
                if (!(search_flags & FX_OPT_SEARCH_FAKE_OBJ))
                    *target_obj = obj;
                else
                    *target_obj = NULL;
            }
            return o;
        }
    }
    return NULL;
}

/**
 * Iterate over AVOptions-enabled children of obj.
 *
 * @param prev result of a previous call to this function or NULL
 * @return next AVOptions-enabled child or NULL
 */

void *fx_opt_child_next(void *obj, void *prev)
{
    const AVClass *c = *(AVClass **)obj;
    if (c->child_next)
        return c->child_next(obj, prev);
    return NULL;
}
/**
 * Iterate over potential AVOptions-enabled children of parent.
 *
 * @param prev result of a previous call to this function or NULL
 * @return AVClass corresponding to next potential child or NULL
 */
const AVClass *fx_opt_child_class_next(const AVClass *parent, const AVClass *prev)
{
    if (parent->child_class_next)
        return parent->child_class_next(prev);
    return NULL;
}
/**
 * Gets a pointer to the requested field in a struct.
 * This function allows accessing a struct even when its fields are moved or
 * renamed since the application making the access has been compiled,
 *
 * @returns a pointer to the field, it can be cast to the correct type and read
 *          or written to.
 */
void *fx_opt_ptr(const AVClass *class, void *obj, const char *name)
{
    const AVOption *opt= fx_opt_find2(&class, name, NULL, 0, FX_OPT_SEARCH_FAKE_OBJ, NULL);
    if(!opt)
        return NULL;
    return (uint8_t*)obj + opt->offset;
}

static int opt_size(enum AVOptionType type)
{
    switch(type) {
    case FX_OPT_TYPE_BOOL:
    case FX_OPT_TYPE_INT:
    case FX_OPT_TYPE_FLAGS:
        return sizeof(int);
    case FX_OPT_TYPE_DURATION:
    case FX_OPT_TYPE_CHANNEL_LAYOUT:
    case FX_OPT_TYPE_INT64:
        return sizeof(int64_t);
    case FX_OPT_TYPE_DOUBLE:
        return sizeof(double);
    case FX_OPT_TYPE_FLOAT:
        return sizeof(float);
    case FX_OPT_TYPE_STRING:
        return sizeof(uint8_t*);
    case FX_OPT_TYPE_VIDEO_RATE:
    case FX_OPT_TYPE_RATIONAL:
        return sizeof(AVRational);
    case FX_OPT_TYPE_BINARY:
        return sizeof(uint8_t*) + sizeof(int);
    case FX_OPT_TYPE_IMAGE_SIZE:
        return sizeof(int[2]);
    case FX_OPT_TYPE_PIXEL_FMT:
        return sizeof(enum AVPixelFormat);
    case FX_OPT_TYPE_SAMPLE_FMT:
        return sizeof(enum AVSampleFormat);
    case FX_OPT_TYPE_COLOR:
        return 4;
    }
    return AVERROR(EINVAL);
}
/**
 * Copy options from src object into dest object.
 *
 * Options that require memory allocation (e.g. string or binary) are malloc'ed in dest object.
 * Original memory allocated for such options is freed unless both src and dest options points to the same memory.
 *
 * @param dest Object to copy from
 * @param src  Object to copy into
 * @return 0 on success, negative on error
 */
int fx_opt_copy(void *dst, const void *src)
{
    const AVOption *o = NULL;
    const AVClass *c;
    int ret = 0;

    if (!src)
        return AVERROR(EINVAL);

    c = *(AVClass **)src;
    if (!c || c != *(AVClass **)dst)
        return AVERROR(EINVAL);

    while ((o = fx_opt_next(src, o))) {
        void *field_dst = (uint8_t *)dst + o->offset;
        void *field_src = (uint8_t *)src + o->offset;
        uint8_t **field_dst8 = (uint8_t **)field_dst;
        uint8_t **field_src8 = (uint8_t **)field_src;

        if (o->type == FX_OPT_TYPE_STRING) {
            if (*field_dst8 != *field_src8)
                fx_freep(field_dst8);
            *field_dst8 = fx_strdup(*field_src8);
            if (*field_src8 && !*field_dst8)
                ret = AVERROR(ENOMEM);
        } else if (o->type == FX_OPT_TYPE_BINARY) {
            int len = *(int *)(field_src8 + 1);
            if (*field_dst8 != *field_src8)
                fx_freep(field_dst8);
            *field_dst8 = fx_memdup(*field_src8, len);
            if (len && !*field_dst8) {
                ret = AVERROR(ENOMEM);
                len = 0;
            }
            *(int *)(field_dst8 + 1) = len;
        } else if (o->type == FX_OPT_TYPE_CONST) {
            // do nothing
        } else if (o->type == FX_OPT_TYPE_DICT) {
            AVDictionary **sdict = (AVDictionary **) field_src;
            AVDictionary **ddict = (AVDictionary **) field_dst;
            if (*sdict != *ddict)
                fx_dict_free(ddict);
            *ddict = NULL;
            fx_dict_copy(ddict, *sdict, 0);
            if (fx_dict_count(*sdict) != fx_dict_count(*ddict))
                ret = AVERROR(ENOMEM);
        } else {
            int size = opt_size(o->type);
            if (size < 0)
                ret = size;
            else
                memcpy(field_dst, field_src, size);
        }
    }
    return ret;
}
/**
 * Get a list of allowed ranges for the given option.
 *
 * The returned list may depend on other fields in obj like for example profile.
 *
 * @param flags is a bitmask of flags, undefined flags should not be set and should be ignored
 *              FX_OPT_SEARCH_FAKE_OBJ indicates that the obj is a double pointer to a AVClass instead of a full instance
 *              FX_OPT_MULTI_COMPONENT_RANGE indicates that function may return more than one component, @see AVOptionRanges
 *
 * The result must be freed with fx_opt_freep_ranges.
 *
 * @return number of compontents returned on success, a negative errro code otherwise
 */

int fx_opt_query_ranges(AVOptionRanges **ranges_arg, void *obj, const char *key, int flags)
{
    int ret;
    const AVClass *c = *(AVClass**)obj;
    int (*callback)(AVOptionRanges **, void *obj, const char *key, int flags) = NULL;

    if (c->version > (52 << 16 | 11 << 8))
        callback = c->query_ranges;

    if (!callback)
        callback = fx_opt_query_ranges_default;

    ret = callback(ranges_arg, obj, key, flags);
    if (ret >= 0) {
        if (!(flags & FX_OPT_MULTI_COMPONENT_RANGE))
            ret = 1;
        (*ranges_arg)->nb_components = ret;
    }
    return ret;
}
/**
 * Get a default list of allowed ranges for the given option.
 *
 * This list is constructed without using the AVClass.query_ranges() callback
 * and can be used as fallback from within the callback.
 *
 * @param flags is a bitmask of flags, undefined flags should not be set and should be ignored
 *              FX_OPT_SEARCH_FAKE_OBJ indicates that the obj is a double pointer to a AVClass instead of a full instance
 *              FX_OPT_MULTI_COMPONENT_RANGE indicates that function may return more than one component, @see AVOptionRanges
 *
 * The result must be freed with fx_opt_free_ranges.
 *
 * @return number of compontents returned on success, a negative errro code otherwise
 */

int fx_opt_query_ranges_default(AVOptionRanges **ranges_arg, void *obj, const char *key, int flags)
{
    AVOptionRanges *ranges = fx_mallocz(sizeof(*ranges));
    AVOptionRange **range_array = fx_mallocz(sizeof(void*));
    AVOptionRange *range = fx_mallocz(sizeof(*range));
    const AVOption *field = fx_opt_find(obj, key, NULL, 0, flags);
    int ret;

    *ranges_arg = NULL;

    if (!ranges || !range || !range_array || !field) {
        ret = AVERROR(ENOMEM);
        goto fail;
    }

    ranges->range = range_array;
    ranges->range[0] = range;
    ranges->nb_ranges = 1;
    ranges->nb_components = 1;
    range->is_range = 1;
    range->value_min = field->min;
    range->value_max = field->max;

    switch (field->type) {
    case FX_OPT_TYPE_BOOL:
    case FX_OPT_TYPE_INT:
    case FX_OPT_TYPE_INT64:
    case FX_OPT_TYPE_PIXEL_FMT:
    case FX_OPT_TYPE_SAMPLE_FMT:
    case FX_OPT_TYPE_FLOAT:
    case FX_OPT_TYPE_DOUBLE:
    case FX_OPT_TYPE_DURATION:
    case FX_OPT_TYPE_COLOR:
    case FX_OPT_TYPE_CHANNEL_LAYOUT:
        break;
    case FX_OPT_TYPE_STRING:
        range->component_min = 0;
        range->component_max = 0x10FFFF; // max unicode value
        range->value_min = -1;
        range->value_max = INT_MAX;
        break;
    case FX_OPT_TYPE_RATIONAL:
        range->component_min = INT_MIN;
        range->component_max = INT_MAX;
        break;
    case FX_OPT_TYPE_IMAGE_SIZE:
        range->component_min = 0;
        range->component_max = INT_MAX/128/8;
        range->value_min = 0;
        range->value_max = INT_MAX/8;
        break;
    case FX_OPT_TYPE_VIDEO_RATE:
        range->component_min = 1;
        range->component_max = INT_MAX;
        range->value_min = 1;
        range->value_max = INT_MAX;
        break;
    default:
        ret = AVERROR(ENOSYS);
        goto fail;
    }

    *ranges_arg = ranges;
    return 1;
fail:
    fx_free(ranges);
    fx_free(range);
    fx_free(range_array);
    return ret;
}
/**
 * Free an AVOptionRanges struct and set it to NULL.
 */
void fx_opt_freep_ranges(AVOptionRanges **rangesp)
{
    int i;
    AVOptionRanges *ranges = *rangesp;

    if (!ranges)
        return;

    for (i = 0; i < ranges->nb_ranges * ranges->nb_components; i++) {
        AVOptionRange *range = ranges->range[i];
        if (range) {
            fx_freep(&range->str);
            fx_freep(&ranges->range[i]);
        }
    }
    fx_freep(&ranges->range);
    fx_freep(rangesp);
}
/**
 * Check if given option is set to its default value.
 *
 * Options o must belong to the obj. This function must not be called to check child's options state.
 * @see fx_opt_is_set_to_default_by_name().
 *
 * @param obj  AVClass object to check option on
 * @param o    option to be checked
 * @return     >0 when option is set to its default,
 *              0 when option is not set its default,
 *             <0 on error
 */
int fx_opt_is_set_to_default(void *obj, const AVOption *o)
{
    int64_t i64;
    double d, d2;
    float f;
    AVRational q;
    int ret, w, h;
    char *str;
    void *dst;

    if (!o || !obj)
        return AVERROR(EINVAL);

    dst = ((uint8_t*)obj) + o->offset;

    switch (o->type) {
    case FX_OPT_TYPE_CONST:
        return 1;
    case FX_OPT_TYPE_BOOL:
    case FX_OPT_TYPE_FLAGS:
    case FX_OPT_TYPE_PIXEL_FMT:
    case FX_OPT_TYPE_SAMPLE_FMT:
    case FX_OPT_TYPE_INT:
    case FX_OPT_TYPE_CHANNEL_LAYOUT:
    case FX_OPT_TYPE_DURATION:
    case FX_OPT_TYPE_INT64:
        read_number(o, dst, NULL, NULL, &i64);
        return o->default_val.i64 == i64;
    case FX_OPT_TYPE_STRING:
        str = *(char **)dst;
        if (str == o->default_val.str) //2 NULLs
            return 1;
        if (!str || !o->default_val.str) //1 NULL
            return 0;
        return !strcmp(str, o->default_val.str);
    case FX_OPT_TYPE_DOUBLE:
        read_number(o, dst, &d, NULL, NULL);
        return o->default_val.dbl == d;
    case FX_OPT_TYPE_FLOAT:
        read_number(o, dst, &d, NULL, NULL);
        f = o->default_val.dbl;
        d2 = f;
        return d2 == d;
    case FX_OPT_TYPE_RATIONAL:
        q = fx_d2q(o->default_val.dbl, INT_MAX);
        return !fx_cmp_q(*(AVRational*)dst, q);
    case FX_OPT_TYPE_BINARY: {
        struct {
            uint8_t *data;
            int size;
        } tmp = {0};
        int opt_size = *(int *)((void **)dst + 1);
        void *opt_ptr = *(void **)dst;
        if (!opt_size && (!o->default_val.str || !strlen(o->default_val.str)))
            return 1;
        if (!opt_size ||  !o->default_val.str || !strlen(o->default_val.str ))
            return 0;
        if (opt_size != strlen(o->default_val.str) / 2)
            return 0;
        ret = set_string_binary(NULL, NULL, o->default_val.str, &tmp.data);
        if (!ret)
            ret = !memcmp(opt_ptr, tmp.data, tmp.size);
        fx_free(tmp.data);
        return ret;
    }
    case FX_OPT_TYPE_DICT:
        /* Binary and dict have not default support yet. Any pointer is not default. */
        return !!(*(void **)dst);
    case FX_OPT_TYPE_IMAGE_SIZE:
        if (!o->default_val.str || !strcmp(o->default_val.str, "none"))
            w = h = 0;
        else if ((ret = fx_parse_video_size(&w, &h, o->default_val.str)) < 0)
            return ret;
        return (w == *(int *)dst) && (h == *((int *)dst+1));
    case FX_OPT_TYPE_VIDEO_RATE:
        q = (AVRational){0, 0};
        if (o->default_val.str) {
            if ((ret = fx_parse_video_rate(&q, o->default_val.str)) < 0)
                return ret;
        }
        return !fx_cmp_q(*(AVRational*)dst, q);
    case FX_OPT_TYPE_COLOR: {
        uint8_t color[4] = {0, 0, 0, 0};
        if (o->default_val.str) {
            if ((ret = fx_parse_color(color, o->default_val.str, -1, NULL)) < 0)
                return ret;
        }
        return !memcmp(color, dst, sizeof(color));
    }
    default:
        fx_log(obj, FX_LOG_WARNING, "Not supported option type: %d, option name: %s\n", o->type, o->name);
        break;
    }
    return AVERROR_PATCHWELCOME;
}
/**
 * Check if given option is set to its default value.
 *
 * @param obj          AVClass object to check option on
 * @param name         option name
 * @param search_flags combination of FX_OPT_SEARCH_*
 * @return             >0 when option is set to its default,
 *                     0 when option is not set its default,
 *                     <0 on error
 */
int fx_opt_is_set_to_default_by_name(void *obj, const char *name, int search_flags)
{
    const AVOption *o;
    void *target;
    if (!obj)
        return AVERROR(EINVAL);
    o = fx_opt_find2(obj, name, NULL, 0, search_flags, &target);
    if (!o)
        return AVERROR_OPTION_NOT_FOUND;
    return fx_opt_is_set_to_default(target, o);
}

#define FX_OPT_SERIALIZE_SKIP_DEFAULTS              0x00000001  ///< Serialize options that are not set to default values only.
#define FX_OPT_SERIALIZE_OPT_FLAGS_EXACT            0x00000002  ///< Serialize options that exactly match opt_flags only.

/**
 * Serialize object's options.
 *
 * Create a string containing object's serialized options.
 * Such string may be passed back to fx_opt_set_from_string() in order to restore option values.
 * A key/value or pairs separator occurring in the serialized value or
 * name string are escaped through the fx_escape() function.
 *
 * @param[in]  obj           AVClass object to serialize
 * @param[in]  opt_flags     serialize options with all the specified flags set (FX_OPT_FLAG)
 * @param[in]  flags         combination of FX_OPT_SERIALIZE_* flags
 * @param[out] buffer        Pointer to buffer that will be allocated with string containg serialized options.
 *                           Buffer must be freed by the caller when is no longer needed.
 * @param[in]  key_val_sep   character used to separate key from value
 * @param[in]  pairs_sep     character used to separate two pairs from each other
 * @return                   >= 0 on success, negative on error
 * @warning Separators cannot be neither '\\' nor '\0'. They also cannot be the same.
 */
int fx_opt_serialize(void *obj, int opt_flags, int flags, char **buffer,
                     const char key_val_sep, const char pairs_sep)
{
    const AVOption *o = NULL;
    uint8_t *buf;
    AVBPrint bprint;
    int ret, cnt = 0;
    const char special_chars[] = {pairs_sep, key_val_sep, '\0'};

    if (pairs_sep == '\0' || key_val_sep == '\0' || pairs_sep == key_val_sep ||
        pairs_sep == '\\' || key_val_sep == '\\') {
        fx_log(obj, FX_LOG_ERROR, "Invalid separator(s) found.");
        return AVERROR(EINVAL);
    }

    if (!obj || !buffer)
        return AVERROR(EINVAL);

    *buffer = NULL;
    fx_bprint_init(&bprint, 64, FX_BPRINT_SIZE_UNLIMITED);

    while (o = fx_opt_next(obj, o)) {
        if (o->type == FX_OPT_TYPE_CONST)
            continue;
        if ((flags & FX_OPT_SERIALIZE_OPT_FLAGS_EXACT) && o->flags != opt_flags)
            continue;
        else if (((o->flags & opt_flags) != opt_flags))
            continue;
        if (flags & FX_OPT_SERIALIZE_SKIP_DEFAULTS && fx_opt_is_set_to_default(obj, o) > 0)
            continue;
        if ((ret = fx_opt_get(obj, o->name, 0, &buf)) < 0) {
            fx_bprint_finalize(&bprint, NULL);
            return ret;
        }
        if (buf) {
            if (cnt++)
                fx_bprint_append_data(&bprint, &pairs_sep, 1);
            fx_bprint_escape(&bprint, o->name, special_chars, FX_ESCAPE_MODE_BACKSLASH, 0);
            fx_bprint_append_data(&bprint, &key_val_sep, 1);
            fx_bprint_escape(&bprint, buf, special_chars, FX_ESCAPE_MODE_BACKSLASH, 0);
            fx_freep(&buf);
        }
    }
    fx_bprint_finalize(&bprint, buffer);
    return 0;
}

#endif /* AVUTIL_OPT_H */
