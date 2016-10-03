/*
 * arbitrary precision integers
 * Copyright (c) 2004 Michael Niedermayer <michaelni@gmx.at>
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
 * arbitrary precision integers
 * @author Michael Niedermayer <michaelni@gmx.at>
 */
//#define _INTEGER
#ifdef _INTEGER


#include "fx_macros.h"
#include "fx_integer.h"
#include "fx_mathops.h"

int main(void){
    int64_t a,b;

    for(a=7; a<256*256*256; a+=13215){
        for(b=3; b<256*256*256; b+=27118){
            AVInteger ai= fx_int2i(a);
            AVInteger bi= fx_int2i(b);

            ASSERT(fx_i2int(ai) == a);
            ASSERT(fx_i2int(bi) == b);
            ASSERT(fx_i2int(fx_add_i(ai,bi)) == a+b);
            ASSERT(fx_i2int(fx_sub_i(ai,bi)) == a-b);
            ASSERT(fx_i2int(fx_mul_i(ai,bi)) == a*b);
            ASSERT(fx_i2int(fx_shr_i(ai, 9)) == a>>9);
            ASSERT(fx_i2int(fx_shr_i(ai,-9)) == a<<9);
            ASSERT(fx_i2int(fx_shr_i(ai, 17)) == a>>17);
            ASSERT(fx_i2int(fx_shr_i(ai,-17)) == a<<17);
            ASSERT(fx_log2_i(ai) == fx_log2(a));
            ASSERT(fx_i2int(fx_div_i(ai,bi)) == a/b);
        }
    }
    return 0;
}
#endif
