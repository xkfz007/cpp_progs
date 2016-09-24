/*
 *  fx_mathops.c
 *
 *  Created on: 2016-05-21 2:25:41 pm
 *  Author: Felix
 */
#include "fx_types.h"

/*
 * greatest common divisor
 */
static inline FX_U64 fx_gcd( FX_U64 a, FX_U64 b )
{
    while( 1 )
    {
        FX_S64 c = a % b;
        if( !c )
            return b;
        a = b;
        b = c;
    }
}
/*
 * least common multiple
 */
static inline FX_U64 lcm( FX_U64 a, FX_U64 b )
{
    return ( a / gcd( a, b ) ) * b;
}


/****************************************************************************
 * fx_reduce_fraction:
void fx_reduce_fraction( FX_U32 *n, FX_U32 *d );
void fx_reduce_fraction64( FX_U64 *n, FX_U64 *d );
 ****************************************************************************/
#define REDUCE_FRACTION( name, type )\
void name( type *n, type *d )\
{                   \
    type a = *n;    \
    type b = *d;    \
    type c;         \
    if( !a || !b )  \
        return;     \
    c = a % b;      \
    while( c )      \
    {               \
        a = b;      \
        b = c;      \
        c = a % b;  \
    }               \
    *n /= b;        \
    *d /= b;        \
}

REDUCE_FRACTION( fx_reduce_fraction  , FX_U32 )
REDUCE_FRACTION( fx_reduce_fraction64, FX_U64 )



/*
 * calculate log2
 */
int fx_log2(int value)   
{
    int x=0;
    while(value>1)
    {
        value>>=1;
        x++;
    }
    return x;
}
/*
 * another way to get log2
 */
int fx_log2_fast(int x)
{
    float fx;
    unsigned long ix, exp;

    fx = (float)x;
    ix = *(unsigned long*)&fx;
    exp = (ix >> 23) & 0xFF;

    return exp - 127;
}



/* 
 * get the sign of input variable
 * x=0: return 0
 * x>0: x>>31 is 0x00000000, namely 0, -x is negative, after cast to unsigned int, the highest bit is 1, 
        then x>>31 is 0x00000001, namely 1, then after xor, result is 1
 * x<0: cause x is signed, x>>31 is 0xffffffff, namely -1, -x is positive, after cast to unsigned int, the highest bit is 0,
        then x>>31 is 0x00000000, namely 0, then after xor, result is -1
 *
 */
inline int fx_xsign (int x )
{
  return (( x >> 31) | (( int)( ((( FX_U32) - x)) >> 31)));
}
