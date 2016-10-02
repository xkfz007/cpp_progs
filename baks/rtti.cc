#ifdef _RTTI
#include <iostream>
#include <typeinfo>
#include <ctime>
#include <cstdlib>
using std::cout;
using std::endl;

class Mammal {
public:
  virtual bool lays_eggs() { return false; } // Mammal is polymorphic
  // ...
};
class Dog: public Mammal {
public:
  // ...
};

class Cat: public Mammal {
public:
  // ...
};

class Platypus: public Mammal {
public:
  bool lays_eggs() { return true; }
  // ...
};
Mammal *factory(int i)
{
    switch(i%3){
        case 0:return new Dog;
        case 1:return new Cat;
        case 2:return new Platypus;
    }
    return 0;
}
int main()
{
    Mammal *ptr;
    int c=0,d=0,p=0;
    srand((int)time(NULL));
    for(int i=0;i<10;i++){
        int value=rand()%100;
        ptr=factory(value);
        cout<<"Object is "<<typeid(*ptr).name()<<endl;
        if(typeid(*ptr)==typeid(Dog))
            d++;
        if(typeid(*ptr)==typeid(Cat))
            c++;
        if(typeid(*ptr)==typeid(Platypus))
            p++;
    }
    cout<<"Animals generated:"<<endl;
    cout<<"Dogs:"<<d<<endl;
    cout<<"Cats:"<<c<<endl;
    cout<<"Platypuses:"<<p<<endl;

    return 0;
}
#endif
