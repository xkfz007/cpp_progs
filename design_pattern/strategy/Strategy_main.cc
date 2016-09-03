#include "./Strategy.h"
#include <cstdlib>
using namespace std;
int main(){
    Context *p;
    srand(time(0));
    for(int i=0;i<10;i++){
        int  val=rand()%100;
        if(val%2)
            p=new Context(new ConcreteStrategy1);
        else
            p=new Context(new ConcreteStrategy2);
        p->DoAction();
        delete p;
    }
}
