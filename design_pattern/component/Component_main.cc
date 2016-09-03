#include "./Component.h"
#include <iostream>
using namespace std;
int main(){
    Leaf* p1=new Leaf;
    Leaf* p2=new Leaf;
    Composite* p=new Composite;
    p->Add(p1);
    p->Add(p2);
    p->Operation();
    p->GetChild(0)->Operation();

    delete p;

    return 0;
}
