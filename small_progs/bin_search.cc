
//#define _BIN_SEARCH
#ifdef _BIN_SEARCH
int bin_search(int x,int v[],int n){
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

int bin_search_v2(int x,int v[],int n){
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