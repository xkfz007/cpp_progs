#ifndef _VISITOR_H_
#define _VISITOR_H_
#include <iostream>
class Visitor;
class Element{
    public:
        virtual ~Element(){}
        virtual void Accept(Visitor*)=0;
};
class ConcreteElement1:public Element{
    public:
        ~ConcreteElement1(){}
        void Accept(Visitor*vt);
};
class ConcreteElement2:public Element{
    public:
        ~ConcreteElement2(){}
        void Accept(Visitor*vt);
};
class Visitor{
    public:
        virtual ~Visitor(){}
        virtual void VisitConcreteElement1(ConcreteElement1*)=0;
        virtual void VisitConcreteElement2(ConcreteElement2*)=0;
};
class ConcreteVisitor1:public Visitor{
    public:
        ~ConcreteVisitor1(){}
        void VisitConcreteElement1(ConcreteElement1*);
        void VisitConcreteElement2(ConcreteElement2*);
};
class ConcreteVisitor2:public Visitor{
    public:
        ~ConcreteVisitor2(){}
        void VisitConcreteElement1(ConcreteElement1*);
        void VisitConcreteElement2(ConcreteElement2*);
};
void ConcreteElement1::Accept(Visitor*vt){
    vt->VisitConcreteElement1(this);
}
void ConcreteElement2::Accept(Visitor*vt){
    vt->VisitConcreteElement2(this);
}
void ConcreteVisitor1::VisitConcreteElement1(ConcreteElement1*p1){
    std::cout<<"ConcreteVisitor1::VisitConcreteElement1:"<<(void*)p1<<'\n';
}
void ConcreteVisitor1::VisitConcreteElement2(ConcreteElement2*p2){
    std::cout<<"ConcreteVisitor1::VisitConcreteElement2:"<<(void*)p2<<'\n';
}
void ConcreteVisitor2::VisitConcreteElement1(ConcreteElement1*p1){
    std::cout<<"ConcreteVisitor2::VisitConcreteElement1:"<<(void*)p1<<'\n';
}
void ConcreteVisitor2::VisitConcreteElement2(ConcreteElement2*p2){
    std::cout<<"ConcreteVisitor2::VisitConcreteElement2:"<<(void*)p2<<'\n';
}


#endif
