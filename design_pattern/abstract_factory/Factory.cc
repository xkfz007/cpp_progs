#include <iostream>
#include "Factory.h"
#include "Product.h"
AbstractFactory::AbstractFactory(){
    std::cout<<"constructor of AbstractFactory\n";
}
AbstractFactory::~AbstractFactory(){
    std::cout<<"destructor of AbstractFactory\n";
}
ConcreteFactory0::ConcreteFactory0(){
    std::cout<<"constructor of ConcreteFactory0\n";
}
ConcreteFactory0::~ConcreteFactory0(){
    std::cout<<"destructor of ConcreteFactory\n";
}
AbstractProductA* ConcreteFactory0::CreateProductA(int n){
    AbstractProductA* p=0;
    switch(n){
        case 0:
            p=new ProductA0;
            break;
        case 1:
            p=new ProductA1;
            break;
    }
    return p;
}
AbstractProductB* ConcreteFactory0::CreateProductB(int n){
    AbstractProductB* p=0;
    switch(n){
        case 0:
            p=new ProductB0;
            break;
        case 1:
            p=new ProductB1;
            break;
    }
    return p;
}
ConcreteFactory1::ConcreteFactory1(){
    std::cout<<"constructor of ConcreteFactory1\n";
}
ConcreteFactory1::~ConcreteFactory1(){
    std::cout<<"destructor of ConcreteFactory\n";
}
AbstractProductA* ConcreteFactory1::CreateProductA(int n){
    AbstractProductA* p=0;
    switch(n){
        case 0:
            p=new ProductA0;
            break;
        case 1:
            p=new ProductA1;
            break;
    }
    return p;
}
AbstractProductB* ConcreteFactory1::CreateProductB(int n){
    AbstractProductB* p=0;
    switch(n){
        case 0:
            p=new ProductB0;
            break;
        case 1:
            p=new ProductB1;
            break;
    }
    return p;
}
