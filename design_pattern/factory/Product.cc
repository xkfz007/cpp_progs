#include "Product.h"
#include <iostream>

Product::Product(){
    std::cout<<"constructor of Product\n";
}
Product::~Product(){
    std::cout<<"destructor of Product\n";
}
ConcreteProduct1::ConcreteProduct1(){
    std::cout<<"constructor of ConcreteProduct1\n";
}
ConcreteProduct1::~ConcreteProduct1(){
    std::cout<<"destructor of ConcreteProduct1\n";
}
void ConcreteProduct1::Operation(){
    std::cout<<"Operation of ConcreteProduct1\n";
}
ConcreteProduct0::ConcreteProduct0(){
    std::cout<<"constructor of ConcreteProduct0\n";
}
ConcreteProduct0::~ConcreteProduct0(){
    std::cout<<"destructor of ConcreteProduct0\n";
}
void ConcreteProduct0::Operation(){
    std::cout<<"Operation of ConcreteProduct0\n";
}
