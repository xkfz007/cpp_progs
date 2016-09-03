#ifndef _STRATEGY_H_
#define _STRATEGY_H_
#include <iostream>
class Strategy{
    public:
        virtual ~Strategy(){}
        virtual void AlgrithmInterface()=0;
};
class ConcreteStrategy1:
    public Strategy{
        public:
            ~ConcreteStrategy1(){}
            void AlgrithmInterface(){
                std::cout<<"ConcreteStrategy1::AlgrithmInterface()\n";
            }
    };
class ConcreteStrategy2:
    public Strategy{
        public:
            ~ConcreteStrategy2(){}
            void AlgrithmInterface(){
                std::cout<<"ConcreteStrategy2::AlgrithmInterface()\n";
            }
    };
class Context{
    public:
        Context(Strategy* st):_strgy(st){}
        ~Context(){
            delete _strgy;
            _strgy=0;
        }
        void DoAction(){
            _strgy->AlgrithmInterface();
        }
    private:
        Strategy* _strgy;
};

#endif
