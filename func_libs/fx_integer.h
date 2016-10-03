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

#ifndef AVUTIL_INTEGER_H
#define AVUTIL_INTEGER_H

#include <string.h>
//#include "common.h"
#include "fx_types.h"
#include "fx_mathops.h"
#include "fx_macros.h"

#define FX_INTEGER_SIZE 8

typedef struct AVInteger{
    uint16_t v[FX_INTEGER_SIZE];
} AVInteger;

static const AVInteger zero_i;

AVInteger fx_add_i(AVInteger a, AVInteger b){
    int i, carry=0;

    for(i=0; i<FX_INTEGER_SIZE; i++){
        carry= (carry>>16) + a.v[i] + b.v[i];
        a.v[i]= carry;
    }
    return a;
}
AVInteger fx_sub_i(AVInteger a, AVInteger b){
    int i, carry=0;

    for(i=0; i<FX_INTEGER_SIZE; i++){
        carry= (carry>>16) + a.v[i] - b.v[i];
        a.v[i]= carry;
    }
    return a;
}

/**
 * Return the rounded-down value of the base 2 logarithm of the given
 * AVInteger. This is simply the index of the most significant bit
 * which is 1, or 0 if all bits are 0.
 */
int fx_log2_i(AVInteger a){
    int i;

    for(i=FX_INTEGER_SIZE-1; i>=0; i--){
        if(a.v[i])
            return fx_log2_16bit(a.v[i]) + 16*i;
    }
    return -1;
}
AVInteger fx_mul_i(AVInteger a, AVInteger b){
    AVInteger out;
    int i, j;
    int na= (fx_log2_i(a)+16) >> 4;
    int nb= (fx_log2_i(b)+16) >> 4;

    memset(&out, 0, sizeof(out));

    for(i=0; i<na; i++){
        unsigned int carry=0;

        if(a.v[i])
            for(j=i; j<FX_INTEGER_SIZE && j-i<=nb; j++){
                carry= (carry>>16) + out.v[j] + a.v[i]*b.v[j-i];
                out.v[j]= carry;
            }
    }

    return out;
}

/**
 * Return 0 if a==b, 1 if a>b and -1 if a<b.
 */
int fx_cmp_i(AVInteger a, AVInteger b){
    int i;
    int v= (int16_t)a.v[FX_INTEGER_SIZE-1] - (int16_t)b.v[FX_INTEGER_SIZE-1];
    if(v) return (v>>16)|1;

    for(i=FX_INTEGER_SIZE-2; i>=0; i--){
        int v= a.v[i] - b.v[i];
        if(v) return (v>>16)|1;
    }
    return 0;
}

/**
 * bitwise shift
 * @param s the number of bits by which the value should be shifted right,
            may be negative for shifting left
 */

AVInteger fx_shr_i(AVInteger a, int s){
    AVInteger out;
    int i;

    for(i=0; i<FX_INTEGER_SIZE; i++){
        unsigned int index= i + (s>>4);
        unsigned int v=0;
        if(index+1<FX_INTEGER_SIZE) v = a.v[index+1]<<16;
        if(index  <FX_INTEGER_SIZE) v+= a.v[index  ];
        out.v[i]= v >> (s&15);
    }
    return out;
}

/**
 * Return a % b.
 * @param quot a/b will be stored here.
 */

AVInteger fx_mod_i(AVInteger *quot, AVInteger a, AVInteger b){
    int i= fx_log2_i(a) - fx_log2_i(b);
    AVInteger quot_temp;
    if(!quot) quot = &quot_temp;

    if ((int16_t)a.v[FX_INTEGER_SIZE-1] < 0) {
        a = fx_mod_i(quot, fx_sub_i(zero_i, a), b);
        *quot = fx_sub_i(zero_i, *quot);
        return fx_sub_i(zero_i, a);
    }

    ASSERT((int16_t)a.v[FX_INTEGER_SIZE-1] >= 0 && (int16_t)b.v[FX_INTEGER_SIZE-1] >= 0);
    ASSERT(fx_log2_i(b)>=0);

    if(i > 0)
        b= fx_shr_i(b, -i);

    memset(quot, 0, sizeof(AVInteger));

    while(i-- >= 0){
        *quot= fx_shr_i(*quot, -1);
        if(fx_cmp_i(a, b) >= 0){
            a= fx_sub_i(a, b);
            quot->v[0] += 1;
        }
        b= fx_shr_i(b, 1);
    }
    return a;
}

/**
 * Return a/b.
 */
AVInteger fx_div_i(AVInteger a, AVInteger b){
    AVInteger quot;
    fx_mod_i(&quot, a, b);
    return quot;
}

/**
 * Convert the given int64_t to an AVInteger.
 */
AVInteger fx_int2i(int64_t a){
    AVInteger out;
    int i;

    for(i=0; i<FX_INTEGER_SIZE; i++){
        out.v[i]= a;
        a>>=16;
    }
    return out;
}

/**
 * Convert the given AVInteger to an int64_t.
 * If the AVInteger is too large to fit into an int64_t,
 * then only the least significant 64 bits will be used.
 */
int64_t fx_i2int(AVInteger a){
    int i;
    int64_t out=(int8_t)a.v[FX_INTEGER_SIZE-1];

    for(i= FX_INTEGER_SIZE-2; i>=0; i--){
        out = (out<<16) + a.v[i];
    }
    return out;
}

#endif /* AVUTIL_INTEGER_H */
