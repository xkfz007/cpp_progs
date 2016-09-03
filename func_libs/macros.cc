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
//#define ROTATE_LEFT(x, s, n) ((x) << (n)) | ((x) >> ((s) - (n)))
//#define ROTATE_RIGHT(x, s, n) ((x) >> (n)) | ((x) << ((s) - (n)))
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
//内存对齐是编译器为了便于CPU快速访问而采用的一项技术，对于不同的编译器有不同的处理方法。
//Win32平台下的微软VC编译器在默认情况下采用如下的对齐规则: 任何基本数据类型T的对齐模数就是T的大小，即sizeof(T)。
//比如对于double类型(8字节)，就要求该类型数据的地址总是8的倍数，而char类型数据(1字节)则可以从任何一个地址开始。
//Linux下的GCC奉行的是另外一套规则:任何2字节大小(包括单字节吗?)的数据类型(比如short)的对齐模数是2，而其它所有
//超过2字节的数据类型(比如long,double)都以4为对齐模数。
//主要是对于double类型，VC采用的是8对齐，而gcc采用的是4对齐

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

