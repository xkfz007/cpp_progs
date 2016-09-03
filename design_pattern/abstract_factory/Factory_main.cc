#include <iostream>
#include "Factory.h"
#include "Product.h"
#include <cstdlib>
using namespace std;
int main(){
    AbstractFactory* f=new ConcreteFactory1;
    AbstractProductA* pA;
    AbstractProductB* pB;
    int n=10;
    for(int i=0;i<n;i++){
        int val=rand()%1000;
        pA=f->CreateProductA(val%2);
        pA->Operation();
        val=rand()%1000;
        pB=f->CreateProductB(val%2);
        pB->Operation();
        delete pA;
        delete pB;
    }
    delete f;
    return 0;
}
