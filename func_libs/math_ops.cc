/*
 * math_ops.cc
 *
 *  Created on: 2016-5月-21 下午2:25:41
 *      Author: Felix
 */



//greatest common divisor
static inline uint64_t gcd( uint64_t a, uint64_t b )
{
    while( 1 )
    {
        int64_t c = a % b;
        if( !c )
            return b;
        a = b;
        b = c;
    }
}

//least common multiple
static inline uint64_t lcm( uint64_t a, uint64_t b )
{
    return ( a / gcd( a, b ) ) * b;
}


/****************************************************************************
 * x264_reduce_fraction:
void x264_reduce_fraction( uint32_t *n, uint32_t *d );
void x264_reduce_fraction64( uint64_t *n, uint64_t *d );
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

REDUCE_FRACTION( x264_reduce_fraction  , uint32_t )
REDUCE_FRACTION( x264_reduce_fraction64, uint64_t )

