/*
 * how to calculate the time interval
 */
#ifdef _TIME_INTERVAL
#include <stdio.h>
#include <sys/timeb.h>
#include <stdlib.h>

#if defined(WIN32)
# define  TIMEB    _timeb
# define  ftime    _ftime
#else
#define TIMEB timeb
#endif

int time_interval()
{
    struct TIMEB ts1,ts2;
    time_t t_sec,ti;
    ftime(&ts1);//��ʼ��ʱ
    //do some work
    {
        int i;
        for(i=0;i<100000;i++)
        {
            int *p=malloc(10000);
            int *q=malloc(10000);
            int *s=malloc(10000);
            int *t=malloc(10000);
            free(p);
            free(q);
            free(s);
            free(t);
        }
    }

    ftime(&ts2);//ֹͣ��ʱ
    t_sec=ts2.time-ts1.time;//��������
    t_ms=ts2.millitm-ts1.millitm;//���������
    ti=t_sec*1000+t_ms;

    return ti;

}
int main()
{
    int ti=time_interval();
    printf("time interval=%d\n",ti);
}
#endif
/*
 * this demenstrates how to record the time.
 */
#ifdef _CLOCK_USAGE
#include <stdio.h>
#include <time.h>
int gcd(int x,int y)
{
    while(y!=0)
    {
        int temp=y;
        y=x%y;
        x=temp;
    }
    return x;
}
int main()
{
    clock_t start,finish;
    start=clock();
    srand((int)time(NULL));
    {
        int i;
        int a,b;
        int value;
        for(i=0;i<1000000;i++)
        {
            a=rand()%100000;
            b=rand()%100000;
            value=gcd(a,b);
//            printf("gcd(%d,%d)=%d\n",a,b,value);
        }
    }
    finish=clock();
    printf("it took %f seconds to excute\n",((double)(finish-start))/CLOCKS_PER_SEC);
}

#endif
/*
 *    Description:  ����linuxϵͳ���ý��к��뼶��ʱ
 */
#ifdef _LINUX_TIMING
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
int main()
{
    struct timeval tvafter,tvpre;
    struct timezone tz;
    int sum=0;
    int i;
    int interval;
    gettimeofday(&tvpre,&tz);
    for(i=0;i<1000000000;++i)
        sum+=i;
    gettimeofday(&tvafter,&tz);
    interval=(tvafter.tv_sec-tvpre.tv_sec)*1000+(tvafter.tv_usec-tvpre.tv_usec)/1000;
    printf("%d\n",interval);
    return EXIT_SUCCESS;
    
}
#endif