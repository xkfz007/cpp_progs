#ifndef _DECORATOR_H_
#define _DECORATOR_H_
#include <iostream>
class Component{
    public:
        Component(){}
        virtual ~Component(){}
        virtual void Operation()=0;
};
class ConcreteComponent: public Component{
    public:
        ConcreteComponent(){}
        ~ConcreteComponent(){}
        void Operation(){
            std::cout<<"ConcreteComponent::Operation()\n";
        }
};
class Decorator: public Component{
    public:
        Decorator(Component* comp):_comp(comp){}
        virtual ~Decorator(){
            if(_comp!=0) {
                delete _comp;
                _comp=0;
            }
        }
    protected:
        Component* _comp;
};
class ConcreteDecoratorA: public Decorator{
    public:
        ConcreteDecoratorA(Component*p):Decorator(p){}
        ~ConcreteDecoratorA(){}
        virtual void Operation(){
            _comp->Operation();
            AddedBehavior();
        }
        void AddedBehavior(){
            std::cout<<"ConcreteDecoratorA::AddedBehavior()\n";
        }
};
class ConcreteDecoratorB: public Decorator{
    public:
        ConcreteDecoratorB(Component*p):Decorator(p){}
        ~ConcreteDecoratorB(){}
        virtual void Operation(){
            _comp->Operation();
            AddedBehavior();
        }
        void AddedBehavior(){
            std::cout<<"ConcreteDecoratorB::AddedBehavior()\n";
        }
};

#endif
