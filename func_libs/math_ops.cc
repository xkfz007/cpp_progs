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



/*
 * calculate log2
 */
//一般可以采用不断除2的方式：
int log2(int value)   //非递归判断一个数是2的多少次方
{
    int x=0;
    while(value>1)
    {
        value>>=1;
        x++;
    }
    return x;
}
//另外一种比较巧的方式就是利用计算机中对浮点数的表示方式：
int log2_fast(int x)
{
    float fx;
    unsigned long ix, exp;

    fx = (float)x;
    ix = *(unsigned long*)&fx;
    exp = (ix >> 23) & 0xFF;

    return exp - 127;
}



/** get the sign of input variable
 * 当x等于0时，返回0；
 * 当x大于0时，x>>31后结果为0x00000000，即0，而-x则为负数，然后经过强制转换为无符号整数（UInt）之后，最高位为1，执行右移31位之后，结果为0x00000001，然后再转换为有符号整数之后即为1，这样最后结果为1。
 * 当x小于0时，由于x是int型，x>>31（算术右移）后结果为0xffffffff，即-1, 而-x则为正数，然后经过强制转换为无符号整数（UInt）之后，最高位为0，同样执行右移31位之后，结果为0，转换为有符号整数之后仍为0，这样最后结果为-1
 *
 */
inline int xSign (int x )
{
  return (( x >> 31) | (( int)( ((( uint32_t) - x)) >> 31)));
}
