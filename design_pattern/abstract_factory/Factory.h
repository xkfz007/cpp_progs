#ifndef _FACTORY_H_
#define _FACTORY_H_
class AbstractProductA;
class AbstractProductB;
class AbstractFactory{
    public:
        virtual ~AbstractFactory();
        virtual AbstractProductA* CreateProductA(int)=0;
        virtual AbstractProductB* CreateProductB(int)=0;
    protected:
        AbstractFactory();
};
class ConcreteFactory0:
    public AbstractFactory{
        public:
            ConcreteFactory0();
            ~ConcreteFactory0();
        AbstractProductA* CreateProductA(int);
        AbstractProductB* CreateProductB(int);
    };
class ConcreteFactory1:
    public AbstractFactory{
        public:
            ConcreteFactory1();
            ~ConcreteFactory1();
        AbstractProductA* CreateProductA(int);
        AbstractProductB* CreateProductB(int);
    };

#endif
