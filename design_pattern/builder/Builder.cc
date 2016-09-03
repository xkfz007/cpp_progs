#include <iostream>
#include "Builder.h"
Builder::Builder(){
    std::cout<<"constructor of Builder\n";
}
Builder::~Builder(){
    std::cout<<"destructor of Builder\n";
}
ConcreteBuilder0::ConcreteBuilder0(){
    std::cout<<"constructor of ConcreteBuilder0\n";
}
ConcreteBuilder0::~ConcreteBuilder0(){
    std::cout<<"destructor of ConcreteBuilder0\n";
}
void ConcreteBuilder0::BuildPartA(){
    std::cout<<"Part A has been built\n";

}
void ConcreteBuilder0::BuildPartB(){
    std::cout<<"Part B has been built\n";

}
void ConcreteBuilder0::BuildPartC(){
    std::cout<<"Part C has been built\n";

}
ConcreteBuilder1::ConcreteBuilder1(){
    std::cout<<"constructor of ConcreteBuilder1\n";
}
ConcreteBuilder1::~ConcreteBuilder1(){
    std::cout<<"destructor of ConcreteBuilder1\n";
}
void ConcreteBuilder1::BuildPartA(){
    std::cout<<"Part A has been built\n";

}
void ConcreteBuilder1::BuildPartB(){
    std::cout<<"Part B has been built\n";

}
void ConcreteBuilder1::BuildPartC(){
    std::cout<<"Part C has been built\n";

}
