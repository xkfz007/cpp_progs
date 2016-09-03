#include "./Singleton.h"
#include <iostream>
using namespace std;
int main(){
    int n=5;
    Singleton* p;
    for(int i=0;i<n;i++){
        p=Singleton::Instance();
        cout<<(void*)p<<endl;
    }
    delete p;
}
