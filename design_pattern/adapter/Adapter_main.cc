#include <iostream>
#include "./Adapter.h"
using namespace std;
int main(){
    Adaptee *pad=new Adaptee;
    Target *pt=new Adapter(pad);
    pt->Request();

    delete pt;
}
