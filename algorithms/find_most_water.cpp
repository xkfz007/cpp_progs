#define _CONTAINER_WITH_MOST_WATER
#ifdef _CONTAINER_WITH_MOST_WATER
/*
[LeetCode] Container With Most Water
*/
#include <cstdio>
#include <cmath>
#define MIN(a,b) ((a)>(b)?(b):(a))
int container_core(int height[],int& l2,int& r2){
    int max_water=0;
    if(l2>=r2)
        return max_water;
    int l=l2,r=r2;
    while(l<r){
        int t=(r-l)*MIN(height[l],height[r]);
        if(t>max_water){
            max_water=t;
            l2=l;
            r2=r;
        }
        if(height[l]<height[r])
            l++;
        else
            r--;
    }
    return max_water;
}
void find_container_with_most_water(int height[],int N){
    int max_water;
    int l=0,r=N-1;
    max_water=container_core(height,l,r);
    printf("max_water=%d[%d,%d]\n",max_water,l,r);
}
int main(){
    int height[]={4,6,2,6,7,11,2};
    int N=sizeof(height)/sizeof(int);
    find_container_with_most_water(height,N);

}

#endif