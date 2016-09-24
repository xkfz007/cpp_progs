#ifndef _FX_BINSEARCH_H
//#define _BIN_SEARCH
static int fx_binsearch(int x,int v[],int n){
    int low,high,mid;
    low=0;
    high=n-1;
    while(low<=high){
        mid=(low+high)>>1;
        if(x<v[mid])
            high=mid-1;
        else if(x>v[mid])
            low=mid+1;
        else
            return mid;
    }
    return -1;
}

static int fx_binsearch2(int x,int v[],int n){
    int low,high,mid;
    low=0;
    high=n-1;
    mid=(low+high)/2;
    while(low<=high&&x!=v[mid]){
        if(x<v[mid])
            high=mid-1;
        else
            low=mid+1;
        mid=(low+high)/2;
    }
    if(x==v[mid])
        return mid;
    else
        return -1;                            
}
#endif