/*
 * this program contains the common bit-wise operations.
 */
#ifndef FX_MACROS_H
#define FX_MACROS_H
#include <limits.h>
#include <stdio.h>
#define MASK1(i) (1u<<i)
#define MASK0(i) ~(1u<<i)

#define SET(n,i)    ((n)|MASK1(i))
#define RESET(n,i)  ((n)&MASK0(i))
#define TOGGLE(n,i) ((n)^MASK1(i))
#define TEST(n,i)   !!((n)&MASK1(i))
#define NBITS(x)    (sizeof(x) * CHAR_BIT)

//circlar shift
#define ROTL(val,s,n) (((val)<<n)|((val)>>(sizeof(val)*s-n)))
#define ROTR(val,s,n) (((val)>>n)|((val)<<(sizeof(val)*s-n)))



#ifndef ASSERT
static void _assert(char*cond,char*filename,long lineno)
{
	fprintf(stdout,"assert:%s in file:%s, at line:%d\n",cond,filename,lineno);
}
#define ASSERT(cond) ((cond)?(void)0:_assert(#cond,__FILE__,__LINE__))
#endif


//#define FX_TOUPPER(ch) ((ch)+'A'-'a')
//#define FX_TOLOWER(ch) ((ch)+'a'-'A')

/*
 * get the position of each MEMBER of the struct.
 */
#ifndef OFFSETOF
#define OFFSETOF(type,f) ((int)((char*)&(((type*)0)->f)-(char*)(type*)0))
#endif
//#define OFFSET(type,f) ((int)&(((type*)0)->f)

//typedef struct
//{
//    int a;
//    float b;
//    char c;
//    double d;
//    int *pa;
//    char *pc;
//}Sta;
//
//int main()
//{
//    printf("a_=%d\n",OFFSET(Sta,a));
//    printf("b_=%d\n",OFFSET(Sta,b));
//    printf("c_=%d\n",OFFSET(Sta,c));
//    printf("d_=%d\n",OFFSET(Sta,d));
//    printf("pa_=%d\n",OFFSET(Sta,pa));
//    printf("pc_=%d\n",OFFSET(Sta,pc));
//    return 0;
//}

#define _GLUE(a, b) a ## b
#define GLUE(a, b) _GLUE(a, b)

#define MKTAG(a,b,c,d) ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))
#define MKBETAG(a,b,c,d) ((d) | ((c) << 8) | ((b) << 16) | ((unsigned)(a) << 24))
#define FFERRTAG(a, b, c, d) (-(int)MKTAG(a, b, c, d))

#define FF_ARRAY_ELEMS(a) (sizeof(a) / sizeof((a)[0]))
#endif

