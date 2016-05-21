/*
use the following formula to calculate square root of n
a(i+1)=(a(i)+n/a(i))/2
*/
//#define _CALC_SQRT
#ifdef _CALC_SQRT
#include <stdio.h>
#include <math.h>
int main(){
	int n=18;
	double r;
	double a;
	a=1.0;
	do 
	{
		r=a;
		a=(r+n/r)/2;
		printf("running:r=%f a=%f\n",r,a);
	} while (fabs(a-r)>1e-8);
	printf("sqrt(%d)=%f\n",n,sqrt(n));

}
#endif