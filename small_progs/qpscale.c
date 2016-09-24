#ifdef _QPSCALE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
static double qp2qscale(int qp)
{
        return 0.85 * pow(2.0, ( qp - 12.0 ) / 6.0);
}
static double qscale2qp(double qscale)
{
        return 12.0 + 6.0 * log(qscale/0.85) / log(2.0) ;
}
static void usage(){
    printf("usage:qpscale.exe number\n\
             int: qp=>qscale\n\
             float: qscale=>qp");
    exit(-1);
}

int main(int argc,char* argv[])
{
    int i;
    char *p;

    if(argc<2){
        //fprintf(stderr,"Not enough parameters!\n");
        usage();
        exit(-1);
    }
    for(i=1;i<argc;i++){
        p=strchr(argv[i],'.');
        if(p==NULL){
           int qp=atoi(argv[i]);
           double qscale=qp2qscale(qp);
            printf("qp=%2d qscale=%7.4f\n",qp,qscale);
        }
        else{
           double qscale=atof(argv[i]);
           double qp=qscale2qp(qscale);
            printf("qscale=%7.4f qp=%7.4f\n",qscale,qp);
        }
    }
    return 0;
}

#endif
