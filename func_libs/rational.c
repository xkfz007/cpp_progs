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
 * rational numbers
 * @author Michael Niedermayer <michaelni@gmx.at>
 */
//#define _RATIONAL
#ifdef _RATIONAL
#include "fx_rational.h"
#include "fx_log.h"
#include "fx_integer.h"
#include "fx_image.h"
int main(void)
{
    AVRational a,b,r;
    int i,j,k;
    static const int64_t numlist[] = {
        INT64_MIN, INT64_MIN+1, INT64_MAX, INT32_MIN, INT32_MAX, 1,0,-1,
        123456789, INT32_MAX-1, INT32_MAX+1LL, UINT32_MAX-1, UINT32_MAX, UINT32_MAX+1LL
    };

    for (a.num = -2; a.num <= 2; a.num++) {
        for (a.den = -2; a.den <= 2; a.den++) {
            for (b.num = -2; b.num <= 2; b.num++) {
                for (b.den = -2; b.den <= 2; b.den++) {
                    int c = fx_cmp_q(a,b);
                    double d = fx_q2d(a) == fx_q2d(b) ?
                               0 : (fx_q2d(a) - fx_q2d(b));
                    if (d > 0)       d = 1;
                    else if (d < 0)  d = -1;
                    else if (d != d) d = INT_MIN;
                    if (c != d)
                        fx_log(NULL, FX_LOG_ERROR, "%d/%d %d/%d, %d %f\n", a.num,
                               a.den, b.num, b.den, c,d);
                    r = fx_sub_q(fx_add_q(b,a), b);
                    if(b.den && (r.num*a.den != a.num*r.den || !r.num != !a.num || !r.den != !a.den))
                        fx_log(NULL, FX_LOG_ERROR, "%d/%d ", r.num, r.den);
                }
            }
        }
    }

    for (i = 0; i < ARRAY_ELEMS(numlist); i++) {
        int64_t a = numlist[i];

        for (j = 0; j < ARRAY_ELEMS(numlist); j++) {
            int64_t b = numlist[j];
            if (b<=0)
                continue;
            for (k = 0; k < ARRAY_ELEMS(numlist); k++) {
                int64_t c = numlist[k];
                int64_t res;
                AVInteger ai;

                if (c<=0)
                    continue;
                res = fx_rescale_rnd(a,b,c, FX_ROUND_ZERO);

                ai = fx_mul_i(fx_int2i(a), fx_int2i(b));
                ai = fx_div_i(ai, fx_int2i(c));

                if (fx_cmp_i(ai, fx_int2i(INT64_MAX)) > 0 && res == INT64_MIN)
                    continue;
                if (fx_cmp_i(ai, fx_int2i(INT64_MIN)) < 0 && res == INT64_MIN)
                    continue;
                if (fx_cmp_i(ai, fx_int2i(res)) == 0)
                    continue;

                // Special exception for INT64_MIN, remove this in case INT64_MIN is handled without off by 1 error
                if (fx_cmp_i(ai, fx_int2i(res-1)) == 0 && a == INT64_MIN)
                    continue;

                fx_log(NULL, FX_LOG_ERROR, "%"PRId64" * %"PRId64" / %"PRId64" = %"PRId64" or %"PRId64"\n", a,b,c, res, fx_i2int(ai));
            }
        }
    }

    for (a.num = 1; a.num <= 10; a.num++) {
        for (a.den = 1; a.den <= 10; a.den++) {
            if (fx_gcd(a.num, a.den) > 1)
                continue;
            for (b.num = 1; b.num <= 10; b.num++) {
                for (b.den = 1; b.den <= 10; b.den++) {
                    int start;
                    if (fx_gcd(b.num, b.den) > 1)
                        continue;
                    if (fx_cmp_q(b, a) < 0)
                        continue;
                    for (start = 0; start < 10 ; start++) {
                        int acc= start;
                        int i;

                        for (i = 0; i<100; i++) {
                            int exact = start + fx_rescale_q(i+1, b, a);
                            acc = fx_add_stable(a, acc, b, 1);
                            if (ABS(acc - exact) > 2) {
                                fx_log(NULL, FX_LOG_ERROR, "%d/%d %d/%d, %d %d\n", a.num,
                                       a.den, b.num, b.den, acc, exact);
                                return 1;
                            }
                        }
                    }
                }
            }
        }
    }

    for (a.den = 1; a.den < 0x100000000U/3; a.den*=3) {
        for (a.num = -1; a.num < (1<<27); a.num += 1 + a.num/100) {
            float f  = fx_int2float(fx_q2intfloat(a));
            float f2 = fx_q2d(a);
            if (fabs(f - f2) > fabs(f)/5000000) {
                fx_log(NULL, FX_LOG_ERROR, "%d/%d %f %f\n", a.num,
                       a.den, f, f2);
                return 1;
            }

        }
    }

    return 0;
}


#endif