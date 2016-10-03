/*
 * rational numbers
 * Copyright (c) 2003 Michael Niedermayer <michaelni@gmx.at>
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

/**
 * @file
 * @ingroup lavu_math_rational
 * Utilties for rational number calculation.
 * @author Michael Niedermayer <michaelni@gmx.at>
 */

#ifndef AVUTIL_RATIONAL_H
#define AVUTIL_RATIONAL_H


#include <limits.h>
#include "fx_mathops.h"
#include "fx_types.h"
#include "fx_macros.h"
#include <stdlib.h>
//#include "fx_round.h"
/**
 * @defgroup lavu_math_rational AVRational
 * @ingroup lavu_math
 * Rational number calculation.
 *
 * While rational numbers can be expressed as floating-point numbers, the
 * conversion process is a lossy one, so are floating-point operations. On the
 * other hand, the nature of FFmpeg demands highly accurate calculation of
 * timestamps. This set of rational number utilities serves as a generic
 * interface for manipulating rational numbers as pairs of numerators and
 * denominators.
 *
 * Many of the functions that operate on AVRational's have the suffix `_q`, in
 * reference to the mathematical symbol "ℚ" (Q) which denotes the set of all
 * rational numbers.
 *
 * @{
 */

/**
 * Rational number (pair of numerator and denominator).
 */
typedef struct AVRational{
    int num; ///< Numerator
    int den; ///< Denominator
} AVRational;

/**
 * Rounding methods.
 */
enum AVRounding {
    FX_ROUND_ZERO     = 0, ///< Round toward zero.
    FX_ROUND_INF      = 1, ///< Round away from zero.
    FX_ROUND_DOWN     = 2, ///< Round toward -infinity.
    FX_ROUND_UP       = 3, ///< Round toward +infinity.
    FX_ROUND_NEAR_INF = 5, ///< Round to nearest and halfway cases away from zero.
    /**
     * Flag telling rescaling functions to pass `INT64_MIN`/`MAX` through
     * unchanged, avoiding special cases for #fx_NOPTS_VALUE.
     *
     * Unlike other values of the enumeration AVRounding, this value is a
     * bitmask that must be used in conjunction with another value of the
     * enumeration through a bitwise OR, in order to set behavior for normal
     * cases.
     *
     * @code{.c}
     * fx_rescale_rnd(3, 1, 2, FX_ROUND_UP | FX_ROUND_PASS_MINMAX);
     * // Rescaling 3:
     * //     Calculating 3 * 1 / 2
     * //     3 / 2 is rounded up to 2
     * //     => 2
     *
     * fx_rescale_rnd(fx_NOPTS_VALUE, 1, 2, FX_ROUND_UP | FX_ROUND_PASS_MINMAX);
     * // Rescaling fx_NOPTS_VALUE:
     * //     fx_NOPTS_VALUE == INT64_MIN
     * //     fx_NOPTS_VALUE is passed through
     * //     => fx_NOPTS_VALUE
     * @endcode
     */
    FX_ROUND_PASS_MINMAX = 8192,
};





/**
 * Rescale a 64-bit integer with specified rounding.
 *
 * The operation is mathematically equivalent to `a * b / c`, but writing that
 * directly can overflow, and does not support different rounding methods.
 *
 * @see fx_rescale(), fx_rescale_q(), fx_rescale_q_rnd()
 */
int64_t fx_rescale_rnd(int64_t a, int64_t b, int64_t c, enum AVRounding rnd)
{
    int64_t r = 0;
    ASSERT(c > 0);
    ASSERT(b >=0);
    ASSERT((unsigned)(rnd&~FX_ROUND_PASS_MINMAX)<=5 && (rnd&~FX_ROUND_PASS_MINMAX)!=4);

    if (c <= 0 || b < 0 || !((unsigned)(rnd&~FX_ROUND_PASS_MINMAX)<=5 && (rnd&~FX_ROUND_PASS_MINMAX)!=4))
        return INT64_MIN;

    if (rnd & FX_ROUND_PASS_MINMAX) {
        if (a == INT64_MIN || a == INT64_MAX)
            return a;
        rnd -= FX_ROUND_PASS_MINMAX;
    }

    if (a < 0)
        return -(uint64_t)fx_rescale_rnd(-MAX(a, -INT64_MAX), b, c, rnd ^ ((rnd >> 1) & 1));

    if (rnd == FX_ROUND_NEAR_INF)
        r = c / 2;
    else if (rnd & 1)
        r = c - 1;

    if (b <= INT_MAX && c <= INT_MAX) {
        if (a <= INT_MAX)
            return (a * b + r) / c;
        else {
            int64_t ad = a / c;
            int64_t a2 = (a % c * b + r) / c;
            if (ad >= INT32_MAX && b && ad > (INT64_MAX - a2) / b)
                return INT64_MIN;
            return ad * b + a2;
        }
    } else {
#if 1
        uint64_t a0  = a & 0xFFFFFFFF;
        uint64_t a1  = a >> 32;
        uint64_t b0  = b & 0xFFFFFFFF;
        uint64_t b1  = b >> 32;
        uint64_t t1  = a0 * b1 + a1 * b0;
        uint64_t t1a = t1 << 32;
        int i;

        a0  = a0 * b0 + t1a;
        a1  = a1 * b1 + (t1 >> 32) + (a0 < t1a);
        a0 += r;
        a1 += a0 < r;

        for (i = 63; i >= 0; i--) {
            a1 += a1 + ((a0 >> i) & 1);
            t1 += t1;
            if (c <= a1) {
                a1 -= c;
                t1++;
            }
        }
        if (t1 > INT64_MAX)
            return INT64_MIN;
        return t1;
    }
#else
        AVInteger ai;
        ai = fx_mul_i(fx_int2i(a), fx_int2i(b));
        ai = fx_add_i(ai, fx_int2i(r));

        return fx_i2int(fx_div_i(ai, fx_int2i(c)));
    }
#endif
}
/**
 * Rescale a 64-bit integer with rounding to nearest.
 *
 * The operation is mathematically equivalent to `a * b / c`, but writing that
 * directly can overflow.
 *
 * This function is equivalent to fx_rescale_rnd() with #FX_ROUND_NEAR_INF.
 *
 * @see fx_rescale_rnd(), fx_rescale_q(), fx_rescale_q_rnd()
 */
int64_t fx_rescale(int64_t a, int64_t b, int64_t c)
{
    return fx_rescale_rnd(a, b, c, FX_ROUND_NEAR_INF);
}
/**
 * Rescale a 64-bit integer by 2 rational numbers with specified rounding.
 *
 * The operation is mathematically equivalent to `a * bq / cq`.
 *
 * @see fx_rescale(), fx_rescale_rnd(), fx_rescale_q()
 */
int64_t fx_rescale_q_rnd(int64_t a, AVRational bq, AVRational cq,
                         enum AVRounding rnd)
{
    int64_t b = bq.num * (int64_t)cq.den;
    int64_t c = cq.num * (int64_t)bq.den;
    return fx_rescale_rnd(a, b, c, rnd);
}
/**
 * Rescale a 64-bit integer by 2 rational numbers.
 *
 * The operation is mathematically equivalent to `a * bq / cq`.
 *
 * This function is equivalent to fx_rescale_q_rnd() with #FX_ROUND_NEAR_INF.
 *
 * @see fx_rescale(), fx_rescale_rnd(), fx_rescale_q_rnd()
 */
int64_t fx_rescale_q(int64_t a, AVRational bq, AVRational cq)
{
    return fx_rescale_q_rnd(a, bq, cq, FX_ROUND_NEAR_INF);
}


/**
 * Create an AVRational.
 *
 * Useful for compilers that do not support compound literals.
 *
 * @note The return value is not reduced.
 * @see fx_reduce()
 */
static inline AVRational fx_make_q(int num, int den)
{
    AVRational r = { num, den };
    return r;
}

/**
 * Compare two rationals.
 *
 * @param a First rational
 * @param b Second rational
 *
 * @return One of the following values:
 *         - 0 if `a == b`
 *         - 1 if `a > b`
 *         - -1 if `a < b`
 *         - `INT_MIN` if one of the values is of the form `0 / 0`
 */
static inline int fx_cmp_q(AVRational a, AVRational b){
    const int64_t tmp= a.num * (int64_t)b.den - b.num * (int64_t)a.den;

    if(tmp) return (int)((tmp ^ a.den ^ b.den)>>63)|1;
    else if(b.den && a.den) return 0;
    else if(a.num && b.num) return (a.num>>31) - (b.num>>31);
    else                    return INT_MIN;
}

/**
 * Convert an AVRational to a `double`.
 * @param a AVRational to convert
 * @return `a` in floating-point form
 * @see fx_d2q()
 */
static inline double fx_q2d(AVRational a){
    return a.num / (double) a.den;
}

#ifndef fx_ctz
#define fx_ctz fx_ctz_c
/**
 * Trailing zero bit count.
 *
 * @param v  input value. If v is 0, the result is undefined.
 * @return   the number of trailing 0-bits
 */
/* We use the De-Bruijn method outlined in:
 * http://supertech.csail.mit.edu/papers/debruijn.pdf. */
static int fx_ctz_c(int v)
{
    static const uint8_t debruijn_ctz32[32] = {
        0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
        31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
    };
    return debruijn_ctz32[(uint32_t)((v & -v) * 0x077CB531U) >> 27];
}
#endif

#ifndef fx_ctzll
#define fx_ctzll fx_ctzll_c
/* We use the De-Bruijn method outlined in:
 * http://supertech.csail.mit.edu/papers/debruijn.pdf. */
static int fx_ctzll_c(long long v)
{
    static const uint8_t debruijn_ctz64[64] = {
        0, 1, 2, 53, 3, 7, 54, 27, 4, 38, 41, 8, 34, 55, 48, 28,
        62, 5, 39, 46, 44, 42, 22, 9, 24, 35, 59, 56, 49, 18, 29, 11,
        63, 52, 6, 26, 37, 40, 33, 47, 61, 45, 43, 21, 23, 58, 17, 10,
        51, 25, 36, 32, 60, 20, 57, 16, 50, 31, 19, 15, 30, 14, 13, 12
    };
    return debruijn_ctz64[(uint64_t)((v & -v) * 0x022FDD63CC95386DU) >> 58];
}
#endif

#ifndef fx_clz
#define fx_clz fx_clz_c
static unsigned fx_clz_c(unsigned x)
{
    unsigned i = sizeof(x) * 8;

    while (x) {
        x >>= 1;
        i--;
    }

    return i;
}
#endif

/**
 * Compute the greatest common divisor of two integer operands.
 *
 * @param a,b Operands
 * @return GCD of a and b up to sign; if a >= 0 and b >= 0, return value is >= 0;
 * if a == 0 and b == 0, returns 0.
 */
/* Stein's binary GCD algorithm:
 * https://en.wikipedia.org/wiki/Binary_GCD_algorithm */
int64_t fx_gcd2(int64_t a, int64_t b) {
    int za, zb, k;
    int64_t u, v;
    if (a == 0)
        return b;
    if (b == 0)
        return a;
    za = fx_ctzll(a);
    zb = fx_ctzll(b);
    k  = MIN(za, zb);
    u = ABS(a >> za);
    v = ABS(b >> zb);
    while (u != v) {
        if (u > v)
            SWAP(int64_t, v, u);
        v -= u;
        v >>= fx_ctzll(v);
    }
    return (uint64_t)u << k;
}

/**
 * Reduce a fraction.
 *
 * This is useful for framerate calculations.
 *
 * @param[out] dst_num Destination numerator
 * @param[out] dst_den Destination denominator
 * @param[in]      num Source numerator
 * @param[in]      den Source denominator
 * @param[in]      max Maximum allowed values for `dst_num` & `dst_den`
 * @return 1 if the operation is exact, 0 otherwise
 */
int fx_reduce(int *dst_num, int *dst_den,
              int64_t num, int64_t den, int64_t max)
{
    AVRational a0 = { 0, 1 }, a1 = { 1, 0 };
    int sign = (num < 0) ^ (den < 0);
    int64_t gcd = fx_gcd2(ABS(num), ABS(den));

    if (gcd) {
        num = ABS(num) / gcd;
        den = ABS(den) / gcd;
    }
    if (num <= max && den <= max) {
        a1 = (AVRational) { num, den };
        den = 0;
    }

    while (den) {
        uint64_t x        = num / den;
        int64_t next_den  = num - den * x;
        int64_t a2n       = x * a1.num + a0.num;
        int64_t a2d       = x * a1.den + a0.den;

        if (a2n > max || a2d > max) {
            if (a1.num) x =          (max - a0.num) / a1.num;
            if (a1.den) x = MIN(x, (max - a0.den) / a1.den);

            if (den * (2 * x * a1.den + a0.den) > num * a1.den)
                a1 = (AVRational) { x * a1.num + a0.num, x * a1.den + a0.den };
            break;
        }

        a0  = a1;
        a1  = (AVRational) { a2n, a2d };
        num = den;
        den = next_den;
    }
    ASSERT(fx_gcd2(a1.num, a1.den) <= 1U);
    ASSERT(a1.num <= max && a1.den <= max);

    *dst_num = sign ? -a1.num : a1.num;
    *dst_den = a1.den;

    return den == 0;
}

/**
 * Multiply two rationals.
 * @param b First rational
 * @param c Second rational
 * @return b*c
 */
AVRational fx_mul_q(AVRational b, AVRational c)
{
    fx_reduce(&b.num, &b.den,
               b.num * (int64_t) c.num,
               b.den * (int64_t) c.den, INT_MAX);
    return b;
}

/**
 * Divide one rational by another.
 * @param b First rational
 * @param c Second rational
 * @return b/c
 */
AVRational fx_div_q(AVRational b, AVRational c)
{
    return fx_mul_q(b, (AVRational) { c.den, c.num });
}

/**
 * Add two rationals.
 * @param b First rational
 * @param c Second rational
 * @return b+c
 */
AVRational fx_add_q(AVRational b, AVRational c) {
    fx_reduce(&b.num, &b.den,
               b.num * (int64_t) c.den +
               c.num * (int64_t) b.den,
               b.den * (int64_t) c.den, INT_MAX);
    return b;
}

/**
 * Subtract one rational from another.
 * @param b First rational
 * @param c Second rational
 * @return b-c
 */

AVRational fx_sub_q(AVRational b, AVRational c)
{
    return fx_add_q(b, (AVRational) { -c.num, c.den });
}

/**
 * Invert a rational.
 * @param q value
 * @return 1 / q
 */
static AVRational fx_inv_q(AVRational q)
{
    AVRational r = { q.den, q.num };
    return r;
}

/**
 * Convert a double precision floating point number to a rational.
 *
 * In case of infinity, the returned value is expressed as `{1, 0}` or
 * `{-1, 0}` depending on the sign.
 *
 * @param d   `double` to convert
 * @param max Maximum allowed numerator and denominator
 * @return `d` in AVRational form
 * @see fx_q2d()
 */

AVRational fx_d2q(double d, int max)
{
    AVRational a;
    int exponent;
    int64_t den;
    if (isnan(d))
        return (AVRational) { 0,0 };
    if (fabs(d) > INT_MAX + 3LL)
        return (AVRational) { d < 0 ? -1 : 1, 0 };
    frexp(d, &exponent);
    exponent = MAX(exponent-1, 0);
    den = 1LL << (61 - exponent);
    // (int64_t)rint() and llrint() do not work with gcc on ia64 and sparc64,
    // see Ticket2713 for affected gcc/glibc versions
    fx_reduce(&a.num, &a.den, floor(d * den + 0.5), den, max);
    if ((!a.num || !a.den) && d && max>0 && max<INT_MAX)
        fx_reduce(&a.num, &a.den, floor(d * den + 0.5), den, INT_MAX);

    return a;
}

/**
 * Find which of the two rationals is closer to another rational.
 *
 * @param q     Rational to be compared against
 * @param q1,q2 Rationals to be tested
 * @return One of the following values:
 *         - 1 if `q1` is nearer to `q` than `q2`
 *         - -1 if `q2` is nearer to `q` than `q1`
 *         - 0 if they have the same distance
 */

int fx_nearer_q(AVRational q, AVRational q1, AVRational q2)
{
    /* n/d is q, a/b is the median between q1 and q2 */
    int64_t a = q1.num * (int64_t)q2.den + q2.num * (int64_t)q1.den;
    int64_t b = 2 * (int64_t)q1.den * q2.den;

    /* rnd_up(a*d/b) > n => a*d/b > n */
    int64_t x_up = fx_rescale_rnd(a, q.den, b, FX_ROUND_UP);

    /* rnd_down(a*d/b) < n => a*d/b < n */
    int64_t x_down = fx_rescale_rnd(a, q.den, b, FX_ROUND_DOWN);

    return ((x_up > q.num) - (x_down < q.num)) * fx_cmp_q(q2, q1);
}

/**
 * Find the value in a list of rationals nearest a given reference rational.
 *
 * @param q      Reference rational
 * @param q_list Array of rationals terminated by `{0, 0}`
 * @return Index of the nearest value found in the array
 */
int fx_find_nearest_q_idx(AVRational q, const AVRational* q_list)
{
    int i, nearest_q_idx = 0;
    for (i = 0; q_list[i].den; i++)
        if (fx_nearer_q(q, q_list[i], q_list[nearest_q_idx]) > 0)
            nearest_q_idx = i;

    return nearest_q_idx;
}

/**
 * Convert an AVRational to a IEEE 32-bit `float` expressed in fixed-point
 * format.
 *
 * @param q Rational to be converted
 * @return Equivalent floating-point value, expressed as an unsigned 32-bit
 *         integer.
 * @note The returned value is platform-indepedant.
 */

uint32_t fx_q2intfloat(AVRational q) {
    int64_t n;
    int shift;
    int sign = 0;

    if (q.den < 0) {
        q.den *= -1;
        q.num *= -1;
    }
    if (q.num < 0) {
        q.num *= -1;
        sign = 1;
    }

    if (!q.num && !q.den) return 0xFFC00000;
    if (!q.num) return 0;
    if (!q.den) return 0x7F800000 | (q.num & 0x80000000);

    shift = 23 + fx_log2(q.den) - fx_log2(q.num);
    if (shift >= 0) n = fx_rescale(q.num, 1LL<<shift, q.den);
    else            n = fx_rescale(q.num, 1, ((int64_t)q.den) << -shift);

    shift -= n >= (1<<24);
    shift += n <  (1<<23);

    if (shift >= 0) n = fx_rescale(q.num, 1LL<<shift, q.den);
    else            n = fx_rescale(q.num, 1, ((int64_t)q.den) << -shift);

    ASSERT(n <  (1<<24));
    ASSERT(n >= (1<<23));

    return sign<<31 | (150-shift)<<23 | (n - (1<<23));
}

/**
 * @}
 */

#endif /* AVUTIL_RATIONAL_H */
