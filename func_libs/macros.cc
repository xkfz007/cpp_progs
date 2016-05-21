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

//位设置
#define SET(n,i) ((n)|(1u<<i)) //置1
#define RESET(n,i) ((n)&~(1u<<i)) //复位
#define TOGGLE(n,i) ((n)^(1u<<i)) //取反
#define TEST(n,i) !!((n)&(1u<<i)) //测试
//循环移位
#define ROTL(val,n) (((val)<<n)|((val)>>(sizeof(val)*8-n)))
#define ROTR(val,n) (((val)>>n)|((val)<<(sizeof(val)*8-n)))
//异或（XOR）逻辑运算
#define XOR(a,b) (((a)||(b))&&!((a)&&(b)))



//count '1' in number n
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
//assert断言
void _assert(char*cond,char*filename,long lineno)
{
printf("assert:%s in file:%s, at line:%d\n",cond,filename,lineno);
}
#define assert(cond) ((cond)?(void)0:_assert(#cond,__FILE__,__LINE__))


// 大小写转换
#define toupper(ch) ((ch)+'A'-'a')
#define tolower(ch) ((ch)+'a'-'A')

//三个数中求最大
#define max(a,b,c) ((a)>(b)?((a)>(c)?(a):(c)):((b)>(c)?(b):(c)))
#define MAX(a,b,c) ((a)>((b)>(c)?(b):(c))?(a):((b)>(c)?(b):(c)))
//宏实现SWAP交换
#define SWAP(x,y) ((x)==(y)?NULL:((x)^=(y),(y)^=(x),(x)^=(y)))
#define SWAP_(x,y) (temp=x,x=y,y=temp)//在使用前定义一个和x，y同类型的tmp临时变量


/*
 * this program is used to demenstrate the position of each MEMBER of the struct.
 */
#ifdef _STRUCTPOS
#include <stdio.h>
//获得结构体中域的偏移量
#define offsetof(type,f) ((int)((char*)&(((type*)0)->f)-(char*)(type*)0))
#define OFFSETOF(type,f) ((int)&(((type*)0)->f)
#define OFFSET(TYPE,MEMBER)  ((int)(&(((TYPE*)0)->MEMBER)))
typedef struct
{
    int a;
    float b;
    char c;
    double d;
    int *pa;
    char *pc;
}Sta;
int main()
{
    printf("a_=%d\n",OFFSET(Sta,a));
    printf("b_=%d\n",OFFSET(Sta,b));
    printf("c_=%d\n",OFFSET(Sta,c));
    printf("d_=%d\n",OFFSET(Sta,d));
    printf("pa_=%d\n",OFFSET(Sta,pa));
    printf("pc_=%d\n",OFFSET(Sta,pc));
    return 0;
}
#endif

#endif

