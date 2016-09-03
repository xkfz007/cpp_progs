#include "./Visitor.h"
int main(){
    Visitor* vt1=new ConcreteVisitor1,
        *vt2=new ConcreteVisitor2;
    Element *el1=new ConcreteElement1,
            *el2=new ConcreteElement2;
    el1->Accept(vt1);
    el1->Accept(vt2);
    el2->Accept(vt1);
    el2->Accept(vt2);
}
