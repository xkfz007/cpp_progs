#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>
using namespace std;
#if 0
int test1(){
    srand(time(NULL));
    for(int i=0;i<20;i++){
        int r=rand()%1000;
        cout<<dec<<setw(4)<<r<<' ';
        cout<<oct<<setw(6)<<r<<' ';
        cout<<hex<<setw(4)<<r<<' ';
        cout<<endl;
    }
    return 0;
}
int& ref_para(int& x){
    x++;
    int k=3;
    printf("ADDR(k)=%x\n",&k);
    return k;
}
void test2(){
    int y,z;
    z=6;
    y=ref_para(z);
    cout<<"y="<<y<<" z="<<z<<endl;
    y=100000;
    cout<<"y="<<y<<" z="<<z<<endl;
    int& d=ref_para(z);
    cout<<"d="<<d<<" z="<<z<<endl;
    printf("ADDR(d)=%x\n",&d);
    y=8;
    cout<<"y="<<y<<" z="<<z<<endl;
    cout<<"d="<<d<<" z="<<z<<endl;
    printf("ADDR(d)=%x\n",&d);
}
void test3(int a=100,int b=20){
    cout<<"a="<<a<<" b="<<b<<endl;
}
class student{
    private:
        string name;
        string id;
    public:
        student(string nm="Tom" ,string idx="000000"):name(nm){
            count++;
            stringstream ss;
            ss<<count;
            id=idx+ss.str();
        }
        void output(ostream& os){
            os<<"No."<<count<<" Name:"<<name<<" Id: "<<id;
        }
    public:
        static int count;
};
ostream& operator<<(ostream& os,student & s){
    s.output(os);
    return os;
}
int student::count=0;
void test4(){
    student a("a1");
    student b("b1");
    student c("c1");
    student d("d1");
    cout<<a<<endl;
    cout<<b<<endl;
    cout<<c<<endl;
    cout<<d<<endl;
    cout<<student::count<<endl;
}
int main(){
    test4();
}

#endif
#if 0
#include <stdio.h>
int main(){
    //int a[4][2];
    //printf("size=%d\n",sizeof(a));
    fpos_t pos;
    pos._pos=5;
    
	
}
#endif
#if 0
#include <stdio.h>
int main(){
  int ctu_x=0,ctu_y=0;
    const int noOfBlocks = 4; //TODO g_maxCUSize / 16;
    const int i_mb_x = ctu_x * noOfBlocks;
    const int i_mb_y = ctu_y * noOfBlocks;
    const int i_mb_height=68,i_mb_width=120;
    const int i_mb_stride=120;
    float qp_offset = 0;
    float tmp_qp=0;
    int cnt = 0, idx = i_mb_x + i_mb_y * i_mb_stride;
    for(int j = 0,jj=i_mb_y; j < noOfBlocks && jj < i_mb_height; j++,jj++) {
      for(int i = 0,ii=i_mb_x; i < noOfBlocks && ii < i_mb_width; i++,ii++){
          //qp_offset += qpoffs[idx + i];
        qp_offset+=100;
        cnt++;
        ii++;
      }
      idx += i_mb_stride;
    }
      qp_offset /= cnt;
      tmp_qp += qp_offset;
fprintf(stdout,"cnt=%d qp_offset=%f tmp_qp=%f\n",cnt,qp_offset,tmp_qp);
fflush(stdout);
}

#endif
#if 0
int (*functiond(int(*a)[5]))[5] {
  return a;
}
int main(){
int (*fa)[5];
int a[5]={15,12,13,14,11};
int i;
fa=functiond(&a);
for(i=0;i<5;i++){
  printf("%d\n",(*fa)[i]);
}
}
#endif
#if 0
#include <stdio.h>

int main ()
{
  int result;
  char oldname[] ="f:/encoder_test_output/as265_output/_rcinfo_as265-2-BlowingBubbles_416x240_50.yuv-RC1_300_300_300_0_10_1_1_P5";
  char newname[] ="f:/encoder_test_output/as265_output/new.txt";
  result= rename( oldname , newname );
  if ( result == 0 )
    puts ( "File successfully renamed" );
  else
    perror( "Error renaming file" );
  return 0;
}

#endif

#if 0
#include <stdio.h>
#include <stdlib.h>
void ff(void* &t){
    t=0;
}
//int f(int a){
//    return a;
//}
//int f(int a,int b=0){
//    return a+b;
//}
int main(){
    int a=3;
    int *p=&a;
    int* p2=(int*)malloc(10*sizeof(int));
    free(p2);
    if(p2!=0){
        fprintf(stdout,"TOO BAD\n");
    }
    return 0;
}
#endif
#if 0
#include <cstdio>
#include <cmath>
#include <cstdlib>
#define log2(x) (log((double)x)* 1.4426950408889640513713538072172)
double qscale2qp(double qscale) {
  return 12.0 + 6.0 * log2(qscale / 0.85);
}
double qp2qscale(double qp) {
  return 0.85 * pow(2.0, (qp - 12.0) / 6.0);
}
int main(){
    double q=57.0894431126749140000;
    //double qscale=qscale2qp(q);
    //printf("qscale=%.19f\n",qscale);
    int size=1000*1000*1000*1;
    int* p=(int*)malloc(size*sizeof(int));
    if(p){
        printf("memory is allocated:%p\n",p);
    }
    else{
        printf("could not allocate memory requested\n");
    }
}
#endif
