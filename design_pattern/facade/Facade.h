#ifndef _FACADE_H_
#define _FACADE_H_
#include <iostream>
class SubSystem1{
    public:
        SubSystem1(){}
        ~SubSystem1(){}
        void Operation(){
            std::cout<<"SubSystem1::Operation()\n";
        }
};
class SubSystem2{
    public:
        SubSystem2(){}
        ~SubSystem2(){}
        void Operation(){
            std::cout<<"SubSystem2::Operation()\n";
        }
};
class Facade{
    public:
        Facade():sb1(new SubSystem1),sb2(new SubSystem2){}
        ~Facade(){
            delete sb1;
            delete sb2;
        }
        void OperationWrapper(){
            std::cout<<"Facade::Operation() ->\n    ";
            sb1->Operation();
            std::cout<<"    ";
            sb2->Operation();
        }
    private:
        SubSystem1* sb1;
        SubSystem2* sb2;
};

#endif
