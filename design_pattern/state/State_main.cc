#include "./State.h"
int main(){
    State *p1=new ConcreteState1;
    Context *pc=new Context(p1);
    pc->Request();
    pc->Request();
    pc->Request();
    pc->Request();
    pc->Request();
    pc->Request();
    delete pc;
}
