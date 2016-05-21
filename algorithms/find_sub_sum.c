#ifdef _FIND_SUB_SUM
#include <stdio.h>
int find_sub_sum(int* arr,int N,int* s,int* e)
{
    int i;
    int sum,sum_best;
    int start,end;
    start=0;
    sum=0;
    sum_best=0;

    for(i=0;i<N;i++)
    {
        sum+=arr[i];
        if(sum>sum_best)
        {
            sum_best=sum;
            *s=start;
            *e=i;//end;
        }
        else if(sum<=0){
            start=i+1;
            sum=0;
        }
    }
    return sum_best;
}
int main(){
    int arr[]={31,-41,59,26,-53,58,97,-93,-23,84};
    int s,e,sum;
    sum=find_sub_sum(arr,sizeof(arr)/sizeof(int),&s,&e);
    printf("sum[%d,%d]=%d\n",s,e,sum);
}
#endif
