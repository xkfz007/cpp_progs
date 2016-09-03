#ifndef _COMPONENT_H_
#define _COMPONENT_H_
#include <vector>
#include <iostream>
class Componnet{
    public:
        Componnet(){
            std::cout<<"Componnet::Componnet()\n";
        }
        virtual ~Componnet(){
            std::cout<<"Componnet::~Componnet\n";
        }
        virtual void Operation()=0;
        virtual void Add(Componnet* pChild){}
        virtual void Remove(Componnet* pChild){}
        virtual Componnet* GetChild(int){return 0;}
};
class Leaf:public Componnet{
    public:
        Leaf(){}
        ~Leaf(){}
        void Operation(){
            std::cout<<"Leaf::Operation()\n";
        }
};
class Composite{
    public:
        Composite(){
            std::cout<<"Composite::Composite()\n";
        }
        ~Composite();
        void Operation();
        void Add(Componnet* pChild);
        void Remove(Componnet* pChild);
        Componnet* GetChild(int index);
    private:
        std::vector<Componnet*> vec;
};
Composite::~Composite(){
    std::vector<Componnet*>::iterator it;
    for(it=vec.begin();it!=vec.end();++it){
        delete *it;
    }
}
void Composite::Operation(){
    std::vector<Componnet*>::iterator it;
    for(it=vec.begin();it!=vec.end();++it){
        (*it)->Operation();
    }
}
void Composite::Add(Componnet* pChild){
    vec.push_back(pChild);
}
void Composite::Remove(Componnet* pChild){
    std::vector<Componnet*>::iterator it;
    for(it=vec.begin();it!=vec.end()&&*it!=pChild;++it); 
    if(it!=vec.end()){
        delete pChild;
        vec.erase(it);
    }
}
Componnet* Composite::GetChild(int index){
    int len=vec.size();
    if(index>=len||index<0)
        return NULL;
    return vec[index];
}

#endif
