#ifdef _SORT
#include <stdio.h>
#include <time.h>
#define NUM 20
#define SWAP(x,y) ((x)==(y)?-1:((x)^=(y),(y)^=(x),(x)^=(y)))
int FLAG[NUM];
void swap(int *x,int *y)
{
    if(*x==*y)
        return;
    else
    {
        *x^=*y;
        *y^=*x;
        *x^=*y;
    }
}
void print_arr(int arr[],int n)
{
    int i;
    for(i=0;i<n;i++)
        printf("%3d",arr[i]);
    printf("\n");
}
void bubble(int arr[],int n)
{
    int i,j;
    int flag=1;
    i=n-1;
    while(i>0&&flag)
    {
        flag=0;
        for(j=0;j<i;j++)
            if(arr[j]>arr[j+1])
            {
                swap(&arr[j],&arr[j+1]);
                flag=1;
            }
        i--;
    printf("aftr:");
    print_arr(arr,NUM);
    }
}
void select(int arr[],int n)
{
    int i,j;
    int index;
    int min;
    for(i=0;i<n;i++)
    {
        index=i;
        min=arr[i];
        for(j=i+1;j<n;j++)
            if(arr[j]<min)
            {
                index=j;
                min=arr[j];
            }
        if(index!=i)
            swap(arr+i,arr+index);
    }
}
void shell(int arr[],int n)
{
    int i;
    int flag=1;
    int gap=n/2;
    while(gap>=1&&flag)
    {
        flag=0;
        for(i=0;i<n-gap;i++)
        {
            if(arr[i]>arr[i+gap])
            {
                swap(&arr[i],&arr[i+gap]);
                flag=1;
            }
        }
        if(gap>1)
            gap/=2;
    printf("aftr:");
    print_arr(arr,NUM);
    }
}
void qsort_(int arr[],int l,int r)
{
    int pivot=arr[(l+r)/2];
    int i=l,j=r;
    while(i<=j)
    {
        while(arr[i]<pivot)
            i++;
        while(arr[j]>pivot)
            j--;
        if(i<=j)
        {
            if(i!=j)
            swap(&arr[i],&arr[j]);
            i++;
            j--;
        }
    }
    printf("aftr:");
    print_arr(arr,NUM);
    if(l<j)
        qsort_(arr,l,j);
    if(i<r)
        qsort_(arr,i,r);

}
void quick(int arr[],int n)
{
    qsort_(arr,0,n-1);
}
void adjust(int v[],int m,int n)
{
    int *b=v-1;
    int j=m;
    int k=2*m;

    while(k<=n)
    {
        if(k<n&&b[k]<b[k+1])
            k++;
        if(b[j]<b[k])
            swap(&b[j],&b[k]);
        j=k;
        k*=2;
    }
}
void heap(int v[],int n)
{
    //build the heap
    int *b,j;
    b=v-1;
    for(j=n/2;j>0;j--)
        adjust(v,j,n);
    for(j=n-1;j>0;j--)
    {
        swap(&b[1],&b[j+1]);
        adjust(v,1,j);
    }

}


int main()
{
    int arr[NUM];
    int i;
    srand((int)time(NULL));
    for(i=0;i<NUM;i++)
        arr[i]=rand()%100;
    printf("orig:");
    print_arr(arr,NUM);
    
//    bubble(arr,NUM);
//    select(arr,NUM);
//   shell(arr,NUM);
//    quick(arr,NUM);
    heap(arr,NUM);
    printf("aftr:");
    print_arr(arr,NUM);

    
    return 0;
}
#endif
