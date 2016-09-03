#ifndef _MEMENTO_H_
#define _MEMENTO_H_
#include <iostream>
typedef int STATE;
class Originator;
class Memento{
    private:
        friend class Originator;
        Memento(const STATE& st):_state(st){}
        ~Memento(){}
        STATE GetState(){
            return _state;
        }
        void SetState(const STATE& st){
            _state=st;
        }
        STATE _state;
};
class Originator{
    public:
        Originator(const STATE& st):_state(st){}
        ~Originator(){}
        void SetMemento(Memento* m){
            m->SetState(_state);
        }
        Memento* CreateMemento(){
            return new Memento(_state);
        }
        STATE GetState(){
            return _state;
        }
        void SetState(const STATE& st){
            _state=st;
        }
        void RestoreState(Memento* pmem){
            _state=pmem->GetState();
        }
        void PrintState(){
            std::cout<<"Originator::_state="<<_state<<'\n';
        }
        
    private:
        STATE _state;

};


#endif
