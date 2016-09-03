#ifndef _SUBJECT_H_
#define _SUBJECT_H_
#include <list>
#include <iostream>
typedef int STATE;
class Subject;
class Observer{
    public:
        Observer():_stat(-1){}
        Observer(const STATE& st):_stat(st){}
        virtual ~Observer(){}
        virtual void Update(Subject*)=0;
    protected:
        STATE _stat;
};

class Subject{
    public:
        virtual ~Subject();
        void Attach(Observer* ob);
        void Detach(Observer* ob);
        void Notify();
        virtual STATE GetState()=0;
        virtual void SetState(const STATE&)=0;
    protected:
        std::list<Observer*> _lst;
};
Subject::~Subject(){
    std::list<Observer*>::iterator it;
    for(it=_lst.begin();it!=_lst.end();++it)
        delete *it;
}
void Subject::Attach(Observer*ob){
    _lst.push_back(ob);
    std::cout<<(void*)ob<<" Attached\n";
}
void Subject::Detach(Observer*ob){
    _lst.remove(ob);
    std::cout<<(void*)ob<<" Detached\n";
}
void Subject::Notify(){
    std::cout<<"Subject::Notify \n";
    std::list<Observer*>::iterator it;
    for(it=_lst.begin();it!=_lst.end();++it) {
        std::cout<<"    ";
        (*it)->Update(this);
    }
}
class ConcreteSubject:public Subject{
    public:
        ConcreteSubject():_stat(-1){}
        STATE GetState(){
            return _stat;
        }
        void SetState(const STATE& st){
            _stat=st;
        }
    private:
        STATE _stat;
};

class ConcreteObserver1:public Observer{
    public:
        ConcreteObserver1():Observer(){}
        ConcreteObserver1(const STATE& st):Observer(st){}
        void Update(Subject*sub){
            std::cout<<"state:"<<_stat<<" -> ";
            _stat=sub->GetState();
            std::cout<<_stat<<'\n';
        }
};
class ConcreteObserver2:public Observer{
    public:
        ConcreteObserver2():Observer(){}
        ConcreteObserver2(const STATE& st):Observer(st){}
        void Update(Subject*sub){
            std::cout<<"state:"<<_stat<<" -> ";
            _stat=sub->GetState();
            std::cout<<_stat<<'\n';
        }
};

#endif
