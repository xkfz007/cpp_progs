#ifndef _STATE_H_
#define _STATE_H_
#include <iostream>
class Context;
class State{
    public:
        virtual ~State(){}
        virtual void Handle(Context*)=0;
};
class Context{
    public:
        Context(State* st):_state(st){}
        void Request(){
            if(_state!=0)
            _state->Handle(this);
        }
        ~Context(){
            delete _state;
        }
        void ChangeState(State* st){
            if(_state!=0){
                delete _state;
            }
            _state=st;
        }
    private:
        State* _state;
};
class ConcreteState1:public State{
    public:
        void Handle(Context* pC);
};
class ConcreteState2:public State{
    public:
        void Handle(Context* pC);
};
void ConcreteState1::Handle(Context* pC){
    std::cout<<"ConcreteState1::Handle\n";
    pC->ChangeState(new ConcreteState2);
}
void ConcreteState2::Handle(Context* pC){
    std::cout<<"ConcreteState2::Handle\n";
    pC->ChangeState(new ConcreteState1);
}

#endif
