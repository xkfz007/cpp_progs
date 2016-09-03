#include <iostream>
#include "Factory.h"
#include "Product.h"
#include <cstdlib>
using namespace std;
int main(){
    Factory* f=new ConcreteFactory;
    Product *p;
    int n=10;
    for(int i=0;i<n;i++){
        int val=rand()%1000;
        p=f->CreateProduct(val%2);
        p->Operation();
        delete p;
    }
    delete f;
    return 0;
}
