#include "./Subject.h"
#include <cstdlib>
using namespace std;
int main(){
    Observer* o1=new ConcreteObserver1(1);
    Observer* o2=new ConcreteObserver2(1);
    Subject *sub=new ConcreteSubject;
    sub->Attach(o1);
    sub->Attach(o2);
    sub->SetState(7);
    sub->Notify();
    for(int i=0;i<5;i++) {
        int val=rand()%100;
        if(val%2)
            sub->Attach(new ConcreteObserver1);
        else
            sub->Attach(new ConcreteObserver2);
    }
    sub->SetState(44);
    sub->Notify();

    delete sub;

}
