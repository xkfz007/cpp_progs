/*
 *    Description:  根据点的坐标来计算角度 
 */

#ifdef _DEGREE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define PI 3.1415926
int main(int argc,char*argv[])
{
    int y,x;
    double dge;
    double ddd;
    double ddd2;
    if(argc<3)
    {
        printf("wrong\n");
        exit(-1);
    }
    y=atoi(argv[1]);
    x=atoi(argv[2]);
    if(x==0&&y==0)
    {
        printf("bad input \n");
        exit(1);
    }
    if(x==0&&y>0)
       ddd=PI/2;
    else if(x==0&&y<0)
       ddd=3*PI/2;
    else if(y==0&&x>0)
       ddd=0;
    else if(y==0&&x<0)
       ddd=PI;
    else
    {
    dge=((double)y)/x;
    printf("dge=%f\n",dge);
    ddd=atan(dge);
    if(ddd<0)
        ddd+=PI;
    }
    printf("ddd=%f\n",ddd);
    ddd2=ddd*180/PI;
    printf("ddd2=%f\n",ddd2);
    
    return 0;

}

#endif