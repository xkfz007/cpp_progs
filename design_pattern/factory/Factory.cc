#include <iostream>
#include "Factory.h"
#include "Product.h"
Factory::Factory(){
    std::cout<<"constructor of Factory\n";
}
Factory::~Factory(){
    std::cout<<"destructor of Factory\n";
}
ConcreteFactory::ConcreteFactory(){
    std::cout<<"constructor of ConcreteFactory\n";
}
ConcreteFactory::~ConcreteFactory(){
    std::cout<<"destructor of ConcreteFactory\n";
}
Product* ConcreteFactory::CreateProduct(int n){
    Product* p=0;
    switch(n){
        case 0:
            p=new ConcreteProduct0;
            break;
        case 1:
            p=new ConcreteProduct1;
            break;
    }
    return p;
}
