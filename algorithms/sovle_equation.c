//#define _SOLVE_EQUATION
#ifdef _SOLVE_EQUATION
/*
   X + X - 9 = 4
   +   -   -
   X - X * X = 4
   /   *   -
   X + X - X = 4
   =   =   =
   4   4   4
   equ1: a1 + a2 - 9 = 4
   equ2: a3 - a4 * a5 = 4
   equ3: a6 + a7 - a8 = 4
   equ4: a1 + a3/a6 = 4
   equ5: a2 - a4 * a7 = 4
   equ6: 9 - a5 - a8 = 4
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>
typedef long long uint64_t;

uint64_t mdate() {
  struct timeb tb;
  ftime(&tb);
  return ((uint64_t)tb.time * 1000 + (uint64_t)tb.millitm) * 1000;
}
void brute_force(){
    int a1,a2,a3,a4,a5,a6,a7,a8;
    uint64_t t_s,t_elapsed;
    t_s=mdate();
    for(a1=1;a1<100;a1++){
        for(a2=1;a2<100;a2++){
            if(a1+a2-9!=4)//equ1
                continue;
            for(a3=1;a3<100;a3++){
                for(a4=1;a4<100;a4++){
                    for(a5=1;a5<100;a5++){
                        if(a3-a4*a5!=4)//equ2
                            continue;
                        for(a6=1;a6<100;a6++){
                            if(a1+a3/a6!=4)//equ4
                                continue;
                            for(a7=1;a7<100;a7++){
                                if(a2-a4*a7!=4)//equ5
                                    continue;
                                for(a8=1;a8<100;a8++){
                                    if(a6+a7-a8!=4||9-a5-a8!=4)//equ3
                                        continue;
                                    printf("a1=%2d a2=%2d a3=%3d a4=%2d a5=%2d a6=%2d a7=%d a8=%2d\n",
                                        a1,a2,a3,a4,a5,a6,a7,a8 );
                                }
                            }
                        }

                    }

                }
            }
        }
    }
    t_elapsed=mdate()-t_s;
    printf("time=%d\n",t_elapsed);
}
#define IF_NOT_IN_RANGE(a) if(a<1||a>=100) continue;
void new_method(){
    int a1,a2,a3,a4,a5,a6,a7,a8;
    uint64_t t_s,t_elapsed;
    t_s=mdate();
    for(a1=1;a1<100;a1++){
        a2=13-a1;
        IF_NOT_IN_RANGE(a2)
        for(a6=1;a6<100;a6++){
            a3=a6*(4-a1);
            IF_NOT_IN_RANGE(a3)
            for(a7=1;a7<100;a7++){
                a4=(9-a1)/a7;
                if(a4*a7!=9-a1)
                    continue;
                IF_NOT_IN_RANGE(a4)
                a5=9-a6-a7;
                IF_NOT_IN_RANGE(a5)
                a8=a6+a7-4;
                IF_NOT_IN_RANGE(a8)
                printf("a1=%2d a2=%2d a3=%3d a4=%2d a5=%2d a6=%2d a7=%d a8=%2d\n",
                    a1,a2,a3,a4,a5,a6,a7,a8 );
            }
        }
    }
    t_elapsed=mdate()-t_s;
    printf("time=%d\n",t_elapsed);
}
int main(){
    brute_force();
    new_method();
}

#endif