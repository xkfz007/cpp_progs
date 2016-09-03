#include "./Abstraction.h"
#include <iostream>
#include <cstdlib>
using namespace std;
int main(){
    Abstraction *pabs;
    int n=5;
    srand(time(0));
    for(int i=0;i<n;i++){
        int val=rand()%1000;
        if(val%2)
            pabs=new RefinedAbstraction(new ConcreteAbstractionImp1);
        else
            pabs=new RefinedAbstraction(new ConcreteAbstractionImp0);
        pabs->Operation();

        delete pabs;
        
    }


}
