#include "./Command.h"
int main(){
    Receiver rec;
    Command *cmd=new ConcreteCommand(&rec);
    Invoker inv(cmd);
    inv.Invoke();
    delete cmd;

}
