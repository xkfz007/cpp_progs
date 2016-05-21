/*
 *    Description: 利用一个比较好的方法判断是否是素数：根据5以后的素数之间的间隔都是2或4
 */
#ifdef _PRIME
#include <stdio.h>
#include <stdlib.h>
int is_prime(int n)
{
    int i,step=2;
    if(n<=3)
        return 1;
    if(n%2==0)
        return 0;
    if(n%3==0)
        return 0;
    for(i=5;i*i<=n;i+=step)
    {
        if(n%i==0)
            return 0;
        step^=6;
    }
    return 1;
}
void help(){
    printf("Usage:prime n(n>1)\n");

}
int main(int argc,char*argv[])
{
    int n;
    if(argc<1)
    {
        help();
        exit(-1);
    }
    n=atoi(argv[1]);
    if(n<=1)
    {
        help();
        exit(-1);
    }
    if(is_prime(n))
        printf("YES\n");
    else
        printf("NO\n");
    return 0;
}
#endif