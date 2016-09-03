#include "./Decorator.h"
#include <iostream>
#include <cstdlib>
using namespace std;
int main(){
    Component* p;
    Decorator* pd;
    int n=5;
    for(int i=0;i<n;i++){
        int val=rand()%1000;
        p=new ConcreteComponent;
        if(val%2)
            pd=new ConcreteDecoratorB(p);
        else
            pd=new ConcreteDecoratorA(p);
        pd->Operation();

        delete pd;
    }
}
