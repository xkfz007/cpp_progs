/*
 * this program contains the common bit-wise operations.
 */
#ifndef FX_MACROS_H
#define FX_MACROS_H
#include <limits.h>
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

//count '1' in number n
static unsigned fx_count_one(unsigned n)
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
#endif

#endif

