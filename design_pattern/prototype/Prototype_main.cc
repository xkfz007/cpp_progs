#include <iostream>
#include <cstdlib>
#include "./Prototype.h"
using namespace std;
int main(){
    Prototype *p0=new ConcretePrototype0;
    Prototype *p1=new ConcretePrototype1;
    int n=5;
    Prototype* pp;
    for(int i=0;i<n;i++){
        int val=rand()%1000;
        if(val%2)
            pp=p1->Clone();
        else
            pp=p0->Clone();
        delete pp;
    }
    delete p0;
    delete p1;
}
