/*
find out the max area value of the histgram
*/
#ifdef _FIND_MAX_AREA
#include <stdio.h>
#define MAX(a,b) ((a)>(b)?(a):(b))
int main(){
    int height[]={2,1,5,6,2,3};
    int len=sizeof(height)/sizeof(height[0]);
    int max_area=0;
    int ln=0,rn=0;
    for(int i=0;i<len;i++){
        rn=i+1;
        for(int j=i+1;j<len;j++){
            if(height[j]<height[i]){
                rn=j;
                break;
            }

        }
        ln=i;
        for(int k=i-1;k>=0;k--){
            if(height[k]<height[i]){
                ln=k;
                break;
            }

        }
        max_area=MAX(max_area,height[i]*(rn-ln));

    }
    printf("Max_AREA=%d\n",max_area);
}

#endif