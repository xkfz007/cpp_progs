#include "./Flyweight.h"
#include <string>
#include <iostream>
#include <cstdlib>
using namespace std;
int main(){
    
    FlyweightFactory<string> ff;
    ff.GetFlyweight("hello");
    ff.GetFlyweight("world");
    ff.GetFlyweight("hello");
    FlyweightFactory<int> fi;
    srand(time(0));
    for(int i=0;i<10;i++){
        int val=rand()%20;
        fi.GetFlyweight(val);
    }
}
