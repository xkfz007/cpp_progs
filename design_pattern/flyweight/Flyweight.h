#ifndef _FLYWEIGHT_H_
#define _FLYWEIGHT_H_
#include <list>
#include <string>
#include <iostream>
template <class STATE>
class Flyweight{
    public:
        Flyweight(const STATE& st):_state(st){}
        virtual ~Flyweight(){}
        virtual void Operation(STATE& extrinsicSate){}
        STATE GetIntrinsicState(){
            return _state;
        }
    private:
        STATE _state;
};
template <class STATE>
class ConcreteFlyweight: public Flyweight<STATE>{
    public:
        ConcreteFlyweight(const STATE& st):Flyweight<STATE>(st){}
        ~ConcreteFlyweight(){}
        void Operation(STATE& extrinsicSate){}
};
template <class STATE>
class FlyweightFactory{
    public:
        FlyweightFactory(){}
        ~FlyweightFactory();
        Flyweight<STATE>* GetFlyweight(const STATE& key);
    private:
        std::list<Flyweight<STATE>*> _lst;
};
template <class STATE>
FlyweightFactory<STATE>::~FlyweightFactory(){
    typename std::list<Flyweight<STATE>*>::iterator it;
    for(it=_lst.begin();it!=_lst.end();++it) {
        delete (*it);
    }
}
template <class STATE>
Flyweight<STATE>* FlyweightFactory<STATE>::GetFlyweight(const STATE& key){
    typename std::list<Flyweight<STATE>*>::iterator it;
    for(it=_lst.begin();it!=_lst.end();++it) {
        if((*it)->GetIntrinsicState()==key){
            std::cout<<"exists\n";
            return *it;
        }
    }
    Flyweight<STATE>* pf=new ConcreteFlyweight<STATE>(key);
    _lst.push_back(pf);
    std::cout<<"created\n";
    return pf;
}

#endif
