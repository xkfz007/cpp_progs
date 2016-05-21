#ifdef _SQRT
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sys/timeb.h>
#define TIMEB timeb
#define ABS(x) ((x)>0?(x):-(x))
#define eps 0.0000000001
time_t time_test(double x,double *y,double(*func)(double))
{
    time_t ltime1, ltime2, tmp_time;
    struct TIMEB tstruct1, tstruct2;

    ftime (&tstruct1);            // start time ms
    time (&ltime1);               // start time s
    *y=func(x);
    time (&ltime2);               // end time sec
    ftime (&tstruct2);            // end time ms

    tmp_time = (ltime2 * 1000 + tstruct2.millitm) - (ltime1 * 1000 + tstruct1.millitm);
    return tmp_time;
}

/*
 * Newton's algorithm
 * use the following formula to calculate square root of n:
 * a(i+1)=(a(i)+n/a(i))/2
*/
double sqrt1(double x)
{
    double g=x;
    while(ABS(g*g-x)>eps)
    {
        g=(g+x/g)/2;
    }
    return g;
}
/*
 * Returns the square root of x, calculated by expanding
 * the Taylor series around a = 1, as described in the
 * text.  The function is effective only if x is in the
 * range 0 < x < 2.  Term i in the summation has the form
 *
 *                   xpower
 *       coeff  *  ---------
 *                 factorial
 *
 * where coeff comes from ith derivative of the function,
 * factorial is i!, and xpower is the ith power of (x - a).
 * Each of these components is computed from its previous
 * value.
*/
double Tsqrt(double x)
{
    double sum,coffe,factorial,xpower,term;
    int i;
    sum=0;
    coffe=1;
    factorial=1;
    xpower=1;
    term=1;
    i=0;
    while(ABS(term)>eps)
    {
        sum+=term;
        coffe*=(0.5-i);
        factorial*=(i+1);
        xpower*=(x-1);
        term=coffe*xpower/factorial;
        i++;
    }
    return sum;

}
/*
 * Returns the square root of x, calculated using a
 * Taylor series expansion, as described in the text.
 * The Sqrt function is actually implemented as two
 * functions.  The job of the outer Sqrt function is to
 * divide the argument repeatedly by 4 until it is in
 * the range 0 < x < 2, where the Taylor series converges.
 * It then calls TSqrt to perform the actual Taylor series
 * calculation.  When finished, Sqrt adjusts the answer by
 * multiplying the result by 2 for each time it needed to
 * be divided by 4 to bring it in range.
 */
double sqrt2(double x)
{
    double correction=1;
    while(x>=2)
    {
        x/=4;
        correction*=2;
    }
    return Tsqrt(x)*correction;
}

double sqrt3(double xi) {
    long i;
    float number=(float)xi;
    float x, y;
    const float f = 1.5F;
    x = number * 0.5F;
    y = number;
    i = * ( long * ) &y;
    i = 0x5f3759df - ( i >> 1 ); //注意这一行 
    y = * ( float* ) &i;
    y = y* ( f - ( x * y * y ) );
    y = y * (f - ( x * y * y ));               
    return number * y;
}
int main(){
    int x;
    double y,y1,y2,y3;
    int i;
    time_t t,t1,t2,t3;
    srand((int)time(NULL));
    for(i=0;i<20;i++)
    {
        x=rand()%1000;
        t=time_test(x,&y,sqrt);
        t1=time_test(x,&y1,sqrt1);
        t2=time_test(x,&y2,sqrt2);
        t3=time_test(x*1.0,&y3,sqrt3);

        printf("x=%3d y=%7.4f<%2d> y1=%7.4f<%2d> y2=%7.4f<%2d> y3=%7.4f<%2d>\n",x,y,t,y1,t1,y2,t2,y3,t3);
    }

    return 0;
}
#endif
