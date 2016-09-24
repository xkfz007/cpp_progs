#ifndef _FX_MATHOPS_H
#define _FX_MATHOPS_H

//decide if x is power of 2
#define FX_ISPOWER2(x) ((x > 0) && (0 == (x & (x - 1))))
/*
 * get min of two numbers using bitwise ops
 * y^((x^y)&(-(x<y)))
 * x<y:  y^((x^y)&(-(x<y))) = y^((x^y)&(-1)) = y^((x^y)&0xffffffff) = y^(x^y) = (y^y)^x = 0^x = x
 * x>=y: y^((x^y)&(-(x<y))) = y^((x^y)&(-0)) = y^((x^y)&0) = y^0 = y
 */
#define FX_MIN_BW(x,y) (y^((x^y)&(-(x<y))))
#define FX_SWAP_BW(x,y) ((x)==(y)?NULL:((x)^=(y),(y)^=(x),(x)^=(y)))

#define FX_MIN(a,b) ( (a)<(b) ? (a) : (b) )
#define FX_MAX(a,b) ( (a)>(b) ? (a) : (b) )
#define FX_MIN3(a,b,c) FX_MIN((a),FX_MIN((b),(c)))
#define FX_MAX3(a,b,c) FX_MAX((a),FX_MAX((b),(c)))
#define FX_MIN4(a,b,c,d) FX_MIN((a),FX_MIN3((b),(c),(d)))
#define FX_MAX4(a,b,c,d) FX_MAX((a),FX_MAX3((b),(c),(d)))

#define FX_XCHG(type,a,b) do{ type t = a; a = b; b = t; } while(0)
#define FX_FIX8(f) ((int)(f*(1<<8)+.5))
#define FX_ALIGN(x,a) (((x)+((a)-1))&~((a)-1))
#define FX_ARRAY_ELEMS(a) ((sizeof(a))/(sizeof(a[0])))

#define LOG2F(x) (logf(x)/0.693147180559945f)
#define LOG2(x) (log(x)/0.693147180559945)


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
static void name( type *n, type *d )\
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
static int fx_log2(int value)   
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
static int fx_log2_fast(int x)
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
static inline int fx_xsign (int x )
{
  return (( x >> 31) | (( int)( ((( FX_U32) - x)) >> 31)));
}




#endif