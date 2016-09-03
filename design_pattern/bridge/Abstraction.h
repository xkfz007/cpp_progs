#ifndef _ABSTRACTION_H_
#define _ABSTRACTION_H_
#include "./AbstractionImp.h"
class Abstraction{
    public:
        Abstraction(AbstractionImp* imp):_abimp(imp){};
        virtual ~Abstraction(){};
        virtual void Operation()=0;
    protected:
            AbstractionImp* _abimp;
};
class RefinedAbstraction:
    public Abstraction{
        public:
            RefinedAbstraction(AbstractionImp* imp):Abstraction(imp){}
            ~RefinedAbstraction(){

                if(_abimp!=0){
                    delete _abimp;
                    _abimp=0;
                }
            }
            void Operation(){
                _abimp->Operation();
            }
    };

#endif

