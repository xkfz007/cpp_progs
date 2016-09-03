#ifndef _ABSTRACTIONIMP_H_
#define _ABSTRACTIONIMP_H_
#include <iostream>
class AbstractionImp{
    public:
        AbstractionImp(){}
        virtual ~AbstractionImp(){}
        virtual void Operation()=0;
};
class ConcreteAbstractionImp0:
    public AbstractionImp{
    public:
        ConcreteAbstractionImp0(){}
        ~ConcreteAbstractionImp0(){}
        void Operation(){
            std::cout<<"ConcreteAbstractionImp0::Operation()\n";

        }
};
class ConcreteAbstractionImp1:
    public AbstractionImp{
    public:
        ConcreteAbstractionImp1(){}
        ~ConcreteAbstractionImp1(){}
        void Operation(){
            std::cout<<"ConcreteAbstractionImp1::Operation()\n";

        }
};


#endif
