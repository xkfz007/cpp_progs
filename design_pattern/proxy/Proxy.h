#ifndef _PROXY_H_
#define _PROXY_H_
#include <iostream>
class Subject{
    public:
        Subject(){}
        virtual ~Subject(){}
        virtual void Request()=0;
};
class RealSubject:public Subject{
    public:
        RealSubject(){}
        ~RealSubject(){}
        void Request(){
            std::cout<<"RealSubject::Request()\n";
        }
};
class Proxy:public Subject{
    public:
        Proxy(Subject*s):_sub(s){}
        ~Proxy(){
            if(_sub!=0) {
                delete _sub;
                _sub=0;
            }
        }
        void Request(){
            std::cout<<"Proxy::Request -> ";
            _sub->Request();
        }
    private:
        Subject* _sub;
};

#endif
