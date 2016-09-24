/*
 * this program contains the common bit-wise operations.
 */
#ifndef FX_MACROS_H
#define FX_MACROS_H
#include <limits.h>
#define FX_MASK1(i) (1u<<i)
#define FX_MASK0(i) ~(1u<<i)

#define FX_SET(n,i)    ((n)|FX_MASK1(i))
#define FX_RESET(n,i)  ((n)&FX_MASK0(i))
#define FX_TOGGLE(n,i) ((n)^FX_MASK1(i))
#define FX_TEST(n,i)   !!((n)&FX_MASK1(i))
#define FX_NBITS(x)    (sizeof(x) * CHAR_BIT)

//circlar shift
#define FX_ROTL(val,s,n) (((val)<<n)|((val)>>(sizeof(val)*s-n)))
#define FX_ROTR(val,s,n) (((val)>>n)|((val)<<(sizeof(val)*s-n)))

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
void _assert(char*cond,char*filename,long lineno)
{
	fprintf(stdout,"assert:%s in file:%s, at line:%d\n",cond,filename,lineno);
}
#define FX_ASSERT(cond) ((cond)?(void)0:_assert(#cond,__FILE__,__LINE__))


//#define FX_TOUPPER(ch) ((ch)+'A'-'a')
//#define FX_TOLOWER(ch) ((ch)+'a'-'A')

/*
 * get the position of each MEMBER of the struct.
 */
#define FX_OFFSETOF(type,f) ((int)((char*)&(((type*)0)->f)-(char*)(type*)0))
#define FX_OFFSET(type,f) ((int)&(((type*)0)->f)

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

