#include <stdio.h>

#ifdef INT_SIZE_TEST
#define PRINT_SIZE(x) printf("sizeof("#x")=%d\n",sizeof(x))
int main()
{
    //int *a[]={0,1,2,3};
    //char *b[]={0,1,2,3};
    //printf("==============\n");
    //printf("a[0]=%d\n",*a+0);
    //printf("a[1]=%d\n",*a+1);
    //printf("a[2]=%d\n",*a+2);
    //printf("a[3]=%d\n",*a+3);
    //printf("==============\n");
    //printf("b[0]=%d\n",*b+0);
    //printf("b[1]=%d\n",*b+1);
    //printf("b[2]=%d\n",*b+2);
    //printf("b[3]=%d\n",*b+3);
	long long a=100000000000;
	double b=0.75;
	long long c=a*b;
	printf("c=%lld\n",c);
	PRINT_SIZE(short);
	PRINT_SIZE(int);
	PRINT_SIZE(long);
	PRINT_SIZE(long long);
	PRINT_SIZE(long double);
	PRINT_SIZE(double);
	

}
#endif

#ifdef MACRO_TEST
#define STRCPY(a,  b) strcpy(a ## _p, #b)
int main(){
    char var1_p[20],var2_p[20];
    char filename[20]=__FILE__;
    strcpy(var1_p,"aaaa");
    strcpy(var2_p,"bbbb");
    STRCPY(var1,var2);
    STRCPY(var2,var1);
    printf("var1= %s\n",var1_p);
    printf("var2= %s\n",var2_p);
    printf("FILE= %s\n",__FILE__);
    printf("FILE2= %s\n",filename);
    
    return 0;
}
#endif
#if 0

int func(int i,int N){
    return (i==N&&printf("%d\n",i))||printf("%d\n",i)&&func(i+1,N);
}
int main(){
    func(1,5);
    return 0;
}
#endif
#if 0
void bar(void){

}
void foo(int *x)
{
  int y = *x;  /* (1) */
  if(!x)       /* (2) */
  {
    return;    /* (3) */
  }
  bar();
  return;
}
int main(){
    foo(0);

}
#endif
#if 0
int main()
{
    signed int n1 = 1;
    signed int n2 = 2;
    unsigned int u1 = 1;
    unsigned int u2 = 2;

    signed int a = n1 + n2;
    unsigned int ua = u1 + u2;

    signed int s = n1 - n2;
    unsigned int us = u1 - u2;

    signed int m = n1 * n2;
    unsigned int um = u1 * u2;

    signed int d = n1 / n2;
    unsigned int ud = u1 / u2;

    signed int t = n1 % n2;
    unsigned int ut = u1 % u2;

    signed int t2=u1%u2;
    unsigned int ut2=n1%n2;
    
    signed int t3=n1%u2;
    unsigned int ut3=u1%n2;

    return 0;
}

#endif