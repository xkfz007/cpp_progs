#include <stdio.h>

#define L(CS) CS+3
#define L_W(CS) L##CS

#define GLOB_LOOP(A) \
    printf("%c\n",A('a'))

int main(){
    GLOB_LOOP(L);
    GLOB_LOOP(L_W);

}
