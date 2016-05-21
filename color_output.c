#ifdef _COLOR_OUTPUT
#include <stdio.h>
#include <stdlib.h>
const char *color[]={ "\033[0;40;31m","\033[0;40;32m","\033[0;40;33m","\033[0;40;34m","\033[0;40;35m","\033[0;40;36m"};
const char *const normal = "\033[0m";
int main(){

    int n=10;
    int i;
    for(i=0;i<n;i++)
    {
        int val=rand()%1000;
        int ci=rand()%6;
        printf("%s%d%s ",color[ci],val,normal);
    }
    printf("\n");
}
#endif