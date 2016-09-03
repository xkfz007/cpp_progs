#include "Product.h"
#include <iostream>

AbstractProductA::AbstractProductA(){
    std::cout<<"constructor of AbstractProductA\n";
}
AbstractProductA::~AbstractProductA(){
    std::cout<<"destructor of AbstractProductA\n";
}
AbstractProductB::AbstractProductB(){
    std::cout<<"constructor of AbstractProductB\n";
}
AbstractProductB::~AbstractProductB(){
    std::cout<<"destructor of AbstractProductB\n";
}
ProductA0::ProductA0(){
    std::cout<<"constructor of ProductA0\n";
}
ProductA0::~ProductA0(){
    std::cout<<"destructor of ProductA0\n";
}
void ProductA0::Operation(){
    std::cout<<"Operation of ConcreteProductA0\n";
}
ProductA1::ProductA1(){
    std::cout<<"constructor of ProductA1\n";
}
ProductA1::~ProductA1(){
    std::cout<<"destructor of ProductA1\n";
}
void ProductA1::Operation(){
    std::cout<<"Operation of ConcreteProductA1\n";
}
ProductB0::ProductB0(){
    std::cout<<"constructor of ProductB0\n";
}
ProductB0::~ProductB0(){
    std::cout<<"destructor of ProductB0\n";
}
void ProductB0::Operation(){
    std::cout<<"Operation of ConcreteProductB0\n";
}
ProductB1::ProductB1(){
    std::cout<<"constructor of ProductB1\n";
}
ProductB1::~ProductB1(){
    std::cout<<"destructor of ProductB1\n";
}
void ProductB1::Operation(){
    std::cout<<"Operation of ConcreteProductB1\n";
}
