#include "./Handler.h"
int main(){
    Handler* pch1=new ConcreteHandle1();
    Handler* pch2=new ConcreteHandle2(pch1);
    pch2->HandleRequest();
    delete pch2;
}
