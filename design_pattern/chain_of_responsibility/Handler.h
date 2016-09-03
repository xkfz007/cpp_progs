#ifndef _HANDLER_H_
#define _HANDLER_H_
#include <iostream>
class Handler{
    public:
        Handler():_successor(0){}
        Handler(Handler* su):_successor(su){}
        virtual ~Handler(){
            delete _successor;
            _successor=0;
        }
        virtual void HandleRequest()=0;
    protected:
        Handler* _successor;
};
class ConcreteHandle1:public Handler{
    public:
        ConcreteHandle1(){}
        ConcreteHandle1(Handler* su):Handler(su){}
        ~ConcreteHandle1(){}
        void HandleRequest(){
            if(_successor!=0){
                _successor->HandleRequest();
            }
            else {
                std::cout<<"ConcreteHandle1 handle the Request\n";
            }
        }

};
class ConcreteHandle2:public Handler{
    public:
        ConcreteHandle2(){}
        ConcreteHandle2(Handler* su):Handler(su){}
        ~ConcreteHandle2(){}
        void HandleRequest(){
            if(_successor!=0){
                std::cout<<"deliver\n   ";
                _successor->HandleRequest();
            }
            else {
                std::cout<<"ConcreteHandle2 handle the Request\n";
            }
        }
};

#endif
