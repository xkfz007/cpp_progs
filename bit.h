/*
 * this program contains the common bit-wise operations.
 */
#ifndef BIT_H
#define BIT_H
#include <limits.h>
#define mask1(i) (1u<<i)
#define mask0(i) ~(1u<<i)

#define set(n,i) ((n)|mask1(i))
#define reset(n,i) ((n)&mask0(i))
#define toggle(n,i) ((n)^mask1(i))
#define test(n,i) !!((n)&maks1(i))
#define nbits(x) (sizeof(x) * CHAR_BIT)
static unsigned count(unsigned n)
{
    unsigned sum=0;
    while(n)
    {
        if(n&1u)
            ++sum;
        n>>=1;
    }
    return sum;
}

#endif

