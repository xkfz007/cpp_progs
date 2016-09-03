#include "Builder.h"
#include "Director.h"
#include <iostream>
Director::Director(Builder* b){
    std::cout<<"constructor of Director\n";
    _bldr=b;
}
Director::~Director(){
    std::cout<<"destructor of Director\n";
    if(_bldr!=0){
        delete _bldr;
        _bldr=0;
    }
}
void Director::Construct(){
    _bldr->BuildPartA();
    _bldr->BuildPartB();
    _bldr->BuildPartC();
}
