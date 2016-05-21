//#define _ROTATE_MATRIX_
#ifdef _ROTATE_MATRIX_
/*
rotating a matrix 90 degrees in place
from https://blog.svpino.com/2015/05/10/programming-challenge-rotating-a-matrix-90-degrees-in-place
*/
#include <cstdio>
const int N=7;
void print_mat(int aa[][N]){
    printf("\n");
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++)
            fprintf(stdout,"%3d",aa[i][j]);
        fprintf(stdout,"\n");
    }
}
int main(){
    int aa[N][N];//={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    int t=0;
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++)
            aa[i][j]=t++;
    }
    print_mat(aa);
    int L=N/2;
    for(int k=0;k<L;k++)//Loop number
    {
        for(int i=k;i<N-1-k;i++)
        {
            int tmp=aa[k][i];
            aa[k][i]=aa[N-1-i][k];
            aa[N-1-i][k]=aa[N-1-k][N-1-i];
            aa[N-1-k][N-1-i]=aa[i][N-1-k];
            aa[i][N-1-k]=tmp;
            print_mat(aa);
        }
    }
}
#endif