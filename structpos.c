/*
 * this program is used to demenstrate the position of each MEMBER of the struct.
 */
#ifdef _STRUCTPOS
#include <stdio.h>
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