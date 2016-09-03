#include "./Iterator.h"
#include <iostream>
#include <cstdlib>
using namespace std;
int main(){
    int n=10;
    ConcreteAggregate<int> ca(n);
    Iterator<int>* it=ca.CreateItertor(&ca);
    it->First();
    for(int i=0;i<n;i++){
        int val=rand()%100;
        std::cout<<val<<' ';
        it->CurrentItem()=val;
        it->Next();
    }
    std::cout<<endl;
    it->First();
    while(!it->IsDone()){
        cout<<it->CurrentItem()<<' ';
        it->Next();
    }
    cout<<endl;
}
