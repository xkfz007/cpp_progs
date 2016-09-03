#include "./Builder.h"
#include "./Director.h"
#include <iostream>
#include <cstdlib>
using namespace std;
int main(){
    Director *p=0;
    int n=5;
    for(int i=0;i<n;i++){
        int val=rand()%100;
        if(val%2)
        p=new Director(new ConcreteBuilder1);
        else
        p=new Director(new ConcreteBuilder0);
        p->Construct();
        delete p;
    }
}
