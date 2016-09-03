#include "./Prototype.h"
#include <iostream>
Prototype::Prototype(){
    std::cout<<"constructor of Prototype\n";
}
Prototype::~Prototype(){
    std::cout<<"destructor of Prototype\n";
}
ConcretePrototype0::ConcretePrototype0(){
    std::cout<<"constructor of ConcretePrototype0\n";
}
ConcretePrototype0::~ConcretePrototype0(){
    std::cout<<"destructor of ConcretePrototype0\n";
}
ConcretePrototype0::ConcretePrototype0(const ConcretePrototype0& rhs){
    std::cout<<"Copy constructor of ConcretePrototype0\n";
}
Prototype* ConcretePrototype0::Clone(){
    return new ConcretePrototype0(*this);
}
ConcretePrototype1::ConcretePrototype1(){
    std::cout<<"constructor of ConcretePrototype1\n";
}
ConcretePrototype1::~ConcretePrototype1(){
    std::cout<<"destructor of ConcretePrototype1\n";
}
ConcretePrototype1::ConcretePrototype1(const ConcretePrototype1& rhs){
    std::cout<<"Copy constructor of ConcretePrototype1\n";
}
Prototype* ConcretePrototype1::Clone(){
    return new ConcretePrototype1(*this);
}
