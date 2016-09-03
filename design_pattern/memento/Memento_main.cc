#include "./Memento.h"
#include <iostream>
using namespace std;
int main(){
    Originator *org=new Originator(1);
    org->PrintState();
    Memento* p=org->CreateMemento();
    org->SetState(2);
    org->PrintState();
    org->RestoreState(p);
    org->PrintState();
}
