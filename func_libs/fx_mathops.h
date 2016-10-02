#ifndef _FX_MATHOPS_H
#define _FX_MATHOPS_H
#include "fx_types.h"

//decide if x is power of 2
#ifndef ISPOWER2
#define ISPOWER2(x) ((x > 0) && (0 == (x & (x - 1))))
#endif
/*
 * get min of two numbers using bitwise ops
 * y^((x^y)&(-(x<y)))
 * x<y:  y^((x^y)&(-(x<y))) = y^((x^y)&(-1)) = y^((x^y)&0xffffffff) = y^(x^y) = (y^y)^x = 0^x = x
 * x>=y: y^((x^y)&(-(x<y))) = y^((x^y)&(-0)) = y^((x^y)&0) = y^0 = y
 */
#ifndef MIN_BW
#define MIN_BW(x,y) (y^((x^y)&(-(x<y))))
#endif
#ifndef SWAP_BW
#define SWAP_BW(x,y) ((x)==(y)?NULL:((x)^=(y),(y)^=(x),(x)^=(y)))
#endif

#ifndef MIN
#define MIN(a,b) ( (a)<(b) ? (a) : (b) )
#endif
#ifndef MAX
#define MAX(a,b) ( (a)>(b) ? (a) : (b) )
#endif
#ifndef MIN3
#define MIN3(a,b,c) MIN((a),MIN((b),(c)))
#endif
#ifndef MAX3
#define MAX3(a,b,c) MAX((a),MAX((b),(c)))
#endif
#ifndef MIN4
#define MIN4(a,b,c,d) MIN((a),MIN3((b),(c),(d)))
#endif
#ifndef MAX4
#define MAX4(a,b,c,d) MAX((a),MAX3((b),(c),(d)))
#endif

#ifndef ABS
#define ABS(X) ((X)>0?(X):-(X))
#endif


#ifndef XCHG
#define XCHG(type,a,b) do{ type t = a; a = b; b = t; } while(0)
#endif
#ifndef FIX8
#define FIX8(f) ((int)(f*(1<<8)+.5))
#endif
#ifndef ALIGN
#define ALIGN(x,a) (((x)+((a)-1))&~((a)-1))
#endif
#ifndef ARRAY_ELEMS
#define ARRAY_ELEMS(a) ((sizeof(a))/(sizeof(a[0])))
#endif

#ifndef LOG2F
#define LOG2F(x) (logf(x)/0.693147180559945f)
#endif
#ifndef LOG2
#define LOG2(x) (log(x)/0.693147180559945)
#endif

#ifndef M_E
#define M_E            2.7182818284590452354   /* e */
#endif
#ifndef M_LN2
#define M_LN2          0.69314718055994530942  /* log_e 2 */
#endif
#ifndef M_LN10
#define M_LN10         2.30258509299404568402  /* log_e 10 */
#endif
#ifndef M_LOG2_10
#define M_LOG2_10      3.32192809488736234787  /* log_2 10 */
#endif
#ifndef M_PHI
#define M_PHI          1.61803398874989484820   /* phi / golden ratio */
#endif
#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif

#ifndef NAN
#define NAN            (float)(0x7fc00000)
#endif
#ifndef INFINITY
#define INFINITY       (float)(0x7f800000)
#endif

#define UINT64_MAX      ((uint64_t)(0xffffffffffffffff))
/*
 * greatest common divisor
 */
static int64_t fx_gcd( int64_t a, int64_t b )
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
/*
 * least common multiple
 */
static int64_t fx_lcm( int64_t a, int64_t b )
{
    return ( a / fx_gcd( a, b ) ) * b;
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

REDUCE_FRACTION( fx_reduce_fraction  , uint32_t )
REDUCE_FRACTION( fx_reduce_fraction64, uint64_t )



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
static int fx_xsign (int x )
{
  return (( x >> 31) | (( int)( ((( uint32_t) - x)) >> 31)));
}


#define FX_CLIP(name, type)\
static int name(type a, type amin, type amax)\
{\
    if      (a < amin) return amin;\
    else if (a > amax) return amax;\
    else               return a;\
}
FX_CLIP(fx_clip,int)
FX_CLIP(fx_clipd,double)

static int fx_median( int a, int b, int c )
{
    int t = (a-b)&((a-b)>>31);
    a -= t;
    b += t;
    b -= (b-c)&((b-c)>>31);
    b += (a-b)&((a-b)>>31);
    return b;
}

static double fx_trunc(double x)
{
    return (x > 0) ? floor(x) : ceil(x);
}

#endif