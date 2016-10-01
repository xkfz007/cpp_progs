/*
 * this program is used to get the numerator and denominator of a fraction
 */
//#define _RAT
#ifdef _RAT
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fx_mathops.h"

//void reduceFraction(int* n, int* d) {
//  int a = *n;
//  int b = *d;
//  int c;
//  if(!a || !b)
//    return;
//  c = a % b;
//  while(c) {
//    a = b;
//    b = c;
//    c = a % b;
//  }
//  *n /= b;
//  *d /= b;
//}
static void usage(){
    printf("usage:\n");
    printf("     rat <int> <int>\n");
    printf("     rat <float> <float>\n");
    printf("     rat <string> the string format is a/b or a:b, a and b may be int or float\n");
    printf("     rat <float> \n");
    printf("     rat <int> \n");
}

int get_factor(double d){
    int p=1;
    while(ABS(d-(int)d)>1e-5){
        d*=10;
        p*=10;
    }
    return p;
}
int get_double(char* arg,double*n2){
    char *pos;
    int p1=1;
    if((pos=strchr(arg,'.'))!=NULL){
        *n2=atof(arg);
        p1=get_factor(*n2);
    }
    else{
        int n=atoi(arg);
        *n2=n*1.0;
    }
    return p1;
}
void process_two_args(char* arg1,char* arg2,int *n,int *d){
    char *pos;
    double n2,d2;
    int p1,p2,p;
    p1=get_double(arg1,&n2);
    p2=get_double(arg2,&d2);
    p=(p1>p2)?p1:p2;
    *n=(int)(n2*p+0.5);
    *d=(int)(d2*p+0.5);
}

int main(int argc,char**argv){
    int n,d;
    char *pos;
    //double n2,d2;
    //n2=atof(argv[1]);
    //n=get_factor(n2);
    //printf("%f:%d\n",n2,n);
    if(argc<2||argc>3){
        usage();
        exit(0);
    }
    if(argc==3){
        process_two_args(argv[1],argv[2],&n,&d);
        printf("%s/%s= ",argv[1],argv[2]);
    }
    else if(argc==2){
        char str[20];
        strcpy(str,argv[1]);
        if((pos=strchr(str,'/'))!=NULL ||
                (pos=strchr(str,':'))!=NULL){
            *pos='\0';
            process_two_args(str,pos+1,&n,&d);
        }
        else{
            double n2;
            int p1=get_double(argv[1],&n2);
            n=(int)(n2*p1+0.5);
            d=p1;
        }
        printf("%s= ",argv[1]);
    }
    fx_reduce_fraction(&n,&d);
    printf("%d/%d\n",n,d);
}
#endif