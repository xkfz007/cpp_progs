#ifndef _COMMAND_H_
#define _COMMAND_H_
#include <iostream>
class Command{
    public:
        Command(){}
        virtual ~Command(){}
        virtual void Execute()=0;
};
class Invoker{
    public:
        Invoker(Command*cmd):_cmd(cmd){}
        ~Invoker(){ }
        void Invoke(){
            if(_cmd!=0)
                _cmd->Execute();
        }
    private:
        Command* _cmd;
};
class Receiver{
    public:
        void Action(){
            std::cout<<"Receiver::Action()\n";
        }
};
class ConcreteCommand:public Command{
    public:
        ConcreteCommand(Receiver* rec):_rec(rec){}
        ~ConcreteCommand(){
        }
        void Execute(){
            if(_rec!=0)
                _rec->Action();
        }
    private:
        Receiver* _rec;
};


#endif
