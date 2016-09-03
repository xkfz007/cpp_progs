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

//判断一个数是不是2的n次方
#define ISPOWER2(x) ((x > 0) && (0 == (x & (x - 1))))
/*
 * 利用按位操作来得到x和y中的较小的数：y^((x^y)&(-(x<y)))
 * 当x<y时，y^((x^y)&(-(x<y))) = y^((x^y)&(-1)) = y^((x^y)&0xffffffff) = y^(x^y) = (y^y)^x = 0^x = x
 * 当x>=y时，y^((x^y)&(-(x<y))) = y^((x^y)&(-0)) = y^((x^y)&0) = y^0 = y
 */
#define MIN(x,y) (y^((x^y)&(-(x<y))))

//三个数中求最大
#define max(a,b,c) ((a)>(b)?((a)>(c)?(a):(c)):((b)>(c)?(b):(c)))
#define MAX(a,b,c) ((a)>((b)>(c)?(b):(c))?(a):((b)>(c)?(b):(c)))
//宏实现SWAP交换
#define SWAP(x,y) ((x)==(y)?NULL:((x)^=(y),(y)^=(x),(x)^=(y)))
#define SWAP_(x,y) (temp=x,x=y,y=temp)//在使用前定义一个和x，y同类型的tmp临时变量

/****************************************************************************
 * Macros
 ****************************************************************************/
#define X264_MIN(a,b) ( (a)<(b) ? (a) : (b) )
#define X264_MAX(a,b) ( (a)>(b) ? (a) : (b) )
#define X264_MIN3(a,b,c) X264_MIN((a),X264_MIN((b),(c)))
#define X264_MAX3(a,b,c) X264_MAX((a),X264_MAX((b),(c)))
#define X264_MIN4(a,b,c,d) X264_MIN((a),X264_MIN3((b),(c),(d)))
#define X264_MAX4(a,b,c,d) X264_MAX((a),X264_MAX3((b),(c),(d)))
#define XCHG(type,a,b) do{ type t = a; a = b; b = t; } while(0)
#define FIX8(f) ((int)(f*(1<<8)+.5))
#define ALIGN(x,a) (((x)+((a)-1))&~((a)-1))
#define ARRAY_ELEMS(a) ((sizeof(a))/(sizeof(a[0])))

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
