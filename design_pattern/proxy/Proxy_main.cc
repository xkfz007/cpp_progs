#include "./Proxy.h"
#include <iostream>
#include <cstdlib>
using namespace std;
int main(){
    Subject* sub;
    Proxy* p;
    for(int i=0;i<5;i++){
        sub=new RealSubject;
        p=new Proxy(sub);
        p->Request();

        delete p;
    }
}
