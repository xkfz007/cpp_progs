#if 0
#include <stdio.h>
#include <stdlib.h>
int main()
{
	int *p=(int*)malloc(5*sizeof(int));

	int i;
	for(i=0;i<6;i++)
		p[i]=i;
	p[i]=99;
	for(i=0;i<7;i++)
		printf("[%p]:%d\n",p+i,p[i]);
	free(p);
	printf("The memory has been released.\n");
}
#endif
#if 0
#include <stdio.h>
#include <stdlib.h>
int f1();
int main() {
 _onexit(f1);
}
int f1() {
 printf("%d\n",main());
 return 0;
}

#endif
#if 0
int main(){
	int *a;
	int b[5];
	a=b;
	for(int i=0;i<5;i++)
		a[i]=i;

}
#endif
#if 0
#include <stdio.h>
int main()
{
    double a=3.01;
    double b=3.01;
    if(a==b)
        printf("%f==%f\n",a,b);
    else
        printf("%f!=%f\n",a,b);


}
#endif


#if 0
#include <cstdio>
typedef struct {
	int a;
	int b;
}tt;
#define ADD(a,b,c){\
	d=a+b;\
	d=d*c;\
}
int main()
{

	int a1=3,b1=4;
	int c1=3;
	int d;
	ADD(a1,b1,c1);
	printf("d=%d\n",d);
	tt k;
	k=(tt){a1,b1};
	printf("k.a=%d\n",k.a);
	printf("k.b=%d\n",k.b);
}

#endif

