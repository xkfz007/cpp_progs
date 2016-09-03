#include "./Adapter.h"
#include <iostream>
Target::Target(){
    std::cout<<"Target::Target()\n";
}
Target::~Target(){
    std::cout<<"Target::~Target()\n";
}
void Target::Request(){
    std::cout<<"Target::Request()\n";
}
Adaptee::Adaptee(){
    std::cout<<"Adaptee::Adaptee()\n";
}
Adaptee::~Adaptee(){
    std::cout<<"Adaptee::~Adaptee()\n";
}
void Adaptee::SpecificRequest(){
    std::cout<<"Adaptee::SpecificRequest()\n";

}
Adapter::Adapter(Adaptee* adp){
    std::cout<<"Adapter::Adapter(Adaptee*)\n";
    this->_adp=adp;
}
Adapter::~Adapter(){
    std::cout<<"Adapter::~Adapter() ";
    if(_adp!=0){
        delete _adp;
        _adp=0;
    }
}
void Adapter::Request(){
    std::cout<<"Adapter::Request() ";
    _adp->SpecificRequest();
}
